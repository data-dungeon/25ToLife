#ifndef LPC10_H
#define LPC10_H

#ifdef __cplusplus
extern "C" {
#endif

#define LPC10_SAMPLES_PER_FRAME 180
#define LPC10_BYTES_PER_EFRAME 7

#include "lpc10int.h"
    
//typedef struct lpc10_encoder_state * lpc10_encoder_state;
//typedef struct lpc10_decoder_state * lpc10_decoder_state;

/* call in this order to encode */
struct lpc10_encoder_state * create_lpc10_encoder_state (void);
void init_lpc10_encoder_state (struct lpc10_encoder_state *st);
int lpc10_encode(short *in, unsigned char *out, struct lpc10_encoder_state *st);
void destroy_lpc10_encoder_state (struct lpc10_encoder_state *st);

/* call in this order to decode */
struct lpc10_decoder_state * create_lpc10_decoder_state (void);
void init_lpc10_decoder_state (struct lpc10_decoder_state *st);
int lpc10_decode(unsigned char *in, short *out, struct lpc10_decoder_state *st);
void destroy_lpc10_decoder_state (struct lpc10_decoder_state *st);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* LPC10_H */
