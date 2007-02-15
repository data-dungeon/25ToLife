/******************************************************************************
*
* Copyright (c) 2002 SCEA. All Rights Reserved.
*
* This software contains the valuable trade secrets of SCEA.  The
* software is protected under copyright laws as an unpublished work of
* SCEA.  Notice is for informational purposes only and does not imply
* publication.  The user of this software may make copies of the software
* for use with parts manufactured by SCEA or under license from
* SCEA and for no other use.
*
******************************************************************************/

/******************************************************************************
*
*  File Name:       nellymoser.c
*
*  Description:	    Wrapper of the Nellymoser audio codec. Libraries compiled
*                   using this file and Nellymoser libraries shall be strictly
*                   for SCEA internal use only, until licensing issue with the
*                   Nellymoser is resolved.  -A.Mai 12/24/2002
*
*
*  Programmers:     Anthony Mai (am) anthony_mai@playstation.sony.com
*
*  History:         12/24/2002 Initial creation
*
*  Notes:           This file uses 4 spaces indents
*
******************************************************************************/
#include "Voice/VoicePCH.h"

#include <stdlib.h>
#include <stdio.h>

#include "nellymoser_wrapper.h"
#include "nci.h"
#include "ncierror.h"
#include "ncicodec.h"
#include "ncisase3200.h"
#include "ncivad.h"
#include "ncivoicefonts.h"
#include "ncivoiceeffects.h"
#include "Layers/Memory.h"
#include "nciExport.h"

//extern void	*memAlloc( unsigned int u32Size, unsigned int u32Align = 16 );

//extern bool	memFree(void *pAddr);

/* To replace the data from the microphone with pre-recorded data,
 * create an 8kHz 16-bit Mono PCM file (with no header) and place
 * it next to the .elf file.
 */

typedef struct NMS
{
	 void*					pEncoderMem;
    void*					pDecoderMem;
    void*               pEffectsMem;
	 NCI_CODEC_IO_BLOCK  IOBlockDecode;
    NCI_CODEC_IO_BLOCK  IOBlockEncode;
} NMS;

typedef struct NMS4
{
	void						*pEncoderMem;
	void						*pDecodeStandAlone;
	void						*pDecoderMem1;
	void						*pDecoderMem2;
	void						*pDecoderMem3;
	void						*pDecoderMem4;
	void						*pEffectsMem;
	NCI_CODEC_IO_BLOCK  IOBlockEncode;
	NCI_CODEC_IO_BLOCK  IOBlockDecodeStandAlone;
	NCI_CODEC_IO_BLOCK  IOBlockDecode1;
	NCI_CODEC_IO_BLOCK  IOBlockDecode2;
	NCI_CODEC_IO_BLOCK  IOBlockDecode3;
	NCI_CODEC_IO_BLOCK  IOBlockDecode4;
} NMS4;

void myfree(void *ptr)
{
	ASSERT_PTR( ptr );
	memFree( ptr );
}

void *mycalloc(size_t num, size_t size)
{
	int bytes = num * size;
	void* ptr = memAlloc( bytes );
	memset( ptr, 0, bytes );
	return ptr;
}

void *mymemalign(size_t boundary, size_t bytes)
{
	void* ptr = memAlloc( bytes, boundary );
	memset( ptr, 0, bytes );
	return ptr;
}

void nmsInit( void )
{
	nciEnableVu0Optimizations(nciVU0_SHARED_PROTECTED);
//	nciSetSPRMode(nciSPR_SHARED_PROTECTED);
//	nciSetDMAUse(nciDMA_ENABLED);
	nciSetSPRMode(nciSPR_SUSPEND);
	nciSetDMAUse(nciDMA_DISABLED);

	nciSetCallocFunction(mycalloc);
	nciSetFreeFunction(myfree);
	nciSetMemalignFunction(mymemalign); 
}

/******************************************************************************
* Function:		nmsCreate4
*
* Description:	Create a NellyMoser codec object handle with 1 encoder and 4 decoders.
*
* Returns:      Handle to the Nellymoser codec object.
******************************************************************************/
NMS4 *nmsCreate4( )
{
	NMS4*    pNMS = NULL;
	NCI_RETCODE retCode;
	NCI_CODEC_INFO_BLOCK infoBlock;

	pNMS = (NMS4 *) memAlloc( sizeof( NMS4 ), 16 );
	if( pNMS == NULL ) 
	{
		ASSERT( pNMS );
		return NULL;
	}
	memset( pNMS, 0, sizeof( NMS4 ) );

	retCode = nciGetInfoSase3200( &infoBlock );
	if (NCI_NO_ERROR != retCode) 
	{
		ASSERT( false );
		memFree( pNMS );
		return NULL;
	}
	printf("%s\n", infoBlock.sCodecVersion);

	// --- init stand alone decoder and 4x decoders
	retCode = nciInitDecodeSase3200( &( pNMS->pDecodeStandAlone ) );
	retCode |= nciInitQuadrupleDecodeSase3200( &( pNMS->pDecoderMem1 ), &( pNMS->pDecoderMem2 ), &( pNMS->pDecoderMem3 ), &( pNMS->pDecoderMem4 ) );
	if( NCI_NO_ERROR != retCode ) 
	{
		ASSERT( false );
		nmsDestroy4( &pNMS );
		return NULL;
	}

	// --- init our single encoder
	retCode = nciInitEncodeSase3200( &( pNMS->pEncoderMem ) );
	if (NCI_NO_ERROR != retCode) 
	{
		ASSERT( false );
		nmsDestroy4( &pNMS );
		return NULL;
	}

	retCode = nciEnableVAD( pNMS->pEncoderMem );
	retCode |= nciSetVADSilenceDuration( pNMS->pEncoderMem, 1500 ); /* 1.5 sec window */
	retCode |= nciSetVADEnergyTrigLevel( pNMS->pEncoderMem, 50 );   /* low trigger    */
	//retCode |= nciEnableVoiceEffects( &( pNMS->pEffectsMem ) );
	if( NCI_NO_ERROR != retCode ) 
	{
		ASSERT( false );
		nmsDestroy4( &pNMS );
		return NULL;
	}
	return pNMS;
}

void nmsDestroy4( HNMS4 *hNMS )
{
	NMS4 * pNMS;
	if( NULL == hNMS )
	{
		return;
	}

	pNMS = *hNMS;
	if( pNMS != NULL )
	{
		if( pNMS->pDecodeStandAlone )
			(void) nciFreeDecodeSase3200( &( pNMS->pDecodeStandAlone ) );
		if( pNMS->pDecoderMem1 && pNMS->pDecoderMem2 && pNMS->pDecoderMem3 && pNMS->pDecoderMem4 )
			(void) nciFreeQuadrupleDecodeSase3200( &( pNMS->pDecoderMem1 ), &( pNMS->pDecoderMem2 ), &( pNMS->pDecoderMem3 ), &( pNMS->pDecoderMem4 ) );
		if( pNMS->pEncoderMem )
			(void) nciDisableVAD( pNMS->pEncoderMem );
		if( pNMS->pEffectsMem )
			(void) nciDisableVoiceEffects( &( pNMS->pEffectsMem ) );
		if( pNMS->pEncoderMem )
			(void) nciFreeEncodeSase3200( &( pNMS->pEncoderMem ) );
		memFree( pNMS );
		*hNMS = NULL;
	}
}

/******************************************************************************
* Function:		nmsEncode
*
* Description:	This function is optomized to work for the Nellymosr Sase
*               codec.  The SASE codec has fixed input and output, and the
*               codec frame size is naturally byte aligned.
*
*               WARNING - this function assumes the caller passes it a
*               buffer (pIn) pointing to at lease 1 frame of audio (320 bytes),
*               and  a buffer (pOut) large enough to store 1 compressed frame.
*
* Returns:      Encoded data in bytes. (8 bytes)
*               Or 0 in event of an error.
******************************************************************************/
int nmsEncode
(
    HNMS4 hNMS,
    const short*        pIn,
    unsigned char*      pOut,
    short               *pVoiced
)
{
    NCI_RETCODE     retCode;

    memset (&(hNMS->IOBlockEncode), 0, sizeof(NCI_CODEC_IO_BLOCK));

    hNMS->IOBlockEncode.pPCMBuffer = (short *) pIn;
    hNMS->IOBlockEncode.pNellyBuffer = pOut;
    hNMS->IOBlockEncode.wNellyBufferSize = NMS_FRAME_SIZE + 1;


    /* Encode 1 frame */
    retCode = nciEncodeSase3200(hNMS->pEncoderMem, &(hNMS->IOBlockEncode));
    if (NCI_NO_ERROR != retCode) {
        printf("nciEncodeSase3200() error %d\n", retCode);
        return 0;
    }

#if 1
	 /* Did frame contain speech? */
    retCode = nciGetVADVoiceActivity(hNMS->pEncoderMem, (unsigned short *)pVoiced);
    if (NCI_NO_ERROR != retCode) {
        printf("nciGetVADVoiceActivity() error %d\n", retCode);
    }
#endif
    return NMS_FRAME_SIZE;
}

/******************************************************************************
* Function:		nmsDecode
*
* Description:	Call the Nellymoser Sase 3200 codec to decode EXACTLY one frame
*               Consumes 8 bytes input, and produces 320 bytes output.
*               Audio produced is 16-bit Mono PCM
*
* Returns:      0 for no error, or Nellymoser error code from nciErrors.h
******************************************************************************/
int nmsDecode4x(	HNMS4 hNMS1,
						const unsigned char* pIn1,
						const unsigned char* pIn2,
						const unsigned char* pIn3,
						const unsigned char* pIn4,
						unsigned int          *nBytesConsumed,
						short*                pOut,
						unsigned int          *nBytesProduced )
{
	int             nSamplesDecoded = 0;
	unsigned char   wNeedsInputFlag;
	NCI_RETCODE     retCode;

	/* Caller must not pass us NULL for these pointers! */
	//    assert(nBytesConsumed);
	//    assert(nBytesProduced);

	*nBytesConsumed = 0;
	*nBytesProduced = 0;

	memset (&(hNMS1->IOBlockDecode1), 0, sizeof(NCI_CODEC_IO_BLOCK));
	memset (&(hNMS1->IOBlockDecode2), 0, sizeof(NCI_CODEC_IO_BLOCK));
	memset (&(hNMS1->IOBlockDecode3), 0, sizeof(NCI_CODEC_IO_BLOCK));
	memset (&(hNMS1->IOBlockDecode4), 0, sizeof(NCI_CODEC_IO_BLOCK));

	hNMS1->IOBlockDecode1.pNellyBuffer = (unsigned char *) pIn1;
	hNMS1->IOBlockDecode1.pPCMBuffer = pOut;
	hNMS1->IOBlockDecode1.wNellyBufferSize = NMS_FRAME_SIZE + 1;
	hNMS1->IOBlockDecode2.pNellyBuffer = (unsigned char *) pIn2;
	hNMS1->IOBlockDecode2.pPCMBuffer = pOut;
	hNMS1->IOBlockDecode2.wNellyBufferSize = NMS_FRAME_SIZE + 1;
	hNMS1->IOBlockDecode3.pNellyBuffer = (unsigned char *) pIn3;
	hNMS1->IOBlockDecode3.pPCMBuffer = pOut;
	hNMS1->IOBlockDecode3.wNellyBufferSize = NMS_FRAME_SIZE + 1;
	hNMS1->IOBlockDecode4.pNellyBuffer = (unsigned char *) pIn4;
	hNMS1->IOBlockDecode4.pPCMBuffer = pOut;
	hNMS1->IOBlockDecode4.wNellyBufferSize = NMS_FRAME_SIZE + 1;

	/* The decoder consumes the encoded frame and produces 80 or 160 samples */
	/* of PCM.  If it produces 80 samples, it can [must] be called again     */
	/* without any additional input, and it will produce the remaining 80    */
	/* samples.                                                              */
	do {
			retCode = nciSetQuadrupleDecodeGainSase3200( 
													hNMS1->pDecoderMem1, hNMS1->pDecoderMem2, hNMS1->pDecoderMem3, hNMS1->pDecoderMem4,
													1.0f, 1.0f, 1.0f, 1.0f );
			if (NCI_NO_ERROR != retCode) {
				printf("nciQuadrupleDecodeGainSase3200() error %d\n", retCode);
				return retCode;
			}
			retCode = 0;
			retCode += nciQuadrupleDecodeSase3200( hNMS1->pDecoderMem1, hNMS1->pDecoderMem2, hNMS1->pDecoderMem3, hNMS1->pDecoderMem4, 
																&(hNMS1->IOBlockDecode1), &(hNMS1->IOBlockDecode2), &(hNMS1->IOBlockDecode3), &(hNMS1->IOBlockDecode4) );

		if (NCI_NO_ERROR != retCode) {
			printf("nciDecodeSase3200() error %d\n", retCode);
			return retCode;
		}
		else if(hNMS1->IOBlockDecode1.wSamplesInPCMBuffer > 0)
		{
			/* write next 80 samples after the previous 80 samples */
			hNMS1->IOBlockDecode1.pPCMBuffer += hNMS1->IOBlockDecode1.wSamplesInPCMBuffer;
			hNMS1->IOBlockDecode2.pPCMBuffer += hNMS1->IOBlockDecode2.wSamplesInPCMBuffer;
			hNMS1->IOBlockDecode3.pPCMBuffer += hNMS1->IOBlockDecode3.wSamplesInPCMBuffer;
			hNMS1->IOBlockDecode4.pPCMBuffer += hNMS1->IOBlockDecode4.wSamplesInPCMBuffer;
			nSamplesDecoded += hNMS1->IOBlockDecode1.wSamplesInPCMBuffer;
		}

		/* Keep calling decode until it needs more input to produce output */
		retCode = nciDecodeNeedsInputSase3200(hNMS1->pDecoderMem1, &wNeedsInputFlag);
		if (NCI_NO_ERROR != retCode) {
			printf("nciDecodeNeedsInputSase3200() error %d\n", retCode);
			return retCode;
		}
	} while (!wNeedsInputFlag);

	/* Apply the current VoiceEffect */
#if 0
	retCode = nciVoiceEffects(hNMS1->pEffectsMem, pOut, nSamplesDecoded);
	if (NCI_NO_ERROR != retCode) {
		printf("nciVoiceEffects() error %d\n", retCode);
		return retCode;
	}
#endif

	*nBytesProduced = nSamplesDecoded * 2;
	*nBytesConsumed = hNMS1->IOBlockDecode1.wNellyBufferReadByteOffset;

	/* Codec always consumes 64 bits and frames are passed in byte-aligned */
	//assert(0 == hNMS->IOBlockDecode.chNellyBufferReadBitOffset);

	return NCI_NO_ERROR;
}

/******************************************************************************
* Function:		nmsDecode
*
* Description:	Call the Nellymoser Sase 3200 codec to decode EXACTLY one frame
*               Consumes 8 bytes input, and produces 320 bytes output.
*               Audio produced is 16-bit Mono PCM
*
* Returns:      0 for no error, or Nellymoser error code from nciErrors.h
******************************************************************************/
int nmsDecode(HNMS4                 hNMS,
              const unsigned char*  pIn,
              unsigned int          *nBytesConsumed,
              short*                pOut,
              unsigned int          *nBytesProduced)
{
    int             nSamplesDecoded = 0;
    unsigned char   wNeedsInputFlag;
    NCI_RETCODE     retCode;

    /* Caller must not pass us NULL for these pointers! */
//    assert(nBytesConsumed);
//    assert(nBytesProduced);

    *nBytesConsumed = 0;
    *nBytesProduced = 0;

    memset (&(hNMS->IOBlockDecodeStandAlone), 0, sizeof(NCI_CODEC_IO_BLOCK));
    hNMS->IOBlockDecodeStandAlone.pNellyBuffer = (unsigned char *) pIn;
    hNMS->IOBlockDecodeStandAlone.pPCMBuffer = pOut;
    hNMS->IOBlockDecodeStandAlone.wNellyBufferSize = NMS_FRAME_SIZE + 1;

    /* The decoder consumes the encoded frame and produces 80 or 160 samples */
    /* of PCM.  If it produces 80 samples, it can [must] be called again     */
    /* without any additional input, and it will produce the remaining 80    */
    /* samples.                                                              */
    do {
        retCode = nciDecodeSase3200(hNMS->pDecodeStandAlone, &(hNMS->IOBlockDecodeStandAlone));
        if (NCI_NO_ERROR != retCode) {
            printf("nciDecodeSase3200() error %d\n", retCode);
            return retCode;
        }
        else if(hNMS->IOBlockDecodeStandAlone.wSamplesInPCMBuffer > 0)
        {
            /* write next 80 samples after the previous 80 samples */
        	hNMS->IOBlockDecodeStandAlone.pPCMBuffer += hNMS->IOBlockDecodeStandAlone.wSamplesInPCMBuffer;
        	nSamplesDecoded += hNMS->IOBlockDecodeStandAlone.wSamplesInPCMBuffer;
        }

        /* Keep calling decode until it needs more input to produce output */
        retCode = nciDecodeNeedsInputSase3200(hNMS->pDecodeStandAlone, &wNeedsInputFlag);
        if (NCI_NO_ERROR != retCode) {
            printf("nciDecodeNeedsInputSase3200() error %d\n", retCode);
            return retCode;
        }
    } while (!wNeedsInputFlag);

    /* Apply the current VoiceEffect */
#if 0
	 retCode = nciVoiceEffects(hNMS->pEffectsMem, pOut, nSamplesDecoded);
    if (NCI_NO_ERROR != retCode) {
        printf("nciVoiceEffects() error %d\n", retCode);
        return retCode;
    }
#endif
    *nBytesProduced = nSamplesDecoded * 2;
    *nBytesConsumed = hNMS->IOBlockDecodeStandAlone.wNellyBufferReadByteOffset;

    /* Codec always consumes 64 bits and frames are passed in byte-aligned */
    //assert(0 == hNMS->IOBlockDecode.chNellyBufferReadBitOffset);

    return NCI_NO_ERROR;
}



int nmsSetDecVoiceFont(HNMS  hNMS,
                       VFont* pVFont)
{
    NCI_RETCODE retCode;

    retCode = nciSetDecPitchChangeSase3200(hNMS->pDecoderMem,
                                           pVFont->fPitchWarp,
                                           pVFont->fPitchOffset);
    if (NCI_NO_ERROR != retCode) {
        printf("nciSetDecPitchChangeSase3200() error %d\n", retCode);
        return retCode;
    }

    retCode = nciSetDecSpectrumChangeSase3200(hNMS->pDecoderMem,
                                              pVFont->fSpectrumWarp,
                                              pVFont->fSpectrumOffset,
                                              pVFont->fSpectrumLedge);
    if (NCI_NO_ERROR != retCode) {
        printf("nciSetDecSpectrumChangeSase3200() error %d\n", retCode);
        return retCode;
    }

    retCode = nciSetDecWhisperSase3200(hNMS->pDecoderMem,
                                       pVFont->fWhisper);
    if (NCI_NO_ERROR != retCode) {
        printf("nciSetDecWhisperSase3200() error %d\n", retCode);
        return retCode;
    }

    retCode = nciSetDecRobotSase3200(hNMS->pDecoderMem,
                                     pVFont->fRobot,
                                     pVFont->fPitchOffset);
    if (NCI_NO_ERROR != retCode) {
        printf("nciSetDecRobotSase3200() error %d\n", retCode);
        return retCode;
    }

    retCode = nciSetVoiceEffect(hNMS->pEffectsMem,
                                pVFont->effect);
    if (NCI_NO_ERROR != retCode) {
        printf("nciSetVoiceEffect() error %d\n", retCode);
        return retCode;
    }

    return NCI_NO_ERROR;
}

int nmsSetVoiceEffect(	HNMS pNMS, float fPitchWarp, float fPitchOffset, float fSpectrumWarp,
								float fSpectrumOffset, float fSpectrumLedge, float fRobot, float fWhisper, int effect )
{
	return 0;

	int retCode;

	retCode = nciSetDecPitchWarpSase3200(pNMS->pDecoderMem, fPitchWarp);
	retCode += nciSetDecPitchOffsetSase3200(pNMS->pDecoderMem, fPitchOffset);
	retCode += nciSetDecSpectrumWarpSase3200(pNMS->pDecoderMem, fSpectrumWarp);
	retCode += nciSetDecSpectrumOffsetSase3200(pNMS->pDecoderMem, fSpectrumOffset);
	retCode += nciSetDecSpectrumLedgeSase3200(pNMS->pDecoderMem, fSpectrumLedge);
	retCode += nciSetDecRobotSase3200(pNMS->pDecoderMem, fRobot, fPitchOffset);
	retCode += nciSetDecWhisperSase3200(pNMS->pDecoderMem, fWhisper);
	retCode += nciSetVoiceEffect( pNMS->pEffectsMem, effect );
	if (NCI_NO_ERROR != retCode) {
		printf("nciSetVoiceEffect() error %d\n", retCode);
		return retCode;
	}

	return NCI_NO_ERROR;
}

int nmsSetVoiceEffect4(	HNMS4 pNMS, int streamIndex, float fPitchWarp, float fPitchOffset, float fSpectrumWarp,
							 float fSpectrumOffset, float fSpectrumLedge, float fRobot, float fWhisper, int effect )
{
	return 0;
	int retCode;

	// --- always set the stand alone decoder
	retCode = nciSetDecPitchWarpSase3200(pNMS->pDecodeStandAlone, fPitchWarp);
	retCode += nciSetDecPitchOffsetSase3200(pNMS->pDecodeStandAlone, fPitchOffset);
	retCode += nciSetDecSpectrumWarpSase3200(pNMS->pDecodeStandAlone, fSpectrumWarp);
	retCode += nciSetDecSpectrumOffsetSase3200(pNMS->pDecodeStandAlone, fSpectrumOffset);
	retCode += nciSetDecSpectrumLedgeSase3200(pNMS->pDecodeStandAlone, fSpectrumLedge);
	retCode += nciSetDecRobotSase3200(pNMS->pDecodeStandAlone, fRobot, fPitchOffset);
	retCode += nciSetDecWhisperSase3200(pNMS->pDecodeStandAlone, fWhisper);

	// --- now set the appropriate decoder
	switch( streamIndex )
	{
		case 0:
			retCode += nciSetDecPitchWarpSase3200(pNMS->pDecoderMem1, fPitchWarp);
			retCode += nciSetDecPitchOffsetSase3200(pNMS->pDecoderMem1, fPitchOffset);
			retCode += nciSetDecSpectrumWarpSase3200(pNMS->pDecoderMem1, fSpectrumWarp);
			retCode += nciSetDecSpectrumOffsetSase3200(pNMS->pDecoderMem1, fSpectrumOffset);
			retCode += nciSetDecSpectrumLedgeSase3200(pNMS->pDecoderMem1, fSpectrumLedge);
			retCode += nciSetDecRobotSase3200(pNMS->pDecoderMem1, fRobot, fPitchOffset);
			retCode += nciSetDecWhisperSase3200(pNMS->pDecoderMem1, fWhisper);
			break;

		case 1:
			retCode += nciSetDecPitchWarpSase3200(pNMS->pDecoderMem2, fPitchWarp);
			retCode += nciSetDecPitchOffsetSase3200(pNMS->pDecoderMem2, fPitchOffset);
			retCode += nciSetDecSpectrumWarpSase3200(pNMS->pDecoderMem2, fSpectrumWarp);
			retCode += nciSetDecSpectrumOffsetSase3200(pNMS->pDecoderMem2, fSpectrumOffset);
			retCode += nciSetDecSpectrumLedgeSase3200(pNMS->pDecoderMem2, fSpectrumLedge);
			retCode += nciSetDecRobotSase3200(pNMS->pDecoderMem2, fRobot, fPitchOffset);
			retCode += nciSetDecWhisperSase3200(pNMS->pDecoderMem2, fWhisper);
			break;

		case 2:
			retCode += nciSetDecPitchWarpSase3200(pNMS->pDecoderMem3, fPitchWarp);
			retCode += nciSetDecPitchOffsetSase3200(pNMS->pDecoderMem3, fPitchOffset);
			retCode += nciSetDecSpectrumWarpSase3200(pNMS->pDecoderMem3, fSpectrumWarp);
			retCode += nciSetDecSpectrumOffsetSase3200(pNMS->pDecoderMem3, fSpectrumOffset);
			retCode += nciSetDecSpectrumLedgeSase3200(pNMS->pDecoderMem3, fSpectrumLedge);
			retCode += nciSetDecRobotSase3200(pNMS->pDecoderMem3, fRobot, fPitchOffset);
			retCode += nciSetDecWhisperSase3200(pNMS->pDecoderMem3, fWhisper);
			break;

		case 3:
			retCode += nciSetDecPitchWarpSase3200(pNMS->pDecoderMem4, fPitchWarp);
			retCode += nciSetDecPitchOffsetSase3200(pNMS->pDecoderMem4, fPitchOffset);
			retCode += nciSetDecSpectrumWarpSase3200(pNMS->pDecoderMem4, fSpectrumWarp);
			retCode += nciSetDecSpectrumOffsetSase3200(pNMS->pDecoderMem4, fSpectrumOffset);
			retCode += nciSetDecSpectrumLedgeSase3200(pNMS->pDecoderMem4, fSpectrumLedge);
			retCode += nciSetDecRobotSase3200(pNMS->pDecoderMem4, fRobot, fPitchOffset);
			retCode += nciSetDecWhisperSase3200(pNMS->pDecoderMem4, fWhisper);
			break;

		default:
			ASSERT( false );

	}

	// --- do not allow effect change
//	retCode += nciSetVoiceEffect( pNMS->pEffectsMem, effect );
	retCode += nciSetVoiceEffect( pNMS->pEffectsMem, 0 );

	if (NCI_NO_ERROR != retCode) 
	{
		printf("nciSetVoiceEffect() error %d\n", retCode);
		return retCode;
	}

	return NCI_NO_ERROR;
}

int nmsSetEncVoiceFont(HNMS  hNMS,
                       VFont* pVFont)
{
    NCI_RETCODE retCode;

    retCode = nciSetEncPitchChangeSase3200(hNMS->pEncoderMem,
                                           pVFont->fPitchWarp,
                                           pVFont->fPitchOffset);
    if (NCI_NO_ERROR != retCode) {
        printf("nciSetEncPitchChangeSase3200() error %d\n", retCode);
        return retCode;
    }

    retCode = nciSetEncSpectrumChangeSase3200(hNMS->pEncoderMem,
                                              pVFont->fSpectrumWarp,
                                              pVFont->fSpectrumOffset,
                                              pVFont->fSpectrumLedge);
    if (NCI_NO_ERROR != retCode) {
        printf("nciSetEncSpectrumChangeSase3200() error %d\n", retCode);
        return retCode;
    }

    retCode = nciSetEncWhisperSase3200(hNMS->pEncoderMem,
                                       pVFont->fWhisper);
    if (NCI_NO_ERROR != retCode) {
        printf("nciSetEncWhisperSase3200() error %d\n", retCode);
        return retCode;
    }

    retCode = nciSetEncRobotSase3200(hNMS->pEncoderMem,
                                     pVFont->fRobot,
                                     pVFont->fPitchOffset);
    if (NCI_NO_ERROR != retCode) {
        printf("nciSetEncRobotSase3200() error %d\n", retCode);
        return retCode;
    }

    /* Currently, we only demonstrate Decode-Side Voice Effects
     * To apply a VoiceEffect to an Encode-Side font, you will
     * need to transmit the effect ID to the remote party and apply
     * that effect after the data is decoded...
     */
    return NCI_NO_ERROR;
}


/* Little helper function to keep VoiceFont values within range */
void nmsSetVFValue(float *pfValue, float fMin, float fMax, int direction)
{
    float fVal;
    float fStepVal = (fMax - fMin) / 500.0F;

    //assert(pfValue);
    fVal = *pfValue;

    fVal += (fStepVal * (float) direction);

    if (fVal > fMax)
    {
        fVal = fMax;
    }
    else if (fVal < fMin)
    {
        fVal = fMin;
    }

    *pfValue = fVal;
}
