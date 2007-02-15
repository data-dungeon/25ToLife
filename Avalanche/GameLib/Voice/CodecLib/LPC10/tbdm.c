#include "lpc10int.h"

/*  Compute Average Magnitude Difference Function */

/* Inputs: */
/*  SPEECH - Low pass filtered speech */
/*           Indices MIN_N1 through MAX_N1+LPITA-1 are read, where */
/*      MIN_N1 = (MAXLAG - MAX_TAU)/2+1  MAX_TAU = max of TAU(I) for I=1,LTAU
*/
/*      MAX_N1 = (MAXLAG - MIN_TAU)/2+1  MIN_TAU = min of TAU(I) for I=1,LTAU
*/
/*  LPITA  - Length of speech buffer */
/*  TAU    - Table of lags */
/*           Indices 1 through LTAU read. */
/*  LTAU   - Number of lag values to compute */
/*  MAXLAG - Maximum possible lag value */
/* Outputs: */
/*  (All of these outputs are also read, but only after being written.) */
/*  AMDF   - Average Magnitude Difference for each lag in TAU */
/*           Indices 1 through LTAU written */
/*  MINPTR - Index of minimum AMDF value */
/*  MAXPTR - Index of maximum AMDF value */

static void difmag(float *speech, integer lpita, integer *tau, 
            integer ltau, integer maxlag, float *amdf,
            integer *minptr, integer *maxptr)
{
    /* Local variables */
    integer i, j, n1, n2;
    integer lmin, lmax;
    float sum;
    
    /* Parameter adjustments */
    --amdf;
    --tau;
    --speech;
    
    /* Function Body */
    lmin = 1;
    lmax = 1;

    for (i = 1; i <= ltau; ++i) {
        integer t = tau[i];

        n1 = (maxlag - t) / 2 + 1;
        n2 = n1 + lpita - 1;
        sum = 0.f;
        t += n1;
        for (j = n1; j <= n2; j += 4, t += 4) {
            float temp = speech[j] - speech[t];

            if(temp < 0.0f)
            {
                sum -= temp;
            }
            else
            {
                sum += temp;
            }
        }
        if (sum < amdf[lmin]) {
            lmin = i;
        } else if (sum > amdf[lmax]) {
            lmax = i;
        }
        amdf[i] = sum;
    }
    *minptr = lmin;
    *maxptr = lmax;
} /* difmag_ */

/*TURBO DIFMAG: Compute High Resolution Average Magnitude Difference Function
*/

/* Note: There are several constants in here that appear to depend on a */
/* particular TAU table.  That's not a problem for the LPC10 coder, but */
/* watch out if you change the contents of TAU in the subroutine ANALYS. */

/* Input: */
/*  SPEECH - Low pass filtered speech */
/*           Indices 1 through MAX+LPITA-1 are read, where: */
/*           MAX = (TAU(LTAU)-TAU(1))/2+1 */
/*           (If TAU(1) .LT. 39, then larger indices could be read */
/*           by the last call to DIFMAG below.) */
/*  LPITA  - Length of speech buffer */
/*  TAU    - Table of lags, sorted in increasing order. */
/*           Indices 1 through LTAU read. */
/*  LTAU   - Number of lag values to compute */
/* Output: */
/*  AMDF   - Average Magnitude Difference for each lag in TAU */
/*          Indices 1 through LTAU written, and several might then be read.*/
/*  MINPTR - Index of minimum AMDF value */
/*  MAXPTR - Index of maximum AMDF value within +/- 1/2 octave of min */
/*  MINTAU - Lag corresponding to minimum AMDF value */

void tbdm_(float *speech, integer lpita, integer *tau, 
          integer ltau, float *amdf, integer *minptr, integer *maxptr, integer *mintau)
{
    /* Local variables */
    float amdf2[6];
    integer minp2, ltau2, maxp2, i, j;
    integer minamd, ptr, tau2[6];
    
    /*   Compute full AMDF using log spaced lags, find coarse minimum */
    /* Parameter adjustments */
    --speech;
    --amdf;
    --tau;
    
    /* Function Body */
    difmag(&speech[1], lpita, &tau[1], ltau, tau[ltau], &amdf[1], minptr, maxptr);
    *mintau = tau[*minptr];
    minamd = (integer)amdf[*minptr];
    /*   Build table containing all lags within +/- 3 of the AMDF minimum */
    /*    excluding all that have already been computed */
    ltau2 = 0;
    ptr = *minptr - 2;
    /* Computing MIN */
    j = min(*mintau + 3, tau[ltau] - 1);
    /* Computing MAX */
    i = max(*mintau - 3, 41);
    for (; i <= j; ++i) {
        while(tau[ptr] < i) {
            ++ptr;
        }
        if (tau[ptr] != i) {
            ++ltau2;
            tau2[ltau2 - 1] = i;
        }
    }
    /*   Compute AMDF of the new lags, if there are any, and choose one */
    /*    if it is better than the coarse minimum */
    if (ltau2 > 0) {
        difmag(&speech[1], lpita, tau2, ltau2, tau[ltau], amdf2, &minp2, &maxp2);
        if (amdf2[minp2 - 1] < (float) minamd) {
            *mintau = tau2[minp2 - 1];
            minamd = (integer)amdf2[minp2 - 1];
        }
    }
    /*   Check one octave up, if there are any lags not yet computed */
    if (*mintau >= 80) {
        i = *mintau / 2;
        if ((i & 1) == 0) {
            ltau2 = 2;
            tau2[0] = i - 1;
            tau2[1] = i + 1;
        } else {
            ltau2 = 1;
            tau2[0] = i;
        }
        difmag(&speech[1], lpita, tau2, ltau2, tau[ltau], amdf2, &minp2, &maxp2);
        if (amdf2[minp2 - 1] < (float) minamd) {
            *mintau = tau2[minp2 - 1];
            minamd = (integer)amdf2[minp2 - 1];
            *minptr += -20;
        }
    }
    /*   Force minimum of the AMDF array to the high resolution minimum */
    amdf[*minptr] = (float) minamd;
    /*   Find maximum of AMDF within 1/2 octave of minimum */
    /* Computing MAX */
    *maxptr = max(*minptr - 5,1);
    /* Computing MIN */
    j = min(*minptr + 5,ltau);
    for (i = *maxptr + 1; i <= j; ++i) {
        if (amdf[i] > amdf[*maxptr]) {
            *maxptr = i;
        }
    }
} /* tbdm_ */

