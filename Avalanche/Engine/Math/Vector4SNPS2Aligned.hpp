
#define INLINE_ONLY extern inline


INLINE_ONLY Vector4X Vector4AlignedData::X() const
{
	return Vector4X(_vec);
}

INLINE_ONLY Vector4Y Vector4AlignedData::Y() const
{
	return Vector4Y(_vec);
}

INLINE_ONLY Vector4Z Vector4AlignedData::Z() const 
{
	return Vector4Z(_vec);
}

INLINE_ONLY Vector4W Vector4AlignedData::W() const 
{
	return Vector4W(_vec);
}

INLINE_ONLY void Vector4AlignedData::InitX(Vector4X v)
{
	_vec = v._vec;
}

INLINE_ONLY void Vector4AlignedData::X(Vector4X v)
{
	asm (
		" ### init Vector4::x with Vector4X ### 1/1 ###\n\t"
		"vaddx.x this, vf00, vx"
		: "+j this" (_vec)
		: "j vx" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::Y(Vector4X v)
{
	asm (
		" ### init Vector4::y with Vector4X ### 1/1 ###\n\t"
		"vaddx.y this, vf00, vx"
		: "+j this" (_vec)
		: "j vx" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::Z(Vector4X v)
{
	asm (
		" ### init Vector4::z with Vector4X ### 1/1 ###\n\t"
		"vaddx.z this, vf00, vx"
		: "+j this" (_vec)
		: "j vx" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::W(Vector4X v)
{
	asm (
		" ### init Vector4::w with Vector4X ### 1/1 ###\n\t"
		"vmulx.w this, vf00, vx"
		: "+j this" (_vec)
		: "j vx" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::X(Vector4Y v)
{
	asm (
		" ### init Vector4::x with Vector4Y ### 1/1 ###\n\t"
		"vaddy.x this, vf00, vy"
		: "+j this" (_vec)
		: "j vy" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::Y(Vector4Y v)
{
	asm (
		" ### init Vector4::y with Vector4Y ### 1/1 ###\n\t"
		"vaddy.y this, vf00, vy"
		: "+j this" (_vec)
		: "j vy" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::Z(Vector4Y v)
{
	asm (
		" ### init Vector4::z with Vector4Y ### 1/1 ###\n\t"
		"vaddy.z this, vf00, vy"
		: "+j this" (_vec)
		: "j vy" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::W(Vector4Y v)
{
	asm (
		" ### init Vector4::w with Vector4Y ### 1/1 ###\n\t"
		"vmuly.w this, vf00, vy"
		: "+j this" (_vec)
		: "j vy" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::X(Vector4Z v)
{
	asm (
		" ### init Vector4::x with Vector4Z ### 1/1 ###\n\t"
		"vaddz.x this, vf00, vz"
		: "+j this" (_vec)
		: "j vz" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::Y(Vector4Z v)
{
	asm (
		" ### init Vector4::y with Vector4Z ### 1/1 ###\n\t"
		"vaddz.y this, vf00, vz"
		: "+j this" (_vec)
		: "j vz" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::Z(Vector4Z v)
{
	asm (
		" ### init Vector4::z with Vector4Z ### 1/1 ###\n\t"
		"vaddz.z this, vf00, vz"
		: "+j this" (_vec)
		: "j vz" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::W(Vector4Z v)
{
	asm (
		" ### init Vector4::w with Vector4Z ### 1/1 ###\n\t"
		"vmulz.w this, vf00, vz"
		: "+j this" (_vec)
		: "j vz" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::X(Vector4W v)
{
	asm (
		" ### init Vector4::x with Vector4W ### 1/1 ###\n\t"
		"vaddw.x this, vf00, vw"
		: "+j this" (_vec)
		: "j vw" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::Y(Vector4W v)
{
	asm (
		" ### init Vector4::y with Vector4W ### 1/1 ###\n\t"
		"vaddw.y this, vf00, vw"
		: "+j this" (_vec)
		: "j vw" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::Z(Vector4W v)
{
	asm (
		" ### init Vector4::z with Vector4W ### 1/1 ###\n\t"
		"vaddw.z this, vf00, vw"
		: "+j this" (_vec)
		: "j vw" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::W(Vector4W v)
{
	asm (
		" ### init Vector4::w with Vector4W ### 1/1 ###\n\t"
		"vmulw.w this, vf00, vw"
		: "+j this" (_vec)
		: "j vw" (v._vec));
}

INLINE_ONLY void Vector4AlignedData::X(float x)
{
	asm (
		" ### init Vector4::x with a float ### 1/1 ### \n\t"
		"ctc2	f_x, $vi21                       \n\t"
		"vnop                                  \n\t"
		"vaddi.x	this, vf00, I"
		: "+j this" (_vec)
		: "r f_x" (x)
		);
}

INLINE_ONLY void Vector4AlignedData::Y(float y)
{
	asm (
		" ### init Vector4::y with a float ### 1/1 ### \n\t"
		"ctc2	f_y, $vi21                       \n\t"
		"vnop                                  \n\t"
		"vaddi.y	this, vf00, I"
		: "+j this" (_vec)
		: "r f_y" (y)
		);
}

INLINE_ONLY void Vector4AlignedData::Z(float z)
{
	asm (
		" ### init Vector4::z with a float ### 1/1 ### \n\t"
		"ctc2	f_z, $vi21                       \n\t"
		"vnop                                  \n\t"
		"vaddi.z	this, vf00, I"
		: "+j this" (_vec)
		: "r f_z" (z)
		);
}

INLINE_ONLY void Vector4AlignedData::W(float w)
{
	asm (
		" ### init Vector4::w with a float ### 1/1 ### \n\t"
		"ctc2	f_w, $vi21                       \n\t"
		"vnop                                  \n\t"
		"vmuli.w	this, vf00, I"
		: "+j this" (_vec)
		: "r f_w" (w)
		);
}


// TJC - old names
INLINE_ONLY Vector4X Vector4AlignedData::x() const	{ return X(); }
INLINE_ONLY Vector4Y Vector4AlignedData::y() const	{ return Y(); }
INLINE_ONLY Vector4Z Vector4AlignedData::z() const	{ return Z(); }
INLINE_ONLY Vector4W Vector4AlignedData::w() const	{ return W(); }

INLINE_ONLY void Vector4AlignedData::x(Vector4X x)	{ X(x); }
INLINE_ONLY void Vector4AlignedData::x(Vector4Y x)	{ X(x); }
INLINE_ONLY void Vector4AlignedData::x(Vector4Z x)	{ X(x); }
INLINE_ONLY void Vector4AlignedData::x(Vector4W x)	{ X(x); }
INLINE_ONLY void Vector4AlignedData::x(float x)		{ X(x); }

INLINE_ONLY void Vector4AlignedData::y(Vector4X y)	{ Y(y); }
INLINE_ONLY void Vector4AlignedData::y(Vector4Y y)	{ Y(y); }
INLINE_ONLY void Vector4AlignedData::y(Vector4Z y)	{ Y(y); }
INLINE_ONLY void Vector4AlignedData::y(Vector4W y)	{ Y(y); }
INLINE_ONLY void Vector4AlignedData::y(float y)		{ Y(y); }

INLINE_ONLY void Vector4AlignedData::z(Vector4X z)	{ Z(z); }
INLINE_ONLY void Vector4AlignedData::z(Vector4Y z)	{ Z(z); }
INLINE_ONLY void Vector4AlignedData::z(Vector4Z z)	{ Z(z); }
INLINE_ONLY void Vector4AlignedData::z(Vector4W z)	{ Z(z); }
INLINE_ONLY void Vector4AlignedData::z(float z)		{ Z(z); }

INLINE_ONLY void Vector4AlignedData::w(Vector4X w)	{ W(w); }
INLINE_ONLY void Vector4AlignedData::w(Vector4Y w)	{ W(w); }
INLINE_ONLY void Vector4AlignedData::w(Vector4Z w)	{ W(w); }
INLINE_ONLY void Vector4AlignedData::w(Vector4W w)	{ W(w); }
INLINE_ONLY void Vector4AlignedData::w(float w)		{ W(w); }


#undef INLINE_ONLY
