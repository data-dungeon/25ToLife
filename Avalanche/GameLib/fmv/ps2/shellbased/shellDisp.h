//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellDisp header file
//
// $Workfile: shellDisp.h $
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

#ifndef _SHELLDISP_H
#define _SHELLDISP_H

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef enum 
{
	SHELLDISP_NO_CLEAR          = 0, 	// Do not clear frame or Z buffers
	SHELLDISP_CLEAR_Z           = 1,		// Clear Z buffer only
	SHELLDISP_CLEAR_FRAME       = 2,		// Clear frame buffer only
	SHELLDISP_CLEAR_Z_AND_FRAME = 3  	// Clear both frame and Z buffers
} EShellDispClear;						// Options for clearing the frame and z buffers

typedef enum
{
	SHELLDISP_NO_INTERLACE	 = 0,		// Display is not interlaced
	SHELLDISP_INTERLACE_FRAME = 1,		// Display is interlaced frame mode (every line displayed each field)
	SHELLDISP_INTERLACE_FIELD = 2,		// Display is interlaced field mode (alternate lines display on each field)
	SHELLDISP_INTERLACE_FIELD_AA = 3		// Display is interlaced field mode, with antialiasing (alternate lines are blended to reduce flicker)
} EShellDispInterlaceMode;				// Options for display buffer type

typedef enum
{
	SHELLDISP_PAL	 = 2,				// PAL video mode
	SHELLDISP_NTSC	 = 3,				// NTSC video mode
	SHELLDISP_DTV480P = 4
} EShellDispTVmode;						// Options for display mode

typedef struct _ShellDispDrawEnv
{
	sceGifTag		gifTag;				// GIFTag for uploading the following settings
	sceGsFrame		frame1;				// FRAME_1 settings (frame buffer setup)
	u_long			frame1addr;			// FRAME_1 gs address
	sceGsZbuf		zbuf1;				// ZBUF_1 settings (z buffer setup)
	u_long			zbuf1addr;			// ZBUF_1 gs address
	sceGsXyoffset	xyoffset1;			// XYOFFSET_1 settings (x and y offset function)
	u_long			xyoffset1addr;		// XYOFFSET_1 gs address
	sceGsScissor	scissor1;			// SCISSOR_1 settings (scissoring rectangle)
	u_long			scissor1addr;		// SCISSER_1 gs address
	sceGsPrmodecont	prmodecont;			// PRMODECONT setting
	u_long			prmodecontaddr;		// PRMODECONT gs address
	sceGsColclamp	colclamp;			// COLCLAMP setting (colour clamping)
	u_long			colclampaddr;		// COLCLAMP gs address
	sceGsDthe		dthe;				// DTHE setting (dither enable)
	u_long			dtheaddr; 			// DTHE gs address
	sceGsTest		test1;				// TEST_1 setting (alpha and z testing options)
	u_long			test1addr;			// TEST_1 gs address
	sceGsDimx       dimx;				// DIMX setting (dither matrix setup)
	u_long			dimxaddr;			// DIMX gs address
// Context 2 registers
	sceGsFrame		frame2;				// FRAME_2 settings (frame buffer setup)
	u_long			frame2addr;			// FRAME_2 gs address
	sceGsZbuf		zbuf2;				// ZBUF_2 settings (z buffer setup)
	u_long			zbuf2addr;			// ZBUF_2 gs address
	sceGsXyoffset	xyoffset2;			// XYOFFSET_2 settings (x and y offset function)
	u_long			xyoffset2addr;		// XYOFFSET_2 gs address
	sceGsScissor	scissor2;			// SCISSOR_2 settings (scissoring rectangle)
	u_long			scissor2addr;		// SCISSER_2 gs address
	sceGsTest		test2;				// TEST_2 setting (alpha and z testing options)
	u_long			test2addr;			// TEST_2 gs address
} ShellDispDrawEnv __attribute__((aligned(16)));	// GS settings for the drawing environment


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

void shellDispInit( EShellDispInterlaceMode interlaceMode,
				   EShellDispTVmode tv_type,
				   short dispwidth, short dispheight, short displayPixelMode,
				   short drawwidth, short drawheight, short drawPixelMode,
				   short zPixelMode
				   );

void shellDispCleanup();
						 
void shellDispSetClearColour( u_char r, u_char g, u_char b,
							 u_char a );               // default: 0,0,0,0
void shellDispSetClearType( EShellDispClear clearType ); // default: Z_AND_FRAME
void shellDispSetTest( u_long test_1);                  // default: ZGEQUAL
void shellDispSetTVoffset( int dx, int dy );            // default: 0, 0
void shellDispGetTVoffset( int *pDX, int *pDY );
void shellDispSyncV(void);

void shellDispSwapDblBuffer( void );

ShellDispDrawEnv *shellDispGetDrawEnv( void );
int shellDispGetDrawFBP( void );
int shellDispGetDispFBP(void);
int shellDispGetZ_FBP( void );
int shellDispGetFirstFreePage( void );

void shellDispAddVSyncCallback(void (*routine)(int),int data);
void shellDispRemoveVSyncCallback(void (*routine)(int));

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif _SHELLDISP_H
