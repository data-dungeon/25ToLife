//	===========================================================================
//	mwUtils_PS2.c			©1999-2000 Metrowerks Inc. All rights reserved.
//	===========================================================================
//
//	Metrowerks Utilities
//
//	Version.1.5.7
//
//	06/07/2000	kashima,	modify mwDbgVifHandler() for Runtime Library 1.6
//	05/17/2000	kashima,	add VU debugging utility functions
//	01/25/2000	kashima,	change mwNotifyOverlayLoaded()
//	01/13/2000	kashima,	add overlay support

/*******************************************************************************
	INCLUDE
*******************************************************************************/

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include "platform/PlatformPCH.h"

/*** no pre-compiled headers for SN ***/

#ifdef SN_TARGET_PS2

/* system includes */

/*	OS dependent header	*/
#include <eekernel.h>
#include <eeregs.h>
#include <sifdev.h>
#include <devvu0.h>
#include <devvu1.h>
/*	STANDARD header	*/
#include <stdlib.h>
#include <string.h>
/*	PROJECT header	*/
#include <mwUtils.h>
#include <mwUtils_PS2.h>

/* engine includes */

#endif //SN_TARGET_PS2



/*******************************************************************************
	DEFINE
*******************************************************************************/

#ifndef	WORD
#define	WORD	unsigned int
#endif
#ifndef	BYTE
#define	BYTE	unsigned char
#endif

/*******************************************************************************
	TYPEDEF
*******************************************************************************/

/*******************************************************************************
	PROTOTYPE
*******************************************************************************/
void	mwDbgVifBreak(void);
int		mwDbgVifHandler(int ca);

/*------------------------------------------------------------------------------
	mwExceptionTableIndexes
------------------------------------------------------------------------------*/
typedef struct ExceptionTableIndex
{
	char*			function_address;
	unsigned int	function_size;
	char*			exception_table;
}	ExceptionTableIndex;
typedef	ExceptionTableIndex	mwExceptionTableIndexes[];

/*------------------------------------------------------------------------------
	mwStaticInitializer
------------------------------------------------------------------------------*/
typedef	void				(*mwStaticInitializer)(void);
typedef	mwStaticInitializer	mwStaticInitializers[];

/*------------------------------------------------------------------------------
	mwOverlayHeader
------------------------------------------------------------------------------*/
typedef struct	mwOverlayHeader
{
	BYTE	identifier[3];	/*	"MWo"	*/
	BYTE	version;
	WORD	id;
	WORD 	address;
	WORD 	sz_text;
	WORD 	sz_data;
	WORD	sz_bss;
	WORD 	_static_init;
	WORD 	_static_init_end;
	BYTE	name[32];
}	mwOverlayHeader;

/*******************************************************************************
	EXTERN
*******************************************************************************/

#ifdef __cplusplus
extern	"C"	{
#endif

/*------------------------------------------------------------------------------
 *	for exception table
------------------------------------------------------------------------------*/
extern	mwExceptionTableIndexes	__exception_table_start__;
extern	mwExceptionTableIndexes	__exception_table_end__;

/*------------------------------------------------------------------------------
 *	for static initializer
------------------------------------------------------------------------------*/
extern	mwStaticInitializers	__static_init;
extern	mwStaticInitializers	__static_init_end;

extern	void	__initialize_cpp_rts(void** si, void** sie, void* ets, void* ete);
extern	void	__destroy_global_chain(void);

/*------------------------------------------------------------------------------
 *	for gcc compatibility
------------------------------------------------------------------------------*/
extern	void	__do_global_ctors(void);
extern	void	__do_global_dtors(void);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

/*******************************************************************************
	FUNCTION
*******************************************************************************/

/*------------------------------------------------------------------------------
 *	__do_global_ctors
------------------------------------------------------------------------------*/
void
__do_global_ctors(void)
{
	mwInit();
}

/*------------------------------------------------------------------------------
 *	__do_global_dtors
------------------------------------------------------------------------------*/
void
__do_global_dtors(void)
{
	mwExit();
}

/*------------------------------------------------------------------------------
 *	mwInit
 *	initialize the C++ runtime
 *	call static initializers and initialize exception table
------------------------------------------------------------------------------*/
void
mwInit(void)
{
	__initialize_cpp_rts((void **)__static_init, (void **)__static_init_end, __exception_table_start__, __exception_table_end__);
}

/*------------------------------------------------------------------------------
 *	mwExit
 *	destroy all constructed global objects
------------------------------------------------------------------------------*/
void
mwExit(void)
{
	__destroy_global_chain();
}

#pragma	mark	-

/*------------------------------------------------------------------------------
 *	mwOverlayInit
 *	initialize overlay
 *	--- CAUTION ---
 *	you have to call this function after the overlay file is loaded
------------------------------------------------------------------------------*/
void
mwOverlayInit(const void* pAddress, int inFileSize)
{
	unsigned char*		pPtr	= (unsigned char*)pAddress;
	mwOverlayHeader*	pOH		= (mwOverlayHeader*)pAddress;
	const unsigned int	nSize	= pOH->sz_bss;

	/* invalidate i-cache */
	FlushCache(INVALIDATE_ICACHE);		/* invalid instruction cashe */

	/* clear bss section*/
	if (nSize > 0)
	{
		pPtr	+= inFileSize;
		memset(pPtr, 0, nSize);
	}

	/* call static initializer of overlay */
	__initialize_cpp_rts((void**)pOH->_static_init, (void**)pOH->_static_init_end, NULL, NULL);
}

/*------------------------------------------------------------------------------
 *	mwOverlayNotifyToDebugger
 *	notify to debugger
 *	--- CAUTION ---
 *	you have to call this function after the overlay file is loaded
------------------------------------------------------------------------------*/
#ifdef	DEBUG
#pragma	auto_inline	off
asm void
mwOverlayNotifyToDebugger(const void*	pAddress)
{
#pragma	unused(pAddress)
	jr	ra
	nop
}
#pragma	auto_inline	reset
#endif

#pragma	mark	-

/*------------------------------------------------------------------------------
 *	mwBload
 *	utility function to load binary file into memory
------------------------------------------------------------------------------*/
int
mwBload(const char* pFilePath, const void* pAddress)
{
	int		rfd			= -1;
	void*	pBuffer		= NULL;
	int		size		= 0;

	/*	open file	*/
	rfd = sceOpen((char*)pFilePath, SCE_RDONLY);
	/*	calc file size	*/
	if (rfd >= 0)
	{
		size	= sceLseek(rfd, 0, SCE_SEEK_END);
		sceLseek(rfd, 0, SCE_SEEK_SET);
	}
	/*	read file into memory	*/
	if (size > 0)
	{
		size	= sceRead(rfd, (void *)pAddress, size);
	}
	/*	close file	*/
	if (rfd >= 0)
	{
        sceClose(rfd);
	}

	return	size;
}

/*------------------------------------------------------------------------------
 *	mwLoadOverlay
 *	utility function to load overlay file
------------------------------------------------------------------------------*/
int
mwLoadOverlay(const char* pFilePath, const void* pAddress)
{
	int	size	= 0;
	int	result	= FALSE;

	/* load overlay file */
	size	= mwBload(pFilePath, pAddress);
	if (size > 0)
	{
		/* nortify to debugger */
		mwOverlayNotifyToDebugger(pAddress);
		/* initialize overlay */
		mwOverlayInit(pAddress, size);
		result	= TRUE;
	}

	return	result;
}

#pragma	mark	-

static int	mwHdlrID_VIF0	= -1;
static int	mwHdlrID_VIF1	= -1;
static int	mwHdlrState_VIF0	= 0;	// 0:stop, 1:enable by mwDbgVifBreakEnable
static int	mwHdlrState_VIF1	= 0;	// 0:stop, 1:enable by mwDbgVifBreakEnable

/*------------------------------------------------------------------------------
 *	mwDbgVifBreak
------------------------------------------------------------------------------*/
#pragma	auto_inline	off
#pragma	nofpregforblkmv	on	/* avoid to use fp */
void
mwDbgVifBreak(void)
{
	mwDbgBreak();
}
#pragma	nofpregforblkmv	reset
#pragma	auto_inline	reset

/*------------------------------------------------------------------------------
 *	mwDbgVifHandler
------------------------------------------------------------------------------*/
#pragma	nofpregforblkmv	on	/* avoid to use fp */
int
mwDbgVifHandler(int ca)
{
	switch (ca)
	{
	// VIF0 interrupt
	case INTC_VIF0:
		if ((DGET_VIF0_STAT() && (VIF0_STAT_VIS_M | VIF0_STAT_MRK_M)) != 0)
		{
			tVIF_MARK	mark = *(tVIF_MARK*)VIF0_MARK;
			if (mark.MARK == 0x1234)
			{	//	this interrupt is caused by mwDbgVifBreak
				//	0x1234 is the magic number for MARK VIF code
				mwDbgVifBreak();	// break in this to set breakpoint in MS
				DPUT_VIF0_MARK(0);	// clear MARK register
				DPUT_VIF0_FBRST(VIF0_FBRST_STC_M);	// continue
			}
		}
		break;
	// VIF1 interrupt
	case INTC_VIF1:
		if ((DGET_VIF1_STAT() && (VIF1_STAT_VIS_M | VIF1_STAT_MRK_M)) != 0)
		{
			tVIF_MARK	mark = *(tVIF_MARK*)VIF1_MARK;
			if (mark.MARK == 0x1234)
			{	//	this interrupt is caused by mwDbgVifBreak
				//	0x1234 is the magic number for MARK VIF code
				mwDbgVifBreak();	// break in this to set breakpoint in MS
				DPUT_VIF1_MARK(0);	// clear MARK register
				DPUT_VIF1_FBRST(VIF1_FBRST_STC_M);	// continue
			}
		}
		break;
	}

    ExitHandler();

	return	0;
}
#pragma	nofpregforblkmv	reset

/*------------------------------------------------------------------------------
 *	mwDbgVifBreakEnable
------------------------------------------------------------------------------*/
int
mwDbgVifBreakEnable(void)
{
	//	install INTC interrupt handler for VIF
	if (mwHdlrID_VIF0 == -1)
		mwHdlrID_VIF0	= AddIntcHandler(INTC_VIF0, &mwDbgVifHandler, 0);
	if (mwHdlrID_VIF1 == -1)
		mwHdlrID_VIF1	= AddIntcHandler(INTC_VIF1, &mwDbgVifHandler, 0);

	//	enable INTC interrupt for VIF
	if (mwHdlrState_VIF0 == 0)
		mwHdlrState_VIF0	= EnableIntc(INTC_VIF0);
	if (mwHdlrState_VIF1 == 0)
		mwHdlrState_VIF1	= EnableIntc(INTC_VIF1);

	return	0;
}

/*------------------------------------------------------------------------------
 *	mwDbgVifBreakDisable
------------------------------------------------------------------------------*/
int
mwDbgVifBreakDisable(void)
{
	//	remove INTC interrupt handler
	if (mwHdlrID_VIF0 != -1)
	{
		RemoveIntcHandler(INTC_VIF0, mwHdlrID_VIF0);
		mwHdlrID_VIF0	= -1;
	}
	if (mwHdlrID_VIF1 != -1)
	{
		RemoveIntcHandler(INTC_VIF1, mwHdlrID_VIF1);
		mwHdlrID_VIF1	= -1;
	}

	//	disable INTC interrupt
	if (mwHdlrState_VIF0 == 1)
	{
		DisableIntc(DMAC_VIF0);
		mwHdlrState_VIF0	= 0;
	}
	if (mwHdlrState_VIF1 != -1)
	{
		DisableIntc(DMAC_VIF1);
		mwHdlrState_VIF1	= 0;
	}

	return	0;
}
