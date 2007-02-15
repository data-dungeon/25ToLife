////////////////////////////////////////////////////////////////////////////
//
// TupMemoryManager
//
// An static memory manager used by the game engine
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_MEMORY_MANAGER_H
#define TUPPERWARE_MEMORY_MANAGER_H

////////////////////////////////////////////////////////////////////////////

// set up the conditions for the memory manager to be turned on
// anytime when using the engine memory manager (too SLOW!!!) 
#if defined(TUPPERWARE_FOR_ENGINE)
#define USE_TUP_MEMORY_MANAGER
#endif

#ifdef USE_TUP_MEMORY_MANAGER
class TupMemoryManager
{
public:
	static void Enable(void);
	static void Disable(void);
	static bool AllBuffersFreed(void);
	static void *Allocate(size_t nSize);
	static void FreeBuffer(void *pBuffer);
};
#endif

////////////////////////////////////////////////////////////////////////////

#endif
