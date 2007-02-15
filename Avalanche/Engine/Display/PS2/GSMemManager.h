//
// GSMemManager.h
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
// the alloc function returns 0xFFFFFFFF on out of memory.
// the free function asserts that the memory you are freeing
// is really allocated.
// 
// it supports forcing page alignment for allocations.  This is
// for frame buffers, zbuffers, shadow buffers, etc...
//
// Currently disabled, support for HL & HH & L allocations is
// fairly easy (for 24 bit buffers with 4 & 8 bit textures
// floating over it)..

#ifndef __GS_MEM_MANAGER_H__
#define __GS_MEM_MANAGER_H__

#include "platform/BaseType.h"

#define GS_MEM_NUM_PAGES		512
#define GS_MEM_BLOCKS_PER_PAGE	32

class GSMemManager
{
protected:

	// some allocations only use a portion of the gs mems
	// 32 bit depth.  there are 3 possible layers...
	enum {
		gsMemLayerL,
		gsMemLayerHL,
		gsMemLayerHH,

		GS_MEM_NUM_LAYERS,
	};
	static u32 memoryMap[GS_MEM_NUM_PAGES] /*[GS_MEM_NUM_LAYERS]*/;

public:
	// represents the information we need to perform an allocation
	struct allocationInfo {
		u32 blockBitPattern;
		u32 maxOffset;
		u32 blocksWide;
		u32 blocksTall;
		u32 pagesWide;
		u32 pagesTall;
		u32 pageWidthInBlocks;
		u32 pageHeightInBlocks;
		u32 bufferWidth; // in pages...
	};

	// this is used to free all allocated memory...
	static void Reset(void);

	// these functions can be useful if you dont want to 
	// allocate you frame buffer and zbuffer from this manager.
	// You probably should just use the normal allocation
	// functions since they will be more adaptive to changes
	// in your buffer sizes, but, ...
	static void ReservePages(u32 startPage, u32 numPages);

	// these functions work on a block basis.  They correctly
	// allocate a two dimensional set of blocks.  They return
	// their addresses as block numbers.  which is what the
	// TEX0_?.TBP0 pointer takes, as well as the TEX0_?.CBP
	// ie. (Word Address/64)
	// RESTRICTION: texture must be a power of 2 wide & tall
	// USAGE: 
	// create a allocationInfo variable.
	// call GetAllocationInfo() to fill the allocationInfo.
	// call BlockAlloc to allocate a block that fits that
	//     description.
	// call BlockFree to free the block that was allocated.
	// if you dont want to store the allocationInfo, you
	// can regenerate it before freeing.
	static void GetAllocationInfo(u32 pixelsWide, u32 pixelsTall, u32 bufferWidth, u32 psmFormat, bool pageAligned, allocationInfo& allocInfo);
	static u32 BlockAlloc(const allocationInfo &allocInfo);
	static void BlockFree(u32 startBlock, const allocationInfo &allocInfo);


	// this is a utility function used by the conversion process
	// to get what the highest block used is...
	static u32 GetHighestBlockUsed(void);

#ifndef SHIP_IT
	// printf out a view of what memory is in use and whats free...
	static void DumpMemTable();
#endif
};


#endif // __GS_MEM_MANAGER_H__
