/* --------------------------------------------------
 * File    : m_Matrix.h
 * Created : Thu Oct 15 17:44:54 1998
 * Descript:
 * --------------------------------------------------*/
#ifndef __M_MATRIX_H
#define __M_MATRIX_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

#include "platform/BaseType.h"

// We interact a lot with the vectors
#include "Math/Vector.h"

// The 3x3 matrix types
#include "Math/Mat3x3.h"

// The 4x4 matrix types
#include "Math/Mat4x4.h"

/******************* compile control ******************************/

// define this if you want to check that all Graphics4x4's have right column == {0, 0, 0, 1}-- it's slow, and uses
// ASSERTs, so it's debug version only
//#define CHECK_GRAPHICS4X4

/* certain routines are improved by a platform-specific implementation (for example, the matrix multiply routines can
be assembly-coded for vu0 on the ps2). this is implemented by defining, on a per-platform basis, labels that will exclude
the generic implementation of the routine, and signal that it needs to be implemented in the platform-specific .cpp or
.hpp file. for example, if i define EXCLUDE_GENERIC_MAT4X4_TIMES_MAT4X4 for the ps2 build, then the implementation of:

	inline Matrix4x4 operator*(const Matrix4x4 &m) const;

will be ifdef'd out of Math\Generic\Mat4x4.hpp and implemented in Math\PS2\Mat4x4.hpp

if you define a new one of these, you should go to the implementation in Math\Generic\Mat4x4.hpp and place an
#ifndef/#endif pair around the routine, then implement the routine in the appropriate platform-specific Mat4x4.hpp.
new defines should always be confined to a particular platform. i haven't been putting a corresponding #ifdef/#endif
pair around the platform-specific implementation, i suppose there's no harm done either way...

the defines should incorporate the name of the class and the name of the routine */

#ifdef WIN32
#if MMX_OK  	// people running older amd's can't use pentium mmx
#define EXCLUDE_GENERIC_MAT4X4_FAST_MULTIPLY							// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_MAT4X4_TIMES_MAT4X4							// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_MAT4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_MAT4X4_TIMES_GRAPHICS4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_GRAPHICS4X4			// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_GRAPHICS4X4_FAST_MULTIPLY					// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4				// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// exclude generic version so that we can implement it as a call to FastMultiply
#endif //MMX_OK
#endif //WIN32

#ifdef PS2
#define EXCLUDE_GENERIC_MAT4X4_FAST_MULTIPLY							// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_MAT4X4_TIMES_MAT4X4							// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_MAT4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_MAT4X4_TIMES_GRAPHICS4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_GRAPHICS4X4			// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_MAT4X4_IDENTITY
#define EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4				// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_GRAPHICS4X4_FAST_MULTIPLY					// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_GRAPHICS4X4_TRANSFORM_ARRAY_VECTOR3
#define EXCLUDE_GENERIC_GRAPHICS4X4_TRANSFORM_ARRAY_VECTOR4
#define EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_ARRAY_VECTOR3
#define EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_ARRAY_VECTOR4
#endif //PS2

#ifdef GCN
#define EXCLUDE_GENERIC_MAT4X4_FAST_MULTIPLY							// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_MAT4X4_TIMES_MAT4X4							// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_MAT4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_MAT4X4_TIMES_GRAPHICS4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_MAT4X4_TIMES_EQUALS_GRAPHICS4X4			// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_GRAPHICS4X4_FAST_MULTIPLY					// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_MAT4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_GRAPHICS4X4				// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_GRAPHICS4X4_TIMES_EQUALS_GRAPHICS4X4	// exclude generic version so that we can implement it as a call to FastMultiply
#endif

// scalar ops
inline Matrix3x3 operator*(float s, const Matrix3x3 &m);
inline Matrix3x3 operator*(float s, const DirCos3x3 &m);
inline Matrix4x4 operator*(float s, const Matrix4x4 &m);
inline Matrix4x4 operator*(float s, const Graphics4x4 &m);
inline Matrix4x4 operator*(float s, const Cartesian4x4 &m);

// The implementation for 3x3 matrix
#include "Math/Generic/Mat3x3.hpp"

// Include the appropriate implementation for the platform
#if defined(WIN32)
#include "Math/win32/Mat4x4.hpp"
#elif defined(PS2)
#if defined(PS2) && defined(SN)
#include "Math/PS2/Mat4x4sn.hpp"
#else
#include "Math/PS2/Mat4x4.hpp"
#endif
#elif defined(GCN)
#include "Math/gcn/Mat4x4.hpp"
#endif

// The implementation for 4x4 matrix (TJC-has to come after the ones above!)
#include "Math/Generic/Mat4x4.hpp"

// Include the implementations for the general functions
#include "Math/Generic/Matrix.hpp"



#endif __M_MATRIX_H
