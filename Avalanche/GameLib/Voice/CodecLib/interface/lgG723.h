
#ifndef _LGG723_H_
#define _LGG723_H_

#include "Voice/CodecLib/G723.1/g72x.h"
#include "liblgcodec.h"

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

typedef struct G723_24
{
    lgCodecHeader header;
    struct g72x_state* encode_state;
    struct g72x_state* decode_state;    
    short encode_in[G723_24_SAMPLES_PER_FRAME];
    int encode_in_samples;
    unsigned char decode_in[G723_24_BYTES_PER_EFRAME];
    int decode_in_bytes;
}G723_24;

int lgCodecG72324Encode(G723_24* g723_24, const short* src, const int src_size, u_char* dest, int* dest_size);
int lgCodecG72324Decode(G723_24* g723_24, const u_char* src, const int src_size, short* dest, int* dest_size);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif

