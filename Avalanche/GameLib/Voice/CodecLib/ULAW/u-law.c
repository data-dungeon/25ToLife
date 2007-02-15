
#include "u-law.h"
#include "g711.h"


int ulawEncode(const short *in, unsigned char *out, int length)
{
    int i;

    for(i=0;i<length;i++)
    {
        out[i] = linear2ulaw(in[i]);
    }

    return length;
}


int ulawDecode(const  unsigned char *in, short *out, int length)
{
    int i;

    for(i=0;i<length;i++)
    {
        out[i] = ulaw2linear(in[i]);
    }

    return length;
}

