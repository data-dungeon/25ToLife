//============================================================================
//=
//= DBLParse.h - Class for parsing chunks in DBL files
//=
//============================================================================

#pragma once
#ifndef _DBL_PARSE_H
#define _DBL_PARSE_H

#include "Game/Database/DBFile.h"

struct DBLFileRecord;

class CDBLParse
{
public:
	bool Init( const char* filename, void* data, int len, int flags );
	bool ParseChunk();
	static const char* GetChunkTypeString( int type, bool easy );
	static int GetChunkTypeVersion( int type );
	DBLFileRecord* MainFileRecord() { return d_mainFileRecord; }
	DBLFileRecord* FileRecord() { return d_fileRecord; }
	void* FirstPersistentData() { return d_firstPersistentData; }
	static void ReleaseChunkData( void* data, int type );

private:
	void SkipHeader( int bytes );
	void* ProcessChunkData( void* data );
	DBLFileRecord* MakeFileRecord( const char* filename, void* data, int len );
	bool CDBLParse::CheckChunkVersion() const;

	ts_DBFileHdr		d_fileHeader;
	ts_DBChunkHdr		d_chunkHeader;
	char*					d_fileBuffer;
	char*					d_readPtr;
	char*					d_writePtr;
	char*					d_endPtr;
	int					d_readSize;
	int					d_originalSize;
	bool					d_swapEndian;
	int					d_flags;
	DBLFileRecord*		d_fileRecord;
	DBLFileRecord*		d_mainFileRecord;
	void*					d_firstPersistentData;



};

#endif //_DBL_PARSE_H
