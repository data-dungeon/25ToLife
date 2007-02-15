//
// MasterList.cpp
// Copyright (C) 2001 by Avalanche Software, All Rights Reserved
//
// Author: Tyler Colbert
// Creation Date: Nov 7, 2001
// 
// quite a bit of code taken from old version of MasterList by Ned Martin
//
// Added ability to handle MSKPATH3 texture uploading
//

// The usage of this module has changed some since the old version.
// Now you call MasterList__Start() to begin a frame, then, at the end
// of a frame, call MasterList__End(), swap the frame buffers, then call
// MasterList__Render() to start the new chain.
// note that anything between End and Render should be SHORT as the render
// pipe is not running anything at that time.  It is possible, and infact
// ideal to have them one right after the other.


#ifndef __PS2RENDERER_MASTERLIST_H
#define __PS2RENDERER_MASTERLIST_H

#include "../Common/CompilerControl.h"

#define USE_SPR_BUFFERS

#ifndef ASSERT
#include <stdio.h>
#if defined(PS2) && defined(SN)
#define ASSERT(x) {																		\
	if(!(x)){																				\
		printf("Assertion failed %s %d: %s\n", __FILE__, __LINE__, #x);	\
 		asm("break 1");																	\
	}																							\
}
#else //defined(PS2) && defined(SN)
#define ASSERT(x) {																		\
	if(!(x)){																				\
		printf("Assertion failed %s %d: %s\n", __FILE__, __LINE__, #x);	\
		asm("breakc 0x0");																\
	}																							\
}
#endif // defined(PS2) && defined(SN)
#endif // ASSERT

#ifdef _DEBUG
#define DBGASSERT(x) ASSERT(x)
#else
#define DBGASSERT(x) do {} while(0)
#endif

// Number of milliseconds to wait for dma to complete before
// considering it an error: 1000 = 1s.
// this maxes out around 14 seconds.
#define MAX_WAIT_MILLISECONDS	(10000u)
// Number of CPU cycles to wait for dma to complete before considering it an error
#define MAX_WAIT_CYCLES			(294912u * MAX_WAIT_MILLISECONDS)
// TRUE to reset the dmac when the masterlist doesnt complete in time.
#define RESET_DMAC_ON_ERROR	TRUE

// if defined, MasterList will generate hashs for each MasterListChunk created, and verify the
// hashes when the chunk has completed rendering...
//#define HASH_MASTERLIST_CHUNKS

// the hashing code requires USE_SPR_BUFFERS.
#ifdef HASH_MASTERLIST_CHUNKS
#ifndef USE_SPR_BUFFERS
#error Hashing code requires USE_SPR_BUFFERS
#endif
#endif

#include <string.h> // for memcpy

#ifndef SHIP_IT
#include <stdio.h>	//for DMA_WAIT
#endif

#if defined(PS2) && defined(SN)
#include <sn_registers.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// headers
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

// These are absolutely vital to avoid having the engine creep into the renderer
// remember when impliemnting things in the renderer that it may be used outside
// the engine with some projects.  Please try to keep it clean of engine-isms and
// game-isms.

// ps2 includes

#include <eeregs.h>
#include <eetypes.h>

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// compile control and similar odds and ends...
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#define MASTERLIST_CHUNK_SIZE		(4 * 1024)
#define MASTERLIST_QW_CHUNK_SIZE	(MASTERLIST_CHUNK_SIZE >> 4)

#define MASTERLIST_MAX_CHAIN_SIZE	(MASTERLIST_CHUNK_SIZE - 16)
#define MASTERLIST_QW_MAX_CHAIN_SIZE (MASTERLIST_MAX_CHAIN_SIZE >> 4)

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// forward declarations
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

// These are absolutely vital to avoid having the engine creep into the renderer
// remember when impliemnting things in the renderer that it may be used outside
// the engine with some projects.  Please try to keep it clean of engine-isms and
// game-isms

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// here lies the public interface for the master list.
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace MasterList {

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// defines
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	// for dma control
#define D_CHCR_SCHAIN_MODE_M  		(0x01  << 2)

	// bit to set if addr field refers to spr instead of main mem...
#define DMA_ADDR_FROM_SPR				(0x80000000)

	// id's for dma tag
#define TAGID_REFE	(0)
#define TAGID_CNT		(1)
#define TAGID_NEXT	(2)
#define TAGID_REF		(3)
#define TAGID_REFS	(4)
#define TAGID_CALL	(5)
#define TAGID_RET		(6)
#define TAGID_END		(7)

	// some general info
#define BYTES_PER_QUAD_WORD		16		// a quad word is 4 32-bit words, or 128 bits, or 16 bytes

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// macros for building dma chains
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	// flush a specific range out of the data
	// cache.  This should be much faster than flushing
	// the whole thing unless you pass a fairly high
	// qwc, something over 300 or so (depending on how
	// many lines are dirty, 300 assumes about half
	// of the cache is dirty).  Definitely should be
	// moved to another header...
	inline void FlushCacheRange(void* src, u_int qwc)
	{
		asm __volatile__("
			sync.l
			add t0, %0, $0
		0:
			cache 0x1c, 0x0(t0)
			addiu t0, t0, 0x40
			nop
			addi %1, %1, -4
			nop
			bgtz %1, 0b
			nop
			sync.l
			" : : "r" (src), "r" (qwc) : "t0");
	}

	// wait for a dma to complete...
	// TODO: this is not very good as far as buss usage goes.  It should be changed to not hit the buss so hard.
	// Please excuse the do{}while instead of while;.   while; issues a warning under metrowerks..
	// includes timing info for debug builds...
	// there are 2 versions of this, the debug one times itself to make sure it doesnt wait too long..
#define WAIT_MSEC_BEFORE_SLEEPING (1u)
#define WAIT_CYCLES_BEFORE_SLEEPING (294912u * WAIT_MSEC_BEFORE_SLEEPING)

#if !defined(SHIP_IT) && defined(IN_MASTERLIST_CPP)
extern u_int gCurrentDMAWaitCycleCount;
#define DMA_Wait(_chcr)																																	\
	{																																							\
		u_int startCycleCount=0, endCycleCount=0;																									\
		asm __volatile__("mfc0 %0,$9" : "=r" (startCycleCount));																				\
		do {																																					\
			asm __volatile__("mfc0 %0,$9" : "=r" (endCycleCount));																			\
			if(endCycleCount - startCycleCount > MAX_WAIT_CYCLES){																			\
				char buffer[200];																															\
				sprintf(buffer,"DMA_Wait(%s) failed to complete in a reasonable time (%s:%d)", #_chcr, __FILE__, __LINE__);	\
				MasterList::ReportAboutDMA(buffer, RESET_DMAC_ON_ERROR, false);															\
				break;																																		\
			}																																					\
			if(endCycleCount - startCycleCount > WAIT_CYCLES_BEFORE_SLEEPING)																\
			{																																					\
				::DelayThread(10);																														\
			}																																					\
		} while(((*( (volatile u_int *) _chcr)) & D_CHCR_STR_M) == D_CHCR_STR_M);														\
		asm __volatile__("mfc0 %0,$9" : "=r" (endCycleCount));																				\
		MasterList::gCurrentDMAWaitCycleCount += endCycleCount - startCycleCount;														\
	}
#else
#define DMA_Wait(_chcr)																																	\
	{																																							\
		u_int startCycleCount=0, endCycleCount=0;																									\
		asm __volatile__("mfc0 %0,$9" : "=r" (startCycleCount));																				\
		do {																																					\
			asm __volatile__("mfc0 %0,$9" : "=r" (endCycleCount));																			\
			if(endCycleCount - startCycleCount > WAIT_CYCLES_BEFORE_SLEEPING)																\
			{																																					\
				::DelayThread(10);																														\
			}																																					\
		} while(((*( (volatile u_int *) _chcr)) & D_CHCR_STR_M) == D_CHCR_STR_M);														\
		asm __volatile__("mfc0 %0,$9" : "=r" (endCycleCount));																				\
	}
#endif


	// TJC - comment from someone else --
	// check if we need to wait for scratchpad-to-memory DMA to finish. i ran into a bug in processing boned models where
	// MasterList__CallDisplayList was being called repeatedly and rapidly. there was no callback needed, and no vertex weighting.
	// so the same matrix packet was being added to the master display list over and over, with the only difference being the
	// call address. what was happening is that i was writing over the previous call address before the DMA had a chance
	// to write the packet into the master display list, with the effect that i got a mismatch between model and transform
	// matrix. so if we are about to modify a packet that was the last one to be DMA'd wait for DMA to finish
#if 0
#define CHECK_LAST_FROM_SPR_DMA(source) 											\
	{																							\
		if ((u_int) (source) == MasterList::u32LastMasterListDMASource)	\
			DMA_Wait(D8_CHCR);															\
	}
#endif

#define DMA_FROM_SPR(source, dest, qwc)									\
	{																					\
		DMA_Wait(D8_CHCR);														\
		DPUT_D8_MADR((u_int) dest & 0x0FFFFFFF);							\
		DPUT_D8_SADR((u_int)(source));										\
		DPUT_D8_QWC(qwc);															\
		asm("sync.l");																\
		asm("sync.p");																\
		DPUT_D8_CHCR(D_CHCR_STR_M);											\
		asm("sync.l");																\
		asm("sync.p");																\
	}

//		MasterList::u32LastMasterListDMASource = (u_int) (source);	


#define DMA_TO_SPR(source, dest, qwc)				\
	{															\
		MasterList::FlushCacheRange(source, qwc);	\
		DMA_Wait(D9_CHCR);								\
		DPUT_D9_MADR((u_int)(source));				\
		DPUT_D9_SADR((u_int)(dest));					\
		DPUT_D9_QWC(qwc);									\
		asm("sync.l");										\
		asm("sync.p");										\
		DPUT_D9_CHCR(D_CHCR_STR_M);					\
		asm("sync.l");										\
		asm("sync.p");										\
	}

	// for copying pre-built packets from main memry into master display list
#define COPY_TO_MASTER_LIST(source, qwc)	MasterList::CopyToMasterList(source, qwc)

#define D_CHCR_MOD_INTERLEAVE (0x2 << D_CHCR_MOD_O)
#define DMA_TO_SPR_INTERLEAVED(source, dest, itemSize, copyItemsWide, copyItemsTall, bufferItemsWide)	\
{																																		\
	u32 is = (itemSize);																											\
	u32 ciw = (copyItemsWide);																									\
	u32 cit = (copyItemsTall);																									\
	u32 biw = (bufferItemsWide);																								\
	u32 csw = ciw * is;																											\
   DMA_Wait(D8_CHCR);																											\
   DMA_Wait(D9_CHCR);																											\
	DPUT_D_SQWC(((biw - ciw) * is) | (csw << 16));																		\
	DPUT_D9_QWC(cit * csw);																										\
	DPUT_D9_MADR((u_int)(source));																							\
	DPUT_D9_SADR((u_int)(dest));																								\
	DPUT_D9_CHCR(D_CHCR_STR_M | D_CHCR_MOD_INTERLEAVE);																\
}

#define DMA_FROM_SPR_INTERLEAVED(source, dest, itemSize, copyItemsWide, copyItemsTall, bufferItemsWide)	\
{																																			\
	u32 is = (itemSize);																												\
	u32 ciw = (copyItemsWide);																										\
	u32 cit = (copyItemsTall);																										\
	u32 biw = (bufferItemsWide);																									\
	u32 csw = ciw * is;																												\
   DMA_Wait(D9_CHCR);																												\
   DMA_Wait(D8_CHCR);																												\
	DPUT_D_SQWC(((biw - ciw) * is) | (csw << 16));																			\
	DPUT_D8_QWC(csw * cit);																											\
	DPUT_D8_MADR(((u_int)(dest)) & 0x0FFFFFFF);																				\
	DPUT_D8_SADR((u_int)(source));																								\
	DPUT_D8_CHCR(D_CHCR_STR_M | D_CHCR_MOD_INTERLEAVE);																	\
}

//	MasterList::u32LastMasterListDMASource = (u_int) (source);															\

	// for dma'ing into pre-built packets in scratchpad from main ram
	// dont forget to wait for it to get there before moving to master
	// currently, in my test cases, the memcpy is faster because the
	// test data is ALWAYS sitting in the cache, so I switched to memcpy
	// for now... the larger the data, the stronger the argument for DMA
#if 0
#define APPEND_DMA(addr, src, qwc)				\
	{														\
		DMA_TO_SPR(src, addr, qwc);				\
		addr += ((qwc) * BYTES_PER_QUAD_WORD);	\
	}
#else
#if 0
#define APPEND_DMA(addr, src, qwc)							\
	{																	\
		memcpy(addr, src, qwc * BYTES_PER_QUAD_WORD);	\
		addr += ((qwc) * BYTES_PER_QUAD_WORD);				\
	}
#endif
#endif

// macros to build display list packets in scratchpad, from steve' anderson's vu.c
#define APPEND4(addr, val)  MasterList::Append4(addr, val)
#define APPEND4f(addr, val) MasterList::AppendF(addr, val)
#define APPEND8(addr, val)  MasterList::Append8(addr, val)
#define APPEND16(addr, val) MasterList::Append16(addr, val)

#define APPEND4_TO_MASTER_LIST(val)  MasterList::Append4(val)
#define APPEND4f_TO_MASTER_LIST(val) MasterList::AppendF(val)
#define APPEND8_TO_MASTER_LIST(val)  MasterList::Append8(val)
#define APPEND16_TO_MASTER_LIST(val) MasterList::Append16(val)

#define DUMP_MTX(mtx)																				\
{																											\
	printf("%f %f %f %f\n", (mtx)[0][0], (mtx)[0][1], (mtx)[0][2], (mtx)[0][3]);	\
	printf("%f %f %f %f\n", (mtx)[1][0], (mtx)[1][1], (mtx)[1][2], (mtx)[1][3]);	\
	printf("%f %f %f %f\n", (mtx)[2][0], (mtx)[2][1], (mtx)[2][2], (mtx)[2][3]);	\
	printf("%f %f %f %f\n", (mtx)[3][0], (mtx)[3][1], (mtx)[3][2], (mtx)[3][3]);	\
}

#define MAKE_DMATAG(qwc, pce, id, irq, addr) MasterList::createDMATAG(qwc, id, addr, irq, pce)

#define APPEND_MTX(addr, mtx)								\
	{																\
		DBGASSERT((int)(addr) % 16 == 0);					\
		DBGASSERT((int)&(mtx)[0][0] % 16 == 0);			\
		MasterList::Append16(addr, *(u_long128*)&(mtx)[0][0]);	\
		MasterList::Append16(addr, *(u_long128*)&(mtx)[1][0]);	\
		MasterList::Append16(addr, *(u_long128*)&(mtx)[2][0]);	\
		MasterList::Append16(addr, *(u_long128*)&(mtx)[3][0]);	\
	}

// 	  asm __volatile__ ("								\
// 			lq t0, 0(%1)			;						\
// 			lq t1, 16(%1)			;						\
// 			lq t2, 32(%1)			;						\
// 			lq t3, 48(%1)			;						\
// 			addiu %0, %0, 64		;						\
// 			sq t0, -64(%0)			;						\
// 			sq t1, -48(%0)			;						\
// 			sq t2, -32(%0)			;						\
// 			sq t3, -16(%0)			;						\
// 			" : "=r" (addr) : "r" (&mtx[0][0]));	\

// macros to calculate how many quad words will be needed for <count> of a particular unpack data type-- partially-filled
// quad words count as a full quad word
#define QWC_V4_32(count)					(count)								// each data entry is a quad word
#define QWC_V3_32(count)					((((count) + 1) * 3) / 4)		// 4 data entries make 3 quad words
#define QWC_V2_32(count)					(((count) + 1) / 2)				// each data entry is 1/2 quad word
#define QWC_V4_8(count)						(((count) + 3) / 4)				// each data entry is 1/4 quad word

// similar macros for calculating the unpack count for a given count. the unpack always unpacks the maximum number
// of data entries that fit in the dma'd qwc-- that is, for a single V4_8 entry, unpack will unpack 4 items
// since 4 fit in a single quad word. note that the count parameter is the original data entry count, not the quad
// word count
#define UNPACK_COUNT_V4_32(count)		(count)								// each quad word yields one data entry
#define UNPACK_COUNT_V3_32(count)		(((count) & 3) == 3) ? (count) + 1 : count)	// each quad word yields 3/4 of a data entry
#define UNPACK_COUNT_V2_32(count)		(((count) + 1) & ~1)				// can also be thought of as QWC_V2_32(count) * 2
#define UNPACK_COUNT_V4_8(count)			(((count) + 3) & ~3)				// can also be thought of as QWC_V4_8(count) * 4

// these simplify if we assume count is divisible by 4, like for sprites
#define QWC_V4_32_4X(count)				(count)								// each data entry is a quad word
#define QWC_V3_32_4X(count)				(((count) * 3) / 4)				// 4 data entries make 3 quad words
#define QWC_V2_32_4X(count)				((count) / 2)						// each data entry is 1/2 quad word
#define QWC_V4_8_4X(count)					((count) / 4)						// each data entry is 1/4 quad word
#define UNPACK_COUNT_V4_32_4X(count)	(count)								// each quad word yields one data entry
#define UNPACK_COUNT_V3_32_4X(count)	(count)								// each quad word yields 3/4 of a data entry
#define UNPACK_COUNT_V2_32_4X(count)	(count)								// can also be thought of as QWC_V2_32(count) * 2
#define UNPACK_COUNT_V4_8_4X(count)		(count)								// can also be thought of as QWC_V2_32(count) * 4

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// global variables
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	// TJC - pointer into current master display list being built
	// this pointer is to uncached memory.  Access is very slow.
	// but this is required because of the dma happening at the
	// same time.  you should probably never access it, instead
	// use the dma functions above or the CUrrentDMAPointer()
	// inlines
	extern SCRATCHPAD(u_char, *gCurrentSPRPos);
	extern SCRATCHPAD(u_char, *gCurrentSPREnd);

// a master list chunk...
// these are always the same size.
typedef struct MasterListChunk
{
	u_long128			mBuffer[MASTERLIST_QW_CHUNK_SIZE-1];
	u_int					mTag;
	MasterListChunk*	pNext;
	u_int					mVIFCode0;
	u_int					mVIFCode1;
} *MasterListChunkPtr;

#ifdef USE_SPR_BUFFERS
	extern SCRATCHPAD(MasterListChunk, gMasterListSPRChunks[2]);
#else
	extern SCRATCHPAD(MasterListChunk, gMasterListMiscData);
#endif
	extern u_char* pPath3DisplayList;


	// flushed/base/offset tracking
	extern bool gCurrentlyFlushed;
	extern u_int gBase;
	extern u_int gOffset;

	// frame number
	extern volatile SCRATCHPAD(u_int, gFrameBeingGenerated);
	extern volatile SCRATCHPAD(u_int, gFrameBeingRendered);
	extern volatile SCRATCHPAD(u_int, gFrameBeingDisplayed);

	// a flag to let us make sure that Start & Render are called alternatingly
	// and to make sure that things arent added when its not in one.
	extern int gMasterListActiveThread;
	extern bool bSuspended;

#ifdef _DEBUG
	extern bool bInMiscBlock;
#endif

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// the main public interface...
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	// initialize/cleanup
	void Init(void);
	bool IsInited(void);
	void Cleanup(void);
	void Reset(void); // restart the current display list (discarding whats made so far)
	void DisableInterruptHandling(void);

	typedef void printfunc(const char* format, ...);
	void SetPrintFunc(printfunc* printFunc);

	// install a callback to be called when the cpu is waiting for the DMA
	// to complete
	void SetDMAWaitCallback(void (*callback)(void));

	// callbacks for the masterlist (DMAC interrupts)
	// any interrupts fired outside of masterlist processing are
	// ignored, and these callbacks are not called.  Install
	// your own interrupt handler to deal with them... or modify
	// mine.
	typedef void MasterListCallback(void);
	// this callback gets called (if its !NULL) when the dma completes
	// This callback is called during a processor interrupt, so
	// follow the rules for an interrupt (MasterList calls ExitHandler()
	// for you).
	void SetEndOfDMACallback(MasterListCallback* callback);
	// this callback gets called (if its !NULL) when a dmatag with the
	// interrupt bit set COMPLETES executing.
	// If this is non-null, the dma chain is not automatically restarted.
	// the callback must restart the dma when its ready... 
	// This callback is called during a processor interrupt, so
	// follow the rules for an interrupt (MasterList calls ExitHandler()
	// for you).
	void SetDMAInterruptCallback(MasterListCallback* callback);

	// TJC - this can be used to unsyncrhonize the frame buffer swap
	// from the vsync interrupt.
	void DisplayVSynced(bool enabled = true);
	bool IsDisplayVSynced();

	// this callback is called every vblank...
	void SetVBlankInterruptCallback(MasterListCallback* callback);
	MasterListCallback* GetVBlankInterruptCallback(void);

	// tell masterlist that interrupts are already disabled.
	void InterruptsAreDisabled(bool really = true);

	// currently, this disables interrupts and disables flush calls...
	void BlockDMA(bool really = true);
	bool IsDMABlocked(void);

	// keep masterlist from changing the number of allocated blocks it has
	// the count of -1 means whatever you currently have, otherwise, the
	// number of blocks is forced to == count
	void LockChunkList(int count = -1);
	void UnlockChunkList(void);


	// minimum vblanks per frame
	void SetMinVBlanks(int minVBlanks);
	int GetMinVBlanks(void);

	// Support for the swapping callback
	typedef void (*SwapBufferCallback)(int bufferidx);
	void SetSwapBufferCallback(SwapBufferCallback callback);
	SwapBufferCallback GetSwapBufferCallback(void);

	// start a new chain
	void Start(void);

	// finish the current chain
	// returns clock cycles spent waiting for prior dma
	u_int End(void);

	// stall till all rendering is complete
	void Flush(void);

	// flush out the current stuff in spr
	void SPRFlush(void);

	// used by the movie player to keep buffer uploads synchronized.
	void StallTillAllDMAIsCompleteOrInProgress();

	// these just toggle the currently active state
	// so that things that check it will think its not active
	void Suspend(void);
	void Resume(void);

	// try to free some memory
	bool TryToFreeMemory(u_int size);

	// get how long (in cycles) the most recent MasterList took to process...
	u_int GetRecentCycleCount(void);

	// upload an image to VRAM
	void UploadImageData(void* pData, u_int address, u_int width, u_int height, u_int format, bool bInterrupt = false);
	// notify masterlist that we are done with the images we have sent up.
	void DoneWithImageData(void);

	// some functions to make creating small chunks of DL easier...
	// these should not be used for everything... please try to make
	// static data in SPR.	If pretty much all of it changes every time
	// then these may be a good idea... they are used internally to the
	// renderer to help keep it a little more decoupled.	note that
	// Start will stall till the dma started by End completes which is
	// sooner than is usually nessisary with SPR static data...
	inline u_char* StartMiscData(void);
	inline void EndMiscData(void* endPtr); // starts copying it to dl
	inline void* UseMiscData(void);

	// these are used to track if the dma chain believes that the vector unit may be
	// currently processing... ie.. if you change BASE/OFFSET, do you need to flush
	// first?...
	inline bool GetFlushed(void)				{ return gCurrentlyFlushed; }
	inline void SetFlushed(bool newValue)	{ gCurrentlyFlushed = newValue; }
	inline u_int GetBase(void) 				{ return gBase; }
	inline u_int GetOffset(void)				{ return gOffset; }
	inline void SetBase(u_int newBase)		{ gBase = newBase; }
	inline void SetOffset(u_int newOffset) { gOffset = newOffset; }
	inline bool IsBaseOffsetSame(u_int newBase, u_int newOffset) { return newBase == gBase && newOffset == gOffset; }

	// get the # of frames rendered
	inline u_int GetFrameBeingGenerated(void)		{ return gFrameBeingGenerated; }
	inline u_int GetFrameBeingRendered(void)		{ return gFrameBeingRendered; }
	inline u_int GetFrameBeingDisplayed(void)		{ return gFrameBeingDisplayed; }
	// get a 0 or 1 for even/odd frames
	inline u_int GetFrameToggle(void)				{ return GetFrameBeingGenerated() & 0x1; }
	// ^1 explaination: please note that the opposite buffer is really the one thats ready.  
	// thats because of when we occur and the frame lag between generate dl & display dl (namely render dl)
	inline u_int GetDisplayBufferToggle(void)		{ return (GetFrameBeingDisplayed() & 0x1) ^ 0x1; }

	// is the master list currently being generated?
	inline bool CurrentlyActive(void)		{ return gMasterListActiveThread == GetThreadId(); }  // in current thread
	inline bool CurrentlyActiveAtAll(void)	{ return gMasterListActiveThread != -1; } // in any thread
	inline int	CurrentlyActiveThreadId(void) { return gMasterListActiveThread; } // which thread is it in?
	inline bool CurrentlySuspended(void)	{ return bSuspended; }

	// get current position in the dma chain
	inline u_char*& CurrentDMAPointer(void) { DBGASSERT(CurrentlyActive()); return gCurrentSPRPos; }

	// get current position in the dma chain
	inline u_char*& CurrentPath3DMAPointer(void) { DBGASSERT(CurrentlyActive()); return pPath3DisplayList; }

	// tell us about the state of the hardware, along
	// with a message and possibly resetting the dma subsystem.
#ifndef SHIP_IT
	void ReportAboutDMA(char* message, bool resetDma, bool inInterrupt = false);
	u_int GetDMAWaitStallCycleCount(void);
	u_int GetBlockWaitStallCycleCount(void);
#else
	#define ReportAboutDMA(message, resetDma, inInterrupt)
#endif

	u_int CurrAllocatedChunks(void);
	u_int ChunksUsedLastFrame(void);
	u_int MinChunksFree(void);

	// inline functions
	// give misc data pointer away for a while...
	inline void* UseMiscData(void)
	{
#ifdef USE_SPR_BUFFERS
		SPRFlush();
		return gMasterListSPRChunks;
#else
		return &gMasterListMiscData;
#endif
	}

	// get start pointer to misc spr chunk...
	inline u_char* StartMiscData(void)
	{
		SPRFlush();

#ifdef _DEBUG
		DBGASSERT(!bInMiscBlock);
		bInMiscBlock = true;
#endif

		return CurrentDMAPointer();
	}

	// handle end of misc spr chunk work...
	// starts copying it to dl
	inline void EndMiscData(void *endPtr)
	{
#ifdef _DEBUG
		DBGASSERT(bInMiscBlock);
		bInMiscBlock = false;
#endif

		// get length in quadwords
#ifdef _DEBUG
		int len = (u_long128*)endPtr - (u_long128*)CurrentDMAPointer();
	
		DBGASSERT(((u_int)endPtr % 16) == 0);		// must add data in whole qword quantities.
		DBGASSERT(len <= MASTERLIST_MAX_CHAIN_SIZE);	// out of misc space.
#endif

		CurrentDMAPointer() = (u_char*)endPtr;
	}

	inline void ClearMem(void* ptr, int qwc)
	{
#if defined(PS2) && defined(SN)
		u_long128* dst = (u_long128*)ptr;
		while(qwc--)
		{
			asm("sq $0, %0" : "=m" (*dst));
			dst++;
		}
#else
		memset(ptr, 0, qwc*16);
#endif
	}

	// if we need it, flush the spr
	inline void CheckSPRFlush(int qwc = 0)
	{
#ifdef _DEBUG
		DBGASSERT(!bInMiscBlock);
#endif

#ifdef HASH_MASTERLIST_CHUNKS
		// we use the last QW of the chunk to store the hash, and size of the valid data
		// in the chunk.  So we cannot allow the code to use the last QW.  We do this by
		// inflating the qwc by 1.  This works because this routine never stores the qwc
		// that it was passed.
		qwc++;
#endif

		u_long128* curr = (u_long128*)CurrentDMAPointer();
		u_long128* end = (u_long128*)gCurrentSPREnd;
		if(curr + qwc >= end)
		{
			SPRFlush();
		}
	}

	// for copying pre-built packets from main memry into master display list
	inline void CopyToMasterList(const void* source, u_int qwc)
	{
		DBGASSERT(qwc <= MASTERLIST_QW_MAX_CHAIN_SIZE);
		CheckSPRFlush(qwc);

		u_long128* dst = (u_long128*)CurrentDMAPointer();
		const u_long128* src = (const u_long128*)source;

		CurrentDMAPointer() += qwc * 16;

		DBGASSERT(((u_int)src & 0xF) == 0);
		DBGASSERT(((u_int)dst & 0xF) == 0);

//		memcpy(dst, src, qwc*16);
		for(; qwc > 0; qwc--)
			*dst++ = *src++;
	}

	// new dma append commands
	inline void Append4(u_char*& dl, u_int val)
	{
		u_int* dst = (u_int*) dl;
		dst[0] = val;
		dl = (u_char*) &dst[1];
	}

	inline void AppendF(u_char*& dl, float val)
	{
		float* dst = (float*) dl;
		dst[0] = val;
		dl = (u_char*) &dst[1];
	}

	inline void Append8(u_char*& dl, u_long val)
	{
		DBGASSERT(((int)dl % 8) == 0);
		u_long* dst = (u_long*) dl;
		dst[0] = val;
		dl = (u_char*) &dst[1];
	}

	inline void Append16(u_char*& dl, u_long128 val)
	{
		DBGASSERT(((int)dl % 16) == 0);
		u_long128* dst = (u_long128*) dl;
		dst[0] = val;
		dl = (u_char*) &dst[1];
	}

	inline void Append4x4(u_char*& dl, u_int v1, u_int v2, u_int v3, u_int v4)
	{
		u_int* dst = (u_int*) dl;
		dst[0] = v1;
		dst[1] = v2;
		dst[2] = v3;
		dst[3] = v4;
		dl = (u_char*) &dst[4];
	}

	inline void Append4xF(u_char*& dl, float v1, float v2, float v3, float v4)
	{
		float* dst = (float*) dl;
		dst[0] = v1;
		dst[1] = v2;
		dst[2] = v3;
		dst[3] = v4;
		dl = (u_char*) &dst[4];
	}

	inline u_long createDMATAG(int qwc = 0, int id = TAGID_CNT, const void* addr = 0, bool irq = false, int pce = 0)
	{
		return ((u_long)qwc) | ((u_long)pce << 26) | ((u_long)id << 28) | ((u_long)irq << 31) | ((u_long)addr << 32);
	}

	// specific append to master list
	inline void Append4(u_int val)		{ Append4(CurrentDMAPointer(), val);  }
	inline void AppendF(float val)		{ AppendF(CurrentDMAPointer(), val);  }
	inline void Append8(u_long val)		{ Append8(CurrentDMAPointer(), val);  }
	inline void Append16(u_long128 val) { Append16(CurrentDMAPointer(), val); }
	inline void Append4x4(u_int v1, u_int v2, u_int v3, u_int v4) { Append4x4(CurrentDMAPointer(), v1, v2, v3, v4); }
	inline void Append4xF(float v1, float v2, float v3, float v4) { Append4xF(CurrentDMAPointer(), v1, v2, v3, v4); }

	// generic append to master list
	inline void Append(u_int val)			{ Append4(CurrentDMAPointer(), val);  }
	inline void Append(float val)			{ AppendF(CurrentDMAPointer(), val);  }
	inline void Append(u_long val)		{ Append8(CurrentDMAPointer(), val);  }
	inline void Append(u_long128 val)	{ Append16(CurrentDMAPointer(), val); }

	// generic append to specific list
	inline void Append(u_char*& dl, u_int val)		{ Append4(dl, val);  }
	inline void Append(u_char*& dl, float val)		{ AppendF(dl, val);  }
	inline void Append(u_char*& dl, u_long val)		{ Append8(dl, val);  }
	inline void Append(u_char*& dl, u_long128 val)	{ Append16(dl, val); }

	inline void AppendDMATAG(int qwc = 0, int id = TAGID_CNT, void* addr = 0, bool irq = false, int pce = 0)
														{ CheckSPRFlush(qwc+1); Append(createDMATAG(qwc, id, addr, irq, pce)); }

	inline void AppendP3(u_int val)     	{ Append(CurrentPath3DMAPointer(), val); }
	inline void AppendP3(float val)     	{ Append(CurrentPath3DMAPointer(), val); }
	inline void AppendP3(u_long val)    	{ Append(CurrentPath3DMAPointer(), val); }
	inline void AppendP3(u_long128 val)		{ Append(CurrentPath3DMAPointer(), val); }

	inline void AppendP34(u_int val)     	{ Append(CurrentPath3DMAPointer(), val); }
	inline void AppendP3F(float val)     	{ Append(CurrentPath3DMAPointer(), val); }
	inline void AppendP38(u_long val) 		{ Append(CurrentPath3DMAPointer(), val); }
	inline void AppendP316(u_long128 val)	{ Append(CurrentPath3DMAPointer(), val); }

	inline void AppendMtx(u_char*& dl, const void* mtxaddr)
	{
		DBGASSERT(((int)mtxaddr % 16) == 0);
		DBGASSERT(((int)dl % 16) == 0);
		const u_long128* src = (const u_long128*)mtxaddr;
		u_long128* dst       = (u_long128*) dl;
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		dst[3] = src[3];
		dl = (u_char*)&dst[4];
	}
	inline void AppendMtx(const void* mtxaddr)	{ AppendMtx(CurrentDMAPointer(), mtxaddr); }

	bool DisplayListDeletesPending();

} // namespace MasterList

#endif // __PS2RENDERER_MASTERLIST_H
