#ifndef _NELLYMOSER_H_INCLUDED_12_24_2002_
#define _NELLYMOSER_H_INCLUDED_12_24_2002_


#define NMS_BLOCK_SAMPLES   160
#define NMS_BLOCK_SIZE      320
#define NMS_FRAME_SIZE      8

typedef struct NMS*		HNMS;
typedef struct NMS4*		HNMS4;

typedef struct VFont_t {
   float fPitchWarp;      /* ~0.5 to 2.5 */
   float fPitchOffset;    /* -75.0 to 300.0 */
   float fSpectrumWarp;   /* 0.5 to 2.5 */
   float fSpectrumOffset; /* -1000 to 1000 (although -4k to +4k is possible) */
   float fSpectrumLedge;  /* ledge (in Hz) */
   float fWhisper;        /* percentage of "whisper-ness". 1.0 is maximum */
   float fRobot;          /* percentage of "robot-ness". 1.0 is maximum */
   short effect;          /* 0 .. 12 */
} VFont, *PVFont;


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

void nmsInit( void );
HNMS4	nmsCreate4( );
void    nmsDestroy4(HNMS4 *hNMS );

int     nmsEncode(HNMS4                  hNMS,
                  const short*          pIn,
                  unsigned char*        pOut,
                  short                 *vadValue);

int     nmsDecode(HNMS4                  hNMS,
                  const unsigned char*  pIn,
                  unsigned int          *nBytesConsumed,
                  short*                pOut,
                  unsigned int          *nBytesProduced);


int nmsSetDecVoiceFont(HNMS   hNMS,
                       VFont* pVFont);

int nmsSetEncVoiceFont(HNMS  hNMS,
                       VFont* pVFont);

void nmsSetVFValue(float *pfValue, float fMin, float fMax, int direction);

int nmsSetVoiceEffect(	HNMS pNMS, float fPitchWarp, float fPitchOffset, float fSpectrumWarp,
								float fSpectrumOffset, float fSpectrumLedge, float fRobot, float fWhisper, int effect );

int nmsSetVoiceEffect4(	HNMS4 pNMS, int streamIndex, float fPitchWarp, float fPitchOffset, float fSpectrumWarp,
							 float fSpectrumOffset, float fSpectrumLedge, float fRobot, float fWhisper, int effect );

int nmsDecode4x(	HNMS4 hNMS,
						const unsigned char*  pIn1,
						const unsigned char*  pIn2,
						const unsigned char*  pIn3,
						const unsigned char*  pIn4,
						unsigned int          *nBytesConsumed,
						short*                pOut,
						unsigned int          *nBytesProduced );

#ifdef __cplusplus
}
#endif //__cplusplus


#endif //#ifndef _NELLYMOSER_H_INCLUDED_12_24_2002_

