/***************************************************************************/
// Win32 specific implementations of the math calls
/***************************************************************************/
#include <math.h>
#include "Layers/Assert/Assert.h"

/***************************************************************************/
// The trig functions
/***************************************************************************/
inline float Math::Sin(
float radians)
{
	return(::sinf(radians));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Cos(
float radians)
{
	return(::cosf(radians));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Tan(
float radians)
{
	return(::tanf(radians));
}

/***************************************************************************/
// return the sin & cos simultaneously (faster on some cpu's)
/***************************************************************************/
inline void Math::SinCos(
float radians,
float &s,
float &c)
{
	s = Sin(radians);
	c = Cos(radians);
}

/***************************************************************************/
// Inverse functions
/***************************************************************************/
inline float Math::ArcSin(
float x)
{
	ASSERT(x >= -1.0f && x <= 1.0f);
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
	
	// we still want to assert to make sure we're at least close to the domain,
	// though.
	ASSERT(x >= -1.0001f && x <= 1.0001f);
	if( x < -1.0f ) x = -1.0f;
	if( x > 1.0f ) x = 1.0f;
	return(::acosf(x));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::ArcTan(
float x)
{
	return(::atanf(x));
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
	return(::fabsf(x));
}

/***************************************************************************/
// return absolute value of i
/***************************************************************************/
inline int Math::Abs(
int i)
{
	return(::fabsf(i));
}

/***************************************************************************/
// Chop of the decimal places without rounding
/***************************************************************************/
inline float Math::Floor(
float x)
{
	return(::floorf(x));
}

/***************************************************************************/
// Return the ceil of x
/***************************************************************************/
inline float Math::Ceil(
float x)
{
	return(::ceilf(x));
}


/***************************************************************************/
// Return the fractional part of x
/***************************************************************************/
inline float Math::Frac(
float x)
{
	return(::fmodf(x, 1.0f));
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
	return(::sqrtf(x));
}

// return 1 / square root of x
inline float Math::InverseSqrt(float x)
{
	return 1.0f / Sqrt(x);
}

// return x / Sqrt(y)
inline float Math::DivSqrt(float x, float y)
{
	return x / Sqrt(y);
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
// The trigonometric functions using series (faster?!?)
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
	return(Tan(radians));
}

/***************************************************************************/
// return the sin & cos simultaneously (faster on some cpu's)
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
	// um, write this.  There are no obvious solutions on the Pentium
	return(Sqrt(x));
}
