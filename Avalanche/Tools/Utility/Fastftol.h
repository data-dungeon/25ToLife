#pragma once

#define FAST_ROUND(fx,x) \
   { \
   	float __fx = (fx); \
      _asm fld __fx \
	   _asm fistp x \
   } 

inline int fast_round( float d ) // this the fastest
{
   int result;
   FAST_ROUND( d, result );
   return result;
}

inline int fast_ftol_unsigned( float d ) // extract the long from the IEEE float
{
// SXXX XXXX   XMMM MMMM   MMMM MMMM   MMMM MMMM
   ULONG *ptr = (ULONG *)&d;
   int exp = ((*ptr & 0x7F800000) >> 23) - 127;
   int man =  (*ptr & 0x007FFFFF) | 0x00800000;
   exp = 23 - exp;
   if (exp > 0) {
      if (exp > 31)
         return 0;
      else
         return man >> exp;
   }
   else
      return man << -exp;
}

inline int fast_ftol_signed( float d ) // extract the long from the IEEE float
{
// SXXX XXXX   XMMM MMMM   MMMM MMMM   MMMM MMMM
   ULONG *ptr = (ULONG *)&d;
   int exp = ((*ptr & 0x7F800000) >> 23) - 127;
   int man =  (*ptr & 0x007FFFFF) | 0x00800000;
   exp = 23 - exp;
   if (*ptr & 0x80000000) { // signed
      if (exp > 0) {
         if (exp > 31)
            return 0;
         else
            return -(man >> exp);
      }
      else
         return -(man << -exp);
   }
   else {
      if (exp > 0) {
         if (exp > 31)
            return 0;
         else
            return man >> exp;
      }
      else
         return man << -exp;
   }   
}

inline int fast_floor( float d ) // extract the long from the IEEE float
{
// SXXX XXXX   XMMM MMMM   MMMM MMMM   MMMM MMMM
   ULONG *ptr = (ULONG *)&d;
   int exp = ((*ptr & 0x7F800000) >> 23) - 127;
   int man =  (*ptr & 0x007FFFFF) | 0x00800000;
   exp = 23 - exp;
   if (*ptr & 0x80000000) { // signed
      if (exp > 0) {
         if (exp > 31)
            return -1;
         int i = man >> exp;
         if ((i << exp) == man)
            return -i;
         else
            return -(i+1);
      }
      else
         return -(man << -exp);
   }
   else {
      if (exp > 0) {
         if (exp > 31)
            return 0;
         else
            return man >> exp;
      }
      else
         return man << -exp;
   }   
}

inline __declspec(naked) float __fastcall fast_InverseSqrt(float a)
{
	__asm {
			mov		eax, 0be6eb508h
			mov		DWORD PTR [esp-12],03fc00000h ;  1.5 on the stack
			sub		eax, DWORD PTR [esp+4]; a
			sub		DWORD PTR [esp+4], 800000h ; a/2 a=Y0
			shr		eax, 1     ; firs approx in eax=R0
			mov		DWORD PTR [esp-8], eax
			
			fld		DWORD PTR [esp-8] ;r
			fmul	st, st            ;r*r
			fld		DWORD PTR [esp-8] ;r
			fxch	st(1)
			fmul	DWORD PTR [esp+4];a ;r*r*y0
			fld		DWORD PTR [esp-12];load 1.5
			fld		st(0)
			fsub	st,st(2)			   ;r1 = 1.5 - y1
			;x1 = st(3)
			;y1 = st(2)
			;1.5 = st(1)
			;r1 = st(0)
			
			fld		st(1)
			fxch	st(1)
			fmul	st(3),st			; y2=y1*r1*...
			fmul	st(3),st			; y2=y1*r1*r1
			fmulp	st(4),st            ; x2=x1*r1
			fsub	st,st(2)               ; r2=1.5-y2
			;x2=st(3)
			;y2=st(2)
			;1.5=st(1)
			;r2 = st(0)
			
			fmul	st(2),st			;y3=y2*r2*...
			fmul	st(3),st			;x3=x2*r2
			fmulp	st(2),st			;y3=y2*r2*r2
			fxch	st(1)
			fsubp	st(1),st			;r3= 1.5 - y3
			;x3 = st(1)
			;r3 = st(0)
			fmulp	st(1), st
			ret 4
	}
}
