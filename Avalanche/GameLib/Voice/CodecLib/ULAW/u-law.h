
#ifndef ULAW_H
#define ULAW_H

int ulawEncode(const short *in, unsigned char *out, int length);
int ulawDecode(const unsigned char *in, short *out, int length);

#endif /*ULAW_H */
