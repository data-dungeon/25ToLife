/*---------------------------------------------------------------------------*
  Project: OS - memory protection API
  File:    OSMemory.h

  Copyright 2001 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/tools/TupperFilters/ExportGCN/Dolphin/os/OSMemory.h $
 * 
 * 1     6/20/03 4:21p Todd Blackburn
    
    1     8/15/01 17:56 Shiki
    Initial check-in.
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __OSMEMORY_H__
#define __OSMEMORY_H__

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_PROTECT_CHAN0            0
#define OS_PROTECT_CHAN1            1
#define OS_PROTECT_CHAN2            2
#define OS_PROTECT_CHAN3            3

// Capability bits
#define OS_PROTECT_CONTROL_NONE     0x00
#define OS_PROTECT_CONTROL_READ     0x01    // OK to read  [addr, addr + nBytes)
#define OS_PROTECT_CONTROL_WRITE    0x02    // OK to write [addr, addr + nBytes)
#define OS_PROTECT_CONTROL_RDWR     (OS_PROTECT_CONTROL_READ | OS_PROTECT_CONTROL_WRITE)

// dsisr bits for memory protection error handler, which tells
// from which region the error was reported
#define OS_PROTECT0_BIT             0x00000001  // by OS_PROTECT_CHAN0 range
#define OS_PROTECT1_BIT             0x00000002  // by OS_PROTECT_CHAN1 range
#define OS_PROTECT2_BIT             0x00000004  // by OS_PROTECT_CHAN2 range
#define OS_PROTECT3_BIT             0x00000008  // by OS_PROTECT_CHAN3 range
#define OS_PROTECT_ADDRERR_BIT      0x00000010  // by [24M or 48M, 64M)

void OSProtectRange( u32 chan, void* addr, u32 nBytes, u32 control );

#ifdef __cplusplus
}
#endif

#endif  // __OSMEMORY_H__
