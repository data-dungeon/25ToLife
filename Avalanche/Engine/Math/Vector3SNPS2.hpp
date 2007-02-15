//
// PS2 specific versions of the vector3 code..
//

// Because of GCC and its inlining rules, we override some stuff that
// we dont really have to just to make the definitions come early on
// so that it can be inlined.

#define INLINE_ONLY extern inline 

INLINE_ONLY void Vector3Packed::Set(Vector3PackedDataCRef v)
{
	x(v._x);
	y(v._y);
	z(v._z);
}


// Vector3Padded

INLINE_ONLY Vector3Padded::Vector3Template()
{
}

INLINE_ONLY void Vector3Padded::Set(float _x, float _y, float _z)
{
	u128 temp;

	asm(
		"### Vector3Padded::Set(x,y,z) ### 1/2 ###\n\t"
		"pextlw temp, _z, _x"
		: "=r temp" (temp)
		: "r _z" (_z), "r _x" (_x));

	asm(
		"### Vector3Padded::Set(x,y,z) ### 2/2 ###\n\t"
		"pextlw this, _y, temp"
		: "=r this" (temp)
		: "r _y" (_y), "r temp" (temp));

	_vec = temp;
}

INLINE_ONLY void Vector3Padded::Set(Vector4CRef v)
{
	vec(v._vec);
}

// INLINE_ONLY void Vector3Padded::Set(Vector4X x, Vector4Y y, Vector4Z z)
// {
// 	_vec = x._vec;
// 	asm(
// 		"### Vector3Padded::Set(V4X, V4Y, V4Z) ### 1/2 ###\n\t"
// 		"vmove.y this, ypart"
// 		: "+j this" (_vec)
// 		: "j ypart" (y._vec));

// 	asm(
// 		"### Vector3Padded::Set(V4X, V4Y, V4Z) ### 2/2 ###\n\t"
// 		"vmove.z this, zpart"
// 		: "+j this" (_vec)
// 		: "j ypart" (z._vec));
// }

INLINE_ONLY void Vector3Padded::Set(Vector3PackedDataCRef v)
{
	u128 pt1;
	u128 pt2;
	u128 temp;
	register u128 sa asm("$0");

 	asm(
 		"### V3Padded::Set(V3Packed) ### 1/4 ###\n\t"
		"mtsab ptr, 0"
		: "=r" (sa)
		: "r ptr" (&v));

	asm(
 		"### V3Padded::Set(V3Packed) ### 2/4 ###\n\t"
		"lq pt1, ptr"
		: "=r pt1" (pt1)
		: "m ptr" (v._x), "m" (v._y), "m" (v._z));

	asm(
 		"### V3Padded::Set(V3Packed) ### 3/4 ###\n\t"
		"lq pt2, 0x10+v"
		: "=r pt2" (pt2)
		: "m v" (v._x), "m" (v._y), "m" (v._z));

	asm(
 		"### V3Padded::Set(V3Packed) ### 4/4 ###\n\t"
		"qfsrv %0, %2, %1"
		: "=r" (temp)
		: "r" (pt1), "r" (pt2), "r" (sa));

	vec(temp);
}

INLINE_ONLY void Vector3Padded::Set(Vector3PaddedDataCRef v)
{
	vec(v._vec);
}

INLINE_ONLY Vector3Padded::Vector3Template(Vector3PackedDataCRef v)
{
	Set(v);
}

INLINE_ONLY Vector3Padded::Vector3Template(Vector3PaddedDataCRef v)
{
	Set(v);
}

INLINE_ONLY void Vector3Packed::Set(Vector3PaddedDataCRef v)
{
	x(v.x());
	y(v.y());
	z(v.z());
}

INLINE_ONLY void Vector3Padded::Clear(void)
{
	u128 temp;

	asm(
		"### Vector3Padded::Clear ### 1/1 ###\n\t"

		"  .if %A0                   \n\t"
		"  vsub this, vf0, vf0		  \n\t"
		"  .endif                    \n\t"

		"  .if %A1                   \n\t"
		"  por this, $0, $0			  \n\t"
		"  .endif"

		: "=j,r this" (temp));

	_vec = temp;
}


INLINE_ONLY Vector3Padded::FloatRet Vector3Padded::SumXYZ(void) const
{
	register u128 acc asm ("vf0");
	u128 ones;
	u128 sum;
	u128 temp = _vec;
	
	asm(
		"### Vector3Padded::SumXYZ ### 1/3 ###\n\t"
		"vadday.x ACC, this, this"
		: "=j" (acc)
		: "j this" (temp));

	asm(
		"### Vector3Padded::SumXYZ ### 2/3 ###\n\t"
		"vmaxw ones, vf0, vf0"
		: "=j ones" (ones));

	asm(
		"### Vector3Padded::SumXYZ ### 3/3 ###\n\t"
		"vmaddz.x sum, ones, this"
		: "=j sum" (sum)
		: "j ones" (ones), "j this" (temp), "j" (acc));

	Vector3Padded retval;
	retval.vec(sum);
	return retval.x();
}

INLINE_ONLY Vector3Padded::FloatRet Vector3Padded::SumXZ(void) const
{
	u128 sum;
	u128 temp = _vec;
	
	asm(
		"### Vector3Padded::SumXZ ### 1/1 ###\n\t"
		"vaddz.x sum, this, this"
		: "=j sum" (sum)
		: "j this" (temp));

	Vector3Padded retval;
	retval.vec(sum);
	return retval.x();
}

INLINE_ONLY Vector3Padded::FloatRet Vector3Padded::SumYZ(void) const
{
	u128 sum;
	u128 temp = _vec;
	
	asm(
		"### Vector3Padded::SumYZ ### 1/1 ###\n\t"
		"vaddz.y sum, this, this"
		: "=j sum" (sum)
		: "j this" (temp));

	Vector3Padded retval;
	retval.vec(sum);
	return retval.y();
}

INLINE_ONLY Vector3Padded::FloatRet Vector3Padded::SumXY(void) const
{
	u128 sum;
	u128 temp = _vec;
	
	asm(
		"### Vector3Padded::SumXY ### 1/1 ###\n\t"
		"vaddy.x sum, this, this"
		: "=j sum" (sum)
		: "j this" (temp));

	Vector3Padded retval;
	retval.vec(sum);
	return retval.x();
}

// Special functions
INLINE_ONLY Vector3PadPadRet Vector3Padded::Cross(Vector3PaddedCRef v) const
{
	u128 res;
	register u128 acc asm ("vf0");
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector3Padded::Cross ### 1/2 ###\n\t"
		"vopmula ACC, lhs, rhs"
		: "=j" (acc)
		: "j lhs" (temp), "j rhs" (vvec));
	asm(
		"### Vector3Padded::Cross ### 2/2 ###\n\t"
		"vopmsub res, rhs, lhs"
		: "=j res" (res)
		: "j lhs" (temp), "j rhs" (vvec), "j" (acc));

	Vector3PadPadRet retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector3PadPadRet Vector3Padded::Diag(Vector3PaddedCRef v) const
{
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector3Padded::Diag ### 1/1 ###\n\t"
		"vmul temp, lhs, rhs"
		: "=j temp" (temp)
		: "j lhs" (temp), "j rhs" (vvec));

	Vector3 retval;
	retval.vec(temp);
	return retval;
}

INLINE_ONLY Vector3 Vector3Padded::operator*(FloatParam scalar) const
{
	u128 res;
	u128 temp = _vec;

	Vector4X vscale(scalar);
	u128 vscalevec = vscale._vec;

	asm(
		"### Vector3Padded::operator*(float) ### 1/1 ###\n\t"
		"vmulx res, this, vscale"
		: "=j res" (res) 
		: "j this" (temp), "j vscale" (vscalevec));

	Vector3 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector3PadPadRet Vector3Padded::operator+(Vector3PaddedCRef v) const
{
	u128 res;
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector3Padded::operator+(Vector3CRef) ### 1/1 ###\n\t"
		"vadd res, lhs, rhs"
		: "=j res" (res) 
		: "j lhs" (temp), "j rhs" (vvec));

	Vector3 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector3PadPadRet Vector3Padded::operator-(Vector3PaddedCRef v) const
{
	u128 res;
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector3Padded::operator-(Vector3CRef) ### 1/1 ###\n\t"
		"vsub res, lhs, rhs"
		: "=j res" (res) 
		: "j lhs" (temp), "j rhs" (vvec));

	Vector3 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector3Padded Vector3Padded::operator-(void) const
{
	u128 res;
	u128 temp = _vec;

	asm(
		"### Vector3Padded::operator-(void) ### 1/1 ### \n\t"
		"vsub res, vf0, vec"
		: "=j res" (res)
		: "j vec" (temp));

	Vector3 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector3Padded Vector3Padded::operator-(FloatParam scalar) const
{
	u128 res;
	u128 temp = _vec;

	Vector4X vscale(scalar);
	u128 vscalevec = vscale._vec;

	asm(
		"### Vector3Padded::operator*(float) ### 1/1 ###\n\t"
		"vsubx res, this, vscale"
		: "=j res" (res) 
		: "j this" (temp), "j vscale" (vscalevec));

	Vector3 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector3Padded Vector3Padded::operator+(FloatParam scalar) const
{
	u128 res;
	u128 temp = _vec;

	Vector4X vscale(scalar);
	u128 vscalevec = vscale._vec;

	asm(
		"### Vector3Padded::operator+(float) ### 1/1 ###\n\t"
		"vaddx res, this, vscale"
		: "=j res" (res) 
		: "j this" (temp), "j vscale" (vscalevec));

	Vector3 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector3Padded Vector3Padded::Min(Vector3CRef v) const
{
	u128 res;
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector3Padded::Min(Vector3CRef) ### 1/1 ###\n\t"
		"vmini res, lhs, rhs"
		: "=j res" (res) 
		: "j lhs" (temp), "j rhs" (vvec));

	Vector3 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector3Padded Vector3Padded::Max(Vector3CRef v) const
{
	u128 res;
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector3Padded::Max(Vector3CRef) ### 1/1 ###\n\t"
		"vmax res, lhs, rhs"
		: "=j res" (res) 
		: "j lhs" (temp), "j rhs" (vvec));

	Vector3 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector3Padded Vector3Padded::Abs(void) const
{
	u128 res;
	u128 temp = _vec;

	asm(
		"### Vector3Padded::Abs(void) ### 1/1 ###\n\t"
		"vabs res, vec"
		: "=j res" (res)
		: "j vec" (temp));

	Vector3 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector3Padded Vector3Padded::operator*(const Matrix3x3 &m) const
{
	float _x = x();
	float _y = y();
	float _z = z();
	Vector3Padded res;
	res.Set(_x * m.e00 + _y * m.e10 + _z * m.e20,
			  _x * m.e01 + _y * m.e11 + _z * m.e21,
			  _x * m.e02 + _y * m.e12 + _z * m.e22);
	return res;
}

INLINE_ONLY Vector3Padded Vector3Padded::operator/(const DirCos3x3 &m) const
{
	float _x = x();
	float _y = y();
	float _z = z();
	
	Vector3Padded res;
	res.Set(_x * m.e00 + _y * m.e01 + _z * m.e02,
			  _x * m.e10 + _y * m.e11 + _z * m.e12,
			  _x * m.e20 + _y * m.e21 + _z * m.e22);
	return res;
}

INLINE_ONLY Vector3Padded Vector3Padded::operator*(const Graphics4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	u128 dest;
	register u128 acc asm ("vf0");
	u128 row0 = m.row0;
	u128 row1 = m.row1;
	u128 row2 = m.row2;
	u128 row3 = m.row3;
	u128 vec = _vec;

	asm("### Vector3Padded::operator*(const Graphics4x4 &m) ### 1/1 ###\n\t"
		 "vmulax ACC, row0, this   \n\t"
		 "vmadday ACC, row1, this  \n\t"
		 "vmaddaz ACC, row2, this  \n\t"
		 "vmaddw dst, row3, vf0    \n\t"
		 : "=j dst" (dest), "=j" (acc)
		 : "j this" (vec), "j row0" (row0), "j row1" (row1), "j row2" (row2), "j row3" (row3), "j" (acc));

	Vector3Padded res;
	res._vec = dest;
	return res;
}
/*
INLINE_ONLY bool Vector3Padded::AllXYZLessOrEqual(FloatClipParam w) const
{
	bool result;
	u128 vec = _vec;
	register u128 clipflags asm ("vf0");

	asm("vclipw.xyz %1, %2\n"
		 : "=j" (clipflags)
		 : "j" (vec), "j" (w));

	asm("cfc2 %0, vi18"
		 : "=r" (result)
		 : "j" (clipflags));

	return (result & 0x15) == 0;
}

INLINE_ONLY bool Vector3Padded::AllXYZGreater(FloatClipParam w) const
{
	bool result;
	u128 vec = _vec;
	register u128 clipflags asm ("vf0");

	asm("vclipw.xyz %1, %2\n"
		 : "=j" (clipflags)
		 : "j" (vec), "j" (w));

	asm("cfc2 %0, vi18"
		 : "=r" (result)
		 : "j" (clipflags));

	return (result & 0x15) == 0x15;
}

INLINE_ONLY bool Vector3Padded::AnyXYZLessOrEqual(FloatClipParam w) const
{
	bool result;
	u128 vec = _vec;
	register u128 clipflags asm ("vf0");

	asm("vclipw.xyz %1, %2\n"
		 : "=j" (clipflags)
		 : "j" (vec), "j" (w));

	asm("cfc2 %0, vi18"
		 : "=r" (result)
		 : "j" (clipflags));

	return (result & 0x15) != 0x15;
}

INLINE_ONLY bool Vector3Padded::AnyXYZGreater(FloatClipParam w) const
{
	bool result;
	u128 vec = _vec;
	register u128 clipflags asm ("vf0");

	asm("vclipw.xyz %1, %2\n"
		 : "=j" (clipflags)
		 : "j" (vec), "j" (w));

	asm("cfc2 %0, vi18"
		 : "=r" (result)
		 : "j" (clipflags));

	return (result & 0x15) != 0;
}
*/
// Multiply by inverse, assuming w == 1 and v is [x y z 1], only rotation and translation
INLINE_ONLY Vector3Padded Vector3Padded::operator/(const Cartesian4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	float dx = x() - m.tx;
	float dy = y() - m.ty;
	float dz = z() - m.tz;

	Vector3Padded res;
	res.Set(dx * m.r00 + dy * m.r01 + dz * m.r02,
			  dx * m.r10 + dy * m.r11 + dz * m.r12,
			  dx * m.r20 + dy * m.r21 + dz * m.r22);
	return res;
}

// Basis multiply (ignores translation, like making a 3x3)
INLINE_ONLY Vector3Padded Vector3Padded::BasisTransform(const Graphics4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

	u128 dest;
	register u128 acc asm ("vf0");

	asm("### Vector3Padded::operator*(const Graphics4x4 &m) ###\n\t"
		 "vmulax ACC, row0, this"
		 : "=j" (acc)
		 : "j row0" (m.row0), "j this" (_vec));

	asm("vmadday ACC, row1, this"
		 : "=j" (acc)
		 : "j row1" (m.row1), "j this" (_vec), "j" (acc));

	asm("vmaddz dest, row2, this"
		 : "=j dest" (dest)
		 : "j row2" (m.row2), "j this" (_vec), "j" (acc));

	Vector3Padded res;
	res._vec = dest;
	return res;
}

INLINE_ONLY Vector3Padded Vector3Padded::InvBasisTransform(const Graphics4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4


	return Vector3(x() * m.r00 + y() * m.r01 + z() * m.r02,
						x() * m.r10 + y() * m.r11 + z() * m.r12,
						x() * m.r20 + y() * m.r21 + z() * m.r22);
}

// fast multiply-- should always a platform-specific implementation. must handle this == v
INLINE_ONLY Vector3Padded Vector3Padded::FastMultiply(Vector3CRef v, const Graphics4x4 &m)
{
	*this = v * m;
	return *this;
}

INLINE_ONLY Vector3Padded Vector3Padded::FastBasisTransform(Vector3CRef v, const Graphics4x4 &m)
{
	*this = v.BasisTransform(m);
	return *this;
}

INLINE_ONLY Vector3Padded::FloatRet Vector3Padded::LengthXZSquared(void) const
{
	return Diag(*this).SumXZ();
}

// TJC - a handful of unusual combinations that could use some speed...

INLINE_ONLY Vector3PadPakRet Vector3Padded::operator+(Vector3PackedCRef v) const
{
	return operator+(Vector3(v));
}

INLINE_ONLY Vector3PadPakRet Vector3Padded::operator-(Vector3PackedCRef v) const
{
	return operator-(Vector3(v));
}

INLINE_ONLY Vector3PadPakRet Vector3Padded::Cross(Vector3PackedCRef v) const
{
	return Cross((Vector3PaddedCRef)v);
}

INLINE_ONLY Vector3PadPakRet Vector3Padded::Diag(Vector3PackedCRef v) const
{
	return Diag((Vector3PaddedCRef)v);
}

INLINE_ONLY Vector3Padded Vector3Padded::Min(Vector3OtherCRef v) const
{
	return Min((Vector3)v);
}

INLINE_ONLY Vector3Padded Vector3Padded::Max(Vector3OtherCRef v) const
{
	return Max((Vector3)v);
}

INLINE_ONLY Vector3PadPakRet Vector3Packed::operator+(Vector3PaddedCRef v) const
{
	return ((Vector3)*this) + v;
}

INLINE_ONLY Vector3PakPadRet Vector3Packed::operator-(Vector3PaddedCRef v) const
{
	return ((Vector3)*this) - v;
}

INLINE_ONLY Vector3PakPadRet Vector3Packed::Cross(Vector3PaddedCRef v) const
{
	return ((Vector3)*this).Cross(v);
}

INLINE_ONLY Vector3PakPadRet Vector3Packed::Diag(Vector3PaddedCRef v) const
{
	return ((Vector3)*this).Diag(v);
}

INLINE_ONLY Vector3PakPadRet Vector3Packed::Min(Vector3PaddedCRef v) const
{
	return v.Min(*this);
}

INLINE_ONLY Vector3PakPadRet Vector3Packed::Max(Vector3PaddedCRef v) const
{
	return v.Max(*this);
}

#undef INLINE_ONLY
