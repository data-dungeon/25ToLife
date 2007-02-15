
#define INLINE_ONLY extern inline


INLINE_ONLY Vector4X Vector3PaddedData::X() const
{
	return Vector4X(_vec);
}

INLINE_ONLY Vector4Y Vector3PaddedData::Y() const
{
	return Vector4Y(_vec);
}

INLINE_ONLY Vector4Z Vector3PaddedData::Z() const 
{
	return Vector4Z(_vec);
}

INLINE_ONLY Vector4W Vector3PaddedData::W() const 
{
	return Vector4W(_vec);
}

INLINE_ONLY u32 Vector3PaddedData::Pad() const
{
	u32 temp;

	asm(
		"### Vector3::pad to u32 ###\n\t"
		"pextuw dest, $0, src"
		: "=r dest" (temp)
		: "r src" (_vec));

	asm(
		"pextuw dest, $0, src"
		: "=r dest" (temp)
		: "r src" (temp));

	return temp;
}

INLINE_ONLY void Vector3PaddedData::InitX(Vector4X v)
{
	_vec = v._vec;
}

INLINE_ONLY void Vector3PaddedData::X(Vector4X v)
{
	asm (
		" ### init Vector3::x with Vector4X ### 1/1 ###\n\t"
		"vaddx.x this, vf00, vx"
		: "+j this" (_vec)
		: "j vx" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::Y(Vector4X v)
{
	asm (
		" ### init Vector3::Y with Vector4X ### 1/1 ###\n\t"
		"vaddx.y this, vf00, vx"
		: "+j this" (_vec)
		: "j vx" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::Z(Vector4X v)
{
	asm (
		" ### init Vector3::Z with Vector4X ### 1/1 ###\n\t"
		"vaddx.z this, vf00, vx"
		: "+j this" (_vec)
		: "j vx" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::W(Vector4X v)
{
	asm (
		" ### init Vector3::W with Vector4X ### 1/1 ###\n\t"
		"vmulx.w this, vf00, vx"
		: "+j this" (_vec)
		: "j vx" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::X(Vector4Y v)
{
	asm (
		" ### init Vector3::X with Vector4Y ### 1/1 ###\n\t"
		"vaddy.x this, vf00, vy"
		: "+j this" (_vec)
		: "j vy" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::Y(Vector4Y v)
{
	asm (
		" ### init Vector3::Y with Vector4Y ### 1/1 ###\n\t"
		"vaddy.y this, vf00, vy"
		: "+j this" (_vec)
		: "j vy" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::Z(Vector4Y v)
{
	asm (
		" ### init Vector3::Z with Vector4Y ### 1/1 ###\n\t"
		"vaddy.z this, vf00, vy"
		: "+j this" (_vec)
		: "j vy" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::W(Vector4Y v)
{
	asm (
		" ### init Vector3::W with Vector4Y ### 1/1 ###\n\t"
		"vmuly.w this, vf00, vy"
		: "+j this" (_vec)
		: "j vy" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::X(Vector4Z v)
{
	asm (
		" ### init Vector3::X with Vector4Z ### 1/1 ###\n\t"
		"vaddz.x this, vf00, vz"
		: "+j this" (_vec)
		: "j vz" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::Y(Vector4Z v)
{
	asm (
		" ### init Vector3::Y with Vector4Z ### 1/1 ###\n\t"
		"vaddz.y this, vf00, vz"
		: "+j this" (_vec)
		: "j vz" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::Z(Vector4Z v)
{
	asm (
		" ### init Vector3::Z with Vector4Z ### 1/1 ###\n\t"
		"vaddz.z this, vf00, vz"
		: "+j this" (_vec)
		: "j vz" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::W(Vector4Z v)
{
	asm (
		" ### init Vector3::W with Vector4Z ### 1/1 ###\n\t"
		"vmulz.w this, vf00, vz"
		: "+j this" (_vec)
		: "j vz" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::X(Vector4W v)
{
	asm (
		" ### init Vector3::X with Vector4W ### 1/1 ###\n\t"
		"vaddw.x this, vf00, vw"
		: "+j this" (_vec)
		: "j vw" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::Y(Vector4W v)
{
	asm (
		" ### init Vector3::Y with Vector4W ### 1/1 ###\n\t"
		"vaddw.y this, vf00, vw"
		: "+j this" (_vec)
		: "j vw" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::Z(Vector4W v)
{
	asm (
		" ### init Vector3::Z with Vector4W ### 1/1 ###\n\t"
		"vaddw.z this, vf00, vw"
		: "+j this" (_vec)
		: "j vw" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::W(Vector4W v)
{
	asm (
		" ### init Vector3::Z with Vector4W ### 1/1 ###\n\t"
		"vmulw.w this, vf00, vw"
		: "+j this" (_vec)
		: "j vw" (v._vec));
}

INLINE_ONLY void Vector3PaddedData::X(float x)
{
	asm (
		" ### init Vector3::X with a float ### 1/1 ### \n\t"
		"ctc2	f_x, $vi21                       \n\t"
		"vnop                                  \n\t"
		"vaddi.x	this, vf00, I"
		: "+j this" (_vec)
		: "r f_x" (x)
		);
}

INLINE_ONLY void Vector3PaddedData::Y(float y)
{
	asm (
		" ### init Vector3::Y with a float ### 1/1 ### \n\t"
		"ctc2	f_y, $vi21                       \n\t"
		"vnop                                  \n\t"
		"vaddi.y	this, vf00, I"
		: "+j this" (_vec)
		: "r f_y" (y)
		);
}

INLINE_ONLY void Vector3PaddedData::Z(float z)
{
	asm (
		" ### init Vector3::Z with a float ### 1/1 ### \n\t"
		"ctc2	f_z, $vi21                       \n\t"
		"vnop                                  \n\t"
		"vaddi.z	this, vf00, I"
		: "+j this" (_vec)
		: "r f_z" (z)
		);
}

INLINE_ONLY void Vector3PaddedData::W(float w)
{
	asm (
		" ### init Vector3::W with a float ### 1/1 ### \n\t"
		"ctc2	f_w, $vi21                       \n\t"
		"vnop                                  \n\t"
		"vmuli.w	this, vf00, I"
		: "+j this" (_vec)
		: "r f_w" (w)
		);
}

INLINE_ONLY Vector4X Vector3PaddedData::x() const { return X(); }
INLINE_ONLY Vector4Y Vector3PaddedData::y() const { return Y(); }
INLINE_ONLY Vector4Z Vector3PaddedData::z() const { return Z(); }
INLINE_ONLY Vector4W Vector3PaddedData::w() const { return W(); }
INLINE_ONLY u32 Vector3PaddedData::pad() const { return Pad(); }
INLINE_ONLY void Vector3PaddedData::pad(u32 pad) { Pad(pad); }

INLINE_ONLY void Vector3PaddedData::x(Vector4X x) { X(x); }
INLINE_ONLY void Vector3PaddedData::x(Vector4Y x) { X(x); }
INLINE_ONLY void Vector3PaddedData::x(Vector4Z x) { X(x); }
INLINE_ONLY void Vector3PaddedData::x(Vector4W x) { X(x); }
INLINE_ONLY void Vector3PaddedData::x(float x) { X(x); }

INLINE_ONLY void Vector3PaddedData::y(Vector4X y) { Y(y); }
INLINE_ONLY void Vector3PaddedData::y(Vector4Y y) { Y(y); }
INLINE_ONLY void Vector3PaddedData::y(Vector4Z y) { Y(y); }
INLINE_ONLY void Vector3PaddedData::y(Vector4W y) { Y(y); }
INLINE_ONLY void Vector3PaddedData::y(float y) { Y(y); }

INLINE_ONLY void Vector3PaddedData::z(Vector4X z) { Z(z); }
INLINE_ONLY void Vector3PaddedData::z(Vector4Y z) { Z(z); }
INLINE_ONLY void Vector3PaddedData::z(Vector4Z z) { Z(z); }
INLINE_ONLY void Vector3PaddedData::z(Vector4W z) { Z(z); }
INLINE_ONLY void Vector3PaddedData::z(float z) { Z(z); }

INLINE_ONLY void Vector3PaddedData::w(Vector4X w) { W(w); }
INLINE_ONLY void Vector3PaddedData::w(Vector4Y w) { W(w); }
INLINE_ONLY void Vector3PaddedData::w(Vector4Z w) { W(w); }
INLINE_ONLY void Vector3PaddedData::w(Vector4W w) { W(w); }
INLINE_ONLY void Vector3PaddedData::w(float w) { W(w); }


#undef INLINE_ONLY
