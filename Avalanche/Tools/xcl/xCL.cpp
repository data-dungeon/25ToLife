//==============================================================================
//
//  xCL - Cross Platform Compile and Link
//
//==============================================================================

//==============================================================================
//  INCLUDES    
//==============================================================================

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crtdbg.h>

#include "xCL.hpp"

#include "xCL_Thru.hpp"
#include "xCL_CodeWarrior.hpp"
#include "xCL_SN.hpp"

#include "config.hpp"

//==============================================================================
//  GLOBAL VARIABLES
//==============================================================================

cmd_line_data   g_CmdLineData;

#ifdef _DEBUG
bool            g_Verbose     = true;
bool            g_EchoCmdLine = true;
#else
bool            g_Verbose     = false;
bool            g_EchoCmdLine = false;
#endif


//==============================================================================
//  PUBLIC FUNCTIONS
//==============================================================================

//==============================================================================
//  OpenSection
//==============================================================================
//  I've been reading the verbose output so much, that I was ready for a little
//  change.  So I jazzed it up a bit.  This should make it easier to read, too.
//  I'm making the "horizontal lines" 75 characters long so they are nice and
//  wide, but they should still fit in a plain text email message.
//
/*

          1    1    2    2    3    3    4    4    5    5    6    6    7    7    8
0....5....0....5....0....5....0....5....0....5....0....5....0....5....0....5....0

   ____
__/    \___________________________________________________________________
===========================================================================

   _____
__/  A  \__________________________________________________________________
===========================================================================

   __________________  
__/  This is a test  \_____________________________________________________
===========================================================================

   ______________________________________________________________________
__/  Command Line and Response file for: VC6 Compile  yadda yadda yadda  \_
===========================================================================

   ________________________________________________________________________  
__/  Command Line and Response file for: VC6 Compile yadda yadda yadda yadda yadda yadda 
===========================================================================

aaaaaaaabbbbbbbbbbbbb
aaaaaTHIS IS A TESTbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
333333333333333333333333333333333333333333333333333333333333333333333333333

*///============================================================================

void OpenSection( char* pTitle )
{

	int  w;
	int  Len = strlen( pTitle );
	
	char L1a[] = "   ____";
	char L1b[] = "____________________________________________________________________";
	char L2a[] = "__/  ";
	char L2b[] = "  \\___________________________________________________________________";
	char L3 [] = "===========================================================================";
            
	w = ( ( Len > 70) ? 70 : Len);
	L1b[   w] = '\0';
	L2b[70 - w] = '\0';

	printf( ".\n" );
	printf( "%s%s\n", L1a, L1b );
	printf( "%s%s%s\n", L2a, pTitle, L2b );
	printf( "%s\n", L3 );
	fflush( stdout );
}

//==============================================================================

void DivideSection( void )
{
	printf( "---------------------------------------------------------------------------\n" );
	fflush( stdout );
}

//==============================================================================

void CloseSection( void )
{
	printf( "===========================================================================\n" );
	printf( ".\n" );
	fflush( stdout );
}

//==============================================================================

void DumpTextFileContent( char *pFileName )
{

	char  Buffer[ 8192 ];
	
	FILE	*pFile = fopen( pFileName, "rt" );

	if( pFile )
	{
		
		bool NL;
		
		while( fgets( Buffer, 8192, pFile ) != NULL )
		{
			NL = ( Buffer[ strlen(Buffer) ] == '\n' );
			printf( "%s", Buffer );
		}

		fclose( pFile );
		if( !NL )
		{
			printf( "\n" );
		}
	}
	else
	{
		printf( "<<< Failed to open file '%s' for read. >>>\n", pFileName );
	}        
}

//==============================================================================

void DumpCommandLineAndResponseFile( char* pTitle,
                                     char* pCmdLine,
                                     char* pResponseFileName )
{
	OpenSection( pTitle );
	printf( "%s\n", pCmdLine );
	DivideSection( );
	DumpTextFileContent( pResponseFileName );
	CloseSection( );
}

//==============================================================================

char * UseOnlyBackSlashes( char *pPath )
{
    
	char* p = pPath;
   
	while( *p )
	{
		if( *p == '/' )
			*p = '\\';
		p++;
	}
	return( pPath );
}

//==============================================================================

char * UseOnlySlashes( char* pPath )
{

	char* p = pPath;
	
	while( *p )
	{
		if( *p == '\\' )
			*p = '/';
		p++;
	}
	return( pPath );
}

//==============================================================================
//  PRIVATE FUNCTIONS
//==============================================================================

static void EchoCommandLine( int argc, char** argv )
{
    
	int i;

	//
	// Echo command line to standard out.
	//

	OpenSection( "Command Line Echo" );
	for( i = 0; i < argc; i++ )
	{
		printf( "[%03d]  %s\n", i, argv[i] );
	}    
	CloseSection();

	// Look for anything that looks like a response file.  Echo it's content.

	for( i = 1; i < argc; i++ )
	{

		char Label[ _MAX_PATH ];

		if( argv[i][0] != '@' )
			continue;

		sprintf( Label, "Response file: %s", argv[i]+1 );
		OpenSection( Label );
		DumpTextFileContent( argv[i]+1 );
		CloseSection();
	}
}

//==============================================================================

static void ReadOptionsFromRegistry( void )
{
}

//==============================================================================

int main( int argc, char** argv )
{   

	int		Result  = EXIT_FAILURE;

	bool		Unknown = false;    
	
	char		*pTarget = NULL;

	// Echo command line?
	if( g_EchoCmdLine )
	{
		printf( "\n.\nxCL -- COMMAND LINE ECHO MODE ACTIVATED\n" );
		EchoCommandLine( argc, argv );
	}

	// Verbose output?
	if( g_Verbose )
	{
		printf( "\n.\n" );
		printf( "***************************************************************************\n" );
		printf( "xCL -- VERBOSE MODE ACTIVATED\n" );
		printf( "***************************************************************************\n" );
	}

	// Absorb the command line into our data structure.
	g_CmdLineData.Activate( argc, argv );

	if( g_CmdLineData.m_Vendor == VENDOR_UNKNOWN )
	{
		printf( "\n.\n" );
		printf( "xCL -- ERROR: Unable to determine compiler vendor\n" );
		printf( "xCL -- ERROR: Processing aborted.\n.\n" );
	}

	if( g_CmdLineData.m_Target == TARGET_UNKNOWN )
	{
		printf( "\n.\n" );
		printf( "xCL -- ERROR: Unable to determine target platform\n" );
		printf( "xCL -- ERROR: Processing aborted.\n.\n" );
	}

	if( g_CmdLineData.m_Vendor == VENDOR_MICROSOFT)
	{
		return( PassThru( argc, argv));
	}

	// Read options from configuration file
	if( ProcessConfigurationFile( ) == -1)
	{
		printf( "ERROR finding configuration file, aborting compilation\n");
		return( EXIT_FAILURE);
	}

	if( g_Verbose )
	{
		printf("Applications...\n");
		for (int i = 0; i < APP_END_OF_LIST; i++)
		{
			if( g_CmdLineData.m_applications[i])
			{
				printf("%s....%s\n", pApplicationNames[i], g_CmdLineData.m_applications[i]);
			}
		}
	}

	// switch according to vendor
	switch( g_CmdLineData.m_Vendor )
	{
		case VENDOR_METROWERKS:         
			Result = Process_CodeWarrior();
			break;
//		case VENDOR_SN:
//			Result = Process_SN();
//			break;                            
		default:
			Result = EXIT_FAILURE;
			break;
	}

	return( Result );
}

