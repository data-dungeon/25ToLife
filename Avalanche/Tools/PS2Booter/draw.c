/*

	Drawing functions - James Russell - July 2000

*/

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <libdev.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>

#include "Draw.h"

//#define AA_ON		// is 2 circuit antialiasing on

	// -------------------------------------------------------------------
	// Constants
	// -------------------------------------------------------------------

#define PI (3.141592f)
#define SETVECTOR(v,x,y,z,w) ((v)[0]=x,(v)[1]=y,(v)[2]=z,(v)[3]=w)

#define setreg(register, value) *(p++) = (value); *(p++) = (register);
#define SPR_start (0x70000000)

	// -------------------------------------------------------------------
	// Typedefs
	// -------------------------------------------------------------------
typedef struct {
    tGS_PMODE   pmode;
    tGS_SMODE2  smode2;
    tGS_DISPFB1 dispfb1;
    tGS_DISPFB2 dispfb2;
    tGS_DISPLAY1 display1;
    tGS_DISPLAY2 display2;
    tGS_BGCOLOR bgcolor;
} DISPENV __attribute__((aligned(16)));

typedef struct {
	sceGifTag giftag;
    sceGsDrawEnv1 draw;
} DRAWENV __attribute__((aligned(16)));

	// -------------------------------------------------------------------
	// Globals
	// -------------------------------------------------------------------
static sceDmaChan *gDmaGif;				// GIF DMA channel handler

static char gCurrentFrame = 0;

static DRAWENV gDrawEnv[2];				// Drawing environments
static DISPENV gDispEnv[2];	// Display environments

static DRAWENV gFastWindowDrawEnv;
static sceGsDispEnv gFastWindowDispEnv;

static short gBufferAddr[2];		// Address of the large display buffers
short gGuiTextureAddr;				// VRAM Address of the font

static u_long *gCurPacketBufferAddr;
static u_long *gPacketBuffers[2];
static int gCurPacketBuffer;
static sceDmaChan *gDmaGif;

static u_long gBackgroundCol;

	// -------------------------------------------------------------------
	// Functions
	// -------------------------------------------------------------------

// ============================================================================
// Returns the address of the address to start building the next set of GIF packets
// and creates a NEXT DMA tag pointing to the previous tag
// ============================================================================

u_long *DRAW_getPacketBuffer(void) {
	return gCurPacketBufferAddr;
}

// ============================================================================
// Initialises the drawing system
// ============================================================================

void DRAW_init2(void) {
	int i;
	static int initialised = 0;

	if(initialised == 0) {
		initialised = 1;
		gDmaGif = sceDmaGetChan(SCE_DMA_GIF);

		for(i = 0 ; i < 2; i++) {
			gPacketBuffers[i] = (u_long *) memalign(16, 2048* 1024);	// 2 Meg packet buffers
			if(gPacketBuffers[i] == NULL) {
				puts("Could not allocate packet buffers\n");
				exit(1);
			}
				// Force buffer pointers to be uncached
			gPacketBuffers[i] = (u_long *)( ((int)gPacketBuffers[i] & 0x0fffffff) | 0x20000000);
		}
		gCurPacketBuffer = 0;
		gCurPacketBufferAddr = gPacketBuffers[gCurPacketBuffer] + 2;
	}
	DRAW_setBGCol(0x80000000);
}

// ============================================================================
// Once a GIF packet has been built, this will add it to the queue (or send the
// queue if the GIF is idle)
// ============================================================================

		// Assumes that the QWC has been filled in
void DRAW_addToDrawQueue(u_long *endOfPacket, int action) {
	u_int qwc;

	// action == 0 - I've just built some packets, if the GIF is free, please send them directly.
	// action = 1 - same as action == 0, but will flush the buffer
	// action = 2 - I've just made a DMA list, can you send it please?

	switch(action) {
		case 0:
		case 1:
		if(endOfPacket != NULL) {
		    if(((u_int)endOfPacket & 0xf) != 0) {    // if not 128 bit aligned, then pad out
   				printf("Error, endOfPacket is not aligned to 128 bits!\n");
				exit(1);
		    }

			gCurPacketBufferAddr = endOfPacket;
			qwc = ((u_int)gCurPacketBufferAddr - (u_int)gPacketBuffers[gCurPacketBuffer])  >> 4;
			gPacketBuffers[gCurPacketBuffer][0] = 0x70000000L | (qwc-1);
		}

			// If idle or flush, complete the DMA packet and send it.
		if(((DGET_D2_CHCR() & 0x0100) == 0) || (action == 1) || endOfPacket == NULL) {
			qwc = ((u_int)gCurPacketBufferAddr - (u_int)gPacketBuffers[gCurPacketBuffer])  >> 4;
			gPacketBuffers[gCurPacketBuffer][0] = 0x70000000L | (qwc - 1);
//			gPacketBuffers[gCurPacketBuffer][1] = 0;

//			FlushCache(0);
			asm("sync.l");

			sceDmaSend(gDmaGif, (u_long128*)((u_int)(gPacketBuffers[gCurPacketBuffer]) & 0x0fffffff));

				// Flip buffer
			gCurPacketBuffer ^= 1;
			gCurPacketBufferAddr = gPacketBuffers[gCurPacketBuffer] + 2;
		}
		break;

		case 2:
			if(gCurPacketBufferAddr != gPacketBuffers[gCurPacketBuffer] + 2) {
				DRAW_addToDrawQueue(NULL, 1);		// Flush anything that's currently there.
			}
			asm("sync.l");
			sceDmaSend(gDmaGif, (u_long128*)((u_int)(gCurPacketBufferAddr) & 0x0fffffff));
				// Flip buffer
			gCurPacketBuffer ^= 1;
			gCurPacketBufferAddr = gPacketBuffers[gCurPacketBuffer] + 2;
			break;
	}
}

	// -------------------------------------------------------------------
	// Drawing functions
	// -------------------------------------------------------------------

// ============================================================================
// set the colours that the screen is cleared to
// ============================================================================

void DRAW_setBGCol(u_int col) {
	gBackgroundCol = col;
}

// ============================================================================
// Clear the screen
// ============================================================================

void DRAW_clearScreen(void) {
	u_long *p = DRAW_getPacketBuffer();
	sceGifTag *gt = (sceGifTag *)p;
	const short x = ((2048 - SCREEN_WIDTH/2) << 4) & 0xffff;
	const short y = ((2048 - SCREEN_HEIGHT/2) << 4) & 0xffff;

	p += 2;

	gt->NLOOP = 1;
	gt->EOP = 1;
	gt->PRE = 0;
	gt->PRIM = SCE_GS_SET_PRIM(
					SCE_GS_PRIM_LINE,	// Prim
					0, // iip,
					0, // tme,
					0, // fge,
					0, // abe,
					0, // aa1,
					0, // fst,
					0, // ctxt,
					0);  //fix
	gt->FLG = 0;	// Packed
	gt->NREG = 4;
	gt->REGS0 = 0xe;
	gt->REGS1 = 0xe;
	gt->REGS2 = 0xe;
	gt->REGS3 = 0xe;

	setreg(SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE,0,0,0,0,0,0,0,0));
	setreg(SCE_GS_RGBAQ, gBackgroundCol);
	setreg(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(x, y, 0));
	setreg(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(x+(SCREEN_WIDTH<<4), y+(SCREEN_HEIGHT<<4), 0));

	DRAW_addToDrawQueue(p, 0);
}

// ============================================================================
// Draw a line
// ============================================================================

void DRAW_line(u_long col, int sx, int sy, int dx, int dy) {
	u_long *p = DRAW_getPacketBuffer();
	sceGifTag *gt = (sceGifTag *)p;

	p += 2;

	gt->NLOOP = 1;
	gt->EOP = 1;
	gt->PRE = 1;
	gt->PRIM = SCE_GS_SET_PRIM(
					SCE_GS_PRIM_LINE,	// Prim
					0, // iip,
					0, // tme,
					0, // fge,
					0, // abe,
					0, // aa1,
					0, // fst,
					0, // ctxt,
					0);  //fix
	gt->FLG = 0;	// Packed
	gt->NREG = 3;
	gt->REGS0 = 0xe;
	gt->REGS1 = 0xe;
	gt->REGS2 = 0xe;

	sx = ORIGINX(sx);
	sy = ORIGINY(sy);
	dx = ORIGINX(dx);
	dy = ORIGINY(dy);

	setreg(SCE_GS_RGBAQ, col);
	setreg(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(sx, sy, 0));
	setreg(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(dx, dy, 0));

	DRAW_addToDrawQueue(p, 0);
}

// ============================================================================
// Draw a solid rectangle
// ============================================================================

void DRAW_filledRect(u_long col, int x1, int y1, int x2, int y2) {
	u_long *p = DRAW_getPacketBuffer();
	sceGifTag *gt;

	gt = (sceGifTag *)p;
	p += 2;

	gt->NLOOP = 1;
	gt->EOP = 1;
	gt->PRE = 0;
	gt->PRIM = 0;
	gt->FLG = 1;    // REGLIST
	gt->NREG = 4;
	gt->REGS0 = 0x0;
	gt->REGS1 = 0x1;
	gt->REGS2 = 0xd;
	gt->REGS3 = 0x5;

	*(p++) = SCE_GS_SET_PRIM(
			SCE_GS_PRIM_SPRITE,   // Prim
				0, // iip,
				0, // tme,
				0, // fge,
				0, // abe,
				0, // aa1,
				0, // fst,
				0, // ctxt,
				0);  //fix

	*(p++) = col;
	*(p++) = ORIGINX(x1) | (ORIGINY(y1) << 16);
	*(p++) = ORIGINX(x2) | (ORIGINY(y2) << 16);

	DRAW_addToDrawQueue(p, 0);
}

// ============================================================================
// Draw a outline rectangle
// ============================================================================

void DRAW_outlineRect(u_long col, int x1, int y1, int x2, int y2) {
	u_long *p = DRAW_getPacketBuffer();
	sceGifTag *gt;

	gt = (sceGifTag *)p;
	p += 2;

	gt->NLOOP = 1;
	gt->EOP = 1;
	gt->PRE = 0;
	gt->PRIM = 0;
	gt->FLG = 1;    // REGLIST
	gt->NREG = 7;
	gt->REGS0 = 0x0;
	gt->REGS1 = 0x1;
	gt->REGS2 = 0xd;
	gt->REGS3 = 0x5;
	gt->REGS4 = 0x5;
	gt->REGS5 = 0x5;
	gt->REGS6 = 0x5;

	*(p++) = SCE_GS_SET_PRIM(
			SCE_GS_PRIM_LINESTRIP,   // Prim
				0, // iip,
				0, // tme,
				0, // fge,
				0, // abe,
				0, // aa1,
				0, // fst,
				0, // ctxt,
				0);  //fix

	*(p++) = col;
	*(p++) = ORIGINX(x1) | (ORIGINY(y1) << 16);
	*(p++) = ORIGINX(x2) | (ORIGINY(y1) << 16);
	*(p++) = ORIGINX(x2) | (ORIGINY(y2) << 16);
	*(p++) = ORIGINX(x1) | (ORIGINY(y2) << 16);
	*(p++) = ORIGINX(x1) | (ORIGINY(y1) << 16);
	*(p++) = 0;	// nop filler

	DRAW_addToDrawQueue(p, 0);
}

// ============================================================================
// Draw pixel
// ============================================================================

void DRAW_pixel(u_long col, int x, int y) {
	u_long *p = DRAW_getPacketBuffer();
	sceGifTag *gt;

	gt = (sceGifTag *)p;
	p += 2;

	gt->NLOOP = 1;
	gt->EOP = 1;
	gt->PRE = 1;
	gt->PRIM = SCE_GS_SET_PRIM(SCE_GS_PRIM_POINT,   // Prim
				0, // iip,
				0, // tme,
				0, // fge,
				0, // abe,
				0, // aa1,
				0, // fst,
				0, // ctxt,
				0);  //fix
	gt->FLG = 0;    // PACKED
	gt->NREG = 2;
	gt->REGS0 = 0xe;
	gt->REGS1 = 0xe;

	setreg(SCE_GS_RGBAQ, col);
	setreg(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(ORIGINX(x), ORIGINY(y), 0));

	DRAW_addToDrawQueue(p, 0);
}

// ============================================================================
// Put DISPENV that does the 2 circuit thing
// ============================================================================


static void myPutDispEnv(DISPENV *d) {
	DPUT_GS_PMODE(*(u_long *)&d->pmode);   //PMODE
	DPUT_GS_SMODE2(*(u_long *)&d->smode2);  //SMODE2
	DPUT_GS_DISPFB1(*(u_long *)&d->dispfb1);  //DISPFB
	DPUT_GS_DISPFB2(*(u_long *)&d->dispfb2);  //DISPFB
	DPUT_GS_DISPLAY1(*(u_long *)&d->display1); //DISPLAY
	DPUT_GS_DISPLAY2(*(u_long *)&d->display2); //DISPLAY
	DPUT_GS_BGCOLOR(*(u_long *)&d->bgcolor); //BGCOLOR
}

// ============================================================================
// Calculates the address following a given width/height, pixel storage mode
// ============================================================================
	
short sceNextFBaddr(short psm, short w, short h) {
	sceGsGParam *gp;
	short nw, nh;

	gp = sceGsGetGParam();
	nw = (w+63)/64;

	if(psm & 0x2){
		nh = (h+63)/64;
	}else{
		nh = (h+31)/32;
	}
	if((gp->sceGsInterMode == SCE_GS_INTERLACE) && (gp->sceGsFFMode == SCE_GS_FIELD)) {
		return (nw*nh);
	} else {
		return (nw*nh*2);
	}
}

// ============================================================================
// Initialises the display and draw structures
// ============================================================================

static void initDispDraw(int mode) {
	int i;
	sceGsDispEnv dispEnv;

	sceGsResetPath();

	if (mode == PAL_MODE)
	{
	sceGsResetGraph(
		0,
		SCE_GS_INTERLACE,
		SCE_GS_PAL,
		SCE_GS_FIELD);
	}
	else
	{
	sceGsResetGraph(
		0,
		SCE_GS_INTERLACE,
		SCE_GS_NTSC,
		SCE_GS_FIELD);
	}

	gBufferAddr[0] = 0;		// VRAM address of first 640x512 buffer
	gBufferAddr[1] = sceNextFBaddr(SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT); //VRAM addr of second 640x512 buffer
	for(i = 0; i < 2; i++) {
		sceGsSetDefDispEnv(&dispEnv, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
		gDispEnv[i].pmode = dispEnv.pmode;
		gDispEnv[i].smode2 = dispEnv.smode2;
		gDispEnv[i].dispfb2 = dispEnv.dispfb;
		gDispEnv[i].display2 = dispEnv.display;
		gDispEnv[i].bgcolor = dispEnv.bgcolor;

		memcpy(&gDispEnv[i].dispfb1, &gDispEnv[i].dispfb2, sizeof(tGS_DISPFB1));
		memcpy(&gDispEnv[i].display1, &gDispEnv[i].display2, sizeof(tGS_DISPLAY1));
		gDispEnv[i].dispfb1.FBP = gBufferAddr[i];
		gDispEnv[i].dispfb2.FBP = gBufferAddr[i];
		gDispEnv[i].dispfb2.DBY = 0;
#ifdef AA_ON
		gDispEnv[i].dispfb2.DBY = 1;
		gDispEnv[i].display2.DH -= 1;
#endif

		gDispEnv[i].pmode.MMOD = 1;
		gDispEnv[i].pmode.EN1 = 1;
		gDispEnv[i].pmode.EN2 = 1;
		gDispEnv[i].pmode.SLBG = 0;
		gDispEnv[i].pmode.ALP = 0x80;

			//-------------- set up draw env
		gDrawEnv[i].giftag.PRE = 0;
		gDrawEnv[i].giftag.FLG = 0;
		gDrawEnv[i].giftag.NLOOP = 8;
        gDrawEnv[i].giftag.EOP = 1;
        gDrawEnv[i].giftag.NREG = 1;
        gDrawEnv[i].giftag.REGS0 = 0xe; // A_D
		sceGsSetDefDrawEnv(&gDrawEnv[i].draw, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
		gDrawEnv[i].draw.frame1.FBP = gBufferAddr[i^1];

		gDrawEnv[i].draw.test1.ATE = 1;
		gDrawEnv[i].draw.test1.ATST = 4;
		gDrawEnv[i].draw.test1.AREF = 0x80;
		gDrawEnv[i].draw.test1.AFAIL = 0;
	}

	gCurrentFrame = 0;

	sceGsSetDefDispEnv(&gFastWindowDispEnv, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

	gFastWindowDrawEnv.giftag.PRE = 0;
	gFastWindowDrawEnv.giftag.FLG = 0;
	gFastWindowDrawEnv.giftag.NLOOP = 8;
	gFastWindowDrawEnv.giftag.EOP = 1;
	gFastWindowDrawEnv.giftag.NREG = 1;
	gFastWindowDrawEnv.giftag.REGS0 = 0xe; // A_D
	sceGsSetDefDrawEnv(&gFastWindowDrawEnv.draw, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
	gFastWindowDrawEnv.draw.frame1.FBP = 0;
	gFastWindowDrawEnv.draw.test1.ATE = 1;
	gFastWindowDrawEnv.draw.test1.ATST = 4;
	gFastWindowDrawEnv.draw.test1.AREF = 0x80;

	asm("sync.l");

	FlushCache(0);
	sceGsSyncV(0);
	myPutDispEnv(&gDispEnv[(u_int)gCurrentFrame]);
	sceGsPutDrawEnv(&gDrawEnv[(u_int)gCurrentFrame].giftag);
}

// ============================================================================
// Ask the handler to flip the disp/draw buffers at the next VBlank
// ============================================================================


void DRAW_flipScreen(void) {
	u_long *p = (u_long *)SPR_start;
	const u_long copy[2] = {
		SCE_GIF_SET_TAG(4, // nloop = 4
						1,	// EOP = 1
						0,	// pre = 0
						0,	// prim = 0
						0,	// flg = 0 = PACKED
						1),	// NREG = 1
			0xe
	};

	DRAW_addToDrawQueue(NULL, 1);	// Flush the output queue

	sceGsSyncV(0);
	gCurrentFrame ^= 1;
	myPutDispEnv(&gDispEnv[(u_int)gCurrentFrame]);
	sceGsPutDrawEnv(&gDrawEnv[(u_int)gCurrentFrame].giftag);


		/* copy over old frame into new drawing buffer */
	*(p++) = copy[0];
	*(p++) = copy[1];

	setreg(SCE_GS_BITBLTBUF, SCE_GS_SET_BITBLTBUF(
			gBufferAddr[(u_int)gCurrentFrame] << 5, SCREEN_WIDTH >> 6, SCE_GS_PSMCT32,
			gBufferAddr[(u_int)gCurrentFrame^1] << 5, SCREEN_WIDTH >> 6, SCE_GS_PSMCT32
			)
	);
	setreg(SCE_GS_TRXREG, SCE_GS_SET_TRXREG(SCREEN_WIDTH, SCREEN_HEIGHT));
	setreg(SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0,0,0,0,0));
	setreg(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(2));	// Local->Local Transmission
	asm("sync.l");
	FlushCache(0);
	sceDmaSendN(gDmaGif, (u_long128*) (SPR_start | 0x80000000), ((u_int)p - (u_int)SPR_start)/16);
	sceGsSyncPath(0,0);
}

// ============================================================================
// Put the fast screen onto the screen
// ============================================================================

void DRAW_putFastScreen(void) {
	DRAW_addToDrawQueue(NULL, 1);
	sceGsSyncV(0);

	sceGsPutDispEnv(&gFastWindowDispEnv);
	sceGsPutDrawEnv(&gFastWindowDrawEnv.giftag);
}

// ============================================================================
// Initialise the system and program variables
// ============================================================================

void DRAW_init(int mode)
{
	u_long *p;
	sceGifTag *gt;

			// GIFTAG used for setting up various registers

		// -------------------------------------------------------------------------
		// Initialise module variables
		// -------------------------------------------------------------------------
		// -------------------------------------------------------------------------
		// Initialise DMA and set up the screen
		// -------------------------------------------------------------------------

	sceDmaReset(1);
	gDmaGif = sceDmaGetChan(SCE_DMA_GIF);		// Get address of GIF DMA handler
	gDmaGif->chcr.TTE = 0;

	initDispDraw(mode);
	DRAW_init2();

	DRAW_clearScreen();
	DRAW_flipScreen();
	DRAW_clearScreen();
	DRAW_flipScreen();
	DRAW_clearScreen();

		// -------------------------------------------------------------------------
		// Load the texture into VRAM
		// -------------------------------------------------------------------------


	gGuiTextureAddr = gBufferAddr[1] * 2 * 2048 / 64;

		// -------------------------------------------------------------------------
		// Set up various GS registers
		// -------------------------------------------------------------------------

	p = DRAW_getPacketBuffer();
	gt = (sceGifTag *)p;
	p += 2;

	gt->NLOOP = 0;
	gt->EOP = 1;
	gt->NREG = 1;
	gt->PRE = 0;
	gt->FLG = 0;		// PACKED
	gt->REGS0 = 0xe;

	setreg(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(1, 0, 2, 2, 0x80));
	gt->NLOOP++;
	setreg(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 5, 0x01, 0,  0, 0, 1, 1));
	gt->NLOOP++;
	setreg(SCE_GS_TEX0_1, SCE_GS_SET_TEX0(
			gGuiTextureAddr,				// TBP0 (Texture Base Pointer)
			4,								// TBW (Texture Width in Texels / 64)
			SCE_GS_PSMCT32,					// PSM (Pixel storage format)
			8,								// TW (Texture Width, 2^TW)
			6,								// TH (Texture Height, 2^TH)
			1,								// TCC (Texture Colour Component)
			0,								// TFX (Texture Function)
			0,								// CBP (CLUT Buffer Base Pointer)
			0,								// CPSM (CLUT Pixel Storage Format)
			0,								// CSM (CLUT Storage Mode)
			0,								// CSA (CLUT Entry Offset)
			0								// CLD (CLUT Buffer Load Control)
			) );
	gt->NLOOP++;
	setreg(SCE_GS_TEX1_1, SCE_GS_SET_TEX1(
			0,			// LCM (LOD Calculation Method)
			0,			// MXL (Maximum MipMap Level)
			0,			// MMAG (Filter when texture is expanded (LOD < 0))
			1,			// MMIN (filter when texture is reduced (LOD >=0))
			0,			// MTBA (Base Address Specification of MipMap Text Level 1 or more)
			0,			// L (LOD Parameter L)
			0			// K (LOD Parameter K)
			) );
	gt->NLOOP++;
	setreg(SCE_GS_TEXA, SCE_GS_SET_TEXA(
			0x0,		// TA0 ('As' Value when A is 0 in RGB16 or RGB24)
			0,			// AEM (Alpha Expansion Method)
			0x80		// TA1 ('As' Value when A is 1 in RGB16 or RGB24)
			) );
	gt->NLOOP++;

    DRAW_addToDrawQueue(p, 0);
	DRAW_flipScreen();
}

