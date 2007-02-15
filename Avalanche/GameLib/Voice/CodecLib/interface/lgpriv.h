
#ifndef _LG_CODEC_H_
#define _LG_CODEC_H_

#ifndef WIN32
#include <sys/types.h>
#else
#define u_char unsigned char
#define u_short unsigned short
#define u_long unsigned long
#endif

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "liblgcodec.h"


typedef int (*PFN_FRAME_ENCODE)(short* in, unsigned char* out, void* enc_param);
typedef int (*PFN_FRAME_DECODE)(unsigned char* in, short* out, void* dec_param);

int lgFrameEncode(const short* src, const int src_size, u_char* dest, int* dest_size,
                  short* encode_in, const int encode_in_size, int* encode_in_samples, 
                  const int samples_per_frame, const int bytes_per_enc_frame, 
                  PFN_FRAME_ENCODE pfn_enc, void* enc_param);

int lgFrameDecode(const u_char* src, const int src_size, short* dest, int* dest_size,
                  unsigned char* decode_in, const int decode_in_size, int* decode_in_bytes, 
                  const int samples_per_frame, const int bytes_per_enc_frame, 
                  PFN_FRAME_DECODE pfn_dec, void* dec_param);

#endif
