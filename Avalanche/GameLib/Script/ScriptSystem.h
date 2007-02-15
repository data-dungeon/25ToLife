//===========================================================================
//=
//= ScriptSystem.h
//=
//= Runtime collection of all scripts.  Manages mapping of functions from
//= scripts to C++.
//=
//===========================================================================//

#ifndef _SCRIPT_SYSTEM_H
#define _SCRIPT_SYSTEM_H

#define MAX_FUNCTIONS 600
#define SCRIPT_REGISTER( name ) g_ScriptSystem.RegisterFunction( #name, Script_ ## name )

class ScriptVM;
class ScriptError;
class ScriptExecutable;
class ScriptVarList;
typedef void (*ScriptFunction)( ScriptVM* vm );

class ScriptSystem
{
public:
	ScriptSystem();
	~ScriptSystem();

	void	RegisterFunction( const char* funcName, ScriptFunction funcPtr );
	int	Initialize( ScriptExecutable* exe );

	ScriptExecutable* LoadExecutable( const char* fileName );
	ScriptVM*   CreateVM( ScriptExecutable* exe );
	void			DestroyVM( ScriptVM* vm );
	void			Reset();
	void			PurgeUnusedExecutables();

#ifdef SCRIPT_COMPILER
	ScriptExecutable* CompileFile( const char* fileName, ScriptError* err, bool verbose, bool debug );
	ScriptExecutable* CompileBuffer( const char* buffer, int bufferLen, ScriptError* err, bool verbose, bool debug );
	void SaveExecutable( ScriptExecutable* exe, const char* fileName );
	void			AddIncludePath( const char* fileName );
	const char* GetIncludePath() const { return m_includePath; }
#endif

private:
	struct FunctionRef
	{
		unsigned long	nameHash;
		ScriptFunction	func;
	};
	struct ExecutableList
	{
		char					name[ 56 ];
		ScriptExecutable*	exe;
		ExecutableList*	next;
	};
	struct VMList
	{
		ScriptVM*	vm;
		VMList*		next;
	};

	FunctionRef m_functions[ MAX_FUNCTIONS ];
	int			m_numFunctions;
	char			m_includePath[ 1024 ];

	ExecutableList*	m_exeHead;
	VMList*				m_VMHead;

	ScriptVarList* m_stack;
	int				m_stackDepth;
};

// a single global system
extern ScriptSystem g_ScriptSystem;

#endif //_SCRIPT_SYSTEM_H