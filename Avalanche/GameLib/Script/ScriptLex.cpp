//===========================================================================
//=
//= ScriptLex.cpp
//=
//===========================================================================

#include "ScriptPCH.h"
#if defined (WIN32) && !defined (_XBOX)
#include <windows.h>
#endif //

#ifdef SCRIPT_COMPILER

//===========================================================================

ScriptLex::ScriptLex( const char* buffer, ScriptError& err ) :
	m_err( err )
{
	m_ptr					= buffer;
	m_tokenBuf[ 0 ]   = 0;
	m_verbose         = false;
	m_line            = 1;
	m_currentFile     = m_fileNames.Add( "no file" );

	// determine what the end-of-line is
	m_eol = '\n';
	for ( const char* p = buffer ; *p != 0 ; p++ )
	{
		if ( *p == '\n' || *p == '\r' )
		{
			m_eol = *p;
			break;
		}
	}
}

//===========================================================================

ScriptLex::~ScriptLex()
{
}

//===========================================================================

ScriptToken ScriptLex::NextToken()
{
	SkipWhitespace();

	ScriptToken token = TOKEN_NONE;
	int         len   = 0;

	if      ( ( token = ScanNumber( len ) ) != TOKEN_NONE )  {}
	else if ( ( token = ScanIdent( len )  ) != TOKEN_NONE )  {}
	else if ( ( token = ScanString() )      != TOKEN_NONE )  {}
	else
	{
		switch ( m_ptr[0] )
		{
			case '(': token = TOKEN_PAREN_L;			len = 1; break;
			case ')': token = TOKEN_PAREN_R;			len = 1; break;
			case '{': token = TOKEN_BRACE_L;			len = 1; break;
			case '}': token = TOKEN_BRACE_R;			len = 1; break;
			case '[': token = TOKEN_BRACKET_L;		len = 1; break;
			case ']': token = TOKEN_BRACKET_R;		len = 1; break;
			case '~': token = TOKEN_BITWISE_NOT;	len = 1; break;
			case ';': token = TOKEN_SEMICOLON;		len = 1; break;
			case ':': token = TOKEN_COLON;			len = 1; break;
			case ',': token = TOKEN_COMMA;			len = 1; break;
			case '?': token = TOKEN_QUESTION_MARK;	len = 1; break;
			case '+':
				if      ( m_ptr[1] == '+' ) { token = TOKEN_INCREMENT;       len = 2; }
				else if ( m_ptr[1] == '=' ) { token = TOKEN_PLUS_EQUALS;     len = 2; }
				else                        { token = TOKEN_PLUS;            len = 1; }
				break;
			case '-':
				if      ( m_ptr[1] == '-' ) { token = TOKEN_DECREMENT;       len = 2; }
				else if ( m_ptr[1] == '=' ) { token = TOKEN_MINUS_EQUALS;    len = 2; }
				else                        { token = TOKEN_MINUS;           len = 1; }
				break;
			case '*':
				if      ( m_ptr[1] == '=' ) { token = TOKEN_MULT_EQUALS;     len = 2; }
				else                        { token = TOKEN_MULT;            len = 1; }
				break;
			case '/':
				if      ( m_ptr[1] == '=' ) { token = TOKEN_DIV_EQUALS;      len = 2; }
				else                        { token = TOKEN_DIV;             len = 1; }
				break;
			case '%':
				if      ( m_ptr[1] == '=' ) { token = TOKEN_MOD_EQUALS;     len = 2; }
				else                        { token = TOKEN_MOD;            len = 1; }
				break;
			case '<':
				if      ( m_ptr[1] == '=' )                    { token = TOKEN_LESS_THAN_EQUAL; len = 2; }
				else if ( m_ptr[1] == '<' && m_ptr[2] == '=' ) { token = TOKEN_SHIFT_L_EQUALS;  len = 3; }
				else if ( m_ptr[1] == '<' )                    { token = TOKEN_SHIFT_L;         len = 2; }
				else                                           { token = TOKEN_LESS_THAN;       len = 1; }
				break;
			case '>':
				if      ( m_ptr[1] == '=' )                    { token = TOKEN_GTR_THAN_EQUAL;  len = 2; }
				else if ( m_ptr[1] == '>' && m_ptr[2] == '=' ) { token = TOKEN_SHIFT_R_EQUALS;  len = 3; }
				else if ( m_ptr[1] == '>' )                    { token = TOKEN_SHIFT_R;         len = 2; }
				else                                           { token = TOKEN_GTR_THAN;        len = 1; }
				break;
			case '&':
				if      ( m_ptr[1] == '&' ) { token = TOKEN_LOGICAL_AND;     len = 2; }
				else if ( m_ptr[1] == '=' ) { token = TOKEN_AND_EQUALS;      len = 2; }
				else                        { token = TOKEN_BITWISE_AND;     len = 1; }
				break;
			case '|':
				if      ( m_ptr[1] == '|' ) { token = TOKEN_LOGICAL_OR;      len = 2; }
				else if ( m_ptr[1] == '=' ) { token = TOKEN_OR_EQUALS;       len = 2; }
				else                        { token = TOKEN_BITWISE_OR;      len = 1; }
				break;
			case '^':
				if      ( m_ptr[1] == '=' ) { token = TOKEN_XOR_EQUALS;      len = 2; }
				else                        { token = TOKEN_BITWISE_XOR;     len = 1; }
				break;
			case '!':
				if      ( m_ptr[1] == '=' ) { token = TOKEN_NOT_EQUALS;      len = 2; }
				else                        { token = TOKEN_LOGICAL_NOT;     len = 1; }
				break;
			case '=':
				if      ( m_ptr[1] == '=' ) { token = TOKEN_EQUALS;          len = 2; }
				else                        { token = TOKEN_ASSIGN;          len = 1; }
				break;
			case '.':
				if      ( m_ptr[1] == '.' && m_ptr[2] == '.' ) { token = TOKEN_DOTDOTDOT; len = 3; }
				break;
			case '#':
				if ( strncmp( m_ptr, "# ", 2 ) == 0 )
				{
					m_ptr += 2;
					SkipWhitespace();
					if ( ( token = ScanNumber( len ) ) != TOKEN_CONST_INT )
					{
						m_err.Error( GetFile(), GetLine(), "# line directive missing line number" );
						return TOKEN_ERROR;
					}
					int newLine = atoi( m_ptr );
					m_ptr += len;
					SkipWhitespace();
					if ( ( token = ScanString() ) != TOKEN_STRING )
					{
						m_err.Error( GetFile(), GetLine(), "# line directive missing filename" );
						return TOKEN_ERROR;
					}
					m_line = newLine;

					#if defined (WIN32) && !defined (_XBOX)
					// convert the filename from relative path to absolute path
					char fullPath[ 256 ];
					char* fileNamePtr;
					GetFullPathName( m_tokenBuf, sizeof( fullPath ), fullPath, &fileNamePtr );
					strcpy( m_tokenBuf, fullPath );
					#endif

					m_currentFile = m_fileNames.Add( m_tokenBuf );
					// consume til the end of line
					while ( m_ptr[ 0 ] && m_ptr[ 0 ] != m_eol )
						m_ptr++;
					if ( m_ptr[ 0 ] == m_eol )
						m_ptr++;
					return NextToken();
				}
				break;
			default: break;
		}
	}

	// make copy of token text
	if ( token != TOKEN_STRING )
	{
		char* p;
		for ( p = m_tokenBuf ; len > 0 ; len-- )
			*p++ = *m_ptr++;
		*p = 0;
	}

	// fill in const value
	if      ( token == TOKEN_CONST_HEX   ) { sscanf( m_tokenBuf, "%x", &m_value.i ); token = TOKEN_CONST_INT; }
	else if ( token == TOKEN_CONST_INT   ) { sscanf( m_tokenBuf, "%d", &m_value.i ); }
	else if ( token == TOKEN_CONST_FLOAT ) { sscanf( m_tokenBuf, "%f", &m_value.f ); }

	// check for reserved keywords
	if ( token == TOKEN_IDENT )
	{
		if      ( strcmp( m_tokenBuf, "if"      ) == 0 ) { token = TOKEN_IF;          }
		else if ( strcmp( m_tokenBuf, "else"    ) == 0 ) { token = TOKEN_ELSE;        }
		else if ( strcmp( m_tokenBuf, "while"   ) == 0 ) { token = TOKEN_WHILE;       }
		else if ( strcmp( m_tokenBuf, "for"     ) == 0 ) { token = TOKEN_FOR;         }
		else if ( strcmp( m_tokenBuf, "int"     ) == 0 ) { token = TOKEN_INT_DECL;    }
		else if ( strcmp( m_tokenBuf, "float"   ) == 0 ) { token = TOKEN_FLOAT_DECL;  }
		else if ( strcmp( m_tokenBuf, "string"  ) == 0 ) { token = TOKEN_STRING_DECL; }
		else if ( strcmp( m_tokenBuf, "void"    ) == 0 ) { token = TOKEN_VOID_DECL;   }
		else if ( strcmp( m_tokenBuf, "goto"    ) == 0 ) { token = TOKEN_GOTO;        }
		else if ( strcmp( m_tokenBuf, "print"   ) == 0 ) { token = TOKEN_PRINT;       }
		else if ( strcmp( m_tokenBuf, "struct"  ) == 0 ) { token = TOKEN_STRUCT_DECL; }
		else if ( strcmp( m_tokenBuf, "sleep"   ) == 0 ) { token = TOKEN_SLEEP;       }
		else if ( strcmp( m_tokenBuf, "return"  ) == 0 ) { token = TOKEN_RETURN;      }
		else if ( strcmp( m_tokenBuf, "extern"  ) == 0 ) { token = TOKEN_EXTERN;      }
		else if ( strcmp( m_tokenBuf, "enum"    ) == 0 ) { token = TOKEN_ENUM;        }
		else if ( strcmp( m_tokenBuf, "do"      ) == 0 ) { token = TOKEN_DO;          }
		else if ( strcmp( m_tokenBuf, "break"   ) == 0 ) { token = TOKEN_BREAK;       }
		else if ( strcmp( m_tokenBuf, "continue") == 0 ) { token = TOKEN_CONTINUE;    }
		else if ( strcmp( m_tokenBuf, "sizeof"  ) == 0 ) { token = TOKEN_SIZEOF;      }
		else if ( strcmp( m_tokenBuf, "abs"     ) == 0 ) { token = TOKEN_ABS;         }
		else if ( strcmp( m_tokenBuf, "switch"  ) == 0 ) { token = TOKEN_SWITCH;      }
		else if ( strcmp( m_tokenBuf, "default" ) == 0 ) { token = TOKEN_DEFAULT;     }
		else if ( strcmp( m_tokenBuf, "case"    ) == 0 ) { token = TOKEN_CASE;        }
		else if ( strcmp( m_tokenBuf, "true"    ) == 0 ) { token = TOKEN_CONST_INT; m_value.i = 1; }
		else if ( strcmp( m_tokenBuf, "false"   ) == 0 ) { token = TOKEN_CONST_INT; m_value.i = 0; }
		else if ( strcmp( m_tokenBuf, "this"    ) == 0 ) { token = TOKEN_THIS;        }
		else if ( strcmp( m_tokenBuf, "static"  ) == 0 ) { token = TOKEN_STATIC;      }
	}

	if ( m_verbose )
	{
		if ( token == TOKEN_IDENT || token == TOKEN_STRING || token == TOKEN_CONST_INT || token == TOKEN_CONST_FLOAT )
			printf( "<%d %s '%s'> ", m_line, ScriptUtil::TokenName( token ), m_tokenBuf );
		else
			printf( "<%d %s> ", m_line, ScriptUtil::TokenName( token ) );
	}

	return token;
}

//===========================================================================

ScriptToken ScriptLex::ScanIdent( int& len )
{
	len = 0;

	if ( isalpha( m_ptr[0] ) || m_ptr[0] == '_' )
	{
		for ( len = 1 ; isalnum( m_ptr[len] ) || m_ptr[len] == '_' ; )
			len++;
		return TOKEN_IDENT;
	}
	else
	{
		return TOKEN_NONE;
	}
}

//===========================================================================

ScriptToken ScriptLex::ScanNumber( int& len )
{
	ScriptToken token = TOKEN_NONE;

	len = 0;

	// quick exit if this isn't a #.
	if ( ! isdigit( m_ptr[ 0 ] ) )
		return TOKEN_NONE;

	// hex?
	if ( m_ptr[ len ] == '0' && m_ptr[ len + 1 ] == 'x' )
	{
		len += 2;
		while ( isxdigit( m_ptr[ len ] ) ) len++;
		if ( len == 2 || len > 10 )
		{
			m_err.Error( GetFile(), GetLine(), "malformed hex number" );
			return TOKEN_ERROR;
		}
		return TOKEN_CONST_HEX;
	}

	// int?
	while ( isdigit( m_ptr[ len ] ) ) len++;
	if ( len )
		token = TOKEN_CONST_INT;

	// float?
	if ( m_ptr[ len ] == '.' && isdigit( m_ptr[ len + 1 ] ) )
	{
		len++;
		token = TOKEN_CONST_FLOAT;
		while ( isdigit( m_ptr[ len ] ) ) len++;
	}
	if ( m_ptr[ len ] == 'e' || m_ptr[ len ] == 'E' )
	{
		len++;
		token = TOKEN_CONST_FLOAT;
		if ( m_ptr[ len ] == '-' && isdigit( m_ptr[ len + 1 ] ) )
			len++;
		while ( isdigit( m_ptr[ len ] ) ) len++;
	}
	if ( m_ptr[ len ] == 'f' )
	{
		token = TOKEN_CONST_FLOAT;
		len++;
	}

	return token;
}

//===========================================================================

ScriptToken ScriptLex::ScanString()
{
	if ( m_ptr[ 0 ] != '"' )
		return TOKEN_NONE;

	// find the end of the string, copy into text buffer as we go along
	char* dest = m_tokenBuf;

	for ( m_ptr++ ; m_ptr[ 0 ] != 0 ; )
	{
		if ( m_ptr[ 0 ] == '\\' && m_ptr[ 1 ] == '"' )
		{
			*dest++ = '"';
			m_ptr += 2;
		}
		else if ( m_ptr[ 0 ] == '"' )
		{
			*dest = 0;
			m_ptr++;
			return TOKEN_STRING;
		}
		else if ( m_ptr[ 0 ] == m_eol )
		{
			m_err.Error( GetFile(), GetLine(), "newline in string constant" );
			return TOKEN_ERROR;
		}
		else
		{
			if ( m_ptr[ 0 ] == m_eol )
				m_line++;
			*dest++ = *m_ptr++;
		}
	}

	m_err.Error( GetFile(), GetLine(), "unterminated string" );
	return TOKEN_ERROR;
}

//===========================================================================

void ScriptLex::SkipWhitespace()
{
	while( 1 )
	{
		while ( m_ptr[ 0 ] != 0 && isspace( m_ptr[ 0 ] ) )
		{
			if ( m_ptr[ 0 ] == m_eol )
				m_line++;
			m_ptr++;
		}

		// skip comments
		if ( m_ptr[ 0 ] == '/' && m_ptr[ 1 ] == '/' )
		{
			// skip to end-of-line
			while ( m_ptr[ 0 ] != 0 && m_ptr[ 0 ] != m_eol )
				m_ptr++;
		}
		else if ( m_ptr[ 0 ] == '/' && m_ptr[ 1 ] == '*' )
		{
			// skip to end of comment
			while ( m_ptr[ 0 ] != 0 && ( m_ptr[ 0 ] != '*' || m_ptr[ 1 ] != '/' ) )
			{
				if ( m_ptr[ 0 ] == m_eol )
					m_line++;
				m_ptr++;
			}
			if ( m_ptr[ 0 ] != 0 )
				m_ptr += 2;
		}
		else
		{
			return;
		}
	}
}

//===========================================================================

void ScriptLex::SaveState( LexState* state )
{
	state->ptr        = m_ptr;
	state->tokenValue = m_value;
	state->line       = m_line;
	strcpy( state->tokenBuf, m_tokenBuf );
	state->file       = m_currentFile;
}

//===========================================================================

void ScriptLex::RestoreState( LexState* state )
{
	m_ptr   = state->ptr;
	m_value = state->tokenValue;
	m_line  = state->line;
	strcpy( m_tokenBuf, state->tokenBuf );
	m_currentFile = state->file;
}

//===========================================================================

#endif //SCRIPT_COMPILER