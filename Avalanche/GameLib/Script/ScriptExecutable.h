//===========================================================================
//=
//= ScriptExecutable.h
//=
//===========================================================================

#ifndef _SCRIPT_EXECUTABLE_H
#define _SCRIPT_EXECUTABLE_H

#include "Script/ScriptVarList.h"

#define SCRIPT_FILE_ID						"SLNG"
#define SCRIPT_FILE_VERSION				0x1006
#define SCRIPT_STATUS_READY				0xcafebeef
#define SCRIPT_STATUS_NEEDS_INIT			0x12345678
#define SCRIPT_STATUS_NEEDS_SWAP_INIT	0x78563412

class ScriptVM;
typedef void (*ScriptFunction)( ScriptVM* vm );

class ScriptExecutable
{
public:
	struct Variable
	{
		char*	name;
		short	type;
		short	size;
	};

	struct StaticVariable
	{
		char* name;
		ScriptVar var;
		short	size;
	};

	struct Function
	{
		char*				name;
		unsigned int	nameHash;
		ScriptFunction func;
		int				stackDelta;	// expected stack consumption by C function (for debug)
	};

	struct VariableLookup
	{
		unsigned int	hash;
		int				varIndex;
	};

	struct LabelLookup
	{
		unsigned int	hash;
		char*				name;
		int				pc;
	};

	struct SwitchTable
	{
		int				value;
		int				pc;
	};

	struct DebugInfo
	{
		char*				file;
		int				line;
	};

	bool	Init();
	void	Deinit();
	void	Print();
	int	LookupLabel( const char* name );
	int	LookupVar( const char* name );

public:
	char					m_fileID[ 4 ];
	int					m_fileVersion;
	int					m_fileStatus;
	int					m_fileSize;
	char*					m_sourceFilename;
	int					m_numOpcodes;
	int					m_numDebugInfo;
	int					m_numVariables;
	int					m_numStatics;
	int					m_numFunctions;
	int					m_numStrings;
	int					m_numVariableLookup;
	int					m_numLabelLookup;
	int					m_numSwitchTable;
	ScriptOpcode*		m_opcodes;
	DebugInfo*			m_debugInfo;
	Variable*			m_variables;
	StaticVariable*	m_statics;
	Function*			m_functions;
	VariableLookup*	m_variableLookup;
	LabelLookup*		m_labelLookup;
	SwitchTable*		m_switchTable;
	char**				m_strings;
};

#endif //_SCRIPT_EXECUTABLE_H
