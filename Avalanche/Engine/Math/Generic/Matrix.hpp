/***************************************************************************/
// Implementations for the matrix class
/***************************************************************************/
#include "Math/MathStats.h"

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 operator*(
float s,
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(F_TIMES_MATRIX3X3);
#endif //MATH_STATS

	Matrix3x3 result;

	result.e00 = m.e00 * s;
	result.e01 = m.e01 * s;
	result.e02 = m.e02 * s;

	result.e10 = m.e10 * s;
	result.e11 = m.e11 * s;
	result.e12 = m.e12 * s;

	result.e20 = m.e20 * s;
	result.e21 = m.e21 * s;
	result.e22 = m.e22 * s;

	return result;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 operator*(
float s,
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(F_TIMES_MATRIX3X3);
#endif //MATH_STATS

	Matrix3x3 result;

	result.e00 = m.e00 * s;
	result.e01 = m.e01 * s;
	result.e02 = m.e02 * s;

	result.e10 = m.e10 * s;
	result.e11 = m.e11 * s;
	result.e12 = m.e12 * s;

	result.e20 = m.e20 * s;
	result.e21 = m.e21 * s;
	result.e22 = m.e22 * s;

	return result;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 operator*(
float s,
const Matrix4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(F_TIMES_MATRIX4X4);
#endif //MATH_STATS

	Matrix4x4 result;

	result.e00 = m.e00 * s;
	result.e01 = m.e01 * s;
	result.e02 = m.e02 * s;
	result.e03 = m.e03 * s;

	result.e10 = m.e10 * s;
	result.e11 = m.e11 * s;
	result.e12 = m.e12 * s;
	result.e13 = m.e13 * s;

	result.e20 = m.e20 * s;
	result.e21 = m.e21 * s;
	result.e22 = m.e22 * s;
	result.e23 = m.e23 * s;

	result.e30 = m.e30 * s;
	result.e31 = m.e31 * s;
	result.e32 = m.e32 * s;
	result.e33 = m.e33 * s;

	return result;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 operator*(
float s,
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(F_TIMES_MATRIX4X4);
#endif //MATH_STATS

	Matrix4x4 result;

	result.e00 = m.e00 * s;
	result.e01 = m.e01 * s;
	result.e02 = m.e02 * s;
	result.e03 = m.e03 * s;

	result.e10 = m.e10 * s;
	result.e11 = m.e11 * s;
	result.e12 = m.e12 * s;
	result.e13 = m.e13 * s;

	result.e20 = m.e20 * s;
	result.e21 = m.e21 * s;
	result.e22 = m.e22 * s;
	result.e23 = m.e23 * s;

	result.e30 = m.e30 * s;
	result.e31 = m.e31 * s;
	result.e32 = m.e32 * s;
	result.e33 = m.e33 * s;

	return result;
}
/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 operator*(
float s,
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(F_TIMES_MATRIX4X4);
#endif //MATH_STATS

	Matrix4x4 result;

	result.e00 = m.e00 * s;
	result.e01 = m.e01 * s;
	result.e02 = m.e02 * s;
	result.e03 = m.e03 * s;

	result.e10 = m.e10 * s;
	result.e11 = m.e11 * s;
	result.e12 = m.e12 * s;
	result.e13 = m.e13 * s;

	result.e20 = m.e20 * s;
	result.e21 = m.e21 * s;
	result.e22 = m.e22 * s;
	result.e23 = m.e23 * s;

	result.e30 = m.e30 * s;
	result.e31 = m.e31 * s;
	result.e32 = m.e32 * s;
	result.e33 = m.e33 * s;

	return result;
}
