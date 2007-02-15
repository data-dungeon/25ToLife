/* SCE CONFIDENTIAL
 $PSLibId$
 */
/* 
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                         - Multistream -
 *
 *                           Shift-JIS
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Europe 
 *                        All Rights Reserved.
 *
 *                            stream.c
 *                  IOP code of Multistream's API
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      3.00            Oct,31,2001      Jason Page
 *      6.00            Nov,28,2002      Jason Page
 *      6.10            Jan,31,2003      Jason Page
 *      6.20            Feb,25,2003      Jason Page
 *		7.00 (DSP)		Jun,25,2003		 Jason Page
 *		7.1				Jan,6,2004		 Jason Page
 *		7.2				Apr,13,2004		 Jason Page
 *		7.3				Nov,5,2004		 Jason Page
 */

/**********************************************************************************
A big thankyou to Criterion Games for first showing the DSP effect via the
hardware effect unit technique at the SCEE DevStation 2003.
***********************************************************************************/

#define SOUND_MULTISTREAM_VERSION_ID 73

#include <kernel.h>
#include <sys/types.h>
#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <msifrpc.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <libsd.h>
#include <libcdvd.h>
#include "stream.h"
#include "ms_dsp.h"
#include "ms_switches.h"
//#include "sulphaapp.h"


#if (SOUND_DTS_ENABLE)
#include "../dts/iopdts.h"
#endif

#if (SCREAM_COMPATIBLE)
	#include "../../scream/iop/include/scream.h"
	
	int	g_SPURightsSema = -1;

	#define GET_SPU_RIGHTS()		{ WaitSema(g_SPURightsSema); 	}
	#define RELEASE_SPU_RIGHTS()	{ SignalSema(g_SPURightsSema); 	}
	
	int	g_SPUDMADoneSema = -1;
#else
	#define GET_SPU_RIGHTS()		{}
	#define RELEASE_SPU_RIGHTS()	{}
#endif

/* Bits set in these next fields indicate voices that MS is allowed to *
 * use. These can be set using the SOUND_SetMultiStreamVoices function.*
 * NOTE: For sake of spead this IS NOT checked every time a voice is   *
 * modified. They are ONLY used by the functions...                    *
 *     SOUND_AllChannelEffectsOn                                       *
 *     SOUND_AllChannelEffectsOff                                      *
 *     SOUND_AllChannelOutputOn                                        *
 *     SOUND_AllChannelOutputOff                                       *
 *     SOUND_Pause                                                     *
 *     SOUND_Resume                                                    *
 *     SOUND_FastPitch0                                                *
 *     SOUND_InitSmoothFade                                            *
 * So, for example, If the end user asks to start a stream on an SPU   *
 * channel that is not marked as usable the code below WILL still go   *
 * ahead and start the stream! The end user must take care in his MS   *
 * Channel selections.                                                 */

u_int g_MSChannelUseMask[2] = {0xffffff, 0xffffff};  

#define CHANNEL_FLAG_SET(x,y) (((x < 24) ? ((1L<<x) & (*y)) : ((1L<<(x-24)) & (*(y+1)))) != 0)

u_int JPTESTADDR;
ADPCMFILE *JPTESTFILE=0;
ADPCMFILE *JPOLDTESTFILE=0;

volatile int loadtest=0;
/*------------------------------------------------------------------ */
// MACROS NOW DISABLED (V7.0)!! NOW USES FUNCTION CALLS FOR DEBUG PURPOSES

/* ADPCM macros. */
/* Allows loop start and end points to be set. */
/* Allows end markers to be cleared */
/* Allows Final marker to be set for once-off playback */

#define _ADPCM_MARK_START 0x04
#define _ADPCM_MARK_LOOP  0x02
#define _ADPCM_MARK_END   0x01

#if 0
/* End marker - sets marker for once off playback (a=start or mid address, s is offset past this point) */
#define _AdpcmSetMarkFINAL(a,s) { *((unsigned char *)((a)+(s)-0x1f)) = (_ADPCM_MARK_END); *((unsigned char *)((a)+(s)-0x0f)) = (_ADPCM_MARK_LOOP | _ADPCM_MARK_START | _ADPCM_MARK_END); }

/* Start of loop */
#define _AdpcmSetMarkSTART(a) {*((unsigned char *)((a)+1)) = (_ADPCM_MARK_LOOP | _ADPCM_MARK_START); };

/* End of loop - note that a (address) is either the start or mid address of the IOP buffer. s is half the buffer size */
#define _AdpcmSetMarkEND(a,s) { *((unsigned char *)((a)+(s)-0x0f)) = (_ADPCM_MARK_LOOP | _ADPCM_MARK_END); };

/* Remove any end markers from original data if looping - (a=start or mid address, s is offset past this point) */
/* Not needed for interleaved data files, as they have already had their end markers removed. */
#define _AdpcmSetMarkRESTART(a,s) { *((unsigned char *)((a)+(s)-0x0f)) = (0); *((unsigned char *)((a)+(s)-0x1f)) = (0);}

#define _AdpcmSetMarkClearEND(a,s) { *((unsigned char *)((a)+(s)-0x0f)) = (0); };
#endif

/*------------------------------------------------------------------ */
/****** Allocation for file and SFX buffers ***** */

#if (MS_SCANLINE_DEBUG==1)
int SOUND_SCANLINE1=0;
int SOUND_SCANLINE2=0;
#endif

volatile static int MS_IntrFlag=0;
volatile static int MS_RPCFlag=0;

ADPCM_SPOTINFO *JSPOTFX;
ADPCMFILE *JADPCM;

SOUND_TRANS SOUND_Transfer;

u_int MAX_ADPCM_FILES=0;	/* Amount of ADPCMFILE records allocated by user */
u_int MAX_SPOT_FX=0;		/* Amount of ADPCM_SPOTINFO records allocated by user */
u_int FILE_COUNTER=0;		/* Counter for how many files that have been set by user (can't go above MAX_ADPCM_FILES) */
u_int SFX_COUNTER=0;		/* Counter for how many SFX that have been set by user (can't go above MAX_SPOT_FX) */
u_int SPOT_IOPAddress;				/* Address of malloc'd IOP RAM used for transferring SpotFX */
char SPOTFX_List_Allocated=0;		/* Flag set if SpotFX memory has been allocated */
char ADPCM_FileList_Allocated=0;	/* Flag set if FileID memory has been allocated */

static int MS_IntrFlagPrevious; //wouldn't need this if MS_IntrFlag were still 0/1, but now it also represents state for debug.

/*------------------------------------------------------------------ */
/****** Hardware effect units ***** */
sceSdEffectAttr effect_attribute __attribute__((aligned (16)));

u_int g_OutputChans[2]={0xffffff,0xffffff};  /* MIXV output for all SPU channels ON */

u_int g_ReverbChans[2]={0,0};	/* Reverb on/off bit patterns for 24 channels on both cores. */
int CurrentReverb[2]={0,0};		/* Current reverb type (Hall/StudioA..etc..etc..) */
u_int SOUND_ReverbAddress[2]={0x1fffff,0x1dffff};
u_int CurrentReverbAddr[2]={0x1fffff,0x1dffff};		/* Reverb core addresses. */
u_int CurrentReverbSize[2]={0,0};	/* Reverb core sizes */
u_int ReverbSizes[10]={0,0x26c0,0x1f40,0x4840,0x6fe0,0xade0,0xf6c0,0x18040,0x18040,0x3c00};	/* List of reverb sizes */

int ooooooo=0;

/*------------------------------------------------------------------ */
/****** Smooth Volume table ***** */
SOUND_SMOOTH_VOL_FADE SOUND_SmoothFade[48][2];	/* Left/Right for 48 SPU channels */

/*------------------------------------------------------------------ */
/****** SIF RPC data buffers ***** */
static int rpc_arg [512];		/* Size of Buffer sent from EE (2K) */
volatile int aret[512];			/* Buffer sent from IOP to EE (2K) */

/*------------------------------------------------------------------ */
/****** SOUND_GetFileInfo data ***** */
u_int SOUND_FILE_LSN=0;			/* LSN of file requested by SOUND_GetFileInfo (returned to EE) */
u_int SOUND_FILE_SIZE=0;		/* Size of file requested by SOUND_GetFileInfo (returned to EE) */
u_int SOUND_FILE_SOURCE=0;		/* Source (host or cd) of file requested by SOUND_GetFileInfo (returned to EE) */
u_int SOUND_FILE_COUNT=0;		/* Counter increased every time a new SOUND_GetFileInfo packet is returned to the EE */
int SOUND_FILE_AT_OPEN=-1;
int SOUND_FILE_AT_ID=-1;

int MyCounter=0;			/* Counter to make sure that the EE keeps in sync with the IOP */
int LoadEECounter=0;		/* As above. */
int E_CDErrorCheck=0;		/* EXTERNAL CD error checking. Set to 1 if a CD error has occured when trying to load data */
int I_CDErrorCheck=0;		/* INTERNAL CD error checking */
u_long MaxIOPRAM=0;			/* Maximum amount of IOP RAM that can be allocated (returned to EE) */
int UserMaxStreams=0;		/* Maximum amount of streams that can be played at once, set by user (0-47) */
int LastLoadCheck=0;		/* Passed from EE->IOP. So IOP knows when a new request has been made for loading etc. */

/*------------------------------------------------------------------ */

/*------------------------------------------------------------------ */
/****** Current write addresses ***** */
u_int StreamMemoryEnd=0x5010;		/* SPU address holding the destination for the next resident sample (can be set by user) */
u_int StreamEEAddress;		/* EE address that data will be transfered to if streaming using STREAM_LOAD_EE */
u_int StreamIOPAddress;		/* IOP address that data will be transfered to if streaming using STREAM_LOAD_IOP */
u_char StreamMemoryEndFlag=0; /* Set to 1 when SOUND_SetSPUWriteAddress is called, set to 0 when SOUND_PlayStream is called */
u_char StreamEEFlag=0; /* Set to 1 when SOUND_SetSPUWriteAddress is called, set to 0 when SOUND_PlayStream is called */
u_char StreamIOPFlag=0; /* Set to 1 when SOUND_SetSPUWriteAddress is called, set to 0 when SOUND_PlayStream is called */

/*------------------------------------------------------------------ */
/****** Stereo playback (pre V5) ***** */
int StereoStarted=0;		/* Flag returned to EE to say if a request for 2 channel pitches to be set has been completed */
int	StereoChan1=0;			/* SPU channel of 1st stereo stream */
int	StereoChan2=0;			/* SPU channel of 2nd stereo stream */
u_int StereoPitch=0;		/* Pitch to set both SPU channels to when both streams are preloaded and keyed on. */

/*------------------------------------------------------------------ */
/****** 48 Stream buffers ***** */
STREAMDATA StreamBuffer[MAX_NUMBER_OF_STREAMS];	/* 48 stream buffers */

TimerCtx timer;
int gThid = 0;		/* Thread ID */
#if MS_TIMER
int gSem = 0;		/* Sema ID (multistream waits for this sema. Sema is set by an interrupt approx once per frame) */
int SOUND_TimerUpdate=0;
#endif

/*------------------------------------------------------------------ */
/****** Fast Internal playback ***** */
u_int SOUND_INTBuffer[10] __attribute__((aligned(64)));	/* Buffer sent to EE */
u_int SOUND_INTPacket[64/4] __attribute__((aligned(64)));	/* Buffer recieved from EE */
#define SOUND_RPC_DEVICE	0x12344321

/*------------------------------------------------------------------ */
/****** MultiStream flags ***** */
volatile int SPU_DMA_TRANS_FLAG=0;	/* Halts checking of doublebuffering as SPU DMA 6transfer currently in operation. */
volatile int USR_HALT_MULTISTREAM=0;	/* As above - set around user calls such as StartStream - just to stop any init problems */
volatile int SOUND_MasterEnable=0;
int IOPRequestLoad=-1;			/* Stream number passed from IOP->EE. EE sends this value back if it allows loading */
volatile int StreamBusy=0;				/* Set to !=0 if a stream is loading from host/cd */
int Cntr1=0;					/* IOP Counter. Increased each time SOUND_GetStatus is called */
int LoadFromEE=0xffff;			/* Stream number passed from EE->IOP to allow loading (0xffff = don't allow loading) */

#if (MS_SCANLINE_DEBUG==1)
int JP_IntCount=0;
int JP_IntCount2=0;
int JP_IntCount3=0;
#endif

#if (MS_DSP_ACTIVE==1)
extern char SOUND_DSP_ENGINE;
extern volatile char SOUND_DSP_PROCESSING;
extern u_short SOUND_DSP_EEPacketSize;

extern int SOUND_DSP_Result[8];
extern u_int SOUND_DSPBuffer;			// EE Address for PCM data to be sent
extern char  SOUND_DSP_EEActive;		// EE DSP effect active (SOUND_DSP_ON / OFF)
//extern volatile char SOUND_DSP_EECounter;		// Current counter value (0-3)
//extern volatile char SOUND_DSP_EECounter2;   	// Current counter value (0-0xff)
//extern volatile u_int SOUND_DSP_EECounter3;   	// DSP_EECounter | (DSP_EECounter2<<8)
extern volatile u_int SOUND_DSP_EECounter4;   // 0-3 (rolls around)
extern volatile u_int SOUND_DSP_EECounter5;   // just cos
extern volatile u_int SOUND_DSP_IOPCounter;	// amount of packets sent back to IOP (0-3)
extern volatile u_int SOUND_DSP_IOPCounter2;	// 0-3 (rolls around)
extern volatile u_int SOUND_DSP_EEReturned;
extern int SOUND_DSP_FinalAddressList;
extern u_int SOUND_DSPBufferCounter;	// EE Address for DSP counter to be sent
extern u_int SOUND_DSP_PCMBuffer;	// Address of PCM buffer for EE DSP effects [512*4]
extern u_int SOUND_DSP_PCMBufferTemp;	// Address of PCM buffer for EE DSP effects [512*4]
#endif

/* compiler does not move '__asm__ volatile'. */
/*#define USR_HALT_MULTISTREAM_SET() __asm__ volatile ("sw %1,%0": "=m" (USR_HALT_MULTISTREAM) : "r" (1)) */
/*#define USR_HALT_MULTISTREAM_CLR() __asm__ volatile ("sw $0,%0": "=m" (USR_HALT_MULTISTREAM)) */

/*------------------------------------------------------------------ */
/****** CD loading things ***** */

/*buffer to store a sector in (when only a part of a sector is reqd) */
static unsigned char cd_buffer[CD_SECTOR_SIZE] __attribute__((aligned(64)));	
/*int Jcderror=SCECdErNO;			// CD error flag */

static u_int SOUND_CD_ERRORCOUNT=0;	// Passed back to EE in event of a CD error
static u_int SOUND_CD_ERRORCODE=0;
static char SOUND_CD_I_ERROR=0;

u_char CDCheck=0; 		/* Flag for CD errors (0 = all ok, 1 = MultiStream paused due to CD error) */
int FilesOnCD=0;   		/* Set to !=0 if any files are on CD */
int CD_IDLE=1;
int CD_EVENT_OCCURED=0; /*set to 1 when the callback is called so things can be handled OUTSIDE of the callback */
int cd_currevent=0;	/*what the current file operation is */
int cd_stage=CD_STAGE_DONE; /*stage in the file operation */
int cd_startsec;	/*start sector of the file */
int cd_readptr;		/*pointer in file that we're currently reading from */
u_int cd_playlen;	/*current amount of data loaded */
int cd_writeptr;	/*pointer to where to write read data to */
int cd_startptr;	/*pointer to where data first gets written to */
int cd_datalen;		/*amount of data left to read */
u_int cd_jdatalen;	/*lenght of data block that is required. */
int cd_filelen;		/*file length (to work out if wrapping needs to be done) */
int cd_stream;		/*current stream */
int cd_readlen;		/*amount of data currently being read */
int cd_localcopy;	/*set to 1 if the read is being copied to cd_buffer */
int cd_localstart;	/*start in local buffer of where the data is that is reqd */
int cd_localstart2;	/*start in local buffer of where the data is that is reqd */
int cd_source;		/*either ADPCM_DATA_ON_HOST or ADPCM_DATA_ON_CD */
cdFunc cd_func;		/*function to call on completion */
cdFunc endfunc_load;
sceCdRMode cd_cdrmode;	/* structure setup in SOUND_InitCD */
u_char CD_READ_STARTED=0;

/*------------------------------------------------------------------ */
/****** SPU channel info ***** */

/* Current key status returned to the EE for all 48 SPU channels (see above for values stored in here..) */
u_char SPU_KeyStatus[48]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

short SPU_VolL[48]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
short SPU_VolR[48]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* Loop counter for each SPU channel (see SOUND_PlaySFXLoop) */
int SPU_LoopCounter[48]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* Table containing pitches for all 48 channels. When in pause mode, all pitches are stored in here, and then all */
/* pitches are set to 0 (pausing everything) */
u_short SPU_PausePitch[48]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* Same as above, but this table is required if there is a CD error (where you need to pause all streams, but not */
/* SFX. Also, you need to make sure that if the user then pauses the game, you don't re-pause these streams.. */
/* Otherwise, you will read 0 as the current pitch for the streams, and then will not start again. */
u_short SPU_CDError[48]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

u_int SPU_NAX[48]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

u_char SOUND_PauseStreamCheck[48];


/*------------------------------------------------------------------ */
/****** SPU Transfer ***** */
SPUDMA_TRANS SOUND_SPUTransfer;
u_int DTS_MAX_SIZE=0;

/*------------------------------------------------------------------ */
/****** EXTERNAL loading information ***** */
u_char	EXTERNAL_CDLOAD=USE_INTERNAL_LOADING;	/* Loading type (internal, MultiStream CD code..) */
u_char	EXTERNAL_CDLOAD_FLAG=CD_TRANSFER_DATA;	/* Flag=CDTransfer */

u_int EXT_LD_DEST;			/* Either STREAM_LOAD_EE or STREAM_LOAD_IOP */
u_int EXT_LD_IOP_ADDR=0;	/* IOP address of stream buffer that is requiring data (passed to EE if EXTERNAL loading) */
u_int EXT_LD_SIZE=0;		/* Size of data required (passed to EE if EXTERNAL loading) */
u_int EXT_LD_CHAIN_COUNT;	/* Amount of streams to chain */
u_int EXT_LD_CHAIN_POS;	 	/* Current position through chain list */
int EXT_LD_CNTR=0;			/* Counter passed to EE if EXTERNAL loading (so EE knows its a new request from IOP) */
u_int EXT_LD_SEEK[9];	 	/* Seek offset into file */
u_int EXT_LD_ID[9];		 	/* ID of file */
u_int EXT_LD_PLAYSIZE[9];	/* Size of data to load in file */
u_int EXT_LD_LSN[9];	 	/* LSN of file on CD */
u_int EXT_LD_REMAIN;		/* Remainder of current file to be loaded */
u_int EXT_LD_OFFSET;

/*------------------------------------------------------------------ */
/****** CD Error temp buffer ***** */
unsigned char CD_ERR_SPUChan;
unsigned char CD_ERR_PlayingHalf;
unsigned char CD_ERR_iStatus;
unsigned char CD_ERR_Stream;
u_short CD_ERR_ChainPos;
int CD_ERR_FileID;
u_int CD_ERR_FileSeek;
u_int CD_ERR_SPUAddress;
u_int CD_ERR_FileRemainder;
u_int CD_ERR_FileSize;
u_int CD_ERR_SectorStart;
u_int CD_ERR_IOPCurrentAddress;
u_int CD_ERR_IOPCurrentRemainder;
u_int CD_ERR_TransCounter;
u_int CD_ERR_LoopInfo;
u_int CD_ERR_Req;

/*------------------------------------------------------------------ */
/****** Multiple KeyOn info ***** */
int SOUND_KeyOnBitmask[2]={0,0};	/* Used if SOUND_KeyOnChannel is called */
int SOUND_SFXAllowKeyOn=0;

/*------------------------------------------------------------------ */
/****** EE data transfer ***** */
u_int EE_TranCount=0;
u_int EE_LastTranSize=0;
u_int SOUND_WaitEEAddress=0;	/* Used for handling IOP->EE waiting */
u_int SOUND_WaitCounter=0;
/*------------------------------------------------------------------ */
/****** PCM playback ***** */
u_int	PCMBuffer=0;
u_int	PCMBufferSize=0;
u_int	PCMLastAddr=0;

int		PCMVolL;
int 	PCMVolR;
int		PCMStream=-1;		/* Stream channel which is playing stream */
u_char	PCMPlaying=0;
u_char	PCMMonoAlloc=0;
u_char	PCMStereo=0;	/* 0 = mono, 1 = stereo */
u_char	PCMType=0;		/* 0 = wav, 1 = ps2 pcm */
int 	PCMHalf=0;
u_char	PCMPause=0;
u_char	PCMCDPause=0;
u_int PCMPausePos;		/* IOP address where playback of PCM was paused */

char SOUND_INTERNAL_FastLoad=SOUND_FASTLOAD_OFF;
char SOUND_BIND_FastLoad=0;
char SOUND_FastLoadDCache=1;
sceSifClientData INTERNALrpc;

int SOUND_DTS_Surround=0;

#if (SOUND_DTS_ENABLE)
int DTS_StopFlag=0;
#define DTS_REQUEST_MULTISTREAM_SHUTDOWN 1
#define DTS_ALLOW_MULTISTREAM_CONTINUE 0
#define DTS_MULTISTREAM_SHUTDOWN 2
volatile int DTS_InitFlag=DTS_ALLOW_MULTISTREAM_CONTINUE;
#endif

SOUND_USER_TRANSFER SOUND_UserTransfer;


/* Priority loading.. */
/*int SOUND_PriorityStream=-1; */
/*int SOUND_PriorityStreamType=SOUND_FORCE_BEFORE_AUDIO; */
/*int SOUND_PriorityStreamTime=SOUND_FORCE_UNTIL_STREAM_DIES; */

/****** End of variables ***** */

/**********************************************************************************
msabs
***********************************************************************************/

int msabs (int a)
{
	if (a < 0) return -a;
	return a;
}

/**********************************************************************************
SOUND_DisplayStats
	Display current MultiStream status on IOP output screen.

	Requires:	Nothing
	Returns:	Nothing

	Displays:	Version number 
				Allocated Spot FX details (ID and SPU address)
				Allocated File details (ID, LSN, Size, Offset to load from)
				Allocated Stream details (channel, IOP & SPU address and sizes)
***********************************************************************************/
void SOUND_DisplayStats(void)
{
ADPCM_SPOTINFO *SFX;
ADPCMFILE *File;
int i,j;

	MSERROR(("\n************************************************************************\n"));
	MSERROR(("SOUND_DisplayStats\n"));
	MSERROR(("========================================================================\n"));
	MSERROR(("VERSION: MultiStream V%d.%d\n",SOUND_MULTISTREAM_VERSION_ID/10,SOUND_MULTISTREAM_VERSION_ID%10));
#if (MS_LOAD62==0)
	MSERROR(("   SPU buffering currently in V5.2 (safe) mode.\n"));
	MSERROR(("   SOUND_ResizeSPUBuffer will have no effect.\n"));
	MSERROR(("   SPU RAM buffers must be the same size as stream IOP buffers.\n"));
#else
	MSERROR(("   SPU buffering currently in V6.2+ mode.\n"));
	MSERROR(("   SOUND_ResizeSPUBuffer ACTIVE.\n"));
	MSERROR(("   SPU RAM buffers can be different sizes to their stream IOP buffers.\n"));
#endif
#if (SCREAM_COMPATIBLE)
	MSERROR(("   SCREAM COMPATIBLE\n"));
#else
	MSERROR(("   NOT SCREAM COMPATIBLE (modify ms_switches.h and recompile IOP to enable)\n"));
#endif

	MSERROR(("INFO: SPOT FX\n"));
	if (SPOTFX_List_Allocated==0)
	{
		MSERROR(("   No Memory Allocated\n"));
	}
	else
	{
		MSERROR(("   Memory Allocated at IOP Address: 0x%x\n",(int)JSPOTFX));
		MSERROR(("   Allocated space for %d SFX\n",MAX_SPOT_FX));
		SFX=JSPOTFX;
		for (i=0;i<MAX_SPOT_FX;i++)	/* Loop through all SFX slots. */
		{
			if (SFX->ID!=-1)
				MSERROR(("      slot %d SFX ID = 0x%x. SPU Address = 0x%x\n",i,SFX->ID,SFX->SPUAddress));
			SFX++;
		}
		MSERROR(("\n"));
	}

	MSERROR(("INFO: FILE\n"));
	if (ADPCM_FileList_Allocated==0)	/* File memory already allocated? */
	{
		MSERROR(("   No Memory Allocated\n"));
	}
	else
	{
		MSERROR(("   Memory Allocated at IOP Address: 0x%x\n",(int)JADPCM));
		MSERROR(("   Allocated space for %d files\n",MAX_ADPCM_FILES));
		File=JADPCM;
		for (i=0;i<MAX_ADPCM_FILES;i++)	/* Loop through all SFX slots. */
		{
			if (File->ID!=-1)
			{
				MSERROR(("      slot %d FileID = 0x%x. LSN = 0x%x. Size = 0x%x. Offset = 0x%x\n",i,File->ID,
					File->lsn, File->size,File->offset));
			}
			File++;
		}
		MSERROR(("\n"));
	}

	j=0;
	MSERROR(("INFO: STREAM\n"));
	for (i=0;i<48;i++)
	{
		if (StreamBuffer[i].MemoryAllocated!=0)
		{
			MSERROR(("   Stream Channel %d allocated: IOP Addr=0x%x IOP Size=0x%x, SPU Addr=0x%x, SPU Size =0x%x\n",
			i,StreamBuffer[i].IOPAddress,StreamBuffer[i].AllocSize,
			StreamBuffer[i].AllocSPUAddress,StreamBuffer[i].SPUBufferSize));
			j++;
		}
	}
	if (j==0)
	{
		MSERROR(("   No streams allocated\n"));
	}
	MSERROR(("\ncall SOUND_DisplayStats to show this information at any time\n"));

	MSERROR(("************************************************************************\n"));
}
/**********************************************************************************
SOUND_InitStreamData
	This routine initialises the buffers and turns everything off - call once.
	Requires:	type		USE_INTERNAL_LOADING	Uses the MultiStream CD/Host loading routines
							USE_EXTERNAL_LOADING	Passes data back to EE for external routines
				maxfiles	Maximum amount of FileID's you will have "open" at once
				maxsfx		maximum amount of SFX you will have open at once

	Note: passing 0 for either maxfiles or maxsfx will cause no change to the current
	memory allocation for these buffers. So, you can call SOUND_AllocateFileMemory
	before this function and then just pass 0 as the maxfiles parameter here.
***********************************************************************************/
int SOUND_InitStreamData(int type,int maxfiles, int maxsfx,u_short priority)
{
int i;

	cd_cdrmode.spindlctrl=SCECdSpinNom;	/* Set to nom as default */

	EXTERNAL_CDLOAD=(unsigned char)type;

	FilesOnCD=0;
	SOUND_FastLoadDCache=1;	// initialised so a Flush on the EE will be performed before sending data

/* Init CD code now a seperate function (SOUND_InitCD) from V5 onwards.. */

	EXTERNAL_CDLOAD_FLAG=CD_TRANSFER_DATA;

	SOUND_AllocateFileMemory(maxfiles);		/* Allocate n slots for holding n file name records */
	SOUND_AllocateSpotFXMemory(maxsfx);		/* Allocate n slots for holding n spot fx records */

	SOUND_Transfer.TranStatus=0;
	SOUND_Transfer.TranMode=SOUND_DTS_OFF;

	StereoStarted=0;	/* Init Stereo playback info (pre 5.0. Not really required now..) */
	StereoChan1=0;
	StereoChan2=0;
	StereoPitch=0;

	SOUND_UserTransfer.Status=0;

	for (i=0;i<MAX_NUMBER_OF_STREAMS;i++)	/* Initialise all streams */
	{
		StreamBuffer[i].Error=0;
		StreamBuffer[i].Status=STREAM_STATUS_OFF;	/* Turn off all streams */
		StreamBuffer[i].VolL=0;
		StreamBuffer[i].VolR=0;
		StreamBuffer[i].IOPAddress=0;
		StreamBuffer[i].IOPAllocAddress=0;
		StreamBuffer[i].MemoryAllocated=0;			/* Clear flag to say that no memory is allocated */
		StreamBuffer[i].PlayingHalf=STREAM_FIRST_HALF;
		StreamBuffer[i].iStatus=iSTATUS_WAIT;
		StreamBuffer[i].ParentStream=-1;
		StreamBuffer[i].AllocSize=0;
		StreamBuffer[i].MIBEnd=0;
		StreamBuffer[i].MIBLoop=0;
	}

	SOUND_InitSmoothFade();

/* Initialise stream pause flags. */
	for (i=0;i<48;i++)
	{
		SOUND_PauseStreamCheck[i]=0;
	}

#if MS_TIMER
    if (gSem == 0)
    {
		struct SemaParam sem;
		sem.initCount = 0;
		sem.maxCount = 1;
		sem.attr = AT_THFIFO;
		gSem = CreateSema (&sem);			/* Create Sema */
    }
#endif

	if( gThid == 0 )
	{
		gThid = makeMyThread(priority);				/* Create Thread */
		StartThread( gThid, (u_long)NULL );
	}

/* Kick off IOP->SPU DMA transfer callback. Each time a transfer is completed, the routine: */
/* _AdpcmDmaInt is called. */
    sceSdSetTransIntrHandler (MS_SPU_DMA, (sceSdTransIntrHandler) _AdpcmDmaInt,(void *) &SPU_DMA_TRANS_FLAG);

#if MS_TIMER
/* This timer wakes the main MultiStream program (StreamADPCM) approx 60fps */
	setTimer(&timer);		/* Setup IRQ interrupt */
	startTimer(&timer);		/* Start IRQ interrupt (calls TimerFunc) */
#endif

// This is how to let SULPHA debug MultiStream..See SULPHA Documentation for more details.
//	Sulpha_SetupRegister(0,(void*)&StreamBuffer[0],2,sizeof(StreamBuffer[0]));

	SOUND_DisplayStats();

	return(i);
}





/**********************************************************************************
SOUND_CloseIOPResources
	Free hardware timer
	Deletes Sema used by StreamADPCM
	Deletes Thread which StreamADPCM is attached to.

	Requires:	Nothing
	Returns:	Nothing

	Notes: Calling this will then free all timer and thread releated resources
	used by MultiStream IOP module.
***********************************************************************************/
void SOUND_CloseIOPResources(void)
{

#if (SCREAM_COMPATIBLE)
	if(g_SPURightsSema != -1)
	{
		snd_SetSPUSema(-1);
		DeleteSema(g_SPURightsSema);
		g_SPURightsSema = -1;
	}
	if(g_SPUDMADoneSema != -1)
	{
		snd_SetExternSPUMemTransFunc(NULL, -1);
		DeleteSema(g_SPUDMADoneSema);
		g_SPUDMADoneSema = -1;
	}
#endif
	
	if (timer.timer_id>0)
	{
		FreeHardTimer(timer.timer_id);
		timer.timer_id=0;
	}

#if MS_TIMER
	if (gSem!=0)
	{
		DeleteSema(gSem);
		gSem=0;
	}
#endif
	if (gThid!=0)
	{
		TerminateThread(gThid);
		DeleteThread(gThid);
		gThid=0;
	}
}

/**********************************************************************************
GetFileID
	Looks through Filename table for corresponding ID.
	Requires:	ID 		ID from FileNames.H
	Returns:	index into filelist
***********************************************************************************/
int GetFileID(int ID)
{
int a;
ADPCMFILE *File;

	File=JADPCM;

	for (a=0;a<MAX_ADPCM_FILES;a++)
	{
		if (File->ID==ID)		/* Is this the File ID we are looking for? */
			return(a);			/* Yes, return offset info File structure for correct info */
		File++;
	}
	return(-1);	 				/* ID not found in list */
}

/**************************************************************************/
/* Create thread that gets called by callback function					  */
/**************************************************************************/
int makeMyThread(u_int priority)
{
struct ThreadParam param;
int	thid;

    param.attr         = TH_C;
    param.entry        = StreamADPCM;		/* routine which does just about everything.. */
    param.initPriority = (int)priority;	
    param.stackSize    = 0x400*16;			/* Stack size = 16K */
    param.option = 0;
	thid = CreateThread(&param);
	return thid;
}

/**********************************************************************************
StopStream
	Stops a stream, keys off channel (or stops PCM playback)

	Requires:	Ch			STREAM Channel number ONLY (no longer takes SPU CHANNEL)
				KeyOffFlag	0 = Perform SPU key off
							1 = Don't perform SPU key off.

	Note:	KeyOffFlag may=1 if you want to play a stream which will start playing when
	the current SPU channel data ends (see (EE) SOUND_PlayStreamFromSFX for more details)
***********************************************************************************/
void StopStream(int Ch,int KeyOffFlag)
{
int SPUChan;
unsigned int sch,sch2;
unsigned short core;
int core2=-1;
unsigned int skoff;
int i,a;

	if (StreamBuffer[Ch].Status!=STREAM_STATUS_OFF)		/* Is the stream active? */
	{
		if (IOPRequestLoad==Ch)	/* Is this stream currently requesting data to be loaded? */
		{
			IOPRequestLoad=-1;	/* Yes..So stop it. */
			StreamBusy=0;
		}

		SPUChan=SOUND_GetStreamSPUChan(Ch);

		if ((SOUND_SPUTransfer.Status==STREAM_STATUS_ON)&&(SOUND_SPUTransfer.Stream==Ch))	// MS7.2
		{
	    	SOUND_SPUTransfer.Status=STREAM_STATUS_OFF;
		}

		StreamBuffer[Ch].AppendFlag=0;
		StreamBuffer[Ch].NextLoad=0;
		StreamBuffer[Ch].NextFileID_FOAPS=0;

		StreamBuffer[Ch].DMAInfo.Status[0]=0;
		StreamBuffer[Ch].DMAInfo.Status[1]=0;

		StreamBuffer[Ch].Chld=0;
		StreamBuffer[Ch].ChldWait=0;
//		SOUND_InitLoadRequest(Ch);

		StreamBuffer[Ch].Status=STREAM_STATUS_OFF;		/* Set stream status flag */
		StreamBuffer[Ch].ParentChildFlag=0;
		StreamBuffer[Ch].ParentStream=-1;
		StreamBuffer[Ch].iStatus=iSTATUS_WAIT;
		StreamBuffer[Ch].IntKeyOn=0;
		StreamBuffer[Ch].AllowKON=0;
		StreamBuffer[Ch].Error=0;
		StreamBuffer[Ch].SamplesPlayed=0;

		if (StreamBuffer[Ch].ResidentFlag==STREAM_AUDIO_ADPCM)
		{
			SPU_LoopCounter[SPUChan]=0;						/* Clear Loop counter */

			if (KeyOffFlag==0)
			{
				SPU_CDError[SPUChan]=0;
/* Set pitch to 0 instead of Volume - reduces clicking */
				SOUND_SetChannelPitch(SPUChan,0);
				sch=(unsigned int)SOUND_GetSPUChannel(SPUChan);
				core=(unsigned short)SOUND_GetCore(SPUChan);
				skoff=(unsigned int)(1<<sch);		/* Bitmask value to use when keying off. */

/* Is this stream using two or more SPU channels? */
				if (StreamBuffer[Ch].Ch2Flag!=0)
				{
/* Yes it is.. */
					for (i=0;i<StreamBuffer[Ch].Ch2Flag;i++)
					{
						a=StreamBuffer[Ch].SPUChannel2[i];
						SPU_KeyStatus[a]=SPU_KEY_STATUS_OFF;
						sch2=(unsigned int)SOUND_GetSPUChannel(a);
						core2=SOUND_GetCore(a);

						GET_SPU_RIGHTS();
						sceSdSetSwitch ((unsigned short)(core2 | SD_S_KOFF), (unsigned int)(1<<sch2));
						sceSdSetParam ((unsigned short)(core2 | (sch2<<1) | SD_VP_ADSR2), 0x1fc0);
						SOUND_DelayKeyOnOff();	/* Cause a delay to allow KeyOff to process */
						RELEASE_SPU_RIGHTS();
					}
					StreamBuffer[Ch].Ch2Flag=0;
				}
/* Key OFF SPU Channel(s) */
				GET_SPU_RIGHTS();
				sceSdSetSwitch ((unsigned short)(core | SD_S_KOFF), (skoff));
				sceSdSetParam ((unsigned short)(core | (sch<<1) | SD_VP_ADSR2), 0x1fc0);
				SPU_KeyStatus[SPUChan]=SPU_KEY_STATUS_OFF;
				SOUND_DelayKeyOnOff();	/* Cause a delay to allow KeyOff to process */
				RELEASE_SPU_RIGHTS();
			}
		}
		else if (StreamBuffer[Ch].ResidentFlag==STREAM_AUDIO_PCM)
		{
			SOUND_StopPCM();	/* If stream is playing PCM data, stop it.. */
		}
	}
}

/**********************************************************************************
SOUND_PlayStream
	This routine sets up the stream to be played and kicks it off.
	It copies values into a required Stream structure. These values are then copied
	into the actual "used" positions in the structure by the routine: StartStream.
	I don't copy the values here, becuase if a stream is already playing, then
	modifying these values could cause strange things to happen (as all of the stream
	parameters are updated via interrupts). As StartStream is called in the interrupt
	itself, it is safe to assume that I can modify the values there without causing
	problems.

Requires:
	SPUChannel	=	hi=Stream number, lo=Channel to play ADPCM stream on (0-47)
	Volume		=	Channel volume (currently copied \into both Left&Right)
	IOPAddress	=	Address of IOP Buffer - see below
	StreamID	=	ID for lookup table containing filenames (ie. 0 = "host0:crowdambience.VB")
	LoopInfo	=	Looping/OnceOff
	AR			=	Attack/Release

Returns:
	Stream channel number (NOT SPU Channel - but the stream data channel...)
	-1 = failed due to file not opened
	-2 = No spare streams (all currently playing/paused)
***********************************************************************************/

int SOUND_PlayStream(int SPUChannel,int VolL,int VolR,int FileID, int Pitch, int LoopInfo,int AR)
{
int S;
int a;
int FileIndex;
char KeyOffFlag;
int LInfo;

//printf("--------------------------------------------------\n");
//printf("PLAYSTREAM %x (%x)\n",SPUChannel,FileID);
//printf("----------------- ---------------------------------\n");

	if (FILE_COUNTER==0)	/* Can't play a stream if no file info is created */
	{
		MSERROR((">>>ERROR: SOUND_PlayStream\n"));
		MSERROR(("No File IDs stored\n"));
		return(-1);
	}

	S=SPUChannel>>8;		/* S = Stream channel */
	S&=0xff;
	SPUChannel&=0xff;

/* KeyOffFlag = bit15 of the Attack and Release */
/* 1=Don't stop any audio from required SPU channel (see SOUND_PlayStreamFromSFX) */
	KeyOffFlag=(char)(AR>>15);
	KeyOffFlag&=1;

	AR&=0x7fff;		/* Mask off KeyOffFlag, to leave Attack and Release values */

	if (StreamBuffer[S].MemoryAllocated==0)	/* Is the stream buffer allocated? */
	{
		MSERROR(("ERROR: SOUND_PlayStream\n"));
		MSERROR(("Requested stream channel (%d) Not allocated\n",S));
		return(-2);
	}

	a=SOUND_CheckStream(S);		/* Check is stream is valid (in range) */
	if (a!=0)
	{
		MSERROR(("ERROR: SOUND_PlayStream\n"));
		MSERROR(("Requested stream channel (%d) out of range.\n",S));
		return(-2);
	}

	SOUND_StopStream(S,KeyOffFlag);	/* Stop any stream currently playing on this strean channel) */

	LInfo=LoopInfo;					/* LInfo = Playback type */
	if (LInfo==STREAM_RESIDENT)		/* To be stored in SPU RAM for playback as a SFX? */
	{
		StreamBuffer[S].ResidentFlag=STREAM_LOAD_SPU;
	}
	else if ((LInfo==STREAM_LOAD_EE)||(LInfo==STREAM_LOAD_EE_LOOP))	/* To be transferred to EE RAM? */
	{
		SPUChannel=0;
		StreamBuffer[S].ResidentFlag=STREAM_LOAD_EE;
		StreamBuffer[S].SPUAddress=StreamEEAddress;
		StreamEEFlag=0;
		if (LInfo==STREAM_LOAD_EE_LOOP)
			LoopInfo=STREAM_LOOPING;
	}
	else if (LInfo==STREAM_LOAD_IOP)	/* To be transferred to a specific IOP RAM address? */
	{
		StreamBuffer[S].ResidentFlag=STREAM_LOAD_IOP;
	}
	else if (LInfo==STREAM_PCM_LOOPING)	/* Looping PCM audio stream? */
	{
		StreamBuffer[S].ResidentFlag=STREAM_AUDIO_PCM;
		PCMStream=S;
		LoopInfo=STREAM_LOOPING;
	}
	else if (LInfo==STREAM_PCM_ONCE_OFF)	/* Once off PCM audio stream? */
	{
		StreamBuffer[S].ResidentFlag=STREAM_AUDIO_PCM;
		PCMStream=S;
		LoopInfo=STREAM_ONCE_OFF;
	}
	else
		StreamBuffer[S].ResidentFlag=STREAM_AUDIO_ADPCM;	/* Must be an ADPCM stream (looping or once off) */

//printf("RESIDENT %x %x\n",S,StreamBuffer[S].ResidentFlag);

	StreamBuffer[S].iStatus=iSTATUS_INITIALISED;	/* Set internal MultiStream status */

	StreamBuffer[S].KeyOffFlag=KeyOffFlag;

	StreamBuffer[S].ChainCount=0;					/* Clear Chain counter */
	StreamBuffer[S].USR_SPUChannel=SPUChannel;		/* Copy values to _USR (these will be copied to */

/* Set to use 1st block of MIB data (or base address of non-MIB data) */
	StreamBuffer[S].IntTransOffset=0;

	if ((LInfo!=STREAM_LOAD_EE)&&(LInfo!=STREAM_LOAD_IOP)&&(LInfo!=STREAM_RESIDENT))	/* New! Only set volumes if an audio stream */
	{
		SPU_VolL[SPUChannel]=(short)VolL;			/* Copy volumes */
		SPU_VolR[SPUChannel]=(short)VolR;
	}

	StreamBuffer[S].USR_StrID=FileID;
	StreamBuffer[S].USR_LInfo=LoopInfo;
	StreamBuffer[S].USR_AR=AR;			/* AR = Attack and Release. */

	if (StreamBuffer[S].ResidentFlag==STREAM_AUDIO_ADPCM)
	{
/* SOUND_PlayStream called (rather than SOUND_PlayStreamFromSFX)? */
		if (KeyOffFlag==0)
		{
/* Yes, so store required pitch */
			StreamBuffer[S].Pitch=(short)Pitch; 
			SPU_PausePitch[SPUChannel]=(unsigned short)Pitch;
		}
		else
		{
/* No, so stream pitch = current SPU channel pitch */
			StreamBuffer[S].Pitch=(short)SPU_PausePitch[SPUChannel];
		}
	}

/* Init info for playback of Interleaved data. */
	StreamBuffer[S].MIBEnd=0;	/* Clear flag to say that its the last packet before looping */
	StreamBuffer[S].MIBLoop=0;	/* Clear offset through data for setting loop marker */

	StreamBuffer[S].ParentChildFlag=0;	/* Set stream to not be either a Parent or Child */
	StreamBuffer[S].TransCounter=0;		/* Clear transfer counter */
	StreamBuffer[S].ParentStream=-1;	/* Clear value of streams parent (it hasn't got one yet..) */
	StreamBuffer[S].IntKeyOn=0;			/* MIB key on flag (only key on if all chans are ready) */
	StreamBuffer[S].AllowKON=0;			/* Clear AllowKeyOn flag (see SOUND_StreamDisableKeyOn) */
	StreamBuffer[S].Error=0;			/* Stream CD error pause flag */
	StreamBuffer[S].PlayOffset=0;		/* Clear current offset though data played */

	StreamBuffer[S].IOPBufferHalfSize=StreamBuffer[S].AllocHalfSize;

/* Get index through File structure for corresponding FileID */
	FileIndex=GetFileID(FileID);
	if (FileIndex<0)
	{
		MSERROR((">>>>> ERROR: SOUND_PlayStream\n"));
		MSERROR(("File ID (0x%x) not found\n",FileID));
		StreamBuffer[S].Status=STREAM_STATUS_OFF;		/* File ID not found. */
		return(-1);
	}

/* Now open the file (actually just copies its sector start/filesize/start offset...) */
/* Load file info into temp area, so they can be copied during interrupt call (StartStream) */
/* Its safer this way, as the "real" parameters may be in use during this call. */
	a=OpenADPCMFile(S,FileIndex,STREAM_LOAD_TO_TEMP);
	if (a!=0)	/* ERROR Opening File ? */
	{
		MSERROR((">>>>> ERROR: SOUND_PlayStream\n"));
		MSERROR(("Error opening file\n"));
		StreamBuffer[S].Status=STREAM_STATUS_OFF;
		return(-1);
	}

/* If playing an ADPCM stream, mark requested SPU channel as busy. */
/* You can't just use the current "real" SPU channel status for this, as the keyon of the SPU */
/* channel won't happen until data is preloaded. */
	if (StreamBuffer[S].ResidentFlag==STREAM_AUDIO_ADPCM)
	{
		SPU_KeyStatus[SPUChannel]=SPU_KEY_STATUS_USED_BY_STREAM;	/* NEW!! 25/7/2001 */
		SPU_LoopCounter[SPUChannel]=0;			// MS7.2 - Thanks Y_Kousu
		SOUND_PauseStreamCheck[SPUChannel]=0;		/* Clear pause flag */
	}

/* As default, if a VAG header is read at the start of loading, its pitch is used as the streams pitch */
	StreamBuffer[S].VAGPitch=SOUND_VAG_PITCH_ON;

	StreamBuffer[S].LoadSource=(char)EXTERNAL_CDLOAD;	/* Use default loading code */
	StreamBuffer[S].Ch2Flag=0;					/* Mark as not using a second SPU channel */
/*	StreamBuffer[S].TotalEETranSize=0; */
	StreamBuffer[S].EEAllow=0;

	StreamBuffer[S].Priority=SOUND_PRIORITY_INIT;
	StreamBuffer[S].PriorityRequest=0;
	StreamBuffer[S].Status=STREAM_STATUS_ON;	/* Set stream channel to ON */

	return(S);
}

/**********************************************************************************
StartStream
	Starts a stream, copying the values from USR_ (set up from SOUND_PlayStream) and
	sets its file size/start position/destination.
	StartStream can ONLY be called by StreamADPCM (via interrupt). If this function
	is called, we know that the system is up and running. And also that no cd error
	has occured.
***********************************************************************************/
int StartStream(int Stream,int SPUChannel,int FileID, int LoopInfo,int AR)
{
int IOPAddress;
int a;
int FileIndex;
int b=0;
int VolL,VolR;


	if ((StreamBuffer[Stream].ResidentFlag!=STREAM_AUDIO_ADPCM)&&
		(StreamBuffer[Stream].ResidentFlag!=STREAM_AUDIO_PCM))		/* Not an audio stream? */
	{
		if (StreamBuffer[Stream].ResidentFlag==STREAM_LOAD_SPU)	/* Loading into SPU RAM as a SFX? */
		{
			
			StreamBuffer[Stream].SPUAddress=StreamMemoryEnd;	/* Set address to load data to. */
			StreamMemoryEndFlag=0;
		}
	}
	else if (StreamBuffer[Stream].ResidentFlag==STREAM_AUDIO_PCM)	/* Playing PCM data? */
	{
		if (PCMBuffer==0)	/* Have you allocated a PCM buffer? */
		{
			MSERROR((">>>>> ERROR: StartStream\n"));
			MSERROR(("No PCM Buffer allocated\n"));
			StreamBuffer[Stream].Status=STREAM_STATUS_OFF;
			return(-1);
		}
		else
			StreamBuffer[Stream].SPUAddress=PCMBuffer;	/* Set PCM address */
	}
	else
	{
/* Is this stream NOT a child? (Child SPU address is set in the SOUND_SetStreamChild_Int routine) */
		if (StreamBuffer[Stream].ParentChildFlag!=SOUND_CHILD)
		{
			StreamBuffer[Stream].SPUAddress=StreamBuffer[Stream].AllocSPUAddress;	/* SPU stream address */
			StreamBuffer[Stream].IOPAddress=StreamBuffer[Stream].IOPAllocAddress;	// 7.2 Set Stream IOP Address
		}
	}

	if (StreamBuffer[Stream].ResidentFlag==STREAM_LOAD_IOP)	/* Loading directly to IOP RAM? */
	{
		IOPAddress=(int)StreamIOPAddress;	/* Set IOP Address */
		StreamIOPFlag=0;
		if (IOPAddress==0)
		{
			MSERROR((">>>>> ERROR: StartStream\n"));
			MSERROR(("IOP Write address not set\n"));
			MSERROR(("Call function: SOUND_SetIOPWriteAddress\n"));
			StreamBuffer[Stream].Status=STREAM_STATUS_OFF;
			return(-1);
		}
	}
	else
	{
		IOPAddress=(int)StreamBuffer[Stream].IOPAddress;			/* Address of IOP Buffer for stream */
		if (IOPAddress==0)
		{
			MSERROR((">>>>> ERROR: StartStream\n"));
			MSERROR(("StreamBuffer 0x%x NOT allocated\n",Stream));
			MSERROR(("Call function: SOUND_AllocateStreamBuffer\n"));
			StreamBuffer[Stream].Status=STREAM_STATUS_OFF;
			return(-1);
		}
	}

	StreamBuffer[Stream].IOPBufferSize=StreamBuffer[Stream].AllocSize;	/* Size of IOP Buffer */
	StreamBuffer[Stream].FileID=FileID;	/* ID of File to play */
	FileIndex=GetFileID(FileID);		/* Returns index through ADPCM_FileNames for corresponding FileID */
	if (FileIndex<0)
	{
		MSERROR((">>>>> ERROR: StartStream\n"));
		MSERROR(("File ID (0x%x) not found\n",FileID));
		StreamBuffer[Stream].Status=STREAM_STATUS_OFF;
		return(-1);
	}

	StreamBuffer[Stream].AR=AR;
	StreamBuffer[Stream].PlayingHalf=STREAM_FIRST_HALF;

	VolL=SPU_VolL[SPUChannel];	/*=VolL; */
	VolR=SPU_VolR[SPUChannel];	/*=VolR; */

	if (StreamBuffer[Stream].ResidentFlag==STREAM_AUDIO_PCM)
	{
		PCMVolL=VolL;
		PCMVolR=VolR;
	}

	StreamBuffer[Stream].VolL=(short)VolL;
	StreamBuffer[Stream].VolR=(short)VolR;

	StreamBuffer[Stream].TranSize=0;
	StreamBuffer[Stream].TotalTranSize=0;

	StreamBuffer[Stream].IOPCurrentAddress=(unsigned int)IOPAddress;

	StreamBuffer[Stream].SPUChannel=(unsigned char)SPUChannel;	/* Which SPU channel to play stream on. */
	StreamBuffer[Stream].IOPCurrentRemainder=StreamBuffer[Stream].AllocSize;
	StreamBuffer[Stream].LoopInfo=(unsigned char)LoopInfo;	/* User has specified either looping or once off.. */

	StreamBuffer[Stream].core0=0;
	StreamBuffer[Stream].core1=0;

	StreamBuffer[Stream].SPUCurrent=0;
	StreamBuffer[Stream].NoMoreLoading=0;

/* Now open the file (actually just copies its sector start/filesize/start offset...) */
	a=OpenADPCMFile(Stream,FileIndex,STREAM_GET_OTHER_INFO);

	if (a!=0)	/* ERROR Opening File ? */
	{
		MSERROR((">>>>> ERROR: StartStream\n"));
		MSERROR(("Error opening file\n"));
		StreamBuffer[Stream].Status=STREAM_STATUS_OFF;
		return(-1);
	}

/* Copy values set up from OpenADPCMFile called from SOUND_PlayStream */
/* I don't just store the values in SOUND_PlayStream as its safer to set things like these in the interrupt callback */
/* (Just to make sure that any CD/SPU DMA callbacks don't start using strange values) */
/* Also, by doing the OpenADPCMFile in SOUND_PlayStream, it is possible to also use the SOUND_SetFileOffsetAndPlaySize */
/* function between each SOUND_PlayStream call, so its possible to play multiple streams from a single file (if they */
/* are packed together in a .MSB file for example) */

	StreamBuffer[Stream].FileSeek=StreamBuffer[Stream].TempSeek;	/* Offset into file to start from.. */
	StreamBuffer[Stream].FileStart=StreamBuffer[Stream].TempSeek;	/* Offset to loop to */
	StreamBuffer[Stream].FilePlaySize=StreamBuffer[Stream].TempPlaySize;	/* Size to play */
	StreamBuffer[Stream].FileRemainder=StreamBuffer[Stream].TempPlaySize;	/* Amount left to play */
	StreamBuffer[Stream].FileSize=StreamBuffer[Stream].TempFileSize;	/* Total file size */

/* Add play offset to the file start position. */
	StreamBuffer[Stream].FileRemainder-=StreamBuffer[Stream].PlayOffset;
	StreamBuffer[Stream].FileSeek+=StreamBuffer[Stream].PlayOffset;

	if ((StreamBuffer[Stream].PlayOffset!=0)&&(StreamBuffer[Stream].LoopInfo==STREAM_LOOPING))
	{
		if (StreamBuffer[Stream].PlayOffset>=(StreamBuffer[Stream].FilePlaySize-StreamBuffer[Stream].AllocSize))
		{
			MSERROR((">>>>> ERROR: StartStream (due to SOUND_SetStreamStartPosition and LOOPING)\n"));
			MSERROR(("PlayOffset Start position (0x%x) is too near (or past) the end of the stream\n",StreamBuffer[Stream].PlayOffset));
			MSERROR(("You need more than one stream buffers worth of data to play correctly\n"));
			MSERROR(("Stream Buffer %d Size = 0x%x\n",Stream,StreamBuffer[Stream].AllocSize));
			MSERROR(("Stream length = 0x%x\n",StreamBuffer[Stream].FilePlaySize));
			MSERROR(("Valid Play offset (via SOUND_SetStreamStartPositon) = 0-0x%x\n",(StreamBuffer[Stream].FilePlaySize-StreamBuffer[Stream].AllocSize-1)&~15));
			SOUND_StopStream(Stream,0);
			return(-1);
		}
	}

	StreamBuffer[Stream].EEAllow=1;

/* Note: ChainCount set to 0 in SOUND_PlayStream */
	StreamBuffer[Stream].ChainPos=0;	/* Start from first position in chain list */
/* Copy this stream file info into first slot of chain info.. */
	StreamBuffer[Stream].ChainList[0]=FileID;
	StreamBuffer[Stream].ChainFileSeek[0]=StreamBuffer[Stream].FileSeek;
	StreamBuffer[Stream].ChainPlaySize[0]=StreamBuffer[Stream].FileRemainder;
	StreamBuffer[Stream].ChainFileSize[0]=StreamBuffer[Stream].FileSize;

//printf("start loading stream %x (size %x)\n",Stream,StreamBuffer[Stream].FileSize);
//printf("streaming to address %x\n",StreamBuffer[Stream].IOPCurrentAddress);

/* If streaming ADPCM audio, check to make sure SPU buffer area is valid */
	if (StreamBuffer[Stream].ResidentFlag==STREAM_AUDIO_ADPCM)
		b=SOUND_CheckSPUMemory(StreamBuffer[Stream].SPUAddress,StreamBuffer[Stream].SPUBufferSize);	/*AllocSize); */
	else if (StreamBuffer[Stream].ResidentFlag==STREAM_LOAD_SPU)
	{
/* If streaming data into SPU RAM to be played as a SFX, check SPU RAM area is valid */
		b=SOUND_CheckSPUMemory(StreamBuffer[Stream].SPUAddress,StreamBuffer[Stream].FilePlaySize);	/*FIX!! 4/Feb/2002 */
	}
	if (b==-1)
	{
		MSERROR((">>>>> ERROR: StartStream\n"));
		MSERROR(("SPU Memory Error\n"));
		StreamBuffer[Stream].Status=STREAM_STATUS_OFF;	/* SPU RAM error. Stop stream. */
		return(-1);
	}

	SOUND_SetCalcPlayingTime(Stream,0,0);	// Initialise playing time counters

	StreamBuffer[Stream].LastSPUAddr=StreamBuffer[Stream].SPUAddress;
	StreamBuffer[Stream].SamplesPlayed=0;

	StreamBuffer[Stream].FastLoadCounter=0;

	if (StreamBuffer[Stream].ParentChildFlag!=SOUND_CHILD)
		StreamBuffer[Stream].LoadSource2=StreamBuffer[Stream].LoadSource;
	else
		StreamBuffer[Stream].LoadSource2=StreamBuffer[StreamBuffer[Stream].ParentStream].LoadSource;
/*MS6 */
#if (MS_LOAD62!=0)
	SOUND_InitLoadRequest(Stream);	/* Initialise SPU transfer management info */
#endif

	if (StreamBuffer[Stream].PriorityRequest==0)		/* No User priority request made */
	{
		if ((StreamBuffer[Stream].ResidentFlag!=STREAM_AUDIO_ADPCM)&&
			(StreamBuffer[Stream].ResidentFlag!=STREAM_AUDIO_PCM))		/* Not an audio stream? */
		{
			StreamBuffer[Stream].Priority=SOUND_PRIORITY_DATA;
		}
		else
		{
			StreamBuffer[Stream].Priority=SOUND_PRIORITY_AUDIO;
		}

		StreamBuffer[Stream].Priority2=0xffffffff;
	}

	return(Stream);
}

/**********************************************************************************
SOUND_AppendStream
	Appends another File onto a currently playing stream.
	Requires:	i		Stream channel
				FileID	FileID of file to stream
				flag	SOUND_APPEND_NOW = Swap to new file straight away
						SOUND_APPEND_AT_LOOP = Swap to new file when current playback reaches end
	Note, due to the buffering of the audio data, you may have to wait a while
  #	before your new audio file actually starts playing.
***********************************************************************************/


int SOUND_AppendStream(int i, int FileID, int flag)
{
int a;
int FileIndex;

	if (StreamBuffer[i].Status!=STREAM_STATUS_ON)	/* Stream not playing, can't append stream. */
	{
		MSERROR((">>>>> ERROR: SOUND_AppendStream\n"));
		MSERROR(("Can not append file. Stream not playing\n"));
		return(-1);
	}

	FileIndex=GetFileID(FileID);		/* Returns index through ADPCM_FileNames for corresponding FileID */
	if (FileIndex<0)
	{
		MSERROR((">>>>> ERROR: SOUND_AppendStream\n"));
		MSERROR(("File ID (0x%x) not found\n",FileID));
		return(-1);
	}

	a=OpenADPCMFile(i,FileIndex,STREAM_GET_APPEND_INFO);
	if (a!=0)	/* ERROR Opening File ? */
	{
		MSERROR((">>>>> ERROR: SOUND_AppendStream\n"));
		MSERROR(("Error opening file (ID 0x%x)\n",FileID));
		return(-1);
	}
//	OpenADPCMFile(i,FileIndex,STREAM_GET_OTHER_INFO);

/* These values will now be used when the current stream loops.. */

	if (flag==SOUND_APPEND_AT_LOOP)	// MS 7.21 - Thanks Y_Kousu!!
	{
		StreamBuffer[i].AP_FileID=FileID;	/* ID of File to play */
		StreamBuffer[i].AppendFlag=1;
	}

/* These values will force the new file to be streamed straight away.. */

	else if (flag==SOUND_APPEND_NOW)
	{
		StreamBuffer[i].FileID=FileID;	/* ID of File to play */
		StreamBuffer[i].FileSeek=StreamBuffer[i].AP_FileStart;
		StreamBuffer[i].FileRemainder=StreamBuffer[i].AP_FilePlaySize;
		StreamBuffer[i].FileSize=StreamBuffer[i].AP_FileSize;

		StreamBuffer[i].ChainPos=0;
		StreamBuffer[i].ChainList[0]=FileID;
		StreamBuffer[i].ChainFileSeek[0]=StreamBuffer[i].FileSeek;			/* V5 */
		StreamBuffer[i].ChainPlaySize[0]=StreamBuffer[i].FileRemainder;	/* NEW! 30/7/2001 */
		StreamBuffer[i].ChainFileSize[0]=StreamBuffer[i].FileSize;
	}
	else
	{
		MSERROR((">>>>> ERROR: SOUND_AppendStream\n"));
		MSERROR(("Flag (0x%x) not valid\n",flag));
		MSERROR(("Flag must be SOUND_APPEND_NOW (%d) or SOUND_APPEND_AT_LOOP (%d)\n",SOUND_APPEND_NOW,SOUND_APPEND_AT_LOOP));
		return(-1);
	}
	return(0);
}

/**********************************************************************************
SOUND_SetStreamStartPosition
	Sets the playback start position though a file.
	The offset passed is added to the offset which was passed to
	SOUND_SetFileOffsetAndPlaySize or SOUND_CreateFileInfo.
	This allows you to play back audio from a specific point through a file.
	Note that for ADPCM playback, the offset has to be a multiple of 16.

	Requires:	i		Stream number
				offset	Offset (in bytes) through file to start playing
	
***********************************************************************************/
void SOUND_SetStreamStartPosition(int i, u_int offset)
{
	if (StreamBuffer[i].Status!=STREAM_STATUS_ON)
	{
		MSERROR((">>>>> ERROR: SOUND_SetStreamStartPosition\n"));
		MSERROR(("Can not set offset for stream 0x%x. Stream not playing\n",i));
		return;
	}
	if (StreamBuffer[i].ParentChildFlag==SOUND_CHILD)
	{
		MSERROR((">>>>> ERROR: SOUND_SetStreamStartPosition\n"));
		MSERROR(("Can only set the start position of a Parent or single stream\n"));
		MSERROR(("As a CHILD does not handle any loading\n"));
		return;
	}

	if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)
		offset&=(unsigned int)(~15);

	StreamBuffer[i].PlayOffset=offset;	/* Set offset through stream to start playing from */
}

/**********************************************************************************
setTimer - IRQ Interrupt
***********************************************************************************/
#if MS_TIMER
void setTimer( TimerCtx* timer )
{
struct SysClock clock;
int timer_id;

	USec2SysClock (ONE_HSCAN, & clock);
    timer->count = (int)clock.low;	/* within 32-bit */

    if ((timer_id = AllocHardTimer (TC_SYSCLOCK, 32, 1)) <= 0)
    {
		MSERROR(("Can NOT allocate hard timer ...\n"));
		return;
    }

    timer->timer_id = timer_id;

    if (SetTimerHandler (timer_id, (unsigned long)timer->count,(void *)TimerFunc, (void *) timer) != KE_OK)
	{
		MSERROR(("Can NOT set timeup timer handler ...\n"));
		return;
    }
    if (SetupHardTimer (timer_id, TC_SYSCLOCK, TM_NO_GATE, 1) != KE_OK)
    {
		MSERROR(("Can NOT setup hard timer ...\n"));
		return;
    }
}
#endif
/**********************************************************************************
startTimer - IRQ Interrupt
***********************************************************************************/
#if MS_TIMER
void startTimer( TimerCtx* timer )
{
    if (StartHardTimer (timer->timer_id) != KE_OK)
	{
		MSERROR(("Can NOT start hard timer ...\n"));
	}
}
#endif
/**********************************************************************************
TimerFunc - IRQ Interrupt
Called, say, once per frame. The quicker its called, the faster you
will know when streams need new data.
This interrupt signals a Sema, which the function StreamADPCM is waiting for..
***********************************************************************************/
#if MS_TIMER

int TimerFunc( void *common)
{
    TimerCtx *tc = (TimerCtx *) common;

#if (MS_SCANLINE_DEBUG==0)
		SOUND_TimerUpdate=1;
		iSignalSema(gSem);	/* Signal StreamADPCM to be called */
#else
	JP_IntCount++;
	JP_IntCount2++;
	JP_IntCount3++;
	if (JP_IntCount3==312)
	{
		JP_IntCount3=0;
		SOUND_TimerUpdate=1;
		iSignalSema(gSem);	/* Signal StreamADPCM to be called */
	}
#endif
    return (tc->count);
}
#endif
/**********************************************************************************
DMA Interrupt - called when IOP->SPU DMA transfer is completed
***********************************************************************************/
int _AdpcmDmaInt (int ch, void *common)
{
int *c = (int *)common;

(void)ch;

    (*c) =0;		/* Set SPU_DMA_TRANS_FLAG=0 */

#if MS_TIMER
		iSignalSema(gSem);	/* Signal StreamADPCM to be called */
#endif
    return (0);
}

/**********************************************************************************
Originally Phils code..Butchered by Jason(hehe!)

_CdStreamStart

	Requires:	startsec	Start sector
				readptr		Position through sector for first byte of data
				writeptr	Address to write data to
				playlen		Length of data to play in file
				filelen		Total length of file
				size		Size of data to read
				i			Stream number
				endfunc		Function to call when loading is completed.

***********************************************************************************/
int	_CdStreamStart(int startsec, int readptr, int writeptr,	int filelen, int size, int i,cdFunc endfunc)
{
u_char *filename;
int a;
ADPCMFILE *File;

	if (cd_stage!=CD_STAGE_DONE)	/* CD is busy loading other data */
		return (0);

 	cd_stream=i;
	cd_startsec=startsec;
	cd_readptr=readptr;
	cd_writeptr=writeptr;
	cd_filelen=filelen;
	cd_datalen=size;
	cd_jdatalen=(unsigned int)size;
	cd_readlen=0;
	cd_stage=CD_STAGE_SEEK;
	cd_localcopy=0;
	cd_localstart=0;
	cd_localstart2=0;
	cd_func=endfunc;
	cd_source=StreamBuffer[cd_stream].Source;	/* host0: or cdrom0: */

	StreamBuffer[cd_stream].LastLoadAddress=(unsigned int)cd_writeptr;

	if (StreamBuffer[cd_stream].LoadSource2==USE_EXTERNAL_LOADING)
	{
		CD_EVENT_OCCURED=1;	/* This will force another callback due to there being no callbacks from host loading. */
		CD_IDLE=0;
		return(1);
	}

	if (cd_source==ADPCM_DATA_ON_HOST)	/* Data from host0: */
	{
		a=GetFileID(StreamBuffer[cd_stream].FileID);
		File=JADPCM;
		File+=a;

		StreamBuffer[cd_stream].ATWinFile=(u_int)a;	// NEW!! For Morgan

		if (File->OpenFlag!=0)
		{
			StreamBuffer[cd_stream].SectorStart=File->OpenID;
		}
		else
		{
			filename=File->FileName;
			StreamBuffer[cd_stream].SectorStart=(unsigned int)open (filename, O_RDONLY);	/* Open new file */
		}

		cd_startsec=(int)StreamBuffer[cd_stream].SectorStart;
		CD_EVENT_OCCURED=1;	/* This will force another callback due to there being no callbacks from host loading. */
		CD_IDLE=0;
		SOUND_HandleCDEvent();
	}
	else
	{
		CD_EVENT_OCCURED=1;	/* This will force another callback due to there being no callbacks from host loading. */
		CD_IDLE=0;
		SOUND_HandleCDEvent();
	}
	return 1;
}

/**********************************************************************************
_CdStreamInt

   This gets called by the callback once the non-blocking CD operation completes,
   It just sets the CD_EVENT_OCCURED flag to 1 which then triggers a call to
   SOUND_HandleCDEvent the next time the CD_EVENT_OCCURED flag is checked in
   StreamADPCM.  Can't put this code in the callback as there are CD operations in
   there, and if they complete then the callback would call itself and cause all
   sorts of problems...
***********************************************************************************/
int CDCallbackFlag=0;
volatile int CDLoadFlag=0;

int _CdStreamInt ()
{
int k;

	if (CDLoadFlag==0)
	{
		return(0);
	}
	CDLoadFlag=0;

	if (E_CDErrorCheck==1)
	{
		MSERROR(("CALLBACK PROBLEM!\n"));
		return(0);
	}


	if (cd_stage==CD_STAGE_DONE)	/* CD is being used by a non-MultiStream application */
		return (0);

	if ((cd_source!=ADPCM_DATA_ON_HOST)&&(CD_READ_STARTED==1))
	{
		CD_READ_STARTED=0;

		k=sceCdGetError();
		if (k!=0)
		{
			if (k==SCECdErREAD)
			{
				MSERROR(("CD ERROR: READ ERROR: (0x%x)\n",k));
				SOUND_CD_I_ERROR=1;
				CDCallbackFlag=1;
			}
			else if (k==SCECdErTRMOPN)		// Tray opened?
			{
				MSERROR(("CD ERROR: TRAY OPENED (0x%x)\n",k));
				SOUND_CD_I_ERROR=2;
				CDCallbackFlag=1;
			}
			else if (k==SCECdErABRT)		// Read aborted? (Thanks Rich Rayl)
			{
				MSERROR(("CD ERROR: ABORT READ: (0x%x)\n",k));
				SOUND_CD_I_ERROR=1;
				CDCallbackFlag=1;
			}
			else
				SOUND_CD_I_ERROR=0;
		}
		else
		{
			SOUND_CD_I_ERROR=0;
			SOUND_CD_ERRORCODE=0;
		}

		if (SOUND_CD_I_ERROR!=0)
		{
			SOUND_CD_ERRORCOUNT++;
			SOUND_CD_ERRORCOUNT&=0xffff;
			SOUND_CD_ERRORCODE=SOUND_CD_I_ERROR;
		}
		else
			SOUND_CD_ERRORCODE=0;
	}

	if (CDCallbackFlag==0)
	{
		CD_EVENT_OCCURED=1;
	}
	else
	{
		CD_EVENT_OCCURED=0;
	}
	CDCallbackFlag=0;

#if MS_TIMER
 	iSignalSema(gSem);	/* Signal StreamADPCM to be called straight after load is completed. */
#endif

	return (0);
}

/**********************************************************************************
SOUND_FindEndOfData
	Searches through the MIB ADPCM data to find where then last 16 bytes of "real"
	ADPCM data are. This is used for setting the loop marker within the MIB data
	so that there are now NO silent gaps when playing back interleaved looping audio.
	Hoorah.

	Requires:	i		Stream

***********************************************************************************/
void SOUND_FindEndOfData(int i)
{
u_int *ptr;
int c=0;
u_int f;
int a,err;
u_int size;

	if (StreamBuffer[i].MIBEndOffset!=SOUND_MIB_END_NOT_SET)
		f=StreamBuffer[i].MIBEndOffset;
	else
	{
		err=0;
		size=StreamBuffer[i].IntTransSize/16;
/* Set ptr to the current write position - 16 */
		ptr=(u_int*)cd_writeptr-4;	/* 4*4=16..(cos its u_int..you see..) */

/* Loop until we get a non zero value. Also skip a VAG Onceoff value */
		while((*ptr==0)||(*ptr==0x77770007))
		{
			ptr-=4;			/* Move back 16 bytes */
			c++;			/* Increase counter */
			if (c>size)		/* Read through ALL of loaded data? */
			{
				err=1;		/* And no actual data found...Use full size of buffer */
				break;
			}
		};
		f=StreamBuffer[i].IntTransSize;	/* Size of one interleaved track. */
		if (err==0)
		{
			f-=(unsigned int)(c*16);	/*+16; */

			if ((f<StreamBuffer[i].IOPBufferHalfSize/2)&&(StreamBuffer[i].LoopInfo==STREAM_LOOPING))
			{
				MSERROR(("WARNING: SOUND_FindEndOfData\n"));
				MSERROR(("End offset (0x%x)is not >= to half the track size (0x%x)!!\n",f,StreamBuffer[i].IOPBufferHalfSize/2));
				MSERROR(("(TrackSize = 0x%x)\n",StreamBuffer[i].IOPBufferHalfSize));
				MSERROR(("Forcing end offset to be 0x%x\n",StreamBuffer[i].IOPBufferHalfSize/2));
				f=StreamBuffer[i].IOPBufferHalfSize/2;
// Don't store in StreamBuffer[i].MIBEndOffset, as you may be chaining streams..

			}

		}
	}
/*	printf("track size = %x\n",StreamBuffer[i].IntTransSize); */
/*	printf("offset to each file = %x (hopefully)\n",f); */
/*	printf("pointer = %x\n",ptr); */

/* Set both parent and child streams. */

	for (a=0;a<UserMaxStreams;a++)
	{
		if (i==StreamBuffer[a].ParentStream)
		{
			if (StreamBuffer[i].LoopInfo==STREAM_LOOPING)
				StreamBuffer[a].MIBEnd=1;	/* Flag to say that its the last packet before looping */
			else
				StreamBuffer[a].MIBEnd=2;	/* Flag to say that its the last packet before dying */

			StreamBuffer[a].MIBLoop=f;	/* Offset through data for setting loop marker */
/* Start address of data. This is used as a reference to double check that the loop marker is */
/* being set for the correct data. */
			StreamBuffer[a].MIBAddress=StreamBuffer[i].LastLoadAddress;
		}
	}
}

/**********************************************************************************
SOUND_StartCDRead
	Starts a CD read - OR restarts it again if recovering from a disk error.

	Requires:	type		0	= Load data
							!=0	= Reload data due to CD Error
***********************************************************************************/
int SOUND_StartCDRead(char type,u_int u_start,u_int u_size,void *u_buff,sceCdRMode *u_mode)
{
static u_int start=0;
static u_int size=0;
static void *buff;
static sceCdRMode *mode;
int ret;

#if (SCREAM_COMPATIBLE)
	sceCdCallback((void*)_CdStreamInt);
#endif

	if (type==0)	// Load new data section? make copy of LSN / Size etc..Incase of CD error..
	{
		start=u_start;
		size=u_size;
		buff=u_buff;
		mode=u_mode;
	}

	CD_READ_STARTED=1;
	sceCdSync(0x10);		/* Wait for completion of any other CD commands..BLOCKING */

	CDLoadFlag=1;
	ret=sceCdRead(start,size,buff,mode);
	return(ret);
}

/**********************************************************************************
SOUND_HandleCDEvent
	Handle all of the streaming for disk/host...(loading/transfering of sectors..)

DONT USE READ_PTR FOR THE LENGTH. ITS JUST FOR THE SEEKING FOR LOADING
USE A COUNTER (STARTING AT 0) AND COMPARE TO THE LENGTH REQUIRED
***********************************************************************************/
void SOUND_HandleCDEvent(void)
{
int start, end, local=0;
int result=1;

int a,b;
u_char *filename;
ADPCMFILE *File;
u_char cont=0;
int parent,child;
//int i;

	if (CD_EVENT_OCCURED==0)	/* NO CD EVENT OCCURED. */
		return;

	CD_EVENT_OCCURED=0;

	if ((StreamBuffer[cd_stream].Status==STREAM_STATUS_OFF)||(StreamBuffer[cd_stream].iStatus==iSTATUS_INITIALISED))
	{
		CD_IDLE=1;
		StreamBusy=0;
		cd_stage=CD_STAGE_DONE;

		if ((cd_source==ADPCM_DATA_ON_HOST)&&(StreamBuffer[cd_stream].LoadSource2!=USE_EXTERNAL_LOADING))
		{
			File=JADPCM;
			File+=StreamBuffer[cd_stream].ATWinFile;
			if (File->OpenFlag==0)
			{
				close(cd_startsec);
			}
		}
		return;
	}
	switch(cd_stage)
	{
	case CD_STAGE_DONE:

		CD_IDLE=1;
		break;				/*not doing anything more */

	case CD_STAGE_CDERROR:
		MSERROR(("trying to recover from CD error CD_STAGE_CDERROR\n"));
		cd_stage=CD_STAGE_DONE;
		break;

	case CD_STAGE_SEEK:
		cd_stage=CD_STAGE_READ;
		/*break not added intentionally, so that it does a read next */
	case CD_STAGE_READ:


	if (StreamBuffer[cd_stream].NoMoreLoading==1)	// MS7.22 All of file in IOP RAM.. No need to load data
	{
		cd_stage=CD_STAGE_DONE;
		break;
	}


		if (cd_localcopy)
		{
/*first,  if something's been copied to a local buffer then write it out to the destination: */
			memcpy((void*)cd_writeptr, &cd_buffer[cd_localstart], (unsigned int)cd_readlen);
		}

/*otherwise the read would've gone directly into memory so nothing needs to be done. */

		cd_readptr+=cd_readlen;		/*update pointer */
		cd_datalen-=cd_readlen;		/*update amount left to read */
		cd_writeptr+=cd_readlen;

		StreamBuffer[cd_stream].FileRemainder-=(unsigned int)cd_readlen;
		StreamBuffer[cd_stream].FileSeek+=(unsigned int)cd_readlen;

/* Is the IOP buffer full of data now? */

		if (cd_datalen==0)
		{
			if (StreamBuffer[cd_stream].ResidentFlag==STREAM_AUDIO_ADPCM)
			{
				if (cd_readlen!=0)
					_AdpcmSetMarkRESTART((int)cd_stream,(u_int)cd_writeptr,0);	// NOT 5.2 / 6.2 SPECIFIC!!!
				if ((StreamBuffer[cd_stream].FileRemainder==0)||(cd_readptr==cd_filelen))
				{

/* Is this stream part of an interleaved stream (either parent or child)? */
/* If so, calculate the offset to store LOOP information.. */
					parent=StreamBuffer[cd_stream].ParentStream;
					if (parent!=-1)
					{
						SOUND_FindEndOfData(parent);
					}
				}
			}

			cd_stage=CD_STAGE_DONE;
			break;
		}

/*now time to set off another read... */
/*if the current pointer is at the end of the file (or just all read required data), then reset the pointer to the */
/*offset of the file of the file. */

		if ((StreamBuffer[cd_stream].FileRemainder==0)||(cd_readptr==cd_filelen))
		{
			StreamBuffer[cd_stream].ChainPos++;	/* NEW! 13/Aug/2001 - Moved. (Move to next chain pointer) */
			a=StreamBuffer[cd_stream].ChainPos;
			b=StreamBuffer[cd_stream].ChainList[a];	/* NEW! 13/Aug/2001 (used to be +1]); */

/* Only clear end marker if (a) stream is looping or (b) stream is chaining to another sample */
			if ((StreamBuffer[cd_stream].LoopInfo==STREAM_LOOPING)||(a<=StreamBuffer[cd_stream].ChainCount))
			{
				if (StreamBuffer[cd_stream].ResidentFlag==STREAM_AUDIO_ADPCM)
				{
					if (cd_readlen!=0)
					{
						_AdpcmSetMarkRESTART2(cd_stream,(u_int)cd_writeptr,0);	// NOT 5.2 / 6.2 SPECIFIC!!
					}
				}
			}

			if (StreamBuffer[cd_stream].AppendFlag==1)
			{
				StreamBuffer[cd_stream].FileStart=StreamBuffer[cd_stream].AP_FileStart;
				StreamBuffer[cd_stream].FilePlaySize=StreamBuffer[cd_stream].AP_FilePlaySize;
				StreamBuffer[cd_stream].FileID=StreamBuffer[cd_stream].AP_FileID;
				StreamBuffer[cd_stream].FileSeek=StreamBuffer[cd_stream].AP_FileSeek;
				StreamBuffer[cd_stream].FileRemainder=StreamBuffer[cd_stream].AP_FileRemainder;
				StreamBuffer[cd_stream].FileSize=StreamBuffer[cd_stream].AP_FileSize;
				a=GetFileID(StreamBuffer[cd_stream].FileID);			/* Set up new info for next file to chain.. */
				StreamBuffer[cd_stream].ChainPos=0;
			    OpenADPCMFile(cd_stream,a,STREAM_GET_OTHER_INFO);    // <<<<<<< I add this command

//Thanks to Cory for this bit..
				if (StreamBuffer[cd_stream].ParentChildFlag == SOUND_PARENT)
			    {
			    	for (child=0;child<UserMaxStreams;child++)
				    {
				    	if (StreamBuffer[child].ParentStream == cd_stream)
						{
							StreamBuffer[child].FileID = StreamBuffer[cd_stream].FileID;
						}
					}
				}
			}
			else if (a>StreamBuffer[cd_stream].ChainCount)
			{
				StreamBuffer[cd_stream].ChainPos=0;
				if (a!=1)				/* New! 13/Aug/2001 (used to be !=0) */
				{
					b=StreamBuffer[cd_stream].ChainList[StreamBuffer[cd_stream].ChainPos];
				}
				else   	/* If only ONE Stream in list (no chaining), no need to set values. */
				{

//printf("**** LOOPING *** (%x,%x)\n",cd_readptr,cd_filelen);
//printf("FileSize %x, BufferSize %x\n",cd_filelen,StreamBuffer[cd_stream].AllocSize);
					if (cd_filelen<StreamBuffer[cd_stream].AllocSize)	// NEW START!!
					{
//						printf("DONE\n");
						StreamBuffer[cd_stream].NoMoreLoading=1;
					}				// NEW END!!!!

					cd_filelen=(int)StreamBuffer[cd_stream].FileSize;
					StreamBuffer[cd_stream].FileRemainder=StreamBuffer[cd_stream].FilePlaySize;
					StreamBuffer[cd_stream].FileSeek=StreamBuffer[cd_stream].FileStart;
					cont=1;
				}
			}
			if (cont==0)
			{
				if (StreamBuffer[cd_stream].AppendFlag==0)
				{
					a=GetFileID(b);					/* Set up new info for next file to chain.. */
					OpenADPCMFile(cd_stream,a,STREAM_GET_INFO);		/*Still need to do OpenADPCMFile, so we get the File LSN/Handle etc.. */
					StreamBuffer[cd_stream].FileID=b;
					StreamBuffer[cd_stream].FileSeek=StreamBuffer[cd_stream].ChainFileSeek[StreamBuffer[cd_stream].ChainPos];
					StreamBuffer[cd_stream].FileRemainder=StreamBuffer[cd_stream].ChainPlaySize[StreamBuffer[cd_stream].ChainPos];
					StreamBuffer[cd_stream].FileSize=StreamBuffer[cd_stream].ChainFileSize[StreamBuffer[cd_stream].ChainPos];
				}
				StreamBuffer[cd_stream].AppendFlag=0;

				cd_filelen=(int)StreamBuffer[cd_stream].FileSize;		/* PlaySize */
				cd_source=StreamBuffer[cd_stream].Source;

				if ((cd_source==ADPCM_DATA_ON_HOST)&&
				(StreamBuffer[cd_stream].LoadSource2!=USE_EXTERNAL_LOADING))
				{

					File=JADPCM;
					File+=StreamBuffer[cd_stream].ATWinFile;	// New For Morgan!
					if (File->OpenFlag==0)
					{
						close(cd_startsec);		/* Close last file */
					}
					StreamBuffer[cd_stream].ATWinFile=(u_int)a;

					File=JADPCM;
					File+=a;		// New For Morgan!

					if (File->OpenFlag!=0)
					{
						StreamBuffer[cd_stream].SectorStart=File->OpenID;
					}
					else
					{
						filename=File->FileName;
						StreamBuffer[cd_stream].SectorStart=(unsigned int)open (filename, O_RDONLY);	/* Open new file */
					}
				}
				cd_startsec=(int)StreamBuffer[cd_stream].SectorStart;
			}
			cd_readptr=(int)StreamBuffer[cd_stream].FileSeek;
		}

/*work out the start and end positions to read from, this done by: */
/*if the start is on a 2048 boundary, and the end is, then read direct to memory */
/*if the start is on a 2048 boundary, and the end isn't then: */
/*	if the amount of data to read is >2048, then read up to the last 2048 boundary before the end */
/*	else read the data from the start to the end into my own 2k cache. */
/*if the start is not on a 2048 boundary, then read up to the next 2048 boundary into my cache */
/*also make sure that the file isn't read past the file length */

		start=cd_readptr;
		end=cd_readptr+cd_datalen;

		if (end>(cd_readptr+StreamBuffer[cd_stream].FileRemainder))
			end=(int)(cd_readptr+StreamBuffer[cd_stream].FileRemainder);

		if ((cd_readptr%CD_SECTOR_SIZE)==0)
		{
			if (end>cd_filelen)
				end=cd_filelen;
			if ((end%CD_SECTOR_SIZE)!=0)
			{
				if ((end-start)>CD_SECTOR_SIZE)
				{
					end-=(end%CD_SECTOR_SIZE); /*make it divisible by 2048 */
				}
				else
				{
/*need to read in <2048 to local and then copy off later... */
					local=1;
				}
			}
		}
		else{
/*doesn't start on a 2048 boundary, so just read to local: */
			local=1;
			if (end>(((start/CD_SECTOR_SIZE)+1)*CD_SECTOR_SIZE))
			{
/*reading PAST the next CD_SECTOR_SIZE boundary */
				end=((start/CD_SECTOR_SIZE)+1)*CD_SECTOR_SIZE;
			}
			if (end>cd_filelen)
				end=cd_filelen;
		}
		cd_readlen=end-start;

/******** Handle external loading (this code does the same as internal loading, but doesn't actually load anything. */
/******** This is so the end of file flags can still be cleared correctly */
		if (StreamBuffer[cd_stream].LoadSource2==USE_EXTERNAL_LOADING)
		{
			CDLoadFlag=1;
			_CdStreamInt(0);	/* (This would normally be called as a CD callback) */
			result=1;
			cd_localcopy=0;
		}
		else if (local)			// Need to load 1 sector (2048 bytes) into temp IOP buffer?
		{
/*set up some variables for local reading... */

			cd_localstart=start%CD_SECTOR_SIZE;		// byte offset through sector to eventually copy from

/******** JP04 - Load From Host  ******* */
/******** Using read(start/CD_SECTOR_SIZE,) ******* */

//
			if (cd_source==ADPCM_DATA_ON_HOST)
			{
				cd_localstart2=0;	// Clear last LSN loaded. Not required for host0 loading.
				SOUND_EnableRPC();
				lseek(cd_startsec, (start/CD_SECTOR_SIZE)*CD_SECTOR_SIZE , SEEK_SET );	/* (startsec is now the filehandle for host data) */
				SOUND_DisableRPC();
				SOUND_EnableRPC();
  				read (cd_startsec, (void*)cd_buffer,CD_SECTOR_SIZE);
				SOUND_DisableRPC();

				CDLoadFlag=1;
				_CdStreamInt(0);	/* (This would normally be called as a CD callback) */
				result=1;
			}
			else
			{
				a=cd_startsec+(start/CD_SECTOR_SIZE);	// actual sector on disk to copy..
				if ((a==cd_localstart2)&&(StreamBuffer[cd_stream].ResidentFlag==STREAM_LOAD_EE))	// Sector already loaded into temp buffer?
				{
					CDLoadFlag=1;
					_CdStreamInt(0);	/* (This would normally be called as a CD callback) */
					result=1;
				}
				else
				{
					if (StreamBuffer[cd_stream].ResidentFlag==STREAM_LOAD_EE)
						cd_localstart2=a;	// Remember last sector loaded.
					result=SOUND_StartCDRead(0,(unsigned int)(cd_startsec+(start/CD_SECTOR_SIZE)),1,(void*)cd_buffer,&cd_cdrmode);
				}
			}
			cd_localcopy=1;
		}
		else
		{

/******** JP04 - Load From Host  ******* */
/******** Using read(start/CD_SECTOR_SIZE,) ******* */

			if (cd_source==ADPCM_DATA_ON_HOST)
			{

				SOUND_EnableRPC();
				lseek(cd_startsec, (start/CD_SECTOR_SIZE)*CD_SECTOR_SIZE , SEEK_SET );	/* (startsec is now the filehandle for host data) */
				SOUND_DisableRPC();
				if (SOUND_Transfer.TranMode==SOUND_DTS_OFF)		/* DTS currently OFF ? */
				{
					for (a=0;a<((end-start)/CD_SECTOR_SIZE);a++)
					{
						SOUND_EnableRPC();
						read (cd_startsec, (void*)(cd_writeptr+(CD_SECTOR_SIZE*a)),CD_SECTOR_SIZE);	/*/CD_SECTOR_SIZE); */
						SOUND_DisableRPC();
					}
				}
				else
				{
					SOUND_EnableRPC();
					for (a=0;a<((end-start)/CD_SECTOR_SIZE);a++)
					{
						read (cd_startsec, (void*)(cd_writeptr+(CD_SECTOR_SIZE*a)),CD_SECTOR_SIZE);	/*/CD_SECTOR_SIZE); */
					}
					SOUND_DisableRPC();
				}

				CDLoadFlag=1;
				_CdStreamInt(0);	/* (This would normally be called as a CD callback) */
				result=1;
			}
			else
			{
				result=SOUND_StartCDRead(0,(unsigned int)(cd_startsec+(start/CD_SECTOR_SIZE)),
					(unsigned int)(end-start)/CD_SECTOR_SIZE, (void*)cd_writeptr, &cd_cdrmode);
			}
			cd_localcopy=0;
		}
#if 1

		if (result==0)
		{
			if (E_CDErrorCheck==0)
			{
				CDError_PauseStreams();		/* Pause all other streams */
				E_CDErrorCheck=1;
				I_CDErrorCheck=1;
				CDCheck=1;
			}
			MSERROR(("****** WARNING, WARNING!!...sceCdRead Error! ******\n"));
			CD_IDLE=1;
			return;
		}
#endif
	}

	if (cd_stage==CD_STAGE_DONE)
	{
/*hooray. read the file...now might have to do some 'other' stuff depending on what event it is... */
/*call cd_func which is a function that was set up in _CdStreamStart, this function */
/*will do anything that is required once the data is in memory */

		if ((cd_source==ADPCM_DATA_ON_HOST)&&
			(StreamBuffer[cd_stream].LoadSource2!=USE_EXTERNAL_LOADING))
		{
			File=JADPCM;
			File+=StreamBuffer[cd_stream].ATWinFile;
			if (File->OpenFlag==0)
			{
				close(cd_startsec);
			}
		}

		if ((E_CDErrorCheck==0)&&(I_CDErrorCheck==1))
		{
			if (CD_ERR_Stream==cd_stream)
			{
				MSERROR(("reloaded data to recover from cd error\n"));
				I_CDErrorCheck=0;		/* Only going to get this far after a cd error recover load has happened */
				CDError_ResumeStreams();
			}
		}

		CD_IDLE=1;
		cd_func(1);	/*call a function */
	}
	return;
}

/**********************************************************************************
//Redesigned CheckToWakeRPC, introduced EnabledRPC and SOUND_DisableRPC [Sean Gugler 2004-03-30]

CheckToWakeRPC
	Checks if the MultiStream RPC has been called and set to sleep due to main thread currently processing.
	If it is in this state, wake it up and let it process now.
***********************************************************************************/
void CheckToWakeRPC(void)
{
	if (MS_RPCFlag==1)	/* adpcmSetParam called and asleep? */
	{
		MS_RPCFlag=0;	/* Yes..Wake it up now, as its processing won't interfere with this thread */
		WakeupThread(MS_RPCThreadID);
	}
}

/**********************************************************************************
SOUND_EnableRPC
	Allows MultiStream main RPC to wake up before a host0 load (as it takes a bit of time..)
	This would be the same as the RPC being called during a (asynchronas) CD load
***********************************************************************************/
void SOUND_EnableRPC(void)
{
	MS_IntrFlagPrevious=MS_IntrFlag;
	MS_IntrFlag=0;
	CheckToWakeRPC();
}

/**********************************************************************************
SOUND_DisableRPC
	Sets MultiStreams parameters back to their original (pre SOUND_EnableRPC) state
	Waits until MultiStream RPC has been processed if required to do so.
***********************************************************************************/
void SOUND_DisableRPC(void)
{
	MS_IntrFlag=MS_IntrFlagPrevious;
	while(SOUND_MasterEnable==1)
	{
		CheckToWakeRPC();
	}
}

/**********************************************************************************
cdend_PreloadDataNowInIOP (aka cdend1)
CDStream - Return function
PreLoad stream loaded into IOP - Now transfer it to SPU
***********************************************************************************/
void cdend_PreloadDataNowInIOP()
{

/* Check to make sure that stream hasn't stopped or been restarted while loading data.. */
	if (StreamBuffer[cd_stream].iStatus==iSTATUS_preTRANSFER1)
	{
		if (StreamBuffer[cd_stream].ParentChildFlag==0)	/* Parent? */
		{
			UpdateStream(cd_stream,(int)StreamBuffer[cd_stream].AllocSize, STREAM_PRELOAD,
			cdend_PreloadedDataNowInSPU);	/* Not a parent. Transfer data to SPU2 */
		}
		else
		{
			UpdateStream(cd_stream,(int)StreamBuffer[cd_stream].AllocSize, STREAM_PRELOAD,
				cdend_InterleavedSecondTransfer);	/* Parent stream. Needs more preload data */
		}
	}
	else
		StreamBusy=0;	/* Stream status is wrong (stream stopped, or started again..) */
}

/**********************************************************************************
cdend_PreloadedDataNowInSPU (aka cdend2)
CDStream - Return function
PreLoaded data transfered to SPU - Now change the status flag to play it.
***********************************************************************************/
void cdend_PreloadedDataNowInSPU()
{
/* Check to make sure that stream hasn't stopped or been restarted while loading data.. */
	if (StreamBuffer[cd_stream].iStatus==iSTATUS_preTRANSFER1)
	{
		StreamBuffer[cd_stream].iStatus=iSTATUS_PLAY;				/* CHANGE STATUS */
	}
	StreamBusy=0;	/* Tells EE that Stream loading from CD has finished */
}

/**********************************************************************************
cdend_IntPreloadedDataNowInSPU (aka cdend2)
CDStream - Return function
Interleaved PreLoaded data transfered to SPU - Now change the status flag to play it.
***********************************************************************************/
void cdend_IntPreloadedDataNowInSPU()
{
/* Check to make sure that stream hasn't stopped or been restarted while loading data.. */
	if (StreamBuffer[cd_stream].iStatus==iSTATUS_preTRANSFER2)
	{
		StreamBuffer[cd_stream].iStatus=iSTATUS_PLAY;		/* CHANGE STATUS */
	}
	StreamBusy=0;	/* Tells EE that Stream loading from CD has finished */
}

/**********************************************************************************
cdend_InterleavedSecondTransfer (aka cdend22)
CDStream - Return function
For Interleaved data: 1st PreLoaded data pack transfered to SPU. Now transfer second
packet, to fill whole of SPU stream buffer.
***********************************************************************************/
void cdend_InterleavedSecondTransfer()
{
/* Check to make sure that stream hasn't stopped or been restarted while loading data.. */
	if (StreamBuffer[cd_stream].iStatus==iSTATUS_preTRANSFER1)
	{
		StreamBuffer[cd_stream].iStatus=iSTATUS_preTRANSFER2;
	}
	StreamBusy=0;	/* Tells EE that Stream loading from CD has finished */
}

/**********************************************************************************
cdend_KeyedOn (aka cdend3)
CDStream - Return function
After loading first half of IOP buffer with data (Once preloaded data is transfered)
***********************************************************************************/
void cdend_KeyedOn()
{
/* Check to make sure that stream hasn't stopped or been restarted while loading data.. */
	if (StreamBuffer[cd_stream].iStatus==iSTATUS_KEYED_ON)
	{
		StreamBuffer[cd_stream].iStatus=iSTATUS_PLAYING;				/* CHANGE STATUS */
	}
	StreamBusy=0;	/* Tells EE that Stream loading from CD has finished */
	IOPRequestLoad=-1;
}

/**********************************************************************************
cdend_FirstHalfPlayed (aka cdend4)
CDStream - Return function
After first half is played, transfer loaded half and load new half to IOP
(Callback is made from the cdload to IOP)
***********************************************************************************/
void cdend_FirstHalfPlayed()
{
	if (StreamBuffer[cd_stream].PlayingHalf!=STREAM_SHORT)
	{
		StreamBuffer[cd_stream].PlayingHalf=STREAM_SECOND_HALF;
	}
	StreamBusy=0;	/* Tells EE that Stream loading from CD has finished */
}

/**********************************************************************************
cdend_SecondHalfPlayed (aka cdend5)
CDStream - Return function
After second half is played, transfer loaded half and load new half to IOP
(Callback is made from the cdload to IOP)
***********************************************************************************/
void cdend_SecondHalfPlayed()
{
	if (StreamBuffer[cd_stream].PlayingHalf!=STREAM_SHORT)
	{
		StreamBuffer[cd_stream].PlayingHalf=STREAM_FIRST_HALF;
	}
	StreamBusy=0;	/* Tells EE that Stream loading from CD has finished */
}

/**********************************************************************************
SetVoiceData
	Sets up the channel ready to play
	Does NOT perform the keyon..

Requires:
			Channel		= SPU Channel (0-47)
			Pitch		= SPU Pitch
			VolL		= Left Volume
			VolR		= Right Volume
			SPUAddress	= Address of sample in SPU RAM
			AR			= Attack and Release values
			flag		= (how it handles setting volume!)
***********************************************************************************/
void SetVoiceData(int Channel,int Pitch,int VolL,int VolR,int SPUAddress, int AR,int flag)
{
int ch;
int a,r;
int core;

	a=AR&0xff;		/* Attack */
	r=AR>>8;		/* Release */
	a<<=8;
	a|=0xff;		/* Attack = SPU Friendly value */
	r|=0x1fc0;		/* Release= SPU Friendly value */

	ch=SOUND_GetSPUChannel(Channel);	/* Get SPU channel */
	core=SOUND_GetCore(Channel);		/* Get SPU core */

	ch=ch<<1;	/* SPU Channel = SPU Friendly value */
	SOUND_SetChannelVolume(Channel,VolL,VolR,flag);			/* Set volume */
	SOUND_SetChannelPitch(Channel,Pitch);				/* Set pitch */
	sceSdSetParam ((unsigned short)(core | ch | SD_VP_ADSR1), (unsigned short)a);			/* Set ADSR1 */
	sceSdSetParam ((unsigned short)(core | ch | SD_VP_ADSR2), (unsigned short)r);			/* Set ADSR2 */
	sceSdSetAddr ((unsigned short)(core | ch | SD_VA_SSA), (unsigned int)SPUAddress);	/* Set SPU RAM Start address */
}

/**********************************************************************************
CDError_PauseStreams
	Pauses any streams that are being played if CD error occures.
	Note: You can't just call the usual Pause routine, as people may want to still
	play spot FX even if the CD is not working. And then they may want to pause the game too.
	Which would cause lots of strange things to happen..where you would be pausing things
	twice and...oooo..lots of stuff like that.
***********************************************************************************/
void CDError_PauseStreams(void)
{
int a,b;
int pitch;
int SPUChan;
int i;

	MSERROR(("CDError_PauseStreams\n"));
	MSERROR(("Pausing all audio streams\n"));

	for (a=0;a<UserMaxStreams;a++)
	{
		if (StreamBuffer[a].Status==STREAM_STATUS_ON)	/* Is Stream ON ? */
		{
			if (StreamBuffer[a].ResidentFlag==STREAM_AUDIO_ADPCM)	/* Streaming ADPCM audio? */
			{
				SPUChan=SOUND_GetStreamSPUChan(a);
				pitch=StreamBuffer[a].Pitch;		/* Current pitch for audio stream */

				if (SOUND_PauseStreamCheck[SPUChan]!=1)	/* If stream is not already paused.. */
				{
					SPU_CDError[SPUChan]=1;				/* pause it.. */
					SOUND_PauseStreamCheck[SPUChan]=0;
					if (StreamBuffer[a].Ch2Flag!=0)		/* Pause other SPU channels if active. */
					{
						for (i=0;i<StreamBuffer[a].Ch2Flag;i++)
						{
							SOUND_PauseStreamCheck[StreamBuffer[a].SPUChannel2[i]]=0;
						}
						for (i=0;i<StreamBuffer[a].Ch2Flag;i++)
						{
							SOUND_SetChannelPitch(StreamBuffer[a].SPUChannel2[i],0);
						}
					}
					SOUND_SetChannelPitch(SPUChan,0);	/* Pause 1st SPU channel */
/* Note: The SPU channel pitches are set to 0 like this, so that there is a minimum time */
/* difference between setting them both... */

					if (StreamBuffer[a].Ch2Flag!=0)		/* Pause other SPU channels if active. */
					{
						for (i=0;i<StreamBuffer[a].Ch2Flag;i++)
						{
							SPU_PausePitch[StreamBuffer[a].SPUChannel2[i]]=(unsigned short)pitch;
						}
					}
					SPU_PausePitch[SPUChan]=(unsigned short)pitch;	/* Copy pitch into SPU channel pause buffer */
				}
			}
			else if (StreamBuffer[a].ResidentFlag==STREAM_AUDIO_PCM)
			{
				b=PCMPause;
				SOUND_PausePCM();
				PCMPause=(unsigned char)b;
				PCMCDPause=1;
			}
		}
	}
}

/**********************************************************************************
CDError_ResumeStreams
	Resumes streams that were paused due to CD error. See above for more details..
***********************************************************************************/
void CDError_ResumeStreams(void)
{
int a;
int SPUChan;
int i,j;
u_int SPUOffset;
u_int chan,core;
int c;

/* This part of the code attempts to get all PARENT/CHILD Streams playback positions in sync, */
/* so that restarting them will not cause any phasing effects.. */

	for (i=0;i<UserMaxStreams;i++)
	{
		if ((StreamBuffer[i].ParentChildFlag==SOUND_PARENT)&&(StreamBuffer[i].Status==STREAM_STATUS_ON))
		{
			c=SOUND_GetStreamSPUChan(i);

 			chan=(unsigned int)SOUND_GetSPUChannel((int)c);
 			core=(unsigned int)SOUND_GetCore((int)c);
 			chan<<=1;
 			SPUOffset=sceSdGetAddr((unsigned short)(core|chan|SD_VA_NAX));	/* Current playback pos of PARENT */

/*Now Make SPUAddr = the OFFSET through the current SPU buffer */

			SPUOffset-=StreamBuffer[i].SPUAddress;		/* Convert to offset through SPU buffer */
			for (a=0;a<UserMaxStreams;a++)
			{
/* Find any CHILD streams for PARENT */
				if ((StreamBuffer[a].ParentStream==StreamBuffer[i].ParentStream)&&
					(StreamBuffer[a].Status==STREAM_STATUS_ON)&&(a!=i))
				{
					c=SOUND_GetStreamSPUChan(a);
   					chan=(unsigned int)SOUND_GetSPUChannel((int)(c));	/* Get SPU channel */
 					core=(unsigned int)SOUND_GetCore((int)(c));			/* Get SPU core */
 					chan<<=1;	/* Makre channel SPU Friendly.. */

/* Set offset of all CHILD stream to the same as the parents. */
 					sceSdSetAddr((unsigned short)(core|chan|SD_VA_NAX),StreamBuffer[a].SPUAddress+SPUOffset);
					if (StreamBuffer[a].Ch2Flag!=0)	/* Using more than one SPU channel? */
					{
						for (j=0;j<StreamBuffer[a].Ch2Flag;j++)
						{
		   					chan=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[a].SPUChannel2[j]);
		 					core=(unsigned int)SOUND_GetCore(StreamBuffer[a].SPUChannel2[j]);
		 					chan<<=1;
		 					sceSdSetAddr((unsigned short)(core|chan|SD_VA_NAX),StreamBuffer[a].SPUAddress+SPUOffset);
						}
					}
				}
			}
		}
	}

/* This part of the code resumes any paused streams.. */

	for (a=0;a<UserMaxStreams;a++)
	{
		if (StreamBuffer[a].Status==STREAM_STATUS_ON)
		{
			if (StreamBuffer[a].ResidentFlag==STREAM_AUDIO_ADPCM)
			{
				SPUChan=SOUND_GetStreamSPUChan(a);

/* Was stream previously paused due to CD error, and SOUND_Pause not called? */
				if(SPU_CDError[SPUChan]!=0)
				{
/* Set pitch for SPU channel(s) */
					if (StreamBuffer[a].Ch2Flag!=0)	
					{
						for (j=0;j<StreamBuffer[a].Ch2Flag;j++)
						{
							SOUND_SetChannelPitch(StreamBuffer[a].SPUChannel2[j],SPU_PausePitch[SPUChan]);
						}
					}
					SOUND_SetChannelPitch(SPUChan,SPU_PausePitch[SPUChan]);
/* Copy pitch back to Stream bufer. */
					StreamBuffer[a].Pitch=(short)SPU_PausePitch[SPUChan];
				}
			}
			else if (StreamBuffer[a].ResidentFlag==STREAM_AUDIO_PCM)
			{
				if ((PCMPause==0)&&(PCMPlaying!=0))
					SOUND_ResumePCM();	/* Resume PCM playback */
				else if ((PCMPlaying==0)&&(PCMPause!=0))
				{
/* PCM was paused before it actually started playing (during preload..) */
					SOUND_StartPCM();
				}
				PCMCDPause=0;
			}
		}
	}

	for (a=0;a<48;a++)
	{
 		SPU_CDError[a]=0;	/* Clear all CD error flags */
	}
}


/**********************************************************************************
SOUND_CheckUpdate
	Checks to see if there is a CD error which needs fixing.

	Requires:	Nothing

	Returns:	strm	Stream channel which needs to recover from CD error
				-1		No Error

***********************************************************************************/
int SOUND_CheckUpdate(void)
{
	if (I_CDErrorCheck==1)		/* Had there been a CD error? */
	{

		if ((StreamBuffer[CD_ERR_Stream].Status!=STREAM_STATUS_ON)||
			(StreamBuffer[CD_ERR_Stream].Error==0))
		{
			MSERROR(("SOUND_CheckUpdate\n"));
			MSERROR(("Can't continue with last stream error\n"));
			MSERROR(("Stream %d either not ON or a new stream has been requested\n",CD_ERR_Stream));

			I_CDErrorCheck=0;
			CDError_ResumeStreams();	/* Offending error stream shut down. */
			return(-1);
		}
		else
		{
			return(CD_ERR_Stream);		/* Resume playback from an error situation */
		}
	}
	return(-1);
}

/**********************************************************************************
SOUND_StopDTS
	Stops DTS system.
	flag needs to be set as the MultiStream thread has to handle the closing of the
	DTS thread to make things nice..For example, if any IOP->SPU DMA transfers are
	already taking place, we need to make sure that we clear up these before shutting
	down the DTS system

	Requires:	flag	0		Stop callbacks and block trans
						1		As with (0), but also closes DTS thread (full shut down)
						2		Only close DTS thread
***********************************************************************************/
void SOUND_StopDTS(int flag)
{
#if (SOUND_DTS_ENABLE)

	if (flag==2)
	{
		DTS_stop(flag);		/* Only turn off thread. */
		return;
	}

	DTS_StopFlag=flag;
	if (SOUND_Transfer.TranMode==SOUND_DTS_ON)		/* DTS currently ON ? */
		SOUND_Transfer.TranMode=SOUND_DTS_CLOSING;	/* Yes..Set to CLOSING until properly shut down. */

#else
	(void) flag;
	printf("ERROR: SOUND_StopDTS\n");
	printf("DTS Not supported\n");
	printf("Set SOUND_DTS_ENABLE to 1\n");
	printf("Also requires DTS Middleware solution\n");

#endif
}

/**********************************************************************************
SOUND_HandleEEWait
Handles the transfering of a counter to the EE everytime this thread is run.
It allows the EE to know when its safe to do another FlushIOPCommand without blocking the
IOP. Too many FlushIOPCommands (or SIF RPC's) will actually stop anything on the IOP getting
a chance to run, as the IOP will be too busy having to handle the RPC functions instead.

***********************************************************************************/
void SOUND_HandleEEWait(void)
{
	if (SOUND_WaitEEAddress!=0)		/* Has the user passed an EE address to transfer to? */
	{
/* Send counter value to EE. */
		SOUND_TransferDataToEE((u_int)(u_int *)&SOUND_WaitCounter,SOUND_WaitEEAddress,64);
		SOUND_WaitCounter++;		/* Increase counter for next time this is called. */
		SOUND_WaitCounter&=0xff;
	}
}

/**********************************************************************************
StreamADPCM
	This is the main one. Handles DMA transfers to IOP and loading/checking of currently
	playing data. Called from IRQ callback function (using semaphore)
***********************************************************************************/

int cs=-1;
int ct=0;

int StreamADPCM()
{
int i,a,b;

	while(1)
	{
		MS_IntrFlag=0;		// MS7.2 (Thanks SeanG)
		CheckToWakeRPC();	// MS7.2 (Thanks SeanG)

#if 0
		if (MS_RPCFlag==1)	/* adpcmSetParam called and asleep? */
		{

			MS_RPCFlag=0;	/* Yes..Wake it up now, as its processing won't interfere with this thread */
			WakeupThread(MS_RPCThreadID);
		}
		MS_IntrFlag=0;
#endif

#if (SOUND_DTS_ENABLE)
		if (DTS_InitFlag==DTS_REQUEST_MULTISTREAM_SHUTDOWN)
		{
			DTS_InitFlag=DTS_MULTISTREAM_SHUTDOWN;
			SleepThread();
		}
#endif

/*printf("stream adpcm end\n"); */

#if MS_TIMER

		WaitSema(gSem);		/* Wait for interrupt to wake this thread */

//if (SOUND_TimerUpdate==0)	// WakeSema called by timer interrupt?
//printf("stream adpcm start\n");
#else
		WaitVblankEnd();	/* Use this instead of all of the interrupt code... */
		SOUND_TimerUpdate=1;
#endif

		if (SOUND_CD_I_ERROR==1)
		{
			if (StreamBuffer[CD_ERR_Stream].Error==1)	/* Was loading during the CD Error? */
			{
				MSERROR(("trying to do read again..\n"));
				CDCallbackFlag=0;
				SOUND_StartCDRead(1,0,0,0,0);	// And kick it off again..
				SOUND_FastINTERNAL_RPC(CD_ERR_Stream,0,0,3);	/* Tell the EE Fastload callback about this error */
				CD_IDLE=1;
			}
			SOUND_CD_I_ERROR=0;
		}
		MS_IntrFlag=1;		// MS7.2

#if (MS_LOAD62!=0)
		if (SOUND_Transfer.TranMode!=SOUND_DTS_ON)		/* DTS currently OFF ? */
		{
			SOUND_UpdateSPUTrans(0);
		}
#endif

		if (SOUND_MasterEnable==0)
		{
		
#if (SOUND_DTS_ENABLE)

/* Has DTS system shut down? */
			if (SOUND_Transfer.TranMode==SOUND_DTS_READY_TO_DIE)
			{
				DTS_stop(DTS_StopFlag);	/* Stop DTS */
				SOUND_Transfer.TranMode=SOUND_DTS_OFF;	/* DTS is finally off.. */
			}
#endif

/* This part will check to make sure that the SPU callbacks are working. */
/* Many people have had problems using MultiStream after playing a movie, due to the mpeg */
/* player code shutting down the SPU Transfer interrupt. */
/* This code checks to make sure the interrupt is working.. */
/* Should only be called when DTS mode is OFF, as DTS initialisation can cause problems otherwise. */

#if MS_TIMER
			if (SOUND_TimerUpdate==1)
			{
				SOUND_CalcPlayingTime();
		  		GetChannelStatus();		/* Mark SPU channels as either ON or OFF */
				SOUND_UpdateSmoothVols();
				SOUND_TimerUpdate=0;
			}
#else
	  		GetChannelStatus();		/* Mark SPU channels as either ON or OFF */
			SOUND_UpdateSmoothVols();
#endif
			SOUND_GetActiveStreamInfo();	/* Gets current SPU RAM playback address and key status to send to EE */
			SOUND_UpdateLoopingSFX();	/* Handle looping SFX (if requested to loop n times) */

			a=0;

			if ((E_CDErrorCheck==0)&&(I_CDErrorCheck==0))	/* No CD Errors? */
			{
/* Check to see if two streams need to start at the same time (pre 5.0..Use MIB files now!) */
				CheckTwoStreamsStatus(StereoChan1,StereoChan2,StereoPitch);
			}
			if (E_CDErrorCheck==0)	/* EE Internal Error flag clear (but maybe still in a pause state) */
			{
				MS_IntrFlag=1;
				SOUND_HandleCDEvent();		/* Check if a CD event has occured (load or a seek..)..Deal with it if it has... */
				a=SOUND_CheckForEERequest();	/* Has EE allowed any IOP requested for data to be loaded? */
			}
			MS_IntrFlag=2;

//printf("a %x, CDError %x, StrmBusy %x, UserMax %x\n",a,E_CDErrorCheck,StreamBusy,UserMaxStreams);

			if ((a==0)&&(E_CDErrorCheck==0)&&(StreamBusy==0)&&(UserMaxStreams!=0))			/* NEW 29/6/01 Not loading anything OR had a load request from EE */
			{

#if (MS_DSP_ACTIVE==1)
				if (SOUND_DSP_ENGINE==2)	// Request DSP to close down? Do it now, as no IOP->SPU Transfer is happening.
				{
					SOUND_ReallyCloseDSP();
					SOUND_Transfer.TranMode=SOUND_TRAN_OFF;	/* DSP is finally off.. */
				}
#endif

				LoadEECounter=MyCounter;	/* Current Stream Request counter value to send to EE */

/* Having three passes allows the system to give priority to AUDIO streams. Because data streams do not require a */
/* pause between loading (ie. Not having to reach a point before requiring more data), they can take all the */
/* resources for the streaming system if you're not careful. Giving priority to the audio streams means that */
/* they don't miss a load. */

				SOUND_CheckUpdate();
				b=SOUND_SortStreamPriorities();

				for (a=0;a<b;a++)
				{
					if ((CD_IDLE==1) && (USR_HALT_MULTISTREAM==0) && (IOPRequestLoad==-1))
					{
						i=SOUND_CheckUpdate();
						if (i==-1)
							i=SOUND_GetNextStreamPriority((u_int)a,(u_int)b);
						if (StreamBuffer[i].Status==STREAM_STATUS_ON)
						{
							MS_IntrFlag=2;

/*SPU_DMA_TRANS_FLAG is set to 1 before a DMA Transfer from IOP->SPURAM and then set to 0 on completeion */
/*USR_HALT_MULTISTREAM is set to 1 around the initialisation of a stream, and to 0 when initialisation is completed */
/*CD_IDLE is 1 if CD not used, 0 if busy. */

/*printf("SPUDMA %x CD %x USR_ %x IOPR %x\n",SPU_DMA_TRANS_FLAG,CD_IDLE,USR_HALT_MULTISTREAM,IOPRequestLoad); */

							if (StreamBuffer[i].iStatus==iSTATUS_INITIALISED)
							{
								SOUND_CopyValues(i,0);	/* Copy stream values to temp buffer */
								SOUND_HandlePreTransfer(i);	/* (calls StartStream and other stuff..) */
							}
							else if (StreamBuffer[i].iStatus==iSTATUS_preTRANSFER2)
							{
								SOUND_CopyValues(i,0);	/* Copy stream values to temp buffer */
								SetIOPRequest(i);		/* (Transfers data from IOP to SPU and other..) */
							}
							else if (StreamBuffer[i].iStatus==iSTATUS_PLAY)
							{
								if ((StreamBuffer[i].ResidentFlag==STREAM_RESIDENT)&&
									(StreamBuffer[i].LoopInfo==STREAM_READY_TO_DIE))
								{
									StreamBuffer[i].iStatus=iSTATUS_PLAYING;
								}
								else
								{
									if (StreamBuffer[i].EEAllow==1)	// NEW!!  MS72 10th May 2004
									{
										SOUND_CopyValues(i,0);	/* Copy stream values to temp buffer */
										SetIOPRequest(i);		/* (KeyOn SPU channel, load more data) */
									}
								}
							}
							else if (StreamBuffer[i].iStatus==iSTATUS_PLAYING)
							{
								if ((StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_ADPCM)&&
									(StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_PCM)&&
									(StreamBuffer[i].PlayingHalf==STREAM_SHORT))	/* File loading? */
								{
									if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_IOP)
									{
										SOUND_UpdateMemoryEndPosition(i);	/* Move next SPU load addr past current */
										StreamBuffer[i].ResidentFlag=STREAM_AUDIO_ADPCM;
										StreamBuffer[i].Status=STREAM_STATUS_OFF;	/* File loaded.. */
										SOUND_CheckForNextLoad(i);
									}
									else if ((StreamBuffer[i].ResidentFlag==STREAM_LOAD_EE)&&(StreamBuffer[i].SPUChannel==3))
									{
//printf("hree2\n");
										SOUND_UpdateMemoryEndPosition(i);	/* Move next SPU load addr past current */
										StreamBuffer[i].ResidentFlag=STREAM_AUDIO_ADPCM;
										StreamBuffer[i].Status=STREAM_STATUS_OFF;	/* File loaded.. */
										SOUND_CheckForNextLoad(i);
									}
									else if (StreamBuffer[i].ResidentFlag!=STREAM_LOAD_EE)
									{

#if (MS_LOAD62!=0)
										if (StreamBuffer[i].RequestLoad==2)
										{
#endif
											StreamBuffer[i].RequestLoad=0;
											SOUND_UpdateMemoryEndPosition(i);	/* Move next SPU load addr past current */

SOUND_FastINTERNAL_RPC(i,StreamBuffer[i].TotalTranSize,StreamBuffer[i].LastTranSize,1);	// 7.3

											StreamBuffer[i].ResidentFlag=STREAM_AUDIO_ADPCM;
											StreamBuffer[i].Status=STREAM_STATUS_OFF;	/* File loaded.. */
											SOUND_CheckForNextLoad(i);
#if (MS_LOAD62!=0)
										}
#endif
									}
								}
								else if ((StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_ADPCM)&&
										(StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_PCM)&&
										(StreamBuffer[i].LoopInfo!=STREAM_READY_TO_DIE))
								{
//printf("hree\n");
										if (StreamBuffer[i].EEAllow==1)	// NEW!! 
										{
											SOUND_CopyValues(i,0);	/* Copy stream values to temp buffer */
											SetIOPRequest(i);	/* Request load for DataFile */
										}
								}
								else
								{
									SOUND_CheckForHalfBufferPlayed(i);	/* Has either half of the SPU buffer been played? */
									SOUND_CheckStreamSPUStatus(i);	/* Shut down stream if SPU channel is keyed off. */
								}
							}
					 	}
					}
	 			}
			}

		SOUND_HandleEEWait();	/* Handle transfering counter to EE for faster EE processing */
		}
	}
	return(0);
}

/**********************************************************************************
SOUND_CheckStreamSPUStatus
	Checks to see if a stream is still active. Either:
	a)	If a KeyOFF has been performed on this channel, and the release value is 0
	b)	The stream playback reaches the end of the file (and its non-looping)

	If the SPU channel is now "dead", SOUND_StopStream needs to be called to shut
	it down properley, and also to kill any "Child" streams that may be running off
	this streams data..
***********************************************************************************/
void SOUND_CheckStreamSPUStatus(int i)
{
int ch,core;
int parent;
int b;

	if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)
	{
/* PCM CHECK FOR FINISHED PLAYING */
		return;
	}

	b=i;
	parent=StreamBuffer[i].ParentStream;
	if (parent!=-1)		/* Is this stream part of an interleaved stream (either parent or child)? */
		b=parent;

	ch=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel);	/* SPU Channel */
	core=SOUND_GetCore(StreamBuffer[i].SPUChannel);		/* SPU Core */
	if (sceSdGetParam((unsigned short)(core|(ch<<1)|SD_VP_ENVX))==0)	/* Current amplitude */
	{
		SOUND_StopStream(i,0);	/* Playback must have reached an end of data marker */
	}
}

/**********************************************************************************
SOUND_CheckForNextLoad
	Checks to see if a new data load has been requested by a stream which has just
	completed - and was also using FASTLOAD.

	Requires:	i		Stream channel
	Returns		Nothing
***********************************************************************************/
void SOUND_CheckForNextLoad(int i)
{
u_int p1,p2,p3;

// Has FASTLOAD callback requested a SetFileOffsetAndPlaySize ?

	if (StreamBuffer[i].NextFileID_FOAPS!=0)
	{
		SOUND_SetFileOffsetAndPlaySize((int)StreamBuffer[i].NextFileID_FOAPS-1,
										StreamBuffer[i].NextFileOffset_FOAPS,
										StreamBuffer[i].NextFileSize_FOAPS);
		StreamBuffer[i].NextFileID_FOAPS=0;
	}

// Has FASTLOAD callback requested a new file to load ?

	p1=StreamBuffer[i].NextLoad;		// 7.2 FIX Sorry, guys!
	if (p1!=0)
	{
		p2=StreamBuffer[i].NextFileID;
		p3=StreamBuffer[i].NextAddress;
		StreamBuffer[i].NextLoad=0;

		if (p3!=0)	// Specified a new loading address?
		{
			if (p1==STREAM_LOAD_EE)
				StreamEEAddress=p3;
			else if (p1==STREAM_LOAD_IOP)
				StreamIOPAddress=p3;
			else if ((p1==STREAM_LOAD_SPU)||(p1==STREAM_RESIDENT))
				StreamMemoryEnd=p3;
		    else
		    {
		        MSERROR(("ERROR: SOUND_CheckForNextLoad. Invalid destination (%x)\n",p1));
		        return;
		    }
			SOUND_PlayStream(i<<8,0,0,(int)p2,0,(int)p1,0);
		}
	}
}

#if (MS_LOAD62!=0)
/**********************************************************************************
SOUND_CheckForHalfBufferPlayed - V6.2 code
	Checks to see if an active stream has played either its first or second half of
	the SPU buffer. If so, it knows that the half it has just played needs more data
	(which is transferred from IOP->SPU) and that this next half it has to play
	needs more data from the CD/host.
***********************************************************************************/
void SOUND_CheckForHalfBufferPlayed(int i)
{
int ch;
int a;

	if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)	/* Playing PCM ? */
	{
/* Check for half of PCM buffer played */
		ch = (int)(sceSdBlockTransStatus (MS_PCM_CORE, 0) >> 24);

		if (PCMPlaying==1)	/* Is PCM playing? */
		{
			if (ch!=StreamBuffer[i].PlayingHalf)	/* Yes..Is it the same buffer half? */
			{
				SOUND_CopyValues(i,0);	/* No.So request another load.. */
				SetIOPRequest(i);
			}
		}
		else if (PCMPlaying==2)	/* PCM playing final data? */
		{
			if (ch!=PCMHalf)	/* Yes..Has buffer half changed? */
			{
				SOUND_PCMOnceOff(i);	/* Yes.Transfer 0's to 2nd buffer half */
				PCMPlaying++;
				IOPRequestLoad=-1;	/* No data load required.. */
				StreamBusy=0;
			}
		}
		else if (PCMPlaying==3)		/* PCM playing final 1st half of data? */
		{
			if (ch==PCMHalf)		/* Yes, check to see if it still is.. */
			{
				SOUND_StopStream(i,0);	/* No, so it must be playing the 0's.. */
				StreamBusy=0;			/* Stop stream. */
			}
		}
		return;
	}

	if (StreamBuffer[i].ParentChildFlag==SOUND_PARENT)
	{
		for (a=0;a<UserMaxStreams;a++)
		{
			if ((StreamBuffer[i].ParentStream==StreamBuffer[a].ParentStream)&&
				(StreamBuffer[i].TransCounter!=StreamBuffer[a].TransCounter))
			{
				return;
			}
		}
	}

	if (StreamBuffer[i].PlayingHalf!=STREAM_SHORT)	/* All audio data loaded into SPU RAM? */
	{
		if (StreamBuffer[i].RequestLoad==1)
		{
			SOUND_CopyValues(i,0);
			SetIOPRequest(i);
		}
	}
}
#else
/**********************************************************************************
SOUND_CheckForHalfBufferPlayed - V5.2 code
	Checks to see if an active stream has played either its first or second half of
	the SPU buffer. If so, it knows that the half it has just played needs more data
	(which is transferred from IOP->SPU) and that this next half it has to play
	needs more data from the CD/host.
***********************************************************************************/
void SOUND_CheckForHalfBufferPlayed(int i)
{
int StreamAddress;
int ch,core;
int ch2;
int core2;

u_int SPUStart;
u_int SPUEnd;
u_int pos;
int a;

	ch2=0;
	core2=0;

	if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)	// Playing PCM ?
	{
// Check for half of PCM buffer played
		ch = (sceSdBlockTransStatus (MS_PCM_CORE, 0) >> 24);

		if (PCMPlaying==1)	// Is PCM playing?
		{
			if (ch!=StreamBuffer[i].PlayingHalf)	// Yes..Is it the same buffer half?
			{
				SOUND_CopyValues(i,0);	// No.So request another load..
				SetIOPRequest(i);
			}
		}
		else if (PCMPlaying==2)	// PCM playing final data?
		{
			if (ch!=PCMHalf)	// Yes..Has buffer half changed?
			{
				SOUND_PCMOnceOff(i);	// Yes.Transfer 0's to 2nd buffer half
				PCMPlaying++;
				IOPRequestLoad=-1;	// No data load required..
				StreamBusy=0;
			}
		}
		else if (PCMPlaying==3)		// PCM playing final 1st half of data?
		{
			if (ch==PCMHalf)		// Yes, check to see if it still is..
			{
				SOUND_StopStream(i,0);	// No, so it must be playing the 0's..
				StreamBusy=0;			// Stop stream.
			}
		}
		return;
	}

	if (StreamBuffer[i].ParentChildFlag==SOUND_PARENT)
	{
		for (a=0;a<UserMaxStreams;a++)
		{
			if ((StreamBuffer[i].ParentStream==StreamBuffer[a].ParentStream)&&
				(StreamBuffer[i].TransCounter!=StreamBuffer[a].TransCounter))
			{
				return;
			}
		}
	}

	SPUStart=StreamBuffer[i].SPUAddress;	// Stream SPU RAM Buffer start address
	SPUEnd=SPUStart+(StreamBuffer[i].IOPBufferHalfSize*2);	// Stream SPU RAM buffer end address

	ch=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel);	// SPU Channel
	core=SOUND_GetCore(StreamBuffer[i].SPUChannel);		// SPU Core

// mid point between start and end SPU RAM buffer address
	pos=StreamBuffer[i].SPUAddress+StreamBuffer[i].IOPBufferHalfSize;

	StreamAddress=sceSdGetAddr(core|(ch<<1)|SD_VA_NAX);	// Current SPU RAM playback address

// This could be true if SOUND_PlayStreamFromSFX is called..
	if ((StreamAddress<SPUStart)||(StreamAddress>SPUEnd))	// Not playing stream..
		return;

	if (StreamBuffer[i].PlayingHalf==STREAM_FIRST_HALF)	// Currently playing first half of buffer
	{

		sceSdSetAddr (core | ch<<1 | SD_VA_LSAX, pos);	// Set loop address to point to 2nd half
		if (StreamBuffer[i].Ch2Flag!=0)
		{
			for (a=0;a<StreamBuffer[i].Ch2Flag;a++)
			{
				ch2=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[a]);
				core2=SOUND_GetCore(StreamBuffer[i].SPUChannel2[a]);
				sceSdSetAddr (core2 | ch2<<1 | SD_VA_LSAX, pos);	// Same for 2nd SPU channel.
			}
		}

		if (StreamAddress>pos)		// Playback position in 2nd half?
		{
			if (StreamAddress<SPUEnd)
			{
				SOUND_SetCalcPlayingTime(i,StreamBuffer[i].SPUAddress,pos);
				SOUND_CopyValues(i,0);	// Yes..Trans data to 1st half, Request Load for 2nd half.
				SetIOPRequest(i);
			}
		}
	}
	else if (StreamBuffer[i].PlayingHalf==STREAM_SECOND_HALF)	// Current playing 2nd half of buffer
	{
		sceSdSetAddr (core | ch<<1 | SD_VA_LSAX, StreamBuffer[i].SPUAddress);	// Loop to 1st half
		if (StreamBuffer[i].Ch2Flag!=0)
		{
			for (a=0;a<StreamBuffer[i].Ch2Flag;a++)
			{
				ch2=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[a]);
				core2=SOUND_GetCore(StreamBuffer[i].SPUChannel2[a]);
				sceSdSetAddr (core2 | ch2<<1 | SD_VA_LSAX, StreamBuffer[i].SPUAddress);	// Same for 2nd SPU channel.
			}
		}

		if (StreamAddress<pos)	// Playback position in 1st half?
		{
			if (StreamAddress>=SPUStart)
			{
				SOUND_SetCalcPlayingTime(i,pos,pos+StreamBuffer[i].IOPBufferHalfSize);
				SOUND_CopyValues(i,0);	// Yes..Trans data to 2nd half, Request Load for 1st half.
				SetIOPRequest(i);

			}
		}
	}

// This part handles the playing the SPU data if no more data loading is required (its all
// loaded into SPU RAM..)
// Swaps between first and second half of buffer, so that a looping file can play without
// requiring more data from the cd/host0.
// If the stream is once-off (non looping), it will just reach its end of sample marker and die

	else if (StreamBuffer[i].PlayingHalf==STREAM_SHORT)	// All audio data loaded into SPU RAM?
	{

		if (StreamAddress<pos)	// Current Playback position in 1st half?
		{
			sceSdSetAddr (core | ch<<1 | SD_VA_LSAX, pos);	// Keep looping in buffer half.
			if (StreamBuffer[i].Ch2Flag!=0)
			{
				for (a=0;a<StreamBuffer[i].Ch2Flag;a++)
				{
					ch2=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[a]);
					core2=SOUND_GetCore(StreamBuffer[i].SPUChannel2[a]);
					sceSdSetAddr (core2 | ch2<<1 | SD_VA_LSAX, pos);		// Same for 2nd SPU channel.
				}
			}
		}
		else
		{
			sceSdSetAddr (core | ch<<1 | SD_VA_LSAX, StreamBuffer[i].SPUAddress);
			if (StreamBuffer[i].Ch2Flag!=0)
			{
				for (a=0;a<StreamBuffer[i].Ch2Flag;a++)
				{
					ch2=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[a]);
					core2=SOUND_GetCore(StreamBuffer[i].SPUChannel2[a]);
					sceSdSetAddr (core2 | ch2<<1 | SD_VA_LSAX, StreamBuffer[i].SPUAddress);
				}
			}
		}
	}
}
#endif

/**********************************************************************************
SOUND_UpdateMemoryEndPosition
	If loading a resident sample (ie. a SFX or .MSB bank):
		This routine is called once it is loaded into SPU RAM. Then the address where
		the next resident sample (StreamMemoryEnd) is increased by the size of data
		that was loaded.
	If loading data into EE RAM:
		The EE write address is increased by the size of data loaded
	If loading data into IOP RAM:
		The IOP write address is increased by the size of data loaded
***********************************************************************************/
void SOUND_UpdateMemoryEndPosition(int i)
{
u_int size;

	size=StreamBuffer[i].TotalTranSize;
	if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_EE)
	{
		if (StreamEEFlag==0)
			StreamEEAddress+=size;	/* Modify Current EE Write Address */
	}
	else if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_IOP)
	{
		if (StreamIOPFlag==0)
			StreamIOPAddress+=size;	/* Modify Current IOP Write Address */
	}
	else if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_SPU)
	{
		size=StreamBuffer[i].TranSize;
		size+=15;
		size&=(unsigned int)(~15);
		if (StreamMemoryEndFlag==0)
			StreamMemoryEnd+=size;	/* Set new SPU RAM Address to load next resident SFX to. */
	}
}

/**********************************************************************************
SOUND_CopyValues
	Copies all relevent Stream info into a temp buffer.
	This is done before a SetIOPRequest is called.
	If any CD errors occure during the load of the requested data, these values
	are copied back into the stream. Therefore, forcing a load of the data again
	once the CD system is running again.

	Requires:	i		Stream number
				dir		0 = Copy stream data to temp buffer
						1 = Copy temp buffer to stream data
***********************************************************************************/

void SOUND_CopyValues(int i,int dir)
{
	if (dir==0)
	{
/* Make a copy of all relevent Stream information. */

		CD_ERR_SPUChan=StreamBuffer[i].SPUChannel;

		CD_ERR_Stream=(unsigned char)i;
		CD_ERR_FileSeek=StreamBuffer[i].FileSeek;
		CD_ERR_SPUAddress=StreamBuffer[i].SPUAddress;
		CD_ERR_PlayingHalf=StreamBuffer[i].PlayingHalf;
		CD_ERR_iStatus=StreamBuffer[i].iStatus;
		CD_ERR_FileRemainder=StreamBuffer[i].FileRemainder;
		CD_ERR_ChainPos=StreamBuffer[i].ChainPos;
		CD_ERR_FileID=StreamBuffer[i].FileID;
		CD_ERR_FileSize=StreamBuffer[i].FileSize;
		CD_ERR_SectorStart=StreamBuffer[i].SectorStart;
		CD_ERR_IOPCurrentAddress=StreamBuffer[i].IOPCurrentAddress;
		CD_ERR_IOPCurrentRemainder=StreamBuffer[i].IOPCurrentRemainder;
		CD_ERR_TransCounter=StreamBuffer[i].TransCounter;
		CD_ERR_LoopInfo=StreamBuffer[i].LoopInfo;
		CD_ERR_Req=StreamBuffer[i].RequestLoad;
		StreamBuffer[i].Error=1;
	}
	else if (dir==1)
	{
/* Copy temp information back into Stream */
		i=CD_ERR_Stream;

		StreamBuffer[i].SPUChannel=CD_ERR_SPUChan;
		StreamBuffer[i].FileSeek=CD_ERR_FileSeek;
		StreamBuffer[i].SPUAddress=CD_ERR_SPUAddress;
		StreamBuffer[i].PlayingHalf=CD_ERR_PlayingHalf;
		StreamBuffer[i].iStatus=CD_ERR_iStatus;
		StreamBuffer[i].FileRemainder=CD_ERR_FileRemainder;
		StreamBuffer[i].ChainPos=CD_ERR_ChainPos;
		StreamBuffer[i].FileID=CD_ERR_FileID;
		StreamBuffer[i].FileSize=CD_ERR_FileSize;
		StreamBuffer[i].SectorStart=CD_ERR_SectorStart;
		StreamBuffer[i].IOPCurrentAddress=CD_ERR_IOPCurrentAddress;
		StreamBuffer[i].IOPCurrentRemainder=CD_ERR_IOPCurrentRemainder;
		StreamBuffer[i].TransCounter=(unsigned char)CD_ERR_TransCounter;
		StreamBuffer[i].LoopInfo=(unsigned char)CD_ERR_LoopInfo;
		StreamBuffer[i].RequestLoad=CD_ERR_Req;
	}
}

/**********************************************************************************
SOUND_HandlePreTransfer

	Handles code for starting a stream, which in turn will request to the EE for a
	load. If the EE agrees, then the whole SPU buffer will be filled with data
***********************************************************************************/
void SOUND_HandlePreTransfer(int i)
{
int stat;

	if (IOPRequestLoad==-1)
	{
		StreamBuffer[i].iStatus=iSTATUS_preTRANSFER1;	/* Set new iSTATUS mode */

/* StartStream (using many values set up in SOUND_StartStream) */

		stat=StartStream(i,StreamBuffer[i].USR_SPUChannel,StreamBuffer[i].USR_StrID,
			StreamBuffer[i].USR_LInfo,StreamBuffer[i].USR_AR);
		if (stat==-1)
		{
			if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)
			{
				SPU_KeyStatus[StreamBuffer[i].USR_SPUChannel]=SPU_KEY_STATUS_OFF;
				MSERROR(("playing error\n"));
			}
		}
		else
		{
			if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)
				SPU_KeyStatus[StreamBuffer[i].USR_SPUChannel]=SPU_KEY_STATUS_USED_BY_STREAM;
			SetIOPRequest(i);	/* Make a request for data to be loaded for stream */
		}
	}
}

/**********************************************************************************
SOUND_CheckForEERequest

Handles the loading of streaming data. A request is sent to the EE (IOPRequestLoad=StreamNumber)
The EE then returns with either -1 (can't load) or StreamNumber (go ahead and load)

First check to make sure if stream that wants data is still active and that it is
the same data packet that the IOP requested (you may have restarted a stream, but
old data was being sent accross from the EE..not good..)
(There is a very slim chance that a SOUND_StopStream would be called after the IOP
Request and before the EE replies..)
***********************************************************************************/
int jh=0;
int SOUND_CheckForEERequest(void)
{
int i;

	if (SOUND_INTERNAL_FastLoad==SOUND_FASTLOAD_ON)	/* Is FASTLOAD enabled? */
		SOUND_HandleCDErrorChecking();	/* Pause active streams if a cd error occures.. */

	if (IOPRequestLoad!=-1)
	{
		if (SOUND_INTERNAL_FastLoad==SOUND_FASTLOAD_ON)	/* Is FASTLOAD enabled? */
		{
			if (StreamBuffer[IOPRequestLoad].ResidentFlag!=STREAM_LOAD_EE)	/* Not loading EE data.. */
			{
				if (StreamBuffer[IOPRequestLoad].LoadSource2==USE_INTERNAL_LOADING)	/* FASTLOAD FIX!! */
				{
					LoadFromEE=IOPRequestLoad;		/* Pretend EE has allowed request.. */
				}
			}
			else if (StreamBuffer[IOPRequestLoad].EEAllow==1)	/* Loading EE data, EE allows all requests.. */
			{
				if (StreamBuffer[IOPRequestLoad].LoadSource2==USE_INTERNAL_LOADING)	/* FASTLOAD FIX!! */
				{
					LoadFromEE=IOPRequestLoad;		/* Pretend EE has allowed request */
				}
			}
			else
			{
				jh=0;
				if (StreamBuffer[IOPRequestLoad].iStatus==iSTATUS_preTRANSFER1)
					StreamBuffer[IOPRequestLoad].iStatus=iSTATUS_INITIALISED;
				IOPRequestLoad=-1;
				LoadFromEE=0xffff;
				StreamBusy=0;
			}
		}
	}
	if ((LoadFromEE!=0xffff)&&(IOPRequestLoad==LoadFromEE))	/* Has EE replied to IOP with same stream ID? */
	{

/* Has a new Stream started on this stream channel since a request for more data has been asked for? */
/* Or has the stream stopped since the request for data was made? */

		if ((StreamBuffer[IOPRequestLoad].iStatus==iSTATUS_INITIALISED)||
			(StreamBuffer[IOPRequestLoad].Status==STREAM_STATUS_OFF))
		{
			IOPRequestLoad=-1;		/* Cancel IOP request */
			LoadFromEE=0xffff;		/* Cancel request sent from EE */
			StreamBusy=0;			/* Stream no longer busy */
		}
	}

/* This part checks to see if you have allowed EE data loading to take place. */
/* If not, the request is canceled to allow other streams to be processed still. */
/* This has been added so that you can have more control over where and when you want to load */
/* more EE data. The request for the EE stream will still be sent again if no other streams require */
/* data. */

	if (IOPRequestLoad!=-1)	/* Has a request been made by the IOP? */
	{
		if ((StreamBuffer[IOPRequestLoad].ResidentFlag==STREAM_LOAD_EE)&&(LoadFromEE!=IOPRequestLoad))
		{
			jh++;
			if (jh>7)	/* Give the EE a good chance of allowing a load of EE data. */
			{
				jh=0;	/* No response from EE to allow this load. Cancel request this time. */
				if (StreamBuffer[IOPRequestLoad].iStatus==iSTATUS_preTRANSFER1)
					StreamBuffer[IOPRequestLoad].iStatus=iSTATUS_INITIALISED;
				IOPRequestLoad=-1;
				LoadFromEE=0xffff;
				StreamBusy=0;
				return(0);
			}
		}
	}

/* EE may have allowed a load for a stream which is no longer playing. */
/* We need to ignore this EE request in this case */

	if ((LoadFromEE!=0xffff)&&(IOPRequestLoad==LoadFromEE))	/* Has EE replied to IOP with same stream ID? */
	{
		if(StreamBuffer[IOPRequestLoad].LoadSource2!=USE_INTERNAL_LOADING)	/* Using EXTERNAL loading? */
		{
			if (LastLoadCheck!=EXT_LD_CNTR)		/* Yes..But is it the same data packet? */
			{
				LoadFromEE=0xffff;	/* No..Abort. IOP will continue to request and wait for correct EE data packet. */
				return(0);	/* Return now, so StreamBusy isn't set to 0 */
			}
		}
	}

	if ((LoadFromEE!=0xffff)&&(IOPRequestLoad==LoadFromEE))	/* Data allowed by EE? */
	{
		if (LoadEECounter==MyCounter)	/* Double check to make sure that its a different request. */
		{
			jh=0;	/* Reset EE Data load counter */

/* Check for CD errors if using MultiStreams own CD loading code. */
/* We know at this point that no other system should be using the CD, so if the CD is not */
/* available, an error of some kind must have occured. */
			if (StreamBuffer[IOPRequestLoad].LoadSource2==USE_INTERNAL_LOADING)
			{
				SOUND_HandleCDErrorChecking();	/* Pause active streams if a cd error occures.. */
			}

			if (E_CDErrorCheck==1)	/* CD error occured? */
			{
				IOPRequestLoad=-1;	/* Yes. */
				LoadFromEE=0xffff;	/* Abort. */
			}
			else
			{
				IOPRequestLoad=-1;	/* At last..The data is valid! */
				i=LoadFromEE;		/* Clear request. */
				LoadFromEE=0xffff;
				if(StreamBuffer[i].LoadSource2==USE_EXTERNAL_LOADING)	/* Are we EXTERNAL loading? */
				{
					LoadDataFromDisk(i,(int)EXT_LD_SIZE,0);	/* Yes..Just handle data already in IOP */
				}
				else
				{
					MS_IntrFlag=4;
					HandleEERequest(i,0);	/* INTERNAL loading. Update stream and start loading.. */
					MS_IntrFlag=5;
				}
				MyCounter++;	/* Increase counter to keep everthing in sync. */
			}
		}

		if (StreamBusy==2)	/* Set to 2 when SOUND_GetStatus is called with a stream number */
			StreamBusy=0;	/* Set it back to 0 (off). */
		return(1);			/* Return 1 (Request processed) */
	}
	else if (StreamBusy==2)	/* Set to 2 when SOUND_GetStatus is called with a stream number */
		StreamBusy=0;		/* Set it back to 0. */
	return(0);				/* No request processed */
}

/**********************************************************************************
SOUND_HandleCDErrorChecking (V5)
	Used to be inline with function: StreamADPCM
	Does just what it says. If there is a problem with the CD when trying to load
	a file, pause all streams that are playing. Then, if everything seems to be
	back to normal, resume the streams.
	If this routine is called, it is because MultiStream has been allowed to load data
	from an EE request. If another routine starts loading data, an error would be
	detected here (as the CD drive is busy, when it shouldn't be), and the streams
	will pause. Care must be taken to make sure that any other CD loading code is
	kept in sync with MultiStreams CD loading.
	Notes:
	E_CDErrorCheck is cleared once the CD is valid again. This also restarts loading
	of the data for the stream which was trying to load when the CD error occured.
	I_CDErrorCheck is cleared once the CD has loaded the CD data required for the
	stream which was trying to load when the CD error occured.
	Only when both of these checks are cleared can proper playback resume (all
	streams where were paused during the CD error will be restarted at this point)

***********************************************************************************/
void SOUND_HandleCDErrorChecking(void)
{
	if (FilesOnCD==1)	/* Are any files being loaded from CD ? */
	{
		if ((USR_HALT_MULTISTREAM==0) && (SOUND_CD_I_ERROR==2))
		{
			CD_IDLE=0;		/* Block all CD loading / MultiStream stream updating */
			if (E_CDErrorCheck==0)	/* Are we currently in a CD Error pause state? */
			{
				CDError_PauseStreams();	/* No. Pause all streams */
				CDCheck=1;
				I_CDErrorCheck=1;		/* Set internal MultiStream CD Error flag */
				E_CDErrorCheck=1;		/* Set external MultiStream CD Error flag */
				MSERROR(("Tray opened? Kicked PS2?...\n"));
				SOUND_FastINTERNAL_RPC(cd_stream,0,0,2);	/* Tell the EE Fastload callback about this error */
			}
		}
	}
}

/**********************************************************************************
SOUND_RestartFromCDError
	Checks the current status of the CD. If its back to normal, restart the streams.
***********************************************************************************/
void SOUND_RestartFromCDError(void)
{
	if (E_CDErrorCheck==1)		/* Already paused due to CD error? */
	{
		if (StreamBuffer[CD_ERR_Stream].Error==1)	/* Was loading during the CD Error? */
		{
			SOUND_CD_I_ERROR=1;
		}
		else
			cd_stage=CD_STAGE_DONE;	// Stream no longer active..

		CD_IDLE=1;
		E_CDErrorCheck=0;				/* Startup any paused streams */
		IOPRequestLoad=-1;
		StreamBusy=0;		/* New */
		MSERROR(("SOUND_RestartFromCDError\n"));
		MSERROR(("CD playing resumed...\n"));
	}
}

/**********************************************************************************
SOUND_CheckCDError
	Checks current CD status

	Returns:	0		CD system available
				1		CD system not available (either loading, or ejected CD?)
***********************************************************************************/
void SOUND_CheckCDError(void)
{
int k;

	k=sceCdDiskReady(1);	/* Check CD status.. */
	if (k!=SCECdNotReady)
		CDCheck=0; 			/* Its ready! */
	else
		CDCheck=1;			/* No it isn't... */
}

/**********************************************************************************
SOUND_GetActiveStreamInfo
	Gets current SPU RAM playback address and SPU channel keystatus for any active
	stream. This info is sent back to the EE via the SOUND_GetStatus routine.
***********************************************************************************/
void SOUND_GetActiveStreamInfo(void)
{
int i;
int core,ch;
	for (i=0;i<UserMaxStreams;i++)
	{
		if (StreamBuffer[i].Status==STREAM_STATUS_ON)
		{
			if(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)	/* ADPCM audio stream? */
			{
				ch=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel);	/* SPU Channel */
				core=SOUND_GetCore(StreamBuffer[i].SPUChannel);		/* SPU Core */
				ch<<=1;
/* Current SPU playback address */
				StreamBuffer[i].SPUCurrent=(int)sceSdGetAddr((unsigned short)(core|ch|SD_VA_NAX));
/* Stream current envelope amplitude (set from ADSR values) */
				StreamBuffer[i].KeyStat=sceSdGetParam((unsigned short)(core|ch|SD_VP_ENVX));
			}
		}
	}
	for (i=0;i<24;i++)
	{
		SPU_NAX[i]=sceSdGetAddr((unsigned short)(0|i<<1|SD_VA_NAX));
		SPU_NAX[i+24]=sceSdGetAddr((unsigned short)(1|i<<1|SD_VA_NAX));
	}
}

/**********************************************************************************
SetIOPRequest
	The IOP wants to load in data for a stream.

	Depending on where the loading is going to take place:

	Internal - Set IOPRequestLoad to stream ID. This is passed to the EE and waits
				for the EE to send the same value back to continue.
	External - Calls the internal load routines JUST to get the file size/destination etc.
				This data is then passed back to the EE.

	Requires:	stream		Stream number
***********************************************************************************/
void SetIOPRequest(int stream)
{
int i;
int check;

/* If Stream is a CHILD, just call HandleEERequest and return. */
/* This is because the PARENT handles the actual loading of data. */
	if (StreamBuffer[stream].ParentChildFlag==SOUND_CHILD)
	{
		if ((StreamBuffer[stream].TransCounter!=StreamBuffer[StreamBuffer[stream].ParentStream].TransCounter)
			||(StreamBuffer[stream].iStatus==iSTATUS_PLAY)
			||(StreamBuffer[stream].iStatus==iSTATUS_preTRANSFER2))
		{
			HandleEERequest(stream,1);		/* More data already loading into IOP for child to use! */
		}
		else
		{
/* Parent has not loaded data yet, so move child back to INITIALISED stage if only just started. */
			if (StreamBuffer[stream].iStatus==iSTATUS_preTRANSFER1)
				StreamBuffer[stream].iStatus=iSTATUS_INITIALISED;
		}
		return;
	}

/* If Stream is a PARENT, make sure all of its CHILD streams are at the same stage before progressing. */
	if (StreamBuffer[stream].ParentChildFlag==SOUND_PARENT)	/* Is stream a PARENT ? */
	{
		for (i=0;i<UserMaxStreams;i++)		/* Yes.. */
		{
/* Check all CHILD counters to make sure that they are all at the same stage as their PARENT.. */
			if ((StreamBuffer[stream].ParentStream==StreamBuffer[i].ParentStream)
				&&(StreamBuffer[stream].TransCounter!=StreamBuffer[i].TransCounter))
			{
/* PARENT can not advance, as one or more of its children as still behind. */

				return;
			}
		}

		if (StreamBuffer[stream].iStatus==iSTATUS_preTRANSFER2)
		{
/* Data already loaded for interleaved MIH/MIB format. The first half of the buffer has already */
/* been transfered to SPU RAM, so now we just need to transfer the second half. */
/* Because the data is already in IOP RAM, there is no need to do a request to the EE for a load. */
			HandleEERequest(stream,1);
			return;
		}
	}
/* Stream is either a parent interleaved stream, or a normal mono stream. IOP asks EE if its OK to load data.. */


	if (StreamBuffer[stream].ParentChildFlag==SOUND_PARENT)	/* Is stream a PARENT ? */
	{
		for (i=0;i<UserMaxStreams;i++)		/* Yes.. */
		{
/* Check all CHILD counters to make sure that they are all at the same stage as their PARENT.. */
			if (StreamBuffer[stream].ParentStream==StreamBuffer[i].ParentStream)
			{
//				StreamBuffer[i].RequestLoad=0;
			}
		}
	}
//	else
		StreamBuffer[stream].RequestLoad=0;

	IOPRequestLoad=stream;

/* If loading from EXTERNAL (ie. not using MultiStreams CD/Host loading code), get relevent info to pass back to EE. */
/* Such as IOP RAM address, Size of data required..etc.. */
/* The EE then needs to transfer the required data to the IOP before sending its request back to MultiStream via the */
/* SOUND_GetStatus function. */

#if MS_TIMER

	SignalSema(gSem);	/* Signal StreamADPCM to be called */
#endif

	if (StreamBuffer[stream].LoadSource2==USE_EXTERNAL_LOADING)
	{


		EXTERNAL_CDLOAD_FLAG=CD_GET_FILE_INFO;	/* This makes LoadDataFromDisk just return info.. */

		check=HandleEERequest(stream,0);				/* Update stream, get info but no loading.. */
		if (check==-1)
			IOPRequestLoad=-1;

		if (StreamBuffer[stream].PlayingHalf==STREAM_SHORT)	/* Playing last part of data? */
			IOPRequestLoad=-1;					/* Yes..No need to request load from EE */

		if (StreamBuffer[stream].NoMoreLoading==1)	// MS7.22 All of file in IOP RAM.. No need to load data	
			IOPRequestLoad=-1;

//	if (IOPRequestLoad!=-1)
//printf("EXTERNAL loaded..%x!!!\n",stream);

		EXTERNAL_CDLOAD_FLAG=CD_TRANSFER_DATA;	/* Set flag back to normal. */
	}
	else
	{
/* Tell EE that MultiStream INTERNAL CD code is handling load */
		EXT_LD_DEST=STREAM_LOAD_MS;	/* (Can be read from the EE with SOUND_GetEXTERNALInfo) */
	}
}

/**********************************************************************************
SOUND_UpdateParentChild
	Updates parent or child counters if playing interleaved data	
	Requires:	i		Stream number
***********************************************************************************/
void SOUND_UpdateParentChild(int i)
{
/* Are we a PARENT, and have we actually loaded some data? */
	if ((StreamBuffer[i].ParentChildFlag==SOUND_PARENT)&&
		(StreamBuffer[i].iStatus!=iSTATUS_preTRANSFER1))
	{

/* Increase counter so children know that there is more data */
		StreamBuffer[i].TransCounter++;
	}
	else if ((StreamBuffer[i].ParentChildFlag==SOUND_CHILD)&&
			(StreamBuffer[i].iStatus!=iSTATUS_preTRANSFER1))
	{
	 	StreamBuffer[i].TransCounter=StreamBuffer[StreamBuffer[i].ParentStream].TransCounter;
	}
}

/**********************************************************************************
SOUND_StreamReadyToDie
	Handles final SPU transfer for data which requires no more loading from host/cd
	Requires:	i			Stream number
				BufferHalf	Which half of the buffer to transfer data to
				Size		Size of buffer
				flag		What sort of transfer is taking place (preload,preload2...)

Notes: Thanks to Shaun Mcabe for helping out with this routine!
***********************************************************************************/
void SOUND_StreamReadyToDie(int i,int BufferHalf,u_int Size,int flag)
{
u_int TranIOPAddr;
u_int TranSize;
u_int TranSPUAddr;
int a;
int *addr;
u_int size;
int offset=0;
u_int JT;



	cd_stream=i;	/* cd_stream normally set up in the CD loading code, but no loading required.. */

	if (StreamBuffer[i].PlayingHalf==STREAM_SHORT)	/* Stream data smaller than stream buffer ? */
	{
		return;	/* Exit (SPU_DMA_TRANS_FLAG is normally cleared once a IOP->SPU DMA is competed) */
	}

/* Loading Non-Audio data? */
	if ((StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_ADPCM)&& 
		(StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_PCM))
	{
/* Resident SFX into SPU RAM? */

/*		if ((SOUND_PriorityStream==i)&&(SOUND_PriorityStreamTime==SOUND_FORCE_SINGLE_TRANSFER)) */
/*			SOUND_PriorityStream=-1; */

		if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_SPU)
		{
			offset=(int)CheckVAGHeader(i,StreamBuffer[i].IOPCurrentAddress);	/* Check if loaded a VAG */
			JT=STREAM_SPU_SFX;	/* SPU Transfer Type */

			SOUND_HandleSPUTransfer(i,StreamBuffer[i].IOPCurrentAddress+offset,	/*Transfer data */
									StreamBuffer[i].SPUAddress,cd_jdatalen-offset,(int)JT);
			StreamBuffer[i].TotalTranSize+=(cd_jdatalen-offset);

		}
		else if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_EE)	/* Loading data to EE RAM? */
		{
/* INTERNAL loading? (Note that if EXTERNAL loading, data is already in the EE anyway..) */
			if (StreamBuffer[i].LoadSource2==USE_INTERNAL_LOADING)
			{
				if (SOUND_FastLoadDCache==1)
					SOUND_FastINTERNAL_RPC(i|0x8000,StreamBuffer[i].TotalTranSize,StreamBuffer[i].LastTranSize,1);

				SOUND_TransferDataToEE(StreamBuffer[i].IOPCurrentAddress,StreamBuffer[i].SPUAddress,
				StreamBuffer[i].LastTranSize);	/* Transfer data to EE RAM */

/* Set Info to pass back to EE (can be read from a SOUND_GetEXTERNALInfo call) */
				EE_LastTranSize=StreamBuffer[i].LastTranSize;
				StreamBuffer[i].TotalTranSize+=EE_LastTranSize;
				EE_TranCount++;
				SOUND_FastINTERNAL_RPC(i,StreamBuffer[i].TotalTranSize,StreamBuffer[i].LastTranSize,1);

			}

/* SPU Current playback address=Transfer Size */
			StreamBuffer[i].SPUCurrent=(int)Size;
/* SPUChannel is used to say which buffer "half" is playing. Can be used for double buffering */
/* your EE data loading (by using the SOUND_ModifyEETransferAddress) */

// Using FastLoad RPC? If so, set flag to shut down stream now, rather than wait for FlushIOPCommand to get data back
// to EE..
			if ((SOUND_INTERNAL_FastLoad==SOUND_FASTLOAD_ON)&&(SOUND_BIND_FastLoad!=0))
				StreamBuffer[i].SPUChannel=3;	// Don't wait for next flush before shutting down stream
			else
				StreamBuffer[i].SPUChannel=2;	// Wait for next flushIOPCommand before shutting down stream

		}
		else if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_IOP)	/* Loading data to EE RAM? */
		{
			StreamBuffer[i].TotalTranSize+=(unsigned int)Size;
		}
	}
	else
	{
/* flag=3..Stream is a MIB file, stream is smaller than half a buffer size. */
/* flag=1..Stream is a MIB file, stream is smaller than a full buffer size. */
		if ((flag==1)||(flag==3))
		{
			TranIOPAddr=StreamBuffer[i].IOPCurrentAddress+StreamBuffer[i].IntTransOffset;
			TranSize=StreamBuffer[i].IntTransSize;
			TranSPUAddr=StreamBuffer[i].SPUAddress;
		}
/* flag=2..Stream is a MIB file, stream not preloading (normal final transfer) */
		else if (flag==2)
		{
 			TranSize=StreamBuffer[i].IntTransSize;
			TranIOPAddr=StreamBuffer[i].IOPCurrentAddress+StreamBuffer[i].IntTransOffset;	/*(Size/2); */
			TranSPUAddr=StreamBuffer[i].SPUAddress+(TranSize*BufferHalf);
		}

/* flag=0.. must be 0..(Which is a normal ADPCM final file transfer..) */
		else if (flag==0)
		{

			TranSize=StreamBuffer[i].LastTranSize;
			TranIOPAddr=StreamBuffer[i].IOPCurrentAddress;
			offset=(int)CheckVAGHeader(i,TranIOPAddr);
			TranSPUAddr=StreamBuffer[i].SPUAddress+(BufferHalf*StreamBuffer[i].AllocHalfSize);
		}
		else
		{
			MSERROR(("SOUND_StreamReadyToDie. Unknown flag type %x\n",flag));
			return;
		}

/* FINAL marker needs to be set on last transfer. As you may just be playing a small section of a ADPCM file, */
/* which doesn't have an endmarker already stored within the data. */
/* Also, Interleaved data does not have end markers stored within the data (so that shorter files will still play */
/* to the end of the who interleaved file. */

/* Only set EndMarker if not transferring the first half of a two half buffer.. */
		if (flag!=1)
		{
			if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)	/* ADPCM data? */
			{
				if (StreamBuffer[i].MIBEnd==2)	/* MIB File? */
				{
					TranSize=StreamBuffer[i].MIBLoop;

#if (MS_LOAD62==0)
			 	 	_AdpcmSetMarkFINAL(i,TranIOPAddr,StreamBuffer[i].MIBLoop);
#endif
				}
				else
				{
/* Not a MIB file. Set end marker at the end of the data. */
/* Transfer size HAS to be at least 32 bytes, as _AdpcmSetMarkFINAL stores end marker info in */
/* the ADPCM data at offsets of -15 and -31 bytes from the end of the loaded data. */

					if (TranSize==16)	/* Only 16 bytes to transfer? */
					{
						addr=(int *)(TranIOPAddr+0);		//16
						*(addr++)=0;	/* Clear next 16 bytes also.. */
						*(addr++)=0;
						*(addr++)=0;
						*(addr++)=0;
						TranSize=32;	/* Set transfer size to 32. */
					}
#if (MS_LOAD62==0)
			 	 	_AdpcmSetMarkFINAL(i,TranIOPAddr,TranSize);		// Set loop FINAL marker.
#endif

				}
			}
			else if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)
			{

/* Last load of PCM once off data. Clear the rest of the buffer out, so you don't hear any */
/* left over audio from a previous load in the buffer half. */

				addr=(int *)TranIOPAddr;
				size=StreamBuffer[i].LastBufferSize-TranSize;
				for (a=0;a<(size/4);a++)
				{
					*(addr++)=0;
				}
			}
		}

/* Transfer data (either a full DMA transfer, or let DTS code handle transfer in between */
/* it doing DTS type stuff.. */
		JT=STREAM_SPU_READY_TO_DIE;		/* SPU Transfer Type */
		SOUND_HandleSPUTransfer(i,TranIOPAddr+offset,TranSPUAddr,TranSize-offset,(int)JT);
 	}
}


/**********************************************************************************
SOUND_StreamPreLoading
	Handles Stream preloading
	Requires:	i		Stream number
				Size	Buffer size
***********************************************************************************/
void SOUND_StreamPreLoading(int i, u_int Size)
{
u_int TranIOPAddr;
u_int TranSize;
u_int offset=0;
u_int JT;

	JT=0;
	TranIOPAddr=0;
	TranSize=0;
	if (StreamBuffer[i].LoopInfo!=STREAM_READY_TO_DIE) 	/* Flags already set if in this mode.. */
	{
		if (StreamBuffer[i].ParentChildFlag!=0)	/* Transferring interleaved data? */
		{
			JT=STREAM_SPU_NORMAL;		/* SPU Transfer type */
			TranSize=StreamBuffer[i].IntTransSize;
			TranIOPAddr=StreamBuffer[i].IOPCurrentAddress+StreamBuffer[i].IntTransOffset;
#if (MS_LOAD62==0)
			_AdpcmSetMarkSTART(i,StreamBuffer[i].IOPAddress+StreamBuffer[i].IntTransOffset);
#endif
		}
		else if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)
		{
/* IOP Address where data has been loaded to... */
			TranIOPAddr=StreamBuffer[i].IOPCurrentAddress;

/* Check for a VAG or MSV header. (Sets playback pitch if required) */
			offset=CheckVAGHeader(i,TranIOPAddr);

/* Skip VAG header if one is present (So we don't transfer it to SPU RAM..) */
			TranIOPAddr+=offset;

/* Check if the file is smaller than buffer. If so, it must also be looping also. */
/* We know this, as if it was once off and smaller than the buffer, then we would now be in */
/* SOUND_StreamReadyToDie now.. */

			JT=STREAM_SPU_NORMAL;		/* SPU Transfer type */

//			if ((StreamBuffer[i].LastTranSize-offset)<=Size)	/* Was the loaded file size smaller than the buffer? */
			if ((StreamBuffer[i].LastTranSize)<=Size)	/* Was the loaded file size smaller than the buffer? */
			{
//printf("small!!\n");
/* Set loop end marker at end of data */
				TranSize=StreamBuffer[i].LastTranSize-offset;
/* Set this to stop SOUND_CheckForHalfBufferPlayed trying to request more data */
				StreamBuffer[i].PlayingHalf=STREAM_PLAYING_SHORT_LOOP;
				JT=STREAM_SPU_SHORT_LOOP;	/* SPU Transfer type */
			}
			else
			{
/* Modify size of transfer if header found.. */

				TranSize=Size-offset;
			}
/* Set start and end loop markers in IOP buffer */

#if (MS_LOAD62==0)
			_AdpcmSetMarkSTART(i,StreamBuffer[i].IOPAddress+offset);
			_AdpcmSetMarkEND(i,StreamBuffer[i].IOPAddress+offset,TranSize);
#endif
		}
		else if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)	/* Streaming PCM data? */
		{
			TranSize=Size;
			TranIOPAddr=StreamBuffer[i].IOPCurrentAddress;
		}

		SOUND_HandleSPUTransfer(i,TranIOPAddr,StreamBuffer[i].SPUAddress,TranSize,(int)JT);
	}
}

/**********************************************************************************
CheckVAGHeader
	Checks first data load for a VAG header. If it has got one, read the pitch value
	and set the stream pitch to the same value. Also, modify the file offset and
	playsize, so if the stream loops, it skips the VAG header from this point onwards.

	Requires:	i			Stream channel number
				IOPAddr		Address of data loaded into IOP RAM

	Returns:	offset		0 (no VAG header found)
							48 (VAG header found, 48 is the offset to skip the header)

***********************************************************************************/
u_int CheckVAGHeader(int i,u_int IOPAddr)
{
u_int *ptr;
u_int val;
u_int Pitch;
u_int tmp;
int j;

	ptr=(u_int*)IOPAddr;
	val=*ptr;

/* Test for "VAGp" or "MSVp" text at the start of a VAG header.. */
	if ((val==0x70474156)||(val==0x7056534d))
	{
		if (StreamBuffer[i].VAGPitch==SOUND_VAG_PITCH_ON)	/* Are we allowed to set the pitch? */
		{
			ptr+=0x4;			/* Skip 16 bytes to get Pitch (in Hz) value. */
			val=(*ptr)>>8;
			tmp=(val&0xff0000)>>16;
			val&=0xff00;
			val|=tmp;			/* Rearrange data so that we have Pitch as correct 16bit value */

			Pitch=((val*0x1000)/48000);		/* Convert pitch (hz) to SPU pitch */

/* Set pitch of stream (depending if the stream is paused or not..) */
			if ((StreamBuffer[i].KeyOffFlag==0)&&(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM))
			{
				if (SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel]==0)
					StreamBuffer[i].Pitch=(short)Pitch;
				SPU_PausePitch[StreamBuffer[i].SPUChannel]=(unsigned short)Pitch;
				if (StreamBuffer[i].Ch2Flag!=0)
				{
					for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
					{
						SPU_PausePitch[StreamBuffer[i].SPUChannel2[i]]=(unsigned short)Pitch;
					}
				}
			}
		}

/* Modify stream start position and length for next time (if looping) */

		if (StreamBuffer[i].ResidentFlag==STREAM_RESIDENT)
			StreamBuffer[i].IOPCurrentRemainder-=48;

		StreamBuffer[i].FilePlaySize-=48;
		StreamBuffer[i].FileStart+=48;
/* Also modify Chain[0] values in the same way.. */
		StreamBuffer[i].ChainFileSeek[0]+=48;
		StreamBuffer[i].ChainPlaySize[0]-=48;

		return(48);
	}
	return(0);
}

/**********************************************************************************
SOUND_HandleSPUTransfer
	Handles any IOP->SPU transfer.
	If DTS is running, request the transfer.
	If not, start a normal DMA transfer.

	Requires:	i			Stream number (or -1 if not using a stream)
				IOPAddr		IOP address
				SPUAddr		SPU address
				Size		Size
***********************************************************************************/
#if (MS_LOAD62==0)

void SOUND_HandleSPUTransfer(int i,u_int IOPAddr, u_int SPUAddr, u_int Size,int TranType)
{
	if (i!=-1)	// Using a stream?
	{
		if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)	// Transferring PCM?
		{
		    if ((PCMStereo==SOUND_MONO)||(PCMStereo==SOUND_BITSTREAM))
			{
				if (SPUAddr!=PCMBuffer)	// Select buffer half to transfer
				{
					SPUAddr=PCMBuffer+(PCMBufferSize/2);
				}
			}
			PCMLastAddr=SPUAddr;		// Passed back to EE (SOUND_GetPCMInfo)
			Size=StreamBuffer[i].LastBufferSize;		// Size of transfer
			SOUND_TransferPCM(IOPAddr,SPUAddr,Size);	// Transfer PCM data
			return;
		}
	}

	if (SOUND_Transfer.TranMode!=SOUND_DTS_ON)		/* DTS currently OFF ? */
	{
		sceSdVoiceTransStatus(MS_SPU_DMA,SD_TRANS_STATUS_WAIT);

		SOUND_Transfer.Status=SOUND_LAST_TRANSFER;	// Do a normal DMA IOP->SPU Transfer
		sceSdVoiceTrans (MS_SPU_DMA, (SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA),
		(unsigned char *) IOPAddr,SPUAddr,Size);
	}
	else
	{
		SOUND_Transfer.IOPAddr=IOPAddr;	// Make a copy if everthing, and let DTS transfer data.
		SOUND_Transfer.SPUAddr=SPUAddr;
		SOUND_Transfer.Size=Size;
		SOUND_Transfer.Status=SOUND_TRANSFER;	// Current transfer mode
	}
	StreamBuffer[i].TranSize+=Size;	// Total amount transferred by stream
}
#else
/**********************************************************************************
SOUND_HandleSPUTransfer
	Handles any IOP->SPU transfer.
	If DTS is running, request the transfer.
	If not, start a normal DMA transfer.

	Requires:	i			Stream number (or -1 if not using a stream)
				IOPAddr		IOP address
				SPUAddr		SPU address
				Size		Size
				TranType	transfer type
***********************************************************************************/
void SOUND_HandleSPUTransfer(int i,u_int IOPAddr, u_int SPUAddr, u_int Size,int TranType)
{
	if (i!=-1)	/* Using a stream? */
	{
		if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)	/* Transferring PCM? */
		{
		    if ((PCMStereo==SOUND_MONO)||(PCMStereo==SOUND_BITSTREAM))
			{
				if (SPUAddr!=PCMBuffer)	/* Select buffer half to transfer */
				{
					SPUAddr=PCMBuffer+(PCMBufferSize/2);
				}
			}
			PCMLastAddr=SPUAddr;		/* Passed back to EE (SOUND_GetPCMInfo) */
			Size=StreamBuffer[i].LastBufferSize;		/* Size of transfer */
			SOUND_TransferPCM(IOPAddr,SPUAddr,Size);	/* Transfer PCM data */
			return;
		}
/*MS6 */
/*This code used to be where the ERROR message below is. Should be fine here now.. */
		SOUND_SetLoadRequest(i,IOPAddr,Size,(unsigned int)TranType);
		StreamBuffer[i].TranSize+=Size;	/* Total amount transferred by stream */
	}
	else
	{
		MSERROR(("SOUND_HandleSPUTrans..moved code error!\n"));

	}
}

#endif

/**********************************************************************************
SOUND_CheckLoopingMIB
	Checks to see if a MIB (interleaved ADPCM audio file) is looping. If so, send
	back the offset through the IOP data to where the LoopEndMarker will need to be
	set.

	Requires:	i		Stream
	Returns:			0	Not an MIB (or not about to transfer end of file data)
						n	Offset into IOP data to set LOOP flag
***********************************************************************************/
u_int SOUND_CheckLoopingMIB(int i)
{
	if (StreamBuffer[i].MIBEnd!=0)	/* Is this a looping MIB file? (!=0 7.1 fix..used to be ==1)*/ 
	{
		if (StreamBuffer[i].IOPCurrentAddress!=StreamBuffer[i].MIBAddress)
		{
			return(0);	/* Not transferring the last buffer of data.. */
		}
		StreamBuffer[i].MIBEnd=0;		/* Clear this flag (will be set again when stream loops) */
		return(StreamBuffer[i].MIBLoop);	/* Send back the offset through buffer. */
	}
	return(0);	/* Not a looping MIB file */
}

/**********************************************************************************
SOUND_StreamTransferToSPU
	Handles transfering first or second half of stream buffer from IOP->SPU
	Requires:	i			Stream number
				Size		Buffer size
				BufferHalf	Half of buffer which needs updating
***********************************************************************************/
void SOUND_StreamTransferToSPU(int i,u_int Size,u_int BufferHalf)
{
u_int TranIOPAddr;
u_int TranSize;
u_int TranSPUAddr;
int s;
u_int JT;

	if (StreamBuffer[i].ParentChildFlag!=0)	/* Transferring interleaved data? */
	{
/* Get correct offset into IOP data for required PARENT / CHILD stream. */
		TranIOPAddr=StreamBuffer[i].IOPCurrentAddress+StreamBuffer[i].IntTransOffset;
		TranSize=StreamBuffer[i].IntTransSize;
		TranSPUAddr=StreamBuffer[i].SPUAddress+BufferHalf*TranSize;	/* Set to correct buffer half */
	}
	else
	{
/* Not an MIB file */
		TranSize=Size;
		TranIOPAddr=StreamBuffer[i].IOPCurrentAddress;
		TranSPUAddr=StreamBuffer[i].SPUAddress+BufferHalf*StreamBuffer[i].AllocHalfSize;
	}

	if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)	/* Playing ADPCM? */
	{
		if (BufferHalf==STREAM_FIRST_HALF)	/* Playing the first half of the buffer? */
		{
/* Set StartLoop marker at the start of the stream data */
#if (MS_LOAD62==0)
			_AdpcmSetMarkSTART(i,StreamBuffer[i].IOPCurrentAddress+StreamBuffer[i].IntTransOffset);	// Mark first half of data (loop start)
#endif

/* Get size of last transfer if playing looping MIB files */
			s=(int)SOUND_CheckLoopingMIB(i);
			if(s!=0)			/* Is this a MIB file ? */
			{
				TranSize=(unsigned int)s;		/* Yes, use this size instead. */
			}
 /* Set EndLoop marker at end of stream data. */

#if (MS_LOAD62==0)
			_AdpcmSetMarkEND(i,StreamBuffer[i].IOPCurrentAddress+StreamBuffer[i].IntTransOffset,TranSize);		// Mark second half (loop end)
#endif
		}
		else if (BufferHalf==STREAM_SECOND_HALF)
		{
/* Set StartLoop marker at the start of the stream data */
#if (MS_LOAD62==0)
		_AdpcmSetMarkSTART(i,StreamBuffer[i].IOPCurrentAddress+StreamBuffer[i].IntTransOffset);	// Mark first half of data (loop start)
#endif
/* Get size of last transfer if playing looping MIB files */
			s=(int)SOUND_CheckLoopingMIB(i);
			if(s!=0)
			{
				TranSize=(unsigned int)s;
			}
 /* Set EndLoop marker at end of stream data. */
#if (MS_LOAD62==0)
		_AdpcmSetMarkEND(i,StreamBuffer[i].IOPCurrentAddress+StreamBuffer[i].IntTransOffset,TranSize);		// Mark second half (loop end)
#endif
		}
	}

/* Transfer data to SPU (either by a IOP->SPU DMA or handled by the DTS system) */

	JT=STREAM_SPU_NORMAL;		/* SPU Transfer type */

	SOUND_HandleSPUTransfer(i,TranIOPAddr,TranSPUAddr,TranSize,(int)JT);
}


/**********************************************************************************
UpdateStream

1. Transfer data buffer to SPU2 (half that has just been played)
2. Load data from CD/DVD/Host into buffer for next half
3. Set loop start/end markers in data from (2)
4. Sets endfunction routine to be called when loading has finished.
***********************************************************************************/
void UpdateStream (int i,int Size,int BufferHalf, cdFunc endfunc)
{
u_int TranIOPAddr;
u_int TranSize;
int b,a;
u_int offset;
u_int JT;

	a=StreamBuffer[i].TransCounter;	/* Amount of transfers a PARENT or CHILD has completed */

	SOUND_UpdateParentChild(i);		/* Update PARENT or CHILD Transfer counter */

	b=StreamBuffer[i].LoopInfo;		/* ONCE_OFF, LOOPING or READY_TO_DIE.. */

/* Are we ready to die (one more transfer of data to go, no more loading..) */
	if (b==STREAM_READY_TO_DIE)
	{
		if (StreamBuffer[i].ParentChildFlag!=0)		/* Are we a PARENT or CHILD (MIB file)? */
		{
			if ((BufferHalf==STREAM_PRELOAD)||(BufferHalf==STREAM_PRELOAD2))	/* Preloading? */
			{
				b=StreamBuffer[i].ParentStream;	/* b = CHILD's PARENT Stream (or the PARENT) */

/* If in PreLoad when STREAM_READY_TO_DIE, make all CHILD Streams STREAM_READY_TO_DIE */

				if (StreamBuffer[i].ParentChildFlag==SOUND_PARENT)
				{
					for (a=0;a<UserMaxStreams;a++)
					{
						if (b==StreamBuffer[a].ParentStream)
						{
							StreamBuffer[a].LoopInfo=StreamBuffer[i].LoopInfo;
							StreamBuffer[a].USR_LInfo=StreamBuffer[i].LoopInfo;
						}
					}
				}

/* Size of data is less than half the size of a stream buffer */
				if (StreamBuffer[b].LastTranSize<StreamBuffer[b].AllocSize)
				{
					if (BufferHalf==STREAM_PRELOAD)
					{

						SOUND_StreamReadyToDie(i,0,(unsigned int)Size,3);	/* Only one SPU transfer left to do..No more loading of data required */
					}
					else
					{
/* PRELOAD2, data already transfered from PRELOAD (above), so just clear IOP->SPU DMA flag */
					}
				}
				else
				{
					if (BufferHalf==STREAM_PRELOAD)	/* in PRELOAD state? */
					{
						BufferHalf=0;	/* Transfer to start of SPU RAM buffer. */
/* Transfer 1st half of ADPCM data */
						SOUND_StreamReadyToDie(i,0,StreamBuffer[i].IntTransSize,1);
/* Move IOP address to position of 2nd half of ADPCM data and transfer it to SPU RAM. */
						StreamBuffer[i].IOPCurrentAddress+=StreamBuffer[b].AllocHalfSize;
						SOUND_StreamReadyToDie(i,1,StreamBuffer[i].IntTransSize,2);
					}
				}
			}
			else	/* MIB file transfer already preloaded, so this is a normal one shut down.. */
			{
				SOUND_StreamReadyToDie(i,BufferHalf,(unsigned int)Size,2);	/* Transfer final data packet */

/* Make all CHILD Streams STREAM_READY_TO_DIE */
				if (StreamBuffer[i].ParentChildFlag==SOUND_PARENT)
				{
					for (a=0;a<UserMaxStreams;a++)
					{
						if (StreamBuffer[i].ParentStream==StreamBuffer[a].ParentStream)
						{
							StreamBuffer[a].LoopInfo=StreamBuffer[i].LoopInfo;
							StreamBuffer[a].USR_LInfo=StreamBuffer[i].LoopInfo;
						}
					}
				}
			}

		}
		else	/* Stream is NOT a MIB file (just a normal mono ADPCM or PCM file) */
		{
			if ((BufferHalf==STREAM_PRELOAD)||(BufferHalf==STREAM_PRELOAD2))
			{
				BufferHalf=0;
			}
			SOUND_StreamReadyToDie(i,BufferHalf,(unsigned int)Size,0);	/* Transfer final data packet */
		}

/* Mark stream playing half as SHORT (as its only got a short while to play for..!) */

		StreamBuffer[i].PlayingHalf=STREAM_SHORT;

	  	StreamBusy=0;	/* Stream no longer busy */
		endfunc(1);		/* Call CD end function. */
		return;
	}

/* Streaming non-audio data? */
	if ((StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_ADPCM)&&
		(StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_PCM))
	{
		if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_SPU)	/* Resident SPU SFX ? */
		{
			offset=CheckVAGHeader(i,StreamBuffer[i].IOPCurrentAddress);	/* Loaded a VAG header? */
			Size-=(int)offset;	/* Make transfer size smaller */

			JT=STREAM_SPU_SFX;	/* SPU Transfer type */

			SOUND_HandleSPUTransfer(i,(unsigned int)StreamBuffer[i].IOPCurrentAddress+offset,StreamBuffer[i].SPUAddress,(unsigned int)Size,(int)JT);
			StreamBuffer[i].TotalTranSize+=(u_int)Size;

			StreamBuffer[i].SPUAddress+=(unsigned int)Size;	/*Move on through SPU RAM. */
		}
		else if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_EE)	/* EE RAM data? */
		{
/* INTERNAL loading? (EXTERNAL loading would mean that the data is already in EE RAM anyway) */
			if (StreamBuffer[i].LoadSource2==USE_INTERNAL_LOADING)
			{
/* Transfer data to EE */

				if (SOUND_FastLoadDCache==1)
					SOUND_FastINTERNAL_RPC(i|0x8000,StreamBuffer[i].TotalTranSize,(unsigned int)Size,1);
				SOUND_TransferDataToEE(StreamBuffer[i].IOPCurrentAddress,StreamBuffer[i].SPUAddress,(unsigned int)Size);
				EE_LastTranSize=(unsigned int)Size;	/* Info can be read from EE (SOUND_GetEXTERNALInfo) */
				StreamBuffer[i].TotalTranSize+=(unsigned int)Size;
				EE_TranCount++;

				StreamBuffer[i].SPUAddress = SOUND_FastINTERNAL_RPC(i,StreamBuffer[i].TotalTranSize, (unsigned int)Size,0);
			}
			else
				StreamBuffer[i].SPUAddress+=(unsigned int)Size;	/* Increase EE RAM transfer position */

			StreamBuffer[i].SPUCurrent=Size;	/* SPUCurrentAddress=TransferSize if EE loading */
			StreamBuffer[i].SPUChannel++;
			StreamBuffer[i].SPUChannel&=1;		/* Toggle value so EE knows when more data has arrived */
		}
		else if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_IOP)	/* IOP RAM data ? */
		{
			StreamBuffer[i].TotalTranSize+=(unsigned int)Size;
		}
	}
	else if (BufferHalf==STREAM_PRELOAD)	/* INITIAL PRE-LOAD TO FILL SPU BUFFER */
	{
		SOUND_StreamPreLoading(i,(unsigned int)Size);		/* Handle preloading of audio data */
	}
	else if (BufferHalf==STREAM_PRELOAD2)	/* ONLY used for interleaved (MIB) data */
	{
		TranSize=StreamBuffer[i].IntTransSize;

		TranIOPAddr=StreamBuffer[i].IOPCurrentAddress+StreamBuffer[i].IntTransOffset+(Size/2);
/* Set END OF LOOP marker for second half of interleaved preloaded data. */
#if (MS_LOAD62==0)
		_AdpcmSetMarkEND(i,StreamBuffer[i].IOPAddress+StreamBuffer[i].IntTransOffset+(Size/2),TranSize);
#endif
		JT=STREAM_SPU_NORMAL;

// THIS IS NEW...
		offset=(int)SOUND_CheckLoopingMIB(i);
		if (offset!=0)
			TranSize=offset;
// END OF NEW

		SOUND_HandleSPUTransfer(i,TranIOPAddr,StreamBuffer[i].SPUAddress+TranSize,TranSize,(int)JT);
	}
	else
	{
/* Just a normal transfer */

		SOUND_StreamTransferToSPU(i,(unsigned int)Size,(unsigned int)BufferHalf);
	}

/* NEW!! Makes sure that CHILD knows its PARENTs status (for STREAM_READY_TO_DIE) */
	if (StreamBuffer[i].ParentChildFlag==SOUND_CHILD)
	{
		StreamBuffer[i].LoopInfo=StreamBuffer[StreamBuffer[i].ParentStream].LoopInfo;
	}

  
/* If preloading first half of buffer for interleaved data, exit now, as you still need to transfer second */
/* half of data to SPU too. Second half of data is already loaded into IOP, and just needs transferring to */
/* SPU. But it can't do this, until the first half is completed */
	if ((StreamBuffer[i].ParentChildFlag!=0)&&(BufferHalf==STREAM_PRELOAD))
	{
		endfunc(1);
		return;
	}

	StreamBuffer[i].IOPCurrentAddress+=(unsigned int)Size;		/* Set new IOP source data address */
	StreamBuffer[i].IOPCurrentRemainder-=(unsigned int)Size;		/* decrease counter (bit pointless..) */

	if (StreamBuffer[i].ResidentFlag!=STREAM_LOAD_IOP)	/* IOP Loads directly to specified address, NOT stream buffer. */
	{
		if (StreamBuffer[i].IOPCurrentRemainder<=0)		/* data read to the end of IOP memory ? */
		{
			StreamBuffer[i].IOPCurrentRemainder=StreamBuffer[i].AllocSize; 	/* reset size */
			StreamBuffer[i].IOPCurrentAddress=StreamBuffer[i].IOPAddress;	/* point source address to start of IOP buffer */
		}
	}

	if ((BufferHalf==STREAM_PRELOAD)||(BufferHalf==STREAM_PRELOAD2)||(StreamBuffer[i].ParentChildFlag==SOUND_CHILD))
	{
		endfunc(1);
		return;
	}

/* MS6 - move this to the start of this routine.? */

	if (E_CDErrorCheck==1)	/* CD error detected? Don't load more data from CD. */
		return;

	endfunc(1);
}


/* Called from HandleEEEvent */
void SOUND_StartLoad(int i,cdFunc endfunc)
{
	if ((StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_ADPCM)&&
		(StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_PCM))
		LoadDataFromDisk(i,(int)StreamBuffer[i].AllocSize,endfunc);	/* Data now loaded into correct half of IOP data */
	else
		LoadDataFromDisk(i,(int)StreamBuffer[i].AllocHalfSize,endfunc);	/* Data now loaded into correct half of IOP data */
}

void cdend_LoadedFirstHalf(void)
{
	if (StreamBuffer[cd_stream].iStatus==iSTATUS_PLAYING)
	{

			UpdateStream(cd_stream,(int)StreamBuffer[cd_stream].AllocHalfSize, STREAM_FIRST_HALF,
			cdend_FirstHalfPlayed);
	}
	else
	{
		StreamBusy=0;	/* Stream status is wrong (stream stopped, or started again..) */
	}
}

void cdend_LoadedNonAudio(void)
{
	if (StreamBuffer[cd_stream].iStatus==iSTATUS_PLAYING)
	{
			UpdateStream(cd_stream,(int)StreamBuffer[cd_stream].AllocSize, STREAM_SECOND_HALF,
			cdend_SecondHalfPlayed);
	}
	else
	{
		StreamBusy=0;	/* Stream status is wrong (stream stopped, or started again..) */
	}
}

void cdend_LoadedSecondHalf(void)
{
	if (StreamBuffer[cd_stream].iStatus==iSTATUS_PLAYING)
	{
			UpdateStream(cd_stream,(int)StreamBuffer[cd_stream].AllocHalfSize, STREAM_SECOND_HALF,
			cdend_SecondHalfPlayed);
	}
	else
	{
		StreamBusy=0;	/* Stream status is wrong (stream stopped, or started again..) */
	}
}

/**********************************************************************************
CDStream - Return function
cdend10 - Called if a full buffer of data has been loaded.
(either becuase there is enough data left to be read from file, or the file is
looped or chained to another..)
***********************************************************************************/
void cdend_StreamFullBuffer()
{
	endfunc_load(1);	/* call routine passed to UpdateStream */
}

/**********************************************************************************
CDStream - Return function
cdend12 - Called if data load gets to the end of the stream AND the stream is once_off
Updated Monday/10th/Feb/2002: Now handles CHILD streams.
***********************************************************************************/
void cdend_StreamOnceOff()
{
/*printf("JAP Stream ready to die callback!\n"); */

	if ((StreamBuffer[cd_stream].ResidentFlag==STREAM_AUDIO_PCM)||
		(StreamBuffer[cd_stream].ResidentFlag==STREAM_AUDIO_ADPCM))
	{
		if (StreamBuffer[cd_stream].LoopInfo==STREAM_ONCE_OFF)
		{
	  		StreamBuffer[cd_stream].LoopInfo=STREAM_READY_TO_DIE;
			endfunc_load(1);
		}
	}
	else
	{
  		StreamBuffer[cd_stream].LoopInfo=STREAM_READY_TO_DIE;
		endfunc_load(1);	/* Call routine passed to UpdateStream */
	}
}

/**********************************************************************************
SOUND_GetFileLSN
Returns the LSN (start sector value of file) of given file ID

Requires:	i		FileID
Returns:	lsn		Logical Sector Number (LSN) of file on CD (0 if on host)

***********************************************************************************/
u_int SOUND_GetFileLSN(int i)
{
ADPCMFILE *File;

	i=GetFileID(i);		/* Returns index through ADPCM_FileNames for corresponding FileID */
	File=JADPCM;
	File+=i;			/* Move to correct file record */
	return(File->lsn);	
}

/**********************************************************************************
SOUND_CopyEXTERNALData
Copies information for EXTERNAL file loading into area to be copied back to EE

Requires:	i	=	Stream number
***********************************************************************************/
void SOUND_CopyEXTERNALData(int i)
{
int a;
	EXT_LD_IOP_ADDR=StreamBuffer[i].IOPCurrentAddress;
	EXT_LD_CHAIN_COUNT=StreamBuffer[i].ChainCount;		/* Amount of samples in chain. */
	EXT_LD_CHAIN_POS=StreamBuffer[i].ChainPos;			/* Current position through chain */
	EXT_LD_REMAIN=StreamBuffer[i].FileRemainder;		/* Amount left to play from file */
	EXT_LD_SEEK[0]=StreamBuffer[i].ChainFileSeek[0];	/* start position into file */
	EXT_LD_OFFSET=StreamBuffer[i].FileSeek;				/* current seek position into file */
	EXT_LD_PLAYSIZE[0]=StreamBuffer[i].ChainPlaySize[0];	/*FilePlaySize; */
	EXT_LD_ID[0]=(unsigned int)StreamBuffer[i].ChainList[0];
	EXT_LD_LSN[0]=StreamBuffer[i].SectorStart;			/* LSN of file on CD */
	for (a=1;a<StreamBuffer[i].ChainCount+1;a++)
	{
		EXT_LD_SEEK[a]=StreamBuffer[i].ChainFileSeek[a];	/* List of all Chaining Seek positions */
		EXT_LD_ID[a]=(unsigned int)StreamBuffer[i].ChainList[a];		/* List of all Chaining ID's */
		EXT_LD_PLAYSIZE[a]=StreamBuffer[i].ChainPlaySize[a];	/* List of all Chaining Play Sizes */
		EXT_LD_LSN[a]=SOUND_GetFileLSN((int)EXT_LD_ID[a]);	/* List of all LSN's for Chaining streams */
	}

/* THIS MUST BE DONE AT THE END OF THIS ROUTINE! */
/* OTHERWISE, IT IS POSSIBLE FOR SOUND_GetStatus TO BE CALLED MID-WAY THROUGH THIS ROUTINE, WHICH WOULD */
/* THEN HAVE STRANGE EFFECTS, WHERE THE EE WOULD BE LOADING DATA USING INCORRECT FILE ID'S OR SIZES. */

	EXT_LD_CNTR++;		/* Counter which is sent back to EE (used as a reference) */
	EXT_LD_CNTR&=63;	/* (AKA SOUND_IOP_DATA_CHECK) */

}
			 
/**********************************************************************************
LoadDataFromDisk

Requires:	i	=	Stream number
   			Size = amount of data to load

Returns:	0	=	complete
			-1	=	failed
***********************************************************************************/
int LoadDataFromDisk(int i,int Size,cdFunc endfunc)
{
int remain;
int a;
u_int DestAddr;


#if 1
	if (StreamBuffer[i].NoMoreLoading==1)	// MS7.22 All of file in IOP RAM.. No need to load data
	{
		cd_stream=i;	/* Not audio data, so no waiting for playback to finish required */
		endfunc(1);
		return(0);
	}
#endif


	if (EXTERNAL_CDLOAD_FLAG==CD_GET_FILE_INFO)		/* EXTERNAL Loading - get info to pass to EE */
	{
/* Copy current stream status info into buffer to be passed to EE */
		SOUND_CopyEXTERNALData(i);

//printf("CopyEXTERNAL %x!\n",i);

/* Note: If its not EE data, all other data MUST go to IOP RAM anyway.. */
		if (StreamBuffer[i].ResidentFlag==STREAM_LOAD_EE)
			EXT_LD_DEST=STREAM_LOAD_EE;	/* Set flag passed to EE to say that data goes to EE RAM */
		else
			EXT_LD_DEST=STREAM_LOAD_IOP;	/* Set flag passed to EE to say data goes to IOP RAM */
	}

	if (StreamBuffer[i].LoopInfo==STREAM_READY_TO_DIE)	/* Stream had its final transfer? */
	{

		StreamBuffer[i].PlayingHalf=STREAM_SHORT;	/* Yes, change Playing half.. */
		StreamBusy=0;	/* Tells EE that Stream loading from CD has finished */
	 	if ((StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_ADPCM)&&
			(StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_PCM))
		{
			cd_stream=i;	/* Not audio data, so no waiting for playback to finish required */
			endfunc(1);
		}
		return(0);
	}

	remain=(int)StreamBuffer[i].FileRemainder;	/* Amount of data left to load from current file */

	a=StreamBuffer[i].ChainPos;				/* Current position through chain */

	DestAddr=StreamBuffer[i].IOPCurrentAddress;

	if ((StreamBuffer[i].LoadSource2==USE_EXTERNAL_LOADING)
	&&(EXTERNAL_CDLOAD_FLAG!=CD_GET_FILE_INFO))
	{
		remain=(int)StreamBuffer[i].LastTranSize;
	}
	else
	{
		StreamBuffer[i].LastBufferSize=(unsigned int)Size;	/* Buffer size required by stream */
		while(a<StreamBuffer[i].ChainCount)		/* Loop through all streams in chain. */
		{
			remain+=(int)StreamBuffer[i].ChainPlaySize[a+1];	/* Add lengths of all other files in chain (if any) */
			a++;
		}

		if (StreamBuffer[i].AppendFlag==1)
		{
			remain+=(int)StreamBuffer[i].AP_FileRemainder;
		}
	}

	StreamBuffer[i].LastTranSize=(unsigned int)remain;	/* Total transfer size for all chain files */

/* Note: The only time that 0 would be passed for endfunc is when allowing a load for EXTERNAL */
/* loading. But, endfunc will already be set up correctly, due to this routine being called when */
/* getting external info (EXTERNAL_CDLOAD_FLAG==CD_GET_FILE_INFO) */

	if (endfunc!=0)				/* Should a routine be called when loading is completed? */
		endfunc_load=endfunc;	/* Yes. Set routine to call.. */

/* Is remaining data in file(s) greated that stream buffer size? */
/* Or are we looping (which will always require a full buffer) */

	if ((remain>Size)||	(StreamBuffer[i].LoopInfo==STREAM_LOOPING))
	{
		if (EXTERNAL_CDLOAD_FLAG==CD_GET_FILE_INFO)	/* EXTERNAL Loading - get info */
		{
			EXT_LD_SIZE=(unsigned int)Size;	/* Info sent to EE (Size of a full buffer or buffer half) */
		}
		else
		{
			_CdStreamStart((int)StreamBuffer[i].SectorStart,	/* Load full buffer of data */
			(int)StreamBuffer[i].FileSeek, (int)DestAddr,
			(int)StreamBuffer[i].FileSize, Size, i, (void*)cdend_StreamFullBuffer);
		}
	}
	else	/* Data required is less than the buffer size */
	{
		if (StreamBuffer[i].LoopInfo==STREAM_ONCE_OFF)		/* Is the stream once off (non-looping) ? */
		{

//printf("Remain=%x, Size %x\n",remain,Size);
			if (EXTERNAL_CDLOAD_FLAG==CD_GET_FILE_INFO)
			{
				EXT_LD_SIZE=(unsigned int)remain;	/* Info sent to EE (remaining size of data to load) */
			}
			else
			{
				_CdStreamStart((int)StreamBuffer[i].SectorStart,
				(int)StreamBuffer[i].FileSeek, (int)DestAddr,
				(int)StreamBuffer[i].FileSize, remain, i,(void*) cdend_StreamOnceOff);
			}
		}
		else if ((StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_ADPCM)&&	/* Loading a resident Sample? */
				(StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_PCM))
		{
/* Same as above. Could put into same part of code.. */
			if (EXTERNAL_CDLOAD_FLAG==CD_GET_FILE_INFO)
			{
				EXT_LD_SIZE=(unsigned int)remain;	/* Load remaining data for non-audio data */
			}
			else
			{
				_CdStreamStart((int)StreamBuffer[i].SectorStart,
				(int)StreamBuffer[i].FileSeek, (int)DestAddr,
				(int)StreamBuffer[i].FileSize, remain, i, (void*)cdend_StreamOnceOff);
			}
		}
		else
		{
			return(-1);	/* Failed.. */
		}
	}
	return(0);	/* OK.. */
}

/**********************************************************************************
OpenADPCMFile
	requires:
				Stream	=	Stream number
				StreamID =	ID corresponding to filename in list ADPCM_FileNames
				tempflag =	0 (store values in "real" area) 1 (store in temp area)

	returns:
				0	=	opened file sucessfully
				-1	=	failed
***********************************************************************************/
int OpenADPCMFile(int Stream,int StreamID,int tempflag)
{
ADPCMFILE *File;

	File=JADPCM;
	File+=StreamID;

	if (File->size==0)	/* Is file valid? */
		return(1);		/* FILE NOT FOUND ERROR */


/* Called from SOUND_PlayStream  */
/* Put into temp area, as they are set up in StartStream routine for real. */
/* But don't want to call this routine from StartStream, as you could use the routine */
/* SOUND_SetFileOffsetAndPlaySize between each SOUND_PlayStream function to set different */
/* streams using the same file, but starting and finishing from different places.. */
/* Also called from SOUND_AppendStream */
	if (tempflag==STREAM_LOAD_TO_TEMP)
	{
		StreamBuffer[Stream].TempSeek=File->PlayOffset;
		StreamBuffer[Stream].TempFileSize=File->size;
		StreamBuffer[Stream].TempPlaySize=File->PlaySize;
	}
	else if (tempflag==STREAM_GET_INFO)
	{
/* Called from SOUND_HandleCDEvent if chaining.. */
		StreamBuffer[Stream].FileSeek=File->PlayOffset;
		StreamBuffer[Stream].FileStart=File->PlayOffset;		/*V5 */
		StreamBuffer[Stream].FilePlaySize=File->PlaySize;
		StreamBuffer[Stream].FileRemainder=File->PlaySize;
		StreamBuffer[Stream].FileSize=File->size;
	}
	if ((tempflag==STREAM_GET_INFO)||(tempflag==STREAM_GET_OTHER_INFO))	/* 2 or 0 passed ... */
	{
/* Called from StartStream (this info can't be changed once it is set initially..) */
/* Called from SOUND_AppendStream */
		StreamBuffer[Stream].SectorStart=File->lsn;
		StreamBuffer[Stream].Source=File->Source;
	}
	else if (tempflag==STREAM_GET_APPEND_INFO)	// New 7.21 - Thanks Y_Kousu!
	{
        StreamBuffer[Stream].AP_FileStart = File->PlayOffset;
        StreamBuffer[Stream].AP_FilePlaySize = File->PlaySize;
        StreamBuffer[Stream].AP_FileSeek = File->PlayOffset;
        StreamBuffer[Stream].AP_FileRemainder = File->PlaySize;
        StreamBuffer[Stream].AP_FileSize = File->size;
        StreamBuffer[Stream].SectorStart = File->lsn;
        StreamBuffer[Stream].Source = File->Source;
	}

	return(0);
}

/**********************************************************************************
AdpcmSetMasterVolume
***********************************************************************************/
void AdpcmSetMasterVolume (int core, unsigned int vol)
{
int dis, oldstat;

	dis = CpuSuspendIntr(&oldstat);
    sceSdSetParam ((unsigned short)(core | SD_P_MVOLL), (unsigned short)vol);	/* Set Master Volume LEFT */
    sceSdSetParam ((unsigned short)(core | SD_P_MVOLR), (unsigned short)vol);	/* Set Master Volume RIGHT */
	if(dis == KE_OK) CpuResumeIntr(oldstat);
    return;
}


/**********************************************************************************
SOUND_SetStreamBuffer

	(Function written by Cory Bloyd. 7/Jan/2004)

	Sets IOP memory for Streamplayer to use.

	requires:
		strm	= Stream number to allocate memory for
		iopaddr = IOP Memory Address
		iopsize	= Size of IOP buffer
		spuaddr = SPU Memory Address
		spusize	= Size of SPU buffer
	returns:
		nothing, but fills in the StreamBuffer[i].IOPAddress values.
***********************************************************************************/
void SOUND_SetStreamBuffer(int strm, u_int iopaddr, u_int iopsize, u_int spuaddr, u_int spusize)
{
	if (StreamBuffer[strm].MemoryAllocated==0)	/* Stream buffer allocated? */
	{
		if (iopaddr != 0 && iopsize!=0)
		{
			StreamBuffer[strm].IOPAddress = iopaddr;
			StreamBuffer[strm].IOPAllocAddress = iopaddr;
			StreamBuffer[strm].MemoryAllocated = 2; /* Set flag to say memory was pre-allocated */
		}

		StreamBuffer[strm].MaxSize = iopsize;			/* Used for SOUND_ResizeStreamBuffer */
		StreamBuffer[strm].AllocSize = iopsize;			/* Current allocated size */
		StreamBuffer[strm].AllocHalfSize = iopsize/2;	/* Size of half of the buffer */
		StreamBuffer[strm].AllocSPUAddress = spuaddr;	/* SPU address to transfer data from buffer to. */
		StreamBuffer[strm].SPUBufferSize = spusize;
	}
	else
	{
		MSERROR(("ERROR: SOUND_SetStreamBuffer\n"));
		MSERROR(("IOP Buffer already allocated for StreamBuffer %x\n", strm));
	}
}

/**********************************************************************************
SOUND_AllocateStreamBuffer
	Allocates IOP memory for Streamplayer to use.

	requires:
		i		=	Stream number to allocate memory for
		spuaddr =	SPU Memory Address
		size	=	Size of buffer to allocate
	returns:
		nothing, but fills in the StreamBuffer[i].IOPAddress values.
***********************************************************************************/
void SOUND_AllocateStreamBuffer(int i,u_int spuaddr,u_int size)
{
int oldstat;

	if (StreamBuffer[i].MemoryAllocated==0)	/* Stream buffer allocated? */
	{
		if (size!=0)
		{
		    CpuSuspendIntr (&oldstat);	/* No..Suspend interupt around memory allocation */
			StreamBuffer[i].IOPAddress = (unsigned int) AllocSysMemory (SMEM_Low, size, NULL);
		    CpuResumeIntr (oldstat);	/* Resume interrupts after memory allocation */

			if ((void*)StreamBuffer[i].IOPAddress==NULL)
			{
				MSERROR(("ERROR: SOUND_AllocateStreamBuffer\n"));
				MSERROR(("Memory allocation failed (requested size 0x%x)\n",size));
				return;
			}
			StreamBuffer[i].MemoryAllocated=1;	/* Set flag to say memory is allocated */
			StreamBuffer[i].IOPAllocAddress=StreamBuffer[i].IOPAddress;
		}
		StreamBuffer[i].MaxSize=size;		/* Used for SOUND_ResizeStreamBuffer */
		StreamBuffer[i].AllocSize = size;	/* Current allocated size */
		StreamBuffer[i].AllocHalfSize = size/2;	/* Size of half of the buffer */
		StreamBuffer[i].AllocSPUAddress=spuaddr;	/* SPU address to transfer data from buffer to. */
		StreamBuffer[i].SPUBufferSize=size;
	}
	else
	{
		MSERROR(("ERROR: SOUND_AllocateStreamBuffer\n"));
		MSERROR(("IOP Buffer already allocated for StreamBuffer %x\n",i));
	}
}

/**********************************************************************************
SOUND_ResizeSPUBuffer
	Allows you to resize the ADPCM stream SPU buffer size. So, for example, you could have
	a 64K IOP buffer, where data is loaded into. Then, this could be piped down to a 4K
	SPU RAM buffer. So, you save LOADS(!) of SPU RAM this way, compared to pre V6 versions.
	(Up to 90% smaller SPU RAM buffers...)

	Requires:	i		Stream
				size	New SPU RAM size to use
						(pass 0 to use default size when calling SOUND_AllocateStreamBuffer)
***********************************************************************************/
void SOUND_ResizeSPUBuffer(int i,u_int size)
{
#if (MS_LOAD62==0)
MSERROR(("SOUND_ResizeSPUBuffer\n"));
MSERROR(("FUNCTION NOT AVAILABLE\n"));
return;
#endif

 	if (SOUND_CheckStream(i)!=0)
	{
		MSERROR(("ERROR: SOUND_ResizeSPUBuffer\n"));
		MSERROR(("Stream 0x%x out of range\n",i));
		return;
	}

	if (size>StreamBuffer[i].AllocSize)
	{
		MSERROR(("ERROR: SOUND_ResizeSPUBuffer\n"));
		MSERROR(("Size (0x%x) is greater than IOP buffer (0x%x)\n",size,StreamBuffer[i].AllocHalfSize));
		return;
	}

 	if (StreamBuffer[i].ParentChildFlag!=0)
 	{
 		MSERROR(("ERROR: SOUND_ResizeSPUBuffer\n"));
 		MSERROR(("Stream (%d) is a MIB parent or child\n",i));
 		MSERROR(("Call this function before SOUND_SetStreamParent_Int\n"));
 		return;
  	}

	if ((StreamBuffer[i].Status==STREAM_STATUS_ON)&&(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM))
		SOUND_StopStream(i,0);	/* Stop stream if playing ADPCM data.. */

	if (size!=0)
		StreamBuffer[i].SPUBufferSize=size;
	else
		StreamBuffer[i].SPUBufferSize=StreamBuffer[i].AllocSize;
}

/**********************************************************************************
SOUND_ResizeStreamBuffer
	Allows you to change the size of a stream buffer. For example, if you are using
	multiple interleaved data files (MIB's), which have different interleaved sizes,
	you can allocated a buffer which is the size of the largest MIB interleaved size
	and then use this command to modify the size which is used in this buffer for
	the smaller MIB files.

	Requires:	i		Stream
				size	New size to use
						(pass 0 to use default size when calling SOUND_AllocateStreamBuffer)
***********************************************************************************/
void SOUND_ResizeStreamBuffer(int i,u_int size)
{
	if (StreamBuffer[i].MemoryAllocated!=0)	/* Is Stream buffer memory allocated? */
	{
		if (StreamBuffer[i].MaxSize<size)	/* Yes..Is size larger than maximum (originally allocated) size? */
		{
			MSERROR(("ERROR: Stream_ResizeStreamBuffer\n"));
			MSERROR(("ERROR: size 0x%x is larger than allocated buffer 0x%x size (0x%x)\n",
				size,i,StreamBuffer[i].MaxSize));
			return;
		}
		if (size!=StreamBuffer[i].AllocSize)	/* different size than current? */
		{
			SOUND_StopStream(i,0);
/* Passing 0 uses size allocated when SOUND_AllocateStreamBuffer was called */
			if (size==0)
				size=StreamBuffer[i].MaxSize;	/* Use default size */

			StreamBuffer[i].AllocSize=size;			/* Set new buffer size */
			StreamBuffer[i].AllocHalfSize=size/2;	/* Set new buffer half size */

			if (StreamBuffer[i].SPUBufferSize>size)
				StreamBuffer[i].SPUBufferSize=StreamBuffer[i].AllocSize;
		}
	}
	else
	{
		MSERROR(("ERROR: Stream_ResizeStreamBuffer\n"));
		MSERROR(("ERROR: Stream Buffer 0x%x not allocated. Can not resize\n",i));
	}
}

/**********************************************************************************
SOUND_InitSPU
	Init SPU2 and set Master Volume
***********************************************************************************/
void SOUND_InitSPU ()
{
    sceSdInit (0);		/* Init SPU */

/*	Disk media: DVD */
/*	Output format: PCM */
/*	Copy guard: normal (one generation recordable / default) */

    sceSdSetCoreAttr (SD_C_SPDIF_MODE,
    				 (SD_SPDIF_MEDIA_DVD | SD_SPDIF_OUT_PCM | SD_SPDIF_COPY_NORMAL));

	AdpcmSetMasterVolume(SD_CORE_0,0x3fff);		/* Set Master Volume Core 0 */
	AdpcmSetMasterVolume(SD_CORE_1,0x3fff);		/* Set Master Volume Core 1 */

	g_ReverbChans[0]=0;		/* Reverb channel bitmask on/off for core 0 */
	g_ReverbChans[1]=0;		/* Reverb channel bitmask on/off for core 1 */

	g_OutputChans[0]=0xffffff;  /* MIXV output for all SPU channels ON (core0) */
	g_OutputChans[1]=0xffffff;  /* MIXV output for all SPU channels ON (core1) */

#if (SCREAM_COMPATIBLE)
	
	{
	struct  SemaParam sp;

		//----- Take this one time oportunity to create SPURightsSema, -----
		//----- and tell SCREAM about it...                            -----
		if(g_SPURightsSema == -1)
		{
			sp.attr			= SA_THFIFO;
			sp.initCount	= 1;
			sp.maxCount		= 1;
			sp.option		= 0;
			g_SPURightsSema 	= CreateSema(&sp);
	
			if(g_SPURightsSema < 0)
			{
				printf("ERROR: sce_adpcm_loop\n");
				printf("Unable to create SPU rights semaphore.\n");
				while(1){}
			}
	
			// tell SCREAM about the SPU rights semaphore...
			snd_SetSPUSema(g_SPURightsSema);
		}

		//----- Take this one time oportunity to create g_SPUDMADoneSema, -----
		//----- and tell SCREAM about it...                               -----
		if(g_SPUDMADoneSema == -1)
		{
			g_SPUDMADoneSema  = CreateSema(&sp);
			if(g_SPUDMADoneSema < 0)
			{
				printf("ERROR: sce_adpcm_loop\n");
				printf("Unable to create SPU DMA done semaphore.\n");
				while(1){}
			}
			
			// tell SCREAM about the function to transfer data to SPU memory...
			snd_SetExternSPUMemTransFunc(SOUND_TransIOPToSPU, g_SPUDMADoneSema);
		}
	}

#endif // SCREAM_COMPATIBLE

    return;
}

/**********************************************************************************
SOUND_SetDTSMode
	Initialise DTS system.
	requires:	mode		SOUND_DTS_START (Initialises the DTS system)
							SOUND_DTS_INIT_THREAD (initialises the DTS thread, so its all running)
							SOUND_DTS_OFF	(Turn DTS off)
				flag		If SOUND_DTS_START
								SPU channel number 0-47
							if SOUND_DTS_OFF
								0 Stops callbacks and PCM block trans DMA
								1 As with (0), but closes DTS thread (full shut down)
								2 Only close DTS thread

	Note that for using SOUND_DTS_START, the system requires an SPU channel. This
	is used for sending white noise to the audio buffers, which is used for the DTS
	can keep everything in sync when initialising.
***********************************************************************************/
void SOUND_SetDTSMode(int mode,int flag)
{
#if (SOUND_DTS_ENABLE)

int sync;

	if (mode==SOUND_DTS_START)		/* Attempt to start DTS ? */
	{
		DTS_InitFlag=DTS_REQUEST_MULTISTREAM_SHUTDOWN;
		while(DTS_InitFlag==DTS_REQUEST_MULTISTREAM_SHUTDOWN)
		{
			DelayThread(TWO_T);	/* Wait two ticks.. */
			DelayThread(TWO_T);	/* Wait two ticks.. */
		};

		sync=JDTS(flag);		/* see DTS.C */
		if (sync!=0)
		{
		  	sceSdBlockTrans(0, SD_TRANS_MODE_STOP,0,0);	/* Sync failed */
			MSERROR(("SOUND_SetDTSMode: DTS_START TimeOut\n"));
			MSERROR(("Couldn't get a good sync\n"));
			MSERROR(("Try again!\n"));
			SOUND_Transfer.TranMode=SOUND_DTS_START_FAILED;
			DTS_InitFlag=DTS_ALLOW_MULTISTREAM_CONTINUE;
			WakeupThread(gThid);
			return;
		}
		DTS_InitFlag=DTS_ALLOW_MULTISTREAM_CONTINUE;
		WakeupThread(gThid);

		SOUND_Transfer.TranMode=SOUND_DTS_ON;	/* Sync ok. DTS Now ON */

		return;
	}
	else if (mode==SOUND_DTS_INIT_THREAD)	/* Start DTS thread? */
	{
		DTS_start();	/* see DTS.C */
	}
	else if (mode==SOUND_DTS_OFF)		/* Stop DTS? */
		SOUND_StopDTS(flag);			/* Request DTS to stop (doesn't happen straight away) */

#else
	(void)flag;
	(void)mode;
	printf("ERROR: SOUND_SetDTSMode\n");
	printf("DTS Not supported\n");
	printf("Set SOUND_DTS_ENABLE to 1\n");
	printf("Also requires DTS Middleware solution\n");
#endif
}

/**********************************************************************************
ProcessEECommand
	Using SIF DMA, the EE sends control commands to this routine.
***********************************************************************************/
void* ProcessEECommand (unsigned int command, void *data_)
{ 
#ifdef DEBUG_STATE
int          data  = *((         int *) data_);
#endif
    switch (command)
    {
		case SOUND_SETPARAMS:
			AdpcmSetParam ((short *)data_);		/* As noticed by Rob - this is the only function that can be called. Move it? */
			break;
	    default:
			MSERROR(("Driver error: unknown command %d \n", data));
		break;
    }
    return (void*)(&aret[0]);
}

/**********************************************************************************
sce_adpcm_loop
	Kicks off everything..
***********************************************************************************/
int sce_adpcm_loop (void)
{
sceSifQueueData qd;
sceSifServeData sd;

    sceSifInitRpc (0);
    sceSifSetRpcQueue (&qd, GetThreadId ());
    sceSifRegisterRpc (&sd, SOUND_DEV, (void*)ProcessEECommand, (void*)rpc_arg, NULL, NULL, &qd);
    sceSifRpcLoop (&qd);
    return 0;
}

/**********************************************************************************
SOUND_GetStatus
	Fills data buffer with relelvent info to be returned to the EE
	This routine is called when the EE SOUND_GetStatus call is called.
***********************************************************************************/
int SOUND_GetStatus(void)
{	
static int jop8=0;
int a;
int b;
int c;
int d;
int count;
u_int v;
#if (MS_DSP_ACTIVE==1)
SPUDSP_TRANS *ptr;
#endif

	count=0;
	for (a=0;a<UserMaxStreams;a++)
	{
		if (StreamBuffer[a].Status==STREAM_STATUS_ON)	/* Is stream ON? */
		{
			if ((StreamBuffer[a].ResidentFlag==STREAM_LOAD_EE)&&(StreamBuffer[a].SPUChannel==3))
				b=2<<10;
			else
				b=StreamBuffer[a].SPUChannel<<10;		/* SPU channel used for stream */
			b|=a<<4;								/* Stream number */
			c=StreamBuffer[a].PlayingHalf;

			if (c==STREAM_PLAYING_SHORT_LOOP)		/* playing looping data smaller than stream buffer? */
				c=0;								/* Yes..Set playing half to 0 */
			b|=c<<2;							/* Current SPU playing half */
			if (cd_stream==a) 
				b|=cd_stage;					/* Store CD status for stream if active */
			b|=StreamBuffer[a].KeyStat<<16;		/* Current envelope amplitude */
			aret[1+count]=b; 
			aret[1+count+1]=StreamBuffer[a].SPUCurrent;			/* Current playback position in SPU RAM */


//			aret[1+count+2]=StreamBuffer[a].FileID;	/* Stream File ID */
			if (StreamBuffer[a].iStatus==iSTATUS_INITIALISED)
				aret[1+count+2]=StreamBuffer[a].USR_StrID;
			else
				aret[1+count+2]=StreamBuffer[a].FileID; /* Stream File ID */

			aret[1+count+3]=StreamBuffer[a].ResidentFlag;	/* EE/IOP/SPU/ADPCM/PCM */

			if ((StreamBuffer[a].iStatus==iSTATUS_PLAYING)||
				(StreamBuffer[a].iStatus==iSTATUS_KEYED_ON))
			{
/* Top bit of ID tells EE that stream is REALLY PLAYING */
				aret[1+count+3]|=0x80000000;
			}

			else if(StreamBuffer[a].iStatus==iSTATUS_PLAY)
			{
				if ((StreamBuffer[a].ResidentFlag!=STREAM_AUDIO_ADPCM)&&
					(StreamBuffer[a].ResidentFlag!=STREAM_AUDIO_PCM))
				{
/* Top bit of ID tells EE that stream is REALLY PLAYING */
					aret[1+count+3]|=0x80000000;
				}
			}

			if (StreamBuffer[a].ResidentFlag!=STREAM_LOAD_IOP)
			{
/* Current write address to either SPU or EE RAM */
				aret[1+count+4]=(int)StreamBuffer[a].SPUAddress;
			}
			else
			{
	/* Current write address to IOP RAM */
				aret[1+count+4]=(int)StreamBuffer[a].IOPCurrentAddress;
			}

			v=StreamBuffer[a].FileSeek-StreamBuffer[a].FileStart;
			aret[1+count+5]=(int)v;
			aret[1+count+6]=(int)StreamBuffer[a].TotalTranSize;

/* If EE stream has completed, shut it down ONLY after sending its final info to the EE */
			if ((StreamBuffer[a].ResidentFlag==STREAM_LOAD_EE)&&(StreamBuffer[a].SPUChannel==2))
			{
				StreamBuffer[a].SPUChannel=3;
			}

			if ((StreamBuffer[a].iStatus==iSTATUS_PLAYING)||
				(StreamBuffer[a].iStatus==iSTATUS_KEYED_ON))
			{
				aret[1+count+7]=(int)SPU_PausePitch[StreamBuffer[a].SPUChannel];
				
			}
			else
				aret[1+count+7]=(int)StreamBuffer[a].Pitch;
			c=StreamBuffer[a].Priority;
			c<<=16;
				aret[1+count+7]|=c;

			aret[1+count+8]=(int)StreamBuffer[a].SamplesPlayed;
			count+=9;
		}
	}
	aret[0]=count/9;		/* Number of streams worth of data stored. */
	aret[0]|=jop8<<8;

	Cntr1++;
	aret[count+1]=Cntr1;


	if (StreamBusy==2)		/* Has EE just requested a stream to load ? */
	{
		aret[count+2]=-1;	/* Yes..Doesn't mean that IOP has allowed the load yet though.. */
	}
	else
	{
		if ((SOUND_INTERNAL_FastLoad==SOUND_FASTLOAD_ON)&&(EXT_LD_DEST!=STREAM_LOAD_MS))	/* (Can be read from the EE with SOUND_GetEXTERNALInfo) */
		{
			aret[count+2]=IOPRequestLoad;		/* 0-47 if stream is requiring data, 0xffff if not. */
		}
		else if (SOUND_INTERNAL_FastLoad==SOUND_FASTLOAD_OFF)
		{
			aret[count+2]=IOPRequestLoad;		/* 0-47 if stream is requiring data, 0xffff if not. */
		}
		else
		{
			aret[count+2]=-1;
		}
	}



	if (StreamBusy!=0)
		aret[count+3]=1;	/* Mark as CD busy */
	else
		aret[count+3]=0;	/* Mark as CD free */


	if (IOPRequestLoad!=-1)
	{
		if (StreamBuffer[IOPRequestLoad].NoMoreLoading==1)	// MS7.22 All of file in IOP RAM.. No need to load data
		{
			aret[count+3]=0;	/* Mark as CD free */
		}
	}

	aret[count+4]=(int)StreamMemoryEnd;	/* Current address where next resident sample will be transferred to. */
	aret[count+5]=StereoStarted;	/* 0=Stereo started, 1=Not started */

	for(a=0;a<48/16;a++)			/* Loop through each SPU channel */
	{
		c=0;
		for (b=0;b<16;b++)			/* Build 2bit per channel mask of key status. */
		{
			d=SPU_KeyStatus[(a*16)+b];
			if (d==SPU_KEY_STATUS_JUST_KEYED_ON)	/* Has it just been keyed on? */
			{
				d=SPU_KEY_STATUS_ON;	/* Set as keyed on in bitmask */
			}
			c<<=2;
			c|=d;
		}
		aret[count+6+a]=c;	/* 3 LONGS. Each one storing 8 SPU key status (2 bits each) */
	}

	aret[count+9]=(int)EXT_LD_IOP_ADDR;	/* EXTERNAL IOP Transfer address */
	aret[count+10]=(int)EXT_LD_SIZE;		/* EXTERNAL Size of data to load/transfer */
	aret[count+11]=(int)EXT_LD_CNTR;		/* EXTERNAL counter to check if loaded data is valid */

	b=count+12;
	for (a=0;a<9;a++)				/* Copy chain info to send to EE */
	{
		aret[b+a]=(int)EXT_LD_SEEK[a];		/* Start offset through file */
		aret[b+9+a]=(int)EXT_LD_ID[a];		/* File ID */
		aret[b+18+a]=(int)EXT_LD_PLAYSIZE[a];	/* Size of data to read from file */
		aret[b+27+a]=(int)EXT_LD_LSN[a];		/* LSN of file */
	}
	b+=36;

	aret[b]=(int)EXT_LD_CHAIN_COUNT;	/* Amount of streams in chain */
	aret[b+1]=(int)EXT_LD_CHAIN_POS;	/* Current position through chain list */

	aret[b+2]=(int)MaxIOPRAM;		/* Calculated from SOUND_GetMaxIOPRAM(); */
	aret[b+3]=(int)((SOUND_FILE_COUNT<<8)+SOUND_FILE_SOURCE);	/* Passed back from SOUND_GetFileInfo */
	aret[b+4]=(int)SOUND_FILE_SIZE;	/* Passed back from SOUND_GetFileInfo */
	aret[b+5]=(int)SOUND_FILE_LSN;	/* Passed back from SOUND_GetFileInfo */
	aret[b+6]=(int)EXT_LD_REMAIN;	/* EXTERNAL remaining data to be loaded */
	aret[b+7]=(int)EXT_LD_OFFSET;	/* Offset through current file to start loading */
	aret[b+8]=(int)StreamEEAddress;	/* Current EE transfer address (for STREAM_LOAD_EE) */
	aret[b+9]=(int)StreamIOPAddress;	/* Current IOP transfer address (for STREAM_LOAD_IOP) */
	aret[b+10]=E_CDErrorCheck;		/* 1=CD Error (Streams Paused), 0=No Error */
	aret[b+10]|=I_CDErrorCheck<<1;
	aret[b+10]|=CDCheck<<2;			/* 1 Error, 0 No Error */
	aret[b+11]=(int)EXT_LD_DEST;			/* Destination for loading data (MS=INTERNAL, IOP/EE=EXTERNAL) */

	aret[b+12]=SOUND_Transfer.TranMode;	/* Current DTS mode */
/* Last transfer of data to EE RAM */
/* (stream may have just finished, so info isn't available that way any more) */
	aret[b+13]=(int)EE_LastTranSize;
	aret[b+14]=(int)EE_TranCount;	/* Amount of IOP->EE transfers completed */

	aret[b+15]=(int)PCMBuffer;		/* Address of the PCM IOP buffer */
	aret[b+16]=(int)PCMBufferSize;	/* Size of the PCM Buffer */
	aret[b+17]=0;
	if (PCMBuffer==0)			/* PCM Buffer allocated? */
		aret[b+18]=SOUND_PCM_BUFFER_NOT_ALLOCATED;	/* No.. */
	else
		aret[b+18]=PCMPlaying;	/* Set PCM playback type (playing/stopped/paused..) */
	aret[b+17]=(int)sceSdBlockTransStatus(MS_PCM_CORE,0)&0x00ffffff;	/* Yes..Get current address */

	aret[b+19]=UserMaxStreams;		/* Maximum amount of streams allocated */
	for (a=0;a<UserMaxStreams;a++)
	{
		aret[b+20+a]=(int)StreamBuffer[a].IOPAddress;	/* IOP address of each Stream Buffer */
	}
	b+=a;
	aret[b+21]=SOUND_UserTransfer.Status;		/* User Transfer status */

	for (a=0;a<48;a++)
	{
		aret[b+22+a]=(int)SPU_NAX[a];
	}
	b+=48;

/* (next is b+23) */

#if (MS_SCANLINE_DEBUG==1)
	aret[b+23]=SOUND_SCANLINE1;
	aret[b+24]=SOUND_SCANLINE2;
#else
	aret[b+23]=jop8;
	aret[b+24]=0;
#endif
	jop8++;
	jop8&=255;


//---------
#if (MS_DSP_ACTIVE==1)
	for (a=0;a<8;a++)
	{

		aret[b+25+a]=SOUND_DSP_Result[a];
	}
	aret[b+33]=SOUND_DSP_PCMBuffer;

	while(SOUND_DSP_PROCESSING!=0){
	DelayThread(ONE_T);};

	SOUND_TransferDSPEE();

	ptr=(SPUDSP_TRANS *)SOUND_DSP_FinalAddressList;
	c=0;
	b+=34;
	for (a=0;a<SOUND_DSP_EEPacketSize;a++)
	{
		if (ptr->Counter>=0x8000)
		{
			aret[b+1+c]=ptr->Counter&0x7fff;
			aret[b+2+c]=ptr->IOPAddress;
			aret[b+3+c]=ptr->EEAddress;
			c+=3;
		}
		ptr++;
	}
	aret[b]=c/3;
	b+=1+c;
#else
	aret[b+34]=0;
	b+=35;
#endif
//---------

// reference aret[b] for next entry..
#if (MS_DSP_ACTIVE==1)
	aret[b]=SOUND_DSP_ENGINE;
#else
	aret[b]=0;
#endif
	aret[b+1]=(int)SOUND_CD_ERRORCODE;
	aret[b+2]=SOUND_FILE_AT_OPEN;		// ATWinMon file open flag (1=open, 0=closed)
	aret[b+3]=SOUND_FILE_AT_ID;			// ATWinMon file handle (or error code if file failed to open)
	return (aret[0]);
}

/**********************************************************************************
GetSFXSlot
	Returns SPU Address of spot FX sample.

	Requires:	ID			ID of sound required (from FileNames.H)
	Returns:	SFX			pointer to correct ADPCM_SPOTINFO structure
							0 = ID not found
***********************************************************************************/
ADPCM_SPOTINFO* GetSFXSlot(int ID)
{
int i;
ADPCM_SPOTINFO *SFX;

	SFX=JSPOTFX;
	for (i=0;i<MAX_SPOT_FX;i++)
	{
		if (SFX->ID==ID)	/* Find SPU ID.. */
		{
			return(SFX);
		}
		SFX++;
	}
	MSERROR((">>>>>ERROR: GetSFXSlot\n"));
	MSERROR(("SFX ID (0x%x) not found\n",ID));
	return(0);
}

/**********************************************************************************
SOUND_ModifyEETransferAddress

	Sets the current streams EE transfer address.
	This function can only be set after the stream is already active. (After
	SOUND_PlayStream has been called).
	You can then use this for double buffering data on the EE side, by just calling
	this function before allowing the load for this stream channel via SOUND_GetStatus.

	Requires:	i		Stream channel number
				addr	EE address for streaming data to

	Returns:	Nothing
***********************************************************************************/
void SOUND_ModifyEETransferAddress(int i,u_int addr)
{
int check;

	check=SOUND_CheckStream(i);
	if (check!=0)
	{
		MSERROR((">>>>>ERROR: SOUND_ModifyEETransferAddress\n"));
		MSERROR(("Stream (%d) out of range\n",i));
		return;
	}

	if (StreamBuffer[i].Status!=STREAM_STATUS_ON)	/* Is stream turned on? */
	{
		MSERROR((">>>>>ERROR: SOUND_ModifyEETransferAddress\n"));
		MSERROR(("Stream not playing. Can not modify address\n"));
		return;
	}

	if (StreamBuffer[i].ResidentFlag!=STREAM_LOAD_EE)
	{
		MSERROR((">>>>>ERROR: SOUND_ModifyEETransferAddress\n"));
		MSERROR(("Stream channel %x is not definied as STREAM_LOAD_EE\n",i));
		return;
	}
	StreamBuffer[i].SPUAddress=addr;	/* Set new EE transfer address */
}

/**********************************************************************************
SOUND_PlaySFX
	Plays a sound

	Requires:	ID			ID from FileNames.H (ie. SFX_01)
				Channel		SPU Channel (0-47)
				VolL		Left volume
				VolR		Right volume
				Pitch		SPU Pitch
				AR			Attack/Release rate
				Loop		Loop counter (0 = infinite, 1 = once, 0xffff = max)

	Returns:	Nothing
***********************************************************************************/
void SOUND_PlaySFX(int ID,int Channel,int VolL,int VolR,int Pitch,int AR,int Loop)
{
int SPUAddress;
int LoopAddress;
int ch,core;
ADPCM_SPOTINFO *SFX;

	ch=0;
	core=0;

	SFX=GetSFXSlot(ID);	/* Get SPU address for corresponding ID */
	if (SFX==0)	/* ID found? */
	{
		MSERROR((">>>>> ERROR: SOUND_PlaySFX\n"));	/* No. */
		MSERROR(("SFX ID 0x%x not found\n",ID));
		return;
	}
	SPUAddress=(int)SFX->SPUAddress;
	SOUND_StopStream(SPU_CH+Channel,0);	/* Stop any stream which is using SFX SPU channel */

/* Set flag to stop hardware reading status too quickly */
	SPU_KeyStatus[Channel]=SPU_KEY_STATUS_JUST_KEYED_ON;
	SPU_LoopCounter[Channel]=Loop;		/* Set loop counter */


/* Only key on channel if global switch is clear */
	ch=SOUND_GetSPUChannel(Channel);	// Moved!! Thanks to Jonathan Lanier @ Midway!
	core=SOUND_GetCore(Channel);

	if (SOUND_SFXAllowKeyOn==0)
	{
		GET_SPU_RIGHTS();
		sceSdSetParam ((unsigned short)(core | (ch<<1) | SD_VP_ADSR2), 0x1fc0);
		sceSdSetSwitch ((u_short)(core | SD_S_KOFF), (u_int)(1<<ch));
		SOUND_DelayKeyOnOff();
		SetVoiceData(Channel,Pitch,VolL,VolR,(int)(SPUAddress+SFX->PlayOffset),AR,0);	/* Set channel info */
		sceSdSetSwitch ((unsigned short)(core | SD_S_KON), (unsigned int)(1<<ch));
		RELEASE_SPU_RIGHTS();
	}
	else
	{
		SetVoiceData(Channel,Pitch,VolL,VolR,(int)(SPUAddress+SFX->PlayOffset),AR,0);	/* Set channel info */
	}

	if (SFX->PlayOffset!=0)
	{
		SOUND_DelayKeyOnOff();	/* Give SPU enough time (4Ts) before setting LSAX */
		if (SFX->LoopFlag==SOUND_LOOP_TO_START)
			LoopAddress=SPUAddress;
		else
			LoopAddress=(int)(SPUAddress+SFX->PlayOffset);
		sceSdSetAddr ((unsigned short)(core | ch<<1 | SD_VA_LSAX), (u_int)LoopAddress);
	}
	else if (Loop==1)	// Only doing one single loop playback
	{
		SOUND_DelayKeyOnOff();	/* Give SPU enough time (4Ts) before setting LSAX */
		sceSdSetAddr ((unsigned short)(core | ch<<1 | SD_VA_LSAX), 0x5000);
		SPU_LoopCounter[Channel]=-1;		/* Set loop counter */
	}
}

/**********************************************************************************
SOUND_KeyOnChannel
	Allows keying on of multiple SPU channels
	Call SOUND_SFXDisableKeyOn before this function, the call SOUND_PlaySFX as normal.
	The SFX will be setup as normal, but no key on will take place until this function
	is called. Then, a single KeyOn function will be set for all requested SPU channels
	at the same time, as this function just builds a bit pattern of SPU channels which
	is then set to KeyOn after all functions are completed (ie. when all info is
	decoded from a FlushIOPCommand.)
	Basically just a nice way to get multiple SPU channels to key on at exactly the
	same time (good for surround sound processing)

	Requires: 	Channel	0-47	SPU channel to key on 
	Returns:	Nothing
	
***********************************************************************************/
void SOUND_KeyOnChannel(int Channel)
{
int chan,core;

	chan=SOUND_GetSPUChannel(Channel);
	core=SOUND_GetCore(Channel);
	SOUND_KeyOnBitmask[core]|=1<<chan;
}

/**********************************************************************************
SOUND_SetChannelVolume
	Sets volume of specified channel
	Requires:	Channel		SPU Channel (0-47)
				VolL		Left volume
				VolR		Right volume
	Returns:	Nothing
***********************************************************************************/
void SOUND_SetChannelVolume(int Channel,int VolL,int VolR,int flag)
{
int ch,core;
int i;

	ch=SOUND_GetSPUChannel(Channel);
	core=SOUND_GetCore(Channel);
	ch<<=1;

	SPU_VolL[Channel]=(short)VolL;
	SPU_VolR[Channel]=(short)VolR;

	SOUND_SmoothFadeOff(Channel);	/* Turn off any automatic volume ramping */

	sceSdSetParam ((unsigned short)(core | ch | SD_VP_VOLL), (unsigned short)VolL);
	sceSdSetParam ((unsigned short)(core | ch | SD_VP_VOLR), (unsigned short)VolR);

	if (flag==0)
	{
		for (i=0;i<UserMaxStreams;i++)
		{
			if ((StreamBuffer[i].Status==STREAM_STATUS_ON)&&(Channel==StreamBuffer[i].SPUChannel)
				&&(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM))
			{
				StreamBuffer[i].VolL=(short)VolL;
				StreamBuffer[i].VolR=(short)VolR;
			}
		}
	}
}

/**********************************************************************************
SOUND_SetChannelPitch
	Sets pitch of specified channel
	Note: If the user has called SOUND_Pause, this function could start a stream/sfx
		playing again. It still needs to set the pitch though, as normally a game goes
		into pause mode when SELECT is pressed - displaying an options menu. SFX playback
		are therefore still required during this period.

	Requires:	Channel		SPU Channel (0-23)
				Pitch		(SPU Format - NOT Khz)
	Returns:	Nothing
***********************************************************************************/
void SOUND_SetChannelPitch(int Channel,int Pitch)
{
int ch,core;

	ch=SOUND_GetSPUChannel(Channel);
	core=SOUND_GetCore(Channel);
	ch<<=1;

	sceSdSetParam ((unsigned short)(core | ch | SD_VP_PITCH), (unsigned short)Pitch);
	SPU_PausePitch[Channel]=(unsigned short)Pitch;	/* Incase any SFX are called after Pause & before Resume.. */
}

/**********************************************************************************
SOUND_Pause
	Pauses all SPU channels (ie. sets their pitch to 0) and PCM channel
	Also handles a case where, if a CD error as occured (which pauses all streams)
	the SPU channel for this stream will not be paused again.
	Also then clears a flag that will not let the CD error resume code restart the
	streams (as you are now in pause mode, due to this function being called)
***********************************************************************************/
void SOUND_Pause(void)
{
int i,j;
u_int chan,core;
int oldstat;
int c;

	SOUND_InitSmoothFade();	/* Reset all channels so they don't ramp volumes.. */

    CpuSuspendIntr (&oldstat);

	for (i=0;i<UserMaxStreams;i++)
	{
		if (StreamBuffer[i].Status==STREAM_STATUS_ON)	/* Is stream active? */
		{
			if ((StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)&&(PCMPause==0))
			{
				SOUND_PausePCM();
			}
			
			if(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)
			{
				c=SOUND_GetStreamSPUChan(i);

				if (I_CDErrorCheck==0)	/* Is there a CD error (where all streams would automatically have been paused)? */
				{
/* Make a copy of the streams pitch value. This is required if you pause a stream when it is preloading data... */
/* ..This is because you need to key on the stream with a pitch of 0 so the stream doesn't start playing once preloading */
/* is completed. */
					if (StreamBuffer[i].Ch2Flag!=0)
					{
						for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
						{
							SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel2[j]]=1;	/* Mark SPU channel as used by preloaded stream */
						}
					}

					StreamBuffer[i].Pitch=0;
					SOUND_PauseStreamCheck[c]=1;	/* Mark SPU channel as used by preloaded stream */
				}
				else if (SOUND_PauseStreamCheck[c]!=2)
				{
/* The streams are already paused due to a CD error. AND this is the first time that SOUND_Pause has been called */
/* after the CD error occured. */
/* First, copy the stream pitch as before. Then set the flag to say that this routine has already been called */
/* Then. Once CD playing has resumed, the streams will stay in pause mode. */
/*					SPU_PausePitch[StreamBuffer[i].SPUChannel]=(unsigned short)StreamBuffer[i].Pitch; */
					if (StreamBuffer[i].Ch2Flag!=0)
					{
						for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
						{
							SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel2[j]]=2;
						}
					}
					StreamBuffer[i].Pitch=0;
					SOUND_PauseStreamCheck[c]=2;
				}
			}
		}
	}

	if ((PCMPause==0)&&(PCMPlaying==1))
		SOUND_PausePCM();

/* Now go through all 48 SPU channels. Only make a copy of the current SPU pitches for SPU channels which are not */
/* being used for playing streams. */
/* ALSO, clear the SPU_CDError flags. This stops the streams resuming once a CD error has been fixed. */

	for (i=0;i<48;i++)
	{
#if (SCREAM_COMPATIBLE)
		if(CHANNEL_FLAG_SET(i,g_MSChannelUseMask))
		{
#endif
			chan=(unsigned int)SOUND_GetSPUChannel(i);
			core=(unsigned int)SOUND_GetCore(i);
			if (SOUND_PauseStreamCheck[i]==0)
			{
				SPU_PausePitch[i]=sceSdGetParam ((unsigned short)(core | chan<<1 | SD_VP_PITCH));
			}
			SPU_CDError[i]=0;		/* Clear these, so CD error resuming won't reset the pitch of a stream */
#if (SCREAM_COMPATIBLE)
		}
#endif
	}

	SOUND_FastPitch0();		/* Set ALL SPU channel pitches to 0 */
    CpuResumeIntr (oldstat);
}

/**********************************************************************************
SOUND_FastPitch0

Set all pitches to 0 (pause ADPCM) as fast as possible
Then set the volumes to 0 also, so we don't get any nasty DC offsets..
***********************************************************************************/
void SOUND_FastPitch0(void)
{
int i;

#if (SCREAM_COMPATIBLE)
	for(i = 0; i < 24; i++)
	{
		if( (1L<< i) & g_MSChannelUseMask[0] )
		{
			sceSdSetParam (0 | i<<1 | SD_VP_PITCH, 0);
			sceSdSetParam ((unsigned short)(0 | i<<1 | SD_VP_VOLL), 0);
			sceSdSetParam ((unsigned short)(0 | i<<1 | SD_VP_VOLR), 0);
		}
		if( (1L<< i) & g_MSChannelUseMask[1] )
		{
			sceSdSetParam (1 | i<<1 | SD_VP_PITCH, 0);
			sceSdSetParam ((unsigned short)(1 | i<<1 | SD_VP_VOLL), 0);
			sceSdSetParam ((unsigned short)(1 | i<<1 | SD_VP_VOLR), 0);
		}
	}
#else
	//this will only work if MS has all the voices!...
	sceSdSetParam (0 | 0<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 1<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 2<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 3<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 4<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 5<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 6<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 7<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 8<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 9<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 10<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 11<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 12<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 13<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 14<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 15<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 16<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 17<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 18<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 19<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 20<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 21<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 22<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (0 | 23<<1 | SD_VP_PITCH, 0);

	sceSdSetParam (1 | 0<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 1<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 2<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 3<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 4<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 5<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 6<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 7<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 8<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 9<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 10<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 11<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 12<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 13<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 14<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 15<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 16<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 17<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 18<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 19<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 20<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 21<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 22<<1 | SD_VP_PITCH, 0);
	sceSdSetParam (1 | 23<<1 | SD_VP_PITCH, 0);

	for (i=0;i<24;i++)
	{
		sceSdSetParam ((unsigned short)(0 | i<<1 | SD_VP_VOLL), 0);
		sceSdSetParam ((unsigned short)(0 | i<<1 | SD_VP_VOLR), 0);
		sceSdSetParam ((unsigned short)(1 | i<<1 | SD_VP_VOLL), 0);
		sceSdSetParam ((unsigned short)(1 | i<<1 | SD_VP_VOLR), 0);
	}
#endif
}

/**********************************************************************************
SOUND_PauseSPUChannels
	Pauses required SPU channels (ie. sets their pitch to 0)
	Also handles a case where, if a CD error as occured (which pauses all streams)
	the SPU channel for this stream will not be paused again.
	Also then clears a flag that will not let the CD error resume code restart the
	streams (as you are now in pause mode, due to this function being called)

	Requires:	First		First SPU channel (0-47)
				Last		Last SPU channel (0-47). Must be larger or equal to First

	Returns:	Nothing

***********************************************************************************/
void SOUND_PauseSPUChannels(u_int First, u_int Last)
{
int i,j;
u_int chan,core;
int oldstat;
int c;

	for (i=(int)First;i<=(int)Last;i++)
	{
		SOUND_SmoothFadeOff(i);	/* Reset required SPU channels, so they don't ramp volumes */
	}

    CpuSuspendIntr (&oldstat);

	for (i=0;i<UserMaxStreams;i++)
	{
		if (StreamBuffer[i].Status==STREAM_STATUS_ON)	/* Is stream active? */
		{
			if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)
			{
				if ((StreamBuffer[i].SPUChannel>=First)&&(StreamBuffer[i].SPUChannel<=Last))
				{
					c=SOUND_GetStreamSPUChan(i);

					if (I_CDErrorCheck==0)	/* Is there a CD error (where all streams would automatically have been paused)? */
					{
/* Make a copy of the streams pitch value. This is required if you pause a stream when it is preloading data... */
/* ..This is because you need to key on the stream with a pitch of 0 so the stream doesn't start playing once preloading */
/* is completed. */
						if (StreamBuffer[i].Ch2Flag!=0)
						{
							for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
							{
								SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel2[j]]=1;	/* Mark SPU channel as used by preloaded stream */
							}
						}
						StreamBuffer[i].Pitch=0;
						SOUND_PauseStreamCheck[c]=1;	/* Mark SPU channel as used by preloaded stream */
					}
					else if (SOUND_PauseStreamCheck[c]!=2)
					{
/* The streams are already paused due to a CD error. AND this is the first time that SOUND_Pause has been called */
/* after the CD error occured. */
/* First, copy the stream pitch as before. Then set the flag to say that this routine has already been called */
/* Then. Once CD playing has resumed, the streams will stay in pause mode. */
/*					SPU_PausePitch[StreamBuffer[i].SPUChannel]=(unsigned short)StreamBuffer[i].Pitch; */
						if (StreamBuffer[i].Ch2Flag!=0)
						{
							for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
							{
								SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel2[j]]=2;
							}
						}
						StreamBuffer[i].Pitch=0;
						SOUND_PauseStreamCheck[c]=2;
					}
				}
			}
		}
	}

/* Now go through all 48 SPU channels. Only make a copy of the current SPU pitches for SPU channels which are not */
/* being used for playing streams. */
/* ALSO, clear the SPU_CDError flags. This stops the streams resuming once a CD error has been fixed. */

	for (i=(int)First;i<=(int)Last;i++)
	{
		chan=(unsigned int)SOUND_GetSPUChannel(i);
		core=(unsigned int)SOUND_GetCore(i);
		if (SOUND_PauseStreamCheck[i]==0)
		{
			SPU_PausePitch[i]=sceSdGetParam ((unsigned short)(core | chan<<1 | SD_VP_PITCH));
		}
		SPU_CDError[i]=0;		/* Clear these, so CD error resuming won't reset the pitch of a stream */
	}

	for (i=(int)First;i<=(int)Last;i++)
	{
		chan=(u_int)i%24;
		core=(u_int)i/24;
		sceSdSetParam ((u_short)(core | chan<<1 | SD_VP_PITCH), 0);
	}
	for (i=(int)First;i<=(int)Last;i++)
	{
		chan=(u_int)i%24;
		core=(u_int)i/24;
		sceSdSetParam ((unsigned short)(core | chan<<1 | SD_VP_VOLL), 0);
		sceSdSetParam ((unsigned short)(core | chan<<1 | SD_VP_VOLR), 0);
	}
    CpuResumeIntr (oldstat);
}


int SOUND_GetStreamSPUChan(int i)
{
	if (StreamBuffer[i].iStatus==iSTATUS_INITIALISED)
		return(StreamBuffer[i].USR_SPUChannel);
	else
		return(StreamBuffer[i].SPUChannel);
}

sceSdBatch JPB[48*4];
u_int S_Cores;
u_int S_Chans;
u_short S_Pitch;
u_int S_Data;
/**********************************************************************************
SOUND_Resume
	Resumes all SPU channels and PCM channel
	Also has a good attempt at keeping all Parent/Child or 2nd SPU channel
	streams in sync..
***********************************************************************************/
void SOUND_Resume(void)
{
int i,j;
u_int chan,core;
u_int SPUChan,SPUChan2;
int S_Counter=0;
int a;
u_int SPUOffset,SPO2;
int c;
u_int StreamAddress=0;

	for (i=0;i<48;i++)
	{
/* Firstly, resume any non stream SPU channels. */
#if (SCREAM_COMPATIBLE)
		if (CHANNEL_FLAG_SET(i,g_MSChannelUseMask) && SOUND_PauseStreamCheck[i]==0)
#else
		if (SOUND_PauseStreamCheck[i]==0)
#endif
		{
			if ((StreamBuffer[i].Status==STREAM_STATUS_ON)&&(i<UserMaxStreams))
			{
				c=SOUND_GetStreamSPUChan(i);
				if (SPU_CDError[c]!=0)	/* If stream already paused by a CD error, don't pause it again! */
					continue;
			}
			chan=(unsigned int)SOUND_GetSPUChannel(i);
			core=(unsigned int)SOUND_GetCore(i);
			sceSdSetParam ((unsigned short)(core | chan<<1 | SD_VP_PITCH), SPU_PausePitch[i]);
		}
	}

/* For Interleaved streams, check the current offset for the Parent and set the NAX */
/* for all Child streams to the same offset. Should handle possible sync problems. */


	for (i=0;i<UserMaxStreams;i++)
	{
		if ((StreamBuffer[i].ParentChildFlag==SOUND_PARENT)&&(StreamBuffer[i].Status==STREAM_STATUS_ON))
		{
			StreamAddress=SOUND_SPUGetStreamAddress(i);	/* Current SPU RAM playback address */
			if ((StreamAddress>=StreamBuffer[i].SPUAddress)&&(StreamAddress<StreamBuffer[i].SPUAddress+StreamBuffer[i].SPUBufferSize))
			{
				c=SOUND_GetStreamSPUChan(i);
	 			chan=(unsigned int)SOUND_GetSPUChannel(c);
	 			core=(unsigned int)SOUND_GetCore(c);
	 			chan<<=1;
	 			SPUOffset=sceSdGetAddr((unsigned short)(core|chan|SD_VA_NAX));
				SPUOffset&=(u_int)(~15);	// MS7.2
				SPO2=SPUOffset;
/*Now Make SPUAddr = the OFFSET through the current SPU buffer */
				SPUOffset-=StreamBuffer[i].SPUAddress;
				if (StreamBuffer[i].Ch2Flag!=0)
				{
					for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
					{
			 			chan=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[j]);
			 			core=(unsigned int)SOUND_GetCore(StreamBuffer[i].SPUChannel2[j]);
			 			chan<<=1;
						sceSdSetAddr((unsigned short)(core|chan|SD_VA_NAX),SPO2);
					}
				}

				for (a=0;a<UserMaxStreams;a++)
				{
					if ((StreamBuffer[a].ParentStream==StreamBuffer[i].ParentStream)&&
						(StreamBuffer[a].Status==STREAM_STATUS_ON)&&(a!=i))
					{
						c=SOUND_GetStreamSPUChan(a);
	   					chan=(unsigned int)SOUND_GetSPUChannel(c);
	 					core=(unsigned int)SOUND_GetCore(c);
	 					chan<<=1;
	 					sceSdSetAddr((unsigned short)(core|chan|SD_VA_NAX),StreamBuffer[a].SPUAddress+SPUOffset);
						if (StreamBuffer[i].Ch2Flag!=0)
						{
							for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
							{
					 			chan=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[j]);
					 			core=(unsigned int)SOUND_GetCore(StreamBuffer[i].SPUChannel2[j]);
					 			chan<<=1;
		 						sceSdSetAddr((unsigned short)(core|chan|SD_VA_NAX),StreamBuffer[a].SPUAddress+SPUOffset);
							}
						}
					}
				}
			}
			else
			{
//				printf("Playback out of range..Must be SOUND_PlayStreamFromSFX..%x\n",i);
//				printf("%x, %x, %x\n",StreamBuffer[i].SPUAddress,StreamBuffer[i].SPUBufferSize,StreamBuffer[i].SPUAddress+StreamBuffer[i].SPUBufferSize);
			}
		}
	}

	for (i=0;i<UserMaxStreams;i++)
	{
		if (StreamBuffer[i].Status==STREAM_STATUS_ON)	/* Is stream active? */
		{
			if(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)	/* Playing PCM? */
			{
				if ((PCMCDPause==0)&&(PCMPlaying!=0))
				{
					SOUND_ResumePCM();	/* Continue PCM playback */
				}
				else if ((PCMCDPause==0)&&(PCMPause!=0))
				{
/* PCM was paused in preload mode (before actually playing) */
					SOUND_StartPCM();
				}
				else
					PCMPause=0;		/* Currently paused due to CD error */
			}

			else if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)
			{
				SPUChan=(u_int)SOUND_GetStreamSPUChan(i);	//StreamBuffer[i].SPUChannel;
				if (I_CDErrorCheck==1)	/* Currently a CD error? */
				{
/* If there is currently a CD error, you can not resume playing streams. */
/* Instead, just set the SPU_CDError flag (which is cleared when calling SOUND_Pause) */
/* This will make the stream resume playing once the CD error is fixed. */

					SPU_CDError[SPUChan]=1;
				}
				else if (SOUND_PauseStreamCheck[SPUChan]!=0)		/* Still preloading from when it was paused. */
				{
/* Otherwise, resume playback of any SPU channel used by a stream */
/* Also, clear the PauseStreamCheck flag. */
/* To do this, make a copy of Any SPU channels that need to be restarted */

					S_Cores=(unsigned int)SOUND_GetCore((int)SPUChan);
					S_Chans=(unsigned int)(SOUND_GetSPUChannel((int)SPUChan))<<1;
					S_Pitch=SPU_PausePitch[SPUChan];
					S_Data=S_Cores|S_Chans|SD_VP_PITCH;

/* Build a list of SPU channels and Pitches to set. Used for the sceSdProcBatch command */
					JPB[S_Counter].func=SD_BSET_PARAM;
					JPB[S_Counter].entry=(unsigned short)S_Data;
					JPB[S_Counter].value=S_Pitch;

					S_Counter++;

					if (StreamBuffer[i].Ch2Flag!=0)	/* Using a more than one SPU channel? */
					{
						for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
						{
							SPUChan2=StreamBuffer[i].SPUChannel2[j];
							S_Cores=(unsigned int)SOUND_GetCore((int)SPUChan2);
							S_Chans=(unsigned int)(SOUND_GetSPUChannel((int)SPUChan2))<<1;
							S_Pitch=SPU_PausePitch[SPUChan];
							S_Data=S_Cores|S_Chans|SD_VP_PITCH;
/* Add 2nd SPU channel data to list also.. */
							JPB[S_Counter].func=SD_BSET_PARAM;
							JPB[S_Counter].entry=(unsigned short)S_Data;
							JPB[S_Counter].value=S_Pitch;

							S_Counter++;
							SOUND_PauseStreamCheck[SPUChan2]=0;
						}
					}
					StreamBuffer[i].Pitch=(short)SPU_PausePitch[SPUChan];	/* Copy pitch back to stream */
					SOUND_PauseStreamCheck[SPUChan]=0;	/* Clear pause flag for SPU channel */
				}
			}
		}
	}

/* Set pitches of all streams */
	if (S_Counter!=0)
		sceSdProcBatch((sceSdBatch *)JPB,0,(unsigned int)S_Counter);

#if (SCREAM_COMPATIBLE)
	for (i=0;i<24;i++)	/* Reset volumes for all SPU channels. */
	{
		if(g_MSChannelUseMask[0] & (1L<<i))
		{
			sceSdSetParam ((unsigned short)(0 | i<<1 | SD_VP_VOLL), (unsigned short)SPU_VolL[i]);
			sceSdSetParam ((unsigned short)(0 | i<<1 | SD_VP_VOLR), (unsigned short)SPU_VolR[i]);
		}
		if(g_MSChannelUseMask[1] & (1L<<i))
		{
			sceSdSetParam ((unsigned short)(1 | i<<1 | SD_VP_VOLL), (unsigned short)SPU_VolL[i+24]);
			sceSdSetParam ((unsigned short)(1 | i<<1 | SD_VP_VOLR), (unsigned short)SPU_VolR[i+24]);
		}
	}
#else
	for (i=0;i<24;i++)	/* Reset volumes for all SPU channels. */
	{
		sceSdSetParam ((unsigned short)(0 | i<<1 | SD_VP_VOLL), (unsigned short)SPU_VolL[i]);
		sceSdSetParam ((unsigned short)(0 | i<<1 | SD_VP_VOLR), (unsigned short)SPU_VolR[i]);
		sceSdSetParam ((unsigned short)(1 | i<<1 | SD_VP_VOLL), (unsigned short)SPU_VolL[i+24]);
		sceSdSetParam ((unsigned short)(1 | i<<1 | SD_VP_VOLR), (unsigned short)SPU_VolR[i+24]);
	}
#endif

	if ((PCMPause==1)&&(PCMPlaying==1))
		SOUND_ResumePCM();
}


/**********************************************************************************
SOUND_ResumeSPUChanels
	Resumes required SPU channels
	Also has a good attempt at keeping all Parent/Child or 2nd SPU channel
	streams in sync..

	Requires:		First		First SPU channel to resume
					Last		Last SPU channel to resume (must be >=First)

	Returns:		Nothing

***********************************************************************************/
void SOUND_ResumeSPUChannels(u_int First,u_int Last)
{
int i,j;
u_int chan,core;
u_int SPUChan,SPUChan2;
int S_Counter=0;
int a;
u_int SPUOffset,SPO2;
int c;
u_int StreamAddress=0;

	for (i=(int)First;i<=(int)Last;i++)
	{
/* Firstly, resume any non stream SPU channels. */
		if (SOUND_PauseStreamCheck[i]==0)
		{
			if ((StreamBuffer[i].Status==STREAM_STATUS_ON)&&(i<UserMaxStreams))
			{
				c=SOUND_GetStreamSPUChan(i);
				if (SPU_CDError[c]!=0)	/* If stream already paused by a CD error, don't pause it again! */
					continue;
			}
			chan=(unsigned int)SOUND_GetSPUChannel(i);
			core=(unsigned int)SOUND_GetCore(i);
			sceSdSetParam ((unsigned short)(core | chan<<1 | SD_VP_PITCH), SPU_PausePitch[i]);
		}
	}

/* For Interleaved streams, check the current offset for the Parent and set the NAX */
/* for all Child streams to the same offset. Should handle possible sync problems. */

	for (i=0;i<UserMaxStreams;i++)
	{
		if ((StreamBuffer[i].ParentChildFlag==SOUND_PARENT)&&(StreamBuffer[i].Status==STREAM_STATUS_ON))
		{
			StreamAddress=SOUND_SPUGetStreamAddress(i);	/* Current SPU RAM playback address */
			if ((StreamAddress>=StreamBuffer[i].SPUAddress)&&(StreamAddress<StreamBuffer[i].SPUAddress+StreamBuffer[i].SPUBufferSize))
			{
				c=SOUND_GetStreamSPUChan(i);
				if ((c>=First)&&(c<=Last))
				{
		 			chan=(unsigned int)SOUND_GetSPUChannel(c);
		 			core=(unsigned int)SOUND_GetCore(c);
		 			chan<<=1;
		 			SPUOffset=sceSdGetAddr((unsigned short)(core|chan|SD_VA_NAX));
					SPO2=SPUOffset;
/*Now Make SPUAddr = the OFFSET through the current SPU buffer */
					SPUOffset-=StreamBuffer[i].SPUAddress;
					if (StreamBuffer[i].Ch2Flag!=0)
					{
						for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
						{
				 			chan=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[j]);
				 			core=(unsigned int)SOUND_GetCore(StreamBuffer[i].SPUChannel2[j]);
				 			chan<<=1;
							sceSdSetAddr((unsigned short)(core|chan|SD_VA_NAX),SPO2);
						}
					}
					for (a=0;a<UserMaxStreams;a++)
					{
						c=SOUND_GetStreamSPUChan(a);
						if ((StreamBuffer[a].ParentStream==StreamBuffer[i].ParentStream)&&
							(StreamBuffer[a].Status==STREAM_STATUS_ON)&&(a!=i))
						{
		   					chan=(unsigned int)SOUND_GetSPUChannel(c);
		 					core=(unsigned int)SOUND_GetCore(c);
		 					chan<<=1;
		 					sceSdSetAddr((unsigned short)(core|chan|SD_VA_NAX),StreamBuffer[a].SPUAddress+SPUOffset);
							if (StreamBuffer[i].Ch2Flag!=0)
							{
								for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
								{
						 			chan=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[j]);
						 			core=(unsigned int)SOUND_GetCore(StreamBuffer[i].SPUChannel2[j]);
						 			chan<<=1;
			 						sceSdSetAddr((unsigned short)(core|chan|SD_VA_NAX),StreamBuffer[a].SPUAddress+SPUOffset);
								}
							}
						}
					}
				}
			}
			else
			{
//				printf("Playback out of range..Must be SOUND_PlayStreamFromSFX..%x\n",i);
//				printf("%x, %x, %x\n",StreamBuffer[i].SPUAddress,StreamBuffer[i].SPUBufferSize,StreamBuffer[i].SPUAddress+StreamBuffer[i].SPUBufferSize);
			}
		}
	}


	for (i=0;i<UserMaxStreams;i++)
	{
		if (StreamBuffer[i].Status==STREAM_STATUS_ON)	/* Is stream active? */
		{
			c=SOUND_GetStreamSPUChan(i);
			if ((StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)&&(c>=First)&&(c<=Last))
			{
				SPUChan=c;	//StreamBuffer[i].SPUChannel;
				if (I_CDErrorCheck==1)	/* Currently a CD error? */
				{
/* If there is currently a CD error, you can not resume playing streams. */
/* Instead, just set the SPU_CDError flag (which is cleared when calling SOUND_Pause) */
/* This will make the stream resume playing once the CD error is fixed. */

					SPU_CDError[SPUChan]=1;
				}
				else if (SOUND_PauseStreamCheck[SPUChan]!=0)		/* Still preloading from when it was paused. */
				{
/* Otherwise, resume playback of any SPU channel used by a stream */
/* Also, clear the PauseStreamCheck flag. */
/* To do this, make a copy of Any SPU channels that need to be restarted */

					S_Cores=(unsigned int)SOUND_GetCore((int)SPUChan);
					S_Chans=(unsigned int)(SOUND_GetSPUChannel((int)SPUChan))<<1;
					S_Pitch=SPU_PausePitch[SPUChan];
					S_Data=S_Cores|S_Chans|SD_VP_PITCH;

/* Build a list of SPU channels and Pitches to set. Used for the sceSdProcBatch command */
					JPB[S_Counter].func=SD_BSET_PARAM;
					JPB[S_Counter].entry=(unsigned short)S_Data;
					JPB[S_Counter].value=S_Pitch;

					S_Counter++;

					if (StreamBuffer[i].Ch2Flag!=0)	/* Using a more than one SPU channel? */
					{
						for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
						{
							SPUChan2=StreamBuffer[i].SPUChannel2[j];
							S_Cores=(unsigned int)SOUND_GetCore((int)SPUChan2);
							S_Chans=(unsigned int)(SOUND_GetSPUChannel((int)SPUChan2))<<1;
							S_Pitch=SPU_PausePitch[SPUChan];
							S_Data=S_Cores|S_Chans|SD_VP_PITCH;
/* Add 2nd SPU channel data to list also.. */
							JPB[S_Counter].func=SD_BSET_PARAM;
							JPB[S_Counter].entry=(unsigned short)S_Data;
							JPB[S_Counter].value=S_Pitch;

							S_Counter++;
							SOUND_PauseStreamCheck[SPUChan2]=0;
						}
					}
					StreamBuffer[i].Pitch=(short)SPU_PausePitch[SPUChan];	/* Copy pitch back to stream */
					SOUND_PauseStreamCheck[SPUChan]=0;	/* Clear pause flag for SPU channel */
				}
			}
		}
	}

/* Set pitches of all streams */
	if (S_Counter!=0)
		sceSdProcBatch((sceSdBatch *)JPB,0,(unsigned int)S_Counter);

	for (i=(int)First;i<=(int)Last;i++)
	{
		chan=(u_int)i%24;
		core=(u_int)i/24;
		sceSdSetParam ((unsigned short)(core | chan<<1 | SD_VP_VOLL), (unsigned short)SPU_VolL[i]);
		sceSdSetParam ((unsigned short)(core | chan<<1 | SD_VP_VOLR), (unsigned short)SPU_VolR[i]);
	}
}

/**********************************************************************************
SOUND_DelayKeyOnOff
	Delays system after a KeyOn or KeyOff has been performed. You need to wait after
	these to allow a key on or off to actually happen.
	Thanks to Tony Bennett for this one..
***********************************************************************************/
void SOUND_DelayKeyOnOff(void)
{
	DelayThread(TWO_T);	/* Wait two ticks.. */
}


void SOUND_CloseStreamBuffer(int i)
{
int oldstat;
	if (StreamBuffer[i].MemoryAllocated!=0)
	{
		SOUND_StopStream(i,0);
	    if (StreamBuffer[i].MemoryAllocated==1)
		{
			while(CDLoadFlag!=0)			// NEW! - Stop any potential memory leak (loading data over free'd memory)
			{
				DelayThread(ONE_T);			// Wait for loading to complete
			};

		    CpuSuspendIntr (&oldstat);
	 		FreeSysMemory( (void*)StreamBuffer[i].IOPAllocAddress);
	   		CpuResumeIntr (oldstat);
	   	}
		StreamBuffer[i].IOPAddress=0;
		StreamBuffer[i].IOPAllocAddress=0;
		StreamBuffer[i].MemoryAllocated=0;
	}
	else
	{
		MSERROR(("ERROR: SOUND_CloseStreamBuffer\n"));
		MSERROR(("Buffer %d not allocated\n",i));
	}
}

/**********************************************************************************
AdpcmSetParam
	Decodes the command/data packets sent from the EE
***********************************************************************************/
void AdpcmSetParam(short *jdata)
{
int b,i,f,j,p;
u_short dt[256];
int length;
u_int tmp,tmp2,tmp3,tmp4;


	SOUND_MasterEnable=1;

#if (MS_DSP_ACTIVE==1)
	while(SOUND_DSP_PROCESSING!=0)
	{
		DelayThread(ONE_T);
	};
#endif

	if (MS_IntrFlag!=0)		// Set in StreamADPCM to say that MultiStream main thread is running..Cleared at end of routine
	{
		MS_RPCFlag=1;
		SleepThread();
	}

	b=*jdata;		/* Sound.C version ID. */
	jdata++;
	if (b!=SOUND_MULTISTREAM_VERSION_ID)
	{
		printf("---------------------------------\n");
		printf("MULTISTREAM ERROR\n");
		printf("EE and IOP versions do not match\n");
		printf("EE version  = %d.%d\n",b/10,b%10);
		printf("IOP version = %d.%d\n",SOUND_MULTISTREAM_VERSION_ID/10,SOUND_MULTISTREAM_VERSION_ID%10);
		if (b>SOUND_MULTISTREAM_VERSION_ID)
			printf("(IOP Stream.C file is older than EE Sound.C)\n");
		else
			printf("(EE Sound.C file is older than IOP Stream.C)\n");
		printf("---------------------------------\n");
		return;
	}

	b=*jdata;		/* Counter for amount of data packets sent from IOP */
	jdata++;

	for (i=0;i<b;i++)
	{
		f=*jdata;			/* IOP Function to call */
		jdata++;
		length=*jdata;		/* amount of data for required for function. */
		jdata++;

		for (j=0;j<length;j++)
		{
			dt[j]=(unsigned short)*jdata;	/* Copy data for command into a temp buffer */
			jdata++;
		}

		switch (f)
		{
			case SND_GET_CD_STATUS:
				tmp=0;
				if (SOUND_INTERNAL_FastLoad==SOUND_FASTLOAD_ON)	/* Is FASTLOAD enabled? */
				{
					if (dt[0]!=0xffff)	/* EE requesting a stream */
					{
						if (IOPRequestLoad!=dt[0])
							tmp=1;
						else if (StreamBuffer[dt[0]].EEAllow==1)	/* Does stream need EE to request it? */
						{
							if (StreamBuffer[dt[0]].LoadSource2==USE_INTERNAL_LOADING)
							{
								tmp=1;	/* No.. */
							}
						}
					}
					else
						tmp=1;
				}

				if (tmp==0)
				{
					if (StreamBusy!=2)	/* New! 15/10/02. Thanks to Geoff Gates */
					{
						LoadFromEE=dt[0];
						if (LoadFromEE!=0xffff)	/* NEW 29/06/01 */
						{
							LastLoadCheck=dt[1];
							StreamBusy=2;		/* NEW 29/06/01 */
						}
					}
				}

				SOUND_GetStatus();		/* Return status of streams */
				break;

			case SND_PLAY_STREAM:
				USR_HALT_MULTISTREAM_SET();
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* File ID */
				p=SOUND_PlayStream(dt[2],dt[3],dt[4],(int)tmp,dt[5],dt[6],dt[7]);		/* Channel, VolL,VolR,FilenameID,Pitch,LoopingFlag */
				USR_HALT_MULTISTREAM_CLR();
				break;

			case SND_PLAY_SFX:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				SOUND_PlaySFX((int)tmp,dt[2],dt[3],dt[4],dt[5],dt[6],0);
				break;
			case SND_PLAY_SFX_LOOP:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				SOUND_PlaySFX((int)tmp,dt[2],dt[3],dt[4],dt[5],dt[6],dt[7]);
				break;

			case SND_STOP_STREAM:
				SOUND_StopStream(dt[0],0);
				break;

			case SND_STOP_SFX:
				if (dt[0]<24)		/* FIX!! 12/6/2001 */
				{
					tmp=SD_CORE_0;
					tmp2=(unsigned int)dt[0];
				}
				else
				{
					SOUND_KeyOnBitmask[1]&=~(1<<(dt[0]-24));
					tmp=SD_CORE_1;
					tmp2=(u_int)(dt[0]-24);
				}
				SOUND_KeyOnBitmask[tmp]&=~(1<<tmp2);
				GET_SPU_RIGHTS();
				sceSdSetSwitch ((u_short)(tmp | SD_S_KOFF), (u_int)(1<<tmp2));
				SOUND_DelayKeyOnOff();	
				RELEASE_SPU_RIGHTS();

/* NOW GET THE SPU CHANNEL STATUS, SO IT CAN BE PASSED BACK TO THE EE ! */
				tmp=sceSdGetParam((unsigned short)(tmp|(tmp2<<1)|SD_VP_ENVX));	/* Get current envelope */
				if(tmp==0)	/* This means that it has been keyed off */
				{
				    SPU_KeyStatus[dt[0]]=SPU_KEY_STATUS_OFF;
					SPU_LoopCounter[dt[0]]=0;
				}
				break;

			case SND_PRIORITY_STREAM:
				SOUND_SetStreamPriority(dt[0],dt[1]);
				break;

			case SND_SET_VOL_LR:
				SOUND_SetChannelVolume(dt[0],dt[1],dt[2],0);
				break;

			case SND_SET_PITCH:
				SOUND_SetChannelPitch(dt[0],dt[1]);
				break;

			case SND_SET_VOL_SMOOTH:
				SOUND_SetChannelVolumeSmooth(dt[0],dt[1],dt[2]);
				break;

			case SND_PAUSE:
				SOUND_Pause();
				break;

			case SND_PAUSE_SPU_CHANS:
				SOUND_PauseSPUChannels(dt[0],dt[1]);
				break;

			case SND_RESUME_SPU_CHANS:
				SOUND_ResumeSPUChannels(dt[0],dt[1]);
				break;

			case SND_RESUME:
				SOUND_Resume();
				break;

			case SND_PAUSE_PCM:
				SOUND_PausePCM();
				break;

			case SND_RESUME_PCM:
				SOUND_ResumePCM();
				break;

			case SND_APPEND_STREAM:
				tmp=(unsigned int)(dt[2]+(dt[1]<<16));		/* File ID */
				SOUND_AppendStream(dt[0],(int)tmp,dt[3]);
				break;

			case SND_IGNORE_VAG_PITCH:
				SOUND_StreamIgnoreVAGPitch(dt[0]);
				break;

			case SND_STREAM_LOAD_SOURCE:
				SOUND_SetStreamLoadSource(dt[0],dt[1]);
				break;

			case SND_SET_MAX_STREAM_LIMIT:
				if (dt[0]<MAX_NUMBER_OF_STREAMS)
					UserMaxStreams=dt[0];
				break;

			case SND_CLOSE_STREAM_BUFFER:
				SOUND_CloseStreamBuffer(dt[0]);
				break;

			case SND_INIT_PCM:
				SOUND_InitPCMPlayback(dt[0],dt[1],dt[2]);
				break;

			case SND_SET_PCM_VOLUME:
				SOUND_SetPCMVolume(dt[0],dt[1]);
				break;


			case SND_SET_MULTI_CHAN_PITCH:
				tmp=(unsigned int)(dt[2]+(dt[1]<<16));	/* Core0 */
				tmp2=(unsigned int)(dt[4]+(dt[3]<<16));	/* Core1 */
				SOUND_SetMultiChannelPitch(dt[0],tmp,tmp2);	/* pitch,core0,core1 */
				break;

			case SND_SET_FILTER:
#if (MS_DSP_ACTIVE==1)
				SOUND_SetDSP(dt[0],dt[1],dt[2]);
#endif
				break;

			case SND_PLAY_PCM:
				SOUND_StartPCM();
				break;

			case SND_STOP_PCM:
				SOUND_StopPCM();
				break;

			case SND_CLOSE_PCM_PLAYBACK:
				SOUND_ClosePCMPlayback();
				break;

			case SND_INIT_PCM_NO_STREAM:
				tmp=(unsigned int)(dt[0]<<16);
				tmp+=dt[1];		/* Stream buffer size */
				SOUND_InitPCMPlaybackNoStream(tmp,dt[2]);
				break;

			case SND_SET_PCM_TYPE:
				SOUND_SetPCMType(dt[0],dt[1]);
				break;

			case SND_SET_SPU_START_ADDRESS:
				tmp=(unsigned int)(dt[0]<<16);
				tmp+=dt[1];
				StreamMemoryEndFlag=1;
				StreamMemoryEnd=tmp;			/* Set new SPURAM base address to load next resident SFX to. */
				break;

			case SND_ALLOC_STREAM_BUFFER:
				tmp=(unsigned int)(dt[1]<<16);
				tmp+=dt[2];		/* SPU RAM Address */
				tmp2=(unsigned int)(dt[3]<<16);
				tmp2+=dt[4];	/* Size */
				SOUND_AllocateStreamBuffer(dt[0],tmp,tmp2);
				break;

			case SND_SET_STREAM_BUFFER:
				tmp=(unsigned int)(dt[1]<<16);
				tmp+=dt[2];		/* IOP RAM Address */
				tmp2=(unsigned int)(dt[3]<<16);
				tmp2+=dt[4];	/* IOP Size */
				tmp3=(unsigned int)(dt[5]<<16);
				tmp3+=dt[6];	/* SPU RAM Address */
				tmp4=(unsigned int)(dt[7]<<16);
				tmp4+=dt[8];	/* Size */
				SOUND_SetStreamBuffer(dt[0],tmp,tmp2,tmp3,tmp4);
				break;

			case SND_RESIZE_STREAM_BUFFER:
				tmp=(unsigned int)(dt[1]<<16);
				tmp+=dt[2];		/* Stream buffer size */
				SOUND_ResizeStreamBuffer(dt[0],tmp);
				break;

			case SND_RESIZE_SPU_BUFFER:
				tmp=(unsigned int)(dt[1]<<16);
				tmp+=dt[2];		/* Stream buffer size */
				SOUND_ResizeSPUBuffer(dt[0],tmp);
				break;

			case SND_SET_STREAM_START_POS:
				tmp=(unsigned int)(dt[1]<<16);
				tmp+=dt[2];		/* Stream start offset */
				SOUND_SetStreamStartPosition(dt[0],tmp);
				break;
				
			case SND_CHAIN_STREAMS:
				SOUND_ChainStreams(dt[0],dt[1],&dt[2]);
				break;

			case SND_ENABLE_FX:
				if (dt[1]==0)
					SOUND_DisableEffects(dt[0]);
				else
					SOUND_EnableEffects(dt[0],dt[1],dt[2],dt[3],dt[4],dt[5]);
				break;

			case SND_DISABLE_FX:
				SOUND_DisableEffects(dt[0]);
				break;

			case SND_SET_SFX_OFFSET:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/*  ID */
				tmp2=(unsigned int)(dt[4]+(dt[3]<<16));		/* SPU offset */
				SOUND_SetSFXPlayOffset(tmp,dt[2],tmp2);
				break;

			case SND_SET_EFFECT_ADDR:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* SPU Address, core0 */
				tmp2=(unsigned int)(dt[3]+(dt[2]<<16));		/* SPU Address, core1 */
				SOUND_SetEffectBufferAddress(tmp,tmp2);
				break;

			case SND_SET_FX_MASTER_VOL:
				SOUND_SetEffectMasterVolume(dt[0],dt[1],dt[2]);
				break;

			case SND_FX_ALL_CHANS_ON:
				SOUND_AllChannelEffectsOn(dt[0]);
				break;

			case SND_FX_ALL_CHANS_OFF:
				SOUND_AllChannelEffectsOff(dt[0]);
				break;

			case SND_FX_CHAN_ON:
				SOUND_SetChannelEffectOn(dt[0]);
				break;

			case SND_FX_CHAN_OFF:
				SOUND_SetChannelEffectOff(dt[0]);
				break;

			case SND_OUT_ALL_CHANS_ON:
			    SOUND_AllChannelOutputOn(dt[0]);
			    break;

			case SND_OUT_ALL_CHANS_OFF:
			    SOUND_AllChannelOutputOff(dt[0]);
			    break;

			case SND_OUT_CHAN_ON:
			    SOUND_SetChannelOutputOn(dt[0]);
			    break;

			case SND_OUT_CHAN_OFF:
			    SOUND_SetChannelOutputOff(dt[0]);
			    break;

			case SND_CHECK_STEREO_READY:
				InitCheckStereoStatus(dt[0],dt[1],dt[2]);
				break;

			case SND_CANCEL_CHECK_STEREO_READY:
				ClearCheckStereoStatus();
				break;

			case SND_INIT_SPU:
				SOUND_InitSPU(0);		/* data means nothing */
				break;

			case SND_INIT_STREAM:
				SOUND_InitStreamData(dt[0],dt[1],dt[2],dt[3]);		/* Sets AnyStream to 1 if any files found - 0 otherwise... */
				break;

			case SND_PATCH_SFX:
				tmp2=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				tmp=(unsigned int)(dt[3]+(dt[2]<<16));		/* SPU Addr */
				SOUND_PatchSFX((int)tmp2,tmp);
				break;

			case SND_GET_FILE_INFO:
				SOUND_GetFileInfo(dt[0],&dt[1]);	/* counter, filename */
				break;

			case SND_SET_FILE_INFO:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				tmp2=(unsigned int)(dt[3]+(dt[2]<<16));		/* LSN */
				tmp3=(unsigned int)(dt[5]+(dt[4]<<16));		/* SIZE */
				tmp4=(unsigned int)(dt[8]+(dt[7]<<16));		/* OFFSET */
				SOUND_SetFileInfo((int)tmp,(int)tmp2,tmp3,dt[6],tmp4);
				break;

			case SND_CREATE_FILE_INFO:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				tmp2=(unsigned int)(dt[3]+(dt[2]<<16));		/* OFFSET */

				SOUND_CreateFileInfo((int)tmp,tmp2,&dt[4]);
				break;

			case SND_FREE_FILE_ID:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				SOUND_FreeFileID((int)tmp);
				break;

			case SND_FREE_ALL_FILE_ID:
				SOUND_FreeAllFileIDs();
				break;

			case SND_STORE_HOST0_FILENAME:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				SOUND_StoreHOST0FileName((int)tmp,&dt[2]);
				break;

			case SND_ALLOCATE_FILE_RAM:
				SOUND_AllocateFileMemory(dt[0]);
				break;

			case SND_FREE_FILE_RAM:
				SOUND_FreeFileMemory();
				break;

			case SND_ALLOCATE_SFX_RAM:
				SOUND_AllocateSpotFXMemory(dt[0]);
				break;

			case SND_FREE_SFX_RAM:
				SOUND_FreeSpotFXMemory();
				break;

			case SND_FREE_ALL_SFX_ID:
				SOUND_FreeAllSpotFXIDs();
				break;

			case SND_FREE_SFX_ID:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				SOUND_FreeSpotFXID((int)tmp);
				break;

			case SND_SET_FILE_OFFSET_AND_SIZE:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				tmp2=(unsigned int)(dt[3]+(dt[2]<<16));		/* OFFSET */
				tmp3=(unsigned int)(dt[5]+(dt[4]<<16));		/* SIZE */
				SOUND_SetFileOffsetAndPlaySize((int)tmp,tmp2,tmp3);
				break;

			case SND_SET_EE_TRANS_ADDRESS:
				SOUND_ModifyEETransferAddress(dt[0],(unsigned int)(dt[2]+(dt[1]<<16)));
				break;

			case SND_INIT_CD:
				SOUND_InitCD(dt[0]);
				break;

			case SND_SET_STREAM_PARENT:
				tmp=(unsigned int)(dt[3]+(dt[2]<<16));
				SOUND_SetStreamParent_Int(dt[0],dt[1],tmp);
				break;

			case SND_SET_STREAM_CHILD:
				tmp=(unsigned int)(dt[6]+(dt[5]<<16));
				SOUND_SetStreamChild_Int(dt[0],dt[1],dt[2],dt[3],dt[4],tmp);
				break;


			case SND_SET_MIB_END:
				tmp=(unsigned int)(dt[2]+(dt[1]<<16));
				SOUND_SetMIBEndOffset(dt[0],tmp);
				break;

			case SND_GET_MAX_RAM:
				SOUND_GetMaxIOPRAM();
				break;

			case SND_ALLOW_KON:
				SOUND_StreamAllowKeyOn(dt[0]);
				break;

			case SND_DISABLE_KON:
				SOUND_StreamDisableKeyOn(dt[0]);
				break;

			case SND_SET_INT_TRACK:
				SOUND_SetStreamTrack_Int(dt[0],dt[1]);
				break;

			case SND_SET_EE_START_ADDRESS:
				tmp=(unsigned int)(dt[0]<<16);
				tmp+=dt[1];
				StreamEEFlag=1;
				StreamEEAddress=tmp;			/* Set new EE transfer address - Used if specified STREAM_LOAD_EE */
				break;

			case SND_SET_IOP_START_ADDRESS:
				tmp=(unsigned int)(dt[0]<<16);
				tmp+=dt[1];
				StreamIOPFlag=1;
				StreamIOPAddress=tmp;			/* Set new IOP transfer address - Used if specified STREAM_LOAD_IOP */
				break;

			case SND_GET_FILE_ID_INFO:
				tmp=(unsigned int)(dt[2]+(dt[1]<<16));	/* ID */
				SOUND_GetFileIDInfo(dt[0],(int)tmp);
				break;

			case SND_TRANS_IOP_TO_SPU:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));	/* IOP address */
				tmp2=(unsigned int)(dt[3]+(dt[2]<<16));	/* SPU address */
				tmp3=(unsigned int)(dt[5]+(dt[4]<<16));	/* Size */
				SOUND_TransIOPToSPU(tmp,tmp2,tmp3);
				break;

			case SND_CD_ERROR_RESTART:
				SOUND_RestartFromCDError();
				break;

			case SND_CD_ERROR_PAUSE_EX:
				SOUND_EXTERNAL_CDErrorPause();
				break;

			case SND_CD_ERROR_RESTART_EX:
				SOUND_EXTERNAL_CDErrorRestart();
				break;

			case SND_CD_CHECK:
				SOUND_CheckCDError();
				break;

			case SND_DISABLE_CD_CALLBACK:
				SOUND_DisableCDCallback();
				break;

			case SND_DISABLE_SPU_CALLBACK:
				SOUND_DisableSPUCallback();
				break;

			case SND_ENABLE_SPU_CALLBACK:
				SOUND_EnableSPUCallback();
				break;

			case SND_INIT_DTS:
				SOUND_SetDTSMode(dt[0],dt[1]);
				break;

			case SND_SET_KEY_ON_FLAG:
				SOUND_SFXAllowKeyOn=dt[0];
				break;

			case SND_KEY_ON_CHAN:
				SOUND_KeyOnChannel(dt[0]);
				break;

			case SND_STRM_ADD_CH:
				SOUND_StreamAddSPUChannel(dt[0],dt[1],dt[2],dt[3]);
				break;

			case SND_SET_OUTPUT:
				SOUND_SetOutput(dt[0]);
				break;

			case SND_INIT_WAIT:

				tmp=(u_int)dt[1]+(dt[0]<<16);
				SOUND_WaitEEAddress=tmp;
				break;

			case SND_SET_IOP_DTS_INFO:
				SOUND_DTS_Surround=dt[0];
				break;

			case SND_INTERNAL_FASTLOAD:
				SOUND_InitINTERNALFastLoad(dt[0],dt[1]);
				break;

			case SND_TV_OUTPUT_ON:
				SOUND_SetTVOutputOn();
				break;

			case SND_TV_OUTPUT_OFF:
				SOUND_SetTVOutputOff();
				break;

			case SND_SET_SWITCH:
				tmp=(unsigned int)(dt[1]<<16);
				tmp+=dt[2];
				SOUND_SetSwitch(dt[0],tmp);
				break;
			case SND_SET_PARAM:
				SOUND_SetParam(dt[0],dt[1]);
				break;

			case SND_SET_ADDR:
				tmp=(unsigned int)(dt[1]<<16);
				tmp+=dt[2];
				SOUND_SetAddr(dt[0],tmp);
				break;

			case SND_SET_COREATTR:
				SOUND_SetCoreAttr(dt[0],dt[1]);
				break;

			case SND_BLOCK_TRANS:
				tmp=(u_int)dt[3]+(dt[2]<<16);
				tmp2=(u_int)dt[5]+(dt[4]<<16);
				tmp3=(u_int)dt[7]+(dt[6]<<16);
				SOUND_BlockTrans(dt[0],dt[1],tmp,tmp2,tmp3);
				break;

			case SND_SET_THREAD_PRIORITY:
				SOUND_SetThreadPriority(dt[0]);
				break;

			case SND_ATWINMON_OPEN:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				SOUND_ATWinMonOpenFile(tmp);
				break;
			case SND_ATWINMON_CLOSE:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				SOUND_ATWinMonCloseFile(tmp);
				break;

			case SND_CLOSE_IOP_RESOURCES:
				SOUND_CloseIOPResources();
				break;

			case SND_SET_DSP_EFFECT:
#if (MS_DSP_ACTIVE==1)
				SOUND_SetDSPEffect(dt[0],dt[1],dt[2]);
#endif
				break;

			case SND_SET_DSP_BUFFER:
#if (MS_DSP_ACTIVE==1)
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));	/* EE Address for PCM data */
				tmp2=(unsigned int)(dt[3]+(dt[2]<<16));	/* EE Address to transfer counter info */
				SOUND_DSPBuffer=tmp;
				SOUND_DSPBufferCounter=tmp2;
				SOUND_DSP_EEActive=(char)SOUND_AllocateEEDSP(dt[4],dt[5]);	// ON/OFF & data packets to allocate
#endif
				break;

			case SND_SET_DSP_COUNT:
#if (MS_DSP_ACTIVE==1)
				SOUND_SetDSPCount(dt[0]);
#endif
				break;

			case SND_CLOSE_DSP:
#if (MS_DSP_ACTIVE==1)
				SOUND_CloseDSP();
#endif
				break;

			case SND_SET_MULTISTREAM_VOICES:
				tmp=(unsigned int)(dt[1]+(dt[0]<<16));		/* ID */
				tmp2=(unsigned int)(dt[3]+(dt[2]<<16));	/* SPU address */
				SOUND_SetMultiStreamVoices( tmp, tmp2 );
				break;

			default:
				MSERROR(("AdpcmSetParam: Unknown command %d (decimal)\n",f));
				break;

		}
	}

/* Handle keying on multiple SPU channels if SOUND_KeyOnChannel is called.. */

	if (gThid)
	{
		tmp=0;
		for (i=0;i<2;i++)
		{
			if (SOUND_KeyOnBitmask[i]!=0)	/* Any SPU channels to be keyed on for core n ? */
			{
				GET_SPU_RIGHTS();
				sceSdSetSwitch ((unsigned short)(i | SD_S_KON), (unsigned int)SOUND_KeyOnBitmask[i]);	/* Yes.. */
				SOUND_KeyOnBitmask[i]=0;
				SOUND_DelayKeyOnOff();
				RELEASE_SPU_RIGHTS();
				tmp++;
			}
		}
		if (tmp!=0)
		{
			GET_SPU_RIGHTS();
			SOUND_UpdateSmoothVols();
			RELEASE_SPU_RIGHTS();
		}
	}


SOUND_MasterEnable=0;

#if MS_TIMER
	SignalSema(gSem);	/* Signal StreamADPCM to be called */
#endif

}


/**********************************************************************************
SOUND_StreamAddSPUChannel
	Makes a stream also play the same audio on another  SPU channel.
	Should be called straight after SOUND_PlayStream, so that everything can be
	set up correctly before any audio has started playing.
	This command is useful if you want to play back streams in surround sound
	(such as DTS). Not so useful for Dolby, as that requires a slight delay between keying
	on the two SPU channels..
	A maximum of 3 Additional SPU channels can be gived to each audio stream.

	Requires:	StrmChan		Stream channel that required stream is playing on
				SPUChan			SPU channel required to also play on.
				VolL			Left volume
				VolR			Right volume

	Returns:	Nothing

***********************************************************************************/
void SOUND_StreamAddSPUChannel(int StrmChan,int SPUChan,int VolL,int VolR)
{
int a;

	if (StreamBuffer[StrmChan].Status!=STREAM_STATUS_ON)
	{
		MSERROR((">>>>> ERROR:SOUND_StreamAddSPUChannel\n"));
		MSERROR(("Stream channel %d is not ON. This function must be called after SOUND_PlayStream\n",StrmChan));
		return;
	}
	if (StreamBuffer[StrmChan].Ch2Flag>=3)
	{
		MSERROR((">>>>> ERROR:SOUND_StreamAddSPUChannel\n"));
		MSERROR(("Stream channel %d is already playing audio on 4 SPU channels\n",StrmChan));
		MSERROR(("Request for SPU channel %d denied\n",SPUChan));
		return;
	}

	if (StreamBuffer[StrmChan].ResidentFlag==STREAM_AUDIO_ADPCM)
	{
		a=StreamBuffer[StrmChan].Ch2Flag;

		SOUND_StopStream(SPU_CH+SPUChan,0);		/* Stop any stream using requested SPU channel */
		StreamBuffer[StrmChan].SPUChannel2[a]=(unsigned short)SPUChan;	/* New SPU channel */
		StreamBuffer[StrmChan].VolL2[a]=(unsigned short)VolL;	/* Volume LEFT */
		StreamBuffer[StrmChan].VolR2[a]=(unsigned short)VolR;	/* Volume RIGHT */
		StreamBuffer[StrmChan].Ch2Flag++;	/* Mark stream as using more than one SPU channel */
		SPU_KeyStatus[SPUChan]=SPU_KEY_STATUS_USED_BY_STREAM;	/* Mark SPU channel as used */
		if (StreamBuffer[StrmChan].KeyOffFlag==0)
		{
			SPU_PausePitch[SPUChan]=(unsigned short)StreamBuffer[StrmChan].Pitch;	/* Use pitch */
		}
	}
	else
	{
		MSERROR((">>>>> ERROR:SOUND_StreamAddSPUChannel\n"));
		MSERROR(("Stream channel %d is not set up for playing ADPCM audio\n",StrmChan));
	}
}

/**********************************************************************************
HandleEERequest
	Called if a request has been allowed by the EE - so go ahead and load in sample data.
	Basically, the LoadDataFromDisk routine should NOT load data anymore - allowing your code
	to do this instead, but the callbacks that this function calls on completion of a load still
	need to be called when your load as successfully loaded data. If your load fails, it should be
	quite simple to shut down the stream/sfx loading to stop any further problems.

	Requires:	i			Stream number
				child		flag if stream is a child (1=child, 0=not a child)

	iSTATUS goes in this order:

	WAIT,INITIALISED,preTRANSFER1,(preTRANSFER2 if playing MIB files),PLAY,KEYED_ON,PLAYING
	
	iSTATUS_WAIT			Stream not doing anything (just waiting around to play..)
	iSTATUS_INITIALISED		SOUND_PlayStream called
	iSTATUS_preTRANSFER1	(stream preloading data)
		iSTATUS_preTRANSFER2 (stream copying more data from loaded data ONLY if a PARENT or CHILD)
	iSTATUS_PLAY	(stream keyed ON, more data loaded for 1st half of stream buffer)
	iSTATUS_KEYED_ON	(loading of 1st buffer half completed)
	iSTATUS_PLAYING		(checks for playback of a buffer half, loads more data if required)

***********************************************************************************/
int HandleEERequest(int i,int child)
{
u_int parent;
int flag;

	parent=(unsigned int)StreamBuffer[i].ParentStream;

	if (child==1)		/* Is this function being called by a "Child" stream? */
		cd_stream=i;	/* If so, set cd_stream.(Normally done within cd loading code..) */

	if (StreamBuffer[i].Status==STREAM_STATUS_ON)	/* Is stream ON? */
	{
		StreamBusy=1;	/* Tells EE that Stream loading from CD is happening. */

		if (StreamBuffer[i].iStatus==iSTATUS_preTRANSFER1)	/* Want to preload data? */
		{
/* If stream is a CHILD, just do the IOP->SPU transfer(data already loaded by parent) */
			if (child==1)
			{
				UpdateStream(cd_stream,(int)StreamBuffer[parent].AllocSize, STREAM_PRELOAD,
				cdend_InterleavedSecondTransfer);	/* Transfer it to SPU2 */
			}
			else
			{
/* Either a PARENT or a non-MIB stream. */
				LoadDataFromDisk(i,(int)StreamBuffer[i].AllocSize, cdend_PreloadDataNowInIOP);
			}
		}
		else if (StreamBuffer[i].iStatus==iSTATUS_preTRANSFER2)	/* Only for PARENT or CHILD streams */
		{
			cd_stream=i;		/* Transfer second half of preload buffer to SPU. */
			UpdateStream(i,(int)StreamBuffer[i].AllocSize, STREAM_PRELOAD2,
					cdend_IntPreloadedDataNowInSPU);	/* Transfer it to SPU2 */
			IOPRequestLoad=-1;	/* EE doesn't need to request a load. Data is already here! */
/* StreamBusy is cleared in the UpdateStream cdend_ callback function. */
/* In this case, cdend_IntPreloadedDataNowinSPU */
		}

		else if (StreamBuffer[i].iStatus==iSTATUS_PLAY)
		{
			if ((StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_ADPCM)&&	/* Non-Audio stream? */
				(StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_PCM))
			{
				cd_stream=i;	/* New! EE Loading fix! JP 12/November/2002 */
				StreamBuffer[i].iStatus=iSTATUS_KEYED_ON;	/* Yes. No need to KeyOn anything. */
/* Request a full buffer load (rather than a half buffer load if playing audio) */
				cdend_KeyedOn(0);	/* Just perform data loaded callback */
			}
			else
			{

/* so now the initial full buffer has been transfered to SPU, we can load the */
/* first half of the new data into IOP RAM...BUT..Before we do that, start playing */
/* the sample that we have loaded. This stops any unnecessary delays between loading */
/* the data and actually playing the stuff.. */

/* MS6. Check for Playbuffer full before keying on.. */

#if (MS_LOAD62==1)
			    if ((StreamBuffer[i].DMAInfo.TransCount==0)&&(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM))
				{
					StreamBusy=0;	/* Probably due to a CHILD stream not being ready yet.. */
					return(-1);
				}
#endif

				flag=SOUND_PerformKeyOn(i);	/* Try to key on SPU channel */
				if (flag==-1)
				{
					if (StreamBuffer[i].LoadSource2==USE_EXTERNAL_LOADING)
						IOPRequestLoad=-1;

					MSERROR(("Key On Failed!\n"));
					StreamBusy=0;	/* Probably due to a CHILD stream not being ready yet.. */
					return(-1);
				}

				if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)
				{
/* Mark SPU channel as ON */
					SPU_KeyStatus[StreamBuffer[i].SPUChannel]=SPU_KEY_STATUS_ON;
				}

				if ((StreamBuffer[i].PlayingHalf!=STREAM_SHORT)&&
				(StreamBuffer[i].PlayingHalf!=STREAM_PLAYING_SHORT_LOOP))
				{

					StreamBuffer[i].iStatus=iSTATUS_KEYED_ON;
					cd_stream=i;
					cdend_KeyedOn(0);	/* Just perform data loaded callback */
				}
				else
				{
					StreamBuffer[i].iStatus=iSTATUS_PLAYING;	/* CHANGE STATUS */
					if (StreamBuffer[i].LoadSource2==USE_EXTERNAL_LOADING)
					{
						StreamBusy=0;
						return(-1);
					}
				}
				StreamBusy=0;
			}
		}
		else if (StreamBuffer[i].iStatus==iSTATUS_PLAYING)	/* Stream playing */
		{
/* Now data is preloaded, and any audio streams are keyed on (playing) */
			if ((StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_ADPCM)&&
				(StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_PCM))
			{
		MS_IntrFlag=0;
/* Non-Audio data just keeps requesting more data to be loaded. Doesn't have to wait for half a */
/* buffer to be played or anything. */
				SOUND_StartLoad(i,(cdFunc)cdend_LoadedNonAudio);
			}
			else
			{
				if (StreamBuffer[i].PlayingHalf==STREAM_FIRST_HALF)	/* Need data for first half. */
				{
					if (child==1)		/* Is this function being called by a "Child" stream? */
					{
						UpdateStream(i,(int)StreamBuffer[i].AllocHalfSize,STREAM_FIRST_HALF,cdend_FirstHalfPlayed);
					}
					else
					{
						SOUND_StartLoad(i,(cdFunc)cdend_LoadedFirstHalf);
					}
				}
				else if (StreamBuffer[i].PlayingHalf==STREAM_SECOND_HALF)	/* Need data for 2nd half */
				{
					if (child==1)		/* Is this function being called by a "Child" stream? */
					{
						UpdateStream(i,(int)StreamBuffer[i].AllocHalfSize,STREAM_SECOND_HALF,cdend_SecondHalfPlayed);
					}
					else
					{
						SOUND_StartLoad(i,(cdFunc)cdend_LoadedSecondHalf);
					}
				}
				else if ((StreamBuffer[i].PlayingHalf==STREAM_SHORT)&&(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM))
				{
					if (PCMPlaying==1)	/* PCM playback coming to an end.. */
					{
/* Set flag to let SOUND_CheckForBufferHalfPlayed knows that PCM needs no more new data. */
						PCMPlaying=2;
/* Get Current PCM buffer half (used by SOUND_CheckForBufferHalfPlayed) */
						PCMHalf = (int)(sceSdBlockTransStatus (MS_PCM_CORE, 0) >> 24);
					}
				}
				if ((StreamBuffer[i].PlayingHalf==STREAM_SHORT)||
				(StreamBuffer[i].PlayingHalf==STREAM_PLAYING_SHORT_LOOP))
				{
					StreamBusy=0;	/* No loading required */
				}

			}					 
		}
		else
		{
			StreamBusy=0;
		}
	}
	return(0);
}

/**********************************************************************************
InitCheckStereoStatus
	Call from EE will mark two SPU Channels and a Pitch value.
	Once two streams that are using these two SPU Channels are playing (but with
	their pitches at 0), the Pitch value will be copied to both SPU Channels
	Therefore playing two channels in stereo.
	So for the two Streams, one should be panned hard left, the other hard right.
	Their pitch should be 0 and the two SPU channels that they use should be
	passed into this function along with the final playback SPU pitch.

	Requires:	channel1		SPU Channel 1 (0-47)
				channel2		SPU Channel 2 (0-47)
				pitch			SPU Pitch

FIX 18/4/2001 (now uses == NOT -= ...Thanks Jake!)

V5 Update. No longer checks for DEFAULT_PITCH, where the pitch would be read from a
FileNames.H file. FileNames.H used to be a shared file between both EE and IOP code
(on versions before V5). This no longer exists.
Also note that this function is not really required with V5, due to the ability to
play interleaved ADPCM files. 

***********************************************************************************/
void InitCheckStereoStatus(int channel1,int channel2,u_int pitch)
{

	StereoStarted=0;	/* Clear this flag while setting other info..Just incase of an interrupt. */
	StereoChan1=channel1;	/* SPU channel 1 */
	StereoChan2=channel2;	/* SPU channel 2 */
	StereoPitch=pitch;		/* SPU Pitch to set both SPU channels to, when both are keyed on */

	StereoStarted=1;		/* Flag to allow checking for status of both SPU channels */

}

/**********************************************************************************
ClearCheckStereoStatus

	Clears any previous request to set two channels pitches (See InitCheckStereoStatus).
	Use this if, say, going between ingame and menus. Just to cancel any requests
	that may not have been setup earlier.
	Not really required now that V5.0+ handles interleaved audio data files (MIB/MIH)
***********************************************************************************/
void ClearCheckStereoStatus(void)
{
	StereoStarted=0;	/* Disable stereo checking */
}

/**********************************************************************************
CheckTwoStreamsStatus
	This routine checks the current stream status of the two streams that may be
	required to playback as stereo. If both streams are playing (ie. their buffers
	are full and their SPU channel has been keyed ON), then set the pitch for
	both channels.
	See InitCheckStereoStatus for more details.

	(V5) NOTE: This function is not really required any more. Although you can play
	stereo sound with the use of two seperate file and streams, you should really
	now be using the interleaved file functions which allow for multiple stream
	files to be created and played.
***********************************************************************************/
void CheckTwoStreamsStatus(int channel1,int channel2,u_int Pitch)
{
int i;
int ch1=0;
int ch2=0;

	if (StereoStarted==0)	/* Are we checking to see if two SPU channels are ready? */
		return;	/* No. */

	for (i=0;i<UserMaxStreams;i++)		/* Stop stream that is playing on SPU Channel n */
	{
		if (StreamBuffer[i].Status!=STREAM_STATUS_OFF)	/* Is Stream ON? */
		{
			if ((StreamBuffer[i].iStatus==iSTATUS_PLAYING)&&(StreamBuffer[i].SPUChannel==channel1)
				&&(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM))
			{
				ch1=1;		/* Found a stream using SPU Channel 1, and its ready to play */
			}
			else if ((StreamBuffer[i].iStatus==iSTATUS_PLAYING)&&(StreamBuffer[i].SPUChannel==channel2)
				&&(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM))
			{
				ch2=1;		/* Found a stream using SPU Channel 2, and its ready to play */
			}
		}
	}
	if ((ch1==1)&&(ch2==1))		/* Check if both streams are in a "ready to play" state. */
	{
		SOUND_SetChannelPitch(channel1,(int)Pitch);	/* Yes they are. So set both SPU pitches */
		SOUND_SetChannelPitch(channel2,(int)Pitch);
		StereoStarted=0;	/* Clear this flag, as we've started both SPU channels now. */
	}
}

/**********************************************************************************
GetChannelStatus
	Fills the buffer SPU_KeyStatus with the current SPU status for each channel.
	Also takes into consideration if a channel is being used by a stream, but not
	actually keyed ON yet (due to data not yet loaded from CD)

	SPU_KeyStatus[n] =	0	Nothing Playing on SPU Channel
						1	SPU Channel active
						2	SPU Channel going to be used for streaming.

***********************************************************************************/
void GetChannelStatus(void)
{
int env;
int i;
int core,channel;


#if MS_TIMER
	if (SOUND_TimerUpdate==0)		/* Only call this routine if woken by main Timer Sema. */
		return;
#endif

	for (i=0;i<48;i++)	/* Check each SPU channel */
	{
		if (SPU_KeyStatus[i]!=SPU_KEY_STATUS_USED_BY_STREAM)	/* Used by stream? */
		{

/* This part gives MultiStream a little time to key on channels before checking their status. */
/* If you check too fast after keying on, the keyon may not have been registered yet. */

			if (SPU_KeyStatus[i]==SPU_KEY_STATUS_JUST_KEYED_ON)	/* No. Has it just been turned on? */
				SPU_KeyStatus[i]=SPU_KEY_STATUS_ON;				/* Yes, so make it ON. */
			else
			{
				channel=SOUND_GetSPUChannel(i);		/* Get SPU Channel */
				core=SOUND_GetCore(i);				/* Get SU Core */
				env=sceSdGetParam((unsigned short)(core|(channel<<1)|SD_VP_ENVX));	/* Get current envelope */
				if(env==0)	/* This means that it has been keyed off, or reached end of data. */
				{

					if (SPU_KeyStatus[i]!=SPU_KEY_STATUS_OFF)
					{
					    SPU_KeyStatus[i]=SPU_KEY_STATUS_OFF;	/* Thanks Nathalie! */
						SPU_LoopCounter[i]=0;			   		/* Clear Loop counter */
					}
				}
				else
				{
				    SPU_KeyStatus[i]=SPU_KEY_STATUS_ON;		/* SPU Channel is active. */
				}
			}
		}
	}
}

/**********************************************************************************
SOUND_UpdateLoopingSFX
	Updates looping SFX, stopping them if required.
***********************************************************************************/
void SOUND_UpdateLoopingSFX(void)
{
int core,channel;
int voicestat;
int offset;
int v;
int koff;

	for (core=0;core<2;core++)	/* Check through both cores (24 SPU channels on each core) */
	{
		koff=0;		/* Bit mask for keying off for core n */

/* Get ENDX info (bitmask, 1=playback reached end of sample data) */
		voicestat=(int)sceSdGetSwitch((unsigned short)(core| SD_S_ENDX));
		for (channel=0;channel<24;channel++)
		{
			offset=(core*24)+channel;	/* Offset into SPU_LoopCounter for correct SPU channel */

/* Is a SPU channel active AND playing a looping sample n times ? */
			if ((SPU_KeyStatus[offset]==SPU_KEY_STATUS_ON)&&(SPU_LoopCounter[offset]!=0))
			{
				if (SPU_LoopCounter[offset]==1)	/* Playback on its last loop? */
				{
					sceSdSetAddr ((unsigned short)(core | channel<<1 | SD_VA_LSAX), 0x5000);	/* Yes..Point playback loop to 0x5000 */
					SPU_LoopCounter[offset]=-1;	/* Mark as playing its last time */
				}
				else
				{
					v=(voicestat>>channel)&1;
					if (v==1)					/* Has SPU channel looped? */
					{
						if (SPU_LoopCounter[offset]==-1)	/* Final playback completed? */
						{
/* Setting ADSR2 stops the SPU channel from fading out when keyed off, depending on the release */
/* rate. So you don't have to wait for a channel to actually stop playing. */
							sceSdSetParam ((unsigned short)(core | (channel<<1) | SD_VP_ADSR2), 0x1fc0);
							koff|=(1<<channel);				/* Build keyOff bitmask for Core n */
							SPU_LoopCounter[offset]=0;		/* Set Loop counter to 0 */
						}
						else
							SPU_LoopCounter[offset]--;		/* Decrease Loop counter */
					}
				}
			}
		}
		if (koff!=0)
		{
			GET_SPU_RIGHTS();
			sceSdSetSwitch ((unsigned short)(core | SD_S_KOFF), (unsigned int)koff); 	/* KeyOFF any required channels on Core n */
			SOUND_DelayKeyOnOff();
			RELEASE_SPU_RIGHTS();
		}
		sceSdSetSwitch ((unsigned short)(core | SD_S_ENDX),0);		/* Clear ENDX bitmask. */
	}
}

/**********************************************************************************
SOUND_EnableEffects
	Initialises reverb on Core_n

	Requires:	core		(0 or 1)
				type		Reverb type (STUDIO_A, HALL etc..)
				depl		Depth Left (0-0x7fff)
				depr		Depth Right (0-0x7fff)
				delay		Delay (type ECHO or DELAY only)
				feedback	Feedback (type ECHO only)


	Hardware effect buffers will be stored in SPU RAM.
	SPU RAM Addresses are: 0x1FFFFF or 0x1DFFFF (depending if both SPU cores are used at once)
	SPU RAM Addresses have to be on a 128K boundry (-1)

REVERB TYPES AND SIZES (all smaller than 128K)

SD_REV_MODE_OFF 0x80 - 0
SD_REV_MODE_ROOM Room  0x26c0 - 1
SD_REV_MODE_STUDIO_A Studio (small)  0x1f40 - 2
SD_REV_MODE_STUDIO_B Studio (medium) 0x4840 - 3
SD_REV_MODE_STUDIO_C Studio (large)  0x6fe0 - 4
SD_REV_MODE_HALL Hall  0xade0 - 5
SD_REV_MODE_SPACE Space echo  0xf6c0 - 6
SD_REV_MODE_ECHO Echo  0x18040 - 7
SD_REV_MODE_DELAY Delay  0x18040 - 8
SD_REV_MODE_PIPE Pipe echo  0x3c00 - 9

***********************************************************************************/

void SOUND_EnableEffects(int core, int type, int depl, int depr, int delay, int feedback )
{
u_int ReverbAddress;
int b;

//int t;

	b=core+1;
	b&=1;			/* b = Other SPU core */



	if (CurrentReverb[b]==0)	/* Other reverb off */
	{ 
		CurrentReverbAddr[core]=SOUND_ReverbAddress[0];	/*0x1fffff;	// Set core to use top SPU RAM address */
	}
	else
	{
		if (CurrentReverbAddr[b]==SOUND_ReverbAddress[0])	/*0x1fffff)	// Other reverb On */
			CurrentReverbAddr[core]=SOUND_ReverbAddress[1];	/*0x1dffff;	// Use address that isn't being used. */
		else
			CurrentReverbAddr[core]=SOUND_ReverbAddress[0];	/*0x1fffff; */
	}

	CurrentReverb[core]=type;		/* Make a copy of the current reverb type */
	CurrentReverbSize[core]=ReverbSizes[type];	/* Make a copy of the size required for reverb */
	ReverbAddress=CurrentReverbAddr[core];		/* make a copy of the curent reverb SPU RAM address */

	sceSdSetAddr ((unsigned short)(core | SD_A_EEA), ReverbAddress);	/* Set Reverb end address on 128K boundry (start address set auto) */

	sceSdSetCoreAttr((unsigned short)(core | SD_C_EFFECT_ENABLE),0);		/* Disable Effects on this core.. */

	effect_attribute.mode=type;			/* Set type */
	effect_attribute.delay    = delay;
	effect_attribute.feedback = feedback;
	effect_attribute.depth_L=0;			/* Depth LEFT = 0 (same as EffectMasterVolume LEFT) */
	effect_attribute.depth_R=0;			/* Depth RIGHT= 0 (same as EffectMasterVolume RIGHT) */

	sceSdSetEffectAttr(core,&effect_attribute);	/* Initialise effect unit with these settings.. */
	sceSdClearEffectWorkArea(core,MS_REVERB_DMA,type);	/* Clear reverb buffer */


/* MS_REVERB_DMA is using the other core from the main streaming DMA channel */
/* This is so you can be streaming audio while initialising the reverb */
/* But, care must be taken NOT to call this routine if any other routine is using this DMA */
/* channel. */


	sceSdSetCoreAttr((unsigned short)(core | SD_C_EFFECT_ENABLE),1);		/* Enable Effects on this core.. */
	sceSdSetParam ((unsigned short)(core | SD_P_EVOLL), (unsigned short)depl);	/* Turn effects volume ON */
	sceSdSetParam ((unsigned short)(core | SD_P_EVOLR), (unsigned short)depr);
}

/**********************************************************************************
SOUND_DisableEffects
	Turns hardware effects off.
	Calls SOUND_EnableEffects, just to shut it down nicely for sorting out memory
	usage..
	Requires:	core		(0 or 1)
***********************************************************************************/
void SOUND_DisableEffects(int core)
{
	SOUND_EnableEffects(core,0,0,0,0,0);				/* Turn it off in a nice way.. */
	CurrentReverbSize[core]=0;							/* Set size to 0 (used for SPU RAM memory checking) */
	sceSdSetCoreAttr((unsigned short)(core | SD_C_EFFECT_ENABLE),0);		/* Disable Effects on this core.. */
}

/**********************************************************************************
SOUND_SetEffectMasterVolume
	Sets the master volume for the core n hardware effects.
	This master volume is also known as depth.
	Effect volume will effect any SPU channels on core n, which have their effects
	turned ON.

	Requires:	core	Core number 0 or 1
***********************************************************************************/
void SOUND_SetEffectMasterVolume(int core, int voll,int volr)
{
	sceSdSetParam ((unsigned short)(core | SD_P_EVOLL), (unsigned short)voll);	/* Set EffectVolume LEFT (Depth Left) */
	sceSdSetParam ((unsigned short)(core | SD_P_EVOLR), (unsigned short)volr);	/* Set EffectVolume RIGHT (Depth Right) */
}

/**********************************************************************************
SOUND_SetChannelEffectOn
	Turns effect processing for an individual channel ON.
***********************************************************************************/
void SOUND_SetChannelEffectOn(int channel)
{
int core;
int dis, oldstat;
u_int work;

#if (SCREAM_COMPATIBLE)
	if(!CHANNEL_FLAG_SET(channel,g_MSChannelUseMask)) return;
#endif

	core=SOUND_GetCore(channel);			/* SPU Core */
	channel=SOUND_GetSPUChannel(channel);	/* SPU Channel */

	g_ReverbChans[core] |= (unsigned int)(1<<channel);	/* Set SPU channel in core n effect bitmask */
	dis = CpuSuspendIntr(&oldstat);
	work = (u_int)sceSdGetSwitch((u_short)(core | SD_S_VMIXEL));
	work &= (~g_MSChannelUseMask[core]);
	work |= (g_ReverbChans[core] & g_MSChannelUseMask[core]);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXEL), work);	/* Set bitmask LEFT */
	work = (u_int)sceSdGetSwitch((u_short)(core | SD_S_VMIXER));
	work &= (~g_MSChannelUseMask[core]);
	work |= (g_ReverbChans[core] & g_MSChannelUseMask[core]);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXER), work);	/* Set bitmask RIGHT */
	if(dis == KE_OK) CpuResumeIntr(oldstat);
}

/**********************************************************************************
SOUND_SetChannelEffectOff
	Turns effect processing for an individual channel OFF.
	Requires:	channel		SPU Channel 0-47
***********************************************************************************/
void SOUND_SetChannelEffectOff(int channel)
{
int core;
int dis, oldstat;
u_int work;

#if (SCREAM_COMPATIBLE)
	if(!CHANNEL_FLAG_SET(channel,g_MSChannelUseMask)) return;
#endif
	
	core=SOUND_GetCore(channel);		/* SPU Core */
	channel=SOUND_GetSPUChannel(channel);	/* SPU Channel */

	g_ReverbChans[core] &= (unsigned int)~(1<<channel);	/* Clear SPU channel in core n effect bitmask */
	dis = CpuSuspendIntr(&oldstat);
	work = (u_int)sceSdGetSwitch((unsigned short)(core | SD_S_VMIXEL));
	work &= (~g_MSChannelUseMask[core]);
	work |= (g_ReverbChans[core] & g_MSChannelUseMask[core]);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXEL), work);	/* Set bitmask LEFT */
	work = (u_int)sceSdGetSwitch((unsigned short)(core | SD_S_VMIXER));
	work &= (~g_MSChannelUseMask[core]);
	work |= (g_ReverbChans[core] & g_MSChannelUseMask[core]);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXER), work);	/* Set bitmask RIGHT */
	if(dis == KE_OK) CpuResumeIntr(oldstat);
}

/**********************************************************************************
SOUND_AllChannelEffectsOn
	Turns effect processing for all 24 channels (core n) ON
	Requires:	core	Core 0 or 1
***********************************************************************************/
void SOUND_AllChannelEffectsOn(int core)
{
int dis, oldstat;
	
	g_ReverbChans[core]=g_MSChannelUseMask[core];		/* Set all bits (24 channels) in bitmask */

	dis = CpuSuspendIntr(&oldstat);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXEL), sceSdGetSwitch((u_short)(core | SD_S_VMIXEL)) | g_MSChannelUseMask[core]);	/* Set bitmask LEFT */
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXER), sceSdGetSwitch((u_short)(core | SD_S_VMIXER)) | g_MSChannelUseMask[core]);	/* Set bitmask RIGHT */
	if(dis == KE_OK) CpuResumeIntr(oldstat);
}

/**********************************************************************************
SOUND_AllChannelEffectsOff
	Turns effect processing for all 24 channels (core n) OFF
	Requires:	core	Core 0 or 1
***********************************************************************************/
void SOUND_AllChannelEffectsOff(int core)
{
int dis, oldstat;

	g_ReverbChans[core]=0;	/* Clear all bits (24 channels) in bitmask */
	
	dis = CpuSuspendIntr(&oldstat);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXEL), sceSdGetSwitch((u_short)(core | SD_S_VMIXEL)) & (~g_MSChannelUseMask[core]) );	/* Set bitmask LEFT */
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXER), sceSdGetSwitch((u_short)(core | SD_S_VMIXER)) & (~g_MSChannelUseMask[core]) );	/* Set bitmask RIGHT */
	if(dis == KE_OK) CpuResumeIntr(oldstat);
}

/**********************************************************************************
SOUND_ChainStreams
	Creates a list of streams to be played after the current one has finished.
	This is useful for, say, playing back commentary - where one sentance may be
	made up of several seperate words. eg "A great goal by","Player 2.","Super!"
	would be made of 3 seperate files, but be played back as one, with no gap inbetween.
	This MUST be called after the SOUND_PlayStream call.

	Requires:
			Chan		Stream Channel that stream is playing on
			Count		Amount of streams to add (max 8)
			*data		Pointer to the stream data:

			Stream data is (all u_short):

			FileID,
			FileSizeHi,
			FileSizeLo, (FileSize=(FileSizeHi<<16)+FileSizeLo)
			FileSeekHi,
			FileSeekLo (FileSeek=(FileSeekHi<<16)+FileSeekLo)

	Note:	Passing 0 for FileSize will force the chain to use the files original
			Seek and Size values (ie. the values passed when calling either
			SOUND_SetFileInfo or SOUND_CreateFileInfo)
***********************************************************************************/
void SOUND_ChainStreams(u_int Chan,int Count,u_short *data)
{
int j,id;
int FileIndex;
ADPCMFILE *File;
u_int PlaySize;
u_int FileSeek;

	if (StreamBuffer[Chan].LoopInfo==STREAM_RESIDENT)
	{
/* Not sure why not? Must have been a reason at some point..Look into this.. */
		MSERROR(("ERROR: SOUND_ChainStreams\n"));
		MSERROR(("Stream type is RESIDENT.\n"));
		MSERROR(("Can not chain streams to a RESIDENT stream\n"));
		return;
	}

	for (j=0;j<Count;j++)	/* Amount of streams to add to chain */
	{
		id=(*data++)<<16;		/* 2nd - File Size (u_int) */
	    id+=*data++;

		FileIndex=GetFileID(id);	/* Search for File ID */
		if (FileIndex==-1)			/* Does it exist? */
		{
			MSERROR(("ERROR: SOUND_ChainStreams\n"));	/* No.. */
			MSERROR(("File ID (0x%x) not found\n",id));
			StreamBuffer[Chan].ChainCount=0;
			return;
		}
		File=JADPCM;
		File+=FileIndex;	/* Offset to correct FileInfo Structure. */

		PlaySize=(unsigned int)(*data++)<<16;		/* 2nd - File Size (u_int) */
	    PlaySize+=*data++;
		FileSeek=(unsigned int)(*data++)<<16;		/* 3rd - File Seek (u_int) */
		FileSeek+=*data++;

		if (PlaySize==0)
		{
			PlaySize=File->PlaySize;		/* Use original play size (when ID was created) */
			FileSeek=File->PlayOffset;		/* Use original offset into file */
		}

		if ((PlaySize+FileSeek) > File->size)	/* Play offset in range? */
		{
			MSERROR((">>>Error: SOUND_ChainStreams\n"));	/* No.. */
			MSERROR(("PlaySize + FileSeek is greater than FileSize\n"));
			MSERROR(("PlaySize 0x%x, Seek 0x%x, FileSize 0x%x\n",PlaySize,FileSeek,File->size));
			StreamBuffer[Chan].ChainCount=0;
			return;
		}
		StreamBuffer[Chan].ChainList[j+1]=id;				/* Set file ID */
		StreamBuffer[Chan].ChainPlaySize[j+1]=PlaySize;		/* Set play size */
		StreamBuffer[Chan].ChainFileSize[j+1]=File->size;	/* Set file size */
		StreamBuffer[Chan].ChainFileSeek[j+1]=FileSeek;		/* Set play offset */
	}
	StreamBuffer[Chan].ChainCount=(unsigned char)Count;				/* Set amount of streams in chain. */
}

/**********************************************************************************
SOUND_PatchSFX
	Patches SFX into MultiStream system. After this, to play,stop,modify SFX, use
	the ID which you pass to this function.

	Requires:	ID			ID to identify the SFX for functions like SOUND_PlaySFX..
				SPUAddr		SPU address where sample data is stored
	
***********************************************************************************/
void SOUND_PatchSFX(int ID,u_int SPUAddr)	/*Hi,u_int SPUAddrLow) */
{
ADPCM_SPOTINFO *SFX;
int Slot;

	Slot=(int)SOUND_GetSpotFXSlot(ID);	/* Get offset into SFX structure */
	if (Slot<0)	/* SFX ID found? */
	{
		MSERROR((">>>>>ERROR: SOUND_PatchSFX\n"));	/* No.. */
		return;
	}

	SFX=JSPOTFX;
	SFX+=Slot;
	SFX->ID=ID;					/* Store ID in current slot */
	SFX->SPUAddress=SPUAddr;	/* Set SFX SPU Address. */
	SFX->PlayOffset=0;
	SFX->LoopFlag=SOUND_LOOP_TO_START;
}

/**********************************************************************************
SOUND_GetMaxIOPRAM (V5)
	Returns to EE the maximum free IOP RAM size available.
	Limits the amount to 2Mb (as DevTools have 8Mb IOP RAM)

	Value returned to EE (EE name: SOUND_MAX_IOP_RAM)
***********************************************************************************/
void SOUND_GetMaxIOPRAM(void)
{
u_long mem;

	mem=QueryMaxFreeMemSize();
	if (mem>(1024*2048))		/* Code running on a Dev Tool (ie. available mem size > 2Mb) ?  */
	{
		mem-=((1024*1024*6));	/* 8 megs on a Tool, only 2 on a PS2..take off 6 megs.. */
	}
	mem&=(unsigned long)(~63);					/* Stream Buffers need to be 64 multiple of 64 bytes.. */
	MaxIOPRAM=mem;				/* Value passed back to EE via SOUND_GetStatus */
}


/**********************************************************************************
SOUND_GetFileInfo (V5)
	If loading from CD, this routine gets the LSN and Size of a file.
	LSN is not required when loading from host0:
	All info is sent back to the EE.

	Requires:	count		value that is returned to the EE, so the EE knows when a
							new SOUND_FILE info packet has arrived.
				*filename	pointer to the filename

	Returns:	0			If successful
				-1			If filename not found
***********************************************************************************/
int SOUND_GetFileInfo(u_short count,u_short *filename)
{
int temp;
sceCdlFILE fp;
int source;
u_char *fname;
u_int soff=0;

	fname=(char *)filename;		/* Pointer to filename */

	SOUND_FILE_SIZE=0;			/* Initialise (incase of a bad return value) */
	SOUND_FILE_COUNT=count;		/* Thanks Dave Dootson! */

	if ((strncmp(fname, "host0:",6)==0)  ||	/* Loading from Host0 ?  */
		(strncmp(fname,  "atfile:",7)==0 ))/* Loading ATWINMON ?  */
	{
		source=ADPCM_DATA_ON_HOST;

		temp=open (fname, O_RDONLY);
		if (temp<0)		/* File found? */
		{
			MSERROR(("SOUND_GetFileInfo: Could not find file: %s\n",fname));	/* No.. */
			return(-1);
		}
		fp.lsn=0;	/* Host0 loading doesn't use LSN's. */
		fp.size=(unsigned int)lseek(temp, 0 , SEEK_END );			/* Get file size */
		close(temp);
	}
	else
	{
		if (strncmp(fname, "cdrom0:",7)==0)	/* Loading from cdrom0 ? */
		{
			soff=7;
		}
		if (strstr(fname, ";1") == NULL)	/* File ends with ;1 ? */
		{
			strcat(fname, ";1"); /* No..Add one, and try to load from CD anyway.. */
		}
		source=ADPCM_DATA_ON_CD;

		sceCdSync(0x10);	/* Wait for completion of any other CD commands..BLOCKING */
		temp=sceCdSearchFile(&fp, &fname[soff]);	/* Check for file on CD */
		if (!temp)	/* File found? */
		{
#if 1
			temp = sceCdGetDiskType();
			if ( temp == SCECdNODISC )	 // No-Disc, Tray opened
			{
				MSERROR(("SOUND_GetFileInfo: CD ERROR: TRAY OPENED\n"));
				SOUND_CD_I_ERROR = 2;
		    }
#endif
			if (soff==0)	/* No. Was cdrom0 at the start of the file name? */
			{
				MSERROR(("SOUND_GetFileInfo: Tried to load from cdrom0:\n"));
				MSERROR(("Neither cdrom0: or host0: was found at the start of filename\n"));
			}
			MSERROR(("SOUND_GetFileInfo: Could not find LSN for file: %s\n",fname));
			return(-1);
		}
	}
	SOUND_FILE_LSN=fp.lsn;		/* First sector (if loading from CD) */
	SOUND_FILE_SIZE=fp.size;	/* Size of file */
	SOUND_FILE_SOURCE=(unsigned int)source;	/* Host or CD */
/* This is used to tell the EE that this is NEW info. The EE can compare the results from */
/* SOUND_GetFileInfo/SOUND_ReturnFileInfo to make sure that the info recieved is for this file */

	return(0);
}


/**********************************************************************************
SOUND_StoreHOST0FileName (V5)
	Allocates RAM and stores name of file if loading from host0:
	Sets FileID's filename pointer to point at the allocated address in RAM.
	Only required if loading from host0:

	Requires:	id			FileID to link filename to.
				*filename	pointer to filename
***********************************************************************************/
void SOUND_StoreHOST0FileName(int id,u_short *filename)
{
u_char *fname;
ADPCMFILE *File;
int i;
u_int length;
u_int addr;
int oldstat;

	fname=(u_char *)filename;	/* Pointer to file */

	File=JADPCM;
	for (i=0;i<MAX_ADPCM_FILES;i++)
	{
		if(File->ID==id)			/* File ID found? */
		{
			if (File->FileName!=0)
			{
				MSERROR((">>>ERROR: SOUND_StoreHOST0FileName\n"));
				MSERROR(("file ID 0x%x already has a file name: %s\n",id,File->FileName));
				MSERROR(("You must first free the file name via SOUND_FreeFileID\n"));
				return;
			}

			length=strlen(fname);	/* Yes. Get length of file name */
		    CpuSuspendIntr (&oldstat);
			addr = (unsigned int) AllocSysMemory (SMEM_Low, length+1, NULL);	/* Allocate memory to store file name. */
		    CpuResumeIntr (oldstat);
			if ((void*)addr==NULL)			/* Allocation failed? */
			{
				MSERROR((">>>ERROR: SOUND_StoreHOST0FileName\n"));
				MSERROR(("file name: %s\n",fname));
				MSERROR(("Memory allocation failed (requested size 0x%x)\n",length));
				return;
			}

			File->FileName=(char*)addr;	/* Store pointer to file name memory */
			strcpy ((char*)addr,fname);	/* Copy file name to allocated memory */
			return;
		}
		File++;
	}
/* File not found */
	MSERROR((">>>ERROR: SOUND_StoreHOSTFileName: File ID (0x%x) not found\n",id));
	MSERROR(("You need to call this routine AFTER SOUND_SetFileInfo(ID..)\n"));
	MSERROR(("Note: You only need to call this routine if loading from host0\n\n"));
}

/**********************************************************************************
SearchForFileID (V5)
	Searches through allocated FileID RAM for File ID.

	Requires:	Reqid	FileID to search for
	Returns:	0		ID already exists in list
				*File	Pointer to address in FileID RAM to store a new entry
***********************************************************************************/
ADPCMFILE* SearchForFileID(int Reqid)
{
ADPCMFILE *File;
int i;

	File=JADPCM;
	for (i=0;i<MAX_ADPCM_FILES;i++)
	{
		if (File->ID==-1)	/* Found an empty slot in list? */
		{
			return(File);	/* Yes.. */
		}
		if (File->ID==Reqid)	/* ID already exists in list? */
			return(0);			/* Yes.. */
		File++;
	}
	MSERROR(("SearchForFileID...ID not found\n"));
	return(0);	/* Should never get here due to SOUND_SetFileInfo restraints */
}

/**********************************************************************************
SOUND_SetFileInfo (V5)
	Stores file info (LSN/Size and Source (host or cd)) in IOP RAM
	ID passed is the ID that this file will, from this point, be accessed with.

	Requires:	id		File ID
				lsn		File LSN (if loading from cdrom0:). Not used if loading from host0
				size	File size
				source	Either ADPCM_DATA_ON_CD or ADPCM_DATA_ON_HOST
				offset	Offset into file to start playing (48 bytes for a VAG file, to skip header...)

	Returns:	0		Successful
				-1		Failed

	Fail returned if:
	1) No spare slots free in allocated file ID memory. (See SOUND_AllocateFileMemory)
	2) File ID is already used by another file. (All files need a different ID)
***********************************************************************************/
int SOUND_SetFileInfo(int id,int lsn,u_int size,int source, u_int offset)
{
ADPCMFILE *File;


	if (MAX_ADPCM_FILES==0)	/* File buffer allocated? */
	{
		MSERROR((">>>>Error. SOUND_SetFileInfo:\n"));
		MSERROR(("No memory allocated. Call SOUND_AllocateFileMemory before this function\n"));
		return (-1);
	}
	else if (FILE_COUNTER==MAX_ADPCM_FILES)	/* Any spare slots available? */
	{
		MSERROR((">>>>Error. SOUND_SetFileInfo:\n"));
		MSERROR(("No More spare file slots available\n"));
		return (-1);
	}

	File=SearchForFileID(id);	/* Find a spare slot */

	if (File==0)	/* Valid slot? */
	{
		MSERROR((">>>>Error. SOUND_SetFileInfo:\n"));
		MSERROR(("ID (0x%x) already exists in list before a free slot could be found\n",id));
		MSERROR(("You need to call SOUND_FreeFileID(id)\n"));
		return(-1);
	}
	else if (offset>size)	/* Is offset through file to start streaming from valid? */
	{
		MSERROR((">>>>Error. SOUND_SetFileInfo:\n"));
		MSERROR(("File ID 0x%x: Play offset (0x%x) is greater than file size (0x%x)\n",id,offset,size));
		return(-1);
	}

/* Passed all error checking. Set info.. */

	File->lsn=(unsigned int)lsn;				/* LSN if loading from CD */
	File->size=size;			/* Size of file */
	File->Source=source;		/* Host0 or cdrom0 */
	File->FileName=0;			/* No filename stored in IOP RAM(yet) */
	File->offset=offset;		/* offset through file to start streaming from */
	File->PlayOffset=offset;
	File->PlaySize=size-offset;	/* Size of data to stream */
	File->ID=id;				/* ID of file */
	File->OpenFlag=0;

	FILE_COUNTER++;		/* Increase counter for number of file info's allocated.. */
	if (source==ADPCM_DATA_ON_CD)	/* Is file on CD? */
	{
		FilesOnCD=1;	/* Do this only if any files are on CD */
	}

	return(0);
}

/**********************************************************************************
SOUND_FreeFileID (V5)
	Free's requested File ID from link list.
	Stops any stream that is using this stream ID.

	Requires:	ID		ID of File
***********************************************************************************/
void SOUND_FreeFileID(int ID)
{
u_int j;
u_int i;
ADPCMFILE *File;
int mem;
int oldstat;
int fid;        // NEW!! 7.2



	if (ADPCM_FileList_Allocated==0)	/* Any File memory allocated? */
	{
		MSERROR((">>>ERROR: SOUND_FreeFileID\n"));	/* No.. */
		MSERROR(("No File memory allocated\n"));
		return;
	}

	if (FILE_COUNTER==0)	/* And files allocated? */
	{
		MSERROR((">>>ERROR: SOUND_FreeFileID\n"));	/* No.. */
		MSERROR(("No File IDs stored\n"));
		return;
	}

	File=JADPCM;
	for (i=0;i<MAX_ADPCM_FILES;i++)
	{
		if (File->ID==ID)	/* Found ID? */
		{
			for (j=0;j<MAX_NUMBER_OF_STREAMS;j++)	/* Stop any stream currently using FileID */
			{

		        if (StreamBuffer[j].iStatus==iSTATUS_INITIALISED)        //NEW!!7.2
		        	fid=StreamBuffer[j].USR_StrID;	// NEW!!
		        else	// NEW!!
		            fid=StreamBuffer[j].FileID;	// NEW!!

		        if((StreamBuffer[j].Status!=STREAM_STATUS_OFF)&&(fid==ID))        // CHANGED !! 7.2
				{
					SOUND_StopStream((int)j,0);
				}
			}
			if (File->FileName!=0)	/* Was file name stored in IOP RAM? */
			{
			    CpuSuspendIntr (&oldstat);	/* Yes.. */
				mem=FreeSysMemory( (void*)File->FileName);	/* Free Filename memory */
			    CpuResumeIntr (oldstat);
				if (mem==KE_ERROR)
				{
					MSERROR(("SOUND_FreeFileID: Specified Memory had not been allocated\n"));
				}
			}
			File->ID=-1;		/* Free ID */
			File->FileName=0;	/* Clear FileName string */
			File->size=0;		/* Clear all other FileInfo values */
			File->offset=0;
			File->PlaySize=0;
			File->PlayOffset=0;
			FILE_COUNTER--;		/* One less File Info held in IOP RAM */
			return;
		}
		File++;
	}

/* File ID not found.. */
	MSERROR((">>>ERROR: SOUND_FreeFileID\n"));
	MSERROR(("File ID (0x%x) not found. Can not free ID\n",ID));
}

/**********************************************************************************
SOUND_CreateFileInfo (V5)
	Get file LSN/SIZE from either host0 or cdrom0:
	Sets file LSN/SIZE in IOP RAM for later use.
	Allocates RAM for filename string if loading from host0: (cd loading just used LSN)

	Requires:	ID			ID of File
				*filename	pointer to the filename
				offset		offset into file to start streaming/loading (ie. 48 to skip a VAG header..)

	Notes:
		This function basically calls SOUND_GetFileInfo,SOUND_SetFileInfo and then,
		if loading from host0, SOUND_StoreHOST0FileName.
***********************************************************************************/
void SOUND_CreateFileInfo(int id, u_int offset, u_short *filename)
{
int check;

	check=SOUND_GetFileInfo((unsigned short)id,filename);	/* Get info on file */
	if (check==0)	/* Valid file? */
	{

		check=SOUND_SetFileInfo(id, (int)SOUND_FILE_LSN,SOUND_FILE_SIZE,(int)SOUND_FILE_SOURCE,offset);
/* Store filename in IOP RAM if loading from host0: */
		if ((check==0)&&(SOUND_FILE_SOURCE==ADPCM_DATA_ON_HOST))
		{

			SOUND_StoreHOST0FileName(id,filename);
		}
		else if (check!=0)
		{
			MSERROR((">>>>>ERROR: SOUND_CreateFileInfo:\n"));
			MSERROR(("SOUND_SetFileInfo failed\n"));
		}
	}
	else
	{
	 	MSERROR((">>>>>ERROR: SOUND_CreateFileInfo:\n"));
	 	MSERROR(("SOUND_GetFileInfo failed\n"));
	}
//printf("** CREATE FILE INFO Complete\n");

}

/**********************************************************************************
SOUND_FreeAllFileIDs (V5)
	Frees all file IDs in IOP RAM.
	Stops all streams that are playing (due to the fact that they can no longer load
	data due to there being no file ID's.

	Requires: Nothing
***********************************************************************************/
void SOUND_FreeAllFileIDs(void)
{
int i;
ADPCMFILE *File;
int mem;
int oldstat;

	if (ADPCM_FileList_Allocated==0)	/* Any file memory allocated? */
	{
		MSERROR((">>>ERROR: SOUND_FreeALLFileIDs\n"));	/* No.. */
		MSERROR(("No File memory allocated\n"));
		return;
	}
	if (FILE_COUNTER==0)	/* Any file info stored? */
	{
		MSERROR((">>>ERROR: SOUND_FreeALLFileIDs\n"));	/* No.. */
		MSERROR(("No Files stored\n"));
		return;
	}

	for (i=0;i<48;i++)	/* Stop all streams */
	{
		SOUND_StopStream(SPU_CH+i,0);	/* Stop ALL Streams!! (SPU channel passed as first param) */
	}

	File=JADPCM;
	for (i=0;i<MAX_ADPCM_FILES;i++)	/* Loop through all files */
	{
		if (File->FileName!=0)	/* File name stored? */
		{
		    CpuSuspendIntr (&oldstat);
			mem=FreeSysMemory( (void*)File->FileName);	/* Free filename string memory */
		    CpuResumeIntr (oldstat);
			if (mem==KE_ERROR)
			{
				MSERROR((">>>ERROR: SOUND_FreeALLFileIDs\n"));
				MSERROR(("SOUND_FileID: Specified Memory has not been allocated\n"));
				return;
			}
		}
		File->FileName=0;	/* Clear filename string */
		File->size=0;		/* Clear all other FileInfo values */
		File->ID=-1;
		File->offset=0;
		File->PlaySize=0;
		File->PlayOffset=0;
		File++;
	}
	FILE_COUNTER=0;			/* No file ID's held in RAM */
}

/**********************************************************************************
SOUND_AllocateFileMemory (V5)
	Allocates IOP RAM to be used by File ID's.
	Each File that MultiStream uses is requested by a unique ID.
	This ID then looks into this File RAM for relevent info about the file.
	This info includes the ID, LSN (if loading from cd), SIZE, SOURCE (host or cd) and
	a pointer to the filename (if loading from host).

	For example, if your ingame audio will use 30 different music streams, set this
	value to 30.
	Note that for SFX, this will need to be set to the maximum amount of SFX you are
	loading. Once they are loaded, you can free this memory (SOUND_FreeFileMemory)
	and then allocate this memory again for streamed files. Once SFX are loaded into
	SPU RAM, there is no longer any need to store their file info.

	Requires:	maxfiles	Maximum amount of files that you will use.
***********************************************************************************/
void SOUND_AllocateFileMemory(int maxfiles)
{
u_int size;
u_int addr;
ADPCMFILE *File;
int i;
int oldstat;

	if (maxfiles==0)		/* Memory for FileInfo required..? */
	{
		return;				/* No.. */
/* No need to print an error here. This might have been called from SOUND_InitStreamData */
	}

	if (ADPCM_FileList_Allocated!=0)	/* File memory already allocated? */
	{
		MSERROR((">>>ERROR: SOUND_AllocateFileMemory\n"));	/* Yes.. */
		MSERROR(("File list already allocated. Use SOUND_FreeFileMemory to free it\n"));
		return;
	}

	size=sizeof(ADPCMFILE);		/* Size of single file entry */
	size*=(unsigned int)maxfiles;				/* Size of total memory required */

    CpuSuspendIntr (&oldstat);
	addr = (unsigned int) AllocSysMemory (SMEM_Low, size, NULL);	/* Allocate RAM */
    CpuResumeIntr (oldstat);
	if ((void*)addr==NULL)	/* Allocated? */
	{
		MSERROR((">>>ERROR: SOUND_AllocateFileMemory\n"));	/* No.. */
		MSERROR(("Memory allocation failed (requested size 0x%x)\n",size));
		return;
	}

	ADPCM_FileList_Allocated=1;		/* Set Flag for file memory allocated */
	JADPCM=(ADPCMFILE*)addr;		/* Set global pointer to start of allocated memory block */

	MAX_ADPCM_FILES=(unsigned int)maxfiles;		/* Set global param for maximum files slots available */
	FILE_COUNTER=0;					/* No file info slots used yet.. */
	File=JADPCM;
	for (i=0;i<MAX_ADPCM_FILES;i++)	/* Initialise all file info slots.. */
	{
		File->ID=-1;			/* File ID */
		File->size=0;			/* Size of file */
		File->offset=0;			/* Offset into file to start streaming from */
		File->PlaySize=0;		/* Size of data to stream */
		File->PlayOffset=0;		/* Offset added to ->offset */
		File->FileName=0;		/* Filename string */
		File++;

/* Note: Offset can be used to point to a data file held within one single file. */
/* PlayOffset is then added to this. */
	}
}

/**********************************************************************************
SOUND_FreeFileMemory (V5)
	Frees file memory.
	Also calls SOUND_FreeAllFileID's, just to stop any playing streams and also to
	free any filename RAM (if loading from host0)
***********************************************************************************/
void SOUND_FreeFileMemory(void)
{
int oldstat;

	if (ADPCM_FileList_Allocated==0)	/* Any file memory allocated? */
	{
		MSERROR((">>>ERROR: SOUND_FreeFileMemory\n"));	/* No.. */
		MSERROR(("No memory allocated - so can not free anything\n"));
		return;
	}

	SOUND_FreeAllFileIDs();		/* Keeps things nice and tidy. Also stops all playing streams */
	ADPCM_FileList_Allocated=0;	/* Clear flag */
	MAX_ADPCM_FILES=0;			/* Clear global counter for maximum amount of file slots */
	FILE_COUNTER=0;				/* No files stored */
    CpuSuspendIntr (&oldstat);
	FreeSysMemory( (void*)JADPCM);	/* Free File memory */
    CpuResumeIntr (oldstat);
}

/**********************************************************************************
SOUND_AllocateSpotFXMemory (V5)
	Allocates IOP RAM for storing SpotFX info. SpotFX info includes SPU RAM address
	and SFX ID.
	For example, if you have a maximum of 80 SFX in your game at one time, set this
	to 80.

	Requires:	maxfx		Maximum amount of SFX that will be in SPU RAM at one time
***********************************************************************************/
void SOUND_AllocateSpotFXMemory(int maxfx)
{
u_int size;
u_int addr;
int oldstat;

	if (maxfx==0)
		return;

	if (SPOTFX_List_Allocated!=0)
	{
		MSERROR((">>>ERROR: SOUND_AllocateSpotFXMemory\n"));
		MSERROR(("Spot FX list already allocated. Use SOUND_FreeSpotFXMemory to free it\n"));
		return;
	}

	size=sizeof(ADPCM_SPOTINFO);		/* Size of single entry */
	size*=(unsigned int)maxfx;						/* Total size required */

    CpuSuspendIntr (&oldstat);
	addr = (unsigned int) AllocSysMemory (SMEM_Low, size, NULL);
    CpuResumeIntr (oldstat);
	if ((void*)addr==NULL)
	{
		MSERROR((">>>ERROR: SOUND_AllocateSpotFXMemory\n"));
		MSERROR(("Memory allocation failed (requested size 0x%x)\n",size));
		return;
	}

	SPOTFX_List_Allocated++;			/* Set switch to say that memory has been allocated */
	MAX_SPOT_FX=(unsigned int)maxfx;
	JSPOTFX=(ADPCM_SPOTINFO*)addr;		/* Set global pointer to start of allocated block of RAM */
	SOUND_FreeAllSpotFXIDs();			/* Initialise all SFX ID's */
}

/**********************************************************************************
SOUND_FreeSpotFXMemory (V5)
	Free spot fx memory. Nothing else..
x
***********************************************************************************/
void SOUND_FreeSpotFXMemory(void)
{
int oldstat;

	if (SPOTFX_List_Allocated==0)	/* Any SFX memory allocated? */
	{
		MSERROR((">>>ERROR: SOUND_FreeSpotFXMemory\n"));	/* No.. */
		MSERROR(("No memory allocated - so can not free anything\n"));
		return;
	}
	SOUND_FreeAllSpotFXIDs();		/*Just to keep things tidy.. */
	SPOTFX_List_Allocated=0;		/* Clear flag */
	MAX_SPOT_FX=0;					/* Clear global counter */
    CpuSuspendIntr (&oldstat);
	FreeSysMemory( (void*)JSPOTFX);	/* Free memory */
    CpuResumeIntr (oldstat);
}

/**********************************************************************************
SOUND_FreeAllSpotFXIDs (V5)
	Frees all spot FX ID's.
***********************************************************************************/
void SOUND_FreeAllSpotFXIDs(void)
{
int i;
ADPCM_SPOTINFO *SFX;

	if (SPOTFX_List_Allocated==0)		/* Any SFX memory allocated? */
	{
		MSERROR((">>>>> ERROR:SOUND_FreeAllSpotFXIDs\n"));	/* No.. */
		MSERROR(("SpotFX Memory not allocated\n"));
		return;
	}

	SFX=JSPOTFX;
	for (i=0;i<MAX_SPOT_FX;i++)
	{
		SFX->ID = -1;		/* Free all SFX ID's in list. */
		SFX++;
	}
}

/**********************************************************************************
SOUND_FreeSpotFXID (V5)
	Free single Spot FX ID from list. This ID can then be used again for another SFX

	Requires:	ID		ID to free
***********************************************************************************/
void SOUND_FreeSpotFXID(int ID)
{
int i;
ADPCM_SPOTINFO *SFX;

	if (SPOTFX_List_Allocated==0)	/* Any SFX memory allocated? */
	{
		MSERROR((">>>>> ERROR:SOUND_FreeSpotFXID\n"));	/* No.. */
		MSERROR(("SpotFX Memory not allocated\n"));
	}

	SFX=JSPOTFX;
	for (i=0;i<MAX_SPOT_FX;i++)	/* Loop through all SFX slots.. */
	{
		if (SFX->ID==ID)	/* Found requested ID? */
		{
			SFX->ID=-1;		/* Yes..Free ID */
			return;
		}
	SFX++;
	}

/* SFX ID not found */
	MSERROR((">>>>> ERROR:SOUND_FreeSpotFXID\n"));
	MSERROR(("SFX ID (0x%x) not found\n",ID));
}

/**********************************************************************************
SearchForSpotFXID (V5)
	Search through allocated spotFX RAM for ID.

	Requires:	Reqid		ID to search for
	Returns:	-1			ID already in use
				a			position through list of a free slot
***********************************************************************************/
int SearchForSpotFXID(int Reqid)
{
ADPCM_SPOTINFO *SFX;
int i;
int a=-2;

	SFX=JSPOTFX;
	for (i=0;i<MAX_SPOT_FX;i++)	/* Loop through all SFX slots. */
	{
		if (SFX->ID==Reqid)		/* ID already exists? */
		{
			MSERROR(("ID 0x%x already used\n", Reqid));	/* Yes */
			return(-1);
		}
		else if ((SFX->ID==-1)&&(a==-2))	/* First valid spare slot? */
		{
			a=i;				/* Offset of first free slot */
		}
		SFX++;
	}
	return(a);					/* Either slot or -2 (-2 = no spare slots) */
}

/**********************************************************************************
SOUND_GetSpotFXSlot (V5)
	Get a free slot in SpotFX RAM to be used by an new SFX.
	This routine is called by SOUND_PatchSFX

	Requires:	id		ID for SFX to use
	Returns:	slot	offset in SpotFX RAM to store SpotFX info (SPU Address & ID)
				-1		Failed

	Failed either because:
	1) No SpotFX RAM allocated (see SOUND_AllocateSpotFXMemory)
	2) No spare slots in allocated RAM (
***********************************************************************************/
int SOUND_GetSpotFXSlot(int id)
{
int slot;

	if (SPOTFX_List_Allocated==0)	/* SFX memory allocated? */
	{
		MSERROR((">>>>>ERROR: SOUND_GetSpotFXSlot\n"));	/* No.. */
		MSERROR(("SpotFX list not allocated\n"));
		return(-1);
	}

	slot=SearchForSpotFXID(id);		/* Find a free slot to store data in. */

	if (slot==-1)
	{
		MSERROR((">>>>>ERROR: SOUND_GetSpotFXSlot:\n"));
		MSERROR(("ID already exists in list before a free slot could be found\n"));
		MSERROR(("You need to call SOUND_FreeSpotFXID(id)\n"));
		MSERROR(("...exiting..\n"));
		return(-1);
	}
	else if (slot==-2)
	{
		MSERROR((">>>>>ERROR: SOUND_GetSpotFXSlot:\n"));
		MSERROR(("No free slots in list\n"));
		MSERROR(("Either free old SFX ID's (SOUND_FreeSpotFXID) or all SFX ID's (SOUND_FreeAllSpotFXIDs)\n"));
		return(-1);
	} 

	return(slot);			/* Return current free slot (or -1 if failed) */
}

/**********************************************************************************
SOUND_SetFileOffsetAndPlaySize (V5)
	Sets the file play position and size to play from.
	For example, if you have a single file containing 2 streams, you can set the offset
	to point at the start of the second stream, and the size to be the size of the second
	stream.

	Requires:	ID		File ID to which you want to modify its play start position / play size
				offset	Offset into file which you want to start streaming/loading data from
				size	size of data in file that you want to stream/load.

	Note: Passing 0 for size will force the default offset and size to be set.
		  The default size is the actual whole file length minus the offset originally passed in SOUND_SetFileInfo
		  The default offset is the original offset passed to SOUND_SetFileInfo

	(A bit of a long name for this one, but couldn't think of anything shorter! Sorry!)
***********************************************************************************/
void SOUND_SetFileOffsetAndPlaySize(int ID, u_int offset, u_int size)
{
ADPCMFILE *File;
int i;
u_int PlayEnd;

	File=JADPCM;
	for (i=0;i<MAX_ADPCM_FILES;i++)		/* Search through all Files.. */
	{
		if (File->ID==ID)				/* Found matching ID ? */
		{
			PlayEnd=offset+size;		/* End of playing data. */
			if (PlayEnd>File->size)		/* End of playing data past end of file? */
			{
				MSERROR((">>>>> ERROR: SOUND_SetFileOffsetAndPlaySize\n"));
				MSERROR(("offset + size will be past the end of the file\n"));
				MSERROR(("ID 0x%x. File size 0x%x, Offset 0x%x, PlaySize 0x%x, (0x%x bytes too long)\n",
					ID,File->size,offset,size,PlayEnd-File->size));
				return;
			}
			if (size==0)				/* If size = 0, set Play size and offsets to default values */
			{
/* Set size and offset to values first passed and calculated in SOUND_SetFileInfo */
				File->PlaySize=File->size-File->offset;
				File->PlayOffset=File->offset;
			}
			else
			{

//printf("%x,%x\n",File->PlayOffset,File->PlaySize);
				File->PlayOffset=offset;	/* Store Play offset */
				File->PlaySize=size;
//printf("%x,%x\n",File->PlayOffset,File->PlaySize);
			}
			return;
		}
		File++;
	}

/* File ID not found.. */
	MSERROR((">>>>> ERROR: SOUND_SetFileOffsetAndPlaySize\n"));
	MSERROR(("File ID 0x%x not found\n",ID));
}

/**********************************************************************************
SOUND_InitCD (V5)
	Initialises the CD mechanism. You may already be doing this in your code, so I
	took it out of SOUND_InitStreamData and made it a seperate function.
	This is an exact copy of Phil Rogers _CdStreamInit call from previous versions
	of MultiStream.

	Requires:	type		SOUND_CD (if using CD)
							SOUND_DVD (if using DVD)
							SOUND_SET_CALLBACK (just sets the CdCallback)
							SOUND_CD_SPIN_STREAM (force CD/DVD to spin at a set rate)
							SOUND_CD_SPIN_NORMAL (allow CD/DVD to spin fast as possible)

	SOUND_SET_CALLBACK should be used if your own code has already initialised the cd
	unit.
	SOUND_CD_SPIN_NORM can allow faster load times, but if there are any errors on
	the disk or the CD mechanism is old, you may actually get slower reads than if
	you use SOUND_CD_SPIN_STREAM.
***********************************************************************************/
void SOUND_InitCD(u_short type)
{
static u_char CDInitFlag=0;
int *a=0;

	if (type!=SOUND_SET_CALLBACK)		/* Just want to initialise the callback? */
	{
		if (type==SOUND_CD)	/* No..the CD? */
			type=SCECdCD;	/* Yes.. */
		else if (type==SOUND_DVD)	/* Or DVD? */
			type=SCECdDVD;	/* Yes.. */
		else if (type==SOUND_CD_SPIN_NORMAL)
		{
			CDInitFlag=1;
			cd_cdrmode.spindlctrl=SCECdSpinNom;
			return;
		}
		else if (type==SOUND_CD_SPIN_STREAM)
		{
			CDInitFlag=1;
			cd_cdrmode.spindlctrl=SCECdSpinStm;
			return;
		}
		else if (type==SOUND_DISPLAY_INFO)
		{
			SOUND_DisplayStats();
			return;
		}
		else
		{
			MSERROR((">>>>>ERROR: SOUND_InitCD\n"));
			MSERROR(("Type not correct (not SOUND_CD or SOUND_DVD\n"));
			MSERROR(("CD system not initialised\n"));
			return;
		}

/* Set cd info. */
		cd_cdrmode.trycount=0;
		if (CDInitFlag==0)	/* Not set by user? */
		{
			cd_cdrmode.spindlctrl=SCECdSpinNom;	/* Set to nom as default */
		}
		cd_cdrmode.datapattern=SCECdSecS2048;

		sceCdInit(SCECdINIT);	/* Initialise the CD */
		sceCdMmode(type);		/* Set the type (DVD or CD) */
	}

	if (type==SOUND_SET_CALLBACK)
	{
		if (CDInitFlag==0)
			cd_cdrmode.spindlctrl=SCECdSpinNom;	/* Set to nom as default */
		cd_cdrmode.trycount=0;
		cd_cdrmode.datapattern=SCECdSecS2048;
		sceCdSync(0x10);		/* Wait for completion of any other CD commands..BLOCKING */
	}

/* set CD callback (which will be called whenever a seek or load is completed) */
	sceCdDiskReady(0);		/* Wait for CD/DVD to be ready.. */

	while(a==0)
	{
		a=sceCdCallback((void*)_CdStreamInt);
	}
}

/**********************************************************************************
SOUND_DisableCDCallback
	Disables CD callback.
	If you are using another loading system (normally due to using mpeg streaming..)
	when not using MultiStream for loading data, you may need to disable the CD
	callback.
	To restart the callback, use: SOUND_InitCD(SOUND_SET_CALLBACK);
***********************************************************************************/
void SOUND_DisableCDCallback(void)
{
	sceCdCallback(0);
}

/**********************************************************************************
SOUND_DisableSPUCallback
	Disables SPU callback.
	If you are using another loading system (normally due to using mpeg streaming..)
	when not using MultiStream for loading data, you may need to disable to SPU
	callback.
	To restart the callback, use: SOUND_EnableSPUCallback();
***********************************************************************************/
void SOUND_DisableSPUCallback(void)
{
	sceSdVoiceTransStatus(MS_SPU_DMA,SD_TRANS_STATUS_WAIT);
    sceSdSetTransIntrHandler (MS_SPU_DMA,0,0);
}

/**********************************************************************************
SOUND_EnableSPUCallback
	Enables SPU callback.
	Call to resume SPU callbacks. This should only be called if SOUND_DisableSPUCallback
	has been called previously. This callback is also setup in the SOUND_InitStreamData
	function
***********************************************************************************/
void SOUND_EnableSPUCallback(void)
{
	sceSdVoiceTransStatus(MS_SPU_DMA,SD_TRANS_STATUS_WAIT);
    sceSdSetTransIntrHandler (MS_SPU_DMA, (sceSdTransIntrHandler) _AdpcmDmaInt,(void *) &SPU_DMA_TRANS_FLAG);
}

/**********************************************************************************
SOUND_SetOutput
	Sets SPU2 output to either PCM (as normal) or BitStream (for AC3 or DTS..) modes

	Requires: 	flag	0	PCM
						1	Bitstream

	Returns:	Nothing

***********************************************************************************/
void SOUND_SetOutput(int flag)
{
	if (flag==0)
		SOUND_EnablePCM();
	else if (flag==1)
		SOUND_EnableBitStream();
	else
	{
		MSERROR((">>>>>ERROR: SOUND_SetOutput\n"));
		MSERROR(("flag passed is neither PCM or BITSTREAM (0x%x)\n",flag));
	}
}

/**********************************************************************************
SOUND_EnableBitStream
	Sets SPU2 output to bitstream mode
	Called from SOUND_SetOutput

***********************************************************************************/
void SOUND_EnableBitStream(void)
{
    sceSdSetCoreAttr(SD_C_SPDIF_MODE, 
        SD_SPDIF_OUT_BITSTREAM | SD_SPDIF_MEDIA_CD | SD_SPDIF_COPY_NORMAL);
}

/**********************************************************************************
SOUND_EnablePCM
	Sets SPU2 output to PCM mode
	Called from SOUND_SetOutput

***********************************************************************************/
void SOUND_EnablePCM(void)
{
    sceSdSetCoreAttr (SD_C_SPDIF_MODE,
		SD_SPDIF_OUT_PCM | SD_SPDIF_MEDIA_CD | SD_SPDIF_COPY_NORMAL);
}

/**********************************************************************************
SOUND_SetStreamChild_Int (V5)
	** Plays interleaved ADPCM data, created with FPACKER PC utility **
	Flags a stream as interleaved and marks it as being a "Child".
	This stream does not require an IOP stream buffer as it transfers data from its
	"Parent" Stream. The "Parent" stream handles the loading of the interleaved stream data
	into IOP RAM. Stopping a parent stream will also stop any child streams that are attached
	to it. Stopping a child stream will still leave the parent stream (and any other attached
	child streams) running.

	Requires:	CP			"Child" stream channel AND "Parents" Stream channel (see SOUND_PlayStream)
				SPUChan		SPU Channel to use
				Track		Track to play from interleaved data
				VolL		Left Volume
				VolR		Right Volume
				SPUAddr		SPU Address for buffer to load into.
							Pass 0 to use the SPUBuffer and IOP RAM allocated with SOUND_AllocateStreamBuffer

	Notes:		If using a pre-allocation stream buffer (ie. SOUND_AllocateStreamBuffer
				has already been called	for this stream), the SPUBuffer size needs to be
				the same size as the Parents buffer size (TrackSize).
				See SOUND_SetStreamParent_Int for more information
				The CP parameter is defined as two 8bit values:
				ChildStream = CP>>8;
				ParentStream = CP&0xff;
***********************************************************************************/
void SOUND_SetStreamChild_Int(int CP, int SPUChan,int Track,int VolL,int VolR,u_int SPUAddr)
{
u_int Child;
u_int Parent;
char check;
u_int TransSize;
u_int OffSize;

	Child=(unsigned int)(CP>>8);		/* CHILD Stream */
	Parent=(unsigned int)(CP&0xff);		/* PARENT Stream */


	check=(char)SOUND_CheckStream((int)Child);
	if (check!=0)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamChild_Int\n"));
		MSERROR(("Child Stream (%d) out of range\n",Child));
		return;
	}
	check=(char)SOUND_CheckStream((int)Parent);
	if (check!=0)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamChild_Int\n"));
		MSERROR(("Parent Stream (%d) out of range\n",Parent));
		return;
	}

/* Is PARENT playing? */
	if (StreamBuffer[Parent].Status==STREAM_STATUS_OFF)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamChild_Int\n"));	/* No.. */
		MSERROR(("Parent Stream (%d) is not playing\n",Parent));
		return;
	}

/* Is PARENT stream defined as a PARENT? */
	if (StreamBuffer[Parent].ParentChildFlag!=SOUND_PARENT)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamChild_Int\n"));
		MSERROR(("Stream Channel (%d) not defined as a parent\n",Parent));
		MSERROR(("call SOUND_SetStreamParent_Int before this function\n"));
		return;
	}

/* Is PARENT stream already playing? */
/* (This routine was not called straight after SOUND_SetStreamParent_Int) */
	if (StreamBuffer[Parent].IntKeyOn!=0)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamChild_Int\n"));
		MSERROR(("Parent stream is already playing\n"));
		MSERROR(("You need to call this function straight after..\n"));
		MSERROR(("SOUND_PlayStream and SOUND_SetStreamParent_Int\n"));
		MSERROR(("Once a stream has been preloaded and keyed on..\n"));
		MSERROR(("its too late to make it a parent or child\n"));
		return;
	}

	TransSize=StreamBuffer[Parent].IntTransSize;
	OffSize=(TransSize*Track)+TransSize;

	if (StreamBuffer[Parent].AllocSize<OffSize)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamChild\n"));
		MSERROR(("Stream Buffer %d IOP buffer too small\n",Child));
		MSERROR(("IOP Buffer Size = 0x%x\n",StreamBuffer[Parent].AllocSize));
		MSERROR(("Requested MIB offset = 0x%x (Track %d)\n",TransSize*Track,Track));
		MSERROR(("Requested Track Size = 0x%x\n",TransSize));
		return;
	}

	USR_HALT_MULTISTREAM_SET();		/* Stop stream updating. */

/* Stop stream - if already active. */
	SOUND_StopStream((int)Child,0);
/* Copy LoopInfo (used for calling StartStream) */
	StreamBuffer[Child].USR_LInfo=StreamBuffer[Parent].USR_LInfo;
/* Copy File ID (used for calling StartStream) */
	StreamBuffer[Child].USR_StrID=StreamBuffer[Parent].USR_StrID;
/* Copy Address of IOP Buffer for stream */

//printf("IOPAddr = %x\n",StreamBuffer[Child].IOPAddress);
	StreamBuffer[Child].IOPAddress=StreamBuffer[Parent].IOPAddress;
//printf("IOPAddr = %x\n",StreamBuffer[Child].IOPAddress);
/* Copy Attack/Release rate */
	StreamBuffer[Child].USR_AR=StreamBuffer[Parent].USR_AR;
/* Set SPU channel to play stream on. */
	StreamBuffer[Child].USR_SPUChannel=SPUChan;

/* Set Volumes */
	SPU_VolL[SPUChan]=(short)VolL;
	SPU_VolR[SPUChan]=(short)VolR;

	if (SPUAddr==0)
	{
		if (StreamBuffer[Child].IOPAllocAddress==0)
		{
			MSERROR((">>>>>ERROR: SOUND_SetStreamChild_Int\n"));
			MSERROR(("Can not use Allocated SPU RAM address as a buffer\n"));
			USR_HALT_MULTISTREAM_CLR();
			return;
		}
/* Stream already allocated. Use this SPU address */
	 	StreamBuffer[Child].SPUAddress=StreamBuffer[Child].AllocSPUAddress;
	}
	else
	{
		StreamBuffer[Child].SPUAddress=SPUAddr;		/* Use given SPU RAM address */
	}

	StreamBuffer[Child].AllocSize = StreamBuffer[Parent].AllocSize;	/* Size of stream buffer */
	StreamBuffer[Child].AllocHalfSize = StreamBuffer[Parent].AllocHalfSize;	/* Half size.. */

	StreamBuffer[Child].MIBEnd=0;	/* V7.2 - From newsgroup posting (Thanks Tony Bennett) */
	StreamBuffer[Child].MIBLoop=0;	/* V7.2 - From newsgroup posting (Thanks Tony Bennett) */

	StreamBuffer[Child].Pitch=StreamBuffer[Parent].Pitch;	/* Copy pitch */
	SPU_PausePitch[SPUChan]=(unsigned short)StreamBuffer[Parent].Pitch;

	StreamBuffer[Child].iStatus=iSTATUS_INITIALISED;	/* Set iStatus */
	StreamBuffer[Child].ResidentFlag=STREAM_AUDIO_ADPCM;	/* Must be ADPCM data */
	StreamBuffer[Child].ParentChildFlag=SOUND_CHILD;	/* Mark as a CHILD */
	StreamBuffer[Child].TransCounter=0;		/* Amount of Transfers from IOP->? completed */
	StreamBuffer[Child].ParentStream=(int)Parent;	/* Set CHILD's PARENT stream */
	StreamBuffer[Child].IntTransOffset=Track*StreamBuffer[Parent].IntTransSize;
	StreamBuffer[Child].IntTransSize=StreamBuffer[Parent].IntTransSize;
	StreamBuffer[Child].IOPBufferHalfSize=StreamBuffer[Parent].IntTransSize;
	StreamBuffer[Child].KeyOffFlag=StreamBuffer[Parent].KeyOffFlag;
	StreamBuffer[Child].SPUBufferSize=StreamBuffer[Parent].SPUBufferSize;

	StreamBuffer[Child].Priority=StreamBuffer[Parent].Priority;
	StreamBuffer[Child].Priority2=0xffffffff;


	SPU_KeyStatus[SPUChan]=SPU_KEY_STATUS_USED_BY_STREAM;	/* Set SPU key status */
	SOUND_PauseStreamCheck[SPUChan]=0;
	StreamBuffer[Child].Status=STREAM_STATUS_ON;	/* DO THIS RIGHT AT THE END!! */
	USR_HALT_MULTISTREAM_CLR();	/* Allow stream updating to continue */
}

/**********************************************************************************
SOUND_PerformKeyOn (V5)
	Internal call. Not called from EE code!
	Keys on a stream.
	Also, if a stream is either a parent or child, it checks to make sure that BOTH
	parent and its multiple child streams are ready to be keyed on. They are then
	all keyed on together, so everything is synced. This is required for playing
	stereo data from interleaved files.
	Otherwise, you would have a case where:
	1) Parent loaded
	2) KeyOn parent
	3) Child loaded
	4) KeyOn child

	So, obviously, a delay between the two keyons would happen.

	Requires:	i	Stream number
***********************************************************************************/
int SOUND_PerformKeyOn(int i)
{
int a,b,c,j;
int core0=0;
int core1=0;
int parent;
int core,ch;
int ch2;
int core2=-1;
int skon,skon2;

	core=0;
	skon=0;
	skon2=0;

	parent=StreamBuffer[i].ParentStream;	/* Get streams PARENT (if loading MIB files) */

	if (parent!=-1)
	{
/* If playing interleaved data (MIB files). Wait for PARENT and CHILD streams to be ready */
/* before keying on.. */

		if (StreamBuffer[parent].IntKeyOn==1)	/* Keyed on already? */
			return(0);	/* Already keyed on all relevent SPU channels */

		for (a=0;a<UserMaxStreams;a++)	/* Loop through all streams */
		{
/* PARENT stream, or a CHILD stream attached to the PARENT? */
			if (StreamBuffer[a].ParentStream==StreamBuffer[i].ParentStream)
			{
/* Are CHILD streams at the same state as the PARENT stream? */
				if (StreamBuffer[a].TransCounter!=StreamBuffer[i].TransCounter)
				{
					return(-1);	/* Either parent or child NOT ready to be keyed on yet.. */
				}
				if (StreamBuffer[i].TransCounter==0)	/* Any transfers happened yet? */
				{
					return(-1);	/* No.. */
				}

/* Build a list of bitpatterns to key on for both PARENT and CHILD streams */
				if (StreamBuffer[a].SPUChannel>23)
					core1|=1<<(StreamBuffer[a].SPUChannel-24);
				else
					core0|=1<<(StreamBuffer[a].SPUChannel);

				if (StreamBuffer[a].Ch2Flag!=0)		/* Using a 2nd SPU channel? */
				{
					for (j=0;j<StreamBuffer[a].Ch2Flag;j++)
					{
						ch=StreamBuffer[a].SPUChannel2[j];	/* Yes.. */
						if (ch>23)
							core1|=1<<(ch-24);	/* Build a list of bitpatterns to key on */
						else
							core0|=1<<ch;
					}
				}
			}
		}

/* **** Both parent and child streams ready to be keyed on. ***** */

/* Mark the parent stream as keyed on (so KON is only performed once for all required channels) */
		StreamBuffer[parent].IntKeyOn=1;

/* Copy KeyOn Bitpatterns. We may not want to key on straight away, SOUND_StreamDisableKeyOn */
/* may have been called. */
		StreamBuffer[parent].core0=(unsigned int)core0;
		StreamBuffer[parent].core1=(unsigned int)core1;

		if (StreamBuffer[parent].AllowKON==1)	/* SOUND_StreamDisableKeyOn called? */
		{
			b=SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel];	/* Yes. */
			for (a=0;a<UserMaxStreams;a++)
			{
/* Found a PARENT or CHILD stream? */
				if (StreamBuffer[a].ParentStream==StreamBuffer[i].ParentStream)
				{
/* Init SPU channel info, point SPU playback address at 0x5000. */
/* Nothing will play until KeyOn anyway, but keeps things tidy. */
					c=SPU_PausePitch[StreamBuffer[i].SPUChannel];
					if (b==0)
						StreamBuffer[a].Pitch=(short)c;
					SetVoiceData(StreamBuffer[a].SPUChannel,StreamBuffer[a].Pitch,0,0,0x5000,0,1);
					if (StreamBuffer[a].Ch2Flag!=0)	/* 2nd SPU channel playing? */
					{
						for (j=0;j<StreamBuffer[a].Ch2Flag;j++)
						{
							SetVoiceData(StreamBuffer[a].SPUChannel2[j],StreamBuffer[a].Pitch,0,0,0x5000,0,1);
						}
					}
						
/* Check if stream is paused (but it must have been paused during preload) */
/* If so, its just been keyed on with a pitch of 0 and this value would have been copied into SPU_PausePitch */
/* when SetVoiceData was called. Therefore, we need to replace this value with the real pause pitch */
					if (b!=0)
					{
						SPU_PausePitch[StreamBuffer[a].SPUChannel]=(unsigned short)c;	/* Reset pause pitch */
						if (StreamBuffer[a].Ch2Flag!=0)
						{
							for (j=0;j<StreamBuffer[a].Ch2Flag;j++)
							{
								SPU_PausePitch[StreamBuffer[a].SPUChannel2[j]]=(unsigned short)c;	/* For 2nd channel also */
							}
						}
					}
				}
			}
		}
		else
		{
/* PARENT and CHILD streams can be keyed on right now.. */

			b=SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel];

  			c=SPU_PausePitch[StreamBuffer[i].SPUChannel];

			for (a=0;a<UserMaxStreams;a++)
			{
/* Found a PARENT or CHILD stream? */
				if (StreamBuffer[a].ParentStream==StreamBuffer[i].ParentStream)
				{
					if (StreamBuffer[i].KeyOffFlag==0)	/* SOUND_PlayStream called? */
					{
/* Init SPU channel info */
						if (b==0)
						{
							StreamBuffer[i].Pitch=(short)c;
						}
						SetVoiceData(StreamBuffer[a].SPUChannel,StreamBuffer[i].Pitch,
						StreamBuffer[a].VolL,StreamBuffer[a].VolR,
						(int)StreamBuffer[a].SPUAddress,StreamBuffer[a].AR,0);
						SPU_KeyStatus[StreamBuffer[a].SPUChannel]=SPU_KEY_STATUS_JUST_KEYED_ON;

						if (StreamBuffer[a].Ch2Flag!=0)	/* Using 2nd SPU channel? */
						{
							for (j=0;j<StreamBuffer[a].Ch2Flag;j++)
							{
								SetVoiceData(StreamBuffer[a].SPUChannel2[j],StreamBuffer[i].Pitch,
								StreamBuffer[a].VolL2[j],StreamBuffer[a].VolR2[j],
								(int)StreamBuffer[a].SPUAddress,StreamBuffer[a].AR,0);
								SPU_KeyStatus[StreamBuffer[a].SPUChannel2[j]]=SPU_KEY_STATUS_JUST_KEYED_ON;
							}
						}
					}
					else	/* SOUND_PlayStreamFromSFX called.. */
					{
						core=SOUND_GetCore(StreamBuffer[a].SPUChannel);			// MS7.21 was i
						ch=SOUND_GetSPUChannel(StreamBuffer[a].SPUChannel);		// MS7.21 was i
/* Set LSAX to point at SPU stream buffer. When playback of the looping SFX finishes, the stream */
/* will start playing instead. */
						sceSdSetAddr ((unsigned short)(core | ch<<1 | SD_VA_LSAX), StreamBuffer[a].SPUAddress);

/* Do the same for SPUChannel2, if required.. */
						if (StreamBuffer[i].Ch2Flag!=0)
						{
							for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
							{
								core=SOUND_GetCore(StreamBuffer[i].SPUChannel2[j]);
								ch=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[j]);
								sceSdSetAddr ((unsigned short)(core | ch<<1 | SD_VA_LSAX), StreamBuffer[a].SPUAddress);
							}
						}
					}
/* Check if stream is paused (but it must have been paused during preload) */
/* If so, its just been keyed on with a pitch of 0 and this value would have been copied into SPU_PausePitch */
/* when SetVoiceData was called. Therefore, we need to replace this value with the real pause pitch */
					if (b!=0)
					{
						SPU_PausePitch[StreamBuffer[a].SPUChannel]=(unsigned short)c;	/* Reset pause pitch */
						if (StreamBuffer[a].Ch2Flag!=0)
						{
							for (j=0;j<StreamBuffer[a].Ch2Flag;j++)
							{
								SPU_PausePitch[StreamBuffer[a].SPUChannel2[j]]=(unsigned short)c;	/* For 2nd channel also */
							}
						}
					}
				}
			}
		}

/* Allow key ON of all required PARENT and CHILD streams (SOUND_StreamDisableKeyOn not called..) */
		if (StreamBuffer[i].KeyOffFlag==0)	/* Allow key on ? */
		{
			GET_SPU_RIGHTS();
			if (core0!=0)	/* Yes. Anything to key on core 0? */
				sceSdSetSwitch (SD_CORE_0 | SD_S_KON, (unsigned int)core0);	/* Yes.. */
			if (core1!=0)	/* Anything to key on core 1? */
				sceSdSetSwitch (SD_CORE_1 | SD_S_KON, (unsigned int)core1);	/* Yes.. */
			SOUND_DelayKeyOnOff();	/* Wait for KeyON to start.. */
			RELEASE_SPU_RIGHTS();
		}

	}
	else
	{
/* **** STREAM IS NOT A PARENT OR A CHILD (Normal ADPCM or PCM stream) **** */

		StreamBuffer[i].IntKeyOn=1;		/* Mark as already keyed on. */

		if(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)	/* Streaming ADPCM? */
		{
			core=SOUND_GetCore(StreamBuffer[i].SPUChannel);		/* Get SPU core */
			ch=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel);	/* Get SPU channel */
			skon=1<<ch;	/* Convert channel to SPU KeyON bitmask */

			if (StreamBuffer[i].Ch2Flag!=0)	/* Using 2nd SPU channel? */
			{
				skon2=0;
				for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
				{
					ch2=StreamBuffer[i].SPUChannel2[j];	/* Yes.. */
					core2=SOUND_GetCore(ch2);
					ch2=SOUND_GetSPUChannel(ch2);
					if (core2==core)	/* 2nd channel playing on the same core as 1st? */
						skon|=(1<<ch2);	/* Yes..Mix bits */
					else
						skon2|=(1<<ch2);
				}
				core2=core+1;
				core2&=1;
			}
		}

		if (StreamBuffer[i].AllowKON==1)	/* SOUND_StreamDisableKeyOn called? */
		{
			if(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)	/* Streaming ADPCM? */
			{
/* Init SPU channel info, point SPU playback address at 0x5000. Will loop from 0x5000->0x500F */
/* Need to do this so that SPU channel is read as active, even though playback hasn't started */
/* until SOUND_StreamAllowKeyOn is called. */
				c=SPU_PausePitch[StreamBuffer[i].SPUChannel];	/* Pitch for SPU channel if in pause mode.. */
				SetVoiceData(StreamBuffer[i].SPUChannel,c,0,0,0x5000,0,1);
				if (StreamBuffer[i].Ch2Flag!=0)	/* 2nd SPU channel used? */
				{
					for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
					{
				 		SetVoiceData(StreamBuffer[i].SPUChannel2[j],c,0,0,0x5000,0,1);	/* Yes.. */
					}
				}
/* Key ON SPU channel(s) */
				GET_SPU_RIGHTS();
				sceSdSetSwitch ((unsigned short)(core | SD_S_KON), (unsigned int)skon);	
				if (skon2!=0)
					sceSdSetSwitch ((unsigned short)(core2 | SD_S_KON), (unsigned int)skon2);
				SOUND_DelayKeyOnOff();	/* Wait for KeyON to start */
				RELEASE_SPU_RIGHTS();
			}
/* No need to check for PCM in this mode. Just check the current stream status. When its */
/* ACTIVE (ie. preloaded), you can just call SOUND_StartPCM for it to play.. */

/* EXIT routine if SOUND_StreamDisableKeyOn is called */
			return(0);
		}

/* SOUND_StreamDisableKeyOn NOT called.. */
		if(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)	/* Streaming PCM? */
		{
			if (PCMPause==0)	/* Are we in Pause mode? */
			{
				SOUND_StartPCM();	/* No..So start PCM playback */
			}
			return(0);
		}

		if(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)	/* Streaming ADPCM? */
		{
			c=SPU_PausePitch[StreamBuffer[i].SPUChannel];	/* Pitch for SPU channel if in pause mode.. */
			if (StreamBuffer[i].KeyOffFlag==0)	/* SOUND_PlayStreamFromSFX called? */
			{
				if (SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel]==0)
					StreamBuffer[i].Pitch=(short)c;

				SetVoiceData(StreamBuffer[i].SPUChannel,StreamBuffer[i].Pitch,
							StreamBuffer[i].VolL,StreamBuffer[i].VolR,
							(int)StreamBuffer[i].SPUAddress,StreamBuffer[i].AR,0);
				if (StreamBuffer[i].Ch2Flag!=0)	/* 2nd SPU channel used? */
				{
					for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
					{
						SetVoiceData(StreamBuffer[i].SPUChannel2[j],StreamBuffer[i].Pitch,
								StreamBuffer[i].VolL2[j],StreamBuffer[i].VolR2[j],
								(int)StreamBuffer[i].SPUAddress,StreamBuffer[i].AR,0);
						SPU_KeyStatus[StreamBuffer[i].SPUChannel2[j]]=SPU_KEY_STATUS_JUST_KEYED_ON;
					}
				}

//printf("KEY ON\n");
				GET_SPU_RIGHTS();
				sceSdSetSwitch ((unsigned short)(core | SD_S_KON), (unsigned int)skon);	/* KeyOn SPU channel */
				if (core2!=-1)
					sceSdSetSwitch ((unsigned short)(core2 | SD_S_KON), (unsigned int)(skon2));	/* KeyOn 2nd SPU channel */
				SPU_KeyStatus[StreamBuffer[i].SPUChannel]=SPU_KEY_STATUS_JUST_KEYED_ON;
				SOUND_DelayKeyOnOff();	/* Wait for keyOn to start */
				RELEASE_SPU_RIGHTS();
			}
			else	/* SOUND_PlayStreamFromSFX has been called.. */
			{
				core=SOUND_GetCore(StreamBuffer[i].SPUChannel);
				ch=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel);

/* Set LSAX to point at SPU stream buffer. When playback of the looping SFX finishes, the stream */
/* will start playing instead. */
				sceSdSetAddr ((unsigned short)(core | ch<<1 | SD_VA_LSAX), StreamBuffer[i].SPUAddress);
				if (StreamBuffer[i].Ch2Flag!=0)	/* 2nd SPU channel used? */
				{
					for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
					{
						core=SOUND_GetCore(StreamBuffer[i].SPUChannel2[j]);	/* Yes.. */
						ch=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[j]);
						sceSdSetAddr ((unsigned short)(core | ch<<1 | SD_VA_LSAX), StreamBuffer[i].SPUAddress);
					}
				}
			}

/* Check if stream is paused (but it must have been paused during preload) */
/* If so, its just been keyed on with a pitch of 0 and this value would have been copied into SPU_PausePitch */
/* when SetVoiceData was called. Therefore, we need to replace this value with the real pause pitch */
			if (SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel]!=0)
				SPU_PausePitch[StreamBuffer[i].SPUChannel]=(unsigned short)c;
		}
	}
	return(0);
}

/**********************************************************************************
SOUND_StopStream (V5)
	Stops a stream from playing, instantly (not the same as a keyoff, which may have
	a release rate to ramp down volume before being silent).
	Also handles the stopping of a "Parent" stream. If you stop a parent stream, you
	also have to stop all of its "Child" streams, as the "Parent" handles the loading
	of the data from cd or host.

	Requires:		i				Stream channel which stream is playing on
				or:	SPU_CH+i		(where i = SPU channel)
***********************************************************************************/
void SOUND_StopStream(int i,int KeyOffFlag)
{
int a;
int ch=-1;
int spuch;
int t;

	if (i>=SPU_CH)	/* Passed a SPU channel, rather than a stream channel? */
	{
		spuch=i&(SPU_CH-1);		/* Stopping Stream on specified SPU channel */
		for (a=0;a<UserMaxStreams;a++)			/* Stop stream that is playing on SPU Channel n */
		{
			if ((StreamBuffer[a].Status==STREAM_STATUS_ON)&&(ch==-1))
			{

				t=SOUND_GetStreamSPUChan(a);
				if ((t==spuch)&&(StreamBuffer[a].ResidentFlag==STREAM_AUDIO_ADPCM))
				{
					ch=a;
					break;
				}
			}
		}
	}
	else
	{
		ch=i;	/* Stream channel.. */
	}

	if (ch<0)		/* No streams using requested SPU channel */
		return;

	if (StreamBuffer[ch].ParentStream==ch)	/* Is this is a "parent" using interleaved data? */
	{
		for (a=0;a<UserMaxStreams;a++)		/* Yes..So stop BOTH parent and its children. (as parent handles CD loading) */
		{
			if (ch==StreamBuffer[a].ParentStream)	/* PARENT or CHILD for a stream? */
			{
				StopStream(a,KeyOffFlag);	/* Yes. Stop it. */
			}
		}
	}
	else
	{

		StopStream(ch,KeyOffFlag);	/* Just a single stream, or a CHILD stream */
	}
}

/**********************************************************************************
SOUND_GetCore
	Returns a core value depending on requested channel (0-47)
	(note: could just return Channel/24)

	Requires:	Channel		0-47
	Returns:	Core		SD_CORE_0 or SD_CORE_1
***********************************************************************************/
int SOUND_GetCore(int Channel)
{
	return (Channel/24);
}

/**********************************************************************************
SOUND_GetSPUChannel
	Returns a SPU channel value depending on requested channel (0-47)
	(note: could just return Channel%24)

	Requires:	Channel		0-47
	Returns:	SPUChannel	0-23
***********************************************************************************/
int SOUND_GetSPUChannel(int Channel)
{
	return(Channel%24);
}

/**********************************************************************************
SOUND_CheckSPUMemory
	Checks to see if a write to SPU RAM (or an allocated SPU RAM buffer) is valid.

	Requires:	Address		SPU RAM address
				Size		Size of data block (either to be loaded or allocated)
	Returns:	0			Valid
				-1			Invalid
***********************************************************************************/
int SOUND_CheckSPUMemory(u_int Address,u_int Size)
{
u_int end;
u_int revaddr;
char error=0;
int i;

	revaddr=0;
	i=0;

	end=Address+Size;	/* End SPU RAM address required. */

	if (end>SPU_MEMORY_END)		/* End address for sample past end of SPU RAM? (7.21 - was >= )*/
		error=2;	/* Yes.. */
	else if (Address < SPU_BASE_ADDRESS)
		error=3;
	else
	{
		for (i=0;i<2;i++)
		{
			if (error==0)
			{
				if (CurrentReverbSize[i]!=0)	/* Are hardware effects turned ON for this core? */
				{
					revaddr=(CurrentReverbAddr[i]-CurrentReverbSize[i])+1;	/* Start address of hardware effects on core n */
					if ((Address<=revaddr)&&(end>revaddr))		/* end address either in middle or after reverb buffer? */
						error=1;
					else if ((Address>=revaddr)&&(Address<=CurrentReverbAddr[i]))	/* Start address in middle of reverb buffer? */
						error=1;
					if ((Address<=revaddr)&&(end>CurrentReverbAddr[i]))	/* Start is before reverb buffer, end is after..? */
						error=1;
				}
			}
		}
	}

	if (error!=0)
	{
		MSERROR((">>>>>ERROR: SOUND_CheckSPUMemory\n"));
		if (error==1)
		{
			MSERROR(("Requested address (0x%x) + size (0x%x) is used for core:%d reverb area\n",Address,Size,i));
			MSERROR(("Core:%d reverb area is from 0x%x to 0x%x\n",i,revaddr,CurrentReverbAddr[i]));
		}
		else if (error==2)
		{
			MSERROR(("Requested address (0x%x) + size (0x%x) > SPU RAM size (0x1FFFFF)",Address,Size));
			MSERROR(("(End address would have been 0x%x\n",end));
		}
		else if (error==3)
		{
		   MSERROR(("Requested address (0x%x) < SPU base address(0x5010)\n",Address));
		}
		return(-1);	/* Invalid address and/or size */
	}
	return(0);		/* Valid address and/or size */
}

/**********************************************************************************
SOUND_CheckStream
	Checks to see if a stream is valid.
	It is not valid if less than the amount of streams you allocated with
	SOUND_SetMaxStreamLimit

	Requires:	stream		Stream number (0-47)
	Returns:	0			Valid
				-1			Invalid
***********************************************************************************/
int SOUND_CheckStream(int stream)
{
	if (stream>=UserMaxStreams)
	{
	 	MSERROR(("ERROR: SOUND_CheckStream\n"));
		if (UserMaxStreams==0)
		{
			MSERROR(("No available streams\n"));
		}
		else
		{
			MSERROR(("Stream needs to range from 0-%d\n",UserMaxStreams-1));
		}
		MSERROR(("Use function: SOUND_SetMaxStreamLimit to allow more streams\n"));
		return(-1);
	}
	return(0);
}

/**********************************************************************************
SOUND_StreamDissableKeyOn
	If called before preloading of a stream is completed (and after the initial
	SOUND_PlayStream), no keyon is performed by a stream (or both parent and children)
	Use the command SOUND_StreamAllowKeyOn to perform the key on.
	With these functions, you can then sync sound to graphics by being able to have
	full control over exactly when a stream starts playing.
	On the EE, check the SOUND_STREAM_ACTIVE[stream] to see if data is preloaded.
	If it returns 0, then data is STILL being preloaded. Otherwise, when ever you
	call SOUND_StreamAllowKeyOn, the stream will play straight away, as preloading
	has already taken place.
	Every time you play a stream with SOUND_PlayStream, the stream is set in the
	SOUND_StreamAllowKeyOn mode as default

	Requires:	i		Stream number
***********************************************************************************/
void SOUND_StreamDisableKeyOn(u_int i)
{
int parent;
int check;

	check=SOUND_CheckStream((int)i);
	if (check!=0)
	{
		MSERROR((">>>>>ERROR: SOUND_StreamDissableKeyOn\n"));
		MSERROR(("Stream (%d) out of range\n",i));
		return;
	}

	if (StreamBuffer[i].Status==STREAM_STATUS_OFF)
	{
		MSERROR((">>>>>ERROR: SOUND_StreamDissableKeyOn\n"));
		MSERROR(("Stream (%d) not playing\n",i));
		MSERROR(("Call SOUND_PlayStream before this function\n"));
		return;
	}
	if (StreamBuffer[i].iStatus!=iSTATUS_INITIALISED)
	{
		MSERROR((">>>>>ERROR: SOUND_StreamDissableKeyOn\n"));
		MSERROR(("Stream (%d) already playing. Too late to disable key on\n",i));
		return;
	}
	parent=StreamBuffer[i].ParentStream;	/* Get streams PARENT if it has one.. */
	if (parent!=-1)
		i=(unsigned int)parent;
	StreamBuffer[i].AllowKON=1;	/* Either set single stream or PARENT stream flag */
}

/**********************************************************************************
SOUND_StreamAllowKeyOn
	Use the command SOUND_StreamAllowKeyOn to perform the key on of a stream (or
	streams) that had SOUND_StreamDissableKeyOn called before it.
	With these functions, you can then sync sound to graphics by being able to have
	full control over exactly when a stream starts playing.
	On the EE, check the SOUND_STREAM_ACTIVE[stream] to see if data is preloaded.
	If it returns 0, then data is STILL being preloaded. Otherwise, when ever you
	call SOUND_StreamAllowKeyOn, the stream will play straight away, as preloading
	has already taken place.
	Every time you play a stream with SOUND_PlayStream, the stream is set in the
	SOUND_StreamAllowKeyOn mode as default

	Requires:	i		Stream number
***********************************************************************************/
void SOUND_StreamAllowKeyOn(u_int i)
{
u_int parent;
u_int core0,core1,core;
u_int ch;
int a,j;
int core2,ch2;

	parent=(unsigned int)StreamBuffer[i].ParentStream;	/* Get streams PARENT if it has one. */

	if (parent!=-1)		/* Is this stream part of an interleaved stream (either PARENT or CHILD)? */
	{
/* Either this function already called, or SOUND_StreamDissableKeyOn not called */
		if (StreamBuffer[parent].AllowKON==0)
			return;
		StreamBuffer[parent].AllowKON=0;		/* Clear check flag */
		if (StreamBuffer[parent].IntKeyOn==1)	/* Has SOUND_PerformKeyOn already been done? */
		{
			for (a=0;a<UserMaxStreams;a++)	/* Yes..Loop through all streams. */
			{
/* Found a PARENT or CHILD for stream? */
				if (StreamBuffer[a].ParentStream==StreamBuffer[i].ParentStream)
				{

/* Was stream paused? If so, resume stream.. */
					if (SOUND_PauseStreamCheck[StreamBuffer[a].SPUChannel]==1)
					{
						SOUND_PauseStreamCheck[StreamBuffer[a].SPUChannel]=0;
						if (StreamBuffer[i].Ch2Flag!=0)
						{
							for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
							{
								SOUND_PauseStreamCheck[StreamBuffer[a].SPUChannel2[j]]=0;
							}
						}
					}
					StreamBuffer[a].Pitch=(short)SPU_PausePitch[StreamBuffer[a].SPUChannel];

/* Set SPU channel data (setting correct SPU RAM address, rather than 0x5000 now..) */

					SetVoiceData(StreamBuffer[a].SPUChannel,StreamBuffer[a].Pitch,	/* Set SPU info for all channels */
					StreamBuffer[a].VolL,StreamBuffer[a].VolR,
					(int)StreamBuffer[a].SPUAddress,StreamBuffer[a].AR,0);
					SPU_KeyStatus[StreamBuffer[a].SPUChannel]=SPU_KEY_STATUS_JUST_KEYED_ON;
					if (StreamBuffer[i].Ch2Flag!=0)	/* Using 2nd SPU channel? */
					{
						for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
						{
							SetVoiceData(StreamBuffer[a].SPUChannel2[j],StreamBuffer[a].Pitch,	/* Set SPU info for all channels */
							StreamBuffer[a].VolL2[j],StreamBuffer[a].VolR2[j],
							(int)StreamBuffer[a].SPUAddress,StreamBuffer[a].AR,0);
							SPU_KeyStatus[StreamBuffer[a].SPUChannel2[j]]=SPU_KEY_STATUS_JUST_KEYED_ON;
						}
					}
				}
			}

			if (StreamBuffer[parent].KeyOffFlag==0)	/* If SOUND_PlayStreamFromSFX NOT called.. */
			{
				core0=StreamBuffer[parent].core0;
				core1=StreamBuffer[parent].core1;
				GET_SPU_RIGHTS();
				if (core0!=0)		/* Anything to keyON core 0? */
					sceSdSetSwitch (SD_CORE_0 | SD_S_KON, core0);	/* Yes. */
				if (core1!=0)		/* Anything to keyON core 1? */
					sceSdSetSwitch (SD_CORE_1 | SD_S_KON, core1);	/* Yes. */
				SOUND_DelayKeyOnOff();	/* Wait for KeyON to start. */
				RELEASE_SPU_RIGHTS();
			}

		}
	}
	else
	{
/* Stream is NOT a PARENT or CHILD. (Single ADPCM or PCM stream) */

		if (StreamBuffer[i].AllowKON==0)	/* Already started? */
			return;	/* Yes.. */
		StreamBuffer[i].AllowKON=0;				/* Clear check flag */

		if (StreamBuffer[i].IntKeyOn==0)	/* Has SOUND_PerformKeyOn already been done? */
			return;

		if(StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)	/* Playing PCM? */
		{
			SOUND_StartPCM();	/* Start it.. */
			return;
		}

/* Was stream paused? If so, resume stream.. */
		if (SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel]==1)
		{
			SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel]=0;
			if (StreamBuffer[i].Ch2Flag!=0)
			{
				for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
				{
					SOUND_PauseStreamCheck[StreamBuffer[i].SPUChannel2[j]]=0;
				}
			}
		}
			StreamBuffer[i].Pitch=(short)SPU_PausePitch[StreamBuffer[i].SPUChannel];

/* Set SPU Voice Data.. */
		SetVoiceData(StreamBuffer[i].SPUChannel,StreamBuffer[i].Pitch,
					StreamBuffer[i].VolL,StreamBuffer[i].VolR,
					(int)StreamBuffer[i].SPUAddress,StreamBuffer[i].AR,0);

		ch=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel);
		core=(unsigned int)SOUND_GetCore(StreamBuffer[i].SPUChannel);
		ch=(unsigned int)(1<<ch);

		if (StreamBuffer[i].Ch2Flag!=0)	/* Using 2nd SPU channel? */
		{
			for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
			{
				SetVoiceData(StreamBuffer[i].SPUChannel2[j],StreamBuffer[i].Pitch,
							StreamBuffer[i].VolL2[j],StreamBuffer[i].VolR2[j],
							(int)StreamBuffer[i].SPUAddress,StreamBuffer[i].AR,0);
				ch2=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[j]);
				ch2=1<<ch2;
				core2=SOUND_GetCore(StreamBuffer[i].SPUChannel2[j]);
				if (core2==core)
					ch|=(unsigned int)ch2;
				else
				{
					GET_SPU_RIGHTS();
					sceSdSetSwitch ((unsigned short)(core2 | SD_S_KON), (unsigned int)ch2);	/* KeyOn */
					SOUND_DelayKeyOnOff();	/*Wait for KeyOn to start.. */
					RELEASE_SPU_RIGHTS();
				}
				SPU_KeyStatus[StreamBuffer[i].SPUChannel2[j]]=SPU_KEY_STATUS_JUST_KEYED_ON;
			}
		}

		GET_SPU_RIGHTS();
		sceSdSetSwitch ((unsigned short)(core | SD_S_KON), ch);	/* KeyOn */
		SPU_KeyStatus[StreamBuffer[i].SPUChannel]=SPU_KEY_STATUS_JUST_KEYED_ON;
		SOUND_DelayKeyOnOff();	/*Wait for KeyOn to start.. */
		RELEASE_SPU_RIGHTS();
	}
}


/**********************************************************************************
SOUND_SetStreamTrack_Int
	Set which track from an interleaved data file you want the requested stream to
	play. The orignal track number was passed with either SOUND_SetStreamChild_Int
	or SOUND_SetStreamParent_Int.
	By changing this value, data from another track will be loaded and played.
	Please note that playback of this data will continue from the current seek
	point through the file. Also note that the requested track will not be heard
	until its data has been loaded and played in SPU RAM.

	Requires:	i		Stream number
				track	Track number to play from interleaved data file (0-n)
***********************************************************************************/
void SOUND_SetStreamTrack_Int(int i,int track)
{
int parent;
int check;
	check=SOUND_CheckStream(i);
	if (check!=0)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamTrack_Int\n"));
		MSERROR(("Stream (%d) out of range\n",i));
		return;
	}

	parent=StreamBuffer[i].ParentStream;
	if (parent==-1)		/* Is this stream part of an interleaved stream (either parent or child)? */
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamTrack_Int\n"));
		MSERROR(("Stream is not playing interleaved data.\n"));
		MSERROR(("(or is not defined as a parent or child)\n"));
		return;
	}

	USR_HALT_MULTISTREAM_SET();
	StreamBuffer[i].IntTransOffset=track*StreamBuffer[parent].IntTransSize;
	USR_HALT_MULTISTREAM_CLR();
}

/**********************************************************************************
SOUND_SetStreamParent_Int (V5)
	* Plays interleaved ADPCM data, created with FPACKER PC utility *
	Flags a stream as interleaved and marks it as being a "Parent".
	What this means is that other streams can lock onto this stream, and use its
	data (loaded into IOP) as their data. The other streams are known as a "Child".
	Defining the main stream as a parent also has other effects on the system,
	such as only performing keyon for this and its child streams once ALL the streams
	are preloaded with data. This keeps everything synced for playing stereo tracks.
	Also, calling SOUND_StopStream with either the SPU channel or stream channel of
	the parent stream will have the effect of stopping both this stream and its
	child streams. This is because the parent stream handles the loading of data from
	cd or host. So if this one stops playing, all others that are using this data need
	to stop too.
	SOUND_PlayStream needs to be called before this function.

	Requires:		S			Stream number to make a parent (the one requested with SOUND_PlayStream)
					Track		Track to play from interleaved data
					TrackSize	Size of a single track.
	Notes:
	If, for example, you had 3 streams interleaved into one file, where the data is
	interleaved at 2048 byte tracks, you would set Track to 0-2 and TrackSize
	to the size of a single track (2048).
***********************************************************************************/
void SOUND_SetStreamParent_Int(int S,int Track,u_int TrackSize)
{
int check;
	check=SOUND_CheckStream(S);
	if (check!=0)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamParent_Int\n"));
		MSERROR(("Parent Stream (%d) out of range\n",S));
		return;
	}

	if (StreamBuffer[S].MemoryAllocated==0)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamParent\n"));
		MSERROR(("Stream IOP RAM not allocated for stream (%d)\n",S));
		MSERROR(("Call SOUND_AllocateStreamBuffer before this function\n"));
		return;
	}
	if (StreamBuffer[S].Status==STREAM_STATUS_OFF)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamParent\n"));
		MSERROR(("Stream (%d) not playing\n",S));
		MSERROR(("Call SOUND_PlayStream before this function\n"));
		return;
	}
	if (StreamBuffer[S].ParentChildFlag==SOUND_CHILD)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamParent\n"));
		MSERROR(("Stream (%d) already defined as a child so can not be a parent\n",S));
		return;
	}
	if (StreamBuffer[S].ResidentFlag!=STREAM_AUDIO_ADPCM)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamParent\n"));
		MSERROR(("Stream not defined as a ADPCM audio stream\n"));
		MSERROR(("Resident SPU, EE or IOP or PCM streams can NOT be parents\n"));
		return;
	}
	if (StreamBuffer[S].IntKeyOn!=0)
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamParent\n"));
		MSERROR(("Stream already playing\n"));
		MSERROR(("It's too late to make it a parent\n"));
		return;
	}

//	if (StreamBuffer[S].AllocSize<(TrackSize*Track)+TrackSize)
	if (StreamBuffer[S].AllocSize<(TrackSize*2*(Track+1)))		// MS7.21 - Thanks Sean!!
	{
		MSERROR((">>>>>ERROR: SOUND_SetStreamParent\n"));
		MSERROR(("Stream Buffer %d IOP buffer too small\n",S));
		MSERROR(("IOP Buffer Size = 0x%x\n",StreamBuffer[S].AllocSize));
		MSERROR(("Requested MIB offset = 0x%x (Track %d)\n",TrackSize*Track,Track));
		MSERROR(("Requested Track Size = 0x%x\n",TrackSize));
		return;
	}

	USR_HALT_MULTISTREAM_SET();		/* Stop MultiStream from processing streams.. */
	StreamBuffer[S].ParentStream=S;
	StreamBuffer[S].ParentChildFlag=SOUND_PARENT;	/* Set stream as a PARENT */
	StreamBuffer[S].IntTransOffset=Track*TrackSize;	/* Set offset through MIB data to use */
	StreamBuffer[S].IntTransSize=TrackSize;		/* Size of interleaved data */
	StreamBuffer[S].IOPBufferHalfSize=TrackSize;

	StreamBuffer[S].MIBEndOffset=SOUND_MIB_END_NOT_SET;

/* Check to make sure that SPU buffer is not larger than half of the IOP buffer */



#if (MS_LOAD62!=0)
	if (StreamBuffer[S].SPUBufferSize>TrackSize)
	{
		MSERROR((">>>>>WARNING: SOUND_SetStreamParent\n"));
		MSERROR(("Stream 0x%x SPU buffer size (0x%x) is larger than half IOP buffer size (0x%x)\n",
			S,StreamBuffer[S].SPUBufferSize,TrackSize));
		MSERROR(("Changing Stream 0x%x SPU buffer size to 0x%x\n",S,TrackSize));
		MSERROR(("Note that all following CHILD streams will automatically be changed also\n"));
		StreamBuffer[S].SPUBufferSize=TrackSize;
	}
#endif


//printf("SPU BUFFER SIZE %x\n",StreamBuffer[S].SPUBufferSize);

	USR_HALT_MULTISTREAM_CLR();		/* Allow MultiStream to process streams.. */
}


/**********************************************************************************
SOUND_TransferDataToEE
	Transfers a block of data from IOP RAM to specified EE address.
	Waits for completion before exiting.
	Used is stream loop mode is specified as STREAM_LOAD_EE (instead of STREAM_RESIDENT
	or STREAM_LOOPING etc..)
	
	Requires:	source		IOP Address (must be on a 4 byte boundry)
				dest		EE Address (must be on a 16 byte boundry)
				size		Size of data to transfer (must be a multiple of 16 bytes)
	Returns:	0			OK
				-1			Failed
***********************************************************************************/
int SOUND_TransferDataToEE(unsigned int source,unsigned int dest, unsigned int size)
{
sceSifDmaData JDMA;
static int SOUND_did=0;
int oldstat;

#if 1
	if (SOUND_did!=0)
	{
	    CpuSuspendIntr (&oldstat);
	  	while(sceSifDmaStat((unsigned int)SOUND_did) >= 0)
	  	{
		    CpuResumeIntr (oldstat);
			DelayThread(ONE_T);
		    CpuSuspendIntr (&oldstat);
	  	}
	    CpuResumeIntr (oldstat);
	}
#endif

    CpuSuspendIntr (&oldstat);
	JDMA.data = source;		/* IOP Address */
	JDMA.addr = dest;		/* EE address */
	JDMA.size = size;		/* Size to transfer */
	JDMA.mode = 0;
	if (!(SOUND_did = (int)sceSifSetDma(&JDMA, 1)))	/* Transfer data to EE */
	{
	    CpuResumeIntr (oldstat);
		MSERROR((">>>>>ERROR: SOUND_TransferDataToEE\n"));
		MSERROR(("Transfer to EE Failed\n"));
		return (-1);
	}

  	while(sceSifDmaStat((unsigned int)SOUND_did) >= 0)
  	{
	    CpuResumeIntr (oldstat);
		DelayThread(ONE_T);
	    CpuSuspendIntr (&oldstat);
  	}
    CpuResumeIntr (oldstat);


	return(0);
}

/**********************************************************************************
SOUND_FindFile
	Finds a required File ID

	Requires:	FileID		ID of file
	Returns:	Addr		Address of File ID record
				0			File ID not found
***********************************************************************************/
ADPCMFILE* SOUND_FindFile(int FileID)
{
int i;
ADPCMFILE *File;

	File=JADPCM;
	for (i=0;i<MAX_ADPCM_FILES;i++)	/* Loop through all files */
	{
		if (File->ID==FileID)		/* Found ID? */
			return(File);			/* Yes.. */
		File++;
	}
	return(0);	/* File not found. */
}

/**********************************************************************************
SOUND_GetFileIDInfo
	The same as SOUND_GetFileInfo, but only requires the ID of a file which has
	already been created with either SOUND_SetFileInfo or SOUND_CreateFileInfo

	Requires:	count		Value that is returned to the EE, so the EE knows when a
							new SOUND_FILE info packet has arrived.
				FileID		ID of File
***********************************************************************************/
void SOUND_GetFileIDInfo(u_int count, int FileID)
{
ADPCMFILE *File;

	File=SOUND_FindFile(FileID);	/* Search for FileID */
	if (File==0)	/* ID found? */
	{
		MSERROR((">>>>>ERROR: SOUND_GetFileIDInfo\n"));	/* No.. */
		MSERROR(("File ID Not found\n"));
		SOUND_FILE_LSN=0;
		SOUND_FILE_SIZE=0;
		SOUND_FILE_SOURCE=0;
		SOUND_FILE_COUNT=count;			/* This is used to tell the EE that this is NEW info. */
		SOUND_FILE_AT_OPEN=-1;
		SOUND_FILE_AT_ID=-1;
		return;
	}

	SOUND_FILE_LSN=File->lsn;		/* Copy LSN to return to EE */
	SOUND_FILE_SIZE=File->size;		/* Copy Size to return to EE */
	SOUND_FILE_SOURCE=(unsigned int)File->Source;	/* Copy source (host or cdrom) to return to EE */
	SOUND_FILE_COUNT=count;			/* Copy value passed to this function to return to EE */
	SOUND_FILE_AT_OPEN=File->OpenFlag;
	SOUND_FILE_AT_ID=File->OpenID;
}

/**********************************************************************************
SOUND_TransIOPToSPU
	Transfers IOP RAM to SPU RAM.
	Uses DMA channel1, so it doesn't interfere with MutiStream (which uses DMA 0)

	Requires:	IOPAddress		Address of data in IOP RAM
				SPUAddress		Address to transfer data to in SPU RAM
				Size			Size of data to transfer
	Returns:    transfer state:
				1  = transfer in progress
				0  = transfer done
				-1 = transfer failed

	This command blocks until the data has been transfered
	
	(MODIFIED BY BUZZ for SCREAM compatibility... added return values)
	
***********************************************************************************/
int SOUND_TransIOPToSPU(u_int IOPAddress,u_int SPUAddress,u_int Size)
{
int b;

 	b=SOUND_CheckSPUMemory(SPUAddress,Size);	/* Check that transfer address is valid */
	if (b==-1)	/* Is it valid? */
	{
		MSERROR((">>>>> ERROR: SOUND_TransIOPToSPU\n"));	/* No. */
		MSERROR(("SPU Memory Error\n"));
		return(-1);
	}

#if (SCREAM_COMPATIBLE)
   	// clear out the semaphore...
	while(PollSema(g_SPUDMADoneSema)!= KE_SEMA_ZERO);
#endif

	if (SOUND_Transfer.TranMode!=SOUND_DTS_ON)		/* DTS currently OFF ? */
	{
/* DTS turned OFF. Do a normal IOP->SPU DMA transfer */
		sceSdVoiceTransStatus(MS_USER_DMA,SD_TRANS_STATUS_WAIT);
		sceSdVoiceTrans (MS_USER_DMA, (SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA),
		(unsigned char *) IOPAddress,SPUAddress,Size);
		sceSdVoiceTransStatus(MS_USER_DMA,SD_TRANS_STATUS_WAIT);

		SOUND_UserTransfer.Status=0;
#if (SCREAM_COMPATIBLE)
		// done, so signal the semaphore...
		SignalSema(g_SPUDMADoneSema);
#endif
		return(0);
	}
	else
	{
		if (SOUND_UserTransfer.Status!=0)
		{
			MSERROR(("WARNING: SOUND_TransIOPToSPU\n"));
			MSERROR(("Last transfer not completed\n"));
			MSERROR(("Overwriting last transfer variables\n"));
		}
		SOUND_UserTransfer.IOPAddress=IOPAddress;
		SOUND_UserTransfer.SPUAddress=SPUAddress;
		SOUND_UserTransfer.Size=Size;
		SOUND_UserTransfer.Status=1;
		return(1);
	}
	return(-1);
}

/**********************************************************************************
SOUND_EXTERNAL_CDError
	If using EXTERNAL loading, call this function if an loading error occures
	(such as ejecting a disk)
	All audio streams will go into pause mode.

	Requires:	Nothing
	Returns:	Nothing
***********************************************************************************/
void SOUND_EXTERNAL_CDErrorPause(void)
{
	if (E_CDErrorCheck==0)	/* Are we already paused due to a CD error? */
	{
		USR_HALT_MULTISTREAM_SET();	/* No..Stop MultiStream from processing streams. */
		CDError_PauseStreams();		/* Pause all streams */
		E_CDErrorCheck=1;			/* Set flag to say that streams are paused due to CD error */
		MSERROR(("SOUND_EXTERNAL_CDErrorPause\n"));
		MSERROR(("USER PAUSING ALL STREAMS\n"));
		USR_HALT_MULTISTREAM_CLR();	/* Allow streams to process. */
	}
	else
	{
		MSERROR(("SOUND_EXTERNAL_CDErrorPause\n"));
		MSERROR(("AUDIO STREAMS ALREADY PAUSED\n"));
	}
}

/**********************************************************************************
SOUND_EXTERNAL_CDErrorRestart
	If using EXTERNAL loading, call this function once you know that the CD is valid
	again.
	All audio streams will restart

	Requires:	Nothing
	Returns:	Nothing
***********************************************************************************/
void SOUND_EXTERNAL_CDErrorRestart(void)
{
	if (E_CDErrorCheck!=0)	/* Are streams paused due to a CD error? */
	{
		CDError_ResumeStreams();	/* Yes. Start them playing again. */
		E_CDErrorCheck=0;	/* Clear cderror pause flag. */
		StreamBusy=0;		/* Clear StreamBusy flag (may have been set when error occured) */
		MSERROR(("SOUND_EXTERNAL_CDErrorPause\n"));
		MSERROR(("AUDIO STREAMS RESTARTED\n"));
	}
	else
	{
		MSERROR(("SOUND_EXTERNAL_CDErrorPause\n"));
		MSERROR(("AUDIO STREAMS ALREADY RESTARTED\n"));
	}
}

/**********************************************************************************
SOUND_SetStreamLoadSource
	Allows user to set each streams load source individually. So files can be loaded
	from either your own CD code (EXTERNAL) or MultiStreams own CD loading code
	(INTERNAL)

	Requires:	stream		Stream number to set load source
				source		USE_INTERNAL_LOADING (use MultiStreams CD code)
							USE_EXTERNAL_LOADING (use your own CD code)
	Returns:	Nothing
	
***********************************************************************************/
void SOUND_SetStreamLoadSource(int stream,int source)
{
int s;

	s=SOUND_CheckStream(stream);	/* Check if stream number is valid */
	if (s!=0)
	{
		MSERROR(("Error: SOUND_SetStreamLoadSource\n"));
		MSERROR(("Stream (%d) out of range\n",stream));
		return;
	}
	else if (StreamBuffer[stream].Status!=STREAM_STATUS_ON)	/* Is stream playing? */
	{
		MSERROR(("Error: SOUND_SetStreamLoadSource\n"));	/* No.. */
		MSERROR(("Stream not playing. Can not set load source\n"));
		return;
	}
	else if (StreamBuffer[stream].iStatus!=iSTATUS_INITIALISED)	/* Too late to set source? */
	{
		MSERROR(("Error: SOUND_SetStreamLoadSource\n"));	/* Yes. */
		MSERROR(("Too late to modify stream %x loading source\n",stream));
		MSERROR(("Stream already running\n"));
		return;
	}
	else
	{
		StreamBuffer[stream].LoadSource=(char)source;	/* Set source (host or cdrom) for stream */
	}
}


/**********************************************************************************
SOUND_StreamIgnoreVAGPitch

Ignores the VAG Header pitch value.
Normally, if a VAG header is read, its pitch value is used as the default pitch setting
for the required stream. By calling this straight after the SOUND_PlayStream function,
the pitch setting in the VAG header is ignored.

	Requires:	i	Stream number
	Returns:	nothing
***********************************************************************************/
void SOUND_StreamIgnoreVAGPitch(int i)
{
int s;

	s=SOUND_CheckStream(i);	/* Check if stream is valid */
	if (s!=0)	/* Is it? */
	{
		MSERROR(("Error: SOUND_StreamIgnoreVAGPitch\n"));	/* No. */
		MSERROR(("Stream (%d) out of range\n",i));
		return;
	}
	else if (StreamBuffer[i].Status!=STREAM_STATUS_ON)
	{
		MSERROR(("Error: SOUND_StreamIgnoreVAGPitch\n"));
		MSERROR(("Stream not playing. Can not ignore VAG pitch yet!\n"));
		return;
	}
	else if (StreamBuffer[i].iStatus!=iSTATUS_INITIALISED)
	{
		MSERROR(("Error: SOUND_StreamIgnoreVAGPitch\n"));
		MSERROR(("Too late to ignore VAG pitch\n"));
		return;
	}
	StreamBuffer[i].VAGPitch=SOUND_VAG_PITCH_OFF;	/* Don't set pitch of stream using VAG info */
}

/**********************************************************************************
SOUND_SetPCMVolume

Sets the volume for PCM playback

	Requires:	VolL		Left volume (0-0x7fff)
				VolR		Right volume (0-0x7fff)
***********************************************************************************/
void SOUND_SetPCMVolume(int VolL,int VolR)
{
//	if (PCMPlaying!=0)	// MUST be comented out for DSP to work..
//	{
		sceSdSetParam (MS_PCM_CORE | SD_P_BVOLL,(unsigned short)VolL);	/* Yes. Set SPU volume */
		sceSdSetParam (MS_PCM_CORE | SD_P_BVOLR,(unsigned short)VolR);
//	}
	PCMVolL=VolL;	/* Copy volume values */
	PCMVolR=VolR;	/* (Used for SOUND_ResumePCM and SOUND_StartPCM) */
}

/**********************************************************************************
SOUND_InitPCMPlayback

Initialises PCM playback. Allocates Stream buffer which is the same size (or twice
the size if you want to stream MONO files). Sets flags for stereo/mono and format
(WAV or PS2 PCM)

	Requires:		stream	Stream number to use for PCM playback
					format	SOUND_WAV or SOUND_PCM
					stereo	SOUND_STEREO or SOUND_MONO
					
***********************************************************************************/
void SOUND_InitPCMPlayback(int stream,int format,int stereo)
{
int oldstat;
u_int size;

	if (PCMBuffer!=0)	/* Is a PCM buffer already allocated? */
	{
		MSERROR(("SOUND_InitPCMPlayback\n"));
		MSERROR(("PCM Stream buffer already allocated\n"));
		MSERROR(("You need to close it before calling this function\n"));
		return;
	}

	if (StreamBuffer[stream].MemoryAllocated==0)	/* Is the stream buffer allocated? */
	{
		MSERROR(("SOUND_InitPCMPlayback\n"));
		MSERROR(("No memory allocated for stream %x\n",stream));
		MSERROR(("You need to allocate stream memory first\n"));
		return;
	}

	size=StreamBuffer[stream].AllocSize;	/* Size of buffer required for PCM buffer. */

	if (size<2048)	/* Buffer size too small? */
	{
		MSERROR(("SOUND_InitPCMPlayback\n"));
		MSERROR(("Stream %x buffer to small for PCM playback\n",stream));
		MSERROR(("Must be a multiple of 2048 bytes\n"));
		return;
	}
	if ((size&0x7ff)!=0)	/* Buffer size NOT a multiple of 2048? */
	{
		MSERROR(("SOUND_InitPCMPlayback\n"));
		MSERROR(("Stream %x buffer size is %x\n",stream,size));
		MSERROR(("Size must be a multiple of 2048 bytes\n"));
		return;
	}
	if ((stereo==SOUND_MONO)||(stereo==SOUND_BITSTREAM))	/* Want to play MONO PCM? */
	{
		PCMMonoAlloc=1;		/* Set MONO playback flag */
/* PCM buffer needs to be twice the size, as you're loading twice the amount of data if */
/* loading MONO. */
		size*=2;
	}
	else
		PCMMonoAlloc=0;		/* Clear MONO playback flag (set to STEREO) */

    CpuSuspendIntr (&oldstat);
	PCMBuffer = (unsigned int) AllocSysMemory (SMEM_Low, size, NULL);	/* Allocate IOP PCM buffer */
    CpuResumeIntr (oldstat);
	PCMBufferSize=size;	/* Copy buffer size. (Sent to EE) */

	SOUND_SetPCMType(format,stereo);				/* Set PCM playback type */
	sceSdSetParam (MS_PCM_CORE | SD_P_BVOLL,0);		/* Set PCM volume to 0 */
	sceSdSetParam (MS_PCM_CORE | SD_P_BVOLR,0);
	PCMPlaying=0;	/* Not playing. */
	PCMStream=-1;	/* Stream which is playing PCM is set in SOUND_PlayStream */
	PCMPause=0;		/* Not paused. */
}


/**********************************************************************************
SOUND_InitPCMPlaybackNoStream

Initialises PCM playback. 

	Requires:		size	Size of stream buffer
					mode	SOUND_PCM or SOUND_BITSTREAM
					
***********************************************************************************/
void SOUND_InitPCMPlaybackNoStream(u_int size, int mode)
{
int oldstat;

	if (PCMBuffer!=0)	/* Is a PCM buffer allocated already? */
	{
		MSERROR(("SOUND_InitPCMPlaybackNoStream\n"));
		MSERROR(("PCM Stream buffer already allocated\n"));
		MSERROR(("You need to close it before calling this function\n"));
		return;
	}

    CpuSuspendIntr (&oldstat);
	PCMBuffer = (unsigned int) AllocSysMemory (SMEM_Low, size, NULL);	/* Allocate PCM buffer */
    CpuResumeIntr (oldstat);

	PCMBufferSize=size;

	sceSdSetParam (MS_PCM_CORE | SD_P_BVOLL,0);	/* Set PCM volume to 0 */
	sceSdSetParam (MS_PCM_CORE | SD_P_BVOLR,0);
	PCMPlaying=0;	/* Not playing */
	PCMStream=-1;	/* No stream using PCM playback */
	PCMPause=0;		/* Not paused */
	if ((mode!=SOUND_PCM)&&(mode!=SOUND_BITSTREAM))
	{
		MSERROR(("SOUND_InitPCMPlaybackNoStream\n"));
		MSERROR(("parameter (0x%x) is no SOUND_PCM or SOUND_BITSTREAM\n",mode));
		return;
	}
	PCMStereo=(unsigned char)mode;
}

/**********************************************************************************
SOUND_SetPCMType
Sets PCM playback type (WAV or PS2 PCM format, Mono or Stereo)

	Requires:	Format	SOUND_WAV or SOUND_PCM
				Stereo	SOUND_MONO or SOUND_STEREO

***********************************************************************************/
void SOUND_SetPCMType(int Format, int Stereo)
{
	if (PCMStream!=-1)	/* Any stream playing PCM? */
	{
		if (StreamBuffer[PCMStream].Status!=STREAM_STATUS_ON)	/* Yes. Is it too ON? */
		{
			MSERROR(("Error: SOUND_SetPCMType\n"));
			MSERROR(("Stream not playing. Can not set PCM type\n"));
			return;
		}
		else if (StreamBuffer[PCMStream].iStatus!=iSTATUS_INITIALISED)	/* Too late? */
		{
			MSERROR(("Error: SOUND_SetPCMType\n"));
			MSERROR(("Too late to set PCM type for stream %x\n",PCMStream));
			MSERROR(("Stream already running\n"));
			return;
		}
	}

	if (PCMBuffer==0)	/* PCM IOP Buffer allocated? */
	{
		MSERROR((">>>>Error: SOUND_SetPCMType\n"));	/* No.. */
		MSERROR(("You need to call SOUND_InitPCMPlayback first\n"));
		return;
	}

	if ((Stereo==SOUND_MONO)||(Stereo==SOUND_BITSTREAM))
	{
		if (PCMMonoAlloc==0)	/* IOP PCM buffer large enough? */
		{
			MSERROR((">>>>Error: SOUND_SetPCMType\n"));
			MSERROR(("You need to call SOUND_InitPCMPlayback with SOUND_MONO or SOUND_BITSTREAM\n"));
			return;
		}
	}
	PCMStereo=(unsigned char)Stereo;
	PCMType=(unsigned char)Format;
}

/**********************************************************************************
SOUND_ClosePCMPlayback

	Free IOP RAM PCM buffer. Stops any PCM playback. Stops PCM callbacks.
	Stops a stream if its used for PCM playback

***********************************************************************************/
void SOUND_ClosePCMPlayback(void)
{
int oldstat;

	if (PCMBuffer!=0)	/* PCM IOP Buffer allocated? */
	{
		if ((PCMPlaying!=0)&&(PCMStream!=-1))	/* Yes. Is a stream playing PCM data? */
			SOUND_StopStream(PCMStream,0);	/* Yes. Stop stream */

		SOUND_StopPCM();	/* Stop PCM playback */

	    CpuSuspendIntr (&oldstat);
	 	FreeSysMemory( (void*)PCMBuffer);	/* Free IOP PCM buffer RAM. */
	    CpuResumeIntr (oldstat);
		PCMBuffer=0;	/* Set flag to say that no PCM buffer is allocated */
	}
	else
	{
		MSERROR(("Error: SOUND_ClosePCMBuffer\n"));
		MSERROR(("Buffer not open - so can't close it\n"));
	}
}

/**********************************************************************************
SOUND_StartPCM
	Starts PCM playback and sets PCM volume
***********************************************************************************/
void SOUND_StartPCM(void)
{
	if (PCMBuffer==0)	/* Is PCM IOP buffer allocated? */
	{
		MSERROR((">>>>Error: SOUND_StartPCM\n"));	/* No. */
		MSERROR(("No PCM buffer allocated\n"));
		MSERROR(("Call either SOUND_InitPCMPlayback or SOUND_InitPCMPlaybackNoStream first\n"));
		return;
	}

/* Set PCM or BITSTREAM playback mode.. */
	if (PCMStereo==SOUND_BITSTREAM)
		SOUND_EnableBitStream();
	else
		SOUND_EnablePCM();

	PCMPause=0;			/* Stream not paused */
	PCMPlaying=1;
	SOUND_SetPCMVolume(PCMVolL,PCMVolR);
	sceSdBlockTrans( MS_PCM_CORE, SD_TRANS_MODE_WRITE|SD_BLOCK_LOOP,
					(u_char*)PCMBuffer, PCMBufferSize);
}

/**********************************************************************************
SOUND_StopPCM
	Stops PCM playback. Sets PCM volume to 0
***********************************************************************************/
void SOUND_StopPCM(void)
{
	if (PCMBuffer==0)	/* IOP PCM buffer allocated? */
	{
		MSERROR((">>>>Error: SOUND_StopPCM\n"));	/* No. */
		MSERROR(("No PCM buffer allocated\n"));
		return;
	}

	SOUND_SetPCMVolume(0,0);	/* Set PCM volume to 0 */
	PCMPlaying=0;	/* Stop playing flag */
	PCMStream=-1;	/* Not attached to a stream */
	PCMPause=0;		/* Not Paused. */
    sceSdBlockTrans (MS_PCM_CORE, SD_TRANS_MODE_STOP, NULL, 0);	/* Stop IOP->SPU transfer */
}

/**********************************************************************************
SOUND_PausePCM
	Pauses PCM playback. Sets volume to 0.
***********************************************************************************/
void SOUND_PausePCM(void)
{
	PCMPause=1;	/* Set PCM paused flag */
	sceSdSetParam (MS_PCM_CORE | SD_P_BVOLL,0);	/* Set volumes to 0 */
	sceSdSetParam (MS_PCM_CORE | SD_P_BVOLR,0);
    PCMPausePos = (u_int)sceSdBlockTrans (MS_PCM_CORE, SD_TRANS_MODE_STOP, NULL, 0)&0x00ffffff;	/* Stop IOP->SPU transfer */
}

/**********************************************************************************
SOUND_ResumePCM
	Resumes playback if paused.
***********************************************************************************/
void SOUND_ResumePCM(void)
{
	PCMPause=0;	/* Pause flag OFF */
	if (PCMPlaying==0)		/* Were we playing before? */
		PCMPlaying=1;		/* No.. */
	SOUND_SetPCMVolume(PCMVolL,PCMVolR);	/* Set PCM volumes */
	sceSdBlockTrans( MS_PCM_CORE, SD_TRANS_MODE_WRITE_FROM|SD_BLOCK_LOOP,
					(u_char*)PCMBuffer, PCMBufferSize,PCMPausePos);	/* Resume for last pos.. */
}

/**********************************************************************************
SOUND_TransferPCM
	Handles the transfer of PCM data, depending on type (wav/pcm, mono/stereo)
	Called by SOUND_HandleSPUTransfer if playing PCM data
	Based on the functions _BgmRaw2Spu and _BgmRaw2Mono taken from the ezBGM sample code

	Requires:	source		StreamBuffer source data address
				dest		PCM playback buffer destination address
				size		size of data to transfer
***********************************************************************************/
void SOUND_TransferPCM (u_int source, u_int dest,u_int size)
{
int i;
long long *dp;	/* Uses long long for speed.. */
long long *sp;	/* if you change this to just a long, change "for" loop to /32 instead of /64 */

	sp=(long long*)source;		/* Stream Buffer address */
	dp=(long long*)dest;		/* PCM buffer address */

    if (PCMStereo==SOUND_MONO)	/* Mono?  */
    {
/* Copy the data to both Left&Right outputs */
	    for (i = 0; i < size/(SPU_PCM_BLOCK_SIZE); i++)
	    {
			memcpy ((void *)((int)dp + (i * SPU_PCM_BLOCK_SIZE *2) ),
			(void *)((int)sp + (i * SPU_PCM_BLOCK_SIZE)),SPU_PCM_BLOCK_SIZE);
			memcpy ((void *)((int)dp + (i * SPU_PCM_BLOCK_SIZE *2)  + SPU_PCM_BLOCK_SIZE),
			(void *)((int)sp +( i * SPU_PCM_BLOCK_SIZE)),SPU_PCM_BLOCK_SIZE);
	    }
    }
	else if (PCMStereo==SOUND_BITSTREAM)
	{
	    for (i = 0; i < size/(SPU_PCM_BLOCK_SIZE); i++)
	    {
			memcpy ((void *)((int)dp + (i * SPU_PCM_BLOCK_SIZE *2) ),
			(void *)((int)sp + (i * SPU_PCM_BLOCK_SIZE)),SPU_PCM_BLOCK_SIZE);
			memset ((void *)((int)dp + (i * SPU_PCM_BLOCK_SIZE *2)  + SPU_PCM_BLOCK_SIZE),
			0,SPU_PCM_BLOCK_SIZE);
		}
	}
	else if (PCMType==SOUND_WAV)	/* WAV file? convert data to 256 samples L/R */
	{
		_BgmRaw2Spu ((u_long *)sp,(u_long *)dp, size/1024);
	}
	else	/* MUST BE A STEREO PS2 PCM file. */
	{
		for (i=0;i<size/64;i++)	/* Copy data from stream buffer to PCM buffer */
		{
			*dp++=*sp++;	/* Unravel loop to speed up transfer a little.. */
			*dp++=*sp++;
			*dp++=*sp++;
			*dp++=*sp++;
			*dp++=*sp++;
			*dp++=*sp++;
			*dp++=*sp++;
			*dp++=*sp++;
		}
	}
}

/**********************************************************************************
SOUND_PCMOnceOff
	Handles clearing of the PCM buffer if playing once off data.
	You need to make sure that: (a) the PCM playback has finished playing the required
	data, and (b) when it has, that it plays silent data until its shut down.
	This routine makes a quiet (zero) buffer.

	Requires:	i	Stream number

***********************************************************************************/
void SOUND_PCMOnceOff(int i)
{
int *addr;
u_int size;
int a;

	addr=(int*)StreamBuffer[i].IOPAddress;	/* Current IOP buffer half */
	size=StreamBuffer[i].LastBufferSize;	/* Size of half of the PCM buffer */

	for (a=0;a<(size/4);a++)
	{
		*(addr++)=0;	/* Clear buffer half */
	}
	SOUND_HandleSPUTransfer(i,StreamBuffer[i].IOPAddress,PCMLastAddr,size,0);
}

/**********************************************************************************
SOUND_FastINTERNAL_RPC
	If loading EE data via INTERNAL loading, and FASTLOAD is enabled, an RPC is sent
	to the EE after the data has been transferred. This RPC will either allow more
	EE data to be loaded or not.

	Requires:	i				Stream channel
				totalsize		total size of file transferred so far
				lastsize		buffer size last transferred
				status		    0 =	more data to load/transfer
								1 = no more data to load/transfer
								2 = CD error detected
								3 = Another Error...(!)
	Returns:	Nothing
***********************************************************************************/
u_int SOUND_FastINTERNAL_RPC(int i,u_int totalsize,u_int lastsize,u_int status)
{
int c;

	c=i;
	i&=0x7fff;

	SOUND_INTBuffer[0]=(unsigned int)i;	/* Stream */
	SOUND_INTBuffer[1]=totalsize;
	SOUND_INTBuffer[2]=lastsize;
	SOUND_INTBuffer[3]=status;
	SOUND_INTBuffer[4]=StreamBuffer[i].SPUAddress;
	SOUND_INTBuffer[6]=(u_int)StreamBuffer[i].FileID;

	if ((SOUND_INTERNAL_FastLoad==SOUND_FASTLOAD_ON)&&(SOUND_BIND_FastLoad!=0))
	{
		if (c>=0x8000)	// perform EE flush cache?
		{
			SOUND_INTBuffer[0]=999;	/* Stream */

			sceSifCallRpc(&INTERNALrpc, 0, 0, (void *)SOUND_INTBuffer,
				sizeof(SOUND_INTBuffer), (void *)SOUND_INTPacket, 64, 0, 0);
				return(0);
		}

		StreamBuffer[i].FastLoadCounter++;
		SOUND_INTBuffer[5]=StreamBuffer[i].FastLoadCounter;
		sceSifCallRpc(&INTERNALrpc, 0, 0, (void *)SOUND_INTBuffer,
			sizeof(SOUND_INTBuffer), (void *)SOUND_INTPacket, 64, 0, 0);

		if (SOUND_INTPacket[0]==1)
			StreamBuffer[i].EEAllow=1;
		else
			StreamBuffer[i].EEAllow=0;

		if (SOUND_INTPacket[6]==1)
		{
			StreamBuffer[i].NextLoad=(u_char)SOUND_INTPacket[7];	// Dest (SOUND_LOAD_EE / IOP / SPU)
			StreamBuffer[i].NextFileID=SOUND_INTPacket[8];	// File ID
			StreamBuffer[i].NextAddress=SOUND_INTPacket[9];	// Next address to load to (0=continue from last)
//		printf("NEXT EE LOAD REQUESTED!\n");
//		printf("Dest %x\n",SOUND_INTPacket[7]);
//		printf("FileID %x\n",SOUND_INTPacket[8]);
//		printf("Address %x\n",SOUND_INTPacket[9]);
		}

		if (SOUND_INTPacket[2]==1)
		{
			StreamBuffer[i].NextFileID_FOAPS=SOUND_INTPacket[3]+1;	// File ID +1
			StreamBuffer[i].NextFileOffset_FOAPS=SOUND_INTPacket[4];
			StreamBuffer[i].NextFileSize_FOAPS=SOUND_INTPacket[5];
//		printf("FOAPS REQUESTED!\n");
//		printf("FileID %x\n",SOUND_INTPacket[3]);
//		printf("Offset %x\n",SOUND_INTPacket[4]);
//		printf("Size %x\n",SOUND_INTPacket[5]);
		}


#if MS_TIMER
	SignalSema(gSem);	/* Signal StreamADPCM to be called */
#endif

		if (SOUND_INTPacket[1]==0)	/* No EE address passed back from RPC ? */
			return (StreamBuffer[i].SPUAddress+lastsize);
		else
			return (SOUND_INTPacket[1]);	/* EE address passed back from RPC. Use this address for next load */
	}
	else
	{
		return (StreamBuffer[i].SPUAddress+lastsize);
	}
}

/**********************************************************************************
SOUND_InitINTERNALFastLoad
	Turns on INTERNAL Fast loading.
	This makes MultiStream run at its optimum rate. It will no longer request a stream
	load from the EE for audio streams. EE data streams will request a load via the
	EE RPC function: SOUND_RPC.
***********************************************************************************/
void SOUND_InitINTERNALFastLoad(int type,int flag)
{
	if (type==SOUND_FASTLOAD_BIND_RPC)
	{
		SOUND_BIND_FastLoad=0;
	    while (1) 
	    {
		    if (sceSifBindRpc(&INTERNALrpc, SOUND_RPC_DEVICE, 0) < 0) 
		    {
				MSERROR(("SOUND_InitINTERNALFastLaod\n"));
		        MSERROR(("Couldn't bind RPC.\n"));
				return;
		    }
		    if (INTERNALrpc.serve != 0) 
			{
				SOUND_BIND_FastLoad=1;
				break;
			}
	    }
		SOUND_BIND_FastLoad=1;
	}
	else if (type==SOUND_FASTLOAD_ON_OFF)
	{
		if (flag==SOUND_FASTLOAD_INVALID_DCACHE_ON)
		{
			SOUND_FastLoadDCache=1;
			return;
		}
		else if (flag==SOUND_FASTLOAD_INVALID_DCACHE_OFF)
		{
			SOUND_FastLoadDCache=0;
			return;
		}
/* Note: type may also be SOUND_FASTLOAD_ON or SOUND_FASTLOAD_OFF at this point.. */
		SOUND_INTERNAL_FastLoad=(char)flag;
		return;
	}

	else if (type==SOUND_FASTLOAD_NEXT)
	{
		StreamBuffer[flag].EEAllow=1;
		return;
	}
}

/**********************************************************************************
USR_HALT_MULTISTREAM_SET
	Software switch to stop any updating of StreamADPCM
***********************************************************************************/
void USR_HALT_MULTISTREAM_SET(void)
{
	USR_HALT_MULTISTREAM=1;
}

/**********************************************************************************
USR_HALT_MULTISTREAM_CLR
	Software switch to allow updating of StreamADPCM
***********************************************************************************/
void USR_HALT_MULTISTREAM_CLR(void)
{
	USR_HALT_MULTISTREAM=0;
}

/**********************************************************************************
SOUND_SetChannelOutputOn
 Turns direct output for an individual channel ON.
 Thankyou to Patrick Bergeron @ Digital Fiction for this routine.
***********************************************************************************/
void SOUND_SetChannelOutputOn(int channel)
{
int core;
int dis, oldstat;
u_int work;

#if (SCREAM_COMPATIBLE)
	if(!CHANNEL_FLAG_SET(channel,g_MSChannelUseMask)) return;
#endif
	
	core=SOUND_GetCore(channel);   /* SPU Core */
	channel=SOUND_GetSPUChannel(channel); /* SPU Channel */

	g_OutputChans[core] |= (unsigned int)(1<<channel); /* Set SPU channel in core n effect bitmask */
    dis = CpuSuspendIntr (&oldstat);
	work = (u_int)sceSdGetSwitch((u_short)(core | SD_S_VMIXL));
	work &= (~g_MSChannelUseMask[core]);
	work |= (g_OutputChans[core] & g_MSChannelUseMask[core]);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXL), work);	/* Set bitmask LEFT */
	work = (u_int)sceSdGetSwitch((u_short)(core | SD_S_VMIXR));
	work &= (~g_MSChannelUseMask[core]);
	work |= (g_OutputChans[core] & g_MSChannelUseMask[core]);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXR), work);	/* Set bitmask RIGHT */
    if(dis == KE_OK) CpuResumeIntr (oldstat);
}

/**********************************************************************************
SOUND_SetChannelOutputOff
 Turns direct output for an individual channel OFF.
 Thankyou to Patrick Bergeron @ Digital Fiction for this routine.

 Requires: channel  SPU Channel 0-47
***********************************************************************************/
void SOUND_SetChannelOutputOff(int channel)
{
int core;
int dis, oldstat;
u_int work;

#if (SCREAM_COMPATIBLE)
	if(!CHANNEL_FLAG_SET(channel,g_MSChannelUseMask)) return;
#endif

	core=SOUND_GetCore(channel);  /* SPU Core */
	channel=SOUND_GetSPUChannel(channel); /* SPU Channel */

	g_OutputChans[core] &= (unsigned int)(~(1<<channel)); /* Clear SPU channel in core n effect bitmask */
    dis = CpuSuspendIntr (&oldstat);
	work = (u_int)sceSdGetSwitch((u_short)(core | SD_S_VMIXL));
	work &= (~g_MSChannelUseMask[core]);
	work |= (g_OutputChans[core] & g_MSChannelUseMask[core]);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXL), work);	/* Set bitmask LEFT */
	work = (u_int)sceSdGetSwitch((u_short)(core | SD_S_VMIXR));
	work &= (~g_MSChannelUseMask[core]);
	work |= (g_OutputChans[core] & g_MSChannelUseMask[core]);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXR), work);	/* Set bitmask RIGHT */
    if(dis == KE_OK) CpuResumeIntr (oldstat);
}

/**********************************************************************************
SOUND_AllChannelEffectsOn
 Turns effect processing for all 24 channels (core n) ON
 Thankyou to Patrick Bergeron @ Digital Fiction for this routine.

 Requires: core Core 0 or 1
***********************************************************************************/
void SOUND_AllChannelOutputOn(int core)
{
int dis, oldstat;
	
	g_OutputChans[core]=g_MSChannelUseMask[core];  /* Set all bits (24 channels) in bitmask */
    dis = CpuSuspendIntr (&oldstat);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXL), sceSdGetSwitch((u_short)(core | SD_S_VMIXL)) | g_MSChannelUseMask[core]);	/* Set bitmask LEFT */
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXR), sceSdGetSwitch((u_short)(core | SD_S_VMIXR)) | g_MSChannelUseMask[core]);	/* Set bitmask RIGHT */
    if(dis == KE_OK) CpuResumeIntr (oldstat);
}

/**********************************************************************************
SOUND_AllChannelEffectsOff
 Turns effect processing for all 24 channels (core n) OFF
 Thankyou to Patrick Bergeron @ Digital Fiction for this routine.

 Requires: core Core 0 or 1
***********************************************************************************/
void SOUND_AllChannelOutputOff(int core)
{
int dis, oldstat;
	
	g_OutputChans[core]=0; /* Clear all bits (24 channels) in bitmask */
    dis = CpuSuspendIntr (&oldstat);
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXL), sceSdGetSwitch((u_short)(core | SD_S_VMIXL)) & (~g_MSChannelUseMask[core]) );	/* Set bitmask LEFT */
	sceSdSetSwitch( (unsigned short)(core | SD_S_VMIXR), sceSdGetSwitch((u_short)(core | SD_S_VMIXR)) & (~g_MSChannelUseMask[core]) );	/* Set bitmask RIGHT */
    if(dis == KE_OK) CpuResumeIntr (oldstat);
}

/**********************************************************************************
SOUND_SetTVOutputOn
	Allow audio to be played through TV output. This is also the default output
	(Everything is turned on - PCM dry / wet, the lot...)

	Core0: 111111110000 (no external input)
	Core1: 111111111111 (external input - core 0 -> core1)

***********************************************************************************/
void SOUND_SetTVOutputOn(void)
{
	sceSdSetParam (SD_CORE_0 | SD_P_MMIX,0xff0);
	sceSdSetParam (SD_CORE_1 | SD_P_MMIX,0xfff);
}

/**********************************************************************************
SOUND_SetTVOutputOff
	Disable audio to be played through TV output. May be necessary, if, for example
	you are streaming audio via SPDIF (for DTS?) and do not want any audio from the
	TV speakers.
***********************************************************************************/
void SOUND_SetTVOutputOff(void)
{
	sceSdSetParam (SD_CORE_0 | SD_P_MMIX,0);
	sceSdSetParam (SD_CORE_1 | SD_P_MMIX,0);
}

#if (MS_LOAD62==1)
/**********************************************************************************
IOP->SPU transfer
V6 now allows you to resize your SPU RAM buffers. This allows you to have, say, a
64K IOP buffer, but stream this into a SPU RAM buffer as small as 1K. This obviously
saves you a lot of SPU RAM.

To do this, the following method is used:

1) Each frame, SOUND_CheckSPUTransfer is called, transferring any data required to
the SPU RAM. The transfer size can be set to a smaller size than the actual SPU buffer,
so DTS can still run if implemented.

2) When MultiStream makes a request for a transfer, the relevent info is passed to
SOUND_SetLoadRequest.

3) There are 3 stages to updating the SPU. (a) Start, (b) Transfer, (c) End.
The start and end code is only called once, but multiple transfers can be called
to get all of the required data to SPU RAM.


***********************************************************************************/
/**********************************************************************************
SOUND_SetLoadRequest
	Called by MultiStream to register a request for ADPCM data to be sent over to SPU
	RAM. This new method allows for stream buffers to be up to 90% smaller than
	in previous versions..

	Requires:	i			Stream number
				IOPAddr		IOP Address to read data from
				Size		Size of data to transfer
				FinalEnd	Flag to say if transfer is NORMAL, SFX or READY TO DIE (last transfer)
***********************************************************************************/
void SOUND_SetLoadRequest(int i,u_int IOPAddr,u_int Size,u_int FinalEnd)
{
int a;

//printf("LOAD REQUEST %x (%x)\n",i,a);

	a=(int)StreamBuffer[i].DMAInfo.CurrentBuffer;

	StreamBuffer[i].DMAInfo.Stream=(unsigned char)i;			/* Stream channel which has requested transfer */
	StreamBuffer[i].DMAInfo.IOPAddress[a]=IOPAddr;		/* IOP Address of ADPCM buffer */
	StreamBuffer[i].DMAInfo.TotalSize+=Size;

	StreamBuffer[i].DMAInfo.IOPSize[a]=(int)Size;		/* Size of IOP buffer */
	StreamBuffer[i].DMAInfo.FinalEnd[a]=FinalEnd;		/* Size of IOP buffer */
	StreamBuffer[i].DMAInfo.Status[a]=1;			/* Current status */
	StreamBuffer[i].DMAInfo.InitIOPAddress=IOPAddr;
	StreamBuffer[i].DMAInfo.InitIOPSize=Size;
	StreamBuffer[i].DMAInfo.InitSize=StreamBuffer[i].DMAInfo.TotalSize;
	StreamBuffer[i].DMAInfo.PacketSize[a]=Size;

//	StreamBuffer[i].DMAInfo.TranSize=0;	//Size; // MS7.3

	StreamBuffer[i].DMAInfo.TransType[a]=StreamBuffer[i].ResidentFlag;

	if (StreamBuffer[i].DMAInfo.FinalEnd[a]==STREAM_SPU_SFX)	/* Transfer SFX? */
		StreamBuffer[i].DMAInfo.CurrentStat[a]=SOUND_SPU_SFX;
	else if (StreamBuffer[i].DMAInfo.Initialised==0)			/* First transfer of ADPCM stream? */
	{
		StreamBuffer[i].DMAInfo.CurrentStat[a]=SOUND_SPU_PRELOAD;

//printf("PRELOAD %x\n",i);
		if (StreamBuffer[i].ParentChildFlag==0)	/* Parent? */
			StreamBuffer[i].DMAInfo.PacketSize[a]/=2;
	}
	else
	{
		StreamBuffer[i].DMAInfo.CurrentStat[a]=SOUND_SPU_UPDATE_HALF_SIZE;	/* ADPCM stream update.. */
//printf("HALFSIZE %x\n",i);
	}

	a=1-a;	/* Swap to other buffer half */
	StreamBuffer[i].DMAInfo.Initialised=1;		/* Set flag to say that at least one transfer has been requested */
	StreamBuffer[i].DMAInfo.CurrentBuffer=(unsigned int)a;	/* New current buffer to write the next request to.. */
}

/**********************************************************************************
SOUND_InitLoadRequest
	Called by MultiStream (SOUND_PlayStream)
	Initialises information of streaming using small SPU RAM buffers

	Requires:	i		Stream number
***********************************************************************************/
void SOUND_InitLoadRequest(int i)
{
//if (i<2) printf("*** InitLoadRequest %x\n",i);

	StreamBuffer[i].DMAInfo.Status[0]=0;			/* Current status OFF */
	StreamBuffer[i].DMAInfo.Status[1]=0;			/* Current status OFF */
	StreamBuffer[i].DMAInfo.CurrentBuffer=0;		/* Buffer used for new requests */
	StreamBuffer[i].DMAInfo.UsedBuffer=0;			/* Buffer currently being processed */
	StreamBuffer[i].DMAInfo.SPUAddress=StreamBuffer[i].SPUAddress;
	StreamBuffer[i].DMAInfo.SPUSize=StreamBuffer[i].SPUBufferSize;

    StreamBuffer[i].DMAInfo.TransCount=0;
    StreamBuffer[i].DMAInfo.SizeCount=0;
    StreamBuffer[i].DMAInfo.PlayHalf=0;
	StreamBuffer[i].DMAInfo.TotalSize=0;
	StreamBuffer[i].Chld=0;
	StreamBuffer[i].ChldWait=0;
	StreamBuffer[i].DMAInfo.Initialised=0;	/* No transfers have been requested */
}

/**********************************************************************************
SOUND_SetSPUTransRequest
	Makes a request for a IOP->SPU transfer.

	Requires:	IOPAddr		IOP address (source)
				SPUAddr		SPU address (destination)
				Size		Size of transfer
				i			Stream number
	
***********************************************************************************/
void SOUND_SetSPUTransRequest(u_int IOPAddr,u_int SPUAddr,u_int Size,u_int i)
{

//printf(">> SetSPUTransRequest %x\n",Size);
	SOUND_SPUTransfer.TransFlag=1;			/* Not really used.. */
	SOUND_SPUTransfer.IOPAddress=IOPAddr;	/* Make a copy of IOP,SPU addresses and Size.. */
	SOUND_SPUTransfer.SPUAddress=SPUAddr;
	SOUND_SPUTransfer.DataSize=Size;
/* TranSize is the amount of data which is to be transferred to SPU each frame.. */
	if ((SOUND_Transfer.TranMode==SOUND_DTS_ON)&&(Size>DTS_MAX_SIZE))	/* DTS ON? Size larger than max DTS size? */
	{
		SOUND_SPUTransfer.TranSize=DTS_MAX_SIZE;	/* Use DTS Max size.. */
	}
	else
		SOUND_SPUTransfer.TranSize=Size;	/* Whole size */


	SOUND_SPUTransfer.Stream=(unsigned char)i;
	SOUND_SPUTransfer.Status=STREAM_STATUS_ON;	/* SPU->IOP request made.. */
}

/**********************************************************************************
SOUND_CheckSPUTransfer
	Transfer a packet of data from IOP->SPU if a request for such has been made
	via SOUND_SetSPUTransRequest.
	If all data is transfered, update the stream status.

	Requires:	Nothing
	Returns:	0			No transfer required
				1			Transfer not completed (more data still to transfer)
				2			Transfer completed
	
***********************************************************************************/
int SOUND_CheckSPUTransfer(void)
{
u_int i,b;
SPUDMA *ptr;
STREAMDATA *p2;

	b=(u_int)sceSdVoiceTransStatus(MS_SPU_DMA,SD_TRANS_STATUS_CHECK);
	if (b==0)
		return (1);


	if (SOUND_SPUTransfer.Status==STREAM_STATUS_ON)	/* Do we need to transfer data to SPU RAM? */
	{
		if (StreamBuffer[SOUND_SPUTransfer.Stream].Status!=STREAM_STATUS_ON)	/* Is Stream still active? */
		{
			SOUND_SPUTransfer.Status=STREAM_STATUS_OFF;	/* No, so don't bother transferring more data.. */
			return(2);	/* Act as if all data is transferred */
		}

		if (SOUND_SPUTransfer.TranSize>SOUND_SPUTransfer.DataSize)	/* Transfer packet size > IOP Data size? */
			SOUND_SPUTransfer.TranSize=SOUND_SPUTransfer.DataSize;	/* Use IOP Data size */

//printf("Transferring %x from %x to %x\n",SOUND_SPUTransfer.TranSize,SOUND_SPUTransfer.IOPAddress,SOUND_SPUTransfer.SPUAddress);

		b=(u_int)sceSdVoiceTrans (MS_SPU_DMA, (SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA),	/* Transfer data to SPU */
		(unsigned char *) SOUND_SPUTransfer.IOPAddress,
		SOUND_SPUTransfer.SPUAddress,
		SOUND_SPUTransfer.TranSize);

		SOUND_SPUTransfer.DataSize-=SOUND_SPUTransfer.TranSize;		/* Decrease IOP Data size */
		SOUND_SPUTransfer.SPUAddress+=SOUND_SPUTransfer.TranSize;	/* Increase SPU RAM address */
		SOUND_SPUTransfer.IOPAddress+=SOUND_SPUTransfer.TranSize;	/* Increase IOP RAM address */

	if (SOUND_Transfer.TranMode==SOUND_DTS_ON)		/* DTS currently ON ? */
	{
		do{
			b=sceSdVoiceTransStatus(MS_SPU_DMA, SD_TRANS_STATUS_CHECK);
		}while(b==0);
	}
	else
		sceSdVoiceTransStatus(MS_SPU_DMA,SD_TRANS_STATUS_WAIT);	// Wait for any transfer to complete

		if (SOUND_SPUTransfer.DataSize==0)		/* All data transfered to SPU RAM? */
		{
			SOUND_SPUTransfer.Status=STREAM_STATUS_OFF;	/* Yes..Turn the transfer status OFF */

			i=SOUND_SPUTransfer.Stream;
			b=StreamBuffer[i].DMAInfo.UsedBuffer;
			p2=&StreamBuffer[i];
			ptr=&p2->DMAInfo;		/* Point to DMAInfo structure for correct stream.. */

			SOUND_SPUHandleEndTransfer((int)i,(int)b,ptr);	/* Handle END part of transfer */
			return(2);	/* Transfer completed */
		}
		return(1);	/* Transfer not completed */
	}
	return(0);	/* No transfer required */
}

/**********************************************************************************
SOUND_UpdateSPUTrans

	Called by StreamADPCM.
	Updates everything for IOP->SPU transfers
	
***********************************************************************************/
void SOUND_UpdateSPUTrans(u_int size)
{
int check;
int i,b;
SPUDMA *ptr;
STREAMDATA *p2;

	DTS_MAX_SIZE=size;
	check=SOUND_CheckSPUTransfer();	/* Transfer any currently requested data to SPU */

	if (SOUND_Transfer.TranMode==SOUND_DTS_ON)		/* DTS currently ON ? */
	{
		if (check==2)	/* Transfer completed, but DTS is on..So quit, as we only want one transfer.. */
		{
			return;
		}
	}
	if (check==1)		/* Still more data to transfer? If so, quit.. */
	{
		return;
	}

	SOUND_SPUTransfer.TransFlag=0;

	for (i=0;i<UserMaxStreams;i++)	/* Loop through all streams */
	{
		if (StreamBuffer[i].Status==STREAM_STATUS_ON)	/* Is stream ON? */
		{
			p2=&StreamBuffer[i];
			ptr=&p2->DMAInfo;
			b=(int)StreamBuffer[i].DMAInfo.UsedBuffer;

			if ((ptr->TransType[b]==STREAM_AUDIO_ADPCM)||(ptr->TransType[b]==STREAM_LOAD_SPU))
			{
				if (ptr->Status[b]==1)	/* Has a request from MultiStream been made for this stream? */
				{
					check=SOUND_SPUProcessUpdate(i,b,ptr);	/* Process this stream. */
					if (check==1)	/* Started transfer, but not completed? (or trans complete in DTS mode) */
						return;
				}
				else if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)		// New! 7.22 (Thanks Steve!)
				{
/* No data requested for stream, but check to see if SPU playback position means that it should do.. */
					SOUND_SPUCheckForOutOfRange(i,ptr);
				}
			}
		}
	}

	if (SOUND_SPUTransfer.TransFlag==0)
	{
/* No streams requiring data. Handle User IOP->SPU transfer (from SOUND_TransIOPToSPU). */
		if (SOUND_UserTransfer.Status==1)
		{
			b=(int)SOUND_UserTransfer.Size;

/* DTS ON? Size larger than max DTS size? */
			if ((SOUND_Transfer.TranMode==SOUND_DTS_ON)&&(SOUND_UserTransfer.Size>DTS_MAX_SIZE))
				b=(int)DTS_MAX_SIZE;

			sceSdVoiceTransStatus(MS_SPU_DMA,SD_TRANS_STATUS_WAIT);		/* Wait for any other transfer to complete */
			sceSdVoiceTrans (MS_SPU_DMA, (SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA),	/* Transfer data to SPU */
			(unsigned char *) SOUND_UserTransfer.IOPAddress,
			SOUND_UserTransfer.SPUAddress,(unsigned int)b);
		
			SOUND_UserTransfer.IOPAddress+=(unsigned int)b;		/* Modify IOP/SPU Addresses and transfer size.  */
			SOUND_UserTransfer.SPUAddress+=(unsigned int)b;
			SOUND_UserTransfer.Size-=(unsigned int)b;
			if (SOUND_UserTransfer.Size==0)			/* All data transferred? */
			{
				SOUND_UserTransfer.Status=0;		/* Transfer completed */
#if (SCREAM_COMPATIBLE)
				SignalSema(g_SPUDMADoneSema);
#endif
			} 
		}
	}
}

/**********************************************************************************
SOUND_SPUProcessUpdate

	If a stream requires data, process its request.

	Requires:	i		Stream channel
				b		Buffer half of stream request
				*ptr	pointer to SPUDMA structure for stream

	Returns:	0		Transfer completed (and DTS is OFF)
				1		Transfer not completed (or DTS is ON)
	
***********************************************************************************/
int SOUND_SPUProcessUpdate(int i,int b, SPUDMA *ptr)
{
int check;
int check2;


	if (ptr->CurrentStat[b]==SOUND_SPU_SFX)		/* Transfer a SFX? */
	{

		check=SOUND_SPUInitSFXTransfer(i,b,ptr);	/* Init SFX transfer */
		if (check==0)								/* Transfer allowed? */
		{
			check2=SOUND_CheckSPUTransfer();		/* Transfer data to SPU RAM */

			if ((check2==2)||(check2==1))
				return(1);							/* Transfer not completed (more to transfer) */

			SOUND_SPUHandleEndTransfer(i,b,ptr);	/* Handle END of SPU transfer */
			if (SOUND_Transfer.TranMode==SOUND_DTS_ON)		/* DTS currently ON ? */
				return(1);

			return(0);
		}
	}
	else if (ptr->CurrentStat[b]==SOUND_SPU_PRELOAD)	/* Transfer ADPCM stream in PRELOAD mode? */
	{
		check=SOUND_SPUInitPreload(i,b,ptr);	/* Init Preload transfer. */
		if (check==0)		/* Transfer allowed? */
		{
			if (SOUND_CheckSPUTransfer()!=2)	/* Transfer data to SPU RAM */
				return(1);						/* Transfer not completed (more to transfer) */
			if (SOUND_Transfer.TranMode==SOUND_DTS_ON)		/* DTS currently ON ? */
				return(1);
			return(0);	/* Half Update never follows straight after a preload.. */
		}
	}

	if (ptr->CurrentStat[b]==SOUND_SPU_UPDATE_HALF_SIZE)	/* Transfer ADPCM stream (not preload) */
	{
		check=SOUND_SPUInitHalfSize(i,b,ptr);	/* Init half-buffer transfer */
		if (check==0)						 	/* Transfer allowed? */
		{
			if (SOUND_CheckSPUTransfer()!=2) 	/* Transfer data to SPU RAM */
				return(1);					 	/* Transfer not completed (more to transfer) */
			if (ptr->CurrentStat[b]==SOUND_SPU_UPDATE_HALF_REMAIN)	/* Need to transfer a remaining bit? */
			{
				if (SOUND_Transfer.TranMode==SOUND_DTS_ON)		/* DTS currently ON ? */
					return(1);	/* Don't allow any more transfers to happen now..Will take too long.. */
			}
			return(0);	/* All completed. */	
		}
	}
	if (ptr->CurrentStat[b]==SOUND_SPU_UPDATE_HALF_REMAIN)
	{
		SOUND_SPUHalfRemainTransInit(i,b,ptr);		/* Init half-buffer remainder transfer */
		if (SOUND_CheckSPUTransfer()!=2)			/* Transfer data to SPU RAM */
			return(1);								/* Transfer not completed (more to transfer) */
		if (SOUND_Transfer.TranMode==SOUND_DTS_ON)		/* DTS currently ON ? */
			return(1);	/* Don't allow any more transfers to happen now..Will take too long.. */
	}
	return(0);	/* Should never get here.. */
}

/**********************************************************************************
SOUND_SPUHandleEndTransfer
	Handles updating the stream info when a IOP->SPU transfer has been completed

	Requires:	i		Stream number
				b		Transfer buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void SOUND_SPUHandleEndTransfer(int i,int b, SPUDMA *ptr)
{

//printf("*************** SOUND_SPUHandleEndTransfer (i %x, b %x) CurrentStat = %x\n",i,b,ptr->CurrentStat[b]);

	SOUND_SPUTransfer.Status=STREAM_STATUS_OFF;	/* Bodge? */

	if (ptr->CurrentStat[b]==SOUND_SPU_SFX)		/* Just transfered a SFX? */
	{
		SOUND_SPUEndSFX(i,b,ptr);				/* Handle end of SFX transfer */
	}
	else if (ptr->CurrentStat[b]==SOUND_SPU_PRELOAD)	/* Just transfered preload data? */
	{
		SOUND_SPUPreloadEnd(i,b,ptr);		/* Handle end of preload transfer */
	}
	else if (ptr->CurrentStat[b]==SOUND_SPU_UPDATE_HALF_SIZE)	/* Just transfered buffer half SIZE */
	{
		SOUND_SPUEndHalfSize(i,b,ptr);			/* Handle end of buffer SIZE transfer */

		if (ptr->TranRemain==0)		/* No remainder to transfer from next buffer? */
		{
			SOUND_SPUFinalEndHalf(i,b,ptr);		/* End buffer half */
			SOUND_SPUTransferComplete(i,b,ptr);	/* Transfer completed. */
		}
		else
		{
			ptr->CurrentStat[b]=SOUND_SPU_UPDATE_HALF_REMAIN;	/* Request remain to be transferred */
		}
	}
	else if (ptr->CurrentStat[b]==SOUND_SPU_UPDATE_HALF_REMAIN)	/* Handle end of REMAIN transfer */
	{
		SOUND_SPUEndHalfRemain(i,b,ptr);		/* End of remain.. */

		if (ptr->FinalEnd[b]!=STREAM_SPU_SHORT_LOOP)	/* 7.22 Looping, and all data in IOP RAM */
		{
			b++;		// Feb2003 FIX! V6.2
			b&=1;		// Feb2003 FIX! V6.2
		}
		SOUND_SPUFinalEndHalf(i,b,ptr);
		SOUND_SPUTransferComplete(i,b,ptr);		/* Transfer completed. */
	}
}

/**********************************************************************************
SOUND_SPUPreloadEnd

	Called when preloading data is completed (all required data transfered to SPU RAM)

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void SOUND_SPUPreloadEnd(int i,int b, SPUDMA *ptr)
{
int check,a;
u_int size;
u_int OldIOPAddr;


	size=ptr->TranSize;		/* Size of data transfered */

	if (ptr->TempCheck==1)	/* Data is looping AND smaller than the SPU buffer */
	{
		ptr->TransCount=1;	/* Mark stream as such */
		StreamBuffer[i].DMAInfo.Status[b]=0; 	/* Current status OFF */
	}

	if (StreamBuffer[i].ParentChildFlag!=0)	/* Parent or Child stream? */
	{

//if (i<2) printf("PreloadEnd %x\n",i);
		check=0;
		StreamBuffer[i].ChldWait=1;		/* Mark as such */
		StreamBuffer[i].Chld++;			/* Increase Counter */

		for (a=0;a<UserMaxStreams;a++)	/* Loop through all streams */
		{
			if ((StreamBuffer[i].ParentStream==StreamBuffer[a].ParentStream)&&
				(StreamBuffer[i].Chld!=StreamBuffer[a].Chld))	/* Parent or Child not in step with each other.. */
			{
				StreamBuffer[i].ChldWait=1;
				check=1;
			}
		}
		if (check==0)		/* Parent and Child all in sync.. */
		{
			for (a=0;a<UserMaxStreams;a++)	/* Loop through all streams again.. */
			{
				if (StreamBuffer[i].ParentStream==StreamBuffer[a].ParentStream)	/* Parent or child for stream? */
				{


//if (i<2) printf("All Streams ready..Clear flags%x\n",a);
					StreamBuffer[a].ChldWait=0;	/* Reset these values to keep them from overflowing.. */
					StreamBuffer[a].Chld=0;
				}
			}
		}
	}


	OldIOPAddr=ptr->IOPAddress[b];
	ptr->IOPAddress[b]+=size;	/* Update IOP address */
	ptr->SizeCount+=size;		/* Update SizeCount (size of data current transfered) */
	ptr->TotalSize-=size;		/* Update TotalSize (amount of data left to transfer) */
	ptr->IOPSize[b]-=(int)size;		/* Update IOP Buffer size */

	ptr->TransCount++;			/* Transfer completed. Increase counter.. */

	if ((ptr->IOPSize[b]==0)&&(ptr->TransCount!=2))	/* All data transfered from buffer & stream is not small&looping */
	{
		StreamBuffer[i].DMAInfo.Status[b]=0;		/* Turn status off for stream */
		ptr->UsedBuffer++;							/* Swap buffer half to be checked next */
		ptr->UsedBuffer&=1;
	}
	else
	{
/* More data in IOP buffer to be transfered. This will be used for transferring in buffer half update */
		ptr->CurrentStat[b]=SOUND_SPU_UPDATE_HALF_SIZE;

		if (ptr->FinalEnd[b]==STREAM_SPU_SHORT_LOOP)	/* All data not in IOP RAM... */
		{
			if (ptr->TempCheck!=1)	/* IOP data smaller than SPU RAM buffer also! */
			{

				if (SOUND_Transfer.TranMode!=SOUND_DTS_ON)		/* DTS currently ON ? */
					sceSdVoiceTransStatus(MS_SPU_DMA,SD_TRANS_STATUS_WAIT);
				_AdpcmSetMarkClearEND(i,OldIOPAddr,size);
			}
		}

	}
}

/**********************************************************************************
SOUND_SPUEndSFX

	Called when SFX data is completed (all required data transfered to SPU RAM)

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void SOUND_SPUEndSFX(int i,int b, SPUDMA *ptr)
{
	ptr->SPUAddress+=(u_int)ptr->IOPSize[b];	/* Update SPU address */
	ptr->Status[b]=0;					/* Turn status off for transfer request */
	ptr->UsedBuffer++;
	ptr->UsedBuffer&=1;					/* Use other buffer half next time.. */
	if (StreamBuffer[i].LoopInfo==STREAM_READY_TO_DIE) 	/* Was this the last transfer for the SFX? */
	{
		StreamBuffer[i].RequestLoad=2;	/* Last transfer of SFX completed. */
	}
}

/**********************************************************************************
SOUND_SPUEndHalfRemain

	Called when HalfBuffer data is completed (using a bit of data from the next buffer)

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void SOUND_SPUEndHalfRemain(int i,int b, SPUDMA *ptr)
{
	if (ptr->ForceSwap==1)	/* Looping stream which is ALL in IOP RAM, so clear markers.. */
	{
		 if (SOUND_Transfer.TranMode!=SOUND_DTS_ON)		/* DTS currently ON ? */
			sceSdVoiceTransStatus(MS_SPU_DMA,SD_TRANS_STATUS_WAIT);

		_AdpcmSetMarkClearEND(i,ptr->IOPAddress[b],ptr->TranRemain);

		ptr->CurrentStat[0]=SOUND_SPU_UPDATE_HALF_SIZE;
	}

	ptr->TranSize=ptr->TranRemain;	/* Size of data transfered is the Remain size.. */
	ptr->ForceSwap++;	/* Not sure about this..! */
}

/**********************************************************************************
SOUND_SPUEndHalfSize

	Called when HalfBuffer transfer data is completed (using the Size)

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void SOUND_SPUEndHalfSize(int i,int b, SPUDMA *ptr)
{

int a;

// MUST BE DONE THIS WAY TO ALLOW DTS/DSP STUFF TO WORK!!

	if (SOUND_Transfer.TranMode==SOUND_DTS_ON)		/* DTS currently ON ? */
	{
		do{
			a=sceSdVoiceTransStatus(MS_SPU_DMA, SD_TRANS_STATUS_CHECK);
		}while(a==0);
	}
	else
		sceSdVoiceTransStatus(MS_SPU_DMA,SD_TRANS_STATUS_WAIT);	// Wait for any transfer to complete

	_AdpcmSetMarkClearEND(i,ptr->IOPAddress[b],ptr->TranSize);

	if (ptr->ForceSwap==1)	/* Looping stream which is ALL in IOP RAM, so clear markers.. */
	{
		_AdpcmSetMarkClearEND(i,ptr->IOPAddress[b],ptr->TranSize);
		if (ptr->TranRemain==0)
		{
			ptr->IOPAddress[0]=ptr->InitIOPAddress;
			ptr->IOPSize[0]=(int)ptr->InitIOPSize;
			ptr->SizeCount=0;
			ptr->TotalSize=ptr->InitSize;
			ptr->TranSize=0;
			ptr->ForceSwap=0;
		}
	}
}

/**********************************************************************************
SOUND_SPUInitPreload

	Initialise PRELOAD transfer

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	0		Init OK
				1		Init failed
***********************************************************************************/
int SOUND_SPUInitPreload(int i,int b, SPUDMA *ptr)
{
	if (ptr->TransCount==0)	/* Preload? But not preloaded.. */
	{

		ptr->TempCheck=0;

//if (i<2) printf("SOUND_SPUInitPreload %x\n",i);

		if (SOUND_SPUCheckParentChild(i)==1)	/* Parent or Child in sync? */
		{
			return(1);	/* No.. */
		}

/* Calculate size to transfer from this buffer, and the remainder from the next buffer */
		SOUND_SPUGetTranSizeAndRemain(b,ptr);

/* Request transfer */
//		SOUND_SetSPUTransRequest(ptr->IOPAddress[b],ptr->SPUAddress,ptr->SPUSize,(unsigned int)i);

		if (ptr->FinalEnd[b]==STREAM_SPU_READY_TO_DIE)	/* Final IOP buffer of data.. */
		{
			if (ptr->TotalSize-ptr->TranSize==0)	/* No more data after this buffer..Must be once off playback */
			{
				_AdpcmSetMarkFINAL(i,ptr->IOPAddress[b],ptr->TranSize);	/* Set FINAL marker in ADPCM data */
				StreamBuffer[i].DMAInfo.Status[b]=0;			/* Current status OFF */
			}
			else
			{
				_AdpcmSetMarkSTART(i,ptr->IOPAddress[b]);		/* Set Loop start marker */
				_AdpcmSetMarkEND(i,ptr->IOPAddress[b],ptr->TranSize);	/* Set loop end marker */
			}
		}
		else if (ptr->FinalEnd[b]!=STREAM_SPU_SHORT_LOOP)	/* All data not in IOP RAM... */
		{

//printf("END Addr = %x\n",ptr->TranSize);
//printf("SPUSize %x\n",ptr->SPUSize);
			_AdpcmSetMarkSTART(i,ptr->IOPAddress[b]);
			_AdpcmSetMarkEND(i,ptr->IOPAddress[b],ptr->TranSize);
		}
		else
		{
/* A Looping stream which is smaller than the IOP buffer */
			_AdpcmSetMarkSTART(i,ptr->IOPAddress[b]);
			_AdpcmSetMarkEND(i,ptr->IOPAddress[b],ptr->TranSize);

			if (ptr->TotalSize-ptr->TranSize==0)
			{
				ptr->TempCheck=1;	/* IOP data smaller than SPU RAM buffer also! */
			}
		}

/* Request transfer */
		SOUND_SetSPUTransRequest(ptr->IOPAddress[b],ptr->SPUAddress,ptr->SPUSize,(unsigned int)i);
		return(0);
	}
	return(1);	/* Already preloaded..Must be looping a smaller than SPU RAM buffer stream.. */
}

/**********************************************************************************
SOUND_SPUInitHalfSize

	Checks to see if more data is required for stream.
	Initialise a Half SPU RAM buffer update using SIZE. SIZE is the amount of data
	in the current IOP buffer request. REMAIN is the size of data to be used from
	the next buffer half to fill the SPU RAM buffer size..confused?! Well, so am i..

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	0		Init OK
				1		Init failed
***********************************************************************************/
int SOUND_SPUInitHalfSize(int i,int b, SPUDMA *ptr)
{
int ret;

	if (SOUND_SPUCheckParentChild(i)==1)	/* Parent and Child in sync? */
		return(1);	/* Nope.. */

	if (SOUND_SPUCheckValidPlaying(i,ptr)==1)	/* Current playback position in range of SPU RAM buffer? */
		return(1);			/* Nope..Must be playing another sample.. */

	ret=SOUND_SPUGetTranSizeAndRemain(b,ptr);		/* Calculate the Size of this buffer and Remain of next buffer */
	if (ret==-1)			// 7.2
	{
		return(1);			// 7.2
	}

	if (SOUND_SPUCheckForHalfUpdate(i,b,ptr)!=0)	/* Played half the SPU buffer? */
		return(1);		/* No.. */

	return(0);	/* Yes! */
}

/**********************************************************************************
SOUND_SPUInitSFXTransfer

	Pretty basic, this one.. Just makes a request for a IOP->SPU transfer..

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	0		Init OK
***********************************************************************************/
int SOUND_SPUInitSFXTransfer(int i,int b, SPUDMA *ptr)
{
	SOUND_SetSPUTransRequest(ptr->IOPAddress[b],ptr->SPUAddress,(u_int)ptr->IOPSize[b],(unsigned int)i);
	return(0);
}

/**********************************************************************************
SOUND_SPUCheckParentChild

	Checks to see if this current stream is part of an interleaved stream.
	If so, check if it is allowed to be updated (To keep PARENT and CHILD streams
	in sync)

	Requires:	i		Stream number

	Returns:	0		OK
				1		Failed
***********************************************************************************/
int SOUND_SPUCheckParentChild(int i)
{
int a;

//if (i<2) printf("SOUND_SPUCheckParentChild %x\n",i);
	if (StreamBuffer[i].ParentChildFlag==SOUND_PARENT)	/* Stream is a PARENT ? */
	{

//if (i<2)printf("Parent..Checking for Children\n");
		for (a=0;a<UserMaxStreams;a++)	/* Loop through all streams */
		{
			if ((StreamBuffer[i].ParentStream==StreamBuffer[a].ParentStream)&&
				(StreamBuffer[a].ChldWait==0)&&(a!=i))	/* Has a CHILD been updated yet? */
			{
/* No...So don't allow PARENT to update again yet..As this sets off a new load request */
//if (i<2) printf("Child Not Ready\n");
				return(1);
			}
		}
	}

/* Check to make sure that a CHILD stream doesn't try to get more data if any other of its */
/* PARENT or CHILD streams are still requiring data. */
   	if (StreamBuffer[i].ParentChildFlag==SOUND_CHILD)
	{
//if (i<2)printf("Child...Checking for Parent\n");
		for (a=0;a<UserMaxStreams;a++)
		{
			if ((StreamBuffer[i].ParentStream==StreamBuffer[a].ParentStream)&&
				(StreamBuffer[i].Chld>StreamBuffer[a].Chld))
			{
//if (i<2) printf("Parent Not Ready\n");
				return(1);	/* Child already updated. Don't allow it to try to update again.. */
			}
		}
	}

#if 0
if (i<2)
{
	printf("** Parent and Child are ready\n");
	for (a=0;a<UserMaxStreams;a++)
	{
		if (StreamBuffer[a].ParentStream==StreamBuffer[i].ParentStream)
		printf("Stream %x = Chld %x, ChldWait %x\n",a,StreamBuffer[a].Chld,StreamBuffer[a].ChldWait);
	}
}
#endif

	return(0);	/* Allow transfer (either Parent/Child is allowed, or stream is not a Parent/Child) */
}

/**********************************************************************************
SOUND_SPUGetTranSizeAndRemain

	Calculates the SIZE of the current buffer to transfer to SPU RAM and the REMAINder
	of the data from the next buffer required to fill SPU RAM..

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	0		OK
				-1		Failed
		
***********************************************************************************/
int SOUND_SPUGetTranSizeAndRemain(int b, SPUDMA *ptr)
{
u_int size,remain;
u_int HalfSize;
int b2;

//static int last=-1;

	size=ptr->SPUSize;	/* Size of SPU Buffer */

	HalfSize=size/2;	/* Half size of SPU Buffer.. */

	ptr->TranPos=ptr->SPUAddress+HalfSize;	/* Position to transfer data to in SPU RAM */

	if (ptr->TransCount!=0)	/* Past preload stage? */
	{
		size=HalfSize;		/* Only require half buffer sizes */
	}

//if (size!=last)
//{
//last=size;
//printf("%x,%x\n",ptr->IOPSize[b],size);
//}

	if (ptr->IOPSize[b]<size)	/* Is IOP data smaller than SPU buffer? */
	{

//printf("HERE??\n");
		remain=size-ptr->IOPSize[b];	/* Yes..remaining data size required */
		size=(u_int)ptr->IOPSize[b];		/* size of current buffer */

		if(ptr->FinalEnd[b]!=STREAM_SPU_SHORT_LOOP)	/* But are we looping? */
		{
			remain=0;		// No..So no remainder from next buffer required.. 7.22
		}
	}
	else
	{
		remain=0;	/* No..remainder required. Data in IOP buffer is larger than SPU RAM buffer */
	}
	if (ptr->FinalEnd[b]==STREAM_SPU_READY_TO_DIE)	/* Final IOP buffer of data.. */
	{
		remain=0;
	}
	b2=1-b;

	if(ptr->FinalEnd[b]!=STREAM_SPU_SHORT_LOOP)	/* Are we looping a stream smaller than the IOP buffer? 7.22 */
	{ 
		if (ptr->IOPSize[b2]<remain)
		{
			if (ptr->Status[b2]==0)		// 7.2
			{
				return(-1);
			}
			remain=(u_int)ptr->IOPSize[b2];
		}
	}

	ptr->TranSize=size;

	ptr->TranRemain=remain;
	ptr->TranHalfSize=HalfSize;
	return(0);
}

/**********************************************************************************
SOUND_SPUCheckValidPlaying

	Checks to see if the current stream is playing audio from a good area of SPU RAM..
	Or actually playing at all..
	Preloading doesn't need to check this, but updating the buffer half does..

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	0		Valid
				1		Invalid
		
***********************************************************************************/
int SOUND_SPUCheckValidPlaying(int i, SPUDMA *ptr)
{
u_int StreamAddress;

	StreamAddress=SOUND_SPUGetStreamAddress(i);	/* Current SPU RAM playback address */

	if (ptr->TransCount==2)
	{
		return(1);	/* A looping stream which is smaller than the SPU buffer.. */
	}

/* Is playback of SPU in stream buffer? If not, its either not keyed on yet, or playing another */
/* part of SPU RAM.. */

	else if ((StreamAddress<ptr->SPUAddress)||(StreamAddress>ptr->SPUAddress+ptr->SPUSize))
	{
		return(1);	/* Playback outside of stream buffer area..Must be playing another sample.. */
	}
	else if (StreamBuffer[i].iStatus!=iSTATUS_PLAYING)
	{
		return(1);	/* Stream not playing yet (probably in preload mode..) */
	}
	return(0);
}

/**********************************************************************************
SOUND_SPUGetStreamAddress

	Returns the current SPU RAM playback address for a given stream..

	Requires:	i		Stream number

	Returns:	SPU RAM addresss
***********************************************************************************/
u_int SOUND_SPUGetStreamAddress(int i)
{
u_int ch,core;

	ch=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel);	/* SPU Channel */
	core=(unsigned int)SOUND_GetCore(StreamBuffer[i].SPUChannel);		/* SPU Core */
	return(sceSdGetAddr((unsigned short)(core|(ch<<1)|SD_VA_NAX)));	/* Current SPU RAM playback address */
}

/**********************************************************************************
SOUND_SPUCheckForOutOfRange

	If a stream hasn't requested any data, this routine checks to see if it should have
	done..Basically, if your buffers are too small, it is possible that the system
	wont pick up that you need data fast enough, and a current buffer half will just
	loop constantly. This sorts the problem out..

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void SOUND_SPUCheckForOutOfRange(int i,SPUDMA *ptr)
{
u_int StreamAddress;


	if (SOUND_SPUCheckValidPlaying(i,ptr)==1)	/* Should stream be allowed to update yet? */
		return;	/* No.. */

	if (IOPRequestLoad==i)	/* Request already being made for stream? */
		return;				/* Yes..Don't do another.. */
	if (StreamBuffer[i].RequestLoad==1)
		return;

	StreamAddress=SOUND_SPUGetStreamAddress(i);	/* Get current SPU RAM playback address */


	if ((ptr->PlayHalf==STREAM_FIRST_HALF)&&(StreamAddress>ptr->TranPos))	/* Playing other buffer half? */
	{
		StreamBuffer[i].RequestLoad=1;	/* Yes..make a request.. */
	}
	else if ((ptr->PlayHalf==STREAM_SECOND_HALF)&&(StreamAddress<ptr->TranPos))
	{
		StreamBuffer[i].RequestLoad=1;
	}
}

/**********************************************************************************
SOUND_SPUCheckForHalfUpdate

	Checks to see if half of the buffer has been played, and requests more data if
	necessary..

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	0		More data requested
				1		No data requested
		
***********************************************************************************/
int SOUND_SPUCheckForHalfUpdate(int i,int b, SPUDMA *ptr)
{
u_int PlayHalf;
u_int StreamAddress;


//	if (i==1) printf("SOUND_SPUCheckForHalfUpdate for %d, %d, ptr->PlayHalf: %d\n", i, b, ptr->PlayHalf);

	if (IOPRequestLoad==i)	/* Request already being made for stream? */
		return(1);				/* Yes..Don't do another.. */

	PlayHalf=ptr->PlayHalf;
	StreamAddress=SOUND_SPUGetStreamAddress(i);		/* Current SPU RAM address */

	if ((PlayHalf==STREAM_FIRST_HALF)&&(StreamAddress>ptr->TranPos))
	{
//		if (i==1) printf("[2] SOUND_SPUCheckForHalfUpdate %d, %d\n", StreamAddress, ptr->TranPos);

		StreamBuffer[i].Chld++;
		SOUND_SPUHalfSizeTransInit(i,b,ptr);	/* Initalise Half buffer transfer */
		return(0);
	}
	else if ((PlayHalf==STREAM_SECOND_HALF)&&(StreamAddress<ptr->TranPos))
	{
//		if (i==1) printf("[3] SOUND_SPUCheckForHalfUpdate %d, %d\n", StreamAddress, ptr->TranPos);

		StreamBuffer[i].Chld++;
		SOUND_SPUHalfSizeTransInit(i,b,ptr);	/* Initialise Half buffer transfer */
		return(0);
	}

//	if (i==1) printf("[4] SOUND_SPUCheckForHalfUpdate %d, %d\n", StreamAddress, ptr->TranPos);
	return(1);	/* Half buffer of data not played yet.. */
}

/**********************************************************************************
SOUND_SPUTransferComplete

	Called when both the SIZE and REMAIN of the SPU Buffer transfer have been completed

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void SOUND_SPUTransferComplete(int i,int b, SPUDMA *ptr)
{
int b2;
u_int z;

	b2=b+1;
	b2&=1;		/* b2 = other buffer half */

	z=ptr->PlayHalf+1;
	z&=1;
	ptr->PlayHalf=z;		/* Swap current play half. */


	if (ptr->ForceSwap==1)	/* Crossed over to other IOP buffer half (Remain)? */
	{
		b=b2;				/* Now using other half of data. */
	}

	z=ptr->PacketSize[b];	/* z = Total requested size of data for IOP->SPU transfer */


/* Have we transferred a full half IOP buffer (or more, to fill the SPU RAM buffer)? */
	if ((z > 0) && (ptr->SizeCount>=z))
	{
		if (StreamBuffer[i].NoMoreLoading==0)
		{
			StreamBuffer[i].RequestLoad=1;
			ptr->SizeCount%=z;	/* Leaves remainder.. */
		}
	}
	if ((int)(ptr->IOPSize[b])<0) /* Not needed...probably... */
	{
		ptr->IOPSize[b]=0;
	}

	if ((ptr->IOPSize[b]==0)||(ptr->ForceSwap==1))	/* Data packet all read into SPU RAM */
	{
		if (ptr->ForceSwap!=1)	/* Not reading from new buffer half? */
			StreamBuffer[i].DMAInfo.Status[b]=0;	/* Current status OFF for old buffer half. */
		ptr->UsedBuffer++;		/* Swap to other buffer for next check.. */
		ptr->UsedBuffer&=1;
	}
	ptr->ForceSwap=0;
}

/**********************************************************************************
SOUND_SPUHalfSizeTransInit

	Initialises transfer of data to fill half of the SPU RAM buffer with more data.
	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void SOUND_SPUHalfSizeTransInit(int i,int b,SPUDMA *ptr)
{
u_int PlayOffset;
u_int size,remain;

	size=ptr->TranSize;
	remain=ptr->TranRemain;
	PlayOffset=ptr->PlayHalf*(ptr->SPUSize/2);	/* Offset into SPU RAM for transferring data to.. */

	ptr->ForceSwap=0;	/* Flag to say if we've gone over to the other buffer for more data.. */

	_AdpcmSetMarkSTART(i,ptr->IOPAddress[b]);		/* Set a LOOP start marker in the ADPCM data.. */

	if (remain==0)	/* Full buffer of data? */
	{
		_AdpcmSetMarkEND(i,ptr->IOPAddress[b],size);	/* Store end of loop marker */
	}

	if (ptr->TotalSize<=(ptr->SPUSize/2))	/* Final packet to transfer? */
	{
		if (ptr->FinalEnd[b]==STREAM_SPU_READY_TO_DIE)	/* Onceoff playback? */
		{
			if (size<0x20)
				size=0x20;

			_AdpcmSetMarkFINAL(i,ptr->IOPAddress[b],size);	/* Set FINAL marker in ADPCM data */
			remain=0;
		}
		else if(ptr->FinalEnd[b]==STREAM_SPU_SHORT_LOOP)	/* Looping, and all data in IOP RAM */
		{
			ptr->ForceSwap=1;	/* Set flag.. */
		}
	}

/* Request a transfer.. */
	SOUND_SetSPUTransRequest(ptr->IOPAddress[b],ptr->SPUAddress+PlayOffset,size,(unsigned int)i);
}

/**********************************************************************************
SOUND_SPUHalfRemainTransInit

	Initialise transfer to fill rest of SPU RAM buffer with Remain (data from other buffer)

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void SOUND_SPUHalfRemainTransInit(int i,int b,SPUDMA *ptr)
{
u_int PlayOffset;
u_int size,remain;

	size=ptr->TranSize;
	remain=ptr->TranRemain;
	PlayOffset=ptr->PlayHalf*(ptr->SPUSize/2);	/* Offset into SPU RAM for transferring data to.. */

	if (remain!=0)	/* Is there any remainder? */
	{
		ptr->SizeCount+=size;	/* Modify SizeCount */
		ptr->TotalSize-=size;	/* Modify TotalSize */
		if (ptr->ForceSwap==0)	/* All stream data is not in IOP RAM.. */
		{
			StreamBuffer[i].DMAInfo.Status[b]=0;	/* Turn status OFF */
			ptr->IOPSize[b]=0;
			b++;
			b&=1;	/* swap to other buffer for more data */
		}
		else
		{
			ptr->IOPAddress[0]=ptr->InitIOPAddress;	/* All data in IOP RAM for stream */
			ptr->IOPSize[0]=(int)ptr->InitIOPSize;
			ptr->SizeCount=0;
			ptr->TotalSize=ptr->InitSize;
		}
		_AdpcmSetMarkEND(i,ptr->IOPAddress[b],remain);	/* Store Loop END marker in SPU RAM */

/* Request a new transfer.. */
		SOUND_SetSPUTransRequest(ptr->IOPAddress[b],ptr->SPUAddress+size+PlayOffset,remain,(unsigned int)i);
	}
	else
	{
	}
}


/**********************************************************************************
WaitForValidSPUAddress (7.2)
	Some people have reported MIB's going out of sync when there seems to be a lot
	of CD errors.
	I can only assume that the problem is caused by the fact that the playback of the
	SPU buffer is very close to the end of the SPU buffer. So, what is happening is:

	a)	SPU playback is close to end of current SPU buffer half
	b)	Set LSAX for first SPU channel to point at new SPU buffer
	c)	First SPU channel loops to new SPU buffer
	d)	(Possible interrupt from RPC or other thread delays setting second LSAX now)
	e)	In the mean time, the playback of the second SPU channel has looped back to the
		start of its current buffer..
	d)	Set LSAX for second SPU channel - too late. Its now playing the same buffer half
		again, but the first SPU channel is playing new data.


	The fix:

	This routine will make sure there's enough playback data for the SPU channel before
	setting all of the LSAX's for the SPU channels.
	If there is not enough time, we wait until there is before returning.
	We also Suspend and Resume interrupts around this part to make sure that we get
	valid results.


	Requires:	i		Stream number
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void WaitForValidSPUAddress(int i, SPUDMA *ptr)
{
u_int addr=0;
u_int bufsize;
u_int oldstat;
int rt=20;

    CpuSuspendIntr (&oldstat);	/* No..Suspend interupt around LSAX setting */
	do{
		addr=SOUND_SPUGetStreamAddress(i);
		if (ptr->PlayHalf==STREAM_FIRST_HALF)
			addr=addr-ptr->TranPos;		// Playback offset through 2nd buffer half
		else
			addr=addr-ptr->SPUAddress;	// Playback offset through 1st buffer half
		bufsize=ptr->TranPos-ptr->SPUAddress;	// Size of buffer half
		bufsize-=addr;					// Amount of data left to play in buffer half
		if (bufsize<256)	// Less that 256 sample left to play before looping?
		{
		    CpuResumeIntr (oldstat);	/* Resume interrupts after LSAX setting */
			DelayThread(TWO_T*10);	/* Wait twenty ticks.. */
		    CpuSuspendIntr (&oldstat);	/* Suspend interupts again */
			rt--;		// Decrease try counter..Eventually we'll just give up and let it continue...
		}
		else
		{
			rt=0;
		}
	}while(rt!=0);
    CpuResumeIntr (oldstat);	/* Resume interrupts after LSAX setting */

}

/**********************************************************************************
SOUND_SPUFinalEndHalf

	Both SIZE and REMAIN transfer is completed...
	Set the LSAX (loop start address) to point at the buffer which we have just transferred..
	But, only do this if all PARENT and CHILD streams have been updated (if playing MIB files)

	Requires:	i		Stream number
				b		Buffer half
				ptr		Pointer to SPUDMA structure

	Returns:	Nothing
***********************************************************************************/
void SOUND_SPUFinalEndHalf(int i,int b, SPUDMA *ptr)
{
int a,j;
int check=0;
u_int ch,core;
u_int ch2,core2;
u_int PlayOffset;
u_int oldstat=0;

//if (i<2) printf("FinalEndHalf %x\n",i);
	PlayOffset=ptr->PlayHalf*(ptr->SPUSize/2);	/* Offset into SPU RAM for transferring data to.. */

	if (StreamBuffer[i].ParentChildFlag!=0)	/* Is this a PARENT or CHILD stream? */
	{
		for (a=0;a<UserMaxStreams;a++)	/* Loop through all streams.. */
		{
			if ((StreamBuffer[i].ParentStream==StreamBuffer[a].ParentStream)&&
				(StreamBuffer[i].Chld!=StreamBuffer[a].Chld))	/* In sync? */
			{

//if (i<2) printf("Other Stream (%x) not in sync with %x\n",a,i);
				StreamBuffer[i].ChldWait=1;	/* Not in sync.. */
				check=1;
			}
		}

		if (check==0)	/* All in sync..? */
		{
			WaitForValidSPUAddress(i,ptr);		// V7.1 Possible sync fix.

		    CpuSuspendIntr (&oldstat);	/* Suspend interupt around LSAX setting */

			for (a=0;a<UserMaxStreams;a++)	/* Loop through all streams.. */
			{
				if (StreamBuffer[i].ParentStream==StreamBuffer[a].ParentStream)	/* Child or Parent? */
				{
					StreamBuffer[a].ChldWait=0;	/* Reset these..just to keep them from overflowing */
					StreamBuffer[a].Chld=0;
					ch=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[a].SPUChannel);	/* SPU Channel */
					core=(unsigned int)SOUND_GetCore(StreamBuffer[a].SPUChannel);		/* SPU Core */
					check=(int)StreamBuffer[a].DMAInfo.SPUAddress;

//if (a==1)
//{
//    printf("[1] SD_VA_LSAX for ch %d at check+PlayOffset %d\n", ch, check+PlayOffset);
//}

					sceSdSetAddr ((unsigned short)(core | ch<<1 | SD_VA_LSAX), check+PlayOffset);	/* Point to loop to other half */
					SOUND_SetCalcPlayingTime(a,check+PlayOffset,check+PlayOffset+(ptr->SPUSize/2));

					if (StreamBuffer[a].Ch2Flag!=0)
					{
						for (j=0;j<StreamBuffer[a].Ch2Flag;j++)
						{
							ch=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[a].SPUChannel2[j]);	/* SPU Channel */
							core=(unsigned int)SOUND_GetCore(StreamBuffer[a].SPUChannel2[j]);		/* SPU Core */
							sceSdSetAddr ((unsigned short)(core | ch<<1 | SD_VA_LSAX), check+PlayOffset);	/* Point to loop in other half */
						}
					}
				}
			}
		    CpuResumeIntr (oldstat);	/* Resume interrupts after LSAX setting */
		}
	}
	else	/* Non - interleaved stream.. */
	{
		ch=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel);	/* SPU Channel */
		core=(unsigned int)SOUND_GetCore(StreamBuffer[i].SPUChannel);		/* SPU Core */

//printf("Set LSAX %x\n",ptr->SPUAddress+PlayOffset);

		sceSdSetAddr ((unsigned short)(core | ch<<1 | SD_VA_LSAX), ptr->SPUAddress+PlayOffset);	/* Point to loop in other half */
		if (StreamBuffer[i].Ch2Flag!=0)
		{
			for (j=0;j<StreamBuffer[i].Ch2Flag;j++)
			{
				ch2=(unsigned int)SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel2[j]);	/* SPU Channel */
				core2=(unsigned int)SOUND_GetCore(StreamBuffer[i].SPUChannel2[j]);		/* SPU Core */

				sceSdSetAddr ((unsigned short)(core2 | ch2<<1 | SD_VA_LSAX), ptr->SPUAddress+PlayOffset);
			}
		}
		SOUND_SetCalcPlayingTime(i,ptr->SPUAddress+PlayOffset,ptr->SPUAddress+PlayOffset+(ptr->SPUSize/2));
	}


		ptr->IOPAddress[b]+=ptr->TranSize;

//printf("%x Modify IOP Address %x (%x)\n",b,ptr->TranSize,ptr->IOPAddress[b]);
//printf("Other Address %x\n",ptr->IOPAddress[1-b]);

		ptr->SizeCount+=ptr->TranSize;
		ptr->TotalSize-=ptr->TranSize;
		ptr->IOPSize[b]-=(int)ptr->TranSize;
}


#endif

void SOUND_SetCalcPlayingTime(int i, u_int low,u_int hi)
{
	if (low==0)	// Will cause everything to init to 0
	{
		StreamBuffer[i].LSAXLowAddr=0;
		hi=2*1024*1024;
		StreamBuffer[i].LSAXHiAddr=hi;
	}

	StreamBuffer[i].LSAXLowAddr_Old=StreamBuffer[i].LSAXLowAddr;
	StreamBuffer[i].LSAXHiAddr_Old=StreamBuffer[i].LSAXHiAddr;
	StreamBuffer[i].LSAXLowAddr=low;
	StreamBuffer[i].LSAXHiAddr=hi;
}


/* This code "may" be used in later versions. Depending on how stable its results are.. */
/* It calculates the total time a stream has played for. */
#if 1
void SOUND_CalcPlayingTime(void)
{
int i;
int chan,core;
u_int SPUOffset;

	for (i=0;i<UserMaxStreams;i++)
	{
		if ((StreamBuffer[i].Status==STREAM_STATUS_ON)&&
			(StreamBuffer[i].iStatus==iSTATUS_PLAYING))
		{
			if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_ADPCM)
			{
	 			chan=SOUND_GetSPUChannel(StreamBuffer[i].SPUChannel);
	 			core=SOUND_GetCore(StreamBuffer[i].SPUChannel);
	 			chan<<=1;
	 			SPUOffset=sceSdGetAddr((unsigned short)(core|chan|SD_VA_NAX));	/* Current playback pos of PARENT */

				if ((SPUOffset>StreamBuffer[i].LSAXLowAddr)&&	// Playing current buffer half?
					(SPUOffset<StreamBuffer[i].LSAXHiAddr))
				{
					if (SPUOffset<StreamBuffer[i].LastSPUAddr)	// looped around current buffer half..
					{
						continue;	/* Not playing new sample data */
					}

					StreamBuffer[i].LastSPUAddr=SPUOffset;
					if (StreamBuffer[i].Pitch!=0)
					{
						SOUND_UpdateStreamTimer(i);
					}
				}
				else if ((SPUOffset>StreamBuffer[i].LSAXLowAddr_Old)&&	// Playing last buffer half?
						(SPUOffset<StreamBuffer[i].LSAXHiAddr_Old))
				{
					if (StreamBuffer[i].Pitch!=0)
					{
						SOUND_UpdateStreamTimer(i);
					}
					StreamBuffer[i].LastSPUAddr=0;
				}
			}
			else if (StreamBuffer[i].ResidentFlag==STREAM_AUDIO_PCM)
			{
				if (PCMPause==0)
					SOUND_UpdateStreamTimer(i);
			}
			else
			{
				StreamBuffer[i].LastSPUAddr=0;		// Must be called from SOUND_PlayStreamFromSFX
			}
		}
		else
		{
			StreamBuffer[i].LastSPUAddr=0;
			StreamBuffer[i].SamplesPlayed=0;
		}
	}
}

void SOUND_UpdateStreamTimer(int i)
{
	StreamBuffer[i].SamplesPlayed++;
	if ((StreamBuffer[i].SamplesPlayed/216000)>19883)
		StreamBuffer[i].SamplesPlayed=0;	// Stream has been playing for 2.2 YEARS!!!!
}

#endif

/**********************************************************************************
SOUND_SetSwitch
SOUND_SetParam
SOUND_SetAddr
SOUND_SetCoreAttr
SOUND_BlockTrans

Wrapper commands to emulate the LibSDr library.

example:

LibSDr call to set a SPU channel pitch:
    sceSdRemote (1,rSdSetParam, core | (channel << 1) | SD_VP_PITCH, pitch);

MultiStream call:
	SOUND_SetParam(core|(channel<<1)|SD_VP_PITCH,pitch);

You will still need to include the sdmacro.h file in your EE code, so the EE knows what
SD_VP_PITCH and such other defines equal.

***********************************************************************************/
void SOUND_SetSwitch(u_short coreANDcommand, u_int bitmask)
{
#if SDR_DEBUG
	if(SOUND_CheckSdrOutOfRange((u_short)(coreANDcommand>>8),SD_S_PMON>>8,SD_S_VMIXER>>8)==1)
	{
		MSERROR(("SOUND_SetSwitch: command type not valid\n"));
		return;
	}
#endif
	sceSdSetSwitch(coreANDcommand,bitmask);
}

void SOUND_SetParam(u_short coreANDcommandANDchannel,u_short value)
{
#if SDR_DEBUG
	if (SOUND_CheckSdrOutOfRange((u_short)(coreANDcommandANDchannel>>8),0,SD_P_MVOLXR>>8)==1)
	{
		MSERROR(("SOUND_SetParam: command type not valid\n"));
		return;
	}
#endif
	sceSdSetParam (coreANDcommandANDchannel,value);
}

void SOUND_SetAddr(u_short coreANDcommandANDchannel,u_int address)
{
#if SDR_DEBUG
	if (SOUND_CheckSdrOutOfRange((u_short)(coreANDcommandANDchannel>>8),SD_A_ESA>>8,SD_VA_NAX>>8)==1)
	{
		MSERROR(("SOUND_SetAddr: command type not valid\n"));
		return;
	}
#endif
	sceSdSetAddr (coreANDcommandANDchannel,address);
}

void SOUND_SetCoreAttr(u_short coreANDcommand,u_short value)
{
#if SDR_DEBUG
	if(SOUND_CheckSdrOutOfRange((u_short)(coreANDcommand>>1),0,SD_C_SPDIF_MODE)==1)
	{
		MSERROR(("SOUND_SetCoreAttr: command type not valid\n"));
		return;
	}
#endif
	sceSdSetCoreAttr(coreANDcommand,value);
}

void SOUND_BlockTrans(u_short DMAChan,u_short Flags, u_int IOPAddr,u_int Size,u_int Offset)
{
	sceSdBlockTrans((short)DMAChan,Flags,(u_char*)IOPAddr,Size,Offset);
}

/**********************************************************************************
SOUND_CheckSdrOutOfRange
	Checks to make sure that you are passing the correct command type for the correct
	routine.
	One of the main problems developers have had when using LibSdr is when they pass
	say, rSdSetParam when they want to set an address (so they should have passed
	rSdSetAddr instead). Currently, the LibSdr libraries do not pick up on this as
	an error, and many weeks can be wasted in trying to track down such a problem!

	To turn this function on, set SDR_DEBUG to 1 (at the top of this file)
	To turn this function off, set SDR_DEBUG to 0

	Requires:	com			command (such as SD_P_PITCH)
				low			lowest command value for the required type (param,switch,address..)
				high		highest command value for the required type

	Returns:	0			command is valid
				1			command is not valid
***********************************************************************************/
int SOUND_CheckSdrOutOfRange(u_short com,u_int low,u_int high)
{
#if SDR_DEBUG
	com&=0xff;
	high&=0xff;
	low&=0xff;
	if ((com<low)||(com>high))
		return(1);
#endif
	return(0);
}

/**********************************************************************************
SOUND_SmoothFadeOff
	Turn smooth fading off for a given SPU channel

	Requires:	i		SPU channel (0-47)
	Returns:	Nothing
***********************************************************************************/
void SOUND_SmoothFadeOff(int i)
{
	SOUND_SmoothFade[i][0].Status=0;
	SOUND_SmoothFade[i][1].Status=0;
}

/**********************************************************************************
SOUND_InitSmoothFade
	Turns smooth fading off for all 48 SPU channels
	Requires:	Nothing
	Returns:	Nothing
***********************************************************************************/
void SOUND_InitSmoothFade(void)
{
int i;
	for (i=0;i<48;i++)
	{
#if (SCREAM_COMPATIBLE)
		if(CHANNEL_FLAG_SET(i,g_MSChannelUseMask))
			SOUND_SmoothFadeOff(i);
#else
		SOUND_SmoothFadeOff(i);
#endif
	}
}

/**********************************************************************************
SOUND_UpdateSmoothVols
	Called each frame. Updates any SPU channels which require smooth volume changes.
	Smooth volume changes are taken care of by using the SPU's ability to automatically
	ramp volumes up or down at a given rate. The only problem here is that the destination
	volume is either full or 0. Therefore, we have to calculate the distance between
	the current and destination volumes, and then calculate which volume ramp we want
	to use to get there as fast as possible, while lowering the risk of overshooting.

	Requires:	Nothing
	Returns:	Nothing
***********************************************************************************/
void SOUND_UpdateSmoothVols(void)
{
int i;

	for (i=0;i<48;i++)
	{
    	if (SPU_KeyStatus[i]==SPU_KEY_STATUS_OFF)
		{
			SOUND_SmoothFade[i][0].Status=0;
			SOUND_SmoothFade[i][1].Status=0;
		}

		if (SOUND_SmoothFade[i][0].Status==1)
				SOUND_ModifySmoothVol(i,0);		/* update left */
		if (SOUND_SmoothFade[i][1].Status==1)
			SOUND_ModifySmoothVol(i,1);		/* update right */
	}
}

/**********************************************************************************
SOUND_SetChannelVolumeSmooth

	Sets a target volume (both left and right) for a given SPU channel
	The routine: SOUND_UpdateSmoothVols (which is called once per frame) will then
	handle fading from the current volume to this value as fast as possible.
	The system can handle both positive and negative volume settings.

	Requires:	i		SPU channel (0-47)
				voll	Left volume
				volr	Right volume

	Notes:
		Calling SOUND_SetChannelVolume will overwrite this function.
***********************************************************************************/
void SOUND_SetChannelVolumeSmooth(int i,int voll,int volr)
{
int l,r;
int RevPhaseL,RevPhaseR;
int FadeDirL,FadeDirR;
int core,spu;


/* This part handles any event of pausing/resuming channels. In which case, the final volume */
/* will be used straight away.. */
	SPU_VolL[i]=(short)voll;
	SPU_VolR[i]=(short)volr;

	for (l=0;l<UserMaxStreams;l++)
	{
		if ((StreamBuffer[l].Status==STREAM_STATUS_ON)&&(i==StreamBuffer[l].SPUChannel))
		{
			StreamBuffer[l].VolL=(short)voll;
			StreamBuffer[l].VolR=(short)volr;
		}
	}

/* Now this part handles setting up the smooth volume code.. */

	if (voll>=0x4000)
		RevPhaseL=1;
	else
		RevPhaseL=0;
	if (volr>=0x4000)
		RevPhaseR=1;
	else
		RevPhaseR=0;

	core=i/24;
	spu=i%24;
	l=sceSdGetParam((u_short)(core|(spu<<1)|SD_VP_VOLXL))/2;
	r=sceSdGetParam((u_short)(core|(spu<<1)|SD_VP_VOLXR))/2;

	if (voll>l)	/* Requested volume is greated that current volume? */
		FadeDirL=1;
	else
		FadeDirL=-1;
	if (volr>r)
		FadeDirR=1;
	else
		FadeDirR=-1;

	SOUND_SmoothFade[i][0].RevPhase=(u_char)RevPhaseL;
	SOUND_SmoothFade[i][1].RevPhase=(u_char)RevPhaseR;
	SOUND_SmoothFade[i][0].FadeDir=(u_char)FadeDirL;
	SOUND_SmoothFade[i][1].FadeDir=(u_char)FadeDirR;
	SOUND_SmoothFade[i][0].DestVol=voll;
	SOUND_SmoothFade[i][1].DestVol=volr;
	SOUND_SmoothFade[i][0].Status=1;
	SOUND_SmoothFade[i][1].Status=1;
}

/**********************************************************************************
SOUND_ModifySmoothVol
	Handles fading from the current to destination value.

	Requires:	i		SPU channel (0-47)
				flag	left (0) or right (1) data to be processed for channel
***********************************************************************************/
int SOUND_ModifySmoothVol(int i,int flag)
{
int cv;
int rv;
int cd;
int offset;
int dir;
int currentvol;
int fd;

int spu,core;

int ReadParam,WriteParam;
int FadeDir,RevPhase,ReqVol;

	spu=i%24;
	core=i/24;
	if (flag==0)
	{
		ReadParam=SD_VP_VOLXL;
		WriteParam=SD_VP_VOLL;
	}
	else
	{
		ReadParam=SD_VP_VOLXR;
		WriteParam=SD_VP_VOLR;
	}

	FadeDir=SOUND_SmoothFade[i][flag].FadeDir;
	RevPhase=SOUND_SmoothFade[i][flag].RevPhase;
	ReqVol=SOUND_SmoothFade[i][flag].DestVol;

	fd=FadeDir;

	rv=ReqVol*2;	/*(VOLXL = 0->0x7fff) */
    currentvol=sceSdGetParam((u_short)(core| (spu<<1) |ReadParam));

	if (RevPhase==1)	/* Destination is a negative volume? */
	{
		RevPhase=2;
		if (currentvol<0x8000)	/* Current volume is positive? */
		{
			sceSdSetParam((u_short)(core|(spu<<1)|WriteParam),0xa000);
			SOUND_SmoothFade[i][flag].FadeDir=(u_char)-FadeDir;
			SOUND_SmoothFade[i][flag].RevPhase=(u_char)RevPhase;
			return(0);	/* Next time around, we fade from 0 > -n */
		}
	}
	else if (RevPhase==0)
	{
		if (currentvol>=0x8000)
		{
			sceSdSetParam((u_short)(core|(spu<<1)|WriteParam),0xb000);
			FadeDir=-FadeDir;
			SOUND_SmoothFade[i][flag].FadeDir=(u_char)-FadeDir;
			SOUND_SmoothFade[i][flag].RevPhase=(u_char)RevPhase;
			return(0);	/* Next time around, we fade from 0 > -n */
		}
	}

	cv=currentvol;
	if (RevPhase==2)
	{
		if (cv==0)
			cv=0xffff;
	}


	cd=msabs(cv-rv);
	if ((rv<cv)&&(fd==1))
	{
		cd=0;
	}
	if ((rv>cv)&&(fd==-1))
	{
		cd=0;
	}

	if (cd<=0x400)	/* difference is close enough? */
	{
		sceSdSetParam((u_short)(core|(spu<<1)|WriteParam),(u_short)ReqVol);
		SOUND_SmoothFade[i][flag].Status=0;
		return(1);	/* done.. */
	}

	if (rv>cv)		/* Required Volume > Current Volume */
	{
		dir=0;
	}
	else
	{
		dir=1;
	}
	offset=SOUND_SmoothFindClosest(cd);

	if (RevPhase==0)
	{
		if (dir==0)
			offset|=0x8000;
		else
			offset|=0xa000;
	}
	else
	{
		if (cv>rv)
		{
			offset&=0x0fff;
			offset|=0x9000;
		}
		else if (cv<rv)
		{
			offset&=0x0fff;
			offset|=0xb000;
		}
	}
  	sceSdSetParam((u_short)(core|spu<<1|WriteParam),(u_short)offset);
	return(0);
}

/**********************************************************************************
SOUND_SmoothFindClosest
	Calculates a good value to use for ramping the volume up or down in the fastest
	way over one frame to reach its required destination (or anywhere close to it..)

	Requires:	val		ABS Difference between current and required volume *2
***********************************************************************************/
int SOUND_SmoothFindClosest(int val)
{
int i;
int v2;

	for (i=0;i<4;i++)
	{
		v2=val/2;
		v2&=(0x2000>>i);
		if (v2!=0)
			break;
	}
	val=i*4;
	val+=0x23;
	return(val);
}

/**********************************************************************************
SOUND_SetMultiChannelPitch
	Sets n SPU channels pitches as fast as possible. This helps to cut down on
	possible sync problems when setting SPU channel pitches.
	
	Requires:	Pitch		SPU pitch
				core0		Bitmask for SPU channels 0-23 to effect
				core1		Bitmask for SPU channels 24-47 to effect

	Returns:	Nothing
***********************************************************************************/
void SOUND_SetMultiChannelPitch(u_short Pitch,u_int core0,u_int core1)
{
u_char spuchan[48];
u_char core[48];
u_int counter=0;
int c=0;
int v;
u_char *ptr1;
u_char *ptr2;

	ptr1=&spuchan[0];
	ptr2=&core[0];
	for (c=0;c<24;c++)
	{
		v=(int)(core0&(1<<c));
		if (v!=0)
		{
			SPU_PausePitch[c]=(unsigned short)Pitch;
			*ptr1++=(u_char)(c<<1);
			*ptr2++=0;
			counter++;
		}
		v=(int)core1&(1<<c);
		if (v!=0)
		{
			SPU_PausePitch[c+24]=(unsigned short)Pitch;
			*ptr1++=(u_char)(c<<1);
			*ptr2++=1;
			counter++;
		}
	}

	ptr1=&spuchan[0];
	ptr2=&core[0];
	for (c=0;c<counter;c++)
	{
		sceSdSetParam ((unsigned short)(*ptr2++ | *ptr1++ | SD_VP_PITCH), Pitch);
	}
}

/**********************************************************************************
SOUND_SetThreadPriority
	Allows user to change the IOP thread priority of MultiStream.

	Requires:	priority		0 (highest) -> 126 (lowest)
								Normal usage is 9 -> 123

	Returns:	Nothing

	Notes: Please see the IOP documentation on IOP thread management functions for more information
***********************************************************************************/
void SOUND_SetThreadPriority(int priority)
{
	if(gThid!=0)
		ChangeThreadPriority(gThid,priority);
	else
	{
		MSERROR((">>>>>Error: SOUND_SetThreadPriority\n"));
		MSERROR(("Thread not initialised.\n"));
		MSERROR(("SOUND_InitStreamData must be called first!\n"));
	}
}

/**********************************************************************************
SOUND_SetMIBEndOffset

	Sets the offset of the final data packet in an interleaved data file.

	Requires:	i			Stream channel
				Offset		Offset through final .MIB data packet where actual data ends
							and zero padding starts.

	Returns:	Nothing

	Note:	The offset can be found in the .MIH file, and can be found from calling
			SOUND_GetMIHInfo, and checking the buffer->MIHEndOffset.
			If this routine is not called, the IOP code will manualy search through
			the last data packet loaded for the end of data. Problems can occure here
			if:
			(a) The files used to create the MIB file are of different lengths
			(b) The files used have long periods of 0's at the end of their data
				(maybe caused by fading out a song)

			This function must be called after SOUND_SetStreamParent_Int.
			The stream channel passed can then be either the parent or any of its
			child streams

***********************************************************************************/
void SOUND_SetMIBEndOffset(int i, u_int Offset)
{
int j;
	j=StreamBuffer[i].ParentStream;
	if (j==-1)
	{
		MSERROR(("SOUND_SetMIBEndOffset\n"));
		MSERROR(("Stream %d is neither a PARENT or a CHILD\n",i));
		MSERROR(("This function must be called after a SOUND_SetStreamParent_Int command\n"));
		return;
	}
	StreamBuffer[j].MIBEndOffset=Offset;
}

/**********************************************************************************
SOUND_SetEffectBufferAddress

	Sets the end address of an effect unit

	Care must be taken to make sure that your reverb buffers are not wiping over any SPU RAM
	which is required for either ADPCM streams or SFX.

	Addresses must be 128K aligned, -1.

	As default, MultiStream uses 0x1fffff (the top of SPU RAM) and 0x1dffff (128K below this)

***********************************************************************************/
void SOUND_SetEffectBufferAddress(u_int address1,u_int address2)
{
int i;

	for (i=0;i<2;i++)	/* Check to make sure that reverb address is not already in use.. */
	{
		if (CurrentReverb[i]!=0)		/* Core using reverb? */
		{
			SOUND_DisableEffects(i);	/* Reverb active at this SPU RAM address. Stop Reverb. */
		}
	}
	SOUND_ReverbAddress[0]=address1;	/* Modify address */
	SOUND_ReverbAddress[1]=address2;	/* Modify address */
}

/**********************************************************************************
SOUND_SetStreamPriority

	Sets the stream priority. The lower the priority, the higher the priority.
	For multiple streams with the same priority, a round-robin system is used (ie. each
	time MultiStream checks the stream status, it starts by checking the next one in
	the list..)

	Requires:	i			Stream channel
				priority	0-255 (0 = highest priority, 255 = lowest)
							SOUND_STREAM_PRIORITY_DEFAULT	= Use default setting.


	Notes:

	As default, the following priorities are used:

	Currently active audio streams = 64
	Currently active data streams = 192
	Streams requested to load (via a SOUND_PlayStream or SOUND_LoadFile call) = 128

	Using this method, if you, say, request streams to play over and over, any currently
	playing audio will still update correctly.

	Care must be taken with setting priorities for data streams. If you set them with
	higher priorities than audio streams, the audio streams will not be updated until
	the data stream has completely loaded (ie. the data stream dies)
***********************************************************************************/
void SOUND_SetStreamPriority(u_int i,u_int priority)
{
int a;

	if (StreamBuffer[i].Status!=STREAM_STATUS_ON)	/* Is stream playing? */
	{
		MSERROR(("SOUND_SetStreamPriority\n"));		/* No.. */
		MSERROR(("Stream %d not playing\n",i));
		MSERROR(("Can not set priority\n"));
		return;
	}


/* Check if a CHILD. If so, modify PARENT priority. */

	if (StreamBuffer[i].ParentChildFlag==SOUND_CHILD)
	{
		i=(u_int)StreamBuffer[i].ParentStream;
		if (StreamBuffer[i].Status!=STREAM_STATUS_ON)	/* Is stream playing? */
		{
			MSERROR(("SOUND_SetStreamPriority\n"));		/* No.. */
			MSERROR(("Stream %d is a CHILD\n",i));
			MSERROR(("PARENT Stream %d not playing\n",StreamBuffer[i].ParentStream));
			MSERROR(("Can not set priority\n"));
			return;
		}
	}


	if (priority==SOUND_STREAM_PRIORITY_DEFAULT)	/* Use Default? */
	{
		if (StreamBuffer[i].iStatus==iSTATUS_INITIALISED)
			priority=SOUND_PRIORITY_INIT;
		else if ((StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_ADPCM)&&(StreamBuffer[i].ResidentFlag!=STREAM_AUDIO_PCM))		/* Not an audio stream? */
			priority=SOUND_PRIORITY_DATA;
		else
			priority=SOUND_PRIORITY_AUDIO;

		StreamBuffer[i].PriorityRequest=0;
	}
	else
		StreamBuffer[i].PriorityRequest=1;

	StreamBuffer[i].Priority=(u_char)priority;
	StreamBuffer[i].Priority2=0xffffffff;

/* Check if a PARENT. If so, modify all CHILD priorities. */

	if (StreamBuffer[i].ParentChildFlag==SOUND_PARENT)
	{
		for (a=0;a<UserMaxStreams;a++)
		{
			if (StreamBuffer[i].ParentStream==StreamBuffer[a].ParentStream)
			{

				StreamBuffer[a].Priority=(u_char)priority;
				StreamBuffer[a].Priority2=0xffffffff;
			}
		}
	}
}

static u_char SOUND_StreamPriorityPrimary[48];
static u_char SOUND_StreamPriorityNum[48];
/**********************************************************************************
SOUND_SortStreamPriorities
	Sorts the stream priorities in order of preference.
	Called by StreamADPCM before updating streams.

	Requires:	Nothing
	Returns:	Number of active streams

***********************************************************************************/
int SOUND_SortStreamPriorities(void)
{
int i,j;
u_char p,p2;
int current=0;
int bubsort;


	for (i=0;i<UserMaxStreams;i++)	/* Copy all priorities into list. */
	{
		if (StreamBuffer[i].Status==STREAM_STATUS_ON)
		{

			p=StreamBuffer[i].Priority;
			SOUND_StreamPriorityPrimary[current]=p;
			SOUND_StreamPriorityNum[current]=(u_char)i;


			if (StreamBuffer[i].Priority2!=0)
				StreamBuffer[i].Priority2--;

			current++;
		}
	}

	if (current==0)
	{
		return(0);		/* No streams are playing / loading.. */
	}

	do{
		bubsort=0;
		for (j=0;j<current-1;j++)
		{
			p=SOUND_StreamPriorityPrimary[j];
			p2=SOUND_StreamPriorityPrimary[j+1];
			if (p>p2)	/* Incorrect order? */
			{
				SOUND_StreamPriorityPrimary[j]=p2;	/* Swap priorities.. */
				SOUND_StreamPriorityPrimary[j+1]=p;

				p=SOUND_StreamPriorityNum[j];
				p2=SOUND_StreamPriorityNum[j+1];
				SOUND_StreamPriorityNum[j]=p2;	/* Swap streams.. */
				SOUND_StreamPriorityNum[j+1]=p;
				bubsort=1;
			}
		}
	}while(bubsort==1);

	return(current);
}

/**********************************************************************************
SOUND_GetNextStreamPriority

	Returns the stream channel which is to updated next.

	Requires:	j		Current position through stream list
				end		Last stream

	Returns:	i		Stream channel

***********************************************************************************/
int SOUND_GetNextStreamPriority(u_int j,u_int end)
{
u_int i;

	for (i=0;i<end;i++)
	{
		if ((SOUND_StreamPriorityPrimary[i]==SOUND_StreamPriorityPrimary[j])&&(j!=i))	/* Same priority & Different stream? */
		{
			if (StreamBuffer[SOUND_StreamPriorityNum[i]].Priority2<StreamBuffer[SOUND_StreamPriorityNum[j]].Priority2)
			{
				j=i;
			}
		}
	}
	i=SOUND_StreamPriorityNum[j];
	StreamBuffer[i].Priority2=0xffffffff;

	return((int)i);
}

/**********************************************************************************
SOUND_ATWinMonOpenFile
	Opens a file.
	If using ATWinMon, Open and Closing of files are very slow to perform, which
	would normally cause MultiStream to slow down. Using the special ATWinMon commands,
	Any files you require to be streamed during, say, a level in your title can be
	pre-opened at the start of the level (via this command), and then closed once the
	level is completed.
	This is not normally good practice, as there is a finite amount of file handles.
	Normally, MultiStream opens and closes a file every time it loads data from it.

	Requires:	id		File ID
	Returns:	Nothing

***********************************************************************************/
void SOUND_ATWinMonOpenFile(u_int id)
{
ADPCMFILE *File;
int i;
u_char *filename;
int cnt=0;

	File=JADPCM;
	for (i=0;i<MAX_ADPCM_FILES;i++)
	{
		if(File->ID==id)			/* File ID found? */
		{
			if (File->FileName==0)
			{
				MSERROR(("Error: SOUND_ATWinMonOpenFile\n"));
				MSERROR(("No Filename stored for File ID 0x%x\n",id));
				return;
			}
			if (File->OpenFlag+1<=0)
			{
				MSERROR(("Error: SOUND_ATWinMonOpenFile\n"));
				MSERROR(("File ID 0x%x already open too many times (%d)\n",id,File->OpenFlag));
				return;
			}
			if (File->OpenFlag>0)
			{
				++File->OpenFlag;
				return;
			}
			filename=File->FileName;
			do{
				File->OpenID=open (filename, O_RDONLY);	/* Open new file */
				if ((File->OpenID==-504)||(File->OpenID==-510))
				{
					if (cnt==0)
					{
						MSERROR(("Warning: SOUND_ATWinMonOpenFile: "));
						MSERROR(("ATWinMon Error %d. Trying again..\n",File->OpenID));
					}
					DelayThread(TWO_T*2);	/* Wait a bit - Thanks Sean */
					cnt++;
					if (cnt>4000)
					{
						MSERROR(("Error: SOUND_ATWinMonOpenFile: "));
						MSERROR(("ATWinMon retry Time-Out. Open Failed\n"));
						SOUND_FILE_AT_OPEN=0;
						return;
					}
				}
				else if (File->OpenID<0)
				{
					MSERROR(("Error: SOUND_ATWinMonOpenFile\n"));
					MSERROR(("Open file failed\n"));
					SOUND_FILE_AT_OPEN=0;
					return;
				}
			}while(File->OpenID<0);

			File->OpenFlag=1;
			SOUND_FILE_AT_OPEN=1;

			return;
		}
		File++;
	}
	MSERROR(("Error: SOUND_ATWinMonOpenFile\n"));
	MSERROR(("File ID 0x%x not found\n",id));
}


/**********************************************************************************
SOUND_ATWinMonCloseFile
	Closes a file.
	If using ATWinMon, Open and Closing of files are very slow to perform, which
	would normally cause MultiStream to slow down. Using the special ATWinMon commands,
	Any files you require to be streamed during, say, a level in your title can be
	pre-opened at the start of the level, and then closed once the level is completed.
	This is not normally good practice, as there is a finite amount of file handles.
	Normally, MultiStream opens and closes a file every time it loads data from it.

	Requires:	id		File ID
	Returns:	Nothing

***********************************************************************************/
void SOUND_ATWinMonCloseFile(u_int id)
{
ADPCMFILE *File;
int i;

	File=JADPCM;
	for (i=0;i<MAX_ADPCM_FILES;i++)
	{
		if(File->ID==id)			/* File ID found? */
		{
			if (File->OpenFlag==0)
			{
				MSERROR(("Error: SOUND_ATWinMonCloseFile\n"));
				MSERROR(("File ID 0x%x already closed\n",id));
				return;
			}
			--File->OpenFlag;
			if (File->OpenFlag==0)
				close((int)File->OpenID);	/* Close file */
			return;
		}
		File++;
	}
	MSERROR(("Error: SOUND_ATWinMonCloseFile\n"));
	MSERROR(("File ID 0x%x not found\n",id));
}

/**********************************************************************************
SOUND_SetSFXPlayOffset

	Sets the offset to play a SFX from in SPU RAM.
	The offset value is added to the SPU RAM address passed to SOUND_PatchSFX.
	Passing 0 will play the SFX from the original address passed to 
	SOUND_PatchSFX.
	As there is no information held about the length of a SFX, care must be
	taken not to set the offset to invalid values.


	Requires:	ID			SFX ID to set offset of.
				LoopFlag	SOUND_LOOP_TO_START (loop back to start of sample
							when playback reaches end of an loop marker)
							SOUND_LOOP_TO_OFFSET (loop back to the offset
							position when playback reaches end of an loop marker)
				Offset		Offset into SFX start address to start playback from

	Returns:	0			OK
				-1			Invalid loop flag

	Note:
		Offset must be a multiple of 16. If the Offset is not a multiple of 16,
		its value will be rounded down (the bottom 4 bits are cleared)

		Due to the nature of ADPCM data, clicking or pops can occure when playing
		data from any start offset. The same may happen if looping to the
		offset position also. Care must be taken to make sure that clicking
		does not happen. Setting a slight attack rate when performing SOUND_PlaySFX
		can remove clicking when first playing a SFX.
***********************************************************************************/
void SOUND_SetSFXPlayOffset(u_int ID, u_short LoopFlag,u_int Offset)
{
ADPCM_SPOTINFO *SFX;

	if (SPOTFX_List_Allocated==0)	/* Any SFX memory allocated? */
	{
		MSERROR((">>>>> ERROR: SOUND_SetSFXPlayOffset\n"));	/* No.. */
		MSERROR(("SpotFX Memory not allocated\n"));
	}

	SFX=GetSFXSlot((int)ID);	/* Get SPU address for corresponding ID */
	if (SFX==0)
	{
		MSERROR((">>>>> ERROR: SOUND_SetSFXPlayOffset\n"));	/* No.. */
		MSERROR(("SFX ID (0x%x) not found\n",ID));
		return;
	}

	SFX->PlayOffset=Offset;
	SFX->LoopFlag=LoopFlag;
}

void SOUND_SetMultiStreamVoices(u_int core0, u_int core1)
{
	g_MSChannelUseMask[0] = core0;
	g_MSChannelUseMask[1] = core1;
}

/**********************************************************************************
_AdpcmSetMarkFINAL
	Sets a end of playback marker in ADPCM stream data

	Requires:	i		Stream channel
				addr	IOP address of ADPCM data
				size	size of data block

	Returns:	0		OK
				-1		invalid address

//#define _AdpcmSetMarkFINAL(a,s) { *((unsigned char *)((a)+(s)-0x1f)) = (_ADPCM_MARK_END); *((unsigned char *)((a)+(s)-0x0f)) = (_ADPCM_MARK_LOOP | _ADPCM_MARK_START | _ADPCM_MARK_END); }

***********************************************************************************/
int _AdpcmSetMarkFINAL(int i,u_int addr, u_int size)
{
u_char *ptr;

	i=SOUND_GetParent(i);
	if (SOUND_CheckADPCMSize(i,addr+size-0x1f)!=0)
	{
		MSERROR(("_AdpcmSetMarkFINAL: IOP Address invalid (0x%x)\n",addr));
		MSERROR(("Stream %d IOP base addr = 0x%x\n",i,StreamBuffer[i].IOPAddress));
		MSERROR(("Stream %d IOP Size 0x%x\n",i,StreamBuffer[i].AllocSize));
		return(-1);
	}
	ptr=(u_char *)addr;
	ptr+=size;
	ptr-=0xf;
	*ptr=(_ADPCM_MARK_LOOP | _ADPCM_MARK_START | _ADPCM_MARK_END);
	ptr-=0x10;
	*ptr=_ADPCM_MARK_END;

	return(0);
}

/**********************************************************************************
_AdpcmSetMarkSTART
	Sets a start loop marker in ADPCM stream data.

	Requires:	i		Stream channel
				addr	IOP address of ADPCM data

	Returns:	0		OK
				-1		invalid address

//#define _AdpcmSetMarkSTART(a) {*((unsigned char *)((a)+1)) = (_ADPCM_MARK_LOOP | _ADPCM_MARK_START); };

***********************************************************************************/
int _AdpcmSetMarkSTART(int i,u_int addr)
{
u_char *ptr;

	i=SOUND_GetParent(i);
	if (SOUND_CheckADPCMSize(i,addr+1)!=0)
	{
		MSERROR(("_AdpcmSetMarkSTART: IOP Address invalid (0x%x)\n",addr));
		MSERROR(("Stream %d IOP base addr = 0x%x\n",i,StreamBuffer[i].IOPAddress));
		MSERROR(("Stream %d IOP Size 0x%x\n",i,StreamBuffer[i].AllocSize));
		return(-1);
	}

	ptr=(u_char *)addr;
	ptr+=1;
	*ptr=(_ADPCM_MARK_LOOP | _ADPCM_MARK_START);
	return(0);
}

/**********************************************************************************
_AdpcmSetMarkEND
	Sets an end loop marker in ADPCM stream data.

	Requires:	i		Stream channel
				addr	IOP address of ADPCM data
				size	size of data block

	Returns:	0		OK
				-1		invalid address

//#define _AdpcmSetMarkEND(a,s) { *((unsigned char *)((a)+(s)-0x0f)) = (_ADPCM_MARK_LOOP | _ADPCM_MARK_END); };

***********************************************************************************/
int _AdpcmSetMarkEND(int i,u_int addr, u_int size)
{
u_char *ptr;

	i=SOUND_GetParent(i);
	if (SOUND_CheckADPCMSize(i,addr+size-0xf)!=0)
	{
		MSERROR(("_AdpcmSetMarkEND: IOP Address invalid (0x%x)\n",addr));
		MSERROR(("Stream %d IOP base addr = 0x%x\n",i,StreamBuffer[i].IOPAddress));
		MSERROR(("Stream %d IOP Size 0x%x\n",i,StreamBuffer[i].AllocSize));
		return(-1);
	}

	ptr=(u_char *)addr;
	ptr+=size;
	ptr-=0xf;
	*ptr=(_ADPCM_MARK_LOOP | _ADPCM_MARK_END);
	return(0);
}

/**********************************************************************************
_AdpcmSetMarkRESTART
	Removes any end markers from the original ADPCM data, if looping.

	Requires:	i		Stream channel
				addr	IOP address of ADPCM data
				size	size of data block

	Returns:	0		OK
				-1		invalid address

//#define _AdpcmSetMarkRESTART(a,s) { *((unsigned char *)((a)+(s)-0x0f)) = (0); *((unsigned char *)((a)+(s)-0x1f)) = (0);}

***********************************************************************************/
int _AdpcmSetMarkRESTART(int i,u_int addr, u_int size)
{
u_char *ptr;

	i=SOUND_GetParent(i);
	if (SOUND_CheckADPCMSize(i,addr+size-0x1f)!=0)
	{
//		MSERROR(("_AdpcmSetMarkRESTART: IOP Address invalid (0x%x)\n",addr));
//		MSERROR(("Stream %d IOP base addr = 0x%x\n",i,StreamBuffer[i].IOPAddress));
//		MSERROR(("Stream %d IOP Size 0x%x\n",i,StreamBuffer[i].AllocSize));
		return(-1);
	}

	ptr=(u_char *)addr;
	ptr+=size;
	ptr-=0xf;
	*ptr=0;
	ptr-=0x10;
	*ptr=0;
	return(0);
}


/**********************************************************************************
_AdpcmSetMarkRESTART2 (7.2)
	Removes any end markers from the original ADPCM data, if looping.
	Also clears all 14 bytes of sample data, just to make sure there's no popping when looping.

	Requires:	i		Stream channel
				addr	IOP address of ADPCM data
				size	size of data block

	Returns:	0		OK
				-1		invalid address

***********************************************************************************/
int _AdpcmSetMarkRESTART2(int i,u_int addr, u_int size)
{
u_char *ptr;

	i=SOUND_GetParent(i);
	if (SOUND_CheckADPCMSize(i,addr+size-0x1f)!=0)
	{

		MSERROR(("_AdpcmSetMarkRESTART: IOP Address invalid (0x%x)\n",addr));
		MSERROR(("Stream %d IOP base addr = 0x%x\n",i,StreamBuffer[i].IOPAddress));
		MSERROR(("Stream %d IOP Size 0x%x\n",i,StreamBuffer[i].AllocSize));
		return(-1);
	}

	ptr=(u_char *)addr;
	ptr+=size;
	ptr-=0xf;
	*ptr=0;
	ptr-=0x10;
	*ptr=0;

#if 0
	ptr=(u_char *)addr;
	ptr+=size;
	ptr-=0xe;
	for (a=0;a<14;a++)
	{
		*ptr=0;
		ptr++;
	}
#endif

	return(0);
}


/**********************************************************************************
_AdpcmSetMarkClearEND
	Removes markers stored within the IOP data if using resizable SPU buffers and
	if all data fits into IOP RAM and is looping.
	This needs to be performed as at a later date, this data will need to be resent
	to the SPU. Therefore, any loop markers previously stored within it need to be
	removed, before any new loop start/end markers can be stored within the correct
	addresses.

	Requires:	i		Stream channel
				addr	IOP address of ADPCM data
				size	size of data block

	Returns:	0		OK
				-1		invalid address

//#define _AdpcmSetMarkClearEND(a,s) { *((unsigned char *)((a)+(s)-0x0f)) = (0); };

***********************************************************************************/
int _AdpcmSetMarkClearEND(int i,u_int addr, u_int size)
{
u_char *ptr;

	i=SOUND_GetParent(i);
	if (SOUND_CheckADPCMSize(i,addr+size-0xf)!=0)
	{
		MSERROR(("_AdpcmSetMarkClearEND: IOP Address invalid (0x%x)\n",addr));
		MSERROR(("Stream %d IOP base addr = 0x%x\n",i,StreamBuffer[i].IOPAddress));
		MSERROR(("Stream %d IOP Size 0x%x\n",i,StreamBuffer[i].AllocSize));
		return(-1);
	}

	ptr=(u_char *)addr;
	ptr+=size;
	ptr-=0xf;
	*ptr=0;
	return(0);
}

/**********************************************************************************
SOUND_CheckADPCMSize
	Check that the address which an _AdpcmSetMark function is going to write to is
	valid.

	Requires	i		Stream channel
				addr	IOP address where data will be written to

	Returns:	0		OK
				-1		Address out of range

***********************************************************************************/
int SOUND_CheckADPCMSize(int i,u_int addr)
{

	if (addr<StreamBuffer[i].IOPAddress)
		return(-1);
	else if (addr>=StreamBuffer[i].IOPAddress+StreamBuffer[i].MaxSize)
		return(-1);
	return(0);
}

/**********************************************************************************
SOUND_GetParent
	Returns the parent of a stream if it has one.

	Requires:	i	  	Stream channel
	Returns:	parent	Parents stream channel (or "i" if it has none..)

***********************************************************************************/
int SOUND_GetParent(int i)
{
int parent;
	parent=StreamBuffer[i].ParentStream;	/* Get streams PARENT if it has one.. */
	if (parent!=-1)
	{
		return(parent);
	}
	return(i);
}


