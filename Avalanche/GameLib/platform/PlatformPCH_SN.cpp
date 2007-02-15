//============================================================================
//=
//= PlatformPCH_SN.cpp - Precompiled header CPP
//=
//============================================================================

// codewarrior flag
#ifdef CW
#define CW_PCH
#endif

#include "PlatformPCH.h"

#if defined(PS2) && defined(SN)

#include "ctroller.cpp"
#include "gameapp.cpp"
#include "keyctrl.cpp"
#include "memcard.cpp"
#include "systemi.cpp"
#include "ctrolmap.cpp"
#include "PS2/PS2app.cpp"
#include "PS2/ps2card.cpp"
#include "PS2/ps2ccd.cpp"
#include "PS2/ps2ctrol.cpp"
#include "PS2/ps2input.cpp"
#include "PS2/CDllLoader.cpp"
#include "PS2/DisplaySimple.cpp"
#include "PS2/DisplaySimpleText.cpp"
#include "PS2/bootlogo.cpp"

#endif // defined(PS2) && defined(SN)
