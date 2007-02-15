/***************************************************************************/
// Implementations for the matrix class
/***************************************************************************/
#include "platform/BaseType.h"
#include "Math/MathClass.h"
#include "Math/Det.h"
#include "Math/MathStats.h"

/***************************************************************************/
// Set the matrix
/***************************************************************************/
inline Matrix4x4::Matrix4x4(
float e00,
float e01,
float e02,
float e03,
float e10,
float e11,
float e12,
float e13,
float e20,
float e21,
float e22,
float e23,
float e30,
float e31,
float e32,
float e33)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_CONSTRUCT_F);
#endif //MATH_STATS

	this->e00 = e00;
	this->e01 = e01;
	this->e02 = e02;
	this->e03 = e03;

	this->e10 = e10;
	this->e11 = e11;
	this->e12 = e12;
	this->e13 = e13;

	this->e20 = e20;
	this->e21 = e21;
	this->e22 = e22;
	this->e23 = e23;

	this->e30 = e30;
	this->e31 = e31;
	this->e32 = e32;
	this->e33 = e33;
}

/***************************************************************************/
// Assign the matrix
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator=(
const Matrix4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_ASSIGN_M4X4);
#endif //MATH_STATS

	e00 = m.e00;
	e01 = m.e01;
	e02 = m.e02;
	e03 = m.e03;

	e10 = m.e10;
	e11 = m.e11;
	e12 = m.e12;
	e13 = m.e13;

	e20 = m.e20;
	e21 = m.e21;
	e22 = m.e22;
	e23 = m.e23;

	e30 = m.e30;
	e31 = m.e31;
	e32 = m.e32;
	e33 = m.e33;

	return *this;
}

/***************************************************************************/
// Assign the matrix
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_ASSIGN_M4X4);
#endif //MATH_STATS

	e00 = m.r00;
	e01 = m.r01;
	e02 = m.r02;
	e03 = m.u0;

	e10 = m.r10;
	e11 = m.r11;
	e12 = m.r12;
	e13 = m.u1;

	e20 = m.r20;
	e21 = m.r21;
	e22 = m.r22;
	e23 = m.u2;

	e30 = m.tx;
	e31 = m.ty;
	e32 = m.tz;
	e33 = m.w;

	return *this;
}

/***************************************************************************/
// Assign the matrix
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_ASSIGN_M4X4);
#endif //MATH_STATS

	e00 = m.r00;
	e01 = m.r01;
	e02 = m.r02;
	e03 = m.u0;

	e10 = m.r10;
	e11 = m.r11;
	e12 = m.r12;
	e13 = m.u1;

	e20 = m.r20;
	e21 = m.r21;
	e22 = m.r22;
	e23 = m.u2;

	e30 = m.tx;
	e31 = m.ty;
	e32 = m.tz;
	e33 = m.w;

	return *this;
}

/***************************************************************************/
// Set the matrix
/***************************************************************************/
inline void Matrix4x4::Set(
float e00,
float e01,
float e02,
float e03,
float e10,
float e11,
float e12,
float e13,
float e20,
float e21,
float e22,
float e23,
float e30,
float e31,
float e32,
float e33)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_F);
#endif //MATH_STATS

	this->e00 = e00;
	this->e01 = e01;
	this->e02 = e02;
	this->e03 = e03;

	this->e10 = e10;
	this->e11 = e11;
	this->e12 = e12;
	this->e13 = e13;

	this->e20 = e20;
	this->e21 = e21;
	this->e22 = e22;
	this->e23 = e23;

	this->e30 = e30;
	this->e31 = e31;
	this->e32 = e32;
	this->e33 = e33;
}

/***************************************************************************/
// Set to identity
/***************************************************************************/
#ifndef EXCLUDE_GENERIC_MAT4X4_IDENTITY	// compile control from matrix.h for platform-specific implementation
inline void Matrix4x4::Identity(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_IDENTITY);
#endif //MATH_STATS

	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f;
	e03 = 0.0f;

	e10 = 0.0f;
	e11 = 1.0f;
	e12 = 0.0f;
	e13 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
	e23 = 0.0f;

	e30 = 0.0f;
	e31 = 0.0f;
	e32 = 0.0f;
	e33 = 1.0f;
}
#endif // EXCLUDE_GENERIC_MAT4X4_IDENTITY

inline bool Matrix4x4::IsIdentity(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_IS_IDENTITY);
#endif //MATH_STATS

	return((e00 == 1.0f) &&
			 (e01 == 0.0f) &&
			 (e02 == 0.0f) &&
			 (e03 == 0.0f) &&

			 (e10 == 0.0f) &&
			 (e11 == 1.0f) &&
			 (e12 == 0.0f) &&
			 (e13 == 0.0f) &&

			 (e20 == 0.0f) &&
			 (e21 == 0.0f) &&
			 (e22 == 1.0f) &&
			 (e23 == 0.0f) &&

			 (e30 == 0.0f) &&
			 (e31 == 0.0f) &&
			 (e32 == 0.0f) &&
			 (e33 == 1.0f));
}

/***************************************************************************/
// Invert (returns false if not invertable)
/***************************************************************************/
inline bool Matrix4x4::Invert(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_INVERT);
#endif //MATH_STATS

	// invert it
	Matrix4x4 inverse;
	if (!Invert(inverse))
		return false;

	// save it
	*this = inverse;
	return true;
}

/***************************************************************************/
// Transpose the matrix
/***************************************************************************/
inline void Matrix4x4::Transpose(
Matrix4x4 &transpose) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TRANSPOSE_M4X4);
#endif //MATH_STATS

	transpose.e00 = e00;
	transpose.e01 = e10;
	transpose.e02 = e20;
	transpose.e03 = e30;

	transpose.e10 = e01;
	transpose.e11 = e11;
	transpose.e12 = e21;
	transpose.e13 = e31;

	transpose.e20 = e02;
	transpose.e21 = e12;
	transpose.e22 = e22;
	transpose.e23 = e32;

	transpose.e30 = e03;
	transpose.e31 = e13;
	transpose.e32 = e23;
	transpose.e33 = e33;
}

/***************************************************************************/
// Transpose the matrix
/***************************************************************************/
inline void Matrix4x4::Transpose(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TRANSPOSE);
#endif //MATH_STATS

	Matrix4x4 t;

	Transpose(t);
	*this = t;
}

/***************************************************************************/
// Static for the determinant
/***************************************************************************/
inline float Matrix4x4::Det(
const Matrix4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DET_M4X4);
#endif //MATH_STATS

	return(Determinant::Calc(m.e00, m.e01, m.e02, m.e03,
									m.e10, m.e11, m.e12, m.e13,
									m.e20, m.e21, m.e22, m.e23,
									m.e30, m.e31, m.e32, m.e33));
}

/***************************************************************************/
// Return the determinant
/***************************************************************************/
inline float Matrix4x4::Det(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DET);
#endif //MATH_STATS

	return Det(*this);
}

/***************************************************************************/
// Make the cofactor matrix
/***************************************************************************/
inline void Matrix4x4::Cof(
Matrix4x4 &cofactor) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_COF_M4X4);
#endif //MATH_STATS

	cofactor.e00 = Determinant::Calc(e11, e12, e13, e21, e22, e23, e31, e32, e33);
	cofactor.e01 = -Determinant::Calc(e10, e12, e13, e20, e22, e23, e30, e32, e33);
	cofactor.e02 = Determinant::Calc(e10, e11, e13, e20, e21, e23, e30, e31, e33);
	cofactor.e03 = -Determinant::Calc(e10, e11, e12, e20, e21, e22, e30, e31, e32);

	cofactor.e10 = -Determinant::Calc(e01, e02, e03, e21, e22, e23, e31, e32, e33);
	cofactor.e11 = Determinant::Calc(e00, e02, e03, e20, e22, e23, e30, e32, e33);
	cofactor.e12 = -Determinant::Calc(e00, e01, e03, e20, e21, e23, e30, e31, e33);
	cofactor.e13 = Determinant::Calc(e00, e01, e02, e20, e21, e22, e30, e31, e32);

	cofactor.e20 = Determinant::Calc(e01, e02, e03, e11, e12, e13, e31, e32, e33);
	cofactor.e21 = -Determinant::Calc(e00, e02, e03, e10, e12, e13, e30, e32, e33);
	cofactor.e22 = Determinant::Calc(e00, e01, e03, e10, e11, e13, e30, e31, e33);
	cofactor.e23 = -Determinant::Calc(e00, e01, e02, e10, e11, e12, e30, e31, e32);

	cofactor.e30 = -Determinant::Calc(e01, e02, e03, e11, e12, e13, e21, e22, e23);
	cofactor.e31 = Determinant::Calc(e00, e02, e03, e10, e12, e13, e20, e22, e23);
	cofactor.e32 = -Determinant::Calc(e00, e01, e03, e10, e11, e13, e20, e21, e23);
	cofactor.e33 = Determinant::Calc(e00, e01, e02, e10, e11, e12, e20, e21, e22);
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix4x4::Cof(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_COF);
#endif //MATH_STATS

	Matrix4x4 cofactor;
	Cof(cofactor);
	*this = cofactor;
}

/***************************************************************************/
// Make the adjoint matrix
/***************************************************************************/
inline void Matrix4x4::Adj(
Matrix4x4 &adjoint) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_ADJ_M4X4);
#endif //MATH_STATS

	adjoint.e00 = Determinant::Calc(e11, e12, e13, e21, e22, e23, e31, e32, e33);
	adjoint.e10 = -Determinant::Calc(e10, e12, e13, e20, e22, e23, e30, e32, e33);
	adjoint.e20 = Determinant::Calc(e10, e11, e13, e20, e21, e23, e30, e31, e33);
	adjoint.e30 = -Determinant::Calc(e10, e11, e12, e20, e21, e22, e30, e31, e32);

	adjoint.e01 = -Determinant::Calc(e01, e02, e03, e21, e22, e23, e31, e32, e33);
	adjoint.e11 = Determinant::Calc(e00, e02, e03, e20, e22, e23, e30, e32, e33);
	adjoint.e21 = -Determinant::Calc(e00, e01, e03, e20, e21, e23, e30, e31, e33);
	adjoint.e31 = Determinant::Calc(e00, e01, e02, e20, e21, e22, e30, e31, e32);

	adjoint.e02 = Determinant::Calc(e01, e02, e03, e11, e12, e13, e31, e32, e33);
	adjoint.e12 = -Determinant::Calc(e00, e02, e03, e10, e12, e13, e30, e32, e33);
	adjoint.e22 = Determinant::Calc(e00, e01, e03, e10, e11, e13, e30, e31, e33);
	adjoint.e32 = -Determinant::Calc(e00, e01, e02, e10, e11, e12, e30, e31, e32);

	adjoint.e03 = -Determinant::Calc(e01, e02, e03, e11, e12, e13, e21, e22, e23);
	adjoint.e13 = Determinant::Calc(e00, e02, e03, e10, e12, e13, e20, e22, e23);
	adjoint.e23 = -Determinant::Calc(e00, e01, e03, e10, e11, e13, e20, e21, e23);
	adjoint.e33 = Determinant::Calc(e00, e01, e02, e10, e11, e12, e20, e21, e22);

}

/***************************************************************************/
/***************************************************************************/
inline void Matrix4x4::Adj(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_ADJ);
#endif //MATH_STATS

	Matrix4x4 adjoint;
	Adj(adjoint);
	*this = adjoint;
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Matrix4x4::operator==(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_EQUALS_M4X4);
#endif //MATH_STATS

	return (e00 == m.e00 && e01 == m.e01 && e02 == m.e02 && e03 == m.e03 &&
				e10 == m.e10 && e11 == m.e11 &&	e12 == m.e12 && e13 == m.e13 &&
				e20 == m.e20 && e21 == m.e21 && e22 == m.e22 && e23 == m.e23 &&
				e30 == m.e30 && e31 == m.e31 && e32 == m.e32 && e33 == m.e33);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Matrix4x4::operator!=(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_NOT_EQUAL_M4X4);
#endif //MATH_STATS

	return (e00 != m.e00 || e01 != m.e01 || e02 != m.e02 || e03 != m.e03 ||
				e10 != m.e10 || e11 != m.e11 ||	e12 != m.e12 || e13 != m.e13 ||
				e20 != m.e20 || e21 != m.e21 || e22 != m.e22 || e23 != m.e23 ||
				e30 != m.e30 || e31 != m.e31 || e32 != m.e32 || e33 != m.e33);
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Matrix4x4::operator==(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_EQUALS_M4X4);
#endif //MATH_STATS

	return (e00 == m.r00 && e01 == m.r01 && e02 == m.r02 && e03 == m.u0 &&
				e10 == m.r10 && e11 == m.r11 &&	e12 == m.r12 && e13 == m.u1 &&
				e20 == m.r20 && e21 == m.r21 && e22 == m.r22 && e23 == m.u2 &&
				e30 == m.tx && e31 == m.ty && e32 == m.tz && e33 == m.w);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Matrix4x4::operator!=(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_NOT_EQUAL_M4X4);
#endif //MATH_STATS

	return (e00 != m.r00 || e01 != m.r01 || e02 != m.r02 || e03 != m.u0 ||
				e10 != m.r10 || e11 != m.r11 ||	e12 != m.r12 || e13 != m.u1 ||
				e20 != m.r20 || e21 != m.r21 || e22 != m.r22 || e23 != m.u2 ||
				e30 != m.tx || e31 != m.ty || e32 != m.tz || e33 != m.w);
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Matrix4x4::operator==(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_EQUALS_M4X4);
#endif //MATH_STATS

	return (e00 == m.r00 && e01 == m.r01 && e02 == m.r02 && e03 == m.u0 &&
				e10 == m.r10 && e11 == m.r11 &&	e12 == m.r12 && e13 == m.u1 &&
				e20 == m.r20 && e21 == m.r21 && e22 == m.r22 && e23 == m.u2 &&
				e30 == m.tx && e31 == m.ty && e32 == m.tz && e33 == m.w);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Matrix4x4::operator!=(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_NOT_EQUAL_M4X4);
#endif //MATH_STATS

	return (e00 != m.r00 || e01 != m.r01 || e02 != m.r02 || e03 != m.u0 ||
				e10 != m.r10 || e11 != m.r11 ||	e12 != m.r12 || e13 != m.u1 ||
				e20 != m.r20 || e21 != m.r21 || e22 != m.r22 || e23 != m.u2 ||
				e30 != m.tx || e31 != m.ty || e32 != m.tz || e33 != m.w);
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator*(
float s) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_F);
#endif //MATH_STATS

	Matrix4x4 m;

	m.e00 = e00 * s;
	m.e01 = e01 * s;
	m.e02 = e02 * s;
	m.e03 = e03 * s;

	m.e10 = e10 * s;
	m.e11 = e11 * s;
	m.e12 = e12 * s;
	m.e13 = e13 * s;

	m.e20 = e20 * s;
	m.e21 = e21 * s;
	m.e22 = e22 * s;
	m.e23 = e23 * s;

	m.e30 = e30 * s;
	m.e31 = e31 * s;
	m.e32 = e32 * s;
	m.e33 = e33 * s;

	return m;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator/(
float s) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_F);
#endif //MATH_STATS

	Matrix4x4 m;

	float inv = 1.0f / s;
	m = *this * inv;

	return m;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator*=(
float s)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_F);
#endif //MATH_STATS

	e00 *= s;
	e01 *= s;
	e02 *= s;
	e03 *= s;

	e10 *= s;
	e11 *= s;
	e12 *= s;
	e13 *= s;

	e20 *= s;
	e21 *= s;
	e22 *= s;
	e23 *= s;

	e30 *= s;
	e31 *= s;
	e32 *= s;
	e33 *= s;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator/=(
float s)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_EQUALS_F);
#endif //MATH_STATS

	float inv = 1.0f / s;

	e00 *= inv;
	e01 *= inv;
	e02 *= inv;
	e03 *= inv;

	e10 *= inv;
	e11 *= inv;
	e12 *= inv;
	e13 *= inv;

	e20 *= inv;
	e21 *= inv;
	e22 *= inv;
	e23 *= inv;

	e30 *= inv;
	e31 *= inv;
	e32 *= inv;
	e33 *= inv;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator+(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.e00 = e00 + m.e00;
	sum.e01 = e01 + m.e01;
	sum.e02 = e02 + m.e02;
	sum.e03 = e03 + m.e03;

	sum.e10 = e10 + m.e10;
	sum.e11 = e11 + m.e11;
	sum.e12 = e12 + m.e12;
	sum.e13 = e13 + m.e13;

	sum.e20 = e20 + m.e20;
	sum.e21 = e21 + m.e21;
	sum.e22 = e22 + m.e22;
	sum.e23 = e23 + m.e23;

	sum.e30 = e30 + m.e30;
	sum.e31 = e31 + m.e31;
	sum.e32 = e32 + m.e32;
	sum.e33 = e33 + m.e33;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator+=(
const Matrix4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_EQUALS_M4X4);
#endif //MATH_STATS

	e00 += m.e00;
	e01 += m.e01;
	e02 += m.e02;
	e03 += m.e03;

	e10 += m.e10;
	e11 += m.e11;
	e12 += m.e12;
	e13 += m.e13;

	e20 += m.e20;
	e21 += m.e21;
	e22 += m.e22;
	e23 += m.e23;

	e30 += m.e30;
	e31 += m.e31;
	e32 += m.e32;
	e33 += m.e33;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator-(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.e00 = e00 - m.e00;
	sum.e01 = e01 - m.e01;
	sum.e02 = e02 - m.e02;
	sum.e03 = e03 - m.e03;

	sum.e10 = e10 - m.e10;
	sum.e11 = e11 - m.e11;
	sum.e12 = e12 - m.e12;
	sum.e13 = e13 - m.e13;

	sum.e20 = e20 - m.e20;
	sum.e21 = e21 - m.e21;
	sum.e22 = e22 - m.e22;
	sum.e23 = e23 - m.e23;

	sum.e30 = e30 - m.e30;
	sum.e31 = e31 - m.e31;
	sum.e32 = e32 - m.e32;
	sum.e33 = e33 - m.e33;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator-=(
const Matrix4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_EQUALS_M4X4);
#endif //MATH_STATS

	e00 -= m.e00;
	e01 -= m.e01;
	e02 -= m.e02;
	e03 -= m.e03;

	e10 -= m.e10;
	e11 -= m.e11;
	e12 -= m.e12;
	e13 -= m.e13;

	e20 -= m.e20;
	e21 -= m.e21;
	e22 -= m.e22;
	e23 -= m.e23;

	e30 -= m.e30;
	e31 -= m.e31;
	e32 -= m.e32;
	e33 -= m.e33;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator+(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.e00 = e00 + m.e00;
	sum.e01 = e01 + m.e01;
	sum.e02 = e02 + m.e02;
	sum.e03 = e03 + m.e03;

	sum.e10 = e10 + m.e10;
	sum.e11 = e11 + m.e11;
	sum.e12 = e12 + m.e12;
	sum.e13 = e13 + m.e13;

	sum.e20 = e20 + m.e20;
	sum.e21 = e21 + m.e21;
	sum.e22 = e22 + m.e22;
	sum.e23 = e23 + m.e23;

	sum.e30 = e30 + m.e30;
	sum.e31 = e31 + m.e31;
	sum.e32 = e32 + m.e32;
	sum.e33 = e33 + m.e33;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator+=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_EQUALS_M4X4);
#endif //MATH_STATS

	e00 += m.e00;
	e01 += m.e01;
	e02 += m.e02;
	e03 += m.e03;

	e10 += m.e10;
	e11 += m.e11;
	e12 += m.e12;
	e13 += m.e13;

	e20 += m.e20;
	e21 += m.e21;
	e22 += m.e22;
	e23 += m.e23;

	e30 += m.e30;
	e31 += m.e31;
	e32 += m.e32;
	e33 += m.e33;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator-(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.e00 = e00 - m.e00;
	sum.e01 = e01 - m.e01;
	sum.e02 = e02 - m.e02;
	sum.e03 = e03 - m.e03;

	sum.e10 = e10 - m.e10;
	sum.e11 = e11 - m.e11;
	sum.e12 = e12 - m.e12;
	sum.e13 = e13 - m.e13;

	sum.e20 = e20 - m.e20;
	sum.e21 = e21 - m.e21;
	sum.e22 = e22 - m.e22;
	sum.e23 = e23 - m.e23;

	sum.e30 = e30 - m.e30;
	sum.e31 = e31 - m.e31;
	sum.e32 = e32 - m.e32;
	sum.e33 = e33 - m.e33;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator-=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_EQUALS_M4X4);
#endif //MATH_STATS

	e00 -= m.e00;
	e01 -= m.e01;
	e02 -= m.e02;
	e03 -= m.e03;

	e10 -= m.e10;
	e11 -= m.e11;
	e12 -= m.e12;
	e13 -= m.e13;

	e20 -= m.e20;
	e21 -= m.e21;
	e22 -= m.e22;
	e23 -= m.e23;

	e30 -= m.e30;
	e31 -= m.e31;
	e32 -= m.e32;
	e33 -= m.e33;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator+(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.e00 = e00 + m.e00;
	sum.e01 = e01 + m.e01;
	sum.e02 = e02 + m.e02;
	sum.e03 = e03 + m.e03;

	sum.e10 = e10 + m.e10;
	sum.e11 = e11 + m.e11;
	sum.e12 = e12 + m.e12;
	sum.e13 = e13 + m.e13;

	sum.e20 = e20 + m.e20;
	sum.e21 = e21 + m.e21;
	sum.e22 = e22 + m.e22;
	sum.e23 = e23 + m.e23;

	sum.e30 = e30 + m.e30;
	sum.e31 = e31 + m.e31;
	sum.e32 = e32 + m.e32;
	sum.e33 = e33 + m.e33;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator+=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_EQUALS_M4X4);
#endif //MATH_STATS

	e00 += m.e00;
	e01 += m.e01;
	e02 += m.e02;
	e03 += m.e03;

	e10 += m.e10;
	e11 += m.e11;
	e12 += m.e12;
	e13 += m.e13;

	e20 += m.e20;
	e21 += m.e21;
	e22 += m.e22;
	e23 += m.e23;

	e30 += m.e30;
	e31 += m.e31;
	e32 += m.e32;
	e33 += m.e33;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator-(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.e00 = e00 - m.e00;
	sum.e01 = e01 - m.e01;
	sum.e02 = e02 - m.e02;
	sum.e03 = e03 - m.e03;

	sum.e10 = e10 - m.e10;
	sum.e11 = e11 - m.e11;
	sum.e12 = e12 - m.e12;
	sum.e13 = e13 - m.e13;

	sum.e20 = e20 - m.e20;
	sum.e21 = e21 - m.e21;
	sum.e22 = e22 - m.e22;
	sum.e23 = e23 - m.e23;

	sum.e30 = e30 - m.e30;
	sum.e31 = e31 - m.e31;
	sum.e32 = e32 - m.e32;
	sum.e33 = e33 - m.e33;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator-=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_EQUALS_M4X4);
#endif //MATH_STATS

	e00 -= m.e00;
	e01 -= m.e01;
	e02 -= m.e02;
	e03 -= m.e03;

	e10 -= m.e10;
	e11 -= m.e11;
	e12 -= m.e12;
	e13 -= m.e13;

	e20 -= m.e20;
	e21 -= m.e21;
	e22 -= m.e22;
	e23 -= m.e23;

	e30 -= m.e30;
	e31 -= m.e31;
	e32 -= m.e32;
	e33 -= m.e33;

	return *this;
}

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_MAT4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

/*** NOTE: this routine is provided so that new platform can get up and running without implementing ***/
/*** a platform-specific version of this. this generic version is probably NOT fast!!!               ***/

inline Matrix4x4 &Matrix4x4::FastMultiply(const Matrix4x4 &m1, const Matrix4x4 &m2)	// this = m1 * m2
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_FAST_MULTIPLY);
#endif //MATH_STATS

	Matrix4x4 product;

	/* must handle this == m1 or this == m2, so can't calculate directly into this */

	product.e00 = m1.e00 * m2.e00 + m1.e01 * m2.e10 + m1.e02 * m2.e20 + m1.e03 * m2.e30;
	product.e01 = m1.e00 * m2.e01 + m1.e01 * m2.e11 + m1.e02 * m2.e21 + m1.e03 * m2.e31;
	product.e02 = m1.e00 * m2.e02 + m1.e01 * m2.e12 + m1.e02 * m2.e22 + m1.e03 * m2.e32;
	product.e03 = m1.e00 * m2.e03 + m1.e01 * m2.e13 + m1.e02 * m2.e23 + m1.e03 * m2.e33;
					  	  					  	  		 			  	  		 			  	  		 
	product.e10 = m1.e10 * m2.e00 + m1.e11 * m2.e10 + m1.e12 * m2.e20 + m1.e13 * m2.e30;
	product.e11 = m1.e10 * m2.e01 + m1.e11 * m2.e11 + m1.e12 * m2.e21 + m1.e13 * m2.e31;
	product.e12 = m1.e10 * m2.e02 + m1.e11 * m2.e12 + m1.e12 * m2.e22 + m1.e13 * m2.e32;
	product.e13 = m1.e10 * m2.e03 + m1.e11 * m2.e13 + m1.e12 * m2.e23 + m1.e13 * m2.e33;
					  	  					  	  		 			  	  		 			  	  		 
	product.e20 = m1.e20 * m2.e00 + m1.e21 * m2.e10 + m1.e22 * m2.e20 + m1.e23 * m2.e30;
	product.e21 = m1.e20 * m2.e01 + m1.e21 * m2.e11 + m1.e22 * m2.e21 + m1.e23 * m2.e31;
	product.e22 = m1.e20 * m2.e02 + m1.e21 * m2.e12 + m1.e22 * m2.e22 + m1.e23 * m2.e32;
	product.e23 = m1.e20 * m2.e03 + m1.e21 * m2.e13 + m1.e22 * m2.e23 + m1.e23 * m2.e33;
					  	  					  	  		 			  	  		 			  	  		 
	product.e30 = m1.e30 * m2.e00 + m1.e31 * m2.e10 + m1.e32 * m2.e20 + m1.e33 * m2.e30;
	product.e31 = m1.e30 * m2.e01 + m1.e31 * m2.e11 + m1.e32 * m2.e21 + m1.e33 * m2.e31;
	product.e32 = m1.e30 * m2.e02 + m1.e31 * m2.e12 + m1.e32 * m2.e22 + m1.e33 * m2.e32;
	product.e33 = m1.e30 * m2.e03 + m1.e31 * m2.e13 + m1.e32 * m2.e23 + m1.e33 * m2.e33;

	// copy it in
	*this = product;
	return *this;
}

#endif //EXCLUDE_GENERIC_MAT4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_MAT4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Matrix4x4::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_M4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20 + e03 * m.e30;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21 + e03 * m.e31;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22 + e03 * m.e32;
	product.e03 = e00 * m.e03 + e01 * m.e13 + e02 * m.e23 + e03 * m.e33;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20 + e13 * m.e30;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21 + e13 * m.e31;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22 + e13 * m.e32;
	product.e13 = e10 * m.e03 + e11 * m.e13 + e12 * m.e23 + e13 * m.e33;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20 + e23 * m.e30;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21 + e23 * m.e31;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22 + e23 * m.e32;
	product.e23 = e20 * m.e03 + e21 * m.e13 + e22 * m.e23 + e23 * m.e33;

	product.e30 = e30 * m.e00 + e31 * m.e10 + e32 * m.e20 + e33 * m.e30;
	product.e31 = e30 * m.e01 + e31 * m.e11 + e32 * m.e21 + e33 * m.e31;
	product.e32 = e30 * m.e02 + e31 * m.e12 + e32 * m.e22 + e33 * m.e32;
	product.e33 = e30 * m.e03 + e31 * m.e13 + e32 * m.e23 + e33 * m.e33;
 
	return product;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 &Matrix4x4::operator*=(
const Matrix4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_M4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20 + e03 * m.e30;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21 + e03 * m.e31;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22 + e03 * m.e32;
	product.e03 = e00 * m.e03 + e01 * m.e13 + e02 * m.e23 + e03 * m.e33;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20 + e13 * m.e30;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21 + e13 * m.e31;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22 + e13 * m.e32;
	product.e13 = e10 * m.e03 + e11 * m.e13 + e12 * m.e23 + e13 * m.e33;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20 + e23 * m.e30;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21 + e23 * m.e31;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22 + e23 * m.e32;
	product.e23 = e20 * m.e03 + e21 * m.e13 + e22 * m.e23 + e23 * m.e33;

	product.e30 = e30 * m.e00 + e31 * m.e10 + e32 * m.e20 + e33 * m.e30;
	product.e31 = e30 * m.e01 + e31 * m.e11 + e32 * m.e21 + e33 * m.e31;
	product.e32 = e30 * m.e02 + e31 * m.e12 + e32 * m.e22 + e33 * m.e32;
	product.e33 = e30 * m.e03 + e31 * m.e13 + e32 * m.e23 + e33 * m.e33;

	// copy it in
	*this = product;
	return *this;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_MAT4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_MAT4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Matrix4x4::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_G4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.r00 + e01 * m.r10 + e02 * m.r20 + e03 * m.tx;
	product.e01 = e00 * m.r01 + e01 * m.r11 + e02 * m.r21 + e03 * m.ty;
	product.e02 = e00 * m.r02 + e01 * m.r12 + e02 * m.r22 + e03 * m.tz;
	product.e03 = e03;

	product.e10 = e10 * m.r00 + e11 * m.r10 + e12 * m.r20 + e13 * m.tx;
	product.e11 = e10 * m.r01 + e11 * m.r11 + e12 * m.r21 + e13 * m.ty;
	product.e12 = e10 * m.r02 + e11 * m.r12 + e12 * m.r22 + e13 * m.tz;
	product.e13 = e13;

	product.e20 = e20 * m.r00 + e21 * m.r10 + e22 * m.r20 + e23 * m.tx;
	product.e21 = e20 * m.r01 + e21 * m.r11 + e22 * m.r21 + e23 * m.ty;
	product.e22 = e20 * m.r02 + e21 * m.r12 + e22 * m.r22 + e23 * m.tz;
	product.e23 = e23;

	product.e30 = e30 * m.r00 + e31 * m.r10 + e32 * m.r20 + e33 * m.tx;
	product.e31 = e30 * m.r01 + e31 * m.r11 + e32 * m.r21 + e33 * m.ty;
	product.e32 = e30 * m.r02 + e31 * m.r12 + e32 * m.r22 + e33 * m.tz;
	product.e33 = e33;
 
	return product;
}

inline Matrix4x4 Matrix4x4::operator*(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_G4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.r00 + e01 * m.r10 + e02 * m.r20 + e03 * m.tx;
	product.e01 = e00 * m.r01 + e01 * m.r11 + e02 * m.r21 + e03 * m.ty;
	product.e02 = e00 * m.r02 + e01 * m.r12 + e02 * m.r22 + e03 * m.tz;
	product.e03 = e03;

	product.e10 = e10 * m.r00 + e11 * m.r10 + e12 * m.r20 + e13 * m.tx;
	product.e11 = e10 * m.r01 + e11 * m.r11 + e12 * m.r21 + e13 * m.ty;
	product.e12 = e10 * m.r02 + e11 * m.r12 + e12 * m.r22 + e13 * m.tz;
	product.e13 = e13;

	product.e20 = e20 * m.r00 + e21 * m.r10 + e22 * m.r20 + e23 * m.tx;
	product.e21 = e20 * m.r01 + e21 * m.r11 + e22 * m.r21 + e23 * m.ty;
	product.e22 = e20 * m.r02 + e21 * m.r12 + e22 * m.r22 + e23 * m.tz;
	product.e23 = e23;

	product.e30 = e30 * m.r00 + e31 * m.r10 + e32 * m.r20 + e33 * m.tx;
	product.e31 = e30 * m.r01 + e31 * m.r11 + e32 * m.r21 + e33 * m.ty;
	product.e32 = e30 * m.r02 + e31 * m.r12 + e32 * m.r22 + e33 * m.tz;
	product.e33 = e33;
 
	return product;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_GRAPHIS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 &Matrix4x4::operator*=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.r00 + e01 * m.r10 + e02 * m.r20 + e03 * m.tx;
	product.e01 = e00 * m.r01 + e01 * m.r11 + e02 * m.r21 + e03 * m.ty;
	product.e02 = e00 * m.r02 + e01 * m.r12 + e02 * m.r22 + e03 * m.tz;
	product.e03 = e03;

	product.e10 = e10 * m.r00 + e11 * m.r10 + e12 * m.r20 + e13 * m.tx;
	product.e11 = e10 * m.r01 + e11 * m.r11 + e12 * m.r21 + e13 * m.ty;
	product.e12 = e10 * m.r02 + e11 * m.r12 + e12 * m.r22 + e13 * m.tz;
	product.e13 = e13;

	product.e20 = e20 * m.r00 + e21 * m.r10 + e22 * m.r20 + e23 * m.tx;
	product.e21 = e20 * m.r01 + e21 * m.r11 + e22 * m.r21 + e23 * m.ty;
	product.e22 = e20 * m.r02 + e21 * m.r12 + e22 * m.r22 + e23 * m.tz;
	product.e23 = e23;

	product.e30 = e30 * m.r00 + e31 * m.r10 + e32 * m.r20 + e33 * m.tx;
	product.e31 = e30 * m.r01 + e31 * m.r11 + e32 * m.r21 + e33 * m.ty;
	product.e32 = e30 * m.r02 + e31 * m.r12 + e32 * m.r22 + e33 * m.tz;
	product.e33 = e33;

	// copy it in
	*this = product;
	return *this;
}

inline Matrix4x4 &Matrix4x4::operator*=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.r00 + e01 * m.r10 + e02 * m.r20 + e03 * m.tx;
	product.e01 = e00 * m.r01 + e01 * m.r11 + e02 * m.r21 + e03 * m.ty;
	product.e02 = e00 * m.r02 + e01 * m.r12 + e02 * m.r22 + e03 * m.tz;
	product.e03 = e03;

	product.e10 = e10 * m.r00 + e11 * m.r10 + e12 * m.r20 + e13 * m.tx;
	product.e11 = e10 * m.r01 + e11 * m.r11 + e12 * m.r21 + e13 * m.ty;
	product.e12 = e10 * m.r02 + e11 * m.r12 + e12 * m.r22 + e13 * m.tz;
	product.e13 = e13;

	product.e20 = e20 * m.r00 + e21 * m.r10 + e22 * m.r20 + e23 * m.tx;
	product.e21 = e20 * m.r01 + e21 * m.r11 + e22 * m.r21 + e23 * m.ty;
	product.e22 = e20 * m.r02 + e21 * m.r12 + e22 * m.r22 + e23 * m.tz;
	product.e23 = e23;

	product.e30 = e30 * m.r00 + e31 * m.r10 + e32 * m.r20 + e33 * m.tx;
	product.e31 = e30 * m.r01 + e31 * m.r11 + e32 * m.r21 + e33 * m.ty;
	product.e32 = e30 * m.r02 + e31 * m.r12 + e32 * m.r22 + e33 * m.tz;
	product.e33 = e33;

	// copy it in
	*this = product;
	return *this;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator/(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_C4X4);
#endif //MATH_STATS

	Matrix4x4 product;
	float xTerm, yTerm, zTerm;

	xTerm = e00 - e03 * m.tx;
	yTerm = e01 - e03 * m.ty;
	zTerm = e02 - e03 * m.tz;
	product.e00 = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.e01 = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.e02 = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.e03 = e03;

	xTerm = e10 - e13 * m.tx;
	yTerm = e11 - e13 * m.ty;
	zTerm = e12 - e13 * m.tz;
	product.e10 = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.e11 = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.e12 = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.e13 = e13;

	xTerm = e20 - e23 * m.tx;
	yTerm = e21 - e23 * m.ty;
	zTerm = e22 - e23 * m.tz;
	product.e20 = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.e21 = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.e22 = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.e23 = e23;

	xTerm = e30 - e33 * m.tx;
	yTerm = e31 - e33 * m.ty;
	zTerm = e32 - e33 * m.tz;
	product.e30 = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.e31 = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.e32 = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.e33 = e33;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator/=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_EQUALS_C4X4);
#endif //MATH_STATS

	Matrix4x4 product;
	float xTerm, yTerm, zTerm;

	xTerm = e00 - e03 * m.tx;
	yTerm = e01 - e03 * m.ty;
	zTerm = e02 - e03 * m.tz;
	product.e00 = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.e01 = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.e02 = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.e03 = e03;

	xTerm = e10 - e13 * m.tx;
	yTerm = e11 - e13 * m.ty;
	zTerm = e12 - e13 * m.tz;
	product.e10 = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.e11 = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.e12 = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.e13 = e13;

	xTerm = e20 - e23 * m.tx;
	yTerm = e21 - e23 * m.ty;
	zTerm = e22 - e23 * m.tz;
	product.e20 = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.e21 = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.e22 = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.e23 = e23;

	xTerm = e30 - e33 * m.tx;
	yTerm = e31 - e33 * m.ty;
	zTerm = e32 - e33 * m.tz;
	product.e30 = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.e31 = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.e32 = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.e33 = e33;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator*(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_M3X3);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;
	product.e03 = e03;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;
	product.e13 = e13;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;
	product.e23 = e23;

	product.e30 = e30 * m.e00 + e31 * m.e10 + e32 * m.e20;
	product.e31 = e30 * m.e01 + e31 * m.e11 + e32 * m.e21;
	product.e32 = e30 * m.e02 + e31 * m.e12 + e32 * m.e22;
	product.e33 = e33;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator*=(
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_M3X3);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;
	product.e03 = e03;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;
	product.e13 = e13;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;
	product.e23 = e23;

	product.e30 = e30 * m.e00 + e31 * m.e10 + e32 * m.e20;
	product.e31 = e30 * m.e01 + e31 * m.e11 + e32 * m.e21;
	product.e32 = e30 * m.e02 + e31 * m.e12 + e32 * m.e22;
	product.e33 = e33;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator*(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_M3X3);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;
	product.e03 = e03;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;
	product.e13 = e13;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;
	product.e23 = e23;

	product.e30 = e30 * m.e00 + e31 * m.e10 + e32 * m.e20;
	product.e31 = e30 * m.e01 + e31 * m.e11 + e32 * m.e21;
	product.e32 = e30 * m.e02 + e31 * m.e12 + e32 * m.e22;
	product.e33 = e33;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator*=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_M3X3);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;
	product.e03 = e03;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;
	product.e13 = e13;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;
	product.e23 = e23;

	product.e30 = e30 * m.e00 + e31 * m.e10 + e32 * m.e20;
	product.e31 = e30 * m.e01 + e31 * m.e11 + e32 * m.e21;
	product.e32 = e30 * m.e02 + e31 * m.e12 + e32 * m.e22;
	product.e33 = e33;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix4x4::operator/(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_D3X3);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e01 + e02 * m.e02;
	product.e01 = e00 * m.e10 + e01 * m.e11 + e02 * m.e12;
	product.e02 = e00 * m.e20 + e01 * m.e21 + e02 * m.e22;
	product.e03 = e03;

	product.e10 = e10 * m.e00 + e11 * m.e01 + e12 * m.e02;
	product.e11 = e10 * m.e10 + e11 * m.e11 + e12 * m.e12;
	product.e12 = e10 * m.e20 + e11 * m.e21 + e12 * m.e22;
	product.e13 = e13;

	product.e20 = e20 * m.e00 + e21 * m.e01 + e22 * m.e02;
	product.e21 = e20 * m.e10 + e21 * m.e11 + e22 * m.e12;
	product.e22 = e20 * m.e20 + e21 * m.e21 + e22 * m.e22;
	product.e23 = e23;

	product.e30 = e30 * m.e00 + e31 * m.e01 + e32 * m.e02;
	product.e31 = e30 * m.e10 + e31 * m.e11 + e32 * m.e12;
	product.e32 = e30 * m.e20 + e31 * m.e21 + e32 * m.e22;
	product.e33 = e33;

	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 &Matrix4x4::operator/=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_EQUALS_D3X3);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e01 + e02 * m.e02;
	product.e01 = e00 * m.e10 + e01 * m.e11 + e02 * m.e12;
	product.e02 = e00 * m.e20 + e01 * m.e21 + e02 * m.e22;
	product.e03 = e03;

	product.e10 = e10 * m.e00 + e11 * m.e01 + e12 * m.e02;
	product.e11 = e10 * m.e10 + e11 * m.e11 + e12 * m.e12;
	product.e12 = e10 * m.e20 + e11 * m.e21 + e12 * m.e22;
	product.e13 = e13;

	product.e20 = e20 * m.e00 + e21 * m.e01 + e22 * m.e02;
	product.e21 = e20 * m.e10 + e21 * m.e11 + e22 * m.e12;
	product.e22 = e20 * m.e20 + e21 * m.e21 + e22 * m.e22;
	product.e23 = e23;

	product.e30 = e30 * m.e00 + e31 * m.e01 + e32 * m.e02;
	product.e31 = e30 * m.e10 + e31 * m.e11 + e32 * m.e12;
	product.e32 = e30 * m.e20 + e31 * m.e21 + e32 * m.e22;
	product.e33 = e33;

	*this = product;
	return *this;
}

/***************************************************************************/
// Get a row (copies)
/***************************************************************************/
inline Vector3 Matrix4x4::Row(
int row) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_ROW_I);
#endif //MATH_STATS

	switch (row)
	{
		case 0:
		default:
			return Vector3(e00, e01, e02);

		case 1:
			return Vector3(e10, e11, e12);

		case 2:
			return Vector3(e20, e21, e22);

		case 3:
			return Vector3(e30, e31, e32);
	}
}

/***************************************************************************/
// Get a column (copies)
/***************************************************************************/
inline Vector3 Matrix4x4::Column(
int column) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_COLUMN_I);
#endif //MATH_STATS

	switch (column)
	{
		case 0:
		default:
			return Vector3(e00, e10, e20);

		case 1:
			return Vector3(e01, e11, e21);

		case 2:
			return Vector3(e02, e12, e22);

		case 3:
			return Vector3(e03, e13, e23);
	}
}

/***************************************************************************/
// Set a row
/***************************************************************************/
inline void Matrix4x4::SetRow(
int row,
Vector3CRef data)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_ROW_I_V3);
#endif //MATH_STATS

	switch (row)
	{
		case 0:
			e00 = data.x();
			e01 = data.y();
			e02 = data.z();
			break;

		case 1:
			e10 = data.x();
			e11 = data.y();
			e12 = data.z();
			break;

		case 2:
			e20 = data.x();
			e21 = data.y();
			e22 = data.z();
			break;

		case 3:
			e30 = data.x();
			e31 = data.y();
			e32 = data.z();
			break;
	}
}

/***************************************************************************/
// Set a column
/***************************************************************************/
inline void Matrix4x4::SetColumn(
int column,
Vector3CRef data)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_COLUMN_I_V3);
#endif //MATH_STATS

	switch (column)
	{
		case 0:
			e00 = data.x();
			e10 = data.y();
			e20 = data.z();
			break;

		case 1:
			e01 = data.x();
			e11 = data.y();
			e21 = data.z();
			break;

		case 2:
			e02 = data.x();
			e12 = data.y();
			e22 = data.z();
			break;

		case 3:
			e03 = data.x();
			e13 = data.y();
			e23 = data.z();
			break;
	}
}

/***************************************************************************/
// Get a the first three values of a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix4x4::GetRow0(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_GET_ROW0_V3);
#endif //MATH_STATS

	return Vector3(e00, e01, e02);
}

/***************************************************************************/
// Get a the first three values of a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix4x4::GetRow1(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_GET_ROW1_V3);
#endif //MATH_STATS

	return Vector3(e10, e11, e12);
}

/***************************************************************************/
// Get a the first three values of a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix4x4::GetRow2(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_GET_ROW2_V3);
#endif //MATH_STATS

	return Vector3(e20, e21, e22);
}

/***************************************************************************/
// Get a the first three values of a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix4x4::GetRow3(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_GET_ROW3_V3);
#endif //MATH_STATS

	return Vector3(e30, e31, e32);
}

/***************************************************************************/
// Get a the first three values of a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix4x4::GetColumn0(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_GET_COLUMN0_V3);
#endif //MATH_STATS

	return Vector3(e00, e10, e20);
}

/***************************************************************************/
// Get a the first three values of a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix4x4::GetColumn1(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_GET_COLUMN1_V3);
#endif //MATH_STATS

	return Vector3(e01, e11, e21);
}

/***************************************************************************/
// Get a the first three values of a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix4x4::GetColumn2(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_GET_COLUMN2_V3);
#endif //MATH_STATS

	return Vector3(e02, e12, e22);
}

/***************************************************************************/
// Get a the first three values of a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix4x4::GetColumn3(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_GET_COLUMN3_V3);
#endif //MATH_STATS

	return Vector3(e03, e13, e23);
}

/***************************************************************************/
// Set a the first three values of a row or column from a vector
/***************************************************************************/
inline void Matrix4x4::SetRow0(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_ROW0_V3);
#endif //MATH_STATS

	e00 = v.x();
	e01 = v.y();
	e02 = v.z();
}

/***************************************************************************/
// Set a the first three values of a row or column from a vector
/***************************************************************************/
inline void Matrix4x4::SetRow1(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_ROW1_V3);
#endif //MATH_STATS

	e10 = v.x();
	e11 = v.y();
	e12 = v.z();
}

/***************************************************************************/
// Set a the first three values of a row or column from a vector
/***************************************************************************/
inline void Matrix4x4::SetRow2(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_ROW2_V3);
#endif //MATH_STATS

	e20 = v.x();
	e21 = v.y();
	e22 = v.z();
}

/***************************************************************************/
// Set a the first three values of a row or column from a vector
/***************************************************************************/
inline void Matrix4x4::SetRow3(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_ROW3_V3);
#endif //MATH_STATS

	e30 = v.x();
	e31 = v.y();
	e32 = v.z();
}

/***************************************************************************/
// Set a the first three values of a row or column from a vector
/***************************************************************************/
inline void Matrix4x4::SetColumn0(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_COLUMN0_V3);
#endif //MATH_STATS

	e00 = v.x();
	e10 = v.y();
	e20 = v.z();
}

/***************************************************************************/
// Set a the first three values of a row or column from a vector
/***************************************************************************/
inline void Matrix4x4::SetColumn1(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_COLUMN1_V3);
#endif //MATH_STATS

	e01 = v.x();
	e11 = v.y();
	e21 = v.z();
}

/***************************************************************************/
// Set a the first three values of a row or column from a vector
/***************************************************************************/
inline void Matrix4x4::SetColumn2(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_COLUMN2_V3);
#endif //MATH_STATS

	e02 = v.x();
	e12 = v.y();
	e22 = v.z();
}

/***************************************************************************/
// Set a the first three values of a row or column from a vector
/***************************************************************************/
inline void Matrix4x4::SetColumn3(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_SET_COLUMN3_V3);
#endif //MATH_STATS

	e03 = v.x();
	e13 = v.y();
	e23 = v.z();
}

#ifdef GCN
/***************************************************************************/
// Swap the bytes for gamecube
/***************************************************************************/
inline void Matrix4x4::EndianSwap(void)
{
	ByteSwap(e00);
	ByteSwap(e01);
	ByteSwap(e02);
	ByteSwap(e03);

	ByteSwap(e10);
	ByteSwap(e11);
	ByteSwap(e12);
	ByteSwap(e13);

	ByteSwap(e20);
	ByteSwap(e21);
	ByteSwap(e22);
	ByteSwap(e23);

	ByteSwap(e30);
	ByteSwap(e31);
	ByteSwap(e32);
	ByteSwap(e33);
}
#endif

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4::Graphics4x4(
const Matrix3x3 &m,
Vector3CRef pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CONSTRUCT_M3X3_V3);
#endif //MATH_STATS

	r00 = m.e00;
	r01 = m.e01;
	r02 = m.e02;
	u0 = 0.0f;

	r10 = m.e10;
	r11 = m.e11;
	r12 = m.e12;
	u1 = 0.0f;

	r20 = m.e20;
	r21 = m.e21;
	r22 = m.e22;
	u2 = 0.0f;

  	tx = pos.x();
  	ty = pos.y();
	tz = pos.z();
	w = 1.0f;
}

/***************************************************************************/
// Specifiy all the relevant elements
/***************************************************************************/
inline Graphics4x4::Graphics4x4(
float r00,
float r01,
float r02,
float r10,
float r11,
float r12,
float r20,
float r21,
float r22,
float tx,
float ty,
float tz)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CONSTRUCT_F);
#endif //MATH_STATS

	this->r00 = r00;
	this->r01 = r01;
	this->r02 = r02;
	this->u0 = 0.0f;

	this->r10 = r10;
	this->r11 = r11;
	this->r12 = r12;
	this->u1 = 0.0f;

	this->r20 = r20;
	this->r21 = r21;
	this->r22 = r22;
	this->u2 = 0.0f;

	this->tx = tx;
	this->ty = ty;
	this->tz = tz;
	this->w = 1.0f;
}

/***************************************************************************/
// Assign the matrix
/***************************************************************************/
inline Graphics4x4 &Graphics4x4::operator=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_ASSIGN_M4X4);
#endif //MATH_STATS

	r00 = m.r00;
	r01 = m.r01;
	r02 = m.r02;
	u0 = m.u0;

	r10 = m.r10;
	r11 = m.r11;
	r12 = m.r12;
	u1 = m.u1;

	r20 = m.r20;
	r21 = m.r21;
	r22 = m.r22;
	u2 = m.u2;

	tx = m.tx;
	ty = m.ty;
	tz = m.tz;
	w = m.w;

	return *this;
}

/***************************************************************************/
// Assign the matrix
/***************************************************************************/
inline Graphics4x4 &Graphics4x4::operator=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_ASSIGN_M4X4);
#endif //MATH_STATS

	r00 = m.r00;
	r01 = m.r01;
	r02 = m.r02;
	u0 = m.u0;

	r10 = m.r10;
	r11 = m.r11;
	r12 = m.r12;
	u1 = m.u1;

	r20 = m.r20;
	r21 = m.r21;
	r22 = m.r22;
	u2 = m.u2;

	tx = m.tx;
	ty = m.ty;
	tz = m.tz;
	w = m.w;

	return *this;
}

/***************************************************************************/
// Set the 3x3 into the orientation area
/***************************************************************************/
inline Graphics4x4 &Graphics4x4::operator=(
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_ASSIGN_M3X3);
#endif //MATH_STATS

	r00 = m.e00;
	r01 = m.e01;
	r02 = m.e02;
	u0 = 0.0f;

	r10 = m.e10;
	r11 = m.e11;
	r12 = m.e12;
	u1 = 0.0f;

	r20 = m.e20;
	r21 = m.e21;
	r22 = m.e22;
	u2 = 0.0f;

  	tx = 0.0f;
  	ty = 0.0f;
	tz = 0.0f;
	w = 1.0f;

	return *this;
}

/***************************************************************************/
// Set the 3x3 into the orientation area
/***************************************************************************/
inline Graphics4x4 &Graphics4x4::operator=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_ASSIGN_M3X3);
#endif //MATH_STATS

	r00 = m.e00;
	r01 = m.e01;
	r02 = m.e02;
	u0 = 0.0f;

	r10 = m.e10;
	r11 = m.e11;
	r12 = m.e12;
	u1 = 0.0f;

	r20 = m.e20;
	r21 = m.e21;
	r22 = m.e22;
	u2 = 0.0f;

  	tx = 0.0f;
  	ty = 0.0f;
	tz = 0.0f;
	w = 1.0f;

	return *this;
}

/***************************************************************************/
// Specifiy all the relevant elements
/***************************************************************************/
inline void Graphics4x4::Set(
float r00,
float r01,
float r02,
float r10,
float r11,
float r12,
float r20,
float r21,
float r22,
float tx,
float ty,
float tz)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CONSTRUCT_F);
#endif //MATH_STATS

	this->r00 = r00;
	this->r01 = r01;
	this->r02 = r02;
	this->u0 = 0.0f;

	this->r10 = r10;
	this->r11 = r11;
	this->r12 = r12;
	this->u1 = 0.0f;

	this->r20 = r20;
	this->r21 = r21;
	this->r22 = r22;
	this->u2 = 0.0f;

	this->tx = tx;
	this->ty = ty;
	this->tz = tz;
	this->w = 1.0f;
}

/***************************************************************************/
/***************************************************************************/
inline Vector3 Graphics4x4::GetTranslation(
void) const
{
	return GetRow3();
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::Set(
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_M3X3);
#endif //MATH_STATS

	r00 = m.e00;
	r01 = m.e01;
	r02 = m.e02;
	u0 = 0.0f;

	r10 = m.e10;
	r11 = m.e11;
	r12 = m.e12;
	u1 = 0.0f;

	r20 = m.e20;
	r21 = m.e21;
	r22 = m.e22;
	u2 = 0.0f;

  	tx = 0.0f;
  	ty = 0.0f;
	tz = 0.0f;
	w = 1.0f;
}

/***************************************************************************/
// Static for the determinant
/***************************************************************************/
inline float Graphics4x4::Det(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DET_G4X4);
#endif //MATH_STATS

	return(Determinant::Calc(m.e00, m.e01, m.e02,
									m.e10, m.e11, m.e12,
									m.e20, m.e21, m.e22));
}

/***************************************************************************/
// Return the determinant
/***************************************************************************/
inline float Graphics4x4::Det(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DET);
#endif //MATH_STATS

	return Det(*this);
}

/***************************************************************************/
// Make the cofactor matrix
/***************************************************************************/
inline void Graphics4x4::Cof(
Matrix4x4 &cofactor) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_COF_M4X4);
#endif //MATH_STATS

	cofactor.e00 = Determinant::Calc(e11, e12, e21, e22);
	cofactor.e01 = -Determinant::Calc(e10, e12, e20, e22);
	cofactor.e02 = Determinant::Calc(e10, e11, e20, e21);
	cofactor.e03 = -Determinant::Calc(e10, e11, e12, e20, e21, e22, e30, e31, e32);

	cofactor.e10 = -Determinant::Calc(e01, e02, e21, e22);
	cofactor.e11 = Determinant::Calc(e00, e02, e20, e22);
	cofactor.e12 = -Determinant::Calc(e00, e01, e20, e21);
	cofactor.e13 = Determinant::Calc(e00, e01, e02, e20, e21, e22, e30, e31, e32);

	cofactor.e20 = Determinant::Calc(e01, e02, e11, e12);
	cofactor.e21 = -Determinant::Calc(e00, e02, e10, e12);
	cofactor.e22 = Determinant::Calc(e00, e01, e10, e11);
	cofactor.e23 = -Determinant::Calc(e00, e01, e02, e10, e11, e12, e30, e31, e32);

	cofactor.e30 = 0.0f;
	cofactor.e31 = 0.0f;
	cofactor.e32 = 0.0f;
	cofactor.e33 = Determinant::Calc(e00, e01, e02, e10, e11, e12, e20, e21, e22);
}

/***************************************************************************/
// Make the adjoint matrix
/***************************************************************************/
inline void Graphics4x4::Adj(
Matrix4x4 &adjoint) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_ADJ_M4X4);
#endif //MATH_STATS

	adjoint.e00 = Determinant::Calc(e11, e12, e21, e22);
	adjoint.e10 = -Determinant::Calc(e10, e12, e20, e22);
	adjoint.e20 = Determinant::Calc(e10, e11, e20, e21);
	adjoint.e30 = -Determinant::Calc(e10, e11, e12, e20, e21, e22, e30, e31, e32);

	adjoint.e01 = -Determinant::Calc(e01, e02, e21, e22);
	adjoint.e11 = Determinant::Calc(e00, e02, e20, e22);
	adjoint.e21 = -Determinant::Calc(e00, e01, e20, e21);
	adjoint.e31 = Determinant::Calc(e00, e01, e02, e20, e21, e22, e30, e31, e32);

	adjoint.e02 = Determinant::Calc(e01, e02, e11, e12);
	adjoint.e12 = -Determinant::Calc(e00, e02, e10, e12);
	adjoint.e22 = Determinant::Calc(e00, e01, e10, e11);
	adjoint.e32 = -Determinant::Calc(e00, e01, e02, e10, e11, e12, e30, e31, e32);

	adjoint.e03 = 0.0f;
	adjoint.e13 = 0.0f;
	adjoint.e23 = 0.0f;
	adjoint.e33 = Determinant::Calc(e00, e01, e02, e10, e11, e12, e20, e21, e22);
}

/***************************************************************************/
// Invert (returns false if not invertable)
/***************************************************************************/
inline bool Graphics4x4::Invert(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_INVERT);
#endif //MATH_STATS

	// invert it
	Graphics4x4 inverse;
	if (!Invert(inverse))
		return false;

	// save it
	*this = inverse;
	return true;
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Graphics4x4::operator==(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_EQUALS_G4X4);
#endif //MATH_STATS

	return (r00 == m.e00 && r01 == m.e01 && r02 == m.e02 && m.e03 == 0.0f &&
				r10 == m.e10 && r11 == m.e11 &&	r12 == m.e12 && m.e13 == 0.0f &&
				r20 == m.e20 && r21 == m.e21 && r22 == m.r22 && m.e23 == 0.0f &&
				tx == m.tx && ty == m.ty && m.tz == tz && m.e33 == 1.0f);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Graphics4x4::operator!=(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_NOT_EQUAL_G4X4);
#endif //MATH_STATS

	return (r00 != m.e00 || r01 != m.e01 || r02 != m.e02 || m.e03 != 0.0f ||
				r10 != m.e10 || r11 != m.e11 ||	r12 != m.e12 || m.e13 != 0.0f ||
				r20 != m.e20 || r21 != m.e21 || r22 != m.r22 || m.e23 != 0.0f ||
				tx != m.tx || ty != m.ty || m.tz != tz || m.e33 != 1.0f);
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Graphics4x4::operator==(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_EQUALS_G4X4);
#endif //MATH_STATS

	return (r00 == m.r00 && r01 == m.r01 && r02 == m.r02 &&
				r10 == m.r10 && r11 == m.r11 &&	r12 == m.r12 &&
				r20 == m.r20 && r21 == m.r21 && r22 == m.r22 &&
				tx == m.tx && ty == m.ty && m.tz == tz);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Graphics4x4::operator!=(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_NOT_EQUAL_G4X4);
#endif //MATH_STATS

	return (r00 != m.r00 || r01 != m.r01 || r02 != m.r02 ||
				r10 != m.r10 || r11 != m.r11 || r12 != m.r12 ||
				r20 != m.r20 || r21 != m.r21 || r22 != m.r22 ||
				tx != m.tx || ty != m.ty || m.tz != tz);
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Graphics4x4::operator==(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_EQUALS_G4X4);
#endif //MATH_STATS

	return (r00 == m.r00 && r01 == m.r01 && r02 == m.r02 &&
				r10 == m.r10 && r11 == m.r11 &&	r12 == m.r12 &&
				r20 == m.r20 && r21 == m.r21 && r22 == m.r22 &&
				tx == m.tx && ty == m.ty && m.tz == tz);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Graphics4x4::operator!=(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_NOT_EQUAL_G4X4);
#endif //MATH_STATS

	return (r00 != m.r00 || r01 != m.r01 || r02 != m.r02 ||
				r10 != m.r10 || r11 != m.r11 || r12 != m.r12 ||
				r20 != m.r20 || r21 != m.r21 || r22 != m.r22 ||
				tx != m.tx || ty != m.ty || m.tz != tz);
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Graphics4x4::operator*(
float s) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_F);
#endif //MATH_STATS

	Matrix4x4 m;

	m.e00 = r00 * s;
	m.e01 = r01 * s;
	m.e02 = r02 * s;
	m.e03 = 0.0f;

	m.e10 = r10 * s;
	m.e11 = r11 * s;
	m.e12 = r12 * s;
	m.e12 = 0.0f;

	m.e20 = r20 * s;
	m.e21 = r21 * s;
	m.e22 = r22 * s;
	m.e23 = 0.0f;

	m.e30 = tx * s;
	m.e31 = ty * s;
	m.e32 = tz * s;
	m.e33 = s;

	return m;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Graphics4x4::operator/(
float s) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_F);
#endif //MATH_STATS

	Matrix4x4 m;

	float inv = 1.0f / s;
	m = *this * inv;

	return m;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Graphics4x4::operator+(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.e00 = r00 + m.e00;
	sum.e01 = r01 + m.e01;
	sum.e02 = r02 + m.e02;
	sum.e03 = m.e03;

	sum.e10 = r10 + m.e10;
	sum.e11 = r11 + m.e11;
	sum.e12 = r12 + m.e12;
	sum.e13 = m.e13;

	sum.e20 = r20 + m.e20;
	sum.e21 = r21 + m.e21;
	sum.e22 = r22 + m.e22;
	sum.e23 = m.e23;

	sum.e30 = tx + m.e30;
	sum.e31 = ty + m.e31;
	sum.e32 = tz + m.e32;
	sum.e33 = 1.0f + m.e33;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Graphics4x4::operator-(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.e00 = r00 - m.e00;
	sum.e01 = r01 - m.e01;
	sum.e02 = r02 - m.e02;
	sum.e03 = m.e03;

	sum.e10 = r10 - m.e10;
	sum.e11 = r11 - m.e11;
	sum.e12 = r12 - m.e12;
	sum.e13 = m.e13;

	sum.e20 = r20 - m.e20;
	sum.e21 = r21 - m.e21;
	sum.e22 = r22 - m.e22;
	sum.e23 = m.e23;

	sum.e30 = tx - m.e30;
	sum.e31 = ty - m.e31;
	sum.e32 = tz - m.e32;
	sum.e33 = 1.0f - m.e33;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Graphics4x4::operator+(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.r00 = r00 + m.r00;
	sum.r01 = r01 + m.r01;
	sum.r02 = r02 + m.r02;
	sum.u0 = 0.0f;

	sum.r10 = r10 + m.r10;
	sum.r11 = r11 + m.r11;
	sum.r12 = r12 + m.r12;
	sum.u1 = 0.0f;

	sum.r20 = r20 + m.r20;
	sum.r21 = r21 + m.r21;
	sum.r22 = r22 + m.r22;
	sum.u2 = 0.0f;

	sum.tx = tx + m.tx;
	sum.ty = ty + m.ty;
	sum.tz = tz + m.tz;
	sum.w = 2.0f;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Graphics4x4::operator-(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.r00 = r00 - m.r00;
	sum.r01 = r01 - m.r01;
	sum.r02 = r02 - m.r02;
	sum.u0 = 0.0f;

	sum.r10 = r10 - m.r10;
	sum.r11 = r11 - m.r11;
	sum.r12 = r12 - m.r12;
	sum.u1 = 0.0f;

	sum.r20 = r20 - m.r20;
	sum.r21 = r21 - m.r21;
	sum.r22 = r22 - m.r22;
	sum.u2 = 0.0f;

	sum.tx = tx - m.tx;
	sum.ty = ty - m.ty;
	sum.tz = tz - m.tz;
	sum.w = 0.0f;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Graphics4x4::operator+(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.r00 = r00 + m.r00;
	sum.r01 = r01 + m.r01;
	sum.r02 = r02 + m.r02;
	sum.u0 = 0.0f;

	sum.r10 = r10 + m.r10;
	sum.r11 = r11 + m.r11;
	sum.r12 = r12 + m.r12;
	sum.u1 = 0.0f;

	sum.r20 = r20 + m.r20;
	sum.r21 = r21 + m.r21;
	sum.r22 = r22 + m.r22;
	sum.u2 = 0.0f;

	sum.tx = tx + m.tx;
	sum.ty = ty + m.ty;
	sum.tz = tz + m.tz;
	sum.w = 2.0f;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Graphics4x4::operator-(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.r00 = r00 - m.r00;
	sum.r01 = r01 - m.r01;
	sum.r02 = r02 - m.r02;
	sum.u0 = 0.0f;

	sum.r10 = r10 - m.r10;
	sum.r11 = r11 - m.r11;
	sum.r12 = r12 - m.r12;
	sum.u1 = 0.0f;

	sum.r20 = r20 - m.r20;
	sum.r21 = r21 - m.r21;
	sum.r22 = r22 - m.r22;
	sum.u2 = 0.0f;

	sum.tx = tx - m.tx;
	sum.ty = ty - m.ty;
	sum.tz = tz - m.tz;
	sum.w = 0.0f;

	return sum;
}

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

/*** NOTE: this routine is provided so that new platform can get up and running without implementing ***/
/*** a platform-specific version of this. this generic version is probably NOT fast!!!               ***/

inline Graphics4x4 &Graphics4x4::FastMultiply(const Graphics4x4 &m1, const Graphics4x4 &m2)	// this = m1 * m2
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_FAST_MULTIPLY);
#endif //MATH_STATS

	Graphics4x4 product;

	/* must handle this == m1 or this == m2, so can't calculate directly into this */

	// Make some assumptions, e3, e7, r11 are 0.  r15 is 1
	product.r00 = m1.r00 * m2.r00 + m1.r01 * m2.r10 + m1.r02 * m2.r20;
	product.r01 = m1.r00 * m2.r01 + m1.r01 * m2.r11 + m1.r02 * m2.r21;
	product.r02 = m1.r00 * m2.r02 + m1.r01 * m2.r12 + m1.r02 * m2.r22;
	product.u0 = 0.0f;

	product.r10 = m1.r10 * m2.r00 + m1.r11 * m2.r10 + m1.r12 * m2.r20;
	product.r11 = m1.r10 * m2.r01 + m1.r11 * m2.r11 + m1.r12 * m2.r21;
	product.r12 = m1.r10 * m2.r02 + m1.r11 * m2.r12 + m1.r12 * m2.r22;
	product.u1 = 0.0f;

	product.r20 = m1.r20 * m2.r00 + m1.r21 * m2.r10 + m1.r22 * m2.r20;
	product.r21 = m1.r20 * m2.r01 + m1.r21 * m2.r11 + m1.r22 * m2.r21;
	product.r22 = m1.r20 * m2.r02 + m1.r21 * m2.r12 + m1.r22 * m2.r22;
	product.u2 = 0.0f;

	product.tx = m1.tx * m2.r00 + m1.ty * m2.r10 + m1.tz * m2.r20 + m2.tx;
	product.ty = m1.tx * m2.r01 + m1.ty * m2.r11 + m1.tz * m2.r21 + m2.ty;
	product.tz = m1.tx * m2.r02 + m1.ty * m2.r12 + m1.tz * m2.r22 + m2.tz;
	product.w = 1.0f;

	// copy it in
	*this = product;
	return *this;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Graphics4x4::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_M4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Matrix4x4 product;

	product.e00 = r00 * m.e00 + r01 * m.e10 + r02 * m.e20;
	product.e01 = r00 * m.e01 + r01 * m.e11 + r02 * m.e21;
	product.e02 = r00 * m.e02 + r01 * m.e12 + r02 * m.e22;
	product.e03 = r00 * m.e03 + r01 * m.e13 + r02 * m.e23;

	product.e10 = r10 * m.e00 + r11 * m.e10 + r12 * m.e20;
	product.e11 = r10 * m.e01 + r11 * m.e11 + r12 * m.e21;
	product.e12 = r10 * m.e02 + r11 * m.e12 + r12 * m.e22;
	product.e13 = r10 * m.e03 + r11 * m.e13 + r12 * m.e23;

	product.e20 = r20 * m.e00 + r21 * m.e10 + r22 * m.e20;
	product.e21 = r20 * m.e01 + r21 * m.e11 + r22 * m.e21;
	product.e22 = r20 * m.e02 + r21 * m.e12 + r22 * m.e22;
	product.e23 = r20 * m.e03 + r21 * m.e13 + r22 * m.e23;

	product.e30 = tx * m.e00 + ty * m.e10 + tz * m.e20 + m.e30;
	product.e31 = tx * m.e01 + ty * m.e11 + tz * m.e21 + m.e31;
	product.e32 = tx * m.e02 + ty * m.e12 + tz * m.e22 + m.e32;
	product.e33 = tx * m.e03 + ty * m.e13 + tz * m.e23 + m.e33;

	return product;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Graphics4x4 Graphics4x4::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

	Graphics4x4 product;

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	// Make some assumptions because of u0, u1, u2 and w
	product.r00 = r00 * m.r00 + r01 * m.r10 + r02 * m.r20;
	product.r01 = r00 * m.r01 + r01 * m.r11 + r02 * m.r21;
	product.r02 = r00 * m.r02 + r01 * m.r12 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r10 + r12 * m.r20;
	product.r11 = r10 * m.r01 + r11 * m.r11 + r12 * m.r21;
	product.r12 = r10 * m.r02 + r11 * m.r12 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r10 + r22 * m.r20;
	product.r21 = r20 * m.r01 + r21 * m.r11 + r22 * m.r21;
	product.r22 = r20 * m.r02 + r21 * m.r12 + r22 * m.r22;
	product.u2 = 0.0f;

	product.tx = tx * m.r00 + ty * m.r10 + tz * m.r20 + m.tx;
	product.ty = tx * m.r01 + ty * m.r11 + tz * m.r21 + m.ty;
	product.tz = tx * m.r02 + ty * m.r12 + tz * m.r22 + m.tz;
	product.w = 1.0f;

	return product;
}

inline Graphics4x4 Graphics4x4::operator*(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

	Graphics4x4 product;

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	// Make some assumptions because of u0, u1, u2 and w
	product.r00 = r00 * m.r00 + r01 * m.r10 + r02 * m.r20;
	product.r01 = r00 * m.r01 + r01 * m.r11 + r02 * m.r21;
	product.r02 = r00 * m.r02 + r01 * m.r12 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r10 + r12 * m.r20;
	product.r11 = r10 * m.r01 + r11 * m.r11 + r12 * m.r21;
	product.r12 = r10 * m.r02 + r11 * m.r12 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r10 + r22 * m.r20;
	product.r21 = r20 * m.r01 + r21 * m.r11 + r22 * m.r21;
	product.r22 = r20 * m.r02 + r21 * m.r12 + r22 * m.r22;
	product.u2 = 0.0f;

	product.tx = tx * m.r00 + ty * m.r10 + tz * m.r20 + m.tx;
	product.ty = tx * m.r01 + ty * m.r11 + tz * m.r21 + m.ty;
	product.tz = tx * m.r02 + ty * m.r12 + tz * m.r22 + m.tz;
	product.w = 1.0f;

	return product;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Graphics4x4 &Graphics4x4::operator*=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

	Graphics4x4 product;

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	// Make some assumptions, e3, e7, r11 are 0.  r15 is 1
	product.r00 = r00 * m.r00 + r01 * m.r10 + r02 * m.r20;
	product.r01 = r00 * m.r01 + r01 * m.r11 + r02 * m.r21;
	product.r02 = r00 * m.r02 + r01 * m.r12 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r10 + r12 * m.r20;
	product.r11 = r10 * m.r01 + r11 * m.r11 + r12 * m.r21;
	product.r12 = r10 * m.r02 + r11 * m.r12 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r10 + r22 * m.r20;
	product.r21 = r20 * m.r01 + r21 * m.r11 + r22 * m.r21;
	product.r22 = r20 * m.r02 + r21 * m.r12 + r22 * m.r22;
	product.u2 = 0.0f;

	product.tx = tx * m.r00 + ty * m.r10 + tz * m.r20 + m.tx;
	product.ty = tx * m.r01 + ty * m.r11 + tz * m.r21 + m.ty;
	product.tz = tx * m.r02 + ty * m.r12 + tz * m.r22 + m.tz;
	product.w = 1.0f;

	// copy it in
	*this = product;
	return *this;
}

inline Graphics4x4 &Graphics4x4::operator*=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

	Graphics4x4 product;

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	// Make some assumptions, e3, e7, r11 are 0.  r15 is 1
	product.r00 = r00 * m.r00 + r01 * m.r10 + r02 * m.r20;
	product.r01 = r00 * m.r01 + r01 * m.r11 + r02 * m.r21;
	product.r02 = r00 * m.r02 + r01 * m.r12 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r10 + r12 * m.r20;
	product.r11 = r10 * m.r01 + r11 * m.r11 + r12 * m.r21;
	product.r12 = r10 * m.r02 + r11 * m.r12 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r10 + r22 * m.r20;
	product.r21 = r20 * m.r01 + r21 * m.r11 + r22 * m.r21;
	product.r22 = r20 * m.r02 + r21 * m.r12 + r22 * m.r22;
	product.u2 = 0.0f;

	product.tx = tx * m.r00 + ty * m.r10 + tz * m.r20 + m.tx;
	product.ty = tx * m.r01 + ty * m.r11 + tz * m.r21 + m.ty;
	product.tz = tx * m.r02 + ty * m.r12 + tz * m.r22 + m.tz;
	product.w = 1.0f;

	// copy it in
	*this = product;
	return *this;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 Graphics4x4::operator/(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DIVIDE_C4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 product;
	float xTerm, yTerm, zTerm;

	product.r00 = r00 * m.r00 + r01 * m.r01 + r02 * m.r02;
	product.r01 = r00 * m.r10 + r01 * m.r11 + r02 * m.r12;
	product.r02 = r00 * m.r20 + r01 * m.r21 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r01 + r12 * m.r02;
	product.r11 = r10 * m.r10 + r11 * m.r11 + r12 * m.r12;
	product.r12 = r10 * m.r20 + r11 * m.r21 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r01 + r22 * m.r02;
	product.r21 = r20 * m.r10 + r21 * m.r11 + r22 * m.r12;
	product.r22 = r20 * m.r20 + r21 * m.r21 + r22 * m.r22;
	product.u2 = 0.0f;

	xTerm = tx - m.tx;
	yTerm = ty - m.ty;
	zTerm = tz - m.tz;
	product.tx = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.ty = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.tz = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 &Graphics4x4::operator/=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DIVIDE_EQUALS_C4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 product;
	float xTerm, yTerm, zTerm;

	product.r00 = r00 * m.r00 + r01 * m.r01 + r02 * m.r02;
	product.r01 = r00 * m.r10 + r01 * m.r11 + r02 * m.r12;
	product.r02 = r00 * m.r20 + r01 * m.r21 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r01 + r12 * m.r02;
	product.r11 = r10 * m.r10 + r11 * m.r11 + r12 * m.r12;
	product.r12 = r10 * m.r20 + r11 * m.r21 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r01 + r22 * m.r02;
	product.r21 = r20 * m.r10 + r21 * m.r11 + r22 * m.r12;
	product.r22 = r20 * m.r20 + r21 * m.r21 + r22 * m.r22;
	product.u2 = 0.0f;

	xTerm = tx - m.tx;
	yTerm = ty - m.ty;
	zTerm = tz - m.tz;
	product.tx = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.ty = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.tz = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.w = 1.0f;

	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 Graphics4x4::operator*(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_M3X3);
#endif //MATH_STATS

	Graphics4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e10 + r02 * m.e20;
	product.r01 = r00 * m.e01 + r01 * m.e11 + r02 * m.e21;
	product.r02 = r00 * m.e02 + r01 * m.e12 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e10 + r12 * m.e20;
	product.r11 = r10 * m.e01 + r11 * m.e11 + r12 * m.e21;
	product.r12 = r10 * m.e02 + r11 * m.e12 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e10 + r22 * m.e20;
	product.r21 = r20 * m.e01 + r21 * m.e11 + r22 * m.e21;
	product.r22 = r20 * m.e02 + r21 * m.e12 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e10 + tz * m.e20;
	product.ty = tx * m.e01 + ty * m.e11 + tz * m.e21;
	product.tz = tx * m.e02 + ty * m.e12 + tz * m.e22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 &Graphics4x4::operator*=(
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_M3X3);
#endif //MATH_STATS

	Graphics4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e10 + r02 * m.e20;
	product.r01 = r00 * m.e01 + r01 * m.e11 + r02 * m.e21;
	product.r02 = r00 * m.e02 + r01 * m.e12 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e10 + r12 * m.e20;
	product.r11 = r10 * m.e01 + r11 * m.e11 + r12 * m.e21;
	product.r12 = r10 * m.e02 + r11 * m.e12 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e10 + r22 * m.e20;
	product.r21 = r20 * m.e01 + r21 * m.e11 + r22 * m.e21;
	product.r22 = r20 * m.e02 + r21 * m.e12 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e10 + tz * m.e20;
	product.ty = tx * m.e01 + ty * m.e11 + tz * m.e21;
	product.tz = tx * m.e02 + ty * m.e12 + tz * m.e22;
	product.w = 1.0f;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 Graphics4x4::operator*(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_M3X3);
#endif //MATH_STATS

	Graphics4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e10 + r02 * m.e20;
	product.r01 = r00 * m.e01 + r01 * m.e11 + r02 * m.e21;
	product.r02 = r00 * m.e02 + r01 * m.e12 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e10 + r12 * m.e20;
	product.r11 = r10 * m.e01 + r11 * m.e11 + r12 * m.e21;
	product.r12 = r10 * m.e02 + r11 * m.e12 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e10 + r22 * m.e20;
	product.r21 = r20 * m.e01 + r21 * m.e11 + r22 * m.e21;
	product.r22 = r20 * m.e02 + r21 * m.e12 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e10 + tz * m.e20;
	product.ty = tx * m.e01 + ty * m.e11 + tz * m.e21;
	product.tz = tx * m.e02 + ty * m.e12 + tz * m.e22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 &Graphics4x4::operator*=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_M3X3);
#endif //MATH_STATS

	Graphics4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e10 + r02 * m.e20;
	product.r01 = r00 * m.e01 + r01 * m.e11 + r02 * m.e21;
	product.r02 = r00 * m.e02 + r01 * m.e12 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e10 + r12 * m.e20;
	product.r11 = r10 * m.e01 + r11 * m.e11 + r12 * m.e21;
	product.r12 = r10 * m.e02 + r11 * m.e12 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e10 + r22 * m.e20;
	product.r21 = r20 * m.e01 + r21 * m.e11 + r22 * m.e21;
	product.r22 = r20 * m.e02 + r21 * m.e12 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e10 + tz * m.e20;
	product.ty = tx * m.e01 + ty * m.e11 + tz * m.e21;
	product.tz = tx * m.e02 + ty * m.e12 + tz * m.e22;
	product.w = 1.0f;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 Graphics4x4::operator/(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_D3X3);
#endif //MATH_STATS

	Graphics4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e01 + r02 * m.e02;
	product.r01 = r00 * m.e10 + r01 * m.e11 + r02 * m.e12;
	product.r02 = r00 * m.e20 + r01 * m.e21 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e01 + r12 * m.e02;
	product.r11 = r10 * m.e10 + r11 * m.e11 + r12 * m.e12;
	product.r12 = r10 * m.e20 + r11 * m.e21 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e01 + r22 * m.e02;
	product.r21 = r20 * m.e10 + r21 * m.e11 + r22 * m.e12;
	product.r22 = r20 * m.e20 + r21 * m.e21 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e01 + tz * m.e02;
	product.ty = tx * m.e10 + ty * m.e11 + tz * m.e12;
	product.tz = tx * m.e20 + ty * m.e21 + tz * m.e22;
	product.w = 1.0f;

	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 &Graphics4x4::operator/=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_EQUALS_D3X3);
#endif //MATH_STATS

	Graphics4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e01 + r02 * m.e02;
	product.r01 = r00 * m.e10 + r01 * m.e11 + r02 * m.e12;
	product.r02 = r00 * m.e20 + r01 * m.e21 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e01 + r12 * m.e02;
	product.r11 = r10 * m.e10 + r11 * m.e11 + r12 * m.e12;
	product.r12 = r10 * m.e20 + r11 * m.e21 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e01 + r22 * m.e02;
	product.r21 = r20 * m.e10 + r21 * m.e11 + r22 * m.e12;
	product.r22 = r20 * m.e20 + r21 * m.e21 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e01 + tz * m.e02;
	product.ty = tx * m.e10 + ty * m.e11 + tz * m.e12;
	product.tz = tx * m.e20 + ty * m.e21 + tz * m.e22;
	product.w = 1.0f;

	*this = product;
	return *this;
}

/***************************************************************************/
// Transform homogenous x y z 1 with m
/***************************************************************************/
inline Vector3 Graphics4x4::Transform(
float x,
float y,
float z) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TRANSFORM_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float rx = x * r00 + y * r10 + z * r20 + tx;
	float ry = x * r01 + y * r11 + z * r21 + ty;
	float rz = x * r02 + y * r12 + z * r22 + tz;

	return Vector3(rx, ry, rz);
}

/***************************************************************************/
// Create a scale matrix (overwrites existing matrix, not cat'ed)
/***************************************************************************/
inline void Graphics4x4::SetScale(
float xScale,
float yScale,
float zScale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_SCALE_F);
#endif //MATH_STATS

	r00 = xScale;
	r01 = 0.0f;
	r02 = 0.0f;
	u0 = 0.0f;

	r10 = 0.0f;
	r11 = yScale;
	r12 = 0.0f;
	u1 = 0.0f;

	r20 = 0.0f;
	r21 = 0.0f;
	r22 = zScale;
	u2 = 0.0f;

	tx = 0.0f;
	ty = 0.0f;
	tz = 0.0f;
	w = 1.0f;
}

/***************************************************************************/
// Simple (sets other scale values to 1)
/***************************************************************************/
inline void Graphics4x4::SetXScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_XSCALE_F);
#endif //MATH_STATS

	SetScale(scale, 1.0f, 1.0f);
}

/***************************************************************************/
// Simple (sets other scale values to 1)
/***************************************************************************/
inline void Graphics4x4::SetYScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_YSCALE_F);
#endif //MATH_STATS

	SetScale(1.0f, scale, 1.0f);
}

/***************************************************************************/
// Simple (sets other scale values to 1)
/***************************************************************************/
inline void Graphics4x4::SetZScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_ZSCALE_F);
#endif //MATH_STATS

	SetScale(1.0f, 1.0f, scale);
}

/***************************************************************************/
// Do rotations around x,y,z axes.  Does not cat, sets
/***************************************************************************/
inline void Graphics4x4::SetXRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_XROTATION_F);
#endif //MATH_STATS

	float s, c;
	Math::SinCos(radians, s, c);

	r00 = 1.0f;
	r01 = 0.0f;
	r02 = 0.0f;
	u0 = 0.0f;

	r10 = 0.0f;
	r11 = c;
	r12 = s;
	u1 = 0.0f;

	r20 = 0.0f;
	r21 = -s;
	r22 = c;
	u2 = 0.0f;

	tx = 0.0f;
	ty = 0.0f;
	tz = 0.0f;
	w = 1.0f;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::SetYRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_YROTATION_F);
#endif //MATH_STATS

	float s, c;
	Math::SinCos(radians, s, c);

	r00 = c;
	r01 = 0.0f;
	r02 = -s;
	u0 = 0.0f;

	r10 = 0.0f;
	r11 = 1.0f;
	r12 = 0.0f;
	u1 = 0.0f;

	r20 = s;
	r21 = 0.0f;
	r22 = c;
	u2 = 0.0f;

	tx = 0.0f;
	ty = 0.0f;
	tz = 0.0f;
	w = 1.0f;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::SetZRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_ZROTATION_F);
#endif //MATH_STATS

	float s, c;
	Math::SinCos(radians, s, c);

	r00 = c;
	r01 = s;
	r02 = 0.0f;
	u0 = 0.0f;

	r10 = -s;
	r11 = c;
	r12 = 0.0f;
	u1 = 0.0f;

	r20 = 0.0f;
	r21 = 0.0f;
	r22 = 1.0f;
	u2 = 0.0f;

	tx = 0.0f;
	ty = 0.0f;
	tz = 0.0f;
	w = 1.0f;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::SetXYZRotation(
Vector3CRef vRadians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_XYZROTATION_V3);
#endif //MATH_STATS

	Graphics4x4 mX;
	Graphics4x4 mY;
	Graphics4x4 mZ;
	Graphics4x4 mTemp;

	mX.SetXRotation(vRadians.x());
	mY.SetYRotation(vRadians.y());
	mZ.SetZRotation(vRadians.z());
//xxx	mTemp = mX * mY;
//xxx	*this = mZ * mTemp;
	mTemp.FastMultiply(mX, mY);
	this->FastMultiply(mZ, mTemp);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::SetXZYRotation(
Vector3CRef vRadians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_XYZROTATION_V3);
#endif //MATH_STATS

	Graphics4x4 mX;
	Graphics4x4 mY;
	Graphics4x4 mZ;
	Graphics4x4 mTemp;

	mX.SetXRotation(vRadians.x());
	mY.SetYRotation(vRadians.y());
	mZ.SetZRotation(vRadians.z());
//xxx	mTemp = mX * mY;
//xxx	*this = mZ * mTemp;
	mTemp.FastMultiply(mX, mZ);
	this->FastMultiply(mY, mTemp);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::SetXYZRotationDeg(
Vector3CRef vDegrees)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_XYZROTATION_DEG_V3);
#endif //MATH_STATS

	Graphics4x4 mX;
	Graphics4x4 mY;
	Graphics4x4 mZ;
	Graphics4x4 mTemp;

	mX.SetXRotation(Math::Deg2Rad(vDegrees.x()));
	mY.SetYRotation(Math::Deg2Rad(vDegrees.y()));
	mZ.SetZRotation(Math::Deg2Rad(vDegrees.z()));
//xxx	mTemp = mX * mY;
//xxx	*this = mZ * mTemp;
	mTemp.FastMultiply(mX, mY);
	this->FastMultiply(mZ, mTemp);
}

/***************************************************************************/
// Translation
/***************************************************************************/
inline void Graphics4x4::SetTranslation(
float x,
float y,
float z)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_TRANSLATION_F);
#endif //MATH_STATS

	r00 = 1.0f;
	r01 = 0.0f;
	r02 = 0.0f;
	u0 = 0.0f;

	r10 = 0.0f;
	r11 = 1.0f;
	r12 = 0.0f;
	u1 = 0.0f;

	r20 = 0.0f;
	r21 = 0.0f;
	r22 = 1.0f;
	u2 = 0.0f;

	tx = x;
	ty = y;
	tz = z;
	w = 1.0f;
}

/***************************************************************************/
// Translation
/***************************************************************************/
inline void Graphics4x4::SetTranslation(
Vector3CRef t)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_TRANSLATION_V3);
#endif //MATH_STATS

	SetTranslation(t.x(), t.y(), t.z());
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::SetXTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_XTRANSLATION_F);
#endif //MATH_STATS

	r00 = 1.0f;
	r01 = 0.0f;
	r02 = 0.0f;
	u0 = 0.0f;

	r10 = 0.0f;
	r11 = 1.0f;
	r12 = 0.0f;
	u1 = 0.0f;

	r20 = 0.0f;
	r21 = 0.0f;
	r22 = 1.0f;
	u2 = 0.0f;

	tx = pos;
	ty = 0.0f;
	tz = 0.0f;
	w = 1.0f;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::SetYTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_YTRANSLATION_F);
#endif //MATH_STATS

	r00 = 1.0f;
	r01 = 0.0f;
	r02 = 0.0f;
	u0 = 0.0f;

	r10 = 0.0f;
	r11 = 1.0f;
	r12 = 0.0f;
	u1 = 0.0f;

	r20 = 0.0f;
	r21 = 0.0f;
	r22 = 1.0f;
	u2 = 0.0f;

	tx = 0.0f;
	ty = pos;
	tz = 0.0f;
	w = 1.0f;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::SetZTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_SET_ZTRANSLATION_F);
#endif //MATH_STATS

	r00 = 1.0f;
	r01 = 0.0f;
	r02 = 0.0f;
	u0 = 0.0f;

	r10 = 0.0f;
	r11 = 1.0f;
	r12 = 0.0f;
	u1 = 0.0f;

	r20 = 0.0f;
	r21 = 0.0f;
	r22 = 1.0f;
	u2 = 0.0f;

	tx = 0.0f;
	ty = 0.0f;
	tz = pos;
	w = 1.0f;
}

/***************************************************************************/
// Poke translation
/***************************************************************************/
inline void Graphics4x4::PokeTranslation(
float x,
float y,
float z)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_POKE_TRANSLATION_F);
#endif //MATH_STATS

	tx = x;
	ty = y;
	tz = z;
}

/***************************************************************************/
// Poke translation
/***************************************************************************/
inline void Graphics4x4::PokeTranslation(
Vector3CRef t)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_POKE_TRANSLATION_V3);
#endif //MATH_STATS

	tx = t.x();
	ty = t.y();
	tz = t.z();
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::PokeXTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_POKE_XTRANSLATION_F);
#endif //MATH_STATS

	tx = pos;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::PokeYTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_POKE_YTRANSLATION_F);
#endif //MATH_STATS

	ty = pos;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::PokeZTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_POKE_ZTRANSLATION_F);
#endif //MATH_STATS

	tz = pos;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatScale(
float xScale,
float yScale,
float zScale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_SCALE_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 scale;
	scale.SetScale(xScale, yScale, zScale);
	*this *= scale;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatXScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_XSCALE_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 scaleM;
	scaleM.SetXScale(scale);
	*this *= scaleM;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatYScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_YSCALE_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 scaleM;
	scaleM.SetYScale(scale);
	*this *= scaleM;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatZScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_ZSCALE_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 scaleM;
	scaleM.SetZScale(scale);
	*this *= scaleM;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatXRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_XROTATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 rot;
	rot.SetXRotation(radians);
	*this *= rot;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatYRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_YROTATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 rot;
	rot.SetYRotation(radians);
	*this *= rot;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatZRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_ZROTATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 rot;
	rot.SetZRotation(radians);
	*this *= rot;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatTranslation(
float x,
float y,
float z)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_TRANSLATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 trans;
	trans.SetTranslation(x, y, z);
	*this *= trans;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatXTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_XTRANSLATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 trans;
	trans.SetXTranslation(pos);
	*this *= trans;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatYTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_YTRANSLATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 trans;
	trans.SetYTranslation(pos);
	*this *= trans;
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::CatZTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_CAT_ZTRANSLATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 trans;
	trans.SetZTranslation(pos);
	*this *= trans;
}

/***************************************************************************/
//Reverse cat's
/***************************************************************************/
inline void Graphics4x4::ReverseCatScale(
float xScale,
float yScale,
float zScale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_SCALE_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 scaleM;
	scaleM.SetScale(xScale, yScale, zScale);
//xxx	*this = scale * *this;
	this->FastMultiply(scaleM, *this);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::ReverseCatXScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_XSCALE_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 scaleM;
	scaleM.SetXScale(scale);
//xxx	*this = scaleM * *this;
	this->FastMultiply(scaleM, *this);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::ReverseCatYScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_YSCALE_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 scaleM;
	scaleM.SetYScale(scale);
//xxx	*this = scaleM * *this;
	this->FastMultiply(scaleM, *this);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::ReverseCatZScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_ZSCALE_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 scaleM;
	scaleM.SetZScale(scale);
//xxx	*this = scaleM * *this;
	this->FastMultiply(scaleM, *this);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::ReverseCatXRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_XROTATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 rot;
	rot.SetXRotation(radians);
//xxx	*this = rot * *this;
	this->FastMultiply(rot, *this);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::ReverseCatYRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_YROTATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 rot;
	rot.SetYRotation(radians);
//xxx	*this = rot * *this;
	this->FastMultiply(rot, *this);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::ReverseCatZRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_ZROTATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 rot;
	rot.SetZRotation(radians);
//xxx	*this = rot * *this;
	this->FastMultiply(rot, *this);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::ReverseCatTranslation(
float x,
float y,
float z)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_TRANSLATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 trans;
	trans.SetTranslation(x, y, z);
//xxx	*this = trans * *this;
	this->FastMultiply(trans, *this);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::ReverseCatXTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_XTRANSLATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 trans;
	trans.SetXTranslation(pos);
//xxx	*this = trans * *this;
	this->FastMultiply(trans, *this);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::ReverseCatYTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_YTRANSLATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 trans;
	trans.SetYTranslation(pos);
//xxx	*this = trans * *this;
	this->FastMultiply(trans, *this);
}

/***************************************************************************/
/***************************************************************************/
inline void Graphics4x4::ReverseCatZTranslation(
float pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_REVERSE_CAT_ZTRANSLATION_F);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Graphics4x4 trans;
	trans.SetZTranslation(pos);
//xxx	*this = trans * *this;
	this->FastMultiply(trans, *this);
}

/***************************************************************************/
// Get the 3x3 equivalent
/***************************************************************************/
inline void Graphics4x4::GetOrientation(
Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_GET_ORIENTATION_M3X3);
#endif //MATH_STATS

	m.e00 = r00;
	m.e01 = r01;
	m.e02 = r02;

	m.e10 = r10;
	m.e11 = r11;
	m.e12 = r12;

	m.e20 = r20;
	m.e21 = r21;
	m.e22 = r22;
}

/***************************************************************************/
// Set the 3x3 equivalent
/***************************************************************************/
inline void Graphics4x4::PokeOrientation(
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_POKE_ORIENTATION_M3X3);
#endif //MATH_STATS

	r00 = m.e00;
	r01 = m.e01;
	r02 = m.e02;

	r10 = m.e10;
	r11 = m.e11;
	r12 = m.e12;

	r20 = m.e20;
	r21 = m.e21;
	r22 = m.e22;
}

/***************************************************************************/
// Compose/decompose from a position orientation
/***************************************************************************/
inline void Graphics4x4::Compose(
const Matrix3x3 &o,
Vector3CRef pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_COMPOSE_M3X3_V3);
#endif //MATH_STATS

	r00 = o.e00;
	r01 = o.e01;
	r02 = o.e02;
	u0 = 0.0f;

	r10 = o.e10;
	r11 = o.e11;
	r12 = o.e12;
	u1 = 0.0f;

	r20 = o.e20;
	r21 = o.e21;
	r22 = o.e22;
	u2 = 0.0f;

	tx = pos.x();
	ty = pos.y();
	tz = pos.z();
	w = 1.0f;
}

/***************************************************************************/
// Compose/decompose from a position orientation
/***************************************************************************/
inline void Graphics4x4::Decompose(
Matrix3x3 &o,
Vector3 &pos) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DECOMPOSE_M3X3_V3);
#endif //MATH_STATS

	o.e00 = r00;
	o.e01 = r01;
	o.e02 = r02;

	o.e10 = r10;
	o.e11 = r11;
	o.e12 = r12;

	o.e20 = r20;
	o.e21 = r21;
	o.e22 = r22;

	pos.Set(tx, ty, tz);
}

/***************************************************************************/
// construct the matrix from rotation and position
/***************************************************************************/
inline Cartesian4x4::Cartesian4x4(
const DirCos3x3 &m,
Vector3CRef pos)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_CONSTRUCT_M3X3_V3);
#endif //MATH_STATS

	r00 = m.e00;
	r01 = m.e01;
	r02 = m.e02;
	u0 = 0.0f;

	r10 = m.e10;
	r11 = m.e11;
	r12 = m.e12;
	u1 = 0.0f;

	r20 = m.e20;
	r21 = m.e21;
	r22 = m.e22;
	u2 = 0.0f;

  	tx = pos.x();
  	ty = pos.y();
	tz = pos.z();
	w = 1.0f;
}

/***************************************************************************/
// Assign the matrix
/***************************************************************************/
inline Cartesian4x4 &Cartesian4x4::operator=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_ASSIGN_M4X4);
#endif //MATH_STATS

	r00 = m.r00;
	r01 = m.r01;
	r02 = m.r02;
	u0 = m.u0;

	r10 = m.r10;
	r11 = m.r11;
	r12 = m.r12;
	u1 = m.u1;

	r20 = m.r20;
	r21 = m.r21;
	r22 = m.r22;
	u2 = m.u2;

	tx = m.tx;
	ty = m.ty;
	tz = m.tz;
	w = m.w;

	return *this;
}

/***************************************************************************/
// Set the 3x3 into the orientation area
/***************************************************************************/
inline Cartesian4x4 &Cartesian4x4::operator=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_ASSIGN_M3X3);
#endif //MATH_STATS

	r00 = m.e00;
	r01 = m.e01;
	r02 = m.e02;
	u0 = 0.0f;

	r10 = m.e10;
	r11 = m.e11;
	r12 = m.e12;
	u1 = 0.0f;

	r20 = m.e20;
	r21 = m.e21;
	r22 = m.e22;
	u2 = 0.0f;

  	tx = 0.0f;
  	ty = 0.0f;
	tz = 0.0f;
	w = 1.0f;

	return *this;
}

/***************************************************************************/
// Invert, always works. (it's the transpose and translation transformation)
// Watch out, this function is not virtual
/***************************************************************************/
inline bool Cartesian4x4::Invert(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	// invert it
	Cartesian4x4 inverse;
	if (!Invert(inverse))
		return(false);

	// save it
	*this = inverse;
	return true;
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Cartesian4x4::operator==(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_EQUALS_G4X4);
#endif //MATH_STATS

	return (r00 == m.e00 && r01 == m.e01 && r02 == m.e02 && m.e03 == 0.0f &&
				r10 == m.e10 && r11 == m.e11 &&	r12 == m.e12 && m.e13 == 0.0f &&
				r20 == m.e20 && r21 == m.e21 && r22 == m.r22 && m.e23 == 0.0f &&
				tx == m.tx && ty == m.ty && m.tz == tz && m.e33 == 1.0f);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Cartesian4x4::operator!=(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_NOT_EQUAL_G4X4);
#endif //MATH_STATS

	return (r00 != m.e00 || r01 != m.e01 || r02 != m.e02 || m.e03 != 0.0f ||
				r10 != m.e10 || r11 != m.e11 ||	r12 != m.e12 || m.e13 != 0.0f ||
				r20 != m.e20 || r21 != m.e21 || r22 != m.r22 || m.e23 != 0.0f ||
				tx != m.tx || ty != m.ty || m.tz != tz || m.e33 != 1.0f);
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Cartesian4x4::operator==(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_EQUALS_G4X4);
#endif //MATH_STATS

	return (r00 == m.r00 && r01 == m.r01 && r02 == m.r02 &&
				r10 == m.r10 && r11 == m.r11 &&	r12 == m.r12 &&
				r20 == m.r20 && r21 == m.r21 && r22 == m.r22 &&
				tx == m.tx && ty == m.ty && m.tz == tz);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Cartesian4x4::operator!=(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_NOT_EQUAL_G4X4);
#endif //MATH_STATS

	return (r00 != m.r00 || r01 != m.r01 || r02 != m.r02 ||
				r10 != m.r10 || r11 != m.r11 || r12 != m.r12 ||
				r20 != m.r20 || r21 != m.r21 || r22 != m.r22 ||
				tx != m.tx || ty != m.ty || m.tz != tz);
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Cartesian4x4::operator==(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_EQUALS_G4X4);
#endif //MATH_STATS

	return (r00 == m.r00 && r01 == m.r01 && r02 == m.r02 &&
				r10 == m.r10 && r11 == m.r11 &&	r12 == m.r12 &&
				r20 == m.r20 && r21 == m.r21 && r22 == m.r22 &&
				tx == m.tx && ty == m.ty && m.tz == tz);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Cartesian4x4::operator!=(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_NOT_EQUAL_G4X4);
#endif //MATH_STATS

	return (r00 != m.r00 || r01 != m.r01 || r02 != m.r02 ||
				r10 != m.r10 || r11 != m.r11 || r12 != m.r12 ||
				r20 != m.r20 || r21 != m.r21 || r22 != m.r22 ||
				tx != m.tx || ty != m.ty || m.tz != tz);
}


/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Cartesian4x4::operator*(
float s) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_F);
#endif //MATH_STATS

	Matrix4x4 m;

	m.e00 = r00 * s;
	m.e01 = r01 * s;
	m.e02 = r02 * s;
	m.e03 = 0.0f;

	m.e10 = r10 * s;
	m.e11 = r11 * s;
	m.e12 = r12 * s;
	m.e12 = 0.0f;

	m.e20 = r20 * s;
	m.e21 = r21 * s;
	m.e22 = r22 * s;
	m.e23 = 0.0f;

	m.e30 = tx * s;
	m.e31 = ty * s;
	m.e32 = tz * s;
	m.e33 = s;

	return m;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Cartesian4x4::operator/(
float s) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_F);
#endif //MATH_STATS

	Matrix4x4 m;

	float inv = 1.0f / s;
	m = *this * inv;

	return m;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Cartesian4x4::operator+(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.e00 = r00 + m.e00;
	sum.e01 = r01 + m.e01;
	sum.e02 = r02 + m.e02;
	sum.e03 = m.e03;

	sum.e10 = r10 + m.e10;
	sum.e11 = r11 + m.e11;
	sum.e12 = r12 + m.e12;
	sum.e13 = m.e13;

	sum.e20 = r20 + m.e20;
	sum.e21 = r21 + m.e21;
	sum.e22 = r22 + m.e22;
	sum.e23 = m.e23;

	sum.e30 = tx + m.e30;
	sum.e31 = ty + m.e31;
	sum.e32 = tz + m.e32;
	sum.e33 = 1.0f + m.e33;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Cartesian4x4::operator-(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.e00 = r00 - m.e00;
	sum.e01 = r01 - m.e01;
	sum.e02 = r02 - m.e02;
	sum.e03 = m.e03;

	sum.e10 = r10 - m.e10;
	sum.e11 = r11 - m.e11;
	sum.e12 = r12 - m.e12;
	sum.e13 = m.e13;

	sum.e20 = r20 - m.e20;
	sum.e21 = r21 - m.e21;
	sum.e22 = r22 - m.e22;
	sum.e23 = m.e23;

	sum.e30 = tx - m.e30;
	sum.e31 = ty - m.e31;
	sum.e32 = tz - m.e32;
	sum.e33 = 1.0f - m.e33;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Cartesian4x4::operator+(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.r00 = r00 + m.r00;
	sum.r01 = r01 + m.r01;
	sum.r02 = r02 + m.r02;
	sum.u0 = 0.0f;

	sum.r10 = r10 + m.r10;
	sum.r11 = r11 + m.r11;
	sum.r12 = r12 + m.r12;
	sum.u1 = 0.0f;

	sum.r20 = r20 + m.r20;
	sum.r21 = r21 + m.r21;
	sum.r22 = r22 + m.r22;
	sum.u2 = 0.0f;

	sum.tx = tx + m.tx;
	sum.ty = ty + m.ty;
	sum.tz = tz + m.tz;
	sum.w = 2.0f;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Cartesian4x4::operator-(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.r00 = r00 - m.r00;
	sum.r01 = r01 - m.r01;
	sum.r02 = r02 - m.r02;
	sum.u0 = 0.0f;

	sum.r10 = r10 - m.r10;
	sum.r11 = r11 - m.r11;
	sum.r12 = r12 - m.r12;
	sum.u1 = 0.0f;

	sum.r20 = r20 - m.r20;
	sum.r21 = r21 - m.r21;
	sum.r22 = r22 - m.r22;
	sum.u2 = 0.0f;

	sum.tx = tx - m.tx;
	sum.ty = ty - m.ty;
	sum.tz = tz - m.tz;
	sum.w = 0.0f;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Cartesian4x4::operator+(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_PLUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.r00 = r00 + m.r00;
	sum.r01 = r01 + m.r01;
	sum.r02 = r02 + m.r02;
	sum.u0 = 0.0f;

	sum.r10 = r10 + m.r10;
	sum.r11 = r11 + m.r11;
	sum.r12 = r12 + m.r12;
	sum.u1 = 0.0f;

	sum.r20 = r20 + m.r20;
	sum.r21 = r21 + m.r21;
	sum.r22 = r22 + m.r22;
	sum.u2 = 0.0f;

	sum.tx = tx + m.tx;
	sum.ty = ty + m.ty;
	sum.tz = tz + m.tz;
	sum.w = 2.0f;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Cartesian4x4::operator-(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_MINUS_M4X4);
#endif //MATH_STATS

	Matrix4x4 sum;

	sum.r00 = r00 - m.r00;
	sum.r01 = r01 - m.r01;
	sum.r02 = r02 - m.r02;
	sum.u0 = 0.0f;

	sum.r10 = r10 - m.r10;
	sum.r11 = r11 - m.r11;
	sum.r12 = r12 - m.r12;
	sum.u1 = 0.0f;

	sum.r20 = r20 - m.r20;
	sum.r21 = r21 - m.r21;
	sum.r22 = r22 - m.r22;
	sum.u2 = 0.0f;

	sum.tx = tx - m.tx;
	sum.ty = ty - m.ty;
	sum.tz = tz - m.tz;
	sum.w = 0.0f;

	return sum;
}

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Cartesian4x4::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_M4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Matrix4x4 product;

	product.e00 = r00 * m.e00 + r01 * m.e10 + r02 * m.e20;
	product.e01 = r00 * m.e01 + r01 * m.e11 + r02 * m.e21;
	product.e02 = r00 * m.e02 + r01 * m.e12 + r02 * m.e22;
	product.e03 = r00 * m.e03 + r01 * m.e13 + r02 * m.e23;

	product.e10 = r10 * m.e00 + r11 * m.e10 + r12 * m.e20;
	product.e11 = r10 * m.e01 + r11 * m.e11 + r12 * m.e21;
	product.e12 = r10 * m.e02 + r11 * m.e12 + r12 * m.e22;
	product.e13 = r10 * m.e03 + r11 * m.e13 + r12 * m.e23;

	product.e20 = r20 * m.e00 + r21 * m.e10 + r22 * m.e20;
	product.e21 = r20 * m.e01 + r21 * m.e11 + r22 * m.e21;
	product.e22 = r20 * m.e02 + r21 * m.e12 + r22 * m.e22;
	product.e23 = r20 * m.e03 + r21 * m.e13 + r22 * m.e23;

	product.e30 = tx * m.e00 + ty * m.e10 + tz * m.e20 + m.e30;
	product.e31 = tx * m.e01 + ty * m.e11 + tz * m.e21 + m.e31;
	product.e32 = tx * m.e02 + ty * m.e12 + tz * m.e22 + m.e32;
	product.e33 = tx * m.e03 + ty * m.e13 + tz * m.e23 + m.e33;

	return product;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Graphics4x4 Cartesian4x4::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

	Graphics4x4 product;

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	// Make some assumptions because of u0, u1, u2 and w
	product.r00 = r00 * m.r00 + r01 * m.r10 + r02 * m.r20;
	product.r01 = r00 * m.r01 + r01 * m.r11 + r02 * m.r21;
	product.r02 = r00 * m.r02 + r01 * m.r12 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r10 + r12 * m.r20;
	product.r11 = r10 * m.r01 + r11 * m.r11 + r12 * m.r21;
	product.r12 = r10 * m.r02 + r11 * m.r12 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r10 + r22 * m.r20;
	product.r21 = r20 * m.r01 + r21 * m.r11 + r22 * m.r21;
	product.r22 = r20 * m.r02 + r21 * m.r12 + r22 * m.r22;
	product.u2 = 0.0f;

	product.tx = tx * m.r00 + ty * m.r10 + tz * m.r20 + m.tx;
	product.ty = tx * m.r01 + ty * m.r11 + tz * m.r21 + m.ty;
	product.tz = tx * m.r02 + ty * m.r12 + tz * m.r22 + m.tz;
	product.w = 1.0f;

	return product;
}

inline Cartesian4x4 Cartesian4x4::operator*(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

	Cartesian4x4 product;

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	// Make some assumptions because of u0, u1, u2 and w
	product.r00 = r00 * m.r00 + r01 * m.r10 + r02 * m.r20;
	product.r01 = r00 * m.r01 + r01 * m.r11 + r02 * m.r21;
	product.r02 = r00 * m.r02 + r01 * m.r12 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r10 + r12 * m.r20;
	product.r11 = r10 * m.r01 + r11 * m.r11 + r12 * m.r21;
	product.r12 = r10 * m.r02 + r11 * m.r12 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r10 + r22 * m.r20;
	product.r21 = r20 * m.r01 + r21 * m.r11 + r22 * m.r21;
	product.r22 = r20 * m.r02 + r21 * m.r12 + r22 * m.r22;
	product.u2 = 0.0f;

	product.tx = tx * m.r00 + ty * m.r10 + tz * m.r20 + m.tx;
	product.ty = tx * m.r01 + ty * m.r11 + tz * m.r21 + m.ty;
	product.tz = tx * m.r02 + ty * m.r12 + tz * m.r22 + m.tz;
	product.w = 1.0f;

	return product;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifndef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Cartesian4x4 &Cartesian4x4::operator*=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

	Cartesian4x4 product;

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	// Make some assumptions, e3, e7, r11 are 0.  r15 is 1
	product.r00 = r00 * m.r00 + r01 * m.r10 + r02 * m.r20;
	product.r01 = r00 * m.r01 + r01 * m.r11 + r02 * m.r21;
	product.r02 = r00 * m.r02 + r01 * m.r12 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r10 + r12 * m.r20;
	product.r11 = r10 * m.r01 + r11 * m.r11 + r12 * m.r21;
	product.r12 = r10 * m.r02 + r11 * m.r12 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r10 + r22 * m.r20;
	product.r21 = r20 * m.r01 + r21 * m.r11 + r22 * m.r21;
	product.r22 = r20 * m.r02 + r21 * m.r12 + r22 * m.r22;
	product.u2 = 0.0f;

	product.tx = tx * m.r00 + ty * m.r10 + tz * m.r20 + m.tx;
	product.ty = tx * m.r01 + ty * m.r11 + tz * m.r21 + m.ty;
	product.tz = tx * m.r02 + ty * m.r12 + tz * m.r22 + m.tz;
	product.w = 1.0f;

	// copy it in
	*this = product;
	return *this;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 Cartesian4x4::operator/(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DIVIDE_C4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Cartesian4x4 product;
	float xTerm, yTerm, zTerm;

	product.r00 = r00 * m.r00 + r01 * m.r01 + r02 * m.r02;
	product.r01 = r00 * m.r10 + r01 * m.r11 + r02 * m.r12;
	product.r02 = r00 * m.r20 + r01 * m.r21 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r01 + r12 * m.r02;
	product.r11 = r10 * m.r10 + r11 * m.r11 + r12 * m.r12;
	product.r12 = r10 * m.r20 + r11 * m.r21 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r01 + r22 * m.r02;
	product.r21 = r20 * m.r10 + r21 * m.r11 + r22 * m.r12;
	product.r22 = r20 * m.r20 + r21 * m.r21 + r22 * m.r22;
	product.u2 = 0.0f;

	xTerm = tx - m.tx;
	yTerm = ty - m.ty;
	zTerm = tz - m.tz;
	product.tx = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.ty = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.tz = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 &Cartesian4x4::operator/=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_DIVIDE_EQUALS_C4X4);
#endif //MATH_STATS

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Cartesian4x4 product;
	float xTerm, yTerm, zTerm;

	product.r00 = r00 * m.r00 + r01 * m.r01 + r02 * m.r02;
	product.r01 = r00 * m.r10 + r01 * m.r11 + r02 * m.r12;
	product.r02 = r00 * m.r20 + r01 * m.r21 + r02 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.r00 + r11 * m.r01 + r12 * m.r02;
	product.r11 = r10 * m.r10 + r11 * m.r11 + r12 * m.r12;
	product.r12 = r10 * m.r20 + r11 * m.r21 + r12 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.r00 + r21 * m.r01 + r22 * m.r02;
	product.r21 = r20 * m.r10 + r21 * m.r11 + r22 * m.r12;
	product.r22 = r20 * m.r20 + r21 * m.r21 + r22 * m.r22;
	product.u2 = 0.0f;

	xTerm = tx - m.tx;
	yTerm = ty - m.ty;
	zTerm = tz - m.tz;
	product.tx = xTerm * m.r00 + yTerm * m.r01 + zTerm * m.r02;
	product.ty = xTerm * m.r10 + yTerm * m.r11 + zTerm * m.r12;
	product.tz = xTerm * m.r20 + yTerm * m.r21 + zTerm * m.r22;
	product.w = 1.0f;

	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 Cartesian4x4::operator*(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_M3X3);
#endif //MATH_STATS

	Graphics4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e10 + r02 * m.e20;
	product.r01 = r00 * m.e01 + r01 * m.e11 + r02 * m.e21;
	product.r02 = r00 * m.e02 + r01 * m.e12 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e10 + r12 * m.e20;
	product.r11 = r10 * m.e01 + r11 * m.e11 + r12 * m.e21;
	product.r12 = r10 * m.e02 + r11 * m.e12 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e10 + r22 * m.e20;
	product.r21 = r20 * m.e01 + r21 * m.e11 + r22 * m.e21;
	product.r22 = r20 * m.e02 + r21 * m.e12 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e10 + tz * m.e20;
	product.ty = tx * m.e01 + ty * m.e11 + tz * m.e21;
	product.tz = tx * m.e02 + ty * m.e12 + tz * m.e22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 Cartesian4x4::operator*(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_M3X3);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e10 + r02 * m.e20;
	product.r01 = r00 * m.e01 + r01 * m.e11 + r02 * m.e21;
	product.r02 = r00 * m.e02 + r01 * m.e12 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e10 + r12 * m.e20;
	product.r11 = r10 * m.e01 + r11 * m.e11 + r12 * m.e21;
	product.r12 = r10 * m.e02 + r11 * m.e12 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e10 + r22 * m.e20;
	product.r21 = r20 * m.e01 + r21 * m.e11 + r22 * m.e21;
	product.r22 = r20 * m.e02 + r21 * m.e12 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e10 + tz * m.e20;
	product.ty = tx * m.e01 + ty * m.e11 + tz * m.e21;
	product.tz = tx * m.e02 + ty * m.e12 + tz * m.e22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 &Cartesian4x4::operator*=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_M3X3);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e10 + r02 * m.e20;
	product.r01 = r00 * m.e01 + r01 * m.e11 + r02 * m.e21;
	product.r02 = r00 * m.e02 + r01 * m.e12 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e10 + r12 * m.e20;
	product.r11 = r10 * m.e01 + r11 * m.e11 + r12 * m.e21;
	product.r12 = r10 * m.e02 + r11 * m.e12 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e10 + r22 * m.e20;
	product.r21 = r20 * m.e01 + r21 * m.e11 + r22 * m.e21;
	product.r22 = r20 * m.e02 + r21 * m.e12 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e10 + tz * m.e20;
	product.ty = tx * m.e01 + ty * m.e11 + tz * m.e21;
	product.tz = tx * m.e02 + ty * m.e12 + tz * m.e22;
	product.w = 1.0f;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 Cartesian4x4::operator/(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_D3X3);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e01 + r02 * m.e02;
	product.r01 = r00 * m.e10 + r01 * m.e11 + r02 * m.e12;
	product.r02 = r00 * m.e20 + r01 * m.e21 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e01 + r12 * m.e02;
	product.r11 = r10 * m.e10 + r11 * m.e11 + r12 * m.e12;
	product.r12 = r10 * m.e20 + r11 * m.e21 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e01 + r22 * m.e02;
	product.r21 = r20 * m.e10 + r21 * m.e11 + r22 * m.e12;
	product.r22 = r20 * m.e20 + r21 * m.e21 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e01 + tz * m.e02;
	product.ty = tx * m.e10 + ty * m.e11 + tz * m.e12;
	product.tz = tx * m.e20 + ty * m.e21 + tz * m.e22;
	product.w = 1.0f;

	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 &Cartesian4x4::operator/=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_DIVIDE_EQUALS_D3X3);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = r00 * m.e00 + r01 * m.e01 + r02 * m.e02;
	product.r01 = r00 * m.e10 + r01 * m.e11 + r02 * m.e12;
	product.r02 = r00 * m.e20 + r01 * m.e21 + r02 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r10 * m.e00 + r11 * m.e01 + r12 * m.e02;
	product.r11 = r10 * m.e10 + r11 * m.e11 + r12 * m.e12;
	product.r12 = r10 * m.e20 + r11 * m.e21 + r12 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r20 * m.e00 + r21 * m.e01 + r22 * m.e02;
	product.r21 = r20 * m.e10 + r21 * m.e11 + r22 * m.e12;
	product.r22 = r20 * m.e20 + r21 * m.e21 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = tx * m.e00 + ty * m.e01 + tz * m.e02;
	product.ty = tx * m.e10 + ty * m.e11 + tz * m.e12;
	product.tz = tx * m.e20 + ty * m.e21 + tz * m.e22;
	product.w = 1.0f;

	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Cartesian4x4::InvertTimes(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT_TIMES_M4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = r00 * m.e00 + r10 * m.e10 + r20 * m.e20;
	product.e01 = r00 * m.e01 + r10 * m.e11 + r20 * m.e21;
	product.e02 = r00 * m.e02 + r10 * m.e12 + r20 * m.e22;
	product.e03 = r00 * m.e03 + r10 * m.e13 + r20 * m.e23;

	product.e10 = r01 * m.e00 + r11 * m.e10 + r21 * m.e20;
	product.e11 = r01 * m.e01 + r11 * m.e11 + r21 * m.e21;
	product.e12 = r01 * m.e02 + r11 * m.e12 + r21 * m.e22;
	product.e13 = r01 * m.e03 + r11 * m.e13 + r21 * m.e23;

	product.e20 = r02 * m.e00 + r12 * m.e10 + r22 * m.e20;
	product.e21 = r02 * m.e01 + r12 * m.e11 + r22 * m.e21;
	product.e22 = r02 * m.e02 + r12 * m.e12 + r22 * m.e22;
	product.e23 = r02 * m.e03 + r12 * m.e13 + r22 * m.e23;

	float txInv = tx * r00 + ty * r01 + tz * r02;
	float tyInv = tx * r10 + ty * r11 + tz * r12;
	float tzInv = tx * r20 + ty * r21 + tz * r22;
	product.e30 = m.e30 - txInv * m.e00 - tyInv * m.e10 - tzInv * m.e20;
	product.e31 = m.e31 - txInv * m.e01 - tyInv * m.e11 - tzInv * m.e21;
	product.e32 = m.e32 - txInv * m.e02 - tyInv * m.e12 - tzInv * m.e22;
	product.e33 = m.e33 - txInv * m.e03 - tyInv * m.e13 - tzInv * m.e23;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 Cartesian4x4::InvertTimes(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT_TIMES_G4X4);
#endif //MATH_STATS

	Graphics4x4 product;

	product.r00 = r00 * m.r00 + r10 * m.r10 + r20 * m.r20;
	product.r01 = r00 * m.r01 + r10 * m.r11 + r20 * m.r21;
	product.r02 = r00 * m.r02 + r10 * m.r12 + r20 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r01 * m.r00 + r11 * m.r10 + r21 * m.r20;
	product.r11 = r01 * m.r01 + r11 * m.r11 + r21 * m.r21;
	product.r12 = r01 * m.r02 + r11 * m.r12 + r21 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r02 * m.r00 + r12 * m.r10 + r22 * m.r20;
	product.r21 = r02 * m.r01 + r12 * m.r11 + r22 * m.r21;
	product.r22 = r02 * m.r02 + r12 * m.r12 + r22 * m.r22;
	product.u2 = 0.0f;

	float txInv = tx * r00 + ty * r01 + tz * r02;
	float tyInv = tx * r10 + ty * r11 + tz * r12;
	float tzInv = tx * r20 + ty * r21 + tz * r22;
	product.tx = m.tx - txInv * m.r00 - tyInv * m.r10 - tzInv * m.r20;
	product.ty = m.ty - txInv * m.r01 - tyInv * m.r11 - tzInv * m.r21;
	product.tz = m.tz - txInv * m.r02 - tyInv * m.r12 - tzInv * m.r22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 &Cartesian4x4::InvertTimesEquals(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT_TIMES_EQUALS_C4X4);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = r00 * m.r00 + r10 * m.r10 + r20 * m.r20;
	product.r01 = r00 * m.r01 + r10 * m.r11 + r20 * m.r21;
	product.r02 = r00 * m.r02 + r10 * m.r12 + r20 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r01 * m.r00 + r11 * m.r10 + r21 * m.r20;
	product.r11 = r01 * m.r01 + r11 * m.r11 + r21 * m.r21;
	product.r12 = r01 * m.r02 + r11 * m.r12 + r21 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r02 * m.r00 + r12 * m.r10 + r22 * m.r20;
	product.r21 = r02 * m.r01 + r12 * m.r11 + r22 * m.r21;
	product.r22 = r02 * m.r02 + r12 * m.r12 + r22 * m.r22;
	product.u2 = 0.0f;

	float txInv = tx * r00 + ty * r01 + tz * r02;
	float tyInv = tx * r10 + ty * r11 + tz * r12;
	float tzInv = tx * r20 + ty * r21 + tz * r22;
	product.tx = m.tx - txInv * m.r00 - tyInv * m.r10 - tzInv * m.r20;
	product.ty = m.ty - txInv * m.r01 - tyInv * m.r11 - tzInv * m.r21;
	product.tz = m.tz - txInv * m.r02 - tyInv * m.r12 - tzInv * m.r22;
	product.w = 1.0f;

	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 Cartesian4x4::InvertTimes(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT_TIMES_M3X3);
#endif //MATH_STATS

	Graphics4x4 product;

	product.r00 = r00 * m.e00 + r10 * m.e10 + r20 * m.e20;
	product.r01 = r00 * m.e01 + r10 * m.e11 + r20 * m.e21;
	product.r02 = r00 * m.e02 + r10 * m.e12 + r20 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r01 * m.e00 + r11 * m.e10 + r21 * m.e20;
	product.r11 = r01 * m.e01 + r11 * m.e11 + r21 * m.e21;
	product.r12 = r01 * m.e02 + r11 * m.e12 + r21 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r02 * m.e00 + r12 * m.e10 + r22 * m.e20;
	product.r21 = r02 * m.e01 + r12 * m.e11 + r22 * m.e21;
	product.r22 = r02 * m.e02 + r12 * m.e12 + r22 * m.e22;
	product.u2 = 0.0f;

	float txInv = tx * r00 + ty * r01 + tz * r02;
	float tyInv = tx * r10 + ty * r11 + tz * r12;
	float tzInv = tx * r20 + ty * r21 + tz * r22;
	product.tx = -txInv * m.e00 - tyInv * m.e10 - tzInv * m.e20;
	product.ty = -txInv * m.e01 - tyInv * m.e11 - tzInv * m.e21;
	product.tz = -txInv * m.e02 - tyInv * m.e12 - tzInv * m.e22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 &Cartesian4x4::InvertTimesEquals(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT_TIMES_EQUALS_D3X3);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = r00 * m.e00 + r10 * m.e10 + r20 * m.e20;
	product.r01 = r00 * m.e01 + r10 * m.e11 + r20 * m.e21;
	product.r02 = r00 * m.e02 + r10 * m.e12 + r20 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r01 * m.e00 + r11 * m.e10 + r21 * m.e20;
	product.r11 = r01 * m.e01 + r11 * m.e11 + r21 * m.e21;
	product.r12 = r01 * m.e02 + r11 * m.e12 + r21 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r02 * m.e00 + r12 * m.e10 + r22 * m.e20;
	product.r21 = r02 * m.e01 + r12 * m.e11 + r22 * m.e21;
	product.r22 = r02 * m.e02 + r12 * m.e12 + r22 * m.e22;
	product.u2 = 0.0f;

	float txInv = tx * r00 + ty * r01 + tz * r02;
	float tyInv = tx * r10 + ty * r11 + tz * r12;
	float tzInv = tx * r20 + ty * r21 + tz * r22;
	product.tx = -txInv * m.e00 - tyInv * m.e10 - tzInv * m.e20;
	product.ty = -txInv * m.e01 - tyInv * m.e11 - tzInv * m.e21;
	product.tz = -txInv * m.e02 - tyInv * m.e12 - tzInv * m.e22;
	product.w = 1.0f;

	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 Cartesian4x4::InvertDivide(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT_DIVIDE_C4X4);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = r00 * m.r00 + r10 * m.r01 + r20 * m.r02;
	product.r01 = r00 * m.r10 + r10 * m.r11 + r20 * m.r12;
	product.r02 = r00 * m.r20 + r10 * m.r21 + r20 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r01 * m.r00 + r11 * m.r01 + r21 * m.r02;
	product.r11 = r01 * m.r10 + r11 * m.r11 + r21 * m.r12;
	product.r12 = r01 * m.r20 + r11 * m.r21 + r21 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r02 * m.r00 + r12 * m.r01 + r22 * m.r02;
	product.r21 = r02 * m.r10 + r12 * m.r11 + r22 * m.r12;
	product.r22 = r02 * m.r20 + r12 * m.r21 + r22 * m.r22;
	product.u2 = 0.0f;

	float txInv = tx * r00 + ty * r01 + tz * r02 + m.tx;
	float tyInv = tx * r10 + ty * r11 + tz * r12 + m.ty;
	float tzInv = tx * r20 + ty * r21 + tz * r22 + m.tz;
	product.tx = -txInv * m.r00 - tyInv * m.r01 - tzInv * m.r02;
	product.ty = -txInv * m.r10 - tyInv * m.r11 - tzInv * m.r12;
	product.tz = -txInv * m.r20 - tyInv * m.r21 - tzInv * m.r22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 &Cartesian4x4::InvertDivideEquals(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT_DIVIDE_EQUALS_C4X4);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = r00 * m.r00 + r10 * m.r01 + r20 * m.r02;
	product.r01 = r00 * m.r10 + r10 * m.r11 + r20 * m.r12;
	product.r02 = r00 * m.r20 + r10 * m.r21 + r20 * m.r22;
	product.u0 = 0.0f;

	product.r10 = r01 * m.r00 + r11 * m.r01 + r21 * m.r02;
	product.r11 = r01 * m.r10 + r11 * m.r11 + r21 * m.r12;
	product.r12 = r01 * m.r20 + r11 * m.r21 + r21 * m.r22;
	product.u1 = 0.0f;

	product.r20 = r02 * m.r00 + r12 * m.r01 + r22 * m.r02;
	product.r21 = r02 * m.r10 + r12 * m.r11 + r22 * m.r12;
	product.r22 = r02 * m.r20 + r12 * m.r21 + r22 * m.r22;
	product.u2 = 0.0f;

	float txInv = tx * r00 + ty * r01 + tz * r02 + m.tx;
	float tyInv = tx * r10 + ty * r11 + tz * r12 + m.ty;
	float tzInv = tx * r20 + ty * r21 + tz * r22 + m.tz;
	product.tx = -txInv * m.r00 - tyInv * m.r01 - tzInv * m.r02;
	product.ty = -txInv * m.r10 - tyInv * m.r11 - tzInv * m.r12;
	product.tz = -txInv * m.r20 - tyInv * m.r21 - tzInv * m.r22;
	product.w = 1.0f;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 Cartesian4x4::InvertDivide(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT_DIVIDE_C4X4);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = r00 * m.e00 + r10 * m.e01 + r20 * m.e02;
	product.r01 = r00 * m.e10 + r10 * m.e11 + r20 * m.e12;
	product.r02 = r00 * m.e20 + r10 * m.e21 + r20 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r01 * m.e00 + r11 * m.e01 + r21 * m.e02;
	product.r11 = r01 * m.e10 + r11 * m.e11 + r21 * m.e12;
	product.r12 = r01 * m.e20 + r11 * m.e21 + r21 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r02 * m.e00 + r12 * m.e01 + r22 * m.e02;
	product.r21 = r02 * m.e10 + r12 * m.e11 + r22 * m.e12;
	product.r22 = r02 * m.e20 + r12 * m.e21 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = -tx * product.r00 - ty * product.r10 - tz * product.r20;
	product.ty = -tx * product.r01 - ty * product.r11 - tz * product.r21;
	product.tz = -tx * product.r02 - ty * product.r12 - tz * product.r22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 &Cartesian4x4::InvertDivideEquals(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(CARTESIAN4X4_INVERT_DIVIDE_EQUALS_C4X4);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = r00 * m.e00 + r10 * m.e01 + r20 * m.e02;
	product.r01 = r00 * m.e10 + r10 * m.e11 + r20 * m.e12;
	product.r02 = r00 * m.e20 + r10 * m.e21 + r20 * m.e22;
	product.u0 = 0.0f;

	product.r10 = r01 * m.e00 + r11 * m.e01 + r21 * m.e02;
	product.r11 = r01 * m.e10 + r11 * m.e11 + r21 * m.e12;
	product.r12 = r01 * m.e20 + r11 * m.e21 + r21 * m.e22;
	product.u1 = 0.0f;

	product.r20 = r02 * m.e00 + r12 * m.e01 + r22 * m.e02;
	product.r21 = r02 * m.e10 + r12 * m.e11 + r22 * m.e12;
	product.r22 = r02 * m.e20 + r12 * m.e21 + r22 * m.e22;
	product.u2 = 0.0f;

	product.tx = -tx * product.r00 - ty * product.r10 - tz * product.r20;
	product.ty = -tx * product.r01 - ty * product.r11 - tz * product.r21;
	product.tz = -tx * product.r02 - ty * product.r12 - tz * product.r22;
	product.w = 1.0f;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::SetScale(
float xScale,
float yScale,
float zScale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::SetXScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::SetYScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::SetZScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::CatScale(
float xScale,
float yScale,
float zScale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::CatXScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::CatYScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::CatZScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::ReverseCatScale(
float xScale,
float yScale,
float zScale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::ReverseCatXScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::ReverseCatYScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void Cartesian4x4::ReverseCatZScale(
float scale)
{
	ASSERT(false);
}

/************ for later ******************/

//inline Matrix4x4 &Matrix4x4::Product(Matrix4x4 &m1, Matrix4x4 &m2)
//{
//	e00 = m1.e00 * m2.e00 + m1.e01 * m2.e10 + m1.e02 * m2.e20 + m1.e03 * m2.e30;
//	e01 = m1.e00 * m2.e01 + m1.e01 * m2.e11 + m1.e02 * m2.e21 + m1.e03 * m2.e31;
//	e02 = m1.e00 * m2.e02 + m1.e01 * m2.e12 + m1.e02 * m2.e22 + m1.e03 * m2.e32;
//	e03 = m1.e00 * m2.e03 + m1.e01 * m2.e13 + m1.e02 * m2.e23 + m1.e03 * m2.e33;
//			  					    			  	      			  		   			  
//	e10 = m1.e10 * m2.e00 + m1.e11 * m2.e10 + m1.e12 * m2.e20 + m1.e13 * m2.e30;
//	e11 = m1.e10 * m2.e01 + m1.e11 * m2.e11 + m1.e12 * m2.e21 + m1.e13 * m2.e31;
//	e12 = m1.e10 * m2.e02 + m1.e11 * m2.e12 + m1.e12 * m2.e22 + m1.e13 * m2.e32;
//	e13 = m1.e10 * m2.e03 + m1.e11 * m2.e13 + m1.e12 * m2.e23 + m1.e13 * m2.e33;
//			  					    			  	      			  		   			  
//	e20 = m1.e20 * m2.e00 + m1.e21 * m2.e10 + m1.e22 * m2.e20 + m1.e23 * m2.e30;
//	e21 = m1.e20 * m2.e01 + m1.e21 * m2.e11 + m1.e22 * m2.e21 + m1.e23 * m2.e31;
//	e22 = m1.e20 * m2.e02 + m1.e21 * m2.e12 + m1.e22 * m2.e22 + m1.e23 * m2.e32;
//	e23 = m1.e20 * m2.e03 + m1.e21 * m2.e13 + m1.e22 * m2.e23 + m1.e23 * m2.e33;
//			  					    			  	      			  		   			  
//	e30 = m1.e30 * m2.e00 + m1.e31 * m2.e10 + m1.e32 * m2.e20 + m1.e33 * m2.e30;
//	e31 = m1.e30 * m2.e01 + m1.e31 * m2.e11 + m1.e32 * m2.e21 + m1.e33 * m2.e31;
//	e32 = m1.e30 * m2.e02 + m1.e31 * m2.e12 + m1.e32 * m2.e22 + m1.e33 * m2.e32;
//	e33 = m1.e30 * m2.e03 + m1.e31 * m2.e13 + m1.e32 * m2.e23 + m1.e33 * m2.e33;
//
//	return(*this);
//}

//inline Matrix4x4 &Matrix4x4::ProductVU(Matrix4x4 &m1, Matrix4x4 &m2)
//{
//#ifdef PS2
//	asm __volatile__("
//
//	// load matrix m1
//
//	lqc2				vf8,0x0(&m1)
//	lqc2				vf9,0x10(&m1)
//	lqc2				vf10,0x20(&m1)
//	lqc2				vf11,0x30(&m1)
//
//	// load matrix m2
//
//	lqc2				vf4,0x0(&m2)
//	lqc2				vf5,0x10(&m2)
//	lqc2				vf6,0x20(&m2)
//	lqc2				vf7,0x30(&m2)
//
//	// calculate resulting matrix
//
//	vmulax.xyzw		ACC,vf4,vf8
//	vmadday.xyzw	ACC,vf5,vf8
//	vmaddaz.xyzw	ACC,vf6,vf8
//	vmaddw.xyzw		vf12,vf7,vf8
//
//	vmulax.xyzw		ACC,vf4,vf9
//	vmadday.xyzw	ACC,vf5,vf9
//	vmaddaz.xyzw	ACC,vf6,vf9
//	vmaddw.xyzw		vf13,vf7,vf9
//
//	vmulax.xyzw		ACC,vf4,vf10
//	vmadday.xyzw	ACC,vf5,vf10
//	vmaddaz.xyzw	ACC,vf6,vf10
//	vmaddw.xyzw		vf14,vf7,vf10
//
//	vmulax.xyzw		ACC,vf4,vf11
//	vmadday.xyzw	ACC,vf5,vf11
//	vmaddaz.xyzw	ACC,vf6,vf11
//	vmaddw.xyzw		vf15,vf7,vf11
//
//	// save results
//
//	sqc2				vf12,0x0(this)
//	sqc2				vf13,0x10(this)
//	sqc2				vf14,0x20(this)
//	sqc2				vf15,0x30(this)
//
//	": : : );
//#endif //PS2
//
//	return(*this);
//}

