/*---------------------------------------------------------------------------*
  Project:  Dolphin
  File:     THPAudioDecode.h

  Copyright 2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/GameLib/fmv/gcn/THPAudioDecode.h $
 * 
 * 1     4/03/02 10:14a Todd Blackburn
    
    1     02/01/16 11:03a Akagi
    Initial revision made by Suzuki-san (IRD).
  $NoKeywords: $  

 *---------------------------------------------------------------------------*/

#ifndef __THP_AUDIO_DECODE_H__
#define __THP_AUDIO_DECODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <dolphin.h>
#include "THPPlayer.h"

extern BOOL           CreateAudioDecodeThread(OSPriority priority, u8 *ptr);
extern void           AudioDecodeThreadStart(void);
extern void           AudioDecodeThreadCancel(void);
extern THPAudioBuffer *PopFreeAudioBuffer();
extern void           PushFreeAudioBuffer(THPAudioBuffer *buffer);
extern THPAudioBuffer *PopDecodedAudioBuffer(s32 flag);
extern void           PushDecodedAudioBuffer(THPAudioBuffer *buffer);

#ifdef __cplusplus
}
#endif

#endif // __THP_AUDIO_DECODE_H__
