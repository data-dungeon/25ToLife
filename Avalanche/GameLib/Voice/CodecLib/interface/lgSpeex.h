
#ifndef _LGSPEEX_H_
#define _LGSPEEX_H_

#include "Voice/CodecLib/SPEEX/speex.h"
#include "liblgcodec.h"

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#define SPEEX8_SAMPLES_PER_FRAME    160
#define SPEEX8_BYTES_PER_EFRAME     20

typedef struct SPEEX8
{
    lgCodecHeader header;
    void* encode_state; 
    void* decode_state; 
    float float_buffer[SPEEX8_SAMPLES_PER_FRAME];
    short encode_in[SPEEX8_SAMPLES_PER_FRAME];
    int encode_in_samples;
    unsigned char decode_in[SPEEX8_BYTES_PER_EFRAME];
    int decode_in_bytes;
    SpeexBits speex_bits;
}SPEEX8;

int lgCodecSpeex8Encode(SPEEX8* speex, const short* src, const int src_size, u_char* dest, int* dest_size);
int lgCodecSpeex8Decode(SPEEX8* speex, const u_char* src, const int src_size, short* dest, int* dest_size);


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif

