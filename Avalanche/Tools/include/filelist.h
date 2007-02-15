/* --------------------------------------------------
 * File    : FILELIST.H
 * Created : Mon Sep 21 11:37:24 1998
 * Descript: 
 * --------------------------------------------------*/
#ifndef __FILELIST_H
#define __FILELIST_H
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <windows.h>
#include <fstream>
#include <stdlib.h>

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
#define MAX_PARAMS 200
#define NO_FILEID 0xffff

typedef struct FileListEntry_tag
{
	char acFileName[_MAX_PATH];
	unsigned short usFileID;
	char acParams[MAX_PARAMS];
} ts_FileListEntry;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

class FileList
{
	std::ifstream				*pStream;

	char					acFileName[_MAX_PATH];

	ts_FileListEntry	*pEntries;

	int					nEntries,
							nCurrentEntry;

	void					Setup( char *pNewName);
	void					Detach( void);
	void					Attach( void);
	void					Process( void);

	enum					FileListError 
	{
		FLE_NO_ERROR,
		FLE_NOT_EXIST			//acFileName does not exist
	};

public:
	FileList( void);
	FileList( const char *pNewName);
	FileList( char *pNewName);
	FileList( char **ppFileList, int nFileNames, char **ppArgList, int nArgCount);
	~FileList( );

	void CreateList( void);
	void CreateList( const char *pNewName);
	void CreateList( char *pNewName);

	void Reset( void) { nCurrentEntry = 0; };

	int NumEntries( void) { return nEntries; };

	ts_FileListEntry	* Get( void);
	ts_FileListEntry	* Get( int iEntry);

	FileListError	LastError;
} ;

#endif