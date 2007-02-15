#pragma once

#ifndef UTILITYEXPORT
#define UTILITYEXPORT
#endif

#include "Fastftol.h"
#include <math.h>

#define A_PI               3.141592653589793238462643383279502884197169399375105820974944592308
#define ABS(a)             (((a)<0)?(-(a)):(a))
#undef MAX
#define MAX(a,b)           (((a)>(b))?(a):(b))
#undef MIN
#define MIN(A,B)           ((A)<(B)?(A):(B))
#define ROWBYTES(A)        (((A)+15)>>4<<1)
#define ROWWORDS(A)        (((A)+15)>>4)
#define ROUNDOFF(A)        (LONG)((A)<(0)?((A)-.5):((A)+.5))
#define ISODD(A)           (A&0x1)
#define INVERSE(A)         { A.x=-A.x;A.y=-A.y;A.z=-A.z; }
#define FLOOR(a)           ((a)>(0) ? (int)(a) : -(int)(-a))
#define CEILING(a)         ((a)==(int)(a) ? (a) : (a)>0 ? 1+(int)a : -(1+(int)(-a)))
#define SIGN(x)            ((x) >= 0 ? 1 : -1)
#define NUMELEMENTS(array) (sizeof(array)/sizeof(array[0]))
#define NULLDELETE(a)      { delete a; a = NULL; }
#define PADWIDTH(x)        (((x)*8 + 31)  & (~31))/8
#define RADIANS			   57.29578f
#define DEGTORAD	         (A_PI / 180.0)
#define RADTODEG	         (180.0 / A_PI)
#define MIN_FLOAT          1.175494351e-38F //3.4e-38f
#define MAX_FLOAT          3.402823466e+38F //3.4e+38f

#define Rnd() ((float)rand() / RAND_MAX)

inline void Round( float &number, float modulo )
{
   float dnum = number;
   dnum += SIGN(dnum) * 0.5f * modulo;
   dnum -= fmod(dnum, modulo);
   number = dnum;
}

inline float RoundValue( float number, float modulo )
{
   float rvalue( number );
   Round( rvalue, modulo );
   return rvalue;
}

inline long RoundValue( float number )
{
   return (long)(number + SIGN(number) * .5f);
}

inline void Round( double &number, double modulo )
{
   double dnum = number;
   dnum += SIGN(dnum) * 0.5 * modulo;
   dnum -= fmod(dnum, modulo);
   number = dnum;
}

inline double RoundValue( double number, double modulo )
{
   double rvalue( number );
   Round( rvalue, modulo );
   return rvalue;
}

inline long RoundValue( double number )
{
   return (long)(number + SIGN(number) * .5f);
}

UINT UTILITYEXPORT GetPrimeLargerThan( UINT nStart );

__inline bool IsDownControl() { return (GetKeyState(VK_CONTROL) & 0x8000) == 0x8000; }
__inline bool IsDownShift() { return (GetKeyState(VK_SHIFT) & 0x8000) == 0x8000; }
__inline bool IsDownAlt() { return (GetKeyState(VK_MENU) & 0x8000) == 0x8000; }

#define THROW_FATAL_ERROR(MSG)			\
   {										      \
   throw (String)MSG;		            \
   }

#define RETHROW_FATAL_ERROR(MSG)			\
{										      \
   throw (String)MSG;  \
}
