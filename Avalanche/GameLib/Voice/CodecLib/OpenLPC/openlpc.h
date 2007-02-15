/*
 * LPC subroutine declarations
 */

#ifndef OPENLPC_H
#define OPENLPC_H

#ifdef __cplusplus
extern "C" {
#endif

#define OPENLPC_FRAMESIZE_1_8	    250
#define OPENLPC_FRAMESIZE_1_4	    320
#define OPENLPC_ENCODED_FRAME_SIZE  7

#define LPC_FILTORDER		10
#define FS		8000.0f	/* Sampling rate */
#define MAXWINDOW	1000	/* Max analysis window length */

typedef struct openlpc_e_state{
	float   s[MAXWINDOW], y[MAXWINDOW], h[MAXWINDOW];
    int     framelen, buflen;
    float   xv1[3], yv1[3], 
            xv2[2], yv2[2], 
			xv3[1], yv3[3], 
			xv4[2], yv4[2];
    float   w[MAXWINDOW], r[LPC_FILTORDER+1];
} openlpc_e_state_t;

typedef struct openlpc_d_state{
		float Oldper, OldG, Oldk[LPC_FILTORDER + 1];
        float bp[LPC_FILTORDER+1];
        float exc;
		int pitchctr, framelen, buflen;
} openlpc_d_state_t;

typedef struct openlpc_e_state openlpc_encoder_state;
typedef struct openlpc_d_state openlpc_decoder_state;

openlpc_encoder_state *create_openlpc_encoder_state(void);
void init_openlpc_encoder_state(openlpc_encoder_state *st, int framelen);
int  openlpc_encode(const short *in, unsigned char *out, openlpc_encoder_state *st);
void destroy_openlpc_encoder_state(openlpc_encoder_state *st);

openlpc_decoder_state *create_openlpc_decoder_state(void);
void init_openlpc_decoder_state(openlpc_decoder_state *st, int framelen);
int  openlpc_decode(unsigned char *in, short *out, openlpc_decoder_state *st);
void destroy_openlpc_decoder_state(openlpc_decoder_state *st);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* OPENLPC_H */
