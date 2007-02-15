//===========================================================================
//=
//= ScriptLex.h
//=
//===========================================================================

#ifndef _SCRIPT_LEX_H
#define _SCRIPT_LEX_H

#ifdef SCRIPT_COMPILER

// class
class ScriptLex
{
public:
	struct LexState
	{
		const char* ptr;
		char			tokenBuf[ 256 ];
		ScriptValue	tokenValue;
		int			line;
		int			file;
	};

	ScriptLex( const char* buffer, ScriptError& err );
	~ScriptLex();

	ScriptValue TokenValue() const { return m_value; }
	const char* TokenText() const { return m_tokenBuf; }
	ScriptToken NextToken();
	void			SetVerbose( bool verbose ) { m_verbose = verbose; }
	int			GetLine() const { return m_line; }
	char*			GetFile()       { return m_fileNames[ m_currentFile ]; }

	void			SaveState( LexState* state );
	void			RestoreState( LexState* state );

private:
	ScriptToken ScanIdent( int& len );
	ScriptToken ScanNumber( int& len );
	ScriptToken ScanString();
	void			SkipWhitespace();

	struct History { const char* ptr; int line; };

private:
	ScriptError&		m_err;
	const char*			m_ptr;					// current pointer into the input text buffer
	ScriptValue			m_value;					// current token value, if CONST_INT or CONST_FLOAT
	char					m_tokenBuf[ 256 ];	// current token text
	bool					m_verbose;
	int					m_line;
	int					m_eol;					// end of line delimiter
	ScriptStringList	m_fileNames;			// list of filenames we've processed
	int					m_currentFile;
};

#endif //SCRIPT_COMPILER
#endif //_SCRIPT_LEX_H