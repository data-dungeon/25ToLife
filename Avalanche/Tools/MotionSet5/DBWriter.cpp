#include "Stdafx.h"
#include "DBWriter.h"

#define NULL_HANDLE ((void *)-1)

#define BLOCK_HANDLE_OFFSET 0x10000000
#define MAKE_BLOCK_HANDLE(d) ((BLOCK_HANDLE)(d+BLOCK_HANDLE_OFFSET))
#define GET_BLOCK_INDEX(d) (((unsigned long)d)-BLOCK_HANDLE_OFFSET)

#define LINK_FROM_HANDLE_OFFSET 0x20000000
#define MAKE_LINK_FROM_HANDLE(d) ((LINK_FROM_HANDLE)(d+LINK_FROM_HANDLE_OFFSET))
#define GET_LINK_FROM_INDEX(d) (((unsigned long)d)-LINK_FROM_HANDLE_OFFSET)

#define LINK_TO_HANDLE_OFFSET 0x30000000
#define MAKE_LINK_TO_HANDLE(d) ((LINK_TO_HANDLE)(d+LINK_TO_HANDLE_OFFSET))
#define GET_LINK_TO_INDEX(d) (((unsigned long)d)-LINK_TO_HANDLE_OFFSET)

DBWriter::DBWriter(void) :
	m_chunkDataStart(0),
	m_pFile(NULL),
	m_bBlockActive(false)
{
}

FILE *DBWriter::GetDBLFile(void)
{
	return m_pFile;
}

long DBWriter::GetChunkDataStart(void)
{
	return m_chunkDataStart;
}

// open database file, write out header 
void DBWriter::StartDBLFile(const char *pFileName,const char *pText,bool bGenericData,const char *pExportMode)
{
	memset(&m_currentFileHeader,0,sizeof(m_currentFileHeader));

	/* save passed file name */
	m_fileName = pFileName;

	/* open file for writing */

	m_pFile = fopen(pFileName, "w+b");	// open for writing and reading
	if (m_pFile == NULL)
	{
		throw(Exception(ERROR_FATAL,"Unable to open %s file for writing",pFileName));
	}

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
				WriteData(cSkipCount, 8);
				WriteData(cFileName, strlen(cFileName));
				WriteData(cExportMode, strlen(cExportMode));
				WriteData(pText, nTextCount);
				WriteData(cPad, nPadCount);
			}
			catch (Exception e)
			{
				throw(Exception(ERROR_FATAL,"StartDBLFile - Name Write Failed\n  %s",(const char *)(e)));
			}
		}
	}

	/* fill in file header and write out */

	strcpy(m_currentFileHeader.cSkipCount, "0");	// atoi's to 0. see comments in _dblGetFileHdr in DBFile.cpp
	if (pExportMode)
		strcpy(m_currentFileHeader.cID, pExportMode);
	else
		strcpy(m_currentFileHeader.cID, "DB");	// unknown
	if (bGenericData)
		strcpy(m_currentFileHeader.cID, "DB");	// some data (e.g. collision data) is not meant to be run by game engine

	m_currentFileHeader.u32Version = DB_VERSION;
	m_currentFileHeader.u32FileFlags = 0;
	m_currentFileHeader.u32Chunks = 0;
	m_currentFileHeader.u32MaxChunkSize = 0;
	m_fileHeaderStart = ftell(m_pFile);	// record file position so i can go back and overwrite it later
	try
	{
		WriteData(&m_currentFileHeader,sizeof(m_currentFileHeader));
	}
	catch (Exception e)
	{
		throw(Exception(ERROR_FATAL,"StartDBLFile - Header Write Failed\n  %s",(const char *)(e)));
	}

	/* alignment */

	//yuch-- since file header is collapsed, this is not necessary (and is a bug, actually, since the collapse is
	// sizeof(ts_DBFileHdr) and if sizeof(ts_DBFileHdr) is not aligned we'll end up at the pad bytes...
	//	if ((iReturnCode = AlignFile()) != PluginError::RETURN_SUCCESS)
	//		return(iReturnCode);

	/* record where first chunk starts */

	m_firstChunkStart = ftell(m_pFile);
}

/* update database header, close database */

u32 DBWriter::FinishDBLFile(void)
{
	/* maybe was never opened */

	if (m_pFile == NULL)
		assert(FALSE);

	/* update database header */

	fseek(m_pFile, m_fileHeaderStart, SEEK_SET);
	try
	{
		WriteData(&m_currentFileHeader,sizeof(m_currentFileHeader));
	}
	catch (Exception e)
	{
		throw(Exception(ERROR_FATAL,"FinishDBLFile - Header Write Failed\n  %s",(const char *)(e)));
	}

	/* close */
	fseek(m_pFile, 0, SEEK_END);
	u32 fileSize = ftell(m_pFile);

	fclose(m_pFile);
	m_pFile = NULL;
	return fileSize;
}

/* clean up. must be able to be called multiple times with no harm done */

void DBWriter::CleanUpDBLFile(void)
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;				// so subsequent calls don't do anything
	}
}

/* start of a chunk-- pcName can be NULL for unnamed chunks */
void DBWriter::StartChunk(u16 u16Type, u16 u16Version, u16 u16DBFlags, const char *pcName)
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

	m_chunkHeaderStart = ftell(m_pFile);

	/* write chunk header */

	memset(&m_currentChunkHeader,0,sizeof(m_currentChunkHeader));

	m_currentChunkHeader.u16Type = u16Type;
	m_currentChunkHeader.u16Version = u16Version;
	m_currentChunkHeader.u16Flags = u16DBFlags;
	m_currentChunkHeader.u32Size = 0;

	if (pcName)
	{
		strncpy(m_currentChunkHeader.cName, pcName, DBL_MAXNAME);
		try
		{
			WriteData(&m_currentChunkHeader,sizeof(m_currentChunkHeader));
		}
		catch (Exception e)
		{
			throw(Exception(ERROR_FATAL,"StartChunk - Header Write Failed\n   %s",(const char *)(e)));
		}
	}
	else
	{
		try
		{
			WriteData(&m_currentChunkHeader,sizeof(ts_DBBaseChunkHdr));
		}
		catch (Exception e)
		{
			throw(Exception(ERROR_FATAL,"StartChunk - Header Write Failed\n  %s",(const char *)(e)));
		}
	}

	/* record start of chunk data so i can calculate chunk size */
	m_chunkDataStart = ftell(m_pFile);

	/* ok */

	fflush( m_pFile);

}

/* update chunk header, global chunk count */
/*	Added bLogChunk to prevent chunks being counted twice when being
overwritten. (ie: for the DMA SOM export) 
bLogChunk defaults to "true"*/

u32 DBWriter::FinishChunk(void)
{
	/* we align all chunks to a multiple of 32 bytes long. this supports the gamecube, where 32-byte alignment */
	/* is important, without really any cost for other platforms. this way, all chunks in .dbl file will */
	/* be 32-byte aligned when loaded into memory, whether or not chunks get collapsed away during processing */

	AlignFile32Byte();

	/* store chunk size in chunk header */

	m_currentChunkHeader.u32Size = ftell(m_pFile) - m_chunkDataStart;
	fseek(m_pFile, m_chunkHeaderStart, SEEK_SET);
	try
	{
		WriteData(&m_currentChunkHeader,sizeof(m_currentChunkHeader));
	}
	catch (Exception e)
	{
		throw(Exception(ERROR_FATAL,"FinishChunk - Header Write Failed\n  %s",(const char *)(e)));
	}
	//	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"  Chunk data size %d bytes", m_currentChunkHeader.u32Size);

	/* another chunk added */
	m_currentFileHeader.u32Chunks++;
	if (m_currentChunkHeader.u32Size > m_currentFileHeader.u32MaxChunkSize)
		m_currentFileHeader.u32MaxChunkSize = m_currentChunkHeader.u32Size;

	bool bError = false;

	// connect up links in file
	int numFromLinks = m_linkFromArray.GetSize();
	for (int linkIndex=0;linkIndex<numFromLinks;linkIndex++)
	{
		LinkFrom &linkFrom = m_linkFromArray[linkIndex];
		if (linkFrom.m_linkToHandle!=NULL_HANDLE)
		{
			// seek to location to write link
			int linkFromBlockIndex = GET_BLOCK_INDEX(linkFrom.m_blockHandle);
			assert(linkFromBlockIndex>=0 && linkFromBlockIndex<m_blocks.GetSize());
			Block &linkFromBlock = m_blocks[linkFromBlockIndex];
			assert(linkFrom.m_offset>=0 && linkFrom.m_offset<linkFromBlock.m_size);
			int sourceOffset = m_blocks[linkFromBlockIndex].m_filePos+linkFrom.m_offset;
			fseek(m_pFile,sourceOffset, SEEK_SET);

			// get offset for link
			int linkToIndex = GET_LINK_TO_INDEX(linkFrom.m_linkToHandle);
			assert(linkToIndex>=0 && linkToIndex<m_linkToArray.GetSize());
			LinkTo &linkTo = m_linkToArray[linkToIndex];
			int linkToBlockIndex = GET_BLOCK_INDEX(linkTo.m_blockHandle);
			assert(linkToBlockIndex>=0 && linkToBlockIndex<m_blocks.GetSize());
			Block &linkToBlock = m_blocks[linkToBlockIndex];
			assert(linkTo.m_offset>=0 && linkTo.m_offset<linkToBlock.m_size);
			int offsetToWrite = ((linkToBlock.m_filePos + linkTo.m_offset) - sourceOffset) - 1;
			try
			{
				WriteData(&offsetToWrite,sizeof(offsetToWrite));
			}
			catch (Exception e)
			{
				throw(Exception(ERROR_FATAL,"FinishChunk link %s->%s Write Failed\n  %s",
					(const char *)linkFrom.m_name,(const char *)linkTo.m_name,(const char *)(e)));
			}
		}
		else
		{
			int linkFromBlockIndex = GET_BLOCK_INDEX(linkFrom.m_blockHandle);
			assert(linkFromBlockIndex>=0 && linkFromBlockIndex<m_blocks.GetSize());
			Block &linkFromBlock = m_blocks[linkFromBlockIndex];
			printf("Error in FinishChunk, link '%s' from block '%s' has no connection\n",(const char *)linkFrom.m_name,(const char *)linkFromBlock.m_name);
			bError = true;
		}
	}

	if (bError)
		throw(Exception(ERROR_FATAL,"Error(s) in FinishChunk\n"));

	// done with blocks and links
	m_blocks.RemoveAll();
	m_linkFromArray.RemoveAll();
	m_linkToArray.RemoveAll();


	/* back to end of file */

	fseek(m_pFile, 0, SEEK_END);

	/* we are not within chunk start/finish bookmarks */

	m_chunkDataStart = 0;

	return m_currentChunkHeader.u32Size;
}

/* align file position to n-byte alignment from start of chunk data */

void DBWriter::AlignFile(void)
{
	static u8 u8Filler[16] = {0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee};	// more visible in data than 0's
	unsigned long ulSize;
	unsigned long ulPosition = ftell(m_pFile) - (unsigned) m_chunkDataStart;

	assert(m_chunkDataStart != 0);	// should only use this inside a chunk
	ulSize = (ALIGN_BYTES - (ulPosition & (ALIGN_BYTES - 1)));
	if (ulSize != ALIGN_BYTES)
	{
		try
		{
			WriteData(u8Filler, ulSize);
		}
		catch (Exception e)
		{
			throw(Exception(ERROR_FATAL,"AlignFile - WriteData failed\n  %s",(const char *)(e)));
		}
	}
}

/* pad file so that current file position is quad-word (16 bytes) aligned from start of chunk. since chunk is loaded into
a quad-word aligned buffer in engine, anything that is quad word aligned from start of chunk is quad word aligned in
memory. chunk header is collapsed during chunk loading, so actual start of chunk to consider here is chunk data start */

void DBWriter::AlignFileQuadWord(void)
{
	static u8 u8Filler[16] = {0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee};	// more visible in data than 0's
	unsigned long ulSize;
	unsigned long ulPosition = ftell(m_pFile) - (unsigned) m_chunkDataStart;

	ulSize = (16 - (ulPosition & (16 - 1)));
	if (ulSize != 16)
	{
		try
		{
			WriteData(u8Filler, ulSize);
		}
		catch (Exception e)
		{
			throw(Exception(ERROR_FATAL,"AlignFileQuadWord - WriteData failed\n  %s",(const char *)(e)));
		}
	}
}

void DBWriter::AlignFile32Byte(u8 padChar)
{
	u8 u8Filler[32];

	unsigned long ulSize;
	unsigned long ulPosition = ftell(m_pFile) - (unsigned) m_chunkDataStart;

	memset(u8Filler,padChar,32);
	ulSize = (32 - (ulPosition & (32 - 1)));
	if (ulSize != 32)
	{
		try
		{
			WriteData(u8Filler, ulSize);
		}
		catch (Exception e)
		{
			throw(Exception(ERROR_FATAL,"AlignFile32Byte - WriteData failed\n  %s",(const char *)(e)));
		}
	}
}

void DBWriter::WriteData(const void *pData, u32 u32Length)
{
	if (fwrite(pData, u32Length, 1,  m_pFile) != 1)
		throw(Exception(ERROR_FATAL,"WriteData - fwrite error"));
}


BLOCK_HANDLE DBWriter::StartBlock(const char *pFormatString,...)
{
	static char acMessage[800];
	va_list PrintArgs;

	/* create message */

	va_start(PrintArgs, pFormatString);
	vsprintf(acMessage, pFormatString, PrintArgs);
	va_end(PrintArgs);

	assert(!m_bBlockActive);

	m_bBlockActive = true;
	Block newBlock;
	newBlock.m_filePos = ftell(m_pFile);
	newBlock.m_name = acMessage;
	newBlock.m_size = 0;
	m_blocks.Add(newBlock);
	return MAKE_BLOCK_HANDLE(m_blocks.GetSize()-1);
}

void DBWriter::EndBlock(void)
{
	assert(m_bBlockActive);

	m_bBlockActive = false;
	Block &block = m_blocks[m_blocks.GetSize()-1];
	block.m_size = ftell(m_pFile)-block.m_filePos;
}

u32 DBWriter::GetBlockSize(BLOCK_HANDLE blockHandle)
{
	u32 size=0;

	int blockIndex = GET_BLOCK_INDEX(blockHandle);
	assert(blockIndex>=0 && blockIndex<m_blocks.GetSize());
	Block &block = m_blocks[blockIndex];
	// if we are still active and last block
	if (m_bBlockActive && blockIndex==(m_blocks.GetSize()-1))
	{
		size = ftell(m_pFile)-block.m_filePos;
	}
	else
	{
		size = block.m_size;
	}
	return size;
}

LINK_FROM_HANDLE DBWriter::RegisterLinkFrom(BLOCK_HANDLE blockHandle,u32 offset,const char *pFormatString,...)
{
	assert(m_bBlockActive);

	static char acMessage[800];
	va_list PrintArgs;

	/* create message */

	va_start(PrintArgs, pFormatString);
	vsprintf(acMessage, pFormatString, PrintArgs);
	va_end(PrintArgs);

	LinkFrom linkFrom;
	linkFrom.m_blockHandle = MAKE_BLOCK_HANDLE(m_blocks.GetSize()-1);
	linkFrom.m_linkToHandle = NULL_HANDLE;
	linkFrom.m_name = acMessage;
	linkFrom.m_offset = offset;
	m_linkFromArray.Add(linkFrom);
	return MAKE_LINK_FROM_HANDLE(m_linkFromArray.GetSize()-1);
}

LINK_TO_HANDLE DBWriter::RegisterLinkTo(BLOCK_HANDLE recordHandle,u32 offset,const char *pFormatString,...)
{
	assert(m_bBlockActive);

	static char acMessage[800];
	va_list PrintArgs;

	/* create message */

	va_start(PrintArgs, pFormatString);
	vsprintf(acMessage, pFormatString, PrintArgs);
	va_end(PrintArgs);

	LinkTo linkTo;
	linkTo.m_blockHandle = MAKE_BLOCK_HANDLE(m_blocks.GetSize()-1);
	linkTo.m_name = acMessage;
	linkTo.m_offset = offset;
	m_linkToArray.Add(linkTo);
	return MAKE_LINK_TO_HANDLE(m_linkToArray.GetSize()-1);
}

void DBWriter::ConnectLinks(LINK_FROM_HANDLE linkFromHandle,LINK_TO_HANDLE linkToHandle)
{
	int linkFromIndex = GET_LINK_FROM_INDEX(linkFromHandle);
	int linkToIndex = GET_LINK_TO_INDEX(linkToHandle);
	assert(linkFromIndex>=0 && linkFromIndex<m_linkFromArray.GetSize());
	assert(linkToIndex>=0 && linkToIndex<m_linkToArray.GetSize());
	LinkFrom &linkFrom = m_linkFromArray[linkFromIndex];
	assert(linkFrom.m_linkToHandle==NULL_HANDLE);
	linkFrom.m_linkToHandle = linkToHandle;
}

