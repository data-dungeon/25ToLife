/*

  $Log: /PlayStation2/Libs/liblgcodec/codecs/lpc10/pitsyn.c $
 * 
 * 1     3/24/03 9:16p Lbldr
 * 
 * 1     8/13/02 2:52p Lbldr
  Revision 1.1  2000/06/05 04:45:12  robertj
  Added LPC-10 2400bps codec
  
    * Revision 1.2  1996/08/20  20:40:12  jaf
    * Removed all static local variables that were SAVE'd in the Fortran
    * code, and put them in struct lpc10_decoder_state that is passed as an
    * argument.
    *
    * Removed init function, since all initialization is now done in
    * init_lpc10_decoder_state().
    *
    * Revision 1.1  1996/08/19  22:31:12  jaf
    * Initial revision
    *
    
*/

#include "lpc10int.h"
#include "ftol.h"

/* ***************************************************************** */

/* 	PITSYN Version 53 */

/* ***************************************************************** */

/*   Synthesize a single pitch epoch */

/* Input: */
/*  ORDER  - Synthesis order (number of RC's) */
/*  VOICE  - Half frame voicing decisions */
/*           Indices 1 through 2 read. */
/*  LFRAME - Length of speech buffer */
/* Input/Output: */
/*  PITCH  - Pitch */
/*           This value should be in the range MINPIT (20) to MAXPIT */
/*           (156), inclusive. */
/*           PITCH can be modified under some conditions. */
/*  RMS    - Energy  (can be modified) */
/*           RMS is changed to 1 if the value passed in is less than 1. */
/*  RC     - Reflection coefficients */
/*           Indices 1 through ORDER can be temporarily overwritten with */
/*           RCO, and then replaced with original values, under some */
/*           conditions. */
/* Output: */
/*  IVUV   - Pitch epoch voicing decisions */
/*           Indices (I) of IVUV, IPITI, and RMSI are written, */
/*           and indices (J,I) of RCI are written, */
/*           where I ranges from 1 to NOUT, and J ranges from 1 to ORDER. */
/*  IPITI  - Pitch epoch length */
/*  RMSI   - Pitch epoch energy */
/*  RCI    - Pitch epoch RC's */
/*  NOUT   - Number of pitch periods in this frame */
/*           This is at least 0, at least 1 if MAXPIT .LT. LFRAME (this */
/*           is currently true on every call), and can never be more than */
/*           (LFRAME+MAXPIT-1)/PITCH, which is currently 16 with */
/*           LFRAME=180, MAXPIT=156, and PITCH .GE. 20, as SYNTHS */
/*           guarantees when it calls this subroutine. */
/*  RATIO  - Previous to present energy ratio */
/*           Always assigned a value. */

int pitsyn_(integer *order, integer *voice, 
            integer *pitch, float *rms, float *rc, integer *lframe, integer *ivuv, 
            integer *ipiti, float *rmsi, float *rci, integer *nout, float *ratio,
            struct lpc10_decoder_state *st)
{
    /* Initialized data */
    
    float *rmso;
    logical *first;
    
    /* System generated locals */
    integer rci_dim1 = *order;
    integer rci_offset;
    
    /* Local variables */
    float alrn, alro, yarc[10], prop;
    integer i, j, vflag, jused, lsamp;
    integer *jsamp;
    float slope;
    integer *ipito;
    float uvpit;
    integer ip, nl, ivoice;
    integer *ivoico;
    integer istart;
    float *rco;
    float xxy;
    
    /* Parameter adjustments */
    if (rc) {
        --rc;
    }
    if (rci) {
        rci_offset = rci_dim1 + 1;
        rci -= rci_offset;
    }
    if (voice) {
        --voice;
    }
    if (ivuv) {
        --ivuv;
    }
    if (ipiti) {
        --ipiti;
    }
    if (rmsi) {
        --rmsi;
    }
    
    /* Function Body */
    ivoico = &(st->ivoico);
    ipito = &(st->ipito);
    rmso = &(st->rmso);
    rco = &(st->rco[0]);
    jsamp = &(st->jsamp);
    first = &(st->first_pitsyn);
    
    if (*rms < 1.f) {
        *rms = 1.f;
    }
    if (*rmso < 1.f) {
        *rmso = 1.f;
    }
    uvpit = 0.f;
    *ratio = *rms / (*rmso + 8.f);
    if (*first) {
        lsamp = 0;
        ivoice = voice[2];
        if (ivoice == 0) {
            *pitch = *lframe / 4;
        }
        *nout = *lframe / *pitch;
        *jsamp = *lframe - *nout * *pitch;
        
        /*          SYNTHS only calls this subroutine with PITCH in the range 20 */
        /*          to 156.  LFRAME = MAXFRM = 180, so NOUT is somewhere in the */
        /*          range 1 to 9. */
        
        /*          JSAMP is "LFRAME mod PITCH", so it is in the range 0 to */
        /*          (PITCH-1), or 0 to MAXPIT-1=155, after the first call. */
        
        for (i = 1; i <= *nout; ++i) {
            for (j = 1; j <= *order; ++j) {
                rci[j + i * rci_dim1] = rc[j];
            }
            ivuv[i] = ivoice;
            ipiti[i] = *pitch;
            rmsi[i] = *rms;
        }
        *first = FALSE;
    } else {
        vflag = 0;
        lsamp = *lframe + *jsamp;
        slope = (*pitch - *ipito) / (float) lsamp;
        *nout = 0;
        jused = 0;
        istart = 1;
        if (voice[1] == *ivoico && voice[2] == voice[1]) {
            if (voice[2] == 0) {
                /* SSUV - -   0  ,  0  ,  0 */
                *pitch = *lframe / 4;
                *ipito = *pitch;
                if (*ratio > 8.f) {
                    *rmso = *rms;
                }
            }
            /* SSVC - -   1  ,  1  ,  1 */
            slope = (*pitch - *ipito) / (float) lsamp;
            ivoice = voice[2];
        } else {
            if (*ivoico != 1) {
                if (*ivoico == voice[1]) {
                    /* UV2VC2 - -  0  ,  0  ,  1 */
                    nl = lsamp - *lframe / 4;
                } else {
                    /* UV2VC1 - -  0  ,  1  ,  1 */
                    nl = lsamp - *lframe * 3 / 4;
                }
                ipiti[1] = nl / 2;
                ipiti[2] = nl - ipiti[1];
                ivuv[1] = 0;
                ivuv[2] = 0;
                rmsi[1] = *rmso;
                rmsi[2] = *rmso;
                for (i = 1; i <= *order; ++i) {
                    rci[i + rci_dim1] = rco[i - 1];
                    rci[i + (rci_dim1 << 1)] = rco[i - 1];
                    rco[i - 1] = rc[i];
                }
                slope = 0.f;
                *nout = 2;
                *ipito = *pitch;
                jused = nl;
                istart = nl + 1;
                ivoice = 1;
            } else {
                if (*ivoico != voice[1]) {
                    /* VC2UV1 - -   1  ,  0  ,  0 */
                    lsamp = *lframe / 4 + *jsamp;
                } else {
                    /* VC2UV2 - -   1  ,  1  ,  0 */
                    lsamp = *lframe * 3 / 4 + *jsamp;
                }
                for (i = 1; i <= *order; ++i) {
                    yarc[i - 1] = rc[i];
                    rc[i] = rco[i - 1];
                }
                ivoice = 1;
                slope = 0.f;
                vflag = 1;
            }
        }
        /* Here is the value of most variables that are used below, depending on */
        /* the values of IVOICO, VOICE(1), and VOICE(2).  VOICE(1) and VOICE(2) */
        /* are input arguments, and IVOICO is the value of VOICE(2) on the */
        /* previous call (see notes for the IF (NOUT .NE. 0) statement near the */
        /* end).  Each of these three values is either 0 or 1.  These three */
        /* values below are given as 3-bit long strings, in the order IVOICO, */
        /* VOICE(1), and VOICE(2).  It appears that the code above assumes that */
        /* the bit sequences 010 and 101 never occur, but I wonder whether a */
        /* large enough number of bit errors in the channel could cause such a */
        /* thing to happen, and if so, could that cause NOUT to ever go over 11? */
        
        /* Note that all of the 180 values in the table are really LFRAME, but */
        /* 180 has fewer characters, and it makes the table a little more */
        /* concrete.  If LFRAME is ever changed, keep this in mind.  Similarly, */
        /* 135's are 3*LFRAME/4, and 45's are LFRAME/4.  If LFRAME is not a */
        /* multiple of 4, then the 135 for NL-JSAMP is actually LFRAME-LFRAME/4, */
        /* and the 45 for NL-JSAMP is actually LFRAME-3*LFRAME/4. */
        
        /* Note that LSAMP-JSAMP is given as the variable.  This was just for */
        /* brevity, to avoid adding "+JSAMP" to all of the column entries. */
        /* Similarly for NL-JSAMP. */
        
        /* Variable    | 000  001    011,010  111       110       100,101 */
        /* ------------+-------------------------------------------------- */
        /* ISTART      | 1    NL+1   NL+1     1         1         1 */
        /* LSAMP-JSAMP | 180  180    180      180       135       45 */
        /* IPITO       | 45   PITCH  PITCH    oldPITCH  oldPITCH  oldPITCH */
        /* SLOPE       | 0    0      0        seebelow  0         0 */
        /* JUSED       | 0    NL     NL       0         0         0 */
        /* PITCH       | 45   PITCH  PITCH    PITCH     PITCH     PITCH */
        /* NL-JSAMP    | --   135    45       --        --        -- */
        /* VFLAG       | 0    0      0        0         1         1 */
        /* NOUT        | 0    2      2        0         0         0 */
        /* IVOICE      | 0    1      1        1         1         1 */
        
        /* while_loop  | once once   once     once      twice     twice */
        
        /* ISTART      | --   --     --       --        JUSED+1   JUSED+1 */
        /* LSAMP-JSAMP | --   --     --       --        180       180 */
        /* IPITO       | --   --     --       --        oldPITCH  oldPITCH */
        /* SLOPE       | --   --     --       --        0         0 */
        /* JUSED       | --   --     --       --        ??        ?? */
        /* PITCH       | --   --     --       --        PITCH     PITCH */
        /* NL-JSAMP    | --   --     --       --        --        -- */
        /* VFLAG       | --   --     --       --        0         0 */
        /* NOUT        | --   --     --       --        ??        ?? */
        /* IVOICE      | --   --     --       --        0         0 */
        
        
        /* UVPIT is always 0.0 on the first pass through the DO WHILE (.TRUE.) */
        /* loop below. */
        
        /* The only possible non-0 value of SLOPE (in column 111) is */
        /* (PITCH-IPITO)/FLOAT(LSAMP) */
        
        /* Column 101 is identical to 100.  Any good properties we can prove */
        /* for 100 will also hold for 101.  Similarly for 010 and 011. */
        
        /* SYNTHS calls this subroutine with PITCH restricted to the range 20 to */
        /* 156.  IPITO is similarly restricted to this range, after the first */
        /* call.  IP below is also restricted to this range, given the */
        /* definitions of IPITO, SLOPE, UVPIT, and that I is in the range ISTART */
        /* to LSAMP. */
        
        for(;;) {
            
            for (i = istart; i <= lsamp; ++i) {
                if (uvpit != 0.f) {
                    ip = round(uvpit);
                }
                else {
                    ip = round(*ipito + slope * i + .5f);
                }
                
                if (ip <= i - jused) {
                    ++(*nout);
                    ipiti[*nout] = ip;
                    *pitch = ip;
                    ivuv[*nout] = ivoice;
                    jused += ip;
                    prop = (jused - ip / 2) / (float) lsamp;
                    for (j = 1; j <= *order; ++j) {
                        alro = (float)log((rco[j - 1] + 1) / (1 - rco[j - 1]));
                        alrn = (float)log((rc[j] + 1) / (1 - rc[j]));
                        xxy = alro + prop * (alrn - alro);
                        xxy = (float)exp(xxy);
                        rci[j + *nout * rci_dim1] = (xxy - 1) / (xxy + 1);
                    }
                    rmsi[*nout] = (float)(log(*rmso) + prop * (log(*rms) - log(*rmso)));
                    rmsi[*nout] = (float)exp(rmsi[*nout]);
                }
            }
            if (vflag != 1) {
                break;
            }
            
            /*             I want to prove what range UVPIT must lie in after the */
            /*             assignments to it below.  To do this, I must determine */
            /*             what range (LSAMP-ISTART) must lie in, after the */
            /*             assignments to ISTART and LSAMP below. */
            
            /*             Let oldLSAMP be the value of LSAMP at this point in the */
            /*             execution.  This is 135+JSAMP in state 110, or 45+JSAMP in */
            /*             states 100 or 101. */
            
            /*             Given the loop invariant on JUSED above, we know that: */
            
            /*             (oldLSAMP - MAXPIT + 1) .LE. JUSED .LE. oldLSAMP */
            
            /*             ISTART is one more than this. */
            
            /*             Let newLSAMP be the value assigned to LSAMP below. This */
            /*             is 180+JSAMP.  Thus (newLSAMP-oldLSAMP) is either 45 or */
            /*             135, depending on the state. */
            
            /*             Thus, the range of newLSAMP-ISTART is: */
            
            /*             (newLSAMP-(oldLSAMP+1)) .LE. newLSAMP-ISTART */
            /*             .LE. (newLSAMP-(oldLSAMP - MAXPIT + 2)) */
            
            /*             or: */
            
            /*             46 .LE. newLSAMP-ISTART .LE. 133+MAXPIT .EQ. 289 */
            
            /*             Therefore, UVPIT is in the range 23 to 144 after the first */
            /*             assignment to UVPIT below, and after the conditional */
            /*             assignment, it is in the range 23 to 90. */
            
            /*             The important thing is that it is in the range 20 to 156, */
            /*             so that in the loop above, IP is always in this range. */
            
            vflag = 0;
            istart = jused + 1;
            lsamp = *lframe + *jsamp;
            slope = 0.f;
            ivoice = 0;
            uvpit = (float) ((lsamp - istart) / 2);
            if (uvpit > 90.f) {
                uvpit /= 2;
            }
            *rmso = *rms;
            for (i = 1; i <= *order; ++i) {
                rco[i - 1] = rc[i] = yarc[i - 1];
            }
        }
        *jsamp = lsamp - jused;
    }
    /*       Given that the maximum pitch period MAXPIT .LT. LFRAME (this is 
    */
    /*       currently true on every call, since SYNTHS always sets */
    /*       LFRAME=180), NOUT will always be .GE. 1 at this point. */
    if (*nout != 0) {
        *ivoico = voice[2];
        *ipito = *pitch;
        *rmso = *rms;
        for (i = 1; i <= *order; ++i) {
            rco[i - 1] = rc[i];
        }
    }
    return 0;
} /* pitsyn_ */
