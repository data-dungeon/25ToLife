////////////////////////////////////////////////////////////////////////////
//
// TupMemoryManager
//
// An static memory manager used by the game engine
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "memorymanager.h"

#ifdef USE_TUP_MEMORY_MANAGER

#define DEBUG_MEMORY_BLOCK

// block size in bytes
#define MEMORY_BLOCK_SIZE 5000

/* align memory block sub-blocks to 4-byte alignment */
#define MEMORY_BLOCK_ALIGNMENT 4
#define ALIGN_MEMORY_BLOCK(n)					n = (n + MEMORY_BLOCK_ALIGNMENT - 1) & ~(MEMORY_BLOCK_ALIGNMENT - 1)
#define IS_MEMORY_BLOCK_ALIGNED(pAddr)			((int) pAddr & (MEMORY_BLOCK_ALIGNMENT - 1)) == 0
#define MAX_MEMORY_SIZE							(MEMORY_BLOCK_SIZE-((sizeof(MemoryHeader) + MEMORY_BLOCK_ALIGNMENT - 1) & ~(MEMORY_BLOCK_ALIGNMENT - 1)))

struct MemoryHeader
{
	size_t m_nSize;					// size of malloc'd block-- always MEMORY_BLOCK_SIZE? let use override?
	size_t m_nCurrentOffset;		// offset of next available memory in block
	size_t m_nActiveAllocations;	// number of active allocations made within this block
	MemoryHeader *m_pNext;			// the next memory block in the linked list
	MemoryHeader *m_pPrev;			// the previous memory block in the linked list
};

static MemoryHeader *pCurrentMemoryBlock = 0;
static bool bMemoryManagerEnabled = false;

#ifdef DEBUG_MEMORY_BLOCK
static int nTotalMemoryRequestCount;
static int nTotalMemoryRequestTotal;
static int nTotalMemoryRequestAlignedTotal;
static int nTotalMemoryBlocksAllocated;
static int nCurrentMemoryRequestCount;
static int nCurrentMemoryBlocksAllocated;
static int nTotalOtherAllocations=0;
static int nCurrentOtherAllocations=0;
#endif //DEBUG_MEMORY_BLOCK

void TupMemoryManager::Enable(void)
{
	bMemoryManagerEnabled = true;
}

void TupMemoryManager::Disable(void)
{
	bMemoryManagerEnabled = false;
}

void *TupMemoryManager::Allocate(size_t nSize)
{
	if (nSize==0) nSize=1; // if allocating 0 bytes make the size 1

	// attempt only when enabled and will fit in a block
	if (bMemoryManagerEnabled && nSize<MAX_MEMORY_SIZE)
	{
		// if there are no blocks at this time
		if (!pCurrentMemoryBlock)
		{
			pCurrentMemoryBlock = (MemoryHeader *) ::operator new(MEMORY_BLOCK_SIZE);
			pCurrentMemoryBlock->m_nSize = MEMORY_BLOCK_SIZE;
			pCurrentMemoryBlock->m_nCurrentOffset = sizeof(MemoryHeader);	// first available memory is after memory block record
			ALIGN_MEMORY_BLOCK(pCurrentMemoryBlock->m_nCurrentOffset);
			pCurrentMemoryBlock->m_nActiveAllocations=0;
			pCurrentMemoryBlock->m_pNext = NULL;
			pCurrentMemoryBlock->m_pPrev = NULL;

#ifdef DEBUG_MEMORY_BLOCK
			nTotalMemoryRequestCount = 0;
			nTotalMemoryRequestTotal = 0;
			nTotalMemoryRequestAlignedTotal = 0;
			nTotalMemoryBlocksAllocated = 1;
			nCurrentMemoryBlocksAllocated = 1;
			nCurrentMemoryRequestCount = 0;
#endif //DEBUG_MEMORY_BLOCK
		}

		if (pCurrentMemoryBlock->m_nSize - pCurrentMemoryBlock->m_nCurrentOffset < nSize)
		{
			MemoryHeader *pMemoryBlock = (MemoryHeader *) ::operator new(MEMORY_BLOCK_SIZE);

			// fill in memory record 
			pMemoryBlock->m_nSize = MEMORY_BLOCK_SIZE;
			pMemoryBlock->m_nCurrentOffset = sizeof(MemoryHeader);	// first available memory is after memory block record
			ALIGN_MEMORY_BLOCK(pMemoryBlock->m_nCurrentOffset);
			// hook in to chain
			pMemoryBlock->m_pNext = pCurrentMemoryBlock;
			pCurrentMemoryBlock->m_pPrev = pMemoryBlock;
			pMemoryBlock->m_pPrev = NULL;
			pMemoryBlock->m_nActiveAllocations=0;
			pCurrentMemoryBlock = pMemoryBlock;
#ifdef DEBUG_MEMORY_BLOCK
			nTotalMemoryBlocksAllocated++;
			nCurrentMemoryBlocksAllocated++;
#endif //DEBUG_MEMORY_BLOCK
		}

		/* for debugging only */
#ifdef DEBUG_MEMORY_BLOCK
		nTotalMemoryRequestCount++;
		nCurrentMemoryRequestCount++;
		nTotalMemoryRequestTotal += nSize;
		nTotalMemoryRequestAlignedTotal += ALIGN_MEMORY_BLOCK(nSize);
#endif //DEBUG_MEMORY_BLOCK

		/* grab memory. memory record is first thing in block, so can use it's address for start of block */
		assert(pCurrentMemoryBlock->m_nSize - pCurrentMemoryBlock->m_nCurrentOffset >= nSize);	// if this hits, requesting more than XML_MEMORY_BLOCK_SIZE!
		void *pReturn = (void *) ((int) pCurrentMemoryBlock + pCurrentMemoryBlock->m_nCurrentOffset);
		assert(IS_MEMORY_BLOCK_ALIGNED(pReturn));
		pCurrentMemoryBlock->m_nCurrentOffset += nSize;
		ALIGN_MEMORY_BLOCK(pCurrentMemoryBlock->m_nCurrentOffset);
		pCurrentMemoryBlock->m_nActiveAllocations++;
		return(pReturn);
	}
	// if not enabled or too big then use the standard memory allocation code
	else
	{
#ifdef DEBUG_MEMORY_BLOCK
		nTotalOtherAllocations++;
		nCurrentOtherAllocations++;
#endif
		return ::operator new(nSize);
	}
}

void TupMemoryManager::FreeBuffer(void *pBuffer)
{
	// if a null pointer then return
	if (!pBuffer)
		return;

	// scan through and see if this buffer was allocated from the memory manager
	MemoryHeader *pMemoryBlock = pCurrentMemoryBlock;
	while(pMemoryBlock)
	{
		// is the buffer within this block?
		if (((size_t) pBuffer)>((size_t) pMemoryBlock) && ((size_t) pBuffer)<((size_t) pMemoryBlock + pMemoryBlock->m_nSize))
		{
			// found it...
			pMemoryBlock->m_nActiveAllocations--;  // one less buffer used within this block
#ifdef DEBUG_MEMORY_BLOCK
			nCurrentMemoryRequestCount--;
#endif //DEBUG_MEMORY_BLOCK
			// if there are no more allocation then free the entire block
			if (pMemoryBlock->m_nActiveAllocations==0)
			{
				// remove from linked list
				if (pMemoryBlock->m_pPrev)
				{
					pMemoryBlock->m_pPrev->m_pNext = pMemoryBlock->m_pNext;
				}
				else // the head of the list
				{
					// this better be the first one...
					assert(pMemoryBlock == pCurrentMemoryBlock);
					pCurrentMemoryBlock = pMemoryBlock->m_pNext;
				}
				if (pMemoryBlock->m_pNext)
				{
					pMemoryBlock->m_pNext->m_pPrev = pMemoryBlock->m_pPrev;
				}
#ifdef DEBUG_MEMORY_BLOCK
				nCurrentMemoryBlocksAllocated--;
#endif //DEBUG_MEMORY_BLOCK
				delete pMemoryBlock; // delete the block
			}
			return; // we are done
		}
		// go to the next block...
		pMemoryBlock = pMemoryBlock->m_pNext;
	}
	// if we are this far it doesn't look like we allocated it so let the global one to it
#ifdef DEBUG_MEMORY_BLOCK
	nCurrentOtherAllocations--;
#endif
	delete (char*)pBuffer;
}

bool TupMemoryManager::AllBuffersFreed(void)
{
	if (pCurrentMemoryBlock)
	{
		return false;
	}
	else
	{
		return true;
	}
}


#endif // USE_TUP_MEMORY_MANAGER

