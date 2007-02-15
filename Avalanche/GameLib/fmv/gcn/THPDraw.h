/*---------------------------------------------------------------------------*
  Project:  Dolphin
  File:     THPDraw.h

  Copyright 2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/GameLib/fmv/gcn/THPDraw.h $
 * 
 * 1     4/03/02 10:14a Todd Blackburn
    
    1     02/01/16 11:03a Akagi
    Initial revision made by Suzuki-san (IRD).
  $NoKeywords: $  

 *---------------------------------------------------------------------------*/

#ifndef __THP_DRAW_H__
#define __THP_DRAW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <dolphin.h>

extern void THPGXRestore( void );
extern void THPGXYuv2RgbDraw( u8* y_data, 
                              u8* u_data, 
                              u8* v_data, 
                              s16 x, 
                              s16 y, 
                              s16 textureWidth, 
                              s16 textureHeight, 
                              s16 polygonWidth, 
                              s16 polygonHeight );
extern void THPGXYuv2RgbSetup( GXRenderModeObj *rmode );

#ifdef __cplusplus
}
#endif

#endif // __THP_DRAW_H__
