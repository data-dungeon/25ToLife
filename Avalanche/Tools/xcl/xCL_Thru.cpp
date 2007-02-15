//==============================================================================
//
//  xCL_Thru.cpp
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <process.h>
#include <windows.h>

#include "xCL.hpp"
#include "xCL_Thru.hpp"

//==============================================================================
//  VARIABLES
//==============================================================================

//==============================================================================
//  FUNCTIONS
//==============================================================================

// We've been getting some odd behavior with the executable name in argv[0] sometimes
// not getting quoted on some machines, using _spawnvp.  This version will insure
// proper quoting and use the windows CreateProcess directly.

// Note - this might seem a little gross or excessive, but it's exactly the same
// crap done internally inside _spawnvp, except it forgets to quote the args...

int spawnQuoted( int flag, char* appName, char** args )
{
	int i;
	unsigned long retVal = 1;

	//-- compute length of command line
	int len = 0;
	for ( i = 0 ; args[ i ] != NULL ; i++ )
		len += strlen( args[ i ] ) + 3;		// length + double quotes + space separator

	//-- make the quotes version of the command line
	char* commandLine = new char[ len + 1 ];	// +1 for final null.
	char* cPtr = commandLine;
	*cPtr = 0;
	for ( i = 0 ; args[ i ] != NULL ; i++ )
		cPtr += sprintf( cPtr, "\"%s\" ", args[ i ] );
	if ( cPtr > commandLine )					// remove trailing space
		*( --cPtr ) = 0;

	//-- spawn it
	STARTUPINFO         StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	memset( &StartupInfo, 0, sizeof(StartupInfo) );
	memset( &ProcessInfo, 0, sizeof(ProcessInfo) );
	StartupInfo.cb = sizeof( StartupInfo );

	if ( g_Verbose )
		printf( "[xCl] EXEC: %s\n", commandLine );

	int status = CreateProcess( appName, commandLine, NULL, NULL, true, 0, NULL, NULL, &StartupInfo, &ProcessInfo );
	if ( status != 0 )
	{
		WaitForSingleObject( ProcessInfo.hProcess, (DWORD)( -1L ) );
		GetExitCodeProcess( ProcessInfo.hProcess, &retVal );
		CloseHandle( ProcessInfo.hProcess );
		CloseHandle( ProcessInfo.hThread );
	}
	else
	{
		printf( "[xCl] ERROR executing %s\n", commandLine );
	}

	delete [] commandLine;
	return retVal;
}

//==============================================================================
//  FUNCTIONS
//==============================================================================
int PassThru( int argc, char **argv )
{

	// We are going to call the original cl.exe or link.exe.  We need to do two
	// things:
	//  (1) We must replace "cl" with "_cl", or "link" with "_link".
	//  (2) We must place quotes around any argument with spaces.

	// First, take care of argument 0, which is the cl or link invokation.
	char	Whole[ _MAX_PATH   ];
	char	Dir  [ _MAX_DIR    ];
	char	Drive[ _MAX_DRIVE  ];
	char	FName[ _MAX_FNAME  ];
	char	Ext  [ _MAX_EXT    ];
	char	*pNewFName = FName;
	char	**ppNewArgv;

	int	iArg;

	ppNewArgv = new char* [ argc + 1 ];	// leave room for terminating NULL
	
	for( iArg = 1; iArg < argc; iArg++)
	{
		ppNewArgv[iArg] = argv[iArg];
	}
	ppNewArgv[argc] = NULL;

	// Break argument 0 into its various parts.
	_splitpath( argv[0], Drive, Dir, FName, Ext );

	// Special case check...
	//
	// If the FName is 'xCL' then we have a recursive call!
	// We're probably debugging xCL or something link that.
	//
	if( stricmp( FName, "xCL" ) == 0 )
		return( 0 );

	// special case for jim henn's machine! when linking a library, instead of getting the first argument as
	// "c:\program files\microsoft visual studio\vc98\bin\link.exe", we're just getting "lib"!
	if( stricmp( FName, "LIB" ) == 0 )
	{
		/* have to make assumptions about where .exe's are if not set? or are we by default in the right directory? */

		// TJC - this isnt nessisary any more since the code below searches for
		// the executables if it cant find them.
		/*
		if (strlen(Drive) == 0)
			strcpy(Drive, "C:");
		if (strlen(Dir) == 0)
			strcpy(Dir, "\\Program Files\\Microsoft Visual Studio\\VC98\\BIN\\");
		*/
		if (strlen(Ext) == 0)
			strcpy(Ext, ".exe");
		strcpy(FName, "LINK");
	}

	// Change "CL" to "_cl".
	if( stricmp( FName, "CL" ) == 0 )
		pNewFName = "_cl";

	// Change "LINK" to "_link".
	if( stricmp( FName, "LINK" ) == 0 )
		pNewFName = "_link";

	// Put it all back together, and make it the first argument
	_makepath( Whole, Drive, Dir, pNewFName, Ext );
	ppNewArgv[0] = Whole;

	// TJC - 
	// we need to search the PATH for _cl... because of SN Systems
	// snCL that might run before us, we might not be in the same
	// directory as _cl anymore...
	// we only do this if we really need to...
	DWORD type;
	BOOL foundIt = GetBinaryType(ppNewArgv[0], &type);
	if(!foundIt)
	{
		char* pointerToPlaceInFullPathWhereExecutableNameStarts;
		
		SearchPath(NULL, ppNewArgv[0], ".exe", _MAX_PATH, Whole, &pointerToPlaceInFullPathWhereExecutableNameStarts);
	}

	// debugging

	if (g_Verbose)
	{
		printf("PassThru input:\n");
		for (int i = 0; i < argc; i++)
			printf("   %d: %s\n", i, argv[i]);
		printf("PassThru output:\n");
		i = 0;
		while (ppNewArgv[i] != NULL)
		{
			printf("   %d: %s\n", i, ppNewArgv[i]);
			i++;
		}
		if ((i > 0) && (ppNewArgv[i - 1][0] == '@'))
		{
			FILE *pResponseFile = fopen(&ppNewArgv[i - 1][1], "rt");
			if (pResponseFile)
			{
				printf("Contents of response file:\n");
				char c;
				while (fread(&c, 1, 1, pResponseFile) == 1)
					printf("%c", c);
				fclose(pResponseFile);
			}
		}
		fflush( stdout );
	}

	//
	// Now that we have our command line, invoke it!
	//
	int Result = spawnQuoted( _P_WAIT, ppNewArgv[0], &ppNewArgv[0] );
	
	return( Result );    
}

//==============================================================================
