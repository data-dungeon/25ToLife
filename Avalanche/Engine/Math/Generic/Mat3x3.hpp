/***************************************************************************/
// Implementations for the matrix class
/***************************************************************************/
#include "Math/MathClass.h"
#include "Math/Det.h"
#include "Math/MathStats.h"

/***************************************************************************/
// Data for the BuildFromAxis calls
/***************************************************************************/
static struct
{
	int d_tertiaryRow;
	bool d_flipSecondaryAxis;
} l_buildFromAxisInfo[3][3] =
{
	{
		{-1, false},
		{ 2, true},
		{ 1, false}
	},
	{
		{ 2, true},
		{-1, false},
		{ 0, false}
	},
	{
		{ 1, true},
		{ 0, false},
		{-1, false}
	}
};

/***************************************************************************/
// Set the matrix
/***************************************************************************/
inline Matrix3x3::Matrix3x3(
float e00,
float e01,
float e02,
float e10,
float e11,
float e12,
float e20,
float e21,
float e22)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CONSTRUCT_F);
#endif //MATH_STATS

	this->e00 = e00;
	this->e01 = e01;
	this->e02 = e02;

	this->e10 = e10;
	this->e11 = e11;
	this->e12 = e12;

	this->e20 = e20;
	this->e21 = e21;
	this->e22 = e22;
}

/***************************************************************************/
// create the diagonalized matrix
/***************************************************************************/
inline Matrix3x3::Matrix3x3(
Vector3CRef diagonalize)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CONSTRUCT_V3);
#endif //MATH_STATS

	e00 = diagonalize.x();
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = diagonalize.y();
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = diagonalize.z();
}

/***************************************************************************/
// copy the upper left 3 x 3 from a Graphics4x4
/***************************************************************************/
inline Matrix3x3::Matrix3x3(Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CONSTRUCT_G4X4);
#endif //MATH_STATS

	e00 = m.r00;
	e01 = m.r01;
	e02 = m.r02;

	e10 = m.r10;
	e11 = m.r11;
	e12 = m.r12;
				 
	e20 = m.r20;
	e21 = m.r21;
	e22 = m.r22;
}

inline Matrix3x3 &Matrix3x3::operator=(
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_ASSIGN_M3X3);
#endif //MATH_STATS

	e00 = m.e00;
	e01 = m.e01;
	e02 = m.e02;

	e10 = m.e10;
	e11 = m.e11;
	e12 = m.e12;

	e20 = m.e20;
	e21 = m.e21;
	e22 = m.e22;

	return *this;
}

inline Matrix3x3 &Matrix3x3::operator=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_ASSIGN_M3X3);
#endif //MATH_STATS

	e00 = m.e00;
	e01 = m.e01;
	e02 = m.e02;

	e10 = m.e10;
	e11 = m.e11;
	e12 = m.e12;

	e20 = m.e20;
	e21 = m.e21;
	e22 = m.e22;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_ASSIGN_G4X4);
#endif //MATH_STATS

	e00 = m.r00;
	e01 = m.r01;
	e02 = m.r02;

	e10 = m.r10;
	e11 = m.r11;
	e12 = m.r12;

	e20 = m.r20;
	e21 = m.r21;
	e22 = m.r22;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_ASSIGN_G4X4);
#endif //MATH_STATS

	e00 = m.r00;
	e01 = m.r01;
	e02 = m.r02;

	e10 = m.r10;
	e11 = m.r11;
	e12 = m.r12;

	e20 = m.r20;
	e21 = m.r21;
	e22 = m.r22;

	return *this;
}

/***************************************************************************/
// Set the matrix
/***************************************************************************/
inline void Matrix3x3::Set(
float e00,
float e01,
float e02,
float e10,
float e11,
float e12,
float e20,
float e21,
float e22)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_F);
#endif //MATH_STATS

	this->e00 = e00;
	this->e01 = e01;
	this->e02 = e02;

	this->e10 = e10;
	this->e11 = e11;
	this->e12 = e12;

	this->e20 = e20;
	this->e21 = e21;
	this->e22 = e22;
}

/***************************************************************************/
// Diagonalize the vector across the matrix
/***************************************************************************/
inline void Matrix3x3::Diagonalize(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DIAGONALIZE_V3);
#endif //MATH_STATS

	e00 = v.x();
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = v.y();
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = v.z();
}

/***************************************************************************/
// Skew symmetric
/***************************************************************************/
inline void Matrix3x3::SkewSymmetric(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SKEW_SYMMETRIC_V3);
#endif //MATH_STATS

	e00 = 0;
	e01 = v.z();
	e02 = -v.y();

	e10 = -v.z();
	e11 = 0;
	e12 = v.x();

	e20 = v.y();
	e21 = -v.x();
	e22 = 0;
}

/***************************************************************************/
// Set to identity
/***************************************************************************/
inline void Matrix3x3::Identity(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_IDENTITY);
#endif //MATH_STATS

	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = 1.0f;
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
}

inline bool Matrix3x3::IsIdentity(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_IS_IDENTITY);
#endif //MATH_STATS

	return((e00 == 1.0f) &&
			 (e01 == 0.0f) &&
			 (e02 == 0.0f) &&

			 (e10 == 0.0f) &&
			 (e11 == 1.0f) &&
			 (e12 == 0.0f) &&

			 (e20 == 0.0f) &&
			 (e21 == 0.0f) &&
			 (e22 == 1.0f));
}

/***************************************************************************/
// Transpose the matrix
/***************************************************************************/
inline void Matrix3x3::Transpose(
Matrix3x3 &transpose) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TRANSPOSE_M3X3);
#endif //MATH_STATS

	transpose.e00 = e00;
	transpose.e01 = e10;
	transpose.e02 = e20;

	transpose.e10 = e01;
	transpose.e11 = e11;
	transpose.e12 = e21;

	transpose.e20 = e02;
	transpose.e21 = e12;
	transpose.e22 = e22;
}

/***************************************************************************/
// Transpose the matrix
/***************************************************************************/
inline void Matrix3x3::Transpose(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TRANSPOSE);
#endif //MATH_STATS

	Matrix3x3 t;

	Transpose(t);
	*this = t;
}

/***************************************************************************/
// Return the transposed matrix
/***************************************************************************/
inline Matrix3x3 Matrix3x3::Transposed(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TRANSPOSED);
#endif //MATH_STATS

	Matrix3x3 transposed;

	transposed.e00 = e00;
	transposed.e01 = e10;
	transposed.e02 = e20;

	transposed.e10 = e01;
	transposed.e11 = e11;
	transposed.e12 = e21;

	transposed.e20 = e02;
	transposed.e21 = e12;
	transposed.e22 = e22;

	return transposed;
}

/***************************************************************************/
// Invert (returns false if not invertable)
/***************************************************************************/
inline bool Matrix3x3::Invert(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_INVERT);
#endif //MATH_STATS

	// invert it
	Matrix3x3 inverse;
	if (!Invert(inverse))
		return(false);

	// save it
	*this = inverse;
	return true;
}

/***************************************************************************/
// Static for the determinant
/***************************************************************************/
inline float Matrix3x3::Det(
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DET_M3X3);
#endif //MATH_STATS

	return Determinant::Calc(m.e00, m.e01, m.e02,
										m.e10, m.e11, m.e12,
										m.e20, m.e21, m.e22);
}

/***************************************************************************/
// Return the determinant
/***************************************************************************/
inline float Matrix3x3::Det(void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DET);
#endif //MATH_STATS

	return Det(*this);
}

/***************************************************************************/
// Make the cofactor matrix
/***************************************************************************/
inline void Matrix3x3::Cof(
Matrix3x3 &cofactor) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_COF_M3X3);
#endif //MATH_STATS

	cofactor.e00 = Determinant::Calc(e11, e12, e21, e22);
	cofactor.e01 = -Determinant::Calc(e10, e12, e20, e22);
	cofactor.e02 = Determinant::Calc(e10, e11, e20, e21);

	cofactor.e10 = -Determinant::Calc(e01, e02, e21, e22);
	cofactor.e11 = Determinant::Calc(e00, e02, e20, e22);
	cofactor.e12 = -Determinant::Calc(e00, e01, e20, e21);

	cofactor.e20 = Determinant::Calc(e01, e02, e11, e12);
	cofactor.e21 = -Determinant::Calc(e00, e02, e10, e12);
	cofactor.e22 = Determinant::Calc(e00, e01, e10, e11);
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::Cof(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_COF);
#endif //MATH_STATS

	Matrix3x3 cofactor;
	Cof(cofactor);
	*this = cofactor;
}

/***************************************************************************/
// Make the adjoint matrix
/***************************************************************************/
inline void Matrix3x3::Adj(
Matrix3x3 &adjoint) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_ADJ_M3X3);
#endif //MATH_STATS

	adjoint.e00 = Determinant::Calc(e11, e12, e21, e22);
	adjoint.e10 = -Determinant::Calc(e10, e12, e20, e22);
	adjoint.e20 = Determinant::Calc(e10, e11, e20, e21);

	adjoint.e01 = -Determinant::Calc(e01, e02, e21, e22);
	adjoint.e11 = Determinant::Calc(e00, e02, e20, e22);
	adjoint.e21 = -Determinant::Calc(e00, e01, e20, e21);

	adjoint.e02 = Determinant::Calc(e01, e02, e11, e12);
	adjoint.e12 = -Determinant::Calc(e00, e02, e10, e12);
	adjoint.e22 = Determinant::Calc(e00, e01, e10, e11);
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::Adj(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_ADJ);
#endif //MATH_STATS

	Matrix3x3 adjoint;
	Adj(adjoint);
	*this = adjoint;
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Matrix3x3::operator==(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_EQUALS_M3X3);
#endif //MATH_STATS

	return(e00 == m.e00 && e01 == m.e01 && e02 == m.e02 &&
				e10 == m.e10 && e11 == m.e11 && e12 == m.e12 &&
				e20 == m.e20 && e21 == m.e21 && e22 == m.e22);
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int Matrix3x3::operator==(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_EQUALS_M3X3);
#endif //MATH_STATS

	return(e00 == m.e00 && e01 == m.e01 && e02 == m.e02 &&
				e10 == m.e10 && e11 == m.e11 && e12 == m.e12 &&
				e20 == m.e20 && e21 == m.e21 && e22 == m.e22);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Matrix3x3::operator!=(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_NOT_EQUAL_M3X3);
#endif //MATH_STATS

	return(e00 != m.e00 || e01 != m.e01 || e02 != m.e02 ||
				e10 != m.e10 || e11 != m.e11 || e12 != m.e12 ||
				e20 != m.e20 || e21 != m.e21 || e22 != m.e22);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int Matrix3x3::operator!=(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_NOT_EQUAL_M3X3);
#endif //MATH_STATS

	return(e00 != m.e00 || e01 != m.e01 || e02 != m.e02 ||
				e10 != m.e10 || e11 != m.e11 || e12 != m.e12 ||
				e20 != m.e20 || e21 != m.e21 || e22 != m.e22);
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 Matrix3x3::operator*(
float s) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_F);
#endif //MATH_STATS

	Matrix3x3 m;

	m.e00 = e00 * s;
	m.e01 = e01 * s;
	m.e02 = e02 * s;

	m.e10 = e10 * s;
	m.e11 = e11 * s;
	m.e12 = e12 * s;

	m.e20 = e20 * s;
	m.e21 = e21 * s;
	m.e22 = e22 * s;

	return m;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator*=(
float s)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_EQUALS_F);
#endif //MATH_STATS

	e00 *= s;
	e01 *= s;
	e02 *= s;

	e10 *= s;
	e11 *= s;
	e12 *= s;

	e20 *= s;
	e21 *= s;
	e22 *= s;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 Matrix3x3::operator/(
float s) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DIVIDE_F);
#endif //MATH_STATS

	Matrix3x3 m;
	float inv = 1.0f / s;

	m.e00 = e00 * inv;
	m.e01 = e01 * inv;
	m.e02 = e02 * inv;

	m.e10 = e10 * inv;
	m.e11 = e11 * inv;
	m.e12 = e12 * inv;

	m.e20 = e20 * inv;
	m.e21 = e21 * inv;
	m.e22 = e22 * inv;

	return m;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator/=(
float s)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DIVIDE_EQUALS_F);
#endif //MATH_STATS

	float inv = 1.0f / s;

	e00 *= inv;
	e01 *= inv;
	e02 *= inv;

	e10 *= inv;
	e11 *= inv;
	e12 *= inv;

	e20 *= inv;
	e21 *= inv;
	e22 *= inv;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 Matrix3x3::operator+(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_PLUS_M3X3);
#endif //MATH_STATS

	Matrix3x3 sum;

	sum.e00 = e00 + m.e00;
	sum.e01 = e01 + m.e01;
	sum.e02 = e02 + m.e02;

	sum.e10 = e10 + m.e10;
	sum.e11 = e11 + m.e11;
	sum.e12 = e12 + m.e12;

	sum.e20 = e20 + m.e20;
	sum.e21 = e21 + m.e21;
	sum.e22 = e22 + m.e22;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator+=(
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_PLUS_EQUALS_M3X3);
#endif //MATH_STATS

	e00 += m.e00;
	e01 += m.e01;
	e02 += m.e02;

	e10 += m.e10;
	e11 += m.e11;
	e12 += m.e12;

	e20 += m.e20;
	e21 += m.e21;
	e22 += m.e22;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 Matrix3x3::operator+(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_PLUS_M3X3);
#endif //MATH_STATS

	Matrix3x3 sum;

	sum.e00 = e00 + m.e00;
	sum.e01 = e01 + m.e01;
	sum.e02 = e02 + m.e02;

	sum.e10 = e10 + m.e10;
	sum.e11 = e11 + m.e11;
	sum.e12 = e12 + m.e12;

	sum.e20 = e20 + m.e20;
	sum.e21 = e21 + m.e21;
	sum.e22 = e22 + m.e22;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator+=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_PLUS_EQUALS_M3X3);
#endif //MATH_STATS

	e00 += m.e00;
	e01 += m.e01;
	e02 += m.e02;

	e10 += m.e10;
	e11 += m.e11;
	e12 += m.e12;

	e20 += m.e20;
	e21 += m.e21;
	e22 += m.e22;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 Matrix3x3::operator-(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_MINUS_M3X3);
#endif //MATH_STATS

	Matrix3x3 sum;

	sum.e00 = e00 - m.e00;
	sum.e01 = e01 - m.e01;
	sum.e02 = e02 - m.e02;

	sum.e10 = e10 - m.e10;
	sum.e11 = e11 - m.e11;
	sum.e12 = e12 - m.e12;

	sum.e20 = e20 - m.e20;
	sum.e21 = e21 - m.e21;
	sum.e22 = e22 - m.e22;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator-=(
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_MINUS_EQUALS_M3X3);
#endif //MATH_STATS

	e00 -= m.e00;
	e01 -= m.e01;
	e02 -= m.e02;

	e10 -= m.e10;
	e11 -= m.e11;
	e12 -= m.e12;

	e20 -= m.e20;
	e21 -= m.e21;
	e22 -= m.e22;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 Matrix3x3::operator-(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_MINUS_M3X3);
#endif //MATH_STATS

	Matrix3x3 sum;

	sum.e00 = e00 - m.e00;
	sum.e01 = e01 - m.e01;
	sum.e02 = e02 - m.e02;

	sum.e10 = e10 - m.e10;
	sum.e11 = e11 - m.e11;
	sum.e12 = e12 - m.e12;

	sum.e20 = e20 - m.e20;
	sum.e21 = e21 - m.e21;
	sum.e22 = e22 - m.e22;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator-=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_MINUS_EQUALS_M3X3);
#endif //MATH_STATS

	e00 -= m.e00;
	e01 -= m.e01;
	e02 -= m.e02;

	e10 -= m.e10;
	e11 -= m.e11;
	e12 -= m.e12;

	e20 -= m.e20;
	e21 -= m.e21;
	e22 -= m.e22;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 Matrix3x3::operator*(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_M3X3);
#endif //MATH_STATS

	Matrix3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;

	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator*=(
const Matrix3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_EQUALS_M3X3);
#endif //MATH_STATS

	Matrix3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;

	// copy it in
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 Matrix3x3::operator*(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_M3X3);
#endif //MATH_STATS

	Matrix3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;

	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator*=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_EQUALS_M3X3);
#endif //MATH_STATS

	Matrix3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;

	// copy it in
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 Matrix3x3::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_M4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;
	product.e03 = e00 * m.e03 + e01 * m.e13 + e02 * m.e23;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;
	product.e13 = e10 * m.e03 + e11 * m.e13 + e12 * m.e23;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;
	product.e23 = e20 * m.e03 + e21 * m.e13 + e22 * m.e23;

	product.e30 = m.e30;
	product.e31 = m.e31;
	product.e32 = m.e32;
	product.e33 = m.e33;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 Matrix3x3::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_G4X4);
#endif //MATH_STATS

	Graphics4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;
	product.e03 = 0.0f;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;
	product.e13 = 0.0f;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;
	product.e23 = 0.0f;

	product.e30 = m.e30;
	product.e31 = m.e31;
	product.e32 = m.e32;
	product.e33 = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 Matrix3x3::operator*(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_G4X4);
#endif //MATH_STATS

	Graphics4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;
	product.e03 = 0.0f;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;
	product.e13 = 0.0f;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;
	product.e23 = 0.0f;

	product.e30 = m.e30;
	product.e31 = m.e31;
	product.e32 = m.e32;
	product.e33 = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 Matrix3x3::operator/(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DIVIDE_C4X4);
#endif //MATH_STATS

	Graphics4x4 product;

	product.e00 = e00 * m.r00 + e01 * m.r01 + e02 * m.r02;
	product.e01 = e00 * m.r10 + e01 * m.r11 + e02 * m.r12;
	product.e02 = e00 * m.r20 + e01 * m.r21 + e02 * m.r22;
	product.e03 = 0.0f;

	product.e10 = e10 * m.r00 + e11 * m.r01 + e12 * m.r02;
	product.e11 = e10 * m.r10 + e11 * m.r11 + e12 * m.r12;
	product.e12 = e10 * m.r20 + e11 * m.r21 + e12 * m.r22;
	product.e13 = 0.0f;

	product.e20 = e20 * m.r00 + e21 * m.r01 + e22 * m.r02;
	product.e21 = e20 * m.r10 + e21 * m.r11 + e22 * m.r12;
	product.e22 = e20 * m.r20 + e21 * m.r21 + e22 * m.r22;
	product.e23 = 0.0f;

	product.e30 = -m.tx * m.r00 - m.ty * m.r01 - m.tz * m.r02;
	product.e31 = -m.tx * m.r10 - m.ty * m.r11 - m.tz * m.r12;
	product.e32 = -m.tx * m.r20 - m.ty * m.r21 - m.tz * m.r12;
	product.e33 = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 Matrix3x3::operator/(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DIVIDE_D3X3);
#endif //MATH_STATS

	Matrix3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e01 + e02 * m.e02;
	product.e01 = e00 * m.e10 + e01 * m.e11 + e02 * m.e12;
	product.e02 = e00 * m.e20 + e01 * m.e21 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e01 + e12 * m.e02;
	product.e11 = e10 * m.e10 + e11 * m.e11 + e12 * m.e12;
	product.e12 = e10 * m.e20 + e11 * m.e21 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e01 + e22 * m.e02;
	product.e21 = e20 * m.e10 + e21 * m.e11 + e22 * m.e12;
	product.e22 = e20 * m.e20 + e21 * m.e21 + e22 * m.e22;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::operator/=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DIVIDE_EQUALS_D3X3);
#endif //MATH_STATS

	Matrix3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e01 + e02 * m.e02;
	product.e01 = e00 * m.e10 + e01 * m.e11 + e02 * m.e12;
	product.e02 = e00 * m.e20 + e01 * m.e21 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e01 + e12 * m.e02;
	product.e11 = e10 * m.e10 + e11 * m.e11 + e12 * m.e12;
	product.e12 = e10 * m.e20 + e11 * m.e21 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e01 + e22 * m.e02;
	product.e21 = e20 * m.e10 + e21 * m.e11 + e22 * m.e12;
	product.e22 = e20 * m.e20 + e21 * m.e21 + e22 * m.e22;
 
	*this = product;
	return *this;
}

/***************************************************************************/
// just work on the upper 3x3 of a Graphics4x4
/***************************************************************************/
inline Matrix3x3 Matrix3x3::TimesOrientation(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_UL_G4X4);
#endif //MATH_STATS

	Matrix3x3 product;

	product.e00 = e00 * m.r00 + e01 * m.r10 + e02 * m.r20;
	product.e01 = e00 * m.r01 + e01 * m.r11 + e02 * m.r21;
	product.e02 = e00 * m.r02 + e01 * m.r12 + e02 * m.r22;

	product.e10 = e10 * m.r00 + e11 * m.r10 + e12 * m.r20;
	product.e11 = e10 * m.r01 + e11 * m.r11 + e12 * m.r21;
	product.e12 = e10 * m.r02 + e11 * m.r12 + e12 * m.r22;

	product.e20 = e20 * m.r00 + e21 * m.r10 + e22 * m.r20;
	product.e21 = e20 * m.r01 + e21 * m.r11 + e22 * m.r21;
	product.e22 = e20 * m.r02 + e21 * m.r12 + e22 * m.r22;

	return product;
}

/***************************************************************************/
// just work on the upper 3x3 of a Graphics4x4
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::TimesEqualsOrientation(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_EQUALS_UL_G4X4);
#endif //MATH_STATS

	Matrix3x3 product;

	product.e00 = e00 * m.r00 + e01 * m.r10 + e02 * m.r20;
	product.e01 = e00 * m.r01 + e01 * m.r11 + e02 * m.r21;
	product.e02 = e00 * m.r02 + e01 * m.r12 + e02 * m.r22;

	product.e10 = e10 * m.r00 + e11 * m.r10 + e12 * m.r20;
	product.e11 = e10 * m.r01 + e11 * m.r11 + e12 * m.r21;
	product.e12 = e10 * m.r02 + e11 * m.r12 + e12 * m.r22;

	product.e20 = e20 * m.r00 + e21 * m.r10 + e22 * m.r20;
	product.e21 = e20 * m.r01 + e21 * m.r11 + e22 * m.r21;
	product.e22 = e20 * m.r02 + e21 * m.r12 + e22 * m.r22;

	// copy it in
	*this = product;
	return *this;
}

/***************************************************************************/
// Get a row (copies)
/***************************************************************************/
inline Vector3 Matrix3x3::Row(
int row) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_ROW_I);
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
	};
}

/***************************************************************************/
// Get a column (copies)
/***************************************************************************/
inline Vector3 Matrix3x3::Column(
int column) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_COLUMN_I);
#endif //MATH_STATS

	switch (column)
	{
		case 0:
		default:
			return Vector3(e00, e10, e20);
			break;

		case 1:
			return Vector3(e01, e11, e21);
			break;

		case 2:
			return Vector3(e02, e12, e22);
			break;
	};
}

/***************************************************************************/
// Set a row
/***************************************************************************/
inline void Matrix3x3::SetRow(
int row,
Vector3CRef data)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_ROW_I_V3);
#endif //MATH_STATS

	switch (row)
	{
		case 0:
		default:
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
	};
}

/***************************************************************************/
// Set a column
/***************************************************************************/
inline void Matrix3x3::SetColumn(
int column,
Vector3CRef data)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_COLUMN_I_V3);
#endif //MATH_STATS

	switch (column)
	{
		case 0:
		default:
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
	};
}

/***************************************************************************/
// Get a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix3x3::GetRow0(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_GET_ROW0_V3);
#endif //MATH_STATS

	return Vector3(e00, e01, e02);
}

/***************************************************************************/
	// Get a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix3x3::GetRow1(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_GET_ROW1_V3);
#endif //MATH_STATS

	return Vector3(e10, e11, e12);
}

/***************************************************************************/
// Get a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix3x3::GetRow2(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_GET_ROW2_V3);
#endif //MATH_STATS

	return Vector3(e20, e21, e22);
}

/***************************************************************************/
// Get a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix3x3::GetColumn0(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_GET_COLUMN0_V3);
#endif //MATH_STATS

	return Vector3(e00, e10, e20);
}

/***************************************************************************/
// Get a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix3x3::GetColumn1(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_GET_COLUMN1_V3);
#endif //MATH_STATS

	return Vector3(e01, e11, e21);
}

/***************************************************************************/
// Get a row or column into a vector
/***************************************************************************/
inline Vector3 Matrix3x3::GetColumn2(
void) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_GET_COLUMN2_V3);
#endif //MATH_STATS

	return Vector3(e02, e12, e22);
}

/***************************************************************************/
// Set a row or column from a vector
/***************************************************************************/
inline void Matrix3x3::SetRow0(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_ROW0_V3);
#endif //MATH_STATS

	e00 = v.x();
	e01 = v.y();
	e02 = v.z();
}

/***************************************************************************/
// Set a row or column from a vector
/***************************************************************************/
inline void Matrix3x3::SetRow1(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_ROW1_V3);
#endif //MATH_STATS

	e10 = v.x();
	e11 = v.y();
	e12 = v.z();
}

/***************************************************************************/
// Set a row or column from a vector
/***************************************************************************/
inline void Matrix3x3::SetRow2(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_ROW2_V3);
#endif //MATH_STATS

	e20 = v.x();
	e21 = v.y();
	e22 = v.z();
}

/***************************************************************************/
// Set a row or column from a vector
/***************************************************************************/
inline void Matrix3x3::SetColumn0(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_COLUMN0_V3);
#endif //MATH_STATS

	e00 = v.x();
	e10 = v.y();
	e20 = v.z();
}

/***************************************************************************/
// Set a row or column from a vector
/***************************************************************************/
inline void Matrix3x3::SetColumn1(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_COLUMN1_V3);
#endif //MATH_STATS

	e01 = v.x();
	e11 = v.y();
	e21 = v.z();
}

/***************************************************************************/
// Set a row or column from a vector
/***************************************************************************/
inline void Matrix3x3::SetColumn2(
Vector3CRef v)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_COLUMN2_V3);
#endif //MATH_STATS

	e02 = v.x();
	e12 = v.y();
	e22 = v.z();
}

/***************************************************************************/
// Given a vector, create a matrix that is an orthogonal transformation
// matrix with uniform scale. Three orthogonal axes will be created. The
// first, or primary axis, will be the provided vector and will form the row
// specified in i_primaryRow. The second, or secondary axis, will lie on a
// line that is perpendicular to the primary axis and and that lies in the
// plane formed by the primary axis and the principle axis that is the
// closest to being perpendicular to the primary axis (that is, the secondary
// axis will lie on the line that is perpendicular to the primary axis and is
// as close to one of the principle axes as possible). Call the identified
// principle axis the up axis. The direction of the secondary axis will
// selected such that the angle between the the secondary axis and the up
// axis will be the smaller of the two possibilities. The secondary axis will
// form the row specified in i_secondaryRow. The remaining or tertiary axis
// will be perpendicular to the primary and secondary axes and oriented so
// that a right-handed coordinate system is formed. The tertiary axis will
// form the remaining row. The lengths of the three axes will be equal length
// to the length of the provided, primary axis.
/***************************************************************************/
inline Matrix3x3 &Matrix3x3::BuildFromAxis(Vector3CRef i_primaryAxis,
		int i_primaryRow, int i_secondaryRow)
{
	// Make sure input is valid.
	ASSERT((i_primaryRow >= 0) && (i_primaryRow < 3) && (i_secondaryRow >= 0) &&
			(i_secondaryRow < 3) && (i_primaryRow != i_secondaryRow));

	// Find the tertiary axis.
	Vector3 primaryAbs = i_primaryAxis.Abs();
	Vector3 tertiaryAxis;
	if (primaryAbs.X() < primaryAbs.Y())
	{
		if (primaryAbs.X() < primaryAbs.Z())
			tertiaryAxis.Set(0.0f, -i_primaryAxis.Z(), i_primaryAxis.Y());
		else
			tertiaryAxis.Set(-i_primaryAxis.Y(), i_primaryAxis.X(), 0.0f);
	}
	else if (primaryAbs.Y() < primaryAbs.Z())
		tertiaryAxis.Set(i_primaryAxis.Z(), 0.0f, -i_primaryAxis.X());
	else
		tertiaryAxis.Set(-i_primaryAxis.Y(), i_primaryAxis.X(), 0.0f);

	if (!tertiaryAxis.SafeNormalize())
		Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	else
	{
		// Set the rows.
		SetRow(i_primaryRow, i_primaryAxis);
		SetRow(i_secondaryRow, i_primaryAxis.Cross(tertiaryAxis));
		if (l_buildFromAxisInfo[i_primaryRow][i_secondaryRow].d_flipSecondaryAxis)
			SetRow(l_buildFromAxisInfo[i_primaryRow][i_secondaryRow].d_tertiaryRow,
					-i_primaryAxis.Length() * tertiaryAxis);
		else
			SetRow(l_buildFromAxisInfo[i_primaryRow][i_secondaryRow].d_tertiaryRow,
					i_primaryAxis.Length() * tertiaryAxis);
	}
	return(*this);
}

/***************************************************************************/
// Create a scale matrix (overwrites existing matrix, not cat'ed)
/***************************************************************************/
inline void Matrix3x3::SetScale(
float xScale,
float yScale,
float zScale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_SCALE_F);
#endif //MATH_STATS

	e00 = xScale;
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = yScale;
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = zScale;
}

/***************************************************************************/
// Simple (sets other scale values to 1)
/***************************************************************************/
inline void Matrix3x3::SetXScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_XSCALE_F);
#endif //MATH_STATS

	SetScale(scale, 1.0f, 1.0f);
}

/***************************************************************************/
// Simple (sets other scale values to 1)
/***************************************************************************/
inline void Matrix3x3::SetYScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_YSCALE_F);
#endif //MATH_STATS

	SetScale(1.0f, scale, 1.0f);
}

/***************************************************************************/
// Simple (sets other scale values to 1)
/***************************************************************************/
inline void Matrix3x3::SetZScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_ZSCALE_F);
#endif //MATH_STATS

	SetScale(1.0f, 1.0f, scale);
}

/***************************************************************************/
// Do rotations around x,y,z axes.  Does not cat, sets
/***************************************************************************/
inline void Matrix3x3::SetXRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_XROTATION_F);
#endif //MATH_STATS

	float s, c;
	Math::SinCos(radians, s, c);

	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = c;
	e12 = s;

	e20 = 0.0f;
	e21 = -s;
	e22 = c;
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::SetYRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_YROTATION_F);
#endif //MATH_STATS

	float s, c;
	Math::SinCos(radians, s, c);

	e00 = c;
	e01 = 0.0f;
	e02 = -s;

	e10 = 0;
	e11 = 1.0f;
	e12 = 0.0f;

	e20 = s;
	e21 = 0.0f;
	e22 = c;
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::SetZRotation(
const float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_ZROTATION_F);
#endif //MATH_STATS

	float s, c;
	Math::SinCos(radians, s, c);

	e00 = c;
	e01 = s;
	e02 = 0.0f;

	e10 = -s;
	e11 = c;
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::SetXZYRotation(
Vector3CRef v3Radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_SET_XYZROTATION_V3);
#endif //MATH_STATS

	float sx, cx;
	Math::SinCos(v3Radians.x(), sx, cx);
	float sy, cy;
	Math::SinCos(v3Radians.y(), sy, cy);
	float sz, cz;
	Math::SinCos(v3Radians.z(), sz, cz);

	e00 = cy * cz;
	e01 = sz;
	e02 = -sy * cz;

	e10 = -cx * cy * sz + sx * sy;
	e11 = cx * cz;
	e12 = cx * sy * sz + sx * cy;

	e20 = sx * cy * sz + cx * sy;
	e21 = -sx * cz;
	e22 = -sx * sy * sz + cx * cy;
}


/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::CatScale(
float xScale,
float yScale,
float zScale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CAT_SCALE_F);
#endif //MATH_STATS

	Matrix3x3 scale;
	scale.SetScale(xScale, yScale, zScale);
	*this *= scale;
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::CatScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CAT_SCALE_F);
#endif //MATH_STATS

	CatScale( scale, scale, scale);
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::CatXScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CAT_XSCALE_F);
#endif //MATH_STATS

	Matrix3x3 scaleM;
	scaleM.SetXScale(scale);
	*this *= scaleM;
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::CatYScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CAT_YSCALE_F);
#endif //MATH_STATS

	Matrix3x3 scaleM;
	scaleM.SetYScale(scale);
	*this *= scaleM;
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::CatZScale(
float scale)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CAT_ZSCALE_F);
#endif //MATH_STATS

	Matrix3x3 scaleM;
	scaleM.SetZScale(scale);
	*this *= scaleM;
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::CatXRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CAT_XROTATION_F);
#endif //MATH_STATS

	Matrix3x3 rot;
	rot.SetXRotation(radians);
	*this *= rot;
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::CatYRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CAT_YROTATION_F);
#endif //MATH_STATS

	Matrix3x3 rot;
	rot.SetYRotation(radians);
	*this *= rot;
}

/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::CatZRotation(
float radians)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_CAT_ZROTATION_F);
#endif //MATH_STATS

	Matrix3x3 rot;
	rot.SetZRotation(radians);
	*this *= rot;
}

#ifdef GCN
/***************************************************************************/
/***************************************************************************/
inline void Matrix3x3::EndianSwap( void)
{
	ByteSwap(e00);
	ByteSwap(e01);
	ByteSwap(e02);
	ByteSwap(e10);
	ByteSwap(e11);
	ByteSwap(e12);
	ByteSwap(e20);
	ByteSwap(e21);
	ByteSwap(e22);

}
#endif

/***************************************************************************/
/***************************************************************************/
inline bool Matrix3x3::IsNaN(void) const
{
	return (Math::IsNaN(e00) || Math::IsNaN(e01) || Math::IsNaN(e02) ||
			Math::IsNaN(e10) || Math::IsNaN(e11) || Math::IsNaN(e12) ||
			Math::IsNaN(e20) || Math::IsNaN(e21) || Math::IsNaN(e22));
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3::DirCos3x3(
float e00,
float e01,
float e02,
float e10,
float e11,
float e12,
float e20,
float e21,
float e22) : Matrix3x3(e00, e01, e02, e10, e11, e12, e20, e21,e22)
{
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3 &DirCos3x3::operator=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_ASSIGN_M3X3);
#endif //MATH_STATS

	e00 = m.e00;
	e01 = m.e01;
	e02 = m.e02;

	e10 = m.e10;
	e11 = m.e11;
	e12 = m.e12;

	e20 = m.e20;
	e21 = m.e21;
	e22 = m.e22;

	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3 &DirCos3x3::operator=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_ASSIGN_G4X4);
#endif //MATH_STATS

	e00 = m.r00;
	e01 = m.r01;
	e02 = m.r02;

	e10 = m.r10;
	e11 = m.r11;
	e12 = m.r12;

	e20 = m.r20;
	e21 = m.r21;
	e22 = m.r22;

	return *this;
}

/***************************************************************************/
// Invert, always works. (it's the transpose)
// Watch out, this function is not virtual
/***************************************************************************/
inline bool DirCos3x3::Invert(void)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_INVERT);
#endif //MATH_STATS

	// invert it
	DirCos3x3 inverse;
	if (!Invert(inverse))
		return false;

	// save it
	*this = inverse;
	return true;
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int DirCos3x3::operator==(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_EQUALS_M3X3);
#endif //MATH_STATS

	return(e00 == m.e00 && e01 == m.e01 && e02 == m.e02 &&
				e10 == m.e10 && e11 == m.e11 && e12 == m.e12 &&
				e20 == m.e20 && e21 == m.e21 && e22 == m.e22);
}

/***************************************************************************/
// equal?
/***************************************************************************/
inline int DirCos3x3::operator==(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_EQUALS_M3X3);
#endif //MATH_STATS

	return(e00 == m.e00 && e01 == m.e01 && e02 == m.e02 &&
				e10 == m.e10 && e11 == m.e11 && e12 == m.e12 &&
				e20 == m.e20 && e21 == m.e21 && e22 == m.e22);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int DirCos3x3::operator!=(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_NOT_EQUAL_M3X3);
#endif //MATH_STATS

	return(e00 != m.e00 || e01 != m.e01 || e02 != m.e02 ||
				e10 != m.e10 || e11 != m.e11 || e12 != m.e12 ||
				e20 != m.e20 || e21 != m.e21 || e22 != m.e22);
}

/***************************************************************************/
// not equal
/***************************************************************************/
inline int DirCos3x3::operator!=(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_NOT_EQUAL_M3X3);
#endif //MATH_STATS

	return(e00 != m.e00 || e01 != m.e01 || e02 != m.e02 ||
				e10 != m.e10 || e11 != m.e11 || e12 != m.e12 ||
				e20 != m.e20 || e21 != m.e21 || e22 != m.e22);
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 DirCos3x3::operator*(
float s) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_F);
#endif //MATH_STATS

	Matrix3x3 m;

	m.e00 = e00 * s;
	m.e01 = e01 * s;
	m.e02 = e02 * s;

	m.e10 = e10 * s;
	m.e11 = e11 * s;
	m.e12 = e12 * s;

	m.e20 = e20 * s;
	m.e21 = e21 * s;
	m.e22 = e22 * s;

	return m;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 DirCos3x3::operator/(
float s) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DIVIDE_F);
#endif //MATH_STATS

	Matrix3x3 m;
	float inv = 1.0f / s;

	m.e00 = e00 * inv;
	m.e01 = e01 * inv;
	m.e02 = e02 * inv;

	m.e10 = e10 * inv;
	m.e11 = e11 * inv;
	m.e12 = e12 * inv;

	m.e20 = e20 * inv;
	m.e21 = e21 * inv;
	m.e22 = e22 * inv;

	return m;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 DirCos3x3::operator+(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_PLUS_M3X3);
#endif //MATH_STATS

	Matrix3x3 sum;

	sum.e00 = e00 + m.e00;
	sum.e01 = e01 + m.e01;
	sum.e02 = e02 + m.e02;

	sum.e10 = e10 + m.e10;
	sum.e11 = e11 + m.e11;
	sum.e12 = e12 + m.e12;

	sum.e20 = e20 + m.e20;
	sum.e21 = e21 + m.e21;
	sum.e22 = e22 + m.e22;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 DirCos3x3::operator+(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_PLUS_M3X3);
#endif //MATH_STATS

	Matrix3x3 sum;

	sum.e00 = e00 + m.e00;
	sum.e01 = e01 + m.e01;
	sum.e02 = e02 + m.e02;

	sum.e10 = e10 + m.e10;
	sum.e11 = e11 + m.e11;
	sum.e12 = e12 + m.e12;

	sum.e20 = e20 + m.e20;
	sum.e21 = e21 + m.e21;
	sum.e22 = e22 + m.e22;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 DirCos3x3::operator-(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_MINUS_M3X3);
#endif //MATH_STATS

	Matrix3x3 sum;

	sum.e00 = e00 - m.e00;
	sum.e01 = e01 - m.e01;
	sum.e02 = e02 - m.e02;

	sum.e10 = e10 - m.e10;
	sum.e11 = e11 - m.e11;
	sum.e12 = e12 - m.e12;

	sum.e20 = e20 - m.e20;
	sum.e21 = e21 - m.e21;
	sum.e22 = e22 - m.e22;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 DirCos3x3::operator-(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_MINUS_M3X3);
#endif //MATH_STATS

	Matrix3x3 sum;

	sum.e00 = e00 - m.e00;
	sum.e01 = e01 - m.e01;
	sum.e02 = e02 - m.e02;

	sum.e10 = e10 - m.e10;
	sum.e11 = e11 - m.e11;
	sum.e12 = e12 - m.e12;

	sum.e20 = e20 - m.e20;
	sum.e21 = e21 - m.e21;
	sum.e22 = e22 - m.e22;

	return sum;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 DirCos3x3::operator*(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_M3X3);
#endif //MATH_STATS

	Matrix3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;

	return product;
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3 DirCos3x3::operator*(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_M3X3);
#endif //MATH_STATS

	DirCos3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;

	return product;
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3 &DirCos3x3::operator*=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_EQUALS_M3X3);
#endif //MATH_STATS

	DirCos3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;

	// copy it in
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 DirCos3x3::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_M4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;
	product.e03 = e00 * m.e03 + e01 * m.e13 + e02 * m.e23;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;
	product.e13 = e10 * m.e03 + e11 * m.e13 + e12 * m.e23;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;
	product.e23 = e20 * m.e03 + e21 * m.e13 + e22 * m.e23;

	product.e30 = m.e30;
	product.e31 = m.e31;
	product.e32 = m.e32;
	product.e33 = m.e33;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 DirCos3x3::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_G4X4);
#endif //MATH_STATS

	Graphics4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;
	product.e03 = 0.0f;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;
	product.e13 = 0.0f;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;
	product.e23 = 0.0f;

	product.e30 = m.e30;
	product.e31 = m.e31;
	product.e32 = m.e32;
	product.e33 = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 DirCos3x3::operator*(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_TIMES_G4X4);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20;
	product.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21;
	product.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22;
	product.e03 = 0.0f;

	product.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20;
	product.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21;
	product.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22;
	product.e13 = 0.0f;

	product.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20;
	product.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21;
	product.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22;
	product.e23 = 0.0f;

	product.e30 = m.e30;
	product.e31 = m.e31;
	product.e32 = m.e32;
	product.e33 = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 DirCos3x3::operator/(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DIVIDE_C4X4);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.e00 = e00 * m.r00 + e01 * m.r01 + e02 * m.r02;
	product.e01 = e00 * m.r10 + e01 * m.r11 + e02 * m.r12;
	product.e02 = e00 * m.r20 + e01 * m.r21 + e02 * m.r22;
	product.e03 = 0.0f;

	product.e10 = e10 * m.r00 + e11 * m.r01 + e12 * m.r02;
	product.e11 = e10 * m.r10 + e11 * m.r11 + e12 * m.r12;
	product.e12 = e10 * m.r20 + e11 * m.r21 + e12 * m.r22;
	product.e13 = 0.0f;

	product.e20 = e20 * m.r00 + e21 * m.r01 + e22 * m.r02;
	product.e21 = e20 * m.r10 + e21 * m.r11 + e22 * m.r12;
	product.e22 = e20 * m.r20 + e21 * m.r21 + e22 * m.r22;
	product.e23 = 0.0f;

	product.e30 = -m.tx * m.r00 - m.ty * m.r01 - m.tz * m.r02;
	product.e31 = -m.tx * m.r10 - m.ty * m.r11 - m.tz * m.r12;
	product.e32 = -m.tx * m.r20 - m.ty * m.r21 - m.tz * m.r12;
	product.e33 = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3 DirCos3x3::operator/(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DIVIDE_D3X3);
#endif //MATH_STATS

	DirCos3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e01 + e02 * m.e02;
	product.e01 = e00 * m.e10 + e01 * m.e11 + e02 * m.e12;
	product.e02 = e00 * m.e20 + e01 * m.e21 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e01 + e12 * m.e02;
	product.e11 = e10 * m.e10 + e11 * m.e11 + e12 * m.e12;
	product.e12 = e10 * m.e20 + e11 * m.e21 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e01 + e22 * m.e02;
	product.e21 = e20 * m.e10 + e21 * m.e11 + e22 * m.e12;
	product.e22 = e20 * m.e20 + e21 * m.e21 + e22 * m.e22;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3 &DirCos3x3::operator/=(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX3X3_DIVIDE_EQUALS_D3X3);
#endif //MATH_STATS

	DirCos3x3 product;

	product.e00 = e00 * m.e00 + e01 * m.e01 + e02 * m.e02;
	product.e01 = e00 * m.e10 + e01 * m.e11 + e02 * m.e12;
	product.e02 = e00 * m.e20 + e01 * m.e21 + e02 * m.e22;

	product.e10 = e10 * m.e00 + e11 * m.e01 + e12 * m.e02;
	product.e11 = e10 * m.e10 + e11 * m.e11 + e12 * m.e12;
	product.e12 = e10 * m.e20 + e11 * m.e21 + e12 * m.e22;

	product.e20 = e20 * m.e00 + e21 * m.e01 + e22 * m.e02;
	product.e21 = e20 * m.e10 + e21 * m.e11 + e22 * m.e12;
	product.e22 = e20 * m.e20 + e21 * m.e21 + e22 * m.e22;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix4x4 DirCos3x3::InvertTimes(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_INVERT_TIMES_M4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	product.e00 = e00 * m.e00 + e10 * m.e10 + e20 * m.e20;
	product.e01 = e00 * m.e01 + e10 * m.e11 + e20 * m.e21;
	product.e02 = e00 * m.e02 + e10 * m.e12 + e20 * m.e22;
	product.e03 = e00 * m.e03 + e10 * m.e13 + e20 * m.e23;

	product.e10 = e01 * m.e00 + e11 * m.e10 + e21 * m.e20;
	product.e11 = e01 * m.e01 + e11 * m.e11 + e21 * m.e21;
	product.e12 = e01 * m.e02 + e11 * m.e12 + e21 * m.e22;
	product.e13 = e01 * m.e03 + e11 * m.e13 + e21 * m.e23;

	product.e20 = e02 * m.e00 + e12 * m.e10 + e22 * m.e20;
	product.e21 = e02 * m.e01 + e12 * m.e11 + e22 * m.e21;
	product.e22 = e02 * m.e02 + e12 * m.e12 + e22 * m.e22;
	product.e23 = e02 * m.e03 + e12 * m.e13 + e22 * m.e23;

	product.e30 = m.e30;
	product.e31 = m.e31;
	product.e32 = m.e32;
	product.e33 = m.e33;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Graphics4x4 DirCos3x3::InvertTimes(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_INVERT_TIMES_G4X4);
#endif //MATH_STATS

	Graphics4x4 product;

	product.e00 = e00 * m.e00 + e10 * m.e10 + e20 * m.e20;
	product.e01 = e00 * m.e01 + e10 * m.e11 + e20 * m.e21;
	product.e02 = e00 * m.e02 + e10 * m.e12 + e20 * m.e22;
	product.e03 = 0.0f;

	product.e10 = e01 * m.e00 + e11 * m.e10 + e21 * m.e20;
	product.e11 = e01 * m.e01 + e11 * m.e11 + e21 * m.e21;
	product.e12 = e01 * m.e02 + e11 * m.e12 + e21 * m.e22;
	product.e13 = 0.0f;

	product.e20 = e02 * m.e00 + e12 * m.e10 + e22 * m.e20;
	product.e21 = e02 * m.e01 + e12 * m.e11 + e22 * m.e21;
	product.e22 = e02 * m.e02 + e12 * m.e12 + e22 * m.e22;
	product.e23 = 0.0f;

	product.e30 = m.e30;
	product.e31 = m.e31;
	product.e32 = m.e32;
	product.e33 = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline Matrix3x3 DirCos3x3::InvertTimes(
const Matrix3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_INVERT_TIMES_M3X3);
#endif //MATH_STATS

	Matrix3x3 product;

	product.e00 = e00 * m.e00 + e10 * m.e10 + e20 * m.e20;
	product.e01 = e00 * m.e01 + e10 * m.e11 + e20 * m.e21;
	product.e02 = e00 * m.e02 + e10 * m.e12 + e20 * m.e22;

	product.e10 = e01 * m.e00 + e11 * m.e10 + e21 * m.e20;
	product.e11 = e01 * m.e01 + e11 * m.e11 + e21 * m.e21;
	product.e12 = e01 * m.e02 + e11 * m.e12 + e21 * m.e22;

	product.e20 = e02 * m.e00 + e12 * m.e10 + e22 * m.e20;
	product.e21 = e02 * m.e01 + e12 * m.e11 + e22 * m.e21;
	product.e22 = e02 * m.e02 + e12 * m.e12 + e22 * m.e22;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3 DirCos3x3::InvertTimes(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_INVERT_TIMES_M3X3);
#endif //MATH_STATS

	DirCos3x3 product;

	product.e00 = e00 * m.e00 + e10 * m.e10 + e20 * m.e20;
	product.e01 = e00 * m.e01 + e10 * m.e11 + e20 * m.e21;
	product.e02 = e00 * m.e02 + e10 * m.e12 + e20 * m.e22;

	product.e10 = e01 * m.e00 + e11 * m.e10 + e21 * m.e20;
	product.e11 = e01 * m.e01 + e11 * m.e11 + e21 * m.e21;
	product.e12 = e01 * m.e02 + e11 * m.e12 + e21 * m.e22;

	product.e20 = e02 * m.e00 + e12 * m.e10 + e22 * m.e20;
	product.e21 = e02 * m.e01 + e12 * m.e11 + e22 * m.e21;
	product.e22 = e02 * m.e02 + e12 * m.e12 + e22 * m.e22;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3 &DirCos3x3::InvertTimesEquals(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_INVERT_TIMES_EQUALS_D3X3);
#endif //MATH_STATS

	DirCos3x3 product;

	product.e00 = e00 * m.e00 + e10 * m.e10 + e20 * m.e20;
	product.e01 = e00 * m.e01 + e10 * m.e11 + e20 * m.e21;
	product.e02 = e00 * m.e02 + e10 * m.e12 + e20 * m.e22;

	product.e10 = e01 * m.e00 + e11 * m.e10 + e21 * m.e20;
	product.e11 = e01 * m.e01 + e11 * m.e11 + e21 * m.e21;
	product.e12 = e01 * m.e02 + e11 * m.e12 + e21 * m.e22;

	product.e20 = e02 * m.e00 + e12 * m.e10 + e22 * m.e20;
	product.e21 = e02 * m.e01 + e12 * m.e11 + e22 * m.e21;
	product.e22 = e02 * m.e02 + e12 * m.e12 + e22 * m.e22;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline Cartesian4x4 DirCos3x3::InvertDivide(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_INVERT_DIVIDE_C4X4);
#endif //MATH_STATS

	Cartesian4x4 product;

	product.r00 = e00 * m.r00 + e10 * m.r01 + e20 * m.r02;
	product.r01 = e00 * m.r10 + e10 * m.r11 + e20 * m.r12;
	product.r02 = e00 * m.r20 + e10 * m.r21 + e20 * m.r22;
	product.u0 = 0.0f;

	product.r10 = e01 * m.r00 + e11 * m.r01 + e21 * m.r02;
	product.r11 = e01 * m.r10 + e11 * m.r11 + e21 * m.r12;
	product.r12 = e01 * m.r20 + e11 * m.r21 + e21 * m.r22;
	product.u1 = 0.0f;

	product.r20 = e02 * m.r00 + e12 * m.r01 + e22 * m.r02;
	product.r21 = e02 * m.r10 + e12 * m.r11 + e22 * m.r12;
	product.r22 = e02 * m.r20 + e12 * m.r21 + e22 * m.r22;
	product.u2 = 0.0f;

	product.tx = -m.tx * m.r00 - m.ty * m.r01 - m.tz * m.r02;
	product.ty = -m.tx * m.r10 - m.ty * m.r11 - m.tz * m.r12;
	product.tz = -m.tx * m.r20 - m.ty * m.r21 - m.tz * m.r22;
	product.w = 1.0f;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3 DirCos3x3::InvertDivide(
const DirCos3x3 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_INVERT_DIVIDE_D3X3);
#endif //MATH_STATS

	DirCos3x3 product;

	product.e00 = e00 * m.e00 + e10 * m.e01 + e20 * m.e02;
	product.e01 = e00 * m.e10 + e10 * m.e11 + e20 * m.e12;
	product.e02 = e00 * m.e20 + e10 * m.e21 + e20 * m.e22;

	product.e10 = e01 * m.e00 + e11 * m.e01 + e21 * m.e02;
	product.e11 = e01 * m.e10 + e11 * m.e11 + e21 * m.e12;
	product.e12 = e01 * m.e20 + e11 * m.e21 + e21 * m.e22;

	product.e20 = e02 * m.e00 + e12 * m.e01 + e22 * m.e02;
	product.e21 = e02 * m.e10 + e12 * m.e11 + e22 * m.e12;
	product.e22 = e02 * m.e20 + e12 * m.e21 + e22 * m.e22;
 
	return product;
}

/***************************************************************************/
/***************************************************************************/
inline DirCos3x3 &DirCos3x3::InvertDivideEquals(
const DirCos3x3 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(DIRCOS3X3_INVERT_DIVIDE_EQUALS_D3X3);
#endif //MATH_STATS

	DirCos3x3 product;

	product.e00 = e00 * m.e00 + e10 * m.e01 + e20 * m.e02;
	product.e01 = e00 * m.e10 + e10 * m.e11 + e20 * m.e12;
	product.e02 = e00 * m.e20 + e10 * m.e21 + e20 * m.e22;

	product.e10 = e01 * m.e00 + e11 * m.e01 + e21 * m.e02;
	product.e11 = e01 * m.e10 + e11 * m.e11 + e21 * m.e12;
	product.e12 = e01 * m.e20 + e11 * m.e21 + e21 * m.e22;

	product.e20 = e02 * m.e00 + e12 * m.e01 + e22 * m.e02;
	product.e21 = e02 * m.e10 + e12 * m.e11 + e22 * m.e12;
	product.e22 = e02 * m.e20 + e12 * m.e21 + e22 * m.e22;
 
	*this = product;
	return *this;
}

/***************************************************************************/
/***************************************************************************/
inline void DirCos3x3::SetAngleAxis(
float angle,
Vector3CRef axis)
{
	// Make sure the input is good.
	ASSERT(Math::Equal(axis.LengthSquared(), 1.0f, 1.0e-4f));

	// Need the sine and cosine.
	float sinAngle, cosAngle;
	Math::SinCos(angle, sinAngle, cosAngle);

	// Precompute some terms.
	Vector3 oneMinusCosAxis = (1.0f - cosAngle) * axis;
	Vector3 sinAxis = sinAngle * axis;

	// Find the components.
	e00 = oneMinusCosAxis.X() * axis.X() + cosAngle;
	e11 = oneMinusCosAxis.Y() * axis.Y() + cosAngle;
	e22 = oneMinusCosAxis.Z() * axis.Z() + cosAngle;

	e01 = oneMinusCosAxis.X() * axis.Y();
	e02 = oneMinusCosAxis.X() * axis.Z();
	e12 = oneMinusCosAxis.Y() * axis.Z();

	e10 = e01 - sinAxis.Z();
	e01 += sinAxis.Z();

	e20 = e02 + sinAxis.Y();
	e02 -= sinAxis.Y();

	e21 = e12 - sinAxis.X();
	e12 += sinAxis.X();
}

/***************************************************************************/
// Given a vector, create a direction cosine matrix. Three orthogonal axes
// will be created. The first, or primary axis, will be the provided vector
// and will form the row specified in i_primaryRow. The second, or secondary
// axis, will lie on a line that is perpendicular to the primary axis and and
// that lies in the plane formed by the primary axis and the principle axis
// that is the closest to being perpendicular to the primary axis (that is,
// the secondary axis will lie on the line that is perpendicular to the
// primary axis and is as close to one of the principle axes as
// possible). Call the identified principle axis the up axis. The direction
// of the secondary axis will selected such that the angle between the the
// secondary axis and the up axis will be the smaller of the two
// possibilities. The secondary axis will form the row specified in
// i_secondaryRow. The remaining or tertiary axis will be perpendicular to
// the primary and secondary axes and oriented so that a right-handed
// coordinate system is formed. The tertiary axis will form the remaining
// row. If the provided primary axis is of zero length, the identity matrix
// will be created.
/***************************************************************************/
inline DirCos3x3 &DirCos3x3::BuildFromAxis(Vector3CRef i_primaryAxis,
		int i_primaryRow, int i_secondaryRow)
{
	// Make sure input is valid.
	ASSERT((i_primaryRow >= 0) && (i_primaryRow < 3) && (i_secondaryRow >= 0) &&
			(i_secondaryRow < 3) && (i_primaryRow != i_secondaryRow));

	// Make sure the primary axis is of unit length.
	Vector3 primaryAxis = i_primaryAxis;
	if (!primaryAxis.SafeNormalize())
		Identity();
	else
	{
		// Find the tertiary axis.
		Vector3 primaryAbs = primaryAxis.Abs();
		Vector3 tertiaryAxis;
		if (primaryAbs.X() < primaryAbs.Y())
		{
			if (primaryAbs.X() < primaryAbs.Z())
				tertiaryAxis.Set(0.0f, -primaryAxis.Z(), primaryAxis.Y());
			else
				tertiaryAxis.Set(-primaryAxis.Y(), primaryAxis.X(), 0.0f);
		}
		else if (primaryAbs.Y() < primaryAbs.Z())
			tertiaryAxis.Set(primaryAxis.Z(), 0.0f, -primaryAxis.X());
		else
			tertiaryAxis.Set(-primaryAxis.Y(), primaryAxis.X(), 0.0f);
		tertiaryAxis.Normalize();

		// Set the rows.
		SetRow(i_primaryRow, primaryAxis);
		SetRow(i_secondaryRow, primaryAxis.Cross(tertiaryAxis));
		if (l_buildFromAxisInfo[i_primaryRow][i_secondaryRow].d_flipSecondaryAxis)
			tertiaryAxis.Negate();
		SetRow(l_buildFromAxisInfo[i_primaryRow][i_secondaryRow].d_tertiaryRow,
				tertiaryAxis);
	}
	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline void DirCos3x3::SetScale(
float xScale,
float yScale,
float zScale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void DirCos3x3::SetXScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void DirCos3x3::SetYScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void DirCos3x3::SetZScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void DirCos3x3::CatScale(
float xScale,
float yScale,
float zScale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void DirCos3x3::CatScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void DirCos3x3::CatXScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void DirCos3x3::CatYScale(
float scale)
{
	ASSERT(false);
}

/***************************************************************************/
/***************************************************************************/
inline void DirCos3x3::CatZScale(
float scale)
{
	ASSERT(false);
}

/******** for later *****************/

//inline Matrix3x3 &Matrix3x3::Product(Matrix3x3 &m1, Matrix3x3 &m2)
//{
//	asm __volatile__("
//
//	addiu    sp,sp,-16
//	swc1     $f23,12(sp)
//	swc1     $f22,8(sp)
//	swc1     $f21,4(sp)
//	swc1     $f20,0(sp)
//
//	lwc1     $f8,0(&m1)
//	lwc1     $f12,4(&m2)
//	lwc1     $f22,8(&m2)
//	lwc1     $f14,0(&m2)
//	lwc1     $f11,4(&m1)
//	lwc1     $f20,16(&m2)
//	lwc1     $f3,20(&m2)
//	mul.s    $f7,$f8,$f12
//	lwc1     $f16,12(&m1)
//	lwc1     $f0,8(&m1)
//	lwc1     $f1,28(&m2)
//	lwc1     $f13,12(&m2)
//	mul.s    $f6,$f11,$f20
//	lwc1     $f21,16(&m1)
//	lwc1     $f4,32(&m2)
//	lwc1     $f5,24(&m2)
//	lwc1     $f2,20(&m1)
//	adda.s   $f7,$f6
//	mul.s    $f10,$f8,$f22
//	lwc1     $f6,28(&m1)
//	lwc1     $f23,24(&m1)
//	lwc1     $f7,32(&m1)
//	mul.s    $f9,$f8,$f14
//	mul.s    $f15,$f16,$f12
//	mul.s    $f8,$f11,$f3
//	madd.s   $f17,$f0,$f1
//	adda.s   $f10,$f8
//	mul.s    $f8,$f11,$f13
//	mul.s    $f11,$f16,$f14
//	mul.s    $f10,$f21,$f13
//	madd.s   $f18,$f0,$f4
//	adda.s   $f11,$f10
//	mul.s    $f11,$f21,$f20
//	madd.s   $f19,$f2,$f5
//	adda.s   $f15,$f11
//	mul.s    $f15,$f21,$f3
//	mul.s    $f13,$f6,$f13
//	mul.s    $f11,$f6,$f20
//	mul.s    $f3,$f6,$f3
//	mul.s    $f16,$f16,$f22
//	madd.s   $f6,$f2,$f1
//	adda.s   $f16,$f15
//	mul.s    $f14,$f23,$f14
//	madd.s   $f2,$f2,$f4
//	adda.s   $f14,$f13
//	mul.s    $f12,$f23,$f12
//	madd.s   $f13,$f7,$f5
//	adda.s   $f12,$f11
//	mul.s    $f10,$f23,$f22
//	madd.s   $f1,$f7,$f1
//	adda.s   $f10,$f3
//	madd.s   $f3,$f7,$f4
//	adda.s   $f9,$f8
//	madd.s   $f0,$f0,$f5
//	swc1     $f0,0(this)
//	swc1     $f17,4(this)
//	swc1     $f18,8(this)
//	swc1     $f19,12(this)
//	swc1     $f6,16(this)
//	swc1     $f2,20(this)
//	swc1     $f13,24(this)
//	swc1     $f1,28(this)
//	swc1     $f3,32(this)
//
//	lwc1     $f23,12(sp)
//	lwc1     $f22,8(sp)
//	lwc1     $f21,4(sp)
//	lwc1     $f20,0(sp)
//	addiu    sp,sp,16
//
//	": : : );
//
////	e00 = m1.e00 * m2.e00 + m1.e01 * m2.e10 + m1.e02 * m2.e20;
////	e01 = m1.e00 * m2.e01 + m1.e01 * m2.e11 + m1.e02 * m2.e21;
////	e02 = m1.e00 * m2.e02 + m1.e01 * m2.e12 + m1.e02 * m2.e22;
////			  					    			  	      			  
////	e10 = m1.e10 * m2.e00 + m1.e11 * m2.e10 + m1.e12 * m2.e20;
////	e11 = m1.e10 * m2.e01 + m1.e11 * m2.e11 + m1.e12 * m2.e21;
////	e12 = m1.e10 * m2.e02 + m1.e11 * m2.e12 + m1.e12 * m2.e22;
////			  					    			  	      			  
////	e20 = m1.e20 * m2.e00 + m1.e21 * m2.e10 + m1.e22 * m2.e20;
////	e21 = m1.e20 * m2.e01 + m1.e21 * m2.e11 + m1.e22 * m2.e21;
////	e22 = m1.e20 * m2.e02 + m1.e21 * m2.e12 + m1.e22 * m2.e22;
//
//	return(*this);
//}

