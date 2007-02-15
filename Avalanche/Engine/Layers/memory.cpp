/* memory.cpp */

/******************* includes ****************************/

#include "Layers/LayersPCH.h"
#include "EngineHelper/filestream.h"

// This header defines the colors used on output to be the ansi
// escape codes for those colors.  That way the output can be in
// color.  This works in PS2-SN, and PS2-CW.  It probably works in
// GCN-SN, and possibly works in GCN-CW.  I really have no clue if it
// works in MSVC.
#include "ttyColors.h"


/* -----------------4/27/98 5:30PM-------------------
 * Local Defines
 * --------------------------------------------------*/

//#define MEM_USE_DEBUG_PARANOID_HEAP

#define __SYSTEM__			"MEM"

// User Modified
#define MEM_MAX_REGIONS		4

#ifdef _DEBUG
#define MEM_DBG_CAPS				// Define for Block Debugging
#endif //_DEBUG

// UnModifyable

#ifdef MEM_DBG_CAPS
#define MEM_DBG_OFFSET		MEM_ADDR_ALIGN
#else
#define MEM_DBG_OFFSET		0
#endif

#define MEM_DBG_MARKER		0xDEADDEADL

#ifdef MEM_DEBUG_BLOCKS
#define ALLOC_NAME_LENGTH				(14-4 /*+16*/)	// this plus the size of the u16UseBits field should equal 16
#define MEM_PUSH_ALLOC_NAME(pcName)	{ strcpy(acSaveAllocName, acAllocName); memSetAllocName(pcName); }
#define MEM_POP_ALLOC_NAME()			{ strcpy(acAllocName, acSaveAllocName); }

static char acAllocName[ALLOC_NAME_LENGTH] = "No name";
static char acSaveAllocName[ALLOC_NAME_LENGTH] = "";
static u16 u16UseBits = 0;

#else //MEM_DEBUG_BLOCKS
#define MEM_PUSH_ALLOC_NAME(pcName)	((void)0)
#define MEM_POP_ALLOC_NAME()			((void)0)
#endif //MEM_DEBUG_BLOCKS

#if defined(MEM_USE_DEBUG_PARANOID_HEAP)
#include "memory_debugheap.cpp"
#elif defined(MEM_USE_WIN32_MANAGER)
#include "memory_mallocfree.cpp"
#elif defined(MEM_SEPARATE_HEADERS)
#include "memory_separateheader.cpp"
#else
#include "memory_attachedheader.cpp"
#endif

#include "memory_newdelete.cpp"

#include "container/Darray.h"

static DynamicArray<OutOfMemCallback*> sOutOfMemCallbacks;

void memAddOutOfMemCallback(OutOfMemCallback* callback)
{
	for(int i = 0; i < sOutOfMemCallbacks.Count(); i++)
		if(sOutOfMemCallbacks[i] == callback)
			return;

	sOutOfMemCallbacks.Add(callback);
}

void memRemoveOutOfMemCallback(OutOfMemCallback* callback)
{
	for(int i = 0; i < sOutOfMemCallbacks.Count(); i++)
	{
		if(sOutOfMemCallbacks[i] == callback)
		{
			sOutOfMemCallbacks.Delete(i);
			break;
		}
	}
}

bool memCallOutOfMemCallback(u32 size, u32 align)
{
	for(int i = 0; i < sOutOfMemCallbacks.Count(); i++)
	{
		if((*sOutOfMemCallbacks[i])(size, align))
			return true;
	}
	return false;
}


