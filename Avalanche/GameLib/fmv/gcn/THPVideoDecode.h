/*---------------------------------------------------------------------------*
  Project:  Dolphin
  File:     THPVideoDecode.h

  Copyright 2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/GameLib/fmv/gcn/THPVideoDecode.h $
 * 
 * 1     4/03/02 10:14a Todd Blackburn
    
    1     02/01/16 11:04a Akagi
    Initial revision made by Suzuki-san (IRD).
  $NoKeywords: $  

 *---------------------------------------------------------------------------*/

#ifndef __THP_VIDEO_DECODE_H__
#define __THP_VIDEO_DECODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <dolphin.h>
#include "THPPlayer.h"

extern BOOL           CreateVideoDecodeThread(OSPriority priority, u8 *ptr);
extern void           VideoDecodeThreadStart(void);
extern void           VideoDecodeThreadCancel(void);
extern THPTextureSet  *PopFreeTextureSet();
extern void           PushFreeTextureSet(THPTextureSet *buffer);
extern THPTextureSet  *PopDecodedTextureSet(s32 flag);
extern void           PushDecodedTextureSet(THPTextureSet *buffer);

#ifdef __cplusplus
}
#endif

#endif // __THP_VIDEO_DECODE_H__
