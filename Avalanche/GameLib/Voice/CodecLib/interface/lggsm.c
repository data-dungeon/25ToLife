
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "utils.h"
#include "lgpriv.h"

// codecs
#include "lggsm.h"


int gsm_frame_encode(short* in, unsigned char* out, void* enc_param);
int gsm_frame_decode(unsigned char* in, short* out, void* dec_param);


int gsm_frame_encode(short* in, unsigned char* out, void* enc_param)
{
    GSM* pgsm = (GSM*)enc_param;
    return gsm_encode(pgsm->encode_state,
                      in, 
                      out);
                      
}

int gsm_frame_decode(unsigned char* in, short* out, void* dec_param)
{
    GSM* pgsm = (GSM*)dec_param;
    return gsm_decode(pgsm->decode_state,
                        in, 
                        out);
                        
}

int lgCodecGSMEncode(GSM* pgsm, const short* src, const int src_size, u_char* dest, int* dest_size)
{
    return lgFrameEncode(src, src_size, dest, dest_size,
                         pgsm->encode_in, sizeof(pgsm->encode_in), &pgsm->encode_in_samples, 
                         GSM_SAMPLES_PER_FRAME, GSM_BYTES_PER_EFRAME, 
                         gsm_frame_encode, pgsm);
}

int lgCodecGSMDecode(GSM* pgsm, const u_char* src, const int src_size, short* dest, int* dest_size)
{

    return lgFrameDecode(src, src_size, dest, dest_size,
                         pgsm->decode_in, sizeof(pgsm->decode_in), &pgsm->decode_in_bytes, 
                         GSM_SAMPLES_PER_FRAME, GSM_BYTES_PER_EFRAME, 
                         gsm_frame_decode, pgsm);
}

