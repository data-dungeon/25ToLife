//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: tstring.cpp                                                      //
//    Author: Daniel Silver                                                    //
//   Created: 04/23/2001                                                       //
//=============================================================================//

#include "pch.h"
#include "tstring.h"

/*
char* Printf( const char* fmt, ... )
{
	static int i = 0;
	static char szBuffer[8][512];

	va_list args;
	va_start( args, fmt );
	vsprintf( szBuffer[i&7], fmt, args );
	va_end( args );

	return szBuffer[i++&7];
}
*/

char* TrueOrFalse( bool Value )
{
	if( Value )
		return "true";

	return "false";
}

char tupper( char ch )
{
	static int initialized = 0;
	static char table[256];

	if( !initialized )
	{
		for( int i = 0; i < 256; i++ )
			table[i] = toupper(i);

		initialized = ~0;
	}

	return table[ch];
}

char tlower( char ch )
{
	static int initialized = 0;
	static char table[256];

	if( !initialized )
	{
		for( int i = 0; i < 256; i++ )
			table[i] = tolower(i);

		initialized = ~0;
	}

	return table[ch];
}

int tstrlen( const char* str )
{
	int i = 0;

	if( str )
	{
		while( str[i] != '\0' )
			i++;
	}

	return i;
}

void tstrcpy( char* to, const char* from )
{
	if( to )
	{
		to[0] = '\0';

		if( from )
			memcpy( to, from, tstrlen( from ) + 1 );
	}

	return;
}

void tstrncpy( char* to, const char* from, int size )
{
	if( to )
	{
		to[0] = '\0';

		if( from )
			strncpy( to, from, size );
	}

	return;
}

char* tstrdup( const char* str )
{
	char* p = new char[tstrlen(str)+1];

	if( p )
		tstrcpy( p, str );

	return p;
}

int tstrcmp( const char* a, const char* b )
{
	if( !a && !b )
		return ~0;

	if( a && b )
	{
		for( int i = 0; a[i] == b[i]; i++ )
		{
			if( a[i] == '\0' )
				return ~0;
		}
	}

	return 0;
}

int tstricmp( const char* a, const char* b )
{
	if( !a && !b )
		return ~0;

	if( a && b )
	{
		for( int i = 0; tlower(a[i]) == tlower(b[i]); i++ )
		{
			if( a[i] == '\0' )
				return ~0;
		}
	}

	return 0;
}

const char* tstrstr( const char* a, const char *b )
{
	int u = tstrlen(a);
	int v = tstrlen(b);
	int n = u - v;
	int i, x;

	if( a && b )
	{
		for( i = 0; i <= n; i++ )
		{
			for( x = 0; x <= v; x++ )
			{
				if( b[x] == '\0' )
					return &a[i];

				if( b[x] != a[i+x] )
					break;
			}
		}
	}

	return 0;
}

const char* tstristr( const char* a, const char* b )
{
	int u = tstrlen(a);
	int v = tstrlen(b);
	int n = u - v;
	int i, x;

	char ch1;
	char ch2;

	if( a && b )
	{
		for( i = 0; i <= n; i++ )
		{
			for( x = 0; x <= v; x++ )
			{
				if( b[x] == '\0' )
					return &a[i];

				ch1 = tlower(a[i+x]);
				ch2 = tlower(b[0+x]);

				if( ch1 != ch2 )
					break;
			}
		}
	}

	return 0;
}

tstring::tstring()
{
	m_string = 0;
	m_buffsize = 0;
}

tstring::~tstring()
{
	release();
}

int tstring::reserve( int size )
{
	if( m_string && (size <= m_buffsize) )
		return 1;

	char* string = new char[size+1];

	if( !string )
		return 0;

	string[0] = '\0';

	if( m_string )
		strncpy( string, m_string, size );

	string[size] = '\0';

	if( m_string )
		release();

	m_string = string;
	m_buffsize = size;

	return 1;
}

void tstring::release( void )
{
	if( m_string )
		delete [] m_string;

	m_string = 0;
	m_buffsize = 0;
}

int tstring::length( void )
{
	return tstrlen( m_string );
}

void tstring::set( const char* pText )
{
	reserve( tstrlen( pText ) );

	tstrcpy( m_string, pText );
}

void tstring::set( const char* pText, int MaxSize )
{
	int size = tstrlen( pText );

	if( size > MaxSize )
		size = MaxSize;

	reserve( size );

	tstrncpy( m_string, pText, size );
	
	m_string[size] = 0;
}

char* tstring::get( void )
{
	return m_string;
}

char tstring::getch( int i )
{
	int len = length();

	if( i < 0 || i > len )
		return 0;

	return m_string[i];
}

int tstring::strcmp( const char* pText )
{
	return ::strcmp( m_string, pText );
}

int tstring::stricmp( const char* pText )
{
	return ::stricmp( m_string, pText );
}

void tstring::tolower( void )
{
	int len = length();

	for( int i = 0; i < len; i++ )
		m_string[i] = tlower(m_string[i]);

	return;
}

void tstring::toupper( void )
{
	int len = length();

	for( int i = 0; i < len; i++ )
		m_string[i] = tupper(m_string[i]);

	return;
}

bool tstring::equals( const char* pText )
{
	if( 0 != strcmp(pText) )
		return false;

	return true;
}

bool tstring::notequals( const char* pText )
{
	if( equals(pText) )
		return false;

	return true;
}

void tstring::_tofilepath( const char* pFullPath )
{
	if( pFullPath )
	{
		const char* p = pFullPath;
		int len = tstrlen(p);
		int c;

		for( c = len; c >= 0; c-- )
		{
			if(p[c] == '\\' ||
				p[c] == ':' ||
				p[c] == '/' )
			{
				break;
			}
		}

		set( p, c );
	}

	return;
}

void tstring::_tofilename( const char* pFullPath )
{
	if( pFullPath )
	{
		const char* p = pFullPath;
		int len = tstrlen(p);
		int c;

		for( c = len; c >= 0; c-- )
		{
			if(p[c] == '\\' ||
				p[c] == ':' ||
				p[c] == '/' )
			{
				break;
			}
		}

		set( &p[c+1] );
	}

	return;
}

void tstring::_tofileext( const char* pFileName )
{
	if( pFileName )
	{
		const char* p = pFileName;
		int len = tstrlen(p);
		int c;

		for( c = len-1; c >= 0; c-- )
		{
			if( p[c] == '.' )
			{
				break;
			}
		}

		set( &p[c+1] );
	}

	return;
}

filename::filename( void )
{
}

filename::filename( const char* name )
{
	set( name );
}

void filename::driveletter( void )
{
	char temp[2];

	temp[0] = getch(0);
	temp[1] = 0;

	set( temp );
}

void filename::driveletter( const char* path )
{
	set( path, 1 );
}

void filename::filepath( void )
{
	tstring tmp;
	tmp.set( get() );
	filepath( tmp.get() );
}

void filename::filepath( const char* path )
{
	if( path )
	{
		int len = tstrlen( path );
		int c, ch;

		for( c = len-1; c >= 0; c-- )
		{
			ch = path[c];

			if(ch =='\\' ||
				ch == '/' ||
				ch == ':' )
			{
				break;
			}
		}

		set( path, c );
	}

	return;
}

void filename::relativepath( const char* currentpath )
{
	tstring tmp;
	tmp.set( get() );
	relativepath( tmp.get(), currentpath );
}

void filename::relativepath( const char* path, const char* currentpath )
{
	int len1 = tstrlen(path)+1;
	int len2 = tstrlen(currentpath)+1;
	int c, ch1, ch2;
	char* root = (char* ) path;

	const char* s1 = path;
	const char* s2 = currentpath;

	int maxlen = len1;

	if( maxlen > len2 )
		maxlen = len2;

	for( c = 0; c < maxlen; c++ )
	{
		ch1 = tlower(s1[c]);
		ch2 = tlower(s2[c]);

		if( ch1 != ch2 && ch2 != 0 )
			break;

		if(ch1 == '\\' ||
			ch1 == '/' ||
			ch1 == ':' )
		{
			root = (char* ) &s1[c+1];
		}

		if( ch2 == 0 )
			break;
	}

	set( root );

	return;
}

void filename::getfilename( void )
{
	tstring tmp;
	tmp.set( get() );
	getfilename( tmp.get() );
}

void filename::getfilename( const char* path )
{
	if( path )
	{
		int len = tstrlen(path);
		int c, ch;

		for( c = len-1; c >= 0; c-- )
		{
			ch = path[c];

			if(ch =='\\' ||
				ch == '/' ||
				ch == ':' )
			{
				break;
			}
		}

		set( &path[c+1] );
	}

	return;
}

void filename::getext( void )
{
	tstring tmp;
	tmp.set( get() );
	getext( tmp.get() );
}

void filename::getext( const char* name )
{
	int len = tstrlen(name);
	int c, ch;

	for( c = len-1; c >= 0; c-- )
	{
		ch = name[c];

		if(ch =='\\' ||
			ch == '/' ||
			ch == ':' ||
			ch == '.' )
		{
			break;
		}

		if( ch == '.' )
			set( &name[c+1] );
		else
			set( 0 );
	}

	return;
}

void filename::delext( void )
{
	tstring tmp;
	tmp.set( get() );
	delext( tmp.get() );
}

void filename::delext( const char* name )
{
	int len = tstrlen(name);
	int c, ch;

	for( c = len-1; c >=0; c-- )
	{
		ch = name[c];

		if(ch == '\\'||
			ch == '/' ||
			ch == ':' ||
			ch == '.' )
		{
			break;
		}
	}

	if( ch == '.' )
		set( name, c );
	else
		set( name );

	return;
}