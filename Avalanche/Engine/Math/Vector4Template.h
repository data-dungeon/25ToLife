
class Matrix3x3;
class DirCos3x3;
class Matrix4x4;
class Graphics4x4;
struct ts_Quatvw;

template<class DATA>
class Vector4Template : public DATA
{
	// TJC - these simplify the definitions below.  They represent the
	// kind of return types for this particular template instance that
	// we should use for either a Vector3 or the opposite type of a
	// Vector3.
	typedef TYPENAME Vector4TemplateTypes<DATA, Vector4DataType>::Ret RetType;
	typedef TYPENAME Vector4TemplateTypes<DATA, Vector4OtherDataType>::Ret OtherRetType;
	typedef TYPENAME Vector4Template<DATA> BaseRetType;
	typedef TYPENAME VectorTemplateFloats<DATA>::Ret FloatRet;
	typedef TYPENAME VectorTemplateFloats<DATA>::Param FloatParam;

public:
	// Our various constructors
	inline Vector4Template(void);
	inline Vector4Template(float x, float y, float z, float w = 1.0f);
	inline Vector4Template(float* floatArray);
	inline Vector4Template(Vector3CRef v);
	inline Vector4Template(Vector4PackedDataCRef v);
	inline Vector4Template(Vector4AlignedDataCRef v);

	inline operator Vector4PackedData() const;
	inline operator Vector4AlignedData() const;

	
	// rotate one field left (x<=y<=z<=w<=x)
	inline BaseRetType RotateLeft(void);
	// rotate one field right (x=>y=>z=>w=>x)
	inline BaseRetType RotateRight(void);
	
	// get/set at the values
	inline void Clear(void);
	inline void Set(float x, float y, float z, float w = 1.0f);
	inline void Set(const float *xyzwArray);
	inline void Set(Vector3CRef v);
	inline void Set(Vector4PackedDataCRef v);
	inline void Set(Vector4AlignedDataCRef v);

	// These allow you to only set certain fields in unusual ways...
	// there are a lot of them, so hold in there...
	inline void ClearX(void);
	inline void ClearY(void);
	inline void ClearZ(void);
	inline void ClearW(void);
	inline void ClearXY(void);
	inline void ClearYZ(void);
	inline void ClearXZ(void);
	inline void ClearXW(void);
	inline void ClearYW(void);
	inline void ClearZW(void);
	inline void ClearXYW(void);
	inline void ClearYZW(void);
	inline void ClearXZW(void);
	inline void XY(float x, float y);
	inline void YZ(float y, float z);
	inline void XZ(float x, float z);
	inline void XW(float x, float y);
	inline void YW(float y, float z);
	inline void ZW(float x, float z);
	inline void XYZ(float x, float y, float z);
	inline void XYW(float x, float y, float w);
	inline void YZW(float y, float z, float w);
	inline void XZW(float x, float z, float w);
	inline void XY(float val);
	inline void YZ(float val);
	inline void XZ(float val);
	inline void XW(float val);
	inline void YW(float val);
	inline void ZW(float val);
	inline void XYZ(float val);
	inline void XYW(float val);
	inline void YZW(float val);
	inline void XZW(float val);
	inline void XY(Vector4AlignedDataCRef v);
	inline void YZ(Vector4AlignedDataCRef v);
	inline void XZ(Vector4AlignedDataCRef v);
	inline void XY(Vector4PackedDataCRef v);
	inline void YZ(Vector4PackedDataCRef v);
	inline void XZ(Vector4PackedDataCRef v);
	inline void XYZ(Vector4AlignedDataCRef v);
	inline void XYZ(Vector4PackedDataCRef v);
	inline void XYW(Vector4AlignedDataCRef v);
	inline void YZW(Vector4AlignedDataCRef v);
	inline void XZW(Vector4AlignedDataCRef v);
	inline void XYW(Vector4PackedDataCRef v);
	inline void YZW(Vector4PackedDataCRef v);
	inline void XZW(Vector4PackedDataCRef v);
	inline void XW(Vector4AlignedDataCRef v);
	inline void YW(Vector4AlignedDataCRef v);
	inline void ZW(Vector4AlignedDataCRef v);
	inline void XW(Vector4PackedDataCRef v);
	inline void YW(Vector4PackedDataCRef v);
	inline void ZW(Vector4PackedDataCRef v);

	// Special functions
	inline FloatRet Dot(Vector4CRef v) const;
	inline FloatRet Dot(Vector4OtherCRef v) const;

	inline RetType Cross(Vector4CRef v) const;
	inline OtherRetType Cross(Vector4OtherCRef v) const;

	inline RetType Diag(Vector4CRef v) const;
	inline OtherRetType Diag(Vector4OtherCRef v) const;

	inline FloatRet Length(void) const;
	inline FloatRet LengthXZ(void) const;
	inline FloatRet LengthSquared(void) const;
	inline FloatRet LengthXZSquared(void) const;
	inline FloatRet Normalize(void);
	inline bool SafeNormalize(void);
	inline void Negate(void);
	inline BaseRetType Negated(void) const;
	inline BaseRetType Normalized(void) const;

	// Static versions
	inline static FloatRet Dot(Vector4CRef v1, Vector4CRef v2);
	inline static Vector4 Cross(Vector4CRef v1, Vector4CRef v2);
	inline static Vector4 Diag(Vector4CRef v1, Vector4CRef v2);
	inline static FloatRet Length(Vector4CRef v);
	inline static Vector4 Min(Vector4CRef v1, Vector4CRef v2);
	inline static Vector4 Max(Vector4CRef v1, Vector4CRef v2);

	// operators!!
   inline int operator==(Vector4CRef v) const;
   inline int operator!=(Vector4CRef v) const;
	inline BaseRetType operator=(Vector4PackedDataCRef v);
	inline BaseRetType operator=(Vector4AlignedDataCRef v);
	inline BaseRetType operator=(Vector3CRef v);
	inline BaseRetType operator=(float f);
	inline BaseRetType operator=(int i);

   inline RetType operator+=(Vector4CRef v);
   inline RetType operator-=(Vector4CRef v);
   inline OtherRetType operator+=(Vector4OtherCRef v);
   inline OtherRetType operator-=(Vector4OtherCRef v);
   inline BaseRetType operator*=(FloatParam scale);
   inline BaseRetType operator/=(FloatParam scale);

   inline RetType operator+(Vector4CRef v) const;
   inline RetType operator-(Vector4CRef v) const;
   inline OtherRetType operator+(Vector4OtherCRef v) const;
   inline OtherRetType operator-(Vector4OtherCRef v) const;
   inline BaseRetType operator*(FloatParam scale) const;
   inline BaseRetType operator/(FloatParam scale) const;

   inline BaseRetType operator-(void) const;

	// unusual broadcast add & subtracts [x-float, y-float, z-float]
	inline BaseRetType operator- (FloatParam scalar) const;
	inline BaseRetType operator+ (FloatParam scalar) const;
	inline BaseRetType operator-=(FloatParam scalar);
	inline BaseRetType operator+=(FloatParam scalar);

	// piecewise functions.
	inline RetType Min(Vector4CRef v) const;
	inline RetType Max(Vector4CRef v) const;
	inline OtherRetType Min(Vector4OtherCRef v) const;
	inline OtherRetType Max(Vector4OtherCRef v) const;
	inline BaseRetType Abs(void) const;
	inline FloatRet SumXYZW(void) const;
	inline FloatRet SumXYZ(void) const;
	inline FloatRet SumXZ(void) const;
	inline FloatRet SumYZ(void) const;
	inline FloatRet SumXY(void) const;
	//---------------------------------
	// vector/matrix interaction code
	//---------------------------------

	// Multiply, assuming w == 1 and v is [x y z 1]
	inline Vector4 operator*(const Graphics4x4 &m) const;

	// Full multiply
	inline Vector4 Transform(const Matrix4x4 &m) const;

	// Basis multiply (ignores translation, like making a 3x3)
	inline Vector4 BasisTransform(const Graphics4x4 &m) const;
	inline Vector4 InvBasisTransform(const Graphics4x4 &m) const;

	// fast multiply-- should always a platform-specific implementation. must handle this == v
	inline Vector4 FastMultiply(Vector4CRef v, const Graphics4x4 &m);	// this = v * m, assume w = 1
	inline Vector4 FastTransform(Vector4CRef v, const Matrix4x4 &m);	// this = v * m, full multiply
	inline Vector4 FastBasisTransform(Vector4CRef v, const Graphics4x4 &m);	// this = v * m, no translation

#ifdef GCN
	inline void EndianSwap(void);
#endif
	
	// Debug aids
	inline bool IsNaN(void) const;
	
};
