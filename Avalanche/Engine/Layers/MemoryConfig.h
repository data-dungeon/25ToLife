//============================================================================
//=
//= MemoryConfig.h -
//=
//=   This is the structure for initializing the memory manager.
//=
//=   Since the memory manager must be initialized during the constructors
//=   phase, we can't wait for main() to initialize.  When "new" is called
//=   it will auto-initialize.
//=
//=   To accomplish this, the engine requires that one function is
//=   exists called "GetApplicationMemoryConfig".  The memory mamanger
//=   will call this at initialization time - it will fill in this
//=   structure.
//=
//============================================================================

#ifndef _MEMORY_CONFIG_H
#define _MEMORY_CONFIG_H

struct CMemPool;

struct ts_MemoryConfig
{
	u32			Size;
	u32			OSReserve;	// space to hold back for OS.
	u32			Secondary;	// size to put in secondary pool
	u32			Handles;
	CMemPool*	MainRamPools;
	CMemPool*	AudioRamPools;
};

extern ts_MemoryConfig g_MemoryConfig;

// the appliation must define this function.
extern void GetApplicationMemoryConfig( ts_MemoryConfig* config );

#endif _MEMORY_CONFIG_H

//============================================================================
