/*

$Log: /PlayStation2/Libs/liblgcodec/codecs/lpc10/lpcdec.c $
 * 
 * 1     3/24/03 9:16p Lbldr
 * 
 * 1     8/13/02 2:52p Lbldr
Revision 1.1  2000/06/05 04:45:12  robertj
Added LPC-10 2400bps codec

 * Revision 1.2  1996/08/20  20:30:11  jaf
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
 * Revision 1.1  1996/08/19  22:31:48  jaf
 * Initial revision
 *

*/

#include "lpc10int.h"
#include "ftol.h"

/* Table of constant values */
static integer detau[128] = { 0,0,0,3,0,3,3,31,0,3,3,21,3,3,29,30,0,3,3,
        20,3,25,27,26,3,23,58,22,3,24,28,3,0,3,3,3,3,39,33,32,3,37,35,36,
        3,38,34,3,3,42,46,44,50,40,48,3,54,3,56,3,52,3,3,1,0,3,3,108,3,78,
        100,104,3,84,92,88,156,80,96,3,3,74,70,72,66,76,68,3,62,3,60,3,64,
        3,3,1,3,116,132,112,148,152,3,3,140,3,136,3,144,3,3,1,124,120,128,
        3,3,3,3,1,3,3,3,1,3,1,1,1 };
static integer rmst[64] = { 1024,936,856,784,718,656,600,550,502,460,420,
        384,352,328,294,270,246,226,206,188,172,158,144,132,120,110,102,
        92,84,78,70,64,60,54,50,46,42,38,34,32,30,26,24,22,20,18,17,16,15,
        14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 };
static integer detab7[32] = { 4,11,18,25,32,39,46,53,60,66,72,77,82,87,92,
        96,101,104,108,111,114,115,117,119,121,122,123,124,125,126,127,127 };
static float descl[8] = { .6953f,.625f,.5781f,.5469f,.5312f,.5391f,.4688f,.3828f };
static integer deadd[8] = { 1152,-2816,-1536,-3584,-1280,-2432,768,-1920 };
static integer qb[8] = { 511,511,1023,1023,1023,1023,2047,4095 };
static integer nbit[10] = { 8,8,5,5,4,4,4,4,3,2 };

static integer c__10 = 10;

extern int chanrd_(integer *, integer *, integer *, integer *, integer *);

extern int pitsyn_(integer *, integer *, integer *, float *, float *, integer *, integer *,
                   integer *, float *, float *, integer *, float *, struct lpc10_decoder_state *);

#define MIDTAP 1
#define MAXTAP 4
static short y[MAXTAP+1]={-21161, -8478, 30892,-10216, 16950};
static int j=MIDTAP, k=MAXTAP;

static int random (void)
{
    int the_random;
    
    /*   The following is a 16 bit 2's complement addition,
    *   with overflow checking disabled	*/
    
    y[k] = (short)(y[k] + y[j]);
    
    the_random = y[k];
    k--;
    if (k < 0) k = MAXTAP;
    j--;
    if (j < 0) j = MAXTAP;
    
    return(the_random);
}

static integer kexc[25] = { 8,-16,26,-48,86,-162,294,-502,718,-728,184,
        672,-610,-672,184,728,718,502,294,162,86,48,26,16,8 };

/*   Synthesize One Pitch Epoch */

/* Input: */
/*  COEF  - Predictor coefficients */
/*          Indices 1 through ORDER read. */
/*  IP    - Pitch period (number of samples to synthesize) */
/*  IV    - Voicing for the current epoch */
/*  RMS   - Energy for the current epoch */
/*  RATIO - Energy slope for plosives */
/*  G2PASS- Sharpening factor for 2 pass synthesis */
/* Output: */
/*  SOUT  - Synthesized speech */
/*          Indices 1 through IP written. */

/* This subroutine maintains local state from one call to the next.  If */
/* you want to switch to using a new audio stream for this filter, or */
/* reinitialize its state for any other reason, call the ENTRY */
/* INITBSYNZ. */

static void bsynz(float *coef, integer ip, integer *iv, 
           float *sout, float *rms, float *ratio, float *g2pass,
           struct lpc10_decoder_state *st)
{
    /* Initialized data */
    
    integer *ipo;
    float *rmso;
    float *exc;
    float *exc2;
    float lpi1;
    float lpi2;
    float hpi1;
    float hpi2;
     
    /* Local variables */
    float gain, xssq;
    integer i, j, k;
    float pulse;
    integer px;
    float sscale;
    float xy, sum, ssq;
    float lpi0, hpi0;
    integer order;
    
    /* Parameter adjustments */
    if (coef) {
        --coef;
    }
    
    /* Function Body */
    ipo = &(st->ipo);
    exc = &(st->exc[0]);
    exc2 = &(st->exc2[0]);
    lpi1 = st->lpi1;
    lpi2 = st->lpi2;
    hpi1 = st->hpi1;
    hpi2 = st->hpi2;
    rmso = &(st->rmso_bsynz);
    order = st->order;
    
    /*                  MAXPIT+MAXORD=166 */
    /*  Calculate history scale factor XY and scale filter state */
    /* Computing MIN */
    xy = min((*rmso / (*rms + 1e-6f)),8.f);
    *rmso = *rms;
    for (i = 0; i < order; ++i) {
        exc2[i] = exc2[*ipo + i] * xy;
    }
    *ipo = ip;
    if (*iv == 0) {
        /*  Generate white noise for unvoiced */
        for (i = 0; i < ip; ++i) {
            exc[order + i] = (float) (random() >> 6);
        }
        px = ((random() + 32768) * (ip - 1) >> 16) + order + 1;
        pulse = *ratio * 85.5f;
        if (pulse > 2e3f) {
            pulse = 2e3f;
        }
        exc[px - 1] += pulse;
        exc[px] -= pulse;
        /*  Load voiced excitation */
    } else {
        sscale = (float)sqrt((float) (ip)) * 0.144341801f;
        for (i = 0; i < ip; ++i) {
            float temp;

            if (i > 27) {
                temp = 0.f;
            }
            else if (i < 25) {
                lpi0 = temp = sscale * kexc[i];
                temp = lpi0 * .125f + lpi1 * .75f + lpi2 * .125f;
                lpi2 = lpi1;
                lpi1 = lpi0;
            }
            else{
                lpi0 = temp = 0.f;
                temp = lpi1 * .75f + lpi2 * .125f;
                lpi2 = lpi1;
                lpi1 = lpi0;
            }
            hpi0 = (float)(random() >> 6);
            exc[order + i] = temp + hpi0 * -.125f + hpi1 * .25f + hpi2 * -.125f;
            hpi2 = hpi1;
            hpi1 = hpi0;
        }
    }
    /*   Synthesis filters: */
    /*    Modify the excitation with all-zero filter  1 + G*SUM */
    xssq = 0.f;
    for (i = 0; i < ip; ++i) {
        k = order + i;
        sum = 0.f;
        for (j = 1; j <= order; ++j) {
            sum += coef[j] * exc[k - j];
        }
        sum *= *g2pass;
        exc2[k] = sum + exc[k];
    }
    /*   Synthesize using the all pole filter  1 / (1 - SUM) */
    for (i = 0; i < ip; ++i) {
        k = order + i;
        sum = 0.f;
        for (j = 1; j <= order; ++j) {
            sum += coef[j] * exc2[k - j];
        }
        exc2[k] += sum;
        xssq += exc2[k] * exc2[k];
    }
    /*  Save filter history for next epoch */
    for (i = 0; i < order; ++i) {
        exc[i] = exc[ip + i];
        exc2[i] = exc2[ip + i];
    }
    /*  Apply gain to match RMS */
    ssq = *rms * *rms * ip;
    gain = (float)sqrt(ssq / xssq);
    for (i = 0; i < ip; ++i) {
        sout[i] = gain * exc2[order + i];
    }
    st->lpi1 = lpi1;
    st->lpi2 = lpi2;
    st->hpi1 = hpi1;
    st->hpi2 = hpi2;
} /* bsynz_ */

/*   Convert Reflection Coefficients to Predictor Coeficients */

/* Inputs: */
/*  RC     - Reflection coefficients */
/*           Indices 1 through ORDER read. */
/*  ORDER  - Number of RC's */
/*  GPRIME - Excitation modification gain */
/* Outputs: */
/*  PC     - Predictor coefficients */
/*           Indices 1 through ORDER written. */
/*           Indices 1 through ORDER-1 are read after being written. */
/*  G2PASS - Excitation modification sharpening factor */

static void irc2pc(float *rc, float *pc, float gprime, float *g2pass)
{
    /* System generated locals */
    integer i2;
    
    /* Local variables */
    float temp[10];
    integer i, j;
    
    /* Parameter adjustments */
    --pc;
    --rc;
    
    /* Function Body */
    *g2pass = 1.f;
    for (i = 1; i <= 10; ++i) {
        *g2pass *= 1.f - rc[i] * rc[i];
    }
    *g2pass = gprime * (float)sqrt(*g2pass);
    pc[1] = rc[1];
    for (i = 2; i <= 10; ++i) {
        i2 = i - 1;
        for (j = 1; j <= i2; ++j) {
            temp[j - 1] = pc[j] - rc[i] * pc[i - j];
        }
        i2 = i - 1;
        for (j = 1; j <= i2; ++j) {
            pc[j] = temp[j - 1];
        }
        pc[i] = rc[i];
    }
} /* irc2pc_ */

/*  De-Emphasize output speech with   1 / ( 1 - .75z**-1 ) */
/*    cascaded with 200 Hz high pass filter */
/*    ( 1 - 1.9998z**-1 + z**-2 ) / ( 1 - 1.75z**-1 + .78z**-2 ) */

/*  WARNING!  The coefficients above may be out of date with the code */
/*  below.  Either that, or some kind of transformation was performed */
/*  on the coefficients above to create the code below. */

/* Input: */
/*  N  - Number of samples */
/* Input/Output: */
/*  X  - Speech */
/*       Indices 1 through N are read before being written. */

/* This subroutine maintains local state from one call to the next.  If */
/* you want to switch to using a new audio stream for this filter, or */
/* reinitialize its state for any other reason, call the ENTRY */
/* INITDEEMP. */

static void deemp(float *x, integer n, struct lpc10_decoder_state *st)
{
    /* Initialized data */
    
    float dei1;
    float dei2;
    float deo1;
    float deo2;
    float deo3;
    
    /* Local variables */
    integer k;
    float dei0;
    
    /* Function Body */
    
    dei1 = st->dei1;
    dei2 = st->dei2;
    deo1 = st->deo1;
    deo2 = st->deo2;
    deo3 = st->deo3;
    
    for (k = 0; k < n; ++k) {
        dei0 = x[k];
        x[k] = dei0 - dei1 * 1.9998f + dei2 + deo1 * 2.5f - deo2 * 2.0925f + deo3 * .585f;
        dei2 = dei1;
        dei1 = dei0;
        deo3 = deo2;
        deo2 = deo1;
        deo1 = x[k];
    }
    st->dei1 = dei1;
    st->dei2 = dei2;
    st->deo1 = deo1;
    st->deo2 = deo2;
    st->deo3 = deo3;
} /* deemp_ */

/* The note below is from the distributed version of the LPC10 coder. */
/* The version of the code below has been modified so that SYNTHS always */
/* has a constant frame length output of MAXFRM. */

/* Also, BSYNZ and DEEMP need not be modified to work on variable */
/* positions within an array.  It is only necessary to pass the first */
/* index desired as the array argument.  What actually gets passed is the */
/* address of that array position, which the subroutine treats as the */
/* first index of the array. */

/* This technique is used in subroutine ANALYS when calling PREEMP, so it */
/* appears that multiple people wrote different parts of this LPC10 code, */
/* and that they didn't necessarily have equivalent knowledge of Fortran */
/* (not surprising). */

/*  NOTE: There is excessive buffering here, BSYNZ and DEEMP should be */
/*        changed to operate on variable positions within SOUT.  Also, */
/*        the output length parameter is bogus, and PITSYN should be */
/*        rewritten to allow a constant frame length output. */

/* Input: */
/*  VOICE  - Half frame voicing decisions */
/*           Indices 1 through 2 read. */
/* Input/Output: */
/*  PITCH  - Pitch */
/*           PITCH is restricted to range 20 to 156, inclusive, */
/*           before calling subroutine PITSYN, and then PITSYN */
/*           can modify it further under some conditions. */
/*  RMS    - Energy */
/*           Only use is for debugging, and passed to PITSYN. */
/*           See comments there for how it can be modified. */
/*  RC     - Reflection coefficients */
/*           Indices 1 through ORDER restricted to range -.99 to .99, */
/*           before calling subroutine PITSYN, and then PITSYN */
/*           can modify it further under some conditions. */
/* Output: */
/*  SPEECH - Synthesized speech samples. */
/*           Indices 1 through the final value of K are written. */
/*  K      - Number of samples placed into array SPEECH. */
/*           This is always MAXFRM. */

static void synths(integer *voice, integer pitch, float rms, float *rc, float *speech, struct lpc10_decoder_state *st)
{
    /* Initialized data */

    float *buf;
    integer *buflen;

    /* System generated locals */
    integer i1;
    float r__1, r__2;

    /* Local variables */
    float rmsi[16];
    integer nout, ivuv[16], i, j;
    float ratio;
    integer ipiti[16];
    float g2pass;
    float pc[10];
    float rci[160]	/* was [10][16] */;


/*       Indices 1 through BUFLEN contain samples that are left over from 
*/
/*       the last call to SYNTHS.  Given the way that PITSYN works, */
/*       BUFLEN should always be in the range MAXFRM-MAXPIT+1 through */
/*       MAXFRM, inclusive, after a call to SYNTHS is complete. */

/*       On the first call to SYNTHS (or the first call after */
/*       reinitializing with the entry INITSYNTHS), BUFLEN is MAXFRM, and */
/*       a frame of silence is always returned. */
    /* Parameter adjustments */
    if (voice) {
	--voice;
	}
    if (rc) {
	--rc;
	}
    if (speech) {
	--speech;
	}

    /* Function Body */
    buf = &(st->buf[0]);
    buflen = &(st->buflen);

/* Computing MAX */
    i1 = min(pitch,156);
    pitch = max(i1,20);
    for (i = 1; i <= st->order; ++i) {
/* Computing MAX */
/* Computing MIN */
	r__2 = rc[i];
	r__1 = min(r__2,.99f);
	rc[i] = max(r__1,-.99f);
    }
    pitsyn_(&st->order, &voice[1], &pitch, &rms, &rc[1], &st->lframe, 
	    ivuv, ipiti, rmsi, rci, &nout, &ratio, st);
    if (nout > 0) {
	i1 = nout;
	for (j = 1; j <= i1; ++j) {

/*             Add synthesized speech for pitch period J to the end of */
/*             BUF. */

	    irc2pc(&rci[j * 10 - 10], pc, 0.7f, &g2pass);
	    bsynz(pc, ipiti[j - 1], &ivuv[j - 1], &buf[*buflen], &rmsi[j - 1], &ratio, &g2pass, st);
	    deemp(&buf[*buflen], ipiti[j - 1], st);
	    *buflen += ipiti[j - 1];
	}

/*          Copy first MAXFRM samples from BUF to output array SPEECH */
/*          (scaling them), and then remove them from the beginning of */
/*          BUF. */

	for (i = 1; i <= st->lframe; ++i) {
	    speech[i] = buf[i - 1] / 4096.f;
	}
	*buflen += -st->lframe;
	i1 = *buflen;
	for (i = 1; i <= i1; ++i) {
	    buf[i - 1] = buf[i + st->lframe - 1];
	}
    }
} /* synths_ */

/*   This subroutine provides error correction and decoding */
/*   for all LPC parameters */

/* Input: */
/*  IPITV  - Index value of pitch */
/*  IRMS   - Coded Energy */
/*  CORRP  - Error correction: */
/*    If FALSE, parameters are decoded directly with no delay.  If TRUE, */
/*    most important parameter bits are protected by Hamming code and */
/*    median smoothed.  This requires an additional frame of delay. */
/* Input/Output: */
/*  IRC    - Coded Reflection Coefficients */
/*           Indices 1 through ORDER always read, then written. */
/* Output: */
/*  VOICE  - Half frame voicing decisions */
/*           Indices 1 through 2 written. */
/*  PITCH  - Decoded pitch */
/*  RMS    - Energy */
/*  RC     - Reflection coefficients */
/*           Indices 1 through ORDER written. */

/*  NOTE: Zero RC's should be done more directly, but this would affect */
/*   coded parameter printout. */

static void decode(integer ipitv, integer irms, integer *irc, integer *voice, integer *pitch,
             float *rms, float *rc, struct lpc10_decoder_state *st)
{
    /* Initialized data */
    
    logical *first;
    integer *ivp2h;
    integer *iovoic;
    integer *iavgp;
    integer *iptold;
    integer *erate;
    integer *drc;
    integer *dpit;
    integer *drms;
    
    /* Local variables */
    integer i, i1, i2, i4;
    integer ishift;
    
    /* Parameter adjustments */
    if (irc) {
        --irc;
    }
    if (voice) {
        --voice;
    }
    if (rc) {
        --rc;
    }
    
    /* Function Body */
    
    iptold = &(st->iptold);
    first = &(st->first);
    ivp2h = &(st->ivp2h);
    iovoic = &(st->iovoic);
    iavgp = &(st->iavgp);
    erate = &(st->erate);
    drc = &(st->drc[0]);
    dpit = &(st->dpit[0]);
    drms = &(st->drms[0]);
    
    /* DATA statements for "constants" defined above. */
    /* 	IF (LISTL.GE.3) WRITE(FDEBUG,800) IPITV,IRMS,(IRC(J),J=1,ORDER) */
    /* 800	FORMAT(1X,' <<ERRCOR IN>>',T32,6X,I6,I5,T50,10I8) */
    /*  If no error correction, do pitch and voicing then jump to decode */
    i4 = detau[ipitv];
        voice[1] = 1;
        voice[2] = 1;
        if (ipitv <= 1) {
            voice[1] = 0;
        }
        if (ipitv == 0 || ipitv == 2) {
            voice[2] = 0;
        }
        *pitch = i4;
        if (*pitch <= 4) {
            *pitch = *iptold;
        }
        if (voice[1] == 1 && voice[2] == 1) {
            *iptold = *pitch;
        }
        if (voice[1] != voice[2]) {
            *pitch = *iptold;
        }
    /*   Decode RMS */
    irms = rmst[(31 - irms) * 2];
    /*  Decode RC(1) and RC(2) from log-area-ratios */
    /*  Protect from illegal coded value (-16) caused by bit errors */
    for (i = 1; i <= 2; ++i) {
        i2 = irc[i];
        i1 = 0;
        if (i2 < 0) {
            i1 = 1;
            i2 = -i2;
            if (i2 > 15) {
                i2 = 0;
            }
        }
        i2 = detab7[i2 * 2];
        if (i1 == 1) {
            i2 = -i2;
        }
        ishift = 15 - nbit[i - 1];
        irc[i] = i2 * (2 << (ishift-1));
    }
    /*  Decode RC(3)-RC(10) to sign plus 14 bits */
    for (i = 3; i <= st->order; ++i) {
        i2 = irc[i];
        ishift = 15 - nbit[i - 1];
        i2 *= (2 << (ishift-1));
        i2 += qb[i - 3];
        irc[i] = (int)(i2 * descl[i - 3] + deadd[i - 3]);
    }
    /*  Scale RMS and RC's to floats */
    *rms = (float) (irms);
    for (i = 1; i <= st->order; ++i) {
        rc[i] = irc[i] / 16384.f;
    }
} /* decode_ */

/* ***************************************************************** */

/* Decode 54 bits to one frame of 180 speech samples. */

/* Input: */
/*  BITS   - 54 encoded bits, stored 1 per array element. */
/*           Indices 1 through 53 read (SYNC bit ignored). */
/* Output: */
/*  SPEECH - Speech encoded as float values in the range [-1,+1]. */
/*           Indices 1 through 180 written. */

/* This subroutine maintains local state from one call to the next.  If */
/* you want to switch to using a new audio stream for this filter, or */
/* reinitialize its state for any other reason, call the ENTRY */
/* INITLPCDEC. */

static void lpc10decode(integer *bits, float *speech,
				  struct lpc10_decoder_state *st)
{
    integer irms, voice[2], pitch, ipitv;
    float rc[10];
    integer irc[10];
    float rms;

    /* Parameter adjustments */
    if (bits) {
	--bits;
	}
    if (speech) {
	--speech;
	}

    /* Function Body */
    chanrd_(&c__10, &ipitv, &irms, irc, &bits[1]);
    decode(ipitv, irms, irc, voice, &pitch, &rms, rc, st);
    synths(voice, pitch, rms, rc, &speech[1], st);
} /* lpcdec_ */

int lpc10_decode(unsigned char *in, short *out,
				  struct lpc10_decoder_state *st)
{
    float   speech[LPC10_SAMPLES_PER_FRAME];
    integer bits[LPC10_BITS_IN_COMPRESSED_FRAME];
    int     i;

    /* unpack bits into array */
	for (i = 0; i < LPC10_BITS_IN_COMPRESSED_FRAME; i++)
    {
        bits[i] = (in[i >> 3] & (1 << (i & 7))) != 0 ? 1 : 0;
	}

    /* decode speech */
    lpc10decode(bits, speech, st);

    /* convert from float to short */
    for(i=0;i<LPC10_SAMPLES_PER_FRAME;i++)
    {
        out[i] = (short) max(-32768, min(round(32768.0f * speech[i]), 32767));
    }

    return LPC10_SAMPLES_PER_FRAME;
}

