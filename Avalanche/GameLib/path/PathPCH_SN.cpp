//============================================================================
//=
//= PathPCH_SN.cpp - Precompiled header CPP
//=
//============================================================================

// codewarrior flag
#ifdef CW
#define CW_PCH
#endif

#include "PathPCH.h"

#if defined(PS2) && defined(SN)

#include "Path.cpp"
#include "PathCollection.cpp"
#include "SegmentPath.cpp"
#include "ShortcutPath.cpp"
#include "SplinePath.cpp"

#endif // defined(PS2) && defined(SN)
