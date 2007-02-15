/***************************************************************************/
// Mat4x4.hpp
// Implementations for the matrix class
/***************************************************************************/

/***************************************************************************/
// PS2 implementation-- some of these routines run faster on vu0
/***************************************************************************/

/***************************************************************************/
// if this file is empty, it means that no routines have been given a
// platform-specific implementation for this platform
/***************************************************************************/

#include "Math/MathStats.h"
#if defined(PS2) && defined(SN)
#include <sn_registers.h>	// SN systems assemblers needs explicit declaration of register names.
#endif

/******************* defines *****************************/

/* mips register correspondence (sn compiler doesn't allow use of names, just numbers):

$0  = zero	read only
$1  = at		reserved for assembler
$2  = v0		return result
$3  = v1		return result
$4  = a0		argument0
$5  = a1		argument1
$6  = a2		argument2
$7  = a3		argument3
$8  = t0		temp, caller saves
$9  = t1		temp, caller saves
$10 = t2		temp, caller saves
$11 = t3		temp, caller saves
$12 = t4		temp, caller saves
$13 = t5		temp, caller saves
$14 = t6		temp, caller saves
$15 = t7		temp, caller saves
$16 = s0		temp, callee saves
$17 = s1		temp, callee saves
$18 = s2		temp, callee saves
$19 = s3		temp, callee saves
$20 = s4		temp, callee saves
$21 = s5		temp, callee saves
$22 = s6		temp, callee saves
$23 = s7		temp, callee saves
$24 = t8		temp, caller saves
$25 = t9		temp, caller saves
$26 = k0		reserved for operating system
$27 = k1		reserved for operating system
$28 = gp		pointer to global area
$29 =	sp		stack pointer
$30 =	fp		frame pointer
$31 =	ra		return address

a description of the mips calling convention can be found at:	www.cs.cornell.edu/cs314-fa99/resources/callconv.html

the output/input/temp section at the bottom is organized as follows:

  first section (between first 2 colons) is output section. put variables here whose contents are changed by the assembly.
  this is to prevent the compiler from re-using the register later, still thinking that it has the original value. the
  syntax is:

		"=r" (<variable name>)
		"=m" (<variable name>)

  using "=m" means that the memory pointed to by the variable is changed. this allows the compiler to avoid data caching issues

  second section is the input section. put variables here that do not change during assembly. the syntax is:

		"r" (<variable name>)
		"m" (<variable name>)

  to reference variables in the output and input section, use %<n>, where <n> is the 0-based index of the variable,
  starting with the first variable in the output section and counting up

  the third section is a list of registers that routine uses.

*/

/******************* functions ***************************/

/***************************************************************************/
// Set to identity
/***************************************************************************/

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

/***************************************************************************/
/***************************************************************************/

inline Matrix4x4 Matrix4x4::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_M4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	Matrix4x4 product;

	/* i've tried to implement this as vu assembly, but we get a compile error in trying to return the local variable */
	/* product. i put in a tech support to metrowerks, but their suggested fix didn't work; i'll try again */

	product.FastMultiply(*this, m);

	return product;
}

/***************************************************************************/
/***************************************************************************/

inline Matrix4x4 &Matrix4x4::operator*=(
const Matrix4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_M4X4);
#endif //MATH_STATS
	
#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT
	
	// define register use
	
#define THIS		%0		// order in output/input sections at bottom of assembly
#define M			%1		// order in output/input sections at bottom of assembly

	Matrix4x4 temp = *this;
	asm __volatile__("

	// load matrix this

	lqc2				vf8,0x0(THIS)
	lqc2				vf9,0x10(THIS)
	lqc2				vf10,0x20(THIS)
	lqc2				vf11,0x30(THIS)

	// load matrix m

	lqc2				vf4,0x0(M)
	lqc2				vf5,0x10(M)
	lqc2				vf6,0x20(M)
	lqc2				vf7,0x30(M)

	// calculate resulting matrix

	vmulax.xyzw		ACC,vf4,vf8
	vmadday.xyzw	ACC,vf5,vf8
	vmaddaz.xyzw	ACC,vf6,vf8
	vmaddw.xyzw		vf12,vf7,vf8

	vmulax.xyzw		ACC,vf4,vf9
	vmadday.xyzw	ACC,vf5,vf9
	vmaddaz.xyzw	ACC,vf6,vf9
	vmaddw.xyzw		vf13,vf7,vf9

	vmulax.xyzw		ACC,vf4,vf10
	vmadday.xyzw	ACC,vf5,vf10
	vmaddaz.xyzw	ACC,vf6,vf10
	vmaddw.xyzw		vf14,vf7,vf10

	vmulax.xyzw		ACC,vf4,vf11
	vmadday.xyzw	ACC,vf5,vf11
	vmaddaz.xyzw	ACC,vf6,vf11
	vmaddw.xyzw		vf15,vf7,vf11

	// save results

	sqc2				vf12,0x0(THIS)
	sqc2				vf13,0x10(THIS)
	sqc2				vf14,0x20(THIS)
	sqc2				vf15,0x30(THIS)

	" : "=m" (temp) : "r" (&m) : );
	*this = temp;
	return *this;
	
	/* clean up */
 
#undef THIS
#undef M
}

inline Matrix4x4 Matrix4x4::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	Matrix4x4 product;

	/* i've tried to implement this as vu assembly, but we get a compile error in trying to return the local variable */
	/* product. i put in a tech support to metrowerks, but their suggested fix didn't work; i'll try again */

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
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	Matrix4x4 product;

	/* i've tried to implement this as vu assembly, but we get a compile error in trying to return the local variable */
	/* product. i put in a tech support to metrowerks, but their suggested fix didn't work; i'll try again */

	product.FastMultiply(*this, m);

	return product;
}

/***************************************************************************/
/***************************************************************************/

inline Matrix4x4 &Matrix4x4::operator*=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	/* define register use */

	Matrix4x4 temp = *this; 

#define THIS		%0		// order in output/input sections at bottom of assembly
#define M			%1		// order in output/input sections at bottom of assembly

	asm __volatile__("

	// load matrix this

	lqc2				vf8,0x0(THIS)
	lqc2				vf9,0x10(THIS)
	lqc2				vf10,0x20(THIS)
	lqc2				vf11,0x30(THIS)

	// load matrix m

	lqc2				vf4,0x0(M)
	lqc2				vf5,0x10(M)
	lqc2				vf6,0x20(M)
	lqc2				vf7,0x30(M)

	// calculate resulting matrix

	vmulax.xyzw		ACC,vf4,vf8
	vmadday.xyzw	ACC,vf5,vf8
	vmaddaz.xyzw	ACC,vf6,vf8
	vmaddw.xyzw		vf12,vf7,vf8

	vmulax.xyzw		ACC,vf4,vf9
	vmadday.xyzw	ACC,vf5,vf9
	vmaddaz.xyzw	ACC,vf6,vf9
	vmaddw.xyzw		vf13,vf7,vf9

	vmulax.xyzw		ACC,vf4,vf10
	vmadday.xyzw	ACC,vf5,vf10
	vmaddaz.xyzw	ACC,vf6,vf10
	vmaddw.xyzw		vf14,vf7,vf10

	vmulax.xyzw		ACC,vf4,vf11
	vmadday.xyzw	ACC,vf5,vf11
	vmaddaz.xyzw	ACC,vf6,vf11
	vmaddw.xyzw		vf15,vf7,vf11

	// save results

	sqc2				vf12,0x0(THIS)
	sqc2				vf13,0x10(THIS)
	sqc2				vf14,0x20(THIS)
	sqc2				vf15,0x30(THIS)

	" : "=m" (temp) : "r" (&m) : );
	*this = temp;
	return *this;

	/* clean up */

#undef THIS
#undef M
}

/***************************************************************************/
/***************************************************************************/

inline Matrix4x4 &Matrix4x4::operator*=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

	/* define register use */

#define THIS		%0		// order in output/input sections at bottom of assembly
#define M			%1		// order in output/input sections at bottom of assembly

	Matrix4x4 temp = *this;
	asm __volatile__("

	// load matrix this

	lqc2				vf8,0x0(THIS)
	lqc2				vf9,0x10(THIS)
	lqc2				vf10,0x20(THIS)
	lqc2				vf11,0x30(THIS)

	// load matrix m

	lqc2				vf4,0x0(M)
	lqc2				vf5,0x10(M)
	lqc2				vf6,0x20(M)
	lqc2				vf7,0x30(M)

	// calculate resulting matrix

	vmulax.xyzw		ACC,vf4,vf8
	vmadday.xyzw	ACC,vf5,vf8
	vmaddaz.xyzw	ACC,vf6,vf8
	vmaddw.xyzw		vf12,vf7,vf8

	vmulax.xyzw		ACC,vf4,vf9
	vmadday.xyzw	ACC,vf5,vf9
	vmaddaz.xyzw	ACC,vf6,vf9
	vmaddw.xyzw		vf13,vf7,vf9

	vmulax.xyzw		ACC,vf4,vf10
	vmadday.xyzw	ACC,vf5,vf10
	vmaddaz.xyzw	ACC,vf6,vf10
	vmaddw.xyzw		vf14,vf7,vf10

	vmulax.xyzw		ACC,vf4,vf11
	vmadday.xyzw	ACC,vf5,vf11
	vmaddaz.xyzw	ACC,vf6,vf11
	vmaddw.xyzw		vf15,vf7,vf11

	// save results

	sqc2				vf12,0x0(THIS)
	sqc2				vf13,0x10(THIS)
	sqc2				vf14,0x20(THIS)
	sqc2				vf15,0x30(THIS)

	" : "=m" (temp) : "r" (&m) : );
	*this = temp;
	return *this;

	/* clean up */

#undef THIS
#undef M
}

/***************************************************************************/
/***************************************************************************/

inline Matrix4x4 &Matrix4x4::FastMultiply(const Matrix4x4 &m1, const Matrix4x4 &m2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(MATRIX4X4_FAST_MULTIPLY);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m1));
	ASSERT(IS_VU_ALIGNED(&m2));
#endif //CHECK_MATH_ALIGNMENT

	/* define register use */

#define THIS		%0		// order in output/input sections at bottom of assembly
#define M1			%1		// order in output/input sections at bottom of assembly
#define M2			%2		// order in output/input sections at bottom of assembly

	Matrix4x4 temp = *this;

	asm __volatile__("

	// load matrix m1

	lqc2				vf8,0x0(M1)
	lqc2				vf9,0x10(M1)
	lqc2				vf10,0x20(M1)
	lqc2				vf11,0x30(M1)

	// load matrix m2

	lqc2				vf4,0x0(M2)
	lqc2				vf5,0x10(M2)
	lqc2				vf6,0x20(M2)
	lqc2				vf7,0x30(M2)

	// calculate resulting matrix

	vmulax.xyzw		ACC,vf4,vf8
	vmadday.xyzw	ACC,vf5,vf8
	vmaddaz.xyzw	ACC,vf6,vf8
	vmaddw.xyzw		vf12,vf7,vf8

	vmulax.xyzw		ACC,vf4,vf9
	vmadday.xyzw	ACC,vf5,vf9
	vmaddaz.xyzw	ACC,vf6,vf9
	vmaddw.xyzw		vf13,vf7,vf9

	vmulax.xyzw		ACC,vf4,vf10
	vmadday.xyzw	ACC,vf5,vf10
	vmaddaz.xyzw	ACC,vf6,vf10
	vmaddw.xyzw		vf14,vf7,vf10

	vmulax.xyzw		ACC,vf4,vf11
	vmadday.xyzw	ACC,vf5,vf11
	vmaddaz.xyzw	ACC,vf6,vf11
	vmaddw.xyzw		vf15,vf7,vf11

	// save results

	sqc2				vf12,0x0(THIS)
	sqc2				vf13,0x10(THIS)
	sqc2				vf14,0x20(THIS)
	sqc2				vf15,0x30(THIS)

	":  "=m" (temp) : "r" (&m1), "r" (&m2) : );
	*this = temp;
	return *this;

	/* clean up */

#undef THIS
#undef M1
#undef M2
}

/***************************************************************************/
/***************************************************************************/

inline Matrix4x4 Graphics4x4::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_M4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Matrix4x4 product;

	/* i've tried to implement this as vu assembly, but we get a compile error in trying to return the local variable */
	/* product. i put in a tech support to metrowerks, but their suggested fix didn't work; i'll try again */

	product.FastMultiply(*this, m);

	return product;
}

/***************************************************************************/
/***************************************************************************/

inline Graphics4x4 Graphics4x4::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
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

	/* i've tried to implement this as vu assembly, but we get a compile error in trying to return the local variable */
	/* product. i put in a tech support to metrowerks, but their suggested fix didn't work; i'll try again */

	product.FastMultiply(*this, m);

	return product;
}

/***************************************************************************/
/***************************************************************************/

inline Graphics4x4 Graphics4x4::operator*(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
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

	/* i've tried to implement this as vu assembly, but we get a compile error in trying to return the local variable */
	/* product. i put in a tech support to metrowerks, but their suggested fix didn't work; i'll try again */

	product.FastMultiply(*this, m);

	return product;
}

/***************************************************************************/
/***************************************************************************/

inline Graphics4x4 &Graphics4x4::operator*=(
const Graphics4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
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

	/* define register use */

#define THIS		%0		// order in output/input sections at bottom of assembly
#define M			%1		// order in output/input sections at bottom of assembly

	Graphics4x4 temp = *this;

	asm __volatile__("

	// load matrix this

	lqc2				vf8,0x0(THIS)
	lqc2				vf9,0x10(THIS)
	lqc2				vf10,0x20(THIS)
	lqc2				vf11,0x30(THIS)

	// load matrix m

	lqc2				vf4,0x0(M)
	lqc2				vf5,0x10(M)
	lqc2				vf6,0x20(M)
	lqc2				vf7,0x30(M)

	// calculate resulting matrix

	vmulax.xyzw		ACC,vf4,vf8
	vmadday.xyzw	ACC,vf5,vf8
	vmaddaz.xyzw	ACC,vf6,vf8
	vmaddw.xyzw		vf12,vf7,vf8

	vmulax.xyzw		ACC,vf4,vf9
	vmadday.xyzw	ACC,vf5,vf9
	vmaddaz.xyzw	ACC,vf6,vf9
	vmaddw.xyzw		vf13,vf7,vf9

	vmulax.xyzw		ACC,vf4,vf10
	vmadday.xyzw	ACC,vf5,vf10
	vmaddaz.xyzw	ACC,vf6,vf10
	vmaddw.xyzw		vf14,vf7,vf10

	vmulax.xyzw		ACC,vf4,vf11
	vmadday.xyzw	ACC,vf5,vf11
	vmaddaz.xyzw	ACC,vf6,vf11
	vmaddw.xyzw		vf15,vf7,vf11

	// save results

	sqc2				vf12,0x0(THIS)
	sqc2				vf13,0x10(THIS)
	sqc2				vf14,0x20(THIS)
	sqc2				vf15,0x30(THIS)

	" : "=m" (temp) : "r" (&m) : );
	*this = temp;
	return *this;

	/* clean up */

#undef THIS
#undef M
}

/***************************************************************************/
/***************************************************************************/

inline Graphics4x4 &Graphics4x4::operator*=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
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

	/* define register use */

#define THIS		%0		// order in output/input sections at bottom of assembly
#define M			%1		// order in output/input sections at bottom of assembly

	Graphics4x4 temp = *this;

	asm __volatile__("

	// load matrix this

	lqc2				vf8,0x0(THIS)
	lqc2				vf9,0x10(THIS)
	lqc2				vf10,0x20(THIS)
	lqc2				vf11,0x30(THIS)

	// load matrix m

	lqc2				vf4,0x0(M)
	lqc2				vf5,0x10(M)
	lqc2				vf6,0x20(M)
	lqc2				vf7,0x30(M)

	// calculate resulting matrix

	vmulax.xyzw		ACC,vf4,vf8
	vmadday.xyzw	ACC,vf5,vf8
	vmaddaz.xyzw	ACC,vf6,vf8
	vmaddw.xyzw		vf12,vf7,vf8

	vmulax.xyzw		ACC,vf4,vf9
	vmadday.xyzw	ACC,vf5,vf9
	vmaddaz.xyzw	ACC,vf6,vf9
	vmaddw.xyzw		vf13,vf7,vf9

	vmulax.xyzw		ACC,vf4,vf10
	vmadday.xyzw	ACC,vf5,vf10
	vmaddaz.xyzw	ACC,vf6,vf10
	vmaddw.xyzw		vf14,vf7,vf10

	vmulax.xyzw		ACC,vf4,vf11
	vmadday.xyzw	ACC,vf5,vf11
	vmaddaz.xyzw	ACC,vf6,vf11
	vmaddw.xyzw		vf15,vf7,vf11

	// save results

	sqc2				vf12,0x0(THIS)
	sqc2				vf13,0x10(THIS)
	sqc2				vf14,0x20(THIS)
	sqc2				vf15,0x30(THIS)

	" : "=m" (temp) : "r" (&m) : );
	*this = temp;
	return *this;

	/* clean up */

#undef THIS
#undef M
}

/***************************************************************************/
/***************************************************************************/

inline Matrix4x4 Cartesian4x4::operator*(
const Matrix4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_M4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
#endif //CHECK_MATH_ALIGNMENT

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Matrix4x4 product;

	/* i've tried to implement this as vu assembly, but we get a compile error in trying to return the local variable */
	/* product. i put in a tech support to metrowerks, but their suggested fix didn't work; i'll try again */

	product.FastMultiply(*this, m);

	return product;
}

/***************************************************************************/
/***************************************************************************/

inline Graphics4x4 Cartesian4x4::operator*(
const Graphics4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
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

	/* i've tried to implement this as vu assembly, but we get a compile error in trying to return the local variable */
	/* product. i put in a tech support to metrowerks, but their suggested fix didn't work; i'll try again */

	product.FastMultiply(*this, m);

	return product;
}

/***************************************************************************/
/***************************************************************************/

inline Cartesian4x4 Cartesian4x4::operator*(
const Cartesian4x4 &m) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
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

	/* i've tried to implement this as vu assembly, but we get a compile error in trying to return the local variable */
	/* product. i put in a tech support to metrowerks, but their suggested fix didn't work; i'll try again */

	product.FastMultiply(*this, m);

	return product;
}

/***************************************************************************/
/***************************************************************************/

inline Cartesian4x4 &Cartesian4x4::operator*=(
const Cartesian4x4 &m)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TIMES_EQUALS_G4X4);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m));
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

	/* define register use */

#define THIS		%0		// order in output/input sections at bottom of assembly
#define M			%1		// order in output/input sections at bottom of assembly

	Cartesian4x4 temp = *this;
	asm __volatile__("

	// load matrix this

	lqc2				vf8,0x0(THIS)
	lqc2				vf9,0x10(THIS)
	lqc2				vf10,0x20(THIS)
	lqc2				vf11,0x30(THIS)

	// load matrix m

	lqc2				vf4,0x0(M)
	lqc2				vf5,0x10(M)
	lqc2				vf6,0x20(M)
	lqc2				vf7,0x30(M)

	// calculate resulting matrix

	vmulax.xyzw		ACC,vf4,vf8
	vmadday.xyzw	ACC,vf5,vf8
	vmaddaz.xyzw	ACC,vf6,vf8
	vmaddw.xyzw		vf12,vf7,vf8

	vmulax.xyzw		ACC,vf4,vf9
	vmadday.xyzw	ACC,vf5,vf9
	vmaddaz.xyzw	ACC,vf6,vf9
	vmaddw.xyzw		vf13,vf7,vf9

	vmulax.xyzw		ACC,vf4,vf10
	vmadday.xyzw	ACC,vf5,vf10
	vmaddaz.xyzw	ACC,vf6,vf10
	vmaddw.xyzw		vf14,vf7,vf10

	vmulax.xyzw		ACC,vf4,vf11
	vmadday.xyzw	ACC,vf5,vf11
	vmaddaz.xyzw	ACC,vf6,vf11
	vmaddw.xyzw		vf15,vf7,vf11

	// save results

	sqc2				vf12,0x0(THIS)
	sqc2				vf13,0x10(THIS)
	sqc2				vf14,0x20(THIS)
	sqc2				vf15,0x30(THIS)

	" : "=m" (temp) : "r" (&m) : );
	*this = temp;
	return *this;

	/* clean up */

#undef THIS
#undef M
}

/***************************************************************************/
/***************************************************************************/

inline Graphics4x4 &Graphics4x4::FastMultiply(const Graphics4x4 &m1, const Graphics4x4 &m2)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_FAST_MULTIPLY);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(&m1));
	ASSERT(IS_VU_ALIGNED(&m2));
#endif //CHECK_MATH_ALIGNMENT

	/* define register use */

#define THIS		%0		// order in output/input sections at bottom of assembly
#define M1			%1		// order in output/input sections at bottom of assembly
#define M2			%2		// order in output/input sections at bottom of assembly

	Graphics4x4 temp = *this;
	asm __volatile__("

	// load matrix m1

	lqc2				vf8,0x0(M1)
	lqc2				vf9,0x10(M1)
	lqc2				vf10,0x20(M1)
	lqc2				vf11,0x30(M1)

	// load matrix m2

	lqc2				vf4,0x0(M2)
	lqc2				vf5,0x10(M2)
	lqc2				vf6,0x20(M2)
	lqc2				vf7,0x30(M2)

	// calculate resulting matrix

	vmulax.xyzw		ACC,vf4,vf8
	vmadday.xyzw	ACC,vf5,vf8
	vmaddaz.xyzw	ACC,vf6,vf8
	vmaddw.xyzw		vf12,vf7,vf8

	vmulax.xyzw		ACC,vf4,vf9
	vmadday.xyzw	ACC,vf5,vf9
	vmaddaz.xyzw	ACC,vf6,vf9
	vmaddw.xyzw		vf13,vf7,vf9

	vmulax.xyzw		ACC,vf4,vf10
	vmadday.xyzw	ACC,vf5,vf10
	vmaddaz.xyzw	ACC,vf6,vf10
	vmaddw.xyzw		vf14,vf7,vf10

	vmulax.xyzw		ACC,vf4,vf11
	vmadday.xyzw	ACC,vf5,vf11
	vmaddaz.xyzw	ACC,vf6,vf11
	vmaddw.xyzw		vf15,vf7,vf11

	// save results

	sqc2				vf12,0x0(THIS)
	sqc2				vf13,0x10(THIS)
	sqc2				vf14,0x20(THIS)
	sqc2				vf15,0x30(THIS)

	":  "=m" (temp) : "r" (&m1), "r" (&m2) : );
	*this = temp;
	return *this;

	/* clean up */

#undef THIS
#undef M1
#undef M2
}

