/***************************************************************************/
// Command console
// All of this is written for simplicity, not for speed
/***************************************************************************/

//
// A note about messages.
//
// If a message owner is specified, a message named "ConsoleVarModified" will be
// sent to the owner when a variable is changed. If a value for the message data
// is provided and is not equal to ~0x0, that data will be sent with the
// message; otherwise, a pointer to the name of the variable being changed will
// be sent.
//

#ifndef CMDCON_H
#define CMDCON_H

#include "container/dynlist.h"

#include "cmdcon/convar.h"
#include "cmdcon/ccondisp.h"

/* forward declarations */

class VariableContext;
class CVarLib;

#ifdef WIN32
//define for those platforms that have console interactivity (so contexts are left open for alteration)
#define INTERACTIVE_CONSOLE
#endif

// Reserved words
#define CONCOMMAND_RUN					"exec"
#define CONCOMMAND_CONTEXT				"context"
#define CONCOMMAND_CONTEXTEND			"end"
#define CONCOMMAND_CONTEXTDEFAULT	"global"
#define CONCOMMAND_VERBOSITY			"verbosity"
#define CONCOMMAND_HELP					"help"
#define CONCOMMAND_LISTCONTEXTS		"contexts"
#define CONCOMMAND_LISTVARIABLES		"list"
#define CONCOMMAND_SAVECONTEXT		"save"
#define CONCOMMAND_ECHO					"echo"

#define CONCOMMAND_VERBOSITY0			"none"
#define CONCOMMAND_VERBOSITY1			"error"
#define CONCOMMAND_VERBOSITY2			"notify"
#define CONCOMMAND_VERBOSITY3			"debug"

#define CONSCRIPT_DEFAULTEXT			".var"

#define CONSOLE_DEFAULTCONTEXT		"global"

#ifdef WIN32
#define MYCDECL __cdecl
#else
#define MYCDECL
#endif

// Console verbosity levels
enum VERBOSITY
{
	VERBOSITY_NONE = 0x0,
	VERBOSITY_ERROR = 0x1,
	VERBOSITY_NOTIFY = 0x2,
	VERBOSITY_DEBUG = 0x3
};

// Types of console variables
enum CONREALTYPE
{
	CONVAR_REAL,
	CONVAR_INVREAL,
	CONVAR_DEGREES,
	CONVAR_INVDEGREES
};

// The all powerful console
class CommandConsole
{
public:
	// Duh
	CommandConsole(void);
	~CommandConsole(void);

	//--------------------------------------------------------------------
	// This following section details all the members you ever need to see
	//--------------------------------------------------------------------

	// Set the display mechanism (you must still delete)
  	void SetDisplay(CommandConsoleDisplay *consoleDisplay);

	// Log a message to the console
	void MYCDECL Log(VERBOSITY theVerbosity, const char *format, ...);
	void MYCDECL Echo(const char *format, ...);
	void MYCDECL Error(const char *format, ...);
	void MYCDECL Notify(const char *format, ...);
	void MYCDECL Debug(const char *format, ...);

	// Utilities for creating/variables (null context uses global, "" uses current)
#if !defined(PS2) // this causes problems with PS2... as int & int32 are the same type  (whereas on PC, int32==long)
	bool CreateVar(const char *context, const char *name, int &var, bool hex = false, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, uint &var, bool hex = false, uint flags = CONVAR_DEFAULT, bool isSigned = false, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
#endif //!PS2
	bool CreateVar(const char *context, const char *name, int32 &var, bool hex = false, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, uint32 &var, bool hex = false, uint flags = CONVAR_DEFAULT, bool isSigned = false, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, int16 &var, bool hex = false, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, uint16 &var, bool hex = false, uint flags = CONVAR_DEFAULT, bool isSigned = false, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, int8 &var, bool hex = false, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, uint8 &var, bool hex = false, uint flags = CONVAR_DEFAULT, bool isSigned = false, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, float &var, CONREALTYPE type, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, float &var, bool invert = false , float convert = 1.0f, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, float &var, const char* type, TSimpleTransformFn setTransform, TSimpleTransformFn getTransform, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, float &var, const char* type, TArgTransformFn setTransform, TArgTransformFn getTransform, void *arg, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, float &var, float &invVar, CONREALTYPE type = CONVAR_REAL, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, bool &var, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateToggle(const char *context, const char *name, bool &var, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, char *var, int maxlen, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, Vector3Packed *var, bool normalize = false, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, Vector4 *var, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, Matrix4x4 &var, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	bool CreateVar(const char *context, const char *name, Matrix3x3 &var, uint flags = CONVAR_DEFAULT, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Make a context permanent
	void SetPermanence(const char *contextName, bool permanent);

	// Clear out everything that is not marked permanent
	void Clear(void);

	// Process a stream
	bool Process(const char *stream);

	// Set the default execute load path, this is
	// only used on scripts that don't have a path on them
	bool SetExecutePath(const char *path);
	const char *GetExecutePath();

	// Bind a script file to a context
	bool BindScript(const char *context, const char *pathFile);

	// Does this script exist?
	bool ScriptExists(const char *pathFile);

	// Execute a context using it's default script
	bool ExecuteContext(const char *context);

	// Execute a stream (from disk)
	bool ExecuteFile(const char *pathFile);

	// Load a script as a data chunk and execute it at will
	typedef void *ScriptHandle;
	ScriptHandle LoadScript(const char *pathFile);
	bool ExecuteScript(ScriptHandle handle, const char *forceContext = NULL);
	void UnloadScript(ScriptHandle handle);

	// Find a value quickly!
	bool FindValueQuick(ScriptHandle handle, const char *varName, char *value, int len);

	// Just execute a damn string, dammit.
	bool ExecuteString(const char *string)
		{ return ProcessCommand(string); }

	// Handle .var libraries
	bool LoadLibrary(const char* filename);
	bool UnloadLibrary(const char* filename);
	bool IsInLibrary(const char* filename);
	bool ExecuteFromLibrary(const char* filename);

	// Save a context
	// Leave scriptName NULL to use either the bound or default name
	bool SaveContext(const char *context, const char *scriptName = NULL);

	// Write a CSV for a context
	bool SaveContextCSV(const char *context, bool doHeader = true, const char *csvName = NULL);

	//------------------------
	// Minor details
	//------------------------

	// Error verbosity
	void SetVerbosity(VERBOSITY newVerbosity)
#if defined(CONSUMER_BUILD)
		{ verbosity = VERBOSITY_NONE; }
#else
		{ verbosity = newVerbosity; }
#endif
	VERBOSITY Verbosity(void)
		{ return(verbosity); }

	// Add/delete a context (i own it now)
	bool AddContext(VariableContext *newContext);
	bool DeleteContext(const char *killContext);
	VariableContext *FindContext(const char *contextName);

	// Access the default context
	void SelectDefaultContext(void)
		{ currentContext = 0; }
	VariableContext *DefaultContext(void)
		{ return(context[0]); }
	bool InDefaultContext(void)
		{ return(currentContext == 0); }

	// Select a context
	bool BeginContext(const char *newContext);
	void EndContext(void)
		{ SelectDefaultContext(); }
	VariableContext *CurrentContext(void)
		{ return(currentContext < 0 ? NULL : context[currentContext]); }

	// Take advantage of the ability to prepend paths.
	void MakeScriptFileName(const char *pathFile, char *outPathFile);

private:
	// Our display mechanism
	CommandConsoleDisplay 		*display;
	void DisplayString(const char *string);

	// Message logging (0 - none > 0 more) range 0-3
	VERBOSITY verbosity;
	void LogMessage(VERBOSITY theVerbosity, const char *message);
	void LogMessage(VERBOSITY theVerbosity, const char *format, va_list args);

	// A list of variable context's
	DynList<VariableContext>	context;
	int								lastFoundContext;
	int								FindContextIndex(const char *contextName);

	// Our current context
	int								currentContext;

	// Parsing crap
	char								*lineBuff;
	int								lineBuffSize;

	// A list of loaded libraries
	DynList<CVarLib>				libraries;

	// Process a command (stream picks off stuff into commands)
	bool ProcessCommand(const char *commandLine);

	// Stuff for processing a command
	typedef bool (CommandConsole::*COMMANDFN)(const char *command, const char *parameters);
	struct COMMANDDESC
	{
		COMMANDFN	process;
		char 			*name;
		char 			*help;
	};
	COMMANDDESC	*commandList;
	int			commands;

	// Commands
	bool CommandRun(const char *command, const char *parameters);
	bool CommandContextDefault(const char *command, const char *parameters);
	bool CommandContext(const char *command, const char *parameters);
	bool CommandContextEnd(const char *command, const char *parameters);
	bool CommandVerbosity(const char *command, const char *parameters);
	bool CommandHelp(const char *command, const char *parameters);
	bool CommandListContexts(const char *command, const char *parameters);
	bool CommandListVariables(const char *command, const char *parameters);
	bool CommandSaveContext(const char *command, const char *parameters);
	bool CommandEcho(const char *command, const char *parameters);

	// Handle a variable inspect/set
	bool VariableOp(const char *context, const char *var, const char *value);

	// Parseing commands
	static const char *ParseWord(const char *parseFrom, const char *delim, char *word, int maxWord, bool *foundDelim = NULL);
	static bool Delim(char c, const char *delim);
	static const char *SkipBlanks(const char *parse);
	static void RemoveTrailing(char *parse);

	// Ensure a context exists
	VariableContext *CreateContext(const char *context);

	// Make a filename
	char executePath[64];

	// Does this path file actually have a path specified?
	static bool HasPath(const char *pathFile);

	// Script support
	class Script
	{
	public:
		Script(const char *name, int dataSize)
		{
			ASSERT(strlen(name) < sizeof(m_name));
			strcpy(m_name, name);
			m_data = (char*) memAllocForIO(dataSize);
			ASSERT(m_data);
			m_inLibrary = false;
		}
		Script(const char *name)
		{
			ASSERT(strlen(name) < sizeof(m_name));
			strcpy(m_name, name);
			m_data = NULL;
			m_inLibrary = true;
		}
		~Script()
		{
			if (m_data)
				memFree(m_data);
		}

		const char *Name()
			{ return m_name; }
		char *Data()
			{ return m_data; }
		bool InLibrary()
			{ return m_inLibrary; }

	private:
		char	m_name[64];
		char	*m_data;
		bool	m_inLibrary;
	};

public:
	// Some transforms for real vars
	static float Identity(float v)
		{ return(v); }

	static float ScaleMul(float v, void* scaleFactor)
		{ return(v * (*(float*)&scaleFactor)); }

	static float ScaleDiv(float v, void* scaleFactor)
		{ return(v / (*(float*)&scaleFactor)); }

	static float InverseScale(float v, void* scaleFactor)
		{ return(1.0f / (v * (*(float*)&scaleFactor))); }
};

// command console singleton
extern CommandConsole *g_consolePtr;
inline CommandConsole &SafePtr(CommandConsole *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_console	SafePtr(g_consolePtr)

#endif
