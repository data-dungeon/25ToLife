#include <stdio.h>
#include <malloc.h>

#include <textdb.h>

typedef struct TextDBHdr
{
	t_sflagbits		Flags;
	unsigned short	usVersion;
	unsigned long	ulTextures;
} ts_TextDBHdr;


ts_TextDBHdr;


TextureDB::TextureDB( DBWriter *pNewDB)
{
	pMyDB = pNewDB;
	pStream = &pMyDB->stream( );

	pTextList = NULL;
	ulTextCount = 0;
	DBFlags = 0;

}

TextureDB::~TextureDB( void )
{

	ReleaseList( );
}

void TextureDB::ReleaseList( void)
{
ts_IntText			*pList, *pNext;

	pList = pTextList;

	while( pList)
	{
		pNext = pList->pNext;

		free( pList);

		pList = pNext;
	}

}

void TextureDB::SetDBFormat( t_lflagbits Flags)
{
	DBFlags = Flags;
}

bool TextureDB::AddTexture( t_lflagbits Flags,
									unsigned short usWidth, unsigned short usHeight,
									void *pData)
{
ts_IntText			*pNew;

	pNew = (ts_IntText *) malloc( sizeof( ts_IntText));

	if( pNew == NULL)
		return false;

	pNew->usWidth = usWidth;
	pNew->usHeight = usHeight;
	pNew->ucFlags = (unsigned char) Flags;
	pNew->pData = pData;

	pNew->pNext = pTextList;

	pTextList = pNew;

	ulTextCount++;

	return true;
}

bool TextureDB::Write( char *pcName)
{

ts_IntText			*pText;


	if( ulTextCount == 0)
		return false;

	if( pcName)
		pMyDB->ChunkStart( TEX_DB_TYPE, 0, 0, pcName);
	else
		pMyDB->ChunkStart( TEX_DB_TYPE, 0, 0);

	WriteHdr( );

	pText = pTextList;	

	while( pText)
	{
		WriteTexture( pText);
		pText = pText->pNext;
	}

	pMyDB->ChunkEnd( );	

	return true;
}

void TextureDB::WriteTexture( ts_IntText *pText)
{
unsigned char		ucMode;

	ucMode = (unsigned char) DBFlags & 0x03;

	switch( ucMode)
	{
	case TEX_HDR_PACK:
		WriteTextHdrPack( pText);
		break;
	default:
		WriteTextHdrNorm( pText);
		break;
	}

	WriteTextData( pText);

	return;
}

void TextureDB::WriteTextHdrPack( ts_IntText *pText)
{
unsigned long		ulPackHdr;

	ulPackHdr = ((unsigned long) pText->ucFlags) << 24;
	ulPackHdr |= ((unsigned long) pText->usWidth) << 12;
	ulPackHdr |= ((unsigned long) pText->usHeight);
		
	*pStream << ulPackHdr;
}

void TextureDB::WriteTextHdrNorm( ts_IntText *pText)
{
	*pStream << pText->usWidth;
	*pStream << pText->usHeight;

	*pStream << (unsigned short) pText->ucFlags;

}


void TextureDB::WriteTextData( ts_IntText *pText)
{
unsigned long		ulDataSize;

	ulDataSize = pText->usWidth * pText->usHeight;

	if( pText->ucFlags & TEX_FMT_RGBA)
		ulDataSize *= 4;
	else
		ulDataSize *= 3;

	pStream->Write( (unsigned char *) pText->pData, ulDataSize);
}

void TextureDB::WriteHdr( void)
{
	
	*pStream << (unsigned short) (DBFlags & 0x0000FFFF);
	*pStream << (unsigned short) TEX_DB_VERSION;
	*pStream << ulTextCount;

}


