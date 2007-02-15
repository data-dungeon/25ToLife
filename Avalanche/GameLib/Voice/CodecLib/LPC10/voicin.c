#include "lpc10int.h"
#include "ftol.h"

/*  Calculate voicing parameters: */

/* Input: */
/*  VWIN   - Voicing window limits */
/*           Indices 1 through 2 read. */
/*  INBUF  - Input speech buffer */
/*           Indices START-1 through STOP read, */
/*          where START and STOP are defined in the code (only written once).
*/
/*           Note that STOP can be as large as VWIN(2)+1 ! */
/*  LPBUF  - Low pass filtered speech */
/*           Indices START-MINTAU through STOP+MINTAU read, */
/*          where START and STOP are defined in the code (only written once).
*/
/*  BUFLIM - Array bounds for INBUF and LPBUF */
/*           Indices 1 through 4 read. */
/*  HALF   - Half frame (1 or 2) */
/*  MINTAU - Lag corresponding to minimum AMDF value (pitch estimate) */
/* Input/Output: */
/*  DITHER - Zero crossing threshold */
/*           The resulting value might be the negation of the input */
/*           value.  It might always be the same as the input value, */
/*           if the DO loop below always executes an even number of times. */
/* Output: (all of them are written on every call) */
/*  ZC     - Zero crossing rate */
/*  LBE    - Low band energy (sum of magnitudes - SM) */
/*  FBE    - Full band energy (SM) */
/*  QS     - Ratio of 6 dB/oct preemphasized energy to full band energy */
/*  RC1    - First reflection coefficient */
/*  AR_B   - Product of the causal forward and reverse pitch */
/*           prediction gains */
/*  AR_F   - Product of the noncausal forward and reverse pitch */
/*           prediction gains */
/* Internal: */
/*  OLDSGN - Previous sign of dithered signal */
/*  VLEN   - Length of voicing window */
/*  START  - Lower address of current half of voicing window */
/*  STOP   - Upper address of current half of voicing window */
/*  E_0    - Energy of LPF speech (sum of squares - SS) */
/*  E_B    - Energy of LPF speech backward one pitch period (SS) */
/*  E_F    - Energy of LPF speech forward one pitch period (SS) */
/*  R_B    - Autocovariance of LPF speech backward one pitch period */
/*  R_F    - Autocovariance of LPF speech forward one pitch period */
/*  LP_RMS - Energy of LPF speech (sum of magnitudes - SM) */
/*  AP_RMS - Energy of all-pass speech (SM) */
/*  E_PRE  - Energy of 6dB preemphasized speech (SM) */
/*  E0AP   - Energy of all-pass speech (SS) */

static void vparms(integer *vwin, float *inbuf, float *lpbuf, integer 
            *buflim, integer *half, float *dither, integer *mintau, integer *zc, 
            integer *lbe, integer *fbe, float *qs, float *rc1, float *ar_b__, float *
            ar_f__)
{
    /* System generated locals */
    integer inbuf_offset, lpbuf_offset;
    float r__1;
    
    /* Local variables */
    integer vlen, stop, i;
    float e_pre__;
    integer start;
    float ap_rms__, e_0__, oldsgn, lp_rms__, e_b__, e_f__, r_b__, r_f__, e0ap;
    
    /*       Arguments */
    /*       Local variables that need not be saved */
    /*   Calculate zero crossings (ZC) and several energy and correlation */
    /*   measures on low band and full band speech.  Each measure is taken */
    /*   over either the first or the second half of the voicing window, */
    /*   depending on the variable HALF. */
    /* Parameter adjustments */
    --vwin;
    --buflim;
    lpbuf_offset = buflim[3];
    lpbuf -= lpbuf_offset;
    inbuf_offset = buflim[1];
    inbuf -= inbuf_offset;
    
    /* Function Body */
    lp_rms__ = 0.f;
    ap_rms__ = 0.f;
    e_pre__ = 0.f;
    e0ap = 0.f;
    *rc1 = 0.f;
    e_0__ = 0.f;
    e_b__ = 0.f;
    e_f__ = 0.f;
    r_f__ = 0.f;
    r_b__ = 0.f;
    *zc = 0;
    vlen = vwin[2] - vwin[1] + 1;
    start = vwin[1] + (*half - 1) * vlen / 2 + 1;
    stop = start + vlen / 2 - 1;
    
    r__1 = inbuf[start - 1] - *dither;
    oldsgn = (r__1<0.0f)?-1.0f:1.0f;
    for (i = start; i <= stop; ++i) {
        if(lpbuf[i] < 0.0f)
            lp_rms__ -= lpbuf[i];
        else
            lp_rms__ += lpbuf[i];
        if(inbuf[i] < 0.0f)
            ap_rms__ -= inbuf[i];
        else
            ap_rms__ += inbuf[i];
        r__1 = inbuf[i] - inbuf[i - 1];
        if(r__1 < 0.0f)
            e_pre__ -= r__1;
        else
            e_pre__ += r__1;
        /* Computing 2nd power */
        r__1 = inbuf[i];
        e0ap += r__1 * r__1;
        *rc1 += inbuf[i] * inbuf[i - 1];
        /* Computing 2nd power */
        r__1 = lpbuf[i];
        e_0__ += r__1 * r__1;
        /* Computing 2nd power */
        r__1 = lpbuf[i - *mintau];
        e_b__ += r__1 * r__1;
        /* Computing 2nd power */
        r__1 = lpbuf[i + *mintau];
        e_f__ += r__1 * r__1;
        r_f__ += lpbuf[i] * lpbuf[i + *mintau];
        r_b__ += lpbuf[i] * lpbuf[i - *mintau];
        r__1 = inbuf[i] + *dither;
        if (((r__1<0.0f)?-1.0f:1.0f) != oldsgn) {
            ++(*zc);
            oldsgn = -oldsgn;
        }
        *dither = -(*dither);
    }
    *rc1 /= max(e0ap,1.f);
    *qs = e_pre__ / max(ap_rms__ * 2.f, 1.f);
    *ar_b__ = r_b__ / max(e_b__,1.f) * (r_b__ / max(e_0__,1.f));
    *ar_f__ = r_f__ / max(e_f__,1.f) * (r_f__ / max(e_0__,1.f));
    *zc = round((float) (*zc << 1) * (90.f / vlen));
    /* Computing MIN */
    *lbe = min(round(lp_rms__ * 0.25f * (90.f / vlen)),32767);
    /* Computing MIN */
    *fbe = min(round(ap_rms__ * 0.25f * (90.f / vlen)),32767);
} /* vparms_ */


/****************************************************************************/

/*        Voicing Detection (VOICIN) makes voicing decisions for each half */
/*  frame of input speech.  Tentative voicing decisions are made two frames*/
/*   in the future (2F) for each half frame.  These decisions are carried */
/*   through one frame in the future (1F) to the present (P) frame where */
/*   they are examined and smoothed, resulting in the final voicing */
/*   decisions for each half frame. */
/*        The voicing parameter (signal measurement) column vector (VALUE) */
/*   is based on a rectangular window of speech samples determined by the */
/*  window placement algorithm.  The voicing parameter vector contains the*/
/*  AMDF windowed maximum-to-minimum ratio, the zero crossing rate, energy*/
/*   measures, reflection coefficients, and prediction gains.  The voicing */
/*  window is placed to avoid contamination of the voicing parameter vector*/
/*   with speech onsets. */
/*        The input signal is then classified as unvoiced (including */
/*   silence) or voiced.  This decision is made by a linear discriminant */
/*   function consisting of a dot product of the voicing decision */
/*   coefficient (VDC) row vector with the measurement column vector */
/*  (VALUE).  The VDC vector is 2-dimensional, each row vector is optimized*/
/*   for a particular signal-to-noise ratio (SNR).  So, before the dot */
/*   product is performed, the SNR is estimated to select the appropriate */
/*   VDC vector. */
/*        The smoothing algorithm is a modified median smoother.  The */
/*  voicing discriminant function is used by the smoother to determine how*/
/*   strongly voiced or unvoiced a signal is.  The smoothing is further */
/*   modified if a speech onset and a voicing decision transition occur */
/*   within one half frame.  In this case, the voicing decision transition */
/*  is extended to the speech onset.  For transmission purposes, there are*/
/*   constraints on the duration and transition of voicing decisions.  The */
/*   smoother takes these constraints into account. */
/*        Finally, the energy estimates are updated along with the dither */
/*   threshold used to calculate the zero crossing rate (ZC). */

/* Inputs: */
/*  VWIN      - Voicing window limits */
/*              The indices read of arrays VWIN, INBUF, LPBUF, and BUFLIM */
/*              are the same as those read by subroutine VPARMS. */
/*  INBUF     - Input speech buffer */
/*  LPBUF     - Low-pass filtered speech buffer */
/*  BUFLIM    - INBUF and LPBUF limits */
/*  HALF      - Present analysis half frame number */
/*  MINAMD    - Minimum value of the AMDF */
/*  MAXAMD    - Maximum value of the AMDF */
/*  MINTAU    - Pointer to the lag of the minimum AMDF value */
/*  IVRC(2)   - Inverse filter's RC's */
/*              Only index 2 of array IVRC read under normal operation. */
/*              (Index 1 is also read when debugging is turned on.) */
/*  OBOUND    - Onset boundary descriptions */
/*             Indices 1 through 3 read if (HALF .NE. 1), otherwise untouched.
*/
/*  AF        - The analysis frame number */
/* Output: */
/*  VOIBUF(2,0:AF) - Buffer of voicing decisions */
/*              Index (HALF,3) written. */
/*              If (HALF .EQ. 1), skip down to "Read (HALF,3)" below. */
/*              Indices (1,2), (2,1), (1,2), and (2,2) read. */
/*              One of the following is then done: */
/*                 read (1,3) and possibly write (1,2) */
/*                 read (1,3) and write (1,2) or (2,2) */
/*                 write (2,1) */
/*                 write (2,1) or (1,2) */
/*                 read (1,0) and (1,3) and then write (2,2) or (1,1) */
/*                 no reads or writes on VOIBUF */
/*              Finally, read (HALF,3) */
/* Internal: */
/*  QS        - Ratio of preemphasized to full-band energies */
/*  RC1       - First reflection coefficient */
/* AR_B      - Product of the causal forward and reverse pitch prediction gain
s*/
/* AR_F      - Product of the noncausal forward and rev. pitch prediction gain
s*/
/*  ZC        - Zero crossing rate */
/*  DITHER    - Zero crossing threshold level */
/*  MAXMIN    - AMDF's 1 octave windowed maximum-to-minimum ratio */
/*  MINPTR    - Location  of minimum AMDF value */
/*  NVDC      - Number of elements in each VDC vector */
/*  NVDCL     - Number of VDC vectors */
/*  VDCL      - SNR values corresponding to the set of VDC's */
/*  VDC       - 2-D voicing decision coefficient vector */
/*  VALUE(9)  - Voicing Parameters */
/*  VOICE(2,3)- History of LDA results */
/*              On every call when (HALF .EQ. 1), VOICE(*,I+1) is */
/*              shifted back to VOICE(*,I), for I=1,2. */
/*              VOICE(HALF,3) is written on every call. */
/*              Depending on several conditions, one or more of */
/*              (1,1), (1,2), (2,1), and (2,2) might then be read. */
/*  LBE       - Ratio of low-band instantaneous to average energies */
/*  FBE       - Ratio of full-band instantaneous to average energies */
/*  LBVE      - Low band voiced energy */
/*  LBUE      - Low band unvoiced energy */
/*  FBVE      - Full band voiced energy */
/*  FBUE      - Full band unvoiced energy */
/*  OFBUE     - Previous full-band unvoiced energy */
/*  OLBUE     - Previous low-band unvoiced energy */
/*  REF       - Reference energy for initialization and DITHER threshold */
/*  SNR       - Estimate of signal-to-noise ratio */
/*  SNR2      - Estimate of low-band signal-to-noise ratio */
/*  SNRL      - SNR level number */
/*  OT        - Onset transition present */
/*  VSTATE    - Decimal interpretation of binary voicing classifications */
/*  FIRST     - First call flag */

/* This subroutine maintains local state from one call to the next.  If */
/* you want to switch to using a new audio stream for this filter, or */
/* reinitialize its state for any other reason, call the ENTRY */
/* INITVOICIN. */

void voicin_(integer *vwin, float *inbuf, float *lpbuf, integer *buflim, integer *half,
            float *minamd, float *maxamd, integer *mintau, float *ivrc, integer *obound,
            integer *voibuf, struct lpc10_encoder_state *st)
{
    /* Initialized data */
    
    float *dither;
    static float vdc[100]	/* was [10][10] */ = { 0.f,1714.f,-110.f,
        334.f,-4096.f,-654.f,3752.f,3769.f,0.f,1181.f,0.f,874.f,-97.f,
        300.f,-4096.f,-1021.f,2451.f,2527.f,0.f,-500.f,0.f,510.f,-70.f,
        250.f,-4096.f,-1270.f,2194.f,2491.f,0.f,-1500.f,0.f,500.f,-10.f,
        200.f,-4096.f,-1300.f,2e3f,2e3f,0.f,-2e3f,0.f,500.f,0.f,0.f,
        -4096.f,-1300.f,2e3f,2e3f,0.f,-2500.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
        0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };
    static integer nvdcl = 5;
    static float vdcl[10] = { 600.f,450.f,300.f,200.f,0.f,0.f,0.f,0.f,0.f,0.f };
    
    /* System generated locals */
    integer inbuf_offset, lpbuf_offset;
    
    /* Local variables */
    float ar_b__, ar_f__;
    integer *lbve, *lbue, *fbve, *fbue;
    integer snrl, i;
    integer *ofbue, *sfbue;
    float *voice;
    integer *olbue, *slbue;
    float value[9];
    integer zc;
    logical ot;
    float qs;
    float *maxmin;
    integer vstate;
    float rc1;
    integer fbe, lbe;
    float *snr;
    float snr2;
    
    
    /*   Declare and initialize filters: */
    
    dither = (&st->dither);
    snr = (&st->snr);
    maxmin = (&st->maxmin);
    voice = (&st->voice[0]);
    lbve = (&st->lbve);
    lbue = (&st->lbue);
    fbve = (&st->fbve);
    fbue = (&st->fbue);
    ofbue = (&st->ofbue);
    olbue = (&st->olbue);
    sfbue = (&st->sfbue);
    slbue = (&st->slbue);
    
    /* Parameter adjustments */
    if (vwin) {
        --vwin;
    }
    if (buflim) {
        --buflim;
    }
    
    inbuf_offset = buflim[1];
    inbuf -= inbuf_offset;
    lpbuf_offset = buflim[3];
    lpbuf -= lpbuf_offset;
    
    if (ivrc) {
        --ivrc;
    }
    if (obound) {
        --obound;
    }
    if (voibuf) {
        --voibuf;
    }
    
    /* Function Body */
    
    /*   Update linear discriminant function history each frame: */
    if (*half == 1) {
        voice[0] = voice[2];
        voice[1] = voice[3];
        voice[2] = voice[4];
        voice[3] = voice[5];
        *maxmin = *maxamd / max(*minamd,1.f);
    }
    /*   Calculate voicing parameters twice per frame: */
    vparms(&vwin[1], &inbuf[inbuf_offset], &lpbuf[lpbuf_offset], &buflim[1], 
        half, dither, mintau, &zc, &lbe, &fbe, &qs, &rc1, &ar_b__, &
        ar_f__);
        /*   Estimate signal-to-noise ratio to select the appropriate VDC vector. 
    */
    *snr = (float) round((*snr + *fbve / (float) max(*fbue,1)) * 63 / 64.f);
    snr2 = *snr * *fbue / max(*lbue,1);
    /*   Quantize SNR to SNRL according to VDCL thresholds. */
    for (snrl = 1; snrl <= nvdcl - 1; ++snrl) {
        if (snr2 > vdcl[snrl - 1]) {
            break;
        }
    }
    /*   Linear discriminant voicing parameters: */
    value[0] = *maxmin;
    value[1] = (float) lbe / max(*lbve,1);
    value[2] = (float) zc;
    value[3] = rc1;
    value[4] = qs;
    value[5] = ivrc[2];
    value[6] = ar_b__;
    value[7] = ar_f__;
    /*   Evaluation of linear discriminant function: */
    voice[*half + 3] = vdc[snrl * 10 - 1];
    for (i = 1; i <= 8; ++i) {
        voice[*half + 3] += vdc[i + snrl * 10 - 11] * value[i - 1];
    }
    /*   Classify as voiced if discriminant > 0, otherwise unvoiced */
    /*   Voicing decision for current half-frame:  1 = Voiced; 0 = Unvoiced */
    if (voice[*half + 3] > 0.f) {
        voibuf[*half + 6] = 1;
    } else {
        voibuf[*half + 6] = 0;
    }
    /*   Skip voicing decision smoothing in first half-frame: */
    vstate = -1;
    if (*half != 1) {
        /*   Determine if there is an onset transition between P and 1F. */
        /*   OT (Onset Transition) is true if there is an onset between */
        /*   P and 1F but not after 1F. */
        ot = ((obound[1] & 2) != 0 || obound[2] == 1) && (obound[3] & 1) == 0;
        /*   Multi-way dispatch on voicing decision history: */
        vstate = (voibuf[3] << 3) + (voibuf[4] << 2) + (voibuf[5] << 1) + voibuf[6];
        
        switch (vstate + 1) {
        case 1:
            break;
        case 2:
            if (ot && voibuf[7] == 1) {
                voibuf[5] = 1;
            }
            break;
        case 3:
            if (voibuf[7] == 0 || voice[2] < -voice[3]) {
                voibuf[5] = 0;
            } else {
                voibuf[6] = 1;
            }
            break;
        case 4:
            break;
        case 5:
            voibuf[4] = 0;
            break;
        case 6:
            if (voice[1] < -voice[2]) {
                voibuf[4] = 0;
            } else {
                voibuf[5] = 1;
            }
            break;
        case 7:
            if (voibuf[1] == 1 || voibuf[7] == 1 || voice[3] > voice[0]) {
                voibuf[6] = 1;
            } else {
                voibuf[3] = 1;
            }
            break;
        case 8:
            if (ot) {
                voibuf[4] = 0;
            }
            break;
        case 9:
            if (ot) {
                voibuf[4] = 1;
            }
            break;
        case 10:
            break;
        case 11:
            if (voice[2] < -voice[1]) {
                voibuf[5] = 0;
            } else {
                voibuf[4] = 1;
            }
            break;
        case 12:
            voibuf[4] = 1;
            break;
        case 13:
            break;
        case 14:
            if (voibuf[7] == 0 && voice[3] < -voice[2]) {
                voibuf[6] = 0;
            } else {
                voibuf[5] = 1;
            }
            break;
        case 15:
            if (ot && voibuf[7] == 0) {
                voibuf[5] = 0;
            }
            break;
        case 16:
            break;
        }
    }
    /*   Now update parameters: */
    if (voibuf[*half + 6] == 0) {
        /* Computing MIN */
        *sfbue = round((*sfbue * 63 + (min(fbe, *ofbue * 3) << 3)) / 64.f);
        *fbue = *sfbue / 8;
        *ofbue = fbe;
        /* Computing MIN */
        *slbue = round((*slbue * 63 + (min(lbe, *olbue * 3) << 3)) / 64.f);
        *lbue = *slbue / 8;
        *olbue = lbe;
    } else {
        *lbve = round((*lbve * 63 + lbe) / 64.f);
        *fbve = round((*fbve * 63 + fbe) / 64.f);
    }
    /*   Set dither threshold to yield proper zero crossing rates in the */
    /*   presence of low frequency noise and low level signal input. */
    *dither = min(max(((float)sqrt((float) (*lbue * *lbve)) * 64 / 3000), 1.f),20.f);
    /*   Voicing decisions are returned in VOIBUF. */
} /* voicin_ */
