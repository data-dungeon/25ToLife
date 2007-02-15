//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellDisp - efficient display handling and fullscreen antialiasing
//
// $Workfile: shellDisp.c $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
// Description: Setup of display double buffer
//              Sets up VRAM for anti-alias in either two-circuit or sprite
//              downsampled mode
//              Performs fast screen clears using strips
//              Supports 50/60hz switching
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "shellCore.h"


//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef struct _ShellDblBuff
{
	tGS_PMODE    pmode;
	tGS_SMODE2   smode2;
	tGS_DISPLAY1 display1;
	tGS_DISPLAY1 display2;
	tGS_DISPFB1  dispfb1[2];	// Double buffered
	tGS_DISPFB1  dispfb2[2];

	short bufferPointer[2];

	int currentBuffer;
	
	int rebuild;

	short zPointer;
	
	short dispPSM;
	short dispWidth;
	short dispHeight;
	short dispDX;
	short dispDY;
	
	short drawPSM;
	short drawWidth;
	short drawHeight;
	
	short zPSM;
	
	int fieldMode;
	int interlaceMode;
	int tvType;
	int canFlip;
	int offsetRender;
	int clearType;
	unsigned int clearColour;
	
	int twoCircuit;

} ShellDblBuff __attribute__((aligned(64)));


//-----------------------------------------------------------------------------
// G L O B A L   V A R I A B L E S
//-----------------------------------------------------------------------------

#define VU_EXTERN(a)\
extern unsigned int a __attribute__((section(".vudata")))

VU_EXTERN(gvShellDispVUDrawEnv1);
VU_EXTERN(gvShellDispVUDrawEnv2);
VU_EXTERN(gvShellDispShrinkAndClearStart1);
VU_EXTERN(gvShellDispShrinkAndClearEnd1);
VU_EXTERN(gvShellDispClearScreenStart1);
VU_EXTERN(gvShellDispClearScreenEnd1);
VU_EXTERN(gvShellDispShrinkScreenStart1);
VU_EXTERN(gvShellDispShrinkScreenEnd1);
VU_EXTERN(gvShellDispNoClearStart1);
VU_EXTERN(gvShellDispNoClearEnd1);
VU_EXTERN(gvShellDispShrinkAndClearStart2);
VU_EXTERN(gvShellDispShrinkAndClearEnd2);
VU_EXTERN(gvShellDispClearScreenStart2);
VU_EXTERN(gvShellDispClearScreenEnd2);
VU_EXTERN(gvShellDispShrinkScreenStart2);
VU_EXTERN(gvShellDispShrinkScreenEnd2);
VU_EXTERN(gvShellDispNoClearStart2);
VU_EXTERN(gvShellDispNoClearEnd2);
VU_EXTERN(gvShellDispShrinkScreenEnv1);
VU_EXTERN(gvShellDispShrinkSpriteTag01);
VU_EXTERN(gvShellDispShrinkSpriteTag11);
VU_EXTERN(gvShellDispShrinkScreenEnv2);
VU_EXTERN(gvShellDispShrinkSpriteTag02);
VU_EXTERN(gvShellDispShrinkSpriteTag12);
VU_EXTERN(gvShellDispClearScreenEnv1);
VU_EXTERN(gvShellDispClearSpriteTag01);
VU_EXTERN(gvShellDispClearSpriteTag11);
VU_EXTERN(gvShellDispClearScreenEnv2);
VU_EXTERN(gvShellDispClearSpriteTag02);
VU_EXTERN(gvShellDispClearSpriteTag12);


struct {
	ShellDispDrawEnv *DrawEnv;
	ShellDmaTag *ShrinkAndClearStart;
	ShellDmaTag *ShrinkAndClearEnd;
	ShellDmaTag *ClearStart;
	ShellDmaTag *ClearEnd;
	ShellDmaTag *ShrinkStart;
	ShellDmaTag *ShrinkEnd;
	ShellDmaTag *EnvOnlyStart;
	ShellDmaTag *EnvOnlyEnd;
	void *ClearSprite0;
	void *ClearSprite1;
	void *ClearEnv;
	void *ShrinkSprite0;
	void *ShrinkSprite1;
	void *ShrinkEnv;
} _VUPointers[2]={
	{
		(ShellDispDrawEnv *)&gvShellDispVUDrawEnv1,
		(ShellDmaTag *)&gvShellDispShrinkAndClearStart1,
		(ShellDmaTag *)&gvShellDispShrinkAndClearEnd1,
		(ShellDmaTag *)&gvShellDispClearScreenStart1,
		(ShellDmaTag *)&gvShellDispClearScreenEnd1,
		(ShellDmaTag *)&gvShellDispShrinkScreenStart1,
		(ShellDmaTag *)&gvShellDispShrinkScreenEnd1,
		(ShellDmaTag *)&gvShellDispNoClearStart1,
		(ShellDmaTag *)&gvShellDispNoClearEnd1,
		&gvShellDispClearSpriteTag01,
		&gvShellDispClearSpriteTag11,
		&gvShellDispClearScreenEnv1,
		&gvShellDispShrinkSpriteTag01,
		&gvShellDispShrinkSpriteTag11,
		&gvShellDispShrinkScreenEnv1
	},{
		(ShellDispDrawEnv *)&gvShellDispVUDrawEnv2,
		(ShellDmaTag *)&gvShellDispShrinkAndClearStart2,
		(ShellDmaTag *)&gvShellDispShrinkAndClearEnd2,
		(ShellDmaTag *)&gvShellDispClearScreenStart2,
		(ShellDmaTag *)&gvShellDispClearScreenEnd2,
		(ShellDmaTag *)&gvShellDispShrinkScreenStart2,
		(ShellDmaTag *)&gvShellDispShrinkScreenEnd2,
		(ShellDmaTag *)&gvShellDispNoClearStart2,
		(ShellDmaTag *)&gvShellDispNoClearEnd2,
		&gvShellDispClearSpriteTag02,
		&gvShellDispClearSpriteTag12,
		&gvShellDispClearScreenEnv2,
		&gvShellDispShrinkSpriteTag02,
		&gvShellDispShrinkSpriteTag12,
		&gvShellDispShrinkScreenEnv2
	}
};

#define MAX_VSYNC_CALLBACKS 8

static ShellDblBuff _shellDispDB;
static ShellDblBuff *_pShellDispDB = NULL;
static int _shellDispFrame = 0;
static int _waitDisplay;
static void(*_Vcallback[MAX_VSYNC_CALLBACKS])(int);
int _Vuserdata[MAX_VSYNC_CALLBACKS];
u_long _test_1;
static volatile int _VBlank;

static int vbshandler = -1;
static int vbehandler = -1;

//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

static inline short _GetNumPages( short width, short height, short pixelFormat );
static void _SetDefDispEnv( ShellDblBuff *pDB);
static void _SetDispBuffers( ShellDblBuff *pDB );
static void _SetDrawEnv( ShellDispDrawEnv *draw, short drawFBP, short psm, 
						 short w, short h, short zFBP,
						 short zpsm );
static void _CopyDrawEnvToContext2( ShellDispDrawEnv *draw);
static void _SetupShrinkSprites( ShellDblBuff *pDB );
static void _SetupClearBuffer( ShellDblBuff *pDB, EShellDispClear clearType );

static void _shellDispResetGraph(  EShellDispTVmode tv_type, EShellDispInterlaceMode interlace_mode );

static void _shellDispSetupDblBuffer(short dispWidth, short dispHeight, 
									short displayPixelMode, 
									short drawWidth, short drawHeight,
									short drawPixelMode,
									short zPixelMode);
static int _VBlankHandler(int c,void *,void *);

//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// _GetNumPages()
// Description: Works out rounded up number of pages based on width, height and
//              pixel storage format
// Parameters: width, height, pixelFormat 
//             - where pixelFormat is a standard GS pixel or Z buffer format
// Returns:    Number of pages
//-----------------------------------------------------------------------------

static inline short _GetNumPages( short width, short height, short pixelFormat )
{
	width = (short)((width+63)/64);
	
	if(pixelFormat&0x2)  // if 16-bit
		height = (short)((height+63)/64);
	else
		height = (short)((height+31)/32);
	
	return (short) (width*height);
}

//-----------------------------------------------------------------------------
// _SetDefDispEnv()
// Description: Sets default display buffer 
// Parameters: pDB  - pointer to double buffer
// Notes:      Replaces sceGsSetDefDispEnv to support two-circuit display env
//-----------------------------------------------------------------------------
static void _SetDefDispEnv(ShellDblBuff *pDB)
{
int w,h,dx,dy;
int s;

	*(u_long *) &pDB->pmode = (u_long) 0x0000000000008026; // EN1=0
	if(pDB->interlaceMode==SCE_GS_INTERLACE)
	{ 
		if(pDB->fieldMode==SCE_GS_FRAME)
			*(u_long *) &pDB->smode2 = (u_long) 3; // frame
		else
			*(u_long *) &pDB->smode2 = (u_long) 1; // field
	}
	else  // no interlace
	{	
		*(u_long *) &pDB->smode2 = (u_long) 2;
	}
	
	w=pDB->dispWidth;
	h=pDB->dispHeight;
	dx=pDB->dispDX;
	dy=pDB->dispDY;	

	pDB->dispfb1[0].FBW = pDB->dispWidth>>6;
	pDB->dispfb1[0].PSM = pDB->dispPSM;
	pDB->dispfb1[0].DBX = 0;
	
	if(pDB->canFlip)
	{
		s=1;
		pDB->dispfb1[0].DBY = 0;
		pDB->dispfb1[1].DBY = 0;
		pDB->dispfb1[1].FBP = pDB->bufferPointer[1];
		pDB->dispfb1[0].FBP = pDB->bufferPointer[0];
	} else {
		s=3;

		pDB->dispfb1[0].DBY = 1;
		pDB->dispfb1[1].DBY = 1;
		pDB->dispfb1[1].FBP = 0;
		pDB->dispfb1[0].FBP = 0;
	}
	pDB->dispfb1[1].FBW = pDB->dispWidth>>6;
	pDB->dispfb1[1].PSM = pDB->dispPSM;
	pDB->dispfb1[1].DBX = 0;
		
	if(pDB->tvType == SCE_GS_NTSC)
	{
		if(pDB->interlaceMode == SCE_GS_INTERLACE)	
		{
			*(u_long *) &pDB->display1 = (u_long) 0
				| ((u_long) (636+(dx*((2560+w-1)/w)))&0xfff)
				| ((u_long) ((50+dy) & 0xfff)             << 12)
				| ((u_long) (((2560+w-1)/w)-1)            << 23)
				| ((u_long) ((((2560+w-1)/w)*w)-1)        << 32)
				| ((u_long) ((h<<(pDB->fieldMode?1:0))-s) << 44);
		}
		else
		{
			*(u_long *) &pDB->display1 = (u_long) 0
				| ((u_long) (636+(dx*((2560+w-1)/w)))&0xfff)
				| ((u_long) ((25+dy) & 0xfff)      << 12)
				| ((u_long) (((2560+w-1)/w)-1)     << 23)
				| ((u_long) ((((2560+w-1)/w)*w)-1) << 32)
				| ((u_long) (h-s)                  << 44);
		}
	}
	else if(pDB->tvType == SCE_GS_PAL)
	{
		if(pDB->interlaceMode == SCE_GS_INTERLACE)
		{
			*(u_long *) &pDB->display1 = (u_long) 0
				| (((u_long) 656 + (dx*((2560+w-1)/w)))&0xfff)
				| ((u_long) ((72+dy) & 0xfff)             << 12)
				| ((u_long) (((2560+w-1)/w)-1)            << 23)
				| ((u_long) ((((2560+w-1)/w)*w)-1)        << 32)
				| ((u_long) ((h<<(pDB->fieldMode?1:0))-s) << 44);
		}
		else
		{
			*(u_long *) &pDB->display1 = (u_long) 0
				| (((u_long) 656 + (dx*((2560+w-1)/w)))&0xfff)
				| ((u_long) ((36+dy) & 0xfff)      << 12)
				| ((u_long) (((2560+w-1)/w)-1)     << 23)
				| ((u_long) ((((2560+w-1)/w)*w)-1) << 32)
				| ((u_long) (h-s)                  << 44);
		}
	}
	else
	{
//479,1439,0,1,35,232
		{
			*(u_long *) &pDB->display1 = (u_long) 232+dx
				| ((u_long) (35+dy)     <<12 )
				| ((u_long) ((1550/w)-1) << 23)
				| ((u_long) 0     << 27)
				| ((u_long) (w*(1550/w)-1) << 32)
				| ((u_long) h-s      << 44);
		}
	}

	if(_pShellDispDB->twoCircuit)
		_pShellDispDB->pmode.EN1=1;
}

//-----------------------------------------------------------------------------
// _SetDispBuffers()
// Description: Sets display buffer structure for double buffer
// Parameters:  pDB - pointer to structure holding display settings
// Notes:       Replaces sceGsSetDispEnv to allow for two-circuit buffers
//-----------------------------------------------------------------------------
static void _SetDispBuffers( ShellDblBuff *pDB )
{
	// Buffer 1 
	_SetDefDispEnv(pDB); // Set env for 1 circuit

	pDB->dispfb2[0]=pDB->dispfb1[0]; // Copy to 2nd circuit
	pDB->dispfb2[1]=pDB->dispfb1[1];
	pDB->display2=pDB->display1;
	
	pDB->dispfb1[0].DBY=pDB->dispfb1[1].DBY=(pDB->dispfb1[0].DBY+1); // Move down DBY of 2nd circuit
	pDB->display1.DH=pDB->display1.DH-1; // Draw one line less of 2nd circuit
}

//-----------------------------------------------------------------------------
// _CopyDrawEnvToContext2()
// Description: Ensures all context1 draw env settings are copied to context2
// Parameters:  draw          - draw environment structure
// Notes:       This must only be called when all context1 values are complete
//-----------------------------------------------------------------------------
static void _CopyDrawEnvToContext2( ShellDispDrawEnv *draw)
{

	draw->frame2addr     = (long) SCE_GS_FRAME_2;
	draw->zbuf2addr      = (long) SCE_GS_ZBUF_2; 
	draw->xyoffset2addr  = (long) SCE_GS_XYOFFSET_2;
	draw->scissor2addr   = (long) SCE_GS_SCISSOR_2;
	draw->test2addr      = (long) SCE_GS_TEST_2; 

	*(u_long *) &draw->frame2    = *(u_long *) &draw->frame1;
	*(u_long *) &draw->zbuf2     = *(u_long *) &draw->zbuf1;
	*(u_long *) &draw->xyoffset2 = *(u_long *) &draw->xyoffset1;
	*(u_long *) &draw->scissor2  = *(u_long *) &draw->scissor1;
	*(u_long *) &draw->test2     = *(u_long *) &draw->test1;
	
}
//-----------------------------------------------------------------------------
// _SetDrawEnv()
// Description: Sets drawenv structure
// Parameters:  draw          - draw environment structure
//              drawFBP       - draw frame buffer pointer
//              psm           - draw pixel storage format
//              w, h          - draw width and height
//              zFBP          - Z buffer pointer
//              zPSM          - z pixel storage format
// Notes:       Replaces sceGsSetDrawEnv to allow support for alpha test modes
//-----------------------------------------------------------------------------
static void _SetDrawEnv( ShellDispDrawEnv *draw, short drawFBP, short psm, 
						 short w, short h, short zFBP,
						 short zpsm )
{

	SCE_GIF_CLEAR_TAG(&draw->gifTag);
	draw->gifTag.NLOOP=14;  
	draw->gifTag.EOP=1;
	draw->gifTag.NREG=1;    
	draw->gifTag.REGS0=0xe;

	draw->frame1addr     = (long) SCE_GS_FRAME_1;
	draw->zbuf1addr      = (long) SCE_GS_ZBUF_1; 
	draw->xyoffset1addr  = (long) SCE_GS_XYOFFSET_1;
	draw->scissor1addr   = (long) SCE_GS_SCISSOR_1;
	draw->prmodecontaddr = (long) SCE_GS_PRMODECONT;
	draw->colclampaddr   = (long) SCE_GS_COLCLAMP;
	draw->dtheaddr       = (long) SCE_GS_DTHE; 
	draw->test1addr      = (long) SCE_GS_TEST_1; 
	draw->dimxaddr       = (long) SCE_GS_DIMX; 
	
	*(u_long *) &draw->frame1 = SCE_GS_SET_FRAME(drawFBP, ((w+63)>>6)&0x3f, 
												 psm&0xf, 0);
	
	// Fix for GS depth test restriction
	if((draw->test1.ZTE) == 0) // If zte = 0 
    {
		*(u_long *)&draw->zbuf1 = SCE_GS_SET_ZBUF(zFBP, zpsm&0xf, 1); // zmsk=1
		draw->test1.ZTE = 1;  // zte=on
		draw->test1.ZTST = 1;  // ztst=always
    }
	else
    {
		*(u_long *)&draw->zbuf1 = SCE_GS_SET_ZBUF(zFBP, zpsm&0xf, 0);
    }
		
	*(u_long *)&draw->scissor1 = SCE_GS_SET_SCISSOR(0, (w-1), 0, (h-1));
	
	draw->prmodecont.AC = 1;   // get prim attributes from prim register
	draw->colclamp.CLAMP = 1;  // as we don't want colours wrapping round
	*(u_long *)&draw->dimx = 0x0716524334256170;// default dither matrix
	
	if(psm & 0x2) draw->dthe.DTHE = 1; // enable dither for 16-bit
	else          draw->dthe.DTHE = 0; // else disable dither
	
	__asm__ __volatile__("sync.l"); 
}

//-----------------------------------------------------------------------------
// _SetupShrinkSprites()
// Description: Sets up sprites ready for VU1 microprogram
// Parameters:  pDB - pointer to structure holding display settings
// Notes:       This is for anti-interlace flicker filtering by downsampling
//              an offscreen buffer to the display using a VU1 program
//-----------------------------------------------------------------------------
static void _SetupShrinkSprites( ShellDblBuff *pDB )
{
	int numLoop = pDB->dispWidth>>6;
	unsigned int width;
	
	u_long *pShrinkScreenEnv = (u_long*) _VUPointers[pDB->currentBuffer].ShrinkEnv;
	u_int *pShrinkSpriteTag0 = (u_int*)	 _VUPointers[pDB->currentBuffer].ShrinkSprite0;
	u_int *pShrinkSpriteTag1 = (u_int*)	 _VUPointers[pDB->currentBuffer].ShrinkSprite1;
	
	// Set up environment for shrink (only need to do this once)
	pShrinkScreenEnv[0] = SCE_GS_SET_XYOFFSET_1(0, 0);

	// Always
 	pShrinkScreenEnv[2] = SCE_GS_SET_TEST_1(0, 0, 0, 0, 0, 0, 1, 1);        

	// Don't write Z
	pShrinkScreenEnv[4] = SCE_GS_SET_ZBUF_1(pDB->zPointer, pDB->dispPSM&0xf, 1);
	
	pShrinkScreenEnv[6] = SCE_GS_SET_FRAME_1(0, pDB->dispWidth>>6,
											 pDB->dispPSM, 0);
	
	// Dither if 16-bit
	pShrinkScreenEnv[8]  = SCE_GS_SET_DTHE((pDB->dispPSM&0x2)==0x2);

	pShrinkScreenEnv[10] = SCE_GS_SET_SCISSOR_1(0, pDB->dispWidth-1, 0,
												pDB->dispHeight-1);

	// Bilinear enabled
	pShrinkScreenEnv[12] = SCE_GS_SET_TEX1_1(1, 0, 1, 1, 1, 0, 1);

	// RGBA, Decal, no clut
	pShrinkScreenEnv[14] = SCE_GS_SET_TEX0_1(pDB->bufferPointer[0]<<5,
											 pDB->drawWidth>>6, pDB->drawPSM,
											 10, 10, 1, 1, 0, 0, 0, 0, 0);
	
	// Set up sprites and loop count for shrink (uv sprite)
	*(long *) pShrinkSpriteTag0 = SCE_GIF_SET_TAG(1, 1, 1, 0x116, 0, 5);
	
	// Set up loop count (width divided by 64)
	pShrinkSpriteTag0[3] = (unsigned int) numLoop; 
	
	pShrinkSpriteTag0[4] = 0x80;   // rgba
	pShrinkSpriteTag0[5] = 0x80;   // rgba
	pShrinkSpriteTag0[6] = 0x80;   // rgba
	pShrinkSpriteTag0[7] = 0x80;   // rgba
	
	pShrinkSpriteTag0[8] = 0x3f800008;	// uv (set u=left)
	pShrinkSpriteTag0[9] = 0x00000008;	// uv (set v=top)
	
	pShrinkSpriteTag0[12] = 0x3f800000;   // xyz (set x=left)
	pShrinkSpriteTag0[13] = 0x0;   // xyz (set y=top)

	width=(unsigned int)((pDB->drawWidth*16) / (numLoop*2));
	width+=0x3f800000;
	*(float *)&width-=1.0f;
	
	pShrinkSpriteTag0[16] = width;               // uv (set u=right)
	pShrinkSpriteTag0[17] = 8+(unsigned int) ((pDB->drawHeight)<<4);  // uv (set v=bottom)

	width=0x3f800200;
	*(float *)&width-=1.0f;
	pShrinkSpriteTag0[20] = width;              
	pShrinkSpriteTag0[21] = 0+(unsigned int) ((pDB->dispHeight)<<4); // xyz (set y=bottom)

	// Set up sprite1
	memcpy(pShrinkSpriteTag1, pShrinkSpriteTag0, 96);
}



//-----------------------------------------------------------------------------
// _SetupClearBuffer()
// Description: Setup of clear buffer ready for microprogram start
// Parameters:  pDB         - pointer to structure holding display settings
//              clearType   - SHELLDISP_NO_CLEAR           (no clear)
//                            SHELLDISP_CLEAR_Z            (Z buffer)
//                            SHELLDISP_CLEAR_FRAME        (frame)
//                            SHELLDISP_CLEAR_Z_AND_FRAME  (frame and Z)
// Notes:       Uses both frame and Z write to clear single buffer twice as
//              fast. Uses 32-bit mode to clear 16-bit twice as fast
//-----------------------------------------------------------------------------
static void _SetupClearBuffer( ShellDblBuff *pDB, EShellDispClear clearType )
{
	int halfZsize = _GetNumPages(pDB->drawWidth, pDB->drawHeight, pDB->zPSM)>>1;
	int halfFrameSize = _GetNumPages(pDB->drawWidth,pDB->drawHeight,pDB->drawPSM)>>1; 
	int stripHeight = pDB->drawHeight;
	int currentFBP;
	int numStrips = pDB->drawWidth>>6; // width divided by 64
	int bStrangeHeight = 0;
	u_long *pClearScreenEnv = (u_long*) _VUPointers[pDB->currentBuffer].ClearEnv;
	u_int *pClearSpriteTag0 = (u_int*)  _VUPointers[pDB->currentBuffer].ClearSprite0;
	u_int *pClearSpriteTag1 = (u_int*)  _VUPointers[pDB->currentBuffer].ClearSprite1;


	currentFBP = pDB->bufferPointer[1^pDB->currentBuffer];

	// Set default RGBA = black, Z=0
	pClearSpriteTag0[4]  = 0x0;   //rg
	pClearSpriteTag0[5]  = 0x0;   //ba
	pClearSpriteTag0[6]  = 0x0;   //rg
	pClearSpriteTag0[7]  = 0x0;   //ba
	pClearSpriteTag0[10] = 0x0;  //z
	pClearSpriteTag0[14] = 0x0;  //z

	// Mask colour bits to 16-bit if using 16-bit format
	if((pDB->drawPSM&0x2)==0x2)
		pDB->clearColour = pDB->clearColour & 0xfff8f8f8;

	// Clear in 32 pixel wide strips using VU program to create strips
	// Use Z write for half of the clear when clearing only one buffer
	
	// Set up environment and strip height for selected type of clear
	switch(clearType)   // Select clear type =(none, z, frame, both)
    {
    case SHELLDISP_NO_CLEAR:
		break;

    case SHELLDISP_CLEAR_Z: 
		if((pDB->zPSM&0x2)||(pDB->drawPSM&0x2))
			bStrangeHeight = (pDB->drawHeight%128)!=0; // 16-bit pages are 64 pixels high
		else
			bStrangeHeight = (pDB->drawHeight%64)!=0; // 32-bit pages are 32 pixels high

		if (bStrangeHeight)
		{
			stripHeight = pDB->drawHeight;
			pClearScreenEnv[2] = SCE_GS_SET_ZBUF_1(pDB->zPointer, pDB->zPSM, 0); // write z
			pClearScreenEnv[4] = SCE_GS_SET_FRAME_1(currentFBP,
												   (pDB->drawWidth>>6),
													pDB->drawPSM, 0xffffffff); // no frame write
		}
		else // halve the draw height and use z and frame together for fast clear
		{
			stripHeight = pDB->drawHeight>>1; 
			pClearScreenEnv[2] = SCE_GS_SET_ZBUF_1(pDB->zPointer+halfZsize, pDB->zPSM, 0); // write z
			pClearScreenEnv[4] = SCE_GS_SET_FRAME_1(pDB->zPointer,
												   (pDB->drawWidth>>6),
													pDB->drawPSM, 0); // write frame
		}
		
		break;

    case SHELLDISP_CLEAR_FRAME: 
		if((pDB->zPSM&0x2)||(pDB->drawPSM&0x2))
			bStrangeHeight = (pDB->drawHeight%128)!=0; // 16-bit pages are 64 pixels high
		else
			bStrangeHeight = (pDB->drawHeight%64)!=0; // 32-bit pages are 32 pixels high

		if (bStrangeHeight)
		{
			stripHeight = pDB->drawHeight;
			pClearScreenEnv[2] = SCE_GS_SET_ZBUF_1(pDB->zPointer, pDB->zPSM, 1); // no z write
			pClearScreenEnv[4] = SCE_GS_SET_FRAME_1(currentFBP,
												   (pDB->drawWidth>>6),
													pDB->drawPSM, 0); // write frame
		}
		else // halve the draw height and use z and frame together for fast clear
		{
			stripHeight = pDB->drawHeight>>1;
			pClearScreenEnv[2] = SCE_GS_SET_ZBUF_1(currentFBP+halfFrameSize, pDB->zPSM, 0); // write z
			pClearScreenEnv[4] = SCE_GS_SET_FRAME_1(currentFBP,
												   (pDB->drawWidth>>6),
													pDB->drawPSM, 0); // write frame
		}
		break;

    case SHELLDISP_CLEAR_Z_AND_FRAME:
		// Clear frame and Z (slowest type of clear)
		stripHeight = pDB->drawHeight;
		pClearSpriteTag0[4] = pDB->clearColour&0xff;       // rgb
		pClearSpriteTag0[5] = (pDB->clearColour>>8)&0xff;  // rgb
		pClearSpriteTag0[6] = (pDB->clearColour>>16)&0xff; // rgb
		pClearSpriteTag0[7] = (pDB->clearColour>>24)&0xff; // rgb

		if((pDB->zPSM&0x2)||(pDB->drawPSM&0x2))
			bStrangeHeight = (pDB->drawHeight%64)!=0; // 16-bit pages are 64 pixels high
		else
			bStrangeHeight = (pDB->drawHeight%32)!=0; // 32-bit pages are 32 pixels high

		if( ((pDB->drawPSM&0x2)==0x2) && ((pDB->zPSM&0x2)==0x2) 
			 && (!bStrangeHeight) && (pDB->clearColour==0))
		{ 
			// if both Z and draw are 16-bit and black use 32-bit mode to clear twice as fast
			stripHeight>>=1; 
			pClearScreenEnv[4] = SCE_GS_SET_FRAME_1(currentFBP, (pDB->drawWidth>>6),
													SCE_GS_PSMCT32, 0);
			pClearScreenEnv[2] = SCE_GS_SET_ZBUF_1(pDB->zPointer,
												   SCE_GS_PSMZ32, 0);
		}
		else
		{
			pClearScreenEnv[4] = SCE_GS_SET_FRAME_1(currentFBP, (pDB->drawWidth>>6),
													pDB->drawPSM, 0);
			pClearScreenEnv[2] = SCE_GS_SET_ZBUF_1(pDB->zPointer, pDB->zPSM, 0);
		}
		break;

    }

	if(clearType!=SHELLDISP_NO_CLEAR)
	{
		pClearScreenEnv[0]  = SCE_GS_SET_TEST_1(0, 0, 0, 0, 0, 0, 1, 1);  // always
		pClearScreenEnv[6]  = SCE_GS_SET_XYOFFSET_1(0, 0);
		pClearScreenEnv[8]  = 0;  // dither off
		pClearScreenEnv[10] = SCE_GS_SET_SCISSOR(0, (pDB->drawWidth-1), 0, (pDB->drawHeight-1));
		
		// Set up sprite0
		*(long*)pClearSpriteTag0 = SCE_GIF_SET_TAG(1, 1, 1, 6, 0,3); 
		
		// Set up loop count (width divided by 64)
		pClearSpriteTag0[3]  = (unsigned int) numStrips;   
		pClearSpriteTag0[8]  = 0x0;              // xyz (set x=left)
		pClearSpriteTag0[9]  = 0x0;              // xyz (set y=top)
		pClearSpriteTag0[12] = 32<<4;           // xyz (set x=right)
		pClearSpriteTag0[13] = (unsigned int) (stripHeight<<4);  // xyz (set y=bottom)
		
		// Set up sprite1
		memcpy(pClearSpriteTag1, pClearSpriteTag0, 64);
		
	}
}

//-----------------------------------------------------------------------------
// _shellDispResetGraph()
// Description: Resets GS into selected mode 
// Parameters:  tv_type       - SCE_GS_PAL or SCE_GS_NTSC
// Notes:       Same as sceGsResetGraph but also sets shellDisp variables
//-----------------------------------------------------------------------------

#define UNCACHE(a) *((unsigned int*)&a)|=0x20000000;

static void _shellDispResetGraph(  EShellDispTVmode tv_type, EShellDispInterlaceMode interlace_mode )
{
	int tvType;
	int oldInterlaceMode;

	oldInterlaceMode = _pShellDispDB->interlaceMode;
	
	switch(tv_type)
	{
	case SHELLDISP_PAL:
		tvType = (short) SCE_GS_PAL;
		break;
	case SHELLDISP_NTSC:
		tvType = (short) SCE_GS_NTSC;
		break;
//	case SHELLDISP_DTV480P:
//		tvType = (short) SCE_GS_DTV480P;
//		interlace_mode=SHELLDISP_NO_INTERLACE;
//		break;
	}

	switch(interlace_mode)
	{
		case SHELLDISP_NO_INTERLACE:
			_pShellDispDB->interlaceMode = (short) SCE_GS_NOINTERLACE;
			_pShellDispDB->fieldMode = (short) SCE_GS_FRAME;
			_pShellDispDB->offsetRender = 0;
			break;
		case SHELLDISP_INTERLACE_FIELD:
		case SHELLDISP_INTERLACE_FIELD_AA:
			_pShellDispDB->interlaceMode = (short) SCE_GS_INTERLACE;		
			_pShellDispDB->fieldMode = (short) SCE_GS_FIELD;
			_pShellDispDB->offsetRender = 0;
			break;
		case SHELLDISP_INTERLACE_FRAME:
			_pShellDispDB->interlaceMode = (short) SCE_GS_INTERLACE;		
			_pShellDispDB->fieldMode = (short) SCE_GS_FRAME;
			_pShellDispDB->offsetRender = 1;
			break;
	}

	if ((_pShellDispDB->tvType!=tvType)||(oldInterlaceMode!=interlace_mode))
	{
		_pShellDispDB->tvType=tvType;
		sceGsResetGraph(0,  (short) _pShellDispDB->interlaceMode, 
							(short) _pShellDispDB->tvType, 
							(short) _pShellDispDB->fieldMode);
	}
}

//-----------------------------------------------------------------------------
// _shellDispSetupDblBuffer()
// Description: Setup of double buffer and antialiasing support
// Parameters:  dispWidth, dispHeight	- width and height of display Buffer
//              dispPSM					- pixel format of display buffer
//				drawWidth, drawHeight	- width and height of display Buffer
//              drawPSM				    - pixel format of draw buffer
//				zPixelMode				- pixel format of Z buffer
// Notes:       Initialises the whole double buffer structure, and works out
//				where all the various buffers need to go.
//-----------------------------------------------------------------------------
static void _shellDispSetupDblBuffer(short dispWidth, short dispHeight, 
									short dispPSM, 
									short drawWidth, short drawHeight,
									short drawPSM,
									short zPixelMode)
{
	ASSERTS(!((drawPSM==SCE_GS_PSMCT16)&&(zPixelMode==SCE_GS_PSMZ16S)),
		   "Incompatible Draw and Z pixel mode");
	ASSERTS(!((drawPSM!=SCE_GS_PSMCT16)&&(zPixelMode==SCE_GS_PSMZ16 )),
		   "Incompatible Draw and Z pixel mode");
 	
	// Remember these settings in case the mode switches
	_pShellDispDB->clearColour = 0;

	_pShellDispDB->dispWidth = dispWidth;
	_pShellDispDB->dispHeight = dispHeight;
	_pShellDispDB->dispPSM = dispPSM;

	_pShellDispDB->drawWidth = drawWidth;
	_pShellDispDB->drawHeight = drawHeight;
	_pShellDispDB->drawPSM = drawPSM;

	_pShellDispDB->zPSM = zPixelMode;

	_pShellDispDB->clearType = SHELLDISP_CLEAR_Z_AND_FRAME;

	if(drawPSM==dispPSM && drawWidth==dispWidth && drawHeight==dispHeight)
	{
		_pShellDispDB->canFlip=1;

	_pShellDispDB->bufferPointer[0] = 0;
	_pShellDispDB->bufferPointer[1]=_GetNumPages(dispWidth,dispHeight,dispPSM);
	_pShellDispDB->zPointer=(short)(_pShellDispDB->bufferPointer[1]+_GetNumPages(drawWidth,drawHeight,drawPSM));

	}
	else
	{		
	
		_pShellDispDB->bufferPointer[1]=
		_pShellDispDB->bufferPointer[0]=_GetNumPages(dispWidth,dispHeight,dispPSM);
		_pShellDispDB->zPointer=(short)(_pShellDispDB->bufferPointer[0]+_GetNumPages(drawWidth,drawHeight,drawPSM));

		if (drawHeight>288)
			_pShellDispDB->offsetRender = 0; //(if copying AND shrinking)
			
		_pShellDispDB->canFlip=0;

}		
/*		_SetDrawEnv(_VUPointers[0].DrawEnv, _pShellDispDB->bufferPointer[0],
					_pShellDispDB->drawPSM, _pShellDispDB->drawWidth, _pShellDispDB->drawHeight,
					_pShellDispDB->zPointer,_pShellDispDB->zPSM);

	
		_SetDrawEnv(_VUPointers[1].DrawEnv, _pShellDispDB->bufferPointer[1],
					_pShellDispDB->drawPSM, _pShellDispDB->drawWidth, _pShellDispDB->drawHeight,
					_pShellDispDB->zPointer,_pShellDispDB->zPSM);
*/
	_pShellDispDB->rebuild=2;

	_SetDispBuffers(_pShellDispDB);
}


int _VBlankHandler(int c, void *ignore1, void *ignore2)
{
	int i;

	(void) ignore1;
	(void) ignore2;

	if(c==INTC_VBLANK_S)
	{
		_VBlank=1;
		for(i=0;i<MAX_VSYNC_CALLBACKS;i++)
			if(_Vcallback[i]) _Vcallback[i](_Vuserdata[i]);
	}
	else
	{
		_VBlank=0;
	}
	ExitHandler();
	return 0;
}


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// shellDispSyncV()
// Description: wait for the next vblank interval.
// Parameters:
// Notes:
//-----------------------------------------------------------------------------

void shellDispSyncV(void)
{
	while(!_VBlank) {}
	_VBlank=0;
}

//-----------------------------------------------------------------------------
// shellDispRemoveVSyncCallback()
// Description: Remove a specified vsync callback routine
// Parameters: 	routine - address of previously added routine for removal
// Notes: 		Will remove all occurancies of a routine, should one have been
//				added more than once for some reason.
//-----------------------------------------------------------------------------

void shellDispRemoveVSyncCallback(void (*routine)(int))
{
int i;

	DI();
	for(i=0;i<MAX_VSYNC_CALLBACKS;i++)
	{
		if(_Vcallback[i]==routine)
		{
			_Vcallback[i]=0;
			_Vuserdata[i]=0;
		}
	}
	EI();
}

//-----------------------------------------------------------------------------
// shellDispAddVSyncCallback()
// Description: Add a callback to the vsync handler
// Parameters: 	routine - address of routine to add
//				data - data to be passed to the routine when called
// Notes: 		There is a limit to the number of callback routines, this
//				function asserts (in debug mode) when this is reached.
//				The function will not be added if the list is already full.
//-----------------------------------------------------------------------------

void shellDispAddVSyncCallback(void (*routine)(int),int data)
{
int i;

	DI();
	for(i=0;i<MAX_VSYNC_CALLBACKS;i++)
	{
		if(_Vcallback[i]==0)
		{
			_Vcallback[i]=routine;
			_Vuserdata[i]=data;
			break;
		}
	}
	EI();

	ASSERTS(i<MAX_VSYNC_CALLBACKS,"Too many VSync callbacks");
}

//-----------------------------------------------------------------------------
// shellDispInit()
// Description: Resets GS and sets up double buffers
//              Setup of double buffer and antialiasing support
// Parameters:  interlaceMode	- SHELLDISP_INTERLACE or SHELL_DISP_NOINTERLACE
//              tvType			- SHELLDISP_PAL or SHELLDISP_NTSC
//              dispWidth		- width of display buffer
//				dispHeight		- height of display Buffer
//              dispPSM			- pixel format of display buffer
//              drawWidth		- width of draw buffer
//				drawHeight		- height of draw Buffer
//              drawPSM			- pixel format of draw buffer
//				zPixelMode		- pixel format of Z buffer
// Notes:    1) GS reset like sceGsResetGraph but also sets shellDisp vars.
//           2) Sets up a double buffer with support for anti-aliasing.
//				Antialiasing for field mode can be specified in the
//				interlaceMode parameter. Otherwise specifying a different size
//				for draw and display buffers implies a bilinear stretch which
//				can also be used to AA the display. These can even be mixed,
//				vram allowing.
//-----------------------------------------------------------------------------

void shellDispInit( EShellDispInterlaceMode interlaceMode,
				   EShellDispTVmode tvType,
				   short dispWidth, short dispHeight, short dispPSM,
				   short drawWidth, short drawHeight, short drawPSM,
				   short zPixelMode
				   )
{
static int once;

	_waitDisplay = 1;

	if(!once)
	{
		_pShellDispDB = (ShellDblBuff*)(((u_int)&_shellDispDB)|0x20000000);
		_pShellDispDB->tvType=~0;
		_pShellDispDB->interlaceMode=~0;

		// Moved these from _shellDispSetupDblBuffer to set these only once
		_pShellDispDB->dispDX = 0;
		_pShellDispDB->dispDY = 0;
		_pShellDispDB->currentBuffer = 0;

		ASSERTS(GET_ALIGNMENT(_pShellDispDB)>=64, "shellDblBuff not 64-byte aligned");
	}

	if(interlaceMode==SHELLDISP_INTERLACE_FIELD_AA)
	{
		_pShellDispDB->twoCircuit=1;
	} else {
		_pShellDispDB->twoCircuit=0;	
	}

	_shellDispResetGraph(tvType, interlaceMode);

	if(!once)
	{
		int i;
		unsigned int *pt;

		pt=(unsigned int *)&_VUPointers[0];
		for(i=0;i<sizeof(_VUPointers)/4;i++)
		{
			*pt++|=0x20000000;
		}
	
		sceGsResetPath();
	
		ASSERTS(vbshandler == -1, "cant init twice");
		ASSERTS(vbehandler == -1, "cant init twice");
		vbshandler = shellCoreAddIntcHandler(INTC_VBLANK_S, _VBlankHandler, 0, 0);
		vbehandler = shellCoreAddIntcHandler(INTC_VBLANK_E, _VBlankHandler, 0, 0);
		once=1;
		_test_1=SCE_GS_SET_TEST(0,0,0,0,0,0,1,2);
	}
	
	_shellDispSetupDblBuffer(dispWidth, dispHeight, 
							dispPSM, 
							drawWidth, drawHeight,
							drawPSM,
							zPixelMode);
}

void shellDispCleanup( void )
{
	ASSERTS(vbshandler != -1, "cant cleanup if we werent inited");
	ASSERTS(vbehandler != -1, "cant cleanup if we werent inited");
	shellCoreRemoveIntcHandler(INTC_VBLANK_S, vbshandler);
	shellCoreRemoveIntcHandler(INTC_VBLANK_E, vbehandler);
	vbshandler = vbehandler = -1;
}

//-----------------------------------------------------------------------------
// shellDispGetCurrentDrawEnv()
// Description: Returns current drawing environment
// Parameters:  None
//-----------------------------------------------------------------------------
ShellDispDrawEnv *shellDispGetDrawEnv( void )
{
	return _VUPointers[_pShellDispDB->currentBuffer].DrawEnv; 
}

//-----------------------------------------------------------------------------
// shellDispSwapDblBuffer
// Description: Swaps buffers, performs downsampling, clears buffers
// Notes:       Performs buffer swap, buffer shrinking for anti-flicker for
//              frame mode, and buffer clearing. Fills in the appropriate
//				half-pixel offset in the drawing environment, and attaches
//				to the front of the existing drawing list, before kicking this
//				to the VIF.
//-----------------------------------------------------------------------------
void shellDispSwapDblBuffer( void )
{
	int oddField;
	int offset;
	
	EShellDispClear clearType = (EShellDispClear)_pShellDispDB->clearType;
	
	_shellDispFrame++;
	oddField = !!(int)((DGET_GS_CSR()>>13)&0x1);

	_pShellDispDB->currentBuffer^=1;

	if(_waitDisplay)
	{
		// Don't allow the new uninitialised back buffer to be displayed on
		// the first buffer swap after reconfiguring VRAM.
		_waitDisplay = 0;
		DPUT_GS_PMODE(4);
	}
	else
	{
		DPUT_GS_PMODE(    *(u_long *) &_pShellDispDB->pmode);   
		DPUT_GS_SMODE2(   *(u_long *) &_pShellDispDB->smode2);
		DPUT_GS_DISPLAY1( *(u_long *) &_pShellDispDB->display1);	
		DPUT_GS_DISPLAY2( *(u_long *) &_pShellDispDB->display2);
		DPUT_GS_DISPFB1(  *(u_long *) &_pShellDispDB->dispfb1[_pShellDispDB->currentBuffer]);
		DPUT_GS_DISPFB2(  *(u_long *) &_pShellDispDB->dispfb2[_pShellDispDB->currentBuffer]);
	}
	
	*(u_long *)&_VUPointers[_pShellDispDB->currentBuffer].DrawEnv->test1 = _test_1;

	if(_pShellDispDB->rebuild)
	{
		if(!_pShellDispDB->currentBuffer)
		{
			_SetDrawEnv(_VUPointers[0].DrawEnv, _pShellDispDB->bufferPointer[1],
						_pShellDispDB->drawPSM, _pShellDispDB->drawWidth, _pShellDispDB->drawHeight,
						_pShellDispDB->zPointer,_pShellDispDB->zPSM);
			_CopyDrawEnvToContext2(_VUPointers[0].DrawEnv);
		}
		else
		{
			_SetDrawEnv(_VUPointers[1].DrawEnv, _pShellDispDB->bufferPointer[0],
						_pShellDispDB->drawPSM, _pShellDispDB->drawWidth, _pShellDispDB->drawHeight,
						_pShellDispDB->zPointer,_pShellDispDB->zPSM);
			_CopyDrawEnvToContext2(_VUPointers[1].DrawEnv);
		}
		_SetupShrinkSprites(_pShellDispDB);
		_SetupClearBuffer(_pShellDispDB, clearType);

		_pShellDispDB->rebuild--;
	}

	if(_pShellDispDB->offsetRender)
	{
		*(u_long *)&_VUPointers[_pShellDispDB->currentBuffer].DrawEnv->xyoffset1 =
						SCE_GS_SET_XYOFFSET_1((2048-(_pShellDispDB->drawWidth>>1))<<4,
								((!oddField)<<3)+((2048-(_pShellDispDB->drawHeight>>1))<<4));	
		*(u_long *)&_VUPointers[_pShellDispDB->currentBuffer].DrawEnv->xyoffset2 =
			*(u_long *)&_VUPointers[_pShellDispDB->currentBuffer].DrawEnv->xyoffset1;
	} 
	else 
	{
		*(u_long *)&_VUPointers[_pShellDispDB->currentBuffer].DrawEnv->xyoffset1 =
						SCE_GS_SET_XYOFFSET_1((2048-(_pShellDispDB->drawWidth>>1))<<4,
							((2048-(_pShellDispDB->drawHeight>>1))<<4));
		*(u_long *)&_VUPointers[_pShellDispDB->currentBuffer].DrawEnv->xyoffset2 =
			*(u_long *)&_VUPointers[_pShellDispDB->currentBuffer].DrawEnv->xyoffset1;
	}
	// If shrink or different draw and clear, do both copy and clear together
	if(!_pShellDispDB->canFlip)
	{      
		u_int *pShrinkSpriteTag0 = (u_int*) _VUPointers[_pShellDispDB->currentBuffer].ShrinkSprite0;
		u_int *pShrinkSpriteTag1 = (u_int*) _VUPointers[_pShellDispDB->currentBuffer].ShrinkSprite1;
		u_long *pShrinkScreenEnvironment = (u_long*) _VUPointers[_pShellDispDB->currentBuffer].ShrinkEnv;

		if(_pShellDispDB->drawHeight == _pShellDispDB->dispHeight) offset=0;
		else
		{
			offset=3;
			if(_pShellDispDB->interlaceMode!=SCE_GS_NOINTERLACE)
				if(oddField==0) offset=12;
		}

		pShrinkSpriteTag0[13]  = *(unsigned int*) &offset;              // xyz (set y=top)
		pShrinkSpriteTag0[21] = (unsigned int) (offset+(_pShellDispDB->dispHeight<<4));  // uv (set y=bottom)
		
		pShrinkSpriteTag1[13]  = *(unsigned int*) &offset;              // xy (set v=top)
		pShrinkSpriteTag1[21] = (unsigned int) (offset+(_pShellDispDB->dispHeight<<4));  // xy (set y=bottom)
		
		pShrinkScreenEnvironment[12] =
			SCE_GS_SET_TEX1_1(1, 0, 1, 1, 1, 0, 1); 

		if(clearType!=0)
		{
				shellDmaPrependVif1List(	_VUPointers[_pShellDispDB->currentBuffer].ShrinkAndClearStart,
										_VUPointers[_pShellDispDB->currentBuffer].ShrinkAndClearEnd);
		}
		else
		{
				shellDmaPrependVif1List(	_VUPointers[_pShellDispDB->currentBuffer].ShrinkStart,
										_VUPointers[_pShellDispDB->currentBuffer].ShrinkEnd);
		}
	}
	else
	{
		if(clearType!=0)
		{
				shellDmaPrependVif1List(	_VUPointers[_pShellDispDB->currentBuffer].ClearStart,
										_VUPointers[_pShellDispDB->currentBuffer].ClearEnd);
		} else {
				shellDmaPrependVif1List(	_VUPointers[_pShellDispDB->currentBuffer].EnvOnlyStart,
										_VUPointers[_pShellDispDB->currentBuffer].EnvOnlyEnd);
		}
	}
	
	shellDmaKickVif1List();
}

//-----------------------------------------------------------------------------
// shellDispSetClearColour()
// Description: Sets colour for clear screen on buffer swap
// Parameters:  r,g,b,a - colour
//-----------------------------------------------------------------------------
void shellDispSetClearColour( u_char r, u_char g, u_char b, u_char a )
{
	_pShellDispDB->clearColour = (unsigned int)r | (((unsigned int)g)<<8) | (((unsigned int)b)<<16) | (((unsigned int)a)<<24);
	_pShellDispDB->rebuild=2;
}
 
//-----------------------------------------------------------------------------
// shellDispSetClearType()
// Description: Set up clear to be done with double buffer swap
// Parameters:  EShellDispClear clearType (no_clear, frame, z, z_and_frame)
//-----------------------------------------------------------------------------
void shellDispSetClearType( EShellDispClear clearType )
{
	_pShellDispDB->clearType = clearType;
	_pShellDispDB->rebuild=2;
}

//-----------------------------------------------------------------------------
// shellDispSetTest()
// Description: Sets pixel test mode (GS test1 register)
// Parameters:  u_long test1
//-----------------------------------------------------------------------------
void shellDispSetTest( u_long test_1 )
{
	_test_1 = test_1;
	_pShellDispDB->rebuild=2;
}

//-----------------------------------------------------------------------------
// shellDispGetDrawFBP()
// Description: Get the Vram address of the current draw buffer
// Parameters:  None
// Returns:     Vram address of current draw buffer
//-----------------------------------------------------------------------------
int shellDispGetDrawFBP( void )
{
	return _pShellDispDB->bufferPointer[_pShellDispDB->currentBuffer];
}

int shellDispGetDispFBP(void)
{
	return _pShellDispDB->dispfb1[_pShellDispDB->currentBuffer].FBP;
}

//-----------------------------------------------------------------------------
// shellDispGetZ_FBP()
// Description: Gets address of Z buffer in vram
// Parameters:  None
// Returns:     Z buffer address
//-----------------------------------------------------------------------------
int shellDispGetZ_FBP( void )
{
	return _pShellDispDB->zPointer;
}

//-----------------------------------------------------------------------------
// shellDispGetFirstFreePage()
// Description: Get the first available free address in vram, after the buffers
// Parameters:  None
// Returns:     Start of free VRAM
//-----------------------------------------------------------------------------
int shellDispGetFirstFreePage( void )
{
	return _pShellDispDB->zPointer + _GetNumPages(_pShellDispDB->drawWidth, _pShellDispDB->drawHeight, _pShellDispDB->zPSM);
}

//-----------------------------------------------------------------------------
// shellDispSetTVoffset()
// Description: Sets top left xy offset of display on TV screen
// Parameters:  dx  - x coord
//              dy  - y coord
// Notes:      Start with 0 0 and adjust from there
//-----------------------------------------------------------------------------
void shellDispSetTVoffset( int dx, int dy )
{
	_pShellDispDB->dispDX = (short) dx;
	_pShellDispDB->dispDY = (short) dy;
	_SetDispBuffers(_pShellDispDB); 
}

//-----------------------------------------------------------------------------
// shellDispGetTVoffset()
// Description: Returns top left xy offset of display on TV screen
// Parameters:  pDX - pointer to int holding x coord
//              pDY - pointer to int holding y coord
//-----------------------------------------------------------------------------
void shellDispGetTVoffset( int *pDX, int *pDY )
{
	*pDX=_pShellDispDB->dispDX;
	*pDY=_pShellDispDB->dispDY;
}
