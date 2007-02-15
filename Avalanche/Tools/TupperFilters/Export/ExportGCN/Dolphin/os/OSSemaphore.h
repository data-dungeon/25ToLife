/*---------------------------------------------------------------------------*
  Project:  Dolphin OS Semaphore API
  File:     OSSemaphore.h

  Copyright 2001 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/tools/TupperFilters/ExportGCN/Dolphin/os/OSSemaphore.h $
 * 
 * 1     6/20/03 4:21p Todd Blackburn
    
    1     10/22/01 15:56 Shiki
    Initial check-in.
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __OSSEMAPHORE_H__
#define __OSSEMAPHORE_H__

#include <dolphin/os/OSThread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSSemaphore
{
    s32           count;
    OSThreadQueue queue;
} OSSemaphore;

void OSInitSemaphore    ( OSSemaphore* sem, s32 count );
s32  OSWaitSemaphore    ( OSSemaphore* sem );
s32  OSTryWaitSemaphore ( OSSemaphore* sem );
s32  OSSignalSemaphore  ( OSSemaphore* sem );
s32  OSGetSemaphoreCount( OSSemaphore* sem );


#ifdef __cplusplus
}
#endif

#endif  // __OSSEMAPHORE_H__
