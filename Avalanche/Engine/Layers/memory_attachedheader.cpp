////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Includes
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "Layers/Semaphore/Semaphore.h"
#include "Layers/MemPool.h"
#include "Layers/exception/exception.h"

#if defined(WIN32)
// Placement new for memory semaphore
#include <new>
#endif

#if defined(PS2) && !defined(CONSUMER_BUILD)
//#define RAM_CLOBBER_DEBUG
#include "Layers/Breakpoint/HardwareBreak.h"
#endif

/// @}


/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Debugging defines
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// Amount of debugging information to print.  Levels over 1 would be
/// pretty much completely useless unless you happen to be debugging
/// the memory manager.
/// 0 = none
/// 1 = just the basic function call info
//  2 = bit more useful stuff
/// 3 = fairly useful
/// 4 = little bit overboard
/// 5 = so much output that you wont be able to breath.
//  6 = there are a few things at this level.
#define MEM_DEBUG_OUTPUT 0
#define MEM_DEBUG_OUTPUT_LEVEL MEM_DEBUG_OUTPUT

/// This causes the free list to be dumped at extremely frequent
/// intervals.  This is really not useful unless your debugging the
/// memory manager.
#undef MEM_DEBUG_DUMP_FREE_LIST

/// This checks the main allocator for lots of little fauxpas
/// that might occur if youve been farting around with it.
#define MEM_ASSERT_INTERNAL

/// This verifies that the heap has not been corrupted.
#define MEM_ASSERT_CORRUPT

/// This enables the unit test for the memory manager.
#undef UNIT_TEST_MEM_MANAGER

/// This causes an assert when we run out of memory.
#define MEM_ASSERT_OUT_OF_MEM

#ifdef UNIT_TEST_MEM_MANAGER
#ifdef MEM_ASSERT_OUT_OF_MEM
#error unit test and out of mem asserts arent compatible
#endif
#endif

#if !defined(CONSUMER_BUILD)
#define TAIL_SAFETY_CHECK
#endif

/// This causes the free heap to be scanned for errors after
/// every alloc or free.  this is probably too slow for regular use.
#ifdef _DEBUG
//#define MEM_FANATICAL_HEAP_CHECKS
#endif //_DEBUG

/// for memory overrun checking and header confirmation
#define MEM_MAGIC_NUMBER	0xbadfaced

// The per platform IO Buffer information
#if defined(WIN32) && !defined(_XBOX)
	#define IO_ALIGN	16
	#define IO_SLOP	0
#elif defined(_XBOX)
	#define IO_ALIGN	16
	#define IO_SLOP	XBOX_DVD_SECTOR_SIZE
#elif defined(PS2)
	#define IO_ALIGN	64
	#define IO_SLOP	64
#elif defined(GCN)
	#define IO_ALIGN	32
	#define IO_SLOP	0
#endif

/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Configuration defines
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// Used to enable the pool allocator.  If this is not defined, the
/// pool allocator will be shut off.
#define MEM_SUPPORT_POOLING

/// @{
/// Used to specify how many bits belong to alignment vs size.  They
/// should add up to sizeof(MemAllocHdr.mAlign_And_Size)*8.  They
/// probably wont ever need to be changed.  There are also masks
/// specified here for these values.  I suppose I could have the
/// preprocessor generate the masks, but Im lazy.
#define MEM_ALIGN_BITS 5
#define MEM_SIZE_BITS  27
#define MEM_ALIGN_MASK 0xF8000000
#define MEM_SIZE_MASK  0x07FFFFFF

/// @}

/// This value should never occur within MemAllocHeader or all
/// hell will break loose...
#define MEM_PADDING_VALUE 0xFFFFFFFF

/// minimum amount of space that a block can be.  This must be at
/// least 1.  its used by:
///   memShrink to see if it should really shrink a block
///   memAlloc to see if there is enough left of the old block to free it
///   memAlloc to see if alignment padding should be converted to a
///   free block
/// I would recommend that it be no larger than the element size of the
/// largest pool minus sizeof(MemAllocHeader) (252 for 256 byte pool).
/// smaller helps free more memory, but at the cost of a larger free list.
#define MEM_MIN_BLOCK_SIZE 60

/// memShrink to see if it should really shrink a block (must be
/// shrunk by >= MEM_MIN_SHRINK_SIZE and result must be >=
/// MEM_MIN_BLOCK_SIZE before a shrink will really happen.
/// The value MUST NOT BE LESS THAN MEM_MIN_BLOCK_SIZE +
/// sizeof(MemAllocHeader) so thats the default.
#define MEM_MIN_SHRINK_SIZE (MEM_MIN_BLOCK_SIZE + sizeof(MemAllocHeader))

/// this is a wrapper for assert to make sure that we dont ever
/// get caught  in an assert loop. Seems like assert should do this for
/// us.  but it doesnt yet.
#define MEM_CORRUPT_HANDLER() memCorruptHandler
#define MEM_HEAP_ASSERT_BASE(assrt) do { if(!MEM_CORRUPT_HANDLER()) {memCorruptHandler = true; assrt; memCorruptHandler = false; }} while(0)

#if 0
/// @{
/// definitions for assert that dont require asserts to be
/// active.  this can be useful at times.
#define MEM_ASSERTS(cond,str)		do { if(!(cond)){         memCorruptHandler = false; LOCKUP(); } } while(0)
#define MEM_ASSERTS_PTR(ptr,str) do { if(!CHECK_PTR(ptr)){ memCorruptHandler = false; LOCKUP(); } } while(0)
/// @}
#else
/// @{
/// the normal definitions for assert.
#define MEM_ASSERTS(cond,str)		ASSERTS    (cond,str)
#define MEM_ASSERTS_PTR(ptr,str) ASSERTS_PTR(ptr,str)
/// @}
#endif

/// @{
/// these are the basis of all the asserts below, as well as
/// being the basic assert type
#define MEM_HEAP_ASSERTS(cond,str)     MEM_HEAP_ASSERT_BASE(        MEM_ASSERTS    (cond, str))
#define MEM_HEAP_ASSERTS_PTR(ptr,str)  MEM_HEAP_ASSERT_BASE(        MEM_ASSERTS_PTR(ptr,  str))
#define MEM_HEAP_ASSERTS_NPTR(ptr,str) MEM_HEAP_ASSERT_BASE(if(ptr) MEM_ASSERTS_PTR(ptr,  str))
/// @}

/// @{
/// these are the asserts for checking for heap corruption
#ifdef MEM_ASSERT_CORRUPT
#define MEM_HEAP_ASSERTC(cond)     MEM_HEAP_ASSERTS     (cond, MEM_CORRUPT_STRING)
#define MEM_HEAP_ASSERTC_PTR(ptr)  MEM_HEAP_ASSERTS_PTR (ptr,  MEM_CORRUPT_STRING)
#define MEM_HEAP_ASSERTC_NPTR(ptr) MEM_HEAP_ASSERTS_NPTR(ptr,  MEM_CORRUPT_STRING)
#else
#define MEM_HEAP_ASSERTC(cond)     do{}while(0)
#define MEM_HEAP_ASSERTC_PTR(ptr)  do{}while(0)
#define MEM_HEAP_ASSERTC_NPTR(ptr) do{}while(0)
#endif
/// @}

/// these are the asserts for checking the main manager for
/// internal errors.
#ifdef MEM_ASSERT_INTERNAL
#define MEM_HEAP_ASSERTI(cond)     MEM_HEAP_ASSERTS     (cond, MEM_INTERNAL_ERROR)
#define MEM_HEAP_ASSERTI_PTR(ptr)  MEM_HEAP_ASSERTS_PTR (ptr,  MEM_INTERNAL_ERROR)
#define MEM_HEAP_ASSERTI_NPTR(ptr) MEM_HEAP_ASSERTS_NPTR(ptr,  MEM_INTERNAL_ERROR)
#else
#define MEM_HEAP_ASSERTI(cond)     do{}while(0)
#define MEM_HEAP_ASSERTI_PTR(ptr)  do{}while(0)
#define MEM_HEAP_ASSERTI_NPTR(ptr) do{}while(0)
#endif

/// this macro is used to add an assert that can be used to debug
/// when certain addresses are referenced within the memory manager.
#define MEM_HEAP_DEBUG_STOP(ptr,val)  do { if(((u32)ptr) == (val)) LOCKUP(); } while(0)

/// a couple of addresses to debug so you don't have to search the code.
#define MEM_HEAP_DEBUG_NORMAL_ADDR 0x00000000

/// @{
/// here are the strings used in the asserts in different conditions.
#define MEM_CORRUPT_STRING "Heap corrupted! Rerun with debug heap!"
#define MEM_INTERNAL_ERROR "Heap system Internal Error"
#define MEM_INTERNAL_POOL_ERROR "Heap system Internal Error in pool manager"
/// @}

/// @}

/// @{


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Nonconfigurable defines
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// the minimum alignment allowed by MemAllocHeader (due to its
/// alignment requirements)
#define MEM_MIN_ALIGN 4

/// a mask to match the MEM_MIN_ALIGN
#define MEM_MIN_ALIGN_MASK 0xFFFFFFFC

/// the number of entries in a single sub-pool.
#define MEM_NUM_ENTRIES_PER_POOL (sizeof(trackType)*8)

/// macro to output a debug string.
#define DEBUGOUTPUT(x) dbgPrint x
#define DEBUGDUMPBLOCK(x) DumpBlock x
#if MEM_DEBUG_OUTPUT > 0
#define DEBUGOUTPUTV(v,x) {if(MEM_DEBUG_OUTPUT_LEVEL >= v) DEBUGOUTPUT(x);}
#define DEBUGDUMPBLOCKV(v, x) { if(MEM_DEBUG_OUTPUT_LEVEL >= v) DEBUGDUMPBLOCK(x);}
#else
#define DEBUGOUTPUTV(v,x)
#define DEBUGDUMPBLOCKV(v, x)
#endif

/// this is the minimum size of a block with the header included.
#define MEM_MIN_BLOCK_SIZE_WITH_HEADER (MEM_MIN_BLOCK_SIZE+sizeof(MemAllocHeader))

/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Local types
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* ned says: just a note on the theory here...

this memory manager replaces the old one. the old one had a big chunk of memory reserved for the memory block headers.
free and malloc'd blocks used the same type of header record. there were two big disadvantages of this system:

1. the system had to pre-allocate the max number of headers needed-- it couldn't expand.
2. freeing a malloc'd block required running a linked list to find its header-- potentially very slow.

the old memory manager is implemented by the file memory_separateheader.cpp

this new manager addresses these disadvantages by putting each memory block's header just in front of the
memory itself. so the header can always be found quickly, and the number of malloc'd blocks is only limited by
the amount of memory available. this new method is called, naturally, memory_attachedheader.cpp.

the new method uses different headers for free and malloc'd blocks-- free blocks need to be in a linked list so that
when a malloc'd block is free'd we can insert it into the correct place in the free list (and possibly merge
neighboring free blocks). but in theory, the memory manager can lose track of a malloc'd block-- the owner is
responsible for returning it to the memory manager. so the malloc'd memory block does not need to be in a linked list.
this lets us use a very lean header for malloc'd blocks

the only disadvantages to the new system are:

1. memory corruption (e.g. someone writing past the end of their malloc'd block) can overwrite a free block header.
if this happens, the free block linked list is corrupted and we're screwed.
2. if a malloc'd block is not freed, it is completely lost

the lean malloc'd block header does not support the debugging functionality of the old memory manager-- you can't walk
the list of malloc'd blocks and collect stats. so we'll add the required functionality to the debug and release versions
of the code, and use the lean version in cdrom builds */

/// this header leads every allocated block.  The size stored in
/// here does not include the size of this header, only the size of the
/// data
class MemAllocHeader
{
#ifdef MEM_DEBUG_BLOCKS
public:
	// include fields that let us debug and keep track of malloc'd blocks. in a cdrom build, malloc'd blocks
	// are lost by the memory manager until they are freed again

	MemAllocHeader		*pPrevAlloc;		// doubly-linked to improve performance when freeing a malloc'd block-- don't have to run the list
	MemAllocHeader		*pNextAlloc;
	u32					ra;
	char					AllocName[ALLOC_NAME_LENGTH];
	u16					u16UseBits;
	u32					u32MagicNumber;	// to make sure that header is really a header
protected:
#endif //MEM_DEBUG_BLOCKS

	/// alignment and size are packed into this value.  alignment
   /// is the top 5 bits, while size is the bottom 27 bits.  Actual
   /// alignment of the block is 2^(mAlign).
	u32   mAlign_And_Size;

	/// decompress the alignment value for this block from bits
	/// into a real world alignment value.
	static inline u32 BitsToAlignment(u32 bits);

	/// compress the given alignment value into the bit form.
	static inline u32 AlignmentToBits(u32 align);

	/// get the compressed version of the alignment information
	/// for this block
	inline u32 GetAlignmentBits(void);

	/// change the compressed version of the alignment information
	/// for this block
	inline void SetAlignmentBits(u32 bits);

public:
	/// from a void* at the beginning of a data block returned by
	/// the memory manager, find its header by searching backwards
	/// through the alignment paddng
	static inline MemAllocHeader* FromData(void* data);

	/// given a header, move forward (optionally filling padding
	/// correctly), until the correct alignment is reached and then
	/// return that void*
	inline void* ToData(bool FillPadding);

	/// return a void* that points to the block immediately
	/// following this one.
	inline void* AfterBlock(void);

	/// return the amount of alignment padding that this block uses.
	inline u32 AlignmentPadding(void);

	/// get the size of the data area of this block.
	inline s32 GetSize(void);

	/// get the combined size of the data area of this block and
	/// its alignment padding
	inline s32 GetSizeWithPad(void);

	/// change the size of the data area for this block.
	inline void SetSize(s32 size);

	/// get the mask that represents the valid bits in the address
	/// given the alignment of this block
	inline u32 GetAlignmentMask(void);

	/// get the current alignment of this block.
	inline u32 GetAlignment(void);

	/// change the alignment of this block, and modify the data
	/// size of the block to account for padding differences
	inline void SetAlignment(u32 align);

	/// initialize this block.  Its a new block, and its 'size'
	/// big.  Sets alignment to 1.
	inline void Initialize(s32 size);

	/// get size of block with a given alignment.
	inline s32 GetSizeAligned(u32 align);
};

/// this header leads every freed block.  The size stored in the
/// MemAllocHeader does include the additional values in this class.
/// ***************************************************************
/// the data members are overwritten by client data
/// when the block gets allocated.
/// ***************************************************************
class MemFreeHeader : public MemAllocHeader
{

public:

	/// next pointer for a doubly linked list of free blocks..
	MemFreeHeader* pNext;

	/// prev pointer for a doubly linked list of free blocks..
	MemFreeHeader* pPrev;
};

// predeclare what a region is.
struct ts_memRegion;

/// function that finds a free block that the requested allocation can
/// fit in.
typedef MemFreeHeader* t_FitFunc(ts_memRegion* pRegion, u32 u32Size, u32 u32Align);

/// Function that allocates part of a chunk within a free chunk.
typedef MemAllocHeader* t_AllocFunc(ts_memRegion* pRegion, MemFreeHeader* pFreeBlock, u32 u32Size, u32 u32Align);

/// This represents a region of managed memory.
struct ts_memRegion
{
	/// pointer to the first node on the free list
	MemFreeHeader* pFreeList;
	MemFreeHeader* pFreeListTail; // the list's tail.

#ifdef MEM_DEBUG_BLOCKS
	/// if we are including extra debug info in the malloc'd block header,
	/// this is a pointer to the first malloc'd block
	MemAllocHeader *pAllocList;
#endif //MEM_DEBUG_BLOCKS

	/// memRegions exist on a singly linked list.  this is the next pointer.
	ts_memRegion* pNext;

	/// a bunch of flags for this region.
	u32 u32Flags;

	/// start of region memory
	unsigned char *pMemStart;

	/// size of region memory
	u32 u32MemLength;

	/// description of this region
	char *pDescriptor;

	/// current t_FitFunc for this region
	t_FitFunc* pFitFunc;

	/// current t_AllocFunc for this region
	t_AllocFunc* pAllocFunc;

	/// for tracking high water mark if REGION_TEST_HIGH_WATER_MARK is set in u32Flags
	int nMinMemory;

	/// for pools
#ifdef MEM_SUPPORT_POOLING
	CMemPoolSet* d_pools;
#endif
};

/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// local functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// t_FitFunc functions

/// @{
/// internal
MemFreeHeader* _memFitFirst(ts_memRegion* pRegion, u32 u32Size, u32 u32Align);
MemFreeHeader* _memFitBest(ts_memRegion* pRegion, u32 u32Size, u32 u32Align);
MemFreeHeader* _memFitLast(ts_memRegion* pRegion, u32 u32Size, u32 u32Align);
MemFreeHeader* _memFitLow(ts_memRegion* pRegion, u32 u32Size, u32 u32Align);
MemFreeHeader* _memFitHigh(ts_memRegion* pRegion, u32 u32Size, u32 u32Align);
/// @}

/// @{
// t_AllocFunc functions
MemAllocHeader* _memAllocFromTop(ts_memRegion* pRegion, MemFreeHeader* pFreeBlock, u32 u32Size, u32 u32Align);
MemAllocHeader* _memAllocFromBottom(ts_memRegion* pRegion, MemFreeHeader* pFreeBlock, u32 u32Size, u32 u32Align);
/// @}

/// @{
// internal work methods
MemFreeHeader* _memFreeMemory(ts_memRegion* pRegion, void* baseAddr, u32 u32Size);
MemFreeHeader* _memFreeMemory(ts_memRegion* pRegion, void* baseAddr, void* end);
MemFreeHeader* _memFreeBlock(ts_memRegion* pRegion, MemAllocHeader* allocBlock, bool bValidAlloc = false);
MemAllocHeader* _memAllocMemory(void* bottom, void* top);
MemFreeHeader* _memFindMatchingFree(ts_memRegion* region, void* block);
u32 _memValidAlignment(u32 u32Align);
bool _memVerifyAlignment(MemAllocHeader* allocBlock, u32 align);
void *_memShrinkBlock(MemAllocHeader *pBlock, u32 u32ShrinkAmount);
ts_memRegion *_memFindMemRegion( ts_memRegion *pList, void *pAddr);
bool _memIsValidRegion(ts_memRegion *pRegion);

void _memVerifyHeaps();
void _memVerifyFree();
void _memVerifyAllocated();
/// @}

bool _memInPool( void* addr );
bool _memPoolFree( void* addr );
void _memDumpPoolInfo(ts_memRegion* region);

bool memCorruptHandler = false;
char* f88x(void* ptr);
char* f88x(u32 x);

void _memDumpFreeList(ts_memRegion* pRegion);
void _memDumpAllocList(ts_memRegion* pRegion);

#ifdef UNIT_TEST_MEM_MANAGER
void memmgr_unit_test();
#endif

/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// local variables
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

t_FitFunc*			pFitList[FIT_MAX] =
{
	_memFitFirst,
	_memFitBest,
	_memFitLast,
	_memFitLow,
	_memFitHigh
} ;

t_AllocFunc*		pAllocList[ALC_MAX] =
{
	_memAllocFromTop,
	_memAllocFromBottom
} ;

s32					s32FitMatch[FIT_MAX] =
{
	ALC_TOP,
	ALC_TOP,
	ALC_TOP,
	ALC_TOP,
	ALC_BTM
} ;

static ts_memRegion* pActiveRegion;
static ts_memRegion* pFreeRegionList;
static ts_memRegion* pUsedRegionList;

static ts_memRegion StaticRegions[MEM_MAX_REGIONS];

static CSemaphore *memorySemaphore = NULL;

/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Debugging helpers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if defined(RAM_CLOBBER_DEBUG)

int disabledCount = 0;

void _disableBreakpoint()
{
	if (disabledCount++ == 0)
	{
		HardwareBreak::Disable();
	}
}

void _enableBreakpoint()
{
	if (--disabledCount == 0)
	{
		// find the largest block
		MemFreeHeader* largest = pActiveRegion->pFreeList;
		u32 largestSize = largest->GetSize();
		for (MemFreeHeader* b = pActiveRegion->pFreeList; b; b = b->pNext)
		{
			u32 size = b->GetSize();
			if (size > largestSize)
			{
				largestSize = size;
				largest = b;
			}
		}

		HardwareBreak::Enable(true, true, (u32)largest->ToData(false), (u32)largest->GetSize());
	}
}

class CScopeDisableBreakpoint
{
public:
	CScopeDisableBreakpoint()  { _disableBreakpoint(); }
	~CScopeDisableBreakpoint() { _enableBreakpoint(); }
};

#else

class CScopeDisableBreakpoint
{
public:
	CScopeDisableBreakpoint()  {}
	~CScopeDisableBreakpoint() {}
};

#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Dump an allocated block to the console.
///
/// @param depth number of spaces to put at the beginning of the line
/// @param block the block we are dumping
/// @param color the color of the line dumped (ansi string to do it)
/// @param premsg printf style format string for message before block info
void DumpBlock(int depth, MemAllocHeader* block, const char* color, const char* premsg, ...)
{
	char buffer[1000];
	char indent[] = "          ";
	va_list args;
	va_start(args, premsg);
	vsprintf(buffer, premsg, args);
	indent[depth] = '\0';
	DEBUGOUTPUT(("%s%s%s [0x%s (%d:%d)]" TTY_RESET "\n", indent, color, buffer, f88x(block), block->GetSizeWithPad(), block->GetAlignment()));
	va_end(args);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Return address variables

#ifdef MEM_DEBUG_BLOCKS

#define NUM_SAVED_RA	8
struct { u32 thread; u32 ra; } s_ReturnAddresses[ NUM_SAVED_RA ] = { {~0,0}, {~0,0}, {~0,0}, {~0,0}, {~0,0}, {~0,0}, {~0,0}, {~0,0} };

void memSetReturnAddress( u32 ra )
{
#if defined(PS2)
	u32 thread = GetThreadId();
#else
	u32 thread = 0;
#endif

	// set the return address uniquely for the thread, to the first caller that
	// sets it.
	int i;
	for ( i = 0 ; i < NUM_SAVED_RA ; i++ )
		if ( s_ReturnAddresses[i].thread == thread )
			break;
	if ( i >= NUM_SAVED_RA )
		for ( i = 0 ; i < NUM_SAVED_RA ; i++ )
			if ( s_ReturnAddresses[i].ra == 0 )
				break;
	if ( i < NUM_SAVED_RA && s_ReturnAddresses[i].ra == 0 )
	{
		s_ReturnAddresses[i].thread = thread;
		s_ReturnAddresses[i].ra     = ra;
	}
}

u32 memRetrieveReturnAddress()
{
#if defined(PS2)
	u32 thread = GetThreadId();
#else
	u32 thread = 0;
#endif

	for ( int i = 0 ; i < NUM_SAVED_RA ; i++ )
	{
		u32 ra = s_ReturnAddresses[i].ra;
		if ( ra && s_ReturnAddresses[i].thread == thread )
		{
			s_ReturnAddresses[i].ra = 0;
			return ra;
		}
	}
	return 0;
}

#endif //MEM_DEBUG_BLOCKS


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Essentially does a sprintf(internalbuffer, "%08.8X", x).  But
/// since some of the output functions that I used dont have the 08.8
/// part of that, I wrote this to do it for me.
///
/// @param x value to format
///
/// @return formatted string
///
char* f88x(u32 x)
{
	static int buff = 0;
	static char buffer[4][9] = {
		"00000000",
		"00000000",
		"00000000",
		"00000000"
	};
	char* curr = &buffer[buff][0];
	for ( int shift = 7 * 4 ; shift >= 0 ; shift -= 4 )
	{
		int v = ( x >> shift ) & 0xf;
		*curr++ = ( v > 9 ) ? ( 'A' - 10 + v ) : ( '0' + v );
	}
	curr = &buffer[buff++][0];
	if(buff >= 4) buff = 0;
	return curr;
}

/// typecasts the pointer to a u32 and then calls the u32 version of this function (which sprintfs it into a pretty hex string).
///
/// @param ptr pointer to be formatted
///
/// @return formatted version of the string
///
char* f88x(void* ptr)
{
	return f88x((u32)ptr);
}
/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Inline methods from MemAllocHeader
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline MemAllocHeader* MemAllocHeader::FromData(void* data)
{
#ifndef CDROM
	// shouldn't be looking if memory comes out of pool-- there's no header
	if ( _memInPool( data ) )
	{
		ASSERTS(false, "Pool memory buffers do not have headers");
		return(NULL);
	}
#endif //CDROM

	// start the search from the passed data pointer
	u32* search = (u32*)data;

	// search backwards from data for a non-MEM_PADDING_VALUE word...
	while(*(--search) == MEM_PADDING_VALUE);

	// move forward one, so that we point at the point right after the
	// header, then move back the size of a header.
	MemAllocHeader* retval = ((MemAllocHeader*)(search+1))-1;
#ifdef MEM_DEBUG_BLOCKS
	ASSERTS(retval->u32MagicNumber == MEM_MAGIC_NUMBER, "Not a valid memory block header");
#endif //MEM_DEBUG_BLOCKS

	// thats it.
	return retval;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline void* MemAllocHeader::ToData(bool FillPadding)
{
	// determine how far forward we need to go to get the data
	// pointer
	u32 alignmask = GetAlignmentMask();
	u32* search = (u32*)(this+1);

	// search forward for the correct alignment and set
	// padding as we go..
	while(((u32)search & alignmask) != 0)
	{
		// if we are supposed to be filling, do so.
		if(FillPadding)
			*search = MEM_PADDING_VALUE;

		// next spot
		search++;
	}

	// verify that we skipped the right amount forward.
	MEM_HEAP_ASSERTI(((u32)search - (u32)(this+1)) == AlignmentPadding());

	// return the data pointer.
	return search;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline s32 MemAllocHeader::GetSize(void)
{
	return mAlign_And_Size & MEM_SIZE_MASK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline void* MemAllocHeader::AfterBlock(void)
{
	// the point after the block comes after the block header, after
	// the padding, and after the data portion of the block.
	void* after = (u8*)(this+1) + AlignmentPadding() + GetSize();

	// return it.  This is a separate statement to make debugging a
	// little easier under SN.
	return after;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline u32 MemAllocHeader::AlignmentPadding(void)
{
	u32 padding = 0;

	// to determine how much padding this block will require, we first
	// need the alignment
	u32 align = GetAlignment();

	// then we need to skip the header.
	u32 base = (u32)(this+1);

	// now we calculate the alignment required at this point.
	if(base % align != 0)
		padding = align - base % align;

	// and thats what we return.
	return padding;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline s32 MemAllocHeader::GetSizeWithPad(void)
{
	// return the size of the data block including the amount of space
	// used to pad the block for alignment.
	s32 sizewithpad = GetSize() + AlignmentPadding();
	return sizewithpad;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline void MemAllocHeader::SetSize(s32 size)
{
	MEM_ASSERTS(size >= 0, "size must be positive");

	// to set the size, first clear the old size
	mAlign_And_Size &= MEM_ALIGN_MASK;

	// the set the new size
	mAlign_And_Size |= size & MEM_SIZE_MASK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline u32 MemAllocHeader::GetAlignmentBits(void)
{
	// grab the alignment bits from the size/align variable
	u32 bits = (mAlign_And_Size & MEM_ALIGN_MASK) >> MEM_SIZE_BITS;

	return bits;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline void MemAllocHeader::SetAlignmentBits(u32 bits)
{
	// clear the old alignment
	mAlign_And_Size &= MEM_SIZE_MASK;

	// set the new alignment
	mAlign_And_Size |= (bits << MEM_SIZE_BITS) & MEM_ALIGN_MASK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline u32 MemAllocHeader::GetAlignmentMask(void)
{
	// generate a mask that is 0 for bits which are valid in an address
	// given the alignment requirements and a 1 for bits which should
	// always be a 0 in an address given the alignment requirements
	u32 mask = ~(0xFFFFFFFF << GetAlignmentBits());
	return mask;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline u32 MemAllocHeader::BitsToAlignment(u32 bits)
{
	// bits to alignment... its just a shift for now
	u32 align = 1 << bits;
	return align;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline u32 MemAllocHeader::AlignmentToBits(u32 align)
{
	// alignment to bits is just a shift the other way, but currently I
	// find it by looping and shifting until I get the right value.
	// Rather icky.. something better should be done here.
	MEM_HEAP_ASSERTI(align >= 1);
	u32 bits = 0;
	while(align != (1u << bits))
		bits++;
	return bits;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline u32 MemAllocHeader::GetAlignment(void)
{
	// grab the alignment bits and convert them.
	return BitsToAlignment(GetAlignmentBits());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline void MemAllocHeader::SetAlignment(u32 align)
{
	// grab the current amount of padding
	u32 pad = AlignmentPadding();

	// convert the alignment to bits and store it
	SetAlignmentBits(AlignmentToBits(align));

	// adjust the size of the datablock to compensate for the change in
	// the amount of alignment padding requried.
	SetSize(GetSize() - AlignmentPadding() + pad);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline s32 MemAllocHeader::GetSizeAligned(u32 align)
{
	// sanity check
	MEM_ASSERTS(GetAlignment() == 1, "GetSizeAligned requires current alignment to be 1");
	MEM_ASSERTS(GetSize() >= 0, "GetSizeAligned requires size to be non-negative");

	// calculate alignment padding (align must be power of 2, which we require anyway)
	u32 base = (u32)(this + 1);
	u32 alignbase = (base + (align - 1)) & ~(align - 1);
	u32 padding = alignbase - base;

	// got answer
	return GetSize() - padding;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline void MemAllocHeader::Initialize(s32 size)
{
	// reserve space for the header
	size -= sizeof(MemAllocHeader);

	// check size-- has to fit in available bits. the second check is because MemAllocHeader::GetSize will
	// return a negative number if the high bit of the size is set. see comments there, but it looks like
	// it is a valid condition
	ASSERTS((size & MEM_ALIGN_MASK) == 0, "Memory buffer too big!");	// limited by MEM_SIZE_BITS portion of mAlign_And_Size
	ASSERTS((size & (1 << (MEM_SIZE_BITS - 1))) == 0, "Memory buffer too big!");

	// force alignment to 0.
	mAlign_And_Size = size & MEM_SIZE_MASK;

#ifdef MEM_DEBUG_BLOCKS
	/* clear debug fields */

	pPrevAlloc = NULL;
	pNextAlloc = NULL;
	u16UseBits = 0;
	u32MagicNumber = MEM_MAGIC_NUMBER;
	strcpy(AllocName, "No name");
#endif //MEM_DEBUG_BLOCKS
}
/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// The meat and potatoes
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void memInit(void)
{
	int					i;

	// clear all the region structures
	for( i = 0; i < MEM_MAX_REGIONS; i++)
	{
		StaticRegions[i].u32Flags = 0;
		StaticRegions[i].pDescriptor = NULL;
		StaticRegions[i].pFreeList = NULL;
		StaticRegions[i].pFreeListTail = NULL;
#ifdef MEM_DEBUG_BLOCKS
		StaticRegions[i].pAllocList = NULL;
#endif //MEM_DEBUG_BLOCKS
		if( i == MEM_MAX_REGIONS - 1)
			StaticRegions[i].pNext = NULL;
		else
			StaticRegions[i].pNext = &StaticRegions[i + 1];
	}

	pFreeRegionList = StaticRegions;
	pUsedRegionList = NULL;
	pActiveRegion = NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void memSetFitScheme(s32 s32Method)
{
	// verify that its a valid request
	MEM_HEAP_ASSERTI(s32Method < FIT_MAX);
	MEM_HEAP_ASSERTI(s32Method >= 0);

	// just incase the assert fails, we like to continue running :)
	if(s32Method >= FIT_MAX) return;

	// change the fit method
	memSetFitMethod(s32Method);

	// change the alloc method
	memSetAllocMethod(s32FitMatch[s32Method]);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void memSetFitMethod(s32 s32Method)
{
	// verify that its a valid request
	MEM_HEAP_ASSERTI(s32Method < FIT_MAX);
	MEM_HEAP_ASSERTI(s32Method >= 0);

	// just incase the assert fails, we like to continue running :)
	if(s32Method >= FIT_MAX) return;

	// change the function pointer used to do the fit calls.
	pActiveRegion->pFitFunc = pFitList[s32Method];
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void memSetAllocMethod(s32 s32Method)
{
	// verify that its a valid request
	MEM_HEAP_ASSERTI(s32Method < ALC_MAX);
	MEM_HEAP_ASSERTI(s32Method >= 0);

	// just incase the assert fails, we like to continue running :)
	if(s32Method >= ALC_MAX) return;

	// change the function pointer used to do the alloc calls.
	pActiveRegion->pAllocFunc = pAllocList[s32Method];
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int memGetRegionLength(t_Handle Handle)
{
	ts_memRegion *pRegion = (ts_memRegion *) Handle;
	ASSERT(_memIsValidRegion(pRegion));

	// return how big the region is.
	return ((ts_memRegion*)Handle)->u32MemLength;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void memGetRegionStats(t_Handle Handle, int *pnFreeBytes, int *pnFreeHandles, int *pnLargestFree, int *pnAllocBytes, int *pnAllocHandles)
{
	ts_memRegion *pRegion = (ts_memRegion *) Handle;
	ASSERT(_memIsValidRegion(pRegion));

	// grab info about free blocks
	int freeHandles = 0;
	int freeBytes   = 0;
	int largestFree = 0;
	for ( MemFreeHeader* free = pRegion->pFreeList ; free ; free = free->pNext )
	{
		MEM_HEAP_ASSERTC_PTR(free);
		MEM_HEAP_ASSERTC(free->GetAlignment() == 1);

		int size = free->GetSize();
		MEM_HEAP_ASSERTC(size >= MEM_MIN_BLOCK_SIZE);

		freeBytes += size;
		freeHandles++;
		if ( size > largestFree )
			largestFree = size;
	}

	// grab info about alloc blocks
	int allocHandles = 0;
#ifdef MEM_DEBUG_BLOCKS
	for ( MemAllocHeader* alloc = pRegion->pAllocList ; alloc ; alloc = alloc->pNextAlloc )
		allocHandles++;
#endif //MEM_DEBUG_BLOCKS

	// return info
	if ( pnFreeBytes    )	*pnFreeBytes    = freeBytes;
	if ( pnFreeHandles  )	*pnFreeHandles  = freeHandles;
	if ( pnLargestFree  )	*pnLargestFree  = largestFree;
	if ( pnAllocHandles )   *pnAllocHandles = allocHandles;
	if ( pnAllocBytes   )	*pnAllocBytes   = memGetRegionLength(Handle) - freeBytes;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
t_Handle	memAddRegion(unsigned char *pStart, u32 u32Length, u32 u32Flags, char *pdesc, u32 u32Handles, unsigned short usfitmethod, CMemPool* pools)
{
	// make sure people ask for valid things
	MEM_HEAP_ASSERTI( pStart != NULL);
	MEM_HEAP_ASSERTI( u32Handles > 0);// these are ignored.
	MEM_HEAP_ASSERTI( usfitmethod < FIT_MAX);

	// are we out of em?
	if( pFreeRegionList == NULL)
		CRT_ERROR( "Out of Memory Regions.\n");

	// remove a region from the free list
	ts_memRegion* pNewRegion = pFreeRegionList;
	pFreeRegionList = pNewRegion->pNext;

	// put it on the used list. put it at the end, on the theory that the first region added is the main region
	// (since we have to search regions when memFree is called)
	if (pUsedRegionList)
	{
		ts_memRegion *pTail = pUsedRegionList;
		while (pTail->pNext)	// while there's a next...
			pTail = pTail->pNext;
		pTail->pNext = pNewRegion;		// put new region at tail of existing list
	}
	else
		pUsedRegionList = pNewRegion;	// used list is empty, so new region is head of list
	pNewRegion->pNext = NULL;			// we put new region at end of list, so pNext is always NULL

	// set variables
	pNewRegion->u32Flags = u32Flags;
	pNewRegion->pDescriptor = pdesc;
	pNewRegion->pMemStart = pStart;
	pNewRegion->u32MemLength = u32Length;

	// the size of a memory block is limited by MEM_SIZE_BITS, which divides the bits of the mAlign_And_Size
	// field of the MemAllocHeader between size and alignment of a block. the limit is currently 64 MB
	// (0x04000000 bytes). if the game is requesting a larger region, then we have to break it up into
	// smaller free buffers. in order to prevent them from coalescing back into a large buffer, we leave
	// a small empty space between them

	unsigned char *pCurrentMem = pStart;
	u32 u32CurrentMemLength = u32Length;
	while (u32CurrentMemLength > 0)
	{
		// limit size of free block
#define MAX_FREE_BUFFER_SIZE (1 << (MEM_SIZE_BITS - 1))	// "- 1" because the high bit of size needs to be clear so size isn't considered negative. see comments in MemAllocHeader::GetSize
		u32 u32FreeBufferLength = u32CurrentMemLength;
		if (u32FreeBufferLength > MAX_FREE_BUFFER_SIZE)
			u32FreeBufferLength = MAX_FREE_BUFFER_SIZE;

		// build a large free block..
		_memFreeMemory(pNewRegion, pCurrentMem, u32FreeBufferLength);

		// more to do? if so, skip a bit to prevent free buffers from coalescing. to the free buffers, this looks
		// like an allocated buffer
#define SKIP_BYTES	256	// a nice round number, alignment-wise
		u32CurrentMemLength -= u32FreeBufferLength;
		if (u32CurrentMemLength)
		{
			pCurrentMem += u32FreeBufferLength;
			pCurrentMem += SKIP_BYTES;	// skip a bit of space so free buffers don't coalesce
			if (u32CurrentMemLength < SKIP_BYTES)
				u32CurrentMemLength = 0;
			else
			{
				u32CurrentMemLength -= SKIP_BYTES;

				// look for ridiculously small leftover
				if (u32CurrentMemLength < MEM_MIN_BLOCK_SIZE)
					u32CurrentMemLength = 0;
			}
		}
	}

#ifdef MEM_DEBUG_BLOCKS
	// shouldn't be any alloc'd blocks yet
	ASSERT(pNewRegion->pAllocList == NULL);
#endif //MEM_DEBUG_BLOCKS

	// default fit & alloc functions
	// the correct way would seem to be this:
// 	pNewRegion->pAllocFunc = pAllocList[s32FitMatch[usfitmethod]];
// 	pNewRegion->pFitFunc = pFitList[usfitmethod];
	// except thats not what the old code does... the old code only has
	// a global fit/alloc function pointer, not one per region, and it
	// ignores the usfitmethod parameter (well, it stores it, but never
	// uses the stored value).  So this parallels what it does...
	pNewRegion->pAllocFunc = pAllocList[ALC_TOP]; //pAllocList[ALC_BTM];
	pNewRegion->pFitFunc = pFitList[FIT_LAST];	//pFitList[FIT_BEST];

	// make it active if we dont have an active one yet.
	if(pActiveRegion == NULL)
		pActiveRegion = pNewRegion;

	// setup the pools
#ifdef MEM_SUPPORT_POOLING
	pNewRegion->d_pools = NULL;
	if ( pools )
	{
		int poolHeaderSize = ( sizeof(CMemPoolSet) + 15 ) & ~15;
		int poolBodySize   = CMemPoolSet::Size( pools );
		int totalSize      = poolHeaderSize + poolBodySize;

		t_Handle oldRegion = memSetActiveHandle( (t_Handle) pNewRegion );
		void* poolBlock = memAlloc( totalSize, 16 );
		memSetActiveHandle( oldRegion );

		pNewRegion->d_pools = (CMemPoolSet*) poolBlock;
		pNewRegion->d_pools->Init( pools, (char*)poolBlock + poolHeaderSize );
	}
#endif

#ifndef CONSUMER_BUILD
	/* high water tracking off by default */
	memTrackHighWater((t_Handle) pNewRegion, false);
#endif //CONSUMER_BUILD

	// initialize our first semaphore
	if (!memorySemaphore)
	{
		// can't call new directly because we're still initializing the memory system
		memorySemaphore = (CSemaphore*) memAlloc(sizeof(CSemaphore));
		new (memorySemaphore) CSemaphore(true);
	}

	/* handle is region address */
	return((t_Handle) pNewRegion);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* returns previously-active region, if any */

t_Handle	memSetActiveHandle(t_Handle Handle)
{
	// currently active region
	ts_memRegion* pPrevRegion = pActiveRegion;

	// were we given a request to set the region or a request to clear it?
	if (Handle == INVALID_HANDLE)
		pActiveRegion = NULL;
	else
	{
		ts_memRegion *pRegion = (ts_memRegion *) Handle;
		if (_memIsValidRegion(pRegion))
			pActiveRegion = pRegion;
		else
			ASSERTS(false, "Invalid region handle passed");
	}

	// if we set it to something, return its handle, else -1.
	return (pPrevRegion == NULL) ? INVALID_HANDLE : (t_Handle) pPrevRegion;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* move specified region to front or back of region list. when memFree is called, it looks through the regions to
find which one contains the memory block. normally, the main memory region is at the front of the list, since
almost all the the malloc'd buffers are in it. but it is possible to have a sub-region within the main region, and
when we free any buffer that is in that region is also in the main region. we want to find the sub-region, so we need
to be able to move the sub-region to the front of the region list, and then restore it to the back. this is to support
the different versions of tak (regular or costumed) in tak2, which have to share the same permanent memory block */

void memMoveRegionInList(t_Handle Handle, bool bFront)
{
	/* remove region from list */

	ts_memRegion *pRegion = (ts_memRegion *) Handle;
	ts_memRegion **ppListRegion = &pUsedRegionList;	// have to be able to modify it in place
	bool bFound = false;
	while ((*ppListRegion) && (bFound == false))
	{
		if (*ppListRegion == pRegion)
		{
			*ppListRegion = pRegion->pNext;	// update to skip over region-- this removes it from list
			bFound = true;
		}
		else
			ppListRegion = &((*ppListRegion)->pNext);
	}
	if (bFound == false)
	{
		ASSERT(false);
		return;
	}

	/* front or back? */

	if (bFront)
	{
		pRegion->pNext = pUsedRegionList;
		pUsedRegionList = pRegion;
	}
	else
	{
		if (pUsedRegionList)
		{
			ts_memRegion *pTail = pUsedRegionList;
			while (pTail->pNext)			// while there's a next...
				pTail = pTail->pNext;
			pTail->pNext = pRegion;		// put new region at tail of existing list
		}
		else
			pUsedRegionList = pRegion;	// used list is empty, so new region is head of list
		pRegion->pNext = NULL;			// we put new region at end of list, so pNext is always NULL
	}

#if 0
	DEBUGOUTPUT(("Region list after moving region %08x (%s), bFront = %d\n", Handle, pRegion->pDescriptor, bFront));
	ts_memRegion *pListRegion = pUsedRegionList;
	while (pListRegion)
	{
		DEBUGOUTPUT(("   %s\n", pListRegion->pDescriptor));
		pListRegion = pListRegion->pNext;
	}
#endif //0/1
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* check if region is empty */

bool memRegionIsEmpty(t_Handle Handle)
{
#ifndef MEM_DEBUG_BLOCKS
	return(true);	// who knows? should be sure of yourself before you go to cdrom build
#else //MEM_DEBUG_BLOCKS
	/* find the region */

	ts_memRegion* pRegion = (ts_memRegion *) Handle;
	if (_memIsValidRegion(pRegion) == false)
	{
		ASSERT(false);
		return(true);
	}

	/* any malloc'd blocks? */

	return(pRegion->pAllocList == NULL);
#endif //MEM_DEBUG_BLOCKS
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
u32 memHandleFree(t_Handle Handle)
{
	// find the region
	ts_memRegion* pRegion = (ts_memRegion*)Handle;
	if (_memIsValidRegion(pRegion) == false)
	{
		ASSERT(false);
		return(0);
	}

	// start at the first free block
	MemFreeHeader* freeBlocks = pRegion->pFreeList;

	u32 total = 0;

	// walk free blocks and total them.
	while(freeBlocks)
	{
		// check that we arent corrupt
		MEM_HEAP_ASSERTC_PTR(freeBlocks);
		MEM_HEAP_ASSERTC(freeBlocks->GetAlignment() == 1);

		// get the blocks size
		s32 size = freeBlocks->GetSize();

		// check that we arent corrupt
		MEM_HEAP_ASSERTC(size >= MEM_MIN_BLOCK_SIZE);

		// add to total
		total += (u32)size;

		// next
		freeBlocks = freeBlocks->pNext;
	}

	return total;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#if 1
void *memAlloc(u32 u32Size, u32 u32Align, bool canFreeRamToDoIt)
{
	MEM_SET_RETURN_ADDRESS();


#ifdef MEM_DEBUG_BLOCKS
	u32 ra = memRetrieveReturnAddress();
#endif

	CScopeLockSemaphore lock(memorySemaphore);

#ifdef UNIT_TEST_MEM_MANAGER
	#define timecount 100
	static int i = 0;
	if (i <= timecount) i++;
	if (i == timecount) {
		memmgr_unit_test();
	}
#endif

	DEBUGOUTPUTV(1, ( TTY_CYAN "memAlloc(%d, %d) Start\n" TTY_RESET, u32Size, u32Align));

	// check that its a valid request
	MEM_HEAP_ASSERTS(u32Size != 0x00000000 && u32Size < 0x08000000, "Invalid allocation size detected");
	MEM_HEAP_ASSERTS(u32Align != 0x00000000 && u32Align < 0x08000000, "Invalid allocation alignment detected");

	MEM_HEAP_DEBUG_STOP(u32Size, 0xFFFFFFFF);
	MEM_HEAP_DEBUG_STOP(u32Size, 0);

#ifdef MEM_SUPPORT_POOLING
	if (u32Align <= 16 && pActiveRegion->d_pools)
	{
		void* retVal = pActiveRegion->d_pools->Alloc( u32Size );
		if (retVal != NULL)
		{
			ASSERT(((unsigned int)retVal % u32Align) == 0);
			return retVal;
		}
	}
#endif //MEM_SUPPORT_POOLING

	CScopeDisableBreakpoint xxx;

#ifdef TAIL_SAFETY_CHECK
	u32Size += 4;
#endif

	// too small?  make it bigger.
	if(u32Size < MEM_MIN_BLOCK_SIZE)
		u32Size = MEM_MIN_BLOCK_SIZE;

	// round size up to nearest word...
	if(u32Size % MEM_MIN_ALIGN)
	{
		u32Size += MEM_MIN_ALIGN - (u32Size % MEM_MIN_ALIGN);
	}

	// make the alignment a valid alignment (considering alignment
	// compression into header)
	u32Align = _memValidAlignment(u32Align);

	// find a block it will fit in.
	MemFreeHeader* freeBlock;
	do
	{
		freeBlock = pActiveRegion->pFitFunc(pActiveRegion, u32Size, u32Align);
	}
	while(freeBlock == NULL && canFreeRamToDoIt && memCallOutOfMemCallback(u32Size, u32Align));


	// did we run out of mem?
	if ( !freeBlock )
	{
		#if defined(MEM_ASSERT_OUT_OF_MEM) && !defined(CDROM)
			_memDumpFreeList(pActiveRegion);
			_memDumpPoolInfo(pActiveRegion);
			MEM_HEAP_ASSERTS_PTR(freeBlock, "Out of memory (no free block that could contain the allocation was found)");
		#endif

		#if defined(CDROM)
		LOCKUP_MEM();
		#endif

		return NULL;
	}

	// alloc from the block we found
	MemAllocHeader* allocBlock = pActiveRegion->pAllocFunc(pActiveRegion, freeBlock, u32Size, u32Align);

#ifdef MEM_DEBUG_BLOCKS
	// fill in debug info
	if (pActiveRegion->pAllocList)
		pActiveRegion->pAllocList->pPrevAlloc = allocBlock;	// previous head now points back to new head
	allocBlock->pPrevAlloc = NULL;									// new head has no previous record
	allocBlock->pNextAlloc = pActiveRegion->pAllocList;		// new head points forward to previous head (if any)
	pActiveRegion->pAllocList = allocBlock;						// new head of list
	allocBlock->ra = ra;
	strcpy(allocBlock->AllocName, acAllocName);
	strcpy(acAllocName, "No name");	// this is a one-shot name
	ASSERT((u16UseBits & MEM_PERMANENT_DATA) == 0);	// can only be set through memMarkPermanentMemory
	allocBlock->u16UseBits = u16UseBits;
#endif //MEM_DEBUG_BLOCKS

#ifndef CONSUMER_BUILD
	/* maybe watch memory high-water mark */

	if (pActiveRegion->u32Flags & REGION_TEST_HIGH_WATER_MARK)
	{
		/* watching how much free memory remains is reasonably fast since there are typically few free blocks */

		int nMinMemory = memHandleFree((t_Handle) pActiveRegion);
		if (nMinMemory < pActiveRegion->nMinMemory)
			pActiveRegion->nMinMemory = nMinMemory;
	}
#endif //CONSUMER_BUILD

	// make sure it worked
	MEM_HEAP_ASSERTI_PTR(allocBlock);

	// get the data pointer (and clear the padding)
	void* data = allocBlock->ToData(true);

#ifdef TAIL_SAFETY_CHECK
	*(int*)((int)data + allocBlock->GetSize() - 4) = allocBlock->GetSize();
#endif //TAIL_SAFETY_CHECK

	DEBUGDUMPBLOCKV(2, (2, allocBlock, TTY_RED, "alloc @ 0x%s (%d) leaves (%d)", f88x(data), u32Size, memHandleFree((t_Handle)pActiveRegion)));

	// this is a spot for you to cause the system to halt while
	// debugging the memory manager.  If you know an address that doesn't
	// work right, just feed it into here.
	MEM_HEAP_DEBUG_STOP(allocBlock, 0x00000000);
	MEM_HEAP_DEBUG_STOP(allocBlock, 0x00000000);

	// this is a spot to help you debug memory leaks or other issues
	// external to the memory manager.  If you know the address that an
	// alloc happens at that you're interested in, just feed it in here.
	MEM_HEAP_DEBUG_STOP(data, MEM_HEAP_DEBUG_NORMAL_ADDR);
	MEM_HEAP_DEBUG_STOP(data, 0x0);

#ifdef MEM_FANATICAL_HEAP_CHECKS
	// verify the heap integrety
	_memVerifyHeaps();
#endif

	// return data pointer.
	ASSERT(((unsigned int)data % u32Align) == 0);
	return data;
}
#endif
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void *memAllocForIO(u32 u32Size)
{
	MEM_SET_RETURN_ADDRESS();

	// Compute the real size
	u32 realSize;
	if (IO_SLOP == 0)
		realSize = u32Size;
	else
		realSize = (u32Size + (IO_SLOP - 1)) & ~(IO_SLOP - 1);

	// Give them the real block
	return memAlloc(realSize, IO_ALIGN);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void *memAllocForIO(u32 u32Size, u32 u32Align)
{
	MEM_SET_RETURN_ADDRESS();

	// Compute the real size
	u32 realSize;
	if (IO_SLOP == 0)
		realSize = u32Size;
	else
		realSize = (u32Size + (IO_SLOP - 1)) & ~(IO_SLOP - 1);

	// Real alignment
	u32 realAlign;
	if (u32Align > IO_ALIGN)
		realAlign = u32Align;
	else
		realAlign = IO_ALIGN;

	// Give them the real block
	return memAlloc(realSize, realAlign);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// is the allocation possible?
bool memCouldAlloc(u32 u32Size, u32 u32Align, bool canFreeRamToDoIt)
{
	MemFreeHeader* block;
	do
	{
		// if so, this function would find the first fit that would hold it.
		block = _memFitFirst(pActiveRegion,u32Size,u32Align);
	}
	while(block == NULL && canFreeRamToDoIt && memCallOutOfMemCallback(u32Size, u32Align));

	return block != NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ts_memRegion *_memFindMemRegion( ts_memRegion *pList, void *pAddr )
{
	// find region that contains this block
	for ( ; pList ; pList = pList->pNext )
	{
		if( (unsigned char *) pAddr >= pList->pMemStart &&
			 (unsigned char *) pAddr < pList->pMemStart + pList->u32MemLength )
		{
			return pList;
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* make sure region is on active region list */
bool _memIsValidRegion(ts_memRegion *pRegion)
{
	ts_memRegion *pActiveRegion = pUsedRegionList;
	while (pActiveRegion)
	{
		if (pRegion == pActiveRegion)
			return(true);
		pActiveRegion = pActiveRegion->pNext;
	}
	return(false);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void _memRemoveFreeBlock(ts_memRegion* region, MemFreeHeader* freeBlock)
{
	// update list tail first...
	if(region->pFreeListTail == freeBlock)
	{
		region->pFreeListTail = freeBlock->pPrev;
		DEBUGDUMPBLOCKV(1, (3, freeBlock, TTY_PURPLE, "removed Block was tail. %s now tail", f88x(region->pFreeListTail)));
	}
	// now update prev block
	if(freeBlock->pPrev)
	{
		// tell block before us that we dont exist
		freeBlock->pPrev->pNext = freeBlock->pNext;
	}
	else
	{
		// no block before us, so it must be the head of the free list
		MEM_HEAP_ASSERTI(region->pFreeList == freeBlock);
		region->pFreeList = freeBlock->pNext;
	}
	if(freeBlock->pNext)
	{
		// tell block after us that we dont exist
		freeBlock->pNext->pPrev = freeBlock->pPrev;
		freeBlock->pNext = NULL;
	}
	freeBlock->pPrev = NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MemFreeHeader* _memFindMatchingFree(ts_memRegion* region, void* block)
{
	// find a free that is at block, or null if its not in the free list.
	MemFreeHeader* currFree = region->pFreeList;
	while(currFree && currFree != block)
	{
		currFree = currFree->pNext;
	}
	return currFree;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
u32 _memValidAlignment(u32 u32Align)
{
	// valid alignments are powers of 2.  this is a bad way to do this
	// and we could probably use a better one.
	u32 tAlign = MEM_ADDR_ALIGN;
	while(tAlign < u32Align)
		tAlign <<= 1;
	return tAlign;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool _memVerifyAlignment(MemAllocHeader* allocBlock, u32 align)
{
	u32 olddata = (u32) allocBlock->ToData(false);
	u32 base = (u32)(allocBlock + 1);
	u32 alignbase = (base + (align - 1)) & ~(align - 1);
	return olddata == alignbase;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MemFreeHeader* _memFreeMemory(ts_memRegion* pRegion, void* baseAddr, u32 u32Size)
{
	// I intentionally use a cast here instead of FromData()!
	// because the memory is not yet initialized as a block
	MemAllocHeader * allocBlock = (MemAllocHeader*)baseAddr;

	// set the block up
	allocBlock->Initialize(u32Size);

	// link it into free list
	return _memFreeBlock(pRegion, allocBlock);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MemFreeHeader* _memFreeMemory(ts_memRegion* pRegion, void* baseAddr, void* end)
{
	// how big will it be
	s32 size = (s32)end - (s32)baseAddr;

	// check that its valid
	MEM_HEAP_ASSERTI(size >= MEM_MIN_BLOCK_SIZE_WITH_HEADER);

	// do it
	return _memFreeMemory(pRegion, baseAddr, size);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MemAllocHeader* _memAllocMemory(void* bottom, void* top)
{
	// I intentionally use a cast here instead of FromData()!
	// because the memory is not yet initialized as a block
	MemAllocHeader * allocBlock = (MemAllocHeader*)bottom;

	// set up the block based on size
	allocBlock->Initialize((u32)top - (u32)bottom);

	// return it
	return allocBlock;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MemFreeHeader* _memFreeBlock(ts_memRegion* pRegion, MemAllocHeader* allocBlock, bool bValidAlloc /*= false*/)
{
	// valid request?
	MEM_HEAP_ASSERTI_PTR(pRegion);
	MEM_HEAP_ASSERTI_PTR(allocBlock);

#ifdef MEM_DEBUG_BLOCKS
	// remove from alloc block linked list. it is possible that this block is not in the alloc list--
	// for example, this routine is called to add leftover memory back into the free list when malloc'ing
	// a block from the start of a free block
	if (bValidAlloc)
	{
		if (allocBlock->pNextAlloc)
			allocBlock->pNextAlloc->pPrevAlloc = allocBlock->pPrevAlloc;
		if (allocBlock->pPrevAlloc)
			allocBlock->pPrevAlloc->pNextAlloc = allocBlock->pNextAlloc;
		if (pRegion->pAllocList == allocBlock)
			pRegion->pAllocList = allocBlock->pNextAlloc;
		allocBlock->AllocName[0] = 0;
		allocBlock->ra = 0;
	}
#endif //MEM_DEBUG_BLOCKS

	// this routine is passed a malloc'd block header, we'll turn
	// that into a free block header (free block header is derived
	// from malloc'd block header)
	MemFreeHeader* freeBlock = (MemFreeHeader*)allocBlock;

	// change its alignment to 1...
	allocBlock->SetAlignment(1);

	// check valid
	MEM_HEAP_ASSERTC(allocBlock->GetSize() >= MEM_MIN_BLOCK_SIZE);

	// find insertion point...
	MemFreeHeader* curr = pRegion->pFreeList;
	MemFreeHeader* prior = NULL;
	while(curr && freeBlock > curr)
	{
		DEBUGDUMPBLOCKV(5, (6, curr, TTY_BLUE, "step"));
		prior = curr;
		curr = curr->pNext;
		MEM_HEAP_ASSERTC_NPTR(curr);
	}

	bool inserted = false;

	if(prior)
		MEM_HEAP_ASSERTS(prior->AfterBlock() <= freeBlock, "Freed memory twice?");
	if(curr)
		MEM_HEAP_ASSERTS(curr >= freeBlock->AfterBlock(), "Freed memory twice?");

	// merge left
	if(prior)
	{
		if(prior->AfterBlock() == freeBlock)
		{
			DEBUGDUMPBLOCKV(4, (6, prior, TTY_YELLOW, "Block being freed merges left with this block"));
			prior->SetSize(prior->GetSize() + freeBlock->GetSize() + sizeof(MemAllocHeader));
			inserted = true;
			freeBlock = prior;
			prior = prior->pPrev;
		}
	}

	// merge right
	if(curr)
	{
		if(freeBlock->AfterBlock() == curr)
		{
			DEBUGDUMPBLOCKV(4, (6, curr, TTY_YELLOW, "Block being freed merges right with this block"));
			MemFreeHeader* next = curr->pNext;
			_memRemoveFreeBlock(pRegion, curr);
			freeBlock->SetSize(curr->GetSize() + freeBlock->GetSize() + sizeof(MemAllocHeader));

			// update pFreeListTail
			if ( pRegion->pFreeListTail == prior )
			{
				pRegion->pFreeListTail = freeBlock;
				DEBUGOUTPUTV(1, ( TTY_PURPLE "   new tail is %s\n" TTY_RESET, f88x(freeBlock)));
			}

			curr = next;
		}
	}
	else
	{
		// update pFreeListTail
		pRegion->pFreeListTail = freeBlock;
		DEBUGOUTPUTV(1, ( TTY_PURPLE "   new tail is %s\n" TTY_RESET, f88x(freeBlock )));
	}

	// ok, now stick it in.
	if(!inserted)
	{
		// front of list
		if(!prior)
		{
			freeBlock->pNext = pRegion->pFreeList;
			freeBlock->pPrev = NULL;
			if(pRegion->pFreeList)
				pRegion->pFreeList->pPrev = freeBlock;
			pRegion->pFreeList = freeBlock;
		}
		else
		{
			freeBlock->pPrev = prior;
			freeBlock->pNext = prior->pNext;
			if(prior->pNext)
				prior->pNext->pPrev = freeBlock;
			prior->pNext = freeBlock;
		}
	}

#ifdef MEM_FANATICAL_HEAP_CHECKS
	// scan the heap for corruption
	_memVerifyHeaps();
#endif

	return freeBlock;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void *_memShrinkBlock(MemAllocHeader *pBlock, u32 u32ShrinkAmount)
{
	// force diff to be minimum alignment...
	if(u32ShrinkAmount % MEM_MIN_ALIGN != 0)
	{
		u32ShrinkAmount -= u32ShrinkAmount % MEM_MIN_ALIGN;
	}

	// get the current size of the block.
	u32 oldSize = pBlock->GetSize();

	// calculate the size of the new block (this will be
	// slightly larger than requested because of MEM_MIN_ALIGN
	s32 newSize = oldSize - (s32)u32ShrinkAmount;

	// if the new size will be less than the minimum block
	// size, just resize to the minimum
	if(newSize < MEM_MIN_BLOCK_SIZE)
	{
		// avoid an assert by adjusting shrinkamount as well..
		u32ShrinkAmount -= MEM_MIN_BLOCK_SIZE - newSize;

		newSize = MEM_MIN_BLOCK_SIZE;
	}

	// if this fails, we just ignore the resize request since it
	// wouldn't free enough memory to be of consequence...
	if(u32ShrinkAmount >= MEM_MIN_SHRINK_SIZE)
	{
		// go ahead and do the resize...

		// find region that contains passed buffer
		ts_memRegion* pRegion = _memFindMemRegion( pUsedRegionList, pBlock);
		MEM_HEAP_ASSERTS_PTR(pRegion, "You passed me a pointer that doesnt point into a known memory region");
		if (!pRegion)
			return pBlock->ToData(false);

		// we need to know the exact address we currently end at
		// so that the new free block on top of us can end at the same
		// place.
		void* afterblock = pBlock->AfterBlock();

		// adjust size of alloced block...
		pBlock->SetSize(newSize);

		// makes sure we didnt shrink to far. and that things seem
		// to have gone ok.
		MEM_HEAP_ASSERTI((u32)pBlock->GetSize() >= oldSize - u32ShrinkAmount);
		MEM_HEAP_ASSERTI(((u32)afterblock - (u32)pBlock->AfterBlock()) == u32ShrinkAmount);

		// free the block...
		MemFreeHeader* newBlock = _memFreeMemory(pRegion, pBlock->AfterBlock(), afterblock);
		// Cant do this assert because memFreeMemory might merge
		// the block with another, causing this assert to fail
//		MEM_HEAP_ASSERTI(newBlock->GetSize() == u32ShrinkAmount - sizeof(MemAllocHeader));

		// dump the new free block
		DEBUGDUMPBLOCKV(3, (4, newBlock, TTY_BLUE, "shrink creates block"));
	}

	// dump the new free block
	DEBUGDUMPBLOCKV(3, (4, pBlock, TTY_BLUE, "Shrink result block"));

#ifdef MEM_FANATICAL_HEAP_CHECKS
	// scan the heap for corruption
	_memVerifyHeaps();
#endif

	// return the data pointer
	return pBlock->ToData(false);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void *memShrink(void *pAddr, u32 u32NewSize)
{
	DEBUGOUTPUTV(1, ( TTY_CYAN "memShrink(%s, %d) Start\n" TTY_RESET, f88x(pAddr), u32NewSize));

	// currently we can't shrink pooled allocs
	if ( _memInPool(pAddr) )
	{
		DEBUGOUTPUTV(3, ( TTY_PURPLE "  pool Shrink ignored\n" TTY_RESET ));
		// didnt change anything... send it back..
		return pAddr;
	}

#ifdef TAIL_SAFETY_CHECK
	u32NewSize += 4;
#endif //TAIL_SAFETY_CHECK

	// find the header
	MemAllocHeader* allocBlock = MemAllocHeader::FromData(pAddr);

	// if the new size is significantly less than the current
	// size, resize it...
	s32 diff = allocBlock->GetSize() - u32NewSize;

	DEBUGDUMPBLOCKV(3, (2, allocBlock, TTY_PURPLE, "shrink @ 0x%s (%d) to (%d)", f88x(pAddr), allocBlock->GetSize(), u32NewSize));

	// this is a bad state, but we should handle it fairly gracefully.
	MEM_HEAP_ASSERTS(diff >= 0, "memShrink called to enlarge a block...");
	if(diff <= 0)
		return memResize(pAddr, u32NewSize, allocBlock->GetAlignment());

	// do the shrink
	void* data = _memShrinkBlock(allocBlock, diff);

#ifdef TAIL_SAFETY_CHECK
	*(int*)((int)data + allocBlock->GetSize() - 4) = allocBlock->GetSize();
#endif //TAIL_SAFETY_CHECK

	return data;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void *memResize(void *pAddr, u32 u32NewSize, u32 u32Align)
{
	MEM_SET_RETURN_ADDRESS();

	CScopeLockSemaphore lock(memorySemaphore);
	DEBUGOUTPUTV(1, ( TTY_CYAN "memResize(0x%s, %d, %d)\n" TTY_RESET, f88x(pAddr), u32NewSize, u32Align));

	if ( _memInPool( pAddr ) )
	{
		void* newblock = memAlloc( u32NewSize, u32Align );
		memcpy( newblock, pAddr, u32NewSize );
		memFree( pAddr );
		return newblock;
	}

	CScopeDisableBreakpoint xxx;

	// check to see if we can easy-out this to the memShrink
	// routine...

	MemAllocHeader* allocBlock = MemAllocHeader::FromData(pAddr);

	DEBUGDUMPBLOCKV(3, (2, allocBlock, TTY_PURPLE, "resize original block"));

	// get a valid alignment
	u32Align = _memValidAlignment(u32Align);

#ifdef TAIL_SAFETY_CHECK
	u32NewSize += 4;
#endif //TAIL_SAFETY_CHECK

	// if the new size is significantly less than the current
	// size, resize it...
	s32 diff = u32NewSize - allocBlock->GetSize();

	// find region that contains passed buffer
	ts_memRegion* pRegion = _memFindMemRegion( pUsedRegionList, pAddr);
	MEM_HEAP_ASSERTI(pRegion);

	// see if alignment changed significantly...
	if(_memVerifyAlignment(allocBlock, u32Align) && pRegion)
	{
		// remember the new alignment
		// we cant do this because it might require the data to move
		// (if for example, it was a less strict alignment)
		//allocBlock->SetAlign(u32Align);

		// looks like we are ok on the alignment front...
		// that makes some of these checks simpler..

		// not changing size, and alignment is compatible..
		if(diff == 0)
			return pAddr;

		// shrinking and alignment is compatible
		if(diff < 0)
		{
			void* data = _memShrinkBlock(allocBlock, -diff);
#ifdef TAIL_SAFETY_CHECK
			*(int*)((int)data + allocBlock->GetSize() - 4) = allocBlock->GetSize();
#endif //TAIL_SAFETY_CHECK
			return data;
		}

		// lets see if there is a block after us that we can grow
		// in to.
		MemFreeHeader* freeBlock = _memFindMatchingFree(pRegion, allocBlock->AfterBlock());
		if(freeBlock)
		{
			// make sure we stay aligned by increasing the diff
			// until the diff is aligned.
			if(diff % MEM_MIN_ALIGN)
				diff += MEM_MIN_ALIGN - (diff % MEM_MIN_ALIGN);

			// how much space will be left in the block we found
			s32 freeleft = (freeBlock->GetSize() + sizeof(MemAllocHeader)) - diff;

			// is this going to be solved with just this free block?
			if(freeleft >= 0)
			{
				DEBUGDUMPBLOCKV(3, (2, freeBlock, TTY_BLUE, "found adjacent block (%d left after use)", freeleft));

				// remove the free block
				_memRemoveFreeBlock(pRegion, freeBlock);

				if(freeleft < MEM_MIN_BLOCK_SIZE_WITH_HEADER)
				{
					// grow our block to encompass entire free block...
					allocBlock->SetSize(allocBlock->GetSize() + freeBlock->GetSize() + sizeof(MemAllocHeader));
					DEBUGDUMPBLOCKV(3, (4, allocBlock, TTY_BLUE, "entire adjacent consumed (%d overhead)", freeleft));
				}
				else
				{
					// grow our block
					allocBlock->SetSize(allocBlock->GetSize() + diff);

					// free the remainder...
					MemFreeHeader* newFree = _memFreeMemory(pRegion, allocBlock->AfterBlock(), freeBlock->AfterBlock());
					DEBUGDUMPBLOCKV(3, (4, newFree, TTY_BLUE, "resize created new free block"));
				}

				// get the data pointer
				void* retptr = allocBlock->ToData(false);

				DEBUGDUMPBLOCKV(3, (4, allocBlock, TTY_BLUE, "returned block (no move) 0x%s", f88x(retptr)));

#ifdef TAIL_SAFETY_CHECK
				*(int*)((int)retptr + allocBlock->GetSize() - 4) = allocBlock->GetSize();
#endif //TAIL_SAFETY_CHECK

				// return the alloced block
				return retptr;
			}
		}
	}

	// if we get here, there is no easy way out of it, we need to
	// allocate a new block and move the data..
	void* newBlock = memAlloc(u32NewSize, u32Align);
	if(!newBlock)
	{
		/* agh! fail! free original block too! */

		_memFreeBlock(pRegion, allocBlock, true);
		return NULL;
	}

	// copy the data
	uint copySize = allocBlock->GetSize();
	if (u32NewSize < copySize)
		copySize = u32NewSize;
	memcpy(newBlock, pAddr, copySize);

#ifdef TAIL_SAFETY_CHECK
	MemAllocHeader* newBlockHeader = MemAllocHeader::FromData(newBlock);
	*(int*)((int)newBlock + newBlockHeader->GetSize() - 4) = newBlockHeader->GetSize();
#endif //TAIL_SAFETY_CHECK

	DEBUGDUMPBLOCKV(3, (4, MemAllocHeader::FromData(newBlock), TTY_BLUE, "returned block (with move) 0x%s (%d)", f88x(newBlock), u32NewSize));
	DEBUGDUMPBLOCKV(3, (4, allocBlock, TTY_BLUE, "old block freed"));

#ifdef MEM_DEBUG_BLOCKS
	// copy over header debug contents, but buffer loses its permanent marking
	if ( !_memInPool(newBlock) )
	{
		MemAllocHeader *pNewHeader = MemAllocHeader::FromData(newBlock);
		strcpy(pNewHeader->AllocName, allocBlock->AllocName);
		pNewHeader->u16UseBits = allocBlock->u16UseBits & ~MEM_PERMANENT_DATA;
	}
#endif //MEM_DEBUG_BLOCKS

	// free old block
	_memFreeBlock(pRegion, allocBlock, true);

	// return new block
	return newBlock;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void memGetBlockExtents(void* block, u32& start, u32& end)
{
	// find header
	if (!_memInPool(block))
	{
		MemAllocHeader* allocBlock = MemAllocHeader::FromData(block);
		start = (u32)allocBlock;
		end = (u32)allocBlock->AfterBlock() - 1;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool memFree(void *pAddr)
{
	CScopeLockSemaphore lock(memorySemaphore);

	DEBUGOUTPUTV(1, ( TTY_CYAN "memFree(0x%s) Start\n" TTY_RESET, f88x(pAddr)));

	// this is a good place to stop if you're trying to debug something
	// outside the memory manager.  use these to stop when a particular
	// address gets freed.
	MEM_HEAP_DEBUG_STOP(pAddr, 0x00000000);
	MEM_HEAP_DEBUG_STOP(pAddr, 0x00000000);

	if ( _memPoolFree( pAddr ) )
		return true;

	CScopeDisableBreakpoint xxx;

	// find header
	MemAllocHeader* allocBlock = MemAllocHeader::FromData(pAddr);

#ifdef TAIL_SAFETY_CHECK
	if ( *(int*)((int)pAddr + allocBlock->GetSize() - 4) != allocBlock->GetSize() )
	{
		MEM_ASSERTS( false, "HEAP: head and tail don't match, possible memory overrun" );
		#if !defined(DISABLE_EXCEPTIONS)
		char buf[256];
		int* head = (int*)(allocBlock);
		int* tail = (int*)((int)pAddr + allocBlock->GetSize() - 4);
		sprintf(buf, "MemFree: HEAD AND TAIL LINER DON'T MATCH\n"
						 "H %08X:%08X %08X [%08X] %08X %08X\n"
						 "T %08X:%08X %08X [%08X] %08X %08X\n",
						 head, head[-2], head[-1], head[0], head[1], head[2],
						 tail, tail[-2], tail[-1], tail[0], tail[1], tail[2]);
		ExceptionHandler::SetExceptionMessage(buf);
		*(volatile int*)1 = 0;
		#endif
	}
#endif //TAIL_SAFETY_CHECK

	// zap block we're freeing with bogus data, help catch crashes faster
#if !defined(CONSUMER_BUILD)
	memset( pAddr, 0xed, allocBlock->GetSize() );
#endif

	// this is a good place to stop if you're trying to debug the memory
	// manager.  use these for stopping when a particular block gets
	// freed.
	MEM_HEAP_DEBUG_STOP(allocBlock, 0x00000000);
	MEM_HEAP_DEBUG_STOP(allocBlock, 0x00000000);

	// find the region it's in
	ts_memRegion* pRegion = _memFindMemRegion( pUsedRegionList, pAddr);
	MEM_HEAP_ASSERTI(pRegion);
	if(!pRegion) return FALSE;

	DEBUGDUMPBLOCKV(2, (2, allocBlock, TTY_RED, "free @ 0x%s (%d) leaves (%d)", f88x(pAddr), allocBlock->GetSize(), memHandleFree((t_Handle)pRegion)));

	// free it
	MemFreeHeader* freeBlock = _memFreeBlock(pRegion, allocBlock, true);
	MEM_HEAP_ASSERTI(freeBlock && freeBlock->GetAlignment() == 1);

#ifdef MEM_DEBUG_DUMP_FREE_LIST
	// dump the free list if we are supposed to be dumping LOTS of
	// debuggin info.
	_memDumpFreeList(pRegion);
#endif

	return freeBlock != NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* turn high-water tracking on or off */
#ifndef CONSUMER_BUILD
void memTrackHighWater(t_Handle Handle, bool bState)
{
	ts_memRegion *pRegion = (ts_memRegion *) Handle;
	ASSERT(_memIsValidRegion(pRegion));
	if (bState)
		pRegion->u32Flags |= REGION_TEST_HIGH_WATER_MARK;
	else
		pRegion->u32Flags &= ~REGION_TEST_HIGH_WATER_MARK;
	memResetHighWater(Handle);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* status of high water check */
bool memHighWaterOn(t_Handle Handle)
{
	ts_memRegion *pRegion = (ts_memRegion *) Handle;
	ASSERT(_memIsValidRegion(pRegion));
	return((pRegion->u32Flags & REGION_TEST_HIGH_WATER_MARK) != 0);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void memResetHighWater(t_Handle Handle)
{
	ts_memRegion *pRegion = (ts_memRegion *) Handle;
	ASSERT(_memIsValidRegion(pRegion));
	pRegion->nMinMemory = MAXINT;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void memGetHighWater(t_Handle Handle, int *pnMinBytes, int *pnMaxBytes)
{
	ts_memRegion *pRegion = (ts_memRegion *) Handle;
	ASSERT(_memIsValidRegion(pRegion));
	if (pRegion->nMinMemory != MAXINT)
	{
		if (pnMinBytes)
			*pnMinBytes = pRegion->nMinMemory;
		if (pnMaxBytes)
			*pnMaxBytes = pRegion->u32MemLength - pRegion->nMinMemory;		// high water mark is (initial size - low water mark)
	}
	else
	{
		if (pnMinBytes)
			*pnMinBytes = 0;
		if (pnMaxBytes)
			*pnMaxBytes = 0;
	}
}
#endif //CONSUMER_BUILD

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* mark currently allocate blocks as "permanent." this routine is called after game engine initialization is finished,
but before the first fluff or game loop has started. it marks the memory allocations that are in place for the entire
game. on game platforms, these blocks will never be free'd, since the game never "ends" (that is, GameEngine__Shutdown
is never called). the intent of this routine is to provide a way to look for memory leaks. in between switching game
loops, the memory manager state should make it back to this initial state (all blocks marked as permanent) or else we
have a memory leak that will eventually gum up the works (though, of course, it may take a long time) */
void memMarkPermanentMemory(t_Handle RegionHandle)
{
#ifdef MEM_DEBUG_BLOCKS
	// get region
	ts_memRegion *pRegion = (ts_memRegion *) RegionHandle;
	ASSERT(pRegion);

	// create report
	int nAllocHandles, nAllocBytes, nFreeHandles, nLargestFree, nFreeBytes;
	memGetRegionStats( RegionHandle, &nFreeBytes, &nFreeHandles, &nLargestFree, &nAllocBytes, &nAllocHandles );
	dbgPrint( "memMarkPermanentMemory: region %s, alloc bytes %d, handles %d, free bytes %d, handles %d, largest free %d (%.1f%%)\n",
				 pRegion->pDescriptor, nAllocBytes, nAllocHandles, nFreeBytes, nFreeHandles,
				 nLargestFree, (float)nLargestFree / (float)nFreeBytes * 100.0f);

	// mark allocated blocks
	for ( MemAllocHeader* blk = pActiveRegion->pAllocList ; blk ; blk = blk->pNextAlloc )
	{
		blk->u16UseBits |= MEM_PERMANENT_DATA;
	}
#endif //MEM_DEBUG_BLOCKS
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* mark an individual buffer as permanent. only used on gcn as far as i can tell, and seems dangerous to me (ned) */
void memMarkBufferPermanent(void *pAddr)
{
#ifdef MEM_DEBUG_BLOCKS

#ifndef GCN
	ASSERT(false);	// gcn only?
#endif //GCN

	/* get block's header and set bit */
	MemAllocHeader* allocBlock = MemAllocHeader::FromData(pAddr);
	allocBlock->u16UseBits |= MEM_PERMANENT_DATA;

#endif //MEM_DEBUG_BLOCKS
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* return true if all current allocations are marked as permanent-- helps looking for memory leaks-- after
exiting a level and resetting the game engine, all mallocs should be permanent */
bool memCheckPermanentMemory(t_Handle RegionHandle)
{
#ifndef MEM_DEBUG_BLOCKS
	return(true);
#else //MEM_DEBUG_BLOCKS

#ifdef GCN
	OSLockMutex( &mutexMemAlloc);
#endif

	/* get to region */

	ts_memRegion *pRegion = (ts_memRegion *) RegionHandle;
	ASSERT(pRegion);

	/* nothing wrong yet */

	bool bResult = true;

	/* go through allocated blocks */

	MemAllocHeader *allocBlock = pActiveRegion->pAllocList;
	while (allocBlock)
	{
		if ((allocBlock->u16UseBits & MEM_PERMANENT_DATA) == 0)
		{
			/* found one */

			if (bResult)	// print header if this is the first one
				dbgPrint("*** Permanent memory check failed:\n");
			dbgPrint("    Buffer at 0x%08x ra=%08X \"%s\", %d bytes\n", (int) allocBlock->ToData(false), allocBlock->ra, allocBlock->AllocName, allocBlock->GetSize());
			bResult = false;
		}

		/* to next block */

		allocBlock = allocBlock->pNextAlloc;
	}

#ifdef GCN
	OSUnlockMutex( &mutexMemAlloc);
#endif

	/* return result */

	return(bResult);

#endif //MEM_DEBUG_BLOCKS
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memShowRegionFreeSize(t_Handle Handle)
{
	static int nLastFreeHandles = 0;
	static int nLastFreeBytes = 0;
	int nAllocHandles;
	int nAllocBytes;
	int nFreeHandles;
	int nFreeBytes;
	int nLargestFree;
	memGetRegionStats(Handle, &nFreeBytes, &nFreeHandles, &nLargestFree, &nAllocBytes, &nAllocHandles);

	dbgPrint("\n=Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free=\n");
	dbgPrint("Region %s: %d handles %d bytes; %d new handles %d new bytes\n",
		((ts_memRegion *) Handle)->pDescriptor,
		nFreeHandles,
		nFreeBytes,
		nFreeHandles - nLastFreeHandles,
		nFreeBytes - nLastFreeBytes);
	dbgPrint("=Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free=\n\n");
	nLastFreeHandles = nFreeHandles;
	nLastFreeBytes = nFreeBytes;
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memShowRegionAllocSize(t_Handle Handle)
{
	static int nLastAllocHandles = 0;
	static int nLastAllocBytes = 0;
	int nAllocHandles;
	int nAllocBytes;
	int nFreeHandles;
	int nFreeBytes;
	int nLargestFree;
	memGetRegionStats(Handle, &nFreeBytes, &nFreeHandles, &nLargestFree, &nAllocBytes, &nAllocHandles);

	dbgPrint("\n=Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc=\n");
	dbgPrint("Region %s: %d handles %d bytes; %d new handles %d new bytes\n",
		((ts_memRegion *) Handle)->pDescriptor,
		nAllocHandles,
		nAllocBytes,
		nAllocHandles - nLastAllocHandles,
		nAllocBytes - nLastAllocBytes);
	dbgPrint("=Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc=\n\n");
	nLastAllocHandles = nAllocHandles;
	nLastAllocBytes = nAllocBytes;
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memShowRegionBriefInfo(t_Handle Handle)
{
	int nAllocHandles;
	int nAllocBytes;
	int nFreeHandles;
	int nFreeBytes;
	int nLargestFree;
	memGetRegionStats(Handle, &nFreeBytes, &nFreeHandles, &nLargestFree, &nAllocBytes, &nAllocHandles);

	dbgPrint("\n=========================================================================\n");
	dbgPrint(" System Memory:  Used   %d handles, %d bytes\n",
		nAllocHandles,
		nAllocBytes
		);
	dbgPrint(" System Memory:  Free   %d handles, %d bytes\n",
		nFreeHandles,
		nFreeBytes
		);
	dbgPrint(" System Memory:  Largest free %d bytes (%.1f%%)\n",
		nLargestFree, (float)nLargestFree / (float)nFreeBytes * 100.0f
		);
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memCheckHeapUsage( t_Handle Handle, u32 *LayersUsage, u32 *LayersTotal, u32 *CHeapUsage, u32 *CHeapTotal, u32 *u32TotalHandles)
{
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memSetUseBit(const u16 u16Bit)
{
	u16UseBits |= u16Bit;
	ASSERT((u16UseBits & MEM_PERMANENT_DATA) == 0);	// can only mark things as permanent through memMarkPermanentMemory
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memClearUseBit(const u16 u16Bit)
{
	u16UseBits &= ~u16Bit;
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memDumpBlockInfo( FileStream* logFile )
{
	for ( ts_memRegion* region = pUsedRegionList ; region ; region = region->pNext )
	{
		logFile->Print( "======================================================================\n" );
		logFile->Print( "REGION %08X - %08X\n", region->pMemStart, region->pMemStart + region->u32MemLength );
		logFile->Print( "======================================================================\n" );

		// walk all allocated blocks
		MemAllocHeader* block;
		for ( block = region->pAllocList ; block ; block = block->pNextAlloc )
		{
			char  buf[ 256 ];
			char* p = buf;
			p += sprintf( p, "%08X: %08X %04X ra=%08X ", block->ToData( false ), block->GetSize(), block->u16UseBits, block->ra );
			for ( char* name = block->AllocName ; *name != 0 && name < &block->AllocName[ALLOC_NAME_LENGTH] ; )
				*p++ = *name++;
			*p++ = '\n';
			*p++ = 0;
			logFile->Print( buf );
		}

		int freeHandles = 0;
		int freeBytes   = 0;
		int largestFree = 0;
		for ( MemFreeHeader* free = region->pFreeList ; free ; free = free->pNext )
		{
			int size = free->GetSize();
			freeBytes += size;
			freeHandles++;
			if ( size > largestFree )
				largestFree = size;
		}
		#ifdef MEM_SUPPORT_POOLING
		if ( region->d_pools )
			region->d_pools->Dump( logFile );
		#endif
		logFile->Print( "FREE: %d  LARGEST_FREE: %d  FREE_HANDLES: %d\n", freeBytes, largestFree, freeHandles );
	}
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memDumpBlockInfo( const char* filename )
{
	FileStream fp;
	if ( fp.Open( filename, FileStream::WRITE, true ) )
		memDumpBlockInfo( &fp );
	fp.Close();
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
int memQueryBlockBit(const u16 u16Bit, const char *pBlockName, int *pnHandleCount, const bool bMatchAllBits, const bool bDetail, FileStream *pLogFile)
{
	/* loop through all memory blocks */

	int nTotalSize = 0;
	int nHandles = 0;
	int nIndex = 0;
	int nBlockNameLength = 0;
	if (pBlockName)
		nBlockNameLength = strlen(pBlockName);
	char Message[200];
	MemAllocHeader *allocBlock = pActiveRegion->pAllocList;
	while (allocBlock)
	{
		bool bFound;
		if (bMatchAllBits)
			bFound = (allocBlock->u16UseBits & u16Bit) == u16Bit;
		else
			bFound = (allocBlock->u16UseBits & u16Bit) != 0;
		if (bFound)
		{
			/* optional block name can total up a subset of blocks marked with current bit */

			if ((pBlockName == NULL) || (strnicmp(pBlockName, allocBlock->AllocName, nBlockNameLength) == 0))
			{
				if (bDetail)
				{
					void *pAddr = allocBlock->ToData(false);
					sprintf(Message, "  %3d: 0x%08x - 0x%08x (%8lu), %s\n", nIndex++, pAddr, (int) pAddr + allocBlock->GetSize() - 1, allocBlock->GetSize(), allocBlock->AllocName);
					if (pLogFile) pLogFile->Print(Message); else dbgPrint(Message);
				}
				nTotalSize += allocBlock->GetSize();
				nHandles++;
			}
		}

		/* to next block */

		allocBlock = allocBlock->pNextAlloc;
	}

	/* return results */

	if (bDetail)
	{
		sprintf(Message, "  Done\n");
		if (pLogFile) pLogFile->Print(Message); else dbgPrint(Message);
	}
	if (pnHandleCount)
		*pnHandleCount = nHandles;
	return(nTotalSize);
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memClearBlockBit(const u16 u16Bit, const char *pBlockName)
{
	/* loop through all memory blocks clearing the bit */

	u16 u16ClearMask = ~u16Bit;
	int nBlockNameLength = 0;
	if (pBlockName)
		nBlockNameLength = strlen(pBlockName);
	MemAllocHeader *allocBlock = pActiveRegion->pAllocList;
	while (allocBlock)
	{
		if (allocBlock->u16UseBits & u16Bit)
		{
			/* optional block name can total up a subset of blocks marked with current bit */

			if ((pBlockName == NULL) || (strnicmp(pBlockName, allocBlock->AllocName, nBlockNameLength) == 0))
				allocBlock->u16UseBits &= u16ClearMask;
		}

		/* to next block */

		allocBlock = allocBlock->pNextAlloc;
	}
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memSetAllocName(char *pcName)
{
	strncpy(acAllocName, pcName, ALLOC_NAME_LENGTH);
	acAllocName[ALLOC_NAME_LENGTH - 1] = 0;	//make sure it's null-terminated
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memSetAllocFileName(char *pcType, const char *pcFileName)
{
#if 1
	int offset = strlen( pcFileName ) + 1 - ALLOC_NAME_LENGTH;	//+1 for null
	if ( offset < 0 ) offset = 0;
	memSetAllocName( (char*)pcFileName + offset );
#elif 0
	const char *fname;
	char name[ALLOC_NAME_LENGTH*2], *cptr;
	strncpy(name, pcType, ALLOC_NAME_LENGTH);
	fname = pcFileName;
	while((cptr=strchr(fname, '\\')) != NULL)
		fname = cptr + 1;
	while((cptr=strchr(fname, '/')) != NULL)
		fname = cptr + 1;
	strncat(name, fname, (ALLOC_NAME_LENGTH - strlen(pcType)));
	memSetAllocName(name);
#else
	memSetAllocName(pcType);
#endif
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memSetBlockName(void *pAddr, char *pcName)
{
	if ( _memInPool( pAddr ) )
		return;

	MemAllocHeader* allocBlock = MemAllocHeader::FromData(pAddr);
	strncpy(allocBlock->AllocName, pcName, ALLOC_NAME_LENGTH);
}
#endif //MEM_DEBUG_BLOCKS

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// find the first free block in the list (which is the lowest) which
/// can satisfy the requested allocation
///
/// @param pRegion region this is happening in
/// @param u32Size size of the allocation request
/// @param u32Align alignment of the allocation request
///
/// @return free block found or NULL
///
MemFreeHeader* _memFitFirst(ts_memRegion* pRegion, u32 u32Size, u32 u32Align)
{
	// find the first region that we fit in...
	MemFreeHeader* curr = pRegion->pFreeList;
	while(curr)
	{
		s32 size = curr->GetSizeAligned(u32Align);
		if(size > (s32)u32Size)
		{
			DEBUGDUMPBLOCKV(2, (2, curr, TTY_PURPLE, "ffit %d - size after align (%d)", u32Size, size));
			MEM_HEAP_ASSERTI((u32)curr->GetSize() > u32Size);
			return curr;
		}
		curr = curr->pNext;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// finds the smallest free block that can contain the requested allocation.
///
/// @param pRegion region this is happening in
/// @param u32Size size of the request
/// @param u32Align alignment of the request
///
/// @return the free block or NULL
///
MemFreeHeader* _memFitBest(ts_memRegion* pRegion, u32 u32Size, u32 u32Align)
{
	MemFreeHeader* best = NULL;
	MEM_HEAP_ASSERTI_PTR(pRegion);
	MemFreeHeader* curr = pRegion->pFreeList;
	s32 bestSize = 0x7FFFFFFF;
	MEM_HEAP_ASSERTC_NPTR(curr);
	while(curr)
	{
		s32 size = curr->GetSizeAligned(u32Align);
		if(size > (s32)u32Size && size < (s32)bestSize)
		{
			bestSize = size;
			best = curr;
		}
		curr = curr->pNext;
		MEM_HEAP_ASSERTC_NPTR(curr);
	}
	if(best)
	{
		DEBUGDUMPBLOCKV(2, (2, best, TTY_PURPLE, "bfit %d  sizeafteralign(%d)", u32Size, bestSize));
	}
	return best;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// returns the last free block (highest in memory) that can satisfy
/// the requested allocation
///
/// @param pRegion the region this is happening in
/// @param u32Size the size of the request
/// @param u32Align the alignment of the request
///
/// @return the free block or NULL
///
MemFreeHeader* _memFitLast(ts_memRegion* pRegion, u32 u32Size, u32 u32Align)
{
	// find the first region that we fit in...
	MemFreeHeader* curr = pRegion->pFreeListTail;
	while(curr)
	{
		s32 size = curr->GetSizeAligned(u32Align);
		if(size > (s32)u32Size)
		{
			DEBUGDUMPBLOCKV(2, (2, curr, TTY_PURPLE, "lfit %d - size after align (%d)", u32Size, size));
 			MEM_HEAP_ASSERTI((u32)curr->GetSize() > u32Size);
			return curr;
		}
		curr = curr->pPrev;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// See _memFitFirst
MemFreeHeader* _memFitLow(ts_memRegion* pRegion, u32 u32Size, u32 u32Align)
{
	// since our free list is sorted, this is identical to
	// memfitfirst
	return _memFitFirst(pRegion, u32Size, u32Align);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// See _memFitLast
MemFreeHeader* _memFitHigh(ts_memRegion* pRegion, u32 u32Size, u32 u32Align)
{
	// since our free list is sorted, this is identical to
	// memfitlast
	return _memFitLast(pRegion, u32Size, u32Align);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// grabs a portion at the bottom of an allcoated block of mem and
/// returns it, but first it adds the rest of the block back into the
/// free store.
///
/// @param pRegion the region this is happening in
/// @param pFreeBlock the block to allocate within
/// @param u32Size the size of the request
/// @param u32Align the alignment of the request
///
/// @return the allocated block
///
MemAllocHeader* _memAllocFromBottom(ts_memRegion* pRegion, MemFreeHeader* pFreeBlock, u32 u32Size, u32 u32Align)
{
	// mark used
	_memRemoveFreeBlock(pRegion, pFreeBlock);

	DEBUGOUTPUTV(2, ( TTY_PURPLE "  bdiff by alloc %d\n" TTY_RESET, u32Size));

	// change its alignment (this adjusts its size to not include
	// the padding that is necessary)
	pFreeBlock->SetAlignment(u32Align);
	u32 padding = pFreeBlock->AlignmentPadding();
	if(padding > MEM_MIN_BLOCK_SIZE_WITH_HEADER)
	{
		void* blockTop = pFreeBlock->AfterBlock();

		// free the original block as a padding block
		pFreeBlock->SetAlignment(1);
		pFreeBlock->SetSize(padding - sizeof(MemAllocHeader));
		_memFreeBlock(pRegion, pFreeBlock);
		DEBUGDUMPBLOCKV(3, (4, pFreeBlock, TTY_BLUE, "bottom pad block"));

		// create a new block in the space left over.
		pFreeBlock = (MemFreeHeader*)_memAllocMemory(
			pFreeBlock->AfterBlock(), blockTop);

		DEBUGDUMPBLOCKV(3, (4, pFreeBlock, TTY_BLUE, "new block for final operations"));

		pFreeBlock->SetAlignment(u32Align);
		MEM_HEAP_ASSERTI(pFreeBlock->AlignmentPadding() == 0);
	}

	// how big is the free block (without alloc header)?
	u32 freesize = pFreeBlock->GetSize();

	// space left in free block
	s32 diff = freesize - u32Size;
	MEM_HEAP_ASSERTI(diff >= 0);

	// deflate diff until it matches the minimum alignment amount
	diff &= MEM_MIN_ALIGN_MASK;

	// how much of the free block will we use?
	if(diff > MEM_MIN_BLOCK_SIZE_WITH_HEADER)
	{
		// reserve u32Size of it.
		pFreeBlock->SetSize(u32Size);

		// free the top
		MemFreeHeader* freeBlk = _memFreeMemory(pRegion, pFreeBlock->AfterBlock(), diff);
		DEBUGDUMPBLOCKV(3, (4, freeBlk, TTY_BLUE, "top (free) block"));
	}

	DEBUGDUMPBLOCKV(3, (4, pFreeBlock, TTY_BLUE, "result block"));

#ifdef MEM_DEBUG_DUMP_FREE_LIST
	_memDumpFreeList(pRegion);
#endif

	return pFreeBlock;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// grabs a portion at the top of an allcoated block of mem and
/// returns it, but first it adds the rest of the block back into the
/// free store.
///
/// @param pRegion the region this is happening in
/// @param pFreeBlock the block to allocate within
/// @param u32Size the size of the request
/// @param u32Align the alignment of the request
///
/// @return the allocated space
///
MemAllocHeader* _memAllocFromTop(ts_memRegion* pRegion, MemFreeHeader* pFreeBlock, u32 u32Size, u32 u32Align)
{
	// mark used
	_memRemoveFreeBlock(pRegion, pFreeBlock);

	// how big is the free block (without alloc header)?
	u32 freesize = pFreeBlock->GetSize();

	// space left in free block
	s32 diff = (freesize - u32Size);
	// remove the space required to align the new block
	void* blockTop = pFreeBlock->AfterBlock();
	u32 work = (u32)blockTop;
	work -= u32Size;
	u32 alignPad = 0;
	if(((u32)work % u32Align) != 0)
		alignPad = work % u32Align;

	diff -= alignPad;

	MEM_HEAP_ASSERTI(diff >= 0);
	// deflate diff until it matches the minimum alignment amount
	diff &= MEM_MIN_ALIGN_MASK;

	DEBUGOUTPUTV(2, (TTY_PURPLE "  tdiff = %d by alloc %d\n" TTY_RESET, diff, u32Size));

	// how much of the free block will we use?
	if(diff > MEM_MIN_BLOCK_SIZE_WITH_HEADER)
	{
		diff -= sizeof(MemAllocHeader);

		pFreeBlock->SetSize(diff);

		DEBUGDUMPBLOCKV(3, (4, pFreeBlock, TTY_BLUE, "bottom block"));
		_memFreeBlock(pRegion, pFreeBlock);

		// if there is too much padding, create it as its own
		// block..
		if(alignPad > MEM_MIN_BLOCK_SIZE_WITH_HEADER)
		{
			void* alignBlockTop = blockTop;
			(u32&)blockTop -= alignPad;
			MemFreeHeader* padBlock = _memFreeMemory(pRegion, blockTop, alignBlockTop);
			DEBUGDUMPBLOCKV(3, (4, padBlock, TTY_BLUE, "top pad block"));
		}

		// now build the new allocation block
		MemFreeHeader* pNewBlock = (MemFreeHeader*)_memAllocMemory(pFreeBlock->AfterBlock(), blockTop);
		DEBUGDUMPBLOCKV(3, (4, pNewBlock, TTY_BLUE, "top (returned) block"));
		MEM_HEAP_ASSERTI(pNewBlock->AfterBlock() == blockTop);

		// set alignment back
		pNewBlock->SetAlignment(u32Align);

		// verify everything went ok.
		MEM_HEAP_ASSERTI(pNewBlock->AlignmentPadding() == 0);
		MEM_HEAP_ASSERTI((u32)pNewBlock->GetSize() >= u32Size);
		MEM_HEAP_ASSERTI(pFreeBlock->AfterBlock() == pNewBlock);
		MEM_HEAP_ASSERTI(pFreeBlock->GetSize() >= MEM_MIN_BLOCK_SIZE);
		MEM_HEAP_ASSERTI(pNewBlock->AfterBlock() == blockTop);
		MEM_HEAP_ASSERTI((u32)pNewBlock->ToData(false) + pNewBlock->GetSize() == (u32)blockTop);
		MEM_HEAP_ASSERTI((u32)pNewBlock->ToData(false) % u32Align == 0);

		pFreeBlock = pNewBlock;
	}
	else
	{
		// set alignment
		pFreeBlock->SetAlignment(u32Align);

		DEBUGDUMPBLOCKV(3, (4, pFreeBlock, TTY_BLUE, "full block consumed"));
	}

#ifdef MEM_DEBUG_DUMP_FREE_LIST
	_memDumpFreeList(pRegion);
#endif

	return pFreeBlock;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void _memDumpFreeList(ts_memRegion* pRegion)
{
	MemFreeHeader* curr = pRegion->pFreeList;
	while(curr)
	{
		MEM_HEAP_ASSERTC_NPTR(curr->pNext);
		MEM_HEAP_ASSERTC_NPTR(curr->pPrev);
		DEBUGDUMPBLOCK((6, curr, TTY_GREEN, "FreeBlock (p:%s, n:%s)", f88x(curr->pPrev), f88x(curr->pNext)));
		curr = curr->pNext;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void _memDumpAllocList(ts_memRegion* pRegion)
{
#ifdef MEM_DEBUG_BLOCKS
	MemAllocHeader* curr = pRegion->pAllocList;
	while(curr)
	{
		DEBUGDUMPBLOCK((6, curr, TTY_GREEN, "AllocBlock (%s)", curr->AllocName));
		curr = curr->pNextAlloc;
	}
#endif //MEM_DEBUG_BLOCKS
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void memShowAllocationInfo(void* alloc, char* msg)
{
	if ( !_memInPool( alloc ) )
	{
		MemAllocHeader * a = MemAllocHeader::FromData(alloc);
		DEBUGDUMPBLOCK((6, a, TTY_BLUE, "%s (%s)", msg, f88x(alloc)));
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_DEBUG_BLOCKS
void memShowRegionInfo( t_Handle Handle)
{
	DEBUGOUTPUT((TTY_RED "Free List\n" TTY_RESET));
	_memDumpFreeList((ts_memRegion*)Handle);
	DEBUGOUTPUT((TTY_RED "Alloc List\n" TTY_RESET));
	_memDumpAllocList((ts_memRegion*)Handle);
}
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// verify the heaps for any obvious signs of corruption
void _memVerifyHeaps()
{
	_memVerifyFree();
//	_memVerifyAllocated();	// this is SLOW, not recommended for normal use
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// runs through all the regions and verifies their free list doesnt
/// have any obvious signs of corruption.
void _memVerifyFree()
{
	// scan all regions
	ts_memRegion* pRegion = pUsedRegionList;
	while(pRegion)
	{
		MEM_HEAP_ASSERTC_PTR(pRegion);
		MemFreeHeader* curr = pRegion->pFreeList;
		MemFreeHeader* prior = NULL;
		MemFreeHeader* next = NULL;
		MEM_HEAP_ASSERTC_NPTR(curr);
		while(curr)
		{
			// adjusting the alignment causes the size to adjust to
			// match.  That way we dont have to deal with it in here at
			// all.
			MEM_HEAP_ASSERTC(curr->pNext != curr);
			MEM_HEAP_ASSERTC(curr->pPrev != curr);
			MEM_HEAP_ASSERTC(((u32)curr->ToData(false) & ~MEM_MIN_ALIGN_MASK) == 0);
			MEM_HEAP_ASSERTC(curr->GetSize() >= MEM_MIN_BLOCK_SIZE);
			MEM_HEAP_ASSERTC(curr->GetAlignment() >= 1);

			next = curr->pNext;

			MEM_HEAP_ASSERTC_NPTR(next);
			if(prior)
				MEM_HEAP_ASSERTC(prior->AfterBlock() < curr);
			if(next)
				MEM_HEAP_ASSERTC(curr->AfterBlock() < next);

			prior = curr;
			curr = next;
		}

		MEM_HEAP_ASSERTC(prior == pRegion->pFreeListTail);

		pRegion = pRegion->pNext;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// runs through all the regions and verifies their allocate list doesn't
/// have any obvious signs of corruption.
void _memVerifyAllocated()
{
#ifdef MEM_DEBUG_BLOCKS
	// scan all regions
	ts_memRegion* pRegion = pUsedRegionList;
	while(pRegion)
	{
		MemAllocHeader* alloc = pRegion->pAllocList;
		MemAllocHeader* allocPrev = NULL;
		for ( alloc = pRegion->pAllocList ; alloc ; allocPrev = alloc, alloc = alloc->pNextAlloc )
		{
			MEM_HEAP_ASSERTC( alloc->pNextAlloc != alloc );
			MEM_HEAP_ASSERTC( alloc->pPrevAlloc != alloc );
			MEM_HEAP_ASSERTC( alloc->pPrevAlloc == allocPrev );
			MEM_HEAP_ASSERTC( alloc->u32MagicNumber == MEM_MAGIC_NUMBER );
			MEM_HEAP_ASSERTC( ( (u32)alloc->ToData(false) & ~MEM_MIN_ALIGN_MASK ) == 0 );
			MEM_HEAP_ASSERTC( alloc->GetAlignment() >= 1 );
			MEM_HEAP_ASSERTC( alloc->GetSize() >= MEM_MIN_BLOCK_SIZE );
		}

		pRegion = pRegion->pNext;
	}
#endif //MEM_DEBUG_BLOCKS
}
/// @}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool _memInPool( void* addr )
{
#ifdef MEM_SUPPORT_POOLING
	for ( ts_memRegion* region = pUsedRegionList ; region ; region = region->pNext )
		if ( region->d_pools && region->d_pools->Owns( addr ) )
			return true;
#endif
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool _memPoolFree( void* addr )
{
#ifdef MEM_SUPPORT_POOLING
	for ( ts_memRegion* region = pUsedRegionList ; region ; region = region->pNext )
	{
		if ( region->d_pools && region->d_pools->Owns( addr ) )
		{
			region->d_pools->Free( addr );
			return true;
		}
	}
#endif
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void _memDumpPoolInfo(ts_memRegion* region)
{
#ifdef MEM_SUPPORT_POOLING
//	if (region && region->d_pools)
//		region->d_pools->Dump();
#endif
}


/// @{
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/// Here down is the unit test that I used to test the memory
/// manager.  if you make changes, you really should rerun this
/// overnight.  Its all poorly documented.
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#ifdef UNIT_TEST_MEM_MANAGER
//#define doprint(x) dbgPrint x
#define doprint(x) do { DI(); scePrintf x; EI(); } while(0)
#define MINGAP 0
#define LOOPSBETWEENPRINTS 10
#define MAXSIZE 1024*1024*20
#define MAXALIGNPOWER 8
struct ts_entry {
	u8* start;
	u8* end;
	s32 size;
	s32 align;
};
static DynamicArray<ts_entry> *entries;
enum {
	FILL,
	EMPTY,
	SCAN,
	THRASH,
	MAKEROOM,
} mode = FILL;
enum entrytype {
	INUSE,
	FREE,
	DONTCARE
};
static int iteration = 0;
static float sqrtmaxsize;
static float cubertmaxsize;
static u32 totalfree;
static int fills = 0;
static int empties = 0;
static int scans = 0;
static int thrashs = 0;
static int makerooms = 0;
static int allocmethodchanges = 0;
static int fitmethodchanges = 0;
static int frees = 0;
static int allocs = 0;
static int shrinks = 0;
static int resizes = 0;
static int highinusecount = 0;
static int lowinusecount = 9999999;
static u64 totalallocsize = 0;
static int minallocsize = MAXSIZE;
static int maxallocsize = 0;
static int maxrandresult = 0;
static int currentinuse = 0;
static int maxcurrentalloc = 0;
static u64 timescanning = 0;
static u64 timethrashing = 0;
static u64 timefilling = 0;
static u64 timeemptying = 0;
static u64 timemakerooming = 0;
static s32 randomentry()
{
	int count = entries->Count();
	if(count == 0)
		return -1;
	return rand() % count;
}
static s32 randomsize()
{
	float max = cubertmaxsize;
	u32 randr = rand();
// 	if(randr & 0x000300000)
// 		max *= 0.25f;
// 	if(randr & 0x000030000)
// 		max *= 0.25f;
// 	if(randr & 0x000003000)
// 		max *= 0.25f;
	if(randr & 0x000000300)
		max *= 0.25f;
	if(randr & 0x000000030)
		max *= 0.25f;
	float temp = (rand() / (float)0x7FFFFFFF) * max;
	s32 size = (s32)(temp*temp*temp);
	return size+1;
}
static s32 randomalign()
{
	s32 align = 1 << (rand() % MAXALIGNPOWER);
	return align;
}
static void shrinkentry(s32 entry)
{
	if(entry == -1)
		return;
	u32 shrinkamount = (u32)((rand() / (float)0x7FFFFFFF) * (*entries)[entry].size);
	if(shrinkamount == (*entries)[entry].size)
		return;
	if(shrinkamount == 0)
		return;
	u32 newsize = (*entries)[entry].size - shrinkamount;
	u8* newstart = (u8*)memShrink((*entries)[entry].start, newsize);
	MEM_HEAP_ASSERTI(newstart == (*entries)[entry].start);
	(*entries)[entry].start = newstart;
	(*entries)[entry].size = newsize;
	(*entries)[entry].end = (*entries)[entry].start + newsize;
	memset(newstart, 0xFFFFFFF, (*entries)[entry].size);
}
static void resizeentry(s32 entry)
{
	if(entry == -1)
		return;
	u32 shrinkamount = (u32)((rand() / (float)0x7FFFFFFF) * (*entries)[entry].size);
	if(shrinkamount == (*entries)[entry].size)
		return;
	if(shrinkamount == 0)
		return;
	if(rand() & 0x00001000)
		shrinkamount = -shrinkamount;
	u32 newsize = (*entries)[entry].size - shrinkamount;
	u32 newalign = randomalign();
	u8* newstart = (u8*)memResize((*entries)[entry].start, newsize, newalign);
	if(!newstart)
	{
		entries->Delete(entry);
		return;
	}
	(*entries)[entry].start = newstart;
	(*entries)[entry].size = newsize;
	(*entries)[entry].end = (*entries)[entry].start + newsize;
	(*entries)[entry].align = newalign;
	memset(newstart, 0xFFFFFFF, (*entries)[entry].size);
}
static void freeentry(s32 entry)
{
	if(entry == -1)
		return;
	memFree((*entries)[entry].start);
	currentinuse -= (*entries)[entry].size;
	(*entries)[entry].size = 0;
	frees++;
	entries->Delete(entry);
	if(lowinusecount > entries->Count())
		lowinusecount = entries->Count();
}
static bool fillentry()
{
	s32 size = randomsize();
	s32 align = randomalign();
	ts_entry entry;
	entry.start = (u8*)memAlloc(size, align);
	if(!entry.start){
		return false;
	}
	entry.end = entry.start + size;
	entry.align = align;
	entry.size = size;
	currentinuse += entry.size;
	if(currentinuse > maxcurrentalloc)
		maxcurrentalloc = currentinuse;
	allocs++;
	totalallocsize += size;
	if(size < minallocsize)
		minallocsize = size;
	if(size > maxallocsize)
		maxallocsize = size;
	entries->Add(entry);
	if(highinusecount < entries->Count())
		highinusecount = entries->Count();

	memset(entry.start, 0xFFFFFFF, size);
	return true;
}
static void stats(char* title)
{
	// print stats...
	doprint((title,scans));
	doprint(( TTY_GREEN "\nfills: %d\n",fills));
	doprint(("empties: %d\n",empties));
	doprint(("thrashs: %d\n", thrashs));
	doprint(("makerooms: %d\n", makerooms));

	doprint((TTY_CYAN "\nallocmethodchanges: %d\n", allocmethodchanges));
	doprint(("fitmethodchanges: %d\n", fitmethodchanges));
	doprint(("frees: %d\n", frees));
	doprint(("allocs: %d\n" TTY_RESET, allocs));

	doprint((TTY_BLUE "\ninusecount: %d\n", entries->Count()));
	doprint(("highinusecount: %d\n", highinusecount));
	doprint(("lowinusecount: %d\n", lowinusecount));
	doprint(("currentallocsize: %d\n", currentinuse));
	doprint(("highwater: %d\n", 	maxcurrentalloc));
	doprint(("totalallocsize: %ld\n", totalallocsize));
	doprint(("avgallocsize: %d\n", totalallocsize / allocs));
	doprint(("minallocsize: %d\n", minallocsize));
	doprint(("maxallocsize: %d\n" TTY_RESET, maxallocsize));

	float totaltime =
		(float)timescanning +
		(float)timethrashing +
		(float)timefilling +
		(float)timeemptying +
		(float)timemakerooming;

	doprint((TTY_YELLOW "\ntimescanning: %d%c\n", (int)(((float)timescanning / totaltime) * 100.0f), '%'));
	doprint(("timethrashing: %d%c\n", (int)(((float)timethrashing / totaltime) * 100.0f),'%'));
	doprint(("timefilling: %d%c\n", (int)(((float)timefilling / totaltime) * 100.0f),'%'));
	doprint(("timeemptying: %d%c\n", (int)(((float)timeemptying / totaltime) * 100.0f),'%'));
	doprint(("timemakerooming: %d%c\n" TTY_RESET, (int)(((float)timemakerooming / totaltime) * 100.0f),'%'));
}
static void listhandles()
{
	for(int i = 0; i < entries->Count(); i++)
	{
		char buffer[1000];
		sprintf(buffer,"#%4.4d:  %08.8x - %08.8x (%d:%d)\n",
					i, (*entries)[i].start, (*entries)[i].end, (*entries)[i].size, (*entries)[i].align);
		doprint(("%s",buffer));
	}
}
static void checkentry(s32 entry)
{
	if((*entries)[entry].size == 0)
		return;

	//  check for overlap
	int curr;
	int count = entries->Count();
	for(curr = entry+1; curr < count; curr++)
	{
		if((*entries)[curr].size == 0)
			continue;

		u8* s1 = (*entries)[curr ].start - MINGAP;
		u8* e1 = (*entries)[curr ].end;
		u8* s2 = (*entries)[entry].start - MINGAP;
		u8* e2 = (*entries)[entry].end;
		if((s1 > s2 && s1 < e2) ||
			(e1 > s2 && e1 < e2))
		{
			char buffer[100];
			stats("---------- Overlap! ----------\nscans: %d\n");
			sprintf(buffer,"#%d", entry);
 			memShowAllocationInfo((*entries)[entry].start, buffer);
			sprintf(buffer,"#%d", curr);
 			memShowAllocationInfo((*entries)[curr].start, buffer);
			listhandles();
			Layers__ShowMemoryState();
			LOCKUP();
		}
	}
}
static u32 freespace()
{
	return memHandleFree(Layers__GetMainHeap());
}
static void checkfree()
{
	if(totalfree != memHandleFree(Layers__GetMainHeap()))
	{
		doprint(("Leak detected!\n"));
		stats("---------- LEAK! ----------\nscans: %d\n");
		listhandles();
		Layers__ShowMemoryState();
		LOCKUP();
	}
}
void memmgr_unit_test()
{
	int i;

	doprint(("starting unit test\n"));

	DynamicArray<ts_entry> stackentries(5000,1000);
	entries = &stackentries;

	sqrtmaxsize = Math::Sqrt(MAXSIZE);
	cubertmaxsize = Math::Pow(MAXSIZE, 0.3333333f);

	totalfree = freespace();
	u32 seed;
	asm("mfc0 %0, $9" : "=r" (seed));
	srand(seed);
//	seed = -2027970026;
	doprint(("seed: %d\n", seed));

	while(1){
		u32 starttime, endtime;
		iteration++;

		if((iteration % LOOPSBETWEENPRINTS) == 0)
		{
			asm("mfc0 %0, $9" : "=r" (seed));
			srand(seed);
			doprint(("********** NEW SEED %d ***********\n", seed));
			mode = SCAN;
		}

		asm("mfc0 %0, $9" : "=r" (starttime));
		u64* counter;

		switch(mode){
		case FILL:
			fills++;
			while(fillentry());
			mode = THRASH;
			counter = &timefilling;
			break;
		case EMPTY:
			empties++;
			i = entries->Count();
			while(i--)
				freeentry(0);
			checkfree();
			mode = THRASH;
			counter = &timeemptying;
			break;
		case SCAN:
			scans++;
			for(i = 0; i < entries->Count(); i++)
				checkentry(i);
			stats("---------- scan %d ----------\n");
			mode = THRASH;
			counter = &timescanning;
			break;
		case THRASH:
			thrashs++;
			while(mode == THRASH)
			{
				static const float allocchance = 50000.0f;
				static const float freechance = 50000.0f;
				static const float shrinkblockchance = 50000.0f;
				static const float resizeblockchance = 50000.0f;
				static const float allocmethchance = 100.0f;
				static const float fitmethchance = 100.0f;
				static const float makeroomchance = 0.5f;
				static const float scanchance = 1.0f;
				static const float fillchance = 0.5f;
				static const float emptychance = 0.5f;
				static const float totalchance =
					allocchance +
					freechance +
					allocmethchance +
					fitmethchance +
					shrinkblockchance +
					resizeblockchance +
					makeroomchance +
					scanchance +
					fillchance +
					emptychance;
				float r = rand() / (float)0x7FFFFFFF;
				r *= totalchance;

				if(r < allocchance)
				{
					int full = 0;
					while(!fillentry() && ++full < 10);
					if(full >= 10)
						mode = MAKEROOM;
					continue;
				}
				r -= allocchance;

				if(r < freechance)
				{
					freeentry(randomentry());
					continue;
				}
				r -= freechance;

				if(r < shrinkblockchance)
				{
					shrinkentry(randomentry());
					continue;
				}
				r -= shrinkblockchance;

				if(r < resizeblockchance)
				{
					resizeentry(randomentry());
					continue;
				}
				r -= resizeblockchance;

				if(r < allocmethchance)
				{
					allocmethodchanges++;
					memSetAllocMethod(rand() % ALC_MAX);
					continue;
				}
				r -= allocmethchance;

				if(r < fitmethchance)
				{
					fitmethodchanges++;
					memSetFitMethod(rand() % FIT_MAX);
					continue;
				}
				r -= fitmethchance;

				if(r < makeroomchance)
				{
					mode = MAKEROOM;
					continue;
				}
				r -= makeroomchance;

				if(r < scanchance)
				{
					mode = SCAN;
					continue;
				}
				r -= scanchance;

				if(r < fillchance)
				{
					mode = FILL;
					continue;
				}
				r -= fillchance;

				ASSERT(r <= emptychance);

				mode = EMPTY;
			}
			counter = &timethrashing;
			break;
		case MAKEROOM:
			makerooms++;
			// randomly free a bunch of entries...
			if(entries->Count() != 0)
			{
				for(i = rand() % entries->Count(); i > 0; i--)
					freeentry(randomentry());
			}
			mode = THRASH;
			counter = &timemakerooming;
			break;
		}
		asm("mfc0 %0, $9" : "=r" (endtime));
		*counter += endtime - starttime;
	}
}
#endif // UNIT_TEST_MEM_MANAGER
/// @}

