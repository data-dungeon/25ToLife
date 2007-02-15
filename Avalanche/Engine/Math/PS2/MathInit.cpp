/******************* includes ****************************/

#include "Math/MathPCH.h"

#if defined(PS2) && defined(SN)
extern char Math_DmaTag[];

void Math::UploadVU0Microcode(void)
{
	// wait for channel to be open
	DMA_Wait(D0_CHCR);

	// send dma chain
	*D0_TADR = (unsigned int)&Math_DmaTag;
	*D0_QWC = 0;
	asm("sync.l");
	asm("sync.p");
	*D0_CHCR = D_CHCR_SCHAIN_MODE_M | D_CHCR_STR_M;

	// wait for channel to be open again
	DMA_Wait(D0_CHCR);
}

float Math::ArcTanBase(float x)
{
	float result;
	asm __volatile__ (
		"lui         t0,0x3F80           \r\n"
		"lui         t1,0xBEAA           \r\n"
		"mtc1        t0,f1               \r\n"
		"ori         t1,t1,0xA61C        \r\n"
		"mtc1        t1,f2               \r\n"
		"lui         t0,0x3F7F           \r\n"
		"add.s       f3,f12,f1           \r\n"
		"ori         t0,t0,0xFFF5        \r\n"
		"sub.s       f12,f12,f1          \r\n"
		"lui         t1,0x3E4C           \r\n"
		"mtc1        t0,f0               \r\n"
		"ori         t1,t1,0x40A6        \r\n"
		"mtc1        t1,f4               \r\n"
		"lui         t0,0xBE0E           \r\n"
		"lui         t1,0x3DC5           \r\n"
		"ori         t0,t0,0x6C63        \r\n"
		"div.s       f12,f12,f3          \r\n"
		"ori         t1,t1,0x77DF        \r\n"
		"mtc1        t0,f5               \r\n"
		"lui         t0,0xBD65           \r\n"
		"mtc1        t1,f6               \r\n"
		"ori         t0,t0,0x1C4         \r\n"
		"mtc1        t0,f7               \r\n"
		"lui         t0,0x3CB3           \r\n"
		"ori         t0,t0,0x1652        \r\n"
		"lui         t1,0xBB84           \r\n"
		"mtc1        t0,f9               \r\n"
		"ori         t1,t1,0xD7E7        \r\n"
		"lui         t0,0x3F49           \r\n"
		"mtc1        t1,f8               \r\n"
		"ori         t0,t0,0xFDB         \r\n"
		"mtc1        t0,f10              \r\n"
		"mul.s       f3,f12,f12          \r\n"
		"mula.s      f12,f0              \r\n"
		"mul.s       f1,f3,f12           \r\n"
		"madda.s     f1,f2               \r\n"
		"mul.s       f1,f1,f3            \r\n"
		"madda.s     f1,f4               \r\n"
		"mul.s       f1,f1,f3            \r\n"
		"mul.s       f2,f1,f3            \r\n"
		"madda.s     f1,f5               \r\n"
		"mul.s       f1,f1,f1            \r\n"
		"madda.s     f2,f6               \r\n"
		"mul.s       f2,f2,f3            \r\n"
		"mul.s       f1,f1,f12           \r\n"
		"madda.s     f2,f7               \r\n"
		"mul.s       f2,f2,f3            \r\n"
		"madda.s     f1,f8               \r\n"
		"madd.s      f0,f2,f9            \r\n"
		"add.s       %0,f0,f10           \r\n"
			: "=f" (result));
	return result;
}

#endif

#if defined(PS2) && defined(SN)
extern "C" {
float cosf(float)
{
	// depricated.  please use Math::
	ASSERT(false);
}
double cos(double)
{
	// depricated.  please use Math::
	ASSERT(false);
}
float sinf(float)
{
	// depricated.  please use Math::
	ASSERT(false);
}
double sin(double)
{
	// depricated.  please use Math::
	ASSERT(false);
}
float sincosf(float)
{
	// depricated.  please use Math::
	ASSERT(false);
}
double sincos(double)
{
	// depricated.  please use Math::
	ASSERT(false);
}
float atanf(float)
{
	// depricated.  please use Math::
	ASSERT(false);
}
float acosf(float)
{
	// depricated.  please use Math::
	ASSERT(false);
}
double atan(double)
{
	// depricated.  please use Math::
	ASSERT(false);
}
double acos(double)
{
	// depricated.  please use Math::
	ASSERT(false);
}
}
#endif

