/***************************************************************************/
// The 4x4 matrix classes
//
// At the most basic level we have an array of 16 floats that is arranged
// in a way to be compatible with the underlying graphics libraries.  Because
// of this organization, all elements are named, and code CANNOT assume that
// there are castable row vectors (or conversely castable column vectors).
//
// The derived Matrix4x4 class encapsulates all the "standard" linear algebra
// type functionality.
//
// Derived from the Matrix4x4 class is the Graphics4x4 class which basically
// represents an orthonormal basis.
//
// Since no virtual functions are used and there is function overloading,
// the appropriate method is decided upon at compile time...for example
// Matrix4x4::Invert is fundamentally different than Graphics4x4::Invert
//
// this file should not be included explicitly-- please use Math\Matrix.h
/***************************************************************************/

#ifndef M_MAT4X4_H
#define M_MAT4X4_H

/******************* compile control ******************************/

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

/******************* forward declarations *************************/

class Cartesian4x4;
class Matrix3x3;
class DirCos3x3;

/******************* class definition *****************************/

class
#ifdef WIN32
#if MMX_OK							// alignment only necessary if using simd instructions
__declspec(align(SIMD_ALIGN))	// for pc, matrices must be aligned for mmx simd implementation of matrix routines
#endif //MMX_OK
#endif //WIN32
Matrix4x4Data
{
public:
	inline Matrix4x4Data(void) {}

	// We contain the union for all the data members (kind of backwards,
	// but the only way possible for c++ to do this)
	union
	{
		// The generic form for Matrix4x4 e<row><column>
		struct
		{
			float e00, e01, e02, e03,
					e10, e11, e12, e13,
					e20, e21, e22, e23,
					e30, e31, e32, e33;
		};

		// The form for Graphics4x4
		// (u0, u1, u2, indicate unused elements)
		struct
		{
			float r00, r01, r02, u0,
					r10, r11, r12, u1,
					r20, r21, r22, u2,
					tx,  ty,  tz,  w;
		};

		// Another form for Nate
		// (x0, x1, x2, indicate unused elements)
		struct
		{
			float rx, ry, rz, x0,
					ux, uy, uz, x1,
					dx, dy, dz, x2,
					px, py, pz, mw;
		};

		// The evil perspective transformation matrix
		// pw = cot(fovW / 2) // fovW = fov Width
		// ph = cot(fovH / 2) // fovH = fov height
		// pq = yon / (yon - hither)
		// pqn = -pq * hither;
		// one = 1.0f;
		// all z<row><column> = 0.0f;
		struct
		{
			float pw,  z01, z02, z03,
					z10, ph,  z12, z13,
					z20, z21, pq,  one,
					z30, z31, pqn, z33;
		};

		// these are for the PS2 to get a speed boost in
		// inline assembly...
#ifdef PS2
		struct
		{
			u128 row0;
			u128 row1;
			u128 row2;
			u128 row3;
		};
#endif
	};
}
#ifdef PS2
__attribute__((aligned (VU_ALIGN)))	// for ps2, matrices must be aligned for vu0 implementation of matrix routines
#endif //PS2
;

// General 4x4 matrix class, make sure you explicitly declare
class
#ifdef WIN32
#if MMX_OK							// alignment only necessary if using simd instructions
__declspec(align(SIMD_ALIGN))	// for pc, matrices must be aligned for mmx simd implementation of matrix routines
#endif //MMX_OK
#endif //WIN32
Matrix4x4 : public Matrix4x4Data
{
public:
	// constructors
	inline Matrix4x4(void) {}
	inline Matrix4x4(float e00, float e01, float e02, float e03,
							float e10, float e11, float e12, float e13,
							float e20, float e21, float e22, float e23,
							float e30, float e31, float e32, float e33);

	// Sets

	// Assignment
	inline Matrix4x4 &operator=(const Matrix4x4 &m);
	inline Matrix4x4 &operator=(const Graphics4x4 &m);
	inline Matrix4x4 &operator=(const Cartesian4x4 &m);

	// Memberwise set
	inline void Set(float e00, float e01, float e02, float e03,
							float e10, float e11, float e12, float e13,
							float e20, float e21, float e22, float e23,
							float e30, float e31, float e32, float e33);

	// Set to identity (and a related test)
	inline void Identity(void);
	inline bool IsIdentity(void) const;

	// General operations on a matrix

	// Transpose the matrix
	inline void Transpose(Matrix4x4 &transpose) const;
	inline void Transpose(void);

	// Return the determinant
	static inline float Det(const Matrix4x4 &m);
	inline float Det(void) const;

	// Make the cofactor matrix
	inline void Cof(Matrix4x4 &cofactor) const;
	inline void Cof(void);

	// Make the adjoint matrix
	inline void Adj(Matrix4x4 &adjoint) const;
	inline void Adj(void);

	// Invert (returns false if not invertable)
	bool Invert(Matrix4x4 &inverse) const;
	inline bool Invert(void);

	// Equality tests
	inline int operator==(const Matrix4x4 &m) const;
	inline int operator!=(const Matrix4x4 &m) const;
	inline int operator==(const Graphics4x4 &m) const;
	inline int operator!=(const Graphics4x4 &m) const;
	inline int operator==(const Cartesian4x4 &m) const;
	inline int operator!=(const Cartesian4x4 &m) const;

	// Memberwise scalar ops
	inline Matrix4x4 operator*(float s) const;
	inline Matrix4x4 operator/(float s) const;
	inline Matrix4x4 &operator*=(float s);
	inline Matrix4x4 &operator/=(float s);

	// Memberwise add and subtract
	inline Matrix4x4 operator+(const Matrix4x4 &m) const;
	inline Matrix4x4 &operator+=(const Matrix4x4 &m);
	inline Matrix4x4 operator-(const Matrix4x4 &m) const;
	inline Matrix4x4 &operator-=(const Matrix4x4 &m);
	inline Matrix4x4 operator+(const Graphics4x4 &m) const;
	inline Matrix4x4 &operator+=(const Graphics4x4 &m);
	inline Matrix4x4 operator-(const Graphics4x4 &m) const;
	inline Matrix4x4 &operator-=(const Graphics4x4 &m);
	inline Matrix4x4 operator+(const Cartesian4x4 &m) const;
	inline Matrix4x4 &operator+=(const Cartesian4x4 &m);
	inline Matrix4x4 operator-(const Cartesian4x4 &m) const;
	inline Matrix4x4 &operator-=(const Cartesian4x4 &m);

	// Various matrix multiplication operations.

	// fast multiply-- always a platform-specific implementation. must handle this == m1, this == m2, m1 == m2
	inline Matrix4x4 &FastMultiply(const Matrix4x4 &m1, const Matrix4x4 &m2);	// this = m1 * m2

	// Multiply two 4x4's making no assumptions about them
	inline Matrix4x4 operator*(const Matrix4x4 &m) const;
	inline Matrix4x4 &operator*=(const Matrix4x4 &m);

	// The remaining multiplies do not depend on any assumptions about the matrix but they
	// only really make sense if the matrix is a 3D homogenous transformation matrix

	// Multiply by a Graphics4x4 / Cartesian4x4
	inline Matrix4x4 operator*(const Graphics4x4 &m) const;
	inline Matrix4x4 operator*(const Cartesian4x4 &m) const;
	inline Matrix4x4 &operator*=(const Graphics4x4 &m);
	inline Matrix4x4 &operator*=(const Cartesian4x4 &m);

	// Multiply by the inverse of a Cartesian4x4, taking advantage of the
	// orthonormal orientation component.
	inline Matrix4x4 operator/(const Cartesian4x4 &m) const;
	inline Matrix4x4 &operator/=(const Cartesian4x4 &m);

	// Mutliply by Matrix3x3's...assumes they are padded like a standard
	// Graphics4x4 w/0's in tx,ty,tz
	inline Matrix4x4 operator*(const Matrix3x3 &m) const;
	inline Matrix4x4 &operator*=(const Matrix3x3 &m);
	inline Matrix4x4 operator*(const DirCos3x3 &m) const;
	inline Matrix4x4 &operator*=(const DirCos3x3 &m);

	// Multiply by the inverse (transpose) of a DirCos3x3, padding with 0's and 1.
	inline Matrix4x4 operator/(const DirCos3x3 &m) const;
	inline Matrix4x4 &operator/=(const DirCos3x3 &m);

	// Gets and sets that are useful when the matrix is a 3D homogenous transformation matrix

	// Get the first three values in a row (copies)
	inline Vector3 Row(int row) const;

	// Get the first three values in a column (copies)
	inline Vector3 Column(int column) const;

	// Set a row or column (only first 3 values...)
	inline void SetRow(int row, Vector3CRef data);
	inline void SetColumn(int column, Vector3CRef data);

	// Get a the first three values of a row or column into a vector
	inline Vector3 GetRow0(void) const;
	inline Vector3 GetRow1(void) const;
	inline Vector3 GetRow2(void) const;
	inline Vector3 GetRow3(void) const;
	inline Vector3 GetColumn0(void) const;
	inline Vector3 GetColumn1(void) const;
	inline Vector3 GetColumn2(void) const;
	inline Vector3 GetColumn3(void) const;
	inline void SetRow0(Vector3CRef v);
	inline void SetRow1(Vector3CRef v);
	inline void SetRow2(Vector3CRef v);
	inline void SetRow3(Vector3CRef v);
	inline void SetColumn0(Vector3CRef v);
	inline void SetColumn1(Vector3CRef v);
	inline void SetColumn2(Vector3CRef v);
	inline void SetColumn3(Vector3CRef v);

	// Deconstructs into scale/shear/perspective values
	// (treats the matrix as a 3D homogenous transformation matrix)
	bool Deconstruct(DirCos3x3 &rot, Vector3 &trans, float &scaleX,
			float &scaleY, float &scaleZ, float &shearXY, float &shearXZ,
			float &shearYZ, float &persX, float &persY, float &persZ,
			float &persW) const;

#ifdef GCN
	// Swap the bytes for gamecube
	inline void EndianSwap(void);
#endif
};

// A homogenous transformation with no perspective.
// u0, u1, u2 = 0, w = 1
class
#ifdef WIN32
#if MMX_OK							// alignment only necessary if using simd instructions
__declspec(align(SIMD_ALIGN))	// for pc, matrices must be aligned for mmx simd implementation of matrix routines
#endif //MMX_OK
#endif //WIN32
Graphics4x4 : public Matrix4x4
{
public:
	// Do nothing
	inline Graphics4x4(void) {}

	// Build from a rot/scale/shear + position
	inline Graphics4x4(const Matrix3x3 &m, Vector3CRef pos);

	// Specifiy all the relevant elements
	inline Graphics4x4(float r00, float r01, float r02,
								float r10, float r11, float r12,
								float r20, float r21, float r22,
								float tx, float ty, float tz);

	// Assignment
	inline Graphics4x4 &operator=(const Graphics4x4 &m);
	inline Graphics4x4 &operator=(const Cartesian4x4 &m);

	// Set the 3x3 into the rot/scale/shear area
	inline Graphics4x4 &operator=(const Matrix3x3 &m);
	inline Graphics4x4 &operator=(const DirCos3x3 &m);

	// Set from the basic data
  	inline void Set(float r00, float r01, float r02,
			  				float r10, float r11, float r12,
			  				float r20, float r21, float r22,
			  				float tx, float ty, float tz);

	// Set the 3x3 into the rot/scale/shear area
	inline void Set(const Matrix3x3 &m);

	// Return the determinant
	// Watch out, this function is not virtual
	static inline float Det(const Graphics4x4 &m);
	inline float Det(void) const;

	// Make the cofactor matrix
	// Watch out, this function is not virtual
	inline void Cof(Matrix4x4 &cofactor) const;

	// Make the adjoint matrix
	// Watch out, this function is not virtual
	inline void Adj(Matrix4x4 &adjoint) const;

	// Invert (returns false if not invertable)
	// Watch out, this function is not virtual
	bool Invert(Matrix4x4 &inverse) const;
	inline bool Invert(void);

	// Equality tests
	inline int operator==(const Matrix4x4 &m) const;
	inline int operator!=(const Matrix4x4 &m) const;
	inline int operator==(const Graphics4x4 &m) const;
	inline int operator!=(const Graphics4x4 &m) const;
	inline int operator==(const Cartesian4x4 &m) const;
	inline int operator!=(const Cartesian4x4 &m) const;

	// Memberwise scalar ops
	inline Matrix4x4 operator*(float s) const;
	inline Matrix4x4 operator/(float s) const;

	// Memberwise add and subtract
	inline Matrix4x4 operator+(const Matrix4x4 &m) const;
	inline Matrix4x4 operator-(const Matrix4x4 &m) const;
	inline Matrix4x4 operator+(const Graphics4x4 &m) const;
	inline Matrix4x4 operator-(const Graphics4x4 &m) const;
	inline Matrix4x4 operator+(const Cartesian4x4 &m) const;
	inline Matrix4x4 operator-(const Cartesian4x4 &m) const;

	// fast multiply-- always a platform-specific implementation. must handle this == m1, this == m2, m1 == m2
	inline Graphics4x4 &FastMultiply(const Graphics4x4 &m1, const Graphics4x4 &m2);	// this = m1 * m2

	// Multiply
	inline Matrix4x4 operator*(const Matrix4x4 &m) const;

	// Multiply
	inline Graphics4x4 operator*(const Graphics4x4 &m) const;
	inline Graphics4x4 operator*(const Cartesian4x4 &m) const;
	inline Graphics4x4 &operator*=(const Graphics4x4 &m);
	inline Graphics4x4 &operator*=(const Cartesian4x4 &m);

	// Multiply by the inverse of a Cartesian4x4, taking advantage of the
	// orthonormal orientation component.
	inline Graphics4x4 operator/(const Cartesian4x4 &m) const;
	inline Graphics4x4 &operator/=(const Cartesian4x4 &m);

	// Mutliply by Matrix3x3's...assumes they are padded like a standard
	// Graphics4x4 w/0's in tx,ty,tz
	inline Graphics4x4 operator*(const Matrix3x3 &m) const;
	inline Graphics4x4 &operator*=(const Matrix3x3 &m);
	inline Graphics4x4 operator*(const DirCos3x3 &m) const;
	inline Graphics4x4 &operator*=(const DirCos3x3 &m);

	// Multiply by the inverse (transpose) of a DirCos3x3, padding with 0's and 1.
	inline Graphics4x4 operator/(const DirCos3x3 &m) const;
	inline Graphics4x4 &operator/=(const DirCos3x3 &m);

	// Transform homogenous x y z 1 with m
	inline Vector3 Transform(float x, float y, float z) const;

	// Create a scale matrix (overwrites existing matrix, not cat'ed)
	inline void SetScale(float xScale, float yScale, float zScale);

	// Simple (sets other scale values to 1)
	inline void SetXScale(float scale);
	inline void SetYScale(float scale);
	inline void SetZScale(float scale);

	// Do rotations around x,y,z axes.  Does not cat, sets
	inline void SetXRotation(float radians);
	inline void SetYRotation(float radians);
	inline void SetZRotation(float radians);
	inline void SetXYZRotation(Vector3CRef vRadians);
	inline void SetXZYRotation(Vector3CRef vRadians);
	inline void SetXYZRotationDeg(Vector3CRef vDegrees);

	// Translation (these setup the entire matrix)
	inline void SetTranslation(float x, float y, float z);
	inline void SetTranslation(Vector3CRef t);
	inline void SetXTranslation(float pos);
	inline void SetYTranslation(float pos);
	inline void SetZTranslation(float pos);

	// Translation (these just twiddle the 3 translation values)
	inline void PokeTranslation(float x, float y, float z);
	inline void PokeTranslation(Vector3CRef t);
	inline void PokeXTranslation(float pos);
	inline void PokeYTranslation(float pos);
	inline void PokeZTranslation(float pos);

	// Cat's
	inline void CatScale(float xScale, float yScale, float zScale);
	inline void CatXScale(float scale);
	inline void CatYScale(float scale);
	inline void CatZScale(float scale);
	inline void CatXRotation(float radians);
	inline void CatYRotation(float radians);
	inline void CatZRotation(float radians);
	inline void CatTranslation(float x, float y, float z);
	inline void CatXTranslation(float pos);
	inline void CatYTranslation(float pos);
	inline void CatZTranslation(float pos);

	// Reverse cat's-- original avalanche math library assumed that matrix order was different: e.g. m = rot * m.
	// the cat's above implement m *= rot
	inline void ReverseCatScale(float xScale, float yScale, float zScale);
	inline void ReverseCatXScale(float scale);
	inline void ReverseCatYScale(float scale);
	inline void ReverseCatZScale(float scale);
	inline void ReverseCatXRotation(float radians);
	inline void ReverseCatYRotation(float radians);
	inline void ReverseCatZRotation(float radians);
	inline void ReverseCatTranslation(float x, float y, float z);
	inline void ReverseCatXTranslation(float pos);
	inline void ReverseCatYTranslation(float pos);
	inline void ReverseCatZTranslation(float pos);

	// Get/set the 3x3 orientation
	inline void GetOrientation(Matrix3x3 &m) const;
	inline void PokeOrientation(const Matrix3x3 &m);

	// Get the translation
	inline Vector3 GetTranslation(void) const;

	// Compose/decompose from a position orientation
	inline void Compose(const Matrix3x3 &o, Vector3CRef pos);
	inline void Decompose(Matrix3x3 &o, Vector3 &pos) const;

	// New and improved (with comments too)!
	bool BuildFromVector(Vector3CRef v, int vectorRow = 2, int alignRow = 1,
			int alignAxis = 1);
	bool BuildFromUnitVector(Vector3CRef v, int vectorRow = 2,
			int alignRow = 1, int alignAxis = 1);

	// Documentation on these would be nice
	bool DecodeToYXZ(Vector3 &v) const;
	bool DecodeToXZY(Vector3 &v) const;
	bool DecodeToZYX(Vector3 &v) const;
	void SetIntermediate(float rPercentage, Graphics4x4 &g3To);
	void BuildFromQuat(float rA, float rB, float rC, float rD);

	// plow through an array of vertices
	void TransformArray(const Vector3 *vIn, Vector3 *vOut, int nCount) const;
	void TransformArray(const Vector4 *vIn, Vector4 *vOut, int nCount) const;
	void BasisTransformArray(const Vector3 *vIn, Vector3 *vOut, int nCount) const;
	void BasisTransformArray(const Vector4 *vIn, Vector4 *vOut, int nCount) const;

	// Deconstructs into scale/shear values
	bool Deconstruct(DirCos3x3 &rot, Vector3 &trans, float &scaleX,
			float &scaleY, float &scaleZ, float &shearXY, float &shearXZ,
			float &shearYZ) const;
};

// A homogenous transformation with only translation and rotation.
// Upper left 3x3 is orthonormal
class
#ifdef WIN32
#if MMX_OK						// alignment only necessary if using simd instructions
__declspec(align(SIMD_ALIGN))	// for pc, matrices must be aligned for mmx simd implementation of matrix routines
#endif //MMX_OK
#endif //WIN32
Cartesian4x4 : public Graphics4x4
{
public:
	// Do nothing
	inline Cartesian4x4(void) {}

	// Build from a rot + position
	inline Cartesian4x4(const DirCos3x3 &m, Vector3CRef pos);

	// Assignment
	inline Cartesian4x4 &operator=(const Cartesian4x4 &m);

	// Set the 3x3 into the rot/scale/shear area
	inline Cartesian4x4 &operator=(const DirCos3x3 &m);

	// Invert, always works. (it's the transpose and translation transformation)
	// Watch out, this function is not virtual
	bool Invert(Matrix4x4 &inverse) const;
	inline bool Invert(void);

	// Equality tests
	inline int operator==(const Matrix4x4 &m) const;
	inline int operator!=(const Matrix4x4 &m) const;
	inline int operator==(const Graphics4x4 &m) const;
	inline int operator!=(const Graphics4x4 &m) const;
	inline int operator==(const Cartesian4x4 &m) const;
	inline int operator!=(const Cartesian4x4 &m) const;

	// Memberwise scalar ops
	inline Matrix4x4 operator*(float s) const;
	inline Matrix4x4 operator/(float s) const;

	// Memberwise add and subtract
	inline Matrix4x4 operator+(const Matrix4x4 &m) const;
	inline Matrix4x4 operator-(const Matrix4x4 &m) const;
	inline Matrix4x4 operator+(const Graphics4x4 &m) const;
	inline Matrix4x4 operator-(const Graphics4x4 &m) const;
	inline Matrix4x4 operator+(const Cartesian4x4 &m) const;
	inline Matrix4x4 operator-(const Cartesian4x4 &m) const;

	// Multiply
	inline Matrix4x4 operator*(const Matrix4x4 &m) const;

	// Multiply
	inline Graphics4x4 operator*(const Graphics4x4 &m) const;
	inline Cartesian4x4 operator*(const Cartesian4x4 &m) const;
	inline Cartesian4x4 &operator*=(const Cartesian4x4 &m);

	// Multiply by the inverse of a Cartesian4x4, taking advantage of the
	// orthonormal orientation component.
	inline Cartesian4x4 operator/(const Cartesian4x4 &m) const;
	inline Cartesian4x4 &operator/=(const Cartesian4x4 &m);

	// Mutliply by Matrix3x3's...assumes they are padded like a standard
	// Graphics4x4 w/0's in tx,ty,tz
	inline Graphics4x4 operator*(const Matrix3x3 &m) const;
	inline Cartesian4x4 operator*(const DirCos3x3 &m) const;
	inline Cartesian4x4 &operator*=(const DirCos3x3 &m);

	// Multiply by the inverse (transpose) of a DirCos3x3, padding with 0's and 1.
	inline Cartesian4x4 operator/(const DirCos3x3 &m) const;
	inline Cartesian4x4 &operator/=(const DirCos3x3 &m);

	// Invert then multiply
	inline Matrix4x4 InvertTimes(const Matrix4x4 &m) const;
	inline Graphics4x4 InvertTimes(const Graphics4x4 &m) const;
	inline Cartesian4x4 &InvertTimesEquals(const Cartesian4x4 &m);
	inline Graphics4x4 InvertTimes(const Matrix3x3 &m) const;
	inline Cartesian4x4 &InvertTimesEquals(const DirCos3x3 &m);

	// Invert then multiply by invert
	inline Cartesian4x4 InvertDivide(const Cartesian4x4 &m) const;
	inline Cartesian4x4 &InvertDivideEquals(const Cartesian4x4 &m);
	inline Cartesian4x4 InvertDivide(const DirCos3x3 &m) const;
	inline Cartesian4x4 &InvertDivideEquals(const DirCos3x3 &m);

	// Override scale operations to block them.
	inline void SetScale(float xScale, float yScale, float zScale);
	inline void SetXScale(float scale);
	inline void SetYScale(float scale);
	inline void SetZScale(float scale);
	inline void CatScale(float xScale, float yScale, float zScale);
	inline void CatXScale(float scale);
	inline void CatYScale(float scale);
	inline void CatZScale(float scale);
	inline void ReverseCatScale(float xScale, float yScale, float zScale);
	inline void ReverseCatXScale(float scale);
	inline void ReverseCatYScale(float scale);
	inline void ReverseCatZScale(float scale);
};

#endif
