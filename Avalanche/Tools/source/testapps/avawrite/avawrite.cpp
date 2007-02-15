/* -----------------8/3/98 3:27PM--------------------
 * Includes
 * --------------------------------------------------*/
#include <stdio.h>

#include <htypes.h>
#include <textdb.h>

#include "avawrite.h"

/* -----------------8/3/98 3:27PM--------------------
 * Local Defines
 * --------------------------------------------------*/

/* -----------------8/3/98 3:27PM--------------------
 * Local Data
 * --------------------------------------------------*/

/* -----------------8/3/98 3:27PM--------------------
 * Local Prototypes
 * --------------------------------------------------*/

/* -----------------8/3/98 3:28PM--------------------
 * Local Data
 * --------------------------------------------------*/

/* -----------------8/3/98 3:28PM--------------------
 * Exported Functions
 * --------------------------------------------------*/

/* -----------------8/3/98 3:28PM--------------------
 * Local Functions
 * --------------------------------------------------*/

AVAWrite::AVAWrite( void)
{
	pDB = NULL;
	pAVA = NULL;

}


AVAWrite::~AVAWrite( void)
{
	if( pDB)
		delete pDB;

	if( pAVA)
		delete pAVA;
}

bool AVAWrite::AttachInput( const char *pcIn)
{

	pAVA = new HashProc( (char *) pcIn);

	if( pAVA->IsOpen( ))
		return pAVA->Process( );

	return false;
}

bool AVAWrite::AttachOutput( const char *pcOut, unsigned char ucFmt)
{
	pDB = new DBWriter( pcOut, (DBWriter::db_stream) ucFmt);

	return pDB->stream_open( );
}

bool AVAWrite::Write( t_lflagbits WriteFlags, t_lflagbits TexFlags)
{

	if( !WriteFlags)
		return true;

	pDB->DBStart( 0);

	if( WriteFlags & AVA_WR_TEXTURES)
		WriteTextures( TexFlags);

	pDB->DBEnd( );

	return true;
}


bool AVAWrite::WriteTextures( t_lflagbits TexFlags)
{
int					iPatchCount;

TextureDB			*pTexWrite;

HPatch				*pPatchList;

	pTexWrite = new TextureDB( pDB);

	pTexWrite->SetDBFormat( TexFlags);

	pPatchList = pAVA->PatchList( );
	
	iPatchCount = pAVA->PatchCount( );
	
	for( int i = 0; i < iPatchCount; i++)
	{
		if( pPatchList[i].pTex != NULL)
			pTexWrite->AddTexture( 0, 
				(unsigned short) pPatchList[i].iWidth, (unsigned short) pPatchList[i].iHeight, 
				pPatchList[i].pTex);
	}

	pTexWrite->Write( );

	delete pTexWrite;

	return true;

}