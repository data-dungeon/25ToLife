#include "Layers/LayersPCH.h"

#ifdef PS2
#include <libnet\in.h>
#endif

#include "MXLookup.h"
#include "TextStream.h"
#include "GameHelper/DebugOutput.h"

namespace MXLookup
{
	static uint8 sOutputCategory;

	//////////////////////////////////////////////////////////////////////////////

	static void DecodeName( char* root, char* name, char* buffer )
	{
		int pos = 0;
	
		buffer[0] = '\0';
		while ( *name )
		{
			if( ( *name & 0xC0 ) == 0xC0)
			{
				u16 offset = *(u16*)name;
				offset = ntohs( offset );
				offset &= 0x3FFF;
				name = &root[offset];
			}
			else
			{
				memcpy( &buffer[pos], name+1, *name );
				pos += *name;
				name += *name + 1;
				if ( *name )
					buffer[pos++] = '.';
				else
					buffer[pos++] = '\0';
			}
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////

	static u32 GetNameLength(char* name)
	{
		u8 len = 0;
		while ( *name )
		{
			if( (*name & 0xC0) == 0xC0)
			{
				return len + 2;
			}
			else
			{
				len += *name + 1;
				name += *name + 1;
			}
		}
		return len+1;
	}
	
	//////////////////////////////////////////////////////////////////////////////

	static void BuildNamePacket(const char* name, char* ip, int& iplen)
	{
		const char* domain = name;
		while ( *domain && *domain != '@' ) domain++;
		if ( *domain ) domain++;
		else domain = name;

		char* dat = ip+1;
		char* len = ip;
		*len = 0;
		iplen = 0;
		while ( *domain )
		{
			if ( *domain == '.' )
			{
				len = dat;
				dat++;
				domain++;
				*len = 0;
				iplen+=2;
			}
			else
			{
				*dat++ = *domain++;
				(*len)++;
				iplen++;
			}
		}
		*dat = 0;
		iplen++;
	}

	//////////////////////////////////////////////////////////////////////////////
	
	static TextStream* ConnectToNameServer( void )
	{
		TextStream* stream = TextStream::Create( );
		if ( stream == NULL )
		{
			g_debugOutput.Print( sOutputCategory, DEBUG_ALERT, "Failed to create TextStream");
			return NULL;
		}

		if ( !stream->ConnectToNameServer( ) )
		{
			g_debugOutput.Print( sOutputCategory, DEBUG_ALERT, "Failed to connect to name server");
			delete stream;
			return NULL;
		}

		return stream;
	}

	//////////////////////////////////////////////////////////////////////////////

	static void dump(char* buffer, int len)
	{
		for(int i = 0; i < len; i++)
		{
			if(i % 8 == 0)
				dbgPrint("\n");
			dbgPrint("%02.2X ", buffer[i]);
		}
		dbgPrint("\n");
	}

	//////////////////////////////////////////////////////////////////////////////

	static bool SendMXRequest(TextStream* conn, const char* name)
	{
		// Sorry this is so obfuscated.  It was simplified from my original to keep it short and sweet
		// my original was 3 classes, several hundred lines each.
		char ip[256];
		int iplen;
		BuildNamePacket(name, ip, iplen);
	
		int buflen = iplen + 4 + 12;
		short buffer[128];
		memset(buffer, 0, buflen+2);
		buffer[0] = htons(buflen);
		buffer[2] = htons(0x0100);
		buffer[3] = htons(1);
		memcpy(&buffer[7], ip, iplen);
		char* bufptr = (char*)&buffer[0];
		bufptr[15+iplen] = 0x0F;
		bufptr[17+iplen] = 0x01;

		return conn->Send( buffer, buflen+2 );
	}

	//////////////////////////////////////////////////////////////////////////////

	static u16 GetShort(char* ptr)
	{
		// This should work cross platform due to the ntohs() in it.
		return ntohs((*ptr) | *(ptr+1) << 8);
	}

	//////////////////////////////////////////////////////////////////////////////

	static bool GetMXReply(TextStream* conn, char* mx)
	{
		// Sorry this is so obfuscated.  It was simplified from my original to keep it short and sweet
		// my original was 3 classes, several hundred lines each.
		u16 length;
		char data[4096];
	
		if ( !conn->RecvBlock( (char*)&length, sizeof( length) ) )
			return false;
	
		length = ntohs( length );

		g_debugOutput.Print( sOutputCategory, DEBUG_BORE, "Reply is %d bytes long", length );

		ASSERT( length < 4096 );
	
		if ( !conn->RecvBlock( data, length ) )
			return false;

		if ( GetShort(data+6) == 0 )
		{
			g_debugOutput.Print( sOutputCategory, DEBUG_ALERT, "NS response contained no answers" );
			return false;
		}
	
		char* ptr = data + 12;
		for ( int question = GetShort(data+4); question > 0; question-- )
		{
			ptr += 4 + GetNameLength( ptr );
		}
		ptr += GetNameLength( ptr );

		u16 type = GetShort(ptr);
		u16 clss =	GetShort(ptr+2);
		if ( type != 0x000F || clss != 0x0001 )
		{
			g_debugOutput.Print( sOutputCategory, DEBUG_ALERT, "NS response answer of wrong type/class %d/%d", type, clss );
			return false;
		}
	
		DecodeName( data, ptr+12, mx );

		g_debugOutput.Print( sOutputCategory, DEBUG_BORE, "mail exchange server %s", mx );

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////

	bool ResearchEMailAddress( const char* name, char* mx )
	{
		sOutputCategory = g_debugOutput.RegisterCategory( "EMail System", "EMAIL" );

		g_debugOutput.Print( sOutputCategory, DEBUG_BORE, "researching email addy %s", name);
	
		TextStream* conn = ConnectToNameServer();
		if(conn == NULL)
			return false;
	
		g_debugOutput.Print( sOutputCategory, DEBUG_BORE, "sending request");
		bool result = SendMXRequest( conn, name );

		if ( result )
		{
			g_debugOutput.Print( sOutputCategory, DEBUG_BORE, "awaiting reply");
			result = GetMXReply(conn, mx);
		}
	
		conn->Close();
		delete conn;
	
		return result;
	}
}

