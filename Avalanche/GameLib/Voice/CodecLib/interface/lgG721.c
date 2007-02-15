
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "utils.h"
#include "lgpriv.h"

// codecs
#include "lgG721.h"


int g721_frame_encode(short* in, unsigned char* out, void* enc_param);
int g721_frame_decode(unsigned char* in, short* out, void* dec_param);



int g721_frame_encode(short* in, unsigned char* out, void* enc_param)
{
    G721* g721 = (G721*)enc_param;
    return g721_encode_window(in, 
                                out,
                                g721->encode_state);
    //return adpcm_coder(in, out, G721_SAMPLES_PER_FRAME, g721->encode_state);
}

int g721_frame_decode(unsigned char* in, short* out, void* dec_param)
{
    G721* g721 = (G721*)dec_param;
    return g721_decode_window(in, 
                                out,
                                g721->decode_state);
    //return adpcm_decoder(in, out, G721_BYTES_PER_EFRAME, g721->decode_state);
}

int lgCodecG721Encode(G721* g721, const short* src, const int src_size, u_char* dest, int* dest_size)
{
    return lgFrameEncode(src, src_size, dest, dest_size,
                         g721->encode_in, sizeof(g721->encode_in), &g721->encode_in_samples, 
                         G721_SAMPLES_PER_FRAME, G721_BYTES_PER_EFRAME, 
                         g721_frame_encode, g721);
}

int lgCodecG721Decode(G721* g721, const u_char* src, const int src_size, short* dest, int* dest_size)
{

    return lgFrameDecode(src, src_size, dest, dest_size,
                         g721->decode_in, sizeof(g721->decode_in), &g721->decode_in_bytes, 
                         G721_SAMPLES_PER_FRAME, G721_BYTES_PER_EFRAME, 
                         g721_frame_decode, g721);
}

