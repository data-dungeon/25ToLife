//============================================================================
//=
//= MathUtilPCH_SN.cpp - Precompiled header CPP
//=
//============================================================================

// codewarrior flag
#ifdef CW
#define CW_PCH
#endif

#include "MathUtilPCH.h"

#if defined(PS2) && defined(SN)

#include "Bairstow.cpp"
#include "Ballistic.cpp"
#include "Cholesky.cpp"
#include "eigen.cpp"
#include "Gauss.cpp"
#include "linearsystem.cpp"
#include "MathUtil.cpp"
#include "NumberRoot.cpp"
#include "SecantSolve.cpp"

#endif // defined(PS2) && defined(SN)
