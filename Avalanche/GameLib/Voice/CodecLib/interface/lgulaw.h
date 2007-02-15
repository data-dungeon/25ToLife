
#ifndef _LGULAW_H_
#define _LGULAW_H_

#include "Voice/CodecLib/ULAW/u-law.h"
#include "liblgcodec.h"

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif


int lgCodecUlawEncode(const short* src, const int src_size, u_char* dest, int* dest_size);
int lgCodecUlawDecode(const u_char* src, const int src_size, short* dest, int* dest_size);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif

