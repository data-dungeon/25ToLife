/*

*/
#define CODEC_LPC10
#define CODEC_ULAW
#define CODEC_G721
#define CODEC_G723_24
#define CODEC_GSM
//#define CODEC_SPEEX

#include "utils.h"
#include "lgpriv.h"

// codecs
#ifdef CODEC_LPC10
#include "lglpc10.h"
#endif
#ifdef CODEC_ULAW
#include "lgulaw.h"
#endif
#ifdef CODEC_GSM
#include "lggsm.h"
#endif
#ifdef CODEC_G723_24
#include "lgG723.h"
#endif
#ifdef CODEC_G721
#include "lgG721.h"
#endif
#ifdef CODEC_VAG
#include "lgvag.h"
#endif
#ifdef CODEC_ILBC
#include "lgilbc.h"
#endif
#ifdef CODEC_SPEEX
#include "lgSpeex.h"
#endif


typedef enum lgCodecID
{
    LGCODEC_AUDIO_PCM       = 10,
    LGCODEC_AUDIO_ULAW      = 100,
    LGCODEC_AUDIO_LPC10     = 200,
    LGCODEC_AUDIO_GSM       = 300,
    LGCODEC_AUDIO_G723_24   = 400,
    LGCODEC_AUDIO_G721      = 500,
    LGCODEC_AUDIO_VAG       = 600,
    LGCODEC_AUDIO_ILBC      = 700,
    LGCODEC_AUDIO_SPEEX8    = 800,
    LGCODEC_AUDIO_SPEEX16   = 801,
    LGCODEC_AUDIO_SPEEX32   = 802,
    LGCODEC_LAST = 0x7fffffff
} lgCodecID;


//************************************************************************
// Codec
//************************************************************************

lgCodecDesc g_codecs[] = {
    // codec id             name      bps       bytes/pcm frame      bytes/enc frame
    { LGCODEC_AUDIO_PCM,    "PCM",    128000,   320,                 320 },     
#ifdef CODEC_ULAW
    { LGCODEC_AUDIO_ULAW,   "uLaw",   64000,    320,                 160 },     
#endif
#ifdef CODEC_G723_24
    { LGCODEC_AUDIO_G723_24,"G723.24",24000,    320,                 60  },    
#endif
#ifdef CODEC_GSM
    { LGCODEC_AUDIO_GSM,    "GSM",    13000,    320,                 33 },    
#endif
#ifdef CODEC_ILBC
    { LGCODEC_AUDIO_ILBC,   "iLBC",   13000,    480,                 50 },    
#endif
#ifdef CODEC_G721
    { LGCODEC_AUDIO_G721,   "G721",   32000,    320,                 80 },
#endif
#ifdef CODEC_LPC10
    { LGCODEC_AUDIO_LPC10,  "LPC10",  2400,     360,                 7  },    
#endif
#ifdef CODEC_VAG
    { LGCODEC_AUDIO_VAG,    "VAG",    32000,    320,                 80 },    
#endif
#ifdef CODEC_SPEEX
    { LGCODEC_AUDIO_SPEEX8, "SPEEX",   6400,    320,                 20 },
#endif
};

//************************************************************************
// lgAudCodecInit
//************************************************************************

int lgCodecInit(void)
{
 
    //printf("Initializing codec\n");

    return LGCODEC_SUCCESS;
}




//************************************************************************
// lgCodecEnumerate
//************************************************************************

lgCodecDesc* lgCodecEnumerate(int index)
{
    int codec_count = sizeof(g_codecs)/sizeof(lgCodecDesc);
    if (index < 0 || index > (codec_count-1))
        return NULL;
    else
        return &g_codecs[index];
}

//************************************************************************
// lgCodecOpen(int codec)
//************************************************************************

int lgCodecOpen(int codec_id, int* codec)
{
    
    int codec_count = sizeof(g_codecs)/sizeof(lgCodecDesc);
    int i;
    int codec_index= -1;

    for (i = 0; i < codec_count; i++)
    {
        if (g_codecs[i].id == codec_id)
        {
            codec_index = i;
            break;
        }
    }

    if (codec_index == -1)
        return LGCODEC_ERR_INVALID_PARAMETER;

    switch(codec_id)
    {
#ifdef CODEC_LPC10
    case LGCODEC_AUDIO_LPC10:
    {
        LPC10* lpc10 = malloc(sizeof(LPC10));
        memset(lpc10, 0, sizeof(LPC10));
        lpc10->encode_state = malloc(sizeof(struct lpc10_encoder_state));
        lpc10->decode_state = malloc(sizeof(struct lpc10_decoder_state));
        init_lpc10_encoder_state(lpc10->encode_state);
        init_lpc10_decoder_state(lpc10->decode_state);
        lpc10->header.desc = g_codecs[codec_index];
        *codec = (int)lpc10;
        break;
    }
#endif
#ifdef CODEC_GSM
    case LGCODEC_AUDIO_GSM:
    {
        GSM* pcodec = malloc(sizeof(GSM));
        int gsm_ltp = 1;

        memset(pcodec, 0, sizeof(GSM));
        pcodec->encode_state = gsm_create();
        pcodec->decode_state = gsm_create();
        gsm_option(pcodec->encode_state, GSM_OPT_LTP_CUT, &gsm_ltp);
        pcodec->header.desc = g_codecs[codec_index];
        *codec = (int)pcodec;
        break;
    }
#endif
#ifdef CODEC_ULAW
    case LGCODEC_AUDIO_PCM:
    case LGCODEC_AUDIO_ULAW:
    {
        // there is no encoding state for ulaw
        lgCodecHeader* pcodec = malloc(sizeof(lgCodecHeader));
        pcodec->desc = g_codecs[codec_index];
        *codec = (int)pcodec;
        break;
    }
#endif
#ifdef CODEC_G723_24
    case LGCODEC_AUDIO_G723_24:
    {
        G723_24* pcodec = malloc(sizeof(G723_24));
        memset(pcodec, 0, sizeof(G723_24));
        pcodec->encode_state = malloc(sizeof(struct g72x_state));
        pcodec->decode_state = malloc(sizeof(struct g72x_state));
        g72x_init_state(pcodec->encode_state);
        g72x_init_state(pcodec->decode_state);
        pcodec->header.desc = g_codecs[codec_index];
        *codec = (int)pcodec;
        break;
    }
#endif
#ifdef CODEC_G721
    case LGCODEC_AUDIO_G721:
    {
        G721* pcodec = malloc(sizeof(G721));
        memset(pcodec, 0, sizeof(G721));
        pcodec->encode_state = malloc(sizeof(struct g72x_state));
        pcodec->decode_state = malloc(sizeof(struct g72x_state));
        g72x_init_state(pcodec->encode_state);
        g72x_init_state(pcodec->decode_state);
        pcodec->header.desc = g_codecs[codec_index];
        *codec = (int)pcodec;
        break;
    }
#endif
#ifdef CODEC_VAG
    case LGCODEC_AUDIO_VAG:
    {
        VAG* pcodec = malloc(sizeof(VAG));
        memset(pcodec, 0, sizeof(VAG));
        pcodec->header.desc = g_codecs[codec_index];
        *codec = (int)pcodec;
        break;
    }
#endif
#ifdef CODEC_ILBC
    case LGCODEC_AUDIO_ILBC:
    {
        ILBC* pcodec = malloc(sizeof(ILBC));
        memset(pcodec, 0, sizeof(ILBC));
        pcodec->encode_state = malloc(sizeof(iLBC_Enc_Inst_t));
        pcodec->decode_state = malloc(sizeof(iLBC_Dec_Inst_t));
        memset(pcodec->encode_state, 0, sizeof(iLBC_Enc_Inst_t));
        memset(pcodec->decode_state, 0, sizeof(iLBC_Dec_Inst_t));
        initEncode(pcodec->encode_state); 
        initDecode(pcodec->decode_state, 1); 
        pcodec->header.desc = g_codecs[codec_index];
        *codec = (int)pcodec;
        break;
    }
#endif
#ifdef CODEC_SPEEX
    case LGCODEC_AUDIO_SPEEX8:
    {
        SPEEX8* pcodec = malloc(sizeof(SPEEX8));
        int val;

        memset(pcodec, 0, sizeof(SPEEX8));

        // codec will allocate enc/dec states
        pcodec->encode_state = speex_encoder_init(&speex_nb_mode);
        pcodec->decode_state = speex_decoder_init(&speex_nb_mode);
        pcodec->header.desc = g_codecs[codec_index];

        // codec will allocate a bit array
        speex_bits_init(&pcodec->speex_bits);

        *codec = (int)pcodec;

        // set some control values
        val=0;
        speex_decoder_ctl(pcodec->decode_state, SPEEX_SET_ENH, &val);
        val=0;
        speex_encoder_ctl(pcodec->encode_state, SPEEX_SET_VBR, &val);
        val=4;
        speex_encoder_ctl(pcodec->encode_state, SPEEX_SET_QUALITY, &val);
        val=1;
        speex_encoder_ctl(pcodec->encode_state, SPEEX_SET_COMPLEXITY, &val);

        break;
    }
#endif
    default:
        break;
    }
        
    return LGCODEC_SUCCESS;
}

//************************************************************************
// lgCodecClose(int codec)
//************************************************************************

int lgCodecClose(int codec)
{

    lgCodecHeader* pheader = (lgCodecHeader*)codec;

    switch(pheader->desc.id)
    {
#ifdef CODEC_LPC10
    case LGCODEC_AUDIO_LPC10:
    {
        LPC10* pcodec = (LPC10*)pheader;
        free(pcodec->encode_state);
        free(pcodec->decode_state);
        free(pcodec);
        break;
    }
#endif
#ifdef CODEC_GSM
    case LGCODEC_AUDIO_GSM:
    {
        GSM* pcodec = (GSM*)pheader;
        gsm_destroy(pcodec->encode_state);
        gsm_destroy(pcodec->decode_state);
        free(pcodec);
        break;
    }
#endif
#ifdef CODEC_ULAW
    case LGCODEC_AUDIO_PCM:
    case LGCODEC_AUDIO_ULAW:
    {
        lgCodecHeader* pcodec = pheader;
        free(pcodec);
        break;
    }
#endif
#ifdef CODEC_G723_24
    case LGCODEC_AUDIO_G723_24:
    {
        G723_24* pcodec = (G723_24*)pheader;
        free(pcodec->encode_state);
        free(pcodec->decode_state);
        free(pcodec);
        break;
    }
#endif
#ifdef CODEC_G721
    case LGCODEC_AUDIO_G721:
    {
        G721* pcodec = (G721*)pheader;
        free(pcodec->encode_state);
        free(pcodec->decode_state);
        free(pcodec);
        break;
    }
#endif
#ifdef CODEC_VAG
    case LGCODEC_AUDIO_VAG:
    {
        VAG* pcodec = (VAG*)pheader;
        free(pcodec);
        break;
    }
#endif
#ifdef CODEC_ILBC
    case LGCODEC_AUDIO_ILBC:
    {
        ILBC* pcodec = (ILBC*)pheader;
        free(pcodec);
        break;
    }
#endif
#ifdef CODEC_SPEEX
    case LGCODEC_AUDIO_SPEEX8:
    {
        SPEEX8* pcodec = (SPEEX8*)pheader;
        speex_bits_destroy(&pcodec->speex_bits);
        speex_encoder_destroy(pcodec->encode_state);
        speex_decoder_destroy(pcodec->decode_state);
        free(pcodec);
        break;
    }
#endif
    default:
        break;
    }
        
    return LGCODEC_SUCCESS;
}


//************************************************************************
// lgCodecEncode
//************************************************************************

int lgCodecEncode(int codec, const short* src, int src_size, u_char* dest, int* dest_size)
{
    lgCodecHeader * pheader = (lgCodecHeader*)codec;
    
    if (pheader == NULL)
        return LGCODEC_ERR_INVALID_PARAMETER;
    
    switch(pheader->desc.id)
    {
#ifdef CODEC_LPC10
    case LGCODEC_AUDIO_LPC10:
    {
        return lgCodecLPC10Encode((LPC10*)pheader, src, src_size, dest, dest_size);
    }
#endif
#ifdef CODEC_ULAW
    case LGCODEC_AUDIO_ULAW:
    {
        return lgCodecUlawEncode(src, src_size, dest, dest_size);
    }
#endif
#ifdef CODEC_GSM
    case LGCODEC_AUDIO_GSM:
    {
        return lgCodecGSMEncode((GSM*)pheader, src, src_size, dest, dest_size);
    }
#endif    
#ifdef CODEC_G723_24
    case LGCODEC_AUDIO_G723_24:
    {
        return lgCodecG72324Encode((G723_24*)pheader, src, src_size, dest, dest_size);
    }
#endif    
#ifdef CODEC_G721
    case LGCODEC_AUDIO_G721:
    {
        return lgCodecG721Encode((G721*)pheader, src, src_size, dest, dest_size);
    }
#endif    
#ifdef CODEC_VAG
    case LGCODEC_AUDIO_VAG:
    {
        return lgCodecVAGEncode((VAG*)pheader, src, src_size, dest, dest_size);
    }
#endif    
#ifdef CODEC_ILBC
    case LGCODEC_AUDIO_ILBC:
    {
        return lgCodecILBCEncode((ILBC*)pheader, src, src_size, dest, dest_size);
    }
#endif    
#ifdef CODEC_SPEEX
    case LGCODEC_AUDIO_SPEEX8:
    {
        return lgCodecSpeex8Encode((SPEEX8*)pheader, src, src_size, dest, dest_size);
    }
#endif    
    case LGCODEC_AUDIO_PCM:
    {
        if (dest == NULL || src == NULL)
            return LGCODEC_ERR_INVALID_PARAMETER;
        else
        {
            unsigned int to_copy = min(src_size, *dest_size);
            memcpy(dest, src, to_copy);
            *dest_size = to_copy;
            return LGCODEC_SUCCESS;
        }
    }
    default:
        break;
    }

    //return lgAudCodecLPC10Encode(dest, size);
    return LGCODEC_ERROR;
}

//************************************************************************
// lgCodecDecode
//************************************************************************

int lgCodecDecode(int codec, const u_char* src, int src_size, short* dest, int* dest_size)
{

    lgCodecHeader * pheader = (lgCodecHeader*)codec;
    
    if (pheader == NULL)
        return LGCODEC_ERR_INVALID_PARAMETER;
    
    switch(pheader->desc.id)
    {
#ifdef CODEC_LPC10
    case LGCODEC_AUDIO_LPC10:
    {
        return lgCodecLPC10Decode( (LPC10*)pheader, src, src_size, dest, dest_size);
    }
#endif
#ifdef CODEC_ULAW
    case LGCODEC_AUDIO_ULAW:
    {
        return lgCodecUlawDecode(src, src_size, dest, dest_size);
    }
#endif
#ifdef CODEC_GSM
    case LGCODEC_AUDIO_GSM:
    {
        return lgCodecGSMDecode((GSM*)pheader, src, src_size, dest, dest_size);
    }
#endif    
#ifdef CODEC_G723_24
    case LGCODEC_AUDIO_G723_24:
    {
        return lgCodecG72324Decode((G723_24*)pheader, src, src_size, dest, dest_size);
    }
#endif    
#ifdef CODEC_G721
    case LGCODEC_AUDIO_G721:
    {
        return lgCodecG721Decode((G721*)pheader, src, src_size, dest, dest_size);
    }
#endif
#ifdef CODEC_VAG
    case LGCODEC_AUDIO_VAG:
    {
        return lgCodecVAGDecode((VAG*)pheader, src, src_size, dest, dest_size);
    }
#endif
#ifdef CODEC_ILBC
    case LGCODEC_AUDIO_ILBC:
    {
        return lgCodecILBCDecode((ILBC*)pheader, src, src_size, dest, dest_size);
    }
#endif
#ifdef CODEC_SPEEX
    case LGCODEC_AUDIO_SPEEX8:
    {
        return lgCodecSpeex8Decode((SPEEX8*)pheader, src, src_size, dest, dest_size);
    }
#endif
    case LGCODEC_AUDIO_PCM:
    {
        if (dest == NULL || src == NULL)
            return LGCODEC_ERR_INVALID_PARAMETER;
        else
        {
            unsigned int to_copy = min(src_size, *dest_size);
            memcpy(dest, src, to_copy);
            *dest_size = to_copy;
            return LGCODEC_SUCCESS;
        }
    }
    default:
        break;
    }

    return LGCODEC_ERROR;
}

// encodes a frame of data
int lgFrameEncode(const short* src, const int src_size, u_char* dest, int* dest_size,
                  short* encode_in, const int encode_in_size, int* encode_in_samples, 
                  const int samples_per_frame, const int bytes_per_enc_frame, 
                  PFN_FRAME_ENCODE pfn_enc, void* enc_param)
{

    const short* next_sample_frame = src;
    u_char* next_enc_frame = dest;

    int consume_samples = 0;
    int samples_to_encode = 0;
    int max_frames_to_decode = 0;
    int bytes_encoded = 0;

    if (src == NULL || dest == NULL)
    {
        *dest_size = 0;
        return LGCODEC_ERR_INVALID_PARAMETER;
    }
    if (src_size % sizeof(short) != 0)
    {
        *dest_size = 0;
        return LGCODEC_ERR_INVALID_PARAMETER;
    }
    if (src_size == 0 || *dest_size == 0)
    {
        *dest_size = 0;
        return LGCODEC_SUCCESS;
    }

    // quick check to make sure destination has enough space
    // todo: just handle frame by frame
    max_frames_to_decode = (src_size/2)/samples_per_frame;
    
    // if there is residue in the encode_in buffer AND the dest_size is
    // not a multiple of the bytes_per_enc_frame the caller is probably
    // using an arbitrary buffer. be safe and require an extra frame
    if (*encode_in_samples && (*dest_size % bytes_per_enc_frame))
        max_frames_to_decode++;
    
    if (*dest_size < (max_frames_to_decode*bytes_per_enc_frame))
    {
        printf("ERROR: bytes provided = %d. Need=%d\n", *dest_size, max_frames_to_decode*bytes_per_enc_frame);
        *dest_size = 0;
        return LGCODEC_ERR_INVALID_PARAMETER;
    }

    samples_to_encode = (src_size/2);

    // clear output fields
    memset(dest, 0, (unsigned int)*dest_size);        
    *dest_size = 0;

    while (1)
    {
        // fill up the buffer (there may be samples already in the encoding buffer)
        consume_samples = min(samples_per_frame - *encode_in_samples, samples_to_encode);
        memcpy(encode_in + *encode_in_samples, next_sample_frame, (unsigned int)consume_samples*2);
        *encode_in_samples += consume_samples;
        next_sample_frame += consume_samples;
        
        // encode it
        if (*encode_in_samples == samples_per_frame)
        {
            bytes_encoded += pfn_enc(encode_in, next_enc_frame, enc_param);

            samples_to_encode -= consume_samples;
            *encode_in_samples = 0;
            memset(encode_in, 0, (unsigned int)encode_in_size);
            next_enc_frame += bytes_per_enc_frame;

        }
        else
        {
            // copy whatever is in the output buffer to the destination
            *dest_size = bytes_encoded;
            break;            
        }

    }

    return LGCODEC_SUCCESS;
}

int lgFrameDecode(const u_char* src, const int src_size, short* dest, int* dest_size,
                  unsigned char* decode_in, const int decode_in_size, int* decode_in_bytes, 
                  const int samples_per_frame, const int bytes_per_enc_frame, 
                  PFN_FRAME_DECODE pfn_enc, void* dec_param)
{

    // decode tne input before we sent it to the headset
    int frames_to_decode = 0;
    const u_char* next_enc_frame = src;
    short* next_sample_frame = dest;
    int consume_bytes = 0;
    int samples_decoded = 0;
    int bytes_to_encode = 0;
    int dest_bytes_left = 0;
        
    // check for weirdness
    if (src == NULL || dest == NULL)
    {
        *dest_size = 0;
        return LGCODEC_ERR_INVALID_PARAMETER;
    }
    if (*dest_size % sizeof(short) != 0)
    {
        *dest_size = 0;
        return LGCODEC_ERR_INVALID_PARAMETER;
    }
    if (src_size == 0 || *dest_size == 0)
    {
        *dest_size = 0;
        return LGCODEC_SUCCESS;
    }


    bytes_to_encode = src_size;
    dest_bytes_left = *dest_size;

    // we must have at least bytes_per_enc_frame bytes
    frames_to_decode = src_size / bytes_per_enc_frame;
    if (frames_to_decode <= 0)
    {
        *dest_size = 0;
        return LGCODEC_SUCCESS;
    }
/*
    // check if user has provided enough destination buffer
    if (*dest_size < (frames_to_decode * samples_per_frame * 2))
    {
        printf("provided %d bytes. need %d bytes\n", *dest_size, (frames_to_decode * LPC10_SAMPLES_PER_FRAME * 2));
        *dest_size = 0;
        return LGCODEC_ERR_INVALID_PARAMETER;
    }
*/

    frames_to_decode = min(*dest_size/(samples_per_frame*2), frames_to_decode);
    bytes_to_encode = min(frames_to_decode * samples_per_frame * 2, bytes_to_encode);

    // clear output fields
    memset(dest, 0, (unsigned int)*dest_size);        
    *dest_size = 0;

    // decode the data
    while (1)
    {

        // fill up the buffer (there may be samples already in the decoding buffer)
        consume_bytes = min(bytes_per_enc_frame - *decode_in_bytes, bytes_to_encode);
        memcpy(decode_in + *decode_in_bytes, next_enc_frame, (unsigned int)consume_bytes);
        next_enc_frame += consume_bytes;
        *decode_in_bytes += consume_bytes;

        if (*decode_in_bytes == bytes_per_enc_frame &&
            ( dest_bytes_left >= ( int )(samples_per_frame*sizeof(short))))
        {
            pfn_enc(decode_in, next_sample_frame, dec_param);

            samples_decoded += samples_per_frame;
            bytes_to_encode -= bytes_per_enc_frame;
            next_sample_frame += samples_per_frame;
            *decode_in_bytes = 0;
            dest_bytes_left -= (samples_decoded * sizeof(short));
        }
        else
        {
            // mark the total bytes decoded
            *dest_size = (samples_decoded * 2);
            break;
        }
    }

    return LGCODEC_SUCCESS;
}

/* end of liblgaud.c */

