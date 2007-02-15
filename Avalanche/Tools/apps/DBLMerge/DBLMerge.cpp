/* --------------------------------------------------
 * File    : DBLMerge.cpp
 * Created : Sat Aug  7 18:07:35 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dbwriter.h>
#include <fn_wrap.h>
#include <t_macros.h>
//#include <t_struct.h>
#include <filelist.h>
#include <argparse.h>

/* engine includes */

#include "Game/Animation/MotionDB.h"

#define MAX_FILE_RECORD_NAME_LENGTH 256

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void Status( char *pPri, char *pSec, int iProgress);
void Usage( void);
unsigned long ULGetLittle( unsigned long *pulData);
unsigned short USGetLittle( unsigned short *pusData);
unsigned long ULGetBig( unsigned long *pulData);
unsigned short USGetBig( unsigned short *pusData);
//void AdjustChunk(int nType, void *pData, u32 &u32Length);
//void TruncateScriptDebugInfo(void *pData, u32 &u32Length);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
bool	bVerbose		= false,
		bBig			= false,
		bFileFlush	= false,
		bUberDBL		= false,
		bParamList	= false,	// if true, parameters list all input files-- no .lst fil
		bNoPad		= false;	// if true, chunk data is not padded to 32-byte length

ts_ArgParseList	Args[] =
{
	{ "VERBOSE",	NULL, &bVerbose,		ARG_BOOLT		},
	{ "BIG",			NULL, &bBig,			ARG_BOOLT		},
	{ "FLUSH",		NULL, &bFileFlush,	ARG_BOOLT		},
	{ "UBERDBL",	NULL, &bUberDBL,		ARG_BOOLT		},
	{ "PARAMLIST",	NULL, &bParamList,	ARG_BOOLT		},
	{ "NOPAD",		NULL, &bNoPad,			ARG_BOOLT		},
	NULL
};

typedef	unsigned long ( *GetULFunc)( unsigned long *pulData);
typedef	unsigned short ( *GetUSFunc)( unsigned short *pusData);

GetULFunc	GetUL;
GetUSFunc	GetUS;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
int main( int nArg, char *apArg[])
{

	FNWrapper				NameWrapper;
								
	DBWriter					*pWriter = NULL;
								
	FileList					*pFileList;
								
	ts_FileListEntry		*pFileListEntry;
								
	char						**ppInFiles = NULL;
								
	unsigned char			*pBuffer,
								*pCurr,
								*pChunkName;

	ts_DBFileHdr			*pDBFileHdr;

	ts_DBChunkHdr			*pDBChunkHdr;
								
	int						iFile,
								iChunk,
								nChunks;
								
	HANDLE					hFile;
								
	DWORD						dwFileSize,
								dwBytesRead;
								
	BOOL						bResult;

	unsigned short			usChunkFlags,
								usChunkVersion,
								usChunkType;

	unsigned long			ulChunkLength;

	DBWriter::db_stream	dbsType = DBWriter::PCSt;

	/* parse arguments, returns number of arguments that ParseArgs thinks are file names. under typical use, the */
	/* first returned argument is a .lst file, and the second is the output file name, and the return count is 2 */

	int nFileNames = ParseArgs( nArg, &apArg[1], Args, &ppInFiles);

	if( ppInFiles[0] == NULL)
	{
		Usage( );
		printf( "Error: No <listfile> specified.\n");
		return -1;
	}

	/* out file is last file specified */

	int nOutFileIndex = bParamList ? nFileNames - 1 : 1;
	if( ppInFiles[nOutFileIndex] == NULL)
	{
		Usage( );
		printf( "Error: No <dblfile> specified.\n");
		return -1;
	}

	if( bBig)
	{
		dbsType = DBWriter::N64St;
	}
	
	/* get list of input files */

	if (bParamList == false)
	{
		Status( "Reading <listfile> ->", ppInFiles[0], 0);
		pFileList = new FileList( ppInFiles[0]);
	}
	else
	{
		/* input files are on parameter list. they are the first (nFileNames - 1) files in ppInFiles */

		Status( "Building list of input files ->", NULL, 0);
		pFileList = new FileList( &ppInFiles[0], nFileNames - 1, &apArg[1], nArg - 1);
	}

	if( pFileList->NumEntries( ) == 0)
	{
		printf( "Error: No entries found in <listfile>\n");
		return -1;
	}

	pFileList->Reset( );

	NameWrapper = ppInFiles[nOutFileIndex];

	Status( "Creating <dblfile> ->", NameWrapper, 0);

	pWriter = new DBWriter( NameWrapper, dbsType);

	if( !pWriter->stream_open( ))
	{
		printf( "Unable to open %s for writing\n", ( (char *)NameWrapper));
		return -1;
	}

	pWriter->stream( ).SetAlignment( 4);

	/* create a comment to put at start of file */

#define MAX_COMMENT_LENGTH 200000

	char* acComment = new char[MAX_COMMENT_LENGTH];
	memset(acComment, 0, MAX_COMMENT_LENGTH);
	sprintf(acComment, "File %s created by DBLMerge\nCommand line:\n   ", ppInFiles[nOutFileIndex]);
	for (int i = 1; i < nArg; i++)
	{
		strcat(acComment, apArg[i]);
		strcat(acComment, " ");
	}
	strcat(acComment, "\nContents:\n");
	for( iFile = 0; iFile < pFileList->NumEntries( ); iFile++)
	{
		/* it's a good thing to be able to look back at data sizes */

		hFile = CreateFile(pFileList->Get(iFile)->acFileName,
								 GENERIC_READ,
								 FILE_SHARE_READ,
								 NULL,
								 OPEN_EXISTING,
								 FILE_ATTRIBUTE_NORMAL,
								 NULL);
		dwFileSize = 0;
		if (hFile != INVALID_HANDLE_VALUE)	// will fail below if file not found, but go ahead and finish comment
		{
			dwFileSize = GetFileSize( hFile, NULL);
			CloseHandle(hFile);
		}

		/* add file to comment */

		char acMessage[1000];
		sprintf(acMessage, "   %d: %s %d %s\n", iFile, pFileList->Get(iFile)->acFileName, dwFileSize, pFileList->Get(iFile)->acParams);
		strcat(acComment, acMessage);
		if (strlen(acComment) > MAX_COMMENT_LENGTH)
		{
			printf( "Error: Comment too long.\n");
			return -1;
		}
	}

	pWriter->DBStart( ( bFileFlush) ? CHK_FLUSHABLE : 0, acComment);

	for( iFile = 0; iFile < pFileList->NumEntries( ); iFile++)
	{
		pFileListEntry = pFileList->Get( );

		char *pKey = strstr( pFileListEntry->acParams, "/KEY");
		if( pKey)
		{
			pKey += 5;
		}

		hFile = CreateFile(	pFileListEntry->acFileName,
									GENERIC_READ,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
	 
		if (hFile == INVALID_HANDLE_VALUE)
		{
			printf( "Error: Could not open %s.\n", pFileListEntry->acFileName);
			return -1;
		}

		dwFileSize = GetFileSize( hFile, NULL);
		pBuffer = (unsigned char *)malloc( dwFileSize);
		if (pBuffer == NULL)
		{
			printf( "Error: Could not allocate memory for %s.\n", pFileListEntry->acFileName);
			return -1;
		}

		bResult = ReadFile(	hFile,
									pBuffer,
									dwFileSize,
									&dwBytesRead,
									NULL);

		CloseHandle( hFile);

		if( !bResult || ( dwBytesRead != dwFileSize))
		{
			printf( "Error: Could not read %s.\n", pFileListEntry->acFileName);
			return -1;
		}

		pCurr = pBuffer;

		/* point to file header */

		pDBFileHdr = (ts_DBFileHdr *)pCurr;

		/* skip over possible text prepended to file-- if there is, it starts with a number that is the number of bytes of text */

		int nSkipCount = atoi(pDBFileHdr->cSkipCount);
		if (nSkipCount)
		{
			pCurr += nSkipCount;
			pDBFileHdr = (ts_DBFileHdr *)pCurr;	// update pointer to file header
		}

		/* skip over header */

		pCurr += sizeof(ts_DBFileHdr);

		if( pDBFileHdr->u32Version == DB_VERSION)
		{
			GetUL = ULGetLittle;
			GetUS = USGetLittle;
		}
		else
		if( ULGetBig( &pDBFileHdr->u32Version) == DB_VERSION)
		{
			GetUL = ULGetBig;
			GetUS = USGetBig;
		}
		else
		{
			printf( "Invalid DBL version in %s.\n", pFileListEntry->acFileName);
			return( -1);
		}

		/* skip empty data (MakeDBL may have wrapped a 0-length file) */

//i'm commenting this back out because it's better to find out from the uberfile that a file is empty than to go to disk...
//		if (pDBFileHdr->u32MaxChunkSize == 0)
//		{
//			printf( "Skipping empty contents of %s.\n", pFileListEntry->acFileName);
//			continue;
//		}

		if( bUberDBL)
		{

			char		acFileName[MAX_FILE_RECORD_NAME_LENGTH * 2];
			memset(acFileName, 0, MAX_FILE_RECORD_NAME_LENGTH * 2);

			pWriter->ChunkStart( DB_FILENAME_CHUNK, CHK_FLUSHABLE, DB_FILENAME_CHUNK_VERSION, DEFAULT_CHUNK_KEY_STRING);
			strncpy( acFileName, pFileListEntry->acFileName, MAX_FILE_RECORD_NAME_LENGTH);

			/* game engine requests file loads relative to data directories-- e.g. it requests Particles\JujuParticles.dbl, */
			/* not Data_PS2\Particles\JujuParticles.dbl. the game engine takes care of looking in both the platform-dependent */
			/* and platform-independent directories. the game engine also relies on matching filenames in order to */
			/* determine whether a file has been loaded already. the file names in a makefile for an uberfile are relative */
			/* to that level's platform-dependent folder, e.g. relative to data_ps2\worlds\mtntop. so we see file names */
			/* like ..\..\Particles\JujuParticles.dbl or ..\..\..\Data\Worlds\MtnTop\Player.var. convert all file names */
			/* to be relative to data directories */

			// convert to lower case, convert slashes
			char* pFixName;
			for (pFixName = acFileName; *pFixName; pFixName++)
			{
				*pFixName = tolower(*pFixName);
				if (*pFixName == '/')
					*pFixName = '\\';
			}
			pFixName = NULL;

			/* since the makefiles starting directory is data_ps2\worlds\<world> (or data_pc, etc), the only ones i */
			/* really expect to see are "data\" (which always shows up as "..\..\..\data\") and "..\..\", but */
			/* doing this to the platform-dependent ones is safe */

			if      ((pFixName = strstr(acFileName, "data\\"))      != NULL) pFixName += strlen("data\\"     );
			else if ((pFixName = strstr(acFileName, "data_ps2\\"))  != NULL) pFixName += strlen("data_ps2\\" );
			else if ((pFixName = strstr(acFileName, "data_xbox\\")) != NULL) pFixName += strlen("data_xbox\\");
			else if ((pFixName = strstr(acFileName, "data_dx\\"))   != NULL) pFixName += strlen("data_dx\\"  );
			else if ((pFixName = strstr(acFileName, "data_pc\\"))   != NULL) pFixName += strlen("data_pc\\"  );
			else if ((pFixName = strstr(acFileName, "data_gcn\\"))  != NULL) pFixName += strlen("data_gcn\\" );
			else if (strncmp(acFileName, "..\\..\\", 6) == 0)                pFixName  = acFileName + 6;

			if (pFixName == NULL)
				pFixName = acFileName;	// original name is ok
			if (pFixName != acFileName)
				printf("Converted %s to %s\n", acFileName, pFixName);

			/* write it out */

			pWriter->stream( ).Write( (unsigned char *)pFixName, MAX_FILE_RECORD_NAME_LENGTH);

			pWriter->ChunkEnd( );
		}
		nChunks = (int)GetUL( &pDBFileHdr->u32Chunks);

		for( iChunk = 0; iChunk < nChunks; iChunk++)
		{
			pDBChunkHdr = (ts_DBChunkHdr *)pCurr;
			pCurr += sizeof( ts_DBBaseChunkHdrNonAlign);

			usChunkType = GetUS( &pDBChunkHdr->u16Type);
			usChunkFlags = GetUS( &pDBChunkHdr->u16Flags);
			ulChunkLength = GetUL( &pDBChunkHdr->u32Size);
			usChunkVersion = GetUS( &pDBChunkHdr->u16Version);

			if( usChunkFlags & CHK_NAMEDCHNK)
			{
				pChunkName = pCurr;

				//if this file entry has an overridden key, use that for the chunk name
				if( pKey)
					pChunkName = (unsigned char *)pKey;

				pCurr += DBL_MAXNAME;
			}
			else
			{
				pChunkName = NULL;
			}

			if( usChunkFlags & DBL_HEADER_32BYTE)
			{
				pCurr += sizeof pDBChunkHdr->filler;
			}

			/* we may change the chunk length in AdjustChunk, so record original chunk length so that */
			/* moving to next chunk works correctly */

			u32 u32OriginalChunkLength = ulChunkLength;

//			/* possibly change the chunk! for example, this is used to truncate debug data from script files */
//			/* when building an uberfile */
//
//			AdjustChunk(usChunkType, pCurr, ulChunkLength);

			/* write out chunk header-- length is left to be filled in by ChunkEnd */

			pWriter->ChunkStart( usChunkType, usChunkFlags, usChunkVersion, (const char *)pChunkName);

			/* write the chunk data */

			pWriter->stream( ).Write( pCurr, ulChunkLength);
			pCurr += u32OriginalChunkLength;	// to next chunk in file

			if ((bNoPad == false) && (ulChunkLength & 0x1F))
			{

				printf( "Warning: Chunk size increased for 32-byte alignment, chunk type %d, file %s\n", (int)usChunkType, pFileListEntry->acFileName);

				int	remainder = 32 - ( ulChunkLength & 0x1F),
						i;

				unsigned char		fillChar = 0;

				for( i = 0; i < remainder; i++)
				{
					pWriter->stream( ) << fillChar;
				}
			}

			pWriter->ChunkEnd( );
		}

		free( pBuffer);
	}

	pWriter->DBEnd( );

	pWriter->stream( ).stream( ).flush( );

	delete acComment;
	delete pWriter;

	printf("DBLMerge: success\n");

	return 0;
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
void Usage( void)
{
	printf( "DBLMerge [ <options> ] <listfile> <dblfile>\n");
	printf( " <options>:\n");
	printf( "   -VERBOSE           Display progress messages\n");
	printf( "   -BIG               Input files are assumed to be big endian\n");
	printf( "                      <dblfile> is created as big endian\n");
	printf( "   -FLUSH             Mark entire file as FLUSHABLE\n");
	printf( "   -UBERDBL           Add DB_FILENAME_CHUNKs to <dblfile>\n");
	printf( "   -PARAMLIST         Input files are parameters instead of contained in list file\n");
	printf( " <listfile>:\n");
	printf( "   A list file containing two or more DBLoader formatted files.\n");
	printf( "   All 'chunks' from each file are added to <dblfile>.\n");
	printf( " <dblfile>:\n");
	printf( "   The DBL-format file to be created\n");
}

void Status( char *pPri, char *pAlt, int iStatus)
{
	if( !bVerbose)
	{
		return;
	}
	printf( "[%03d%]", iStatus);
	if( pPri)
	{
		printf( " - %s\n", pPri);
		if( pAlt)
			printf( "      - %s\n", pAlt);
	}
}

unsigned long ULGetLittle( unsigned long *pulData)
{
	return *pulData;
}

unsigned short USGetLittle( unsigned short *pusData)
{
	return *pusData;
}

unsigned long ULGetBig( unsigned long *pulData)
{

	unsigned long	ulResult;

	unsigned char	*pucData,
						*pucResult;

	int				iIndex;

	pucData = (unsigned char *)pulData;
	pucResult = (unsigned char *)&ulResult;

	for( iIndex = 0; iIndex < 4; iIndex++)
	{
		pucResult[iIndex] = pucData[3 - iIndex];
	}

	return ulResult;
}

unsigned short USGetBig( unsigned short *pusData)
{

	unsigned short	usResult;

	unsigned char	*pucData,
						*pucResult;

	int				iIndex;

	pucData = (unsigned char *)pusData;
	pucResult = (unsigned char *)&usResult;

	for( iIndex = 0; iIndex < 2; iIndex++)
	{
		pucResult[iIndex] = pucData[1 - iIndex];
	}

	return usResult;
}

/* provide a chance for DBLMerge to edit the contents of a chunk before adding it to the merged file. for example, this
is used to truncate debug info from script files when building an uberfile */

#if 0
void AdjustChunk(int nType, void *pData, u32 &u32Length)
{
	/* does chunk need adjusting? */

	switch(nType)
	{
	case DB_ANIMSCRIPT:
	case DB_AISCRIPT:
		if (bUberDBL)
			TruncateScriptDebugInfo(pData, u32Length);
		break;
	}
}
#endif

/* remove debug info from script file! hack! the same file can be produced by calling AnimRip without the -d (debug) parameter.
but we don't want to have to re-compile all the scripts when building a cd, so we'll truncate the data here. this is a hack
because i have to copy script loading code from the game engine... */

#if 0
void TruncateScriptDebugInfo(void *pData, u32 &u32Length)
{
	/* point to the script */

	ts_animScript *pScript = (ts_animScript *) pData;
	if (pScript->Magic != 0xFEEDBEEF)
		return;	// don't know how to handle it

	/* is there debug info? */

	if (pScript->ppDebugOffsetTable == 0)
		return;	// nothing to do

	/* change size */

	u32Length = (u32) pScript->ppDebugOffsetTable;	// at this point, ppDebugOffsetTable is an offset from start of chunk data

	/* this is a hack, but leave pDebugText non-0 to mark that file originally had debug data. the issue is that */
	/* if there is debug data at the end of the file, then there is also debug data embedded in the opcodes */
	/* (line numbers that correlate the opcodes to the debug data text lines). we can't remove that, since it's */
	/* embedded, so the script-processing code still has to know to skip it. it checks the pDebugText field to */
	/* see whether the script originally had debug data, even though it has been truncated. it would be better */
	/* to have a flags word, then i could have a CONTAINS_DEBUG_DATA and DEBUG_DATA_TRUNCATED flags */

	pScript->ppDebugOffsetTable = 0;
	pScript->pDebugText = (char *) 0x12345678;	// definitely non-0
}
#endif
