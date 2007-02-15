
// get/set at the values

template<class DATA>
inline void Vector4Template<DATA>::Set(float _x, float _y, float _z, float _w)
{
	X(_x);
	Y(_y);
	Z(_z);
	W(_w);
}

template<class DATA>
inline void Vector4Template<DATA>::Set(const float *xyzwArray)
{
	Set(xyzwArray[0], xyzwArray[1], xyzwArray[2], xyzwArray[3]);
}

template<class DATA>
inline void Vector4Template<DATA>::Set(Vector3CRef v)
{
	Set(v.X(), v.Y(), v.Z());
}

template<class DATA>
inline void Vector4Template<DATA>::Set(Vector4PackedDataCRef v)
{
	Set(v.X(), v.Y(), v.Z(), v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::Set(Vector4AlignedDataCRef v)
{
	Set(v.X(), v.Y(), v.Z(), v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::ClearX(void)
{
	X(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearY(void)
{
	Y(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearZ(void)
{
	Z(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearW(void)
{
	W(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearXY(void)
{
	X(0.0f);
	Y(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearXZ(void)
{
	X(0.0f);
	Z(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearXW(void)
{
	X(0.0f);
	W(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearYZ(void)
{
	Y(0.0f);
	Z(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearYW(void)
{
	Y(0.0f);
	W(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearZW(void)
{
	Z(0.0f);
	W(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearXYW(void)
{
	X(0.0f);
	Y(0.0f);
	W(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearXZW(void)
{
	X(0.0f);
	Z(0.0f);
	W(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::ClearYZW(void)
{
	Y(0.0f);
	Z(0.0f);
	W(0.0f);
}

template<class DATA>
inline void Vector4Template<DATA>::XY(float x, float y)
{
	X(x);
	Y(y);
}

template<class DATA>
inline void Vector4Template<DATA>::XZ(float x, float z)
{
	X(x);
	Z(z);
}

template<class DATA>
inline void Vector4Template<DATA>::XW(float x, float w)
{
	X(x);
	W(w);
}

template<class DATA>
inline void Vector4Template<DATA>::YZ(float y, float z)
{
	Y(y);
	Z(z);
}

template<class DATA>
inline void Vector4Template<DATA>::YW(float y, float w)
{
	Y(y);
	W(w);
}

template<class DATA>
inline void Vector4Template<DATA>::ZW(float z, float w)
{
	Z(z);
	W(w);
}

template<class DATA>
inline void Vector4Template<DATA>::XYZ(float x, float y, float z)
{
	X(x);
	Y(y);
	Z(z);
}

template<class DATA>
inline void Vector4Template<DATA>::XZW(float x, float z, float w)
{
	X(x);
	Z(z);
	W(w);
}

template<class DATA>
inline void Vector4Template<DATA>::YZW(float y, float z, float w)
{
	Y(y);
	Z(z);
	W(w);
}

template<class DATA>
inline void Vector4Template<DATA>::XYW(float x, float y, float w)
{
	X(x);
	Y(y);
	W(w);
}

template<class DATA>
inline void Vector4Template<DATA>::XY(float val)
{
	X(val);
	Y(val);
}

template<class DATA>
inline void Vector4Template<DATA>::YZ(float val)
{
	Y(val);
	Z(val);
}

template<class DATA>
inline void Vector4Template<DATA>::XZ(float val)
{
	X(val);
	Z(val);
}

template<class DATA>
inline void Vector4Template<DATA>::XW(float val)
{
	X(val);
	W(val);
}

template<class DATA>
inline void Vector4Template<DATA>::YW(float val)
{
	Y(val);
	W(val);
}

template<class DATA>
inline void Vector4Template<DATA>::ZW(float val)
{
	Z(val);
	W(val);
}

template<class DATA>
inline void Vector4Template<DATA>::XYZ(float val)
{
	X(val);
	Y(val);
	Z(val);
}

template<class DATA>
inline void Vector4Template<DATA>::XYW(float val)
{
	X(val);
	Y(val);
	W(val);
}

template<class DATA>
inline void Vector4Template<DATA>::YZW(float val)
{
	Y(val);
	Z(val);
	W(val);
}

template<class DATA>
inline void Vector4Template<DATA>::XZW(float val)
{
	X(val);
	Z(val);
	W(val);
}

template<class DATA>
inline void Vector4Template<DATA>::XY(Vector4AlignedDataCRef v)
{
	X(v.X());
	Y(v.Y());
}

template<class DATA>
inline void Vector4Template<DATA>::YZ(Vector4AlignedDataCRef v)
{
	Y(v.Y());
	Z(v.Z());
}

template<class DATA>
inline void Vector4Template<DATA>::XZ(Vector4AlignedDataCRef v)
{
	X(v.X());
	Z(v.Z());
}

template<class DATA>
inline void Vector4Template<DATA>::XY(Vector4PackedDataCRef v)
{
	X(v.X());
	Y(v.Y());
}

template<class DATA>
inline void Vector4Template<DATA>::YZ(Vector4PackedDataCRef v)
{
	Y(v.Y());
	Z(v.Z());
}

template<class DATA>
inline void Vector4Template<DATA>::XZ(Vector4PackedDataCRef v)
{
	X(v.X());
	Z(v.Z());
}

template<class DATA>
inline void Vector4Template<DATA>::XYZ(Vector4AlignedDataCRef v)
{
	X(v.X());
	Y(v.Y());
	Z(v.Z());
}

template<class DATA>
inline void Vector4Template<DATA>::XYZ(Vector4PackedDataCRef v)
{
	X(v.X());
	Y(v.Y());
	Z(v.Z());
}

template<class DATA>
inline void Vector4Template<DATA>::XYW(Vector4AlignedDataCRef v)
{
	X(v.X());
	Y(v.Y());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::YZW(Vector4AlignedDataCRef v)
{
	Y(v.Y());
	Z(v.Z());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::XZW(Vector4AlignedDataCRef v)
{
	X(v.X());
	Z(v.Z());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::XYW(Vector4PackedDataCRef v)
{
	X(v.X());
	Y(v.Y());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::YZW(Vector4PackedDataCRef v)
{
	Y(v.Y());
	Z(v.Z());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::XZW(Vector4PackedDataCRef v)
{
	X(v.X());
	Z(v.Z());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::XW(Vector4AlignedDataCRef v)
{
	X(v.X());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::YW(Vector4AlignedDataCRef v)
{
	Y(v.Y());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::ZW(Vector4AlignedDataCRef v)
{
	Z(v.Z());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::XW(Vector4PackedDataCRef v)
{
	X(v.X());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::YW(Vector4PackedDataCRef v)
{
	Y(v.Y());
	W(v.W());
}

template<class DATA>
inline void Vector4Template<DATA>::ZW(Vector4PackedDataCRef v)
{
	Z(v.Z());
	W(v.W());
}


// Our various constructors
template<class DATA>
inline Vector4Template<DATA>::Vector4Template(void)
{
}

template<class DATA>
inline Vector4Template<DATA>::Vector4Template(float x, float y, float z, float w)
{
	Set(x,y,z,w);
}

template<class DATA>
inline Vector4Template<DATA>::Vector4Template(float* floatArray)
{
	Set(floatArray);
}

template<class DATA>
inline Vector4Template<DATA>::Vector4Template(Vector4PackedDataCRef v)
{
	Set(v);
}

template<class DATA>
inline Vector4Template<DATA>::Vector4Template(Vector4AlignedDataCRef v)
{
	Set(v);
}

template<class DATA>
inline Vector4Template<DATA>::Vector4Template(Vector3CRef v)
{
	Set(v);
}

template<class DATA>
inline Vector4Template<DATA>::operator Vector4PackedData() const
{
	return Vector4Packed(*this);
}

template<class DATA>
inline Vector4Template<DATA>::operator Vector4AlignedData() const
{
	return Vector4Aligned(*this);
}
	

// Special functions
template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::Dot(Vector4CRef v) const
{
	return Diag(v).SumXYZ();
}


// Special functions
template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::Dot(Vector4OtherCRef v) const
{
	return Diag(v).SumXYZ();
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::RetType Vector4Template<DATA>::Cross(Vector4CRef v) const
{
	RetType temp;
	temp.Set(
		Y() * v.Z() - v.Y() * Z(),
		-(X() * v.Z() - v.X() * Z()),
		X() * v.Y() - v.X() * Y());
	return temp;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::OtherRetType Vector4Template<DATA>::Cross(Vector4OtherCRef v) const
{
	OtherRetType temp;
	temp.Set(
		Y() * v.Z() - v.Y() * Z(),
		-(X() * v.Z() - v.X() * Z()),
		X() * v.Y() - v.X() * Y());
	return temp;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::RetType Vector4Template<DATA>::Diag(Vector4CRef v) const
{
	return RetType(
		X() * v.X(), 
		Y() * v.Y(),
		Z() * v.Z(),
		W() * v.W());
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::OtherRetType Vector4Template<DATA>::Diag(Vector4OtherCRef v) const
{
	return Vector4(
		X() * v.X(), 
		Y() * v.Y(),
		Z() * v.Z(),
		W() * v.W());
}

template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::LengthSquared(void) const
{
	return Dot(*this);
}

template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::Length(void) const
{
	return Math::Sqrt(LengthSquared());
}

template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::Normalize(void)
{
	FloatRet length = Length();

	operator/=(length);

	return length;
}

// rotate one field left (x<=y<=z<=w<=x)
template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::RotateLeft(void)
{
	return Vector4(Y(),Z(),W(),X());
}

// rotate one field right (x=>y=>z=>w=>x)
template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::RotateRight(void)
{
	return Vector4(W(),X(),Y(),Z());
}
	
template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::Normalized(void) const
{
	Vector4Template<DATA> v = *this;
	v.Normalize();
	return v;
}

template<class DATA>
inline void Vector4Template<DATA>::Negate(void)
{
	*this = Negated();
}

template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::Negated(void) const
{
	return -*this;
}

// Static versions
template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::Dot(Vector4CRef v1, Vector4CRef v2)
{
	return v1.Dot(v2);
}

template<class DATA>
inline Vector4 Vector4Template<DATA>::Cross(Vector4CRef v1, Vector4CRef v2)
{
	return v1.Cross(v2);
}

template<class DATA>
inline Vector4 Vector4Template<DATA>::Diag(Vector4CRef v1, Vector4CRef v2)
{
	return v1.Diag(v2);
}

template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::Length(Vector4CRef v)
{
	return v.Length();
}

template<class DATA>
inline Vector4 Vector4Template<DATA>::Min(Vector4CRef v1, Vector4CRef v2)
{
	return v1.Min(v2);
}

template<class DATA>
inline Vector4 Vector4Template<DATA>::Max(Vector4CRef v1, Vector4CRef v2)
{
	return v1.Max(v2);
}


// operators!!
template<class DATA>
inline int Vector4Template<DATA>::operator==(Vector4CRef v) const
{
	return(X() == v.X() && Y() == v.Y() && Z() == v.Z() && W() == v.W());
}

template<class DATA>
inline int Vector4Template<DATA>::operator!=(Vector4CRef v) const
{
	return !operator==(v);
}

template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::operator=(Vector4PackedDataCRef v)
{
	Set(v);
	return *this;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::operator=(Vector4AlignedDataCRef v)
{
	Set(v);
	return *this;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::operator=(Vector3CRef v)
{
	Set(v);
	return *this;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::operator=(float f)
{
	Set(f);
	return *this;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::operator=(int i)
{
	Set(i);
	return *this;
}


template<class DATA>
inline TYPENAME Vector4Template<DATA>::RetType Vector4Template<DATA>::operator+=(Vector4CRef v)
{
	Set(operator+(v));
	return *this;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::OtherRetType Vector4Template<DATA>::operator+=(Vector4OtherCRef v)
{
	Set(operator+(v));
	return *this;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::RetType Vector4Template<DATA>::operator-=(Vector4CRef v)
{
	Set(operator-(v));
	return *this;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::OtherRetType Vector4Template<DATA>::operator-=(Vector4OtherCRef v)
{
	Set(operator-(v));
	return *this;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::operator*=(FloatParam scale)
{
	Set(operator*(scale));
	return *this;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::operator/=(FloatParam scale)
{
	Set(operator/(scale));
	return *this;
}


template<class DATA>
inline TYPENAME Vector4Template<DATA>::RetType Vector4Template<DATA>::operator+(Vector4CRef v) const
{
	return RetType(
		X() + v.X(),
		Y() + v.Y(),
		Z() + v.Z(),
		W() + v.W());
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::RetType Vector4Template<DATA>::operator-(Vector4CRef v) const
{
	return RetType(
		X() - v.X(),
		Y() - v.Y(),
		Z() - v.Z(),
		W() - v.W());
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::OtherRetType Vector4Template<DATA>::operator+(Vector4OtherCRef v) const
{
	return OtherRetType(
		X() + v.X(),
		Y() + v.Y(),
		Z() + v.Z(),
		W() + v.W());
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::OtherRetType Vector4Template<DATA>::operator-(Vector4OtherCRef v) const
{
	return OtherRetType(
		X() - v.X(),
		Y() - v.Y(),
		Z() - v.Z(),
		W() - v.W());
}

template<class DATA>
inline Vector4Template<DATA> Vector4Template<DATA>::operator- (FloatParam scalar) const
{
	return Vector4Template<DATA>(
		X() - scalar,
		Y() - scalar,
		Z() - scalar,
		W() - scalar);
}

template<class DATA>
inline Vector4Template<DATA> Vector4Template<DATA>::operator+ (FloatParam scalar) const
{
	return Vector4Template<DATA>(
		X() + scalar,
		Y() + scalar,
		Z() + scalar,
		W() + scalar);
}

template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::operator*(FloatParam scale) const
{
	return Vector4(
		X() * scale,
		Y() * scale,
		Z() * scale,
		W() * scale);
}

template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::operator/(FloatParam scale) const
{
	scale = 1.0f / scale;

	return Vector4(
		X() * scale,
		Y() * scale,
		Z() * scale,
		W() * scale);
}


template<class DATA>
inline TYPENAME Vector4Template<DATA> Vector4Template<DATA>::operator-(void) const
{
	return BaseRetType(-X(), -Y(), -Z(), -W());
}


//---------------------------------
// vector/matrix interaction code
//---------------------------------

// Multiply, assuming w == 1 and v is [x y z 1]
template<class DATA>
inline Vector4 Vector4Template<DATA>::operator*(const Graphics4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	return Vector4(
		X() * m.r00 + Y() * m.r10 + Z() * m.r20 + m.tx,
		X() * m.r01 + Y() * m.r11 + Z() * m.r21 + m.ty,
		X() * m.r02 + Y() * m.r12 + Z() * m.r22 + m.tz);
}


// Full multiply
template<class DATA>
inline Vector4 Vector4Template<DATA>::Transform(const Matrix4x4 &m) const
{
	return Vector4(
		X() * m.e00 + Y() * m.e10 + Z() * m.e20 + W() * m.e30,
		X() * m.e01 + Y() * m.e11 + Z() * m.e21 + W() * m.e31,
		X() * m.e02 + Y() * m.e12 + Z() * m.e22 + W() * m.e32,
		X() * m.e03 + Y() * m.e13 + Z() * m.e23 + W() * m.e33);
}


// Basis multiply (ignores translation, like making a 3x3)
template<class DATA>
inline Vector4 Vector4Template<DATA>::BasisTransform(const Graphics4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	return Vector4(
		X() * m.r00 + Y() * m.r10 + Z() * m.r20,
		X() * m.r01 + Y() * m.r11 + Z() * m.r21,
		X() * m.r02 + Y() * m.r12 + Z() * m.r22);
}

template<class DATA>
inline Vector4 Vector4Template<DATA>::InvBasisTransform(const Graphics4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	return Vector4(
		X() * m.r00 + Y() * m.r01 + Z() * m.r02,
		X() * m.r10 + Y() * m.r11 + Z() * m.r12,
		X() * m.r20 + Y() * m.r21 + Z() * m.r22);
}


// fast multiply-- should always a platform-specific implementation. must handle this == v
template<class DATA>
inline Vector4 Vector4Template<DATA>::FastMultiply(Vector4CRef v, const Graphics4x4 &m)
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Vector4 product;

	product.Set(v.X() * m.r00 + v.Y() * m.r10 + v.Z() * m.r20 + m.tx,
					v.X() * m.r01 + v.Y() * m.r11 + v.Z() * m.r21 + m.ty,
					v.X() * m.r02 + v.Y() * m.r12 + v.Z() * m.r22 + m.tz);

	// copy it in
	*this = product;
	return *this;

}

template<class DATA>
inline Vector4 Vector4Template<DATA>::FastTransform(Vector4CRef v, const Matrix4x4 &m)
{
	Vector4 product;

	product.Set(v.X() * m.e00 + v.Y() * m.e10 + v.Z() * m.e20 + v.W() * m.e30,
					v.X() * m.e01 + v.Y() * m.e11 + v.Z() * m.e21 + v.W() * m.e31,
					v.X() * m.e02 + v.Y() * m.e12 + v.Z() * m.e22 + v.W() * m.e32,
					v.X() * m.e03 + v.Y() * m.e13 + v.Z() * m.e23 + v.W() * m.e33);

	// copy it in
	*this = product;
	return *this;
}

template<class DATA>
inline Vector4 Vector4Template<DATA>::FastBasisTransform(Vector4CRef v, const Graphics4x4 &m)
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Vector4 product;

	product.Set(v.X() * m.r00 + v.Y() * m.r10 + v.Z() * m.r20,
					v.X() * m.r01 + v.Y() * m.r11 + v.Z() * m.r21,
					v.X() * m.r02 + v.Y() * m.r12 + v.Z() * m.r22);

	// copy it in
	*this = product;
	return *this;
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::RetType Vector4Template<DATA>::Min(Vector4CRef v) const
{
	return Vector4Template<DATA>::RetType(Math::Min(v.X(), X()), Math::Min(v.Y(), Y()), Math::Min(v.Z(), Z()), Math::Min(v.W(), W()));
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::RetType Vector4Template<DATA>::Max(Vector4CRef v) const
{
	return Vector4Template<DATA>::RetType(Math::Max(v.X(), X()), Math::Max(v.Y(), Y()), Math::Max(v.Z(), Z()), Math::Max(v.W(), W()));
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::OtherRetType Vector4Template<DATA>::Min(Vector4OtherCRef v) const
{
	return Vector4Template<DATA>::RetType(Math::Min(v.X(), X()), Math::Min(v.Y(), Y()), Math::Min(v.Z(), Z()), Math::Min(v.W(), W()));
}

template<class DATA>
inline TYPENAME Vector4Template<DATA>::OtherRetType Vector4Template<DATA>::Max(Vector4OtherCRef v) const
{
	return Vector4Template<DATA>::RetType(Math::Max(v.X(), X()), Math::Max(v.Y(), Y()), Math::Max(v.Z(), Z()), Math::Max(v.W(), W()));
}

template<class DATA>
inline Vector4Template<DATA> Vector4Template<DATA>::Abs(void) const
{
	return Vector4Template<DATA>(Math::Abs(X()), Math::Abs(Y()), Math::Abs(Z()), Math::Abs(W()));
}

template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::SumXYZW(void) const
{
	return X() + Y() + Z() + W();
}

template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::SumXYZ(void) const
{
	return X() + Y() + Z();
}

template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::SumXZ(void) const
{
	return X() + Z();
}

template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::SumYZ(void) const
{
	return Y() + Z();
}

template<class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector4Template<DATA>::SumXY(void) const
{
	return X() + Y();
}


#ifdef GCN
template<class DATA>
inline void Vector4Template<DATA>::EndianSwap(void)
{
	float t = X(); ByteSwap(t); X(t);
	float t = Y(); ByteSwap(t); Y(t);
	float t = Z(); ByteSwap(t); Z(t);
	float t = W(); ByteSwap(t); W(t);
}

#endif


template<class DATA>
inline bool Vector4Template<DATA>::IsNaN(void) const
{
	return 
		Math::IsNaN(X()) ||
		Math::IsNaN(Y()) ||
		Math::IsNaN(Z()) ||
		Math::IsNaN(W());
}

