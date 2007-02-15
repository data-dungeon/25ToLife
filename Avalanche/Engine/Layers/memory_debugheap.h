//============================================================================
//=
//= DebugHeap.h - a heap that offers extra ability to find and track
//=               memory overruns
//=
//============================================================================

#ifndef _DEBUG_HEAP_H
#define _DEBUG_HEAP_H

//============================================================================

class BlockHeader;

//============================================================================

class CDebugHeap
{
public:
	void				Init( void* memory, int size );
	void*				Alloc( int size, int align, bool canFreeRamToDoIt=true );
	void*				Resize( void* ptr, int size, int align );
	void				Free( void* ptr );
	void				Validate();
	void				Dump();
	void				CompactFreeBlocks();
	bool				CouldAlloc( u32 size, u32 align, bool canFreeRamToDoIt=true );
	void				GetStats( int* freeBytes, int* freeHandles, int* largestFree, int* allocBytes, int* allocHandles );

protected:
	friend class BlockHeader;
	void*				StartAddress();
	void*				EndAddress();
	BlockHeader*	FirstFree();
	void				SetFirstFree( BlockHeader* ptr );
	BlockHeader*	FindBlockHeader( void* ptr );

	BlockHeader*	d_blocks;
	BlockHeader*	d_firstFree;
	void*				d_memory;
	void*				d_startAddress;
	void*				d_endAddress;
};

extern CDebugHeap g_debugHeap;

//============================================================================

#endif //_DEBUG_HEAP_H