
#include <assert.h>

#include <fstream.h>
#include <string.h>

#include <mpo_imp.h>

#include "dbwriter.h"


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
	strncpy( FileHdr.cID, "DB", 2);
	FileHdr.sVersion = DB_VERSION;
	FileHdr.FileFlags = 0;
	FileHdr.ulChunks = 0;
	FileHdr.ulMaxChunkSize = 0;

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

bool DBWriter::DBStart( t_lflagbits dbFlags)
{
	CHK_STREAM( );

	FileHdr.FileFlags = dbFlags;

	lFileHdrStart = pMyStream->pos( );

	*pMyStream << FileHdr;

	pMyStream->Align( );

	lFileStart = pMyStream->pos( );

	return true;
}

bool DBWriter::DBEnd( void)
{
	long					lPos;

	CHK_STREAM( );

	pMyStream->Align( );

	ostream &str = pMyStream->stream( );

	lPos = str.tellp( );

	str.seekp( lFileHdrStart, ios::beg);

	*pMyStream << FileHdr;

	str.seekp( lPos, ios::beg);

	return true;
}

bool DBWriter::ChunkStart( unsigned short usType, t_sflagbits dbFlags)
{
	CHK_STREAM( );

	lChunkHdrStart = pMyStream->pos( );

	WriteChunkHdr( usType, dbFlags);

	lChunkStart = pMyStream->pos( );

	return true;
}

bool DBWriter::ChunkStart( unsigned short usType, t_sflagbits dbFlags, const char *pcName)
{
char						cName[CHK_NAMESIZE];

	CHK_STREAM( );

	memset( cName, 0, CHK_NAMESIZE);

	strcpy( cName, pcName);

	lChunkHdrStart = pMyStream->pos( );

	WriteChunkHdr( usType, dbFlags | CHK_NAMEDCHNK);

	pMyStream->Write( (unsigned char *) cName, 32);

	lChunkStart = pMyStream->pos( );

	return true;
}


void DBWriter::WriteChunkHdr( unsigned short usType, t_sflagbits dbFlags)
{
	ChunkHdr.usType = usType;
	ChunkHdr.ChunkFlags = dbFlags;
	ChunkHdr.ulChunkSize = 0;

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

	ChunkHdr.ulChunkSize = lChunkSize;

	*pMyStream << ChunkHdr;

	str.seekp( lPos, ios::beg);

	FileHdr.ulChunks++;
	if( (unsigned long) lChunkSize > FileHdr.ulMaxChunkSize)
		FileHdr.ulMaxChunkSize = lChunkSize;

	return true;
}

mpostream &operator<<( mpostream &stream, ts_DBFileHdr &hdr)
{
	stream.Write( (unsigned char *) &hdr.cID[0], ID_BYTES);
	stream << hdr.sVersion;
	stream << hdr.FileFlags;
	stream << hdr.ulChunks;
	stream << hdr.ulMaxChunkSize;

	return stream;
} ;


mpostream &operator<<( mpostream &stream, ts_DBChunkHdr &hdr)
{
	stream << hdr.usType;
	stream << hdr.ChunkFlags;
	stream << hdr.ulChunkSize;

	return stream;
} ;


