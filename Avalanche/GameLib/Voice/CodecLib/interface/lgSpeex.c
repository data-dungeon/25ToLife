
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "lgpriv.h"
#include "utils.h"

// codecs
#include "lgSpeex.h"


int speex8_frame_encode(short* in, unsigned char* out, void* enc_param);
int speex8_frame_decode(unsigned char* in, short* out, void* dec_param);



int speex8_frame_encode(short* in, unsigned char* out, void* enc_param)
{
    SPEEX8* codec = (SPEEX8*)enc_param;
    int i = 0;
    int enc_bytes = 0;

    // copy short to float (ugh)
    for (i = 0; i < SPEEX8_SAMPLES_PER_FRAME; i++)
        (codec->float_buffer[i] = (float)in[i]);

    // reset the bits array
    speex_bits_reset(&codec->speex_bits);

    // encode it
    speex_encode(codec->encode_state, codec->float_buffer, &codec->speex_bits);

    // copy the speex bits into the out buffer
    enc_bytes = speex_bits_write(&codec->speex_bits, (char*)out, SPEEX8_BYTES_PER_EFRAME);

    return SPEEX8_BYTES_PER_EFRAME;
}

int speex8_frame_decode(unsigned char* in, short* out, void* dec_param)
{
    SPEEX8* codec = (SPEEX8*)dec_param;
    int i = 0;

    // copy the encoded buffer to the speex bits
    speex_bits_reset(&codec->speex_bits);
    speex_bits_read_from(&codec->speex_bits, (char*)in, SPEEX8_BYTES_PER_EFRAME);

    // decode the float buffer
    speex_decode(codec->decode_state, &codec->speex_bits, codec->float_buffer);

    // copy float to short (ugh)
    for (i = 0; i < SPEEX8_SAMPLES_PER_FRAME; i++)
    {
        int v = ( int )codec->float_buffer[i];
        
        out[i] = min(32767, max(-32768, v));
    }
    
    return SPEEX8_SAMPLES_PER_FRAME;
}

int lgCodecSpeex8Encode(SPEEX8* codec, const short* src, const int src_size, u_char* dest, int* dest_size)
{
    return lgFrameEncode(src, src_size, dest, dest_size,
                         codec->encode_in, sizeof(codec->encode_in), &codec->encode_in_samples,
                         SPEEX8_SAMPLES_PER_FRAME, SPEEX8_BYTES_PER_EFRAME, 
                         speex8_frame_encode, codec);
}

int lgCodecSpeex8Decode(SPEEX8* codec, const u_char* src, const int src_size, short* dest, int* dest_size)
{

    return lgFrameDecode(src, src_size, dest, dest_size,
                         codec->decode_in, sizeof(codec->decode_in), &codec->decode_in_bytes, 
                         SPEEX8_SAMPLES_PER_FRAME, SPEEX8_BYTES_PER_EFRAME, 
                         speex8_frame_decode, codec);
}
