/***************************************************************************/
// PS2 specific implementations of the math calls
/***************************************************************************/
#include <math.h>

#include "platform/BaseType.h"

/***************************************************************************/
// PS2 Microcode declarations
/***************************************************************************/
extern "C"
{
	extern void PS2_Math_SinCos(void) __attribute__ ((section(".vutext")));
	extern void PS2_Math_Sin4(void) __attribute__ ((section(".vutext")));
	extern void PS2_Math_Cos4(void) __attribute__ ((section(".vutext")));
}

/***************************************************************************/
// The trig functions
/***************************************************************************/

inline float Math::Sin(
float radians)
{
#if defined(PS2) && defined(SN)
#ifdef __VCPRECOMPILE // SN preprocess
	return 0.0f;
#else
	float results;
	register u128 sincosin asm("vf1");
	register u128 sincosout asm("vf2");
	register u128 acc asm("vf0");
	asm("qmtc2			%1,	  %0" : "=j" (sincosin) : "r" (radians));

	// this asm needs to be one block to keep the interlock from getting
	// a non-interlocking instruction before it.
	asm(
		"vcallms		PS2_Math_Sin4  \r\n"
		"qmfc2.i		%0,	  %1     \r\n" : "=r" (results), "=j" (sincosout), "=j" (acc), "+j" (sincosin), "=j" (acc));
	return results;
#endif
#else
	return(::sinf(radians));
#endif
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Cos(
float radians)
{
#if defined(PS2) && defined(SN)
#ifdef __VCPRECOMPILE // SN preprocess
	return 0.0f;
#else
	float results;
	register u128 sincosin asm("vf1");
	register u128 sincosout asm("vf2");
	register u128 acc asm("vf0");
	asm("qmtc2			%1,	  %0" : "=j" (sincosin) : "r" (radians));
	// this asm needs to be one block to keep the interlock from getting
	// a non-interlocking instruction before it.
	asm(
		"vcallms		PS2_Math_Cos4   \r\n"
		"qmfc2.i		%0,	  %1      \r\n" : "=r" (results), "=j" (sincosout), "+j" (sincosin), "=j" (acc));
	return results;
#endif
#else
	return(::cosf(radians));
#endif
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Tan(
float radians)
{
	return(::tanf(radians));
}

/***************************************************************************/
// return the Sin & Cos simultaneously (faster on some cpu's)
/***************************************************************************/
inline void Math::SinCos(
float radians,
float &s,
float &c)
{
#if defined(PS2) && defined(SN)
#ifndef __VCPRECOMPILE // SN preprocess
	register u128 sincosin asm("vf1");
	register u128 sinout asm("vf2");
	register u128 cosout asm("vf3");
	register u128 acc asm("vf0");
	asm("qmtc2			%1,	  %0" : "=j" (sincosin) : "r" (radians));
	// this asm needs to be one block to keep the interlock from getting
	// a non-interlocking instruction before it.
	asm(
		"vcallms		PS2_Math_SinCos  \r\n"
		"qmfc2.i		%0,	  %1       \r\n" : "=r" (s), "=j" (sinout), "=j" (cosout), "+j" (sincosin), "=j" (acc));
	asm("qmfc2		%0,  %1" : "=r" (c) : "j" (cosout));
#endif
#else
	s = ::sinf(radians);
	c = ::cosf(radians);
#endif
}

/***************************************************************************/
// Inverse functions
/***************************************************************************/
inline float Math::ArcSin(
float x)
{
	return(::asinf(x));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::ArcCos(
float x)
{
	// floating point math is such a pain.  Constrain the domain, because
	// a common use of ArcCos is to get the angle between two vectors from
	// their dot product, which can be *very* slightly greater than one in
	// some cases, but even being over 1 by 1 millionth is enough to make 
	// acosf choke.

	// TJC - this also covers some unusal cases where this algorithm
	// does not return correct results.
	if( x < -1.0f || Math::Equal(-1.0f, x)) 
		return Math::Pi;
	if( x > 1.0f ) 
		return 0.0f;
	if(Math::Equal(0.0f, x))
		return Math::HalfPi;

	float retval = Math::ArcTan(Math::Sqrt(1-x*x) / x);
	return (retval > 0.0f) ? retval : Math::Pi + retval;
}

/***************************************************************************/
/***************************************************************************/
inline float Math::ArcTan(
float x)
{
#if defined(PS2) && defined(SN)
	if (x < 0) {
		return -ArcTanBase(-x);
	}
	else
	{
		return ArcTanBase(x);
	}
#else
	return(::atanf(x));
#endif
}

/***************************************************************************/
/***************************************************************************/
inline float Math::ArcTan2(
float y,
float x)
{
	return(::atan2f(y, x));
}

/***************************************************************************/
// return absolute value of x
/***************************************************************************/
inline float Math::Abs(
float x)
{
	float retval;
	asm("abs.s %0, %1" : "=f" (retval) : "f" (x));
	return retval;
}

/***************************************************************************/
// return absolute value of i
/***************************************************************************/
inline int Math::Abs(
int i)
{
	long retval;
	asm("pabsw %0, %1" : "=r" (retval) : "r" ((long)i));
	return retval;
}

/***************************************************************************/
// Chop of the decimal places without rounding
/***************************************************************************/
#define FLOOR_POINTFIVE 0x3F000000 // 0.5f
#define FLOOR_BIGNUM (3 << 22)
inline float Math::Floor(
float x)
{
	register int testval;
	register float temp;
	register float work;

	work = x + FLOOR_BIGNUM;
	work -= FLOOR_BIGNUM;
	
	asm("sub.s		%0, %1, %2"		: "=f" (temp)		: "f" (x), "f" (work));
	asm("mfc1		%0, %1"			: "=r" (testval)	: "f" (temp));
	asm("slt			%0, %1, $zero"	: "=r" (testval)	: "r" (testval));
	asm("mtc1		%1, %0"			: "=f" (temp)		: "r" (testval));
	asm("cvt.s.w	%0, %1"			: "=f" (temp)		: "f" (temp));
	asm("sub.s		%0, %1, %2"		: "=f" (work)		: "f" (work), "f" (temp));

	return work;
}

/***************************************************************************/
// Return the ceil of x
/***************************************************************************/
inline float Math::Ceil(
float x)
{
	register int testval;
	register float temp;
	register float work;

	work = x + FLOOR_BIGNUM;
	work -= FLOOR_BIGNUM;
	
	asm("sub.s		%0, %1, %2"		: "=f" (temp)		: "f" (x), "f" (work));
	asm("mfc1		%0, %1"			: "=r" (testval)	: "f" (temp));
	asm("slt			%0, $zero, %1"	: "=r" (testval)	: "r" (testval));
	asm("mtc1		%1, %0"			: "=f" (temp)		: "r" (testval));
	asm("cvt.s.w	%0, %1"			: "=f" (temp)		: "f" (temp));
	asm("add.s		%0, %1, %2"		: "=f" (work)		: "f" (work), "f" (temp));

	return work;
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Trunc(float x)
{
	register int testval;
	register int signval;
	register float temp;
	register float work;

	work = x + FLOOR_BIGNUM;
	work -= FLOOR_BIGNUM;
	
	asm("mfc1		%0, %1"			: "=r" (testval)	: "f" (x));
	asm("slt			%0, %1, $zero"	: "=r" (signval)	: "r" (testval));
	asm("sub.s		%0, %1, %2"		: "=f" (temp)		: "f" (x), "f" (work));
	asm("mfc1		%0, %1"			: "=r" (testval)	: "f" (temp));
	asm("slt			%0, $zero, %1"	: "=r" (testval)	: "r" (testval));
	asm("and			%0, %1, %2"		: "=r" (testval)	: "r" (signval), "r" (testval));
	asm("mtc1		%1, %0"			: "=f" (temp)		: "r" (testval));
	asm("cvt.s.w	%0, %1"			: "=f" (temp)		: "f" (temp));
	asm("add.s		%0, %1, %2"		: "=f" (work)		: "f" (work), "f" (temp));

	return work;
}

/***************************************************************************/
// Return the fractional part of x
/***************************************************************************/
inline float Math::Frac(
float x)
{
	return x - Trunc(x);
}

/***************************************************************************/
// Return floating point remainder
/***************************************************************************/
inline float Math::Mod(
float x,
float y)
{
	return(::fmodf(x, y));
}

/***************************************************************************/
// return the inverse of x
/***************************************************************************/
inline float Math::Inverse(
float x)
{
	return(1.0f / x);
}

/***************************************************************************/
// return the square root of x
/***************************************************************************/
inline float Math::Sqrt(
float x)
{
	float res;
	asm("sqrt.s %0, %1" : "=f" (res) : "f" (x));
	return res;
}

// return 1 / square root of x
inline float Math::InverseSqrt(float x)
{
	float res;
	asm("rsqrt.s %0, %1, %2" : "=f" (res) : "f" (1.0f), "f" (x));
	return res;
}

// return x / Sqrt(y)
inline float Math::DivSqrt(float x, float y)
{
	float res;
	asm("rsqrt.s %0, %1, %2" : "=f" (res) : "f" (x), "f" (y));
	return res;
}

/***************************************************************************/
// Raise x^y
/***************************************************************************/
inline float Math::Pow(
float x,
float y)
{
	return(::powf(x, y));
}

/***************************************************************************/
// return e^x
/***************************************************************************/
inline float Math::Exp(
float x)
{
	return(::expf(x));
}

/***************************************************************************/
// return natural log of x
/***************************************************************************/
inline float Math::Log(
float x)
{
	return(::logf(x));
}

/***************************************************************************/
// return base 10 log of x
/***************************************************************************/
inline float Math::Log10(
float x)
{
	return(::log10f(x));
}

/***************************************************************************/
// The trigonometric functions uSing series (faster?!?)
/***************************************************************************/
inline float Math::FastSin(
float radians)
{
	float x = radians * radians * InvPiSquared;
	return(radians *
				(1.0f - x) *
				(1.0f - x * 0.25f) *
				(1.0f - x * 0.111111111f));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::FastCos(
float radians)
{
	float x = 4.0f * radians * radians * InvPiSquared;
	return((1.0f - x) *
				(1.0f - x * 0.111111111f) *
				(1.0f - x * 0.04f) *
				(1.0f - x * 0.02040816f));  // Last term needed?
}

/***************************************************************************/
/***************************************************************************/
inline float Math::FastTan(
float radians)
{
	// ha, tricked ya...  Doesn't look like this will be faster.  Ideas?
	return Math::Tan(radians);
}

/***************************************************************************/
// return the Sin & Cos simultaneously (faster on some cpu's)
/***************************************************************************/
inline void Math::FastSinCos(
float radians,
float &s,
float &c)
{
	s = FastSin(radians);
	c = FastCos(radians);
}

/***************************************************************************/
// fast square root, may not be accurate
/***************************************************************************/
inline float Math::FastSqrt(
float x)
{
	return Math::Sqrt(x);
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Min(
float x,
float y)
{
	float temp;
	asm("min.s %0, %1, %2" : "=f" (temp) : "f" (x), "f" (y));
	return temp;
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Max(
float x,
float y)
{
	float temp;
	asm("max.s %0, %1, %2" : "=f" (temp) : "f" (x), "f" (y));
	return temp;
}

/***************************************************************************/
/***************************************************************************/
inline int Math::Min(
int x,
int y)
{
	long temp;
	asm("pminw %0, %1, %2" : "=r" (temp) : "r" ((long)x), "r" ((long)y));
	return temp;
}

/***************************************************************************/
/***************************************************************************/
inline int Math::Max(
int x,
int y)
{
	long temp;
	asm("pmaxw %0, %1, %2" : "=r" (temp) : "r" ((long)x), "r" ((long)y));
	return temp;
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Clamp(
float x,
float minimum,
float maximum)
{
	return Min(Max(x, minimum), maximum);
}

/***************************************************************************/
/***************************************************************************/
inline int Math::Clamp(
int x,
int minimum,
int maximum)
{
	return Min(Max(x, minimum), maximum);
}
