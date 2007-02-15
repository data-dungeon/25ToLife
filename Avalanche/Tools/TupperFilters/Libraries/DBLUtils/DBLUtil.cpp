////////////////////////////////////////////////////////////////////////////
//
// DBLUtil
//
// Various utility functions for the DBL file format
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DBLUtil.cpp $
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * update for streaming system
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 8/21/02    Time: 4:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * DBL Debug Code
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 8/19/02    Time: 4:44p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * chunk header flag
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 5/16/02    Time: 9:35a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * AlignChunkData32byte allows for passing a pad value
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 5/13/02    Time: 4:08p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * added swap flag for StartDBLFile
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * hybrid configuration
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/01/02    Time: 12:05p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Rich Edit Control for the Log.  Added additional flags for font styles
 * and colors.
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Fixed Exception Problem
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

#include "DBLUtilsPch.h"

#include "DBLUtil.h"
#include "DBLSwap.h"
#include "DBLWrite.h"
#include "DBLDebug.h"

long DBLUtil::m_lChunkDataStart = 0;	// record start of chunk data for determining chunk size

// this needs to get checked for clean-up on error
FILE *DBLUtil::m_pDBLFile = NULL; // current DBL file
bool DBLUtil::m_bChunkLogLocked = FALSE;
ts_RecordList *DBLUtil::m_pRecordList = NULL;
char *DBLUtil::m_pCopyChunkBuffer = NULL;

long DBLUtil::m_lChunkHdrStart;		// record chunk header file position for update when chunk ends

ts_DBFileHdr DBLUtil::m_CurrentFileHdr;						// DBL file header
ts_DBChunkHdr DBLUtil::m_CurrentChunkHdr;			// named chunk header, may only write out unnamed portion
unsigned long DBLUtil::m_ulRecordListSize;			// malloc'd size of data header
long DBLUtil::m_lRecordListStart;						// record file position of record list for later updating
unsigned long DBLUtil::m_ulRecordListEntryCount;	// count things as they are output and added to the offset table
long DBLUtil::m_lFirstChunkStart;
char DBLUtil::m_DBLFileName[200];
long DBLUtil::m_lFileHeaderStart;						// not always 0 anymore since there may be some optional text at start of file

/* record size of all chunks written */

ChunkLog DBLUtil::m_chunkLog[MAX_CHUNKS_PER_DBL];
u32 DBLUtil::m_u32ChunkCount;
u32 DBLUtil::m_u32ChunkTotalSize;
u32 DBLUtil::m_u32DBLMemoryFootprint;							// amount of System RAM this DBL is consuming

/* these need to get checked for clean-up on error */

ts_RecordList *m_pRecordList;
char *m_pCopyChunkBuffer;

#ifdef __DEBUG_DBL_ENABLE
int DBLUtil::m_recordListEntry;
int DBLUtil::m_currentChunkEntry;
#endif

FILE *DBLUtil::GetDBLFile(void)
{
	return m_pDBLFile;
}

long DBLUtil::GetChunkDataStart(void)
{
	return m_lChunkDataStart;
}

// open database file, write out header 
void DBLUtil::StartDBLFile(const char *pFileName,const char *pText,bool bGenericData,const char *pExportMode,bool bSwapData)
{

#ifdef __DEBUG_DBL_ENABLE
	DBLDebug::Reset();
	DBLDebug::NextEntry();
#endif

	/* save passed file name */
	strcpy(m_DBLFileName, pFileName);

	/* open file for writing */

	m_pDBLFile = fopen(pFileName, "w+b");	// open for writing and reading
	if (m_pDBLFile == NULL)
	{
		throw(PluginException(ERROR_FATAL,"Unable to open %s file for writing",pFileName));
	}

	// may want the plugin to do this...
	DBLSwap::SetSwapEndianActive(bSwapData);

	/* check DBLUtil */
	DBLSwap::TestSwapEndian();

	/* if user passed some text to put at the start of the file, format it correctly and write it out first */
	if (pText)
	{
		/* how big is it? */

		const char *pEnd = pText;
		while ((*pEnd != '\x1a') && (*pEnd != 0))	// terminators are 0 and ^z
			pEnd++;
		int nTextCount = pEnd - pText + 1;	// include the terminator (we want the ^z there to stop a command prompt "type" command)
		if (nTextCount != 0)
		{
			/* we'll tack on the file name and export mode here */

			char cFileName[200];
			sprintf(cFileName, "%s\n", pFileName);
			char cExportMode[100];
			if (pExportMode)
				sprintf(cExportMode,"Export Mode: %s\n",pExportMode);
			else
				strcpy(cExportMode, "Export Mode: unknown\n");

			/* ok, first word in text portion of file needs to be a number-- the total size of the text chunk, including */
			/* the number itself. also, i want the whole thing to be 32-byte aligned, because in the future we may */
			/* choose not to collapse the file header (it's so small, and collapsing takes time) */

			int nSkipCount = 8 + strlen(cFileName) + strlen(cExportMode) + nTextCount;	// skipcount numeric string will be 8 bytes, padded with spaces
			int nPadCount = 32 - (nSkipCount & 31);
			nSkipCount += nPadCount;
			assert((nSkipCount & 31) == 0);
			char cSkipCount[8];
			sprintf(cSkipCount, "%d", nSkipCount);
			for (int i = strlen(cSkipCount); i < 8; i++)
				cSkipCount[i] = ' ';
			cSkipCount[7] = '\n';

			/* pad with 0's, i guess */

			char cPad[32];
			memset(cPad, 0, 32);

			/* and write out to file */
			try
			{
				DBLWrite::WriteData(cSkipCount, 8, m_pDBLFile);
				DBLWrite::WriteData(cFileName, strlen(cFileName), m_pDBLFile);
				DBLWrite::WriteData(cExportMode, strlen(cExportMode), m_pDBLFile);
				DBLWrite::WriteData(pText, nTextCount, m_pDBLFile);
				DBLWrite::WriteData(cPad, nPadCount, m_pDBLFile);
			}
			catch (PluginException e)
			{
				throw(PluginException(ERROR_FATAL,"StartDBLFile - Name Write Failed\r\n  %s",(const char *)(e)));
			}
		}
	}

	/* fill in file header and write out */

	strcpy(m_CurrentFileHdr.cSkipCount, "0");	// atoi's to 0. see comments in _dblGetFileHdr in DBFile.cpp
	if (pExportMode)
		strcpy(m_CurrentFileHdr.cID, pExportMode);
	else
		strcpy(m_CurrentFileHdr.cID, "DB");	// unknown
	if (bGenericData)
		strcpy(m_CurrentFileHdr.cID, "DB");	// some data (e.g. collision data) is not meant to be run by game engine

	m_CurrentFileHdr.u32Version = DB_VERSION;
	m_CurrentFileHdr.u32FileFlags = 0;
	m_CurrentFileHdr.u32Chunks = 0;
	m_CurrentFileHdr.u32MaxChunkSize = 0;
	m_lFileHeaderStart = ftell(m_pDBLFile);	// record file position so i can go back and overwrite it later
	try
	{
		DBLWrite::WriteRecord(&m_CurrentFileHdr, m_pDBLFile);
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"StartDBLFile - Header Write Failed\r\n  %s",(const char *)(e)));
	}

	/* alignment */

//yuch-- since file header is collapsed, this is not necessary (and is a bug, actually, since the collapse is
// sizeof(ts_DBFileHdr) and if sizeof(ts_DBFileHdr) is not aligned we'll end up at the pad bytes...
//	if ((iReturnCode = AlignFile()) != PluginError::RETURN_SUCCESS)
//		return(iReturnCode);

	/* record where first chunk starts */

	m_lFirstChunkStart = ftell(m_pDBLFile);

	/* track memory footprint of this file */
	DBLUtil::ChunkLogReset();
}

/* update database header, close database */

u32 DBLUtil::FinishDBLFile(void)
{
	/* maybe was never opened */

	if (m_pDBLFile == NULL)
		assert(FALSE);

#ifdef __DEBUG_DBL_ENABLE
	DBLDebug::SetEntryIndex(0);
#endif

	/* update database header */

	fseek(m_pDBLFile, m_lFileHeaderStart, SEEK_SET);
	try
	{
		DBLWrite::WriteRecord(&m_CurrentFileHdr, m_pDBLFile);
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"FinishDBLFile - Header Write Failed\r\n  %s",(const char *)(e)));
	}

	/* close */
	fseek(m_pDBLFile, 0, SEEK_END);
	u32 fileSize = ftell(m_pDBLFile);

	fclose(m_pDBLFile);
	m_pDBLFile = NULL;
	return fileSize;
}

/* clean up. must be able to be called multiple times with no harm done */

void DBLUtil::CleanUpDBLFile(void)
{
	if (m_pDBLFile)
	{
		fclose(m_pDBLFile);
		m_pDBLFile = NULL;				// so subsequent calls don't do anything
	}
	if (m_pRecordList)
	{
		free(m_pRecordList);
		m_pRecordList = NULL;			// so subsequent calls don't do anything
	}
	if (m_pCopyChunkBuffer)
	{
		free(m_pCopyChunkBuffer);
		m_pCopyChunkBuffer = NULL;	// so subsequent calls don't do anything
	}
}

/* start of a chunk-- pcName can be NULL for unnamed chunks */
void DBLUtil::StartChunk(u16 u16Type, u16 u16Version, u16 u16DBFlags, const char *pcName)
{
	char cName[DBL_MAXNAME];

	/* handle chunk name */

	if (pcName != NULL)
	{
		memset(cName, 0, DBL_MAXNAME);
		strncpy(cName, pcName, DBL_MAXNAME);
		u16DBFlags |= DBL_NAMED_CHUNK;
	}

	u16DBFlags |= DBL_HEADER_32BYTE; // make sure it is the new header type

	/* remember current position so i can go back and update it after chunk is done */

	m_lChunkHdrStart = ftell(m_pDBLFile);

	/* write chunk header */

	m_CurrentChunkHdr.u16Type = u16Type;
	m_CurrentChunkHdr.u16Version = u16Version;
	m_CurrentChunkHdr.u16Flags = u16DBFlags;
	m_CurrentChunkHdr.u32Size = 0;

#ifdef __DEBUG_DBL_ENABLE
	m_currentChunkEntry = DBLDebug::NextEntry();
	char cType[64];
	sprintf(cType,"%x %s",(int)u16Type,pcName);
	DBLDebug::SetName(cType);
	DBLDebug::SetIndex(0);
#endif

	if (pcName)
	{
		strncpy(m_CurrentChunkHdr.cName, pcName, DBL_MAXNAME);
		try
		{
			DBLWrite::WriteRecord(&m_CurrentChunkHdr, m_pDBLFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"StartChunk - Header Write Failed\r\n   %s",(const char *)(e)));
		}
	}
	else
	{
		try
		{
			DBLWrite::WriteRecord((ts_DBBaseChunkHdr *) &m_CurrentChunkHdr, m_pDBLFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"StartChunk - Header Write Failed\r\n  %s",(const char *)(e)));
		}
	}

	/* record start of chunk data so i can calculate chunk size */
	m_lChunkDataStart = ftell(m_pDBLFile);

	/* ok */

	fflush( m_pDBLFile);

#ifdef __DEBUG_DBL_ENABLE
	DBLDebug::NextEntry(); // we are ready for the next one
	DBLDebug::SetIndex(0);
#endif

}

/* update chunk header, global chunk count */
/*	Added bLogChunk to prevent chunks being counted twice when being
	overwritten. (ie: for the DMA SOM export) 
	bLogChunk defaults to "true"*/

u32 DBLUtil::FinishChunk(bool bMoveChunkToFront, bool bLogChunk)
{
	/* we align all chunks to a multiple of 32 bytes long. this supports the gamecube, where 32-byte alignment */
	/* is important, without really any cost for other platforms. this way, all chunks in .dbl file will */
	/* be 32-byte aligned when loaded into memory, whether or not chunks get collapsed away during processing */

	AlignChunkData32Byte();

	/* alignment */
	AlignFile();

	/* store chunk size in chunk header */

#ifdef __DEBUG_DBL_ENABLE
	int oldEntryIndex = DBLDebug::GetEntryIndex();
	DBLDebug::SetEntryIndex(m_currentChunkEntry);
#endif

	m_CurrentChunkHdr.u32Size = ftell(m_pDBLFile) - m_lChunkDataStart;
	fseek(m_pDBLFile, m_lChunkHdrStart, SEEK_SET);
	try
	{
		DBLWrite::WriteRecord((ts_DBBaseChunkHdr *) &m_CurrentChunkHdr, m_pDBLFile); // don't need to update named portion
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"FinishChunk - Header Write Failed\r\n  %s",(const char *)(e)));
	}
//	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"  Chunk data size %d bytes", m_CurrentChunkHdr.u32Size);

	/* another chunk added */
	m_CurrentFileHdr.u32Chunks++;
	if (m_CurrentChunkHdr.u32Size > m_CurrentFileHdr.u32MaxChunkSize)
		m_CurrentFileHdr.u32MaxChunkSize = m_CurrentChunkHdr.u32Size;

	/* back to end of file */

	fseek(m_pDBLFile, 0, SEEK_END);

#ifdef __DEBUG_DBL_ENABLE
	DBLDebug::SetEntryIndex(oldEntryIndex);
#endif

	/* maybe move this chunk to front of database file */

	if (bMoveChunkToFront)
	{
		try
		{
			MoveChunkToFront(m_lChunkHdrStart);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"FinishChunk - MoveChunkToFront Failed\r\n  %s",(const char *)(e)));
		}
	}

	/* record chunk in chunk log */

	if (bLogChunk)
		DBLUtil::ChunkLogAddChunk(m_CurrentChunkHdr.u16Type, m_CurrentChunkHdr.u32Size, bMoveChunkToFront);

	/* we are not within chunk start/finish bookmarks */

	m_lChunkDataStart = 0;

	return m_CurrentChunkHdr.u32Size;
}

/* move the last chunk to the start of the DBL file. this is useful for chunks that depend on other chunks for determining
their contents, but need to come before those chunks. an example is the texture context chunk-- i want to collect the texture
contexts during geometry output (since breaking models into tri strips can create other texture contexts than just the 
original patch texture contexts), but the texture context chunk should come before the geometry chunk so that the fix-up
routines can point to the correct texture contexts */

void DBLUtil::MoveChunkToFront(long lChunkStart)
{
	int i;
	FILE *pTempFile;
	long lCurrentFileSize;

	/* make sure there's something to do */

	if (m_CurrentFileHdr.u32Chunks <= 1)
		return;

	/* for error checking */

	lCurrentFileSize = ftell(m_pDBLFile);

	/* open up a temp file */

	pTempFile = fopen("tempdbl.tmp", "w+b");	// open for writing and reading
	if (pTempFile == NULL)
	{
		throw(PluginException(ERROR_FATAL,"MoveChunkToFront - Unable to open tempdbl.tmp file for writing."));
	}

	/* copy file header and possible text */

	void *pCopyData = malloc(m_lFileHeaderStart + sizeof(ts_DBFileHdr));
	if (!pCopyData)
		throw(PluginException(ERROR_FATAL,"MoveChunkToFront - Malloc failed on %d bytes.",m_lFileHeaderStart + sizeof(ts_DBFileHdr)));
	fseek(m_pDBLFile, 0, SEEK_SET);	// to beginning of file
	fread(pCopyData, m_lFileHeaderStart + sizeof(ts_DBFileHdr), 1, m_pDBLFile);
	try
	{
		DBLWrite::WriteData(pCopyData, m_lFileHeaderStart + sizeof(ts_DBFileHdr), pTempFile);
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"MoveChunkToFront - Write failed\r\n  %s",(const char *)(e)));
	}
	free(pCopyData);

	/* alignment-- have to hack to make it work, since AlignFile assumes it's working on m_pDBLFile */

//yuch-- since file header is collapsed, this is not necessary (and is a bug, actually, since the collapse is
// sizeof(ts_DBFileHdr) and if sizeof(ts_DBFileHdr) is not aligned we'll end up at the pad bytes...
//	pSaveDBLFile = m_pDBLFile;
//	m_pDBLFile = pTempFile;
//	if ((iReturnCode = AlignFile()) != PluginError::RETURN_SUCCESS)
//		return(iReturnCode);
//	m_pDBLFile = pSaveDBLFile;

	/* write out last chunk first */

	fseek(m_pDBLFile, lChunkStart, SEEK_SET);	// point to last chunk
	try
	{
		CopyChunk(pTempFile, m_pDBLFile, FALSE);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"MoveChunkToFront, last chunk, source 0x%x, dest 0x%x\r\n  %s", m_pDBLFile, pTempFile,(const char *)(e)));
	}

	/* and now the other chunks */
	fseek(m_pDBLFile, m_lFirstChunkStart, SEEK_SET);	// point to first chunk
	for (i = 0; i < (signed) m_CurrentFileHdr.u32Chunks - 1; i++)
	{
		try
		{
			CopyChunk(pTempFile, m_pDBLFile, FALSE);
		}
		catch (PluginException e)
		{
			throw (PluginException(ERROR_FATAL,"MoveChunkToFront, chunk %d, source 0x%x, dest 0x%x\r\n  %s",i, m_pDBLFile, pTempFile,(const char *)(e)));
		}
	}
	assert(lCurrentFileSize == ftell(pTempFile));

	/* and now copy the whole thing back into the original file-- i suppose i should just rename things... */

	fseek(m_pDBLFile, m_lFirstChunkStart, SEEK_SET);		// reset both files
	fseek(pTempFile, m_lFirstChunkStart, SEEK_SET);
	for (i = 0; i < (signed) m_CurrentFileHdr.u32Chunks; i++)
	{
		try
		{
			CopyChunk(m_pDBLFile, pTempFile, FALSE);
		}
		catch (PluginException e)
		{
			throw (PluginException(ERROR_FATAL,"MoveChunkToFront, final copy, chunk %d, source 0x%x, dest 0x%x\r\n  %s",i, m_pDBLFile, pTempFile,(const char *)(e)));
		}
	}

	/* make sure we're at the end of the database file */

	fseek(m_pDBLFile, 0, SEEK_END);

	/* done */

	fclose(pTempFile);
	remove("tempdbl.tmp");
}

/* copy a chunk from one file to another. source file's file pointer is at the chunk header */

void DBLUtil::CopyChunk(FILE *pDestFile, FILE *pSourceFile, bool bAddingChunk)
{
	long lFilePos;
	u32 u32ChunkSize;
	ts_DBBaseChunkHdr ChunkHeader;

	/* read header to determine size */

	lFilePos = ftell(pSourceFile);
	try
	{
		DBLWrite::ReadRecord(&ChunkHeader, pSourceFile);
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"CopyChunk Read Chunk Header Failed\r\n  %s",(const char *)(e)));
	}
	if (ChunkHeader.u16Flags & DBL_NAMED_CHUNK)
		u32ChunkSize = sizeof(ts_DBChunkHdr);	// includes chunk name
	else
		u32ChunkSize = sizeof(ts_DBBaseChunkHdr);
	fseek(pSourceFile, lFilePos, SEEK_SET);	// back to start of chunk

	/* allocate a buffer for reading in chunk */

	u32ChunkSize += ChunkHeader.u32Size;
	m_pCopyChunkBuffer = (char *) malloc(u32ChunkSize);
	if (m_pCopyChunkBuffer == NULL)
		throw(PluginException(ERROR_FATAL,"%s - %s","ERROR - CopyChunk Malloc Error on %d bytes",u32ChunkSize));

	/* copy chunk */

	try
	{
		DBLWrite::ReadData(m_pCopyChunkBuffer, u32ChunkSize, pSourceFile);	// read entire chunk
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"ERROR - CopyChunk Chunk Read Failed\r\n  %s",(const char *)(e)));
	}
	try
	{
		DBLWrite::WriteData(m_pCopyChunkBuffer, u32ChunkSize, pDestFile); // write entire chunk
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"ERROR - CopyChunk Chunk Write Failed\r\n  %s",(const char *)(e)));
	}

	/* adding a new chunk to file? (this routine is also used to move a chunk to the front of the .dbl) */

	if (bAddingChunk)
	{
		assert(pDestFile == m_pDBLFile);	// must be working on current dbl file
		m_CurrentFileHdr.u32Chunks++;

		/* new max chunk size? */

		if (ChunkHeader.u32Size > m_CurrentFileHdr.u32MaxChunkSize)
			m_CurrentFileHdr.u32MaxChunkSize = ChunkHeader.u32Size;
	}

	/* success */

	free(m_pCopyChunkBuffer);
	m_pCopyChunkBuffer = NULL;
}

/* find a chunk in the file */

bool DBLUtil::FindChunk(int nType, unsigned long *pulChunkStart)
{
	int i;
	long lFilePos;
	u32 u32ChunkSize;
	ts_DBBaseChunkHdr ChunkHeader;

	fseek(m_pDBLFile, m_lFirstChunkStart, SEEK_SET);	// point to first chunk
	for (i = 0; i < (signed) m_CurrentFileHdr.u32Chunks; i++)
	{
		lFilePos = ftell(m_pDBLFile);
		try
		{
			DBLWrite::ReadRecord(&ChunkHeader, m_pDBLFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"FindChunk - ReadRecord %d failed\r\n  %s",i,(const char *)(e)));
		}
		/* found it? */

		if (ChunkHeader.u16Type == nType)
		{
			*pulChunkStart = lFilePos;
			fseek(m_pDBLFile, 0, SEEK_END);	// back to end of file
			return true;
		}				
		/* nope, skip this chunk */

		if (ChunkHeader.u16Flags & DBL_NAMED_CHUNK)
			u32ChunkSize = sizeof(ts_DBChunkHdr);	// includes chunk name
		else
			u32ChunkSize = sizeof(ts_DBBaseChunkHdr);
		u32ChunkSize += ChunkHeader.u32Size;	// add data size to header size for total chunk size
		fseek(m_pDBLFile, lFilePos + u32ChunkSize, SEEK_SET);
	}

	/* not in file! */

	*pulChunkStart = FALSE;
	fseek(m_pDBLFile, 0, SEEK_END);	// back to end of file
	return false;
}


/* align file position to n-byte alignment from start of chunk data */

void DBLUtil::AlignFile(void)
{
	static u8 u8Filler[16] = {0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee};	// more visible in data than 0's
	unsigned long ulSize;
	unsigned long ulPosition = ftell(m_pDBLFile) - (unsigned) m_lChunkDataStart;

	assert(m_lChunkDataStart != 0);	// should only use this inside a chunk
	ulSize = (ALIGN_BYTES - (ulPosition & (ALIGN_BYTES - 1)));
	if (ulSize != ALIGN_BYTES)
	{
		try
		{
			DBLWrite::WriteData(u8Filler, ulSize, m_pDBLFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"AlignFile - WriteData failed\r\n  %s",(const char *)(e)));
		}
	}
}

/* pad file so that current file position is quad-word (16 bytes) aligned from start of chunk. since chunk is loaded into
a quad-word aligned buffer in engine, anything that is quad word aligned from start of chunk is quad word aligned in
memory. chunk header is collapsed during chunk loading, so actual start of chunk to consider here is chunk data start */

void DBLUtil::AlignChunkDataQuadWord(void)
{
	static u8 u8Filler[16] = {0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee};	// more visible in data than 0's
	unsigned long ulSize;
	unsigned long ulPosition = ftell(m_pDBLFile) - (unsigned) m_lChunkDataStart;

	ulSize = (16 - (ulPosition & (16 - 1)));
	if (ulSize != 16)
	{
		try
		{
			DBLWrite::WriteData(u8Filler, ulSize, m_pDBLFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"AlignChunkDataQuadWord - WriteData failed\r\n  %s",(const char *)(e)));
		}
	}
}

void DBLUtil::AlignChunkData32Byte(u8 padChar)
{
	u8 u8Filler[32];

	unsigned long ulSize;
	unsigned long ulPosition = ftell(m_pDBLFile) - (unsigned) m_lChunkDataStart;

	memset(u8Filler,padChar,32);
	ulSize = (32 - (ulPosition & (32 - 1)));
	if (ulSize != 32)
	{
		try
		{
			DBLWrite::WriteData(u8Filler, ulSize, m_pDBLFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"AlignChunkData32Byte - WriteData failed\r\n  %s",(const char *)(e)));
		}
	}
}

/* start a record list chunk */

void DBLUtil::StartRecordList(unsigned long ulRecordCount, unsigned short usFlags, unsigned long ulRecordSize)
{
	/* file should be byte-aligned at this point */

	if (!IS_ALIGNED(ftell(m_pDBLFile) - m_lChunkDataStart))	// if this hits, chunk header needs to be padded
		throw(PluginException(ERROR_FATAL,"StartRecordList - File not aligned"));

	/* every record list has at least the standard record list header */
	if (ulRecordCount <= 0)
		throw(PluginException(ERROR_FATAL,"StartRecordList - RecordCount <=0"));
	m_ulRecordListSize = sizeof(ts_RecordList);

	/* a variable-length record data chunk uses an offset table to point to the individual records. */
	/* header structure includes one-entry offset table, have to alloc the rest of the offset table entries */

	if (usFlags & RECORD_LIST_HAS_OFFSET_TABLE)
		m_ulRecordListSize += (ulRecordCount - 1) * sizeof(u32);

	/* quad-word alignment? */

	if (usFlags & RECORD_LIST_QUAD_WORD_ALIGNED)
		m_ulRecordListSize = (m_ulRecordListSize + 15) & ~15;	// pad out to quad-word (16-byte) boundary

	/* set up */

	if (m_pRecordList != NULL)	// if this hits, some other chunk failed to call FinishRecordList
		throw(PluginException(ERROR_FATAL,"StartRecordList - Chunk record list is not NULL"));

	m_pRecordList = (ts_RecordList *) malloc(m_ulRecordListSize);
	if (m_pRecordList == NULL)
		throw(PluginException(ERROR_FATAL,"StartRecordList - Malloc failed on %d bytes",m_ulRecordListSize));

#ifdef __DEBUG_DBL_ENABLE
	// entry for the record list
	m_recordListEntry = DBLDebug::GetEntryIndex();
#endif

	memset(m_pRecordList, 0, m_ulRecordListSize);
	m_ulRecordListEntryCount = 0;

	/* fill in stuff we know */

	if (ulRecordCount >= 65535)
		throw(PluginException(ERROR_FATAL,"StartRecordList - Record Count is too big at %d records",ulRecordCount));

	m_pRecordList->usRecordCount = (unsigned short) ulRecordCount;
	m_pRecordList->usFlags = usFlags;
	if (usFlags & RECORD_LIST_SAME_SIZE_RECORDS)
	{
		/* record size should be aligned */

		if (!IS_ALIGNED(ulRecordSize))	// if this hits, pad your data structure
			throw(PluginException(ERROR_FATAL,"StartRecordList - Record Size is not Aligned at %d size",ulRecordSize));
		m_pRecordList->u32RecordSize = ulRecordSize;
	}

	/* save table's file position, write out header, dummy table for record lists with an offset table */

	m_lRecordListStart = ftell(m_pDBLFile);

	try
	{
		DBLWrite::WriteRecord(m_pRecordList, m_pDBLFile);
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"StartRecordList - WriteRecordList failed\r\n  %s",(const char *)(e)));
	}
	u32 u32BytesWritten = sizeof(ts_RecordList);
	if (usFlags & RECORD_LIST_HAS_OFFSET_TABLE)
	{
		try
		{
			DBLWrite::WriteRecords((u32 *) ((char *) m_pRecordList + u32BytesWritten), m_pDBLFile, ulRecordCount - 1);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"StartRecordList - WriteRecords failed\r\n  %s",(const char *)(e)));
		}
		u32BytesWritten += (ulRecordCount - 1) * sizeof(u32);
	}
	if (u32BytesWritten != m_ulRecordListSize)	// any alignment padding to write out?
	{
		try
		{
			DBLWrite::WriteData((void *) ((char *) m_pRecordList + u32BytesWritten), m_ulRecordListSize - u32BytesWritten, m_pDBLFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"StartRecordList - WritePadding failed\r\n  %s",(const char *)(e)));
		}
	}

	/* alignment */

	if (!IS_ALIGNED(ftell(m_pDBLFile) - m_lChunkDataStart))	// if this hits, ts_RecordList needs to be padded, or offset table needs to be even number, or something
		throw(PluginException(ERROR_FATAL,"StartRecordList - RecordList needs to be padded."));
	try
	{
		AlignFile();
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"StartRecordList - Align file failed\r\n  %s",(const char *)(e)));
	}
}

/* write out completed record list */

void DBLUtil::FinishRecordList(void)
{
#ifdef __DEBUG_DBL_ENABLE
	int oldEntryIndex = DBLDebug::GetEntryIndex();
	DBLDebug::SetEntryIndex(m_recordListEntry);
#endif

	assert(m_pRecordList);

	/* all offset table entries should have been filled in */
	if (m_ulRecordListEntryCount != m_pRecordList->usRecordCount)
		throw(PluginException(ERROR_FATAL,"FinishRecordList - Record Count does not match"));

	/* if using an offset table, write it out */
	if (m_pRecordList->usFlags & RECORD_LIST_HAS_OFFSET_TABLE)
	{
		/* replace dummy offset table in file with completed table. the first table entry is in the ts_RecordList */
		/* structure, so write that out as well */

		fseek(m_pDBLFile, m_lRecordListStart, SEEK_SET);
		try
		{
			DBLWrite::WriteRecord(m_pRecordList, m_pDBLFile);
			DBLWrite::WriteRecords((u32 *) ((char *) m_pRecordList + sizeof(ts_RecordList)), m_pDBLFile, m_pRecordList->usRecordCount - 1);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"FinishRecordList - Record List Write Failed\r\n  %s",(const char *)(e)));
		}
		/* back to end of file */
		fseek(m_pDBLFile, 0, SEEK_END);
	}

	/* free table */
	free(m_pRecordList);
	m_pRecordList = NULL;

#ifdef __DEBUG_DBL_ENABLE
	DBLDebug::SetEntryIndex(oldEntryIndex);
#endif

}

/* record a new offset in the offset table */
void DBLUtil::RecordListEntry(void)
{
	/* maybe nothing to do */

	if (m_pRecordList == NULL)
		return;

	/* records should always be aligned */

	if (!IS_ALIGNED(ftell(m_pDBLFile) - m_lChunkDataStart))	// if this hits, you will need to pad out whatever data structure was written out last
		throw(PluginException(ERROR_FATAL,"RecordListEntry - Record List needs to be aligned"));
	try
	{
		AlignFile();
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"RecordListEntry - Align Error\r\n  %s",(const char *)(e)));
	}

	/* if record list uses an offset table, record file position */

	if (m_pRecordList->usFlags & RECORD_LIST_HAS_OFFSET_TABLE)
	{
		/* record offset */

		if (m_ulRecordListEntryCount >= m_pRecordList->usRecordCount)
			throw(PluginException(ERROR_FATAL,"RecordListEntry - Table Size Error"));
		m_pRecordList->u32OffsetTable[m_ulRecordListEntryCount] = ftell(m_pDBLFile) - m_lChunkDataStart;	// offset is from start of chunk to current position
	}

	/* another record added */
#ifdef __DEBUG_DBL_ENABLE
	DBLDebug::NextEntry();
	DBLDebug::SetIndex(m_ulRecordListEntryCount);
#endif

	m_ulRecordListEntryCount++;
}

/* this routine is called when a record is skipped-- that is, not exported. for example, when exporting SOM's, it is
hard to predict ahead of time which SOM's might be empty, and so not valid for export. so if such a SOM is found, this
routine is called so that the total record count is accurate. this leaves empty record list entries at the end of the
list, but that's a small waste */

void DBLUtil::SkipRecordListEntry(void)
{
	if(m_pRecordList->usRecordCount <=0)
		throw(PluginException(ERROR_FATAL,"SkipRecordListEntry - Record Count needs to be >0"));
	m_pRecordList->usRecordCount--;
}

/* handle chunk log */

void DBLUtil::ChunkLogReset(void)
{
	/* locked? */

	if (m_bChunkLogLocked)
		return;

	/* reset */

	m_u32ChunkCount = 0;
	m_u32ChunkTotalSize = 0;
	m_u32DBLMemoryFootprint = 0;
}

void DBLUtil::ChunkLogAddChunk(u32 u32Type, u32 u32Size, bool bMoveChunkToFront)
{
	bool bIncludeChunk;

	/* locked? */

	if (m_bChunkLogLocked)
		return;

	/* if this chunk was moved to front, then adjust other chunk's file position */

	if (bMoveChunkToFront)
	{
		for (int i = 0; i < (signed) m_u32ChunkCount; i++)
			m_chunkLog[i].u32FileOrder++;
	}

	/* add chunk */

	m_chunkLog[m_u32ChunkCount].u32ChunkType = u32Type;
	m_chunkLog[m_u32ChunkCount].u32ChunkDataSize = u32Size;
	m_chunkLog[m_u32ChunkCount].u32ChunkDataSubSize = 0;
	if (bMoveChunkToFront)
		m_chunkLog[m_u32ChunkCount].u32FileOrder = 0;	// chunk was moved to front of file
	else
		m_chunkLog[m_u32ChunkCount].u32FileOrder = m_u32ChunkCount;
	m_u32ChunkCount++;

	/* some chunks are transient-- don't include them in footprint */

	bIncludeChunk = FALSE;
// hack
	if (bIncludeChunk)
		m_u32DBLMemoryFootprint += u32Size;
}
void DBLUtil::ChunkLogSetSubSize(u32 u32Type, u32 u32SubSize)
{
	/* locked? */

	if (m_bChunkLogLocked)
		return;

	/* error check */

	assert(m_u32ChunkCount > 0);
	assert(m_chunkLog[m_u32ChunkCount - 1].u32ChunkType == u32Type);
	m_chunkLog[m_u32ChunkCount - 1].u32ChunkDataSubSize = u32SubSize;
}

int DBLUtil::ChunkLogSortCompare(const void *arg1, const void *arg2)
{
	/* largest chunks at the top of the list */

	return(((ChunkLog *) arg2)->u32ChunkDataSize - ((ChunkLog *) arg1)->u32ChunkDataSize);
}

void DBLUtil::ChunkLogSort(void)
{
	qsort(m_chunkLog, m_u32ChunkCount, sizeof(m_chunkLog), DBLUtil::ChunkLogSortCompare);
}

void DBLUtil::ChunkLogGetSize(u32 u32Index, u32 *pu32Size, u32 *pu32SubSize, u32 *pu32TotalSize)
{
	int i;

	/* calculate total size if not done yet */

	if (m_u32ChunkTotalSize == 0)
	{
		for (i = 0; i < (signed) m_u32ChunkCount; i++)
			m_u32ChunkTotalSize += m_chunkLog[i].u32ChunkDataSize;
	}

	/* fill in requested fields */

	assert(u32Index < m_u32ChunkCount);
	if (pu32Size)
		*pu32Size = m_chunkLog[u32Index].u32ChunkDataSize;
	if (pu32SubSize)
	{
		if (pu32Size)
			*pu32Size -= m_chunkLog[u32Index].u32ChunkDataSubSize;
		*pu32SubSize = m_chunkLog[u32Index].u32ChunkDataSubSize;
	}
	if (pu32TotalSize)
		*pu32TotalSize = m_u32ChunkTotalSize;
}
u32 DBLUtil::ChunkLogGetDBLMemoryFootprint(void)
{
	return(m_u32DBLMemoryFootprint);
}
void DBLUtil::ChunkLogLock(void)
{
	m_bChunkLogLocked = TRUE;
}
void DBLUtil::ChunkLogUnlock(void)
{
	m_bChunkLogLocked = FALSE;
}

/* print out a summary of chunks */

void DBLUtil::ChunkLogShowSummary(void)
{
	int i;
	u32 u32Size;
	u32 u32SubSize;	// chunks can have a single "sub-portion" of data-- that is, data that may be significant size, like vertex colors portion of instance chunk
	u32 u32TotalSize;
	char acChunkName[100];
	char acMessage[400];

	DBLUtil::ChunkLogSort();	// sort chunks by size
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Chunk type\t\t\tKBytes\t\t\tPercent\t\tFile Order");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"------------------\t\t\t------------\t\t-------------\t\t----------------");
	for (i = 0; i < (signed) m_u32ChunkCount; i++)
	{
		u32SubSize = 0;
		switch(m_chunkLog[i].u32ChunkType)
		{
		case DB_SNGLSOM:
		case DB_SNGLSOMBONE:
		case DB_SOMARRAY:
		case DB_SOMARRAYBONE:
			strcpy(acChunkName, "SOM (models)\t");
			break;
		case DB_DMASOMARRAY:
		case DB_DMASNGLSOM:
			strcpy(acChunkName, "DMA SOMs\t\t");
			break;
//		case DB_ENV_INSTANCE:
//			strcpy(acChunkName, "Env. AI\t\t\t\t");
//			break;
		case DB_EXPANSION_TABLE: 
			strcpy(acChunkName, "Expansion table");
			break;
		case DB_GROUP:
			strcpy(acChunkName, "Group\t\t\t\t");
			break;
		case DB_INSTANCE:
			strcpy(acChunkName, "Instance data\t");
			break;
		case DB_LOD:
			strcpy(acChunkName, "LOD\t\t\t\t\t");
			break;
		case DB_LIGHTS:
			strcpy(acChunkName, "Lights\t\t\t\t");
			break;
		case DB_NAMED_POINTS:
			strcpy(acChunkName, "Named points\t");
			break;
		case DB_PATHS:
			strcpy(acChunkName, "Paths\t\t\t\t");
			break;
		case DB_TEXTURE_CONTEXT:
			strcpy(acChunkName, "Texture context");
			break;
		case DB_SECTORS:
			strcpy(acChunkName, "Sectors\t\t\t\t");
			break;
		case DB_TEXTURE_SET:
			strcpy(acChunkName, "Texture\t\t\t\t");
			break;
		case DB_ANIMATED_TEXTURE_SET:
			strcpy(acChunkName, "Anim. texture\t");
			break;
		case DB_MERGED_TEXTURE_SET:
			strcpy(acChunkName, "Merged texture");
			break;
		case DB_TERRAIN_LIST:
			strcpy(acChunkName, "Terrain list\t\t");
			break;
		case DB_WORLD_EMITTERS:
			strcpy(acChunkName, "World emitters\t\t");
			break;
		case DB_WORLD_DISTORTIONS:
			strcpy(acChunkName, "World distorts\t");
			break;
		case DB_PARTICLE_EMITTER:
			strcpy(acChunkName, "Emitters\t\t\t");
			break;
		case DB_PARTICLE_SOURCE:
			strcpy(acChunkName, "Particles\t\t\t");
			break;
		case DB_PARTICLE_EFFECT:
			strcpy(acChunkName, "Effects\t\t\t\t");
			break;
		case DB_EMITTER_LIST:
			strcpy(acChunkName, "Emitter names\t");
			break;
		case DB_EFFECT_LIST:
			strcpy(acChunkName, "Effect names\t");
			break;
		case DB_BONEINFO:
			strcpy(acChunkName, "Bone Info\t");
			break;
		case DB_SCENE_TREE:
			strcpy(acChunkName, "Scene tree\t\t");
			break;
		default:
			assert(FALSE);
			strcpy(acChunkName, "Unknown\t\t\t");
			break;
		}
		DBLUtil::ChunkLogGetSize(i, &u32Size, &u32SubSize, &u32TotalSize);
		sprintf(acMessage, "%s\t%06.1f\t\t\t%04.1f\t\t\t\t%2d",
				  acChunkName,
				  (float) u32Size / 1024.0f,
				  (float) u32Size * 100.0f / (float) u32TotalSize,
				  m_chunkLog[i].u32FileOrder);

		/* possible postpend */

		switch(m_chunkLog[i].u32ChunkType)
		{
		case DB_SNGLSOM:
			strcat(acMessage, "\t\t\t\t\t(Type: Single SOM)");
			break;
		case DB_SNGLSOMBONE:
			strcat(acMessage, "\t\t\t\t\t(Type: Single SOM with bones)");
			break;
		case DB_SOMARRAY:
			strcat(acMessage, "\t\t\t\t\t(Type: SOM Array)");
			break;
		case DB_SOMARRAYBONE:
			strcat(acMessage, "\t\t\t\t\t(Type: SOM Array with bones)");
			break;
		}

		/* write it */

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,acMessage);

		/* may want to see size adjust portion too */

		if (u32SubSize != 0)
		{
			switch(m_chunkLog[i].u32ChunkType)
			{
			case DB_INSTANCE:
				strcpy(acChunkName, "Vertex colors\t");
				break;
			default:
				assert(FALSE);
				strcpy(acChunkName, "Unknown\t\t\t");
				break;
			}
			sprintf(acMessage, "%s\t%06.1f\t\t\t%04.1f\t\t\t\tSub-portion",
					  acChunkName,
					  (float) u32SubSize / 1024.0f,
					  (float) u32SubSize * 100.0f / (float) u32TotalSize);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,acMessage);
		}
	}
}