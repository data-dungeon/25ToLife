#ifndef MACOSX
#include <malloc.h>
#endif
#include <stdlib.h>
#include "lpc10int.h"


/* Allocate memory for, and initialize, the state that needs to be
   kept from encoding one frame to the next for a single
   LPC-10-compressed audio stream.  Return 0 if malloc fails,
   otherwise return pointer to new structure. */

struct lpc10_encoder_state *create_lpc10_encoder_state(void)
{
    struct lpc10_encoder_state *st;

    st = (struct lpc10_encoder_state *)malloc((unsigned) sizeof (struct lpc10_encoder_state));
    return (st);
}



void init_lpc10_encoder_state(struct lpc10_encoder_state *st)
{
    int i;

    /* State used only by function hp100 */
    st->z11 = 0.0f;
    st->z21 = 0.0f;
    st->z12 = 0.0f;
    st->z22 = 0.0f;
    
    /* State used by function analys */
    for (i = 0; i < 540; i++) {
	st->inbuf[i] = 0.0f;
	st->pebuf[i] = 0.0f;
    }
    for (i = 0; i < 696; i++) {
	st->lpbuf[i] = 0.0f;
    }
    for (i = 0; i < 312; i++) {
	st->ivbuf[i] = 0.0f;
    }
    st->bias = 0.0f;
    st->osptr = 1;
    for (i = 0; i < 3; i++) {
	st->obound[i] = 0;
    }
    st->vwin[4] = 307;
    st->vwin[5] = 462;
    st->awin[4] = 307;
    st->awin[5] = 462;
    for (i = 0; i < 8; i++) {
	st->voibuf[i] = 0;
    }
    for (i = 0; i < 3; i++) {
	st->rmsbuf[i] = 0.0f;
    }
    for (i = 0; i < 30; i++) {
	st->rcbuf[i] = 0.0f;
    }
    st->zpre = 0.0f;


    /* State used by function onset */
    st->n = 0.0f;
    st->d = 1.0f;
    for (i = 0; i < 16; i++) {
	st->l2buf[i] = 0.0f;
    }
    st->l2sum1 = 0.0f;
    st->l2ptr1 = 1;
    st->l2ptr2 = 9;
    st->hyst = FALSE;

    /* State used by function voicin */
    st->dither = 20.0f;
    st->maxmin = 0.0f;
    for (i = 0; i < 6; i++) {
	st->voice[i] = 0.0f;
    }
    st->lbve = 3000;
    st->fbve = 3000;
    st->fbue = 187;
    st->ofbue = 187;
    st->sfbue = 187;
    st->lbue = 93;
    st->olbue = 93;
    st->slbue = 93;
    st->snr = (float) (st->fbve / st->fbue << 6);

    /* State used by function dyptrk */
    for (i = 0; i < 60; i++) {
	st->s[i] = 0.0f;
    }
    for (i = 0; i < 120; i++) {
	st->p[i] = 0;
    }
    st->ipoint = 0;
    st->alphax = 0.0f;

    /* State used by function chanwr */
    st->isync = 0;

    /* added for lower rates */
    st->lframe = 180;

    /* misc */
    st->order = 10;
}

/* free the memory */
void destroy_lpc10_encoder_state (struct lpc10_encoder_state *st)
{
    if(st != NULL)
    {
        free(st);
        st = NULL;
    }
}

/* Allocate memory for, and initialize, the state that needs to be
   kept from decoding one frame to the next for a single
   LPC-10-compressed audio stream.  Return 0 if malloc fails,
   otherwise return pointer to new structure. */

struct lpc10_decoder_state *create_lpc10_decoder_state(void)
{
    struct lpc10_decoder_state *st;

    st = (struct lpc10_decoder_state *)malloc((unsigned) sizeof (struct lpc10_decoder_state));
    return (st);
}



void init_lpc10_decoder_state(struct lpc10_decoder_state *st)
{
    int i;

    /* State used by function decode */
    st->iptold = 60;
    st->first = TRUE;
    st->ivp2h = 0;
    st->iovoic = 0;
    st->iavgp = 60;
    st->erate = 0;
    for (i = 0; i < 30; i++) {
	st->drc[i] = 0;
    }
    for (i = 0; i < 3; i++) {
	st->dpit[i] = 0;
	st->drms[i] = 0;
    }

    /* State used by function synths */
    for (i = 0; i < 360; i++) {
	st->buf[i] = 0.0f;
    }
    st->buflen = 180;

    /* State used by function pitsyn */
    st->rmso = 1.0f;
    st->first_pitsyn = TRUE;

    /* State used by function bsynz */
    st->ipo = 0;
    for (i = 0; i < 166; i++) {
	st->exc[i] = 0.0f;
	st->exc2[i] = 0.0f;
    }
    st->lpi1 = 0.0f;
    st->lpi2 = 0.0f;
    st->lpi3 = 0.0f;
    st->hpi1 = 0.0f;
    st->hpi2 = 0.0f;
    st->hpi3 = 0.0f;
    st->rmso_bsynz = 0.0f;

    /* State used by function deemp */
    st->dei1 = 0.0f;
    st->dei2 = 0.0f;
    st->deo1 = 0.0f;
    st->deo2 = 0.0f;
    st->deo3 = 0.0f;

    /* added for lower rates */
    st->lframe = 180;

    /* misc */
    st->order = 10;
}

void destroy_lpc10_decoder_state (struct lpc10_decoder_state *st)
{
    if(st != NULL)
    {
        free(st);
        st = NULL;
    }
}
