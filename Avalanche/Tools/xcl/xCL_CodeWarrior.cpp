//==============================================================================
//
//  xCL_CodeWarrior.cpp
//
//==============================================================================

//========================  History ============================================
/*

  Moved the Object to be linked in before any libraries - Rafi 2/10/2001 
  Added 'closure' to the argument when generating the map file - Rafi

*/
 //==============================================================================


//==============================================================================
//  INCLUDES
//==============================================================================

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <process.h>
#include <windows.h>
#include <assert.h>

#include "xCL.hpp"
#include "xCL_CodeWarrior.hpp"

//==============================================================================
//  DEFINES
//==============================================================================

#define PC  1
#define PS2 2

//==============================================================================
//  VARIABLES
//==============================================================================

static cmd_line_data&   C = g_CmdLineData;

static token_list       ObjLib;
static token_list       Section;

static char*            pBase;

static char*            pLabel[] = { "<ERROR>", "PC", "PS2" };

//==============================================================================
//  FUNCTIONS
//==============================================================================

//==============================================================================
//  The output from the compiler (errors and warnings and other) was sent to a 
//  file.  This function will parse through that file attempting to pick out
//  the errors and warnings.  When it finds something, it reformats the message
//  and prints it to standard out.  The new format is one that Developer Studio
//  will be able to recognize.  All text which is NOT recognized is just printed
//  out as is for the user in case it is important.

/* 
** The format for errors is:
**

   Error : <MESSAGE>
<FILENAME.EXT> line <line_number>  <offending_code>

**
** The format for warnings is the same except for the first word:
**

 Warning : <MESSAGE>
<FILENAME.EXT> line <line_number>  <offending_code>

**
** There is a tab ('\t') between the <line_number> and the <offending_code>.
** Sometimes the <MESSAGE> can span multiple lines:
**

   Error : <MESSAGE>
           <MORE_MESSAGE>
<FILENAME.EXT> line <line_number>  <offending_code>

**
** Samples:
**

   Error : undefined identifier 'rebar'
Test.cpp line 5      rebar  r;
   Error : undefined identifier 'foobar'
Test.cpp line 8      foobar f;
 Warning : variable / argument 'a' is not used in function
Test.cpp line 6      float a;

**
** Note that the PS2 compiler seems to inject a blank line.  Like so:
**

   Error : undefined identifier 'rebar'

Test.cpp line 5      rebar  r;
   Error : undefined identifier 'foobar'

Test.cpp line 8      foobar f;
 Warning : variable / argument 'a' is not used in function

Test.cpp line 6      float a;

**
*/


static
void ProcessMessages( char* pFileName )
{
    FILE* pFile;
    char* pCursor;   
    char  OriginalText    [ 8192 ];
    char  CurrentDirectory[ 8192 ];
    char  Message         [ 8192 ];
    char  SourceFile      [ 8192 ];
    char  Path            [ 8192 ];
    char  LineNumber      [   16 ];
    char* pLine2;
    
    // Open the error message file.
    pFile = fopen( pFileName, "rt" );
    if( !pFile )
    {
        printf( "xCL -- ERROR: Unable to open file for read: %s\n", pFileName );
        return;
    }   

    // Get the current directory.
    if( _getcwd( CurrentDirectory, 8192 ) == NULL )
        printf( "xCL -- WARNING: Unable to read current directory.\n" );

    // Loop until we have processed everything in the file.
    while( !feof( pFile ) )
    {
        bool  Error   = false;
        bool  Warning = false;
        char* p;
        char* q;
        int   l;

        // Read in a line from the file.  Position cursor to end of buffer, 
        // ready for more reading.
        p       = fgets( OriginalText, 8192, pFile );

        pCursor = OriginalText + strlen( OriginalText );

        // If we didn't get anything, then just continue to the next iteration,
        if( !p )
            continue;

        // Got an error message?
        if( strncmp( OriginalText, "   Error : ", 11 ) == 0 )
            Error = true;

        // Got a warning message?
        if( strncmp( OriginalText, " Warning : ", 11 ) == 0 )
            Warning = true;

        // If we have NEITHER an error NOR a warning...
        if( !(Error || Warning ) )
        {
            // Print out everything we have in our OriginalText buffer.
            printf( "%s", OriginalText );
            continue;
        }

        //
        // At this point, we seem to have either an error OR a warning.
        //

        // Copy the message portion.
        strcpy( Message, OriginalText+11 );

        // We have to handle any additional lines containing MESSAGE, or any 
        // blank lines from the PS2 tools.
        //
        // When we are done, we want "pLine2" pointing at the text (typically
        // the second line in the original text) where the line containing the
        // filename and line number are found.

        // As much as I hate it, the best way to implement this section is with
        // a damned goto.  So here is the label.
        GET_ANOTHER_LINE:

        // Nick off any trailing '\n' from the Message we are building.
        l = strlen( Message );
        if( Message[l-1] == '\n' )  
            Message[l-1] = '\0';

        // Get the next line of text from the file.
        pLine2  = pCursor;
        p       = fgets( pCursor, (pCursor - OriginalText + 8192), pFile );
        pCursor = OriginalText + strlen( OriginalText );

        // Did we get anything?
        if( !p )
        {
            // We didn't get any more text.  (Possibly EOF.)
            // Print out everything in the OriginalText buffer.
            printf( "%s", OriginalText );
            continue;
        }

        // Do we have a multi-line message?
        if( strncmp( pLine2, "           ", 11 ) == 0 )
        {
            // Add to the Message we are building, and get another line of text.
            strcat( Message, "  " );
            strcat( Message, pLine2+11 );
            goto GET_ANOTHER_LINE;
        }

        // Or did we get a blank line from a PS2 tool?
        if( pLine2[0] == '\n' )
            goto GET_ANOTHER_LINE;

        //
        // At this point, pLine2 should point to the text which is typically
        // on the second line of the original message.  This line should contain
        // the file name and line number.
        //

        // Search for " line ".
        p = strstr( pLine2, " line " );

        // If we did NOT find " line ".
        if( !p )
        {
            // There is an error or warning, but we couldn't parse out a file 
            // and line number.  Print out the text in our OriginalText buffer
            // because it may be important.
            printf( "%s", OriginalText );
            continue;
        }

        // At this point, we have an error or warning, and we have
        // found " line " in the message.  The file name we want is
        // between pLine2 and p.  Lift it out.
        strncpy( SourceFile, pLine2, (p-pLine2) );
        SourceFile[ p-pLine2 ] = '\0';

        // Advance past " line ".
        p += 6;

        // The line number is now at p.  We need to lift it out into our buffer
        // called LineNumber.
        q = LineNumber;
        while( !isspace(*p) )
        {
            *q = *p;
            p++;
            q++;
        }
        *q = '\0';

        //
        // We now have all of the parts of the original message to produce a
        // message suitable to our needs.
        //

        // Ah!  But, the filename may not include a path.  If it does not, then
        // we must provide one.  We will use the current directory.
        _splitpath( SourceFile, NULL, Path, NULL, NULL );
        if( Path[0] == '\0' )
        {
            // There is no path.  Use the current directory.
            printf( "%s\\", CurrentDirectory );
        }

        // Print our message.
        printf( "%s(%s) : %s : %s\n", 
                SourceFile, 
                LineNumber,
                (Error ? "Error" : "Warning"),
                Message );

        fflush( stdout );
    }

    fclose( pFile );
}

//==============================================================================

static int CompileSourceCode( void )
{

	int		Result = EXIT_SUCCESS;
	int		i;

	char		OutFile[_MAX_PATH]; // CodeWarrior compiler output
	char		App[_MAX_PATH];
	char		AppArgv[_MAX_PATH];
	char		ResponseFile[_MAX_PATH];
	char		ObjectFile[_MAX_PATH];
	char		NewRName[_MAX_PATH]; // New response file name

	FILE		*pNewRFile;              // New response file pointer

	HANDLE	hSTDOUTSave,
				hSTDOUTNew;

	SECURITY_ATTRIBUTES		secAttr;

	//--------------------------------------------------------------------------
	// Setup local and temporary variables.
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	// Build the CodeWarrior response file.
	//--------------------------------------------------------------------------

	// Determine response file name.
	sprintf( NewRName, "%s\\_CW_ResponseFile.txt", C.m_OutputPath );

	// Open the response file.
	pNewRFile = fopen( NewRName, "wt" );
	if( pNewRFile == NULL )
	{
		printf( "xCL -- ERROR: Unable to open CodeWarrior response file: %s\n", NewRName );
		return( EXIT_FAILURE );
	}   

	// Add some standard compile options.    
	fprintf( pNewRFile, " -nowraplines \n" );       // Don't wrap output message lines
	fprintf( pNewRFile, " -nostderr    \n" );       // Don't use stderr for messages
	fprintf( pNewRFile, " -enum int    \n" );       // Enums are integer sized (32 bit)
	fprintf( pNewRFile, " -Cpp_exceptions off\n" ); // Turn off exceptions

	if( C.m_CompileOnly)
	{
		fprintf( pNewRFile, " -c           \n" );       // Compile only; don't link
	}

	/* force symbols for ps2 so we can run the profiler in a release build */

	if ((C.m_Target == TARGET_PS2) && (C.m_Vendor == VENDOR_METROWERKS))
		C.m_Symbols = true;
	if ( C.m_Symbols)
	{
		fprintf( pNewRFile, " -g           \n" );       // Add symbols
	}
	else
	{
		fprintf( pNewRFile, " -sym off     \n" );       // Delete symbols
	}

	if( C.m_RTTI)
	{
		fprintf( pNewRFile, " -RTTI on     \n" );       // RTTI enabled
	}
	else
	{
		fprintf( pNewRFile, " -RTTI off    \n" );       // RTTI disabled
	}

	/* if creating pre-compiled header, let compiler know */

	if (C.m_CreatePrecompiled)
	{
		assert(strlen(g_CmdLineData.m_PCHFile) != 0);
		fprintf(pNewRFile, " -precompile \"%s\"\n", g_CmdLineData.m_PCHFile);	// create specified pre-compiled header file
	}

	/* ignore pre-compiled header file unless using it */

	if (C.m_UsePrecompiled == false)
		strcpy(C.m_PCHFile, "");

	// Specify prefix file. if using a pre-compiled header, that overrides prefix file (can't use both) */
	if (C.m_PCHFile[0] != '\0')
		fprintf( pNewRFile, " -prefix \"%s\"\n", g_CmdLineData.m_PCHFile );
	else if (C.m_PrefixFile[0] != '\0')
		fprintf( pNewRFile, " -prefix \"%s\"\n", g_CmdLineData.m_PrefixFile );

	// Add PS2 specific standard compile options.
	if( C.m_Target == TARGET_PS2 )
	{
		if (C.m_ForceC == false)
			fprintf( pNewRFile, " -lang c++  \n" );
		fprintf( pNewRFile, " -msgstyle IDE\n" );       // Use "IDE" message style-- they are converted to VC-style errors by ProcessMessages
		fprintf( pNewRFile, " -ansi off\n" );
		fprintf( pNewRFile, " -msext on\n" );
		fprintf( pNewRFile, " -w noemptydecl\n" );
		fprintf( pNewRFile, " -w nounusedarg\n" );
		fprintf( pNewRFile, " -w noimplicitconv\n" );
		fprintf( pNewRFile, " -w nounwanted\n" );
		fprintf( pNewRFile, " -gccinc\n" );
	}

	// Add GCN specific standard compile options.
	if( C.m_Target == TARGET_GAMECUBE )
	{
		fprintf( pNewRFile, " -nodefaults  \n" );       // No default libs/includes/etc
		fprintf( pNewRFile, " -nosyspath   \n" );       // ignore difference between "" and <>
		fprintf( pNewRFile, " -cwd source\n"   );       // use dir of including file first
		fprintf( pNewRFile, " -proc gekko\n" );
		fprintf( pNewRFile, " -align powerpc \n" );
		fprintf( pNewRFile, " -fp hardware\n" );
		fprintf( pNewRFile, " -maf on\n" );
		fprintf( pNewRFile, " -msgstyle IDE\n" );       // Use "IDE" message style
	}

	// Add compile optimization specification.
	switch( C.m_Optimization )
	{
		case 'd':
			fprintf( pNewRFile, " -opt off\n" );
			fprintf( pNewRFile, " -inline off\n" );
			break;

		case '1':
		case 's':
			fprintf( pNewRFile, " -opt space,level=2\n" );
			fprintf( pNewRFile, " -inline auto,level=6\n" );
			break;

		case '2':
		case 't':
		case 'x':
			fprintf( pNewRFile, " -O4,p\n" );
			fprintf( pNewRFile, " -inline auto\n" );
			break;
	}

	// ignore standard include directory?

//	if (C.m_StdInc == false)
//		fprintf( pNewRFile, " -nostdinc\n" );

	// Add the defines to the response file.
	for( i = 0; i < C.m_Define.GetCount(); i++ )
	{
		fprintf( pNewRFile, " -D%s \n", C.m_Define[i] );
	}

	// Add compile options to the response file.
	for( i = 0; i < C.m_CompileOptions.GetCount(); i++ )
	{
		fprintf( pNewRFile, " %s \n", C.m_CompileOptions[i] );
	}

	// Add current directory as an include directory.
	fprintf( pNewRFile, " -i  .\n" );
//	fprintf( pNewRFile, " -i-\n" );

	// Add the include paths.
	for( i = 0; i < C.m_IncludeDir.GetCount(); i++ )
	{
		fprintf( pNewRFile, " -i  \"%s\" \n", C.m_IncludeDir[i] );
	}

	// That's all for the new response file.
	fclose( pNewRFile );

	fflush( stdout );

	//--------------------------------------------------------------------------
	// Execute!
	//--------------------------------------------------------------------------

	// We redirect the CW output to a file and then parse it later.
	// Create the name for this file.
	sprintf( OutFile, "%s\\_CompilerOutput.txt", C.m_OutputPath );

	//
	// For every file we need to compile, we must run it through VC6 
	// (preprocess only) and then through CodeWarrior.
	//

	for( i = 0; i < C.m_SourceCode.GetCount(); i++ )
	{   

		char SrcDrive[ _MAX_DRIVE ];
		char SrcDir[ _MAX_DIR ];
		char SrcFName[ _MAX_FNAME ];
		char SrcExt[ _MAX_EXT ];

		_splitpath( C.m_SourceCode[i], SrcDrive, SrcDir, SrcFName, SrcExt );

		printf( "%s%s\n",SrcFName, SrcExt);
		fflush( stdout );

		//---------------//
		//  CodeWarrior  //
		//---------------//
		sprintf( App, "%s\\%s", 
					g_CmdLineData.m_app_paths[APP_COMPILER],
					g_CmdLineData.m_applications[APP_COMPILER]);

		sprintf( AppArgv, "\"%s\\%s\"", 
					g_CmdLineData.m_app_paths[APP_COMPILER],
					g_CmdLineData.m_applications[APP_COMPILER]);

		sprintf( ResponseFile, "@\"%s\"", 
					NewRName);

		sprintf( ObjectFile, "\"%s\\%s.obj\"", 
					C.m_OutputPath,
					SrcFName);

		// Extra verbage?
		if( g_Verbose )
		{
			DumpCommandLineAndResponseFile( "CodeWarrior Compile", App, NewRName );
		}

		hSTDOUTSave = GetStdHandle( STD_OUTPUT_HANDLE);

		secAttr.nLength = sizeof secAttr;
		secAttr.lpSecurityDescriptor = NULL;
		secAttr.bInheritHandle = true;

		hSTDOUTNew = CreateFile(	OutFile,
											GENERIC_WRITE,
											FILE_SHARE_WRITE,
											&secAttr,
											CREATE_ALWAYS,
											FILE_ATTRIBUTE_NORMAL,
											NULL);

		SetStdHandle( STD_OUTPUT_HANDLE, hSTDOUTNew);

		// Execute command for CodeWarrior compiler. don't specify output file if creating a pre-compiled header file
		if (C.m_CreatePrecompiled)
			Result |= spawnl( _P_WAIT, App, AppArgv, ResponseFile, C.m_SourceCode[i], NULL);
		else
			Result |= spawnl( _P_WAIT, App, AppArgv, ResponseFile, C.m_SourceCode[i], "-o", ObjectFile, NULL);

		SetStdHandle( STD_OUTPUT_HANDLE, hSTDOUTSave);

		CloseHandle( hSTDOUTNew);

		/* here's a hack. the file that creates the pre-compiled header is still passed to the linker as */
		/* needing a .obj file. but we don't have one for it. so we create a small but valid .obj file */
		/* for it. the contents were created by compiling an empty .cpp file */

		if ((C.m_CreatePrecompiled) && ((C.m_Target == TARGET_GAMECUBE) || (C.m_Target == TARGET_PS2)))
		{
#define PS2_EMPTY_OBJ_SIZE 376
			unsigned char PS2EmptyObjData[PS2_EMPTY_OBJ_SIZE] = {
				0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01, 0x01, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x01, 0x00, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0xb0, 0x00, 0x00, 0x00, 0x01, 0x40, 0x92, 0x20,  0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00,
				0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x2e, 0x73, 0x79, 0x6d, 0x74, 0x61, 0x62,  0x00, 0x2e, 0x73, 0x74, 0x72, 0x74, 0x61, 0x62,
				0x00, 0x2e, 0x73, 0x68, 0x73, 0x74, 0x72, 0x74,  0x61, 0x62, 0x00, 0x2e, 0x63, 0x6f, 0x6d, 0x6d,
				0x65, 0x6e, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x4d, 0x57, 0x20, 0x4d, 0x49, 0x50, 0x53, 0x20,  0x43, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c,
				0x65, 0x72, 0x20, 0x28, 0x32, 0x2e, 0x34, 0x2e,  0x31, 0x2e, 0x30, 0x31, 0x29, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x40, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
				0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
				0x09, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x50, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x11, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x60, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x1b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x90, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
			};

#define GCN_EMPTY_OBJ_SIZE 392
			unsigned char GCNEmptyObjData[GCN_EMPTY_OBJ_SIZE] = {
				0x7F, 0x45, 0x4C, 0x46, 0x01, 0x02, 0x01, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x01, 0x00, 0x14, 0x00, 0x00, 0x00, 0x01,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0xC0, 0x80, 0x00, 0x00, 0x00,  0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 
				0x00, 0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x04, 0x00, 0xFF, 0xF1, 0x00, 0x61, 0x73, 0x73,  0x65, 0x72, 0x74, 0x2E, 0x63, 0x70, 0x70, 0x00, 
				0x00, 0x2E, 0x73, 0x79, 0x6D, 0x74, 0x61, 0x62,  0x00, 0x2E, 0x73, 0x74, 0x72, 0x74, 0x61, 0x62, 
				0x00, 0x2E, 0x73, 0x68, 0x73, 0x74, 0x72, 0x74,  0x61, 0x62, 0x00, 0x2E, 0x63, 0x6F, 0x6D, 0x6D, 
				0x65, 0x6E, 0x74, 0x00, 0x43, 0x6F, 0x64, 0x65,  0x57, 0x61, 0x72, 0x72, 0x69, 0x6F, 0x72, 0x0A, 
				0x02, 0x04, 0x02, 0x01, 0x01, 0x02, 0x00, 0x16,  0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x20, 
				0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02,  0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x10, 
				0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x03,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x0C,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,  0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x03, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x24, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 
				0x00, 0x00, 0x00, 0x1B, 0x00, 0x00, 0x00, 0x01,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x3C,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01
			};

			char EmptyObjFile[_MAX_PATH];
			char EmptyObjDir[_MAX_DIR];
			sprintf(EmptyObjDir, "%s%s", SrcDir, C.m_OutputPath);	// add compile target directory to source file dir
			_makepath(EmptyObjFile, SrcDrive, EmptyObjDir, SrcFName, ".obj"); 
//			printf("Pre-compiled header: creating empty .obj file %s\n", EmptyObjFile);
			FILE *pFile = fopen(EmptyObjFile, "wb");
			if (pFile)
			{
				switch(C.m_Target)
				{
				case TARGET_GAMECUBE:
					fwrite(GCNEmptyObjData, GCN_EMPTY_OBJ_SIZE, 1, pFile);
					break;
				case TARGET_PS2:
					fwrite(PS2EmptyObjData, PS2_EMPTY_OBJ_SIZE, 1, pFile);
					break;
				default:
					printf("Pre-compiled header: unknown target creating empty .obj file %s\n", EmptyObjFile);
					break;
				}
				fclose(pFile);
			}
			else
				printf("Pre-compiled header: error creating empty .obj file %s\n", EmptyObjFile);
		}

		//------------------------------------------
		// Now take care of the errors and warnings.
		//------------------------------------------
		ProcessMessages( OutFile );
	}

	//--------------------------------------------------------------------------
	//  We're done!
	//--------------------------------------------------------------------------
	return( Result );
}

//==============================================================================

static
int Compile_s_File( int i )
{
    char    FName   [ _MAX_FNAME ];
    char    Ext     [ _MAX_EXT   ];
    char    CmdLine [ 32784 ];
    int     Result = EXIT_SUCCESS;

    // Pick the name apart.
    _splitpath( C.m_SourceOther[i], NULL, NULL, FName, Ext );

	// change to use the assembler defined in the applications
	// Need to add assembler to config file
    // Build the command line.
    sprintf( CmdLine, "%s -gnu -o \"%s\\%s.obj\" \"%s\"", 
					  C.m_applications[APP_ASSEMBLER],
                      C.m_OutputPath,
                      FName,   
                      C.m_SourceOther[i] );

	/*  TAKEN OUT 10/20/00 since it caused exception with file open operation
    // Verbose?
	if( g_Verbose )
    {
	    DumpCommandLineAndResponseFile( "PS2/CW Assemble",
                                        CmdLine,
                                        NULL );
    }
	*/

    // Feedback for the user.
    printf( "%s%s\n", FName, Ext );
    fflush( stdout );

    // Execute command for CW assembler.
    Result = system( CmdLine );
    return( Result );
}

//==============================================================================

static
int Compile_vu_File( int i )
{
    char    Drive   [ _MAX_DRIVE ];
    char    Dir     [ _MAX_DIR   ];
    char    FName   [ _MAX_FNAME ];
    char    Ext     [ _MAX_EXT   ];
    char    CmdLine [ 32784 ];
    char    Buffer  [ 32784 ];
    int     Result = EXIT_SUCCESS;
    int     j;

    //
    // When we proces .vu files, we will first send them through the Visual 
    // Studio C preprocessor.  Then we send the resulting file through the 
    // standard VU assembler.
    // 

    _splitpath( C.m_SourceOther[i], Drive, Dir, FName, Ext );

/*    //
    // First pass:  C preprocess.
    //
    {
        // Start building the command line.
        // Add standard options:
        //   /TC = Ignore input file extension.  Treat it like C code.
        //   /P  = Write preprocessor output to a file.
        // Add current directory as an include directory.
        sprintf( CmdLine, "_cl /nologo /TC /P -I ." );

        // Add the include paths.
        for( j = 0; j < C.m_IncludeDir.GetCount(); j++ )
        {
            sprintf( Buffer, " -I \"%s\"", C.m_IncludeDir[j] );
            strcat ( CmdLine, Buffer );
        }

        // Add the file name.
        sprintf( Buffer, " \"%s\"", C.m_SourceOther[i] );
        strcat ( CmdLine, Buffer );

       // Verbose?
        if( g_Verbose )
        {
	        DumpCommandLineAndResponseFile( "PS2/CW VU Preprocess",
                                            CmdLine,
                                            NULL );
        }
		
        // Execute command for preprocess.
        Result = system( CmdLine );
    }

    // Move the .i file to the build output directory.
    sprintf( CmdLine, "move \"%s%s%s.i\" \"%s\" ",
                      Drive, Dir, FName,
                      C.m_OutputPath );

    // Do the move.
    Result = system( CmdLine );
*/
    //
    // Second pass:  VU assemble.
    //
    {
		// change to use the compiler defined in the applications

        // Start building the command line.
        // Add standard options:
        // Add current directory as an include directory.
        //sprintf( CmdLine, "ee-dvp-as.exe -g -I." );

		sprintf( CmdLine, "%s -g -I.", g_CmdLineData.m_applications[APP_VUASSEMBLER] );

        // Add the include paths.
        for( j = 0; j < C.m_IncludeDir.GetCount(); j++ )
        {
            sprintf( Buffer, " -I\"%s\"", C.m_IncludeDir[j] );
            strcat ( CmdLine, Buffer );
        }

        // Add the file names and redirection.
		// Commented out since no preprocessing
		//        sprintf( Buffer, " -o \"%s/%s.obj\" \"%s/%s.i\" > \"%s/%s.lst\" ",
		sprintf( Buffer, " -o \"%s/%s.obj\" \"%s.dsm\" > \"%s/%s.lst\" ",
				   		C.m_OutputPath, FName,
                          FName,
                          C.m_OutputPath, FName );
        strcat ( CmdLine, Buffer );

 /*
		// Verbose?
        if( g_Verbose )
        {
	        DumpCommandLineAndResponseFile( "PS2/CW VU Assemble",
                                            CmdLine,
                                            NULL );
        }
*/
		// print out file name to indicate progress
		printf("%s.dsm\n", FName);
		fflush( stdout );

        // Execute command for CW/VU assembler.
        Result = system( CmdLine );
    }

    // That's it.
    return( Result );
}


//==============================================================================

static 
int CompileSourceOther( void )
{
    char    Ext[ _MAX_EXT ];
    int     Result = EXIT_SUCCESS;
    int     i;

    for( i = 0; i < C.m_SourceOther.GetCount(); i++ )
    {
        _splitpath( C.m_SourceOther[i], NULL, NULL, NULL, Ext );

        if( stricmp( ".s", Ext ) == 0 )
        {
            Result |= Compile_s_File( i );
        }

        else
        if( stricmp( ".vu", Ext ) == 0 )
        {
            Result |= Compile_vu_File( i );
        }
			else
			if( stricmp( ".dsm", Ext ) == 0 )
			{
			    Result |= Compile_vu_File( i );
			}
        else
        {
            printf( ".\n" );
            printf( "xCL -- ERROR: Do not know how to process file: %s\n", C.m_SourceOther[i] );
            printf( ".\n" );
            fflush( stdout );
        }
    }

    return( Result );
}

//==============================================================================

static 
int Compiler( void )
{
    int Result = EXIT_SUCCESS;

    if( C.m_SourceCode. GetCount() > 0 )  Result |= CompileSourceCode();
    if( C.m_SourceOther.GetCount() > 0 )  Result |= CompileSourceOther();

    return( Result );
}

//==============================================================================

static 
int Linker( void )
{
	
	int		i;
	int		Result;

	char		Map[_MAX_PATH];
	char		Name[_MAX_PATH];
	char		Drive[_MAX_DRIVE];
	char		Dir[_MAX_DIR];
	char		FName[_MAX_FNAME];
	char		Ext[_MAX_EXT];
	char		CmdLine[32784];
	char		App[_MAX_PATH];
	char		AppArgv[_MAX_PATH];
	char		ResponseFile[_MAX_PATH];

	FILE		*pLRF = NULL;

	// Determine response file name and map name.
	_splitpath( C.m_OutputFile, Drive, Dir, FName, Ext);
	_makepath( Name, Drive, Dir, "_CW_Link", ".txt");
	_makepath( Map, Drive, Dir, FName, ".map.txt" );

	// Build the command line.
	// change to use the compiler defined in the applications
	sprintf( CmdLine, "%s @\"%s\"", g_CmdLineData.m_applications[APP_LINKER], Name );

	// Open the response file.
	pLRF = fopen( Name, "wt" );
	if( pLRF == NULL )
	{
		printf( "xCL -- ERROR: Unable to open file for write: %s\n", Name );
		return( 0 );
	}

	//
	// Write stuff to the response file.
	//

	// Output.
	fprintf( pLRF, " -o %s\n", C.m_OutputFile );

	// Add any custom link options.
	// Add compile options to the response file.
	for( i = 0; i < C.m_LinkOptions.GetCount(); i++ )
	{
		fprintf( pLRF, " %s \n", C.m_LinkOptions[i] );
	}

	// Generic options.
	if( C.m_Symbols)
	{
		fprintf( pLRF, " -g           \n" );       // Add symbols
	}
	else
	{
		fprintf( pLRF, " -sym off     \n" );       // Delete symbols
	}

	// Add PS2 specific standard compile options.
	if( C.m_Target == TARGET_PS2 )
	{
		fprintf( pLRF, " -stderr\n" );
		fprintf( pLRF, " -w off\n" );
		fprintf( pLRF, " -Cpp_exceptions off\n" );
		fprintf( pLRF, " -m ENTRYPOINT\n" );
		fprintf( pLRF, " -map\n" );
	}

	// Add GCN specific standard compile options.
	if( C.m_Target == TARGET_GAMECUBE )
	{
		fprintf( pLRF, " -nostderr    \n" );
		fprintf( pLRF, " -nostdlib    \n" );
		fprintf( pLRF, " -nowraplines \n" );
		fprintf( pLRF, " -msgstyle IDE\n" );
		fprintf( pLRF, " -application \n" );
		fprintf( pLRF, " -fp fmadd\n" );
	}

	// Add project specified system library paths.
	for( i = 0; i < C.m_LibraryDir.GetCount(); i++ )                       
		fprintf( pLRF, " -L\"%s\"\n", C.m_LibraryDir[i] );

	// Add specified objects.
	// Moved the Object to be linked in before any libraries - Rafi 2/10/2001 
	for( i = 0; i < C.m_ObjectFile.GetCount(); i++ )                       
		fprintf( pLRF, " %s\n", C.m_ObjectFile[i] );

	// Add each of the library files.
	for( i = 0; i < C.m_LibraryFile.GetCount(); i++ )
	{   
		char Path[ 8192 ];

		// Does the library name include a path?
		_splitpath( C.m_LibraryFile[i], NULL, Path, NULL, NULL );
		if( *Path == '\0' )
		{
			// There is no path.  Use "-l" to search library paths.
			fprintf( pLRF, " -l\"%s\"\n", C.m_LibraryFile[i] );
		}
		else
		{
			fprintf( pLRF, " \"%s\"\n", C.m_LibraryFile[i] );
		}
	}


	//
	// Done with link response file.
	//
	fclose( pLRF );

	// Extra verbage?
	if( g_Verbose )
	{
		DumpCommandLineAndResponseFile( "CodeWarrior Link", CmdLine, Name );
	}

	// A little more standard verbage...
	printf( "%s%s\n", FName, Ext );
	fflush( stdout );

	sprintf( App, "%s\\%s", 
				g_CmdLineData.m_app_paths[APP_LINKER],
				g_CmdLineData.m_applications[APP_LINKER]);

	sprintf( AppArgv, "\"%s\\%s\"", 
				g_CmdLineData.m_app_paths[APP_LINKER],
				g_CmdLineData.m_applications[APP_LINKER]);

	sprintf( ResponseFile, "@\"%s\"", 
				Name);

	// Execute command for CodeWarrior librarian
	Result = spawnl( _P_WAIT, App, AppArgv, ResponseFile, NULL);

	// We're done!
	return( Result );
}

//==============================================================================

static 
int Librarian( void )
{

	int		i;
	int		Result;
	
	char		Name[_MAX_PATH];
	char		Drive[_MAX_DRIVE];
	char		Dir[ _MAX_DIR];
	char		CmdLine[32784];
	char		App[_MAX_PATH];
	char		AppArgv[_MAX_PATH];
	char		ResponseFile[_MAX_PATH];
	
	FILE		*pFile = NULL;

	// Determine response file name.
	_splitpath( C.m_OutputFile, Drive, Dir, NULL, NULL);
	_makepath( Name, Drive, Dir, "_CW_Lib", ".txt");

	// Open the response file.
	pFile = fopen( Name, "wt" );
	if( pFile == NULL )
	{
		printf( "xCL -- ERROR: Unable to open file for write: %s\n", Name );
		return( 0 );
	}

	// Build the command line.
	// use the compiler defined in the applications
	sprintf(CmdLine, "%s @%s", g_CmdLineData.m_applications[APP_LINKER], Name);

	//
	// Write stuff to the response file.
	//

	//
	// Other options.
	//

	fprintf( pFile, " -library      \n" );
	fprintf( pFile, " -nostderr     \n" );
	fprintf( pFile, " -nostdlib     \n" );
	fprintf( pFile, " -nowraplines  \n" );
	fprintf( pFile, " -msgstyle IDE \n" );
	fprintf( pFile, " -g           \n" );       // Add symbols

	// Add GCN specific standard compile options.
	if( C.m_Target == TARGET_GAMECUBE )
	{
		fprintf( pFile, " -fp fmadd\n" );
	}

	// Add each of the object files.
	for( i = 0; i < C.m_ObjectFile.GetCount(); i++ )
	{
		fprintf( pFile, " %s\n", C.m_ObjectFile[i] );
	}

#if 0
	// Add each of the library files.
	for( i = 0; i < C.m_LibraryFile.GetCount(); i++ )
	{
		fprintf( pFile, " %s\n", C.m_LibraryFile[i] );
	}
#endif

	// Output.
	fprintf( pFile, " -o %s\n", C.m_OutputFile );

	// We're done with the response file.
	fclose( pFile );

	// Extra verbage?
	if( g_Verbose )
	{
		DumpCommandLineAndResponseFile( "CodeWarrior Librarian", CmdLine, Name);
	}

	sprintf( App, "%s\\%s", 
				g_CmdLineData.m_app_paths[APP_LINKER],
				g_CmdLineData.m_applications[APP_LINKER]);

	sprintf( AppArgv, "\"%s\\%s\"", 
				g_CmdLineData.m_app_paths[APP_LINKER],
				g_CmdLineData.m_applications[APP_LINKER]);

	sprintf( ResponseFile, "@\"%s\"", 
				Name);

	// Execute command for CodeWarrior librarian
	Result = spawnl( _P_WAIT, App, AppArgv, ResponseFile, NULL);

	// We're done!
	return( Result );
}

//==============================================================================

int Process_CodeWarrior( void )
{

	int Result = EXIT_FAILURE;

	// Warn user about any unknown parameters.
	{   

		int i;

		for( i = 0; i < C.m_UnknownOption.GetCount(); i++ )
		{
			printf( "xCL -- Warning: Option not processed: %s\n", C.m_UnknownOption[i] );
		}
	}

	// Based on the desired tool, call a function to deal with it.
	switch( C.m_Tool )
	{
		case TOOL_COMPILER:
			Result = Compiler( );
			break;

		case TOOL_LINKER:
			Result = Linker( );
			break;

		case TOOL_LIBRARIAN:
			Result = Librarian( );
			break;

		default:                
			printf( "xCL -- ERROR: Unable to determine tool for CodeWarrior operation.\n" );
			break;
	}

	// We're done.
	return( Result );
}
