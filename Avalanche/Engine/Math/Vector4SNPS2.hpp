// Because of GCC and its inlining rules, we override some stuff that
// we dont really have to just to make the definitions come early on
// so that it can be inlined.

#define INLINE_ONLY extern inline 

INLINE_ONLY void Vector4Packed::Set(Vector4PackedDataCRef v)
{
	x(v._x);
	y(v._y);
	z(v._z);
	w(v._w);
}

INLINE_ONLY Vector4Aligned::Vector4Template()
{
}

INLINE_ONLY void Vector4Aligned::Set(float _x, float _y, float _z, float _w)
{
	u128 temp, temp2;

	asm(
		"### Vector4Aligned::Set(x,y,z,w) ### 1/3 ###\n\t"
		"pextlw temp, _z, _x"
		: "=r temp" (temp)
		: "r _z" (_z), "r _x" (_x));

	asm(
		"### Vector4Aligned::Set(x,y,z,w) ### 2/3 ###\n\t"
		"pextlw temp, _w, _y"
		: "=r temp" (temp2)
		: "r _w" (_w), "r _y" (_y));


	asm(
		"### Vector4Aligned::Set(x,y,z,w) ### 3/3 ###\n\t"
		"pextlw this, temp2, temp1"
		: "=r this" (temp)
		: "r temp2" (temp2), "r temp1" (temp));

	_vec = temp;
}

INLINE_ONLY void Vector4Aligned::Set(Vector3CRef v)
{
	vec(v.vec());
}

INLINE_ONLY void Vector4Aligned::Set(Vector4PackedDataCRef v)
{
	u128 pt1;
	u128 pt2;
	u128 temp;
	register u128 sa asm("$0");

 	asm(
 		"### V4Aligned::Set(V4Packed) ### 1/4 ###\n\t"
		"mtsab ptr, 0"
		: "=r" (sa)
		: "r ptr" (&v));

	asm(
 		"### V4Aligned::Set(V4Packed) ### 2/4 ###\n\t"
		"lq pt1, ptr"
		: "=r pt1" (pt1)
		: "m ptr" (v._x), "m" (v._y), "m" (v._z));

	asm(
 		"### V4Aligned::Set(V4Packed) ### 3/4 ###\n\t"
		"lq pt2, 0x10+v"
		: "=r pt2" (pt2)
		: "m v" (v._x), "m" (v._y), "m" (v._z));

	asm(
 		"### V4Aligned::Set(V4Packed) ### 4/4 ###\n\t"
		"qfsrv %0, %2, %1"
		: "=r" (temp)
		: "r" (pt1), "r" (pt2), "r" (sa));

	vec(temp);
}

INLINE_ONLY void Vector4Aligned::Set(Vector4AlignedDataCRef v)
{
	vec(v.vec());
}

INLINE_ONLY Vector4Aligned::Vector4Template(Vector4PackedDataCRef v)
{
	Set(v);
}

INLINE_ONLY Vector4Aligned::Vector4Template(Vector4AlignedDataCRef v)
{
	Set(v);
}

INLINE_ONLY void Vector4Packed::Set(Vector4AlignedDataCRef v)
{
	x(v.x());
	y(v.y());
	z(v.z());
	w(v.w());
}

INLINE_ONLY void Vector4Aligned::Clear(void)
{
	u128 temp;

	asm(
		"### Vector4Aligned::Clear ### 1/1 ###\n\t"

		"  .if %A0                   \n\t"
		"  vsub this, vf0, vf0		  \n\t"
		"  .endif                    \n\t"

		"  .if %A1                   \n\t"
		"  por this, $0, $0			  \n\t"
		"  .endif"

		: "=j,r this" (temp));

	_vec = temp;
}


INLINE_ONLY Vector4Aligned::FloatRet Vector4Aligned::SumXYZW(void) const
{
	register u128 acc asm ("vf0");
	u128 ones;
	u128 sum;
	u128 temp = _vec;
	
	asm(
		"### Vector4Aligned::SumXYZW ### 1/4 ###\n\t"
		"vadday.x ACC, this, this"
		: "=j" (acc)
		: "j this" (temp));

	asm(
		"### Vector4Aligned::SumXYZW ### 2/4 ###\n\t"
		"vmaxw ones, vf0, vf0"
		: "=j ones" (ones));

	asm(
		"### Vector4Aligned::SumXYZW ### 3/4 ###\n\t"
		"vmaddaz.x ACC, ones, this"
		: "=j" (acc)
		: "j ones" (ones), "j this" (temp), "j" (acc));

	asm(
		"### Vector4Aligned::SumXYZW ### 4/4 ###\n\t"
		"vmaddw.x sum, vf0, this"
		: "=j sum" (sum)
		: "j this" (temp), "j" (acc));

	Vector4Aligned retval;
	retval.vec(sum);
	return retval.x();
}

INLINE_ONLY Vector4Aligned::FloatRet Vector4Aligned::SumXYZ(void) const
{
	register u128 acc asm ("vf0");
	u128 ones;
	u128 sum;
	u128 temp = _vec;
	
	asm(
		"### Vector4Aligned::SumXYZ ### 1/3 ###\n\t"
		"vadday.x ACC, this, this"
		: "=j" (acc)
		: "j this" (temp));

	asm(
		"### Vector4Aligned::SumXYZ ### 2/3 ###\n\t"
		"vmaxw ones, vf0, vf0"
		: "=j ones" (ones));

	asm(
		"### Vector4Aligned::SumXYZ ### 3/3 ###\n\t"
		"vmaddz.x sum, ones, this"
		: "=j sum" (sum)
		: "j ones" (ones), "j this" (temp), "j" (acc));

	Vector4Aligned retval;
	retval.vec(sum);
	return retval.x();
}

INLINE_ONLY Vector4Aligned::FloatRet Vector4Aligned::SumXZ(void) const
{
	u128 sum;
	u128 temp = _vec;
	
	asm(
		"### Vector4Aligned::SumXZ ### 1/1 ###\n\t"
		"vaddz.x sum, this, this"
		: "=j sum" (sum)
		: "j this" (temp));

	Vector4Aligned retval;
	retval.vec(sum);
	return retval.x();
}

INLINE_ONLY Vector4Aligned::FloatRet Vector4Aligned::SumYZ(void) const
{
	u128 sum;
	u128 temp = _vec;
	
	asm(
		"### Vector4Aligned::SumYZ ### 1/1 ###\n\t"
		"vaddz.y sum, this, this"
		: "=j sum" (sum)
		: "j this" (temp));

	Vector4Aligned retval;
	retval.vec(sum);
	return retval.y();
}

INLINE_ONLY Vector4Aligned::FloatRet Vector4Aligned::SumXY(void) const
{
	u128 sum;
	u128 temp = _vec;
	
	asm(
		"### Vector4Aligned::SumXY ### 1/1 ###\n\t"
		"vaddy.x sum, this, this"
		: "=j sum" (sum)
		: "j this" (temp));

	Vector4Aligned retval;
	retval.vec(sum);
	return retval.x();
}

// Special functions
INLINE_ONLY Vector4AARet Vector4Aligned::Cross(Vector4AlignedCRef v) const
{
	u128 res;
	register u128 acc asm ("vf0");
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector4Aligned::Cross ### 1/3 ###\n\t"
		"vopmula ACC, lhs, rhs"
		: "=j" (acc)
		: "j lhs" (temp), "j rhs" (vvec));
	asm(
		"### Vector4Aligned::Cross ### 2/3 ###\n\t"
		"vopmsub res, rhs, lhs"
		: "=j res" (res)
		: "j lhs" (temp), "j rhs" (vvec), "j" (acc));

	asm(
		"### Vector4Aligned::Cross ### 3/3 ###\n\t"
		"vmove.w res, vf0"
		: "+j res" (res));

	Vector4AARet retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector4AARet Vector4Aligned::Diag(Vector4AlignedCRef v) const
{
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector4Aligned::Diag ### 1/1 ###\n\t"
		"vmul temp, lhs, rhs"
		: "=j temp" (temp)
		: "j lhs" (temp), "j rhs" (vvec));

	Vector4AARet retval;
	retval.vec(temp);
	return retval;
}

INLINE_ONLY Vector4 Vector4Aligned::operator*(FloatParam scalar) const
{
	u128 res;
	u128 temp = _vec;

	Vector4X vscale(scalar);
	u128 vscalevec = vscale._vec;

	asm(
		"### Vector4Aligned::operator*(float) ### 1/1 ###\n\t"
		"vmulx res, this, vscale"
		: "=j res" (res) 
		: "j this" (temp), "j vscale" (vscalevec));

	Vector4 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector4AARet Vector4Aligned::operator+(Vector4AlignedCRef v) const
{
	u128 res;
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector4Aligned::operator+(Vector4CRef) ### 1/1 ###\n\t"
		"vadd res, lhs, rhs"
		: "=j res" (res) 
		: "j lhs" (temp), "j rhs" (vvec));

	Vector4 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector4AARet Vector4Aligned::operator-(Vector4AlignedCRef v) const
{
	u128 res;
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector4Aligned::operator-(Vector4CRef) ### 1/1 ###\n\t"
		"vsub res, lhs, rhs"
		: "=j res" (res) 
		: "j lhs" (temp), "j rhs" (vvec));

	Vector4 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector4Aligned Vector4Aligned::operator-(void) const
{
	u128 res;
	u128 temp = _vec;

	asm(
		"### Vector4Aligned::operator-(void) ### 1/1 ### \n\t"
		"vsub res, vf0, vec"
		: "=j res" (res)
		: "j vec" (temp));

	Vector4 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector4Aligned Vector4Aligned::operator-(FloatParam scalar) const
{
	u128 res;
	u128 temp = _vec;

	Vector4X vscale(scalar);
	u128 vscalevec = vscale._vec;

	asm(
		"### Vector4Aligned::operator*(float) ### 1/1 ###\n\t"
		"vsubx res, this, vscale"
		: "=j res" (res) 
		: "j this" (temp), "j vscale" (vscalevec));

	Vector4 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector4Aligned Vector4Aligned::operator+(FloatParam scalar) const
{
	u128 res;
	u128 temp = _vec;

	Vector4X vscale(scalar);
	u128 vscalevec = vscale._vec;

	asm(
		"### Vector4Aligned::operator+(float) ### 1/1 ###\n\t"
		"vaddx res, this, vscale"
		: "=j res" (res) 
		: "j this" (temp), "j vscale" (vscalevec));

	Vector4 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector4Aligned Vector4Aligned::Min(Vector4CRef v) const
{
	u128 res;
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector4Aligned::Min(Vector4CRef) ### 1/1 ###\n\t"
		"vmini res, lhs, rhs"
		: "=j res" (res) 
		: "j lhs" (temp), "j rhs" (vvec));

	Vector4 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector4Aligned Vector4Aligned::Max(Vector4CRef v) const
{
	u128 res;
	u128 temp = _vec;
	u128 vvec = v._vec;

	asm(
		"### Vector4Aligned::Max(Vector4CRef) ### 1/1 ###\n\t"
		"vmax res, lhs, rhs"
		: "=j res" (res) 
		: "j lhs" (temp), "j rhs" (vvec));

	Vector4 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector4Aligned Vector4Aligned::Abs(void) const
{
	u128 res;
	u128 temp = _vec;

	asm(
		"### Vector4Aligned::Abs(void) ### 1/1 ###\n\t"
		"vabs res, vec"
		: "=j res" (res)
		: "j vec" (temp));

	Vector4 retval;
	retval.vec(res);
	return retval;
}

INLINE_ONLY Vector4Aligned::FloatRet Vector4Aligned::LengthXZSquared(void) const
{
	return Diag(*this).SumXZ();
}

// TJC - a handful of unusual combinations that could use some speed...

INLINE_ONLY Vector4APRet Vector4Aligned::operator+(Vector4PackedCRef v) const
{
	return operator+(Vector4Aligned(v));
}

INLINE_ONLY Vector4APRet Vector4Aligned::operator-(Vector4PackedCRef v) const
{
	return operator-(Vector4Aligned(v));
}

INLINE_ONLY Vector4APRet Vector4Aligned::Cross(Vector4PackedCRef v) const
{
	return Cross((Vector4AlignedCRef)v);
}

INLINE_ONLY Vector4APRet Vector4Aligned::Diag(Vector4PackedCRef v) const
{
	return Diag((Vector4AlignedCRef)v);
}

INLINE_ONLY Vector4Aligned Vector4Aligned::Min(Vector4PackedCRef v) const
{
	return Min((Vector4Aligned)v);
}

INLINE_ONLY Vector4Aligned Vector4Aligned::Max(Vector4PackedCRef v) const
{
	return Max((Vector4Aligned)v);
}
/*
INLINE_ONLY Vector4Aligned Vector4Packed::operator+(Vector4AlignedCRef v) const
{
	return v + *this;
}

INLINE_ONLY Vector4PARet Vector4Packed::operator-(Vector4AlignedCRef v) const
{
	return ((Vector4Aligned)*this) - v;
}

INLINE_ONLY Vector4PARet Vector4Packed::Cross(Vector4AlignedCRef v) const
{
	return ((Vector4Aligned)*this).Cross(v);
}

INLINE_ONLY Vector4PARet Vector4Packed::Diag(Vector4AlignedCRef v) const
{
	return v.Diag(*this);
}

INLINE_ONLY Vector4PARet Vector4Packed::Min(Vector4AlignedCRef v) const
{
	return v.Min(*this);
}

INLINE_ONLY Vector4PARet Vector4Packed::Max(Vector4AlignedCRef v) const
{
	return v.Max(*this);
}
*/

// Full graphics [x y z 1] multiply
INLINE_ONLY Vector4 Vector3Padded::Transform3(const Matrix4x4 &m) const
{
#ifdef CHECK_GRAPHICS4X4
	ASSERT(m.u0 == 0.0f);
	ASSERT(m.u1 == 0.0f);
	ASSERT(m.u2 == 0.0f);
	ASSERT(m.w == 1.0f);
#endif //CHECK_GRAPHICS4X4

#if 0
	Vector4 result;

	result.x(x() * m.e00 + y() * m.e10 + z() * m.e20 + m.e30);
	result.y(x() * m.e01 + y() * m.e11 + z() * m.e21 + m.e31);
	result.z(x() * m.e02 + y() * m.e12 + z() * m.e22 + m.e32);
	result.w(x() * m.e03 + y() * m.e13 + z() * m.e23 + m.e33);

	return result;
#else

	u128 dest;
	u128 v = vec();

#if 1
	// TJC - vf0 is used to represent the acc in the trash list below.
	// Thats what I use everywhere, so we should be ok.
	asm("### Vector3Padded::Transform3(const Matrix4x4 &m) 1/1 ###\n\t"
		 "vmulax ACC, row0, this   \n\t"
		 "vmadday ACC, row1, this  \n\t"
		 "vmaddaz ACC, row2, this  \n\t"
		 "vmaddw dest, row3, vf0   \n\t"
		 : "=j dest" (dest)
		 : "j row0" (m.row0), "j row1" (m.row1), "j row2" (m.row2), "j row3" (m.row3), "j this" (v) : "vf0");
#endif

	Vector4 res;
	res._vec = dest;

	// Assumpiton based on affine transromations...which we won't do
	//result.w = m.e[15];

	return res;
#endif
}



#undef INLINE_ONLY
