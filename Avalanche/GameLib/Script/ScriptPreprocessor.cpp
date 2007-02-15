//============================================================================
//=
//= ScriptPreprocessor.cpp
//=
//= Preprocess a script file handling #define, #include, ...
//=
//============================================================================

#include "ScriptPCH.h"

#ifdef SCRIPT_COMPILER

//============================================================================

ScriptPreprocessor::ScriptPreprocessor( ScriptError* err )
{
	m_err  = err;
	OutputReset();
	m_syms.AddSymbol( ScriptSymbols::SYMBOL_CONST_STRING, "_MSC_VER",           (int)"1200", 0 );
	m_syms.AddSymbol( ScriptSymbols::SYMBOL_CONST_STRING, "_WIN32",             (int)"",     0 );
	m_syms.AddSymbol( ScriptSymbols::SYMBOL_CONST_STRING, "_M_IX86",            (int)"500",  0 );
	m_syms.AddSymbol( ScriptSymbols::SYMBOL_CONST_STRING, "_INTEGRAL_MAX_BITS", (int)"32",   0 );
}

//============================================================================

ScriptPreprocessor::~ScriptPreprocessor()
{
	m_syms.Reset();
}

//============================================================================

bool ScriptPreprocessor::GetLine( char** inBufPtr, char* outBuf )
{
	char* in  = *inBufPtr;
	char* out = outBuf;

	// skip to eol or end of buffer
	while ( *in != 0 && *in != '\r' && *in != '\n' )
		*out++ = *in++;
	*out = 0;

	// skip in past eol
	if      ( in[ 0 ] == '\n' && in[ 1 ] == '\r' )  in += 2;
	else if ( in[ 0 ] == '\r' && in[ 1 ] == '\n' )  in += 2;
	else if ( in[ 0 ] == '\n' || in[ 0 ] == '\r' )  in += 1;

	bool endOfFile = ( in == *inBufPtr );

	// return new buffer pointer
	*inBufPtr = in;

	// return if we still have data to process
	return( ! endOfFile );
}

//============================================================================

bool ScriptPreprocessor::Subst( const char* filename, int line, char* buffer )
{
	char* p = buffer;
	bool definedFlag = false;

	while ( *p )
	{
		// look through this line looking for identifiers
		for ( ; *p && !isalpha( *p ) && *p != '_' ; )
		{
			if ( *p == '"' )
			{
				// skip string
				for ( p++ ; *p && ( *p != '"' || p[-1] == '\\' ) ; ) p++;
				if ( *p == 0 )
				{
					m_err->Error( filename, line, "found end-of-line in string constant" );
					return false;
				}
				ASSERT( *p == '"' );
			}
			p++;
		}

		char* identStart = p;
		char identBuf[ 256 ], *ident = identBuf;
		for ( ; isalnum( *p ) || *p == '_' ; )
			*ident++ = *p++;
		*ident = 0;

		if ( strcmp( identBuf, "defined" ) == 0 )
		{
			definedFlag = true;
		}
		else
		{
			if ( !definedFlag )
			{
				int index;
				if ( identBuf[ 0 ] != 0 && ( index = m_syms.FindConstantByName( identBuf, 0, false ) ) >= 0 )
				{
					char newbuf[ 1024 ];
					memcpy( newbuf, buffer, identStart - buffer ); newbuf[ identStart - buffer ] = 0;
					strcat( newbuf, (char*)m_syms[ index ].data );
					strcat( newbuf, p );
					strcpy( buffer, newbuf );
					p = identStart;
				}
			}
			definedFlag = false;
		}
	}

	return true;
}

//============================================================================

bool ScriptPreprocessor::GetIdentifier( char** inBufPtr, char* identBuf )
{
	char* in  = *inBufPtr;
	char* out = identBuf;

	if ( isalpha( *in ) || *in == '_' )
	{
		while ( isalnum( *in ) || *in == '_' )
			*out++ = *in++;
		*out      = 0;
		*inBufPtr = in;
		return true;
	}
	else
	{
		*out = 0;
		return false;
	}
}

//============================================================================

void ScriptPreprocessor::OutputReset()
{
	m_outputFile[ 0 ] = 0;
	m_outputLine = 0;
}

//============================================================================

void ScriptPreprocessor::Output( const char* file, int line, const char* fmt, ... )
{
	// line jump if we have to or if it's shorter (in characters) than outputing newlines.
	if ( strcmp( file, m_outputFile ) != 0 || ( line - m_outputLine >= 10 + (int)strlen( m_outputFile ) ) )
	{
		printf( "# %d \"%s\"\n", line, file );
		strcpy( m_outputFile, file );
		m_outputLine = line;
	}

	while ( m_outputLine < line )
	{
		printf( "\n" );
		m_outputLine++;
	}

	va_list args;
	va_start( args, fmt );
	vprintf( fmt, args );
	va_end( args );

	m_outputLine++;
}

//============================================================================

bool ScriptPreprocessor::FindIncludeFile( char* file )
{
	FILE* fp;

	// is it in current path?
	printf( "include search testing '%s'\n", file );
	if ( ( fp = fopen( file, "rb" ) ) != NULL )
	{
		fclose( fp );
		return true;
	}

	// search the include paths
	const char* includes = g_ScriptSystem.GetIncludePath();
	const char* p = includes;
	while ( *p )
	{
		// skip until first quoted string
		while ( *p && *p != '"' ) p++;
		if ( *p == '"' )
		{
			char path[ 256 ], *d = path;
			for ( p++ ; *p && *p != '"' ; ) *d++ = *p++;
			*d = 0;
			if ( d > path && d[ -1 ] != '/' && d[ -1 ] != '\\' )
			{
				*d++ = '/';
				*d = 0;
			}
			if ( *p == '"' ) p++;

			if ( path[ 0 ] )
			{
				char fullpath[ 256 ];
				sprintf( fullpath, "%s%s", path, file );
				printf( "include search testing '%s'\n", fullpath );
				if ( ( fp = fopen( fullpath, "rb" ) ) != NULL )
				{
					fclose( fp );
					strcpy( file, fullpath );
					return true;
				}
			}
		}
	}
	return false;
}

//============================================================================

bool ScriptPreprocessor::ProcessBuffer( const char* filename, char* buffer )
{
	int line = 0;
	char* buf;
	char linebuf[ 1024 ];
	char* p;
	unsigned int condFalseFlags  = 0;
	unsigned int condElseFlags   = 0;
	unsigned int condFalseSticky = 0;
	unsigned int condBit         = 0;

	// process lines
	buf = buffer;
	while ( GetLine( &buf, linebuf ) )
	{
		line++;

		// strip line comments
		if ( ( p = strstr( linebuf, "//" ) ) != NULL )
			*p = 0;

		// find out if this is a preprocessor line
		for ( p = linebuf ; *p != 0 && isspace( *p ) && *p != '#' ; ) p++;
		if ( *p == '#' )
		{
			// append lines as long as we end with a '\'
			int l;
			while ( ( l = strlen( p ) ) && p[ l - 1 ] == '\\' && GetLine( &buf, p + l - 1 ) )
				line++;

			// skip pound and extra whitespace
			for ( p++ ; isspace( *p ) ; ) p++;

			enum {
				CMD_NONE,
				CMD_INCLUDE,
				CMD_DEFINE,  CMD_UNDEF,
				CMD_IF, CMD_ELIF, CMD_ELSE, CMD_ENDIF,
				CMD_IFDEF, CMD_IFNDEF,
				CMD_ERROR,
				CMD_PRAGMA
			};

			// do pp command
			int len = 0;
			int cmd = 0;
			if      ( strncmp( p, "include", 7 ) == 0 )  { cmd = CMD_INCLUDE; len = 7; }
			else if ( strncmp( p, "define",  6 ) == 0 )  { cmd = CMD_DEFINE;  len = 6; }
			else if ( strncmp( p, "ifdef",   5 ) == 0 )  { cmd = CMD_IFDEF;   len = 5; }
			else if ( strncmp( p, "ifndef",  6 ) == 0 )  { cmd = CMD_IFNDEF;  len = 6; }
			else if ( strncmp( p, "else",    4 ) == 0 )  { cmd = CMD_ELSE;    len = 4; }
			else if ( strncmp( p, "elif",    4 ) == 0 )  { cmd = CMD_ELIF;    len = 4; }
			else if ( strncmp( p, "if",      2 ) == 0 )  { cmd = CMD_IF;      len = 2; }
			else if ( strncmp( p, "error",   5 ) == 0 )  { cmd = CMD_ERROR;   len = 5; }
			else if ( strncmp( p, "undef",   5 ) == 0 )  { cmd = CMD_UNDEF;   len = 5; }
			else if ( strncmp( p, "endif",   5 ) == 0 )  { cmd = CMD_ENDIF;   len = 5; }
			else if ( strncmp( p, "pragma",  6 ) == 0 )  { cmd = CMD_PRAGMA;  len = 6; }
			else if ( *p == 0 )                          { cmd = CMD_NONE;    len = 0; }
			else
			{
				m_err->Error( filename, line, "invalid preprocessor directive '%s'", p );
				return false;
			}

			// skip command and trailing whitespace
			for ( p += len ; isspace( *p ) ; ) p++;

			switch ( cmd )
			{
				case CMD_INCLUDE:
					if ( condFalseFlags == 0 )
					{
						int end;
						if      ( *p == '"' ) end = '"';
						else if ( *p == '<' ) end = '>';
						else
						{
							m_err->Error( filename, line, "invalid #include, expecting <filename> or \"filename\"" );
							return false;
						}
						char includeFile[ 256 ], *inc = includeFile;
						for ( p++ ; *p != 0 && *p != end ; )
							*inc++ = *p++;
						*inc = 0;
						if ( *p != end )
						{
							m_err->Error( filename, line, "missing '%c'", end );
							return false;
						}
						if ( !FindIncludeFile( includeFile ) )
						{
							m_err->Error( filename, line, "can't find include file '%s'", includeFile );
							return false;
						}
						if ( !ProcessFile( includeFile ) )
						{
							m_err->Error( filename, line, "error processing include file '%s'", includeFile );
							return false;
						}
					}
					break;

				case CMD_DEFINE:
					if ( condFalseFlags == 0 )
					{
						char identifier[ 256 ];
						if ( ! GetIdentifier( &p, identifier ) )
						{
							m_err->Error( filename, line, "missing identifier after #define" );
							return false;
						}
						// if this guy has parameters, rememember them
						if ( *p == '(' )
						{
							m_err->Error( filename, line, "#define IDENT() not yet supported" );
							return false;
						}
						// skip whitespace after identifier
						while ( isspace( *p ) ) p++;
						int index = m_syms.FindConstantByName( identifier, 0, 0 );
						if ( index >= 0 )
						{
							m_err->Error( filename, line, "identifer '%s' already defined", identifier );
							return false;
						}
						m_syms.AddSymbol( ScriptSymbols::SYMBOL_CONST_STRING, identifier, (int)strdup( p ), 0 );
					}
					break;

				case CMD_IFDEF:
				case CMD_IFNDEF:
					{
						if ( condBit & ( 1 << 31 ) )
						{
							m_err->Error( filename, line, "maximum #if nesting level reached" );
							return false;
						}
						char identifier[ 256 ];
						if ( ! GetIdentifier( &p, identifier ) )
						{
							m_err->Error( filename, line, "expecting identifier after #ifdef" );
							return false;
						}
						int index = m_syms.FindConstantByName( identifier, 0, false );

						condBit = ( !condBit ) ? 1 : ( condBit << 1 );

						if ( ( cmd == CMD_IFDEF && index < 0 ) || ( cmd == CMD_IFNDEF && index >= 0 ) )
							condFalseFlags |= condBit;
					}
					break;
					
				case CMD_ELSE:
					{
						if ( !condBit || ( condElseFlags & condBit ) )
						{
							m_err->Error( filename, line, "unexpected #else" );
							return false;
						}
						condFalseFlags ^= condBit;
						condFalseFlags |= ( condFalseSticky & condBit );
						condElseFlags  |= condBit;
					}
					break;

				case CMD_ENDIF:
					{
						if ( !condBit )
						{
							m_err->Error( filename, line, "unexpected #endif" );
							return false;
						}
						condFalseFlags  &= ~condBit;
						condElseFlags   &= ~condBit;
						condFalseSticky &= ~condBit;
						condBit >>= 1;
					}
					break;

				case CMD_IF:
				case CMD_ELIF:
					{
						if ( cmd == CMD_IF && ( condBit & ( 1 << 31 ) ) )
						{
							m_err->Error( filename, line, "maximum #if nesting level reached" );
							return false;
						}
						else if ( cmd == CMD_ELIF && ( !condBit || ( condElseFlags & condBit ) ) )
						{
							m_err->Error( filename, line, "unexpected #elif" );
							return false;
						}

						if ( ! Subst( filename, line, p ) )
							return false;

//						printf( "cond <%s>\n", p );

						ScriptLex lex( p, *m_err );
						ScriptParse parse( lex, m_syms, *m_err );
						ScriptParseTree* tree = parse.ParsePreprocessorExpr();

						if ( !tree || m_err->ErrorCount() )
							return false;

						if ( tree->m_token != TOKEN_CONST_INT )
						{
							tree->Print( 0 );
							m_err->Error( filename, line, "#if expecting constant expression" );
							return false;
						}

						if ( cmd == CMD_IF )
							condBit = ( !condBit ) ? 1 : ( condBit << 1 );
						else if ( ( condFalseFlags & condBit ) == 0 )
							condFalseSticky |= condBit;

						if ( tree->m_value.i == 0 || ( condFalseSticky & condBit ) )
							condFalseFlags |= condBit;
						else
							condFalseFlags &= ~condBit;
					}
					break;

				case CMD_ERROR:
					if ( condFalseFlags == 0 )
					{
						m_err->Error( filename, line, "#error %s", p );
						return false;
					}
					break;

				case CMD_UNDEF:
					if ( condFalseFlags == 0 )
					{
						char identifier[ 256 ];
						if ( ! GetIdentifier( &p, identifier ) )
						{
							m_err->Error( filename, line, "expecting identifier after #undef" );
							return false;
						}
						int index = m_syms.FindConstantByName( identifier, 0, false );
						if ( index >= 0 )
						{
							// just jam in something in the name so we won't find it again. redefs will get a new symbol
							m_syms[ index ].name[ 0 ] = '-';
						}
					}
					break;

				default:
					break;
			}
		}
		else
		{
			if ( condFalseFlags == 0 )
			{
				if ( ! Subst( filename, line, linebuf ) )
					return false;

				// strip end of line whitespace
				for ( p = linebuf ; *p != 0 ; ) p++;
				for ( ; p > linebuf && isspace( p[-1] ) ; ) *(--p) = 0;

				// only output if it's not empty
				if ( linebuf[ 0 ] )
				{
					Output( filename, line, "%s\n", linebuf );
				}
			}
		}
	}

	if ( condBit )
	{
		m_err->Error( filename, line, "unexpected end of file found" );
		return false;
	}

	return true;
}

//============================================================================

bool ScriptPreprocessor::ProcessFile( const char* filename )
{
	bool success = false;

	FILE* fp = fopen( filename, "rb" );
	if ( fp != NULL )
	{
		fseek( fp, 0, SEEK_END );
		int len = ftell( fp );
		fseek( fp, 0, SEEK_SET );

		char* buf = new char[ len + 1 ];
		fread( buf, 1, len, fp );
		buf[ len ] = 0;

		fclose( fp );

		success = ProcessBuffer( filename, buf );

		delete [] buf;
	}

	return success;
}

//============================================================================

void ScriptPreprocessor::PrintSymbols()
{
	m_syms.Print();
}

//============================================================================

#endif //SCRIPT_COMPILER
