//
// Vector3Template
// 
// This is all the generic version of the Vector3 methods.
// All but X(),Y(),Z() must not assume anything about the
// data they access.  So they must all use X(),Y(),Z() to
// access their data members.  No exceptions.
//
// You should try to define new functions in terms of old
// functions because on some platforms, the old functions
// might be optimized and you can piggy back on that
// optimization by using it.
//

#include "Math/MathClass.h"

	// get/set at the values
template <class DATA>
inline void Vector3Template<DATA>::Set(float _x, float _y, float _z)
{
	X(_x);
	Y(_y);
	Z(_z);
}

template <class DATA>
inline void Vector3Template<DATA>::Set(const float *xyzArray)
{
	Set(xyzArray[0], xyzArray[1], xyzArray[2]);
}

template <class DATA>
inline void Vector3Template<DATA>::Set(Vector4CRef v)
{
	Set(v.X(), v.Y(), v.Z());
}

template <class DATA>
inline void Vector3Template<DATA>::Clear(void)
{
	Set(0.0f, 0.0f, 0.0f);
}

template<class DATA>
inline void Vector3Template<DATA>::Set(Vector3PackedDataCRef v)
{
	Set(v.X(), v.Y(), v.Z());
}

template<class DATA>
inline void Vector3Template<DATA>::Set(Vector3PaddedDataCRef v)
{
	Set(v.X(), v.Y(), v.Z());
}

template <class DATA>
inline Vector3Template<DATA>::Vector3Template(Vector3PackedDataCRef v)
{
	Set(v);
}

template <class DATA>
inline Vector3Template<DATA>::Vector3Template(Vector3PaddedDataCRef v)
{
	Set(v);
}

template <class DATA>
inline Vector3Template<DATA>::Vector3Template()
{
}

template <class DATA>
inline Vector3Template<DATA>::Vector3Template(float* floatArray)
{
	Set(floatArray);
}

template <class DATA>
inline Vector3Template<DATA>::Vector3Template(Vector4CRef v)
{
	Set(v);
}

template <class DATA>
inline Vector3Template<DATA>::Vector3Template(float x, float y, float z)
{
	Set(x,y,z);
}

template <class DATA>
inline Vector3Template<DATA>::operator Vector3PackedData() const
{
	return Vector3Packed(*this);
}

template <class DATA>
inline Vector3Template<DATA>::operator Vector3PaddedData() const
{
	return Vector3Padded(*this);
}
	


// Special functions

// rotate one field left (x<=y<=z<=w<=x)
template<class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::RotateLeft(void)
{
	float temp = X();
	x(Y());
	y(Z());
	z(temp);
}

// rotate one field right (x=>y=>z=>w=>x)
template<class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::RotateRight(void)
{
	float temp = X();
	x(Z());
	z(Y());
	y(temp);
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::Dot(Vector3CRef v) const
{
	return Diag(v).SumXYZ();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::Dot(Vector3OtherCRef v) const
{
	return Diag(v).SumXYZ();
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::RetType Vector3Template<DATA>::Cross(Vector3CRef v) const
{
	RetType temp;
	temp.Set(
		Y() * v.Z() - v.Y() * Z(),
		-(X() * v.Z() - v.X() * Z()),
		X() * v.Y() - v.X() * Y());
	return(temp);
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::OtherRetType Vector3Template<DATA>::Cross(Vector3OtherCRef v) const
{
	OtherRetType temp;
	temp.Set(
		Y() * v.Z() - v.Y() * Z(),
		-(X() * v.Z() - v.X() * Z()),
		X() * v.Y() - v.X() * Y());
	return(temp);
}


template <class DATA>
inline TYPENAME Vector3Template<DATA>::RetType Vector3Template<DATA>::Diag(Vector3CRef v) const
{
	float tx = X() * v.X();
	float ty = Y() * v.Y();
	float tz = Z() * v.Z();
	return RetType(tx, ty, tz);
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::OtherRetType Vector3Template<DATA>::Diag(Vector3OtherCRef v) const
{
	float tx = X() * v.X();
	float ty = Y() * v.Y();
	float tz = Z() * v.Z();
	return OtherRetType(tx, ty, tz);
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::RetType Vector3Template<DATA>::Projection(Vector3CRef v) const
{
	return(*this * Dot(v) / LengthSquared());
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::OtherRetType Vector3Template<DATA>::Projection(Vector3OtherCRef v) const
{
	return(*this * Dot(v) / LengthSquared());
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::SumXYZ(void) const
{
	return X() + Y() + Z();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::SumXZ(void) const
{
	return X() + Z();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::SumYZ(void) const
{
	return Y() + Z();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::SumXY(void) const
{
	return X() + Y();
}

template <class DATA>
inline bool Vector3Template<DATA>::AllXYZNegative() const
{
	return (X() < 0.0f) && (Y() < 0.0f) && (Z() < 0.0f);
}

template <class DATA>
inline bool Vector3Template<DATA>::AnyXYZNegative() const
{
	return (X() < 0.0f) || (Y() < 0.0f) || (Z() < 0.0f);
}

template <class DATA>
inline bool Vector3Template<DATA>::AllXYZPositive() const
{
	return (X() >= 0.0f) && (Y() >= 0.0f) && (Z() >= 0.0f);
}

template <class DATA>
inline bool Vector3Template<DATA>::AnyXYZPositive() const
{
	return (X() >= 0.0f) || (Y() >= 0.0f) || (Z() >= 0.0f);
}

template <class DATA>
inline bool Vector3Template<DATA>::AllXYZZero() const
{
	return Math::Equal(X(),0.0f) && Math::Equal(Y(), 0.0f) && Math::Equal(Z(), 0.0f);
}

template <class DATA>
inline bool Vector3Template<DATA>::AnyXYZZero() const
{
	return Math::Equal(X(),0.0f) || Math::Equal(Y(), 0.0f) || Math::Equal(Z(), 0.0f);
}


template <class DATA>
inline bool Vector3Template<DATA>::AllXYZLessOrEqual(FloatClipParam w) const
{
	return (X() <= w) && (Y() <= w) && (Z() <= w);
}

template <class DATA>
inline bool Vector3Template<DATA>::AllXYZGreater(FloatClipParam w) const
{
	return (X() > w) && (Y() > w) && (Z() > w);
}

// returns (X() <= w || Y() <= w || Z() <= w)
template <class DATA>
inline bool Vector3Template<DATA>::AnyXYZLessOrEqual(FloatClipParam w) const
{
	return (X() <= w) || (Y() <= w) || (Z() <= w);
}

// returns (X() > w || Y() > w || Z() > w)
template <class DATA>
inline bool Vector3Template<DATA>::AnyXYZGreater(FloatClipParam w) const
{
	return (X() > w) || (Y() > w) || (Z() > w);
}

// returns (X() <= v.X() && Y() <= v.Y() && Z() <= v.Z())
template <class DATA>
inline bool Vector3Template<DATA>::AllXYZLessOrEqual(Vector3CRef v) const
{
	return (X() <= v.X()) && (Y() <= v.Y()) && (Z() <= v.Z());
}

// returns (X() > v.X() && Y() > v.Y() && Z() > v.Z())
template <class DATA>
inline bool Vector3Template<DATA>::AllXYZGreater(Vector3CRef v) const
{
	return (X() > v.X()) && (Y() > v.Y()) && (Z() > v.Z());
}

// returns (X() <= v.X() || Y() <= v.Y() || Z() <= v.Z())
template <class DATA>
inline bool Vector3Template<DATA>::AnyXYZLessOrEqual(Vector3CRef v) const
{
	return (X() <= v.X()) || (Y() <= v.Y()) || (Z() <= v.Z());
}

// returns (X() > v.X() || Y() > v.Y() || Z() > v.Z())
template <class DATA>
inline bool Vector3Template<DATA>::AnyXYZGreater(Vector3CRef v) const
{
	return (X() > v.X()) || (Y() > v.Y()) || (Z() > v.Z());
}

// returns (X() <= v.X() && Y() <= v.Y() && Z() <= v.Z())
template <class DATA>
inline bool Vector3Template<DATA>::AllXYZLessOrEqual(Vector3OtherCRef v) const
{
	return (X() <= v.X()) && (Y() <= v.Y()) && (Z() <= v.Z());
}

// returns (X() > v.X() && Y() > v.Y() && Z() > v.Z())
template <class DATA>
inline bool Vector3Template<DATA>::AllXYZGreater(Vector3OtherCRef v) const
{
	return (X() > v.X()) && (Y() > v.Y()) && (Z() > v.Z());
}

// returns (X() <= v.X() || Y() <= v.Y() || Z() <= v.Z())
template <class DATA>
inline bool Vector3Template<DATA>::AnyXYZLessOrEqual(Vector3OtherCRef v) const
{
	return (X() <= v.X()) || (Y() <= v.Y()) || (Z() <= v.Z());
}

// returns (X() > v.X() || Y() > v.Y() || Z() > v.Z())
template <class DATA>
inline bool Vector3Template<DATA>::AnyXYZGreater(Vector3OtherCRef v) const
{
	return (X() > v.X()) || (Y() > v.Y()) || (Z() > v.Z());
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::LengthSquared(void) const
{
	return Diag(*this).SumXYZ();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::LengthXZSquared(void) const
{
	return (X() * X() +  Z() * Z());
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::Length(void) const
{
	return(Math::Sqrt(LengthSquared()));
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::LengthXZ(void) const
{
	return(Math::Sqrt(LengthXZSquared()));
}

// Static versions
template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::Dot(Vector3CRef v1, Vector3CRef v2)
{
	return v1.Dot(v2);
}

template <class DATA>
inline Vector3 Vector3Template<DATA>::Cross(Vector3CRef v1, Vector3CRef v2)
{
	return v1.Cross(v2);
}

template <class DATA>
inline Vector3 Vector3Template<DATA>::Diag(Vector3CRef v1, Vector3CRef v2)
{
	return v1.Diag(v2);
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::Length(Vector3CRef v)
{
	return v.Length();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::LengthXZ(Vector3CRef v)
{
	return v.LengthXZ();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::LengthSquared(Vector3CRef v)
{
	return v.LengthSquared();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::LengthXZSquared(Vector3CRef v)
{
	return v.LengthXZSquared();
}

template <class DATA>
inline Vector3 Vector3Template<DATA>::Projection(Vector3CRef a, Vector3CRef b)
{
	return a.Projection(b);
}


// operators!!
template <class DATA>
inline int Vector3Template<DATA>::operator==(Vector3CRef v) const
{
	return(X() == v.X() && Y() == v.Y() && Z() == v.Z());
}

template <class DATA>
inline int Vector3Template<DATA>::operator!=(Vector3CRef v) const
{
	return !operator==(v);
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator=(Vector3PaddedDataCRef v)
{
	Set(v);
	return *this;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator=(Vector3PackedDataCRef v)
{
	Set(v);
	return *this;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator=(Vector4CRef v)
{
	Set(v);
	return *this;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator=(float f)
{
	Set(f, f, f);
	return *this;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator=(int i)
{
	float temp = i;
	Set(temp, temp, temp);
	return *this;
}

template <class DATA>
inline void Vector3Template<DATA>::XY(float _x, float _y)
{
	X(_x);
	Y(_y);
}

template <class DATA>
inline void YZ(float _y, float _z)
{
	Y(_y);
	Z(_z);
}

template <class DATA>
inline void XZ(float _x, float _z)
{
	X(_x);
	Z(_z);
}

template <class DATA>
inline void _XY(float _val)
{
	X(_x);
	Y(_y);
}

template <class DATA>
inline void _YZ(float _val)
{
	Y(_y);
	Z(_z);
}

template <class DATA>
inline void _XZ(float _val)
{
	X(_x);
	Z(_z);
}

template <class DATA>
inline void _XY(Vector3PaddedDataCRef v)
{
	X(v.X());
	Y(v.Y());
}

template <class DATA>
inline void _YZ(Vector3PaddedDataCRef v)
{
	Y(v.Y());
	Z(v.Z());
}

template <class DATA>
inline void _XZ(Vector3PaddedDataCRef v)
{
	X(v.X());
	Z(v.Z());
}

template <class DATA>
inline void _XY(Vector3PackedDataCRef v)
{
	X(v.X());
	Y(v.Y());
}

template <class DATA>
inline void _YZ(Vector3PackedDataCRef v)
{
	Y(v.Y());
	Z(v.Z());
}

template <class DATA>
inline void _XZ(Vector3PackedDataCRef v)
{
	X(v.X());
	Z(v.Z());
}


// TJC - Vector3 & is bad.  so these return a copy of the vector.  Not quite
// how you might expect them to behave, but close enough for most cases...

template <class DATA>
inline TYPENAME Vector3Template<DATA>::RetType Vector3Template<DATA>::operator+(Vector3CRef v) const
{
	return RetType(X() + v.X(), Y() + v.Y(), Z() + v.Z());
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::RetType Vector3Template<DATA>::operator-(Vector3CRef v) const
{
	return RetType(X() - v.X(), Y() - v.Y(), Z() - v.Z());
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::OtherRetType Vector3Template<DATA>::operator+(Vector3OtherCRef v) const
{
	return OtherRetType(X() + v.X(), Y() + v.Y(), Z() + v.Z());
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::OtherRetType Vector3Template<DATA>::operator-(Vector3OtherCRef v) const
{
	return OtherRetType(X() - v.X(), Y() - v.Y(), Z() - v.Z());
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator*(FloatParam scale) const
{
	return BaseRetType(X() * scale, Y() * scale, Z() * scale);
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator/(FloatParam scale) const
{
	ASSERT(!Math::DivByZero(1.0f, scale));
	scale = 1.0f / scale;
	return *this * scale;
}


template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator-(void) const
{
	return BaseRetType(-X(), -Y(), -Z());
}


// unusual broadcast add & subtracts [x-float, y-float, z-float]
template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator-(FloatParam scalar) const
{
	return BaseRetType(X() - scalar, Y() - scalar, Z() - scalar);
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator+(FloatParam scalar) const
{
	return BaseRetType(X() + scalar, Y() + scalar, Z() + scalar);
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator-=(FloatParam scalar)
{
	return *this = *this - scalar;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator+=(FloatParam scalar)
{
	return *this = *this + scalar;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::RetType Vector3Template<DATA>::operator+=(Vector3CRef v)
{
	RetType retval = *this + v;
	*this = retval;
	return retval;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::RetType Vector3Template<DATA>::operator-=(Vector3CRef v)
{
	RetType retval = *this - v;
	*this = retval;
	return retval;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::OtherRetType Vector3Template<DATA>::operator+=(Vector3OtherCRef v)
{
	OtherRetType retval = *this + v;
	*this = retval;
	return retval;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::OtherRetType Vector3Template<DATA>::operator-=(Vector3OtherCRef v)
{
	OtherRetType retval = *this - v;
	*this = retval;
	return retval;
}


template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator*=(FloatParam scale)
{
	*this = *this * scale;
	return *this;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::operator/=(FloatParam scale)
{
	*this = *this / scale;
	return *this;
}

template <class DATA>
inline bool Vector3Template<DATA>::SafeNormalize(void)
{
	FloatRet length = Length();

	if (Math::DivByZero(1.0f, length))
		return(false);

	operator/=(length);

	return(true);
}

#define EXP_SHIFT 23
#define EXP_MASK  (0xFF << EXP_SHIFT)
#define EXP_BIAS  (0x7F << EXP_SHIFT)
#define EXP_MAX   (0xBF << EXP_SHIFT)
#define EXP_MIN   (0x40 << EXP_SHIFT)

template <class DATA>
inline bool Vector3Template<DATA>::ReallySafeNormalize(void)
{
	float mag;
	return(ReallySafeNormalize(mag));
}

template <class DATA>
inline bool Vector3Template<DATA>::ReallySafeNormalize(float &mag)
{
	// Since this operation squares the components of the vector, perfectly valid
	// values can yield incorrect results if squaring results in an overflow or
	// underflow. This routine detects if the components are outside the allowed
	// range and scales them if necessary. Scaling is performed by changing the
	// floating point exponent so the scaling is perfect.
	//
	// While simple in concept, this implementation seems awkward in execution.
	// I invite anyone who thinks they can make it better to give it a shot. In
	// particular, platform specific solutions could operate directly on the
	// data rather than copying it out.

	// Copy the components into real life floats. Put them in a union so the bits
	// can be played with.
	union
	{
		float num;
		unsigned int bits;
	} uX, uY, uZ;
	uX.num = X();
	uY.num = Y();
	uZ.num = Z();

	// Extract the exponent bits. Note that they are not shifted to the right so
	// value is actually the biased exponent times 2^23.
	unsigned int expX = uX.bits & EXP_MASK;
	unsigned int expY = uY.bits & EXP_MASK;
	unsigned int expZ = uZ.bits & EXP_MASK;

	// Find the largest exponent.
	unsigned int expMax;
	if (expX > expY)
	{
		if (expX > expZ)
			expMax = expX;
		else
			expMax = expZ;
	}
	else if (expY > expZ)
		expMax = expY;
	else
		expMax = expZ;

	// If the largest exponent is zero, they all must be zero and the vector
	// has zero length.
	if (expMax == 0)
	{
		mag = 0.0f;
		return(false);
	}

	// Find the smallest non-zero exponent. Also, if the differance between the
	// components is so large that scaling cannot make them both legal, make the
	// smaller one zero.
	unsigned int expMin = expMax;
	if ((expMax - expX) >= EXP_BIAS)
	{
		uX.num = 0.0f;
		expX = 0;
	}
	else if (expX < expMin)
		expMin = expX;
	if ((expMax - expY) >= EXP_BIAS)
	{
		uY.num = 0.0f;
		expY = 0;
	}
	else if (expY < expMin)
		expMin = expY;
	if ((expMax - expZ) >= EXP_BIAS)
	{
		uZ.num = 0.0f;
		expZ = 0;
	}
	else if (expZ < expMin)
		expMin = expZ;

	// If any components are out of the legal range, shift them all so they are
	// in.
	unsigned int expMid = EXP_BIAS;
	if ((expMax >= EXP_MAX) || (expMin <= EXP_MIN))
	{
		// Find the middle of the exponent range.
		expMid = ((expMin + expMax) >> 1) & EXP_MASK;

		// Zeros are special so ignore them; otherwise, clear the exponent bits
		// then reset them so the middle move to the bias. The shift value is
		// computed each time so that nothing goes negative.
		if (expX != 0)
		{
			uX.bits &= ~EXP_MASK;
			uX.bits |= (expX + EXP_BIAS - expMid) & EXP_MASK;
		}
		if (expY != 0)
		{
			uY.bits &= ~EXP_MASK;
			uY.bits |= (expY + EXP_BIAS - expMid) & EXP_MASK;
		}
		if (expZ != 0)
		{
			uZ.bits &= ~EXP_MASK;
			uZ.bits |= (expZ + EXP_BIAS - expMid) & EXP_MASK;
		}
	}

	// Find the magnitude and the normal.
	mag = Math::Sqrt(uX.num * uX.num + uY.num * uY.num + uZ.num * uZ.num);
	Set(uX.num / mag, uY.num / mag, uZ.num / mag);

	// Shift the magnitude exponent back if necessary.
	if (expMid != EXP_BIAS)
	{
		unsigned int &magBits = *((unsigned int*)&mag);
		unsigned int expMag = magBits & EXP_MASK;
		magBits &= ~EXP_MASK;
		magBits |= (expMag + expMid - EXP_BIAS) & EXP_MASK;
	}

	return(true);
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::Normalize()
{
	FloatRet length = Length();

	operator/=(length);

	return length;
}

template <class DATA>
inline void Vector3Template<DATA>::Negate(void)
{
	*this = Negated();
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::Negated(void) const
{
	return -*this;
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::Normalized(void) const
{
	Vector3Template<DATA> v = *this;
	v.Normalize();
	return v;
}


// piecewise functions...
template <class DATA>
inline TYPENAME Vector3Template<DATA>::RetType Vector3Template<DATA>::Min(Vector3CRef v) const
{
	return RetType(
		Math::Min(X(), v.X()),
		Math::Min(Y(), v.Y()),
		Math::Min(Z(), v.Z()));
}

// piecewise functions...
template <class DATA>
inline TYPENAME Vector3Template<DATA>::OtherRetType Vector3Template<DATA>::Min(Vector3OtherCRef v) const
{
	return OtherRetType(
		Math::Min(X(), v.X()),
		Math::Min(Y(), v.Y()),
		Math::Min(Z(), v.Z()));
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::RetType Vector3Template<DATA>::Max(Vector3CRef v) const
{
	return RetType(
		Math::Max(X(), v.X()),
		Math::Max(Y(), v.Y()),
		Math::Max(Z(), v.Z()));
}

template <class DATA>
inline TYPENAME Vector3Template<DATA>::OtherRetType Vector3Template<DATA>::Max(Vector3OtherCRef v) const
{
	return OtherRetType(
		Math::Max(X(), v.X()),
		Math::Max(Y(), v.Y()),
		Math::Max(Z(), v.Z()));
}

template <class DATA>
inline TYPENAME Vector3Template<DATA> Vector3Template<DATA>::Abs(void) const
{
	return BaseRetType(
		Math::Abs(X()),
		Math::Abs(Y()),
		Math::Abs(Z()));
}

template <class DATA>
inline Vector3 Vector3Template<DATA>::Min(Vector3CRef v1, Vector3CRef v2)
{
	return v1.Min(v2);
}

template <class DATA>
inline Vector3 Vector3Template<DATA>::Max(Vector3CRef v1, Vector3CRef v2)
{
	return v1.Max(v2);
}

template <class DATA>
inline Vector3 Vector3Template<DATA>::Abs(Vector3CRef v)
{
	return v.Abs();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret Vector3Template<DATA>::SumXYZ(Vector3CRef v)
{
	return v.SumXYZ();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret SumXZ(Vector3CRef v)
{
	return v.SumXZ();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret SumYZ(Vector3CRef v)
{
	return v.SumYZ();
}

template <class DATA>
inline TYPENAME VectorTemplateFloats<DATA>::Ret SumXY(Vector3CRef v)
{
	return v.SumXY();
}

//---------------------------------
// vector/matrix interaction code
//---------------------------------

// multiply
template <class DATA>
inline Vector3 Vector3Template<DATA>::operator*(const Matrix3x3 &m) const
{
	float rx = X() * m.e00 + Y() * m.e10 + Z() * m.e20;
	float ry = X() * m.e01 + Y() * m.e11 + Z() * m.e21;
	float rz = X() * m.e02 + Y() * m.e12 + Z() * m.e22;

	return Vector3(rx, ry, rz);
}


// multiply by transpose
template <class DATA>
inline Vector3 Vector3Template<DATA>::operator/(const DirCos3x3 &m) const
{
	float rx = X() * m.e00 + Y() * m.e01 + Z() * m.e02;
	float ry = X() * m.e10 + Y() * m.e11 + Z() * m.e12;
	float rz = X() * m.e20 + Y() * m.e21 + Z() * m.e22;

	return Vector3(rx, ry, rz);
}


// Multiply, assuming w == 1 and v is [x y z 1]
template <class DATA>
inline Vector3 Vector3Template<DATA>::operator*(const Graphics4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float rx = X() * m.r00 + Y() * m.r10 + Z() * m.r20 + m.tx;
	float ry = X() * m.r01 + Y() * m.r11 + Z() * m.r21 + m.ty;
	float rz = X() * m.r02 + Y() * m.r12 + Z() * m.r22 + m.tz;

	return Vector3(rx, ry, rz);
}


// Multiply by inverse, assuming w == 1 and v is [x y z 1], only rotation and translation
template <class DATA>
inline Vector3 Vector3Template<DATA>::operator/(const Cartesian4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float dx = X() - m.tx;
	float dy = Y() - m.ty;
	float dz = Z() - m.tz;

	float rx = dx * m.r00 + dy * m.r01 + dz * m.r02;
	float ry = dx * m.r10 + dy * m.r11 + dz * m.r12;
	float rz = dx * m.r20 + dy * m.r21 + dz * m.r22;

	return Vector3(rx, ry, rz);
}


// Multiply by a unit quaternion, representing a rotation
template <class DATA>
inline Vector3 Vector3Template<DATA>::operator*(const ts_Quatvw &q) const
{
	Vector3 result;

	qatVectMul( &q, this, &result );

	return( result );
}


// Multiply by the transpose of a unit quaternion
template <class DATA>
inline Vector3 Vector3Template<DATA>::operator/(const ts_Quatvw &q) const
{
	Vector3 result;
	ts_Quatvw inverse;

	inverse = q;
	inverse.W *= -1.0f;

	qatVectMul( &q, this, &result );

	return( result );
}


// Full graphics [x y z 1] multiply
template <class DATA>
inline Vector4 Vector3Template<DATA>::Transform3(const Matrix4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	Vector4 result;

	result.X(X() * m.e00 + Y() * m.e10 + Z() * m.e20 + m.e30);
	result.Y(X() * m.e01 + Y() * m.e11 + Z() * m.e21 + m.e31);
	result.Z(X() * m.e02 + Y() * m.e12 + Z() * m.e22 + m.e32);
	result.w(X() * m.e03 + Y() * m.e13 + Z() * m.e23 + m.e33);

	// Assumpiton based on affine transromations...which we won't do
	//result.w = m.e[15];

	return result;
}


// Basis multiply (ignores translation, like making a 3x3)
template <class DATA>
inline Vector3 Vector3Template<DATA>::BasisTransform(const Graphics4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float rx = X() * m.r00 + Y() * m.r10 + Z() * m.r20;
	float ry = X() * m.r01 + Y() * m.r11 + Z() * m.r21;
	float rz = X() * m.r02 + Y() * m.r12 + Z() * m.r22;

	return Vector3(rx, ry, rz);
}

template <class DATA>
inline Vector3 Vector3Template<DATA>::InvBasisTransform(const Graphics4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float rx = X() * m.r00 + Y() * m.r01 + Z() * m.r02;
	float ry = X() * m.r10 + Y() * m.r11 + Z() * m.r12;
	float rz = X() * m.r20 + Y() * m.r21 + Z() * m.r22;

	return Vector3(rx, ry, rz);
}


// fast multiply-- should always a platform-specific implementation. must handle this == v
template <class DATA>
inline Vector3 Vector3Template<DATA>::FastMultiply(Vector3CRef v, const Graphics4x4 &m)
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float productx = v.X() * m.r00 + v.Y() * m.r10 + v.Z() * m.r20 + m.tx;
	float producty = v.X() * m.r01 + v.Y() * m.r11 + v.Z() * m.r21 + m.ty;
	float productz = v.X() * m.r02 + v.Y() * m.r12 + v.Z() * m.r22 + m.tz;

	// copy it in
	X(productx);
	Y(producty);
	Z(productz);
	return *this;
}

template <class DATA>
inline Vector3 Vector3Template<DATA>::FastBasisTransform(Vector3CRef v, const Graphics4x4 &m)
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4


	float productx = v.X() * m.r00 + v.Y() * m.r10 + v.Z() * m.r20;
	float producty = v.X() * m.r01 + v.Y() * m.r11 + v.Z() * m.r21;
	float productz = v.X() * m.r02 + v.Y() * m.r12 + v.Z() * m.r22;

	// copy it in
	X(productx);
	Y(producty);
	Z(productz);
	return *this;
}

#ifdef GCN
template<class DATA>
inline void Vector3Template<DATA>::EndianSwap(void)
{
	float t = X(); ByteSwap(t); X(t);
	float u = Y(); ByteSwap(u); Y(u);
	float v = Z(); ByteSwap(v); Z(v);
}

#endif

template<class DATA>
inline bool Vector3Template<DATA>::IsNaN(void) const
{
	return 
		Math::IsNaN(X()) ||
		Math::IsNaN(Y()) ||
		Math::IsNaN(Z());
}

