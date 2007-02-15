/***************************************************************************/
// The 3x3 matrix classes
//
// At the most basic level we have an array of 9 floats that is arranged
// in a way to be compatible with the underlying graphics libraries.  Because
// of this organization, all elements are named, and code CANNOT assume that
// there are castable row vectors (or conversely castable column vectors).
//
// The derived Matrix3x3 class encapsulates all the "standard" linear algebra
// type functionality.
//
// Derived from the Matrix3x3 class is the DirCos3x3 class which basically
// represents an orthonormal basis.
//
// Since no virtual functions are used and there is function overloading,
// the appropriate method is decided upon at compile time...for example
// Matrix3x3::Invert is fundamentally different than DirCos3x3::Invert
//
// this file should not be included explicitly-- please use Math\Matrix.h
/***************************************************************************/

#ifndef M_MAT3X3_H
#define M_MAT3X3_H

/******************* forward declarations *************************/

class Graphics4x4;
class Cartesian4x4;
class DirCos3x3;

/******************* class definition *****************************/

class Matrix3x3Data
{
public:
	inline Matrix3x3Data(void) {}

	// We contain the union for all the data members (kind of backwards,
	// but the only way possible for c++ to do this)
	union
	{
		// The generic form for Matrix3x3 e<row><column>
		struct
		{
			float e00, e01, e02,
					e10, e11, e12,
					e20, e21, e22;
		};
#if 0
		// The generic form for Graphics3x3 r<row><column>
		struct
		{
			float r00, r01, u0,
					r10, r11, u1,
					tx,  ty,  w;
		};
#endif
		// Another form for Nate (right, up, down)
		struct
		{
			float rx, ry, rz,
					ux, uy, uz,
					dx, dy, dz;
		};
	};
};

// General 3x3 matrix class, make sure you explicitly declare
class Matrix3x3 : public Matrix3x3Data
{
public:
	// Constructors
	inline Matrix3x3(void) {}
	inline Matrix3x3(float e00, float e01, float e02,
							float e10, float e11, float e12,
							float e20, float e21, float e22);
	inline Matrix3x3(Vector3CRef diagonalize);
	inline Matrix3x3(Graphics4x4 &m);

	// Sets

	// Assignment
	inline Matrix3x3 &operator=(const Matrix3x3 &m);
	inline Matrix3x3 &operator=(const DirCos3x3 &m);
	inline Matrix3x3 &operator=(const Graphics4x4 &m);
	inline Matrix3x3 &operator=(const Cartesian4x4 &m);

	// Memberwise set
	inline void Set(float e00, float e01, float e02,
				  	float e10, float e11, float e12,
				  	float e20, float e21, float e22);

  	// Diagonalize the vector across the matrix
	// (zero's the other entries)
	inline void Diagonalize(Vector3CRef v);

	// Skew symmetric
	inline void SkewSymmetric(Vector3CRef v);

	// Set to identity
	inline void Identity(void);
	inline bool IsIdentity(void);

	// Transpose the matrix
	inline void Transpose(Matrix3x3 &transpose) const;
	inline void Transpose(void);

	// Return the transposed matrix
	inline Matrix3x3 Transposed(void) const;

	// Invert (returns false if not invertable)
	bool Invert(Matrix3x3 &inverse) const;
	inline bool Invert(void);

	// Return the determinant
	inline float Det(void) const;
	static inline float Det(const Matrix3x3 &m);

	// Make the cofactor matrix
	inline void Cof(Matrix3x3 &cofactor) const;
	inline void Cof(void);

	// Make the adjoint matrix
	inline void Adj(Matrix3x3 &adjoint) const;
	inline void Adj(void);

	// everything else is overloaded operators
	inline int operator==(const Matrix3x3 &m) const;
	inline int operator==(const DirCos3x3 &m) const;
	inline int operator!=(const Matrix3x3 &m) const;
	inline int operator!=(const DirCos3x3 &m) const;

	// Memberwise scalar ops
	inline Matrix3x3 operator*(float s) const;
	inline Matrix3x3 operator/(float s) const;
	inline Matrix3x3 &operator*=(float s);
	inline Matrix3x3 &operator/=(float s);

	// Memberwise add and subtract
	inline Matrix3x3 operator+(const Matrix3x3 &m) const;
	inline Matrix3x3 &operator+=(const Matrix3x3 &m);
	inline Matrix3x3 operator+(const DirCos3x3 &m) const;
	inline Matrix3x3 &operator+=(const DirCos3x3 &m);
	inline Matrix3x3 operator-(const Matrix3x3 &m) const;
	inline Matrix3x3 &operator-=(const Matrix3x3 &m);
	inline Matrix3x3 operator-(const DirCos3x3 &m) const;
	inline Matrix3x3 &operator-=(const DirCos3x3 &m);

	// Various matrix multiplication operations.

	inline Matrix3x3 operator*(const Matrix3x3 &m) const;
	inline Matrix3x3 &operator*=(const Matrix3x3 &m);
	inline Matrix3x3 operator*(const DirCos3x3 &m) const;
	inline Matrix3x3 &operator*=(const DirCos3x3 &m);

	// Pad out to be like a Graphics4x4 and multiply.
	inline Matrix4x4 operator*(const Matrix4x4 &m) const;
	inline Graphics4x4 operator*(const Graphics4x4 &m) const;
	inline Graphics4x4 operator*(const Cartesian4x4 &m) const;
	inline Graphics4x4 operator/(const Cartesian4x4 &m) const;

	// Multiply by the inverse (transpose) of a DirCos3x3.
	inline Matrix3x3 operator/(const DirCos3x3 &m) const;
	inline Matrix3x3 &operator/=(const DirCos3x3 &m);

	// These are odd...they just work on the upper 3x3 of a Graphics4x4
	inline Matrix3x3 TimesOrientation(const Graphics4x4 &m) const;
	inline Matrix3x3 &TimesEqualsOrientation(const Graphics4x4 &m);

	// Get a row (copies)
	inline Vector3 Row(int row) const;

	// Get a column (copies)
	inline Vector3 Column(int column) const;

	// Set a row or column
	inline void SetRow(int row, Vector3CRef data);
	inline void SetColumn(int column, Vector3CRef data);

	// Get/Set a row or column into a vector
	inline Vector3 GetRow0(void) const;
	inline Vector3 GetRow1(void) const;
	inline Vector3 GetRow2(void) const;
	inline Vector3 GetColumn0(void) const;
	inline Vector3 GetColumn1(void) const;
	inline Vector3 GetColumn2(void) const;
	inline void SetRow0(Vector3CRef v);	// These are odd...they just work on the upper 3x3 of a Graphics4x4
	inline void SetRow1(Vector3CRef v);
	inline void SetRow2(Vector3CRef v);
	inline void SetColumn0(Vector3CRef v);
	inline void SetColumn1(Vector3CRef v);
	inline void SetColumn2(Vector3CRef v);

	// The following treat the matrix as a 3D rot/scale/shear matrix.

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
	inline Matrix3x3 &BuildFromAxis(Vector3CRef i_primaryAxis, int i_primaryRow,
			int i_secondaryRow);

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

	// multiple rotations, does not cat, sets
	inline void SetXZYRotation(Vector3CRef v3Radians);

	// Cat's
	inline void CatScale(float scale);
	inline void CatScale(float xScale, float yScale, float zScale);
	inline void CatXScale(float scale);
	inline void CatYScale(float scale);
	inline void CatZScale(float scale);
	inline void CatXRotation(float radians);
	inline void CatYRotation(float radians);
	inline void CatZRotation(float radians);

	bool Deconstruct(DirCos3x3 &rot, float &scaleX, float &scaleY, float &scaleZ,
			float &shearXY, float &shearXZ, float &shearYZ) const;

#ifdef GCN
	inline void EndianSwap(void);
#endif

	// Debug aids
	inline bool IsNaN(void) const;
};

class DirCos3x3 : public Matrix3x3
{
public:
	// Constructors
	inline DirCos3x3(void) {}
	inline DirCos3x3(float e00, float e01, float e02,
							float e10, float e11, float e12,
							float e20, float e21, float e22);

	// Assignment
	inline DirCos3x3 &operator=(const DirCos3x3 &m);
	inline DirCos3x3 &operator=(const Cartesian4x4 &m);

	// Invert, always works. (it's the transpose)
	// Watch out, this function is not virtual
	bool Invert(Matrix3x3 &inverse) const;
	inline bool Invert(void);

	// everything else is overloaded operators
	inline int operator==(const Matrix3x3 &m) const;
	inline int operator==(const DirCos3x3 &m) const;
	inline int operator!=(const Matrix3x3 &m) const;
	inline int operator!=(const DirCos3x3 &m) const;

	// Memberwise scalar ops
	inline Matrix3x3 operator*(float s) const;
	inline Matrix3x3 operator/(float s) const;

	// Memberwise add and subtract
	inline Matrix3x3 operator+(const Matrix3x3 &m) const;
	inline Matrix3x3 operator+(const DirCos3x3 &m) const;
	inline Matrix3x3 operator-(const Matrix3x3 &m) const;
	inline Matrix3x3 operator-(const DirCos3x3 &m) const;

	// Various matrix multiplication operations.

	inline Matrix3x3 operator*(const Matrix3x3 &m) const;
	inline DirCos3x3 operator*(const DirCos3x3 &m) const;
	inline DirCos3x3 &operator*=(const DirCos3x3 &m);

	// Pad out to be like a Graphics4x4 and multiply.
	inline Matrix4x4 operator*(const Matrix4x4 &m) const;
	inline Graphics4x4 operator*(const Graphics4x4 &m) const;
	inline Cartesian4x4 operator*(const Cartesian4x4 &m) const;
	inline Cartesian4x4 operator/(const Cartesian4x4 &m) const;

	// Multiply by the inverse (transpose) of a DirCos3x3.
	inline DirCos3x3 operator/(const DirCos3x3 &m) const;
	inline DirCos3x3 &operator/=(const DirCos3x3 &m);

	// Invert then multiply
	inline Matrix4x4 InvertTimes(const Matrix4x4 &m) const;
	inline Graphics4x4 InvertTimes(const Graphics4x4 &m) const;
	inline Matrix3x3 InvertTimes(const Matrix3x3 &m) const;
	inline DirCos3x3 InvertTimes(const DirCos3x3 &m) const;
	inline DirCos3x3 &InvertTimesEquals(const DirCos3x3 &m);

	// Invert then multiply by invert
	inline Cartesian4x4 InvertDivide(const Cartesian4x4 &m) const;
	inline DirCos3x3 InvertDivide(const DirCos3x3 &m) const;
	inline DirCos3x3 &InvertDivideEquals(const DirCos3x3 &m);

	// Set operations
	inline void SetAngleAxis(float angle, Vector3CRef axis);

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
	inline DirCos3x3 &BuildFromAxis(Vector3CRef i_primaryAxis, int i_primaryRow,
			int i_secondaryRow);

	// Override scale operations to block them.
	inline void SetScale(float xScale, float yScale, float zScale);
	inline void SetXScale(float scale);
	inline void SetYScale(float scale);
	inline void SetZScale(float scale);
	inline void CatScale(float scale);
	inline void CatScale(float xScale, float yScale, float zScale);
	inline void CatXScale(float scale);
	inline void CatYScale(float scale);
	inline void CatZScale(float scale);

	// Orthonormalize this puppy
	void Orthonormalize(void);

	// Graphics Gems approach
	void Orthogonalize(void);

	// Make sure it's legal
	bool Validate(float epsilon = 1e-7f) const;

	// New and improved (with comments too)!
	bool BuildFromVector(Vector3CRef v, int vectorRow = 2, int alignRow = 1,
			int alignAxis = 1);
	bool BuildFromUnitVector(Vector3CRef v, int vectorRow = 2,
			int alignRow = 1, int alignAxis = 1);

	// Describe as an axis and angle
	void GetAngleAxis(float &angle, Vector3 &axis);

	// Whoever wrote these probably should have written some comments too.
	bool DecodeToYXZ(Vector3 &v) const;
	bool DecodeToXZY(Vector3 &v) const;
	bool DecodeToZYX(Vector3 &v) const;
};

#endif
