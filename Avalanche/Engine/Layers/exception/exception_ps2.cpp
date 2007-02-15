//============================================================================
//=
//= Exception_PS2.cpp - Runtime exception handler for PS2
//=
//============================================================================

#include "Layers/LayersPCH.h"

#ifndef DISABLE_EXCEPTIONS

#include "Layers/exception/exception.h"
#include "Layers/exception/platformexception.h"
#include "platform/PS2/CDllLoader.h"
#include "GameHelper/DebugOutput.h"
#include "Display/PS2/Renderer/Code/MasterList.h"
#include "Layers/Breakpoint/HardwareBreak.h"

#include "Platform/PS2/DisplaySimpleText.h"

#include "EngineHelper/drawutil.h"
#include "Display/ColorConversions.h"
#include "platform/systemi.h"
#include "platform/gameapp.h"
#include "Network/Connection.h"
#include "Script/ScriptVM.h"

#include <libgraph.h>
#include <libdev.h>
#include <eekernel.h>
#include <libpad.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <libdma.h>
#include <libpkt.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libcdvd.h>
#include <libexcep.h>

#include "Display/Common/DisplayPlatform.h"
#include "Display/PS2/Renderer/Code/MasterList.h"

#include "Layers/exception/exceptfontmetrics.h"
#include "Layers/exception/exceptfontdata.h"

// HACK! Hard code reference the build time/date from the application
extern const char* TTL_VERSION;

// Hard code reference to stack checker
extern int stackcheck();

//============================================================================

#define CALL_STACK_HISTORY 14

enum {
	BUT_UP       = ( 1 <<  0 ),
	BUT_DOWN     = ( 1 <<  1 ),
	BUT_LEFT     = ( 1 <<  2 ),
	BUT_RIGHT    = ( 1 <<  3 ),
	BUT_X        = ( 1 <<  4 ),
	BUT_SQUARE   = ( 1 <<  5 ),
	BUT_TRIANGLE = ( 1 <<  6 ),
	BUT_CIRCLE   = ( 1 <<  7 ),
	BUT_L1       = ( 1 <<  8 ),
	BUT_L2       = ( 1 <<  9 ),
	BUT_L3       = ( 1 << 10 ),
	BUT_R1       = ( 1 << 11 ),
	BUT_R2       = ( 1 << 12 ),
	BUT_R3       = ( 1 << 13 ),
	BUT_SELECT   = ( 1 << 14 ),
	BUT_START    = ( 1 << 15 ),
};

//============================================================================

static sceGsDBuff					  db;
static sceExcepIOPExceptionData iop;
static PS2DisplaySimpleText	  s_disp;

static const char* ExceptionCode[] =
{
  "Interrupt",
  "TLB modification exception",
  "TLB exception (load or instruction fetch)",
  "TLB exception (store)",
  "Address error exception (load or instruction fetch)",
  "Address error exception (store)",
  "Bus error exception (instruction fetch)",
  "Bus error exception (data reference: load or store)",
  "Syscall exception",
  "Breakpoint exception",
  "Reserved instruction exception",
  "Coprocessor Unusable exception",
  "Arithmetic overflow exception",
  "Trap exception"
};

//============================================================================

extern "C" int snIsDebuggerRunning(void);	// from libsn.h

void getStackTrace( unsigned int* ra, unsigned int* sp, unsigned int* results, int max );

//============================================================================

void PlatformException::Init()
{
	#ifndef CDROM	// for profiling
	return;
	#endif

	// don't enable exception handling if we're on at atwinmon deck
	extern const char* g_RemoteIPAddress;
	if ( g_RemoteIPAddress != NULL )
	{
		return;
	}

	// don't enable exception handling if we were launched from the debugger
	if ( snIsDebuggerRunning() )
	{
		return;
	}

	// Enable exception handling
	sceExcepSetDebugEEHandler( EEExceptionHandler );
	char* irxName = Media::GetFileName( "irx\\panicsys.irx" );
	sceExcepSetDebugIOPHandler( irxName, IOPExceptionHandler, &iop );

	// Test EE Exceptions
//	*(volatile unsigned long*)1 = 0;

	// Test IOP Exceptions
//	irxName = Media::GetFileName( "irx\\panictst.irx" );
//	while ( sceSifLoadModule( irxName, 0, NULL ) < 0 )
//	{
//		scePrintf( "ERROR: can't load module panictst.irx\n" );
//	}

	HardwareBreak::SetTrapHandler(EEExceptionHandler);
}

//============================================================================

void PlatformException::InitDisplay()
{
	s_disp.Init(640, 448, false, s_exceptionFontData, s_exceptionFontMetrics);
}

//============================================================================

int PlatformException::ReadController( float* X1, float* Y1, float* X2, float* Y2 )
{
	//code to initialize, in stand alone mode
	//static bool initialized = false;
	//if (!initialized)
	//{
	//	struct padDMA
	//	{
	//		u_long128      buff[scePadDmaBufferMax];
	//		unsigned char  actDirect[6];
	//		unsigned char  actAlign[6];
	//	};
	//	padDMA padBuffer[1][1] __attribute__((aligned(64)));
	//	sceSifLoadModule( "host0:DATA_PS2\\IRX\\SIO2MAN.IRX", 0, NULL );
	//	sceSifLoadModule( "host0:DATA_PS2\\IRX\\PADMAN.IRX", 0, NULL );
	//	scePadInit( 0 );
	//	u_long128* dmaBuffer = padBuffer[0][0].buff;
	//	scePadPortOpen( 0, 0, dmaBuffer );
	//	initialized = true;
	//}

	int buttons = 0;
	if ( X1 ) *X1 = 0.0f;
	if ( Y1 ) *Y1 = 0.0f;
	if ( X2 ) *X2 = 0.0f;
	if ( Y2 ) *Y2 = 0.0f;

	int padStatus = scePadGetState( 0, 0 );
	if ( padStatus == scePadStateFindCTP1 || padStatus == scePadStateStable )
	{
		static unsigned char data[32] __attribute__((aligned(64)));
		scePadRead( 0, 0, data );

		if ( ( data[2] & 0x01 ) == 0 )	buttons |= BUT_SELECT;
		if ( ( data[2] & 0x02 ) == 0 )	buttons |= BUT_L3;
		if ( ( data[2] & 0x04 ) == 0 )	buttons |= BUT_R3;
		if ( ( data[2] & 0x08 ) == 0 )	buttons |= BUT_START;
		if ( ( data[2] & 0x10 ) == 0 )	buttons |= BUT_UP;
		if ( ( data[2] & 0x20 ) == 0 )	buttons |= BUT_RIGHT;
		if ( ( data[2] & 0x40 ) == 0 )	buttons |= BUT_DOWN;
		if ( ( data[2] & 0x80 ) == 0 )	buttons |= BUT_LEFT;

		if ( ( data[3] & 0x01 ) == 0 )	buttons |= BUT_L2;
		if ( ( data[3] & 0x02 ) == 0 )	buttons |= BUT_R2;
		if ( ( data[3] & 0x04 ) == 0 )	buttons |= BUT_L1;
		if ( ( data[3] & 0x08 ) == 0 )	buttons |= BUT_R1;
		if ( ( data[3] & 0x10 ) == 0 )	buttons |= BUT_TRIANGLE;
		if ( ( data[3] & 0x20 ) == 0 )	buttons |= BUT_CIRCLE;
		if ( ( data[3] & 0x40 ) == 0 )	buttons |= BUT_X;
		if ( ( data[3] & 0x80 ) == 0 )	buttons |= BUT_SQUARE;

		#define DEAD_ZONE	32

		if ( X1 )
		{
			if      ( data[6] < (0x80 - DEAD_ZONE) )	*X1 = (float)(data[6] - (0x80 - DEAD_ZONE)) / (float)(0x80 - DEAD_ZONE);
			else if ( data[6] > (0x80 + DEAD_ZONE) )	*X1 = (float)(data[6] - (0x80 + DEAD_ZONE)) / (float)(0x7F - DEAD_ZONE);
		}
		if ( Y1 )
		{
			if      ( data[7] < (0x80 - DEAD_ZONE) )	*Y1 = (float)(data[7] - (0x80 - DEAD_ZONE)) / (float)(0x80 - DEAD_ZONE);
			else if ( data[7] > (0x80 + DEAD_ZONE) )	*Y1 = (float)(data[7] - (0x80 + DEAD_ZONE)) / (float)(0x7F - DEAD_ZONE);
		}
		if ( X2 )
		{
			if      ( data[4] < (0x80 - DEAD_ZONE) )	*X2 = (float)(data[4] - (0x80 - DEAD_ZONE)) / (float)(0x80 - DEAD_ZONE);
			else if ( data[4] > (0x80 + DEAD_ZONE) )	*X2 = (float)(data[4] - (0x80 + DEAD_ZONE)) / (float)(0x7F - DEAD_ZONE);
		}
		if ( Y2 )
		{
			if      ( data[5] < (0x80 - DEAD_ZONE) )	*Y2 = (float)(data[5] - (0x80 - DEAD_ZONE)) / (float)(0x80 - DEAD_ZONE);
			else if ( data[5] > (0x80 + DEAD_ZONE) )	*Y2 = (float)(data[5] - (0x80 + DEAD_ZONE)) / (float)(0x7F - DEAD_ZONE);
		}
	}

	return buttons;
}

//============================================================================

void PlatformException::DrawRegisterPanel( u_int stat, u_int cause, u_int epc, u_int bva, u_int bpa, u_long128 *gpr, int memUsage, int freeBytes, int largestFree, unsigned int* callStack )
{
	int i;

	//
	// E E
	//

	s_disp.Printf("EE Exception [%s]\n", ExceptionCode[(cause>>2)&31] );
	s_disp.Printf("stat=0x%08x cause=0x%08x epc=0x%08x\n", stat, cause, epc );
	s_disp.Printf("bva=0x%08x bpa=0x%08x\n",bva,bpa);
	s_disp.Printf("at  =0x%08x     v0-1=0x%08x,0x%08x \n",     gpr[GPR_at],gpr[GPR_v0],gpr[GPR_v1]);
	s_disp.Printf("a0-3=0x%08x,0x%08x,0x%08x,0x%08x\n",        gpr[GPR_a0],gpr[GPR_a1],gpr[GPR_a2],gpr[GPR_a3]);
	s_disp.Printf("t0-7=0x%08x,0x%08x,0x%08x,0x%08x\n",        gpr[GPR_t0],gpr[GPR_t1],gpr[GPR_t2],gpr[GPR_t3]);
	s_disp.Printf("     0x%08x,0x%08x,0x%08x,0x%08x\n",        gpr[GPR_t4],gpr[GPR_t5],gpr[GPR_t6],gpr[GPR_t7]);
	s_disp.Printf("s0-7=0x%08x,0x%08x,0x%08x,0x%08x\n",        gpr[GPR_s0],gpr[GPR_s1],gpr[GPR_s2],gpr[GPR_s3]);
	s_disp.Printf("     0x%08x,0x%08x,0x%08x,0x%08x\n",        gpr[GPR_s4],gpr[GPR_s5],gpr[GPR_s6],gpr[GPR_s7]);
	s_disp.Printf("t8-9=0x%08x,0x%08x   ",                     gpr[GPR_t8],gpr[GPR_t9]);
	s_disp.Printf("k0-1=0x%08x,0x%08x\n",                      gpr[GPR_k0],gpr[GPR_k1]);
	s_disp.Printf("gp=0x%08x sp=0x%08x fp=0x%08x ra=0x%08x\n", gpr[GPR_gp],gpr[GPR_sp],gpr[GPR_fp],gpr[GPR_ra]);
//	s_disp.Printf("NetZ:%07x Free:%07x Largest:%07X Stack:%dk\n", memUsage, freeBytes, largestFree, stackcheck() / 1024 );

	const char* msg = ExceptionHandler::GetExceptionMessage();
	if ( msg && msg[0] != 0 )
	{
		s_disp.Printf( msg );
	}
	else
	{
		char scriptInfo[ 512 ];
		ScriptVM::GetDebugInfo( scriptInfo );
		s_disp.Printf( scriptInfo );
	}

	if (s_disp.CurCol() != 0)
		s_disp.Printf("\n");

	int stackLine = s_disp.CurRow();

	for ( i = 0 ; i < CALL_STACK_HISTORY && callStack[ i ] && stackLine + i < s_disp.MaxRows() ; i++ )
	{
		s_disp.SetPosition( 0, stackLine + i );
		s_disp.Printf( "Stack %2d: 0x%08x", i, callStack[ i ] );
	}

	// show loaded DLLs
	CDllLoader::LoadedDLLInfo* dll;
	int numDLL = g_DllLoader.GetLoadedDLLs( &dll, NULL );
	if ( numDLL )
	{
		s_disp.SetPosition( 23, stackLine );
		s_disp.Printf( "DLL:" );
		for ( i = 0 ; i < numDLL ; i++ )
		{
			s_disp.SetPosition( 23, stackLine + 1 + i );
			s_disp.Printf( "%08x-%08x %s", dll[i].addr, (u32)dll[i].addr + dll[i].size - 1, dll[i].name );
		}
	}
}

//============================================================================

void PlatformException::DrawDebugPanel( int butt, int buttEdge, float X1, float Y1, float X2, float Y2 )
{
#ifdef DEBUG_OUTPUT_ENABLED
	static CDebugDisplay debugDisplay;
	static int initialized = false;
	if ( !initialized )
	{
		debugDisplay.SetOutput( &g_debugOutput );
		debugDisplay.DescribeDisplay( s_disp.MaxRows() - 1, s_disp.MaxCols() );
		initialized = true;
	}

	debugDisplay.LockBuffer();
	debugDisplay.GotoTop();
	int i = 1;	// skip title line
	while( !debugDisplay.PastEnds() && i < s_disp.MaxRows() )
	{
		s_disp.Printf( "%s\n", debugDisplay.GetLine() );
		++debugDisplay;
		++i;
	}
	debugDisplay.UnlockBuffer();

	if ( buttEdge & BUT_UP    ) { debugDisplay.DisplayLineUp();   }
	if ( buttEdge & BUT_DOWN  ) { debugDisplay.DisplayLineDown(); }
	if ( buttEdge & BUT_LEFT  ) { debugDisplay.DisplayPageUp();   }
	if ( buttEdge & BUT_RIGHT ) { debugDisplay.DisplayPageDown(); }
	if ( buttEdge & BUT_L3    ) { debugDisplay.DisplayOldest();   }
	if ( buttEdge & BUT_R3    ) { debugDisplay.DisplayNewest();   }
#else //DEBUG_OUTPUT_ENABLED
	s_disp.Printf( "DEBUG_OUTPUT_ENABLED not defined\n" );
#endif //DEBUG_OUTPUT_ENABLED
}

//============================================================================

void PlatformException::EEExceptionHandler( u_int stat, u_int cause, u_int epc, u_int bva, u_int bpa, u_long128 *gpr )
{
	int frame = 0;
	int i;

	int freeBytes = 0, freeHandles = 0, largestFree = 0, allocBytes = 0, allocHandles = 0;
	//Layers__GetMemoryStats(&freeBytes, &freeHandles, &largestFree, &allocBytes, &allocHandles);

	InitDisplay();
	s_disp.SetBackgroundColor(0x10, 0x10, 0x60);
	s_disp.SetForegroundColor(0xb0, 0xb0, 0xb0);

	// get our stack trace
	unsigned int callStack[ CALL_STACK_HISTORY ];
	getStackTrace( (unsigned int*) gpr[ GPR_ra ], (unsigned int*) gpr[ GPR_sp ], callStack, CALL_STACK_HISTORY );

	u32 memUsage = 0;
	//if (ValidSingleton(CConnection))
	//	memUsage = g_connection.GetMemoryUsage();

	//scePrintf("*** EE Exception [%s] ***\n", ExceptionCode[(cause>>2)&31] );
	//scePrintf("stat=0x%08x cause=0x%08x epc=0x%08x\n",		 stat, cause, epc);
	//scePrintf("bva=0x%08x bpa=0x%08x\n",                   bva,bpa);
	//scePrintf("at  =0x%08x     v0-1=0x%08x,0x%08x\n",      gpr[GPR_at],gpr[GPR_v0],gpr[GPR_v1]);
	//scePrintf("a0-3=0x%08x,0x%08x,0x%08x,0x%08x\n",        gpr[GPR_a0],gpr[GPR_a1],gpr[GPR_a2],gpr[GPR_a3]);
	//scePrintf("t0-7=0x%08x,0x%08x,0x%08x,0x%08x\n",        gpr[GPR_t0],gpr[GPR_t1],gpr[GPR_t2],gpr[GPR_t3]);
	//scePrintf("     0x%08x,0x%08x,0x%08x,0x%08x\n",        gpr[GPR_t4],gpr[GPR_t5],gpr[GPR_t6],gpr[GPR_t7]);
	//scePrintf("s0-7=0x%08x,0x%08x,0x%08x,0x%08x\n",        gpr[GPR_s0],gpr[GPR_s1],gpr[GPR_s2],gpr[GPR_s3]);
	//scePrintf("     0x%08x,0x%08x,0x%08x,0x%08x\n",        gpr[GPR_s4],gpr[GPR_s5],gpr[GPR_s6],gpr[GPR_s7]);
	//scePrintf("t8-9=0x%08x,0x%08x     ",                   gpr[GPR_t8],gpr[GPR_t9]);
	//scePrintf("k0-1=0x%08x,0x%08x\n",                      gpr[GPR_k0],gpr[GPR_k1]);
	//scePrintf("gp=0x%08x sp=0x%08x fp=0x%08x ra=0x%08x\n", gpr[GPR_gp],gpr[GPR_sp],gpr[GPR_fp],gpr[GPR_ra]);
	//scePrintf("NetZ Used:0x%07x  Free:0x%07x  Largest:0x%07x\n", memUsage, freeBytes, largestFree );
	//
	//for ( i = 0 ; i < CALL_STACK_HISTORY && callStack[ i ] ; i++ )
	//{
	//	scePrintf( "Call Stack %2d: 0x%08x\n", i, callStack[ i ] );
	//}

	int panel = 0;
	int ButtonLast = ~0;
	int ButtonNow  = ~0;
	int ButtonEdge = 0;
	float Stick1X  = 0.0f;
	float Stick1Y  = 0.0f;
	float Stick2X  = 0.0f;
	float Stick2Y  = 0.0f;

	while(1)
	{
		s_disp.Clear();
		s_disp.Printf( "VER: %s\n", TTL_VERSION );

		switch ( panel )
		{
			case 0:	DrawRegisterPanel( stat, cause, epc, bva, bpa, gpr, memUsage, freeBytes, largestFree, callStack );	break;
			case 1:	DrawDebugPanel( ButtonNow, ButtonEdge, Stick1X, Stick1Y, Stick2X, Stick2Y ); break;
		}

		if ( ( frame & 0x60 ) != 0x60 )
		{
			s_disp.SetPosition( s_disp.MaxCols() - 7, 0 );
			s_disp.Printf( "PRESS X" );
		}

		s_disp.Flip();

		ButtonLast = ButtonNow;
		ButtonNow  = ReadController( &Stick1X, &Stick1Y, &Stick2X, &Stick2Y );
		ButtonEdge = ButtonNow & ~ButtonLast;

		if ( ButtonEdge & BUT_X )
		{
			panel++;
			if ( panel > 1 ) panel = 0;
		}

		frame++;
	}
}

//============================================================================

void PlatformException::IOPExceptionHandler( void *p,void *d )
{
	int frame = 0;

	InitDisplay();
	s_disp.SetBackgroundColor(0x10, 0x40, 0x10);
	s_disp.SetForegroundColor(0xb0, 0xb0, 0xb0);

	while( 1 )
	{
		//
		// I O P
		//

		s_disp.Clear();

		s_disp.Printf("IOP Exception [%s]\n",ExceptionCode[(iop.reg[IOP_CAUSE]>>2)&31]);
		s_disp.Printf("Module [%s] Version [%02x.%02x] Offset [0x%08x]\n",iop.module ,(iop.version)>>8 ,(iop.version)&0xf,iop.offset); 
		s_disp.Printf("stat=0x%08x cause=0x%08x epc=0x%08x\n",iop.reg[IOP_SR] ,iop.reg[IOP_CAUSE], iop.reg[IOP_EPC]);
		s_disp.Printf("at  =0x%08x     v0-1=0x%08x,0x%08x \n",iop.reg[GPR_at],iop.reg[GPR_v0], iop.reg[GPR_v1]);
		s_disp.Printf("a0-3=0x%08x,0x%08x,0x%08x,0x%08x\n",iop.reg[GPR_a0], iop.reg[GPR_a1], iop.reg[GPR_a2], iop.reg[GPR_a3]);
		s_disp.Printf("t0-7=0x%08x,0x%08x,0x%08x,0x%08x\n",iop.reg[GPR_t0], iop.reg[GPR_t1], iop.reg[GPR_t2], iop.reg[GPR_t3]);
		s_disp.Printf("     0x%08x,0x%08x,0x%08x,0x%08x\n",iop.reg[GPR_t4], iop.reg[GPR_t5], iop.reg[GPR_t6], iop.reg[GPR_t7]);
		s_disp.Printf("s0-7=0x%08x,0x%08x,0x%08x,0x%08x\n",iop.reg[GPR_s0], iop.reg[GPR_s1], iop.reg[GPR_s2], iop.reg[GPR_s3]);
		s_disp.Printf("     0x%08x,0x%08x,0x%08x,0x%08x\n",iop.reg[GPR_s4], iop.reg[GPR_s5], iop.reg[GPR_s6], iop.reg[GPR_s7]);
		s_disp.Printf("t8-9=0x%08x,0x%08x   ",iop.reg[GPR_t8], iop.reg[GPR_t9]);
		s_disp.Printf("k0-1=0x%08x,0x%08x\n",iop.reg[GPR_k0], iop.reg[GPR_k1]);
		s_disp.Printf("gp=0x%08x sp=0x%08x fp=0x%08x ra=0x%08x\n",iop.reg[GPR_gp], iop.reg[GPR_sp], iop.reg[GPR_fp], iop.reg[GPR_ra]);
		s_disp.Printf("hi=0x%08x lo=0x%08x sr=0x%08x epc =0x%08x\n",iop.reg[IOP_HI], iop.reg[IOP_LO], iop.reg[IOP_SR], iop.reg[IOP_EPC]);
		s_disp.Printf("cause=0x%08x tar=0x%08x badadr=0x%08x\n", iop.reg[IOP_CAUSE], iop.reg[IOP_TAR], iop.reg[IOP_BADVADDR]);
		s_disp.Printf("dcic=0x%08x bpc=0x%08x bpcm=0x%08x bda=0x%08x bpam=0x%08x\n",iop.reg[IOP_DCIC], iop.reg[IOP_BPC], iop.reg[IOP_BPCM], iop.reg[IOP_BDA], iop.reg[IOP_BDAM]);

		s_disp.Flip();
		frame++;
	}
}

//============================================================================
//=== BELOW IS CODE TO GET A CALL STACK TRACE
//============================================================================

/*
 * $XConsortium: getretmips.c,v 1.4 94/04/17 20:59:44 keith Exp $
 *
Copyright (c) 1992  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not
be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

/* Return stack generation for MIPS processors
 * This is tricky as MIPS stack frames aren't
 * easily unrolled -- we look for pc restoration
 * and stack adjustment instructions beyond the return
 * address to discover the correct values
 */

/* lw $31,const($sp) is : 100 011 11101 11111 const */
/*                        1000 1111 1011 1111       */

#define RESTORE_RETURNVAL 0x8fbf0000
#define RESTORE_RETURNVAL_MASK 0xffff0000

/* ld $31,const($sp) is : 110 111 11101 11111 const */
/*                        1101 1111 1011 1111       */

#define RESTORE_RETURNVAL2 0xdfbf0000

/* lq $31,const($sp) is : 011110 11101 11111 const */
/* EE-CORE SPECIFIC       0111 1011 1011 1111      */

#define RESTORE_RETURNVAL3  0x7bbf0000

/* addiu $sp, $sp, const is 001 001 11101 11101 const */
/*                          0010 0111 1011 1101 const */

#define ADJUST_STACKP_C  0x27bd0000
#define ADJUST_STACKP_C_MASK 0xffff0000

/* addu $sp, $sp, $at is 000 000 11101 00001 11101 00000 100 001  */
/*                       0000 0011 1010 0001 1110 1000 0010 0001 */

#define ADJUST_STACKP_V  0x03a1e821
#define ADJUST_STACKP_V_MASK 0xffffffff

/* lui $at, const is 001 111 00000 00001 const */
/*       0011 1100 0000 0001 const */

#define SET_UPPER_C  0x3c010000
#define SET_UPPER_C_MASK 0xffff0000

/* ori $at, $at, const is 001 101 00001 00001 const */
/*                        0011 0100 0010 0001 const */

#define OR_LOWER_C  0x34210000
#define OR_LOWER_C_MASK  0xffff0000

/* ori $at, $zero, const is 001 101 00000 00001 const */
/*                          0011 0100 0000 0001 const */

#define SET_LOWER_C  0x34010000
#define SET_LOWER_C_MASK 0xffff0000

/* jr $ra */
#define RETURN     0x03e00008

#define CALL(f)     (0x0c000000 | (((int) (f)) >> 2))

/*
 * This computation is expensive, so we cache the results;
 * a simple hash function and straight-forward replacement.
 */

#define HASH_SIZE   256

typedef struct _returnCache {
    unsigned int    *returnAddress;
    int      raOffset;
    int      spAdjust;
} ReturnCacheRec, *ReturnCachePtr;

static ReturnCacheRec returnCache[HASH_SIZE];

#define HASH(ra)    ((((int) (ra)) >> 2) & (HASH_SIZE - 1))

void getStackTrace( unsigned int* ra, unsigned int* sp, unsigned int* results, int max )
{
	extern int     main();
	unsigned int*	ra_limit;
	unsigned int	inst;
	unsigned int	mainCall;
	unsigned short	const_upper;
	unsigned short	const_lower;
	int				ra_offset;
	int				sp_adjust;
	bool				found_ra_offset, found_sp_adjust;
	bool				found_const_upper, found_const_lower;
	ReturnCachePtr	rc;

	mainCall = CALL(main);

	while (ra && max)
	{
		rc = &returnCache[HASH(ra)];
		if (rc->returnAddress != ra)
		{
			found_ra_offset = FALSE;
			found_sp_adjust = FALSE;
			found_const_upper = FALSE;
			found_const_lower = FALSE;
			const_upper = 0;
			const_lower = 0;
			rc->returnAddress = ra;
			ra_limit = (unsigned int  *) 0x10000000;
			ra_offset = 0;
			sp_adjust = -1;
			while ((!found_ra_offset || !found_sp_adjust) && ra < ra_limit)
			{
				inst = *ra;
				/* look for the offset of the PC in the stack frame */
				if ((inst & RESTORE_RETURNVAL_MASK) == RESTORE_RETURNVAL)
				{
					ra_offset = inst & ~RESTORE_RETURNVAL_MASK;
					found_ra_offset = TRUE;
				}
				else if ((inst & RESTORE_RETURNVAL_MASK) == RESTORE_RETURNVAL2)
				{
					ra_offset = inst & ~RESTORE_RETURNVAL_MASK;
					found_ra_offset = TRUE;
				}
				else if ((inst & RESTORE_RETURNVAL_MASK) == RESTORE_RETURNVAL3)
				{
					ra_offset = inst & ~RESTORE_RETURNVAL_MASK;
					found_ra_offset = TRUE;
				}
				else if ((inst & ADJUST_STACKP_C_MASK) == ADJUST_STACKP_C)
				{
					sp_adjust = (short)(inst & ~ADJUST_STACKP_C_MASK);
					found_sp_adjust = TRUE;
				}
				else if ((inst & ADJUST_STACKP_V_MASK) == ADJUST_STACKP_V)
				{
					sp_adjust = 0;
					found_sp_adjust = TRUE;
				}
				else if ((inst & SET_UPPER_C_MASK) == SET_UPPER_C)
				{
					const_upper = inst & ~SET_UPPER_C_MASK;
					const_lower = 0;
					found_const_upper = TRUE;
				}
				else if ((inst & OR_LOWER_C_MASK) == OR_LOWER_C)
				{
					const_lower = inst & ~OR_LOWER_C_MASK;
					found_const_lower = TRUE;
				}
				else if ((inst & SET_LOWER_C_MASK) == SET_LOWER_C)
				{
					const_lower = inst & ~SET_LOWER_C_MASK;
					const_upper = 0;
					found_const_lower = TRUE;
				}
				else if (inst == RETURN)
					ra_limit = ra + 2;
				ra++;
			}
			if (sp_adjust == 0 && (found_const_upper || found_const_lower))
				sp_adjust = (const_upper << 16) | const_lower;
			rc->raOffset = ra_offset;
			rc->spAdjust = sp_adjust;
		}
		/* if something went wrong, punt */
		if (rc->spAdjust <= 0)
		{
			*results++ = 0;
			break;
		}
		ra = (unsigned int *) sp[rc->raOffset>>2];
		sp += rc->spAdjust >> 2;
		*results++ = ((unsigned int) ra) - 8;
		if (ra[-2] == mainCall)
		{
			*results++ = 0;
			break;
		}
		max--;
	}
}

//============================================================================

#endif // DISABLE_EXCEPTIONS
