//-----------------------------------------------------------------------------
//                               FMV Player
//
// MultiStream (v6 and above) audio handler
//
// $Workfile: fmvaudio_multistream.cpp $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "shellCore.h"
#include "shellMpeg.h"
#include <sdmacro.h>
#include "fmvaudio.h"

// MultiStream's ee directory must be added to the include path
#include "Audio/PS2/sound.h"


//-----------------------------------------------------------------------------
// C O N S T A N T S
//-----------------------------------------------------------------------------

#define AUTODMA_CH 0
#define UNIT_SIZE 1024

#define ADPCM_COPYBLOCK	   1024
#define IOP_SIZE_PCM       24576
#define IOP_SIZE_ADPCM     ADPCM_COPYBLOCK
#define IOP_SIZE_SPDIF     49152

#define VOLUME_MASTER_MAX  0x3fff
#define VOLUME_MASTER_MUTE 0x0000
#define VOLUME_PCM_MAX     0x7fff
#define VOLUME_PCM_MUTE    0x0000
#define VOLUME_ADPCM_MAX   0x3fff
#define VOLUME_ADPCM_MUTE  0x0000

#define SPU_VOICENO        0
#define SPU_MEMBASE        0x5010
#define SPU_BUFSIZE        4096
#define ZERO_BUFF_SIZE	   0x800

#define _ADPCM_MARK_START  0x04
#define _ADPCM_MARK_LOOP   0x02
#define _ADPCM_MARK_END    0x01


//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S 
//-----------------------------------------------------------------------------

static int iopZero;
static int iopBuff;
static int iopBuffSize;

static char _0_buf [ZERO_BUFF_SIZE] bss_align(64);
static u_char adpcmBuff[ADPCM_COPYBLOCK] __attribute__((aligned(64)));

static int started;
static u_int totalBytesSent;
static int iopLastPos;
static int spuLastPos;
static ShellMpegADSHeader *_sshd;
static EShellMpegAudioType _type;


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

static void changeMasterVolume( u_short val );
static void changePcmVolume( u_short val );
static void changeAdpcmVolume( u_short val );
static int sendToIOP2area( int pd0, int d0, int pd1, int d1,
		u_char *ps0, int s0, u_char *ps1, int s1 );
static void sendToIOP( int dst, u_char *src, int size, int wait );
static void sendToSPU( u_char *src, int size, int spuAddr );
static inline void adpcmSetLoopStart( u_char *a );
static inline void adpcmSetLoopEnd( u_char *a );
static void adpcmDeinterleave( u_long128 *src, u_long128 *dst,
						  	   u_int srcSkip, u_int srcTake,
						  	   u_int numBytes );
static u_int sendPCM( u_char *b, u_int bs, u_int d, u_int ds );
static u_int sendADPCM( u_char *b, u_int bs, u_int d, u_int ds );
static u_int sendSPDIF( u_char *b, u_int bs, u_int d, u_int ds );


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

void FMVAudioInit( void )
{
//	if(!shellFileLoadStartModule("libsd", 0, "", NULL))
//		ASSERTS(0, "Couldn't load IOP module");
		
//	if(!shellFileLoadStartModule("stream", 0, "", NULL))
//		ASSERTS(0, "Couldn't load IOP module");
	
//	if(SOUND_InitIOP() < 0) ASSERTS(0, "Init failed");

	SOUND_InitSPU();
	SOUND_InitStreamData(SND_LOAD_INTERNAL, 0, 0);
	SOUND_FlushIOPCommand(0);
}

void FMVAudioCreate( EShellMpegAudioType type, ShellMpegADSHeader *sshd )
{
	_sshd = sshd;
	_type = type;
	
    if     (type == SHELLMPEG_AUDIO_PCM)   iopBuffSize = IOP_SIZE_PCM;
    else if(type == SHELLMPEG_AUDIO_ADPCM) iopBuffSize = IOP_SIZE_ADPCM;
    else if(type == SHELLMPEG_AUDIO_SPDIF) iopBuffSize = IOP_SIZE_SPDIF;
	else                                   iopBuffSize = 0;

	if(iopBuffSize
		&& !(iopBuff = (int) sceSifAllocIopHeap((u_int) iopBuffSize)))
			ASSERTS(0, "Couldn't allocate IOP buffer");
	
    if(_type == SHELLMPEG_AUDIO_SPDIF)
    {
        if (!(iopZero = (int) sceSifAllocIopHeap(ZERO_BUFF_SIZE)))
        	ASSERTS(0, "Couldn't allocate IOP buffer");
        
		sendToIOP(iopZero, (u_char *) _0_buf, ZERO_BUFF_SIZE, 0);
	}
	
	started = 0;
	totalBytesSent = 0;
	iopLastPos = 0;
	spuLastPos = 0;
}

u_int FMVAudioSend( u_char *b, u_int bs, u_int d, u_int ds, int param )
{
	(void) param;
	
	switch(_type)
	{
		case SHELLMPEG_AUDIO_NONE:
			return 0;

		case SHELLMPEG_AUDIO_PCM:
			return sendPCM(b, bs, d, ds);

		case SHELLMPEG_AUDIO_ADPCM:
			return sendADPCM(b, bs, d, ds);
		
		case SHELLMPEG_AUDIO_SPDIF:
			return sendSPDIF(b, bs, d, ds);
			
		default:
			ASSERTS(0, "Unknown audio type");
	}
	
	return 0;
}

int FMVAudioIsStartable( void )
{
	switch(_type)
	{
		case SHELLMPEG_AUDIO_NONE:
			return 1;
			
		case SHELLMPEG_AUDIO_PCM:
		case SHELLMPEG_AUDIO_SPDIF:
			return totalBytesSent >= iopBuffSize;
			
		case SHELLMPEG_AUDIO_ADPCM:
			return totalBytesSent >= SPU_BUFSIZE;
		
		default:
			ASSERTS(0, "Unknown audio type");
			return 0;
	}
}

void FMVAudioStart( void )
{
	int n;

	switch(_type)
	{
		case SHELLMPEG_AUDIO_NONE:
			break;
			
		case SHELLMPEG_AUDIO_PCM:
	
		    changeMasterVolume(VOLUME_MASTER_MAX);
			changePcmVolume(VOLUME_PCM_MAX);

			SOUND_BlockTrans(AUTODMA_CH,
				(SD_TRANS_MODE_WRITE_FROM | SD_BLOCK_LOOP),
				(u_int) iopBuff, ((u_int) iopBuffSize / UNIT_SIZE) * UNIT_SIZE,
				(u_int) iopBuff);

			SOUND_FlushIOPCommand(0);
			break;

		case SHELLMPEG_AUDIO_ADPCM:
			
			changeMasterVolume(VOLUME_MASTER_MAX);
			changeAdpcmVolume(VOLUME_ADPCM_MAX);

			for(n = 0; n < _sshd->ch; n++)
			{	
				SOUND_SetParam((u_short) (AUTODMA_CH | ((SPU_VOICENO + n)<<1)
							| SD_VP_PITCH),
							(u_short) SOUND_HzToSPU((u_int) _sshd->rate));
							
				SOUND_SetParam((u_short) (AUTODMA_CH | ((SPU_VOICENO + n)<<1)
							| SD_VP_ADSR1), 0x00ff);
				
				SOUND_SetParam((u_short) (AUTODMA_CH | ((SPU_VOICENO + n)<<1)
							| SD_VP_ADSR2), 0x1fc0);
			}
				    
			if(_sshd->ch == 1)  // Mono
			{
				SOUND_SetAddr(AUTODMA_CH | SD_VA_SSA | (SPU_VOICENO << 1),
					    SPU_MEMBASE);
				
				SOUND_SetSwitch(AUTODMA_CH | SD_S_KON, (1 << SPU_VOICENO));
			}
		
			else  // Stereo
			{
				SOUND_SetAddr(AUTODMA_CH | SD_VA_SSA | (SPU_VOICENO << 1),
						SPU_MEMBASE);
				
				SOUND_SetAddr(AUTODMA_CH | SD_VA_SSA | ((SPU_VOICENO + 1) << 1),
					    SPU_MEMBASE + SPU_BUFSIZE);
				
				SOUND_SetSwitch(AUTODMA_CH | SD_S_KON,
					    (1 << SPU_VOICENO) | (1 << (SPU_VOICENO + 1)));
			}
		
			SOUND_FlushIOPCommand(0);
			break;
			
		case SHELLMPEG_AUDIO_SPDIF:
			
			SOUND_SetCoreAttr(SD_C_SPDIF_MODE,
				SD_SPDIF_MEDIA_DVD | SD_SPDIF_OUT_BITSTREAM |
				SD_SPDIF_COPY_NORMAL);
					
			SOUND_BlockTrans(AUTODMA_CH,
				(SD_TRANS_MODE_WRITE_FROM | SD_BLOCK_LOOP),
				(u_int) iopBuff, ((u_int) iopBuffSize / UNIT_SIZE) * UNIT_SIZE,
				(u_int) iopBuff);
			
			SOUND_FlushIOPCommand(0);
			break;
		
		default:
			ASSERTS(0, "Unknown audio type");
			break;
	}
	
	started = 1;
}

void FMVAudioDestroy( void )
{
	switch(_type)
	{
		case SHELLMPEG_AUDIO_NONE:
			break;
			
		case SHELLMPEG_AUDIO_PCM:
		
		    changePcmVolume(VOLUME_PCM_MUTE);
		    SOUND_BlockTrans(AUTODMA_CH, SD_TRANS_MODE_STOP, NULL, 0, 0);
			SOUND_FlushIOPCommand(0);	
			break;

		case SHELLMPEG_AUDIO_ADPCM:
		
			changeAdpcmVolume(VOLUME_ADPCM_MUTE);

			if(_sshd->ch == 1)  // Mono
				SOUND_SetSwitch(AUTODMA_CH | SD_S_KOFF,
					    (1 << SPU_VOICENO));
			
			else  // Stereo
				SOUND_SetSwitch(AUTODMA_CH | SD_S_KOFF,
					    (1 << SPU_VOICENO) | (1 << (SPU_VOICENO + 1)));
			
			SOUND_FlushIOPCommand(0);
			break;
			
		case SHELLMPEG_AUDIO_SPDIF:
		
			SOUND_BlockTrans(AUTODMA_CH, SD_TRANS_MODE_STOP, NULL, 0, 0);
			
			// Presumptuously take SPU out of SPDIF bitstream mode
			
			SOUND_SetCoreAttr(SD_C_SPDIF_MODE,
				SD_SPDIF_MEDIA_DVD | SD_SPDIF_OUT_PCM | SD_SPDIF_COPY_NORMAL);
				
			SOUND_FlushIOPCommand(0);
			sceSifFreeIopHeap((void *) iopZero);
			break;
		
		default:
			ASSERTS(0, "Unknown audio type");
	}

    sceSifFreeIopHeap((void *) iopBuff);
}


//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

static void changeMasterVolume( u_short val )
{
	SOUND_SetParam((u_short) (AUTODMA_CH | (0 | SD_P_MVOLL)), val);
	SOUND_SetParam((u_short) (AUTODMA_CH | (0 | SD_P_MVOLR)), val);
	SOUND_SetParam((u_short) (AUTODMA_CH | (1 | SD_P_MVOLL)), val);
	SOUND_SetParam((u_short) (AUTODMA_CH | (1 | SD_P_MVOLR)), val);
}

static void changePcmVolume( u_short val )
{
	SOUND_SetParam(AUTODMA_CH | SD_P_BVOLL, val);
	SOUND_SetParam(AUTODMA_CH | SD_P_BVOLR, val);
}

static void changeAdpcmVolume( u_short val )
{
    if(_sshd->ch == 1)
    {
		SOUND_SetParam(AUTODMA_CH | (SPU_VOICENO<<1) | SD_VP_VOLL, val);
		SOUND_SetParam(AUTODMA_CH | (SPU_VOICENO<<1) | SD_VP_VOLR, val);
    }
    
    else
    {
		SOUND_SetParam(AUTODMA_CH | (SPU_VOICENO<<1) | SD_VP_VOLL,
				val);
		SOUND_SetParam(AUTODMA_CH | (SPU_VOICENO<<1) | SD_VP_VOLR,
				VOLUME_ADPCM_MUTE);
		SOUND_SetParam(AUTODMA_CH | ((SPU_VOICENO + 1)<<1) | SD_VP_VOLL,
				VOLUME_ADPCM_MUTE);
		SOUND_SetParam(AUTODMA_CH | ((SPU_VOICENO + 1)<<1) | SD_VP_VOLR,
				val);
    }
}

static int sendToIOP2area( int pd0, int d0, int pd1, int d1,
		u_char *ps0, int s0, u_char *ps1, int s1 )
{
    if(d0 + d1 < s0 + s1)
    {
    	int diff = (s0 + s1) - (d0 + d1);

		if (diff >= s1)
		{
	    	s0 -= (diff - s1);
		    s1 = 0;
		}
		else s1 -= diff;
    }
    
    // (d0 + d1 >= s0 + s1)
    
	if(s0 >= d0)
    {
		sendToIOP(pd0,             ps0,             d0,             0);
		sendToIOP(pd1,             ps0 + d0,        s0 - d0,        0);
		sendToIOP(pd1 + s0 - d0,   ps1,             s1,             0);
    }
    
    else  // s0 < d0
    {
		if(s1 >= d0 - s0)
		{
			sendToIOP(pd0,          ps0,            s0,             0);
			sendToIOP(pd0 + s0,     ps1,            d0 - s0,        0);
			sendToIOP(pd1,          ps1 + d0 - s0,  s1 - (d0 - s0), 0);
		}
		else  // s1 < d0 - s0
		{
            sendToIOP(pd0,          ps0,            s0,             0);
            sendToIOP(pd0 + s0,     ps1,            s1,             0);
		}
    }
    
    return s0 + s1;
}

static void sendToIOP( int dst, u_char *src, int size, int wait )
{
	if(size <= 0) return;
	
	ASSERTS(src && (!((int) src & 0x3f)), "src not 64-byte aligned, or null");
	ASSERTS(dst && (!(dst & 0x3f)), "dst not 64-byte aligned, or null");

	SOUND_TransferDataToIOP((u_int) src, (u_int) dst, (u_int) size,
							wait ? SOUND_DMA_WAIT : SOUND_DMA_NO_WAIT);
}

static void sendToSPU( u_char *src, int size, int spuAddr )
{
	sendToIOP(iopBuff, src, size, 1);

    SOUND_TransIOPToSPU((u_int) iopBuff, (u_int) spuAddr, (u_int) size);
    SOUND_FlushIOPCommand(0);
}

static inline void adpcmSetLoopStart( u_char *a )
{
    *(a + 1) = _ADPCM_MARK_LOOP | _ADPCM_MARK_START;
    *(a + 0x10 + 1) = _ADPCM_MARK_LOOP;
}

static inline void adpcmSetLoopEnd( u_char *a )
{
    *(a - 0x0f) = _ADPCM_MARK_LOOP | _ADPCM_MARK_END;
}

static void adpcmDeinterleave( u_long128 *src, u_long128 *dst,
						  	   u_int srcSkip, u_int srcTake,
						  	   u_int numBytes )
{
	*D9_SADR = *D8_SADR = 0;
	*D9_QWC  = *D8_QWC  = numBytes / 16;

	*D_SQWC = (u_int) (srcSkip / 16) | ((srcTake / 16) << 16);
	*D9_MADR = (u_int) src;
	*D9_CHCR = (2 << 2) | (1 << 8);
	*D8_MADR = (u_int) dst;

	__asm__ volatile("sync.l");
	while(*D9_CHCR & (1 << 8))
		;

	*D8_CHCR = 1 | (1 << 8);

	while(*D8_CHCR & (1 << 8))
		;
}

static u_int sendPCM( u_char *b, u_int bs, u_int d, u_int ds )
{
    u_char *ps0, *ps1;
    int pd0, pd1;
	u_int s0, s1, d0, d1;
    u_int sent;
	int len;
    int pos;
	SOUND_PCM_INFO PCMInfo;

	if(started)
	{
		// Sync with the IOP to obtain stream position
		SOUND_FlushIOPCommand(0);
		
		SOUND_GetPCMInfo(&PCMInfo);
		pos = (int) PCMInfo.IOPCurrent - (int) iopBuff;
			
		len = (int) ((pos + iopBuffSize - iopLastPos - UNIT_SIZE)
	    		% iopBuffSize);
	    
	    if((pos - iopLastPos) >= 0 && (pos - iopLastPos) < UNIT_SIZE)
	    {
	        d0 = 0;
	        d1 = 0;
	    }

		len = (len / UNIT_SIZE) * UNIT_SIZE;
	    
	    if(iopBuffSize - iopLastPos >= len)  // area0
	    {
			pd0 = iopBuff + (int) iopLastPos;
			d0 = (u_int) len;
			pd1 = 0;
			d1 = 0;
	    }
	    else  // area0 + area1
	    { 
			pd0 = iopBuff +  (int) iopLastPos;
			d0 = (u_int) (iopBuffSize - iopLastPos);
			pd1 = iopBuff;
			d1 = (u_int) (len - (iopBuffSize - iopLastPos));
		}
	}

	else
	{
	    pd0 = iopBuff + (int) (totalBytesSent % iopBuffSize);
	    d0 = (u_int) ((iopBuffSize - (int) totalBytesSent) / UNIT_SIZE)
	    			  * UNIT_SIZE;
	    pd1 = 0;
	    d1 = 0;
	}

	ds = (ds / UNIT_SIZE) * UNIT_SIZE;

	ps0 = b + d;
	ps1 = b;

	if(d + ds > bs)
	{
		s0 = bs - d;
		s1 = ds - (bs - d);
	}
	
	else
	{
		s0 = ds;
		s1 = 0;
	}

	sent = (u_int) sendToIOP2area(pd0, (int) d0, pd1, (int) d1, ps0,
								  (int) s0, ps1, (int) s1);
	iopLastPos = (iopLastPos + (int) sent) % iopBuffSize;
	totalBytesSent += sent;
	return sent;
}

static u_int sendADPCM( u_char *b, u_int bs, u_int d, u_int ds )
{
	int diff = 0;
	u_int retVal = 0;

	if(started)
	{
		// Sync with the IOP to obtain stream position
		SOUND_FlushIOPCommand(0);
			
		diff = ((int) SOUND_GetChannelSPUAddress(SPU_VOICENO) - SPU_MEMBASE
				- spuLastPos + SPU_BUFSIZE) % SPU_BUFSIZE;		
    }

	else
	{
		if(ds < ADPCM_COPYBLOCK) return 0;
	    else diff = SPU_BUFSIZE - (int) totalBytesSent;		
	}
	
	while((diff >= ADPCM_COPYBLOCK) && (ds >= ADPCM_COPYBLOCK * _sshd->ch))
	{
		int ch;

		for(ch = 0; ch < _sshd->ch; ch++)
		{
			u_char *src, *dst;

			src = b + ((d + bs) % bs) + (ch * _sshd->interSize);
			dst = adpcmBuff;

			adpcmDeinterleave((u_long128 *) src, (u_long128 *) adpcmBuff,
							  (u_int) _sshd->interSize * (_sshd->ch - 1),
							  (u_int) _sshd->interSize, ADPCM_COPYBLOCK);
						
			if(spuLastPos + ADPCM_COPYBLOCK == SPU_BUFSIZE)
				adpcmSetLoopEnd((u_char *) ((int) (adpcmBuff + ADPCM_COPYBLOCK)
											| 0x20000000));

			if(spuLastPos == 0)
				adpcmSetLoopStart((u_char *) ((int) adpcmBuff | 0x20000000));

			sendToSPU(adpcmBuff, ADPCM_COPYBLOCK,
			      SPU_MEMBASE + (SPU_BUFSIZE * ch) + spuLastPos);
		}

		spuLastPos = (spuLastPos + ADPCM_COPYBLOCK) % SPU_BUFSIZE;

		d += ADPCM_COPYBLOCK * (u_int) _sshd->ch;		
		ds -= ADPCM_COPYBLOCK * (u_int) _sshd->ch;

		diff -= ADPCM_COPYBLOCK;

		// For ADPCM, totalBytesSent refers to bytes sent for one channel only
		totalBytesSent += ADPCM_COPYBLOCK;
		
		retVal += ADPCM_COPYBLOCK * (u_int) _sshd->ch;
	}

	return retVal;
}

static u_int sendSPDIF( u_char *b, u_int bs, u_int d, u_int ds )
{
	SOUND_PCM_INFO PCMInfo;
	int destSize = 0;
	u_int retVal = 0;
	u_int src = d;

	if(started)
	{
		// Sync with the IOP to obtain stream position
		SOUND_FlushIOPCommand(0);
		SOUND_GetPCMInfo(&PCMInfo);
		
		destSize = ((int) PCMInfo.IOPCurrent - (int) iopBuff - iopLastPos
					+ iopBuffSize) % iopBuffSize;
	}
	
	else destSize = iopBuffSize - (int) totalBytesSent;

	// Each 512 bytes of SPDIF data must be padded with 512 bytes of
	// empty data for correct playback of SPDIF by the SPU.

	while((ds >= 512) && (destSize >= 1024))
	{
		sendToIOP(iopBuff + iopLastPos, b + src, 512, 0);
		src = (src + 512) % bs;
		sendToIOP(iopBuff + iopLastPos + 512, (u_char *) _0_buf, 512, 0);

		iopLastPos = (iopLastPos + 1024) % iopBuffSize;
		ds -= 512;
		destSize -= 1024;

		// For SPDIF, totalBytesSent includes the zero-padding bytes
		totalBytesSent += 1024;
		
		retVal += 512;
	}
	
	return retVal;
}
