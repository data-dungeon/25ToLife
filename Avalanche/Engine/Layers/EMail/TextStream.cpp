
#include "Layers/LayersPCH.h"
#include "TextStream.h"

#include "Network/Connection.h"
#include "EngineHelper/Singleton.h"

#if defined( PS2 )
#include "SocketTextStream.h"
#include "AtWinMonTextStream.h"
#endif

#if defined( WIN32 )
#include "SocketTextStream.h"
#endif

TextStream* TextStream::Create()
{
	#if defined( PS2 )

	extern const char* g_RemoteIPAddress;
	if ( g_RemoteIPAddress == NULL )
		return new SocketTextStream();
	else
		return new AtWinMonTextStream();

	#elif defined( WIN32 ) && !defined( _XBOX )
	
	return new SocketTextStream();
	
	#endif

	return NULL;
}

TextStream::TextStream()
{
	base64EncodeEnabled = false;
	encodeBufferBytes = 0;
	mServiceNetwork = true;
}

void TextStream::FillEncodeBuffer(const void* &data, int &datalen)
{
	while(encodeBufferBytes < 3 && datalen)
	{
		encodeBuffer[encodeBufferBytes++] = *(const char*)data;
		data = ((const char*)data) + 1;
		datalen--;
	}
}

void TextStream::EncodeData( char* outputBuffer )
{
	static const char *base64values = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	ASSERT( encodeBufferBytes > 0 );

	u8 a,b,c,d;

	if ( encodeBufferBytes == 1 )
		encodeBuffer[1] = 0;
	if ( encodeBufferBytes < 3 )
		encodeBuffer[2] = 0;

	a = encodeBuffer[0] >> 2;
	b = ((encodeBuffer[0] & 0x3) << 4) | (encodeBuffer[1] >> 4);
	c = ((encodeBuffer[1] & 0xF) << 2) | (encodeBuffer[2] >> 6);
	d = encodeBuffer[2] & 0x3F;

	outputBuffer[0] = base64values[a];
	outputBuffer[1] = base64values[b];
	if ( encodeBufferBytes == 1 )
		outputBuffer[2] = '=';
	else
		outputBuffer[2] = base64values[c];
	if ( encodeBufferBytes < 3 )
		outputBuffer[3] = '=';
	else
		outputBuffer[3] = base64values[d];

	encodeBufferBytes = 0;
}

bool TextStream::Send(const void* data, int datalen)
{
	if ( !base64EncodeEnabled )
	{
		return Transmit( data,  datalen );
	}
	else
	{
		// we need to encode it.. :)
		char outputBuffer[78];
		int outputBytes = 0;
		while( datalen )
		{
			FillEncodeBuffer( data, datalen );
			if ( encodeBufferBytes == 3 )
			{
				EncodeData( &outputBuffer[outputBytes] );
				outputBytes += 4;
				ASSERT( outputBytes <= 76 );
				if (outputBytes == 76)
				{
					outputBuffer[outputBytes++] = '\r';
					outputBuffer[outputBytes++] = '\n';
					if ( !Transmit( outputBuffer, outputBytes ) )
						return false;
					outputBytes = 0;
				}
			}
		}
		if ( outputBytes > 0 )
		{
			outputBuffer[outputBytes++] = '\r';
			outputBuffer[outputBytes++] = '\n';
			if ( !Transmit( outputBuffer, outputBytes ) )
				return false;
		}
	}
	return true;
}

static int strnchr(char* haystack, int len, char needle)
{
	int i = 0;
	while ( *haystack )
	{
		if ( *haystack == needle )
			return i;
		i++;
		haystack++;
	}
	return -1;
}

// Ick
void TextStream::Sleep()
{
	// service NetZ if necissary
	if ( mServiceNetwork && ValidSingleton( CConnection ) )
	{
		g_connection.Service();
	}

	// Ick...
	#if defined (PS2)
	DelayThread(10 * 14);
	#elif defined (WIN32)
	::Sleep(10);
	#endif
}

int TextStream::RecvLine(char* line, int maxLength)
{
	int timeout_loopcount = 3200;
	int linelength = 0;
	do
	{
		int pos = -1;
	
		if ( mRecvCount > 0 )
		{
			pos = strnchr( mRecvBuffer, mRecvCount, '\n' );
		}
	
		if ( pos >= 0 )
		{
			int len = pos+1;
			if ( linelength + len > maxLength )
				return 0;

			memcpy( &line[linelength], mRecvBuffer, len );
			linelength += len;
			line[linelength] = '\0';
			mRecvCount -= len;

			if ( mRecvCount )
				memmove( mRecvBuffer, &mRecvBuffer[len], mRecvCount );

			dbgPrint( "SOCK<-%s\n", line );
			return linelength;
		}

		// make sure there is room in the buffer for more...
		ASSERT(sizeof(mRecvBuffer)-mRecvCount > 0);

		int length = Receive( &mRecvBuffer[mRecvCount], sizeof(mRecvBuffer)-mRecvCount );
		if ( length <= 0 )
		{
			timeout_loopcount--;
			Sleep( );
		}
		else
			mRecvCount += length;

	} while ( timeout_loopcount );

	return 0;
}

bool TextStream::RecvBlock( void* data, int blockLength, bool waitforit )
{
	int timeout_loopcount = 3200;
	int linelength = 0;
	do
	{
		int pos = -1;

		if ( mRecvCount > 0 )
		{
			int count = Math::Min( mRecvCount, blockLength );
			memcpy( data, mRecvBuffer, count );
			mRecvCount -= count;
			blockLength -= count;
			if ( blockLength == 0 )
				return true;

			data = (char*)data + count;
		}

		int length = Receive( data, blockLength );
		if ( length <= 0 )
		{
			if ( !waitforit )
			{
				// move the data back into the buffer?
				return false;
			}

			timeout_loopcount--;
			Sleep( );
		}
		else
		{
			blockLength -= length;
			data = (char*)data + length;
		}

	} while ( timeout_loopcount && blockLength );

	return blockLength == 0;
}


void TextStream::Base64Encode(bool encodeEnable)
{
	if ( base64EncodeEnabled && encodeBufferBytes != 0 )
	{
		char outputBuffer[6];
		EncodeData( &outputBuffer[0] );
		outputBuffer[4] = '\r';
		outputBuffer[5] = '\n';
		Transmit( outputBuffer,  6 );
	}
	base64EncodeEnabled = encodeEnable;
}

bool TextStream::ConnectToNameServer( void )
{
	// if your lazy or cant figure out how to impliment this routine on a particular
	// platform, either of these two lines should work
	//
	// the internal name server address will only work when connected to our internal
	// network.
	// 
	// The external name server SHOULD work both inside & outside of avalanche but 
	// probably not forever.  Currently it allows external recursive requests, but
	// I can forsee a time when they stop supporting that.

	// an external open name server
	return Connect( "ns1.lunarpages.com", 53 );
	
	// our internal name server.
	//return Connect( "10.0.0.100", 53 );
}

bool TextStream::ConnectToHost( int port )
{
	// default is simple enough...
	return Connect( "localhost", port );
}

void TextStream::ServiceNetwork(bool doservice)
{
	mServiceNetwork = doservice;
}

