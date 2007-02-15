/*---------------------------------------------------------------------------*
  Project:  Dolphin
  File:     THPRead.h

  Copyright 2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/GameLib/fmv/gcn/THPRead.h $
 * 
 * 1     4/03/02 10:14a Todd Blackburn
    
    1     02/01/16 11:03a Akagi
    Initial revision made by Suzuki-san (IRD).
  $NoKeywords: $  

 *---------------------------------------------------------------------------*/

#ifndef __THP_READ_H__
#define __THP_READ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <dolphin.h>
#include "THPPlayer.h"

#define NEXT_READ_SIZE(p)      *(s32 *)(p->ptr)
#define PREV_READ_SIZE(p)      *(s32 *)(p->ptr + 4)

extern BOOL          CreateReadThread(OSPriority priority);
extern void          ReadThreadStart(void);
extern void          ReadThreadCancel(void);
extern THPReadBuffer *PopReadedBuffer();
extern void          PushReadedBuffer(THPReadBuffer *buffer);
extern THPReadBuffer *PopFreeReadBuffer();
extern void          PushFreeReadBuffer(THPReadBuffer *buffer);
extern THPReadBuffer *PopReadedBuffer2();
extern void          PushReadedBuffer2(THPReadBuffer *buffer);

#ifdef __cplusplus
}
#endif

#endif // __THP_READ_H__
