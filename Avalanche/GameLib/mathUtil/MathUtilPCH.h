//============================================================================
//=
//= MathUtilPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __MATHUTIL_PCH_H
#define __MATHUTIL_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <string.h>

#ifdef WIN32
#include <stdio.h>
#endif

// engine

#include "platform/BaseType.h"
#include "Math/MathClass.h"
#include "mathUtil/Bairstow.h"
#include "mathUtil/Ballistic.h"
#include "mathUtil/Cholesky.h"
#include "mathUtil/eigen.h"
#include "mathUtil/Gauss.h"
#include "mathUtil/linearsystem.h"
#include "mathUtil/NumberRoot.h"
#include "mathUtil/SecantSolve.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __MATHUTIL_PCH_H
