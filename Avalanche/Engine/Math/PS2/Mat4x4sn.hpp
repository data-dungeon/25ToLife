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
#if defined(PS2) && defined(SN) && defined(JJS_NOTUSED)
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

  TJC - Ideally, you will have an empty third section.  Using registers that you dont allow the compiler to choose casues
  inefficient code.  For example, lets say that parameters to the function come in through a0+ (which they do), and you
  decide to clobber a0 in your routine, that makes the compiler save them off and restore them around you assembly even though
  there is probably some unused registers floating around.  It is better to declare a local variable of the correct size and
  pass that in as an output/input where appropriate.  This allows the compiler to choose the registers that will be used.  This
  concept is especially important when dealing with inlined functions as you dont really know the structure of the code
  where the routine gets inlined.

  TJC - You should try to declare your asm statements in as small of blocks as you can as the compiler can shuffle code around
  your blocks to fill in delays.  while the compiler doesnt know much about your code, it does know inputs and outputs and it
  (at least GCC) tries to keep initialization as far away from usage as possible, thus reducing delays over totally unoptimized
  code.  Also, it tends to make the asm statements more readable.  However, there are some registers that the compiler knows
  nothing or little to nothing about.  For example, the ACC.  Therefore, you must specify accumulator use as one block to keep
  the compiler from intermixing accumulator uses.  Hopefully intrinsics will resolve a lot of these issues.

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

	u_long128 temp1;
	u_long128 temp2;
	u_long128 temp3;

	asm("vmr32 %0, vf0" : "=j" (temp1));
	asm("vmr32 %0, %1" : "=j" (temp2) : "j" (temp1));
	asm("vmr32 %0, %1" : "=j" (temp3) : "j" (temp2));

	asm("sqc2 vf0, %0" : "=m" (row3));
	asm("sqc2 %1, %0" : "=m" (row2) : "j" (temp1));
	asm("sqc2 %1, %0" : "=m" (row1) : "j" (temp2));
	asm("sqc2 %1, %0" : "=m" (row0) : "j" (temp3));
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

	u128 m11, m12, m13, m14;
	u128 mr1, mr2, mr3, mr4;

	// load matrix1 (this just convinces the compiler to no
	// interleave the loads in the multiplies...)
	m11 = m1.row0;
	m12 = m1.row1;
	m13 = m1.row2;
	m14 = m1.row3;

	// concatinate (split into 4 steps to give room for the optimizer)
	asm __volatile__ (
		  "vmulax.xyzw		ACC, %2, %1\r\n"
		  "vmadday.xyzw	ACC, %3, %1\r\n"
		  "vmaddaz.xyzw	ACC, %4, %1\r\n"
		  "vmaddw.xyzw		%0,  %5, %1\r\n"
							: "=j" (mr1)
							: "j" (m11), "j" (m2.row0), "j" (m2.row1), "j" (m2.row2), "j" (m2.row3));

	asm __volatile__ (
		  "vmulax.xyzw		ACC, %2, %1\r\n"
		  "vmadday.xyzw	ACC, %3, %1\r\n"
		  "vmaddaz.xyzw	ACC, %4, %1\r\n"
		  "vmaddw.xyzw		%0,  %5, %1\r\n"
							: "=j" (mr2)
							: "j" (m12), "j" (m2.row0), "j" (m2.row1), "j" (m2.row2), "j" (m2.row3));

	asm __volatile__ (
		  "vmulax.xyzw		ACC, %2, %1\r\n"
		  "vmadday.xyzw	ACC, %3, %1\r\n"
		  "vmaddaz.xyzw	ACC, %4, %1\r\n"
		  "vmaddw.xyzw		%0,  %5, %1\r\n"
							: "=j" (mr3)
							: "j" (m13), "j" (m2.row0), "j" (m2.row1), "j" (m2.row2), "j" (m2.row3));

	asm __volatile__ (
		  "vmulax.xyzw		ACC, %2, %1\r\n"
		  "vmadday.xyzw	ACC, %3, %1\r\n"
		  "vmaddaz.xyzw	ACC, %4, %1\r\n"
		  "vmaddw.xyzw		%0,  %5, %1\r\n"
							: "=j" (mr4)
							: "j" (m14), "j" (m2.row0), "j" (m2.row1), "j" (m2.row2), "j" (m2.row3));

	// store results...
	// convince compiler that if this is aliased with m2 we dont have
	// issues.  ie, after each multiply step we dont need to reload
	// m2. :)
	row0 = mr1;
	row1 = mr2;
	row2 = mr3;
	row3 = mr4;

	return *this;
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

	Matrix4x4::FastMultiply((const Matrix4x4&)m1, (const Matrix4x4&)m2);
	return *this;
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

	return FastMultiply(*this, m);
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

	FastMultiply(*this, m);
	return *this;
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

	FastMultiply(*this, m);
	return *this;
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

	return FastMultiply(*this, m);
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

	return FastMultiply(*this, m);
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

	FastMultiply(*this, m);
	return *this;
}

