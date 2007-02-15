//
// GSMemManager.cpp
// Copyright (C) 2001, Avalanche Software Inc.
//
// Author: Tyler Colbert
//
// Description:  This module is a 'simple' memory manager for
// PS2 GS Local Memory.  The manager deals with alloc & free
// style allocations (thus simple) in the very complex memory
// layout of the GS.  It uses one word of bits for each page
// of memory.  Thats one bit per block (the smallest transfer
// unit on the GS).  That comes to 512 words of EE core memory
// hopefully not too intrusive :)...  It is completely static
// to help avoid passing a this pointer around and save some
// indirection.  It would not really serve any purpose to be
// able to create more than one anyway.
//
// It does have the one cavate that you must remember how much
// memory you allocated so that you can pass it to the free
// function.  This helps to reduce the EE memory overhead a
// little.  And is usually not too much to ask of the caller
// since they can usually recalculate or store it better than
// this would be able to anyway.
//
// It supports both page based allocations (for frame buffers,
// zbuffers, etc) and block based allocations (for textures,
// palettes, etc).
//
// the alloc functions return 0xFFFFFFFF on out of memory.
// the free functions assert that the memory you are freeing
// is really allocated.
// 

/******************* includes ****************************/

#include "Display/DisplayPCH.h"


/* ImportExport uses this as well and therefore needs to find the header... */
#if defined(IMPORT_EXPORT) || defined(PIGPEN) || ( defined(PS2) && defined(SN) )	|| defined(WIN32_TOOLS_DEBUG) || defined(WIN32_TOOLS_RELEASE)

/* system includes */

#include <string.h> // for strlen & memset

/* engine includes */

#include "Display/PS2/PSDRenderState.h" // for RS_TEX_PSM*
#include "GSMemManager.h"
#include "Layers/Assert/Assert.h"

#ifndef SHIP_IT
#include "stdio.h"
#endif

#endif // defined(PS2) && defined(SN)



u32 GSMemManager::memoryMap[GS_MEM_NUM_PAGES];

static const u8 blocksPSMCT16[][4] = 
{
	{  0,  2,  8, 10 },
	{  1,  3,  9, 11 },
	{  4,  6, 12, 14 },
	{  5,  7, 13, 15 },
	{ 16, 18, 24, 26 },
	{ 17, 19, 25, 27 },
	{ 20, 22, 28, 30 },
	{ 21, 23, 29, 31 },
};
static const u8 blocksPSMCT16S[][4] = 
{
	{  0,  2, 16, 18 },
	{  1,  3, 17, 19 },
	{  8, 10, 24, 26 },
	{  9, 11, 25, 27 },
	{  4,  6, 20, 22 },
	{  5,  7, 21, 23 },
	{ 12, 14, 28, 30 },
	{ 13, 15, 29, 31 },
};
static const u8 blocksPSMZ16[][4] = 
{
	{ 24, 26, 16, 18 },
	{ 25, 27, 17, 19 },
	{ 28, 30, 20, 22 },
	{ 29, 31, 21, 23 },
	{  8, 10,  0,  2 },
	{  9, 11,  1,  3 },
	{ 12, 14,  4,  6 },
	{ 13, 15,  5,  7 },
};
static const u8 blocksPSMZ16S[][4] = 
{
	{ 24, 26,  8, 10 },
	{ 25, 27,  9, 11 },
	{ 16, 18,  0,  2 },
	{ 17, 19,  1,  3 },
	{ 28, 30, 12, 14 },
	{ 29, 31, 13, 15 },
	{ 20, 22,  4,  6 },
	{ 21, 23,  5,  7 },
};
// this set works for PSMCT32 & PSMCT24
static const u8 blocksPSMCT32[][8] = 
{
	{  0,  1,  4,  5, 16, 17, 20, 21 },
	{  2,  3,  6,  7, 18, 19, 22, 23 },
	{  8,  9, 12, 13, 24, 25, 28, 29 },
	{ 10, 11, 14, 15, 26, 27, 30, 31 },
};
// this set works for PSMZ32 & PSMZ24
static const u8 blocksPSMZ32[][8] = 
{
	{ 24, 25, 28, 29,  8,  9, 12, 13 },
	{ 26, 27, 30, 31, 10, 11, 14, 15 },
	{ 16, 17, 20, 21,  0,  1,  4, 05 },
	{ 18, 19, 22, 23,  2,  3,  6, 07 },
};
static const u8 blocksPSMT8[][8] = 
{
	{  0,  1,  4,  5, 16, 17, 20, 21 },
	{  2,  3,  6,  7, 18, 19, 22, 23 },
	{  8,  9, 12, 13, 24, 25, 28, 29 },
	{ 10, 11, 14, 15, 26, 27, 30, 31 },
};
static const u8 blocksPSMT4[][4] = 
{
	{  0,  2,  8, 10 },
	{  1,  3,  9, 11 },
	{  4,  6, 12, 14 },
	{  5,  7, 13, 15 },
	{ 16, 18, 24, 26 },
	{ 17, 19, 25, 27 },
	{ 20, 22, 28, 30 },
	{ 21, 23, 29, 31 },
};

void GSMemManager::Reset(void)
{
	memset(memoryMap, 0x00, sizeof(memoryMap));
}

void GSMemManager::ReservePages(u32 startPage, u32 numPages)
{
	u32 currentPage = startPage;
	u32 endPage = startPage + numPages;
	for(;currentPage < endPage;currentPage++)
	{
		// this assert means that at least one of the the pages
		// requested is already in use.
		ASSERT(memoryMap[currentPage] == 0);

		// mark the entire page allocated.
		memoryMap[currentPage] = 0xFFFFFFFF;
	}
}

void GSMemManager::GetAllocationInfo(u32 pixelsWide, u32 pixelsTall, u32 bufferWidth, u32 psmFormat, bool pageAligned, allocationInfo& allocInfo)
{
	u32 blockWidthInPixels = 0;
	u32 blockHeightInPixels = 0;
	const u8* blockTable = NULL;

	ASSERT(pixelsWide != 0);
	ASSERT(pixelsTall != 0);
	ASSERT(bufferWidth != 0);

	allocInfo.bufferWidth = bufferWidth;

	switch(psmFormat)
	{
	case RS_TEX_PSMCT32:
	case RS_TEX_PSMCT24:
	case RS_TEX_PSMT8H:
	case RS_TEX_PSMT4HL:
	case RS_TEX_PSMT4HH:
		blockWidthInPixels = 8;
		blockHeightInPixels = 8;
		allocInfo.pageWidthInBlocks = 8;
		allocInfo.pageHeightInBlocks = 4;
		blockTable = &blocksPSMCT32[0][0];
		break;
	case RS_TEX_PSMCT16:
		blockWidthInPixels = 16;
		blockHeightInPixels = 8;
		allocInfo.pageWidthInBlocks = 4;
		allocInfo.pageHeightInBlocks = 8;
		blockTable = &blocksPSMCT16[0][0];
		break;
	case RS_TEX_PSMCT16S:
		blockWidthInPixels = 16;
		blockHeightInPixels = 8;
		allocInfo.pageWidthInBlocks = 4;
		allocInfo.pageHeightInBlocks = 8;
		blockTable = &blocksPSMCT16S[0][0];
		break;
	case RS_TEX_PSMT8:
		blockWidthInPixels = 16;
		blockHeightInPixels = 16;
		allocInfo.pageWidthInBlocks = 8;
		allocInfo.pageHeightInBlocks = 4;
		blockTable = &blocksPSMT8[0][0];
		break;
	case RS_TEX_PSMT4:
		blockWidthInPixels = 32;
		blockHeightInPixels = 16;
		allocInfo.pageWidthInBlocks = 4;
		allocInfo.pageHeightInBlocks = 8;
		blockTable = &blocksPSMT4[0][0];
		break;
	case RS_TEX_PSMZ32:
	case RS_TEX_PSMZ24:
		blockWidthInPixels = 8;
		blockHeightInPixels = 8;
		allocInfo.pageWidthInBlocks = 8;
		allocInfo.pageHeightInBlocks = 4;
		blockTable = &blocksPSMZ32[0][0];
		break;
	case RS_TEX_PSMZ16:
		blockWidthInPixels = 16;
		blockHeightInPixels = 8;
		allocInfo.pageWidthInBlocks = 4;
		allocInfo.pageHeightInBlocks = 8;
		blockTable = &blocksPSMZ16[0][0];
		break;
	case RS_TEX_PSMZ16S:
		blockWidthInPixels = 16;
		blockHeightInPixels = 8;
		allocInfo.pageWidthInBlocks = 4;
		allocInfo.pageHeightInBlocks = 8;
		blockTable = &blocksPSMZ16S[0][0];
		break;
	default:
		ASSERT(false); // unsupported format
	}

	allocInfo.blocksWide = (pixelsWide + blockWidthInPixels - 1) / blockWidthInPixels;
	allocInfo.blocksTall = (pixelsTall + blockHeightInPixels - 1) / blockHeightInPixels;
	allocInfo.bufferWidth = (allocInfo.bufferWidth + blockWidthInPixels - 1) / blockWidthInPixels;
	allocInfo.bufferWidth = (allocInfo.bufferWidth + allocInfo.pageWidthInBlocks - 1) / allocInfo.pageWidthInBlocks;

	allocInfo.pagesWide = (allocInfo.blocksWide + allocInfo.pageWidthInBlocks - 1) / allocInfo.pageWidthInBlocks;
	allocInfo.pagesTall = (allocInfo.blocksTall + allocInfo.pageHeightInBlocks - 1) / allocInfo.pageHeightInBlocks;

	if(allocInfo.blocksWide > allocInfo.pageWidthInBlocks) allocInfo.blocksWide = allocInfo.pageWidthInBlocks;
	if(allocInfo.blocksTall > allocInfo.pageHeightInBlocks) allocInfo.blocksTall = allocInfo.pageHeightInBlocks;

	// can we simply allocate this image using a page allocation?
	if(allocInfo.blocksWide == allocInfo.pageWidthInBlocks && allocInfo.blocksTall == allocInfo.pageHeightInBlocks)
	{
		// uses page based allocation, we dont need these params
		allocInfo.blockBitPattern = 0xFFFFFFFF;
		allocInfo.maxOffset = 0;

		return;
	}

	// generate the bit pattern...
	allocInfo.blockBitPattern = 0x00000000;
	u32 i, j;
	u32 highestBlockUsed = 0;
	for(i = 0; i < allocInfo.blocksWide; i++)
	{
		for(j = 0; j < allocInfo.blocksTall; j++)
		{
			u8 blockNumber = blockTable[i + j * allocInfo.pageWidthInBlocks];
			allocInfo.blockBitPattern |= (0x1 << blockNumber);
			if(highestBlockUsed < blockNumber)
				highestBlockUsed = blockNumber;
		}
	}

	allocInfo.maxOffset = 0;
	if(!pageAligned)
	{
		allocInfo.maxOffset = 31 - highestBlockUsed;
	}
}

u32 GSMemManager::BlockAlloc(const allocationInfo &allocInfo)
{
	// this deals with the non-trivial case of bufferWidth != numPagesWide
	u32 currentPage;
	u32 endPage = GS_MEM_NUM_PAGES - allocInfo.bufferWidth * allocInfo.pagesTall;
	u32 currCheckPage;
	u32 numPagesToScan = allocInfo.pagesTall * allocInfo.bufferWidth;

	for(currentPage = 0;currentPage <= endPage;currentPage++)
	{
		u32 bitPattern = allocInfo.blockBitPattern;
		for(u32 currentOffset = 0; currentOffset <= allocInfo.maxOffset; currentOffset++, bitPattern <<= 1)
		{
			if((memoryMap[currentPage] & bitPattern) != 0x00000000)
			{
				continue;
			}

			for(currCheckPage = 0; currCheckPage < numPagesToScan; currCheckPage++)
			{
				// skip pages that wont be used in the buffer
				if(currCheckPage % allocInfo.bufferWidth >= allocInfo.pagesWide)
					continue;

				if((memoryMap[currentPage + currCheckPage] & bitPattern) != 0x00000000)
					break;
			}

			if(currCheckPage == numPagesToScan)
			{
				// we found an appropriate block
				// lets reserve it and return it.
				for(currCheckPage = 0; currCheckPage < numPagesToScan; currCheckPage++)
				{
					// skip pages that wont be used in the buffer
					if(currCheckPage % allocInfo.bufferWidth >= allocInfo.pagesWide)
						continue;
		
					memoryMap[currentPage + currCheckPage] |= bitPattern;
				}
				return currentPage * GS_MEM_BLOCKS_PER_PAGE + currentOffset;
			}
		}
	}

	// didnt find any place to put it...
	return 0xFFFFFFFF;
}

void GSMemManager::BlockFree(u32 startBlock, const allocationInfo &allocInfo)
{
	// this deals with the non-trivial case of bufferWidth != numPagesWide
	u32 currentPage = startBlock / GS_MEM_BLOCKS_PER_PAGE;
	u32 endPage = currentPage + allocInfo.bufferWidth * allocInfo.pagesTall;
	u32 offset = startBlock % GS_MEM_BLOCKS_PER_PAGE;
	u32 bitPattern = allocInfo.blockBitPattern << offset;
	for(;currentPage < endPage;currentPage++)
	{
		// skip unused pages
		if(currentPage % allocInfo.bufferWidth >= allocInfo.pagesWide)
			continue;

		// if this assert fires, for some reason, not all the blocks your freeing
		// were allocated.
		ASSERT((memoryMap[currentPage] & bitPattern) == bitPattern);

		memoryMap[currentPage] &= ~bitPattern;
	}
}

u32 GSMemManager::GetHighestBlockUsed(void)
{
	int currentPage;
	int currentBlock;
	for(currentPage = GS_MEM_NUM_PAGES - 1;currentPage >= 0;currentPage--)
	{
		if(memoryMap[currentPage] != 0x00000000)
		{
			for(currentBlock = GS_MEM_BLOCKS_PER_PAGE - 1; currentBlock >= 0; currentBlock--)
			{
				if(memoryMap[currentPage] & (1L << currentBlock))
				{
					return currentPage * GS_MEM_BLOCKS_PER_PAGE + currentBlock;
				}
			}
		}
	}
	// its empty...
	return 0xFFFFFFFF;
}


#if !defined(WIN32_TOOLS_DEBUG) && !defined(WIN32_TOOLS_RELEASE)
#ifndef SHIP_IT
// printf out a view of what memory is in use and whats free...
void GSMemManager::DumpMemTable()
{
	char buffer[100];

	for(u32 i = 0;i < GS_MEM_NUM_PAGES; i++)
	{
		sprintf(buffer, "%03.3X ", i);
		for(u32 j = 0; j < GS_MEM_BLOCKS_PER_PAGE; j++)
		{
			u32 bit = 1 << j;
			sprintf(&buffer[strlen(buffer)], "%c", (memoryMap[i] & bit) ? ('*') : ('.'));
		}
		dbgPrint("%s\n",buffer);
	}
}
#endif

#endif
