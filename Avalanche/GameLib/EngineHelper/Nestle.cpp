////////////////////////////////////////////////////////////////////////////
//
// Nestle.cpp
//
// Crunches 8-bit/byte character strings into 6-bit/byte character strings.
//
// Nate Robins, February 2001.
//
////////////////////////////////////////////////////////////////////////////


/******************* includes ****************************/

#include "EngineHelper/EngineHelperPCH.h"





////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

// This enum must not exceed 64 (2^6) elements!
enum 
{
	CRUNCH_NULL = 0x00,
	CRUNCH_NEWLINE,

	CRUNCH_A,		// A-Z must be in order.
	CRUNCH_B,
	CRUNCH_C,
	CRUNCH_D,
	CRUNCH_E,
	CRUNCH_F,
	CRUNCH_G,
	CRUNCH_H,
	CRUNCH_I,
	CRUNCH_J,
	CRUNCH_K,
	CRUNCH_L,
	CRUNCH_M,
	CRUNCH_N,
	CRUNCH_O,
	CRUNCH_P,
	CRUNCH_Q,
	CRUNCH_R,
	CRUNCH_S,
	CRUNCH_T,
	CRUNCH_U,
	CRUNCH_V,
	CRUNCH_W,
	CRUNCH_X,
	CRUNCH_Y,
	CRUNCH_Z,

	CRUNCH_0,		// 0-9 must be in order.
	CRUNCH_1,
	CRUNCH_2,
	CRUNCH_3,
	CRUNCH_4,
	CRUNCH_5,
	CRUNCH_6,
	CRUNCH_7,
	CRUNCH_8,
	CRUNCH_9,

	CRUNCH_SPACE,
	CRUNCH_PERIOD,
	CRUNCH_LEFTBRACKET,
	CRUNCH_RIGHTBRACKET,
	CRUNCH_DASH,
	CRUNCH_ASTERISK,
};

////////////////////////////////////////////////////////////////////////////

char* Nestle::Crunch( char* uncrunched )
{
	ASSERT( uncrunched );

	int ulen = strlen( uncrunched );
	int clen = ( int )( ( ulen * 6.0f / 8.0f ) + 1.0f );

	char* crunched = new char[ clen + 1 ];
	ASSERT( crunched );
	memset( crunched, 0, clen + 1 );

	for ( int i = 0, n = 0; i < ulen + 1; i++, n += 6 )
	{
		unsigned short c = CrunchChar( uncrunched[ i ] );

		ASSERT( c <= 0x3F );

		// which byte are we in?
		int byte = n / 8;

		// which bit of the byte are we on?
		int bit = n % 8;

		unsigned short* x = ( unsigned short* )&( crunched[ byte ] );

		*x |= ( c << bit );
	}

	ASSERT( CrunchLength( crunched ) == clen );

	return crunched;
}

////////////////////////////////////////////////////////////////////////////

char* Nestle::UnCrunch( char* crunched )
{
	ASSERT( crunched );

	int clen = CrunchLength( crunched );
	int ulen = UnCrunchLength( crunched );

	char* uncrunched = new char[ ulen + 1 ];
	ASSERT( uncrunched );
	memset( uncrunched, 0, ulen + 1 );

	for ( int i = 0, n = 0; i < ulen; i++, n += 6 )
	{
		// which byte are we in?
		int byte = n / 8;

		// which bit of the byte are we on?
		int bit = n % 8;

		unsigned short* x = ( unsigned short* )&( crunched[ byte ] );

		char c = ( ( *x ) >> bit ) & 0x3F;

		uncrunched[ i ] = UnCrunchChar( c );
	}

	ASSERT( uncrunched[ ulen ] == '\0' );

	return uncrunched;
}

////////////////////////////////////////////////////////////////////////////

int Nestle::CrunchLength( char* crunched )
{
	ASSERT( crunched );

	for ( int n = 0, i = 0; /* */; i++, n += 6 )
	{
		// which byte are we in?
		int byte = n / 8;

		// which bit of the byte are we on?
		int bit = n % 8;

		unsigned short* x = ( unsigned short* )&( crunched[ byte ] );

		char c = ( ( *x ) >> bit ) & 0x3F;

		if ( c == CRUNCH_NULL )
		{
			return ( int )( ( i * 6.0f / 8.0f ) + 1.0f );
		}
	}
}

////////////////////////////////////////////////////////////////////////////

int Nestle::UnCrunchLength( char* crunched )
{
	ASSERT( crunched );

	for ( int n = 0, i = 0; /* */; i++, n += 6 )
	{
		// which byte are we in?
		int byte = n / 8;

		// which bit of the byte are we on?
		int bit = n % 8;

		unsigned short* x = ( unsigned short* )&( crunched[ byte ] );

		char c = ( ( *x ) >> bit ) & 0x3F;

		if ( c == CRUNCH_NULL )
		{
			return i;
		}
	}
}

////////////////////////////////////////////////////////////////////////////

char Nestle::CrunchChar( char c )
{
	if ( c >= 'a' && c <= 'z' )
	{
		c = c - 'a' + 'A';
	}

	if ( isdigit( c ) )
	{
		return c - '0' + CRUNCH_0;
	}
	else if ( isalpha( c ) )
	{
		return c - 'A' + CRUNCH_A;
	}
	else 
	{
		switch( c )
		{
		case '\0':	return CRUNCH_NULL;
		case '\n':	return CRUNCH_NEWLINE;
		case ' ':	return CRUNCH_SPACE;
		case '.':	return CRUNCH_PERIOD;
		case '[':	return CRUNCH_LEFTBRACKET;
		case ']':	return CRUNCH_RIGHTBRACKET;
		case '-':	return CRUNCH_DASH;
		default:	return CRUNCH_ASTERISK;
		}
	}
}

////////////////////////////////////////////////////////////////////////////

char Nestle::UnCrunchChar( char c )
{
	if ( c >= CRUNCH_0 && c <= CRUNCH_9 )
	{
		return c - CRUNCH_0 + '0';
	}
	else if ( c >= CRUNCH_A && c <= CRUNCH_Z )
	{
		return c - CRUNCH_A + 'A';
	}
	else 
	{
		switch( c )
		{
		case CRUNCH_NULL:			return '\0';
		case CRUNCH_NEWLINE:		return '\n';
		case CRUNCH_SPACE:			return ' ';
		case CRUNCH_PERIOD:			return '.';
		case CRUNCH_LEFTBRACKET:	return '[';
		case CRUNCH_RIGHTBRACKET:	return ']';
		case CRUNCH_DASH:			return '-';
		default:					return '*';
		}
	}
}

////////////////////////////////////////////////////////////////////////////

//#define NESTLE_TEST
#ifdef NESTLE_TEST
int main( void )
{
	char test[] = "!@# abc\r\ndefgHIJKLMNOPqrstuvwxyz0123456789 .-[]*";

	for ( int l = strlen( test ); l >= 1; l-- )
	{
		test[ l ] = '\0';

		char* crunched = Nestle::Crunch( test );

		ASSERT( Nestle::UnCrunchLength( crunched ) == ( int )strlen( test ) );

		char* uncrunched = Nestle::UnCrunch( crunched );

		printf( "test (%d):       \"%s\"\n", strlen( test ), test );

		printf( "crunched (%.0f%%): ", 100.0f - ( ( float )Nestle::CrunchLength( crunched ) / strlen( test ) * 100.0f ) );
		for ( int i = 0; i < Nestle::CrunchLength( crunched ); i++ )
		{
			printf( "%02X ", ( unsigned char )crunched[ i ] );
		}
		printf( "\n" );

		printf( "uncrunched (%d): \"%s\"\n", strlen( uncrunched ), uncrunched );

		printf( "\n" );

		delete crunched;
		delete uncrunched;
	}

	return 0;
}
#endif	// NESTLE_TEST

////////////////////////////////////////////////////////////////////////////
