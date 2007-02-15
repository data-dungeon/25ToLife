#include <stdio.h>
#include <string.h>

#include <fstream.h>

#include <dbstructs.h>

void TestRead( ifstream &str);

void main( int iArgC, char **ppcArgL)
{
ifstream				instream;

	for( int i = 1; i < iArgC; i++)
	{
		instream.open( ppcArgL[i], ios::binary | ios::in);
		
		strupr( ppcArgL[i]);

		if( instream.is_open( ))
		{
			printf( "File: %s\n", ppcArgL[i]);
			TestRead( instream);
		}
		else
			printf( "Unable to open %s\n", ppcArgL[i]);

		instream.close( );
	}

}


void TestRead( ifstream &Stream)
{
int					i;

ts_DBFileHdr		FileHdr;
ts_DBChunkHdr		ChunkHdr;

	Stream.read( (char *) &FileHdr, sizeof( ts_DBFileHdr));

	if( strncmp( FileHdr.cID, "DB", 2) != 0)
	{
		printf( "File ID Mismatch: %s\n", FileHdr.cID);
		return;
	}

	printf( "File Flags : 0x%08x\n", FileHdr.FileFlags);
	printf( "File Chunks: %lu\n", FileHdr.ulChunks);
	printf( "Max Chunk  : %lu\n", FileHdr.ulMaxChunkSize);

	i = 0;
	
	while( i++ < (int) FileHdr.ulChunks)
	{
		Stream.read( (char *) &ChunkHdr, sizeof( ts_DBChunkHdr));

		printf( "Chunk %u Type : %u\n", i, ChunkHdr.ucType);
		printf( "Chunk %u Flags: 0x%08x\n", i, ChunkHdr.ChunkFlags);
		printf( "Chunk %u Size : %lu\n", i, ChunkHdr.ulChunkSize);

		Stream.seekg( ChunkHdr.ulChunkSize, ios::cur);
	}

}