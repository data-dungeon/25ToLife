#include "lpc10int.h"


/* Input: */
/*  OSBUF	Buffer which holds sorted indexes of onsets */
/*       	I believe that only indices 1 through OSPTR-1 can be read. */
/*  OSLEN */
/*  OSPTR	Free pointer into OSBUF */
/*  AF */
/*  LFRAME */
/*  MINWIN */
/*  MAXWIN */
/*  DVWINL */
/*  DVWINH	(This argument is never used.  Should it be?) */
/* Input/Output: */
/*  VWIN		Buffer of Voicing Window Positions (Modified) */
/*       	Index (2,AF-1) is read. */
/*       	Indices (1,AF) and (2,AF) are written, */
/*       	and then possibly read. */
/*       	All other indices are unused. */
/*              In all cases, the final values will satsify the condition:*/
/*               VWIN(2,AF)-VWIN(1,AF)+1 .LE. MAXWIN */
/*               I'm not certain yet, but they may also satisfy: */
/*               MINWIN .LE. VWIN(2,AF)-VWIN(1,AF)+1 */
/* Output: */
/*  OBOUND	This variable is set by this procedure and used */
/* 		in placing analysis windows (PLACEA).  Bit 1 */
/* 		indicates whether an onset bounds the left side */
/* 		of the voicing window, and bit 2 indicates whether */
/* 		an onset bounds the right side of the voicing window. */

void placev_(integer *osbuf, integer *osptr, integer *obound, integer *vwin, integer af,
            integer *lframe, integer minwin, integer maxwin, integer dvwinl)
{
    /* Local variables */
    logical crit;
    integer i, q, osptr1, hrange, lrange;
    
    /* Compute the placement range */
    /* Parameter adjustments */
    --osbuf;
    vwin -= 3;
    
    /* Function Body */
    /* Computing MAX */
    lrange = max(vwin[((af - 1) << 1) + 2] + 1, (af - 2) * *lframe + 1);
    hrange = af * *lframe;
    /* Compute OSPTR1, so the following code only looks at relevant onsets. */
    for (osptr1 = *osptr - 1; osptr1 >= 1; --osptr1) {
        if (osbuf[osptr1] <= hrange) {
            break;
        }
    }
    ++osptr1;
    /* Check for case 1 first (fast case): */
    if (osptr1 <= 1 || osbuf[osptr1 - 1] < lrange) {
        /* Computing MAX */
        vwin[(af << 1) + 1] = max(vwin[((af - 1) << 1) + 2] + 1,dvwinl);
        vwin[(af << 1) + 2] = vwin[(af << 1) + 1] + maxwin - 1;
        *obound = 0;
    } else {
        /* Search backward in OSBUF for first onset in range. */
        /* This code relies on the above check being performed first. */
        for (q = osptr1 - 1; q >= 1; --q) {
            if (osbuf[q] < lrange) {
                break;
            }
        }
        ++q;
        /* Check for case 2 (placement before onset): */
        /* Check for critical region exception: */
        crit = FALSE;
        for (i = q + 1; i <= (osptr1 - 1); ++i) {
            if (osbuf[i] - osbuf[q] >= minwin) {
                crit = TRUE;
                break;
            }
        }
        /* Computing MAX */
        if (! crit && osbuf[q] > max((af - 1) * *lframe, lrange + minwin - 1)) {
            vwin[(af << 1) + 2] = osbuf[q] - 1;
            /* Computing MAX */
            vwin[(af << 1) + 1] = max(lrange, vwin[(af << 1) + 2] - maxwin + 1);
            *obound = 2;
            /* Case 3 (placement after onset) */
        } else {
            vwin[(af << 1) + 1] = osbuf[q];
L110:
            ++q;
            if (q >= osptr1) {
                goto L120;
            }
            if (osbuf[q] > vwin[(af << 1) + 1] + maxwin) {
                goto L120;
            }
            if (osbuf[q] < vwin[(af << 1) + 1] + minwin) {
                goto L110;
            }
            vwin[(af << 1) + 2] = osbuf[q] - 1;
            *obound = 3;
            return;
L120:
            /* Computing MIN */
            vwin[(af << 1) + 2] = min(vwin[(af << 1) + 1] + maxwin - 1,hrange);
            *obound = 1;
        }
    }
} /* placev_ */

