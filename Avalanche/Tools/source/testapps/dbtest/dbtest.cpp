#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <fstream.h>

#include <mpo_imp.h>

#include <dbwriter.h>

void WriteTexture(	DBWriter *, unsigned long, unsigned short, unsigned short, void *pData);
void TestWrite( const char *pcName);
void TestRead( const char *pcName);

void main( void)
{

	TestWrite( "test2.dbl");
	TestRead( "test2.dbl");

}

void TestRead( const char *pcName)
{
int					i;

ifstream				*pStream;

ts_DBFileHdr		FileHdr;
ts_DBChunkHdr		ChunkHdr;

	pStream = new ifstream( pcName, ios::in | ios::binary);

	pStream->read( (char *) &FileHdr, sizeof( ts_DBFileHdr));

	i = 0;
	
	while( i++ < (int) FileHdr.ulChunks)
	{
		pStream->read( (char *) &ChunkHdr, sizeof( ts_DBChunkHdr));

		pStream->seekg( ChunkHdr.ulChunkSize, ios::cur);
	}

	pStream->close( );

	delete pStream;

}


void TestWrite( const char *pcName)
{
unsigned char		*pMem;
ofstream				*pStream;
PCStream				*pMPOStream;

DBWriter				*pWriter;

	pMem = (unsigned char *) malloc( 16 * 32);

	pStream = new ofstream( pcName, ios::out | ios::binary);

	pMPOStream = new PCStream( *pStream);

	pWriter = new DBWriter( *pMPOStream);

	pWriter->DBStart( 0x12345678);

	memset( pMem, 0xca, 16 * 32);
	WriteTexture( pWriter, 0, 16, 32, pMem);

	memset( pMem, 0xda, 32 * 16);
	WriteTexture( pWriter, 0, 32, 16, pMem);

	pWriter->DBEnd( );

	delete pWriter;
	delete pMPOStream;

	pStream->close( );

	delete pStream;

	free( pMem);

}

void WriteTexture(	DBWriter *pWriter, 
							unsigned long flags, unsigned short usWidth, unsigned short usHeight, void *pData)
{
mpostream			&str = pWriter->stream( );

	pWriter->ChunkStart( (unsigned short) 1, 0);
	
	str << flags;
	str << usWidth;
	str << usHeight;

	str.Write( (unsigned char *) pData, usWidth * usHeight);

	pWriter->ChunkEnd( );
	
} 


