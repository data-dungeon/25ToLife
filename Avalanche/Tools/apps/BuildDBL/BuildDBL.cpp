/* --------------------------------------------------
 * File    : BuildDBL.cpp
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

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void Status( char *pPri, char *pSec, int iProgress);
void Usage( void);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
char	acOutPath[_MAX_PATH] = { 0 };

bool	bVerbose		= false,
		bBig			= false,
		bFileFlush	= false,
		bParamList	= false,	// if true, parameters list all input files-- no .lst file
		bNoPad		= false;	// if true, chunk data is not padded to 32-byte length

ts_ArgParseList	Args[] =
{
	{ "OPATH",		NULL, &acOutPath,		ARG_STRINGCPY	},
	{ "VERBOSE",	NULL, &bVerbose,		ARG_BOOLT		},
	{ "BIG",			NULL, &bBig,			ARG_BOOLT		},
	{ "FLUSH",		NULL, &bFileFlush,	ARG_BOOLT		},
	{ "PARAMLIST",	NULL, &bParamList,	ARG_BOOLT		},
	{ "NOPAD",		NULL, &bNoPad,			ARG_BOOLT		},
	NULL
};

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
int main( int nArg, char *apArg[])
{

	FNWrapper			NameWrapper;

	DBWriter				*pWriter = NULL;

	FileList				*pFileList;

	ts_FileListEntry	*pFileListEntry;

	char					**ppInFiles = NULL,
							*pKey,
							*pType,
							*pTemp,
							*pVersion,
							*pFill,
							acMsg[500];

	unsigned char		*pBuffer;

	int					nIndex,
							iType,
							iVersion;

	bool					bChunkFlush;

	HANDLE				hFile;

	DWORD					dwFileSize,
							dwBytesRead;

	BOOL					bResult;

	unsigned char		fillChar;

	DBWriter::db_stream	dbsType = DBWriter::PCSt;

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

	if( acOutPath[0] != 0)
	{
		AddSlash( acOutPath);
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

	if( acOutPath[0] != 0)
	{
		NameWrapper.ChangePath( acOutPath);
	}
	
	Status( "Creating <dblfile> ->", NameWrapper, 0);

	pWriter = new DBWriter( NameWrapper, dbsType);

	if ( !pWriter->stream_open() )
	{
		fprintf( stderr, "ERROR: BuildDBL can't open '%s' for write\n", ( const char* )NameWrapper );
		exit( 1 );
	}

	pWriter->stream( ).SetAlignment( 4);

	pWriter->DBStart( ( bFileFlush) ? CHK_FLUSHABLE : 0);

	for( nIndex = 0; nIndex < pFileList->NumEntries( ); nIndex++)
	{
		pFileListEntry = pFileList->Get( );

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

		bChunkFlush = false;

		if( strstr( pFileListEntry->acParams, "/FLUSH"))
		{
			bChunkFlush = true;
		}

		pType = strstr( pFileListEntry->acParams, "/TYPE");
		if( pType == NULL)
		{
			printf( "Error: No <type> given for %s.\n", pFileListEntry->acFileName);
			return -1;
		}

		pType += 6;

		if( sscanf( pType, "%i", &iType) != 1)
		{
			printf( "Error: Invalid <type> given for %s.\n", pFileListEntry->acFileName);
			return -1;
		}

		pVersion = strstr( pFileListEntry->acParams, "/VERSION");
		if( pVersion == NULL)
		{
			iVersion = 1;
		}
		else
		{
			pVersion += 9;

			if( sscanf( pVersion, "%i", &iVersion) != 1)
			{
				printf( "Warning: Invalid <version> given for %s, using <1>.\n", pFileListEntry->acFileName);
				iVersion = 1;
			}
		}

		pFill = strstr( pFileListEntry->acParams, "/FILL");
		if( pFill == NULL)
		{
			fillChar = 0;
		}
		else
		{
			pFill += 6;

			int	iFill;

			if( sscanf( pFill, "%i", &iFill) != 1)
			{
				printf( "Warning: Invalid <fill-char> given for %s, using <1>.\n", pFileListEntry->acFileName);
				iFill = 1;
			}

			fillChar = (unsigned char)iFill;
		}

		pKey = strstr( pFileListEntry->acParams, "/KEY");
		if( pKey == NULL)
		{
			printf( "Error: No <key> given for %s.\n", pFileListEntry->acFileName);
			return -1;
		}
		pKey += 5;

		pTemp = pKey;

		while( *pTemp != ' ' && *pTemp != '\0')
		{
			pTemp++;
		}
		*pTemp = '\0';

		sprintf( acMsg, "Adding chunk from %s, type = %s, key = %s\n", pFileListEntry->acFileName, pType, pKey);
		
		Status( acMsg, NULL, 0);

		pWriter->ChunkStart( iType, ( bChunkFlush) ? CHK_FLUSHABLE : 0, iVersion, pKey);

		pWriter->stream( ).Write( pBuffer, dwFileSize);

		if ((bNoPad == false) && ( dwFileSize & 0x1F))
		{
			Status("Padding chunk data to 32-byte aligned size\n", NULL, 0);

			int	remainder = 32 - ( dwFileSize & 0x1F),
					i;

			for( i = 0; i < remainder; i++)
			{
				pWriter->stream( ) << fillChar;
			}
		}

		pWriter->ChunkEnd( );

		free( pBuffer);
	}

	pWriter->DBEnd( );

	pWriter->stream( ).stream( ).flush( );
	
	delete pWriter;

	return 0;
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
void Usage( void)
{
	printf( "BuildDBL [ <options> ] <listfile> <dblfile>\n");
	printf( " <options>:\n");
	printf( "   -OPATH <path>      Output file Path\n");
	printf( "   -VERBOSE           Display progress messages\n");
	printf( "   -BIG               Generate big-endian output\n");
	printf( "   -FLUSH             Mark entire file as FLUSHABLE\n");
	printf( " <listfile>:\n");
	printf( "   A list file containing one or more entries for adding 'chunks' to <dblfile>\n");
	printf( "   Each entry looks like:\n");
	printf( "     <inputfile> /TYPE=<type> /KEY=<key> [ /VERSION=<version> ] [ /FILL=<fill-char> ] [ /FLUSH ]\n");
	printf( "     Where:\n");
	printf( "       <type> is the 'chunk' type for <inputfile>\n");
	printf( "       <key> is the database 'chunk' key for <inputfile>\n");
	printf( "       <version> OPTIONAL database 'chunk' version for <inputfile> ( default is 1)\n");
	printf( "       <fill> OPTIONAL fill character for padding to 32-byte alignment ( default is 0)\n");
	printf( "       /FLUSH marks the chunk as FLUSHABLE\n");
	printf( " <dblfile>:\n");
	printf( "   The DBL-format file to be created.  All chunks will be a multiple of 32 bytes in size.\n");
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

