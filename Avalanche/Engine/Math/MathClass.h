/***************************************************************************/
// A simple utility class that encompasses all the standard <math.h> stuff,
// along with some more efficient fast functions (all entitled "fast")
/***************************************************************************/
#ifndef M_MATH_H
#define M_MATH_H

class Math
{
public:
	// Some constants
	static const float Pi;
	static const float TwoPi;
	static const float HalfPi;
	static const float QuarterPi;
	static const float PiSquared;
	static const float InvPiSquared;
	static const float InvPi;
	static const float Epsilon;
	static const float PositiveInfinity;
	static const float NegativeInfinity;
	static const float SquareRootOfTwo;

	inline static bool SameSign(float f1, float f2);
	inline static bool SameSign(int i1, int i2);
	inline static float Sign(float f);
	inline static int Sign(int i);

	// Degree to radians and back
	inline static float Deg2Rad(float degrees);
	inline static float Rad2Deg(float radians);

	// Make an angle in the range -pi to pi
	inline static float NormalAngle(float radians);
	inline static float NormalAngleDeg(float degrees);

	// find the smallest delta between 2 angles (result can be positive or negative)
	inline static float AngleDelta(float radians1, float radians2);

	// The trig functions with radians as input
	inline static float Sin(float radians);
	inline static float Cos(float radians);
	inline static float Tan(float radians);

	// return the sin & cos simultaneously (faster on some cpu's)
	inline static void SinCos(float radians, float &s, float &c);

	// The trig functions with degrees as input
	inline static float SinDeg(float degrees);
	inline static float CosDeg(float degrees);
	inline static float TanDeg(float degrees);

	// Inverse functions
	inline static float ArcSin(float x);
	inline static float ArcCos(float x);
	inline static float ArcTan(float x);
	inline static float ArcTan2(float y, float x);
	inline static float FastArcTan2(float y, float x); // TJC - fast approximation

#if defined(PS2) && defined(SN)
	static float ArcTanBase(float x);
#endif

	// Inverse functions that output degrees
	inline static float ArcSinDeg(float x);
	inline static float ArcCosDeg(float x);
	inline static float ArcTanDeg(float x);
	inline static float ArcTan2Deg(float y, float x);

	// The hyperbolic functions
	inline static float Sinh(float x);
	inline static float Cosh(float x);
	inline static float Tanh(float x);

	// Inverse hyperbolic functions
	inline static float ArcSinh(float x);
	inline static float ArcCosh(float x);
	inline static float ArcTanh(float x);

	// return absolute value of x
	inline static float Abs(float x);

	// An integer version (as the PS2 doesn't have it)
	inline static int Abs(int i);

	// Chop of the decimal places without rounding
	inline static float Floor(float x);

	// Return the ceil of x
	inline static float Ceil(float x);

	// Round (with .5 going higher)
	inline static float Round(float x);

	// Return the integer part of x (this is like a floor if x > 0, and a ceil if x < 0)
	inline static float Trunc(float x);

	// Return the fractional part of x
	inline static float Frac(float x);

	// Return floating point remainder (see the standard c docs)
	inline static float Mod(float x, float y);

	// return the inverse of x
	inline static float Inverse(float x);

	// return the square root of x
	inline static float Sqrt(float x);

	// return 1 / square root of x
	inline static float InverseSqrt(float x);

	// return x / Sqrt(y)
	inline static float DivSqrt(float x, float y);

	// Raise x^y
	inline static float Pow(float x, float y);
	inline static float Square(float x);
	inline static float Cube(float x);

	// return e^x
	inline static float Exp(float x);

	// return natural log of x
	inline static float Log(float x);

	// return base 10 log of x
	inline static float Log10(float x);

	// return base 2 log of x
	inline static float Log2(float x);

	// return the minimum/maximum of the two numbers.
	inline static float Min(float x, float y);
	inline static float Max(float x, float y);
	inline static int Min(int x, int y);
	inline static int Max(int x, int y);
	inline static unsigned int Min(unsigned int x, unsigned int y);
	inline static unsigned int Max(unsigned int x, unsigned int y);
	inline static long Min(long x, long y);
	inline static long Max(long x, long y);
	inline static unsigned long Min(unsigned long x, unsigned long y);
	inline static unsigned long Max(unsigned long x, unsigned long y);
	
	// Nice functions for triples
	inline static float Min(float a, float b, float c);
	inline static float Max(float a, float b, float c);

	// Swaps
	template <class T>
	static void Swap(T &a, T &b)
	{
		T temp = a;
		a = b;
		b = temp;
	}

	// Mapping from one range to another
	inline static float LinearMap( float x, float min1, float max1, float min2, float max2 );

	// clamp the number between min and max.
	inline static float Clamp(float x, float minimum = 0.0f, float maximum = 1.0f);
	inline static int Clamp(int x, int minimum = 0, int maximum = 255);

	// return true if the number is within epsilon of zero (0.0f), false
	// otherwise
	inline static bool Zero(float x, float epsilon = 1e-7f);

	// return true if the numbers are within epsilon of each other, false
	// otherwise.
	inline static bool Equal(float x, float y, float epsilon = 1e-7f);

	// get a float AS an int.  THIS IS PROBABLY NOT WHAT YOUR LOOKING FOR!
	// this does basically (*(float*)&x) only it does it in a way that compilers
	// optimizers like a little better
	inline static float IntAsFloat(int x);
	inline static int FloatAsInt(float x);

	// Increment or decrement a floating point number by the smallest possible amount
	inline static float IncEpsilon(float x);
	inline static float DecEpsilon(float x);

	// The functions that can be implemented more quickly
	// than usual, with some loss of precision.
	// Feel free to expand these functions

	// The trigonometric functions
	inline static float FastSin(float radians);
	inline static float FastCos(float radians);
	inline static float FastTan(float radians);

	// return the sin & cos simultaneously (faster on some cpu's)
	inline static void FastSinCos(float radians, float &s, float &c);

	// fast square root, may not be accurate
	inline static float FastSqrt(float x);

	// Inlines to trap IEEE 754 floating point divide by zero before it
	// happens.  If false, the operation n/d will NOT result in a divide by
	// zero. If true, it MAY result in a divide by zero. 64 bit version
	// assumes little-endian storage.
	inline static bool DivByZero(float n, float d);
	//inline static bool DivByZero64(float64 n, float64 d)

	// Macros to see if a IEEE 754 floating point number is a NaN (quiet or
	// signaling) or infinity.
	inline static bool IsNaN(float f);
	//inline static bool IsNaN64(float f);

	// Compress/decompress floating point values.
	inline static unsigned char UnsignedCompressToByte(float value, float maxValue);
	inline static unsigned short UnsignedCompressToWord(float value, float maxValue);
	inline static float UnsignedDecompress(unsigned char value, float maxValue);
	inline static float UnsignedDecompress(unsigned short value, float maxValue);
	inline static signed char SignedCompressToByte(float value, float maxValue);
	inline static short SignedCompressToWord(float value, float maxValue);
	inline static float SignedDecompress(signed char value, float maxValue);
	inline static float SignedDecompress(short value, float maxValue);

	inline static void MoveToValue( float *src, float val, float spd);
	inline static void MoveToValue( int *src, int val, int spd);
	inline static void MoveToValue( unsigned char *src, unsigned char val, char spd);
	inline static void MoveToZero( float *src, float spd);
	inline static void MoveToZero( int *src, int spd);
	inline static void MoveToZero( unsigned char *src, char spd);

#ifdef PS2
	static void UploadVU0Microcode(void);
#endif

#ifdef WIN32
#ifndef CONSUMER_BUILD
	// check whether math project has MMX_OK defined or not. the issue is that all projects in a game
	// must be consistent-- MMX_OK is either 1 or 0 for all. inconsistency leads to bizarre crashes
	static int MMXOK();
#endif //CONSUMER_BUILD
#endif //WIN32

private:
	// Some internal constants
	static const float m_Deg2Rad;
	static const float m_Rad2Deg;
};

// Include the appropriate implementation for the platform
#if defined(WIN32)
#include "Math/win32/Math.hpp"
#elif defined(PS2)
#include "Math/PS2/Math.hpp"
#elif defined(GCN)
#include "Math/gcn/Math.hpp"
#endif

// Include the generic stuff
#include "Math/Generic/Math.hpp"


/***************************************************************************/
// the basic c lib functions should not be used, so make them spout errors
// if they are used.
/***************************************************************************/
#include "Math/MathDeprecate.h"

#undef M_PI
#define M_PI M_PI__DEPRECATED__use_Math_class

// --- some useful fixed point conversion macros
#define CONVERT_FLOAT_TO_U8(fValue, fMaxValue)	(fValue == 0.0f) ? 0 : Math::Max(1, ((u8) ((fValue / fMaxValue) * 255.0f)))		// convert to 0.0 to 1.0, then expand to integer range
#define CONVERT_U8_TO_FLOAT(nValue, fMaxValue)	(((float) nValue / 255.0f) * fMaxValue)	// convert to 0.0 to 1.0, then expand to float range
#define CONVERT_FLOAT_TO_S8(fValue, fMaxValue)	(fValue == 0.0f) ? 0 : (fValue < 0.0f) ? Math::Min(-1, ((s8) ((fValue / fMaxValue) * 127.0f))) : Math::Max(1, ((s8) ((fValue / fMaxValue) * 127.0f)))
#define CONVERT_S8_TO_FLOAT(nValue, fMaxValue)	(((float) nValue / 127.0f) * fMaxValue)
#define CONVERT_FLOAT_TO_U16(fValue, fMaxValue)		(fValue == 0.0f) ? 0 : Math::Max(1, ((u16) ((fValue / fMaxValue) * 65535.0f)))	// convert to 0.0 to 1.0, then expand to integer range
#define CONVERT_U16_TO_FLOAT(nValue, fMaxValue)		(((float) nValue / 65535.0f) * fMaxValue)	// convert to 0.0 to 1.0, then expand to float range
#define CONVERT_FLOAT_TO_S16(fValue, fMaxValue) (fValue == 0.0f) ? 0 : (fValue < 0.0f) ? Math::Min(-1, ((s16) ((fValue / fMaxValue) * 32767.0f))) : Math::Max(1, ((s16) ((fValue / fMaxValue) * 32767.0f)))
#define CONVERT_S16_TO_FLOAT(nValue, fMaxValue)	(((float) nValue / 32767.0f) * fMaxValue)


#endif
