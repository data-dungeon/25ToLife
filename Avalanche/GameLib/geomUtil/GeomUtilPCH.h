/*************************************/
/*                                   */
/*   GeomUtilPCH.h                   */
/*   new engine  06/11/02            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   pre-compiled header             */
/*                                   */
/*************************************/

#ifndef __GEOMUTIL_PCH_H
#define __GEOMUTIL_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

#include "Layers/Assert/Assert.h"
#include "platform/BaseType.h"
#include "Math/MathClass.h"
#include "Math/Vector.h"

#include "geomUtil/planefit.h"
#include "geomUtil/Spline.h"
#include "geomUtil/clipseg.h"
#include "geomUtil/chord.h"
#include "geomUtil/AnimCurve.h"

#endif // defined(WIN32) || defined(CW_PCH)

#endif // __GEOMUTIL_PCH_H
