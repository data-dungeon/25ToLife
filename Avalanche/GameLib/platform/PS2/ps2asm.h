//
// this file impliments some basic operations in inline assembly for SN.
// they are macros to help avoid the compiler choking on references/pointers, etc...
//

// move each piece of a vect3 into the vu0 separately (--> destx[x], desty[x], destz[x])
#define VU0LoadV3(destx, desty, destz, vect)				\
{																		\
	u32 tx, ty, tz;												\
	asm("lw		 %0, %1" : "=r" (tx) : "m" (vect.x));	\
	asm("lw		 %0, %1" : "=r" (ty) : "m" (vect.y));	\
	asm("lw		 %0, %1" : "=r" (tz) : "m" (vect.z));	\
	asm("qmtc2	 %1, %0" : "=j" (destx) : "r" (tx));	\
	asm("qmtc2	 %1, %0" : "=j" (desty) : "r" (ty));	\
	asm("qmtc2	 %1, %0" : "=j" (destz) : "r" (tz));	\
}


// take a vect3 and get its parts into one u128...
#define VU0MergeV3(dest, vect)													\
{																							\
	u32 tx, ty, tz;																	\
	asm("lw		 %0, %1" : "=r" (tx) : "m" (vect.x));						\
	asm("lw		 %0, %1" : "=r" (ty) : "m" (vect.y));						\
	asm("lw		 %0, %1" : "=r" (tz) : "m" (vect.z));						\
	asm("pextlw	 %0, %1, %2" : "=r" (dest) : "r" (tz), "r" (tx));		\
	asm("pextlw	 %0, %1, %2" : "=r" (dest) : "r" (ty), "r" (dest));	\
}


// take a single register vect3 and split it up into where it belongs...
#define VU0SplitV3(dest, vect)										\
{																				\
	asm("addiu	 %0, %1, 0"  : "=r" (dest.x) : "r" (vect));	\
	asm("prot3w	 %0, %1"		 : "=r" (dest.y) : "r" (vect));	\
	asm("pexew	 %0, %1"     : "=r" (dest.z) : "r" (vect));	\
}


// move a value from VU0 register .x into GPR
#define VU0GetFloats(dest, vect) asm("qmfc2 %0, %1" : "=r" (dest) : "j" (vect));

// move a value in a GPR into the FPU (ie, dest = *(float*)&src, only a little cleaner)
#define VU0IntToFloat(dest, src) asm("mtc1	 %1, %0" : "=f" (dest) : "r" (src));


// used to help verify the correctness of inline assembly
// dont use assert as this is often checked in release mode
#define CHECK(a, b) if(Math::Abs((a) - (b)) > 1.0e-5f) asm("break 1");
#define CHECKV3(a, b) {									\
	Vector3 _a = (a);										\
	Vector3 _b = (b);										\
	CHECK(_a.x, _b.x);									\
	CHECK(_a.y, _b.y);									\
	CHECK(_a.z, _b.z);									\
}

