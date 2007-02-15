/*---------------------------------------------------------------------------*
  Project:  Dolphin GD library
  File:     GD.c

  Copyright 2001 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/tools/TupperFilters/ExportGCN/GDBase.cpp $
 * 
 * 2     6/20/03 4:19p Todd Blackburn
 * 
 * 1     6/20/03 11:03a Todd Blackburn
    
    2     9/12/01 3:56p Carl
    Renamed some functions to make it clear which act on "current".
    
    1     9/12/01 1:52p Carl
    Initial revision of GD: Graphics Display List Library.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <dolphin/gd.h>
#include <dolphin/os.h>

/*---------------------------------------------------------------------------*/

// This object is global, but the user should not access it directly:
GDLObj *__GDCurrentDL = NULL;

// Overflow callback pointer
static GDOverflowCallback overflowcb = NULL;

/*---------------------------------------------------------------------------*/

// Initialize parameters
void GDInitGDLObj(GDLObj *dl, void *start, u32 length)
{
    ASSERTMSG( ((u32) start & 31) == 0, "start must be aligned to 32 bytes");
    ASSERTMSG( (length & 31) == 0, "length must be aligned to 32 bytes");

    dl->start  = (u8*) start;
    dl->ptr    = (u8*) start;
    dl->top    = (u8*) start + length;
    dl->length = length;
}
    
// Makes sure current DL is flushed out of cache (on HW)
void GDFlushCurrToMem()
{
#ifdef EPPC
    DCFlushRange(__GDCurrentDL->start, __GDCurrentDL->length);
#endif
}

// Pads current DL out to 32B
void GDPadCurr32()
{
    u32 n = ((u32) __GDCurrentDL->ptr & 31);
    if (n) 
    {
        for(; n<32; n++) 
        {
            __GDWrite(0);
        }
    }
}

// Overflow-related functions
void GDOverflowed()
{
    if (overflowcb)
    {
        (*overflowcb)();
    } else {
        ASSERTMSG(0, "GDWrite: display list overflowed");
    }
}

void GDSetOverflowCallback(GDOverflowCallback callback)
{
    overflowcb = callback;
}

GDOverflowCallback GDGetOverflowCallback( void )
{
    return overflowcb;
}
