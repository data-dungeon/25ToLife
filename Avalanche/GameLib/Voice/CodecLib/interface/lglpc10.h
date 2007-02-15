
#ifndef _LGLPC10_H_
#define _LGLPC10_H_

#include "Voice/CodecLib/LPC10/lpc10.h"
#include "liblgcodec.h"

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#define LPC10_SAMPLES_PER_FRAME   180
#define LPC10_BYTES_PER_EFRAME    7

typedef struct LPC10
{
    lgCodecHeader header;
    struct lpc10_encoder_state* encode_state;
    struct lpc10_decoder_state* decode_state;    
    short encode_in[LPC10_SAMPLES_PER_FRAME];
    int encode_in_samples;
    unsigned char decode_in[LPC10_BYTES_PER_EFRAME];
    int decode_in_bytes;
}LPC10;

int lgCodecLPC10Encode(LPC10* lpc10, const short* src, const int src_size, u_char* dest, int* dest_size);
int lgCodecLPC10Decode(LPC10* lpc10, const u_char* src, const int src_size, short* dest, int* dest_size);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif

