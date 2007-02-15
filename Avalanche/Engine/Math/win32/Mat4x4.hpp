/***************************************************************************/
// Implementations for the matrix class
/***************************************************************************/

/***************************************************************************/
// WIN32 implementation
/***************************************************************************/

/***************************************************************************/
// if this file is empty, it means that no routines have been given a
// platform-specific implementation for this platform
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_MAT4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Matrix4x4::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_M4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	Matrix4x4 product;

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply(*this, m);

	return product;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 &Matrix4x4::operator*=(
const Matrix4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_M4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	this->FastMultiply(*this, m);

	return *this;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_MAT4X4	// compile control from matrix.h for platform-specific implementation

#ifdef EXCLUDE_GENERIC_MAT4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Matrix4x4::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	Matrix4x4 product;

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply(*this, m);

	return product;
}

inline Matrix4x4 Matrix4x4::operator*(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	Matrix4x4 product;

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply(*this, m);

	return product;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 &Matrix4x4::operator*=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	this->FastMultiply(*this, m);

	return *this;
}

inline Matrix4x4 &Matrix4x4::operator*=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	this->FastMultiply(*this, m);

	return *this;
}

#endif //EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_MAT4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 &Matrix4x4::FastMultiply(const Matrix4x4 &m1, const Matrix4x4 &m2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_FAST_MULTIPLY);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m1));
	ASSERT(IS_SIMD_ALIGNED(&m2));
#endif //CHECK_MATH_ALIGNMENT

	/* this routine must support this == m1, this == m2. the simd implementation supports this by loading up all of */
	/* m2 into mmx registers, and then using m1.row0 before writing into this->row0, etc */

	__asm
	{
		push	eax
		push	ecx
		push	edx

		; load up variables

		mov	eax,this	; dest
		mov	edx,m1	; source 1
		mov	ecx,m2	; source 2

		// from http://developer.intel.com/design/pentiumiii/sml/24504501.pdf, page 14. comments are mine

		// instructions used (source: instruction set from ftp://download.intel.com/design/Pentium4/manuals/24547104.pdf):
		//
		//		movss		page 525		move scalar single-precision float
		//		movaps	page 483		move aligned packed single-precision float (quad word)
		//		shufps	page 755		shuffle packed single-precision float
		//		mulps		page 540		multiply packed single-precision float
		//		addps		page 65		add packed single-precision float
		//		movlps	page 500		move low packed single-precision float
		//		movhps	page 495		move high packed single-precision float

		movss		xmm0, dword ptr [edx]				// xmm0.x = m1.e00
		movaps	xmm1, xmmword ptr [ecx]				// xmm1 = m2 first row
		shufps	xmm0, xmm0, 0							// xmm0 = m1.e00 in all 4 floats
		movss		xmm2, dword ptr [edx+4]				// xmm2.x = m1.e01
		mulps		xmm0, xmm1								// xmm0.x = m1.e00 * m2.e00
																// xmm0.y = m1.e00 * m2.e01
																// xmm0.z = m1.e00 * m2.e02
																// xmm0.w = m1.e00 * m2.e03
		shufps	xmm2, xmm2, 0							// xmm2 = m1.e01 in all 4 floats
		movaps	xmm3, xmmword ptr [ecx+10h]		// xmm3 = m2 second row
		movss		xmm7, dword ptr [edx+8]				// xmm7.x = m1.e02
		mulps		xmm2, xmm3								// xmm2.x = m1.e01 * m2.e10
																// xmm2.y = m1.e01 * m2.e11
																// xmm2.z = m1.e01 * m2.e12
																// xmm2.w = m1.e01 * m2.e13
		shufps	xmm7, xmm7, 0							// xmm7 = m1.e02 in all 4 floats
		addps		xmm0, xmm2								// xmm0.x = (m1.e00 * m2.e00) + (m1.e01 * m2.e10)
																// xmm0.y = (m1.e00 * m2.e01) + (m1.e01 * m2.e12)
																// xmm0.z = (m1.e00 * m2.e02) + (m1.e01 * m2.e13)
																// xmm0.w = (m1.e00 * m2.e03) + (m1.e01 * m2.e14)
		movaps	xmm4, xmmword ptr [ecx+20h]		// xmm4 = m2 third row
		movss		xmm2, dword ptr [edx+0Ch]			// xmm2.x = m1.e03
		mulps		xmm7, xmm4								// xmm7.x = m1.e02 * m2.e20
																// xmm7.y = m1.e02 * m2.e21
																// xmm7.z = m1.e02 * m2.e22
																// xmm7.w = m1.e02 * m2.e23
		shufps	xmm2, xmm2, 0							// xmm2 = m1.e03 in all 4 floats
		addps		xmm0, xmm7								// xmm0.x = (m1.e00 * m2.e00) + (m1.e01 * m2.e10) + (m1.e02 * m2.e20)
																// xmm0.y = (m1.e00 * m2.e01) + (m1.e01 * m2.e11) + (m1.e02 * m2.e21)
																// xmm0.z = (m1.e00 * m2.e02) + (m1.e01 * m2.e12) + (m1.e02 * m2.e22)
																// xmm0.w = (m1.e00 * m2.e03) + (m1.e01 * m2.e13) + (m1.e02 * m2.e23)
		movaps	xmm5, xmmword ptr [ecx+30h]		// xmm5 = m2 fourth row
		movss		xmm6, dword ptr [edx+10h]			// xmm6.x = m1.e10
		mulps		xmm2, xmm5								// xmm2.x = m1.e03 * m2.e30
																// xmm2.y = m1.e03 * m2.e31
																// xmm2.z = m1.e03 * m2.e32
																// xmm2.w = m1.e03 * m2.e33
		movss		xmm7, dword ptr [edx+14h]			// xmm7.x = m1.e11
		shufps	xmm6, xmm6, 0							// xmm6 = m1.e10 in all 4 floats
		addps		xmm0, xmm2								// xmm0.x = (m1.e00 * m2.e00) + (m1.e01 * m2.e10) + (m1.e02 * m2.e20) + (m1.e03 * m2.e30)
																// xmm0.y = (m1.e00 * m2.e01) + (m1.e01 * m2.e11) + (m1.e02 * m2.e21) + (m1.e03 * m2.e31)
																// xmm0.z = (m1.e00 * m2.e02) + (m1.e01 * m2.e12) + (m1.e02 * m2.e22) + (m1.e03 * m2.e32)
																// xmm0.w = (m1.e00 * m2.e03) + (m1.e01 * m2.e13) + (m1.e02 * m2.e23) + (m1.e03 * m2.e33)
		shufps	xmm7, xmm7, 0							// xmm7 = m1.e11 in all 4 floats
		movlps	qword ptr [eax], xmm0				// write to this->e00, this->e01
		movhps	qword ptr [eax+8], xmm0				// write to this->e02, this->e03

		// row 2. movss xmm6, shufps xmm6, movss xmm7, shufps xmm7 are part of the row 2 calculation

		mulps		xmm7, xmm3
		movss		xmm0, dword ptr [edx+18h]
		mulps		xmm6, xmm1
		shufps	xmm0, xmm0, 0
		addps		xmm6, xmm7
		mulps		xmm0, xmm4
		movss		xmm2, dword ptr [edx+24h]
		addps		xmm6, xmm0
		movss		xmm0, dword ptr [edx+1Ch]
		movss		xmm7, dword ptr [edx+20h]
		shufps	xmm0, xmm0, 0
		shufps	xmm7, xmm7, 0
		mulps		xmm0, xmm5
		mulps		xmm7, xmm1
		addps		xmm6, xmm0
		shufps	xmm2, xmm2, 0
		movlps	qword ptr [eax+10h], xmm6
		movhps	qword ptr [eax+18h], xmm6

		// row 3. movss xmm2, shufps xmm2, movss xmm7, shufps xmm7 are part of the row 3 calculation

		mulps		xmm2, xmm3
		movss		xmm6, dword ptr [edx+28h]
		addps		xmm7, xmm2
		shufps	xmm6, xmm6, 0
		movss		xmm2, dword ptr [edx+2Ch]
		mulps		xmm6, xmm4
		shufps	xmm2, xmm2, 0
		addps		xmm7, xmm6
		mulps		xmm2, xmm5
		movss		xmm0, dword ptr [edx+34h]
		addps		xmm7, xmm2
		shufps	xmm0, xmm0, 0
		movlps	qword ptr [eax+20h], xmm7
		movss		xmm2, dword ptr [edx+30h]
		movhps	qword ptr [eax+28h], xmm7

		// row 4. movss xmm0, shufps xmm0, movss xmm2 are part of the row 3 calculation

		mulps		xmm0, xmm3
		shufps	xmm2, xmm2, 0
		movss		xmm6, dword ptr [edx+38h]
		mulps		xmm2, xmm1
		shufps	xmm6, xmm6, 0
		addps		xmm2, xmm0
		mulps		xmm6, xmm4
		movss		xmm7, dword ptr [edx+3Ch]
		shufps	xmm7, xmm7, 0
		addps		xmm2, xmm6
		mulps		xmm7, xmm5
		addps		xmm2, xmm7
		movaps	xmmword ptr [eax+30h], xmm2

		; done

		pop	edx
		pop	ecx
		pop	eax
	}

	return *this;
}

#endif //EXCLUDE_GENERIC_MAT4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

#ifdef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Graphics4x4::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_M4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	Matrix4x4 product;

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply(*this, m);

	return product;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Graphics4x4 Graphics4x4::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

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

inline Graphics4x4 Graphics4x4::operator*(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

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

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Graphics4x4 &Graphics4x4::operator*=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

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

	this->FastMultiply(*this, m);

	return *this;
}

inline Graphics4x4 &Graphics4x4::operator*=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

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

	this->FastMultiply(*this, m);

	return *this;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

#ifdef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

inline Matrix4x4 Cartesian4x4::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_M4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	Matrix4x4 product;

	/* use the FastMultiply to improve performance on overlaoded operator version as well */

	product.FastMultiply(*this, m);

	return product;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

inline Graphics4x4 Cartesian4x4::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

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

inline Cartesian4x4 Cartesian4x4::operator*(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

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

inline Cartesian4x4 &Cartesian4x4::operator*=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

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

	this->FastMultiply(*this, m);

	return *this;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_GRAPHICS4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

inline Graphics4x4 &Graphics4x4::FastMultiply(const Graphics4x4 &m1, const Graphics4x4 &m2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_FAST_MULTIPLY);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&m1));
	ASSERT(IS_SIMD_ALIGNED(&m2));
#endif //CHECK_MATH_ALIGNMENT

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(m1.u0 == 0.0f);
	ASSERT(m1.u1 == 0.0f);
	ASSERT(m1.u2 == 0.0f);
	ASSERT(m1.w == 1.0f);
	ASSERT(m2.u0 == 0.0f);
	ASSERT(m2.u1 == 0.0f);
	ASSERT(m2.u2 == 0.0f);
	ASSERT(m2.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	/* this routine must support this == m1, this == m2. the simd implementation supports this by loading up all of */
	/* m2 into mmx registers, and then using m1.row0 before writing into this->row0, etc */

	__asm
	{
		push	eax
		push	ecx
		push	edx

		; load up variables

		mov	eax,this	; dest
		mov	edx,m1	; source 1
		mov	ecx,m2	; source 2

		// from http://developer.intel.com/design/pentiumiii/sml/24504501.pdf, page 14. comments are mine

		// instructions used (source: instruction set from ftp://download.intel.com/design/Pentium4/manuals/24547104.pdf):
		//
		//		movss		page 525		move scalar single-precision float
		//		movaps	page 483		move aligned packed single-precision float (quad word)
		//		shufps	page 755		shuffle packed single-precision float
		//		mulps		page 540		multiply packed single-precision float
		//		addps		page 65		add packed single-precision float
		//		movlps	page 500		move low packed single-precision float
		//		movhps	page 495		move high packed single-precision float

		movss		xmm0, dword ptr [edx]				// xmm0.x = m1.e00
		movaps	xmm1, xmmword ptr [ecx]				// xmm1 = m2 first row
		shufps	xmm0, xmm0, 0							// xmm0 = m1.e00 in all 4 floats
		movss		xmm2, dword ptr [edx+4]				// xmm2.x = m1.e01
		mulps		xmm0, xmm1								// xmm0.x = m1.e00 * m2.e00
																// xmm0.y = m1.e00 * m2.e01
																// xmm0.z = m1.e00 * m2.e02
																// xmm0.w = m1.e00 * m2.e03
		shufps	xmm2, xmm2, 0							// xmm2 = m1.e01 in all 4 floats
		movaps	xmm3, xmmword ptr [ecx+10h]		// xmm3 = m2 second row
		movss		xmm7, dword ptr [edx+8]				// xmm7.x = m1.e02
		mulps		xmm2, xmm3								// xmm2.x = m1.e01 * m2.e10
																// xmm2.y = m1.e01 * m2.e11
																// xmm2.z = m1.e01 * m2.e12
																// xmm2.w = m1.e01 * m2.e13
		shufps	xmm7, xmm7, 0							// xmm7 = m1.e02 in all 4 floats
		addps		xmm0, xmm2								// xmm0.x = (m1.e00 * m2.e00) + (m1.e01 * m2.e10)
																// xmm0.y = (m1.e00 * m2.e01) + (m1.e01 * m2.e12)
																// xmm0.z = (m1.e00 * m2.e02) + (m1.e01 * m2.e13)
																// xmm0.w = (m1.e00 * m2.e03) + (m1.e01 * m2.e14)
		movaps	xmm4, xmmword ptr [ecx+20h]		// xmm4 = m2 third row
		movss		xmm2, dword ptr [edx+0Ch]			// xmm2.x = m1.e03
		mulps		xmm7, xmm4								// xmm7.x = m1.e02 * m2.e20
																// xmm7.y = m1.e02 * m2.e21
																// xmm7.z = m1.e02 * m2.e22
																// xmm7.w = m1.e02 * m2.e23
		shufps	xmm2, xmm2, 0							// xmm2 = m1.e03 in all 4 floats
		addps		xmm0, xmm7								// xmm0.x = (m1.e00 * m2.e00) + (m1.e01 * m2.e10) + (m1.e02 * m2.e20)
																// xmm0.y = (m1.e00 * m2.e01) + (m1.e01 * m2.e11) + (m1.e02 * m2.e21)
																// xmm0.z = (m1.e00 * m2.e02) + (m1.e01 * m2.e12) + (m1.e02 * m2.e22)
																// xmm0.w = (m1.e00 * m2.e03) + (m1.e01 * m2.e13) + (m1.e02 * m2.e23)
		movaps	xmm5, xmmword ptr [ecx+30h]		// xmm5 = m2 fourth row
		movss		xmm6, dword ptr [edx+10h]			// xmm6.x = m1.e10
		mulps		xmm2, xmm5								// xmm2.x = m1.e03 * m2.e30
																// xmm2.y = m1.e03 * m2.e31
																// xmm2.z = m1.e03 * m2.e32
																// xmm2.w = m1.e03 * m2.e33
		movss		xmm7, dword ptr [edx+14h]			// xmm7.x = m1.e11
		shufps	xmm6, xmm6, 0							// xmm6 = m1.e10 in all 4 floats
		addps		xmm0, xmm2								// xmm0.x = (m1.e00 * m2.e00) + (m1.e01 * m2.e10) + (m1.e02 * m2.e20) + (m1.e03 * m2.e30)
																// xmm0.y = (m1.e00 * m2.e01) + (m1.e01 * m2.e11) + (m1.e02 * m2.e21) + (m1.e03 * m2.e31)
																// xmm0.z = (m1.e00 * m2.e02) + (m1.e01 * m2.e12) + (m1.e02 * m2.e22) + (m1.e03 * m2.e32)
																// xmm0.w = (m1.e00 * m2.e03) + (m1.e01 * m2.e13) + (m1.e02 * m2.e23) + (m1.e03 * m2.e33)
		shufps	xmm7, xmm7, 0							// xmm7 = m1.e11 in all 4 floats
		movlps	qword ptr [eax], xmm0				// write to this->e00, this->e01
		movhps	qword ptr [eax+8], xmm0				// write to this->e02, this->e03

		// row 2. movss xmm6, shufps xmm6, movss xmm7, shufps xmm7 are part of the row 2 calculation

		mulps		xmm7, xmm3
		movss		xmm0, dword ptr [edx+18h]
		mulps		xmm6, xmm1
		shufps	xmm0, xmm0, 0
		addps		xmm6, xmm7
		mulps		xmm0, xmm4
		movss		xmm2, dword ptr [edx+24h]
		addps		xmm6, xmm0
		movss		xmm0, dword ptr [edx+1Ch]
		movss		xmm7, dword ptr [edx+20h]
		shufps	xmm0, xmm0, 0
		shufps	xmm7, xmm7, 0
		mulps		xmm0, xmm5
		mulps		xmm7, xmm1
		addps		xmm6, xmm0
		shufps	xmm2, xmm2, 0
		movlps	qword ptr [eax+10h], xmm6
		movhps	qword ptr [eax+18h], xmm6

		// row 3. movss xmm2, shufps xmm2, movss xmm7, shufps xmm7 are part of the row 3 calculation

		mulps		xmm2, xmm3
		movss		xmm6, dword ptr [edx+28h]
		addps		xmm7, xmm2
		shufps	xmm6, xmm6, 0
		movss		xmm2, dword ptr [edx+2Ch]
		mulps		xmm6, xmm4
		shufps	xmm2, xmm2, 0
		addps		xmm7, xmm6
		mulps		xmm2, xmm5
		movss		xmm0, dword ptr [edx+34h]
		addps		xmm7, xmm2
		shufps	xmm0, xmm0, 0
		movlps	qword ptr [eax+20h], xmm7
		movss		xmm2, dword ptr [edx+30h]
		movhps	qword ptr [eax+28h], xmm7

		// row 4. movss xmm0, shufps xmm0, movss xmm2 are part of the row 3 calculation

		mulps		xmm0, xmm3
		shufps	xmm2, xmm2, 0
		movss		xmm6, dword ptr [edx+38h]
		mulps		xmm2, xmm1
		shufps	xmm6, xmm6, 0
		addps		xmm2, xmm0
		mulps		xmm6, xmm4
		movss		xmm7, dword ptr [edx+3Ch]
		shufps	xmm7, xmm7, 0
		addps		xmm2, xmm6
		mulps		xmm7, xmm5
		addps		xmm2, xmm7
		movaps	xmmword ptr [eax+30h], xmm2

		; done

		pop	edx
		pop	ecx
		pop	eax
	}

	return *this;
}

#endif //EXCLUDE_GENERIC_GRAPHICS4X4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

