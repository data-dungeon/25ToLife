/***************************************************************************/
// Command console
// All of this is written for simplicity, not for speed
/***************************************************************************/
#include "cmdcon/CmdConPCH.h"

#include "cmdcon/varlib.h"
#include "enginehelper/filestream.h"
#include "Game/Managers/ResourceManager.h"

// Static command console variable
CommandConsole *g_consolePtr = NULL;

/***************************************************************************/
// Setup
/***************************************************************************/
CommandConsole::CommandConsole(void) :
context(32, 4, true),	// the first number is chosen to be large enough to never need list expansion
libraries(0, 4, true)
{
	context.Lock();	// this is permanent memory
	display = NULL;
#if defined(CONSUMER_BUILD)
	verbosity = VERBOSITY_NONE;
#elif defined(_DEBUG)
	verbosity = VERBOSITY_NOTIFY;
#else
	verbosity = VERBOSITY_ERROR;
#endif

	lineBuff = 0;
	lineBuffSize = 0;

	// Setup our known commands
	static COMMANDDESC commandTable[] =
	{
		{ &CommandConsole::CommandRun, CONCOMMAND_RUN, "\t\t<scriptfile> Runs script file." },
		{ &CommandConsole::CommandContextDefault, CONCOMMAND_CONTEXTDEFAULT, "\t\tRestores global context." },
		{ &CommandConsole::CommandContext, CONCOMMAND_CONTEXT, "\t<cnntext> Selects context." },
		{ &CommandConsole::CommandContextEnd, CONCOMMAND_CONTEXTEND, "\t\tRestores global context." },
		{ &CommandConsole::CommandVerbosity, CONCOMMAND_VERBOSITY, "\t<none/error/notify/debug>Verbosity." },
		{ &CommandConsole::CommandHelp, CONCOMMAND_HELP, "\t\tShows help." },
		{ &CommandConsole::CommandListContexts, CONCOMMAND_LISTCONTEXTS, "\tList available contexts." },
		{ &CommandConsole::CommandListVariables, CONCOMMAND_LISTVARIABLES, "\t\tLists variables in context." },
		{ &CommandConsole::CommandSaveContext, CONCOMMAND_SAVECONTEXT, "\t\t<outfile> Saves current context to outfile." },
		{ &CommandConsole::CommandEcho, CONCOMMAND_ECHO, "\t\t<string> Echo the string." }
	};

	// Copy that nice static array into our real array
	commands = sizeof(commandTable) / sizeof(COMMANDDESC);
	MEM_SET_ONE_SHOT_NEW_NAME("COMMANDDESC array");
	commandList = new COMMANDDESC[commands];
	for (int c = 0; c < commands; c++)
		commandList[c] = commandTable[c];

	executePath[0] = '\0';

	lastFoundContext = -1;

	// Create a default context
#define CONSOLE_DEFAULTCONTEXT_MAX_VARIABLES 60
	MEM_SET_ONE_SHOT_NEW_NAME("VariableContext");
	VariableContext *defaultContext = new VariableContext(CONSOLE_DEFAULTCONTEXT, true, CONSOLE_DEFAULTCONTEXT_MAX_VARIABLES);	// the last number is chosen to avoid the need for expanding the context, for memory purposes
	ASSERT(defaultContext);
	bool ret = AddContext(defaultContext);
	ASSERT(ret);

	// Select it
	SelectDefaultContext();
}

/***************************************************************************/
/***************************************************************************/
CommandConsole::~CommandConsole(void)
{
	delete [] lineBuff;
	delete [] commandList;
}

/***************************************************************************/
// Set the display mechanism (you must still delete)
/***************************************************************************/
void CommandConsole::SetDisplay(
CommandConsoleDisplay *consoleDisplay)
{
	display = consoleDisplay;
}

/***************************************************************************/
// Log a message to the console
/***************************************************************************/
void MYCDECL CommandConsole::Log(
VERBOSITY theVerbosity,
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	LogMessage(theVerbosity, format, args);
	va_end(args);
}

/***************************************************************************/
// Log a message with debug level to the console
/***************************************************************************/
void MYCDECL CommandConsole::Echo(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	LogMessage(VERBOSITY_NONE, format, args);
	va_end(args);
}

/***************************************************************************/
// Log a message with debug level to the console
/***************************************************************************/
void MYCDECL CommandConsole::Error(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	LogMessage(VERBOSITY_ERROR, format, args);
	va_end(args);
}

/***************************************************************************/
// Log a message with debug level to the console
/***************************************************************************/
void MYCDECL CommandConsole::Notify(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	LogMessage(VERBOSITY_NOTIFY, format, args);
	va_end(args);
}

/***************************************************************************/
// Log a message with debug level to the console
/***************************************************************************/
void MYCDECL CommandConsole::Debug(
const char *format,
...)
{
	va_list args;
	va_start(args, format);
	LogMessage(VERBOSITY_DEBUG, format, args);
	va_end(args);
}

/***************************************************************************/
// Add/delete a context (i own it now)
/***************************************************************************/
bool CommandConsole::AddContext(
VariableContext *newContext)
{
	// Does this one already exist?
	if (FindContext(newContext->Name()) != NULL)
		return(true);

	// Annoying
	if (context.Count() > 0 && strcmp(newContext->Name(), CONSOLE_DEFAULTCONTEXT) == 0)
		return(false);

	lastFoundContext = context.Count();
	bool ret = context.Add(newContext);
	return(ret);
}

/***************************************************************************/
// Get rid of the context
/***************************************************************************/
bool CommandConsole::DeleteContext(
const char *killContext)
{
	// Find it in the real array
	int i = FindContextIndex(killContext);
	if (i < 0)
		return(false);

	// Do not allow the deletion of the default context
	if (i == 0)
		return(false);

	// Doh
	if (currentContext == i)
		currentContext = 0;
	else if (currentContext > i)
		--currentContext;

	// Clean this up
	if (lastFoundContext == i)
		lastFoundContext = -1;
	else if (lastFoundContext > i) // it shifted down one
		--lastFoundContext;

	// Delete it
	return(context.Delete(i));
}

/***************************************************************************/
// Find a context with the given name
/***************************************************************************/
VariableContext *CommandConsole::FindContext(
const char *contextName)
{
	int index = FindContextIndex(contextName);
	if (index < 0)
		return(false);
	else
		return(context[index]);
}

/***************************************************************************/
// Select a context
/***************************************************************************/
bool CommandConsole::BeginContext(
const char *newContext)
{
	int index = FindContextIndex(newContext);
	if (index < 0)
		return(false);

	currentContext = index;
	return(true);
}

/***************************************************************************/
// Process a stream
/***************************************************************************/
bool CommandConsole::Process(
const char *stream)
{
	// Allocate the parsing buffer
	int streamSize = strlen(stream) + 1;
	if (streamSize > lineBuffSize)
	{
		delete [] lineBuff;
		lineBuff = NULL;
	}
	if (!lineBuff)
	{
		lineBuffSize = streamSize;
		MEM_SET_ONE_SHOT_NEW_NAME("lineBuff");
		lineBuff = new char[lineBuffSize];
		if (!lineBuff)
			return(false);
	}

	while (*stream)
	{
		// Pick off stuff into the line buffer
		char *line = lineBuff;

		// Skip leading blanks
		stream = SkipBlanks(stream);

		// Copy it in
		while (*stream != '\0' && *stream != '\n' && *stream != ';')
		{
			// Copy in a character
			if (*stream != '\r' && *stream != '\\')
			{
				// remove tabs
				if (*stream == '\t')
					*line++ = ' ';
				else if (*stream == '/')
					*line++ = '\\';
				else
					*line++ = *stream;
			}

			// Skip EOL?
			if (*stream != '\\')
				++stream;
			else
			{
				// Continuation line
				++stream;

				// Skip trailing blanks
				stream = SkipBlanks(stream);

				// skip \r\n
				while (*stream == '\r' || *stream == '\n')
					++stream;
			}
		}

		// Terminate
		*line = '\0';
		RemoveTrailing(line);

		// Process the line as a command
		if (lineBuff[0])
			ProcessCommand(lineBuff);

		// Skip the newline?
		if (*stream == '\n' || *stream == ';')
			++stream;
	}

	return(true);
}

/***************************************************************************/
// Setup the default script execute path
/***************************************************************************/
bool CommandConsole::SetExecutePath(
const char *path)
{
	// Are they clearing it?
	executePath[0] = '\0';
	if (!path)
		return(true);

	// Skip leading spaces (bl33t)
	while (*path == ' ')
		path++;
	if (*path == '\0')
		return(false);

	// Copy it in
	strcpy(executePath, path);
	RemoveTrailing(executePath);

	// Make sure it ends with a slash
	if (executePath[strlen(executePath) - 1] != '\\')
		strcat(executePath, "\\");

	return(true);
}

/***************************************************************************/
/***************************************************************************/
const char *CommandConsole::GetExecutePath()
{
	return executePath;
}

/***************************************************************************/
// Bind a script file to a context
/***************************************************************************/
bool CommandConsole::BindScript(
const char *context,
const char *pathFile)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Bind it
	return(vc->BindScript(pathFile));
}

/***************************************************************************/
// Does this script exist?
/***************************************************************************/
bool CommandConsole::ScriptExists(
const char *fileName)
{
	// Create the file name
	char trueFileName[512];
	if (*fileName)
		MakeScriptFileName(fileName, trueFileName);
	else if (currentContext >= 0)
		MakeScriptFileName(context[currentContext]->Name(), trueFileName);

	// Try to open it
	return(FileStream::Exists(trueFileName));
}

/***************************************************************************/
// Execute a context using it's default script
/***************************************************************************/
bool CommandConsole::ExecuteContext(
const char *context)
{
	VariableContext *vc = FindContext(context);
	const char *script = NULL;
	if (vc != NULL)
		script = vc->Script();
	if (script == NULL)
		script = context;

	return(ExecuteFile(script));
}

/***************************************************************************/
// Execute a stream (from disk)
/***************************************************************************/
bool CommandConsole::ExecuteFile(
const char *fileName)
{
	// Create the file name
	char trueFileName[40];
	if (*fileName)
		MakeScriptFileName(fileName, trueFileName);
	else if (currentContext >= 0)
		MakeScriptFileName(context[currentContext]->Name(), trueFileName);

	// Is it in a mounted library?
	if ( IsInLibrary( trueFileName ) )
	{
		Notify("Executing '%s' from library.", trueFileName);
		VERBOSITY old = Verbosity();
		SetVerbosity(VERBOSITY_ERROR);
		bool ret = ExecuteFromLibrary(trueFileName);
		SetVerbosity(old);
		return ret;
	}

	// Is it loaded as a resource?
	if ( g_resourceManager.IsLoaded( trueFileName ) )
	{
		Notify("Executing '%s' from resource manager.", trueFileName);
		char* script = (char*) g_resourceManager.FileBuffer( trueFileName );
		VERBOSITY old = Verbosity();
		SetVerbosity(VERBOSITY_ERROR);
		bool ret = Process(script);
		SetVerbosity(old);
		return ret;
	}

	// Open the file
	FileStream fs;
	if (!fs.Open(trueFileName, FileStream::READ))
	{
		Error("Cannot open '%s' for executing.\n", trueFileName);
		return(false);
	}

	// How big is the file?
	int size = fs.Size();

	// Allocate the buffer
	MEM_SET_ONE_SHOT_NEW_NAME("CmdCon::ExecFile");
	char *script = (char *)memAlloc(size + 1);
	if (!script)
	{
		Error("Memory error executing '%s'.", fileName);
		return(false);
	}

	// Read this bad boy in
	bool ret = (fs.Read(script, size) == size);
	fs.Close();

	// Did something go wrong?
	if (!ret)
	{
		Error("File read error executing '%s'.", fileName);
		memFree(script);
		return(false);
	}
	script[size] = '\0';

	// We did it!
	Notify("Executing '%s'.", trueFileName);

	// Process it
	VERBOSITY old = Verbosity();
	SetVerbosity(VERBOSITY_ERROR);
	ret = Process(script);
	SetVerbosity(old);
	memFree(script);

	return(ret);
}

/***************************************************************************/
/***************************************************************************/
CommandConsole::ScriptHandle CommandConsole::LoadScript(
const char *pathFile)
{
	// Create the file name
	char trueFileName[40];
	if (*pathFile)
		MakeScriptFileName(pathFile, trueFileName);
	else if (currentContext >= 0)
		MakeScriptFileName(context[currentContext]->Name(), trueFileName);

	// If it's in a mounted library, don't load - instead fake the handle
	// by putting "LIB:filename" so we can execute from handle later.
	if ( IsInLibrary( trueFileName ) )
	{
		Script *script = new Script(trueFileName);
		return (ScriptHandle)script;
	}

	// Open a file
	FileStream fs;
	if (!fs.Open(trueFileName, FileStream::READ))
	{
		Error("Cannot open '%s' for load.\n", trueFileName);
		return NULL;
	}

		// How big is the file?
	int size = fs.Size();

	// Allocate the buffer
	MEM_SET_ONE_SHOT_NEW_NAME("CmdCon::LoadScript");
	Script *script = new Script(trueFileName, size + 1);
	ASSERT(script && script->Data());
	if (!script || !script->Data())
	{
		delete script;
		Error("Memory error loading '%s'.", trueFileName);
		return NULL;
	}

	// Read this bad boy in
	bool ret = (fs.Read(script->Data(), size) == size);
	fs.Close();
	if (!ret)
	{
		Error("File read error on script '%s'.", trueFileName);
		memFree(script);
		return NULL;
	}
	script->Data()[size] = '\0';

	// Give them back the handle
	return (ScriptHandle)script;
}

/***************************************************************************/
/***************************************************************************/
bool CommandConsole::ExecuteScript(
ScriptHandle handle,
const char *forceContext)
{
	// Is there a script?
	if (!handle)
		return false;

	// Access the script
	Script *script = (Script *)handle;
	Notify("Executing '%s'.", script->Name());

	// Push the verbosity
	VERBOSITY old = Verbosity();
	SetVerbosity(VERBOSITY_ERROR);

	// Force a context?
	if (forceContext)
	{
		char fakeCmd[128];
		sprintf(fakeCmd, "context %s", forceContext);
		ExecuteString(fakeCmd);
	}

	// process
	bool ret = false;

	// If this is library based, go execute from libs
	if ( script->InLibrary() )
		ret = ExecuteFromLibrary( script->Name() );
	else
		ret = Process(script->Data());

	// End it
	if (forceContext)
		ExecuteString("end");

	// restore verbosity
	SetVerbosity(old);

	return true;
}

/***************************************************************************/
/***************************************************************************/
void CommandConsole::UnloadScript(
ScriptHandle handle)
{
	Script *script = (Script *)handle;
	delete script;
}

/***************************************************************************/
/***************************************************************************/
bool CommandConsole::FindValueQuick(
ScriptHandle handle,
const char *varName,
char *value,
int len)
{
	ASSERT(len > 0);
	value[0] = '\0';

	// Doh!
	Script *script = (Script *)handle;
	if (!script)
		return false;

	// Wee!
	if (script->InLibrary())
	{
		for ( int i = 0 ; i < libraries.Count() ; i++ )
		{
			if ( libraries[ i ]->FindValueQuick( script->Name(), varName, value, len) )
				return true;
		}
		return false;
	}

	// Parse
	const char *stream = script->Data();
	while (*stream)
	{
		// Pick off stuff into the line buffer
		char *line = lineBuff;

		// Skip leading blanks
		stream = SkipBlanks(stream);

		// Copy it in
		while (*stream != '\0' && *stream != '\n' && *stream != ';')
		{
			// Copy in a character
			if (*stream != '\r' && *stream != '\\')
			{
				// remove tabs
				if (*stream == '\t')
					*line++ = ' ';
				else if (*stream == '/')
					*line++ = '\\';
				else
					*line++ = *stream;
			}

			// Skip EOL?
			if (*stream != '\\')
				++stream;
			else
			{
				// Continuation line
				++stream;

				// Skip trailing blanks
				stream = SkipBlanks(stream);

				// skip \r\n
				while (*stream == '\r' || *stream == '\n')
					++stream;
			}
		}

		// Terminate
		*line = '\0';
		RemoveTrailing(line);

		// Process the line as a command
		if (lineBuff[0])
		{
			char command[64];
			const char *restOfLine = ParseWord(lineBuff, " ", command, sizeof(command) - 1);
			if (strcmpi(command, varName))
			{
				restOfLine = SkipBlanks(restOfLine);
				strncpy(value, restOfLine, len - 1);
				value[len] = '\0';
				return true;
			}
		}

		// Skip the newline?
		if (*stream == '\n' || *stream == ';')
			++stream;
	}

	// not found
	return false;
}

/***************************************************************************/
// Save a context
// Leave scriptName NULL to use either the bound or default name
/***************************************************************************/
bool CommandConsole::SaveContext(
const char *context,
const char *scriptName)
{
	VariableContext *vc = FindContext(context);
	if (!vc)
		return(false);

	// fill out the script name
	char subName[512];
	if (scriptName == NULL || *scriptName == '\0')
	{
		scriptName = vc->Script();
		if (scriptName == NULL)
			scriptName = context;
	}
	else if ((scriptName[0] == '.') &&
			((scriptName[1] == '/') || (scriptName[1] == '\\')) &&
			(vc->Script() != NULL))
	{
		strcpy (subName, vc->Script());
		for (int j = strlen(subName) - 1; j >= 0; j--)
		{
			if ((subName[j] == '/') || (subName[j] == '\\'))
			{
				subName[j + 1] = '\0';
				strcat(subName, scriptName + 2);
				scriptName = subName;
				break;
			}
		}
	}

	// Make the filename
	char trueFileName[512];
	MakeScriptFileName(scriptName, trueFileName);

	/* Ensure that ".VAR" files are written into the data-independent path (where 99.9% of them exist) */

	char *useFileName = Media::GetCommonFileName(trueFileName);	//get the data-independent path name

	bool bCurMangleState = Media::GetNameManglingState();
	Media::EnableNameMangling(false);																		//allow opening of exact filename passed in
	FileStream *fs = FileStream::NewAndOpen(useFileName, FileStream::WRITE, true);	//use data-independent path
	Media::EnableNameMangling(bCurMangleState);

	if (!fs)
	{
		Error("Error creating '%s' for output.", useFileName);
		return(false);
	}

	fs->Print("# Generated script for '%s'\n", vc->Name());
	fs->Print("\n");
	fs->Print("context %s\n\n", vc->Name());

	// Enumerate the variables
	int i = 0;
	ConsoleVariable *v;
	while ((v = (*vc)(i)) != NULL)
	{
		// Get the value
		if (!v->Command() && !v->NoWrite())
		{
			char valueBuffer[4096];
			bool ret = (v->Get(valueBuffer, sizeof(valueBuffer) - 1) != NULL);
			if (ret)
			{
				if (v->ReadOnly())
					fs->Print("# %s %s\n", v->Name(), valueBuffer);
				else
					fs->Print("%s %s\n", v->Name(), valueBuffer);
			}
		}
	}


	fs->Print("\nend\n");
	delete fs;

	// Give them some status
	Notify("%s saved in '%s'.", vc->Name(), useFileName);

	return(true);
}

/***************************************************************************/
// Save a context CSV
// Leave scriptName NULL to use either the bound or default name
/***************************************************************************/
bool CommandConsole::SaveContextCSV(
const char *context,
bool doHeader,
const char *csvName)
{
	VariableContext *vc = FindContext(context);
	if (!vc)
		return(false);

	// fill out the script name
	char subName[512];
	if (csvName == NULL || *csvName == '\0')
	{
		csvName = vc->Script();
		if (csvName == NULL)
			csvName = context;
	}
	else if ((csvName[0] == '.') &&
			((csvName[1] == '/') || (csvName[1] == '\\')) &&
			(vc->Script() != NULL))
	{
		strcpy (subName, vc->Script());
		for (int j = strlen(subName) - 1; j >= 0; j--)
		{
			if ((subName[j] == '/') || (subName[j] == '\\'))
			{
				subName[j + 1] = '\0';
				strcat(subName, csvName + 2);
				csvName = subName;
				break;
			}
		}
	}

	// Make the filename
	char trueFileName[512];
	MakeScriptFileName(csvName, trueFileName);

	// Replace the extension
	int ext = strlen(trueFileName) - 4;
	if (strcmpi(&trueFileName[ext], CONSCRIPT_DEFAULTEXT) == 0)
		strcpy(&trueFileName[ext], ".csv");
	else
		strcat(trueFileName, ".csv");

	/* Ensure that ".VAR" files are written into the data-independent path (where 99.9% of them exist) */
	char *useFileName = Media::GetCommonFileName(trueFileName);	//get the data-independent path name

	bool bCurMangleState = Media::GetNameManglingState();
	Media::EnableNameMangling(false);																		//allow opening of exact filename passed in
	FileStream *fs = FileStream::NewAndOpen(useFileName, FileStream::WRITE, true);	//use data-independent path
	Media::EnableNameMangling(bCurMangleState);

	if (!fs)
	{
		Error("Error creating '%s' for output.", useFileName);
		return(false);
	}

	// Enumerate the variables
	int i = 0, fields = 0;
	ConsoleVariable *v;

	// Do the header
	while ((v = (*vc)(i)) != NULL)
	{
		// Get the value
		if (!v->Command() && !v->NoWrite())
		{
			if (!v->ReadOnly())
			{
				if (fields != 0)
					fs->Print(", ");
				fs->Print("%s", v->Name());
				++fields;
			}
		}
	}
	fs->Print("\n");

	// Do the data
	i = 0, fields = 0;
	while ((v = (*vc)(i)) != NULL)
	{
		// Get the value
		if (!v->Command() && !v->NoWrite())
		{
			if (!v->ReadOnly())
			{
				if (fields != 0)
					fs->Print(", ");

				char valueBuffer[4096];
				if (v->Get(valueBuffer, sizeof(valueBuffer) - 1) != NULL)
					fs->Print("%s", valueBuffer);

				++fields;
			}
		}
	}
	fs->Print("\n");

	delete fs;

	// Give them some status
	Notify("%s saved in '%s'.", vc->Name(), useFileName);

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
#ifndef PS2 // this causes problems with PS2... as int & int32 are the same type  (whereas on PC, int32==long)
bool CommandConsole::CreateVar(
const char *context,
const char *name,
int &var,
bool hex,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	return(CreateVar(context, name, (uint&)var, hex, flags, true, i_messageOwner, i_messageData));
}
#endif //!PS2

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
#if !defined(PS2) // this causes problems with PS2... as int & int32 are the same type (whereas on PC, int32==long)
bool CommandConsole::CreateVar(
const char *context,
const char *name,
uint &var,
bool hex,
uint flags,
bool isSigned,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarInt");
	ConsoleVariable *v = new CVarInt(flags, name, var, !isSigned, hex, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}
#endif //!PS2

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
int32 &var,
bool hex,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	return(CreateVar(context, name, (uint32&)var, hex, flags, true, i_messageOwner, i_messageData));
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
uint32 &var,
bool hex,
uint flags,
bool isSigned,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarInt32");
	ConsoleVariable *v = new CVarInt32(flags, name, var, !isSigned, hex, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
int16 &var,
bool hex,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	return(CreateVar(context, name, (uint16&)var, hex, flags, true, i_messageOwner, i_messageData));
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
uint16 &var,
bool hex,
uint flags,
bool isSigned,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarInt16");
	ConsoleVariable *v = new CVarInt16(flags, name, var, !isSigned, hex, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
int8 &var,
bool hex,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	return(CreateVar(context, name, (uint8&)var, hex, flags, true, i_messageOwner, i_messageData));
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
uint8 &var,
bool hex,
uint flags,
bool isSigned,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarInt8");
	ConsoleVariable *v = new CVarInt8(flags, name, var, !isSigned, hex, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
float &var,
CONREALTYPE type,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	switch (type)
	{
		case CONVAR_REAL:
			return(CreateVar(context, name, var, "real", Identity, Identity, flags, i_messageOwner, i_messageData));
			break;

		case CONVAR_INVREAL:
			return(CreateVar(context, name, var, "1/real", Math::Inverse, Math::Inverse, flags, i_messageOwner, i_messageData));
			break;

		case CONVAR_DEGREES:
		{
			float convert = CONVAR_DEGTORAD;
			return(CreateVar(context, name, var, "deg", ScaleMul, ScaleDiv, *(void**)&convert, flags, i_messageOwner, i_messageData));
			break;
		}

		case CONVAR_INVDEGREES:
		{
			float convert = CONVAR_DEGTORAD;
			return(CreateVar(context, name, var, "1/deg", InverseScale, InverseScale,  *(void**)&convert, flags, i_messageOwner, i_messageData));
			break;
		}
	}

	// Unreachable code
	return(false);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
float &var,
bool invert,
float convert,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	if (convert == 1.0f)
	{
		if (invert)
			return(CreateVar(context, name, var, "1/real", Math::Inverse, Math::Inverse, flags, i_messageOwner, i_messageData));
		else
			return(CreateVar(context, name, var, "real", Identity, Identity, flags, i_messageOwner, i_messageData));
	}

	if (convert == CONVAR_DEGTORAD)
	{
		if (invert)
			return(CreateVar(context, name, var, "1/deg", InverseScale, InverseScale, *(void**)&convert, flags, i_messageOwner, i_messageData));
		else
			return(CreateVar(context, name, var, "deg", ScaleMul, ScaleDiv, *(void**)&convert, flags, i_messageOwner, i_messageData));
	}

	static char type[64];
	if (invert)
	{
		sprintf(type, "*1/%g", convert);
		return(CreateVar(context, name, var, type, InverseScale, InverseScale, *(void**)&convert, flags, i_messageOwner, i_messageData));
	}

	sprintf(type, "*%g", convert);
	return(CreateVar(context, name, var, type, ScaleMul, ScaleDiv, *(void**)&convert, flags, i_messageOwner, i_messageData));
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
float &var,
const char* type,
TSimpleTransformFn setTransform,
TSimpleTransformFn getTransform,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarReal");
	ConsoleVariable *v = new CVarReal(flags, name, var, type, setTransform, getTransform, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
float &var,
const char* type,
TArgTransformFn setTransform,
TArgTransformFn getTransform,
void* arg,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarReal");
	ConsoleVariable *v = new CVarReal(flags, name, var, type, setTransform, getTransform, arg, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
float &var,
float &invVar,
CONREALTYPE type,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	ConsoleVariable *v;
	switch (type)
	{
		case CONVAR_REAL:
			MEM_SET_ONE_SHOT_NEW_NAME("CVarRealInv");
			v = new CVarRealInv(flags, name, var, invVar, 1.0f, i_messageOwner, i_messageData);
			break;

		case CONVAR_DEGREES:
			MEM_SET_ONE_SHOT_NEW_NAME("CVarRealInv");
			v = new CVarRealInv(flags, name, var, invVar, CONVAR_DEGTORAD, i_messageOwner, i_messageData);
			break;

		default:
			// unsupported type
			v= NULL;
			break;
	}

	// Did it work?
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
bool &var,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarBool");
	ConsoleVariable *v = new CVarBool(flags, name, var, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateToggle(
const char *context,
const char *name,
bool &var,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarToggle");
	ConsoleVariable *v = new CVarToggle(name, var, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
char *var,
int maxlen,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarString");
	ConsoleVariable *v = new CVarString(flags, name, var, maxlen, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
Vector3Packed *var,
bool normalize,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarVector3");
	ConsoleVariable *v = new CVarVector3(flags, name, *var, normalize, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
Vector4 *var,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarVector4");
	ConsoleVariable *v = new CVarVector4(flags, name, *var, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
Matrix4x4 &var,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarMatrix4x4");
	ConsoleVariable *v = new CVarMatrix4x4(flags, name, var, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Create a variable with a context
/***************************************************************************/
bool CommandConsole::CreateVar(
const char *context,
const char *name,
Matrix3x3 &var,
uint flags,
CMessageOwner *i_messageOwner,
void *i_messageData)
{
	// Get our context
	VariableContext *vc = CreateContext(context);
	if (!vc)
		return(false);

	// Create the variable
	MEM_SET_ONE_SHOT_NEW_NAME("CVarMatrix3x3");
	ConsoleVariable *v = new CVarMatrix3x3(flags, name, var, i_messageOwner, i_messageData);
	if (!v)
		return(false);

	// Add it to the context
	if (!vc->Add(v))
	{
		delete v;
		return(false);
	}

	return(true);
}

/***************************************************************************/
// Make a context permanent
/***************************************************************************/
void CommandConsole::SetPermanence(
const char *contextName,
bool permanent)
{
	VariableContext *c = CreateContext(contextName);
	if (c)
		c->SetPermanence(true);
}

/***************************************************************************/
/***************************************************************************/
void CommandConsole::Clear(void)
{
	/* "permanent" contexts are ones that are added at game initialization, like "global". all others are per-game-level, */
	/* and should be cleaned up as part of game engine reset */

	for (int i = 0; i < context.Count();)
	{
		if (context[i]->Permanent())
			++i;
		else
			context.Delete(i);
	}

	currentContext = 0;
	lastFoundContext = -1;

	/* clean up lineBuff. it will get re-malloc'd as needed. don't delete */
	/* commandList, it is a one-time buffer created by constructor. executePath is */
	/* typically a one-time malloc to point to default script directory (typically */
	/* cscript). this is used by MakeScriptFileName to build correct file names, so */
	/* leave it there */

	delete [] lineBuff;
	lineBuff = NULL;
	lineBuffSize = 0;
}

/***************************************************************************/
// Display a string
/***************************************************************************/
void CommandConsole::DisplayString(
const char *string)
{
	if (display)
		display->NewLine(string);
}

/***************************************************************************/
// Message logging (0 - none > 0 more) range 0-3
/***************************************************************************/
void CommandConsole::LogMessage(
VERBOSITY theVerbosity,
const char *message)
{
	if (theVerbosity <= verbosity)
		DisplayString(message);
}

/***************************************************************************/
// Log message with format/string
/***************************************************************************/
void CommandConsole::LogMessage(
VERBOSITY theVerbosity,
const char *format,
va_list args)
{
	// this could overflow
	char temp[1024];
	vsprintf(temp, format, args);
	LogMessage(theVerbosity, temp);
}

/***************************************************************************/
// Find a context with the given name
/***************************************************************************/
int CommandConsole::FindContextIndex(
const char *contextName)
{
	char lookFor[VCONTEXT_NAME_SIZE];
	strncpy(lookFor, contextName, sizeof(lookFor) - 1);
	lookFor[sizeof(lookFor) - 1] = '\0';
	strlwr(lookFor);

	// Quickie
	if (lastFoundContext >= 0 && strcmp(lookFor, context[lastFoundContext]->Name()) == 0)
		return lastFoundContext;

	// Linear search for now, someday a bsearch if I get Sort working
	for (int i = 0; i < context.Count(); i++)
	{
		if (strcmp(lookFor, context[i]->Name()) == 0)
			return(i);
	}

	return(-1);
}

/***************************************************************************/
// Process a command (stream picks off stuff into commands)
/***************************************************************************/
bool CommandConsole::ProcessCommand(
const char *commandLine)
{
	// Is this just a comment?
	if (*commandLine == '#')
		return(true);

	// Should be no leading spaces, pick off command
	char command[256];
	commandLine = ParseWord(commandLine, " ", command, sizeof(command) - 1);

	// Scan the known commands
	for (int c = 0; c < commands; c++)
		if (strcmpi(command, commandList[c].name) == 0)
			return((this->*commandList[c].process)(command, commandLine));

	// This is a variable op if we made it here
	char ctext[256];
	bool hasContext;
	const char *var = ParseWord(command, ".", ctext, sizeof(ctext) - 1, &hasContext);
	if (hasContext)
		VariableOp(ctext, var, commandLine);
	else
		VariableOp(NULL, command, commandLine);

	return(true);
}

/***************************************************************************/
// Commands
/***************************************************************************/
bool CommandConsole::CommandRun(
const char *command,
const char *param)
{
	// Execute the file
	return(ExecuteFile(param));
}

/***************************************************************************/
// Commands
/***************************************************************************/
bool CommandConsole::CommandContextDefault(
const char *command,
const char *param)
{
	SelectDefaultContext();
	Notify("Setting global context.");
	return(true);
}

/***************************************************************************/
// Commands
/***************************************************************************/
bool CommandConsole::CommandContext(
const char *command,
const char *param)
{
	if (*param == '\0')
	{
		// Report our context
		if (InDefaultContext())
			Echo("Global context.");
		else if (currentContext >= 0)
			Echo("Context '%s'.", context[currentContext]->Name());
		else
			Echo("ERROR!");
		return(true);
	}
	else
	{
		// Select a context
		bool ret = BeginContext(param);
		if (ret)
			Notify("Setting context '%s'.", param);
		else
			Error("Unknown context '%s'.", param);
		return(ret);
	}
}

/***************************************************************************/
// Commands
/***************************************************************************/
bool CommandConsole::CommandContextEnd(
const char *command,
const char *param)
{
	Notify("Setting global context.");
	EndContext();

	return(true);
}

/***************************************************************************/
// Commands
/***************************************************************************/
bool CommandConsole::CommandVerbosity(
const char *command,
const char *param)
{
	// Get verbosity?
	if (*param == '\0')
	{
		if (verbosity == VERBOSITY_NONE)
			Echo("Verbosity = 'none'.");
		else if (verbosity == VERBOSITY_ERROR)
			Echo("Verbosity = 'error'.");
		else if (verbosity == VERBOSITY_NOTIFY)
			Echo("Verbosity = 'notify'.");
		else
			Echo("Verbosity = 'debug'.");
	}
	else
	{
		// Set verbosity
		if (strcmpi(param, CONCOMMAND_VERBOSITY0) == 0)
		{
			verbosity = VERBOSITY_NONE;
			Notify("Verbosity = 'none'.");
		}
		else if (strcmpi(param, CONCOMMAND_VERBOSITY1) == 0)
		{
			verbosity = VERBOSITY_ERROR;
			Notify("Verbosity = 'error'.");
		}
		else if (strcmpi(param, CONCOMMAND_VERBOSITY2) == 0)
		{
			verbosity = VERBOSITY_NOTIFY;
			Notify("Verbosity = 'notify'.");
		}
		else if (strcmpi(param, CONCOMMAND_VERBOSITY3) == 0)
		{
			verbosity = VERBOSITY_DEBUG;
			Notify("Verbosity = 'debug'.");
		}
		else
			Error("'%s' is an invalid verbosity setting.", param);
	}

	return(true);
}

/***************************************************************************/
// Commands
/***************************************************************************/
bool CommandConsole::CommandHelp(
const char *command,
const char *param)
{
	// List out the commands
	Echo("Commmads:");
	for (int c = 0; c < commands; c++)
		Echo("%s %s", commandList[c].name, commandList[c].help);
	Echo(" ");

	return(true);
}

/***************************************************************************/
// Commands
/***************************************************************************/
bool CommandConsole::CommandListContexts(
const char *command,
const char *param)
{
	// List out the contexts
	Echo("Contexts:");
	for (int c = 0; c < context.Count(); c++)
		Echo("    %s %s", context[c]->Name(), c == currentContext ? "\t(active)" : "");
	Echo("done.");

	return(true);
}

/***************************************************************************/
// Commands
/***************************************************************************/
bool CommandConsole::CommandListVariables(
const char *command,
const char *param)
{
	// Push the current context
	int pushContext = currentContext;

	if (currentContext < 0)
	{
		Error("Somethingis wrong, no context set!");
		return(false);
	}

	// Are they selecting a context?
	if (*param && !BeginContext(param))
	{
		Error("Unknown variable context '%s'.", param);
		return(false);
	}

	VariableContext *listContext = context[currentContext];
	ASSERT(listContext);
	Echo("Variables in %s:", listContext->Name());
	const char *script = listContext->Script();
	Echo("Bound script: %s", script ? script : "<none>");

	// Enumerate the variables
	int i = 0;
	ConsoleVariable *v;
	while ((v = (*listContext)(i)) != NULL)
	{
		// Is this write only?
		if (v->WriteOnly())
			Echo("    [%s] %s (write only)", v->Type(), v->Name());
		else
		{
			// Get the value
			char valueBuffer[4096];
			bool ret = (v->Get(valueBuffer, sizeof(valueBuffer) - 1) != NULL);

			if (!ret)
				Echo("    [%s] %s (error)", v->Type(), v->Name());
			else
			{
				if (v->Command())
					Echo("    [%s] %s = %s (command)", v->Type(), v->Name(), valueBuffer);
				else
					Echo("    [%s] %s = %s", v->Type(), v->Name(), valueBuffer);
			}
		}
	}

	Echo("done.");

	// Restore the context
	currentContext = pushContext;
	return(true);
}

/***************************************************************************/
// Commands
/***************************************************************************/
bool CommandConsole::CommandSaveContext(
const char *command,
const char *param)
{
	// What is the base name for this script?
	if (currentContext < 0)
		return false;

	return(SaveContext(context[currentContext]->Name(), param));
}

/***************************************************************************/
// Commands
/***************************************************************************/
bool CommandConsole::CommandEcho(
const char *command,
const char *param)
{
	Echo(param);
	return(true);
}

/***************************************************************************/
// Handle a variable inspect/set
/***************************************************************************/
bool CommandConsole::VariableOp(
const char *contextName,
const char *var,
const char *value)
{
	// Push the current context
	int pushContext = currentContext;

	// Are they selecting a context?
	if (contextName)
	{
		if (*contextName == '\0')
			SelectDefaultContext();
		else if (!BeginContext(contextName))
		{
			Notify("Unknown variable context '%s'.", contextName);
			return(false);
		}
	}

	// Find the variable in this context
	ConsoleVariable *v = context[currentContext]->Find(var);

	// Try in default context
	if (!v)
		v = DefaultContext()->Find(var);

	// Perhaps this is just a known context
	if (!v && BeginContext(var))
	{
		Notify("Setting context '%s'.", var);
		return(true);
	}

	// Any luck?
	if (!v)
	{
		if (strcmp(var, "class") == 0)
			Notify("Unknown variable '%s'.", var);
		else	
			Error("Unknown variable '%s = %s'.", var, value ? value : "<nil>"); 
		currentContext = pushContext;
		return(false);
	}

	// Is this a set or inspect?
	if ((value && *value) || v->Command())
	{
		// This is a set

		// Is this read only?
		if (v->ReadOnly())
		{
			Error("'%s' is read only.", var);
			currentContext = pushContext;
			return(false);
		}

		// Set the value, could value be a variable/context?
		char valueContextString[256];
		bool valueHasContext;
		const char *valueVarString = ParseWord(value, ".", valueContextString, sizeof(valueContextString) - 1, &valueHasContext);
		int valueContext;
		if (!valueHasContext)
		{
			valueContext = currentContext;
			valueVarString = value;
		}
		else
		{
			if (*valueContextString)
				valueContext = FindContextIndex(valueContextString);
			else
				valueContext = 0;
		}

		// Find the variable
		ConsoleVariable *valueVar;
		if (valueContext)
			valueVar = context[valueContext]->Find(valueVarString);
		else
			valueVar = NULL;

		// actually set it
		bool ret;
		char valueBuffer[4096];
		if (!valueVar || valueVar->WriteOnly() || !valueVar->Get(valueBuffer, sizeof(valueBuffer) - 1))
			ret = v->Set(value);
		else
			ret = v->Set(valueBuffer);

		// Did it work?
		if (!ret)
		{
			Error("Error setting '%s' to '%s'.", var, value);
			currentContext = pushContext;
			return(false);
		}

		// Signal a change
		v->Modified();

		// Silent?
		if (!v->Silent())
		{
			bool ret = (v->Get(valueBuffer, sizeof(valueBuffer) - 1) != NULL);
			if (!ret)
			{
				Error("Error getting value for '%s'.", var);
				currentContext = pushContext;
				return(false);
			}

			if (contextName)
				Notify("%s.%s = %s", contextName, var, valueBuffer);
			else
				Notify("%s = %s", var, valueBuffer);
		}
	}
	else
	{
		// Just a report

		// Is this write only?
		if (v->WriteOnly())
		{
			Error("'%s' is write only.", var);
			currentContext = pushContext;
			return(false);
		}

		// Get the value
		char valueBuffer[4096];
		bool ret = (v->Get(valueBuffer, sizeof(valueBuffer) - 1) != NULL);
		if (!ret)
		{
			Error("Error getting value for '%s'.", var);
			currentContext = pushContext;
			return(false);
		}

		// Show the value
		if (contextName)
			Echo("%s.%s = %s", context, var, valueBuffer);
		else
			Echo("%s = %s", var, valueBuffer);
	}

	// Restore the context
	currentContext = pushContext;
	return(true);
}

/***************************************************************************/
// Parsing commands
/***************************************************************************/
const char *CommandConsole::ParseWord(
const char *parse,
const char *delim,
char *word,
int maxWord,
bool *foundDelim)
{
	// Skip leading
	parse = SkipBlanks(parse);

	// Yank off the word
	while (*parse != '\0' && !Delim(*parse, delim) && maxWord > 0)
	{
		*word++ = *parse++;
		--maxWord;
	}
	*word = '\0';

	// Delimeter?
	bool atDelim = Delim(*parse, delim);
	if (foundDelim)
		*foundDelim = atDelim;

	// Skip the delimieter
	if (atDelim)
		++parse;

	// Skip trailing blanks
	parse = SkipBlanks(parse);
	return(parse);
}

/***************************************************************************/
// Is this character in the delimiter list?
/***************************************************************************/
bool CommandConsole::Delim(
char c,
const char *delim)
{
	while (*delim)
	{
		if (*delim == c)
			return(true);
		delim++;
	}

	return(false);
}

/***************************************************************************/
/***************************************************************************/
const char *CommandConsole::SkipBlanks(
const char *parse)
{
	while (*parse == ' ' || *parse == '\t')
		++parse;

	return(parse);
}

/***************************************************************************/
/***************************************************************************/
void CommandConsole::RemoveTrailing(
char *parse)
{
	int len = strlen(parse);
	while (len > 0 && Delim(parse[len - 1], " \t"))
	{
		--len;
		parse[len] = '\0';
	}
}

/***************************************************************************/
/***************************************************************************/
VariableContext *CommandConsole::CreateContext(
const char *contextName)
{
	// Find the context
	VariableContext *vc;
	if (contextName == NULL)
		vc = DefaultContext();
	else if (*contextName == '\0')
		vc = context[currentContext];
	else
		vc = FindContext(contextName);

	if (vc)
		return(vc);

	// Create the context
	MEM_SET_ONE_SHOT_NEW_NAME("VariableContext");
	vc = new VariableContext(contextName, false);
	if (!AddContext(vc))
	{
		delete vc;
		return(NULL);
	}

	return(vc);
}

/***************************************************************************/
// Make a filename
/***************************************************************************/
void CommandConsole::MakeScriptFileName(
const char *pathFile,
char *outPathFile)
{
	// If it has a path, don't add the default path
	if (HasPath(pathFile))
		strcpy(outPathFile, pathFile);
	else
	{
		if (executePath[0] == '\0')
			strcpy(outPathFile, pathFile);
		else
		{
			strcpy(outPathFile, executePath);
			strcat(outPathFile, pathFile);
		}
	}
	RemoveTrailing(outPathFile);

	// Is there an extension?
	int len = strlen(outPathFile) - 5;
	if (len < 0)
		len = 0;
	char *test = &outPathFile[len];
	while (*test != '\0')
	{
		if (*test == '.')
			return;
		++test;
	}

	// There was no extension
	strcat(outPathFile, CONSCRIPT_DEFAULTEXT);
}

/***************************************************************************/
// Does this path file actually have a path specified?
/***************************************************************************/
bool CommandConsole::HasPath(
const char *pathFile)
{
	int len = strlen(pathFile) - 1;
	if (len <= 0)
		return(false);
	for (int i = 0; i < len; i++)
	{
		if (pathFile[i] == '\\')
			return(true);
	}

	return(false);
}

/***************************************************************************/
// Load a library
/***************************************************************************/
bool CommandConsole::LoadLibrary(
const char* filename)
{
	// bail if we're already loaded
	for ( int i = 0 ; i < libraries.Count() ; i++ )
		if ( stricmp( libraries[ i ]->Name(), filename ) == 0 )
			return true;

	// create a lib and load it
	CVarLib* lib = new CVarLib;
	if ( !lib->Load( filename ) )
	{
		delete lib;
		return false;
	}

	libraries.Add( lib );

	return true;
}

/***************************************************************************/
// Unload a library
/***************************************************************************/
bool CommandConsole::UnloadLibrary(
const char* filename)
{
	// find us in the libs
	for ( int i = 0 ; i < libraries.Count() ; i++ )
	{
		if ( stricmp( libraries[ i ]->Name(), filename ) == 0 )
		{
			libraries.Delete( i );	// auto calls the destructor
			if ( libraries.Count() == 0 )
				libraries.Reset();

			return true;
		}
	}

	return false;
}

/***************************************************************************/
// Find if a script is in a loaded library
/***************************************************************************/
bool CommandConsole::IsInLibrary(
const char* filename)
{
	for ( int i = 0 ; i < libraries.Count() ; i++ )
		if ( libraries[ i ]->Has( filename ) )
			return true;
	return false;
}

/***************************************************************************/
// Execute a script from a library
/***************************************************************************/
bool CommandConsole::ExecuteFromLibrary(
const char* filename)
{
	for ( int i = 0 ; i < libraries.Count() ; i++ )
		if ( libraries[ i ]->Execute( filename ) )
			return true;
	return false;
}
