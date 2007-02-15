/*
#ifdef _MSC_VER
__inline int round (float flt)
{
    int intgr;
    
    _asm
    {
        fld flt
        fistp intgr
    };
    
    return intgr ;
}
#else
#ifdef __GNUC__

#define _ISOC9X_SOURCE
#define _ISOC99_SOURCE
#define __USE_ISOC9X
#define __USE_ISOC99
#include <math.h>
#define round(x)    lrintf(x)
#else
*/
#define FP_BITS(fp) (*(int *)&(fp))
#define FIST_FLOAT_MAGIC_S (float)(7.0f * 2097152.0f)

static int round(float inval)
{
    float tmp = FIST_FLOAT_MAGIC_S + inval;
    int res = ((FP_BITS(tmp)<<10)-0x80000000);
    return res>>10;
}
//#endif
//#endif

