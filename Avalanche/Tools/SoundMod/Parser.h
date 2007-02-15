//============================================================================
//==
//== Project: soundmod
//== File:    parser.h
//== Desc:    Builds a composite sound bank file from wav/aiff files and
//==          control modules files.
//== Author:  Rob Nelson
//== Date:    2/6/2003
//== (c)      2003 Avalanche Software, Inc.
//==
//============================================================================

#include <stdarg.h>
#include <ctype.h>

class Parser
{
public:
	int	nArgs;
	char*	arg[ 16 ];

private:
	int	m_Line;
	FILE*	m_Fp;
	char	m_Buf[ 256 ];
	char	m_Filename[ 256 ];

public:
	Parser( const char* filename )
	{
		memset( this, 0, sizeof(*this) );
		strcpy( m_Filename, filename );
		m_Fp = fopen( m_Filename, "rt" );
		if ( m_Fp == NULL )
			Error( "Couldn't open '%s'\n", m_Filename );
		else
			NextLine();
	}

	~Parser()
	{
		if ( m_Fp != NULL )
			fclose( m_Fp );
	}

	bool End()
	{
		return ( m_Fp == NULL );
	}

	void Print( char* fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		fprintf( stderr, "%s(%d) ", m_Filename, m_Line );
		vfprintf( stderr, fmt, args );
	}

	void Error( char* fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		fprintf( stderr, "ERROR: %s(%d) ", m_Filename, m_Line );
		vfprintf( stderr, fmt, args );
		exit( 1 );
	}

	bool NextLine()
	{
		if ( m_Fp == NULL )
			return false;
		else
		{
			do
			{
				m_Line++;
				if ( fgets( m_Buf, sizeof( m_Buf ), m_Fp ) == NULL )
				{
					fclose( m_Fp );
					m_Fp = NULL;
					return false;
				}
				nArgs = Tokenize( m_Buf, arg, sizeof( arg ) / sizeof( arg[0] ) );
			}
			while ( nArgs == 0 );
			return true;
		}
	}

	int Tokenize( char* buf, char** args, int maxArgs )
	{
		char*	p;
		int		nArgs = 0;

		//-- remove end of line
		if ( ( p = strchr( buf, '\n' ) ) != NULL ) { *p = 0; }

		//-- first remove comments.
		if ( ( p = strchr( buf, ';'  ) ) != NULL ) { *p = 0; }
		if ( ( p = strchr( buf, '#'  ) ) != NULL ) { *p = 0; }
		if ( ( p = strstr( buf, "//" ) ) != NULL ) { *p = 0; }

		//-- skip any leading white space
		for ( p = buf ; *p != 0 && Delimiter( *p ) ; p++ ) {}

		//-- now grab each arg on line
		while ( ( nArgs < maxArgs ) && ( *p != 0 ) )
		{
			//-- mark pointer to argument
			args[ nArgs++ ] = p;

			//-- scan forword until the end of argument. (space or comma delimiter)
			for ( ; *p != 0 && !Delimiter( *p ); p++ ) {}

			//-- now skip delimiter, zap the space with nulls to terminate strings
			for ( ; *p != 0 && Delimiter( *p ) ; p++ ) { *p = 0; }
		}

		return nArgs;
	}

	bool Delimiter( int c )
	{
		return ( isspace( c )			||
				 c == ','					||
				 c == '(' || c == ')'	||
				 c == '<' || c == '>'	||
				 c == '='
		);
	}

	char* MergeArgs( char* buf, int firstArg, int lastArg )
	{
		char* ptr = buf;

		//-- start with nothing
		*ptr = 0;

		//-- merge args
		for ( int i = firstArg ; i <= lastArg && i < nArgs ; i++ )
		{
			ptr += sprintf( ptr, "%s ", arg[ i ] );
		}

		// remove trailing white space
		while ( ptr > buf && *(ptr - 1) == ' ' )
			*(--ptr) = 0;

		return buf;
	}
};
