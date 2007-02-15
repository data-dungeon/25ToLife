/*

$Log: /PlayStation2/Libs/liblgcodec/codecs/lpc10/lpcenc.c $
 * 
 * 1     3/24/03 9:16p Lbldr
 * 
 * 1     8/13/02 2:52p Lbldr
Revision 1.1  2000/06/05 04:45:12  robertj
Added LPC-10 2400bps codec

 * Revision 1.2  1996/08/20  20:31:21  jaf
 * Removed all static local variables that were SAVE'd in the Fortran
 * code, and put them in struct lpc10_encoder_state that is passed as an
 * argument.
 *
 * Removed init function, since all initialization is now done in
 * init_lpc10_encoder_state().
 *
 * Changed name of function from lpcenc_ to lpc10_encode, simply to make
 * all lpc10 functions have more consistent naming with each other.
 *
 * Revision 1.1  1996/08/19  22:31:44  jaf
 * Initial revision
 *

*/

#include <string.h>
#include "lpc10int.h"
#include "ftol.h"

/* Table of constant values */
static integer entau[60] = { 19,11,27,25,29,21,23,22,30,14,15,7,39,38,46,
	    42,43,41,45,37,53,49,51,50,54,52,60,56,58,26,90,88,92,84,86,82,83,
	    81,85,69,77,73,75,74,78,70,71,67,99,97,113,112,114,98,106,104,108,
	    100,101,76 };
static integer enadd[8] = { 1920,-768,2432,1280,3584,1536,2816,-1152 };
static float enscl[8] = { .0204f,.0167f,.0145f,.0147f,.0143f,.0135f,.0125f,.0112f };
static integer enbits[8] = { 6,5,4,4,4,4,3,3 };
static integer entab6[64] = { 0,0,0,0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,
	    3,3,3,3,3,4,4,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,8,9,9,
	    9,10,10,11,11,12,13,14,15 };
static integer rmst[64] = { 1024,936,856,784,718,656,600,550,502,460,420,
	    384,352,328,294,270,246,226,206,188,172,158,144,132,120,110,102,
	    92,84,78,70,64,60,54,50,46,42,38,34,32,30,26,24,22,20,18,17,16,15,
	    14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 };

static integer c__10 = 10;

extern void analys(float *, integer *, integer *, float *, float *, struct lpc10_encoder_state *);
extern void chanwr_(integer *, integer *, integer *, integer *, integer *, struct lpc10_encoder_state *);

/*  Quantize LPC parameters for transmission */

/* INPUTS: */
/*  VOICE  - Half frame voicing decisions */
/*           Indices 1 through 2 read. */
/*  PITCH  - Pitch */
/*  RMS    - Energy */
/*  RC     - Reflection coefficients */
/*           Indices 1 through ORDER read. */
/*  CORRP  - Error Correction: TRUE = yes, FALSE = none */
/*           (this is defined in file control.fh) */
/* OUTPUTS: */
/*  IPITCH - Coded pitch and voicing */
/*  IRMS   - Quantized energy */
/*  IRC    - Quantized reflection coefficients */
/*           Indices 1 through MAX(ORDER,2) written. */
/*           If CORRP is .TRUE., then indices 1 through 10 written */
/*           for unvoiced frames. */

void encode(integer *voice, integer pitch, float rms, float *rc, integer *ipitch,
            integer *irms, integer *irc, struct lpc10_encoder_state *st)
{
    /* Local variables */
    integer idel, nbit, i, j, i2, i3, mrk, order;
    
    /* Parameter adjustments */
    --irc;
    --rc;
    --voice;
    order = st->order;
    
    /* Function Body */
    /*  Scale RMS and RC's to integers */
    *irms = round(rms);
    for (i = 1; i <= order; ++i) {
        irc[i] = round(rc[i] * 32768.f);
    }
    /*  Encode pitch and voicing */
    if (voice[1] != 0 && voice[2] != 0) {
        *ipitch = entau[pitch - 1];
    } else {
        *ipitch = (voice[1] << 1) + voice[2];
    }
    /*  Encode RMS by binary table search */
    j = 32;
    idel = 16;
    *irms = min(*irms,1023);
    while(idel > 0) {
        if (*irms > rmst[j - 1]) {
            j -= idel;
        }
        if (*irms < rmst[j - 1]) {
            j += idel;
        }
        idel /= 2;
    }
    if (*irms > rmst[j - 1]) {
        --j;
    }
    *irms = 31 - j / 2;
    /*  Encode RC(1) and (2) as log-area-ratios */
    for (i = 1; i <= 2; ++i) {
        i2 = irc[i];
        mrk = 0;
        if (i2 < 0) {
            i2 = -i2;
            mrk = 1;
        }
        i2 /= 512;
        i2 = min(i2,63);
        i2 = entab6[i2];
        if (mrk != 0) {
            i2 = -i2;
        }
        irc[i] = i2;
    }
    /*  Encode RC(3) - (10) linearly, remove bias then scale */
    for (i = 3; i <= order; ++i) {
        i2 = irc[i] / 2;
        i2 = round((i2 + enadd[order + 1 - i - 1]) * enscl[order + 1 - i - 1]);
        /* Computing MIN */
        i2 = min(max(i2,-127),127);
        nbit = enbits[order + 1 - i - 1];
        i3 = 0;
        if (i2 < 0) {
            i3 = -1;
        }
        i2 = i2 / (2 << (nbit-1));
        if (i3 == -1) {
            --i2;
        }
        irc[i] = i2;
    }
} /* encode_ */

/*    100 Hz High Pass Filter */

/* Input: */
/*  start, end - Range of samples to filter */
/* Input/Output: */
/*  speech(end) - Speech data. */
/*                Indices start through end are read and modified. */

void hp100(float *speech, integer end, struct lpc10_encoder_state *st)
{
    float z11;
    float z21;
    float z12;
    float z22;

    /* Local variables */
    integer i;
    float si, err;

    /* Function Body */

    z11 = st->z11;
    z21 = st->z21;
    z12 = st->z12;
    z22 = st->z22;

    for (i = 0; i<end; ++i) {
	    err = *speech + z11 * 1.859076f - z21 * .8648249f;
	    si = err - z11 * 2.f + z21;
	    z21 = z11;
	    z11 = err;
	    err = si + z12 * 1.935715f - z22 * .9417004f;
	    si = err - z12 * 2.f + z22;
	    z22 = z12;
	    z12 = err;
	    *speech++ = si * .902428f;
    }

    st->z11 = z11;
    st->z21 = z21;
    st->z12 = z12;
    st->z22 = z22;

} /* hp100_ */

/* ***************************************************************** */

/* Encode one frame of 180 speech samples to 54 bits. */

/* Input: */
/*  SPEECH - Speech encoded as float values in the range [-1,+1]. */
/*           Indices 1 through 180 read, and modified (by PREPRO). */
/* Output: */
/*  BITS   - 54 encoded bits, stored 1 per array element. */
/*           Indices 1 through 54 written. */

/* This subroutine maintains local state from one call to the next.  If */
/* you want to switch to using a new audio stream for this filter, or */
/* reinitialize its state for any other reason, call the ENTRY */
/* INITLPCENC. */

int lpc10encode(float *speech, integer *bits,
				  struct lpc10_encoder_state *st)
{
    integer irms, voice[2], pitch, ipitv;
    float rc[10];
    integer irc[10];
    float rms;


    /* Function Body */
    hp100(speech, st->lframe, st);
    analys(speech, voice, &pitch, &rms, rc, st);
    encode(voice, pitch, rms, rc, &ipitv, &irms, irc, st);
    chanwr_(&c__10, &ipitv, &irms, irc, bits, st);
    return 0;
} /* lpcenc_ */

int lpc10_encode(short *in, unsigned char *out,
				  struct lpc10_encoder_state *st)
{
    float   speech[LPC10_SAMPLES_PER_FRAME];
    float   *sp = speech;
    integer bits[LPC10_BITS_IN_COMPRESSED_FRAME];
    int     i;

    /* convert sound from short to float */
    for(i=LPC10_SAMPLES_PER_FRAME;i--;)
    {
        *sp++ = (float)(*in++ / 32768.0f);
    }

    /* encode it */
    memset(bits, 0, sizeof(bits));
    (void)lpc10encode(speech, bits, st);

    /* pack the bits */
    memset(out, 0, 7);
	for (i = 0; i < 54; i++)
    {
        out[i >> 3] |= ((bits[i] != 0)? 1 : 0) << (i & 7);
	}

    /* return 7 bytes encoded*/
    return 7;
}

