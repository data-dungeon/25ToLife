//==============================================================================
//
//  xCL - Cross Platform Compile and Link
//
//==============================================================================

// Configuration file handler

//==============================================================================
//  INCLUDES    
//==============================================================================

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xCL.hpp"

#include <windows.h>
#include <iostream>
#include <fstream>
#include <cstring>

//==============================================================================
//  DEFINES
//==============================================================================
#define MAX_FILENAME_LENGTH 250
#define MAX_LINE_SIZE 300

#define MAX_TREES 20

typedef enum
{
	STATE_INITIAL = 0,
	STATE_TREES = 1,
	STATE_APPS,
	STATE_INCLUDES,
	STATE_LIBPATH,
	STATE_LIBS,
	STATE_PREFIX,
	STATE_DEFINES,
	STATE_COMPILE_OPTIONS,
	STATE_LINK_OPTIONS,
	LAST_STATE
}
ParserState;


//==============================================================================
//  Variables
//==============================================================================
static char					*pstrTreeNames[MAX_TREES];
static char					*pstrTreeValues[MAX_TREES];
static int					iNumTrees = 0;
static cmd_line_data		&C = g_CmdLineData;

//==============================================================================
//  LOCAL Procedures
//==============================================================================
static int LocateConfigurationFile( char * strFilename);

using namespace std;

string				nullString;
//==============================================================================

// split = into two halves
inline string split_lhs (string& strLine)
{
	int locStrEnd = strLine.find_first_of(" \t", 0);
	return strLine.substr(0, locStrEnd);
}

inline string split_rhs (string& strLine)
{

	int locStrStart = strLine.find_first_not_of(" \t", 1 + strLine.find('=', 0));

	nullString = "";

	if(locStrStart == string::npos)
	{
		return nullString;
	}
	return strLine.substr(locStrStart);
}

//==============================================================================

static
void expandSourceTrees (string& strPath)
{
	/// find {NAME} and replace it with a source tree
	int locStartTree = strPath.find('{', 0);
	int locEndTree = strPath.find('}', 0);

	// if there's a {NAME} to replace...
	if (locStartTree > -1)
	{
		// get the name of the source tree
		string strTreeName = strPath.substr(locStartTree + 1, (locEndTree - locStartTree) - 1);
		char * strTreePath = NULL;

		// look up source tree
		for (int i = 0; i < iNumTrees; i++)
		{
			if (strTreeName.compare(pstrTreeNames[i]) == 0)
			{
				strTreePath = pstrTreeValues[i];
				break;
			}
		}

		if (strTreePath == NULL)
		{
			cerr << "ERROR: can't find definition of source tree {" << strTreeName.c_str() << "}\n";
			return;
		}

		// replace the path
		strPath.replace(
			strPath.begin() + locStartTree, 
			strPath.begin() + locEndTree + 1,
			strTreePath);
		
		//cout << "new path '" << strPath.c_str() << "'\n";
	}
}

//==============================================================================

static
void addTree (string& strTemp)
{
	//printf("add tree %s\n", strTemp.c_str());

	if (iNumTrees == MAX_TREES)
	{
		printf("ERROR: number of source trees exceeded, ignoring this one:\n  %s\n", strTemp.c_str());
	}
	else
	{
		// store tree names and values
		pstrTreeNames[iNumTrees] = strdup(split_lhs(strTemp).c_str());
		pstrTreeValues[iNumTrees] = strdup(split_rhs(strTemp).c_str());
		iNumTrees++;

		if( stricmp( pstrTreeNames[iNumTrees - 1], "DEBUGLIB") == 0 && C.m_DebugLibs)
		{
			pstrTreeNames[iNumTrees] = strdup( "LIBVER");
			pstrTreeValues[iNumTrees] = strdup( pstrTreeValues[iNumTrees - 1]);
			iNumTrees++;
		}
		else
		if( stricmp( pstrTreeNames[iNumTrees - 1], "RELLIB") == 0 && !C.m_DebugLibs)
		{
			pstrTreeNames[iNumTrees] = strdup( "LIBVER");
			pstrTreeValues[iNumTrees] = strdup( pstrTreeValues[iNumTrees - 1]);
			iNumTrees++;
		}
	}			
}

//==============================================================================

static
void addApplication (string& strTemp)
{
	//printf("add application %s\n", strTemp.c_str());
	string strAppName = split_lhs(strTemp);
	string strAppPath = split_rhs(strTemp);
	expandSourceTrees(strAppPath);

	int locAppName = strAppPath.find_last_of("/\\");
	string strApplication;
	string strPath;

	if (locAppName == -1)
	{
		// no path
		strApplication = strAppPath;
	}
	else
	{
		strApplication = strAppPath.substr(locAppName + 1);
		strPath = strAppPath.substr(0, locAppName);
	}

	//printf("APPNAME '%s'\nPATH '%s' APP '%s'\n", strAppName.c_str(), 
	//	strPath.c_str(), strApplication.c_str());
	
	// add the application
	g_CmdLineData.setApplication(strAppName.c_str(), strPath.c_str(), strApplication.c_str());
}

//==============================================================================

static
void addIncludePath (string& strPath)
{
	// expand any source tree in the path
	expandSourceTrees(strPath);

	// add it to the paths
	g_CmdLineData.m_IncludeDir.Add(strdup(strPath.c_str()));
}

//==============================================================================

static
void addLibraryPath (string& strPath)
{
	// expand any source tree in the path
	expandSourceTrees(strPath);

	// add it to the library paths
	g_CmdLineData.m_LibraryDir.Add(strdup(strPath.c_str()));
}

//==============================================================================

static
void addLibrary (string& strLibrary)
{
	// expand any source tree in the path
	expandSourceTrees( strLibrary);

	// add to libraries
	g_CmdLineData.m_LibraryFile.Add(strdup(strLibrary.c_str()));
}

//==============================================================================

static
void setPrefixFile (string& strPrefixFile)
{
	// expand any source tree in the path
	expandSourceTrees(strPrefixFile);

	// set prefix file
	strcpy(g_CmdLineData.m_PrefixFile, strPrefixFile.c_str());
}

//==============================================================================

static
void addDefine( string &strDefine)
{
	// add to defines
	g_CmdLineData.m_Define.Add( strdup( strDefine.c_str( )));
}

//==============================================================================

static
void addCompileOption( string &strCompileOption)
{
	// add to compile options
	expandSourceTrees( strCompileOption);
	g_CmdLineData.m_CompileOptions.Add( strdup( strCompileOption.c_str( )));
}

//==============================================================================

static
void addLinkOption( string &strLinkOption)
{
	// add to compile options
	expandSourceTrees( strLinkOption);
	g_CmdLineData.m_LinkOptions.Add( strdup( strLinkOption.c_str( )));
}

//==============================================================================
// Process the configuration file
//==============================================================================

int ProcessConfigurationFile( )
{

	char strConfigFilename[MAX_FILENAME_LENGTH];
	char strLine[MAX_LINE_SIZE];
	
	string strTemp;

	if( !LocateConfigurationFile( strConfigFilename))
	{
		return -1;
	}

	// open config file
	ifstream input( strConfigFilename, ios::in);

	if( !input.is_open( ))
	{
		printf( "Can't open configuration file %s\n", strConfigFilename);
		return -1;
	}

	// set up the parser state
	ParserState state = STATE_INITIAL;

	// parse up to end of target
	while( !input.eof( ))
	{
		input.getline( strLine, MAX_LINE_SIZE);
		strTemp = strLine;

		// ignore comments
		if( strTemp[0] == '#' || strTemp.empty( ))
		{
			continue;
		}

		// pick up tag start and end
		if( strTemp[0] == '<')
		{
			if (strTemp[1] == '/')
			{
				state = STATE_INITIAL;
			}
			else
			{
				// tag start
				int pos = strTemp.find('>', 0);

				string strTagName = strTemp.substr(1, pos - 1);

				//printf("TAG start '%s'\n", strTagName.c_str());

				if( strTagName.compare( "TREES") == 0)
				{
					state = STATE_TREES;
				}
				else
				if( strTagName.compare( "APPS") == 0)
				{
					state = STATE_APPS;
				}
				else
				if( strTagName.compare( "INCLUDES") == 0)
				{
					state = STATE_INCLUDES;
				}
				else
				if( strTagName.compare( "LIB_PATH") == 0)
				{
					state = STATE_LIBPATH;
				}
				else
				if( strTagName.compare( "LIBRARIES") == 0)
				{
					state = STATE_LIBS;
				}
				else
				if( strTagName.compare( "PREFIX") == 0)
				{
					state = STATE_PREFIX;
				}
				else
				if( strTagName.compare( "DEFINES") == 0)
				{
					state = STATE_DEFINES;
				}
				else
				if( strTagName.compare( "COMPILE_OPTIONS") == 0)
				{
					state = STATE_COMPILE_OPTIONS;
				}
				else
				if( strTagName.compare( "LINK_OPTIONS") == 0)
				{
					state = STATE_LINK_OPTIONS;
				}
			}
			// continue after setting state
			continue;
		}

		// parse based on state
		switch (state)
		{
			case STATE_TREES:
				// add a tree
				addTree( strTemp);
				break;

			case STATE_APPS:
				// add an application
				addApplication( strTemp);
				break;

			case STATE_INCLUDES:
				// add an include to the path
				addIncludePath( strTemp);
				break;

			case STATE_LIBPATH:
				// add a library path
				addLibraryPath( strTemp);
				break;

			case STATE_LIBS:
				// add a library
				addLibrary( strTemp);
				break;

			case STATE_PREFIX:
				setPrefixFile( strTemp);
				break;

			case STATE_DEFINES:
				// add a define
				addDefine( strTemp);
				break;

			case STATE_COMPILE_OPTIONS:
				// add a define
				addCompileOption( strTemp);
				break;

			case STATE_LINK_OPTIONS:
				// add a define
				addLinkOption( strTemp);
				break;

			default:
				// ignore if no state set
				break;
		}
	}

	// free source trees
	for( int i = 0; i < iNumTrees; i++)
	{
		free( pstrTreeNames[i]);
		pstrTreeNames[i] = NULL;

		free( pstrTreeValues[i]);
		pstrTreeValues[i] = NULL;
	}
	iNumTrees = 0;

	if( g_Verbose)
	{
		OpenSection( "Configuration File");

		printf( "filename %s\n", strConfigFilename); 

		//printf("NEW CONFIG\n");
		//g_CmdLineData.Dump();

		CloseSection( );
	}

	return 0;
}


//==============================================================================
// locate the config file
//==============================================================================

static 
int LocateConfigurationFile( char *strFilename)
{

	char		strConfigFilename[_MAX_PATH];
	char		*strFileComponent;
	char		*pTarget;
	char		*pVendor;

	unsigned long iSearchResult;

	switch( C.m_Target)
	{
		case TARGET_PS2:
			pTarget = "PS2_";
			break;

		case TARGET_GAMECUBE:
			pTarget = "GCN_";
			break;

		default:
			pTarget = NULL;
	}

	switch( C.m_Vendor)
	{
		case VENDOR_METROWERKS:
			pVendor = "CW";
			break;

		case VENDOR_SN:
			pVendor = "SN";
			break;

		default:
			pVendor = NULL;
	}

	if( pTarget == NULL || pVendor == NULL)
	{
		printf( "ERROR: Interal program error.  Vendor/Target not recognized\n");
		return( 0);
	}

	strcpy( strConfigFilename, "xCL_");
	strcat( strConfigFilename, pTarget);
	strcat( strConfigFilename, pVendor);
	strcat( strConfigFilename, ".txt");

	// try system paths
	iSearchResult = SearchPath(NULL, strConfigFilename, NULL, MAX_FILENAME_LENGTH, strFilename, &strFileComponent);

	if (iSearchResult > 0)
	{
	// search ok!
		return 1;
	}

	printf("ERROR: Could not locate %s !!!\n", strConfigFilename);
	printf("You can store a configuration file in\n 1. The same directory as cl.exe\n");
	printf(" 2. The Windows system directory\n 3. The Windows directory\n 4. The directories that are listed in the PATH environment variable\n\n");

	// search failed
	return 0;
}




