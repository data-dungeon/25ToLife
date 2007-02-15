//============================================================================
//=
//= ProfilePCH.h - Precompiled headers
//=
//============================================================================

#ifndef __PROFILE_PCH_H
#define __PROFILE_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#if defined (WIN32) && !defined (_XBOX)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#endif //WIN32

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#endif

// engine

#include "Math/MathClass.h"
#include "Layers/Memory.h"
#include "profile/codetimer.h"
#include "profile/ctpage.h"
#include "profile/textpage.h"
#include "profile/log.h"
#include "profile/logtime.h"
#include "profile/textscr.h"
#include "profile/textwin.h"

#if defined (WIN32) && !defined (_XBOX)
#include "profile/Win32/auxtext.h"
#include "profile/Win32/textscrw32.h"
#include "profile/Win32/w32timer.h"
#endif //WIN32

#if defined (_XBOX)
#include "profile/Xbox/xboxtimer.h"
#endif

#ifdef PS2
#include "profile/PS2/ps2timer.h"
#endif //PS2

#ifdef GCN
#include "profile/GCN/gcntimer.h"
#endif //GCN

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __PROFILE_PCH_H
