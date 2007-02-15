
#define INLINE_ONLY extern inline 

INLINE_ONLY Vector4X::Vector4X(u128 v)
{
	_vec = v;
}
INLINE_ONLY Vector4Y::Vector4Y(u128 v)
{
	_vec = v;
}
INLINE_ONLY Vector4Z::Vector4Z(u128 v)
{
	_vec = v;
}
INLINE_ONLY Vector4W::Vector4W(u128 v)
{
	_vec = v;
}

INLINE_ONLY Vector4X::Vector4X(Vector4Y val)
{
	u128 dest;

	asm(
		"### Vector4X from Vector4Y ### \n\t"
		"vaddy.x dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}
INLINE_ONLY Vector4X::Vector4X(Vector4Z val)
{
	u128 dest;

	asm(
		"### Vector4X from Vector4Z ### \n\t"
		"vaddz.x dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}
INLINE_ONLY Vector4X::Vector4X(Vector4W val)
{
	u128 dest;

	asm(
		"### Vector4X from Vector4W ### \n\t"
		"vaddw.x dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}

INLINE_ONLY Vector4Y::Vector4Y(Vector4X val)
{
	u128 dest;

	asm(
		"### Vector4Y from Vector4X ### \n\t"
		"vaddx.y dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}
INLINE_ONLY Vector4Y::Vector4Y(Vector4Z val)
{
	u128 dest;

	asm(
		"### Vector4Y from Vector4Z ### \n\t"
		"vaddz.y dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}
INLINE_ONLY Vector4Y::Vector4Y(Vector4W val)
{
	u128 dest;

	asm(
		"### Vector4Y from Vector4W ### \n\t"
		"vaddw.y dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}

INLINE_ONLY Vector4Z::Vector4Z(Vector4X val)
{
	u128 dest;

	asm(
		"### Vector4Z from Vector4X ### \n\t"
		"vaddx.z dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}
INLINE_ONLY Vector4Z::Vector4Z(Vector4Y val)
{
	u128 dest;

	asm(
		"### Vector4Z from Vector4Y ### \n\t"
		"vaddy.z dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}
INLINE_ONLY Vector4Z::Vector4Z(Vector4W val)
{
	u128 dest;

	asm(
		"### Vector4Z from Vector4W ### \n\t"
		"vaddw.z dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}

INLINE_ONLY Vector4W::Vector4W(Vector4X val)
{
	u128 dest;

	asm(
		"### Vector4W from Vector4X ### \n\t"
		"vmulx.w dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}
INLINE_ONLY Vector4W::Vector4W(Vector4Y val)
{
	u128 dest;

	asm(
		"### Vector4W from Vector4Y ### \n\t"
		"vmuly.w dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}
INLINE_ONLY Vector4W::Vector4W(Vector4Z val)
{
	u128 dest;

	asm(
		"### Vector4W from Vector4Z ### \n\t"
		"vmulz.w dest, vf00, src"
		: "=j dest" (dest)
		: "j src" (val._vec));

	_vec = dest;
}

INLINE_ONLY Vector4X::Vector4X(float val)
{
	u128 vec;
	
	asm (
		" ### Vector4X from a float ###  \n\t"
		"  .if %A0                       \n\t"
		"  move this, new_x              \n\t"
		"  .endif                        \n\t"
		"                                \n\t"
		"  .if %A1                       \n\t"
		"  lw this, new_x                \n\t"
		"  .endif                        \n\t"
		"                                \n\t"
		"  .if %A2                       \n\t"
		"  mfc1 this, new_x              \n\t"
		"  .endif                        \n\t"
		"                                \n\t"
		"  .if %A3                       \n\t"
		"  qmtc2 new_x, this             \n\t"
		"  .endif"
		: "=r,r,r,j this" (vec)
		: "r,m,f,r new_x" (val)
		);
		
	_vec = vec;
}
INLINE_ONLY Vector4Y::Vector4Y(float val)
{
	u128 vec;
	
	asm (
		" ### Vector4Y from a float ###  \n\t"
		" dsll32 this, new_y, 0"
		: "=r this" (vec)
		: "r new_y" (val)
		);
		
	_vec = vec;
}
INLINE_ONLY Vector4Z::Vector4Z(float val)
{
	u128 vec;
	
	asm (
		" ### Vector4Z from a float ###  \n\t"
		" prot3w this, new_z"
		: "=r this" (vec)
		: "r new_z" (val)
		);
		
	_vec = vec;
}
INLINE_ONLY Vector4W::Vector4W(float val)
{
	u128 vec;
	
	asm (
		" ### Vector4W from a float ###  \n\t"
		" pextlw this, new_w, $0"
		: "=r this" (vec)
		: "r new_w" (val));

	asm (
		"pextlw this, src, $0"
		: "=r this" (vec)
		: "r src" (vec));
		
	_vec = vec;
}

INLINE_ONLY Vector4X::operator float()
{
	float new_float;

	asm(
		" ### Vector4X to float ###           \n\t"
		"  .if %A0                            \n\t"
		"  move new_float, this               \n\t"
		"  .endif                             \n\t"
		"                                     \n\t"
		"  .if %A1                            \n\t"
		"  sw this, new_float                 \n\t"
		"  .endif                             \n\t"
		"                                     \n\t"
		"  .if %A2                            \n\t"
		"  mtc1 this, new_float               \n\t"
		"  .endif                             \n\t"
		"                                     \n\t"
		"  .if %A3                            \n\t"
		"  qmfc2 new_float, this              \n\t"
		"  .endif"
		: "=r,m,f,r new_float" (new_float)
		: "r,r,r,j this" (_vec));
		
	return new_float;
}

INLINE_ONLY Vector4Y::operator float()
{
	float new_float;
	asm(
		" ### Vector4Y to float ###           \n\t"
		"prot3w	new_float, this"
		: "=r new_float" (new_float)
		: "r this" (_vec));

	return new_float;
}

INLINE_ONLY Vector4Z::operator float()
{
	float new_float;
	asm (
		" ### Vector4Z to float ###           \n\t"
		"pextuw	new_float, $0, this"
		: "=r new_float" (new_float)
		: "r this" (_vec));

	return new_float;
}

INLINE_ONLY Vector4W::operator float()
{
	float new_float;
	u128 temp;
	
	asm (
		" ### Vector4W to float ### \n\t"
		"pextuw	temp, $0, this"
		: "=r temp" (temp)
		: "r this" (_vec));

	asm("pextuw new_float, $0, temp"
		 : "=r new_float" (new_float)
		 : "r temp" (temp));

	return new_float;
}

// TJC - negation operators
INLINE_ONLY Vector4X Vector4X::operator-(void)
{
	u128 dest;

	asm(
		" ### Vector4X::operator-(void) ### \n\t"
		"vsubx.x dest, vf0, src"
		: "=j dest" (dest)
		: "j src" (_vec));

	return Vector4X(dest);
}
INLINE_ONLY Vector4X Vector4Y::operator-(void)
{
	u128 dest;

	asm(
		" ### Vector4Y::operator-(void) ### \n\t"
		"vsuby.x dest, vf0, src"
		: "=j dest" (dest)
		: "j src" (_vec));

	return Vector4X(dest);
}
INLINE_ONLY Vector4X Vector4Z::operator-(void)
{
	u128 dest;

	asm(
		" ### Vector4Z::operator-(void) ### \n\t"
		"vsubz.x dest, vf0, src"
		: "=j dest" (dest)
		: "j src" (_vec));

	return Vector4X(dest);
}
INLINE_ONLY Vector4X Vector4W::operator-(void)
{
	u128 dest;

	asm(
		" ### Vector4W::operator-(void) ### \n\t"
		"vsubw.x dest, vf0, src"
		: "=j dest" (dest)
		: "j src" (_vec));

	return Vector4X(dest);
}

// TJC - ok, now the base operators...

#define OPERATOR(oper, instr, lhstype, rhstype)								\
INLINE_ONLY lhstype operator oper(lhstype lhs, rhstype rhs)				\
{																							\
	u128 dest;																			\
																							\
	asm(																					\
		" ### operator" #oper "(" # lhstype ", " # rhstype ") ###\n\t"	\
		" " #instr " dest, lhs, rhs"												\
		: "=j dest" (dest)															\
		: "j lhs" (lhs), "j rhs" (rhs));											\
																							\
	return lhstype(dest);															\
}

OPERATOR(+, vaddx.x, Vector4X, Vector4X)
OPERATOR(+, vaddy.x, Vector4X, Vector4Y)
OPERATOR(+, vaddz.x, Vector4X, Vector4Z)
OPERATOR(+, vaddw.x, Vector4X, Vector4W)

OPERATOR(-, vsubx.x, Vector4X, Vector4X)
OPERATOR(-, vsuby.x, Vector4X, Vector4Y)
OPERATOR(-, vsubz.x, Vector4X, Vector4Z)
OPERATOR(-, vsubw.x, Vector4X, Vector4W)

OPERATOR(*, vmulx.x, Vector4X, Vector4X)
OPERATOR(*, vmuly.x, Vector4X, Vector4Y)
OPERATOR(*, vmulz.x, Vector4X, Vector4Z)
OPERATOR(*, vmulw.x, Vector4X, Vector4W)

OPERATOR(+, vaddy.y, Vector4Y, Vector4Y)
OPERATOR(+, vaddz.y, Vector4Y, Vector4Z)
OPERATOR(+, vaddw.y, Vector4Y, Vector4W)

OPERATOR(-, vsuby.y, Vector4Y, Vector4Y)
OPERATOR(-, vsubz.y, Vector4Y, Vector4Z)
OPERATOR(-, vsubw.y, Vector4Y, Vector4W)

OPERATOR(*, vmuly.y, Vector4Y, Vector4Y)
OPERATOR(*, vmulz.y, Vector4Y, Vector4Z)
OPERATOR(*, vmulw.y, Vector4Y, Vector4W)

OPERATOR(+, vaddz.z, Vector4Z, Vector4Z)
OPERATOR(+, vaddw.z, Vector4Z, Vector4W)

OPERATOR(-, vsubz.z, Vector4Z, Vector4Z)
OPERATOR(-, vsubw.z, Vector4Z, Vector4W)

OPERATOR(*, vmulz.z, Vector4Z, Vector4Z)
OPERATOR(*, vmulw.z, Vector4Z, Vector4W)

OPERATOR(+, vaddw.w, Vector4W, Vector4W)

OPERATOR(-, vsubw.w, Vector4W, Vector4W)

OPERATOR(*, vmulw.w, Vector4W, Vector4W)

// TJC - transposes of operator-
OPERATOR(-, vsubx.y, Vector4Y, Vector4X)
OPERATOR(-, vsubx.z, Vector4Z, Vector4X)
OPERATOR(-, vsubx.w, Vector4W, Vector4X)
OPERATOR(-, vsuby.z, Vector4Z, Vector4Y)
OPERATOR(-, vsuby.w, Vector4W, Vector4Y)
OPERATOR(-, vsubz.w, Vector4W, Vector4Z)


#undef OPERATOR

// TJC - now the transposed versions of + *

#define OPERATOR(oper, lhstype, rhstype, rettype)				\
INLINE_ONLY rettype operator oper(lhstype lhs, rhstype rhs)	\
{																				\
	return operator oper(rhs, lhs);									\
}

#define OPERATORS(lhstype, rhstype, rettype)		\
	OPERATOR(+, lhstype, rhstype, rettype)			\
	OPERATOR(*, lhstype, rhstype, rettype)

OPERATORS(Vector4Y, Vector4X, Vector4X);
OPERATORS(Vector4Z, Vector4X, Vector4X);
OPERATORS(Vector4W, Vector4X, Vector4X);
OPERATORS(Vector4Z, Vector4Y, Vector4Y);
OPERATORS(Vector4W, Vector4Y, Vector4Y);
OPERATORS(Vector4W, Vector4Z, Vector4Z);

#undef OPERATORS
#undef OPERATOR

// TJC - now the basic operators with floats..
#define OPERATOR(oper, type)										\
INLINE_ONLY Vector4X operator oper(type lhs, float rhs)	\
{																			\
	return lhs oper Vector4X(rhs);								\
}																			\
INLINE_ONLY Vector4X operator oper(float lhs, type rhs)	\
{																			\
	return Vector4X(lhs) oper rhs;								\
}


OPERATOR(+, Vector4X);
OPERATOR(+, Vector4Y);
OPERATOR(+, Vector4Z);
OPERATOR(+, Vector4W);
OPERATOR(-, Vector4X);
OPERATOR(-, Vector4Y);
OPERATOR(-, Vector4Z);
OPERATOR(-, Vector4W);
OPERATOR(*, Vector4X);
OPERATOR(*, Vector4Y);
OPERATOR(*, Vector4Z);
OPERATOR(*, Vector4W);

#undef OPERATOR
/*
// TJC - now divides... ick....
#define OPERATOR(lhstype, rhstype, lhsreg, rhsreg)			\
INLINE_ONLY Vector4X operator/(lhstype lhs, rhstype rhs)	\
{																			\
	u128 dest;															\
	register u128 PS2_VU0_Q asm ("vf0");						\
																			\
	asm(																	\
		"### operator/(" # lhstype ", " #rhstype ") ###"	\
		"vdiv Q, " #lhsreg ", " #rhsreg							\
		: "=j" (PS2_VU0_Q)											\
		: "j lhs" (lhs), "j rhs" (rhs));							\
																			\
	asm(																	\
		"vwaitq"															\
		: "=j" (PS2_VU0_Q)											\
		: "j" (PS2_VU0_Q));											\
																			\
	asm(																	\
		"vaddq.x dest, vf00, Q"										\
		: "=j dest" (dest)											\
		: "j" (PS2_VU0_Q));											\
																			\
	return Vector4X(dest);											\
}

OPERATOR(Vector4X, Vector4X, lhsx, rhsx);
OPERATOR(Vector4X, Vector4Y, lhsx, rhsy);
OPERATOR(Vector4X, Vector4Z, lhsx, rhsz);
OPERATOR(Vector4X, Vector4W, lhsx, rhsw);
OPERATOR(Vector4Y, Vector4X, lhsy, rhsx);
OPERATOR(Vector4Y, Vector4Y, lhsy, rhsy);
OPERATOR(Vector4Y, Vector4Z, lhsy, rhsz);
OPERATOR(Vector4Y, Vector4W, lhsy, rhsw);
OPERATOR(Vector4Z, Vector4X, lhsz, rhsx);
OPERATOR(Vector4Z, Vector4Y, lhsz, rhsy);
OPERATOR(Vector4Z, Vector4Z, lhsz, rhsz);
OPERATOR(Vector4Z, Vector4W, lhsz, rhsw);
OPERATOR(Vector4W, Vector4X, lhsw, rhsx);
OPERATOR(Vector4W, Vector4Y, lhsw, rhsy);
OPERATOR(Vector4W, Vector4Z, lhsw, rhsz);
OPERATOR(Vector4W, Vector4W, lhsw, rhsw);

#undef OPERATOR

// TJC - float divides...
#define OPERATOR(type)											\
INLINE_ONLY Vector4X operator/(type lhs, float rhs)	\
{																		\
	return lhs * Vector4X(1.0f / rhs);						\
}																		\
INLINE_ONLY Vector4X operator/(float lhs, type rhs)	\
{																		\
	return Vector4X(lhs) / rhs;								\
}

OPERATOR(Vector4X);
OPERATOR(Vector4Y);
OPERATOR(Vector4Z);
OPERATOR(Vector4W);

#undef OPERATOR
*/
// TJC - operator= for same types...
#define OPERATOR(type)									\
INLINE_ONLY type type::operator=(type rhs)		\
{																\
	_vec = rhs._vec;										\
	return type(_vec);									\
}

OPERATOR(Vector4X);
OPERATOR(Vector4Y);
OPERATOR(Vector4Z);
OPERATOR(Vector4W);

#undef OPERATOR

// TJC - operator= for different types...
#define OPERATOR(lhstype, rhstype)							\
INLINE_ONLY lhstype lhstype::operator=(rhstype rhs)	\
{																		\
	_vec = lhstype(rhs)._vec;									\
	return lhstype(_vec);										\
}

OPERATOR(Vector4X, Vector4Y);
OPERATOR(Vector4X, Vector4Z);
OPERATOR(Vector4X, Vector4W);
OPERATOR(Vector4X, float);
OPERATOR(Vector4Y, Vector4X);
OPERATOR(Vector4Y, Vector4Z);
OPERATOR(Vector4Y, Vector4W);
OPERATOR(Vector4Y, float);
OPERATOR(Vector4Z, Vector4X);
OPERATOR(Vector4Z, Vector4Y);
OPERATOR(Vector4Z, Vector4W);
OPERATOR(Vector4Z, float);
OPERATOR(Vector4W, Vector4X);
OPERATOR(Vector4W, Vector4Y);
OPERATOR(Vector4W, Vector4Z);
OPERATOR(Vector4W, float);

#undef OPERATOR

// TJC -  and last, the += operators... ick...
#define OPERATORS(base, rhstype)						\
INLINE_ONLY base base::operator+=(rhstype rhs)	\
{																\
	*this = *this + rhs;									\
	return *this;											\
}																\
INLINE_ONLY base base::operator-=(rhstype rhs)	\
{																\
	*this = *this - rhs;									\
	return *this;											\
}																\
INLINE_ONLY base base::operator*=(rhstype rhs)	\
{																\
	*this = *this * rhs;									\
	return *this;											\
}																\
INLINE_ONLY base base::operator/=(rhstype rhs)	\
{																\
	*this = *this / rhs;									\
	return *this;											\
}

#define OPERATORSET(base)								\
	OPERATORS(base, Vector4X)							\
	OPERATORS(base, Vector4Y)							\
	OPERATORS(base, Vector4Z)							\
	OPERATORS(base, Vector4W)							\
	OPERATORS(base, float)

OPERATORSET(Vector4X);
OPERATORSET(Vector4Y);
OPERATORSET(Vector4Z);
OPERATORSET(Vector4W);

#undef OPERATORSET
#undef OPERATORS

#undef INLINE_ONLY
