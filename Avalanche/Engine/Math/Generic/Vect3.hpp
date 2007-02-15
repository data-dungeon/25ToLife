/***************************************************************************/
// A super-nice Vector class that is fast too.
/***************************************************************************/
#include "Math/MathClass.h"
#include "Math/MathStats.h"
#include "Math/Mat3x3.h"
#include "Math/Mat4x4.h"
#include "Math/Quatern.h"
#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

/***************************************************************************/
// assignment constructor
/***************************************************************************/
inline Vector3::Vector3(
float x,
float y,
float z)
{
	this->_x = x;
	this->_y = y;
	this->_z = z;
}

void Vector3::x(float x)
{
	_x = x;
}

void Vector3::y(float y)
{
	_y = y;
}

void Vector3::z(float z)
{
	_z = z;
}

float Vector3::x(void) const
{
	return _x;
}

float Vector3::y(void) const
{
	return _y;
}

float Vector3::z(void) const
{
	return _z;
}

inline Vector3CRef Vector3::operator=(Vector3CRef v)
{
	x(v.x());
	y(v.y());
	z(v.z());
}

inline Vector3::Vector3(const Vector3Unaligned& v)
{
	x(v.x());
	y(v.y());
	z(v.z());
}

void Vector3Unaligned::operator=(Vector3CRef v)
{
	x(v.x());
	y(v.y());
	z(v.z());
}

Vector3Unaligned::operator Vector3() const
{
	return Vector3(*this); 
}

/***************************************************************************/
// Clear to 0
/***************************************************************************/
inline void Vector3::Clear(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_CLEAR);
#endif //MATH_STATS

	_x = _y = _z = 0.0f;
}

/***************************************************************************/
// Set the values
/***************************************************************************/
inline void Vector3::Set(
float x,
float y,
float z)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_SET_F);
#endif //MATH_STATS

	_x = x;
	_y = y;
	_z = z;
}

/***************************************************************************/
// Set the values
/***************************************************************************/
inline void Vector3::Set(
const float *xyzArray)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_SET_FARRAY);
#endif //MATH_STATS

	_x = xyzArray[0];
	_y = xyzArray[1];
	_z = xyzArray[2];
}

/***************************************************************************/
// Set the values
/***************************************************************************/
inline void Vector3::Set(
const Vector4 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_SET_V4);
#endif //MATH_STATS

	_x = v._x;
	_y = v._y;
	_z = v._z;
}

/***************************************************************************/
// Set the values
/***************************************************************************/
inline void Set(Vector3CRef v)
{
	_x = v._x;
	_y = v._y;
	_z = v._z;
}

/***************************************************************************/
// Return this Dot v
/***************************************************************************/
inline float Vector3::Dot(
Vector3CRef v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_DOT_V3);
#endif //MATH_STATS

	return(_x * v._x + y * v._y + z * v._z);
}

/***************************************************************************/
// Return this X v
/***************************************************************************/
inline Vector3 Vector3::Cross(
Vector3CRef v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_CROSS_V3);
#endif //MATH_STATS

	Vector3 temp;
	temp._x = _y * v._z - v._y * _z;
	temp._y = -(_x * v._z - v._x * _z);
	temp._z = _x * v._y - v._x * _y;
	return(temp);
}

/***************************************************************************/
// Return [this->x * v._x, this->y * v._y, this->z * v._z]
/***************************************************************************/
inline Vector3 Vector3::Diag(
Vector3CRef v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_DIAG_V3);
#endif //MATH_STATS

	float tx = _x * v._x;
	float ty = _y * v._y;
	float tz = _z * v._z;
	return Vector3(tx, ty, tz);
}

/***************************************************************************/
// Return the Length of this Vector3
/***************************************************************************/
inline float Vector3::Length(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_LENGTH);
#endif //MATH_STATS

	return(Math::Sqrt(_x * _x + _y * _y + _z * _z));
}

/***************************************************************************/
// Return the Length of this Vector3's x & z components
/***************************************************************************/
inline float Vector3::LengthXZ(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_LENGTH_XZ);
#endif //MATH_STATS

	return(Math::Sqrt(_x * _x + _z * _z));
}


/***************************************************************************/
// Return the Length squared of this vector
/***************************************************************************/
inline float Vector3::LengthSquared(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_LENGTH_SQUARED);
#endif //MATH_STATS

	return(_x * _x + _y * _y + _z * _z);
}

/***************************************************************************/
// Return the Length squared of this vector
/***************************************************************************/
inline float Vector3::LengthXZSquared(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_LENGTH_XZ_SQUARED);
#endif //MATH_STATS

	return(_x * _x +  _z * _z);
}

/***************************************************************************/
// Normalize the Vector3
/***************************************************************************/
inline void Vector3::Normalize(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_NORMALIZE);
#endif //MATH_STATS

	float length = Length();
	ASSERT(!Math::DivByZero(1.0f, length));

	float invLength = 1.0f / length;
	operator*=(invLength);
}

/***************************************************************************/
// Normalize the Vector3
/***************************************************************************/
inline void Vector3::Normalize(float *length)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_NORMALIZE);
#endif //MATH_STATS

	*length = Math::Sqrt(x * x + y * y + z * z);
	ASSERT(!Math::DivByZero(1.0f, *length));

	float invLength = 1.0f / *length;
	x *= invLength;
	y *= invLength;
	z *= invLength;
}

/***************************************************************************/
// Safely Normalize the Vector3 (even when length is 0)
/***************************************************************************/
inline bool Vector3::SafeNormalize(void)
{
	float length = Length();

	if (Math::DivByZero(1.0f, length))
		return(false);

	float invLength = 1.0f / length;
	operator*=(invLength);

	return(true);
}

/***************************************************************************/
// Return the normalized Vector
/***************************************************************************/
inline Vector3 Vector3::Normalized(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_NORMALIZED);
#endif //MATH_STATS

	float length = Math::Sqrt(x * x + y * y + z * z);
	ASSERT(!Math::DivByZero(1.0f, length));

	float invLength = 1.0f / length;

	return operator*(invLength);
}

/***************************************************************************/
// Negate the Vector3
/***************************************************************************/
inline void Vector3::Negate(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_NEGATE);
#endif //MATH_STATS

	_x = -_x;
	_y = -_y;
	_z = -_z;
}

/***************************************************************************/
// Negate the Vector3
/***************************************************************************/
inline Vector3 Vector3::Negated(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_NEGATED);
#endif //MATH_STATS

	float nx = -_x;
	float ny = -_y;
	float nz = -_z;
	return Vector3(nx, ny, nz);
}

/***************************************************************************/
/***************************************************************************/
inline void Vector3::AbsInPlace(void)
{
	x = Math::Abs(x);
	y = Math::Abs(y);
	z = Math::Abs(z);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3 Vector3::Abs(void) const
{
	Vector3 abs = { Math::Abs(x), Math::Abs(y), Math::Abs(z) };
	return abs;
}

/***************************************************************************/
// Return v1 Dot v2
/***************************************************************************/
inline float Vector3::Dot(
Vector3CRef v1,
Vector3CRef v2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_DOT_V3_V3);
#endif //MATH_STATS

	return(v1._x * v2._x + v1._y * v2._y + v1._z * v2._z);
}

/***************************************************************************/
// Return v1 X v2
/***************************************************************************/
inline Vector3 Vector3::Cross(
Vector3CRef v1,
Vector3CRef v2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_CROSS_V3_V3);
#endif //MATH_STATS

	float tx = v1._y * v2._z - v2._y * v1._z;
	float ty = -(v1._x * v2._z - v2._x * v1._z);
	float tz = v1._x * v2._y - v2._x * v1._y;
	return Vector3(tx, ty, tz);
}

/***************************************************************************/
// Return [v1._x * v2._x, v1._y * v2._y, v1._z * v2._z]
/***************************************************************************/
inline Vector3 Vector3::Diag(
Vector3CRef v1,
Vector3CRef v2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_DIAG_V3_V3);
#endif //MATH_STATS

	float tx = v1._x * v2._x;
	float ty = v1._y * v2._y;
	float tz = v1._z * v2._z;
	return Vector3(tx, ty, tz);
}

/***************************************************************************/
// Return the Length of this Vector3
/***************************************************************************/
inline float Vector3::Length(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_LENGTH_V3);
#endif //MATH_STATS

	return(Math::Sqrt(v._x * v._x + v._y * v._y + v._z * v._z));
}

/***************************************************************************/
// Return the Length of this Vector3's x & z components
/***************************************************************************/
inline float Vector3::LengthXZ(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_LENGTHXZ_V3);
#endif //MATH_STATS

	return(Math::Sqrt(v._x * v._x + v._z * v._z));
}


//***************************************************************************/
// Return the Length of this Vector3
/***************************************************************************/
inline float Vector3::LengthSquared(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_LENGTH_SQUARED_V3);
#endif //MATH_STATS

	return(v._x * v._x + v._y * v._y + v._z * v._z);
}

/***************************************************************************/
// Return the Length of this Vector3's x & z components
/***************************************************************************/
inline float Vector3::LengthXZSquared(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_LENGTHXZ_SQUARED_V3);
#endif //MATH_STATS

	return(v._x * v._x + v._z * v._z);
}

/***************************************************************************/
// Normalize the Vector3
/***************************************************************************/
inline void Vector3::Normalize(
Vector3 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_NORMALIZE_V3);
#endif //MATH_STATS

	float length = Math::Sqrt(v._x * v._x + v._y * v._y + v._z * v._z);
	ASSERT(!Math::DivByZero(1.0f, length));

	float invLength = 1.0f / length;
	v._x *= invLength;
	v._y *= invLength;
	v._z *= invLength;
}

/***************************************************************************/
// Safely Normalize the Vector3 (even when length is 0)
/***************************************************************************/
inline bool Vector3::SafeNormalize(
Vector3 &v)
{
	float length = Math::Sqrt(v._x * v._x + v._y * v._y + v._z * v._z);

	if (Math::DivByZero(1.0f, length))
		return(false);

	float invLength = 1.0f / length;
	v._x *= invLength;
	v._y *= invLength;
	v._z *= invLength;

	return(true);
}

/***************************************************************************/
// Equality
/***************************************************************************/
inline int Vector3::operator==(
Vector3CRef v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_EQUALS_V3);
#endif //MATH_STATS

	return(_x == v._x && _y == v._y && _z == v._z);
}

/***************************************************************************/
// Non-equality
/***************************************************************************/
inline int Vector3::operator!=(
Vector3CRef v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_NOT_EQUAL_V3);
#endif //MATH_STATS

	return(_x != v._x || _y != v._y || _z != v._z);
}

/***************************************************************************/
// Addition
/***************************************************************************/
inline Vector3 &Vector3::operator+=(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_PLUS_EQUALS_V3);
#endif //MATH_STATS

	_x += v._x;
	_y += v._y;
	_z += v._z;

	return(*this);
}

/***************************************************************************/
// Subtraction
/***************************************************************************/
inline Vector3 &Vector3::operator-=(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_MINUS_EQUALS_V3);
#endif //MATH_STATS

	_x -= v._x;
	_y -= v._y;
	_z -= v._z;

	return(*this);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector3 &Vector3::operator*=(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_TIMES_EQUALS_F);
#endif //MATH_STATS

	_x *= scale;
	_y *= scale;
	_z *= scale;

	return(*this);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector3 &Vector3::operator/=(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_DIVIDE_EQUALS_F);
#endif //MATH_STATS

	float invScale = 1.0f / scale;

	_x *= invScale;
	_y *= invScale;
	_z *= invScale;

	return(*this);
}

/***************************************************************************/
// Addition
/***************************************************************************/
inline Vector3 Vector3::operator+(
Vector3CRef  __restrict__ v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_PLUS_V3);
#endif //MATH_STATS

	float tx = _x + v._x;
	float ty = _y + v._y;
	float tz = _z + v._z;
	return Vector3(tx, ty, tz);
}

/***************************************************************************/
// Subtraction
/***************************************************************************/
inline Vector3 Vector3::operator-(
Vector3CRef v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_MINUS_V3);
#endif //MATH_STATS

	float tx = _x - v._x;
	float ty = _y - v._y;
	float tz = _z - v._z;
	return Vector3(tx, ty, tz);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector3 Vector3::operator*(
float scale) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_TIMES_F);
#endif //MATH_STATS

	float tx = _x * scale;
	float ty = _y * scale;
	float tz = _z * scale;
	return Vector3(tx, ty, tz);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector3 Vector3::operator/(
float scale) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_DIVIDE_F);
#endif //MATH_STATS

	float invScale = 1.0f / scale;

	float tx = _x * invScale;
	float ty = _y * invScale;
	float tz = _z * invScale;
	return Vector3(tx, ty, tz);
}

/***************************************************************************/
// Negation
/***************************************************************************/
inline Vector3 Vector3::operator-(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_NEGATE);
#endif //MATH_STATS

	float tx = -_x;
	float ty = -_y;
	float tz = -_z;
	return Vector3(tx, ty, tz);
}

/***************************************************************************/
// multiply
/***************************************************************************/
inline Vector3 Vector3::operator*(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_TIMES_M3X3);
#endif //MATH_STATS

	float rx = _x * m.e00 + _y * m.e10 + _z * m.e20;
	float ry = _x * m.e01 + _y * m.e11 + _z * m.e21;
	float rz = _x * m.e02 + _y * m.e12 + _z * m.e22;

	return Vector3(rx, ry, rz);
}

/***************************************************************************/
// multiply by transpose
/***************************************************************************/
inline Vector3 Vector3::operator/(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_DIVIDE_D3X3);
#endif //MATH_STATS

	float rx = _x * m.e00 + _y * m.e01 + _z * m.e02;
	float ry = _x * m.e10 + _y * m.e11 + _z * m.e12;
	float rz = _x * m.e20 + _y * m.e21 + _z * m.e22;

	return Vector3(rx, ry, rz);
}

/***************************************************************************/
// Mutiply, assuming w == 1 and v is [x y z 1]
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_VECTOR3_TIMES_GRAPHICS4X4	// compile control from vector.h for platform-specific implementation

inline Vector3 Vector3::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float rx = _x * m.r00 + _y * m.r10 + _z * m.r20 + m.tx;
	float ry = _x * m.r01 + _y * m.r11 + _z * m.r21 + m.ty;
	float rz = _x * m.r02 + _y * m.r12 + _z * m.r22 + m.tz;

	return Vector3(rx, ry, rz);
}

#endif //EXCLUDE_GENERIC_VECTOR3_TIMES_GRAPHICS4X4	// compile control from vector.h for platform-specific implementation

/***************************************************************************/
// Mutiply by inverse, assuming w == 1 and v is [x y z 1], only rotation and translation
/***************************************************************************/
inline Vector3 Vector3::operator/(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_DIVIDE_G4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float dx = _x - m.tx;
	float dy = _y - m.ty;
	float dz = _z - m.tz;

	float rx = dx * m.r00 + dy * m.r01 + dz * m.r02;
	float ry = dx * m.r10 + dy * m.r11 + dz * m.r12;
	float rz = dx * m.r20 + dy * m.r21 + dz * m.r22;

	return Vector3(rx, ry, rz);
}

/***************************************************************************/
// Multiply by a unit quaternion, representing a rotation
/***************************************************************************/
inline Vector3 Vector3::operator*(const ts_Quatvw &q) const
{
	Vector3 result;

	qatVectMul( &q, this, &result );

	return( result );
}

/***************************************************************************/
// Multiply by the transpose of a unit quaternion
/***************************************************************************/
inline Vector3 Vector3::operator/(const ts_Quatvw &q) const
{
	Vector3 result;
	ts_Quatvw inverse;

	inverse = q;
	inverse.W *= -1.0f;

	qatVectMul( &q, this, &result );

	return( result );
}


inline Vector3 Vector3::operator-(float scalar) const
{
	return Vector3(_x-scalar, _y-scalar, _z-scalar);
}

inline Vector3 Vector3::operator+(float scalar) const
{
	return Vector3(_x+scalar, _y+scalar, _z+scalar);
}

inline Vector3 Vector3::operator-=(float scalar)
{
	*this = *this - scalar;
	return *this;
}

inline Vector3 Vector3::operator+=(float scalar)
{
	*this = *this + scalar;
	return *this;
}

// piecewise min & max...
inline Vector3 Vector3::Min(Vector3CRef v1, Vector3CRef v2)
{
	return Vector3(
		Math::Min(v1._x, v2._x),
		Math::Min(v1._y, v2._y),
		Math::Min(v1._z, v2._z));
}

inline Vector3 Vector3::Max(Vector3CRef v1, Vector3CRef v2)
{
	return Vector3(
		Math::Max(v1._x, v2._x),
		Math::Max(v1._y, v2._y),
		Math::Max(v1._z, v2._z));
}

inline Vector3 Vector3::Abs(Vector3CRef v) 
{
	return Vector3(
		Math::Abs(v._x),
		Math::Abs(v._y),
		Math::Abs(v._z));
}

inline float Vector3::SumXYZ(Vector3CRef v)
{
	return v._x + v._y + v._z;
}


/***************************************************************************/
// Full graphics [x y z 1] multiply
/***************************************************************************/
inline Vector4 Vector3::Transform3(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(TRANSFORM3_V3_M4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Vector4 result;

	result.x = _x * m.e00 + _y * m.e10 + _z * m.e20 + m.e30;
	result.y = _x * m.e01 + _y * m.e11 + _z * m.e21 + m.e31;
	result.z = _x * m.e02 + _y * m.e12 + _z * m.e22 + m.e32;
	result.w = _x * m.e03 + _y * m.e13 + _z * m.e23 + m.e33;

	// Assumpiton based on affine transromations...which we won't do
	//result.w = m.e[15];

	return result;
}

/***************************************************************************/
// Basis multiply (ignores translation, like making a 3x3)
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_VECTOR3	// compile control from vector.h for platform-specific implementation

inline Vector3 Vector3::BasisTransform(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(BASIS_TRANSFORM_V3_G4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float rx = _x * m.r00 + _y * m.r10 + _z * m.r20;
	float ry = _x * m.r01 + _y * m.r11 + _z * m.r21;
	float rz = _x * m.r02 + _y * m.r12 + _z * m.r22;

	return Vector3(rx, ry, rz);
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_VECTOR3	// compile control from vector.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/
inline Vector3 Vector3::InvBasisTransform(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(INV_BASIS_TRANSFORM_V3_G4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float rx = _x * m.r00 + _y * m.r01 + _z * m.r02;
	float ry = _x * m.r10 + _y * m.r11 + _z * m.r12;
	float rz = _x * m.r20 + _y * m.r21 + _z * m.r22;

	return Vector3(rx, ry, rz);
}

/***************************************************************************/
// fast multiply, assume w == 1
//*** NOTE: this routine is provided so that new platform can get up and
//*** running without implementing a platform-specific version of this.
//*** this generic version is probably NOT fast!!!
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_VECTOR3_FAST_MULTIPLY	// compile control from vector.h for platform-specific implementation

inline Vector3 Vector3::FastMultiply(Vector3CRef v, const Graphics4x4 &m)	// this = v * m
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_FAST_MULTIPLY);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float productx = v._x * m.r00 + v._y * m.r10 + v._z * m.r20 + m.tx;
	float producty = v._x * m.r01 + v._y * m.r11 + v._z * m.r21 + m.ty;
	float productz = v._x * m.r02 + v._y * m.r12 + v._z * m.r22 + m.tz;

	// copy it in
	_x = productx;
	_y = producty;
	_z = productz;
	return *this;
}

#endif //EXCLUDE_GENERIC_VECTOR3_FAST_MULTIPLY	// compile control from vector.h for platform-specific implementation

/***************************************************************************/
// fast basis multiply (ignores translation, like making a 3x3)
//*** NOTE: this routine is provided so that new platform can get up and
//*** running without implementing a platform-specific version of this.
//*** this generic version is probably NOT fast!!!
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_VECTOR3_FAST_BASIS_TRANSFORM	// compile control from vector.h for platform-specific implementation

inline Vector3 Vector3::FastBasisTransform(Vector3CRef v, const Graphics4x4 &m)	// this = v * m, no translation
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_FAST_BASIS_TRANSFORM);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4


	float productx = v._x * m.r00 + v._y * m.r10 + v._z * m.r20;
	float producty = v._x * m.r01 + v._y * m.r11 + v._z * m.r21;
	float productz = v._x * m.r02 + v._y * m.r12 + v._z * m.r22;

	// copy it in
	_x = productx;
	_y = producty;
	_z = productz;
	return *this;
}

#endif //EXCLUDE_GENERIC_VECTOR3_FAST_BASIS_TRANSFORM	// compile control from vector.h for platform-specific implementation

/***************************************************************************/
//cast
/***************************************************************************/
inline Vector3::operator float*(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_CAST_F);
#endif //MATH_STATS

	return(&x);
}

/***************************************************************************/
// Gamecube byte swapper
/***************************************************************************/
#ifdef GCN
inline void Vector3::EndianSwap(void)
{
	ByteSwap(_x);
	ByteSwap(_y);
	ByteSwap(_z);
}
#endif

/***************************************************************************/
// Check for NaNs.
/***************************************************************************/
inline bool Vector3::IsNaN(void) const
{
	return (Math::IsNaN(_x) || Math::IsNaN(_y) || Math::IsNaN(_z));
}

/***************************************************************************/
// Find the projection of v onto this
/***************************************************************************/
inline Vector3 Vector3::Projection(Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_PROJECTION_V3);
#endif //MATH_STATS

	return(*this * Dot(v) / LengthSquared());
}

/***************************************************************************/
// Find the projection of b onto a
/***************************************************************************/
inline Vector3 Vector3::Projection(
Vector3CRef a,
Vector3CRef b)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR3_PROJECTION_V3_V3);
#endif //MATH_STATS

	return(a * Vector3::Dot(a, b) / Vector3::Dot(a, a));
}

