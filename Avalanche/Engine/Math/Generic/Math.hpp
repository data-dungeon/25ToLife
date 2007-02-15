/***************************************************************************/
// A simple utility class that encompasses all the standard <math.h> stuff,
// along with some more efficient fast functions.
//
// The functions in the generic module are all derived from basic,
// potentially platform specific math calls.
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/
inline bool Math::SameSign(float f1, float f2)
{
	return (Sign(f1) == Sign(f2));
}

/***************************************************************************/
/***************************************************************************/
inline bool Math::SameSign(int i1, int i2)
{
	return (Sign(i1) == Sign(i2));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Sign(float f)
{
	if (Math::Zero(f))
		return 0.0f;
	else if (f > 0.0f)
		return 1.0f;
	else
		return -1.0f;
}

/***************************************************************************/
/***************************************************************************/
inline int Math::Sign(int i)
{
	if (i == 0)
		return 0;
	else if (i > 0)
		return 1;
	else
		return -1;
}

/***************************************************************************/
// Degree to radians
/***************************************************************************/
inline float Math::Deg2Rad(
float degrees)
{
	return(degrees * m_Deg2Rad);
}

/***************************************************************************/
// radians to degrees
/***************************************************************************/
inline float Math::Rad2Deg(
float radians)
{
	return(radians * m_Rad2Deg);
}

/***************************************************************************/
// Make an angle in the range -pi to pi
/***************************************************************************/
inline float Math::NormalAngle(
float radians)
{
	// Trap big angles
	if ((radians > 1000.0f) || (radians < -1000.0f))
	{
		// ASSERT(false);
		radians = 0.0f;
	}

	// Can this be done faster with fmod?
	while (radians < -Pi)
		radians += TwoPi;
	while (radians > Pi)
		radians -= TwoPi;

	return(radians);
}

/***************************************************************************/
// Make an angle in the range -180 to 180
/***************************************************************************/
inline float Math::NormalAngleDeg( 
float degrees ) 
{
	return Rad2Deg( NormalAngle( Deg2Rad( degrees ) ) ); 
}

/***************************************************************************/
// find the smallest delta between 2 angles (result can be positive or negative).
// result is calculated so that radians1 + delta = radians2
/***************************************************************************/
inline float Math::AngleDelta(float radians1, float radians2)
{
	return(NormalAngle(radians2 - radians1));
}

/***************************************************************************/
// The trig functions in degrees
/***************************************************************************/
inline float Math::SinDeg(
float degrees)
{
	return(Sin(degrees * m_Deg2Rad));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::CosDeg(
float degrees)
{
	return(Cos(degrees * m_Deg2Rad));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::TanDeg(
float degrees)
{
	return(Tan(degrees * m_Deg2Rad));
}

/***************************************************************************/
// Inverse functions in degrees
/***************************************************************************/
inline float Math::ArcSinDeg(
float x)
{
	return(ArcSin(x) * m_Rad2Deg);
}

/***************************************************************************/
/***************************************************************************/
inline float Math::ArcCosDeg(
float x)
{
	return(ArcCos(x) * m_Rad2Deg);
}

/***************************************************************************/
/***************************************************************************/
inline float Math::ArcTanDeg(
float x)
{
	return(ArcTan(x) * m_Rad2Deg);
}

/***************************************************************************/
/***************************************************************************/
inline float Math::ArcTan2Deg(
float y,
float x)
{
	return(ArcTan2(y, x) * m_Rad2Deg);
}

/***************************************************************************/
// approximation of atan2f(y,x)
/***************************************************************************/
#define REMAP_TO_NPI_TO_PI(val) (((val) * Math::TwoPi * 0.125f) - Math::HalfPi)
inline float Math::FastArcTan2(
float y,
float x)
{
	float ax = Math::Abs(x);
	float ay = Math::Abs(y);
	if(y > 0)
	{
		if(x > 0)
		{
			if(ax > ay)
			{
				// octant 0
				return REMAP_TO_NPI_TO_PI(ay / ax);
			}
			else
			{
				// octant 1
				return REMAP_TO_NPI_TO_PI(1 + (1 - ax / ay));
			}
		}
		else
		{
			if(ax < ay)
			{
				// octant 2
				return REMAP_TO_NPI_TO_PI(2 + (ax / ay));
			}
			else
			{
				// octant 3
				return REMAP_TO_NPI_TO_PI(3 + (1 - ay / ax));
			}
		}
	}
	else
	{
		if(x < 0)
		{
			if(ax > ay)
			{
				// octant 4
				return REMAP_TO_NPI_TO_PI(4 + (ay / ax));
			}
			else
			{
				// octant 5
				return REMAP_TO_NPI_TO_PI(5 + (1 - ax / ay));
			}
		}
		else
		{
			if(ax > ay)
			{
				// octant 6
				return REMAP_TO_NPI_TO_PI(6 + (ax / ay));
			}
			else
			{
				// octant 7
				return REMAP_TO_NPI_TO_PI(7 + (1 - ay / ax));
			}
		}
	}
}

#if !defined(WIN32)
/***************************************************************************/
// The hyperbolic functions
/***************************************************************************/
inline float Math::Sinh(
float x)
{
	return(0.5f * (Exp(x) - Exp(-x)));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Cosh(
float x)
{
	return(0.5f * (Exp(x) + Exp(-x)));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Tanh(
float x)
{
	float exppx = Exp(x);
	float expnx = Exp(-x);
	return((exppx - expnx) / (exppx + expnx));
}
#endif

/***************************************************************************/
// Inverse hyperbolic functions
/***************************************************************************/
inline float Math::ArcSinh(
float x)
{
	return(Log(x + Sqrt(x * x + 1.0f)));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::ArcCosh(
float x)
{
	if (x < 1.0f)
		return(PositiveInfinity);

	return(Log(x + Sqrt(x * x - 1.0f)));
}

/***************************************************************************/
/***************************************************************************/
inline bool Math::DivByZero(float n, float d)
{
	// assumes unsigned int is 32 bits!
	unsigned int dExp = ((*((unsigned int*)&d)) >> 23) & 0xFF;
	if (dExp != 0)
	{
		unsigned int nExp = ((*((unsigned int*)&n)) >> 23) & 0xFF;
		return(nExp >= (0x7F + dExp));
	}
	return true;
}

/***************************************************************************/
/***************************************************************************/
inline float Math::ArcTanh(
float x)
{
	float denom = 1.0f - x * x;
	if (denom <= 0.0f)
		return(PositiveInfinity);
	denom = Sqrt(denom);

	float num = 1.0f + x;
	if (DivByZero(num, denom))
		return(PositiveInfinity);

	return(Log(num / denom));
}

/***************************************************************************/
// Round (with .5 going higher)
/***************************************************************************/
inline float Math::Round(
float x)
{
	return(Floor(x + 0.5f));
}

/***************************************************************************/
/***************************************************************************/
#ifndef PS2
inline float Math::Trunc(float x)
{
	if(x < 0)
		return Ceil(x);
	return Floor(x);
}
#endif

/***************************************************************************/
/***************************************************************************/
inline bool Math::Zero(
float x, 
float epsilon)
{
	return(Abs(x) < epsilon);
}

/***************************************************************************/
/***************************************************************************/
inline bool Math::Equal(
float x,
float y,
float epsilon)
{
	return(Zero(x - y, epsilon));
}

/***************************************************************************/
// Increment or decrement a floating point number by the smallest possible amount
/***************************************************************************/
inline float Math::IncEpsilon(
float x)
{
	int &i = (int &)x;
	++i;
	return(x);
}

/***************************************************************************/
// Increment or decrement a floating point number by the smallest possible amount
/***************************************************************************/
inline float Math::DecEpsilon(
float x)
{
	int &i = (int &)x;
	--i;
	return(x);
}

/***************************************************************************/
// get a float AS an int.  THIS IS PROBABLY NOT WHAT YOUR LOOKING FOR!
// this does basically (*(float*)&x) only it does it in a way that compilers
// optimizers like a little better
/***************************************************************************/
inline float Math::IntAsFloat(int x)
{
	union { float f; int i; } temp;
	temp.i = x;
	return temp.f;
}

/***************************************************************************/
// get a float AS an int.  THIS IS PROBABLY NOT WHAT YOUR LOOKING FOR!
// this does basically (*(float*)&x) only it does it in a way that compilers
// optimizers like a little better
/***************************************************************************/
inline int Math::FloatAsInt(float x)
{
	union { float f; int i; } temp;
	temp.f = x;
	return temp.i;
}


/***************************************************************************/
/***************************************************************************/
inline float Math::Square(
float x)
{
	return x * x;
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Cube(
float x)
{
	return x * x * x;
}

/***************************************************************************/
/***************************************************************************/
inline float Math::Log2(
float x)
{
	return Log10(x) / Log10(2);
}

/***************************************************************************/
/***************************************************************************/
inline float Math::LinearMap( 
float x, 
float min1, 
float max1, 
float min2, 
float max2)
{
	// given a number x that exists in range [min1...max1], perform a linear mapping to range [min2...max2]
	float p = ( x - min1 ) / ( max1 - min1 );

	return min2 + ( (max2 - min2) * p );
}

/***************************************************************************/
/***************************************************************************/
#ifndef PS2
inline float Math::Min(
float x,
float y)
{
	if (x < y)
		return x;
	else
		return y;
}
#endif

/***************************************************************************/
/***************************************************************************/
#ifndef PS2
inline float Math::Max(
float x,
float y)
{
	if (x > y)
		return x;
	else
		return y;
}
#endif

/***************************************************************************/
/***************************************************************************/
#ifndef PS2
inline int Math::Min(
int x,
int y)
{
	if (x < y)
		return x;
	else
		return y;
}
#endif

/***************************************************************************/
/***************************************************************************/
#ifndef PS2
inline int Math::Max(
int x,
int y)
{
	if (x > y)
		return x;
	else
		return y;
}
#endif

/***************************************************************************/
/***************************************************************************/
inline unsigned int Math::Min(
unsigned int x,
unsigned int y)
{
	if (x < y)
		return x;
	else
		return y;
}

/***************************************************************************/
/***************************************************************************/
inline unsigned int Math::Max(
unsigned int x,
unsigned int y)
{
	if (x > y)
		return x;
	else
		return y;
}

/***************************************************************************/
/***************************************************************************/
inline long Math::Min(
long x,
long y)
{
	if (x < y)
		return x;
	else
		return y;
}

/***************************************************************************/
/***************************************************************************/
inline long Math::Max(
long x,
long y)
{
	if (x > y)
		return x;
	else
		return y;
}

/***************************************************************************/
/***************************************************************************/
inline unsigned long Math::Min(
unsigned long x,
unsigned long y)
{
	if (x < y)
		return x;
	else
		return y;
}

/***************************************************************************/
/***************************************************************************/
inline unsigned long Math::Max(
unsigned long x,
unsigned long y)
{
	if (x > y)
		return x;
	else
		return y;
}

/***************************************************************************/
// Nice functions for triples
/***************************************************************************/
inline float Math::Min(
float a, 
float b, 
float c)
{
	// Somebody code this for real
	return Min(Min(a, b), c);
}

/***************************************************************************/
// Nice functions for triples
/***************************************************************************/
inline float Math::Max(
float a, 
float b, 
float c)
{
	// Somebody code this for real
	return Max(Max(a, b), c);
}


#if 0
/***************************************************************************/
// Swaps
/***************************************************************************/
template <class T>
inline void Math::Swap(T &a, T &b)
{
	T temp = a;
	a = b;
	b = temp;
}
#endif

/***************************************************************************/
/***************************************************************************/
#ifndef PS2
inline float Math::Clamp(
float x,
float minimum,
float maximum)
{
	if (x < minimum)
		return minimum;
	else if (x > maximum)
		return maximum;
	else
		return x;
}
#endif

/***************************************************************************/
/***************************************************************************/
#ifndef PS2
inline int Math::Clamp(
int x,
int minimum,
int maximum)
{
	if (x < minimum)
		return minimum;
	else if (x > maximum)
		return maximum;
	else
		return x;
}
#endif

/***************************************************************************/
/***************************************************************************/
//inline bool Math::DivByZero64(float n, float d)
//{
//	u32 dExp = ((*(((u32*)&d) + 1)) >> 20) & 0x7FF;
//	if (dExp != 0)
//	{
//		u32 nExp = ((*(((u32*)&n) + 1)) >> 20) & 0x7FF;
//		return(nExp >= (0x3FF + dExp));
//	}
//	return true;
//}

/***************************************************************************/
/***************************************************************************/
inline bool Math::IsNaN(float f)
{
	// assumes unsigned int is 32 bits!
	return (((*(unsigned int*)&(f)) & 0x7F800000) == 0x7F800000);
}

/***************************************************************************/
/***************************************************************************/
//inline bool Math::IsNaN64(float f)
//{
//	return (((*(((u32*)&(f)) + 1)) & 0x7FF00000) == 0x7FF00000);
//}

/***************************************************************************/
/***************************************************************************/
inline unsigned char Math::UnsignedCompressToByte(float value, float maxValue)
{
	ASSERT((value >= 0.0f) && (value <= maxValue));
	if (value <= 0.0f)
		return(0);
	if (value >= maxValue)
		return(255);
	return((unsigned char)((value / maxValue) * 255.0f));
}

/***************************************************************************/
/***************************************************************************/
inline unsigned short Math::UnsignedCompressToWord(float value, float maxValue)
{
	ASSERT((value >= 0.0f) && (value <= maxValue));
	if (value <= 0.0f)
		return(0);
	if (value >= maxValue)
		return(65535);
	return((unsigned short)((value / maxValue) * 65535.0f));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::UnsignedDecompress(unsigned char value, float maxValue)
{
	ASSERT(maxValue > 0.0f);
	return(((float)value / 255.0f) * maxValue);
}

/***************************************************************************/
/***************************************************************************/
inline float Math::UnsignedDecompress(unsigned short value, float maxValue)
{
	ASSERT(maxValue > 0.0f);
	return(((float)value / 65535.0f) * maxValue);
}

/***************************************************************************/
/***************************************************************************/
inline signed char Math::SignedCompressToByte(float value, float maxValue)
{
	ASSERT((value >= -maxValue) && (value <= maxValue));
	if (value <= -maxValue)
		return(-127);
	if (value >= maxValue)
		return(127);
	return((signed char)((value / maxValue) * 127.0f));
}

/***************************************************************************/
/***************************************************************************/
inline short Math::SignedCompressToWord(float value, float maxValue)
{
	ASSERT((value >= -maxValue) && (value <= maxValue));
	if (value <= -maxValue)
		return(-32767);
	if (value >= maxValue)
		return(32767);
	return((short)((value / maxValue) * 32767.0f));
}

/***************************************************************************/
/***************************************************************************/
inline float Math::SignedDecompress(signed char value, float maxValue)
{
	return(((float)value / 127.0f) * maxValue);
}

/***************************************************************************/
/***************************************************************************/
inline float Math::SignedDecompress(short value, float maxValue)
{
	return(((float)value / 32767.0f) * maxValue);
}

/***************************************************************************/
/***************************************************************************/
inline void Math::MoveToValue( float *src, float val, float spd)
{
	if ( *src > val)
	{
		*src = *src - spd;
		if ( *src < val)
			*src = val;
	}
	else
	if ( *src < val)
	{
		*src = *src + spd;
		if ( *src > val)
			*src = val;
	}
}

inline void Math::MoveToValue( int *src, int val, int spd)
{
	if ( *src > val)
	{
		*src = *src - spd;
		if ( *src < val)
			*src = val;
	}
	else
	if ( *src < val)
	{
		*src = *src + spd;
		if ( *src > val)
			*src = val;
	}
}

inline void Math::MoveToValue( unsigned char *src, unsigned char val, char spd)
{
	if ( *src > val)
	{
		*src = *src - spd;
		if ( *src < val)
			*src = val;
	}
	else
	if ( *src < val)
	{
		*src = *src + spd;
		if ( *src > val)
			*src = val;
	}
}

/***************************************************************************/
/***************************************************************************/
inline void Math::MoveToZero( float *src, float spd)
{
	MoveToValue( src, 0.0f, spd);
}

inline void Math::MoveToZero( int *src, int spd)
{
	MoveToValue( src, 0, spd);
}

inline void Math::MoveToZero( unsigned char *src, char spd)
{
	MoveToValue( src, 0x00, spd);
}

