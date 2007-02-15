#ifndef __VECTOR3TEMPLATE_H__
#define __VECTOR3TEMPLATE_H__

class Matrix3x3;
class DirCos3x3;
class Matrix4x4;
class Graphics4x4;
class Cartesian4x4;
struct ts_Quatvw;

// TJC - this is a template to allow it to represent both the packed
// and padded versions of the vector3.  that way they both always have
// the same API.
template<class DATA>
class Vector3Template : public DATA
{
	// TJC - these simplify the definitions below.  They represent the
	// kind of return types for this particular template instance that
	// we should use for either a Vector3 or the opposite type of a
	// Vector3.
	typedef TYPENAME Vector3TemplateTypes<DATA, Vector3DataType>::Ret RetType;
	typedef TYPENAME Vector3TemplateTypes<DATA, Vector3OtherDataType>::Ret OtherRetType;
	typedef TYPENAME Vector3Template<DATA> BaseRetType;
	typedef TYPENAME VectorTemplateFloats<DATA>::Ret FloatRet;
	typedef TYPENAME VectorTemplateFloats<DATA>::Param FloatParam;
	typedef TYPENAME VectorTemplateFloats<DATA>::ClipParam FloatClipParam;

public:
	// Constructors (typedef magic results in a copy constructor)
	inline Vector3Template();
	inline Vector3Template(float* floatArray);
	inline Vector3Template(float x, float y, float z);
	inline Vector3Template(Vector4CRef v);
	inline Vector3Template(Vector3PackedDataCRef v);
	inline Vector3Template(Vector3PaddedDataCRef v);

	inline operator Vector3PackedData() const;
	inline operator Vector3PaddedData() const;

	// rotate one field left (x<=y<=z<=x)
	inline BaseRetType RotateLeft(void);
	// rotate one field right (x=>y=>z=>x)
	inline BaseRetType RotateRight(void);
	
	inline void Clear(void);
	inline void Set(float x, float y, float z);
	inline void Set(const float *xyzArray);
	inline void Set(Vector4CRef v);
	inline void Set(Vector3PackedDataCRef v);
	inline void Set(Vector3PaddedDataCRef v);

	// These allow you to only set certain fields in unusual ways...
	// there are a lot of them, so hold in there...
	inline void ClearXY(void);
	inline void ClearYZ(void);
	inline void ClearXZ(void);
	inline void XY(float x, float y);
	inline void YZ(float y, float z);
	inline void XZ(float x, float z);
	inline void XY(float val);
	inline void YZ(float val);
	inline void XZ(float val);
	inline void XY(Vector3PaddedDataCRef v);
	inline void YZ(Vector3PaddedDataCRef v);
	inline void XZ(Vector3PaddedDataCRef v);
	inline void XY(Vector3PackedDataCRef v);
	inline void YZ(Vector3PackedDataCRef v);
	inline void XZ(Vector3PackedDataCRef v);

//#if defined(PS2) && defined(SN)
//	inline void Set(Vector4X x, Vector4Y y, Vector4Z z);
//#endif

	// TJC - /////////////////////////////////////////////////////////////////////
	// TJC - for some of the functions below, you'll notice 2 versions
	// of the function.  one is passed a Vector3CRef, the other is
	// passed a Vector3OtherCRef.  I tried to unify these in all cases
	// using template functions, but that was WAY to confusing.
	// TJC - /////////////////////////////////////////////////////////////////////

	// Special functions
	inline FloatRet Dot(Vector3CRef v) const;
	inline FloatRet Dot(Vector3OtherCRef v) const;

	inline RetType Cross(Vector3CRef v) const;
	inline OtherRetType Cross(Vector3OtherCRef v) const;

	inline RetType Diag(Vector3CRef v) const;
	inline OtherRetType Diag(Vector3OtherCRef v) const;

	// returns (x() < 0.0f && y() < 0.0f && z() < 0.0f)
	inline bool AllXYZNegative() const;
	// returns (x() < 0.0f || y() < 0.0f || z() < 0.0f)
	inline bool AnyXYZNegative() const;
	// returns (x() >= 0.0f && y() >= 0.0f && z() >= 0.0f)
	inline bool AllXYZPositive() const;
	// returns (x() >= 0.0f || y() >= 0.0f || z() >= 0.0f)
	inline bool AnyXYZPositive() const;
	// returns (x() == 0.0f && y() == 0.0f && z() == 0.0f) with an epsilon
	inline bool AllXYZZero() const;
	// returns (x() == 0.0f || y() == 0.0f || z() == 0.0f) with an epsilon
	inline bool AnyXYZZero() const;

	// returns (x() <= w && y() <= w && z() <= w)
	inline bool AllXYZLessOrEqual(FloatClipParam w) const;
	// returns (x() > w && y() > w && z() > w)
	inline bool AllXYZGreater(FloatClipParam w) const;
	// returns (x() <= w || y() <= w || z() <= w)
	inline bool AnyXYZLessOrEqual(FloatClipParam w) const;
	// returns (x() > w || y() > w || z() > w)
	inline bool AnyXYZGreater(FloatClipParam w) const;

	// returns (x() <= v.x() && y() <= v.y() && z() <= v.z())
	inline bool AllXYZLessOrEqual(Vector3CRef v) const;
	// returns (x() > v.x() && y() > v.y() && z() > v.z())
	inline bool AllXYZGreater(Vector3CRef v) const;
	// returns (x() <= v.x() || y() <= v.y() || z() <= v.z())
	inline bool AnyXYZLessOrEqual(Vector3CRef v) const;
	// returns (x() > v.x() || y() > v.y() || z() > v.z())
	inline bool AnyXYZGreater(Vector3CRef v) const;

	// returns (x() <= v.x() && y() <= v.y() && z() <= v.z())
	inline bool AllXYZLessOrEqual(Vector3OtherCRef v) const;
	// returns (x() > v.x() && y() > v.y() && z() > v.z())
	inline bool AllXYZGreater(Vector3OtherCRef v) const;
	// returns (x() <= v.x() || y() <= v.y() || z() <= v.z())
	inline bool AnyXYZLessOrEqual(Vector3OtherCRef v) const;
	// returns (x() > v.x() || y() > v.y() || z() > v.z())
	inline bool AnyXYZGreater(Vector3OtherCRef v) const;

	inline FloatRet Length(void) const;
	inline FloatRet LengthXZ(void) const;
	inline FloatRet LengthSquared(void) const;
	inline FloatRet LengthXZSquared(void) const;
	inline FloatRet Normalize(void);
	inline bool SafeNormalize(void);
	inline bool ReallySafeNormalize(void);
	inline bool ReallySafeNormalize(float &mag);
	inline void Negate(void);
	inline BaseRetType Normalized(void) const;
	inline BaseRetType Negated(void) const;

	inline RetType Projection(Vector3CRef v) const;
	inline OtherRetType Projection(Vector3OtherCRef v) const;

	// Static versions
	inline static FloatRet Dot(Vector3CRef v1, Vector3CRef v2);
	inline static Vector3 Diag(Vector3CRef v1, Vector3CRef v2);
	inline static Vector3 Cross(Vector3CRef v1, Vector3CRef v2);

	inline static Vector3 Projection(Vector3CRef a, Vector3CRef b);

	inline static FloatRet LengthSquared(Vector3CRef v);
	inline static FloatRet LengthXZSquared(Vector3CRef v);
	inline static FloatRet Length(Vector3CRef v);
	inline static FloatRet LengthXZ(Vector3CRef v);

	// operators!!
	inline int operator==(Vector3CRef v) const;
	inline int operator!=(Vector3CRef v) const;
	inline BaseRetType operator=(Vector3PaddedDataCRef v);
	inline BaseRetType operator=(Vector3PackedDataCRef v);
	inline BaseRetType operator=(Vector4CRef v);
	inline BaseRetType operator=(float f);
	inline BaseRetType operator=(int i);

	// TJC - Vector3 & is bad.  so these return a copy of the vector.  Not quite
	// how you might expect them to behave, but close enough for most cases...
	inline RetType operator+=(Vector3CRef v);
	inline RetType operator-=(Vector3CRef v);
	inline OtherRetType operator+=(Vector3OtherCRef v);
	inline OtherRetType operator-=(Vector3OtherCRef v);
	inline BaseRetType operator*=(FloatParam scale);
	inline BaseRetType operator/=(FloatParam scale);

	inline RetType operator+(Vector3CRef v) const;
	inline RetType operator-(Vector3CRef v) const;
	inline OtherRetType operator+(Vector3OtherCRef v) const;
	inline OtherRetType operator-(Vector3OtherCRef v) const;
	inline BaseRetType operator*(FloatParam scale) const;
	inline BaseRetType operator/(FloatParam scale) const;

	inline BaseRetType operator-(void) const;

	// unusual broadcast add & subtracts [x-float, y-float, z-float]
	inline BaseRetType operator- (FloatParam scalar) const;
	inline BaseRetType operator+ (FloatParam scalar) const;
	inline BaseRetType operator-=(FloatParam scalar);
	inline BaseRetType operator+=(FloatParam scalar);

	// piecewise functions.
	inline RetType Min(Vector3CRef v) const;
	inline RetType Max(Vector3CRef v) const;
	inline OtherRetType Min(Vector3OtherCRef v) const;
	inline OtherRetType Max(Vector3OtherCRef v) const;
	inline BaseRetType Abs(void) const;
	inline FloatRet SumXYZ(void) const;
	inline FloatRet SumXZ(void) const;
	inline FloatRet SumYZ(void) const;
	inline FloatRet SumXY(void) const;

	// piecewise functions... static versions
	inline static Vector3 Min(Vector3CRef v1, Vector3CRef v2);
	inline static Vector3 Max(Vector3CRef v1, Vector3CRef v2);
	inline static Vector3 Abs(Vector3CRef v);
	inline static FloatRet SumXYZ(Vector3CRef v);
	inline static FloatRet SumXZ(Vector3CRef v);
	inline static FloatRet SumYZ(Vector3CRef v);
	inline static FloatRet SumXY(Vector3CRef v);

	//---------------------------------
	// vector/matrix interaction code
	//---------------------------------

	// multiply
	inline Vector3 operator*(const Matrix3x3 &m) const;

	// multiply by transpose
	inline Vector3 operator/(const DirCos3x3 &m) const;

	// Multiply, assuming w == 1 and v is [x y z 1]
	inline Vector3 operator*(const Graphics4x4 &m) const;

	// Multiply by inverse, assuming w == 1 and v is [x y z 1], only rotation and translation
	inline Vector3 operator/(const Cartesian4x4 &m) const;

	// Multiply by a unit quaternion, representing a rotation
	inline Vector3 operator*(const ts_Quatvw &q) const;

	// Multiply by the transpose of a unit quaternion
	inline Vector3 operator/(const ts_Quatvw &q) const;

	// Full graphics [x y z 1] multiply
	inline Vector4 Transform3(const Matrix4x4 &m) const;

	// Basis multiply (ignores translation, like making a 3x3)
	inline Vector3 BasisTransform(const Graphics4x4 &m) const;
	inline Vector3 InvBasisTransform(const Graphics4x4 &m) const;

	// fast multiply-- should always a platform-specific implementation. must handle this == v
	inline Vector3 FastMultiply(Vector3CRef v, const Graphics4x4 &m);	// this = v * m
	inline Vector3 FastBasisTransform(Vector3CRef v, const Graphics4x4 &m);	// this = v * m, no translation

	// Gamecube byte swapper
#ifdef GCN
	inline void EndianSwap(void);
#endif

	// Debug aids
	inline bool IsNaN(void) const;
	
};

#endif __VECTOR3TEMPLATE_H__
