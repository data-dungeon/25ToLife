
#include <assert.h>
#include <stdio.h>

#include <fstream.h>
#include <string.h>

#include <mpo_imp.h>

#include <dbwriter.h>


#define CHK_NAMESIZE		32

#define CHK_STREAM( ) { if( pMyStream == NULL) return false; \
								if( !pMyStream->stream( ).good( )) return false; }

DBWriter::DBWriter( mpostream &stream)
{
	pMyStream = &stream;

	bOwnStream = false;

	Init( );
}

DBWriter::DBWriter( )
{
	pMyStream = NULL;
	bOwnStream = false;

	Init( );
}

DBWriter::DBWriter( const char *pcFName, db_stream Type)
{
	pMyStream = NULL;
	if( Attach( pcFName, Type) == true)
		bOwnStream = true;
	else
		bOwnStream = false;

	Init( );
}


DBWriter::~DBWriter( void)
{
ostream				*pStream;

	if( pMyStream)
	{
		pStream = &pMyStream->stream( );

		pStream->flush( );
		if( bOwnStream == true)
        {
         delete pStream;
			delete pMyStream;
        }
	}
}

bool DBWriter::Attach( const char *pcFName, db_stream Type)
{
ofstream					*pStream;

	assert( pcFName != NULL);
	assert( Type < ST_MAX);

	pStream = new ofstream( pcFName, ios::out | ios::binary);

	if( pStream->is_open( ))
	{
		switch( Type)
		{
		default:
		case PCSt:
			pMyStream = new PCStream( *pStream);
			break;
		case N64St:
			pMyStream = new N64Stream( *pStream);
			break;
		case PSXSt:
			pMyStream = new PSXStream( *pStream);
			break;
		}

		return true;
	}

	return false;


}
bool DBWriter::Attach( mpostream &stream)
{

	if( pMyStream)
		return false;

	bOwnStream = false;

	pMyStream = &stream;

	return true;
}

void DBWriter::Init( void)
{
	strcpy(FileHdr.cSkipCount, "0");	// atoi's to 0. see comments in _dblGetFileHdr in DBFile.cpp
	strcpy(FileHdr.cID, "DB");			// old style-- we don't know the platform
	FileHdr.u32Version = DB_VERSION;
	FileHdr.u32FileFlags = 0;
	FileHdr.u32Chunks = 0;
	FileHdr.u32MaxChunkSize = 0;
	
	memset( &ChunkHdr, 0, sizeof( ts_DBChunkHdr));

}

mpostream &DBWriter::stream( void)
{
	assert( pMyStream != NULL);

	return *pMyStream;
}

bool DBWriter::stream_open( void)
{
	return pMyStream == NULL ? false : true;
}

bool DBWriter::DBStart( t_lflagbits dbFlags, char *pComment /*= NULL*/)
{
	CHK_STREAM( );

	/* add comment to start of .dbl file if desired. this is useful for bringin files up in an editor and seeing */
	/* info on how they were created */

	if (pComment)
	{
		/* how big is it? */

		char *pEnd = pComment;
		while ((*pEnd != '\x1a') && (*pEnd != 0))	// terminators are 0 and ^z
			pEnd++;
		int nTextCount = pEnd - pComment + 1;	// include the terminator (we want the ^z there to stop a command prompt "type" command)
		if (nTextCount != 0)
		{
			/* ok, first word in text portion of file needs to be a number-- the total size of the text chunk, including */
			/* the number itself. also, i want the whole thing to be 32-byte aligned, because in the future we may */
			/* choose not to collapse the file header (it's so small, and collapsing takes time) */

			int nSkipCount = 8 + nTextCount;	// skipcount numeric string will be 8 bytes, padded with spaces
			int nPadCount = 32 - (nSkipCount & 31);
			nSkipCount += nPadCount;
			ASSERT((nSkipCount & 31) == 0);
			char cSkipCount[8];
			sprintf(cSkipCount, "%d", nSkipCount);
			for (int i = strlen(cSkipCount); i < 8; i++)
				cSkipCount[i] = ' ';
			cSkipCount[7] = '\n';

			/* pad with 0's, i guess */

			char cPad[32];
			memset(cPad, 0, 32);

			/* and write out to file */

			for (i = 0; i < 8; i++)
				*pMyStream << cSkipCount[i];
			for (i = 0; i < nTextCount; i++)
				*pMyStream << pComment[i];
			for (i = 0; i < nPadCount; i++)
				*pMyStream << cPad[i];
		}
	}

	/* now standard header */

	FileHdr.u32FileFlags = dbFlags;

	lFileHdrStart = pMyStream->tellp( );

	*pMyStream << FileHdr;

//yuch-- don't align file header-- it gets collapsed away
//	pMyStream->Align( );
	pMyStream->ResetAlignOffset( );	//consider ourselves aligned at this point

	lFileStart = pMyStream->pos( );

	return true;
}

bool DBWriter::DBEnd( void)
{
streampos		Pos;

	CHK_STREAM( );

//yuch-- don't bother aligning file end
//	pMyStream->Align( );

	Pos = pMyStream->tellp( );

	pMyStream->seekp( lFileHdrStart, ios::beg);

	*pMyStream << FileHdr;

	pMyStream->seekp( Pos);

	return true;
}

/*
bool DBWriter::ChunkStart( unsigned short usType, t_sflagbits dbFlags)
{
	CHK_STREAM( );

	lChunkHdrStart = pMyStream->pos( );

	WriteChunkHdr( usType, dbFlags);

	lChunkStart = pMyStream->pos( );

	return true;
}
 */
bool DBWriter::ChunkStart( unsigned short usType, t_sflagbits dbFlags, unsigned short usVersion, const char *pcName)
{

	CHK_STREAM( );

	lChunkHdrStart = pMyStream->pos( );
	pMyStream->ResetAlignOffset( );	//consider ourselves aligned at this point

	if( pcName == NULL)
	{
		WriteChunkHdr( usType, dbFlags | DBL_HEADER_32BYTE, usVersion, NULL);
	}
	else
	{
		WriteChunkHdr( usType, dbFlags | CHK_NAMEDCHNK | DBL_HEADER_32BYTE, usVersion, pcName);
	}

	lChunkStart = pMyStream->pos( );
	pMyStream->ResetAlignOffset( );	// we want to align within chunk data

	return true;
}


void DBWriter::WriteChunkHdr( unsigned short usType, t_sflagbits dbFlags, unsigned short usVersion, const char *pcName)
{
	ChunkHdr.u16Type = usType;
	ChunkHdr.u16Flags = dbFlags;
	ChunkHdr.u32Size = 0;
	ChunkHdr.u16Version = usVersion;
	strncpy( ChunkHdr.cName, pcName, DBL_MAXNAME);

	*pMyStream << ChunkHdr;
}

bool DBWriter::ChunkEnd( void)
{
	long					lPos;
	long					lChunkSize;

	CHK_STREAM( );

	pMyStream->Align( );

	ostream &str = pMyStream->stream( );

	lPos = str.tellp( );

	lChunkSize = lPos - lChunkStart;

	str.seekp( lChunkHdrStart, ios::beg);
	pMyStream->ResetAlignOffset( );	//consider ourselves aligned at this point

	ChunkHdr.u32Size = lChunkSize;

	*pMyStream << ChunkHdr;

	str.seekp( lPos, ios::beg);
	pMyStream->ResetAlignOffset( );	//consider ourselves aligned at this point

	FileHdr.u32Chunks++;
	if( (unsigned long) lChunkSize > FileHdr.u32MaxChunkSize)
		FileHdr.u32MaxChunkSize = lChunkSize;

	return true;
}

mpostream &operator<<( mpostream &stream, ts_DBFileHdr &hdr)
{
	stream.Write( (unsigned char *) hdr.cSkipCount, SKIP_COUNT_BYTES);
	stream.Write( (unsigned char *) hdr.cID, ID_BYTES);
	stream << hdr.u32Version;
	stream << hdr.u32FileFlags;
	stream << hdr.u32Chunks;
	stream << hdr.u32MaxChunkSize;

	return stream;
} ;


mpostream &operator<<( mpostream &stream, ts_DBChunkHdr &hdr)
{
	stream << hdr.u16Type;
	stream << hdr.u16Flags;
	stream << hdr.u32Size;
	stream << hdr.u16Version;

	bool bAlignBlocks = stream.IsFlagSet( MPO_WR_ALIGNBLOCKS);
	
	stream.ClearWriteFlag( MPO_WR_ALIGNBLOCKS);

	stream.Write( (unsigned char *)hdr.cName, sizeof hdr.cName);

	memset( &hdr.filler, 0, sizeof hdr.filler);
	stream.Write( (unsigned char *)hdr.filler, sizeof hdr.filler);

	if( bAlignBlocks)
		stream.SetWriteFlag( MPO_WR_ALIGNBLOCKS);
	return stream;
} ;

mpostream &operator<<( mpostream &stream, ts_DBBaseChunkHdr &hdr)
{
	stream << hdr.u16Type;
	stream << hdr.u16Flags;
	stream << hdr.u32Size;
	stream << hdr.u16Version;

	bool bAlignBlocks = stream.IsFlagSet( MPO_WR_ALIGNBLOCKS);
	
	stream.ClearWriteFlag( MPO_WR_ALIGNBLOCKS);

	memset( &hdr.filler, 0, sizeof hdr.filler);
	stream.Write( (unsigned char *)hdr.filler, sizeof hdr.filler);

	if( bAlignBlocks)
		stream.SetWriteFlag( MPO_WR_ALIGNBLOCKS);

	return stream;
} ;


