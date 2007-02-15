#include "lpc10int.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__156 = 156;

/* ****************************************************************** */


extern void voicin_(integer *, float *, float *, integer *, integer *, float *, float *, integer *,
               float *, integer *, integer *, struct lpc10_encoder_state *);
extern void placev_(integer *, integer *, integer *, integer *, integer, integer *,
               integer, integer, integer);
extern void placea_(integer *, integer *, integer *, integer *, integer *, integer *, integer *,
               integer *, integer *);
extern void tbdm_(float *, integer, integer *, integer, float *, integer *, integer *,
                 integer *);

/* Load a covariance matrix. */

/* Input: */
/*  ORDER            - Analysis order */
/*  AWINS            - Analysis window start */
/*  AWINF            - Analysis window finish */
/*  SPEECH(AWINF)    - Speech buffer */
/*                     Indices MIN(AWINS, AWINF-(ORDER-1)) through */
/*                             MAX(AWINF, AWINS+(ORDER-1)) read. */
/*                     As long as (AWINF-AWINS) .GE. (ORDER-1), */
/*                     this is just indices AWINS through AWINF. */
/* Output: */
/*  PHI(ORDER,ORDER) - Covariance matrix */
/*                    Lower triangular half and diagonal written, and read.*/
/*                     Upper triangular half untouched. */
/*  PSI(ORDER)       - Prediction vector */
/*                     Indices 1 through ORDER written, */
/*                     and most are read after that. */

static void mload(integer awins, integer awinf, float *speech, float *phi, float *psi)
{
    /* Local variables */
    integer c, i, r, start;
    
    /*       Arguments */
    /*       Local variables that need not be saved */
    /*   Load first column of triangular covariance matrix PHI */
    /* Parameter adjustments */
    phi -= 10;
    speech--;
    
    /* Function Body */
    start = awins + 10;
    for (r = 1; r <= 10; ++r) {
        phi[r + 10 - 1] = 0.f;
        for (i = start; i <= awinf; ++i) {
            phi[r + 10 - 1] += speech[i - 1] * speech[i - r];
        }
    }
    /*   Load last element of vector PSI */
    psi[10 - 1] = 0.f;
    for (i = start; i <= awinf; ++i) {
        psi[10 - 1] += speech[i] * speech[i - 10];
    }
    /*   End correct to get additional columns of PHI */
    for (r = 2; r <= 10; ++r) {
        for (c = 2; c <= r; ++c) {
            phi[r + c * 10 - 1] = phi[r - 1 + (c - 1) * 10 - 1] - 
                speech[awinf + 1 - r] * speech[awinf + 1 - c] + 
                speech[start - r] * speech[start - c];
        }
    }
    /*   End correct to get additional elements of PSI */
    for (c = 1; c < 10; ++c) {
        psi[c - 1] = phi[c + 1 + 10 - 1] - speech[start]
                * speech[start - 1 - c] + speech[awinf] * speech[awinf - c];
    }
} /* mload_ */

/*  Check RC's, repeat previous frame's RC's if unstable */

/* Input: */
/*  ORDER - Number of RC's */
/*  RC1F  - Previous frame's RC's */
/*          Indices 1 through ORDER may be read. */
/* Input/Output: */
/*  RC2F  - Present frame's RC's */
/*          Indices 1 through ORDER may be read, and written. */

/* This subroutine has no local state. */

static void rcchk(float *rc1f, float *rc2f)
{
    /* Local variables */
    integer i;
    
    /* Function Body */
    for (i = 0; i < 10; ++i) {
        if ((fabs(rc2f[i])) > .99f) {
            goto L10;
        }
    }
    return;
L10:
    for (i = 0; i < 10; ++i) {
        rc2f[i] = rc1f[i];
    }
} /* rcchk_ */

/* Calculate and remove DC bias from buffer. */

/* Input: */
/*  LEN    - Length of speech buffers */
/*  SPEECH - Input speech buffer */
/*           Indices 1 through LEN read. */
/* Output: */
/*  SIGOUT - Output speech buffer */
/*           Indices 1 through LEN written */

static void dcbias(integer len, float *speech, float *sigout)
{
    /* Local variables */
    float bias;
    integer i;

    /* Function Body */
    bias = 0.f;
    for (i = 0; i < len; ++i) {
        bias += speech[i];
    }
    bias /= len;
    for (i = 0; i < len; ++i) {
        *sigout++ = *speech++ - bias;
    }
} /* dcbias_ */


/*   Preemphasize speech with a single-zero filter. */
/*  (When coef = .9375, preemphasis is as in LPC43.) */

/* Inputs: */
/*  NSAMP  - Number of samples to filter */
/*  INBUF  - Input speech buffer */
/*           Indices 1 through NSAMP are read. */
/*  COEF   - Preemphasis coefficient */
/* Input/Output: */
/*  Z      - Filter state */
/* Output: */
/*  PEBUF  - Preemphasized speech buffer (can be equal to INBUF) */
/*           Indices 1 through NSAMP are modified. */

static void preemp(float *inbuf, float *pebuf, integer nsamp, float coef, float *z)
{
    /* Local variables */
    float temp;
    integer i;

    /* Function Body */
    for (i = 0; i< nsamp; ++i) {
	    temp = *inbuf - coef * *z;
	    *z = *inbuf++;
	    *pebuf++ = temp;
    }
} /* preemp_ */

/*   31 Point Equiripple FIR Low-Pass Filter */
/*     Linear phase, delay = 15 samples */

/* 	Passband:  ripple = 0.25 dB, cutoff =  800 Hz */
/* 	Stopband:  atten. =  40. dB, cutoff = 1240 Hz */

/* Inputs: */
/*  LEN    - Length of speech buffers */
/*  NSAMP  - Number of samples to filter */
/*  INBUF  - Input speech buffer */
/*           Indices len-nsamp-29 through len are read. */
/* Output: */
/*  LPBUF  - Low passed speech buffer (must be different array than INBUF) */
/*           Indices len+1-nsamp through len are written. */

static void lpfilt(float *inbuf, float *lpbuf, integer len, integer nsamp)
{
    /* Local variables */
    integer j;
    
    /* Function Body */
    lpbuf = &lpbuf[len - nsamp];
    for (j = len - nsamp; j < len; ++j) {
        *lpbuf++ = (inbuf[j] + inbuf[j - 30]) * -.0097201988f
        + (inbuf[j - 1] + inbuf[j - 29]) * -.0105179986f
        + (inbuf[j - 2] + inbuf[j - 28]) * -.0083479648f
        + (inbuf[j - 3] + inbuf[j - 27]) * 5.860774e-4f
        + (inbuf[j - 4] + inbuf[j - 26]) * .0130892089f
        + (inbuf[j - 5] + inbuf[j - 25]) * .0217052232f
        + (inbuf[j - 6] + inbuf[j - 24]) * .0184161253f
        + (inbuf[j - 7] + inbuf[j - 23]) * 3.39723e-4f
        + (inbuf[j - 8] + inbuf[j - 22]) * -.0260797087f
        + (inbuf[j - 9] + inbuf[j - 21]) * -.0455563702f
        + (inbuf[j - 10] + inbuf[j - 20]) * -.040306855f
        + (inbuf[j - 11] + inbuf[j - 19]) * 5.029835e-4f
        + (inbuf[j - 12] + inbuf[j - 18]) * .0729262903f
        + (inbuf[j - 13] + inbuf[j - 17]) * .1572008878f
        + (inbuf[j - 14] + inbuf[j - 16]) * .2247288674f
        + inbuf[j - 15] * .250535965f;
    }
} /* lpfilt_ */

/*   2nd order inverse filter, speech is decimated 4:1 */

/* Input: */
/*  LEN    - Length of speech buffers */
/*  NSAMP  - Number of samples to filter */
/*  LPBUF  - Low pass filtered speech buffer */
/*           Indices LEN-NSAMP-7 through LEN read. */
/* Output: */
/*  IVBUF  - Inverse filtered speech buffer */
/*           Indices LEN-NSAMP+1 through LEN written. */
/*  IVRC   - Inverse filter reflection coefficients (for voicing) */
/*          Indices 1 and 2 both written (also read, but only after writing).
*/

static void ivfilt(float *lpbuf, float *ivbuf, integer len, integer nsamp, float *ivrc)
{
    /* Local variables */
    integer i, j, k;
    float r[3], pc1, pc2;

/*  Calculate Autocorrelations */
    /* Parameter adjustments */
    --ivbuf;
    --lpbuf;
    --ivrc;
    
    /* Function Body */
    for (i = 1; i <= 3; ++i) {
        r[i - 1] = 0.f;
        k = (i - 1) << 2;
        for (j = (i << 2) + len - nsamp; j <= len; j += 2) {
            r[i - 1] += lpbuf[j] * lpbuf[j - k];
        }
    }
    /*  Calculate predictor coefficients */
    pc1 = 0.f;
    pc2 = 0.f;
    ivrc[1] = 0.f;
    ivrc[2] = 0.f;
    if (r[0] > 1e-10f) {
        ivrc[1] = r[1] / r[0];
        ivrc[2] = (r[2] - ivrc[1] * r[1]) / (r[0] - ivrc[1] * r[1]);
        pc1 = ivrc[1] - ivrc[1] * ivrc[2];
        pc2 = ivrc[2];
    }
    /*  Inverse filter LPBUF into IVBUF */
    for (i = len + 1 - nsamp; i <= len; ++i) {
        ivbuf[i] = lpbuf[i] - pc1 * lpbuf[i - 4] - pc2 * lpbuf[i - 8];
    }
} /* ivfilt_ */

/* **************************************************************** */

/*  Invert a covariance matrix using Choleski decomposition method. */

/* Input: */
/*  PHI(ORDER,ORDER) - Covariance matrix */
/*                    Indices (I,J) read, where ORDER .GE. I .GE. J .GE. 1.*/
/*                     All other indices untouched. */
/*  PSI(ORDER)       - Column vector to be predicted */
/*                     Indices 1 through ORDER read. */
/* Output: */
/*  RC(ORDER)        - Pseudo reflection coefficients */
/*                    Indices 1 through ORDER written, and then possibly read.
*/
/* Internal: */
/*  V(ORDER,ORDER)   - Temporary matrix */
/*                     Same indices written as read from PHI. */
/*                     Many indices may be read and written again after */
/*                     initially being copied from PHI, but all indices */
/*                     are written before being read. */

/*  NOTE: Temporary matrix V is not needed and may be replaced */
/*    by PHI if the original PHI values do not need to be preserved. */

static void invert(float *phi, float *psi, float *rc)
{
    /* Local variables */
    float save;
    integer i, j, k;
    float v[100]	/* was [10][10] */;

    
    /* Function Body */
    for (j = 0; j < 10; ++j) {
        for (i = j; i < 10; ++i) {
            v[i + j * 10] = phi[i + j * 10];
        }
        for (k = 0; k < j; ++k) {
            save = v[j + k * 10] * v[k + k * 10];
            for (i = j; i < 10; ++i) {
                v[i + j * 10] -= v[i + k * 10] * save;
            }
        }
        /*  Compute intermediate results, which are similar to RC's */
        if ((fabs(v[j + j * 10])) < 1e-10f) {
            goto L100;
        }
        rc[j] = psi[j];
        for (k = 0; k < j; ++k) {
            rc[j] -= rc[k] * v[j + k * 10];
        }
        v[j + j * 10] = 1.f / v[j + j * 10];
        rc[j] *= v[j + j * 10];

        rc[j] = max(min(rc[j],.999f),-.999f);
    }
    return;
    /*  Zero out higher order RC's if algorithm terminated early */
L100:
    for (i = j; i < 10; ++i) {
        rc[i] = 0.f;
    }
} /* invert_ */

/* ********************************************************************* */

/* Compute RMS energy. */

/* Input: */
/*  LEN    - Length of speech buffer */
/*  SPEECH - Speech buffer */
/*           Indices 1 through LEN read. */
/* Output: */
/*  RMS    - Root Mean Square energy */

static void energy(integer len, float *speech, float *rms)
{
    /* Local variables */
    integer i;

    /* Parameter adjustments */
    --speech;

    /* Function Body */
    *rms = 0.f;
    for (i = 1; i <= len; ++i) {
	    *rms += speech[i] * speech[i];
    }
    *rms = (float)sqrt(*rms / len);
} /* energy_ */

/*   Dynamic Pitch Tracker */

/* Input: */
/*  AMDF   - Average Magnitude Difference Function array */
/*           Indices 1 through LTAU read, and MINPTR */
/*  LTAU   - Number of lags in AMDF */
/*  MINPTR - Location of minimum AMDF value */
/*  VOICE  - Voicing decision */
/* Output: */
/*  PITCH  - Smoothed pitch value, 2 frames delayed */
/*  MIDX   - Initial estimate of current frame pitch */
/* Compile time constant: */
/*  DEPTH  - Number of frames to trace back */

static void dyptrk(float *amdf, integer ltau, integer *minptr, integer *voice, integer *pitch,
            integer *midx, struct lpc10_encoder_state *st)
{
    /* Initialized data */
    
    float *s;
    integer *p;
    integer *ipoint;
    float *alphax;
    
    
    /* Local variables */
    integer pbar;
    float sbar;
    integer path[2], iptr, i, j;
    float alpha, minsc, maxsc;
    
    s = &(st->s[0]);
    p = &(st->p[0]);
    ipoint = &(st->ipoint);
    alphax = &(st->alphax);
    
    
    /* Parameter adjustments */
    if (amdf) {
        --amdf;
    }
    
    /* Function Body */
    
    if (*voice == 1) {
        *alphax = *alphax * .75f + amdf[*minptr] / 2.f;
    } else {
        *alphax *= .984375f;
    }
    alpha = *alphax / 16;
    if (*voice == 0 && *alphax < 128.f) {
        alpha = 8.f;
    }
    /* SEESAW: Construct a pitch pointer array and intermediate winner function*/
    /*   Left to right pass: */
    iptr = *ipoint + 1;
    p[iptr * 60 - 60] = 1;
    i = 1;
    pbar = 1;
    sbar = s[0];
    for (i = 1; i <= ltau; ++i) {
        sbar += alpha;
        if (sbar < s[i - 1]) {
            s[i - 1] = sbar;
            p[i + iptr * 60 - 61] = pbar;
        } else {
            sbar = s[i - 1];
            p[i + iptr * 60 - 61] = i;
            pbar = i;
        }
    }
    /*   Right to left pass: */
    i = pbar - 1;
    sbar = s[i];
    while(i >= 1) {
        sbar += alpha;
        if (sbar < s[i - 1]) {
            s[i - 1] = sbar;
            p[i + iptr * 60 - 61] = pbar;
        } else {
            pbar = p[i + iptr * 60 - 61];
            i = pbar;
            sbar = s[i - 1];
        }
        --i;
    }
    /*   Update S using AMDF */
    /*   Find maximum, minimum, and location of minimum */
    s[0] += amdf[1] / 2;
    minsc = s[0];
    maxsc = minsc;
    *midx = 1;
    for (i = 2; i <= ltau; ++i) {
        s[i - 1] += amdf[i] / 2;
        if (s[i - 1] > maxsc) {
            maxsc = s[i - 1];
        }
        if (s[i - 1] < minsc) {
            *midx = i;
            minsc = s[i - 1];
        }
    }
    /*   Subtract MINSC from S to prevent overflow */
    for (i = 1; i <= ltau; ++i) {
        s[i - 1] -= minsc;
    }
    maxsc -= minsc;
    /*   Use higher octave pitch if significant null there */
    j = 0;
    for (i = 20; i <= 40; i += 10) {
        if (*midx > i) {
            if (s[*midx - i - 1] < maxsc / 4) {
                j = i;
            }
        }
    }
    *midx -= j;
    /*   TRACE: look back two frames to find minimum cost pitch estimate */
    j = *ipoint;
    *pitch = *midx;
    for (i = 1; i <= 2; ++i) {
        j = j % 2 + 1;
        *pitch = p[*pitch + j * 60 - 61];
        path[i - 1] = *pitch;
    }
    
    *ipoint = (*ipoint + 1) % 2;
} /* dyptrk_ */

/*   Detection of onsets in (or slightly preceding) the futuremost frame */
/*   of speech. */

/* Input: */
/*  PEBUF(SBUFL:SBUFH)  - Preemphasized speech */
/*                        Indices SBUFH-LFRAME through SBUFH are read. */
/*  OSLEN  - Maximum number of onsets that can be stored in OSBUF. */
/*  SBUFL, SBUFH        - Range of PEBUF */
/*  LFRAME              - length of a frame, in samples */
/* Input/Output: */
/*  OSBUF(OSLEN) - Buffer which holds sorted indexes of onsets */
/*                 Indices A through B are modified, where A */
/*                 is the original value of OSPTR, and B is the final */
/*                 value of OSPTR-1.  B is at most OSLEN. */
/*  OSPTR        - Free pointer into OSBUF */
/*                 Initial value should be .LE. OSLEN+1. */
/*                 If so, final value grows by one for each new onset */
/*                 found, and final value will be .LE. OSLEN+1. */

static void onset(float *pebuf, integer *osbuf, integer *osptr, integer oslen, integer sbufl,
          integer sbufh, integer lframe, struct lpc10_encoder_state *st)
{
    /* Initialized data */
    
    float n;
    float d;
    float *l2buf;
    float *l2sum1;
    integer *l2ptr1;
    integer *l2ptr2;
    logical *hyst;
    
    /* System generated locals */
    integer pebuf_offset;
    float temp;
    
    /* Local variables */
    integer i;
    integer *lasti;
    float l2sum2;
    float *fpc;


    n = st->n;
    d = st->d;
    fpc = &(st->fpc);
    l2buf = &(st->l2buf[0]);
    l2sum1 = &(st->l2sum1);
    l2ptr1 = &(st->l2ptr1);
    l2ptr2 = &(st->l2ptr2);
    lasti = &(st->lasti);
    hyst = &(st->hyst);
    
    /* Parameter adjustments */
    if (osbuf) {
        --osbuf;
    }
    if (pebuf) {
        pebuf_offset = sbufl;
        pebuf -= pebuf_offset;
    }
    
    /* Function Body */
    
    /*       The following line subtracted a hard-coded "180" from LASTI, */
    /*       instead of using a variable like LFRAME or a constant like */
    /*       MAXFRM.  I changed it to LFRAME, for "generality". */
    if (*hyst) {
        *lasti -= lframe;
    }
    for (i = sbufh - lframe + 1; i <= sbufh; ++i) {
    /*   Compute FPC; Use old FPC on divide by zero; Clamp FPC to +/- 1. */
        n = (pebuf[i] * pebuf[i - 1] + n * 63.f) * 0.015625f;
        /* Computing 2nd power */
        temp = pebuf[i - 1];
        d = (temp * temp + d * 63.f) * 0.015625f;
        if (d != 0.f) {
            if(n > d || n < -(d)){
                *fpc = (n<0.0f)?-1.0f:1.0f;
            } else {
                *fpc = n / d;
            }
        }
        
        l2sum2 = l2buf[*l2ptr1 - 1];
        *l2sum1 = *l2sum1 - l2buf[*l2ptr2 - 1] + *fpc;
        l2buf[*l2ptr2 - 1] = *l2sum1;
        l2buf[*l2ptr1 - 1] = *fpc;
        *l2ptr1 = *l2ptr1 % 16 + 1;
        *l2ptr2 = *l2ptr2 % 16 + 1;
        temp = *l2sum1 - l2sum2;
        if (temp > 1.7f || temp < -1.7f) {
            if (! (*hyst)) {
                /*   Ignore if buffer full */
                if (*osptr <= oslen) {
                    osbuf[*osptr] = i - 9;
                    ++(*osptr);
                }
                *hyst = TRUE;
            }
            *lasti = i;
            /*       After one onset detection, at least OSHYST sample times must go */
            /*       by before another is allowed to occur. */
        } else if ((*hyst) && i - *lasti >= 10) {
            *hyst = FALSE;
        }
    }
    st->n = n;
    st->d = d;
} /* onset_ */

/* 	ANALYS Version 55 */

/* ****************************************************************** */

/* SUBROUTINE ANALYS */

/* Input: */
/*  SPEECH */
/*       Indices 1 through LFRAME read. */
/* Output: */
/*  VOICE */
/*       Indices 1 through 2 written. */
/*  PITCH */
/*       Written in subroutine DYPTRK, and then perhaps read and written */
/*       some more. */
/*  RMS */
/*       Written. */
/*  RC */
/*       Indices 1 through ORDER written (ORDER defined in contrl.fh). */

/* ENTRY PITDEC */

/* Input: */
/*  PITCH   - Encoded pitch index */
/* Output: */
/*  PTAU    - Decoded pitch period */

/* This entry has no local state.  It accesses a "constant" array */
/* declared in ANALYS. */

void analys(float *speech, integer *voice, integer 
            *pitch, float *rms, float *rc, struct lpc10_encoder_state *st)
{
    /* Initialized data */
    
    static integer tau[60] = { 20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,
        35,36,37,38,39,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,
        74,76,78,80,84,88,92,96,100,104,108,112,116,120,124,128,132,136,
        140,144,148,152,156 };
    static integer buflim[4] = { 181,720,25,720 };
    static float precoef = .9375f;
    
    /* System generated locals */
    integer i1;
    
    /* Local variables */
    float amdf[60];
    integer half;
    float abuf[156];
    float *bias;
    integer *awin;
    integer midx, ewin[6]	/* was [2][3] */;
    float ivrc[2], temp;
    float *zpre;
    integer *vwin;
    integer i, j, lanal;
    float *inbuf, *pebuf;
    float *lpbuf, *ivbuf;
    float *rcbuf;
    integer *osptr;
    integer *osbuf;
    integer ipitch;
    integer *obound;
    integer *voibuf;
    integer mintau;
    float *rmsbuf;
    integer minptr, maxptr;
    float phi[100]	/* was [10][10] */, psi[10];
    integer lframe;
    
    /* Parameter adjustments */
    if (speech) {
        --speech;
    }
    if (voice) {
        --voice;
    }
    if (rc) {
        --rc;
    }
    
    /* Function Body */
    
    /*   Calculations are done on future frame due to requirements */
    /*   of the pitch tracker.  Delay RMS and RC's 2 frames to give */
    /*   current frame parameters on return. */
    /*   Update all buffers */
    
    inbuf = &(st->inbuf[0]);
    pebuf = &(st->pebuf[0]);
    lpbuf = &(st->lpbuf[0]);
    ivbuf = &(st->ivbuf[0]);
    bias = &(st->bias);
    osbuf = &(st->osbuf[0]);
    osptr = &(st->osptr);
    obound = &(st->obound[0]);
    vwin = &(st->vwin[0]);
    awin = &(st->awin[0]);
    voibuf = &(st->voibuf[0]);
    rmsbuf = &(st->rmsbuf[0]);
    rcbuf = &(st->rcbuf[0]);
    zpre = &(st->zpre);
    lframe = st->lframe;
    
    i1 = 720 - lframe;
    for (i = lframe; i < i1; ++i) {
        inbuf[i - lframe] = inbuf[i];
        pebuf[i - lframe] = pebuf[i];
    }
    i1 = 540 - lframe - 229;
    for (i = 0; i <= i1; ++i) {
        ivbuf[i] = ivbuf[lframe + i];
    }
    i1 = 720 - lframe - 25;
    for (i = 0; i <= i1; ++i) {
        lpbuf[i] = lpbuf[lframe + i];
    }
    j = 1;
    i1 = (*osptr) - 1;
    for (i = 1; i <= i1; ++i) {
        if (osbuf[i - 1] > lframe) {
            osbuf[j - 1] = osbuf[i - 1] - lframe;
            ++j;
        }
    }
    *osptr = j;
    voibuf[0] = voibuf[2];
    voibuf[1] = voibuf[3];
    for (i = 1; i <= 2; ++i) {
        vwin[(i << 1) - 2] = vwin[((i + 1) << 1) - 2] - lframe;
        vwin[(i << 1) - 1] = vwin[((i + 1) << 1) - 1] - lframe;
        awin[(i << 1) - 2] = awin[((i + 1) << 1) - 2] - lframe;
        awin[(i << 1) - 1] = awin[((i + 1) << 1) - 1] - lframe;
        obound[i - 1] = obound[i];
        voibuf[i << 1] = voibuf[(i + 1) << 1];
        voibuf[(i << 1) + 1] = voibuf[((i + 1) << 1) + 1];
        rmsbuf[i - 1] = rmsbuf[i];
        for (j = 1; j <= 10; ++j) {
            rcbuf[j + i * 10 - 11] = rcbuf[j + (i + 1) * 10 - 11];
        }
    }
    temp = 0.f;
    for (i = 1; i <= lframe; ++i) {
        inbuf[720 - lframe + i - 181] = speech[i] * 4096.f - (*bias);
        temp += inbuf[720 - lframe + i - 181];
    }
    if (temp > (float) lframe) {
        *bias += 1;
    }
    if (temp < (float) (-lframe)) {
        *bias += -1;
    }
    /*   Place Voicing Window */
    i = 721 - lframe;
    preemp(&inbuf[i - 181], &pebuf[i - 181], lframe, precoef, zpre);
    onset(pebuf, osbuf, osptr, 10, 181, 720, lframe, st);
    
    placev_(osbuf, osptr, &obound[2], vwin, 3, &lframe, 90, 156, 307);
    /*       LPFILT reads indices LBUFH-LFRAME-29 = 511 through LBUFH = 720 */
    /*       of INBUF, and writes indices LBUFH+1-LFRAME = 541 through LBUFH */
    /*       = 720 of LPBUF. */
    lpfilt(&inbuf[228], &lpbuf[384], 312, lframe);
    /*       IVFILT reads indices (PWINH-LFRAME-7) = 353 through PWINH = 540 */
    /*       of LPBUF, and writes indices (PWINH-LFRAME+1) = 361 through */
    /*       PWINH = 540 of IVBUF. */
    ivfilt(&lpbuf[204], ivbuf, 312, lframe, ivrc);
    /*       TBDM reads indices PWINL = 229 through */
    /*       (PWINL-1)+MAXWIN+(TAU(LTAU)-TAU(1))/2 = 452 of IVBUF, and writes */
    /*       indices 1 through LTAU = 60 of AMDF. */
    tbdm_(ivbuf, 156, tau, 60, amdf, &minptr, &maxptr, &mintau);
    /*   voicing decisions. */
    for (half = 1; half <= 2; ++half) {
        voicin_(&vwin[4], inbuf, lpbuf, buflim, &half, &amdf[minptr - 1], &
            amdf[maxptr - 1], &mintau, ivrc, obound, voibuf, st);
    }
    /*   Find the minimum cost pitch decision over several frames */
    /*   given the current voicing decision and the AMDF array */
    dyptrk(amdf, 60, &minptr, &voibuf[7], pitch, &midx, st);
    ipitch = tau[midx - 1];
    /*   Place spectrum analysis and energy windows */
    placea_(&ipitch, voibuf, &obound[2], &c__3, vwin, awin, ewin, &lframe, &c__156);
    /*  Remove short term DC bias over the analysis window, Put result in ABUF */
    lanal = awin[5] + 1 - awin[4];
    dcbias(lanal, &pebuf[awin[4] - 181], abuf);
    /*       ABUF(1:LANAL) is now defined.  It is equal to */
    /*       PEBUF(AWIN(1,AF):AWIN(2,AF)) corrected for short term DC bias. */
    /*   Compute RMS over integer number of pitch periods within the */
    /*   analysis window. */
    /*   Note that in a hardware implementation this computation may be */
    /*   simplified by using diagonal elements of PHI computed by MLOAD. */
    i1 = ewin[5] - ewin[4] + 1;
    energy(i1, &abuf[ewin[4] - awin[4]], &rmsbuf[2]);
    /*   Matrix load and invert, check RC's for stability */
    mload(1, lanal, abuf, phi, psi);
    invert(phi, psi, &rcbuf[20]);
    rcchk(&rcbuf[10], &rcbuf[20]);
    /*   Set return parameters */
    voice[1] = voibuf[2];
    voice[2] = voibuf[3];
    *rms = rmsbuf[0];
    for (i = 1; i <= 10; ++i) {
        rc[i] = rcbuf[i - 1];
    }
} /* analys_ */
