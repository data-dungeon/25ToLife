/*
 * This source code is a product of Sun Microsystems, Inc. and is provided
 * for unrestricted use.  Users may copy or modify this source code without
 * charge.
 *
 * SUN SOURCE CODE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING
 * THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun source code is provided with no support and without any obligation on
 * the part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS SOFTWARE
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

/*
 * g72x.h
 *
 * Header file for CCITT conversion routines.
 *
 */
#ifndef ___G72X_H__
#define	___G72X_H__

#include "Voice/CodecLib/ULAW/g711.h"

// handles 240 sample frames
#define G723_24_CHUNKS   20
#define G723_24_SAMPLES_PER_CHUNK   8
#define G723_24_SAMPLES_PER_FRAME   G723_24_CHUNKS * G723_24_SAMPLES_PER_CHUNK
#define G723_24_BYTES_PER_ECHUNK    3
#define G723_24_BYTES_PER_EFRAME    G723_24_CHUNKS * G723_24_BYTES_PER_ECHUNK
#define G723_24_BIT_ENCODING        3

#define G721_CHUNKS   20
#define G721_SAMPLES_PER_CHUNK   8
#define G721_SAMPLES_PER_FRAME   G721_CHUNKS * G721_SAMPLES_PER_CHUNK
#define G721_BYTES_PER_ECHUNK    4
#define G721_BYTES_PER_EFRAME    G721_CHUNKS * G721_BYTES_PER_ECHUNK
#define G721_BIT_ENCODING        4

struct G723_24_ENCODED_CHUNK
{
    
    unsigned f0 : 3;
    unsigned f1 : 3;
    unsigned f2 : 3;
    unsigned f3 : 3;
    unsigned f4 : 3;
    unsigned f5 : 3;
    unsigned f6 : 3;
    unsigned f7 : 3;
};

struct G721_ENCODED_CHUNK
{    
    unsigned f0 : 4;
    unsigned f1 : 4;
    unsigned f2 : 4;
    unsigned f3 : 4;
    unsigned f4 : 4;
    unsigned f5 : 4;
    unsigned f6 : 4;
    unsigned f7 : 4;
};


#define	AUDIO_ENCODING_ULAW	(1)	/* ISDN u-law */
#define	AUDIO_ENCODING_ALAW	(2)	/* ISDN A-law */
#define	AUDIO_ENCODING_LINEAR	(3)	/* PCM 2's-complement (0-center) */

/*
 * The following is the definition of the state structure
 * used by the G.721/G.723 encoder and decoder to preserve their internal
 * state between successive calls.  The meanings of the majority
 * of the state structure fields are explained in detail in the
 * CCITT Recommendation G.721.  The field names are essentially indentical
 * to variable names in the bit level description of the coding algorithm
 * included in this Recommendation.
 */
struct g72x_state {
	long yl;	/* Locked or steady state step size multiplier. */
	short yu;	/* Unlocked or non-steady state step size multiplier. */
	short dms;	/* Short term energy estimate. */
	short dml;	/* Long term energy estimate. */
	short ap;	/* Linear weighting coefficient of 'yl' and 'yu'. */

	short a[2];	/* Coefficients of pole portion of prediction filter. */
	short b[6];	/* Coefficients of zero portion of prediction filter. */
	short pk[2];	/*
			 * Signs of previous two samples of a partially
			 * reconstructed signal.
			 */
	short dq[6];	/*
			 * Previous 6 samples of the quantized difference
			 * signal represented in an internal floating point
			 * format.
			 */
	short sr[2];	/*
			 * Previous 2 samples of the quantized difference
			 * signal represented in an internal floating point
			 * format.
			 */
	char td;	/* delayed tone detect, new in 1988 version */
};

/* External function definitions. */
#ifdef __cplusplus
extern "C" {
#endif

extern void g72x_init_state(struct g72x_state *);
extern int g721_encoder(
		int sample,
		int in_coding,
		struct g72x_state *state_ptr);
extern int g721_decoder(
		int code,
		int out_coding,
		struct g72x_state *state_ptr);
extern int g723_24_encoder(
		int sample,
		int in_coding,
		struct g72x_state *state_ptr);
extern int g723_24_decoder(
		int code,
		int out_coding,
		struct g72x_state *state_ptr);
extern int g723_40_encoder(
		int sample,
		int in_coding,
		struct g72x_state *state_ptr);
extern int g723_40_decoder(
		int code,
		int out_coding,
		struct g72x_state *state_ptr);

extern int g723_24_encode_window(
        short* in,
	    unsigned char* out,
	    struct g72x_state *state_ptr);

extern int g723_24_decode_window(
	    unsigned char* in,
	    short* out,
	    struct g72x_state *state_ptr);

extern int g721_encode_window(
        short* in,
	    unsigned char* out,
	    struct g72x_state *state_ptr);

extern int g721_decode_window(
	    unsigned char* in,
	    short* out,
	    struct g72x_state *state_ptr);

extern int predictor_zero(
		struct g72x_state *state_ptr );

extern int predictor_pole(
		struct g72x_state *state_ptr );

extern int step_size(
		struct g72x_state *state_ptr );

extern int quantize(
		int		d,
		int		y,
		short		*table,
		int		size );

extern int reconstruct(
		int		sign,
		int		dqln,
		int		y );

extern void update(
		int		code_size,
		int		y,	
		int		wi,	
		int		fi,	
		int		dq,	
		int		sr,	
		int		dqsez,
		struct g72x_state *state_ptr );

extern int tandem_adjust_alaw(
		int		sr,
		int		se,
		int		y,
		int		i,
		int		sign,
		short	*qtab );

extern int tandem_adjust_ulaw(
		int		sr,	
		int		se,	
		int		y,	
		int		i,	
		int		sign,
		short	*qtab );


#ifdef __cplusplus
}
#endif

#endif /* !_G72X_H */
