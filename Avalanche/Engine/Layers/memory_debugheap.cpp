//============================================================================
//=
//= DebugHeap.cpp - a heap that offers extra ability to find and track
//=                 memory overruns
//=
//= Block Layout:
//=    [header]
//=    [head liner] 0xad
//=    [align fill] 0xbc
//=    [user block]  - is filled with 0xcd when not allocated
//=    [tail liner] 0xdd
//=
//============================================================================

#include "memory_debugheap.h"
#include "Layers/Semaphore/Semaphore.h"

#if defined(WIN32)
#include <new>
#endif

#define HEAD_LINER_SIZE		16
#define TAIL_LINER_SIZE		16
#define MASTER_ALIGNMENT	16
#define MINIMUM_SIZE			16

#define MAGIC_COOKIE			0xbeefcafe
#define HEAD_LINER_FILL		0xad
#define ALIGN_FILL			0xbd
#define ALLOCATED_FILL		0xcd
#define FREE_FILL				0xdd
#define TAIL_LINER_FILL		0xed

#define BLOCK_ALLOCATED		1
#define BLOCK_FREE			2

#define ROUND_UP( x, align )		( ( ( (x) + (align) - 1 ) / (align) ) * (align) )
#define ROUND_DOWN( x, align )	( ( ( (x) + 0           ) / (align) ) * (align) )

//#define KEEP_TIDY		// recollect free blocks into larger free blocks immediately

#ifndef CDROM
#define TRACK_SIZES
#endif

// The per platform IO Buffer information
#if defined(_XBOX)
	#define IO_ALIGN	16
	#define IO_SLOP	XBOX_DVD_SECTOR_SIZE
#elif defined(WIN32)
	#define IO_ALIGN	16
	#define IO_SLOP	0
#elif defined(PS2)
	#define IO_ALIGN	64
	#define IO_SLOP	64
#elif defined(GCN)
	#define IO_ALIGN	32
	#define IO_SLOP	0
#endif

#define ALLOC_NAME_LEN	16
char s_allocName[ ALLOC_NAME_LEN ];

u32 memRetrieveReturnAddress();

//============================================================================

CDebugHeap g_debugHeap;

static CSemaphore* memorySemaphore = 0;

//============================================================================
 
class BlockHeader
{
public:
	int				NeededSize( int size, int align );
	void				InitFree( int blockSize, int previousBlockSize, bool fill );
	void*				InitAllocated( int size, int align );
	void*				Alloc( int size, int align );
	void				Free();
	void				Validate();
	BlockHeader*	Next();
	BlockHeader*	Prev();
	void				MergeWithNext();
	void*				ToData();

	static void		Fill( u8* ptr, u8 data, int len );
	static void		CheckFill( u8* ptr, u8 data, int len );

public:
	u32	d_magicCookie;
	int	d_type;
	int	d_blockSize;
	int	d_alignSize;
	int	d_size;
	int	d_tailSize;
	int	d_prevBlockSize;
	int	d_ra;
	char	d_desc[ ALLOC_NAME_LEN ];
};

//============================================================================

bool _debugMemAssert( const char* file, int line, const char* exp )
{
	scePrintf( "DEBUG_MEM_ASSERT FAIL : %s:%d '%s'\n", file, line, exp );
	*(volatile int*)1 = 0;
}

#define DEBUG_MEM_ASSERT(exp)			( (exp) ? true : _debugMemAssert(__FILE__, __LINE__, #exp) )
#define DEBUG_MEM_ASSERTS(exp,str)	( (exp) ? true : _debugMemAssert(__FILE__, __LINE__, str ) )
#define DEBUG_MEM_ASSERT_PTR(exp)   ( (exp) != NULL ? true : _debugMemAssert(__FILE__, __LINE__, #exp ) )

//============================================================================

#ifdef TRACK_SIZES
	#define MAX_TRACK_SIZE 1024
	class CMemStat
	{
	public:
		CMemStat()
		{
			memset( d_counts, 0, sizeof(d_counts) );
			memset( d_max, 0, sizeof(d_max) );
		}
		void Inc( int size )
		{
			if ( size > 0 && size < MAX_TRACK_SIZE )
			{
				d_counts[size]++;
            if ( d_counts[size] > d_max[size] ) { d_max[size] = d_counts[size]; }
			}
		}
		void Dec( int size )
		{
			if ( size > 0 && size < MAX_TRACK_SIZE ) { d_counts[size]--; }
		}
		void Dump()
		{
			for ( int i = 0 ; i < MAX_TRACK_SIZE ; i++ )
				if ( d_max[i] > 0 )
					dbgPrint( "MEM: size=%-4d current=%-4d max=%-4d\n", i, d_counts[i], d_max[i] );
		}
		void Dump( FileStream* fp )
		{
			for ( int i = 0 ; i < MAX_TRACK_SIZE ; i++ )
			{
				if ( d_max[i] > 0 )
				{
					char buf[ 128 ];
					sprintf( buf, "MEM: size=%-4d current=%-4d max=%-4d\n", i, d_counts[i], d_max[i] );
					fp->Print( buf );
				}
			}
		}
	private:
		int d_counts[ MAX_TRACK_SIZE ];
		int d_max[ MAX_TRACK_SIZE ];
	};
#else
	class CMemStat
	{
		public:
			void Inc( int size ) {}
			void Dec( int size ) {}
			void Dump()          {}
			void Dump(FileStream* fp) {}
	};
#endif

static CMemStat s_memStat;

//============================================================================

void BlockHeader::Fill( u8* ptr, u8 data, int len )
{
	memset( ptr, data, len );
}

//============================================================================

void BlockHeader::CheckFill( u8* ptr, u8 data, int len )
{
	for ( int i = 0 ; i < len ; i++ )
		DEBUG_MEM_ASSERT( ptr[ i ] == data );
}

//============================================================================

BlockHeader* BlockHeader::Next()
{
	BlockHeader* next = (BlockHeader*)( (u8*)this + d_blockSize );
	return ( next >= g_debugHeap.EndAddress() ) ? NULL : next;
}

//============================================================================

BlockHeader* BlockHeader::Prev()
{
	if ( d_prevBlockSize )
		return (BlockHeader*)( (u8*)this - d_prevBlockSize );
	else
		return NULL;
}

//============================================================================

void* BlockHeader::ToData()
{
	return (void*) ( (u32)this + sizeof(BlockHeader) + HEAD_LINER_SIZE + d_alignSize );
}

//============================================================================

int BlockHeader::NeededSize( int size, int align )
{
	// create a virtual block to determine the total size needed to place
	// a block here with this size and alignment.
	u8* ptr = (u8*) this;
	ptr += sizeof(BlockHeader);
	ptr += HEAD_LINER_SIZE;
	ptr  = (u8*)ROUND_UP( (u32)ptr, align );
	ptr += size;
	ptr += TAIL_LINER_SIZE;
	ptr  = (u8*)ROUND_UP( (u32)ptr, MASTER_ALIGNMENT );
	return (u32)ptr - (u32)this;
}

//============================================================================

void BlockHeader::InitFree( int blockSize, int previousBlockSize, bool fill )
{
	DEBUG_MEM_ASSERT( ROUND_UP( blockSize, MASTER_ALIGNMENT ) == blockSize );
	d_magicCookie   = MAGIC_COOKIE;
	d_type		    = BLOCK_FREE;
	d_blockSize	    = blockSize;
	d_alignSize     = 0;
	d_size			 = blockSize - sizeof(BlockHeader) - HEAD_LINER_SIZE - TAIL_LINER_SIZE;
	d_tailSize      = TAIL_LINER_SIZE;
	d_prevBlockSize = previousBlockSize;
	d_ra            = 0;
	d_desc[0]       = 0;

	u8* ptr = (u8*) this;
	ptr += sizeof(BlockHeader);

	Fill( ptr, HEAD_LINER_FILL, HEAD_LINER_SIZE );
	ptr += HEAD_LINER_SIZE;

	Fill( ptr, ALIGN_FILL, d_alignSize );
	ptr += d_alignSize;

	if ( fill )
		Fill( ptr, FREE_FILL, d_size );
	ptr += d_size;

	Fill( ptr, TAIL_LINER_FILL, d_tailSize );
	ptr += d_tailSize;

	DEBUG_MEM_ASSERT( ptr - (u8*)this == d_blockSize );

	// poke into next guy and set his previous free
	BlockHeader* next = Next();
	if ( next )
		next->d_prevBlockSize = blockSize;
}

//============================================================================

void* BlockHeader::InitAllocated( int size, int align )
{
	// assumes block header is filled in correctly
	DEBUG_MEM_ASSERT( d_magicCookie == MAGIC_COOKIE );
	d_type = BLOCK_ALLOCATED;
	d_ra   = memRetrieveReturnAddress();

	u8* ptr = (u8*) this;
	ptr += sizeof(BlockHeader);

	Fill( ptr, HEAD_LINER_FILL, HEAD_LINER_SIZE );
	ptr += HEAD_LINER_SIZE;

	d_alignSize = ROUND_UP( (u32)ptr, align ) - (u32)ptr;
	Fill( ptr, ALIGN_FILL, d_alignSize );
	ptr += d_alignSize;

	void* returnPtr = ptr;
	d_size = size;
	Fill( ptr, ALLOCATED_FILL, d_size );
	ptr += d_size;

	d_tailSize = ( (u32)this + d_blockSize ) - (u32)ptr;
	DEBUG_MEM_ASSERT( d_tailSize >= TAIL_LINER_SIZE );
	Fill( ptr, TAIL_LINER_FILL, d_tailSize );
	ptr += d_tailSize;

	DEBUG_MEM_ASSERT( ptr - (u8*)this == d_blockSize );

	strncpy( d_desc, s_allocName, sizeof(d_desc) );
	d_desc[ sizeof(d_desc) - 1 ] = 0;
	s_allocName[0] = 0;

	return returnPtr;
}

//============================================================================

void* BlockHeader::Alloc( int size, int align )
{
	if ( d_type == BLOCK_FREE )
	{
		int needed = NeededSize( size, align );
		if ( d_blockSize >= needed )
		{
			// check us out
			Validate();

			// this block can be used.  can it be split in two?
			BlockHeader* block2 = (BlockHeader*) ( (u8*)this + needed );
			int block2Needed = block2->NeededSize( MINIMUM_SIZE, 1 );
			if ( d_blockSize >= needed + block2Needed )
			{
				// setup next block
				block2->InitFree( d_blockSize - needed, needed, false );
				if ( g_debugHeap.FirstFree() == this )
					g_debugHeap.SetFirstFree( block2 );

				// change our block size
				d_blockSize = needed;
			}

			// stats
			s_memStat.Inc( size );

			// and allocate this block
			return InitAllocated( size, align );
		}
	}

	return NULL;
}

//============================================================================

void BlockHeader::Free()
{
	DEBUG_MEM_ASSERT( d_type == BLOCK_ALLOCATED );
	Validate();
	s_memStat.Dec( d_size );
	InitFree( d_blockSize, d_prevBlockSize, true );

	// keep everything tidy (recollect free blocks together). safely removable
	// to give dead blocks a longer lifetime for easier debugging.
#ifdef KEEP_TIDY
	BlockHeader* prev = Prev();
	BlockHeader* b = ( prev && prev->d_type == BLOCK_FREE ) ? prev : this;
	b->MergeWithNext();
	if ( b < g_debugHeap.FirstFree() )
		g_debugHeap.SetFirstFree( b );
#endif
}

//============================================================================

void BlockHeader::Validate()
{
	DEBUG_MEM_ASSERT( d_magicCookie == MAGIC_COOKIE );
	DEBUG_MEM_ASSERT( d_type == BLOCK_ALLOCATED || d_type == BLOCK_FREE );

	u8* ptr = (u8*)this + sizeof(BlockHeader);
	CheckFill( ptr, HEAD_LINER_FILL, HEAD_LINER_SIZE );
	ptr += HEAD_LINER_SIZE;

	CheckFill( ptr, ALIGN_FILL, d_alignSize );
	ptr += d_alignSize;

//	if ( d_type == BLOCK_FREE )
//		CheckFill( ptr, FREE_FILL, d_size );
	ptr += d_size;

	CheckFill( ptr, TAIL_LINER_FILL, d_tailSize );
	ptr += d_tailSize;

	DEBUG_MEM_ASSERT( ptr == (u8*)this + d_blockSize );
	DEBUG_MEM_ASSERT( ROUND_UP( (u32)ptr, MASTER_ALIGNMENT ) == (u32)ptr );
}

//============================================================================

void BlockHeader::MergeWithNext()
{
	if ( d_type == BLOCK_FREE )
	{
		int size = 0;
		for ( BlockHeader* b = this ; b && b->d_type == BLOCK_FREE ; b = b->Next() )
			size += b->d_blockSize;

		if ( size > d_blockSize )
			InitFree( size, d_prevBlockSize, false );
	}
}

//============================================================================
//============================================================================
//============================================================================

void CDebugHeap::Init( void* memory, int size )
{
	// remember master memory chunk, for the hell of it
	d_memory = memory;

	// align, and rememeber our addresses
	d_startAddress = (void*) ROUND_UP( (u32)d_memory, MASTER_ALIGNMENT );
	d_endAddress   = (void*) ROUND_DOWN( (u32)d_memory + size, MASTER_ALIGNMENT );

	// compiler sanity checks
	DEBUG_MEM_ASSERT( ROUND_UP( sizeof(BlockHeader), MASTER_ALIGNMENT ) == sizeof(BlockHeader) );
	DEBUG_MEM_ASSERT( ROUND_UP( HEAD_LINER_SIZE, MASTER_ALIGNMENT ) == HEAD_LINER_SIZE );
	DEBUG_MEM_ASSERT( ROUND_UP( TAIL_LINER_SIZE, MASTER_ALIGNMENT ) == TAIL_LINER_SIZE );

	// set up our main block
	d_blocks = (BlockHeader*) d_startAddress;
	d_blocks->InitFree( (u32)d_endAddress - (u32)d_startAddress, 0, true );
	d_firstFree = d_blocks;
}

//============================================================================

void* CDebugHeap::Alloc( int size, int align, bool canFreeRamToDoIt )
{
	BlockHeader* b;
	void* ptr;

	do
	{
		// start at the head and walk til we find a free block that fits us
		for ( b = d_firstFree ; b ; b = b->Next() )
			if ( ptr = b->Alloc( size, align ) )
				return ptr;
	
		// out of memory.  compact blocks and try again.
		CompactFreeBlocks();
	
		// try again
		for ( b = d_firstFree ; b ; b = b->Next() )
			if ( ptr = b->Alloc( size, align ) )
				return ptr;
	}
	while ( canFreeRamToDoIt && memCallOutOfMemCallback( size, align ) );

	// okay, we're really out of memory
	DEBUG_MEM_ASSERTS( false, "Out Of Memory!" );
	return NULL;
}

//============================================================================

void* CDebugHeap::Resize( void* ptr, int size, int align )
{
	// don't bother trying to resize, just make a new block
	void* newBuffer = Alloc( size, align );

	// if we have an old block, move the data over
	if ( ptr )
	{
		// determine the size of the old block
		DEBUG_MEM_ASSERT( ptr >= d_startAddress && ptr < d_endAddress );
		BlockHeader* b = FindBlockHeader( ptr );
		int copyBytes = ( size < b->d_size ) ? size : b->d_size;

		// copy the data over and free the old
		memcpy( newBuffer, ptr, copyBytes );
		Free( ptr );
	}

	return newBuffer;
}

//============================================================================

void CDebugHeap::Free( void* ptr )
{
	// better belong to us
	DEBUG_MEM_ASSERT_PTR( ptr );
	DEBUG_MEM_ASSERT( ptr >= d_startAddress && ptr < d_endAddress );

	// find the block header
	BlockHeader* b = FindBlockHeader( ptr );

	// and free
	b->Free();
}

//============================================================================

BlockHeader* CDebugHeap::FindBlockHeader( void* ptr )
{
	u8* p = (u8*) ptr;
	while ( p[-1] == ALIGN_FILL )
		p--;

	BlockHeader* b = (BlockHeader*) ( p - HEAD_LINER_SIZE - sizeof(BlockHeader) );
	b->Validate();
	return b;
}

//============================================================================

bool CDebugHeap::CouldAlloc( u32 size, u32 align, bool canFreeRamToDoIt )
{
	BlockHeader* b;
	void* ptr;

	do
	{
		// start at the head and walk til we find a free block that fits us
		for ( b = d_firstFree ; b ; b = b->Next() )
		{
			if ( ptr = b->Alloc( size, align ) )
			{
				Free(ptr);
				return true;
			}
		}
	
		// out of memory.  compact blocks and try again.
		CompactFreeBlocks();
	
		// try again
		for ( b = d_firstFree ; b ; b = b->Next() )
		{
			if ( ptr = b->Alloc( size, align ) )
			{
				Free(ptr);
				return true;
			}
		}
	}
	while ( canFreeRamToDoIt && memCallOutOfMemCallback( size, align ) );

	// okay, we're really out of memory
	return false;
}

//============================================================================

void CDebugHeap::CompactFreeBlocks()
{
	for ( BlockHeader* b = d_blocks ; b ; b = b->Next() )
	{
		if ( b->d_type == BLOCK_FREE )
		{
			b->MergeWithNext();

			if ( b < d_firstFree )
				d_firstFree = b;
		}
	}

	Validate();
}

//============================================================================

void CDebugHeap::Validate()
{
	int prev = 0;
	for ( BlockHeader* b = d_blocks ; b ; b = b->Next() )
	{
		b->Validate();
		DEBUG_MEM_ASSERT( b->d_prevBlockSize == prev );
		prev = b->d_blockSize;
	}
}

//============================================================================

void CDebugHeap::Dump()
{
	printf("\n");
	for ( BlockHeader* b = d_blocks ; b ; b = b->Next() )
	{
		printf( "%08X t=%d s=%06X as=%04X d=%08X bs=%06X ps=%06X %s\n",
					b,   b->d_type, b->d_size, b->d_alignSize, (u32)b->ToData(), b->d_blockSize, b->d_prevBlockSize,
					b == FirstFree() ? "F" : "" );
	}
}

//============================================================================

void* CDebugHeap::StartAddress()
{
	return d_startAddress;
}

//============================================================================

void* CDebugHeap::EndAddress()
{
	return d_endAddress;
}

//============================================================================

BlockHeader* CDebugHeap::FirstFree()
{
	return d_firstFree;
}

//============================================================================

void CDebugHeap::SetFirstFree( BlockHeader* firstFree )
{
	d_firstFree = firstFree;
}

//============================================================================

void CDebugHeap::GetStats( int* o_freeBytes, int* o_freeHandles, int* o_largestFree, int* o_allocBytes, int* o_allocHandles )
{
	int freeBytes    = 0;
	int freeHandles  = 0;
	int largestFree  = 0;
	int allocBytes   = 0;
	int allocHandles = 0;

	for ( BlockHeader* b = d_blocks ; b ; b = b->Next() )
	{
		if ( b->d_type == BLOCK_FREE )
		{
			freeBytes   += b->d_size;
			freeHandles += 1;
			if ( b->d_size > largestFree )
				largestFree = b->d_size;
		}
		else
		{
			allocBytes   += b->d_size;
			allocHandles += 1;
		}
	}

	if ( o_freeBytes    ) *o_freeBytes    = freeBytes;
	if ( o_freeHandles  ) *o_freeHandles  = freeHandles;
	if ( o_largestFree  ) *o_largestFree  = largestFree;
	if ( o_allocBytes   ) *o_allocBytes   = allocBytes;
	if ( o_allocHandles ) *o_allocHandles = allocHandles;
}

//============================================================================

#ifdef MEM_USE_DEBUG_PARANOID_HEAP

void* memAlloc( u32 size, u32 align, bool canFreeRamToDoIt )
{
	MEM_SET_RETURN_ADDRESS();
	CScopeLockSemaphore lock(memorySemaphore);
	return g_debugHeap.Alloc( size, align, canFreeRamToDoIt );
}

bool memFree( void* ptr )
{
	if ( ptr )
	{
		CScopeLockSemaphore lock(memorySemaphore);
		g_debugHeap.Free( ptr );
	}
	return true;
}

void*	memResize( void* ptr, u32 size, u32 align )
{
	MEM_SET_RETURN_ADDRESS();
	CScopeLockSemaphore lock(memorySemaphore);
	return g_debugHeap.Resize( ptr, size, align );
}

t_Handle	memAddRegion( unsigned char* start, u32 length, u32 flags, char* desc, u32 handles, unsigned short fitMethod, CMemPool* pools )
{
	g_debugHeap.Init( start, length );
	if ( !memorySemaphore )
	{
		memorySemaphore = (CSemaphore*) memAlloc( sizeof(CSemaphore) );
		new (memorySemaphore) CSemaphore;
	}
	return (t_Handle) &g_debugHeap;
}

bool memCouldAlloc(u32 u32Size, u32 u32Align, bool canFreeRamToDoIt)
{
	// if so, this function would find the first fit that would hold it.
	return g_debugHeap.CouldAlloc( u32Size, u32Align, canFreeRamToDoIt );
}

void	memInit()
{
}

void	memValidate()
{
	g_debugHeap.Validate();
}

void* memAllocForIO( u32 size )
{
	MEM_SET_RETURN_ADDRESS();
	u32 realSize = (IO_SLOP == 0) ? size : ((size + IO_SLOP - 1) & ~(IO_SLOP - 1));
	return memAlloc(realSize, IO_ALIGN);
}

void* memAllocForIO( u32 size, u32 align )
{
	MEM_SET_RETURN_ADDRESS();
	u32 realSize  = (IO_SLOP == 0) ? size : ((size + IO_SLOP - 1) & ~(IO_SLOP - 1));
	u32 realAlign = (align > IO_ALIGN) ? align : IO_ALIGN;
	return memAlloc(realSize, realAlign);
}

void memDumpBlockInfo( const char* filename )
{
	if ( filename && filename[0] )
	{
		FileStream fp;
		if ( fp.Open( filename, FileStream::WRITE, true ) )
		{
			s_memStat.Dump( &fp );
			fp.Close();
		}
	}
	else
	{
		s_memStat.Dump();
	}
}

void	memSetAllocName( char* name )
{
	strncpy( s_allocName, name, sizeof(s_allocName) );
	s_allocName[ sizeof(s_allocName) - 1 ] = 0;
}

void	memSetAllocFileName( char* type, const char* fileName )
{
	int offset = strlen( fileName ) + 1 - sizeof(s_allocName);	//+1 for null
	if ( offset < 0 ) offset = 0;
	memSetAllocName( (char*)fileName + offset );
}

void	memMarkPermanentMemory( t_Handle regionHandle )		{}
void	memSetFitScheme( s32 method )								{}
void	memSetFitMethod( s32 method )								{}
void	memSetAllocMethod( s32 method )							{}
int	memGetRegionLength( t_Handle handle )					{ return 0; }
bool	memCheckPermanentMemory( t_Handle regionHandle )	{ return true; }
void	memTrackHighWater( t_Handle Handle, bool state )	{}
bool	memHighWaterOn( t_Handle handle )						{ return false; }
t_Handle	memSetActiveHandle( t_Handle handle )				{ return handle; }
void*	memShrink( void* addr, u32 newSize )					{ return addr; }
void	memSetBlockName( void* addr, char* name )				{}
void	memResetHighWater( t_Handle handle)						{}

#ifdef MEM_DEBUG_BLOCKS
void	memClearUseBit( const u16 bit )							{}
void	memSetUseBit( const u16 bit )								{}
void	memShowRegionFreeSize( t_Handle handle )				{}
void	memShowRegionInfo( t_Handle handle )					{}
void	memShowRegionBriefInfo( t_Handle handle )				{}
void	memShowRegionAllocSize( t_Handle handle )				{}
int memQueryBlockBit(const u16 bit, const char* blockName, int* handleCount, const bool matchAllBits, const bool detail, FileStream* logFile)
{
	if ( handleCount ) *handleCount = 0;
	return 0;
}
void memCheckHeapUsage( t_Handle handle, u32* layersUsage, u32* layersTotal, u32* heapUsage, u32* heapTotal, u32* totalHandles )
{
}
#endif

void memGetRegionStats( t_Handle handle, int* freeBytes, int* freeHandles, int* largestFree, int* allocBytes, int* allocHandles )
{
	g_debugHeap.GetStats( freeBytes, freeHandles, largestFree, allocBytes, allocHandles );
}

void memGetHighWater( t_Handle handle, int* minBytes, int* maxBytes )
{
	if ( minBytes ) *minBytes = 0;
	if ( maxBytes ) *maxBytes = 0;
}

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

#endif //MEM_USE_DEBUG_PARANOID_HEAP

//============================================================================

#if 0 // TEST

int main( int argc, char** argv )
{
	#define MASTER_SIZE 550000
	#define BLOCKS 100

	srand( time(0) );

	void* ptr = (void*)malloc( MASTER_SIZE );
	printf( "\nafter init:\n" );
	g_debugHeap.Init( ptr, MASTER_SIZE );
	g_debugHeap.Validate();
	g_debugHeap.Dump();

	u8* blocks[ BLOCKS ];
	for ( int i = 0 ; i < BLOCKS ; i++ )
		blocks[i] = 0;

	int totalAllocated = 0;
	int totalFreed = 0;

	for ( int i = 0 ; i < 10000 ; i++ )
	{
		int op = rand() % 100;
		if ( op < 40 )
		{
			// alloc a block
			int scan[BLOCKS], numScan = 0;
			for ( int j = 0 ; j < BLOCKS ; j++ )
				if ( !blocks[j] )
					scan[ numScan++ ] = j;
			if ( numScan )
			{
				int which = scan[ rand() % numScan ];
				DEBUG_MEM_ASSERT( !blocks[which] );
				int size  = ( which * 91 );
				int align = ( ( which * 7 ) % 32 ) + 1;
				totalAllocated += size;
				printf( "alloc %d (%d,%d tot=%d-%d=%d)\n", which, size, align, totalAllocated, totalFreed, totalAllocated-totalFreed );
				blocks[which] = (u8*)g_debugHeap.Alloc( size, align );
				DEBUG_MEM_ASSERT( blocks[which] );
				DEBUG_MEM_ASSERT( ( (u32)blocks[which] % align ) == 0 );
				DEBUG_MEM_ASSERT( (u32)blocks[which] >= (u32)ptr && (u32)blocks[which] + size <= (u32)ptr + MASTER_SIZE );
				memset( blocks[which], size, size );
				g_debugHeap.Validate();
			}
		}
		else if ( op < 80 )
		{
			// free a block 
			int scan[BLOCKS], numScan = 0;
			for ( int j = 0 ; j < BLOCKS ; j++ )
				if ( blocks[j] )
					scan[ numScan++ ] = j;
			if ( numScan )
			{
				int which = scan[ rand() % numScan ];
				DEBUG_MEM_ASSERT( blocks[which] != 0 );
				int size  = ( which * 91 );
				int align = ( ( which * 7 ) % 32 ) + 1;
				totalFreed += size;
				printf( "free %d (%d,%d tot=%d-%d=%d)\n", which, size, align, totalAllocated, totalFreed, totalAllocated-totalFreed );
				DEBUG_MEM_ASSERT( ( (u32)blocks[which] % align ) == 0 );
				for ( j = 0 ; j < size ; j++ )
					DEBUG_MEM_ASSERT( blocks[which][j] == (u8)size );

				g_debugHeap.Free( blocks[which] );
				blocks[which] = 0;
				printf("after alloc\n"); g_debugHeap.Dump();
				g_debugHeap.Validate();
			}
		}
	}

	printf( "\nafter working for a bit:\n" );
	g_debugHeap.Dump();

	// after free all
	for ( int i = 0 ; i < BLOCKS ; i++ )
	{
		if ( blocks[i] )
		{
			g_debugHeap.Free( blocks[i] );
			g_debugHeap.Validate();
			blocks[i] = 0;
		}
	}

	printf( "\nafter free all:\n" );
	g_debugHeap.Dump();

	printf( "\nafter compact\n" );
	g_debugHeap.CompactFreeBlocks();
	g_debugHeap.Dump();

	getch();
}

#endif //

//============================================================================

