/* --------------------------------------------------
 * File    : FileList.CPP
 * Created : Wed Oct  7 18:07:35 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <filelist.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
char * ParseString( char *pBuffer, unsigned short *pFileID, char *pParams);
int ExecuteSearchCount( char *pBuffer);
void FillFileEntries( char *pBuffer, ts_FileListEntry **ppEntry, unsigned short usFileID, char *pParams);
bool FileNameExpand( char *pNewName, const char *pOldName);

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

FileList::FileList( void)
{
	Setup( (char *)NULL);
}

FileList::FileList( char *pNewName)
{
	Setup( pNewName);
}

FileList::FileList(const char *pNewName)
{
	Setup( (char *)pNewName);
}

/* this version is for file names passed in as arguments to program */

FileList::FileList(char **ppFileList, int nFileNames, char **ppArgList, int nArgCount)
{
	nEntries = nFileNames;
	pEntries = (ts_FileListEntry *)malloc( sizeof(ts_FileListEntry) * nEntries);
	for (int i = 0; i < nFileNames; i++)
	{
		strcpy(pEntries[i].acFileName, ppFileList[i]);
		pEntries[i].usFileID = NO_FILEID;

		/* file may need parameters. find it in original argument list and treat all subsequent arguments */
		/* that start with '-' or '/' as parameters */

		for (int j = 0; j < nArgCount; j++)
		{
			strcpy(pEntries[i].acParams, "");
			if (strcmp(pEntries[i].acFileName, ppArgList[j]) == 0)
			{
				/* found file name. skip to next argument and look for parameters */

				j++;
				while((j < nArgCount) && ((ppArgList[j][0] == '-') || (ppArgList[j][0] == '/')))
				{
					strcat(pEntries[i].acParams, ppArgList[j]);
					strcat(pEntries[i].acParams, " ");
					j++;
				}
				break;
			}
		}
	}
}

void FileList::Setup( char *pNewName)
{
	acFileName[0] = '\0';
	pEntries = NULL;
	nEntries = 0;

	pStream = new std::ifstream;
	if( pStream == NULL)
	{
		return;
	}

	if( pNewName == NULL)
	{
		return;
	}

	strcpy(acFileName,pNewName);
	CreateList( );
}

FileList::~FileList( )
{
	Detach( );
	if( pStream != NULL)
	{
		delete pStream;
	}
}

void FileList::CreateList( void)
{
	Detach( );
	Attach( );
	Process( );
}

void FileList::CreateList( const char *pNewName)
{
	Detach( );
	strcpy(acFileName,pNewName);
	Attach( );
	Process( );
}

void FileList::CreateList( char *pNewName)
{
	Detach( );
	strcpy(acFileName,pNewName);
	Attach( );
	Process( );
}

void FileList::Detach( void)
{
	if( pEntries != NULL)
	{
		free( pEntries);
	}
	pEntries = NULL;
	
	nEntries = 0;
	nCurrentEntry = 0;

	if( pStream != NULL)
	{
		if( pStream->is_open( ))
		{
			pStream->close( );
		}
	}
}

void FileList::Attach( void)
{
	if( pStream == NULL)
	{
		return;
	}

	pStream->open( acFileName, std::ios_base::in );
	if( !pStream->is_open( ))
	{
		LastError = FLE_NOT_EXIST;
	}
	else
	{
		LastError = FLE_NO_ERROR;
	}
}

void FileList::Process( void)
{

	int	nFiles = 0;

	char	*pString,
			acBuffer[_MAX_PATH + MAX_PARAMS + 2],
			acParams[MAX_PARAMS];

	unsigned short usFileID = 0;
	
	ts_FileListEntry *pCurrEntry;

	if( LastError != FLE_NO_ERROR)
	{
		return;
	}
	
	while( !pStream->eof( ))
	{
		pStream->getline( acBuffer, _MAX_PATH + MAX_PARAMS + 2);
		pString = ParseString( acBuffer, &usFileID, acParams);
		if( pString != NULL)
		{
			nFiles += ExecuteSearchCount( pString);
		}
	}

	nEntries = nFiles;

	if( nEntries == 0)
	{
		return;
	}

	pStream->clear( 0 );
	pStream->seekg( 0, std::ios_base::beg );

	pEntries = (ts_FileListEntry *)malloc( sizeof(ts_FileListEntry) * nEntries);
	
	pCurrEntry = pEntries;
	
	while( !pStream->eof( ))
	{
		pStream->getline( acBuffer, _MAX_PATH + MAX_PARAMS + 2);

		usFileID = NO_FILEID;//djr: assume their is no fileid parameter

		pString = ParseString( acBuffer, &usFileID, acParams);
		if( pString != NULL)
		{	
			FillFileEntries( pString, &pCurrEntry, usFileID, acParams);
		}
	}

	pStream->close( );
}

ts_FileListEntry * FileList::Get( void)
{
	if( nCurrentEntry >= nEntries)
	{
		return NULL;
	}
	if( pEntries == NULL)
	{
		return NULL;
	}
	return pEntries + nCurrentEntry++;
}

ts_FileListEntry * FileList::Get( int iEntry)
{
	if( iEntry >= nEntries)
	{
		return NULL;
	}
	if( pEntries == NULL)
	{
		return NULL;
	}
	return pEntries + iEntry;
}

char * ParseString( char *pBuffer, unsigned short *pFileID, char *pParams)
{
	
	char *pStart,
		  *pEnd,
		  *pParam,
		  *pEqual,
		  acExpanded[_MAX_PATH + MAX_PARAMS + 2];

	if( !FileNameExpand( acExpanded, pBuffer))
	{
		return( NULL);
	}

	pParams[0] = '\0';

 	pEnd = strchr( acExpanded, ';');
	if( pEnd != NULL)
	{
		*pEnd = '\0';
	}

	pStart = acExpanded;

	while( isspace( *pStart)) 
	{
		pStart++;
	}
	
	if( *pStart == '\0')
	{
		return NULL;
	}

	pEnd = pStart + strlen( pStart);
	pEnd--;

	while( isspace( *pEnd))
	{
		*pEnd = '\0';
		pEnd--;
	}
 
	pParam = strrchr( pStart,'/');
	while( pParam != NULL)
	{
		pEqual = strchr( pParam,'=');
		if( pEqual == NULL)
		{
			return NULL;
		}
		*pEqual = '\0';
		strupr( pParam);
		strcat( pParams, pParam);
		strcat( pParams, " ");
		strupr( pEqual + 1);
		strcat( pParams, pEqual + 1);
		strcat( pParams, " ");
		if( !stricmp( pParam, "/ID"))
		{
			if( sscanf( pEqual + 1,"%hu", pFileID) != 1)
			{
				*pFileID = 0;
			}
		}
 
		*pParam = '\0';

		pEnd = pStart + strlen( pStart);
		pEnd--;

		while( isspace( *pEnd))
		{
			*pEnd = '\0';
			pEnd--;
		}
 
		pParam = strrchr( pStart,'/');
	}
	
	pEnd = pStart + strlen( pStart);
	pEnd--;

	while( isspace( *pEnd))
	{
		*pEnd = '\0';
		pEnd--;
	}
	
	//get rid of quotes around the file name
	if( *pStart == '\"')
	{
		pStart++;
		char *pEndFileName = strchr(pStart, '\"');
		*pEndFileName = '\0';
	}

	strcpy( pBuffer, pStart);
	return( pBuffer);
}

bool FileNameExpand( char *pNewName, const char *pOldName)
{
 
	char	acOldName[_MAX_PATH + MAX_PARAMS + 2];

	char	*pDollar,
			*pCloseParen,
			*pStart;

	strcpy( acOldName, pOldName);

	pNewName[0] = '\0';

	pStart = acOldName;

	while( ( pDollar = strstr( pStart, "$(")) != NULL)
	{

		char	*pEnv;

		pCloseParen = strchr( pDollar, ')');
		if( pCloseParen == NULL)
		{
			return( false);
		}

		*pDollar = '\0';
		*pCloseParen = '\0';
		strcat( pNewName, pStart);

		pEnv = getenv( pDollar + 2);
		if( pEnv == NULL)
		{
			return( false);
		}

		strcat( pNewName, pEnv);

		pStart = pCloseParen + 1;
	}

	strcat( pNewName, pStart);

	return( true);
}

int ExecuteSearchCount( char *pBuffer)
{

	int	nFiles = 0;

	HANDLE	hSearch;
	
	WIN32_FIND_DATA	fileInfo;

	hSearch = FindFirstFile( pBuffer, &fileInfo);
	
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		printf( "Warning: no files found matching %s\n", pBuffer);
		return 0;
	}
	else
	{
		nFiles++;
	
		while( FindNextFile( hSearch, &fileInfo))
		{
			nFiles++;
		}
	}
	FindClose( hSearch);

	return nFiles;
}

void FillFileEntries( char *pBuffer, ts_FileListEntry **ppEntry, unsigned short usFileID, char *pParams)
{

	HANDLE	hSearch;
	
	WIN32_FIND_DATA	fileInfo;
	
	char pDrive[_MAX_DRIVE];
	char pPath[_MAX_PATH];

	_splitpath( pBuffer,pDrive,pPath,NULL,NULL);

	hSearch = FindFirstFile( pBuffer, &fileInfo);
	if( hSearch != INVALID_HANDLE_VALUE)
	{
		strcpy( (*ppEntry)->acFileName, pDrive);
		strcat( (*ppEntry)->acFileName, pPath);
		strcat( (*ppEntry)->acFileName, fileInfo.cFileName);
		strcpy( (*ppEntry)->acParams, pParams);
		(*ppEntry)->usFileID = usFileID;
		(*ppEntry)++;

		while (FindNextFile(hSearch, &fileInfo))
		{
			strcpy( (*ppEntry)->acFileName, pPath);
			strcat( (*ppEntry)->acFileName, fileInfo.cFileName);
			strcpy( (*ppEntry)->acParams, pParams);
			(*ppEntry)->usFileID = usFileID;
			(*ppEntry)++;
		}
		FindClose( hSearch);
	}
}



