
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "lgpriv.h"
#include "utils.h"

// codecs
#include "lglpc10.h"


int lpc10_frame_encode(short* in, unsigned char* out, void* enc_param);
int lpc10_frame_decode(unsigned char* in, short* out, void* dec_param);



int lpc10_frame_encode(short* in, unsigned char* out, void* enc_param)
{
    LPC10* lpc10 = (LPC10*)enc_param;
    return lpc10_encode(in, 
                        out,
                        lpc10->encode_state);
}

int lpc10_frame_decode(unsigned char* in, short* out, void* dec_param)
{
    LPC10* lpc10 = (LPC10*)dec_param;
    return lpc10_decode(in, 
                        out,
                        lpc10->decode_state);
}

int lgCodecLPC10Encode(LPC10* lpc10, const short* src, const int src_size, u_char* dest, int* dest_size)
{
    return lgFrameEncode(src, src_size, dest, dest_size,
                         lpc10->encode_in, sizeof(lpc10->encode_in), &lpc10->encode_in_samples, 
                         LPC10_SAMPLES_PER_FRAME, LPC10_BYTES_PER_EFRAME, 
                         lpc10_frame_encode, lpc10);
}

int lgCodecLPC10Decode(LPC10* lpc10, const u_char* src, const int src_size, short* dest, int* dest_size)
{

    return lgFrameDecode(src, src_size, dest, dest_size,
                         lpc10->decode_in, sizeof(lpc10->decode_in), &lpc10->decode_in_bytes, 
                         LPC10_SAMPLES_PER_FRAME, LPC10_BYTES_PER_EFRAME, 
                         lpc10_frame_decode, lpc10);
}

