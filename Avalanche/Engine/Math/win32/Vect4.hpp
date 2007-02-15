/***************************************************************************/
// Implementations for the Vector4 class
/***************************************************************************/

/***************************************************************************/
// WIN32 implementation
/***************************************************************************/

/***************************************************************************/
// if this file is empty, it means that no routines have been given a
// platform-specific implementation for this platform
/***************************************************************************/

#ifdef EXCLUDE_GENERIC_VECTOR4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

inline Vector4 Vector4::FastMultiply(const Vector4 &v, const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_FAST_MULTIPLY);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&v));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	__asm
	{
		push	eax
		push	ecx
		push	edx

		; load up variables

		mov	eax,this	; dest
		mov	edx,v		; vector
		mov	ecx,m		; matrix

		// instructions used (source: instruction set from ftp://download.intel.com/design/Pentium4/manuals/24547104.pdf):
		//
		//		movss		page 525		move scalar single-precision float
		//		movaps	page 483		move aligned packed single-precision float (quad word)
		//		shufps	page 755		shuffle packed single-precision float
		//		mulps		page 540		multiply packed single-precision float
		//		addps		page 65		add packed single-precision float
		//		movlps	page 500		move low packed single-precision float
		//		movhps	page 495		move high packed single-precision float

		movss		xmm0, dword ptr [edx]				// xmm0.x = v.x
		movaps	xmm1, xmmword ptr [ecx]				// xmm1 = m first row
		shufps	xmm0, xmm0, 0							// xmm0 = v.x in all 4 floats
		movss		xmm2, dword ptr [edx+4]				// xmm2.x = v.y
		mulps		xmm0, xmm1								// xmm0.x = v.x * m.r00
																// xmm0.y = v.x * m.r01
																// xmm0.z = v.x * m.r02
																// xmm0.w = v.x * m.u0 (= 0)
		shufps	xmm2, xmm2, 0							// xmm2 = v.y in all 4 floats
		movaps	xmm3, xmmword ptr [ecx+10h]		// xmm3 = m second row
		movss		xmm7, dword ptr [edx+8]				// xmm7.x = v.z
		mulps		xmm2, xmm3								// xmm2.x = v.y * m.r10
																// xmm2.y = v.y * m.r11
																// xmm2.z = v.y * m.r12
																// xmm2.w = v.y * m.u1 (= 0)
		shufps	xmm7, xmm7, 0							// xmm7 = v.z in all 4 floats
		addps		xmm0, xmm2								// xmm0.x = (v.x * m.r00) + (v.y * m.r10)
																// xmm0.y = (v.x * m.r01) + (v.y * m.r12)
																// xmm0.z = (v.x * m.r02) + (v.y * m.r13)
																// xmm0.w = (v.x * m.u0) + (v.y * m.u1) (now = 0)
		movaps	xmm4, xmmword ptr [ecx+20h]		// xmm4 = m third row
		mulps		xmm7, xmm4								// xmm7.x = v.z * m.r20
																// xmm7.y = v.z * m.r21
																// xmm7.z = v.z * m.r22
																// xmm7.w = v.z * m.u2 (= 0)
		addps		xmm0, xmm7								// xmm0.x = (v.x * m.r00) + (v.y * m.r10) + (v.z * m.r20)
																// xmm0.y = (v.x * m.r01) + (v.y * m.r11) + (v.z * m.r21)
																// xmm0.z = (v.x * m.r02) + (v.y * m.r12) + (v.z * m.r22)
																// xmm0.w = (v.x * m.u0) + (v.y * m.u1) + (v.z * m.u2) (now = 0)
		movaps	xmm5, xmmword ptr [ecx+30h]		// xmm5 = m fourth row
		addps		xmm0, xmm5								// xmm0.x = (v.x * m.r00) + (v.y * m.r10) + (v.z * m.r20) + (m.tx)
																// xmm0.y = (v.x * m.r01) + (v.y * m.r11) + (v.z * m.r21) + (m.ty)
																// xmm0.z = (v.x * m.r02) + (v.y * m.r12) + (v.z * m.r22) + (m.tz)
																// xmm0.w = (v.x * m.u0) + (v.y * m.u1) + (v.z * m.u2) + (m.w) (now = 1)
		movaps	qword ptr [eax], xmm0				// write to this

		; done

		pop	edx
		pop	ecx
		pop	eax
	}

	return *this;
}

#endif //EXCLUDE_GENERIC_VECTOR4_FAST_MULTIPLY	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/

#ifdef EXCLUDE_GENERIC_VECTOR4_FAST_TRANSFORM	// compile control from matrix.h for platform-specific implementation

inline Vector4 Vector4::FastTransform(const Vector4 &v, const Matrix4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_FAST_TRANSFORM);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&v));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	__asm
	{
		push	eax
		push	ecx
		push	edx

		; load up variables

		mov	eax,this	; dest
		mov	edx,v		; vector
		mov	ecx,m		; matrix

		// instructions used (source: instruction set from ftp://download.intel.com/design/Pentium4/manuals/24547104.pdf):
		//
		//		movss		page 525		move scalar single-precision float
		//		movaps	page 483		move aligned packed single-precision float (quad word)
		//		shufps	page 755		shuffle packed single-precision float
		//		mulps		page 540		multiply packed single-precision float
		//		addps		page 65		add packed single-precision float
		//		movlps	page 500		move low packed single-precision float
		//		movhps	page 495		move high packed single-precision float

		movss		xmm0, dword ptr [edx]				// xmm0.x = v.x
		movaps	xmm1, xmmword ptr [ecx]				// xmm1 = m first row
		shufps	xmm0, xmm0, 0							// xmm0 = v.x in all 4 floats
		movss		xmm2, dword ptr [edx+4]				// xmm2.x = v.y
		mulps		xmm0, xmm1								// xmm0.x = v.x * m.e00
																// xmm0.y = v.x * m.e01
																// xmm0.z = v.x * m.e02
																// xmm0.w = v.x * m.e03
		shufps	xmm2, xmm2, 0							// xmm2 = v.y in all 4 floats
		movaps	xmm3, xmmword ptr [ecx+10h]		// xmm3 = m second row
		movss		xmm7, dword ptr [edx+8]				// xmm7.x = v.z
		mulps		xmm2, xmm3								// xmm2.x = v.y * m.e10
																// xmm2.y = v.y * m.e11
																// xmm2.z = v.y * m.e12
																// xmm2.w = v.y * m.e13
		shufps	xmm7, xmm7, 0							// xmm7 = v.z in all 4 floats
		addps		xmm0, xmm2								// xmm0.x = (v.x * m.e00) + (v.y * m.e10)
																// xmm0.y = (v.x * m.e01) + (v.y * m.e12)
																// xmm0.z = (v.x * m.e02) + (v.y * m.e13)
																// xmm0.w = (v.x * m.e03) + (v.y * m.e14)
		movaps	xmm4, xmmword ptr [ecx+20h]		// xmm4 = m third row
		movss		xmm2, dword ptr [edx+0Ch]			// xmm2.x = v.w
		mulps		xmm7, xmm4								// xmm7.x = v.z * m.e20
																// xmm7.y = v.z * m.e21
																// xmm7.z = v.z * m.e22
																// xmm7.w = v.z * m.e23
		shufps	xmm2, xmm2, 0							// xmm2 = v.w in all 4 floats
		addps		xmm0, xmm7								// xmm0.x = (v.x * m.e00) + (v.y * m.e10) + (v.z * m.e20)
																// xmm0.y = (v.x * m.e01) + (v.y * m.e11) + (v.z * m.e21)
																// xmm0.z = (v.x * m.e02) + (v.y * m.e12) + (v.z * m.e22)
																// xmm0.w = (v.x * m.e03) + (v.y * m.e13) + (v.z * m.e23)
		movaps	xmm5, xmmword ptr [ecx+30h]		// xmm5 = m fourth row
		mulps		xmm2, xmm5								// xmm2.x = v.w * m.e30
																// xmm2.y = v.w * m.e31
																// xmm2.z = v.w * m.e32
																// xmm2.w = v.w * m.e33
		addps		xmm0, xmm2								// xmm0.x = (v.x * m.e00) + (v.y * m.e10) + (v.z * m.e20) + (v.w * m.e30)
																// xmm0.y = (v.x * m.e01) + (v.y * m.e11) + (v.z * m.e21) + (v.w * m.e31)
																// xmm0.z = (v.x * m.e02) + (v.y * m.e12) + (v.z * m.e22) + (v.w * m.e32)
																// xmm0.w = (v.x * m.e03) + (v.y * m.e13) + (v.z * m.e23) + (v.w * m.e33)
		movaps	qword ptr [eax], xmm0				// write to this

		; done

		pop	edx
		pop	ecx
		pop	eax
	}

	return *this;
}

#endif //EXCLUDE_GENERIC_VECTOR4_FAST_TRANSFORM	// compile control from matrix.h for platform-specific implementation

/***************************************************************************/

#ifdef EXCLUDE_GENERIC_VECTOR4_FAST_BASIS_TRANSFORM	// compile control from matrix.h for platform-specific implementation

inline Vector4 Vector4::FastBasisTransform(const Vector4 &v, const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(VECTOR4_FAST_BASIS_TRANSFORM);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_SIMD_ALIGNED(this));
	ASSERT(IS_SIMD_ALIGNED(&v));
	ASSERT(IS_SIMD_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	__asm
	{
		push	eax
		push	ecx
		push	edx

		; load up variables

		mov	eax,this	; dest
		mov	edx,v		; vector
		mov	ecx,m		; matrix

		// instructions used (source: instruction set from ftp://download.intel.com/design/Pentium4/manuals/24547104.pdf):
		//
		//		movss		page 525		move scalar single-precision float
		//		movaps	page 483		move aligned packed single-precision float (quad word)
		//		shufps	page 755		shuffle packed single-precision float
		//		mulps		page 540		multiply packed single-precision float
		//		addps		page 65		add packed single-precision float
		//		movlps	page 500		move low packed single-precision float
		//		movhps	page 495		move high packed single-precision float

		movss		xmm0, dword ptr [edx]				// xmm0.x = v.x
		movaps	xmm1, xmmword ptr [ecx]				// xmm1 = m first row
		shufps	xmm0, xmm0, 0							// xmm0 = v.x in all 4 floats
		movss		xmm2, dword ptr [edx+4]				// xmm2.x = v.y
		mulps		xmm0, xmm1								// xmm0.x = v.x * m.r00
																// xmm0.y = v.x * m.r01
																// xmm0.z = v.x * m.r02
																// xmm0.w = v.x * m.u0 (= 0)
		shufps	xmm2, xmm2, 0							// xmm2 = v.y in all 4 floats
		movaps	xmm3, xmmword ptr [ecx+10h]		// xmm3 = m second row
		movss		xmm7, dword ptr [edx+8]				// xmm7.x = v.z
		mulps		xmm2, xmm3								// xmm2.x = v.y * m.r10
																// xmm2.y = v.y * m.r11
																// xmm2.z = v.y * m.r12
																// xmm2.w = v.y * m.u1 (= 0)
		shufps	xmm7, xmm7, 0							// xmm7 = v.z in all 4 floats
		addps		xmm0, xmm2								// xmm0.x = (v.x * m.r00) + (v.y * m.r10)
																// xmm0.y = (v.x * m.r01) + (v.y * m.r12)
																// xmm0.z = (v.x * m.r02) + (v.y * m.r13)
																// xmm0.w = (v.x * m.u0) + (v.y * m.u1) (now = 0)
		movaps	xmm4, xmmword ptr [ecx+20h]		// xmm4 = m third row
		mulps		xmm7, xmm4								// xmm7.x = v.z * m.r20
																// xmm7.y = v.z * m.r21
																// xmm7.z = v.z * m.r22
																// xmm7.w = v.z * m.u2 (= 0)
		addps		xmm0, xmm7								// xmm0.x = (v.x * m.r00) + (v.y * m.r10) + (v.z * m.r20)
																// xmm0.y = (v.x * m.r01) + (v.y * m.r11) + (v.z * m.r21)
																// xmm0.z = (v.x * m.r02) + (v.y * m.r12) + (v.z * m.r22)
																// xmm0.w = (v.x * m.u0) + (v.y * m.u1) + (v.z * m.u2) (now = 0)
		movaps	qword ptr [eax], xmm0				// write to this

		; done

		pop	edx
		pop	ecx
		pop	eax
	}

	this->w = 1.0f;
	return *this;
}

#endif //EXCLUDE_GENERIC_VECTOR4_FAST_BASIS_TRANSFORM	// compile control from matrix.h for platform-specific implementation
