#pragma once

#define DB_WRITER_H

#ifndef __G_DBFILE_H
#include <Game/Database/DBFile.h>
#endif

typedef void* BLOCK_HANDLE;
typedef void* LINK_FROM_HANDLE;
typedef void* LINK_TO_HANDLE;

#define GET_OFFSET(from,to) (((long)&to)-((long)&from))

class DBWriter
{
	TupString m_fileName;
	FILE *m_pFile;						// current DBL file

	ts_DBFileHdr m_currentFileHeader;		// DBL file header
	long m_fileHeaderStart;					// not always 0 anymore since there may be some optional text at start of file


	ts_DBChunkHdr m_currentChunkHeader;		// named chunk header, may only write out unnamed portion
	long m_firstChunkStart;
	long m_chunkHeaderStart;		// record chunk header file position for update when chunk ends
	long m_chunkDataStart;			// record start of chunk data for determining chunk size

	/* record size of all chunks written */

	long m_chunkCount;
	long m_chunkTotalSize;

	class Block
	{
	public:
		long m_filePos;
		long m_size;
		TupString m_name; // name of block (for error output)
	};

	class LinkFrom
	{
	public:
		BLOCK_HANDLE m_blockHandle;
		long m_offset;
		TupString m_name;
		LINK_TO_HANDLE m_linkToHandle;
	};

	class LinkTo
	{
	public:
		BLOCK_HANDLE m_blockHandle;
		long m_offset;
		TupString m_name;
	};

	bool m_bBlockActive;

	TupArray<Block> m_blocks;
	TupArray<LinkFrom> m_linkFromArray;
	TupArray<LinkTo> m_linkToArray;

public:

	DBWriter(void);

	FILE *GetDBLFile(void);
	long GetChunkDataStart(void);

	void StartDBLFile(const char *pFileName,const char *pText = NULL, bool bGenericData = false,const char *pExportMode = NULL);
	u32 FinishDBLFile(void);
	void CleanUpDBLFile(void);
	void StartChunk(u16 u16Type, u16 u16Version, u16 u16DBFlags, const char *pcName);
	u32 FinishChunk(void);
	void AlignFile(void);
	void AlignFileQuadWord(void);
	void AlignFile32Byte(u8 padChar = 0xee);

	BLOCK_HANDLE StartBlock(const char *pFormatString,...);
	void EndBlock(void);
	u32 GetBlockSize(BLOCK_HANDLE blockHandle);

	void WriteData(const void *pData, u32 length);

	LINK_FROM_HANDLE RegisterLinkFrom(BLOCK_HANDLE recordHandle,u32 offset,const char *pFormatString,...);
	LINK_TO_HANDLE RegisterLinkTo(BLOCK_HANDLE recordHandle,u32 offset,const char *pFormatString,...);

	void ConnectLinks(LINK_FROM_HANDLE linkFromHandle,LINK_TO_HANDLE linkToHandle);
};

