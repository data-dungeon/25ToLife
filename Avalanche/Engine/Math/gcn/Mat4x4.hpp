/***************************************************************************/
// Implementations for the matrix class
/***************************************************************************/

/***************************************************************************/
// GCN implementation
/***************************************************************************/

/***************************************************************************/
// if this file is empty, it means that no routines have been given a
// platform-specific implementation for this platform
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_MAT4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 &Matrix4x4::FastMultiply(const Matrix4x4 &m1, const Matrix4x4 &m2)	// this = m1 * m2
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_FAST_MULTIPLY);
#endif //MATH_STATS

	/* must handle this == m1 or this == m2, so can't calculate directly into this */
	Matrix4x4 product;

	float a0, a1, a2, a3; // keep a row at a time

	// entire b matrix
	float b00, b01, b02, b03;
	float b10, b11, b12, b13;
	float b20, b21, b22, b23;
	float b30, b31, b32, b33;

	b00 = m2.e00;
	b10 = m2.e10;
	b20 = m2.e20;
	b30 = m2.e30;

	b01 = m2.e01;
	b11 = m2.e11;
	b21 = m2.e21;
	b31 = m2.e31;

	b02 = m2.e02;
	b12 = m2.e12;
	b22 = m2.e22;
	b32 = m2.e32;

	b03 = m2.e03;
	b13 = m2.e13;
	b23 = m2.e23;
	b33 = m2.e33;

	//process row 0
	a0 = m1.e00;
	a1 = m1.e01;
	a2 = m1.e02;
	a3 = m1.e03;

	product.e00 = a0 * b00 + a1 * b10 + a2 * b20 + a3 * b30;
	product.e01 = a0 * b01 + a1 * b11 + a2 * b21 + a3 * b31;
	product.e02 = a0 * b02 + a1 * b12 + a2 * b22 + a3 * b32;
	product.e03 = a0 * b03 + a1 * b13 + a2 * b23 + a3 * b33;

	//process row 1
	a0 = m1.e10;
	a1 = m1.e11;
	a2 = m1.e12;
	a3 = m1.e13;

	product.e10 = a0 * b00 + a1 * b10 + a2 * b20 + a3 * b30;
	product.e11 = a0 * b01 + a1 * b11 + a2 * b21 + a3 * b31;
	product.e12 = a0 * b02 + a1 * b12 + a2 * b22 + a3 * b32;
	product.e13 = a0 * b03 + a1 * b13 + a2 * b23 + a3 * b33;

	//process row 2
	a0 = m1.e20;
	a1 = m1.e21;
	a2 = m1.e22;
	a3 = m1.e23;

	product.e20 = a0 * b00 + a1 * b10 + a2 * b20 + a3 * b30;
	product.e21 = a0 * b01 + a1 * b11 + a2 * b21 + a3 * b31;
	product.e22 = a0 * b02 + a1 * b12 + a2 * b22 + a3 * b32;
	product.e23 = a0 * b03 + a1 * b13 + a2 * b23 + a3 * b33;

	//process row 3
	a0 = m1.e30;
	a1 = m1.e31;
	a2 = m1.e32;
	a3 = m1.e33;

	product.e30 = a0 * b00 + a1 * b10 + a2 * b20 + a3 * b30;
	product.e31 = a0 * b01 + a1 * b11 + a2 * b21 + a3 * b31;
	product.e32 = a0 * b02 + a1 * b12 + a2 * b22 + a3 * b32;
	product.e33 = a0 * b03 + a1 * b13 + a2 * b23 + a3 * b33;

	// copy it in
	*this = product;
	
	return *this;
}

#endif //EXCLUDE_GENERIC_MAT4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_MAT4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Matrix4x4::operator*( const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_M4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply( *this, m);

	return product;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 &Matrix4x4::operator*=( const Matrix4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_M4X4);
#endif //MATH_STATS

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	this->FastMultiply( *this, m);

	return *this;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_MAT4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_MAT4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Matrix4x4::operator*( const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_G4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply( *this, m);

	return product;
}

inline Matrix4x4 Matrix4x4::operator*( const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_G4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply( *this, m);

	return product;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 &Matrix4x4::operator*=( const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	this->FastMultiply( *this, m);

	return *this;
}

inline Matrix4x4 &Matrix4x4::operator*=( const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	this->FastMultiply( *this, m);

	return *this;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_GRAPHICS4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

inline Graphics4x4 &Graphics4x4::FastMultiply(const Graphics4x4 &m1, const Graphics4x4 &m2) //this = m1 * m2
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_FAST_MULTIPLY);
#endif //MATH_STATS

	/* must handle this == m1 or this == m2, so can't calculate directly into this */
	Graphics4x4 product;

	float a0, a1, a2; // keep a row at a time

	// entire b matrix
	float b00, b01, b02;
	float b10, b11, b12;
	float b20, b21, b22;
	float b30, b31, b32;

	b00 = m2.e00;
	b10 = m2.e10;
	b20 = m2.e20;
	b30 = m2.e30;

	b01 = m2.e01;
	b11 = m2.e11;
	b21 = m2.e21;
	b31 = m2.e31;

	b02 = m2.e02;
	b12 = m2.e12;
	b22 = m2.e22;
	b32 = m2.e32;

	//process row 0
	a0 = m1.e00;
	a1 = m1.e01;
	a2 = m1.e02;

	product.e00 = a0 * b00 + a1 * b10 + a2 * b20;
	product.e01 = a0 * b01 + a1 * b11 + a2 * b21;
	product.e02 = a0 * b02 + a1 * b12 + a2 * b22;
	product.e03 = 0.0f;

	//process row 1
	a0 = m1.e10;
	a1 = m1.e11;
	a2 = m1.e12;

	product.e10 = a0 * b00 + a1 * b10 + a2 * b20;
	product.e11 = a0 * b01 + a1 * b11 + a2 * b21;
	product.e12 = a0 * b02 + a1 * b12 + a2 * b22;
	product.e13 = 0.0f;

	//process row 2
	a0 = m1.e20;
	a1 = m1.e21;
	a2 = m1.e22;

	product.e20 = a0 * b00 + a1 * b10 + a2 * b20;
	product.e21 = a0 * b01 + a1 * b11 + a2 * b21;
	product.e22 = a0 * b02 + a1 * b12 + a2 * b22;
	product.e23 = 0.0f;

	//process row 3
	a0 = m1.e30;
	a1 = m1.e31;
	a2 = m1.e32;

	product.e30 = a0 * b00 + a1 * b10 + a2 * b20 + b30;
	product.e31 = a0 * b01 + a1 * b11 + a2 * b21 + b31;
	product.e32 = a0 * b02 + a1 * b12 + a2 * b22 + b32;
	product.e33 = 1.0f;

	// copy it in
	*this = product;

	return *this;
}

#endif //EXCLUDE_GENERIC_MAT4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/
#ifdef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Graphics4x4::operator*( const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_M4X4);
#endif //MATH_STATS

	Matrix4x4 product;

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply(*this, m);

	return product;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Graphics4x4 Graphics4x4::operator*( const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
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

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply(*this, m);

	return product;
}

inline Graphics4x4 Graphics4x4::operator*( const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
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

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply(*this, m);

	return product;
}

inline Cartesian4x4 Cartesian4x4::operator*( const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
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

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply(*this, m);

	return product;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Graphics4x4 &Graphics4x4::operator*=( const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
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

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	this->FastMultiply( *this, m);

	return *this;
}

inline Graphics4x4 &Graphics4x4::operator*=( const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
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

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	this->FastMultiply( *this, m);

	return *this;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/


