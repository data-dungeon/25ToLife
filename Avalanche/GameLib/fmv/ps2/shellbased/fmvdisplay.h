//-----------------------------------------------------------------------------
//                               FMV Player
//
// $Workfile: fmvdisplay.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef enum
{
	FMV_PAL,
	FMV_NTSC,
	FMV_PAL_FROM_NTSC,
	FMV_480P

} EFMVDisplayMode;

typedef enum
{
	FMV_WS_NONE,
	FMV_WS_H_CROP,
	FMV_WS_H_LETTERBOX,
	FMV_WS_V_CROP,
	FMV_WS_V_LETTERBOX

} EFMVDisplayWidescreen;


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif


void FMVDisplayInit( EFMVDisplayMode displayMode, int upperFieldFirst,
					 u_int *pTbw, u_int *pTbp );
void FMVDisplayCleanup();
void FMVDisplayCreate( u_int vidW, u_int vidH, EFMVDisplayWidescreen wide );
void FMVDisplayAddFrame( ShellDmaTag *uploadTags );
void FMVDisplayStart( void );
int FMVDisplayFlush( void );
void FMVDisplayDestroy( void );

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif
