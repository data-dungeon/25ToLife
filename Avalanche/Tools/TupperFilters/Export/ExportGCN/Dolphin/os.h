/*---------------------------------------------------------------------------*
  Project:  Dolphin OS
  File:     os.h

  Copyright 1998-2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/tools/TupperFilters/ExportGCN/Dolphin/os.h $
 * 
 * 2     6/25/03 8:32a Todd Blackburn
 * 
 * 1     6/20/03 4:19p Todd Blackburn
    
    60    3/07/03 17:15 Shiki
    9/5/2002 SDK Patch 3
    
    59    1/23/03 9:38 Shiki
    Defined OS_CONSOLE_TDEV, etc.

    58    8/29/02 12:49 Shiki
    Revised __VA_ARGS__ availability test code for NDEBUG, SNSYS.

    57    8/29/02 9:49 Shiki
    Revised __VA_ARGS__ availability test code for SNSYS.

    56    8/26/02 13:32 Shiki
    Removed old WIN emulator specific OS_TIMER_CLOCK definition.
    Fixed __VA_ARGS__ availability test code.

    55    8/22/02 11:57 Shiki
    Clean up.

    54    8/22/02 10:48 Shiki
    Put #ifndef around ASSERTMSGn() macros.
    Refined ASSERTMSG() using the C9x extension for MW 1.3.x users
    Added another const keyword to the OSPanic() function prototype.

    53    8/20/02 21:33 Shiki
    Added OSFatal().

    52    8/12/02 15:16 Shiki
    Added _padding field to OSStopwatch{}.

    51    8/09/02 8:54 Shiki
    Added const keywords to the OSReport() function prototype.

    50    8/07/02 21:41 Shiki
    Added OSRegisterVersion().

    49    8/05/02 18:04 Shiki
    Added const keywords to relevant function prototypes.

    48    11/27/01 21:12 Shiki
    Included <dolphin/os/OSModule.h>.

    47    11/12/01 12:58 Shiki
    Added OS[Get/Set]ProgressiveMode().

    46    10/29/01 11:08 Shiki
    Included <dolphin/os/OSSemaphore.h>.

    45    10/25/01 13:58 Shiki
    Fixed OS_LANG_*.

    44    10/18/01 13:50 Shiki
    Included <dolphin/os/OSExpansion.h>.

    43    9/03/01 14:43 Shiki
    Included <dolphin/os/OSSerial.h>

    42    7/11/01 10:20p Hashida
    Added OSReset.h

    41    01/07/05 21:06 Shiki
    Added OSGetProgressiveMode()/OSSetProgressiveMode().

    40    6/11/01 7:53p Tian
    Changed to use AT_ADDRESS macro

    39    5/17/01 1:26p Shiki
    Updated OS_CORE_CLOCK, OS_BUS_CLOCK.

    38    5/09/01 1:42p Carl
    Conditionalized #define ASSERT.

    37    01/04/27 17:45 Shiki
    Added OS_LANG_NEUTRAL.

    36    01/04/27 17:05 Shiki
    Removed duplicated OS*etSoundMode() declarations.

    35    01/04/27 16:43 Shiki
    Fixed OS_LANG_DUTCH value.

    34    01/04/27 16:25 Shiki
    Added OS_LANG_*.

    33    01/04/19 13:30 Shiki
    Included <dolphin/os/OSFont.h>.

    32    01/02/15 10:55 Shiki
    Added msec and usec to OSCalendarTime.

    31    01/02/13 20:06 Shiki
    Added sound mode and calendar functions.

    30    11/09/00 7:40p Tian
    Added calibrated clock values to HW2

    29    9/11/00 2:59p Shiki
    Added OS_CONSOLE_RETAIL2.

    28    9/07/00 7:54p Shiki
    Fixed OS_*_CLOCK defines and related macros for handling final clock
    setup.

    27    9/07/00 2:57p Shiki
    Removed OSSetTick() and OSSetTime().

    26    9/05/00 5:18p Shiki
    Added OS_CONSOLE_DEVHW2.

    25    8/16/00 6:20p Shiki
    Added OSDiffTick().

    24    7/14/00 3:48p Shiki
    Added OSGetRTC()/OSSetRTC().

    23    7/10/00 6:40p Dante
    Bug Fix (on the PC OSGetTick returns milliseconds)

    22    6/15/00 4:47a Shiki
    Rollback MINNOW clock speed defines for backward boot ROM
    compatibility.

    21    6/15/00 4:41a Shiki
    Revised real-time clock API for HW1.

    20    6/14/00 4:53p Shiki
    Fixed HW1_DRIP clock settings.

    19    5/18/00 9:28a Shiki
    Added OSAllocFromArenaLo() and OSAllocFromArenaHi().

    18    5/14/00 1:38p Shiki
    Fixed DRIP clock settings.

    17    5/10/00 4:24p Shiki
    Added clock configuration for DRIP.

    16    3/29/00 3:05p Hashida
    Added memory size APIs.

    15    3/27/00 2:02p Tian
    Added OSFastCast.h

    14    3/16/00 7:04p Shiki
    Removed timer API. Included <os/OSAlarm.h>.

    13    3/14/00 2:44p Shiki
    Added ASSERTMSGn() macros.

    12    3/01/00 3:29p Tian
    Cleaned up clock rate settings.  They were wrong for GEKKO.

    11    2/15/00 6:03p Shiki
    Revised OS_CONSOLE_* define names.

    10    2/01/00 1:50p Shiki
    Added OS_CONSOLE_DEVELOPMENT1 define name.

    9     1/18/00 6:22p Shiki
      Included <dolphin/os/OSMutex.h>.

    8     1/18/00 5:46p Tian
    MINNOW_MARLIN -> MINNOW

    7     1/18/00 3:29p Shiki
    Included <dolphin/os/OSMessage.h>.

    6     1/18/00 1:57p Tian
    Changed the address translation macros for MINNOW_MARLIN to enable FIFO
    emulation.  Because ORs/ANDs are used, we sacrifice some safety.

    5     1/18/00 10:43a Shiki
    Implemented basic thread API.

    4     1/10/00 2:43p Hashida
    Changed so that OSTicksToMicroseconds and OSMicrosecondsToTicks macros
    works with 33MHz busclock systems.

    3     12/16/99 2:52p Tian
    New core/bus speeds for Gekko + Spruce

    2     12/15/99 3:08p Shiki
    Added OS_*_CLOCK defines for MINNOW.

    17    10/07/99 4:13p Shiki
    Added Idle Function API section.

    16    8/23/99 7:18p Shiki
    Added OS_BASE_CACHED and  OS_BASE_UNCACHED.

    15    8/02/99 2:20p Shiki
    Included OSInterrupt.h

    14    99/07/27 4:51p Shiki
    Added function versions of address translation APIs.

    13    99/07/27 3:43p Shiki
    Revised OSTicksToNanoseconds() and  OSNanosecondsToTicks().

    12    99/07/27 3:16p Shiki
    Fixed time-cycle conversion macros.

    11    99/07/12 9:37p Shiki
    Included OSError.h

    10    99/07/12 3:24p Shiki
    Cleanup. Removed EPPC specific OSReport() and OSPanic() declarations.

    9     6/04/99 3:02p Tianli01
    Cleanup.  Added rounding macros, OSTicksToCycles.

    8     5/25/99 5:28p Shiki
    Added the following line:
    #include <dolphin/os/OSAlloc.h>

    7     5/19/99 5:13p Shiki
    Renamed Halt() to OSHalt().

    6     5/19/99 4:20p Shiki
    Cleaned up address translation macros.

    5     5/18/99 5:06p Shiki
    Fixed address translation macros.

    4     5/11/99 4:42p Shiki
    Refreshed include tree.

    2     5/03/99 5:26p Tianli01
    PPC->EPPC

    1     4/30/99 12:49p Tianli01

    10    4/21/99 4:12p Tianli01
    Added OSGetConsoleType.  Changed OSInterruptLevel to BOOL.  Cleaned up
    Interrupt interface.

    9     4/19/99 5:04p Shiki
    Revised real-time clock API related definitions.

    8     7/15/99 3:46p Shiki
    1. Cleaned up ASSERT stuff
    2. Moved to _DEBUG (avoid DEBUG)

    7     4/15/99 2:45p Tianli01
    Removed OS_BASE_CLOCK (it should have been OS_BUS_CLOCK).  Added
    ASSERTs for PPC.

    6     4/13/99 5:48p Tianli01
    Rearranged.  Added stopwatch APIs.

    5     4/12/99 7:45p Tianli01
    Deleted OSEvent stuff.  Move to DEBUG for consistency (avoid NDEBUG).
    Fixed Halt/OSPanic for PPC target

    4     4/01/99 7:53p Tianli01
    Added OSTimer interfaces.  Fixed time base constants for Arthur.

    3     3/31/99 6:10p Tianli01
    Added arena APIs (not implemented) , address translation constants for
    assembly code, PPC OSReport APIs (only relevant for Arthur board).

    2     3/26/99 2:06p Tianli01
    Broke up into more header files (OSContext.h, OSEmu.h, OSException.h),
    cleaned up, address translation macros added.

    1     3/04/99 2:22p Tianli01
    Initial checkin to new tree

    5     12/18/98 11:28a Shiki
    Removed declaration of  __OSLoadContext2()

    4     12/18/98 9:25a Shiki
    Fixed definition of OSContext for Gecko

    3     12/17/98 9:04a Shiki
    Fixed ASSERT macro

    2     12/16/98 8:54p Shiki
    1. Used OSContext for windows stack environment instead of jmp_buf
       to support __OSWinCall() API
    2. Added the __cdecl modifiers to inline assembler functions

    1     12/15/98 10:08p Shiki

  Change History:
    12/15/1998  Shiki Okasaka   1. Added __OSRaiseException() and
                                   __OSRaiseInterrupt()
                                2. Revised __OSMach structure
    12/12/1998  Shiki Okasaka   Added OSSwitchStack()
    12/11/1998  Shiki Okasaka   1. Moved bEnabled member from __OSMach to
                                   OSContext
                                2. Fixed OSTime from u64 to s64
    12/10/1998  Shiki Okasaka   Revised to refrect the coding guidelines
    12/07/1998  Shiki Okasaka   Included <types.h>
    12/06/1998  Shiki Okasaka   1. Revised __OSInitMach not to time-out
                                   but to take data pointer
                                2. Added OSClearContext()
    12/05/1998  Shiki Okasaka   1. Fixed __OSMach to keep stack bottom
                                2. Revised __OSInitMach to time-out
                                3. Revised OSSaveContext()
    12/03/1998  Shiki Okasaka   1. Added interrupt time-out
                                2. Fixed OSSaveContext()
    12/02/1998  Shiki Okasaka   2nd Edition
    11/28/1998  Shiki Okasaka   Created

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __OS_H__
#define __OS_H__

#ifndef __DOLPHIN_RC__
#include <dolphin/types.h>
#include <dolphin/gx/GXStruct.h>
#endif  // __DOLPHIN_RC__

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
    Arena API
 *---------------------------------------------------------------------------*/

void*   OSGetArenaHi( void );
void*   OSGetArenaLo( void );
void    OSSetArenaHi( void* newHi );
void    OSSetArenaLo( void* newLo );

void*   OSAllocFromArenaLo( u32 size, u32 align );
void*   OSAllocFromArenaHi( u32 size, u32 align );

/*---------------------------------------------------------------------------*
    Round APIs
 *---------------------------------------------------------------------------*/

// Most Dolphin subsystems require 32 byte alignment
#define OSRoundUp32B(x)       (((u32)(x) + 32 - 1) & ~(32 - 1))
#define OSRoundDown32B(x)     (((u32)(x)) & ~(32 - 1))

/*---------------------------------------------------------------------------*
    Basic OS API
 *---------------------------------------------------------------------------*/

void    OSInit          ( void );

#define OS_CONSOLE_MASK             0xf0000000
#define OS_CONSOLE_RETAIL           0x00000000
#define OS_CONSOLE_DEVELOPMENT      0x10000000
#define OS_CONSOLE_TDEV             0x20000000

#define OS_CONSOLE_RETAIL4          0x00000004
#define OS_CONSOLE_RETAIL3          0x00000003
#define OS_CONSOLE_RETAIL2          0x00000002
#define OS_CONSOLE_RETAIL1          0x00000001
#define OS_CONSOLE_TDEVHW4          0x20000007
#define OS_CONSOLE_TDEVHW3          0x20000006
#define OS_CONSOLE_TDEVHW2          0x20000005
#define OS_CONSOLE_TDEVHW1          0x20000004
#define OS_CONSOLE_DEVHW4           0x10000007
#define OS_CONSOLE_DEVHW3           0x10000006
#define OS_CONSOLE_DEVHW2           0x10000005
#define OS_CONSOLE_DEVHW1           0x10000004
#define OS_CONSOLE_MINNOW           0x10000003
#define OS_CONSOLE_ARTHUR           0x10000002
#define OS_CONSOLE_PC_EMULATOR      0x10000001
#define OS_CONSOLE_EMULATOR         0x10000000

u32     OSGetConsoleType( void );

#define OS_SOUND_MODE_MONO          0u
#define OS_SOUND_MODE_STEREO        1u

u32     OSGetSoundMode( void );
void    OSSetSoundMode( u32 mode );

#define OS_PROGRESSIVE_MODE_OFF     0u
#define OS_PROGRESSIVE_MODE_ON      1u

u32     OSGetProgressiveMode( void );
void    OSSetProgressiveMode( u32 on );

#define OS_LANG_ENGLISH         0u
#define OS_LANG_GERMAN          1u
#define OS_LANG_FRENCH          2u
#define OS_LANG_SPANISH         3u
#define OS_LANG_ITALIAN         4u
#define OS_LANG_DUTCH           5u

u8      OSGetLanguage( void );
void    OSSetLanguage( u8 language );

#define OS_EURGB60_OFF          0u
#define OS_EURGB60_ON           1u

u32     OSGetEuRgb60Mode( void );
void    OSSetEuRgb60Mode( u32 on );

void    OSRegisterVersion( const char* id );

/*---------------------------------------------------------------------------*
    Interrupt API
 *---------------------------------------------------------------------------*/

BOOL    OSDisableInterrupts ( void );
BOOL    OSEnableInterrupts  ( void );
BOOL    OSRestoreInterrupts ( BOOL level );

/*---------------------------------------------------------------------------*
    Address Translation API
 *---------------------------------------------------------------------------*/
#ifndef MACOS   // EPPC or GEKKO

// Declarations
void* OSPhysicalToCached    ( u32   paddr  );
void* OSPhysicalToUncached  ( u32   paddr  );
u32   OSCachedToPhysical    ( const void* caddr  );
u32   OSUncachedToPhysical  ( const void* ucaddr );
void* OSCachedToUncached    ( const void* caddr  );
void* OSUncachedToCached    ( const void* ucaddr );

// Upper words of the masks, since UIMM is only 16 bits
#define OS_CACHED_REGION_PREFIX         0x8000
#define OS_UNCACHED_REGION_PREFIX       0xC000
#define OS_PHYSICAL_MASK                0x3FFF

#define OS_BASE_CACHED                  (OS_CACHED_REGION_PREFIX << 16)
#define OS_BASE_UNCACHED                (OS_UNCACHED_REGION_PREFIX << 16)

// Macro overrides
#ifdef MINNOW
// MINNOW makes some assumptions about high order byte of addresses
// for emulation.  Thus we must use ORs/ANDs for this target.
#define OSPhysicalToCached(paddr)       ((void*) ((u32)(paddr) | OS_BASE_CACHED))
#define OSPhysicalToUncached(paddr)     ((void*) ((u32)(paddr) | OS_BASE_UNCACHED))
#define OSCachedToPhysical(caddr)       ((u32)   ((u32)(caddr) & (~OS_BASE_CACHED)))
#define OSUncachedToPhysical(ucaddr)    ((u32)   ((u32)(ucaddr)& (~OS_BASE_UNCACHED)))
#define OSCachedToUncached(caddr)       ((void*) ((u32)(caddr) | (u32)(OS_BASE_UNCACHED - OS_BASE_CACHED)))
#define OSUncachedToCached(ucaddr)      ((void*) ((u32)(ucaddr)& (~ (u32)(OS_BASE_UNCACHED - OS_BASE_CACHED))))

#else // MINNOW
#ifndef _DEBUG
#define OSPhysicalToCached(paddr)       ((void*) ((u32)(paddr) + OS_BASE_CACHED))
#define OSPhysicalToUncached(paddr)     ((void*) ((u32)(paddr) + OS_BASE_UNCACHED))
#define OSCachedToPhysical(caddr)       ((u32)   ((u8*)(caddr) - OS_BASE_CACHED))
#define OSUncachedToPhysical(ucaddr)    ((u32)   ((u8*)(ucaddr)- OS_BASE_UNCACHED))
#define OSCachedToUncached(caddr)       ((void*) ((u8*)(caddr) + (OS_BASE_UNCACHED - OS_BASE_CACHED)))
#define OSUncachedToCached(ucaddr)      ((void*) ((u8*)(ucaddr)- (OS_BASE_UNCACHED - OS_BASE_CACHED)))
#endif  // _DEBUG
#endif // MINNOW
#else   // MACOS - no address translation by default.

#define OSPhysicalToCached(paddr)       ((void*) (paddr))
#define OSPhysicalToUncached(paddr)     ((void*) (paddr))
#define OSCachedToPhysical(caddr)       ((u32)   (caddr))
#define OSUncachedToPhysical(ucaddr)    ((u32)   (ucaddr))
#define OSCachedToUncached(caddr)       ((void*) (caddr))
#define OSUncachedToCached(ucaddr)      ((void*) (ucaddr))

#endif  // MACOS

/*---------------------------------------------------------------------------*
    Real-time Clock API
 *---------------------------------------------------------------------------*/
typedef s64         OSTime;
typedef u32         OSTick;

// SN-Phil: AT ADDRESS MACRO
#if defined(HW1) || defined(HW1_DRIP) || defined(HW2)
u32 __OSBusClock    AT_ADDRESS(OS_BASE_CACHED | 0x00F8);    // sync with OSLoMem.h
u32 __OSCoreClock   AT_ADDRESS(OS_BASE_CACHED | 0x00FC);    // sync with OSLoMem.h
#define OS_BUS_CLOCK        __OSBusClock
#define OS_CORE_CLOCK       __OSCoreClock
#define OS_TIMER_CLOCK      (OS_BUS_CLOCK/4)
#endif  // defined(HW1) || defined(HW1_DRIP)

#ifdef  MINNOW
// true for GEKKO and Lonestars
#define OS_CORE_CLOCK       264000000u
#define OS_BUS_CLOCK        33000000u
#define OS_TIMER_CLOCK      (OS_BUS_CLOCK/4)
#endif  // MINNOW

#ifdef  SPRUCE
#ifdef  GEKKO
// Gekko's on Spruces are running at 333Mhz
#define OS_CORE_CLOCK       333000000u
#define OS_BUS_CLOCK        66000000u
#define OS_TIMER_CLOCK      (OS_BUS_CLOCK/4)
#else   // GEKKO
#define OS_CORE_CLOCK       266000000u
#define OS_BUS_CLOCK        66000000u
#define OS_TIMER_CLOCK      (OS_BUS_CLOCK/4)
#endif  // GEKKO
#endif  // SPRUCE

#ifdef MACOS
extern  u32                 __OSBusClock;
extern  u32                 __OSCoreClock;
extern  u32                 __OSTimerClock;
#define OS_BUS_CLOCK        __OSBusClock
#define OS_CORE_CLOCK       __OSCoreClock
#define OS_TIMER_CLOCK      __OSTimerClock
#endif  // MACOS

// Default Configuration
#ifndef OS_CORE_CLOCK
#define OS_CORE_CLOCK       486000000u
#endif
#ifndef OS_BUS_CLOCK
#define OS_BUS_CLOCK        162000000u
#endif
#ifndef OS_TIMER_CLOCK
#define OS_TIMER_CLOCK      (OS_BUS_CLOCK/4)
#endif

#define OSTicksToCycles( ticks )        (((ticks) * ((OS_CORE_CLOCK * 2) / OS_TIMER_CLOCK)) / 2)
#define OSTicksToSeconds( ticks )       ((ticks) / OS_TIMER_CLOCK)
#define OSTicksToMilliseconds( ticks )  ((ticks) / (OS_TIMER_CLOCK / 1000))
#define OSTicksToMicroseconds( ticks )  (((ticks) * 8) / (OS_TIMER_CLOCK / 125000))
#define OSTicksToNanoseconds( ticks )   (((ticks) * 8000) / (OS_TIMER_CLOCK / 125000))
#define OSSecondsToTicks( sec )         ((sec)  * OS_TIMER_CLOCK)
#define OSMillisecondsToTicks( msec )   ((msec) * (OS_TIMER_CLOCK / 1000))
#define OSMicrosecondsToTicks( usec )   (((usec) * (OS_TIMER_CLOCK / 125000)) / 8)
#define OSNanosecondsToTicks( nsec )    (((nsec) * (OS_TIMER_CLOCK / 125000)) / 8000)

#define OSDiffTick(tick1, tick0)        ((s32) (tick1) - (s32) (tick0))

OSTick  OSGetTick( void );
OSTime  OSGetTime( void );

typedef struct OSCalendarTime
{
    int sec;    // seconds after the minute [0, 61]
    int min;    // minutes after the hour [0, 59]
    int hour;   // hours since midnight [0, 23]
    int mday;   // day of the month [1, 31]
    int mon;    // month since January [0, 11]
    int year;   // years since 0000
    int wday;   // days since Sunday [0, 6]
    int yday;   // days since January 1 [0, 365]

    int msec;   // milliseconds after the second [0,999]
    int usec;   // microseconds after the millisecond [0,999]
} OSCalendarTime;

OSTime OSCalendarTimeToTicks( OSCalendarTime* td );
void   OSTicksToCalendarTime( OSTime ticks, OSCalendarTime* td );

/*---------------------------------------------------------------------------*
    Stopwatch API
 *---------------------------------------------------------------------------*/

typedef struct OSStopwatch
{
    char*       name;       // name of this stopwatch
    u32         hits;       // number of times turned on and off
    OSTime      total;      // total time running
    OSTime      min;        // smallest time measured
    OSTime      max;        // longest time measured

    OSTime      last;       // time at which this sw was last started
    BOOL        running;    // TRUE if sw is running
    u32         _padding;
} OSStopwatch;


void    OSInitStopwatch     ( OSStopwatch* sw, char* name );
void    OSStartStopwatch    ( OSStopwatch* sw );
void    OSStopStopwatch     ( OSStopwatch* sw );
OSTime  OSCheckStopwatch    ( OSStopwatch* sw );
void    OSResetStopwatch    ( OSStopwatch* sw );
void    OSDumpStopwatch     ( OSStopwatch* sw );

/*----------------------------------------------------------------------*
    Debug API
 *----------------------------------------------------------------------*/

#define OSHalt(msg)             OSPanic(__FILE__, __LINE__, msg)

#ifdef _DEBUG

#ifndef ASSERT
#define ASSERT(exp)                                             \
    (void) ((exp) ||                                            \
            (OSPanic(__FILE__, __LINE__, "Failed assertion " #exp), 0))
#endif

#ifndef ASSERTMSG
#if defined(__STDC_VERSION__) && (199901L <= __STDC_VERSION__) || defined(__MWERKS__)  || defined(__SN__)
#define ASSERTMSG(exp, ...)                                     \
    (void) ((exp) ||                                            \
            (OSPanic(__FILE__, __LINE__, __VA_ARGS__), 0))
#else
#define ASSERTMSG(exp, msg)                                     \
    (void) ((exp) ||                                            \
            (OSPanic(__FILE__, __LINE__, (msg)), 0))
#endif
#endif

#ifndef ASSERTMSG1
#define ASSERTMSG1(exp, msg, param1)                            \
    (void) ((exp) ||                                            \
            (OSPanic(__FILE__, __LINE__, (msg),                 \
                     (param1)), 0))
#endif

#ifndef ASSERTMSG2
#define ASSERTMSG2(exp, msg, param1, param2)                    \
    (void) ((exp) ||                                            \
            (OSPanic(__FILE__, __LINE__, (msg),                 \
                     (param1), (param2)), 0))
#endif

#ifndef ASSERTMSG3
#define ASSERTMSG3(exp, msg, param1, param2, param3)            \
    (void) ((exp) ||                                            \
            (OSPanic(__FILE__, __LINE__, (msg),                 \
                     (param1), (param2), (param3)), 0))
#endif

#ifndef ASSERTMSG4
#define ASSERTMSG4(exp, msg, param1, param2, param3, param4)    \
    (void) ((exp) ||                                            \
            (OSPanic(__FILE__, __LINE__, (msg),                 \
                     (param1), (param2), (param3), (param4)), 0))
#endif

#else   // _DEBUG

#ifndef ASSERT
#define ASSERT(exp)                                             ((void) 0)
#endif

#ifndef ASSERTMSG
#if defined(__STDC_VERSION__) && (199901L <= __STDC_VERSION__) || defined(__MWERKS__)  || defined(__SN__)
#define ASSERTMSG(exp, ...)                                     ((void) 0)
#else
#define ASSERTMSG(exp, msg)                                     ((void) 0)
#endif
#endif

#ifndef ASSERTMSG1
#define ASSERTMSG1(exp, msg, param1)                            ((void) 0)
#endif
#ifndef ASSERTMSG2
#define ASSERTMSG2(exp, msg, param1, param2)                    ((void) 0)
#endif
#ifndef ASSERTMSG3
#define ASSERTMSG3(exp, msg, param1, param2, param3)            ((void) 0)
#endif
#ifndef ASSERTMSG4
#define ASSERTMSG4(exp, msg, param1, param2, param3, param4)    ((void) 0)
#endif

#endif  // _DEBUG

#ifndef __DOLPHIN_RC__

inline void OSPanic  ( const char* file, int line, const char* msg, ... )
{

	va_list	vaList;

	va_start( vaList, msg);

	printf( "OSPanic in file %s at line %d, expression: %s");
	assert( 0);

	va_end( vaList);
}
void OSReport ( const char* msg, ... );
void OSVReport( const char* msg, va_list list );
void OSFatal  ( GXColor fg, GXColor bg, const char* msg );

#endif  // __DOLPHIN_RC__

/*---------------------------------------------------------------------------*
    Memory size APIs
 *---------------------------------------------------------------------------*/
u32 OSGetPhysicalMemSize(void);
u32 OSGetConsoleSimulatedMemSize(void);

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*
    Advanced OS APIs
 *---------------------------------------------------------------------------*/

#include <dolphin/os/OSAlloc.h>     // Memory allocation routines and defines
#include <dolphin/os/OSAlarm.h>     // Alarm routines and defines
#include <dolphin/os/OSCache.h>     // Cache operation APIs
#include <dolphin/os/OSContext.h>   // Context structures and defines
#include <dolphin/os/OSError.h>     // Error handler routines and defines
#include <dolphin/os/OSException.h> // Exception handling routines and defines
#include <dolphin/os/OSExpansion.h> // Expansion interface APIs
#include <dolphin/os/OSFastCast.h>  // Fast float/int conversion
#include <dolphin/os/OSFont.h>      // ROM font routines and defines
#include <dolphin/os/OSInterrupt.h> // Interrupt handling routines and defines
#include <dolphin/os/OSMessage.h>   // Message routines and defines
#include <dolphin/os/OSModule.h>    // Relocatable module routines and defines
#include <dolphin/os/OSMutex.h>     // Mutex routines and defines
#include <dolphin/os/OSResetSW.h>   // Reset switch APIs
#include <dolphin/os/OSSerial.h>    // Serial interface APIs
#include <dolphin/os/OSSemaphore.h> // Semaphore routines and defines
#include <dolphin/os/OSThread.h>    // Thread routines and defines
#include <dolphin/os/OSReset.h>     // Reset APIs

#endif  // __OS_H__
