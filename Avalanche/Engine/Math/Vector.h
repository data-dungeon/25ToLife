/* --------------------------------------------------
 * File    : m_Vector.h
 * Created : Mon Oct 19 10:38:42 1998
 * Descript:
 * --------------------------------------------------*/
//
// TJC - We have a bit to talk about in here.  First, here are the
// usable types defined in this file (and its children):
// 
//  Vector3					- The fastest form of Vector3 for this platform (either packed or padded)
//  Vector3Packed			- A form of vector3 that always uses 3 words.
//  Vector3Padded			- A form of vector3 that always uses 4 words. (the 4th is useable by you, but may be damaged by math operations)
//  Vector4					- The fastest form of Vector4 for this platform (either packed or aligned)
//  Vector4Packed			- A form of Vector4 that has only 4byte alignment requirements
//  Vector4Aligned		- A form of Vector4 that has platform specific alignment requirements (probably qword)
//  Vector2					- The fastest form of Vector2 for this platform (packed or padded)
//  [Vector2Packed]		- [unimplimented] A form of Vector2 that always uses 2 words.
//  [Vector2Padded]		- [unimplimented] A form of Vector2 that always uses 4 words. (the extra 2 are usable by you, but may be damaged by math operations)
//  [Vector2PackedData]	- [unimplimented] The data from a packed vector2 (no operators, but can be converted to either valid Vector2 for usage.  Can exist in union)
//  Vector3PackedData	- The data from a packed vector3 (no operators, but can be converted to either valid Vector2 for usage.  Can exist in union)
//  Vector4PackedData	- The data from a packed vector4 (no operators, but can be converted to either valid Vector2 for usage.  Can exist in union)
//  Vector3Data			- Data from either a packed or padded vector3 (whichever is faster.  Can exist in a union)
//
// Whew.. thats quite a few.   GET USED TO THEM!  Usually, you'll be
// using the unqualified VectorN types. But if you are defining data,
// you might find the VectorNPackedData types useful.  Most of the
// rest exist to support the others.  On some platforms, there may be
// additional hidden types used to accelerate certain calculations.
// You dont really need to worry about them.  They should be
// transparent.  [They dont even exist yet :)].  Their usage is
// identical to the old vector classes.  But they have MANY more
// possible methods on them.  USE THE METHODS!  On some platforms,
// they are much faster than doing the floating point math yourself,
// and they are simpler.
//
// References: References are a bad thing for VectorN classes.  On one
// platform, the vector can fit entirely in a single register, so
// there is speed to be had by not using references.  but on other
// platforms, references are the best way.  So, there are a few
// typedefs to help out with this.  They replace a const& to each of
// the VectorN and VectorNData classes.  They should be used whenever
// possible.
//
//  [Vector2CRef]
//  [Vector2PackedCRef]
//  [Vector2PaddedCRef]
//  [Vector2PackedDataCRef]
//  Vector3CRef
//  Vector3PackedCRef
//  Vector3PaddedCRef
//  Vector3PackedDataCRef
//  Vector4CRef
//  Vector4PackedCRef
//  Vector4AlignedCRef
//  Vector4PackedDataCRef
//
// To Extend:  Add the code to the VectorNTemplate.h &
// VectorNTemplate.hpp.  Please do so in a manner that doesnt rely on
// the underlying data.  ie, Use x(),y(),z(),w() instead of accessing
// members directly.  While you can legally access the members
// directly on some platforms, on others they dont even exist.
//
// NOTE: most of this file is to support certain rare optimizations.
// I worked them in because I was here doing changes, and didnt really
// see a reason not to.  They allow us to optimize operations such as
// Vector3Packed + Vector3Padded, and control the result (is it a
// Packed or a Padded) on a per platform basis.  It meansa few extra
// templates.  To make them easyer to deal with, there are LOTS and
// LOTS of typedefs.  Most of this file you should never have to deal
// with.  Its a fairly complicated piece of compiler semantics.
// 
#ifndef __M_VECTOR_H
#define __M_VECTOR_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

#include "platform/BaseType.h"

// TJC - if anyone knows why CW is different, let me know.  It appears
// to be in the standard to have 'typename' in the places where I have
// TYPENAME, but only CW allows it.
#if defined ( CW )
#define TYPENAME typename
#define TYPENAME_DEC typename
#elif ( _MSC_VER > 1300 )
// N8 - looks like VC.net wants the typename stuff as well.  however, it only 
// likes it on the actual definition, not on any typedefs, so I've had to put
// in a new typename definition in addition to the old one.
#define TYPENAME typename
#define TYPENAME_DEC
#else
#define TYPENAME
#define TYPENAME_DEC
#endif

// Forward declare the vector types...
template<class DATA> class Vector3Template;
template<class DATA> class Vector4Template;

class Vector3PackedData;
class Vector3PaddedData;
class Vector4AlignedData;
class Vector4PackedData;

typedef Vector3Template<Vector3PaddedData> Vector3Padded;
typedef Vector3Template<Vector3PackedData> Vector3Packed;
typedef Vector4Template<Vector4PackedData> Vector4Packed;
typedef Vector4Template<Vector4AlignedData> Vector4Aligned;


// 2d vector
#include "Math/Vect2.h"

// 3d vector
#include "Math/Vector3GenericPacked.h"
#include "Math/Vector4GenericPacked.h"

// TJC - these allow us to say that a Packed + Padded = padded, or any
// other combination thats good for the platform.
template<class TYPE, class PARAMDATA>
struct Vector3TemplateTypes
{
	typedef Vector3Packed Ret;
};

template<class TYPE, class PARAMDATA>
struct Vector4TemplateTypes
{
	typedef Vector4Packed Ret;
};

template<class TYPE>
struct VectorTemplateFloats
{
	typedef float Ret;
	typedef float Param;
	typedef float ClipParam;
};

#define V3_RESULTS(TYPE1, TYPE2, RESULT)					\
template<> struct Vector3TemplateTypes<TYPE1, TYPE2>	\
{																		\
	typedef Vector3Template<RESULT> Ret;					\
};

#define V4_RESULTS(TYPE1, TYPE2, RESULT)					\
template<> struct Vector4TemplateTypes<TYPE1, TYPE2>	\
{																		\
	typedef Vector4Template<RESULT> Ret;					\
};

#define V_FLOATIO(TYPE, RETURN, PARAM, CLIP)		\
template<> struct VectorTemplateFloats<TYPE>		\
{																\
	typedef RETURN Ret;									\
	typedef PARAM Param;									\
	typedef CLIP ClipParam;								\
}

// TJC - VSE_PARSER is just a nicity for me
#if defined(PS2) && defined(SN) && !defined(VSE_PARSER)

// TJC - the ps2 specific formats for the fast versions of the vectors
#include "Vector3SNPS2Padded.h"
#include "Vector4SNPS2Aligned.h"

// TJC - the basics of what vectors the engine uses by default. and
// how to pass them around
typedef Vector3Padded Vector3;
typedef const Vector3 Vector3CRef;
typedef Vector4Aligned Vector4;
typedef const Vector4 Vector4CRef;
typedef Vector3PaddedData Vector3Data;

// TJC - these typedefs make the templates able to deal with
// specialization of operators between packed & aligned formats.
typedef Vector3PaddedData Vector3DataType;
typedef Vector3PackedData Vector3OtherDataType;
typedef Vector3Packed Vector3Other;
typedef const Vector3Other& Vector3OtherCRef;
typedef const Vector3Packed& Vector3PackedCRef;
typedef const Vector3Padded Vector3PaddedCRef;
typedef const Vector3PaddedData Vector3PaddedDataCRef;
typedef const Vector3PackedData& Vector3PackedDataCRef;

// TJC - these typedefs make the templates able to deal with
// specialization of operators between packed & aligned formats.
typedef Vector4PackedData Vector4DataType;
typedef Vector4AlignedData Vector4OtherDataType;
typedef Vector4Packed Vector4Other;
typedef const Vector4Other& Vector4OtherCRef;
typedef const Vector4Packed& Vector4PackedCRef;
typedef const Vector4Aligned Vector4AlignedCRef;
typedef const Vector4AlignedData Vector4AlignedDataCRef;
typedef const Vector4PackedData& Vector4PackedDataCRef;

// TJC - inline methods are good... we need these... :)
#include "Vector4SNPS2X.h"
#include "Vector3SNPS2Padded.hpp"
#include "Vector4SNPS2Aligned.hpp"

// TJC - These define which methods return which types of variables.
// It says basically, Packed + Packed = Packed., etc..
V3_RESULTS(Vector3PackedData, Vector3PackedData, Vector3PackedData);
V3_RESULTS(Vector3PackedData, Vector3PaddedData, Vector3PaddedData);
V3_RESULTS(Vector3PaddedData, Vector3PackedData, Vector3PaddedData);
V3_RESULTS(Vector3PaddedData, Vector3PaddedData, Vector3PaddedData);
V_FLOATIO(Vector3PaddedData, Vector4X, Vector4X, Vector4W);
V_FLOATIO(Vector3PackedData, float, float, float);
V4_RESULTS(Vector4PackedData, Vector4PackedData, Vector4PackedData);
V4_RESULTS(Vector4PackedData, Vector4AlignedData, Vector4AlignedData);
V4_RESULTS(Vector4AlignedData, Vector4PackedData, Vector4AlignedData);
V4_RESULTS(Vector4AlignedData, Vector4AlignedData, Vector4AlignedData);
V_FLOATIO(Vector4AlignedData, Vector4X, Vector4X, Vector4W);
V_FLOATIO(Vector4PackedData, float, float, float);

#else // defined(PS2) && defined(SN)

// TJC - generic versions of the data..
#include "Math/Vector3GenericPadded.h"
#include "Math/Vector4GenericAligned.h"

// TJC - the basics of what vectors the engine uses by default. and
// how to pass them around
typedef Vector4Aligned Vector4;
typedef const Vector4& Vector4CRef;
typedef Vector3Packed Vector3;
typedef const Vector3& Vector3CRef;
typedef Vector3PackedData Vector3Data;

// TJC - these typedefs make the templates able to deal with
// specialization of operators between packed & aligned formats.
typedef Vector4AlignedData Vector4DataType;
typedef Vector4PackedData Vector4OtherDataType;
typedef Vector4Packed Vector4Other;
typedef const Vector4Other& Vector4OtherCRef;
typedef const Vector4Packed& Vector4PackedCRef;
typedef const Vector4Aligned& Vector4AlignedCRef;
typedef const Vector4AlignedData& Vector4AlignedDataCRef;
typedef const Vector4PackedData& Vector4PackedDataCRef;

// TJC - these typedefs make the templates able to deal with
// specialization of operators between packed & aligned formats.
typedef Vector3PackedData Vector3DataType;
typedef Vector3PaddedData Vector3OtherDataType;
typedef Vector3Padded Vector3Other;
typedef const Vector3Other& Vector3OtherCRef;
typedef const Vector3Packed& Vector3PackedCRef;
typedef const Vector3Padded& Vector3PaddedCRef;
typedef const Vector3PaddedData& Vector3PaddedDataCRef;
typedef const Vector3PackedData& Vector3PackedDataCRef;

// TJC - These define which methods return which types of variables.
// It says basically, Packed + Packed = Packed., etc..
V3_RESULTS(Vector3PackedData, Vector3PackedData, Vector3PackedData);
V3_RESULTS(Vector3PackedData, Vector3PaddedData, Vector3PackedData);
V3_RESULTS(Vector3PaddedData, Vector3PackedData, Vector3PackedData);
V3_RESULTS(Vector3PaddedData, Vector3PaddedData, Vector3PaddedData);
V_FLOATIO(Vector3PaddedData, float, float, float);
V_FLOATIO(Vector3PackedData, float, float, float);
V4_RESULTS(Vector4PackedData, Vector4PackedData, Vector4PackedData);
V4_RESULTS(Vector4PackedData, Vector4AlignedData, Vector4PackedData);
V4_RESULTS(Vector4AlignedData, Vector4PackedData, Vector4PackedData);
V4_RESULTS(Vector4AlignedData, Vector4AlignedData, Vector4AlignedData);
V_FLOATIO(Vector4AlignedData, float, float, float);
V_FLOATIO(Vector4PackedData, float, float, float);
#endif // defined(PS2) && defined(SN)

// TJC -  these simplify overrides in the platform specific
// code... they define return types & parameters for paticular
// versions of functions.  For example, Vector4APRet defines the
// return type for a binary operator of 2 Vector4s, the first Aligned,
// the second packed (hench *AP*)..
typedef TYPENAME_DEC Vector3TemplateTypes<Vector3PaddedData, Vector3PaddedData>::Ret Vector3PadPadRet;
typedef TYPENAME_DEC Vector3TemplateTypes<Vector3PackedData, Vector3PaddedData>::Ret Vector3PakPadRet;
typedef TYPENAME_DEC Vector3TemplateTypes<Vector3PaddedData, Vector3PackedData>::Ret Vector3PadPakRet;
typedef TYPENAME_DEC Vector3TemplateTypes<Vector3PackedData, Vector3PackedData>::Ret Vector3PakPakRet;
typedef TYPENAME_DEC VectorTemplateFloats<Vector3PaddedData>::Ret Vector3PadFRet;
typedef TYPENAME_DEC VectorTemplateFloats<Vector3PackedData>::Ret Vector3PakFRet;
typedef TYPENAME_DEC VectorTemplateFloats<Vector3PaddedData>::Param Vector3PadFParam;
typedef TYPENAME_DEC VectorTemplateFloats<Vector3PackedData>::Param Vector3PakFParam;
typedef TYPENAME_DEC Vector4TemplateTypes<Vector4AlignedData, Vector4AlignedData>::Ret Vector4AARet;
typedef TYPENAME_DEC Vector4TemplateTypes<Vector4PackedData, Vector4AlignedData>::Ret Vector4PARet;
typedef TYPENAME_DEC Vector4TemplateTypes<Vector4AlignedData, Vector4PackedData>::Ret Vector4APRet;
typedef TYPENAME_DEC Vector4TemplateTypes<Vector4PackedData, Vector4PackedData>::Ret Vector4PPRet;
typedef TYPENAME_DEC VectorTemplateFloats<Vector4AlignedData>::Ret Vector4AFRet;
typedef TYPENAME_DEC VectorTemplateFloats<Vector4PackedData>::Ret Vector4PFRet;
typedef TYPENAME_DEC VectorTemplateFloats<Vector4AlignedData>::Param Vector4AFParam;
typedef TYPENAME_DEC VectorTemplateFloats<Vector4PackedData>::Param Vector4PFParam;

// TJC - now for the templates
#include "Math/Vector3Template.h"
#include "Math/Vector4Template.h"

// TJC - we need the matricies for the inline code below...
#include "Math/Mat3x3.h"
#include "Math/Mat4x4.h"

// TJC - inline the operators and methods and stuff...
// TJC - VSE_PARSER is just a nicity for me
#if ( defined(PS2) && defined(SN)  && !defined(VSE_PARSER) )
#include "Vector3SNPS2.hpp"
#include "Vector4SNPS2.hpp"
#elif defined(WIN32) || defined(VSE_PARSER)
#include "Vector3Generic.hpp"
#include "Vector4Generic.hpp"
#include "Vector3WIN32.hpp"
#include "Vector4WIN32.hpp"
#elif defined(GCN)
#include "Math/Vector3Generic.hpp"
#include "Math/Vector4Generic.hpp"
#include "Math/Vector3GCN.hpp"
#include "Math/Vector4GCN.hpp"
#endif

// TJC - inline the generic versions of all the operators & methods
// and such.
#include "Math/Vector3Template.hpp"
#include "Math/Vector4Template.hpp"

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/

// define this if you want the code to check alignment of variables going through the fast math implementations-- simd
// on the pc and vu0 on the ps2 (both vector and matrix-- Matrix.h includes this file, but not vice versa). it implement
// ASSERTs, so it's debug version only

//#define CHECK_MATH_ALIGNMENT

/******************* compile control ******************************/

// define this if you want to check that all Graphics4x4's have right column == {0, 0, 0, 1}-- it's slow, and uses
// ASSERTs, so it's debug version only
//#define CHECK_GRAPHICS4X4

// TJC - This section is being (slowly) depricated.  The templates
// allow us to define a generic version of a routine, but then
// override it for a specific data format or platform.  Currently, all
// the VectorN ones are depricated.  I hope to remove the others soon.

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

/* on the pc, it turns out that having versions of, say, Vector3 * Graphics4x4 that turn around and call
Vector3::FastMultiply are a performance loss on the pc (call overhead?), so don't exclude the generic
versions of these routines */
 
#define EXCLUDE_GENERIC_VECTOR3_FAST_MULTIPLY						// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_VECTOR3_FAST_BASIS_TRANSFORM				// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_VECTOR4_FAST_MULTIPLY						// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_VECTOR4_FAST_TRANSFORM						// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_VECTOR4_FAST_BASIS_TRANSFORM				// should always be a platform-specific implementation, but a generic version does exist
#endif //MMX_OK
#endif //WIN32

#ifdef PS2
#define EXCLUDE_GENERIC_VECTOR3_TIMES_GRAPHICS4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_VECTOR4_TIMES_GRAPHICS4X4					// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_VECTOR4_TIMES_MATRIX4X4						// exclude generic version so that we can implement it as a call to FastMultiply
#define EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_VECTOR3		// exclude generic version so that we can implement it as a call to FastBasisTransform
#define EXCLUDE_GENERIC_GRAPHICS4X4_BASIS_TRANSFORM_VECTOR4		// exclude generic version so that we can implement it as a call to FastBasisTransform
#define EXCLUDE_GENERIC_VECTOR3_FAST_MULTIPLY						// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_VECTOR3_FAST_BASIS_TRANSFORM				// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_VECTOR4_FAST_MULTIPLY						// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_VECTOR4_FAST_TRANSFORM						// should always be a platform-specific implementation, but a generic version does exist
#define EXCLUDE_GENERIC_VECTOR4_FAST_BASIS_TRANSFORM				// should always be a platform-specific implementation, but a generic version does exist
#endif //PS2

/* --------------------------------------------------
 * General utilities
 * --------------------------------------------------*/

// This can't be coded as part of the classes
inline Vector2 operator*(float scalar, const Vector2 &v);
inline Vector3 operator*(float scalar, Vector3CRef v);
inline Vector4 operator*(float scalar, const Vector4 &v);

// Rotate a point around an arbitrary axis (axis should be normalized)
Vector3 RotatePointAroundAxis(Vector3CRef p, Vector3CRef axis, float theta);

// Distance from point to line segment calls in 2d
Vector2 NearestPointOnLineSeg(const Vector2 &p, const Vector2 &l1, const Vector2 &l2);
void NearestPointOnLineSeg(const Vector2 &p, const Vector2 &l1, const Vector2 &l2, Vector2 &out);
Vector2 NearestPointOnLine(const Vector2 &p, const Vector2 &l1, const Vector2 &l2);
void NearestPointOnLine(const Vector2 &p, const Vector2 &l1, const Vector2 &l2, Vector2 &out);

// Distance from point to line segment calls in 3d
Vector3 NearestPointOnLineSeg(Vector3CRef p, Vector3CRef l1, Vector3CRef l2);
void NearestPointOnLineSeg(Vector3CRef p, Vector3CRef l1, Vector3CRef l2, Vector3 &out);
Vector3 NearestPointOnLine(Vector3CRef p, Vector3CRef l1, Vector3CRef l2);
void NearestPointOnLine(Vector3CRef p, Vector3CRef l1, Vector3CRef l2, Vector3 &out);

// Utility for comparing points in space
inline float Distance(Vector3CRef a, Vector3CRef b);
inline float DistanceSquared(Vector3CRef a, Vector3CRef b);
inline bool PointsCoincide(Vector3CRef a, Vector3CRef b, float thresholdSquared);

// Utiltiy for making an orthoganal vector
Vector3 MakeOrthogonal(Vector3CRef v);

// Some useful constants
extern const Vector3 c_origin;
extern const Vector3 c_up;
extern const Vector3 c_down;

// The implementation for 2d vector
#include "Math/Generic/Vect2.hpp"

// Include the implementations for the general functions
#include "Math/Generic/Vector.hpp"

#endif __M_VECTOR_H
