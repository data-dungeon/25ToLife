//==============================================================================
//
//  CmdLineData.cpp
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "xCL.hpp"
#include "CmdLineData.hpp"
#include "CmdLnTokenizer.hpp"

//==============================================================================
//  VARIABLES
//==============================================================================

static cmd_line_tokenizer	T;
static cmd_line_data			&C = g_CmdLineData;

static const char *pToolName[]   = { "<unknown>", "Compiler", "Linker", "Librarian" };
static const char *pTargetName[] = { "<unknown>", "PC", "PS2", "X-Box", "GameCube" };
static const char *pVendorName[] = { "<unknown>", "Microsoft", "SN Systems", "Metrowerks" };
static const char *pSubSysName[] = { "<default>", "Windows", "WinCon" };

const char *pApplicationNames[] = 
{ 
	"Compiler", 
	"Linker", 
	"Debug", 
	"ComUtil", 
	"Assembler", 
	"VUAssembler" 
};

//==============================================================================
//  TYPES
//==============================================================================

typedef void (*option_handler_fnptr)( void );

typedef struct
{
	char							*Option;
	option_handler_fnptr		Handler;
} option_handler_entry;

//==============================================================================
//  FUNCTIONS
//==============================================================================

//==============================================================================
//  Option Handler Functions
//------------------------------------------------------------------------------
//
//  The following functions are used to process individual options encountered
//  in the command line.  All of the functions are expected to use the local
//  cmd_line_data& C.
//
//==============================================================================

void IgnoreOption0( void )
{

}

//==============================================================================

void IgnoreOption1( void )
{
	T.NextToken( );
}

//==============================================================================

void RecordDefine( void )
{

	C.m_Define.Add( T.NextToken( ) );

	// Check for target definition string.
	if( strncmp( T.CurrentToken( ), "WIN32", 5 ) == 0 )
	{
		strcpy( C.m_TargetString, T.CurrentToken( ));
		C.m_Target = TARGET_PC;
	}
	else
	if( strncmp( T.CurrentToken( ), "PS2", 3) == 0 )
	{
		strcpy( C.m_TargetString, T.CurrentToken( ));
		C.m_Target = TARGET_PS2;
	}
	else
	if( strncmp( T.CurrentToken( ), "XBOX", 4) == 0 )
	{
		strcpy( C.m_TargetString, T.CurrentToken( ));
		C.m_Target = TARGET_XBOX;
	}
	else
	if( strncmp( T.CurrentToken( ), "GCN", 3 ) == 0 )
	{
		strcpy( C.m_TargetString, T.CurrentToken( ));
		C.m_Target = TARGET_GAMECUBE;
	}
                                                              
	// Check for vendor definition string.
	if( strncmp( T.CurrentToken( ), "WIN32", 5 ) == 0 )
	{
		strcpy( C.m_VendorString, T.CurrentToken( ));
		C.m_Vendor = VENDOR_MICROSOFT;
	}
	else
	if( strncmp( T.CurrentToken( ), "XBOX", 5 ) == 0 )
	{
		strcpy( C.m_VendorString, T.CurrentToken( ));
		C.m_Vendor = VENDOR_MICROSOFT;
	}
	else
	if( strncmp( T.CurrentToken( ), "CW", 2) == 0 )
	{
		strcpy( C.m_VendorString, T.CurrentToken( ));
		C.m_Vendor = VENDOR_METROWERKS;
	}
	else
	if( strncmp( T.CurrentToken( ), "PS2_TARGET_SN", 4) == 0 )
	{
		strcpy( C.m_VendorString, T.CurrentToken( ));
		C.m_Vendor = VENDOR_SN;
	}
	else
	if( strncmp( T.CurrentToken( ), "GCN_TARGET_SN", 4) == 0 )
	{
		strcpy( C.m_VendorString, T.CurrentToken( ));
		C.m_Vendor = VENDOR_SN;
	}
}

//==============================================================================

void RecordIncludePath( void )
{
	C.m_IncludeDir.Add( T.NextToken( ) );
}

//==============================================================================

void RecordLibraryPath( void )
{
	C.m_LibraryDir.Add( &T.CurrentToken( )[9] );
}

//==============================================================================

void RecordOptimization( void )
{
	switch( T.CurrentToken( )[2] )
	{
		case 'd':   
		case '1':   
		case 's':   
		case '2':   
		case 't':   
		case 'x':   
			C.m_Optimization = T.CurrentToken( )[2];
			break;
		default:
			break;
	}
}

//==============================================================================

void RecordCompileOnly( void )
{
	C.m_CompileOnly = true;
}

//==============================================================================

void RecordSymbolsMode( void )
{
	C.m_Symbols = true;
}

//==============================================================================

void RecordPreprocess( void )
{
	C.m_Preprocess = true;
}                 

//==============================================================================

void RecordForceC( void )
{
	C.m_ForceC = true;
}                 

//==============================================================================

void RecordForceCPP( void )
{
	C.m_ForceCPP = true;
}                 

//==============================================================================

void RecordNoStdInc( void )
{
	C.m_StdInc = false;
}                 

//==============================================================================

void RecordCreatePrecompiled( void )
{
	C.m_CreatePrecompiled = true;
}                 

//==============================================================================

void RecordUsePrecompiled( void )
{
	C.m_UsePrecompiled = true;
}                 

//==============================================================================

void RecordLibraryMode( void )
{
	C.m_Tool = TOOL_LIBRARIAN;
}

//==============================================================================

void RecordLibraryPS2CW( void)
{
	/* "/PS2CWLINK" option identifies this as codewarrior for ps2 */

	strcpy( C.m_TargetString, "PS2");
	C.m_Target = TARGET_PS2;
	strcpy( C.m_VendorString, "CW");
	C.m_Vendor = VENDOR_METROWERKS;
}

//==============================================================================

void RecordPrecompiled( void )
{
	// The PCH may or may not be present.  And it may or may not be 
	// enclosed in quotes.

	if( T.CurrentToken( )[3] == '\0' )
		return;

	if( T.CurrentToken( )[3] == '"' )
	{
		strcpy( C.m_PCHFile, &T.CurrentToken( )[4] );
		
		int i = strlen( C.m_PCHFile) - 1;
		
		if( C.m_PCHFile[i] == '"' )
			C.m_PCHFile[i] = '\0';
	}
	else
	{
		strcpy( C.m_PCHFile, &T.CurrentToken( )[3] );
	}
}

//==============================================================================

void RecordLinkDefine( void )
{

	char		*pStr;

	pStr = T.CurrentToken( );
	//pStr now points to /include:"<token>"

	pStr += 9;
	//pStr now points to "<token>"
	
	//<token> might not be enclosed in quotes,
	// so make sure one is there and skip it
	if( *pStr == '"')
	{
		pStr++;
	}

	// Check for debuglibs definition string.
	if( strncmp( pStr, "DEBUGLIBS", 9 ) == 0 )
	{
		C.m_DebugLibs = true;
	}

	// Check for target definition string.
	if( strncmp( pStr, "WIN32", 5 ) == 0 )
	{
		strcpy( C.m_TargetString, pStr);
		C.m_Target = TARGET_PC;
	}
	else
	if( strncmp( pStr, "PS2", 3) == 0 )
	{
		strcpy( C.m_TargetString, pStr);
		C.m_Target = TARGET_PS2;
	}
	else
	if( strncmp( pStr, "XBOX", 4) == 0 )
	{
		strcpy( C.m_TargetString, pStr);
		C.m_Target = TARGET_XBOX;
	}
	else
	if( strncmp( pStr, "GCN", 3 ) == 0 )
	{
		strcpy( C.m_TargetString, pStr);
		C.m_Target = TARGET_GAMECUBE;
	}
                                                              
	// Check for vendor definition string.
	if( strncmp( pStr, "WIN32", 5 ) == 0 )
	{
		strcpy( C.m_VendorString, pStr);
		C.m_Vendor = VENDOR_MICROSOFT;
	}
	else
	if( strncmp( pStr, "XBOX", 5 ) == 0 )
	{
		strcpy( C.m_VendorString, pStr);
		C.m_Vendor = VENDOR_MICROSOFT;
	}
	else
	if( strncmp( pStr, "CW", 2) == 0 )
	{
		strcpy( C.m_VendorString, pStr);
		C.m_Vendor = VENDOR_METROWERKS;
	}
	else
	if( strncmp( pStr, "PS2_TARGET_SN", 4) == 0 )
	{
		strcpy( C.m_VendorString, pStr);
		C.m_Vendor = VENDOR_SN;
	}
	else
	if( strncmp( pStr, "GCN_TARGET_SN", 4) == 0 )
	{
		strcpy( C.m_VendorString, pStr);
		C.m_Vendor = VENDOR_SN;
	}
}

//==============================================================================

void RecordLinkOptions( void )
{
}

//==============================================================================

void RecordSyntaxOnly( void )
{
	/* we are getting a /Zs coming through as an argument, which means check syntax only, but i don't know why. ignore it */
}

//==============================================================================

void RecordOutputFile( void )
{
    // Output file specification may or may not be enclosed in quotes.

    if( T.CurrentToken()[5] == '"' )
    {
        strcpy( C.m_OutputFile, &T.CurrentToken()[6] );
        int i = strlen(C.m_OutputFile) - 1;
        if( C.m_OutputFile[ i ] == '"' )
            C.m_OutputFile[ i ] = '\0';
    }
    else
    {
        strcpy( C.m_OutputFile, &T.CurrentToken()[5] );
    }
}

//==============================================================================

void RecordOutputPath( void )
{
    // Output path specification may or may not be enclosed in quotes.

    if( T.CurrentToken()[3] == '"' )
    {
        strcpy( C.m_OutputPath, &T.CurrentToken()[4] );
        int i = strlen(C.m_OutputPath) - 1;
        if( C.m_OutputPath[ i ] == '"' )
            C.m_OutputPath[ i ] = '\0';
    }
    else
    {
        strcpy( C.m_OutputPath, &T.CurrentToken()[3] );
    }

    // Snip off trailing '/'.
    {
        int Len = strlen( C.m_OutputPath );
        if( C.m_OutputPath[Len-1] == '/' )
            C.m_OutputPath[Len-1] =  '\0';
    }
}

//==============================================================================

void RecordSBRPath( void )
{
    // SBR path specification may or may not be enclosed in quotes.

    if( T.CurrentToken()[3] == '"' )
    {
        strcpy( C.m_SBRPath, &T.CurrentToken()[4] );
        int i = strlen(C.m_SBRPath) - 1;
        if( C.m_SBRPath[ i ] == '"' )
            C.m_SBRPath[ i ] = '\0';
    }
    else
    {
        strcpy( C.m_SBRPath, &T.CurrentToken()[3] );
    }

    // Snip off trailing '/'.
    {
        int Len = strlen( C.m_SBRPath );
        if( C.m_SBRPath[Len-1] == '/' )
            C.m_SBRPath[Len-1] =  '\0';
    }
}

//==============================================================================

void RecordPDBPath( void )
{
    // PDB path specification may or may not be enclosed in quotes.

    if( T.CurrentToken()[3] == '"' )
    {
        strcpy( C.m_PDBPath, &T.CurrentToken()[4] );
        int i = strlen(C.m_PDBPath) - 1;
        if( C.m_PDBPath[ i ] == '"' )
            C.m_PDBPath[ i ] = '\0';
    }
    else
    {
        strcpy( C.m_PDBPath, &T.CurrentToken()[3] );
    }

    // Snip off trailing '/'.
    {
        int Len = strlen( C.m_PDBPath );
        if( C.m_PDBPath[Len-1] == '/' )
            C.m_PDBPath[Len-1] =  '\0';
    }
}

//==============================================================================

void RecordEnableRTTI( void )
{

	char *p = &( T.CurrentToken()[1]);

	if( stricmp( p, "GR-" ) == 0 ) 
	{
		C.m_RTTI = false;
	}
	else
	{
		C.m_RTTI = true;
	}
}

//==============================================================================
//  Option Handler Table
//------------------------------------------------------------------------------
//  
//  This table associates the Option Handler Functions with strings that signal
//  the option.
//  
//  NOTE:  The order of the strings in the list is relevant!  Longer strings
//  should preceed shorter strings to avoid errors.  For example, "Ob" must
//  occur before "O".
//  
//==============================================================================

static option_handler_entry  OptionHandlerList[] = {

    // --- Compiler ('C') or Linker ('L') -----.
    //                                         |
	{ "incrementa",	IgnoreOption0      		},  // L:enable incremental link
	{ "subsystem:",	IgnoreOption0      		},  // L:target subsystem
	{ "PS2CWLINK",		RecordLibraryPS2CW  		},  // L:vendor and target
	{ "libpath:",		RecordLibraryPath   		},  // L:
	{ "pdbtype:",		IgnoreOption0      		},  // L:PDB file type
	{ "include:",		RecordLinkDefine    		},  // L:Used for setting target and vendor on link
	{ "machine:",		IgnoreOption0      		},  // L:target CPU
	{ "nologo",			IgnoreOption0      		},  // C:disable compiler banner
	{ "entry:",			IgnoreOption0      		},  // L:entry point
	{ "debug",			RecordSymbolsMode   		},  // L:
	{ "base:",			IgnoreOption0      		},  // L:base address
	{ "map:",			IgnoreOption0      		},  // L:map file
	{ "out:",			RecordOutputFile    		},  // L:
	{ "pdb:",			IgnoreOption0      		},  // L:PDB file
	{ "lib",				RecordLibraryMode   		},  // L:
	{ "Ob0",				IgnoreOption0      		},  // C:in-line function expansion
	{ "Ob1",				IgnoreOption0      		},  // C:in-line function expansion
	{ "Ob2",				IgnoreOption0      		},  // C:in-line function expansion
	{ "EP",				IgnoreOption0      		},  // C:preprocess to stdout
	{ "FD",				IgnoreOption0      		},  // C:generate file dependancies
	{ "Fp",				RecordPrecompiled    	},  // C:PCH file
	{ "Fr",				IgnoreOption1      		},  // C:
	{ "Fo",				RecordOutputPath    		},  // C:
	{ "Fd",				RecordPDBPath       		},  // C:
	{ "FR",				RecordSBRPath       		},  // C:SBR file
	{ "FI",				IgnoreOption0      		},  // C:force include
	{ "G3",				IgnoreOption0      		},  // C:code generation, target '386
	{ "G4",				IgnoreOption0      		},  // C:code generation, target '486
	{ "G5",				IgnoreOption0      		},  // C:code generation, target Pentium
	{ "G6",				IgnoreOption0      		},  // C:code generation, target Pentium Pro
	{ "G7",				IgnoreOption0      		},  // C:code generation, target Pentium II
	{ "Gm",				IgnoreOption0      		},  // C:enable minimal rebuild
	{ "GX",				IgnoreOption0      		},  // C:enable exception handling
	{ "GZ",				IgnoreOption0      		},  // C:enable runtime error checks
	{ "ML",				IgnoreOption0      		},  // C:link with LIBC.LIB
	{ "GR",				RecordEnableRTTI    		},  // C:enable RTTI
	{ "MT",				IgnoreOption0      		},  // C:multi-threaded
	{ "TC",				RecordForceC        		},  // C:force C   compilation
	{ "TP",				RecordForceCPP      		},  // C:force C++ compilation
	{ "Zi",				RecordSymbolsMode   		},  // C:
	{ "Yc",				RecordCreatePrecompiled	},  // C:create precompiled header file. file specified by /Fp
	{ "Yu",				RecordUsePrecompiled		},  // C:use precompiled header file. file specified by /Fp
	{ "YX",				IgnoreOption0      		},  // C:automatic PCH...not supported
	{ "ZI",				RecordSymbolsMode			},  // C:
	{ "Z7",				RecordSymbolsMode			},  // C:
	{ "Zs",				RecordSyntaxOnly			},  // C:check syntax only-- ignored
	{ "L:",				RecordLinkOptions   		},  // L:Not a standard link option
	{ "c",				RecordCompileOnly   		},  // C:compile only
	{ "C",				IgnoreOption0      		},  // C:preserve comments in pre-processor
	{ "D",				RecordDefine        		},  // C:
	{ "I",				RecordIncludePath   		},  // C:
	{ "l",				IgnoreOption1      		},  // C:locality specifier
	{ "O",				RecordOptimization  		},  // C:
	{ "P",				RecordPreprocess    		},  // C:Preprocess to file
	{ "W",				IgnoreOption0      		},  // C:set warning level
	{ "w",				IgnoreOption0      		},  // C:set warning level
	{ "X",				RecordNoStdInc      		},  // C:ignore standard include directory
};

//==============================================================================
//  Member functions for cmd_line_data.
//==============================================================================

void cmd_line_data::ProcessOption( char* pToken )
{

	int i;
	int ListLength  = sizeof OptionHandlerList / sizeof(option_handler_entry);

	assert( (pToken[0] == '/') || (pToken[0] == '-') );

	// Search the list of options for a handler.
	for( i = 0; i < ListLength; i++ )
	{
		/* do a partial strcmp so that we can put O in the command list above and still catch /Od option. */
		/* however, this is somewhat dangerous, since if we have P in list and pass /PS2LINK option, it */
		/* will think that it found the correct option when in fact it didn't. the list is sorted longest */
		/* to shortest, so if PS2LINK is in the list, it will correctly find it first, but if a command */
		/* is missing, there is a possibility of incorrectly handling it */

		if (strncmp(pToken + 1, OptionHandlerList[i].Option, strlen(OptionHandlerList[i].Option)) == 0)
		{
			/* possible warning */

			if ((g_Verbose) && (strlen(pToken + 1) > strlen(OptionHandlerList[i].Option)))
				printf("Possible command misinterpretation? %s found as command %s\n", pToken, OptionHandlerList[i].Option);
			OptionHandlerList[i].Handler( );
			return;
		}
	}

	// Record the option as an unknown option.
	m_UnknownOption.Add( pToken );
}

///////////////////////////////////////////////////////////////////////////

void cmd_line_data::ProcessFile( char *pToken )
{

	char  Ext[_MAX_EXT];

	// Strange thing here.  If the path or filename has a space in it, then
	// the system will surround the path with double quotes.  Before we 
	// break the extension out, we need to see if we have quotes around the
	// file name, and if so, remove them.

	if( pToken[0] == '"' )
	{

		int x;
		
		pToken += 1;
		x = strlen( pToken) - 1;
		pToken[ x ] = '\0';
	}

	_splitpath( pToken, NULL, NULL, NULL, Ext );

	if     ( stricmp( Ext, ".lib" ) == 0 )  m_LibraryFile.  Add( pToken ); 
	else if( stricmp( Ext, ".a"   ) == 0 )  m_LibraryFile.  Add( pToken ); 
	else if( stricmp( Ext, ".obj" ) == 0 )  m_ObjectFile.   Add( pToken ); 
	else if( stricmp( Ext, ".o"   ) == 0 )  m_ObjectFile.   Add( pToken ); 
	else if( stricmp( Ext, ".lcf" ) == 0 )  m_ObjectFile.   Add( pToken ); 
	else if( stricmp( Ext, ".s"   ) == 0 )  m_SourceOther.  Add( pToken ); 
	else if( stricmp( Ext, ".vu"  ) == 0 )  m_SourceOther.  Add( pToken );
	else if( stricmp( Ext, ".dsm" ) == 0 )  m_SourceOther.  Add( pToken );
	else                                    m_SourceCode.   Add( pToken );
}

///////////////////////////////////////////////////////////////////////////

cmd_line_data::cmd_line_data( void )
{
	// Set initial values to member fields.
	m_Tool            	= TOOL_LINKER;
	m_Target          	= TARGET_PC;
	m_Vendor          	= VENDOR_MICROSOFT;
	m_Symbols         	= false;
	m_Preprocess      	= false;
	m_ForceC          	= false;
	m_ForceCPP        	= false;
	m_StdInc          	= true;
	m_CompileOnly			= false;
	m_RTTI					= false;
	m_DebugLibs				= false;
	m_CreatePrecompiled	= false;
	m_UsePrecompiled		= false;
	m_Optimization    	= 't';
	m_TargetString[0] 	= '\0';
	m_VendorString[0] 	= '\0';
	m_OutputFile[0]   	= '\0';
	m_OutputPath[0]   	= '\0';
	m_SBRPath[0]      	= '\0';
	m_PDBPath[0]      	= '\0';
	m_PrefixFile[0]   	= '\0';

	for (int i = 0; i < APP_ARRAY_SIZE; i++)
	{
		m_applications[i] = NULL;
	}
	m_PrefixFile[0]   = '\0';

	m_argc            = 0;
	m_argv            = NULL;
}

///////////////////////////////////////////////////////////////////////////

cmd_line_data::~cmd_line_data()
{
	// free applications paths
	for( int i = 0; i < APP_ARRAY_SIZE; i++)
	{
		if( m_applications[i] != NULL)
		{
			free( m_applications[i]);
		}
	}
}
                                                                        
///////////////////////////////////////////////////////////////////////////

void cmd_line_data::Activate( int argc, char **argv )
{   
    
	char		*pToken;

	// Retain the original command line information.
	m_argc = argc;
	m_argv = argv;                         

	// Activate the tokenizer.
	T.Activate( argc, argv );

	// Display all tokens found.
	if( g_Verbose )
	{
		
		int i = 0;

		OpenSection( "Input Token Stream" );
		
		while( ( pToken = T.NextToken( )) )
		{
			printf( "    [%03d]  %s\n", i++, pToken );
		}
		
		CloseSection();
		T.Reset();
	}

	// Make intial decision regarding the tool.
	{

		char FName[ _MAX_FNAME ];
		char Ext  [ _MAX_EXT   ];
		
		_splitpath( argv[0], NULL, NULL, FName, Ext );
		if ((stricmp( Ext, ".exe" ) == 0 ) || (strlen(Ext) == 0))
		{
			if( stricmp( FName, "cl" ) == 0  )
			{
				m_Tool = TOOL_COMPILER;
			}
			if( stricmp( FName, "link" ) == 0  )
			{
				m_Tool = TOOL_LINKER;
			}
		}
	}

	// Process input tokens.
	while( (pToken = T.NextToken()) )
	{
		if( (pToken[0] == '/') || (pToken[0] == '-') )
		{
			ProcessOption( pToken );
		}
		else
		{
			ProcessFile( pToken );
		}
	}

	// Display argument state.
	if( g_Verbose )
	{
		Dump( );
	}
}

//==============================================================================

void cmd_line_data::Dump( void )
{
	OpenSection( "Processed Command Line State" );

	printf( "Tool...........%s\n", pToolName[m_Tool] );
	printf( "Optimization...%c\n", m_Optimization );
	printf( "Symbols........%s\n", m_Symbols  ? "Yes" : "No" );
	printf( "RTTI...........%s\n", m_RTTI  ? "Yes" : "No" );
	printf( "DebugLibs......%s\n", m_DebugLibs ? "Yes" : "No" );
	printf( "Target.........%s (%s)\n", pTargetName[m_Target], m_TargetString );
	printf( "Vendor.........%s (%s)\n", pVendorName[m_Vendor], m_VendorString );
	printf( "Output path....%s\n", m_OutputPath );
	printf( "Output file....%s\n", m_OutputFile );
	printf( "SBR path.......%s\n", m_SBRPath );
	printf( "PDB path.......%s\n", m_PDBPath );
	printf( "PCH file.......%s\n", m_PCHFile );

	printf( "Miscellaneous.." );
	if( m_Preprocess )  printf( "[Preprocess to file]  "    );
	if( m_ForceC     )  printf( "[Force C compilation]  "   );
	if( m_ForceCPP   )  printf( "[Force C++ compilation]  " );
	printf( "\n" );

	DivideSection();

	m_Define.       Dump( "Defines"             );
	m_IncludeDir.   Dump( "Include path"        );
	m_LibraryDir.   Dump( "Library path"        );
	m_SourceCode.   Dump( "Source list C/C++"   );
	m_SourceOther.  Dump( "Source list Other"   );
	m_ObjectFile.   Dump( "Object list"         );
	m_LibraryFile.  Dump( "Library list"        );
	m_UnknownOption.Dump( "Unknown options"     );

	DivideSection();

	CloseSection();
}

//==============================================================================

const char * cmd_line_data::getTargetName( )
{
	return pTargetName[m_Target];
}

//==============================================================================

void cmd_line_data::setApplication( int iAppName, const char * strPath, const char * strApplication)
{
	// free existing application?
	if( m_applications[iAppName] != NULL)
	{
		free( m_applications[iAppName]);
		free( m_app_paths[iAppName]);
	}

	// duplicate string and store application
	m_applications[iAppName] = strdup( strApplication);
	m_app_paths[iAppName] = strdup( strPath);

	//printf("set application %d <%s> to '%s' '%s'\n", iAppName, pApplicationNames[iAppName], strPath, strApplication);
}

//==============================================================================

void cmd_line_data::setApplication( const char * strAppName, const char * strPath, const char * strApplication)
{
	
	int iAppName = 0;

	// look up the application name from the list
	while( iAppName < APP_END_OF_LIST)
	{
		if( strcmp( strAppName, pApplicationNames[iAppName]) == 0)
		{
			// set that application
			setApplication( iAppName, strPath, strApplication);
			return;
		}

		iAppName++;
	}
}

//==============================================================================
