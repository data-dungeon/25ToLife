/* --------------------------------------------------
 * File    : N64TexSet.CPP
 * Created : Wed Oct  7 18:07:35 1998
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
#include <t_struct.h>
#include <filelist.h>
#include <readimage.h>
#include <cropbmi.h>
#include "compress.h"
#include "n64zip.h"
#include "\lib\argparse\argparse.h"

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

#define TEX_FLAG_SIZE_MASK			0x00000003
#define TEX_FLAG_SIZE_4_BIT		0x00000000
#define TEX_FLAG_SIZE_8_BIT		0x00000001
#define TEX_FLAG_SIZE_16_BIT		0x00000002
#define TEX_FLAG_SIZE_32_BIT		0x00000003

#define TEX_FLAG_FMT_MASK			0x00000070
#define TEX_FLAG_FMT_CI				0x00000010
#define TEX_FLAG_FMT_IA				0x00000020
#define TEX_FLAG_FMT_RGBA			0x00000040

typedef struct ROMTexHdr
{
	unsigned long	lFlags;
	unsigned long	ulOffset;
	short				sWidth,
						sHeight,
						sCompSize,
						sDecompSize;
} ts_ROMTexHdr;

typedef struct TextureList_tag
{
	struct TextureList_tag	*pNext;
	char							*pFileName;
} ts_TextureList;

typedef struct TexSet
{
	unsigned long	pNext;
	char			 	acSetName[10];
	short			 	sNumEntries;
	unsigned long 	apTexInfo[1];
} ts_TexSet;

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void Status( char *pPri, char *pSec, int iProgress);
void Usage( void);
void AddTexture( ts_FileListEntry *pFileListEntry);
unsigned long GetTextureIndex( ts_FileListEntry *pFileListEntry);
mpostream & operator << (mpostream &str, ts_TexSet &Hdr);
mpostream & operator << (mpostream &str, ts_ROMTexHdr &Hdr);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
char	acOutPath[_MAX_PATH] = { 0 };

bool	bVerbose = false,
		bPC		= false;

ts_ArgParseList	Args[] =
{
	{ "OPATH",		NULL, &acOutPath,	ARG_STRINGCPY	},
	{ "VERBOSE",	NULL, &bVerbose,	ARG_BOOLT		},
	{ "PC",			NULL, &bPC,			ARG_BOOLT		},
	NULL
};

ts_TextureList	*pMasterTextureList;

int	nTextures = 0;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
int main( int nArg, char *apArg[])
{

	FNWrapper			NameWrapper;

	DBWriter				*pWriter = NULL;

	FileList				*pListFileList,
							**ppTexSetList;

	ts_FileListEntry	*pFileListEntry;

	ts_TexSet			TexSet;

	ts_ROMTexHdr		*pROMTexHdr;

	ts_TextureList		*pCurrTex;

	BITMAPINFO			*bmi;

	RECT					recCrop;

	char					**ppInFiles = NULL,
							*pSetName = NULL,
							*bmBits,
							*pSrcImage,
							*pDestImage;

	int					nIndex,
							nTexture,
							nResult,
							nMaxPalette,
							nDecompSize,
							nCompSize;

	HANDLE				hFile;

	DBWriter::db_stream			dbsType = DBWriter::N64St;

	ParseArgs( nArg, &apArg[1], Args, &ppInFiles);

	if( ppInFiles[0] == NULL)
	{
		Usage( );
		printf( "Error: No <listfile> specified.\n");
		return -1;
	}

	if( ppInFiles[1] == NULL)
	{
		Usage( );
		printf( "Error: No <masterfile> specified.\n");
		return -1;
	}

	if( acOutPath[0] != 0)
		AddSlash( acOutPath);

	if( bPC)
	{
		dbsType = DBWriter::PCSt;
	}

	Status( "Reading List File", ppInFiles[0], 0);
	pListFileList = new FileList( ppInFiles[0]);

	if( pListFileList->NumEntries( ) == 0)
	{
		printf( "Error: No list files found in <listfile>\n");
		return -1;
	}

	ppTexSetList = (FileList **)malloc( sizeof(FileList *) * pListFileList->NumEntries( ));

	pListFileList->Reset( );

	for( nIndex = 0; nIndex < pListFileList->NumEntries( ); nIndex++)
	{
		pFileListEntry = pListFileList->Get( );
		Status( "Reading Set File", pFileListEntry->acFileName, 0);
		ppTexSetList[nIndex] = new FileList( pFileListEntry->acFileName);
	}

	pMasterTextureList = NULL;
	nTextures = 0;

	for( nIndex = 0; nIndex < pListFileList->NumEntries( ); nIndex++)
	{
		ppTexSetList[nIndex]->Reset( );
		for( nTexture = 0; nTexture < ppTexSetList[nIndex]->NumEntries( ); nTexture++)
		{
			AddTexture( ppTexSetList[nIndex]->Get( ));
		}
	}

	pListFileList->Reset( );

	for( nIndex = 0; nIndex < pListFileList->NumEntries( ); nIndex++)
	{
		pFileListEntry = pListFileList->Get( );

		NameWrapper = pFileListEntry->acFileName;

		NameWrapper.ChangeExt( ".txs");
		if( acOutPath[0] != 0)
		{
			NameWrapper.ChangePath( acOutPath);
		}
		NameWrapper.Break( NULL, &pSetName, NULL, FNW_GET_NAME);
		strupr( pSetName);

		Status( "Creating Set File", NameWrapper, 0);

		pWriter = new DBWriter( NameWrapper, dbsType);
		pWriter->stream( ).SetAlignment( 4);
		pWriter->DBStart( 0);
		pWriter->ChunkStart( 0x80, 0, pSetName);

		TexSet.pNext = 0;
		strncpy( TexSet.acSetName, pSetName, 10);
		TexSet.sNumEntries = ppTexSetList[nIndex]->NumEntries( );

		pWriter->stream( ) << TexSet;

		ppTexSetList[nIndex]->Reset( );
		for( nTexture = 0; nTexture < ppTexSetList[nIndex]->NumEntries( ); nTexture++)
		{
			pWriter->stream( ) << GetTextureIndex( ppTexSetList[nIndex]->Get( ));
		}

		pWriter->ChunkEnd( );
		pWriter->DBEnd( );
		delete pWriter;
	}
	SFREE( pSetName);

	InitCompress( );

	pROMTexHdr = (ROMTexHdr *)malloc( nTextures * sizeof(ROMTexHdr));
	
	NameWrapper = ppInFiles[1];

	if( acOutPath[0] != 0)
	{
		NameWrapper.ChangePath( acOutPath);
	}
	
	Status( "Creating <masterfile>", NameWrapper, 0);

	pWriter = new DBWriter( NameWrapper, dbsType);
	pWriter->stream( ).SetAlignment( 4);

	for( nTexture = 0; nTexture < nTextures; nTexture++)
	{
		pWriter->stream( ) << pROMTexHdr[nTexture];
	}

	pCurrTex = pMasterTextureList;

	for( nTexture = 0; nTexture < nTextures; nTexture++)
	{
		hFile = CreateFile(	pCurrTex->pFileName,
									GENERIC_READ,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
	 
		if (hFile == INVALID_HANDLE_VALUE)
		{
			printf( "Error: Could not open %s.\n",pCurrTex->pFileName);
			return -1;
		}
		
		nResult = DetermineImageType( hFile);
		if( nResult == TYPE_ANM)
		{
			printf( "Error: Anim files not supported %s.\n",pCurrTex->pFileName);
			return -1;
		}

		nResult = ReadImage( hFile, &bmi, &bmBits);	 
		if( nResult == FALSE)
		{
			printf( "Error: Image file %s could not be read.\n",pCurrTex->pFileName);
			return -1;
		}
	
		CloseHandle( hFile);

		pSrcImage = FlipImage( bmi, bmBits, &nMaxPalette, &recCrop);

		if( bPC)
		{
			MakeRGBAImage( &pSrcImage, bmi, bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight);
			pROMTexHdr[nTexture].lFlags = TEX_FLAG_FMT_RGBA | TEX_FLAG_SIZE_32_BIT;
			nDecompSize = (bmi->bmiHeader.biWidth * 4) * bmi->bmiHeader.biHeight;
		}
		else
		if( nMaxPalette < 16)
		{
			Pack4BitImage( &pSrcImage, bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight);
			pROMTexHdr[nTexture].lFlags = TEX_FLAG_FMT_CI;
			nDecompSize = (bmi->bmiHeader.biWidth + 1) >> 1;
			nDecompSize *= bmi->bmiHeader.biHeight;
		}
		else
		{
			PackImage( &pSrcImage, bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight);
			pROMTexHdr[nTexture].lFlags = TEX_FLAG_FMT_CI | TEX_FLAG_SIZE_8_BIT;
			nDecompSize = bmi->bmiHeader.biWidth * bmi->bmiHeader.biHeight;
		}
		pDestImage = (char *)malloc( nDecompSize * 2);

		pROMTexHdr[nTexture].ulOffset = pWriter->stream( ).tellp( );
		pROMTexHdr[nTexture].sWidth = (short)bmi->bmiHeader.biWidth;
		pROMTexHdr[nTexture].sHeight = (short)bmi->bmiHeader.biHeight;
		pROMTexHdr[nTexture].sDecompSize = (short)nDecompSize;
//		pROMTexHdr[nTexture].sCompSize = DictionaryEncode( (unsigned char *)pSrcImage, nDecompSize, pDestImage) * sizeof(long);
		if( bPC)
		{
			nCompSize = nDecompSize;
		}
		else
		{
			nCompSize = EncodeLZSS(	(unsigned char *)pSrcImage, (unsigned char *)pDestImage, nDecompSize);
		}
		if( nCompSize >= nDecompSize)
		{
			pROMTexHdr[nTexture].sCompSize = (short)nDecompSize;
			pWriter->stream( ).Write( (unsigned char *)pSrcImage, (int)pROMTexHdr[nTexture].sDecompSize);
		}
		else
		{
			pROMTexHdr[nTexture].sCompSize = (short)nCompSize;
			pWriter->stream( ).Write( (unsigned char *)pDestImage, (int)pROMTexHdr[nTexture].sCompSize);
		}
		pWriter->stream( ).Align( );
		free( bmi);
		free( bmBits);
		free( pSrcImage);
		free( pDestImage);
		pCurrTex = pCurrTex->pNext;
	}

	pWriter->stream( ).seekp( 0);
	for( nTexture = 0; nTexture < nTextures; nTexture++)
	{
		pWriter->stream( ) << pROMTexHdr[nTexture];
		if( pWriter->stream( ).stream( ).fail( ))
		{
			Status( "Rewrite Failed","",0);
		}
	}
	pWriter->stream( ).stream( ).flush( );

	SFREE( ppInFiles);

	return 0;
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
void Usage( void)
{
	printf( "N64TexSet [ <options> ] <listfile> <masterfile>\n");
	printf( " <options>:\n");
	printf( "   -OPATH <path>      Output file Path\n");
	printf( "   -VERBOSE           Display progress messages\n");
	printf( "   -PC                Generate PC Output\n");
	printf( " <listfile>:\n");
	printf( "   A list file containing list files for texture sets\n");
	printf( " <masterfile>:\n");
	printf( "   Master texture data file\n");
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

void AddTexture( ts_FileListEntry *pFileListEntry)
{

	ts_TextureList	*pCurrTex;

	pCurrTex = pMasterTextureList;

	while( pCurrTex)
	{
		if( !stricmp( pCurrTex->pFileName, pFileListEntry->acFileName))
		{
			return;
		}
		pCurrTex = pCurrTex->pNext;
	}

	pCurrTex = (ts_TextureList *)malloc( sizeof(ts_TextureList));
	pCurrTex->pNext = pMasterTextureList;
	pCurrTex->pFileName = pFileListEntry->acFileName;
	pMasterTextureList = pCurrTex;
	nTextures++;
}

unsigned long GetTextureIndex( ts_FileListEntry *pFileListEntry)
{

	ts_TextureList	*pCurrTex;

	unsigned long	ulIndex = 0;

	pCurrTex = pMasterTextureList;

	while( pCurrTex)
	{
		if( !stricmp( pCurrTex->pFileName, pFileListEntry->acFileName))
		{
			return ulIndex;
		}
		pCurrTex = pCurrTex->pNext;
		ulIndex++;
	}

	return -1;
}

mpostream & operator << (mpostream &str, ts_TexSet &Hdr)
{

	int	nIndex;

	str << Hdr.pNext;
	for( nIndex = 0; nIndex < sizeof Hdr.acSetName; nIndex++)
	{
		str << Hdr.acSetName[nIndex];
	}
	str << Hdr.sNumEntries;
	return str;
}

mpostream & operator << (mpostream &str, ts_ROMTexHdr &Hdr)
{
	str << Hdr.lFlags;
	str << Hdr.ulOffset;
	str << Hdr.sWidth;
	str << Hdr.sHeight;
	str << Hdr.sCompSize;
	str << Hdr.sDecompSize;
	return str;
}

