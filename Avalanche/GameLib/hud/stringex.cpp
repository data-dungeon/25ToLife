/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/stringex.h"


char *Printf( const char *fmt, ... )
{
	static int i = 0;
	static char szPrintf[8][512];

	va_list args;
	va_start( args, fmt );
	vsprintf( szPrintf[i&7], fmt, args );
	va_end( args );

	return szPrintf[i++&7];
}

int xupper( int ch )
{
	static const unsigned char upper[256] = 
	{
		 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15, 
		16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31, 
		' ', '!', '"', '#', '$', '%', '&','\'', '(', ')', '*', '+', ',', '-', '.', '/', 
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', 
		'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[','\\', ']', '^', '_', 
		'`', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '~', '', 
		'Ä', 'Å', 'Ç', 'É', 'Ñ', 'Ö', 'Ü', 'á', 'à', 'â', 'ä', 'ã', 'å', 'ç', 'é', 'è', 
		'ê', 'ë', 'í', 'ì', 'î', 'ï', 'ñ', 'ó', 'ò', 'ô', 'ä', 'õ', 'å', 'ù', 'é', 'ü', 
		'†', '°', '¢', '£', '§', '•', '¶', 'ß', '®', '©', '™', '´', '¨', '≠', 'Æ', 'Ø', 
		'∞', '±', '≤', '≥', '¥', 'µ', '∂', '∑', '∏', 'π', '∫', 'ª', 'º', 'Ω', 'æ', 'ø', 
		'¿', '¡', '¬', '√', 'ƒ', '≈', '∆', '«', '»', '…', ' ', 'À', 'Ã', 'Õ', 'Œ', 'œ', 
		'–', '—', '“', '”', '‘', '’', '÷', '◊', 'ÿ', 'Ÿ', '⁄', '€', '‹', '›', 'ﬁ', 'ﬂ', 
		'¿', '¡', '¬', '√', 'ƒ', '≈', '∆', '«', '»', '…', ' ', 'À', 'Ã', 'Õ', 'Œ', 'œ', 
		'–', '—', '“', '”', '‘', '’', '÷', '˜', 'ÿ', 'Ÿ', '⁄', '€', '‹', '›', 'ﬁ', 'ü', 
	};

	return upper[ch&255];
}

int xlower( int ch )
{
	static const unsigned char lower[256] = 
	{
		 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15, 
		16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31, 
		' ', '!', '"', '#', '$', '%', '&','\'', '(', ')', '*', '+', ',', '-', '.', '/', 
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', 
		'@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
		'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[','\\', ']', '^', '_', 
		'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
		'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', '', 
		'Ä', 'Å', 'Ç', 'É', 'Ñ', 'Ö', 'Ü', 'á', 'à', 'â', 'ö', 'ã', 'ú', 'ç', 'û', 'è', 
		'ê', 'ë', 'í', 'ì', 'î', 'ï', 'ñ', 'ó', 'ò', 'ô', 'ö', 'õ', 'ú', 'ù', 'û', 'ˇ', 
		'†', '°', '¢', '£', '§', '•', '¶', 'ß', '®', '©', '™', '´', '¨', '≠', 'Æ', 'Ø', 
		'∞', '±', '≤', '≥', '¥', 'µ', '∂', '∑', '∏', 'π', '∫', 'ª', 'º', 'Ω', 'æ', 'ø', 
		'‡', '·', '‚', '„', '‰', 'Â', 'Ê', 'Á', 'Ë', 'È', 'Í', 'Î', 'Ï', 'Ì', 'Ó', 'Ô', 
		'', 'Ò', 'Ú', 'Û', 'Ù', 'ı', 'ˆ', '◊', '¯', '˘', '˙', '˚', '¸', '˝', '˛', 'ﬂ', 
		'‡', '·', '‚', '„', '‰', 'Â', 'Ê', 'Á', 'Ë', 'È', 'Í', 'Î', 'Ï', 'Ì', 'Ó', 'Ô', 
		'', 'Ò', 'Ú', 'Û', 'Ù', 'ı', 'ˆ', '˜', '¯', '˘', '˙', '˚', '¸', '˝', '˛', 'ˇ', 
	};

	return lower[ch&255];
}

int xstrlen( const char *str )
{
	int i = 0;

	if( str )
	{
		while( str[i] != '\0' )
			i++;
	}

	return i;
}

int xstrcpy( char *to, const char *from )
{
	if( to )
	{
		to[0] = '\0';

		if( from )
			memcpy( to, from, xstrlen( from ) + 1 );
	}

	return xstrlen( to );
}

int xstrncpy( char *to, const char *from, int size )
{
	if( to )
	{
		to[0] = '\0';

		if( from )
		{
			int len = xstrlen( from ) + 1;

			if(len > size)
				len = size;

			memcpy( to, from, len );
		}
	}
	
	return xstrlen( to );
}

int xstrncpyz( char *to, const char* from, int size )
{
	int len = xstrncpy( to, from, size );

	if( to && len == size )
		to[size-1] = 0;

	return len;
}

int xstrncat( char* to, const char* from, int size )
{
	int len = xstrlen(to);

	return len + xstrncpy( &to[len], from, size-len );
}

int xstrncatz( char* to, const char* from, int size )
{
	int len = xstrlen(to);

	return len + xstrncpyz( &to[len], from, size-len );
}

char *xstrdup( const char *str )
{
	char *p = new char[xstrlen(str)+1];

	if( p )
		xstrcpy( p, str );

	return p;
}

int xstrcmp( const char *a, const char *b )
{
	if( a == b )
		return 0;

	if( a && !b )
		return (int) a[0];

	if( b && !a )
		return (int) -b[0];

	int i;
	for( i = 0; a[i] == b[i]; i++ )
	{
		if( a[i] == '\0' )
			return 0;
	}

	return b[i] - a[i];
}

int xstricmp( const char *a, const char *b )
{
	if( a == b )
		return 0;

	if( a && !b )
		return (int) a[0];

	if( b && !a )
		return (int) -b[0];

	int i;
	for( i = 0; tolower(a[i]) == tolower(b[i]); i++ )
	{
		if( a[i] == '\0' )
			return 0;
	}

	return b[i] - a[i];
}

int xstrsort( const char *a, const char *b )
{
	int i;

	for( i = 0; a[i] == b[i]; i++ )
	{
		if( a[i] == '\0' )
			return 0; // strings are identical
	}

	if( a[i] - b[i] > 0 )
		return ~0;

	return 0;
}

int xstrisort( const char *a, const char *b )
{
	int i;

	for( i = 0; tolower(a[i]) == tolower(b[i]); i++ )
	{
		if( a[i] == '\0' )
			return 0; // strings are identical (case insensitive)
	}

	if( tolower(a[i]) - tolower(b[i]) > 0 )
		return ~0;

	return 0;
}

xstring::xstring()
{
	m_buffsize = 0;
	m_string = 0;
}

xstring::~xstring()
{
	release();
}

void xstring::alloc( int size )
{
	delete [] m_string;

	m_string = new char[size+1];

	if( m_string )
		m_string[0] = m_string[size] = '\0';

	m_buffsize = size;

	return;
}

void xstring::reAlloc( int size )
{
	char *string = new char[size+1];

	if( string )
	{
		string[size] = '\0';

		xstrncpy( string, m_string, size );

		delete [] m_string;

		m_string = string;

		m_buffsize = size;
	}

	return;
}

void xstring::optimize( void )
{
	reAlloc( length() );
}

void xstring::release( void )
{
	delete [] m_string;

	m_string = 0;
	m_buffsize = 0;
}

void xstring::set( const char *string )
{
	int len = xstrlen( string );

	if( len > m_buffsize || m_string == 0 )
	{
		MEM_SET_ONE_SHOT_NEW_NAME( (char*)string );
		alloc( len );
	}

	xstrcpy( m_string, string );
}

void xstring::set( int i )
{
	set( Printf( "%d", i ) );
}

void xstring::set( float f )
{
	set( Printf( "%f", f ) );
}

char * xstring::get( void )
{
	return m_string;
}

char xstring::getch( int i )
{
	char *string = get();

	if( string )
		return string[i];

	return 0;
}

void xstring::setch( int i, char ch )
{
	char *string = get();

	if( string )
		string[i] = ch;

	return;
}

int xstring::length( void )
{
	return xstrlen( m_string );
}

int xstring::atoi( void )
{
	return ::atoi( m_string );
}

float xstring::atof( void )
{
	return (float) ::atof( m_string );
}

int xstring::compare( const char *string )
{
	if( !::xstrcmp( m_string, string ) )
		return ~0;

	return 0;
}

int xstring::compare( xstring &string )
{
	return compare( string.get() );
}

int xstring::same( xstring &string )
{
	return compare( string );
}

int xstring::same( const char *string )
{
	return compare( string );
}

int xstring::diff( xstring &string )
{
	return !compare( string );
}

int xstring::diff( const char *string )
{
	return !compare( string );
}

int xstring::xstrcmp( const char *string )
{
	return ::xstrcmp( m_string, string );
}

int xstring::xstrcmp( xstring &string )
{
	return xstrcmp( string.get() );
}

int xstring::xstricmp( const char *string )
{
	return ::xstricmp( m_string, string );
}

int xstring::xstricmp( xstring &string )
{
	return xstricmp( string.get() );
}

void xstring::copy( const char *string )
{
	set( string );
}

void xstring::copyn( const char *string, int count )
{
	int len = xstrlen( string );

	if( len < count )
		copy( string );
	else
	{
		if( count > m_buffsize )
		{
			MEM_SET_ONE_SHOT_NEW_NAME( (char*)string );
			alloc( count );
		}

		memcpy( m_string, string, count );
	}

	return;
}
