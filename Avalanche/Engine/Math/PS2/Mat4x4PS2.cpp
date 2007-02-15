/***************************************************************************/
// Mat4x4.cpp
// Implementations for the matrix class
/***************************************************************************/

/***************************************************************************/
// PS2 implementation-- some of these routines run faster on vu0
/***************************************************************************/

/***************************************************************************/
// if this file is empty, it means that no routines have been given a
// platform-specific implementation for this platform
/***************************************************************************/

/******************* includes ****************************/

#include "Math/MathPCH.h"





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
void Graphics4x4::TransformArray(
const Vector3 *vIn,
Vector3 *vOut,
int nCount) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TRANSFORM_ARRAY_V3_V3_I);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
#endif //CHECK_MATH_ALIGNMENT

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

#if defined(PS2) && defined(SN)
	u128 vin = vIn->vec();
	u128 vout, mtx1, mtx2, mtx3, mtx4;
	u128 vouty, voutz;
	Vector3* vend = vOut + nCount;

	// load matrix
	mtx1 = row0;
	mtx2 = row1;
	mtx3 = row2;
	mtx4 = row3;

	do
	{
		register u128 acc asm ("vf0");
		asm("vmulax.xyzw	ACC, %1, %2" : "=j" (acc) : "j" (mtx1), "j" (vin));
		asm("vmadday.xyzw ACC, %1, %2" : "+j" (acc) : "j" (mtx2), "j" (vin));
		asm("vmaddaz.xyzw ACC, %1, %2" : "+j" (acc) : "j" (mtx3), "j" (vin));
		asm("vmaddw.xyzw %0, %1, vf00" : "=j" (vout) : "j" (mtx4), "j" (acc));

		// move to next vtx...
		vIn++;

		// store prior vtx
		vOut->vec(vout);

		vin = vIn->vec();

	} while(++vOut != vend);

#else

	/* define register use */

#define VOUT		%0		// order in output/input sections at bottom of assembly
#define VIN			%1		// order in output/input sections at bottom of assembly
#define NCOUNT		%2		// order in output/input sections at bottom of assembly
#define THIS		%3		// order in output/input sections at bottom of assembly

	asm __volatile__("

	lw					t0,0(VIN)			/* load first input vector xyz components, don't count on any particular alignment (other than 32-bit) */
	lw					t1,4(VIN)
	lw					t2,8(VIN)

	lqc2				vf04,0x0(THIS)		/* load matrix */
	lqc2				vf05,0x10(THIS)
	lqc2				vf06,0x20(THIS)
	lqc2				vf07,0x30(THIS)

_loopTransformArrayVector3:

	qmtc2				t0,vf08				/* vf08.x = input.x */
	qmtc2				t1,vf09				/* vf09.x = input.y */
	qmtc2				t2,vf10				/* vf10.x = input.z */
	vmulax.xyz		ACC,vf04,vf08		/* input vector x-component */
	vmaddax.xyz		ACC,vf05,vf09		/* input vector y-component */
	vmaddax.xyz		ACC,vf06,vf10		/* input vector z-component */
	vmaddw.xyz		vf11,vf07,vf00		/* vf00.w = 1.0f-- add in translation */

	/* load next input vector */
	addi				VIN,0x0c				/* to next input vector-- 12 bytes each */
	lw					t0,0(VIN)			/* load input vector xyz components, don't count on any particular alignment (other than 32-bit) */
	lw					t1,4(VIN)
	lw					t2,8(VIN)

	/* save last result */
	vaddy.x			vf12,vf00,vf11		/* vf12.x = vf11.y */
	vaddz.x			vf13,vf00,vf11		/* vf13.x = vf11.z */
	addi				NCOUNT,-1			/* decrement counter */
	qmfc2				t3,vf11				/* vf11.x (x-component result) into mips register */
	qmfc2				t4,vf12				/* vf12.x (y-component result) into mips register */
	qmfc2				t5,vf13				/* vf13.x (z-component result) into mips register */
	sw					t3,0(VOUT)			/* save x-component result */
	sw					t4,4(VOUT)			/* save y-component result */
	sw					t5,8(VOUT)			/* save z-component result */
	addi				VOUT,0x0c			/* to next output vector-- 12 bytes each */
	bne				zero,NCOUNT,_loopTransformArrayVector3
	" : "=r" (vOut), "=r" (vIn), "=r" (nCount) : "r" (this) : "t0", "t1", "t2", "t3", "t4", "t5");

	/* clean up */

#undef VOUT
#undef VIN
#undef NCOUNT
#undef THIS

#endif
}

/***************************************************************************/
/***************************************************************************/

void Graphics4x4::TransformArray(
const Vector4 *vIn,
Vector4 *vOut,
int nCount) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_TRANSFORM_ARRAY_V4_V4_I);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(vIn));
	ASSERT(IS_VU_ALIGNED(vOut));
#endif //CHECK_MATH_ALIGNMENT

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

#if defined(PS2) && defined(SN)
	u128 vin, vout, mtx1, mtx2, mtx3, mtx4;
	Vector4* vend = vOut + nCount;

	// laod first vertex
	asm ("lqc2	 %0, %1"	: "=j" (vin) : "m" (*vIn));

	// load matrix
	asm ("lqc2	 %0, 0x00(%1)" : "=j" (mtx1) : "r" (this), "m" (*this));
	asm ("lqc2	 %0, 0x10(%1)" : "=j" (mtx2) : "r" (this), "m" (*this));
	asm ("lqc2	 %0, 0x20(%1)" : "=j" (mtx3) : "r" (this), "m" (*this));
	asm ("lqc2	 %0, 0x30(%1)" : "=j" (mtx4) : "r" (this), "m" (*this));

	do
	{
		asm __volatile__ ("
		  vmulax.xyzw		ACC, %2, %1
		  vmadday.xyzw		ACC, %3, %1
		  vmaddaz.xyzw		ACC, %4, %1
		  vmaddw.xyzw		%0, %5, %1" : "=j" (vout) : "j" (vin), "j" (mtx1), "j" (mtx2), "j" (mtx3), "j" (mtx4));

		// move to next vtx...
		vIn++;

		// store prior vtx
		asm ("sqc2			%1, %0" : "=m" (*vOut) : "j" (vout));

		// load next vtx
		asm ("lqc2			%0, %1"	: "=j" (vin) : "m" (*vIn));
	} while(++vOut != vend);

#else

	/* define register use */

#define VOUT		%0		// order in output/input sections at bottom of assembly
#define VIN			%1		// order in output/input sections at bottom of assembly
#define NCOUNT		%2		// order in output/input sections at bottom of assembly
#define THIS		%3		// order in output/input sections at bottom of assembly

	asm __volatile__("
	lqc2				vf08,0x0(VIN)		/* load first input vector */
	lqc2				vf04,0x0(THIS)		/* load matrix */
	lqc2				vf05,0x10(THIS)
	lqc2				vf06,0x20(THIS)
	lqc2				vf07,0x30(THIS)

_loopTransformArrayVector4:

	vmulax.xyzw		ACC,vf04,vf08
	vmadday.xyzw	ACC,vf05,vf08
	vmaddaz.xyzw	ACC,vf06,vf08
	vmaddw.xyzw		vf09,vf07,vf08
	addi				VIN,0x10				/* to next input vector */
	lqc2				vf08,0x0(VIN)		/* load input vector */
	addi				NCOUNT,-1			/* decrement counter */
	sqc2				vf09,0x0(VOUT)		/* save output vector */
	addi				VOUT,0x10			/* to next output vector */
	bne				zero,NCOUNT,_loopTransformArrayVector4
	" : "=r" (vOut), "=r" (vIn), "=r" (nCount) : "r" (this) : );

	/* clean up */

#undef VOUT
#undef VIN
#undef NCOUNT
#undef THIS

#endif

}

/***************************************************************************/
/***************************************************************************/

void Graphics4x4::BasisTransformArray(
const Vector3 *vIn,
Vector3 *vOut,
int nCount) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_BASIS_TRANSFORM_ARRAY_V3_V3_I);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(vIn));
	ASSERT(IS_VU_ALIGNED(vOut));
#endif //CHECK_MATH_ALIGNMENT

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

#if defined(PS2) && defined(SN)
	u128 vinx, viny, vinz, vout, mtx1, mtx2, mtx3;
	u128 vouty, voutz;
	Vector3* vend = vOut + nCount;

	// load first vertex
	u128 vin = vIn->vec();

	// load matrix
	mtx1 = row0;
	mtx2 = row1;
	mtx3 = row2;

	do
	{
		register u128 acc asm ("vf0");

		asm("vmulax.xyzw	ACC, %1, %2" : "=j" (acc) : "j" (mtx1), "j" (vin));
		asm("vmadday.xyzw	ACC, %1, %2" : "+j" (acc) : "j" (mtx2), "j" (vin));
		asm("vmaddz.xyzw	%0, %1, %2" : "=j" (vout) : "j" (mtx3), "j" (vin), "j" (acc));

		// move to next vtx...
		vIn++;

		// store prior vtx
		vOut->vec(vout);

		vin = vIn->vec();
	} while(++vOut != vend);

#else

	/* define register use */

#define VOUT		%0		// order in output/input sections at bottom of assembly
#define VIN			%1		// order in output/input sections at bottom of assembly
#define NCOUNT		%2		// order in output/input sections at bottom of assembly
#define THIS		%3		// order in output/input sections at bottom of assembly

	asm __volatile__("
	lw					t0,0(VIN)			/* load first input vector xyz components, don't count on any particular alignment (other than 32-bit) */
	lw					t1,4(VIN)
	lw					t2,8(VIN)

	lqc2				vf04,0x0(THIS)		/* load matrix */
	lqc2				vf05,0x10(THIS)
	lqc2				vf06,0x20(THIS)
	lqc2				vf07,0x30(THIS)

_loopMultVct3fArrayNoTranslateFast:

	qmtc2				t0,vf08				/* vf08.x = input.x */
	qmtc2				t1,vf09				/* vf09.x = input.y */
	qmtc2				t2,vf10				/* vf10.x = input.z */
	vmulax.xyz		ACC,vf04,vf08		/* input vector x-component */
	vmaddax.xyz		ACC,vf05,vf09		/* input vector y-component */
	vmaddx.xyz		vf11,vf06,vf10		/* input vector z-component */

	/* load next input vector */
	addi				VIN,0x0c				/* to next input vector-- 12 bytes each */
	lw					t0,0(VIN)			/* load input vector xyz components, don't count on any particular alignment (other than 32-bit) */
	lw					t1,4(VIN)
	lw					t2,8(VIN)

	/* save last result */
	vaddy.x			vf12,vf00,vf11		/* vf12.x = vf11.y */
	vaddz.x			vf13,vf00,vf11		/* vf13.x = vf11.z */
	addi				NCOUNT,-1			/* decrement counter */
	qmfc2				t3,vf11				/* vf11.x (x-component result) into mips register */
	qmfc2				t4,vf12				/* vf12.x (y-component result) into mips register */
	qmfc2				t5,vf13				/* vf13.x (z-component result) into mips register */
	sw					t3,0(VOUT)			/* save x-component result */
	sw					t4,4(VOUT)			/* save y-component result */
	sw					t5,8(VOUT)			/* save z-component result */
	addi				VOUT,0x0c			/* to next output vector-- 12 bytes each */
	bne				zero,NCOUNT,_loopMultVct3fArrayNoTranslateFast
	" : "=r" (vOut), "=r" (vIn), "=r" (nCount) : "r" (this) : "t0", "t1", "t2", "t3", "t4", "t5");

	/* clean up */

#undef VOUT
#undef VIN
#undef NCOUNT
#undef THIS

#endif
}

/***************************************************************************/
/***************************************************************************/

void Graphics4x4::BasisTransformArray(
const Vector4 *vIn,
Vector4 *vOut,
int nCount) const
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(GRAPHICS4X4_BASIS_TRANSFORM_ARRAY_V4_V4_I);
#endif //MATH_STATS

#ifdef CHECK_MATH_ALIGNMENT	// defined in Math\Vector.h
	ASSERT(IS_VU_ALIGNED(this));
	ASSERT(IS_VU_ALIGNED(vIn));
	ASSERT(IS_VU_ALIGNED(vOut));
#endif //CHECK_MATH_ALIGNMENT

#ifdef CHECK_GRAPHICS4X4	// defined in Math\Matrix.h
	ASSERT(u0 == 0.0f);
	ASSERT(u1 == 0.0f);
	ASSERT(u2 == 0.0f);
	ASSERT(w == 1.0f);
#endif //CHECK_GRAPHICS4X4

#if defined(PS2) && defined(SN)
	u128 vin, vout, mtx1, mtx2, mtx3, mtx4;
	Vector4* vend = vOut + nCount;

	// laod first vertex
	asm ("lqc2	 %0, %1"	: "=j" (vin) : "m" (*vIn));

	// load matrix
	asm ("lqc2	 %0, 0x00(%1)" : "=j" (mtx1) : "r" (this), "m" (*this));
	asm ("lqc2	 %0, 0x10(%1)" : "=j" (mtx2) : "r" (this), "m" (*this));
	asm ("lqc2	 %0, 0x20(%1)" : "=j" (mtx3) : "r" (this), "m" (*this));

	do
	{
		asm __volatile__ ("
		  vmulax.xyzw		ACC, %2, %1
		  vmadday.xyzw		ACC, %3, %1
		  vmaddaz.xyzw		ACC, %4, %1
		  vmaddw.xyzw		%0, vf00, %1" : "=j" (vout) : "j" (vin), "j" (mtx1), "j" (mtx2), "j" (mtx3));

		// move to next vtx...
		vIn++;

		// store prior vtx
		asm ("sqc2			%1, %0" : "=m" (*vOut) : "j" (vout));

		// load next vtx
		asm ("lqc2			%0, %1"	: "=j" (vin) : "m" (*vIn));
	} while(++vOut != vend);

#else

	/* define register use */

#define VOUT		%0		// order in output/input sections at bottom of assembly
#define VIN			%1		// order in output/input sections at bottom of assembly
#define NCOUNT		%2		// order in output/input sections at bottom of assembly
#define THIS		%3		// order in output/input sections at bottom of assembly

	asm __volatile__("
	lqc2				vf08,0x0(VIN)		/* load first input vector */
	lqc2				vf04,0x0(THIS)		/* load matrix */
	lqc2				vf05,0x10(THIS)
	lqc2				vf06,0x20(THIS)

_loopBasisTransformArrayVector4:

	vmulax.xyzw		ACC,vf04,vf08
	vmadday.xyzw	ACC,vf05,vf08
	vmaddaz.xyzw	ACC,vf06,vf08
	vmaddw.xyzw		vf09,vf00,vf08		/* vf00 is 0, 0, 0, 1-- no translation vector */
	addi				VIN,0x10				/* to next input vector */
	lqc2				vf08,0x0(VIN)		/* load next input vector */
	addi				NCOUNT,-1			/* decrement counter */
	sqc2				vf09,0x0(VOUT)		/* save output vector */
	addi				VOUT,0x10			/* to next output vector */
	bne				zero,NCOUNT,_loopBasisTransformArrayVector4
	" : "=r" (vOut), "=r" (vIn), "=r" (nCount) : "r" (this) : );

	/* clean up */

#undef VOUT
#undef VIN
#undef NCOUNT
#undef THIS

#endif
}
 
