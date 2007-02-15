/***************************************************************************/
// A super-nice Vector class that is fast too.
/***************************************************************************/
#include "Math/MathClass.h"
#include "Math/MathStats.h"
#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

/***************************************************************************/
// assignment constructor
/***************************************************************************/
inline Vector2::Vector2(
float x,
float y)
{
	this->_x = x;
	this->_y = y;
}

/***************************************************************************/
// Clear to 0
/***************************************************************************/
inline void Vector2::Clear(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_CLEAR);
#endif //MATH_STATS

	_x = _y = 0.0f;
}

/***************************************************************************/
// Set the values
/***************************************************************************/
inline void Vector2::Set(
float x,
float y)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_SET_F);
#endif //MATH_STATS

	this->_x = x;
	this->_y = y;
}

/***************************************************************************/
// Set the values
/***************************************************************************/
inline void Vector2::Set(
const float *xyArray)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_SET_FARRAY);
#endif //MATH_STATS

	_x = xyArray[0];
	_y = xyArray[1];
}

/***************************************************************************/
// Return this Dot v
/***************************************************************************/
inline float Vector2::Dot(
const Vector2 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_DOT_V2);
#endif //MATH_STATS

	return(_x * v._x + _y * v._y);
}

/***************************************************************************/
// Return [this->x * v._x, this->y * v._y]
/***************************************************************************/
inline Vector2 Vector2::Diag(
const Vector2 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_DIAG_V2);
#endif //MATH_STATS

	Vector2 temp;
	temp._x = _x * v._x;
	temp._y = _y * v._y;
	return(temp);
}

/***************************************************************************/
// Return the Length of this Vector2
/***************************************************************************/
inline float Vector2::Length(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_LENGTH);
#endif //MATH_STATS

	return(Math::Sqrt(_x * _x + _y * _y));
}

/***************************************************************************/
// Return the Length squared of this vector
/***************************************************************************/
inline float Vector2::LengthSquared(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_LENGTH_SQUARED);
#endif //MATH_STATS

	return(_x * _x + _y * _y);
}

/***************************************************************************/
// Normalize the Vector2
/***************************************************************************/
inline void Vector2::Normalize(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_NORMALIZE);
#endif //MATH_STATS

	float invLength = 1.0f / Math::Sqrt(_x * _x + _y * _y);
	_x *= invLength;
	_y *= invLength;
}

/***************************************************************************/
// Return the normalized Vector
/***************************************************************************/
inline Vector2 Vector2::Normalized(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_NORMALIZED);
#endif //MATH_STATS

	float invLength = 1.0f / Math::Sqrt(_x * _x + _y * _y);
	Vector2 n;
	n._x = _x * invLength;
	n._y = _y * invLength;
	return(n);
}

/***************************************************************************/
// Negate the Vector2
/***************************************************************************/
inline void Vector2::Negate(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_NEGATE);
#endif //MATH_STATS

	_x = -_x;
	_y = -_y;
}

/***************************************************************************/
// Return v1 Dot v2
/***************************************************************************/
inline float Vector2::Dot(
const Vector2 &v1,
const Vector2 &v2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_DOT2_V2_V2);
#endif //MATH_STATS

	return(v1._x * v2._x + v1._y * v2._y);
}

/***************************************************************************/
// Return [v1._x * v2._x, v1._y * v2._y]
/***************************************************************************/
inline Vector2 Vector2::Diag(
const Vector2 &v1,
const Vector2 &v2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_DIAG_V2_V2);
#endif //MATH_STATS

	Vector2 temp;
	temp._x = v1._x * v2._x;
	temp._y = v1._y * v2._y;
	return(temp);
}

/***************************************************************************/
// Return the Length of this Vector2
/***************************************************************************/
inline float Vector2::Length(
const Vector2 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_LENGTH_V2);
#endif //MATH_STATS

	return(Math::Sqrt(v._x * v._x + v._y * v._y));
}

/***************************************************************************/
// Normalize the Vector2
/***************************************************************************/
inline void Vector2::Normalize(
Vector2 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_NORMALIZE_V2);
#endif //MATH_STATS

	float invLength = 1.0f / Math::Sqrt(v._x * v._x + v._y * v._y);
	v._x *= invLength;
	v._y *= invLength;
}

/***************************************************************************/
// Equality
/***************************************************************************/
inline int Vector2::operator==(
const Vector2 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_EQUALS_V2);
#endif //MATH_STATS

	return(_x == v._x && _y == v._y);
}

/***************************************************************************/
// Non-equality
/***************************************************************************/
inline int Vector2::operator!=(
const Vector2 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_NOT_EQUAL_V2);
#endif //MATH_STATS

	return(_x != v._x || _y != v._y);
}

/***************************************************************************/
// Assignment
/***************************************************************************/
//inline Vector2 &Vector2::operator=(
//xxxconst Vector2 &v)
//xxx{
//xxx	x = v._x;
//xxx	y = v._y;
//xxx	return(*this);
//xxx}

/***************************************************************************/
// Assignment
/***************************************************************************/
//inline Vector2 &Vector2::operator=(
//xxxconst float f)
//xxx{
//xxx	x = y = f;
//xxx	return(*this);
//xxx}

/***************************************************************************/
// Assignment
/***************************************************************************/
//inline Vector2 &Vector2::operator=(
//xxxint i)
//xxx{
//xxx	x = y = (float)i;
//xxx	return(*this);
//xxx}

/***************************************************************************/
// Addition
/***************************************************************************/
inline Vector2 &Vector2::operator+=(
const Vector2 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_PLUS_EQUALS_V2);
#endif //MATH_STATS

	_x += v._x;
	_y += v._y;

	return(*this);
}

/***************************************************************************/
// Subtraction
/***************************************************************************/
inline Vector2 &Vector2::operator-=(
const Vector2 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_MINUS_EQUALS_V2);
#endif //MATH_STATS

	_x -= v._x;
	_y -= v._y;

	return(*this);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector2 &Vector2::operator*=(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_TIMES_EQUALS_F);
#endif //MATH_STATS

	_x *= scale;
	_y *= scale;

	return(*this);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector2 &Vector2::operator/=(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_DIVIDE_EQUALS_F);
#endif //MATH_STATS

	float invScale = 1.0f / scale;
	_x *= invScale;
	_y *= invScale;

	return(*this);
}

/***************************************************************************/
// Addition
/***************************************************************************/
inline Vector2 Vector2::operator+(
const Vector2 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_PLUS_V2);
#endif //MATH_STATS

	Vector2 temp;
	temp._x = _x + v._x;
	temp._y = _y + v._y;
	return(temp);
}

/***************************************************************************/
// Subtraction
/***************************************************************************/
inline Vector2 Vector2::operator-(
const Vector2 &v) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_MINUS_V2);
#endif //MATH_STATS

	Vector2 temp;
	temp._x = _x - v._x;
	temp._y = _y - v._y;
	return(temp);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector2 Vector2::operator*(
float scale) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_TIMES_F);
#endif //MATH_STATS

	Vector2 temp;
	temp._x = _x * scale;
	temp._y = _y * scale;
	return(temp);
}

/***************************************************************************/
// Scaling
/***************************************************************************/
inline Vector2 Vector2::operator/(
float scale) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_DIVIDE_F);
#endif //MATH_STATS

	float invScale = 1.0f / scale;
	Vector2 temp;
	temp._x = _x * invScale;
	temp._y = _y * invScale;
	return(temp);
}

/***************************************************************************/
// Negation
/***************************************************************************/
inline Vector2 Vector2::operator-(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_NEGATE);
#endif //MATH_STATS

	Vector2 temp;
	temp._x = -_x;
	temp._y = -_y;
	return(temp);
}

/***************************************************************************/
/***************************************************************************/
inline Vector2::operator float*(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_CAST_F);
#endif //MATH_STATS

	return(&_x);
}

/***************************************************************************/
// Find the projection of v onto this
/***************************************************************************/
inline Vector2 Vector2::Projection(const Vector2 &v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR2_PROJECTION_V2);
#endif //MATH_STATS

	return(*this * Dot(v) / LengthSquared());
}

#ifdef GCN
/***************************************************************************/
// Byte-swapping for Gamecube
/***************************************************************************/
inline void Vector2::EndianSwap(void)
{
	ByteSwap(_x);
	ByteSwap(_y);
}
#endif

/***************** for later ******************/

//inline Vector2 &Vector2::Sum(Vector2 &v1, Vector2 &v2)
//{
//	x = v1._x + v2._x;
//	y = v1._y + v2._y;
//
//	return(*this);
//}

