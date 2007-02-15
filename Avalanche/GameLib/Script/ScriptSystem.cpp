//============================================================================
//=
//= ScriptSystem.cpp
//=
//= Runtime collection of all scripts.  Manages mapping of functions from
//= scripts to C++.
//=
//============================================================================

#include "ScriptPCH.h"

#if defined( WIN32_TOOLS_DEBUG ) || defined( WIN32_TOOLS_RELEASE )
#include <stdlib.h>
#include <windows.h>
#else
#include "Layers/Media.h"
#include "Game/Managers/ResourceManager.h"
#endif

//============================================================================

// global system variable
ScriptSystem g_ScriptSystem;

//============================================================================

ScriptSystem::ScriptSystem()
{
	m_numFunctions = 0;
	for ( int i = 0 ; i < MAX_FUNCTIONS ; i++ )
	{
		m_functions[ i ].func = 0;
		m_functions[ i ].nameHash = 0;
	}
	m_includePath[ 0 ] = 0;
	m_stack = new ScriptVarList( SCRIPT_STACK_SIZE );
	m_exeHead = 0;
	m_VMHead  = 0;
}

//============================================================================

ScriptSystem::~ScriptSystem()
{
	if ( m_stack )
	{
		delete m_stack;
		m_stack = NULL;
	}
}

//============================================================================

void ScriptSystem::RegisterFunction( const char* funcName, ScriptFunction funcPtr )
{
	ASSERTS( funcPtr, "can't register a null function" );
	ASSERTS( funcName, "can't register a null function name" );

	// hash the name
	int nameLen = strlen( funcName );
	ASSERTS( nameLen > 0, "can't register an empty function name" );
	unsigned int hash = Hash::CalcChecksum( funcName, nameLen );

	// store it in hash table
	if ( m_numFunctions < MAX_FUNCTIONS - 1 )
	{
		int hashIndex = hash % MAX_FUNCTIONS;
		while ( m_functions[ hashIndex ].func && m_functions[ hashIndex ].nameHash != hash )
			hashIndex = ( hashIndex + 1 ) % MAX_FUNCTIONS;
		if ( m_functions[ hashIndex ].func == funcPtr ) return;	// it's okay if it's already been registered
		ASSERTF( m_functions[ hashIndex ].func == 0, ( "script function '%s' has hash collision with another function", funcName ) );
		m_functions[ hashIndex ].func = funcPtr;
		m_functions[ hashIndex ].nameHash = hash;
		m_numFunctions++;
	}
	else
		ASSERTS( false, "too many functions registered - increase MAX_FUNCTIONS" );
}

//============================================================================

int ScriptSystem::Initialize( ScriptExecutable* exe )
{
	// swap and patch offsets as necessary
	if ( ! exe->Init() )
		return false;

	// register C functions
	for ( int i = 0 ; i < exe->m_numFunctions ; i++ )
	{
		unsigned int hash = exe->m_functions[ i ].nameHash;
		int hashIndex = hash % MAX_FUNCTIONS;
		while ( m_functions[ hashIndex ].func && m_functions[ hashIndex ].nameHash != hash )
			hashIndex = ( hashIndex + 1 ) % MAX_FUNCTIONS;
		ASSERTF( m_functions[ hashIndex ].func, ( "couldn't register function '%s'", exe->m_functions[ i ].name ) );
		exe->m_functions[ i ].func = m_functions[ hashIndex ].func;
	}

	return true;
}

//============================================================================

#ifdef SCRIPT_COMPILER
ScriptExecutable* ScriptSystem::CompileFile( const char* fileName, ScriptError* err, bool verbose, bool debug )
{
#if defined (WIN32) && !defined (_XBOX)
	ScriptExecutable* exe = NULL;

	// prepare to call preprocessor

	char drive[ _MAX_DRIVE ], dir[ _MAX_PATH ], file[ _MAX_FNAME ], ext[ _MAX_EXT ];
	_splitpath( fileName, drive, dir, file, ext );
	char defaultIncludePath[ _MAX_PATH ];
	sprintf( defaultIncludePath, "%s%s", drive, dir );

	char cppOutputFile[ _MAX_PATH ];
	sprintf( cppOutputFile, "%s%s%s.i", drive, dir, file );
	unlink( cppOutputFile );

	char commandLine[ 1024 ];
	sprintf( commandLine, "slang-cpp.exe", cppOutputFile );

	if ( defaultIncludePath[ 0 ] != 0 )
	{
		int len = strlen( defaultIncludePath );
		if ( defaultIncludePath[ len - 1 ] == '/' || defaultIncludePath[ len - 1 ] == '\\' )
			defaultIncludePath[ len - 1 ] = 0;

		sprintf( commandLine + strlen( commandLine ), " -I \"%s\"", defaultIncludePath );
	}

	strcat( commandLine, m_includePath );
	sprintf( commandLine + strlen( commandLine ), " \"%s\" \"%s\"", fileName, cppOutputFile );

	// call preprocessor

   STARTUPINFO         StartupInfo;
   PROCESS_INFORMATION ProcessInfo;
   memset( &StartupInfo, 0, sizeof(StartupInfo) );
   memset( &ProcessInfo, 0, sizeof(ProcessInfo) );
   StartupInfo.cb = sizeof( StartupInfo );
//	StartupInfo.dwFlags |= STARTF_USESTDHANDLES; // this, with null values in the handles, makes it so we don't see the stdout from cpp.
//	StartupInfo.hStdOutput = GetStdHandle( STD_OUTPUT_HANDLE );
//	StartupInfo.hStdError  = GetStdHandle( STD_ERROR_HANDLE );

	if ( verbose )
		printf( "EXEC [%s]\n", commandLine );

   int status = CreateProcess( NULL, commandLine, NULL, NULL, false, 0, NULL, NULL, &StartupInfo, &ProcessInfo );
   if ( status != 0 )
   {
		DWORD retVal;
      WaitForSingleObject( ProcessInfo.hProcess, (DWORD)( -1L ) );
      GetExitCodeProcess( ProcessInfo.hProcess, &retVal );
      CloseHandle( ProcessInfo.hProcess );
      CloseHandle( ProcessInfo.hThread );
   }
	else
	{
		// failed creating process, print error message
		LPVOID lpMsgBuf;
		if (!FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL ))
		{
			err->Error( "CompileFile", 0, "error creating process and creating error message" );
			return NULL;
		}

		printf( "PATH: %s\n", getenv("PATH") );

		err->Error( "CompileFile", 0, (const char*)lpMsgBuf );
		LocalFree( lpMsgBuf );

		return NULL;
	}

	// read the preprocessed file

	FILE* fp = fopen( cppOutputFile, "rb" );
	if ( !fp )
	{
		err->Error( "CompileFile", 0, "couldn't preprocess file" );
		return NULL;
	}

	fseek( fp, 0, SEEK_END );
	int len = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	char* buf = new char[ len + 1 ];
	fread( buf, 1, len, fp );
	buf[ len ] = 0;

	fclose( fp );

	unlink( cppOutputFile );

	// parse it

	ScriptSymbols	syms;
	ScriptLex		lex( buf, *err );
	ScriptParse		parse( lex, syms, *err );
	ScriptCodeGen	code( syms, *err );

	lex.SetVerbose( verbose );
	parse.SetVerbose( verbose );

	ScriptParseTree* tree = parse.Parse();

	delete [] buf;

	if ( err->ErrorCount() || !tree )
		return NULL;

	if ( verbose )
	{
		printf( "\n\n----- original tree -----\n" );
		tree->Print( 0 );
	}
	
	tree->Simplify();

	if ( verbose )
	{
		printf( "\n----- simplified tree -----\n" );
		tree->Print( 0 );
		parse.PrintSymbols();
	}

	exe = code.CreateExecutable( tree, fileName, debug );

	if ( err->ErrorCount() )
		return NULL;

	if ( verbose )
	{
		printf( "\n----- code gen symbol table -----\n" );
		code.PrintSymbols();
	}

	return exe;
#else
	return NULL;
#endif //WIN32
}
#endif //SCRIPT_COMPILER

//============================================================================

#ifdef SCRIPT_COMPILER
ScriptExecutable* ScriptSystem::CompileBuffer( const char* buffer, int bufferLen, ScriptError* err, bool verbose, bool debug )
{
#ifdef WIN32
	const char* tempName = "stdin.tmp";

	unlink( tempName );
	FILE* fp = fopen( tempName, "wb" );
	if ( !fp )
		return NULL;
	fwrite( buffer, 1, bufferLen, fp );
	fclose( fp );
	ScriptExecutable* exe = CompileFile( tempName, err, verbose, debug );
	unlink( tempName );

	return exe;
#else
	return NULL;
#endif //WIN32
}
#endif //SCRIPT_COMPILER

//============================================================================

static bool fileNamesMatch( const char* name1, const char* name2 )
{
	#define IS_SLASH(p) ( (p) == '/' || (p) == '\\' )

	while ( *name1 && *name2 &&
		     ( tolower( *name1 ) == tolower( *name2 ) ||
			    ( IS_SLASH( *name1 ) && IS_SLASH( *name2 ) ) ) )
	{
		name1++;
		name2++;
	}
	return *name1 == *name2;
}

//============================================================================

ScriptExecutable* ScriptSystem::LoadExecutable( const char* fileName )
{
	// is this already loaded and managed by the script system?
	for ( ExecutableList* exeNode = m_exeHead ; exeNode ; exeNode = exeNode->next )
		if ( fileNamesMatch( fileName, exeNode->name ) )
			return exeNode->exe;

#if defined( WIN32_TOOLS_DEBUG ) || defined( WIN32_TOOLS_RELEASE )
	FILE* fp = fopen( fileName, "rb" );
	if ( !fp )
		return NULL;
	fseek( fp, 0, SEEK_END );
	int len = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	char* buf = new char[ len ];
	fread( buf, 1, len, fp );
	fclose( fp );
	ScriptExecutable* exe = (ScriptExecutable*) buf;
	if ( ! exe->Init() )
	{
		delete [] buf;
		return NULL;
	}
#else
	// is this already loaded and managed by the resource manager?
	void* buf = g_resourceManager.FileBuffer( fileName );
	if ( buf )
	{
		ScriptExecutable* exe = (ScriptExecutable*) buf;
		bool success = exe->Init();
		ASSERT( success );
		return exe;
	}

	// load it
	s32 fileLen;
	buf = Media::Load( fileName, &fileLen, 0 );
	ScriptExecutable* exe = (ScriptExecutable*) buf;
	if ( exe && !exe->Init() )
	{
		memFree( buf );
		exe = NULL;
	}
#endif

	// if we loaded, add it to our list of managed exes
	if ( exe )
	{
		ExecutableList* newExeNode = new ExecutableList;
		ASSERT( strlen( fileName ) < sizeof( newExeNode->name ) );
		strcpy( newExeNode->name, fileName );
		newExeNode->exe = exe;
		newExeNode->next = m_exeHead;
		m_exeHead = newExeNode;
	}

	return exe;
}

//============================================================================

#ifdef SCRIPT_COMPILER
void ScriptSystem::SaveExecutable( ScriptExecutable* exe, const char* fileName )
{
#ifdef WIN32
	FILE* fp = fopen( fileName, "wb" );
	if ( fp == NULL )
	{
		fprintf( stderr, "ERROR: couldn't open '%s' for write\n", fileName );
		exit( 1 );
	}

	exe->Deinit();
	fwrite( exe, exe->m_fileSize, 1, fp );
	exe->Init();

	fclose( fp );
#endif //WIN32
}
#endif //SCRIPT_COMPILER

//============================================================================

#ifdef SCRIPT_COMPILER
void ScriptSystem::AddIncludePath( const char* fileName )
{
	sprintf( m_includePath + strlen( m_includePath ), " -I \"%s\"", fileName );
}
#endif //SCRIPT_COMPILER

//============================================================================

ScriptVM* ScriptSystem::CreateVM( ScriptExecutable* exe )
{
	ScriptVM* vm = new ScriptVM( exe, m_stack, m_stackDepth );

	// register it so we can clean it up if people are sloppy.
	VMList* vmNode = new VMList;
	vmNode->vm = vm;
	vmNode->next = m_VMHead;
	m_VMHead = vmNode;

	return vm;
}

//============================================================================

void ScriptSystem::DestroyVM( ScriptVM* vm )
{
	if ( vm )
	{
		bool found = false;

		// unregister this VM from the list of active vm's
		VMList** vmParent = &m_VMHead;
		VMList*  vmNode;
		for ( vmNode = *vmParent ; vmNode ; vmNode = *vmParent )
		{
			if ( vmNode->vm == vm )
			{
				*vmParent = vmNode->next;
				delete vmNode;
				found = true;
				break;
			}
			else
			{
				vmParent = &vmNode->next;
			}
		}

		ASSERTS( found, "DestroyVM can't find VM to free" );

		delete vm;
	}
}

//============================================================================

void ScriptSystem::Reset()
{
	// free all the VMs
	VMList* vmNode;
	VMList* vmNext;
	ASSERT( m_VMHead == NULL );
	for ( vmNode = m_VMHead ; vmNode ; vmNode = vmNext )
	{
		vmNext = vmNode->next;
		delete vmNode->vm;
		delete vmNode;
	}
	m_VMHead = NULL;

	// free executables
	PurgeUnusedExecutables();
}

//============================================================================

void ScriptSystem::PurgeUnusedExecutables()
{
	ExecutableList** parent = &m_exeHead;
	for ( ExecutableList* exeNode = *parent ; exeNode ; exeNode = *parent )
	{
		// is this exe used by any VMs?
		bool used = false;
		for ( VMList* vmNode = m_VMHead ; vmNode ; vmNode = vmNode->next )
		{
			if ( vmNode->vm->m_executable == exeNode->exe )
			{
				used = true;
				break;
			}
		}

		// delete if unused
		if ( !used )
		{
			#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
			memFree( exeNode->exe );
			#else
			delete exeNode->exe;
			#endif

			*parent = exeNode->next;
			delete exeNode;
		}
		else
		{
			parent = &exeNode->next;
		}
	}
}

//============================================================================
