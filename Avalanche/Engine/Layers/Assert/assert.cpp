/* assert.c */

/******************* includes ****************************/

#include "Layers/LayersPCH.h"

#ifdef ASSERT_SUPPORT_EMAIL
#include "Layers/EMail/EMail.h"
#include "GameHelper/DebugOutput.h"
#endif

#ifndef DISABLE_ASSERTS

#define ASSERTF_BUFFER_LEN 512

namespace Assert
{
	static char AssertFBuffer[ASSERT_BUFFER_LEN];
	static Handler* sCurrentHandler = Default;
	static bool inAssert = false;

	bool InAssert( )
	{
		return inAssert;
	}

	void SetHandler( Handler* handler )
	{
		sCurrentHandler = handler;
	}

	bool CallHandler( const char *expr, const char *file, int line, const char* string )
	{
		if ( inAssert )
			return false;
		inAssert = true;

		bool ret;
		if ( sCurrentHandler )
			ret = (*sCurrentHandler)( expr, file, line, string );
		else
			ret = Default( expr, file, line, string );

		inAssert = false;
		return ret;
	}

	const char* FormatString( const char* format, ... )
	{
		format = StringLookup( format );

		va_list list;
		va_start( list, format );
		vsprintf( AssertFBuffer, format, list );
		va_end( list );

		return &AssertFBuffer[0];
	}

	bool Default( const char *expr, const char *file, int line, const char* string )
	{
		// just in case the assert handler was changed from something that
		// handles IgnoreAlways to Default
#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
		if ( IgnoreAlways(expr, file, line, string) )
			return true;
#endif

		dbgPrint( "Assertion failure:\n");
		if ( string )
			dbgPrint( "String: %s\n", StringLookup( string ) );
		dbgPrint( "Expression: %s\n", StringLookup( expr ) );
		dbgPrint( "File %s\n", StringLookup( file ) );
		dbgPrint( "Line %d\n", line );
		LOCKUP( );
		return false;
	}

// ignore always support
#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
#define MAX_ASSERT_ENTRIES 1024

	struct _IgnoreTableEntry
	{
		// this is a hash of the file string and the expression string together...
		unsigned int hash : 24;
		// lines > 128 wrap... probably ok since a single assert hitting another will be VERY rare..
		// and even if they do, the hash should be different...
		unsigned int line : 7;
		unsigned int ignored : 1;
	};

	static _IgnoreTableEntry IgnoreTable[MAX_ASSERT_ENTRIES];
	static int nextEntry = 0;

	static int GenerateIgnoreHash( const char* expr, const char* file, const char* string )
	{
		// note that string may be null, and all 3 of these are not nessisaraly valid pointers
		// On the PS2, these are really offsets into a strings section of the elf file.
		return (((int)expr << 4) ^ ((int)file << 2) ^ ((int)string)) & 0x00FFFFFF;
	}

	// note that line is actually line % 128
	static int FindEntry(unsigned int hash, unsigned int line)
	{
		int i;
		for ( i = 0 ; i < nextEntry; i++ )
		{
			if ( hash == IgnoreTable[i].hash && line == IgnoreTable[i].line )
				return i;
		}
		return -1;
	}

	bool IgnoreAlways( const char* expr, const char* file, int line, const char* string )
	{
		int encodedline = line % 128;
		int hash = GenerateIgnoreHash( expr, file, string );
		int entry;

		entry = FindEntry( hash, encodedline );

		if ( entry != -1 )
		{
			if ( IgnoreTable[entry].ignored )
				return true;

			return false;
		}

		// allocate a new entry...
		if ( nextEntry < MAX_ASSERT_ENTRIES )
		{
			IgnoreTable[nextEntry].hash = hash;
			IgnoreTable[nextEntry].line = encodedline;
			IgnoreTable[nextEntry].ignored = false;
			nextEntry++;
		}

		return false;
	}

	void SetIgnoreAlways( const char* expr, const char* file, int line, const char* string, bool ignore )
	{
		int encodedline = line % 128;
		int hash = GenerateIgnoreHash( expr, file, string );

		int entry = FindEntry( hash, encodedline );

		if ( entry != -1 )
		{
			IgnoreTable[entry].ignored = ignore;
			return;
		}
	}

#endif // ASSERT_SUPPORT_IGNORE_ALWAYS

#ifdef ASSERT_SUPPORT_EMAIL
	// here are helpers to make supporting emailing asserts easier for handlers
	void EmailAssert( int EMailAddressIndex, const char* expr, const char* file, int line, const char* string )
	{
		char buffer[256];
		const EMailRecipient* recipient = EMail::GetRecipient( EMailAddressIndex );
		sprintf( buffer, "assert@%s", recipient->GetDomain( ) );

		EMail email;
		if ( email.Begin( buffer, recipient->GetEMailAddress( ), "Assertion Failure" ) )
		{
			email.AppendLine( "Assert Information" );
			email.AppendLine( "----------------------------" );
			sprintf( buffer, "File://%s", StringLookup( file ) );
			email.AppendLine( buffer );
			sprintf( buffer, "Line:%d", line );
			email.AppendLine( buffer );
			sprintf( buffer, "Expression:%s", StringLookup( expr ) );
			email.AppendLine( buffer );
			if ( string )
			{
				sprintf( buffer, "String:%s", StringLookup( string ) );
				email.AppendLine( buffer );
			}

			// attach the output buffer...
			if ( ValidSingleton( CDebugOutput ) )
			{
				g_debugOutput.AttachBufferToEmail( &email, true );
			}

			// End the message and close the connection.
			email.End( );
		}
	}
#endif

};


#endif // DISABLE_ASSERTS


