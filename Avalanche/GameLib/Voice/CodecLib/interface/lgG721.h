
#ifndef _LGG721_H_
#define _LGG721_H_

#include "Voice/CodecLib/G723.1/g72x.h"
//#include "codecs/g723.1/adpcm.h"
#include "liblgcodec.h"

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

typedef struct G721
{
    lgCodecHeader header;
    struct g72x_state* encode_state;
    struct g72x_state* decode_state;
    //struct adpcm_state* encode_state;
    //struct adpcm_state* decode_state;
    short encode_in[G721_SAMPLES_PER_FRAME];
    int encode_in_samples;
    unsigned char decode_in[G721_BYTES_PER_EFRAME];
    int decode_in_bytes;
}G721;

int lgCodecG721Encode(G721* g721, const short* src, const int src_size, u_char* dest, int* dest_size);
int lgCodecG721Decode(G721* g721, const u_char* src, const int src_size, short* dest, int* dest_size);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif

