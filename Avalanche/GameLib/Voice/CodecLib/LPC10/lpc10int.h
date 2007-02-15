#ifndef LPC10INT_H
#define LPC10INT_H

#include "lpc10.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
 #pragma warning (disable:4056) /* to disable bogus warning in MSVC 5.0 */
 #pragma warning (disable:4001) /* to disable warning in <math.h> */
 #pragma warning (disable:4711) /* to disable automatic inline warning */
#endif
#include <math.h>

#define LPC10_BITS_IN_COMPRESSED_FRAME 54

typedef long		INT32;

#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif

#ifndef min
#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))
#endif


/* The initial values for every member of this structure is 0, except
   where noted in comments. */
typedef INT32 integer;
typedef INT32 logical;

typedef struct lpc10_encoder_state {
    /* State used only by function hp100 */
    float z11;
    float z21;
    float z12;
    float z22;
    
    /* State used by function analys */
    float inbuf[540], pebuf[540];
    float lpbuf[696], ivbuf[312];
    float bias;
    integer osbuf[10];  /* no initial value necessary */
    integer osptr;     /* initial value 1 */
    integer obound[3];
    integer vwin[6]	/* was [2][3] */;   /* initial value vwin[4] = 307; vwin[5] = 462; */
    integer awin[6]	/* was [2][3] */;   /* initial value awin[4] = 307; awin[5] = 462; */
    integer voibuf[8]	/* was [2][4] */;
    float rmsbuf[3];
    float rcbuf[30]	/* was [10][3] */;
    float zpre;


    /* State used by function onset */
    float n;
    float d;   /* initial value 1.f */
    float fpc;   /* no initial value necessary */
    float l2buf[16];
    float l2sum1;
    integer l2ptr1;   /* initial value 1 */
    integer l2ptr2;   /* initial value 9 */
    integer lasti;    /* no initial value necessary */
    logical hyst;   /* initial value FALSE_ */

    /* State used by function voicin */
    float dither;   /* initial value 20.f */
    float snr;
    float maxmin;
    float voice[6]	/* was [2][3] */;   /* initial value is probably unnecessary */
    integer lbve, lbue, fbve, fbue;
    integer ofbue, sfbue;
    integer olbue, slbue;

    /* State used by function dyptrk */
    float s[60];
    integer p[120]	/* was [60][2] */;
    integer ipoint;
    float alphax;

    /* State used by function chanwr */
    integer isync;

    /* added for lower rates */
    integer lframe;

    /* misc */
    integer order;
}_lpc10_encoder_state;


typedef struct lpc10_decoder_state {

    /* State used by function decode */
    integer iptold;   /* initial value 60 */
    logical first;   /* initial value TRUE_ */
    integer ivp2h;
    integer iovoic;
    integer iavgp;   /* initial value 60 */
    integer erate;
    integer drc[30]	/* was [3][10] */;
    integer dpit[3];
    integer drms[3];

    /* State used by function synths */
    float buf[360];
    integer buflen;   /* initial value 180 */

    /* State used by function pitsyn */
    integer ivoico;   /* no initial value necessary as long as first_pitsyn is initially TRUE_ */
    integer ipito;   /* no initial value necessary as long as first_pitsyn is initially TRUE_ */
    float rmso;   /* initial value 1.f */
    float rco[10];   /* no initial value necessary as long as first_pitsyn is initially TRUE_ */
    integer jsamp;   /* no initial value necessary as long as first_pitsyn is initially TRUE_ */
    logical first_pitsyn;   /* initial value TRUE_ */

    /* State used by function bsynz */
    integer ipo;
    float exc[166];
    float exc2[166];
    float lpi1;
    float lpi2;
    float lpi3;
    float hpi1;
    float hpi2;
    float hpi3;
    float rmso_bsynz;

    /* State used by function deemp */
    float dei1;
    float dei2;
    float deo1;
    float deo2;
    float deo3;

    /* added for lower rates */
    integer lframe;

    /* misc */
    integer order;
}_lpc10_decoder_state;


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* LPC10_H */
