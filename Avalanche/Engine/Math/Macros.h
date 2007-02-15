/* --------------------------------------------------
 * File    : M_MACROS.H
 * Created : Mon Oct 26 15:23:50 1998
 * Descript:
 * --------------------------------------------------*/
#ifndef __M_MACROS_H
#define __M_MACROS_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

#include "Math/MathClass.h"
#include "Math/Vector.h"

#if defined(__cplusplus)
extern "C" {
#endif

//#ifndef max
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#endif
//
//#ifndef min
//#define min(a,b)            (((a) < (b)) ? (a) : (b))
//#endif

/* --------------------------------------------------
 * Look in m_math.h for the macros...they've moved
 * into a class.
 * --------------------------------------------------*/

// JMH - inline function to turn a normal into a slope
//   0 degrees = level floor
//  90 degrees = straight up/down wall
//	180 degrees = level ceiling
inline float mthGetSlopeOfNormal( Vector3 &normal)
{
	return( Math::Abs( Math::Rad2Deg( Math::ArcTan2( normal.y, normal.LengthXZ()) - ( Math::HalfPi ))));
}

#if defined(__cplusplus)
}
#endif

#endif __M_MACROS_H
