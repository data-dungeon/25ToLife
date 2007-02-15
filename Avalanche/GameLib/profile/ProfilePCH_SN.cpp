//============================================================================
//=
//= ProfilePCH_SN.cpp - Precompiled header CPP
//=
//============================================================================

// codewarrior flag
#ifdef CW
#define CW_PCH
#endif

#include "ProfilePCH.h"

#if defined(PS2) && defined(SN)

#include "PS2/ps2timer.cpp"
#include "codetimer.cpp"
#include "ctpage.cpp"
#include "Log.cpp"
#include "logtime.cpp"
#include "textpage.cpp"
#include "textscr.cpp"
#include "textwin.cpp"

#endif // defined(PS2) && defined(SN)
