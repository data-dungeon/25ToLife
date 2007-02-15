//============================================================================
//=
//= ScriptPreprocessor.h
//=
//= Preprocess a script file handling #define, #include, ...
//=
//============================================================================

#ifndef _SCRIPT_PREPROCESSOR_H
#define _SCRIPT_PREPROCESSOR_H

#ifdef SCRIPT_COMPILER

#include <stdarg.h>

class ScriptPreprocessor
{
public:
	ScriptPreprocessor( ScriptError* err );
	~ScriptPreprocessor();

	bool ProcessBuffer( const char* filename, char* buffer );
	bool ProcessFile( const char* filename );
	void PrintSymbols();

private:
	bool GetLine( char** inBufPtr, char* outBuf );
	bool Subst( const char* filename, int line, char* buffer );
	bool GetIdentifier( char** inBufPtr, char* identBuf );
	void OutputReset();
	void Output( const char* file, int line, const char* fmt, ... );
	bool FindIncludeFile( char* file );

	ScriptError*	m_err;
	ScriptSymbols	m_syms;
	char				m_outputFile[ 256 ];
	int				m_outputLine;
};

//============================================================================

#endif //SCRIPT_COMPILER
#endif //_SCRIPT_PREPROCESSOR_H