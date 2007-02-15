////////////////////////////////////////////////////////////////////////////
//
// DBLUtil
//
// Various utility functions for the DBL file format
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DBLUtil.h $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * update for streaming system
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/21/02    Time: 4:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * DBL Debug Code
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 5/16/02    Time: 9:35a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * AlignChunkData32byte allows for passing a pad value
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/13/02    Time: 4:08p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * added swap flag for StartDBLFile
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added library for handling the dbl file format (taken from Import
 * Export)
 */

#ifndef __DBLUTIL_H
#define __DBLUTIL_H

#ifndef __DBLDEBUG_H
#include "DBLDebug.h"
#endif

#ifndef _INC_STDIO
#include <stdio.h>
#endif

#ifndef P_BASETYPE_H
#include <Platform/BaseType.h>
#endif

#ifndef __G_DBFILE_H
#include <Game/Database/DBFile.h>
#endif

/******************* defines *****************************/

// this is not an actual limit, but is an arbitrary value to set aside space for the chunk log
#define MAX_CHUNKS_PER_DBL 200

/******************* macros ******************************/

/* for showing chunk/export summary */
#define WRITE_SUMMARY(message)		if (bLogFile) PluginError::WriteLogFile("%s", message); else PluginError::WriteWindow(message, FALSE);


struct ChunkLog
{
	u32	u32ChunkType;
	u32	u32ChunkDataSize;
	u32	u32ChunkDataSubSize;	// each chunk can have an optional "sub-chunk" size, like vertex colors in instance chunk
	u32	u32FileOrder;			// position of chunk in file
};

class DBLUtil
{
	static long m_lChunkHdrStart;		// record chunk header file position for update when chunk ends
	static long m_lChunkDataStart;	// record start of chunk data for determining chunk size
	static FILE *m_pDBLFile;		// current DBL file

	static ts_DBFileHdr m_CurrentFileHdr;						// DBL file header
	static ts_DBChunkHdr m_CurrentChunkHdr;			// named chunk header, may only write out unnamed portion
	static unsigned long m_ulRecordListSize;			// malloc'd size of data header
	static long m_lRecordListStart;						// record file position of record list for later updating
	static unsigned long m_ulRecordListEntryCount;	// count things as they are output and added to the offset table
	static long m_lFirstChunkStart;
	static char m_DBLFileName[200];
	static long m_lFileHeaderStart;						// not always 0 anymore since there may be some optional text at start of file

	/* record size of all chunks written */

	static ChunkLog m_chunkLog[MAX_CHUNKS_PER_DBL];
	static u32 m_u32ChunkCount;
	static u32 m_u32ChunkTotalSize;
	static u32 m_u32DBLMemoryFootprint;							// amount of System RAM this DBL is consuming
	static bool m_bChunkLogLocked;

	/* these need to get checked for clean-up on error */

	static ts_RecordList *m_pRecordList;
	static char *m_pCopyChunkBuffer;

#ifdef __DEBUG_DBL_ENABLE
	static int m_recordListEntry;
	static int m_currentChunkEntry;
#endif

public:
	static FILE *GetDBLFile(void);
	static const char *GetDBLFileName(void)	{ return m_DBLFileName; }
	static long GetChunkDataStart(void);

	static void StartDBLFile(const char *pFileName,const char *pText = NULL, bool bGenericData = false,const char *pExportMode = NULL,bool bSwapData = false);
	static u32 FinishDBLFile(void);
	static void CleanUpDBLFile(void);
	static void StartChunk(u16 u16Type, u16 u16Version, u16 u16DBFlags, const char *pcName);
	static u32 FinishChunk(bool bMoveChunkToFront, bool bLogChunk=true);
	static void CopyChunk(FILE *pDestFile, FILE *pSourceFile, bool bAddingChunk);
	static bool FindChunk(int nType, unsigned long *pulChunkStart);
	static void AlignFile(void);
	static void AlignChunkDataQuadWord(void);
	static void AlignChunkData32Byte(u8 padChar = 0xee);
	static void StartRecordList(unsigned long ulRecordCount, unsigned short usFlags, unsigned long ulRecordSize);
	static void FinishRecordList(void);
	static void RecordListEntry(void);
	static void SkipRecordListEntry(void);

	static void ChunkLogReset(void);
	static void ChunkLogAddChunk(u32 u32Type, u32 u32Size, bool bMoveChunkToFront);
	static int ChunkLogSortCompare(const void *arg1, const void *arg2);
	static void ChunkLogSort(void);
	static void ChunkLogGetSize(u32 u32Index, u32 *pu32Size, u32 *pu32SubSize, u32 *pu32TotalSize);
	static void MoveChunkToFront(long lChunkStart);
	static void ChunkLogSetSubSize(u32 u32Type, u32 u32SubSize);
	static u32 ChunkLogGetDBLMemoryFootprint(void);
	static void ChunkLogLock(void);
	static void ChunkLogUnlock(void);
	static void ChunkLogShowSummary(void);

};

#endif // __DBLUTIL_H
