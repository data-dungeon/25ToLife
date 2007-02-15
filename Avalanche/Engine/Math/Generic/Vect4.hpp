/***************************************************************************/
// A super-nice Vector class that is fast too.
/***************************************************************************/
#include "Math/MathClass.h"
#include "Math/MathStats.h"
#include "Math/Mat4x4.h"
#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

/***************************************************************************/
// assignment constructor
/***************************************************************************/
//xxxinline Vector4::Vector4(
//xxxfloat x,
//xxxfloat y,
//xxxfloat z,
//xxxfloat w)
//xxx{
//xxx	this->x = x;
//xxx	this->y = y;
//xxx	this->z = z;
//xxx	this->w = w;
//xxx}

/***************************************************************************/
// Clear to 0
/***************************************************************************/
inline void Vector4::Clear(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_CLEAR);
#endif //MATH_STATS

	x = y = z = 0.0f;
	w = 1.0f;
}

/***************************************************************************/
// Set the values
/***************************************************************************/
inline void Vector4::Set(
float x,
float y,
float z,
float w)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_SET_F);
#endif //MATH_STATS

	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

/***************************************************************************/
// Set the values
/***************************************************************************/
inline void Vector4::Set(
const float *xyzwArray)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_SET_FARRAY);
#endif //MATH_STATS

	x = xyzwArray[0];
	y = xyzwArray[1];
	z = xyzwArray[2];
	w = xyzwArray[3];
}

/***************************************************************************/
// Set the values
/***************************************************************************/
inline void Vector4::Set(
const Vector3 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_SET_V3);
#endif //MATH_STATS

	x = v.x();
	y = v.y();
	z = v.z();
	w = 1.0f;
}

/***************************************************************************/
// Return this Dot v
/***************************************************************************/
inline float Vector4::Dot(
const Vector4 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_DOT_V4);
#endif //MATH_STATS

	return(x * v.x + y * v.y + z * v.z);
}

/***************************************************************************/
// Return this X v
/***************************************************************************/
inline Vector4 Vector4::Cross(
const Vector4 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_CROSS_V4);
#endif //MATH_STATS

	Vector4 temp;
	temp.x = y * v.z - v.y * z;
	temp.y = -(x * v.z - v.x * z);
	temp.z = x * v.y - v.x * y;
	temp.w = 1.0f;
	return(temp);
}


/***************************************************************************/
// Return [this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w]
/***************************************************************************/
inline Vector4 Vector4::Diag(
const Vector4 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_DIAG_V4);
#endif //MATH_STATS

	Vector4 temp;
	temp.x = x * v.x;
	temp.y = y * v.y;
	temp.z = z * v.z;
	temp.w = w * v.w;
	return(temp);
}

/***************************************************************************/
// Return the Length of this Vector4
/***************************************************************************/
inline float Vector4::Length(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_LENGTH);
#endif //MATH_STATS

	return(Math::Sqrt(x * x + y * y + z * z));
}

/***************************************************************************/
// Return the Length squared of this vector
/***************************************************************************/
inline float Vector4::LengthSquared(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_LENGTH_SQUARED);
#endif //MATH_STATS

	return(x * x + y * y + z * z);
}

/***************************************************************************/
// Normalize the Vector4
/***************************************************************************/
inline void Vector4::Normalize(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_NORMALIZE);
#endif //MATH_STATS

	float invLength = 1.0f / Math::Sqrt(x * x + y * y + z * z);
	x *= invLength;
	y *= invLength;
	z *= invLength;
}

/***************************************************************************/
// Return the normalized Vector
/***************************************************************************/
inline Vector4 Vector4::Normalized(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_NORMALIZED);
#endif //MATH_STATS

	float invLength = 1.0f / Math::Sqrt(x * x + y * y + z * z);
	Vector4 n;
	n.x = x * invLength;
	n.y = y * invLength;
	n.z = z * invLength;
	n.w = 1.0f;
	return(n);
}

/***************************************************************************/
// Negate the Vector4
/***************************************************************************/
inline void Vector4::Negate(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_NEGATE);
#endif //MATH_STATS

	x = -x;
	y = -y;
	z = -z;
}

/***************************************************************************/
// Return v1 Dot v2
/***************************************************************************/
inline float Vector4::Dot(
const Vector4 &v1,
const Vector4 &v2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_DOT_V4_V4);
#endif //MATH_STATS

	return(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

/***************************************************************************/
// Return v1 X v2
/***************************************************************************/
inline Vector4 Vector4::Cross(
const Vector4 &v1,
const Vector4 &v2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_CROSS_V4_V4);
#endif //MATH_STATS

	Vector4 temp;
	temp.x = v1.y * v2.z - v2.y * v1.z;
	temp.y = -(v1.x * v2.z - v2.x * v1.z);
	temp.z = v1.x * v2.y - v2.x * v1.y;
	temp.w = 1.0f;
	return(temp);
}

/***************************************************************************/
// Return [v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w]
/***************************************************************************/
inline Vector4 Vector4::Diag(
const Vector4 &v1,
const Vector4 &v2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_DIAG_V4_V4);
#endif //MATH_STATS

	Vector4 temp;
	temp.x = v1.x * v2.x;
	temp.y = v1.y * v2.y;
	temp.z = v1.z * v2.z;
	temp.w = v1.w * v2.w;
	return(temp);
}

/***************************************************************************/
// Return the Length of this Vector4
/***************************************************************************/
inline float Vector4::Length(
const Vector4 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_LENGTH_V4);
#endif //MATH_STATS

	return(Math::Sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

/***************************************************************************/
// Normalize the Vector4
/***************************************************************************/
inline void Vector4::Normalize(
Vector4 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_NORMALIZE_V4);
#endif //MATH_STATS

	float invLength = 1.0f / Math::Sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x *= invLength;
	v.y *= invLength;
	v.z *= invLength;
}

/***************************************************************************/
// Equality
/***************************************************************************/
inline int Vector4::operator==(
const Vector4 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_EQUALS_V4);
#endif //MATH_STATS

	return(x == v.x && y == v.y && z == v.z && w == v.w);
}

/***************************************************************************/
// Non-equality
/***************************************************************************/
inline int Vector4::operator!=(
const Vector4 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_NOT_EQUAL_V4);
#endif //MATH_STATS

	return(x != v.x || y != v.y || z != v.z || w != v.w);
}

/***************************************************************************/
// Assignment
/***************************************************************************/
//xxxinline Vector4 &Vector4::operator=(
//xxxconst Vector4 &v)
//xxx{
//xxx	x = v.x;
//xxx	y = v.y;
//xxx	z = v.z;
//xxx	w = v.w;
//xxx	return(*this);
//xxx}

/***************************************************************************/
/***************************************************************************/
//xxxinline Vector4 &Vector4::operator=(
//xxxconst Vector3 &v)
//xxx{
//xxx	x = v.x;
//xxx	y = v.y;
//xxx	z = v.z;
//xxx	w = 1.0f;
//xxx	return(*this);
//xxx}

/***************************************************************************/
// Assignment
/***************************************************************************/
//xxxinline Vector4 &Vector4::operator=(
//xxxconst float f)
//xxx{
//xxx	x = y = z = f;
//xxx	w = 1.0f;
//xxx	return(*this);
//xxx}

/***************************************************************************/
// Assignment
/***************************************************************************/
//xxxinline Vector4 &Vector4::operator=(
//xxxint i)
//xxx{
//xxx	x = y = z = (float)i;
//xxx	w = 1.0f;
//xxx	return(*this);
//xxx}

/***************************************************************************/
// Addition
/***************************************************************************/
inline Vector4 &Vector4::operator+=(
const Vector4 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_PLUS_EQUALS_V4);
#endif //MATH_STATS

	x += v.x;
	y += v.y;
	z += v.z;

	return(*this);
}

/***************************************************************************/
// Subtraction
/***************************************************************************/
inline Vector4 &Vector4::operator-=(
const Vector4 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_MINUS_EQUALS_V4);
#endif //MATH_STATS

	x -= v.x;
	y -= v.y;
	z -= v.z;

	return(*this);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector4 &Vector4::operator*=(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_TIMES_EQUALS_F);
#endif //MATH_STATS

	x *= scale;
	y *= scale;
	z *= scale;

	return(*this);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector4 &Vector4::operator/=(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_DIVIDE_EQUALS_F);
#endif //MATH_STATS

	float invScale = 1.0f / scale;

	x *= invScale;
	y *= invScale;
	z *= invScale;

	return(*this);
}

/***************************************************************************/
// Addition
/***************************************************************************/
inline Vector4 Vector4::operator+(
const Vector4 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_PLUS_V4);
#endif //MATH_STATS

	Vector4 temp;
	temp.x = x + v.x;
	temp.y = y + v.y;
	temp.z = z + v.z;
	temp.w = 1.0f;
	return(temp);
}

/***************************************************************************/
// Subtraction
/***************************************************************************/
inline Vector4 Vector4::operator-(
const Vector4 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_MINUS_F4);
#endif //MATH_STATS

	Vector4 temp;
	temp.x = x - v.x;
	temp.y = y - v.y;
	temp.z = z - v.z;
	temp.w = 1.0f;
	return(temp);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector4 Vector4::operator*(
float scale) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_TIMES_F);
#endif //MATH_STATS

	Vector4 temp;
	temp.x = x * scale;
	temp.y = y * scale;
	temp.z = z * scale;
	temp.w = 1.0f;
	return(temp);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector4 Vector4::operator/(
float scale) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_DIVIDE_F);
#endif //MATH_STATS

	float invScale = 1.0f / scale;
	Vector4 temp;
	temp.x = x * invScale;
	temp.y = y * invScale;
	temp.z = z * invScale;
	temp.w = 1.0f;
	return(temp);
}

/***************************************************************************/
// Negation
/***************************************************************************/
inline Vector4 Vector4::operator-(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_NEGATE);
#endif //MATH_STATS

	Vector4 temp;
	temp.x = -x;
	temp.y = -y;
	temp.z = -z;
	temp.w = w;
	return(temp);
}

/***************************************************************************/
// Mutiply, assuming w == 1 and v is [x y z 1]
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_VECTOR4_TIMES_GRAPHICS4X4	// compile control from vector.h for platform-specific implementation

inline Vector4 Vector4::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(V4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Vector4 result;

	result.x = x * m.r00 + y * m.r10 + z * m.r20 + m.tx;
	result.y = x * m.r01 + y * m.r11 + z * m.r21 + m.ty;
	result.z = x * m.r02 + y * m.r12 + z * m.r22 + m.tz;
	result.w = 1.0f;

	return result;
}

#endif //EXCLUDE_GENERIC_VECTOR4_TIMES_GRAPHICS4X4	// compile control from vector.h for platform-specific implementation

/***************************************************************************/
// Full multiply-- no assumption of w == 1
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_VECTOR4_TIMES_MATRIX4X4	// compile control from vector.h for platform-specific implementation

inline Vector4 Vector4::Transform(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(TRANSFORM_V4_M4X4);
#endif //MATH_STATS

	Vector4 result;

	result.x = x * m.e00 + y * m.e10 + z * m.e20 + w * m.e30;
	result.y = x * m.e01 + y * m.e11 + z * m.e21 + w * m.e31;
	result.z = x * m.e02 + y * m.e12 + z * m.e22 + w * m.e32;
	result.w = x * m.e03 + y * m.e13 + z * m.e23 + w * m.e33;

	return result;
}

#endif //EXCLUDE_GENERIC_VECTOR4_TIMES_MATRIX4X4	// compile control from vector.h for platform-specific implementation

/***************************************************************************/
// Basis multiply (ignores translation, like making a 3x3)
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_VECTOR4	// compile control from vector.h for platform-specific implementation

inline Vector4 Vector4::BasisTransform(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(BASIS_TRANSFORM_V4_G4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Vector4 result;

	result.x = x * m.r00 + y * m.r10 + z * m.r20;
	result.y = x * m.r01 + y * m.r11 + z * m.r21;
	result.z = x * m.r02 + y * m.r12 + z * m.r22;
	result.w = 1.0f;

	return result;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_VECTOR4	// compile control from vector.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/
inline Vector4 Vector4::InvBasisTransform(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(INV_BASIS_TRANSFORM_V4_G4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Vector4 result;

	result.x = x * m.r00 + y * m.r01 + z * m.r02;
	result.y = x * m.r10 + y * m.r11 + z * m.r12;
	result.z = x * m.r20 + y * m.r21 + z * m.r22;
	result.w = 1.0f;

	return result;
}

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_VECTOR4_FAST_MULTIPLY	// compile control from vector.h for platform-specific implementation

/*** NOTE: this routine is provided so that new platform can get up and running without implementing ***/
/*** a platform-specific version of this. this generic version is probably NOT fast!!!               ***/

inline Vector4 Vector4::FastMultiply(const Vector4 &v, const Graphics4x4 &m)	// this = v * m
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_FAST_MULTIPLY);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Vector4 product;

	product.x = v.x * m.r00 + v.y * m.r10 + v.z * m.r20 + m.tx;
	product.y = v.x * m.r01 + v.y * m.r11 + v.z * m.r21 + m.ty;
	product.z = v.x * m.r02 + v.y * m.r12 + v.z * m.r22 + m.tz;
	product.w = 1.0f;

	// copy it in
	*this = product;
	return *this;
}

#endif //EXCLUDE_GENERIC_VECTOR4_FAST_MULTIPLY	// compile control from vector.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_VECTOR4_FAST_TRANSFORM	// compile control from vector.h for platform-specific implementation

/*** NOTE: this routine is provided so that new platform can get up and running without implementing ***/
/*** a platform-specific version of this. this generic version is probably NOT fast!!!               ***/

inline Vector4 Vector4::FastTransform(const Vector4 &v, const Matrix4x4 &m)	// this = v * m
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_FAST_TRANSFORM);
#endif //MATH_STATS

	Vector4 product;

	product.x = v.x * m.e00 + v.y * m.e10 + v.z * m.e20 + v.w * m.e30;
	product.y = v.x * m.e01 + v.y * m.e11 + v.z * m.e21 + v.w * m.e31;
	product.z = v.x * m.e02 + v.y * m.e12 + v.z * m.e22 + v.w * m.e32;
	product.w = v.x * m.e03 + v.y * m.e13 + v.z * m.e23 + v.w * m.e33;

	// copy it in
	*this = product;
	return *this;
}

#endif //EXCLUDE_GENERIC_VECTOR4_FAST_TRANSFORM	// compile control from vector.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_VECTOR4_FAST_BASIS_TRANSFORM	// compile control from vector.h for platform-specific implementation

/*** NOTE: this routine is provided so that new platform can get up and running without implementing ***/
/*** a platform-specific version of this. this generic version is probably NOT fast!!!               ***/

inline Vector4 Vector4::FastBasisTransform(const Vector4 &v, const Graphics4x4 &m)	// this = v * m, no translation
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_FAST_BASIS_TRANSFORM);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Vector4 product;

	product.x = v.x * m.r00 + v.y * m.r10 + v.z * m.r20;
	product.y = v.x * m.r01 + v.y * m.r11 + v.z * m.r21;
	product.z = v.x * m.r02 + v.y * m.r12 + v.z * m.r22;
	product.w = 1.0f;

	// copy it in
	*this = product;
	return *this;
}

#endif //EXCLUDE_GENERIC_VECTOR4_FAST_BASIS_TRANSFORM	// compile control from vector.h for platform-specific implementation

/***************************************************************************/
//cast
/***************************************************************************/
inline Vector4::operator float*(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_CAST_F);
#endif //MATH_STATS

	return(&x);
}

/***************************************************************************/
// Gamecube byte swapper
/***************************************************************************/
#ifdef GCN
inline void Vector4::EndianSwap(void)
{
	ByteSwap(x);
	ByteSwap(y);
	ByteSwap(z);
	ByteSwap(w);
}
#endif

