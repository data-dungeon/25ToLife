//////////////////////////////////////////////////////////////////////////////
//
// Interpolate.h
//
// Functions for interpolation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef INTERPOLATE_H
#define INTERPOLATE_H

//////////////////////////////////////////////////////////////////////////////
// Includes
//////////////////////////////////////////////////////////////////////////////

#include "Math/Vector.h"


//////////////////////////////////////////////////////////////////////////////
// Classes
//////////////////////////////////////////////////////////////////////////////

class Interpolate
{
public:

	// interpolate from a (at t == 0) to b (at t == 1) linearly.  expects 
    // (but does not require) t to be between 0 and 1.
	static inline float Linear( float a, float b, float t );

	// same as above, but for each component of the vector.
	static inline Vector2 Linear( const Vector2& a, const Vector2& b, float t );
	static inline Vector3 Linear( const Vector3& a, const Vector3& b, float t );
	static inline Vector4 Linear( const Vector4& a, const Vector4& b, float t );

	// interpolate from a (at t == 0) to b (at t == 1) using a cubic (hermite)
    // where the initial tangent and final tangent are zero.  expects (but 
    // does not require) t to be between 0 and 1.
	static inline float Cubic( float a, float b, float t );
	static inline Vector2 Cubic( const Vector2& a, const Vector2& b, float t );
	static inline Vector3 Cubic( const Vector3& a, const Vector3& b, float t );
	static inline Vector4 Cubic( const Vector4& a, const Vector4& b, float t );

	// interpolate from a and aTangent (at t == 0) to b and bTangent (at 
    // t == 1) using a cubic formula.  expects (but does not require) t to be 
    // between 0 and 1.  (ex: tangents could be an initial and final 
    // velocity.)
	static inline float Cubic( float a, float b, float aTangent, float bTangent, float t );
	static inline Vector2 Cubic( const Vector2& a, const Vector2& b, const Vector2& aTangent, const Vector2& bTangent, float t );
	static inline Vector3 Cubic( const Vector3& a, const Vector3& b, const Vector3& aTangent, const Vector3& bTangent, float t );
	static inline Vector4 Cubic( const Vector4& a, const Vector4& b, const Vector4& aTangent, const Vector4& bTangent, float t );

	// get the derivative (rate of change/tangent) of the cubic path at time t.
	static inline float CubicDerivative( float a, float b, float aTangent, float bTangent, float t );
	static inline Vector2 CubicDerivative( const Vector2& a, const Vector2& b, const Vector2& aTangent, const Vector2& bTangent, float t );
	static inline Vector3 CubicDerivative( const Vector3& a, const Vector3& b, const Vector3& aTangent, const Vector3& bTangent, float t );
	static inline Vector4 CubicDerivative( const Vector4& a, const Vector4& b, const Vector4& aTangent, const Vector4& bTangent, float t );

	// bilinear interpolate between w00, w01, w10, w11
	// expects (but does not require) u, v to be between 0 and 1.
	//
	// w01^           . w11
	//    |
	//    |
	//    |    . u,v
	//    |
	//    |
	//	w00+-----------> w10
	static inline float Bilinear( float w00, float w10, float w11, float w01, float u, float v );

	// do the same as above, but normalize x/y using grid size.
	static inline float Bilinear( float w00, float w10, float w11, float w01, float x, float y, float grid );

private:

	// don't allow construction.
	Interpolate();
	~Interpolate();

};

//////////////////////////////////////////////////////////////////////////////

inline float Interpolate::Linear( float a, float b, float t )
{
	return a + ( b - a ) * t;

	// or, possibly more clear, but slower(?):
	//return a * ( 1.0f - t ) + b * t;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector2 Interpolate::Linear( const Vector2& a, const Vector2& b, float t )
{
	Vector2 v;

	v.Set( Linear( a.x(), b.x(), t ),
			 Linear( a.y(), b.y(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector3 Interpolate::Linear( const Vector3& a, const Vector3& b, float t )
{
	Vector3 v;

	v.Set( Linear( a.x(), b.x(), t ),
			 Linear( a.y(), b.y(), t ),
			 Linear( a.z(), b.z(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector4 Interpolate::Linear( const Vector4& a, const Vector4& b, float t )
{
	Vector4 v;

	v.Set( Linear( a.x(), b.x(), t ),
			 Linear( a.y(), b.y(), t ),
			 Linear( a.z(), b.z(), t ),
			 Linear( a.w(), b.w(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline float Interpolate::Cubic( float a, float b, float t )
{
	// simplified version of cubic interpolation where initial and final tangents are zero.
	return a + ( b - a ) * ( 3.0f - 2.0f * t ) * t * t;
}

//////////////////////////////////////////////////////////////////////////////

inline float Interpolate::Cubic( float a, float b, float aTangent, float bTangent, float t )
{
	float A = (      aTangent    ) + bTangent + ( -2.0f * ( b - a ) );
	float B = ( -2.0f * aTangent ) - bTangent + (  3.0f * ( b - a ) );
	float C = aTangent;
	float D = a;
	return ( ( ( A * t ) + B ) * t + C ) * t + D;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector2 Interpolate::Cubic( const Vector2& a, const Vector2& b, float t )
{
	Vector2 v;

	v.Set( Cubic( a.x(), b.x(), t ),
			 Cubic( a.y(), b.y(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector2 Interpolate::Cubic( const Vector2& a, const Vector2& b, const Vector2& aTangent, const Vector2& bTangent, float t )
{
	Vector2 v;

	v.Set( Cubic( a.x(), b.x(), aTangent.x(), bTangent.x(), t ),
			 Cubic( a.y(), b.y(), aTangent.y(), bTangent.y(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector3 Interpolate::Cubic( const Vector3& a, const Vector3& b, float t )
{
	Vector3 v;

	v.Set( Cubic( a.x(), b.x(), t ),
			 Cubic( a.y(), b.y(), t ),
			 Cubic( a.z(), b.z(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector3 Interpolate::Cubic( const Vector3& a, const Vector3& b, const Vector3& aTangent, const Vector3& bTangent, float t )
{
	Vector3 v;

	v.Set(Cubic( a.x(), b.x(), aTangent.x(), bTangent.x(), t ),
			Cubic( a.y(), b.y(), aTangent.y(), bTangent.y(), t ),
			Cubic( a.z(), b.z(), aTangent.z(), bTangent.z(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector4 Interpolate::Cubic( const Vector4& a, const Vector4& b, float t )
{
	Vector4 v;

	v.Set( Cubic( a.x(), b.x(), t ),
			 Cubic( a.y(), b.y(), t ),
			 Cubic( a.z(), b.z(), t ),
			 Cubic( a.w(), b.w(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector4 Interpolate::Cubic( const Vector4& a, const Vector4& b, const Vector4& aTangent, const Vector4& bTangent, float t )
{
	Vector4 v;

	v.Set(Cubic( a.x(), b.x(), aTangent.x(), bTangent.x(), t ),
			Cubic( a.y(), b.y(), aTangent.y(), bTangent.y(), t ),
			Cubic( a.z(), b.z(), aTangent.z(), bTangent.z(), t ),
			Cubic( a.w(), b.w(), aTangent.w(), bTangent.w(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline float Interpolate::CubicDerivative( float a, float b, float aTangent, float bTangent, float t )
{
	float A = (      aTangent    ) + bTangent + ( -2.0f * ( b - a ) );
	float B = ( -2.0f * aTangent ) - bTangent + (  3.0f * ( b - a ) );
	float C = aTangent;
	return ( ( 3.0f * A * t ) + 2.0f * B ) * t + C;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector2 Interpolate::CubicDerivative( const Vector2& a, const Vector2& b, const Vector2& aTangent, const Vector2& bTangent, float t )
{
	Vector2 v;

	v.Set( CubicDerivative( a.x(), b.x(), aTangent.x(), bTangent.x(), t ),
			 CubicDerivative( a.y(), b.y(), aTangent.y(), bTangent.y(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector3 Interpolate::CubicDerivative( const Vector3& a, const Vector3& b, const Vector3& aTangent, const Vector3& bTangent, float t )
{
	Vector3 v;

	v.Set(CubicDerivative( a.x(), b.x(), aTangent.x(), bTangent.x(), t ),
			CubicDerivative( a.y(), b.y(), aTangent.y(), bTangent.y(), t ),
			CubicDerivative( a.z(), b.z(), aTangent.z(), bTangent.z(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline Vector4 Interpolate::CubicDerivative( const Vector4& a, const Vector4& b, const Vector4& aTangent, const Vector4& bTangent, float t )
{
	Vector4 v;

	v.Set(CubicDerivative( a.x(), b.x(), aTangent.x(), bTangent.x(), t ),
			CubicDerivative( a.y(), b.y(), aTangent.y(), bTangent.y(), t ),
			CubicDerivative( a.z(), b.z(), aTangent.z(), bTangent.z(), t ),
			CubicDerivative( a.w(), b.w(), aTangent.w(), bTangent.w(), t ));

	return v;
}

//////////////////////////////////////////////////////////////////////////////

inline float Interpolate::Bilinear( float w00, float w10, float w11, float w01, float u, float v )
{
	float i1 = w00 + ( w10 - w00 ) * u;
	float i2 = w01 + ( w11 - w01 ) * u;

	float w = i1 + ( i2 - i1 ) * v;
	return w;
}

//////////////////////////////////////////////////////////////////////////////

inline float Interpolate::Bilinear( float w00, float w10, float w11, float w01, float x, float y, float grid )
{
	float u = Math::Mod( x, grid );
	float v = Math::Mod( y, grid );

	return Bilinear( u, v, w00, w10, w11, w01 );
}

//////////////////////////////////////////////////////////////////////////////

#endif // INTERPOLATE_H
