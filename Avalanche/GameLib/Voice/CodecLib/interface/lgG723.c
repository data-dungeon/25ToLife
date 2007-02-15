
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "utils.h"
#include "lgpriv.h"

// codecs
#include "lgG723.h"


int g723_24_frame_encode(short* in, unsigned char* out, void* enc_param);
int g723_24_frame_decode(unsigned char* in, short* out, void* dec_param);



int g723_24_frame_encode(short* in, unsigned char* out, void* enc_param)
{
    G723_24* g723_24 = (G723_24*)enc_param;
    return g723_24_encode_window(in, 
                                out,
                                g723_24->encode_state);
}

int g723_24_frame_decode(unsigned char* in, short* out, void* dec_param)
{
    G723_24* g723_24 = (G723_24*)dec_param;
    return g723_24_decode_window(in, 
                                out,
                                g723_24->decode_state);
}

int lgCodecG72324Encode(G723_24* g723_24, const short* src, const int src_size, u_char* dest, int* dest_size)
{
    return lgFrameEncode(src, src_size, dest, dest_size,
                         g723_24->encode_in, sizeof(g723_24->encode_in), &g723_24->encode_in_samples, 
                         G723_24_SAMPLES_PER_FRAME, G723_24_BYTES_PER_EFRAME, 
                         g723_24_frame_encode, g723_24);
}

int lgCodecG72324Decode(G723_24* g723_24, const u_char* src, const int src_size, short* dest, int* dest_size)
{

    return lgFrameDecode(src, src_size, dest, dest_size,
                         g723_24->decode_in, sizeof(g723_24->decode_in), &g723_24->decode_in_bytes, 
                         G723_24_SAMPLES_PER_FRAME, G723_24_BYTES_PER_EFRAME, 
                         g723_24_frame_decode, g723_24);
}

