
#ifndef _LGGSM_H_
#define _LGGSM_H_

#include "Voice/CodecLib/GSM/gsm.h"
#include "liblgcodec.h"

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#define GSM_SAMPLES_PER_FRAME   160
#define GSM_BYTES_PER_EFRAME    33

typedef struct GSM
{
    lgCodecHeader header;
    struct gsm_state* encode_state;
    struct gsm_state* decode_state;
    short encode_in[GSM_SAMPLES_PER_FRAME];
    int encode_in_samples;
    unsigned char decode_in[GSM_BYTES_PER_EFRAME];
    int decode_in_bytes;
}GSM;

int lgCodecGSMEncode(GSM* pgsm, const short* src, const int src_size, u_char* dest, int* dest_size);
int lgCodecGSMDecode(GSM* pgsm, const u_char* src, const int src_size, short* dest, int* dest_size);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif

