//-----------------------------------------------------------------------------
//                               FMV Player
//
// $Workfile: fmvdisplay.c $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "shellCore.h"
#include "fmvdisplay.h"


//-----------------------------------------------------------------------------
// C O N S T A N T S
//-----------------------------------------------------------------------------

// Display mode screen dimensions

#define DISP_WIDTH_PAL 640
#define DISP_HEIGHT_PAL 256

#define DISP_WIDTH_NTSC 640
#define DISP_HEIGHT_NTSC 240

#define DISP_WIDTH_480P 704
#define DISP_HEIGHT_480P 480

// Offscreen texture buffer dimensions

#define MAX_WIDTH 768
#define UPLOADPAGE ((MAX_WIDTH * 576) / 64)


//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S 
//-----------------------------------------------------------------------------

static EFMVDisplayMode mode;
static int upperFirst;

static short dispWidth, dispHeight;
static EShellDispTVmode dispMode;
static void (*dispCallback)(int data);


// PATH3 DMA chain, always accessed through uncached pointers
static ShellDmaTag sprite[10] __attribute__((aligned(64)));
static volatile ShellDmaTag joinedList[4] __attribute__((aligned(64)));

// Registers used by the vblank handler

volatile static int tagQueueDepth;
volatile static ShellDmaTag *tagQueue[2];

// Interlace playback variables
volatile int frames;
volatile int topDone;

// PAL-from-NTSC playback variables
volatile int ntscTenThousandths;
volatile int fieldCount;
volatile int fieldCountTarget;

// Progressive playback variables
volatile int displayFields;


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

static void vSyncCallbackInterlace( int data );
static void vSyncCallbackPalFromNtsc( int data );
static void vSyncCallbackProgressive( int data );

static void spriteCreate( u_int polyX, u_int polyY, u_int polyW, u_int polyH,
						  u_int srcW, u_int srcH, u_int dbw, int tbp );
						 

//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

void FMVDisplayInit( EFMVDisplayMode displayMode, int upperFieldFirst,
					 u_int *pTbw, u_int *pTbp )
{
	mode = displayMode;
	upperFirst = upperFieldFirst;

	switch(mode)
	{
		case FMV_PAL:
			dispWidth = DISP_WIDTH_PAL;
			dispHeight = DISP_HEIGHT_PAL;
			dispMode = SHELLDISP_PAL;
			dispCallback = vSyncCallbackInterlace;
			break;

		case FMV_NTSC:
			dispWidth = DISP_WIDTH_NTSC;
			dispHeight = DISP_HEIGHT_NTSC;
			dispMode = SHELLDISP_NTSC;
			dispCallback = vSyncCallbackInterlace;
			break;

		case FMV_PAL_FROM_NTSC:
			dispWidth = DISP_WIDTH_PAL;
			dispHeight = DISP_HEIGHT_PAL;
			dispMode = SHELLDISP_PAL;
			dispCallback = vSyncCallbackPalFromNtsc;
			break;

		case FMV_480P:
			dispWidth = DISP_WIDTH_480P;
			dispHeight = DISP_HEIGHT_480P;
			dispMode = SHELLDISP_DTV480P;
			dispCallback = vSyncCallbackProgressive;
			break;
	}

	shellDispInit(SHELLDISP_INTERLACE_FRAME, dispMode,
				 dispWidth, dispHeight, SCE_GS_PSMCT32,
				 dispWidth, dispHeight, SCE_GS_PSMCT32,
				 SCE_GS_PSMZ16S);
				 
	shellDispSetTest(SCE_GS_SET_TEST_1(0, 0, 0, 0, 0, 0, 0, 0));
	shellDispSetClearType(SHELLDISP_NO_CLEAR);
	
	if(mode == FMV_480P)
	{
		// Progressive output bypasses shellDisp and always uses the display
		// at FBP=0
	
		sceGsSyncPath(0, 0);
		shellDispSwapDblBuffer();
		
		sceGsSyncPath(0, 0);
		*GS_DISPFB2 = (dispWidth / 64) << 9;
	}
	
	*pTbp = UPLOADPAGE;
	*pTbw = (MAX_WIDTH + 63) / 64;
}

void FMVDisplayCleanup()
{
	shellDispCleanup();
}

void FMVDisplayCreate( u_int vidW, u_int vidH, EFMVDisplayWidescreen wide )
{
	u_int polyX, polyY, polyW, polyH;

	frames = 0;
	tagQueueDepth = 0;
	
	switch(mode)
	{
		case FMV_PAL:
		case FMV_NTSC:
		default:
			polyW = vidW > dispWidth ? dispWidth : vidW;
			polyH = vidH / 2;
			topDone = 0;
			break;

		case FMV_PAL_FROM_NTSC:
			polyW = vidW > dispWidth ? dispWidth : vidW;
			polyH = (u_int) ((vidH/2) * 6) / 5;
			ntscTenThousandths = 0;
			fieldCount = 0;
			fieldCountTarget = 0;
			break;

		case FMV_480P:
			polyW = (u_int) dispWidth;
			polyH = (u_int) dispHeight;
			displayFields = 2; // ie. ready for a new one immediately
			break;		
	}
		
	switch(wide)
	{
		case FMV_WS_H_CROP:
			polyW = (polyW * 4) / 3;
			break;

		case FMV_WS_H_LETTERBOX:
			polyW = (polyW * 3) / 4;
			break;

		case FMV_WS_V_CROP:
			polyH = (polyH * 4) / 3;
			break;

		case FMV_WS_V_LETTERBOX:
			polyH = (polyH * 3) / 4;
			break;

		case FMV_WS_NONE:
		default:
			break;
	}

	polyX = 0x8000 - ((polyW / 2) << 4);
	polyY = 0x8000 - ((polyH / 2) << 4);

	if(mode == FMV_PAL_FROM_NTSC)
	{		
		// Y-offset needed to recentre image after stretch;
		// a function of the different (default) TV offsets between
		// PAL and NTSC
		polyY += 8 << 4;
	}
		
	spriteCreate(polyX, polyY, polyW, polyH, vidW, vidH,
				 (MAX_WIDTH + 63) / 64, (int) UPLOADPAGE);
}

void FMVDisplayAddFrame( ShellDmaTag *uploadTags )
{
	DI();
	tagQueue[tagQueueDepth++] = uploadTags;
	EI();

	// If the display handler hasn't processed the previous image yet, we have
	// to block to avoid overwriting it when we decode the next frame.

	while(tagQueueDepth == 2) shellCoreCallIdleRoutine();
}

void FMVDisplayStart( void )
{
	shellDispAddVSyncCallback(dispCallback, 0);

	while(frames == 0) shellCoreCallIdleRoutine();
}

int FMVDisplayFlush( void )
{
	switch(mode)
	{
		case FMV_PAL:
		case FMV_NTSC:
			return (tagQueueDepth == 0) && topDone;
			break;

		case FMV_PAL_FROM_NTSC:
			return (tagQueueDepth == 0) && (fieldCount >= fieldCountTarget);
			break;

		case FMV_480P:
			return (tagQueueDepth == 0) && (displayFields++ >= 2);
			break;
		
		default:
			return 1;
	}
}

void FMVDisplayDestroy( void )
{
	shellDispRemoveVSyncCallback(dispCallback);
}


//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

static void vSyncCallbackInterlace( int data )
{
	(void) data;

	// If the field just displayed was showing odd lines, and the movie
	// is encoded with odd-lines-first, then display the even field,
	// transfer the newly-decoded frame to the offscreen buffer, and begin
	// drawing its odd field to the odd buffer.
	// (And vice-versa for even-lines-first)
	
	if((((*GS_CSR) >> 13) & 1) == upperFirst)
	{
		if(tagQueueDepth > 0)
		{
			ShellDmaTag *ptr = (ShellDmaTag *) ((int) joinedList | 0x20000000);
		
			sceGsSyncPath(0, 0);			
			shellDispSwapDblBuffer();
			
			ptr->fields.id = SHELLDMA_TAG_ID_CALL;
			ptr->fields.next = (ShellDmaTag *) tagQueue[0];
			ptr++->fields.qwc = 0;
			
			ptr->fields.id = SHELLDMA_TAG_ID_NEXT;
			ptr->fields.next = sprite;
			ptr->fields.qwc = 0;

			sceGsSyncPath(0, 0);
			shellDmaStartChain(SHELLDMA_CHANNEL_GIF, (ShellDmaTag *) joinedList);
		
			if(tagQueueDepth == 2) tagQueue[0] = tagQueue[1];
			tagQueueDepth--;
			
			frames++;
			topDone = 1;
		}
		
		else PRINT("Frame out, frame %d", frames);
	}
	
	else
	{
		if(topDone)
		{
			sceGsSyncPath(0, 0);
			shellDispSwapDblBuffer();

			sceGsSyncPath(0, 0);
			shellDmaStartChain(SHELLDMA_CHANNEL_GIF, sprite);	

			topDone = 0;
		}
	}
}

static void vSyncCallbackPalFromNtsc( int data )
{
	ShellDmaTag *ptr = (ShellDmaTag *) ((int) joinedList | 0x20000000);

	(void) data;

	sceGsSyncPath(0, 0);
	shellDispSwapDblBuffer();

	if(fieldCount >= fieldCountTarget)
	{
		if(tagQueueDepth > 0)
		{
			if(ntscTenThousandths > 10000)
			{
				ntscTenThousandths -= 10000;
				fieldCountTarget = 1;
			}

			else fieldCountTarget = 2;
		
			fieldCount = 0;

			ptr->fields.id = SHELLDMA_TAG_ID_CALL;
			ptr->fields.next = (ShellDmaTag *) tagQueue[0];
			ptr++->fields.qwc = 0;
			
			if(tagQueueDepth == 2) tagQueue[0] = tagQueue[1];
			tagQueueDepth--;		
				
			frames++;
		}
		
		else PRINT("Frame out, frame %d", frames);
	}
			
	ptr->fields.id = SHELLDMA_TAG_ID_NEXT;
	ptr->fields.next = sprite;
	ptr->fields.qwc = 0;
		
	sceGsSyncPath(0, 0);
	shellDmaStartChain(SHELLDMA_CHANNEL_GIF, (ShellDmaTag *) joinedList);

	// One PAL field corresponds to exactly 1.1988 NTSC fields (59.94 / 50).
	// Thus for every PAL field, we drift 1988 ten-thousandths of a field
	// ahead of the corresponding NTSC field.

	ntscTenThousandths += 1988;

	fieldCount++;
}

static void vSyncCallbackProgressive( int data )
{
	(void) data;

	if(displayFields++ >= 2)
	{
		if(tagQueueDepth > 0)
		{
			sceGsSyncPath(0, 0);		
			shellDmaStartChain(SHELLDMA_CHANNEL_GIF,
								(ShellDmaTag *) tagQueue[0]);
		
			if(tagQueueDepth == 2) tagQueue[0] = tagQueue[1];
			tagQueueDepth--;
			
			sceGsSyncPath(0, 0);
			shellDmaStartChain(SHELLDMA_CHANNEL_GIF, sprite);

			frames++;
			displayFields = 1;
		}
	
		else PRINT("Frame out, frame %d", frames);
	}
}

static void spriteCreate( u_int polyX, u_int polyY, u_int polyW, u_int polyH,
						  u_int srcW, u_int srcH, u_int dbw, int tbp )
{
	ShellDmaTag *t = (ShellDmaTag *) ((int) sprite | 0x20000000);

	t->fields.qwc = 9;
	t++->fields.id = SHELLDMA_TAG_ID_RET;
	
	t->ulong.ul[0] = SCE_GIF_SET_TAG(8, 1, 1,
				SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 0, 0, 1, 0, 0),
				SCE_GIF_PACKED, 1);
	t++->ulong.ul[1] = SCE_GIF_PACKED_AD;

	if(mode == FMV_480P)
	{
		t->ulong.ul[0] = SCE_GS_SET_FRAME_1(0, dispWidth / 64, SCE_GS_PSMCT32, 0);
		t++->ulong.ul[1] = SCE_GS_FRAME_1;
	}

	else t++->ulong.ul[1] = SCE_GS_NOP;

	t->ulong.ul[0] = SCE_GS_SET_TEX0_1(tbp, dbw, SCE_GS_PSMCT32, 10, 10, 0,
								 SCE_GS_DECAL, 0, SCE_GS_PSMCT32, 0, 0, 0);
	t++->ulong.ul[1] = SCE_GS_TEX0_1;

	t->ulong.ul[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR,
								 0, 0, 0 );
	t++->ulong.ul[1] = SCE_GS_TEX1_1;
	
	t->ulong.ul[0] = 0;
	t++->ulong.ul[1] = SCE_GS_TEXFLUSH;

	t->ulong.ul[0] = SCE_GS_SET_UV(8, 8);
	t++->ulong.ul[1] = SCE_GS_UV;
		
	t->ulong.ul[0] = SCE_GS_SET_XYZ2(polyX, polyY, 0);
	t++->ulong.ul[1] = SCE_GS_XYZ2;

	t->ulong.ul[0] = SCE_GS_SET_UV(8 + (srcW << 4), 8 + (srcH << 4));
	t++->ulong.ul[1] = SCE_GS_UV;
	
	t->ulong.ul[0] = SCE_GS_SET_XYZ2(polyX + (polyW << 4),
								 polyY + (polyH << 4), 0);

	t++->ulong.ul[1] = SCE_GS_XYZ2;
}
