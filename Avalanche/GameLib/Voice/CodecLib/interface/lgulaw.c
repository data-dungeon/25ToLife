
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "lgpriv.h"
#include "utils.h"

// codecs
#include "lgulaw.h"



int lgCodecUlawEncode(const short* src, const int src_size, u_char* dest, int* dest_size)
{

    int length = 0;

    if (dest_size == NULL)
    {
        printf("error 1\n");
        return LGCODEC_ERR_INVALID_PARAMETER;
    }
    if (src_size == 0 || src == NULL || dest == NULL)
    {
        *dest_size = 0;
        printf("error 2\n");
        return LGCODEC_ERR_INVALID_PARAMETER;
    }
    if (src_size % sizeof(short) != 0)
    {
        *dest_size = 0;
        printf("error 3\n");
        return LGCODEC_ERR_INVALID_PARAMETER;
    }
    if (src_size == 0 || *dest_size == 0)
    {
        *dest_size = 0;
        return LGCODEC_SUCCESS;
    }

    // the destination must be at least half the size of the source
    if (*dest_size < (src_size/2))
        printf("ERROR: lgCodecUlawEncode: need %d bytes. have %d bytes. clipping\n", src_size/2, *dest_size);

    length = min(src_size/2, *dest_size);

    // ulawEncode returns the number of samples
    *dest_size = ulawEncode(src, dest, length);
    
    return LGCODEC_SUCCESS;
}

//************************************************************************
// lgCodecUlawDecode
//************************************************************************
int lgCodecUlawDecode(const u_char* src, const int src_size, short* dest, int* dest_size)
{
        
    int length = 0;

    // check for weirdness
    if (dest_size == NULL)
        return LGCODEC_ERR_INVALID_PARAMETER;
    if (src_size == 0 || src == NULL || dest == NULL)
    {
        *dest_size = 0;
        return LGCODEC_ERR_INVALID_PARAMETER;
    }
    if (*dest_size % sizeof(short) != 0)
    {
        *dest_size = 0;
        return LGCODEC_ERR_INVALID_PARAMETER;
    }
    if (src_size == 0 || *dest_size == 0)
    {
        *dest_size = 0;
        return LGCODEC_SUCCESS;
    }

    // the destination must be at least twice the size of the source
    if ((*dest_size) < (2 * src_size))
        printf("ERROR: lgCodecUlawEncode: need %d bytes. have %d bytes. clipping\n", src_size*2, *dest_size);

    length = min(src_size, *dest_size/2);

    // ulawDecode returns sample count
    *dest_size = ulawDecode(src, dest, length) * sizeof(short);

    return LGCODEC_SUCCESS;
}

