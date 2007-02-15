/* SCE CONFIDENTIAL
 $PSLibId$
 */
/* 
 *                         - Multistream -
 *
 *                           Shift-JIS
 *
 *      Copyright (C) 2000-2003 Sony Computer Entertainment Europe 
 *                        All Rights Reserved.
 *
 *                            ms_dsp.c
 *                  DSP IOP code for Multistream API
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      7.0			11/June/2003		Jason Page
 *      7.3			05/November/2004	Jason Page / Simon Goodwin
 */


/* Thanks to Criterion Games for making this technique public at the Sony DevStation 2003 */
/* Thanks to Simon Goodwin for the asm version of the filter routine */

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
#include <cpureg.h>
#include "stream.h"
#include "ms_dsp.h"
#include "ms_switches.h"

#if 0
#define C0_BPC		$3	/* breakpoint program counter register */
#define C0_BDA		$5	/* breakpoint data address register */
#define C0_TAR		$6	/* target address register (read only) */
#define C0_DCIC 	$7	/* debug and cache invalidate control register */
#define	C0_BADVADDR	$8	/* bad virtual address */
#define C0_BDAM		$9	/* breakpoint data address mask register */

#define C0_BPCM		$11	/* breakpoint program counter mask register*/
#define	C0_SR		$12	/* status register */
#define	C0_CAUSE	$13	/* exception cause */
#define	C0_EPC		$14	/* exception pc */
#define	C0_PRID		$15	/* revision identifier */
#endif

#if (MS_SCANLINE_DEBUG==1)
extern int JP_IntCount;
extern int JP_IntCount2;
extern int JP_IntCount3;
extern int SOUND_SCANLINE1;
extern int SOUND_SCANLINE2;

#endif

int DeltaTime(struct SysClock* pTime0, struct SysClock* pTime1);

char SOUND_DSP_ENGINE=0;
volatile char SOUND_DSP_PROCESSING=0;
#if (MS_DSP_ACTIVE==1)
u_short SOUND_DSP_EEPacketSize=0;
int SOUND_DSP_Result[8]={0,0,0,0,0,0,0,0};
u_int SOUND_DSPBuffer=0;					// EE Address for PCM data to be sent
char  SOUND_DSP_EEActive=0;					// EE DSP effect active (SOUND_DSP_ON / OFF)
volatile char SOUND_DSP_EECounter=0;		// Current counter value (0-n)
volatile u_int SOUND_DSP_EECounter4=0;  	// 0-n (rolls around)
volatile u_int SOUND_DSP_EECounter5=0;  	// Counter of packets sent to the EE
volatile u_int SOUND_DSP_IOPCounter=0;		// amount of packets sent back to IOP
volatile u_int SOUND_DSP_IOPCounter2=0;		// 0-n (rolls around)
volatile u_int SOUND_DSP_EEReturned=0;
int SOUND_DSP_FinalAddressList=0;
u_int SOUND_DSPBufferCounter=0;	// EE Address for DSP counter to be sent
u_int SOUND_DSP_PCMBuffer=0;	// Address of PCM buffer for EE DSP effects [512*4]
u_int SOUND_DSP_PCMBufferTemp=0;	// Address of PCM buffer for EE DSP effects [512*4]
char SOUND_DSP_BuffersReady=0;

extern SOUND_TRANS SOUND_Transfer;


typedef struct DSP_INFO
{
	u_char	Function;
	u_char	Core;
	u_char	MuteFlag;
	u_int	Value[11];
} DSP_INFO;

DSP_INFO SOUND_DSPList[SOUND_MAX_DSP];
u_char SOUND_DSPCount=0;

int SOUND_CompressSideChain=0;
int lastoff=-1;
short *cptr1;
short *cptr2;
volatile u_char SOUND_DSP_COUNTER=0;
volatile u_char SOUND_DSP_COUNTER2=0;
volatile u_char SOUND_DEBUG_DSP=0;

#if (MS_DSP_DEBUG!=0)
static int SOUND_DSP_ERROR=0;
static int SOUND_DSP_ERROR_SKIP=0;
#endif

u_int SOUND_DSP_EEAddressList;
u_int SOUND_DSP_IOPAddressList;

volatile int DoingOther1=0;
volatile int DoingOther2=0;

#define RESMAX 256
#define FILMAX 512
#define COMPRESSOR_TGAIN_MAX 2048		// 2048 = Original volume

/**********************************************************************************
DSP ENGINE
Allows SPU channels to be passed through a user DSP unit.

A SPU DMA channel is required to play the effected data.
For this to work, a number of other rules apply:

1) No PCM playback is possible in the "normal" way. PCM Data would instead have to be mixed by software
	as a seperate DSP function. 
2) No DTS encoding is possible.

Credits:

Credit must go to Criterion Games for demonstrating this technique at the DevStation
developer conference - April 2003. As used in their BurnOut2 title. Thanks very much!
***********************************************************************************/

char spuoutputbufCore0[2048] __attribute__((aligned(64)));
char spuoutputbufCore1[2048] __attribute__((aligned(64)));
char playbuf[2048] __attribute__((aligned(64)));

int dsp_thid=0;
int DSP_ThreadFlag=0;
#define TSIZE 3072	//3072	//3072

int SOUND_DSP_CORE_ACTIVE=0;
#define SOUND_DSP_CORE0 1
#define SOUND_DSP_CORE1 2
#define SOUND_DSP_BOTH_CORES 3

/**************************************************************
 *                                                            *
 * Function      		:	DeltaTime()                       *
 *                                                            *
 * Author (Date) 		:	Simon Goodwin (2003-11-19)        *
 *                          Respecified (2003-11-27)          *
 *                                                            *
 * Description   		:   Reports difference in µs between  *
 *                          two times to the caller           *
 *                                                            *
 * Returns the number of microseconds between calls, or -1 if *
 * the first is later than the second implying an intervening *
 * clock reset, or -123456789 if the interval exceeds 2,146   *
 * seconds, the longest time in µs signed integers can hold.  *
 *                                                            *
 **************************************************************/

int DeltaTime(struct SysClock* pTime0, struct SysClock* pTime1)
{
    /* Timing variables: suffix 0 for before, 1 for after interval */
    int sec0, sec1, usec0, usec1, delta;

    SysClock2USec(pTime0, &sec0, &usec0);
    SysClock2USec(pTime1, &sec1, &usec1);
    
    if (sec1 - sec0 > 2146)
    {
        MSERROR(("**** Elapsed time exceeded measurement limit."));
        /* 32 bit signed arithmetic is good for 35.79 minutes */
        return -123456789;
    }
    else if (sec1 < sec0)
    {
        /* If the clock went backwards all bets are off */
        MSERROR(("[PITH] **** Timer reset?! No time available\n"));
        return -1;
    }
    else
    {
        delta = 1000000 * (sec1 - sec0); /* Millionths of whole seconds */
        delta += usec1; 
        delta -= usec0;
        return delta;
    }
}

/**********************************************************************************
SOUND_InitDSPEngine

	Initialises the DSP engine. Creates a seperate thread for it to run on.
	Starts the thread and initialises the PCM playback and core syncing.

	Requires:	value		SPU channel used for syncing SPU core 0 & 1
				priority	Thread priority (must be higher than anything else)

	Returns:	0			Initialisation failed
				1			Initialisation successful.
***********************************************************************************/
int SOUND_InitDSPEngine(int value,int priority)
{
struct ThreadParam param;

/* We have to start a thread since the EE function 'LoadModule' waits */
/* until this function returns. */

	if (SOUND_DSP_ENGINE==0)
	{
		param.attr = TH_C;
		param.entry = SOUND_DSPThread;

		param.initPriority = priority;
		param.stackSize = 0x4000;
		param.option = 0;
		dsp_thid = CreateThread(&param);
		DSP_ThreadFlag=0;
		if (dsp_thid > 0)
		{
			StartThread(dsp_thid, 0);
			DSP_ThreadFlag=1;
		}
		else
		{
			MSERROR(("SOUND_InitDSPEngine\n"));
			MSERROR(("Failed to start IOP thread.\n"));
			return (-1);
		}	

		SOUND_DSP_EECounter=0;
		SOUND_Transfer.TranMode=SOUND_TRAN_ON;
		SOUND_DSP_ENGINE=1;		// DSP Engine is now ON
		lastoff=-1;

	    sceSdSetTransIntrHandler (MS_PCM_CORE, 0,0);
/* Begin autoDMA PCM transfer from IOP buffer to output */
	  	sceSdBlockTrans(MS_PCM_CORE, SD_TRANS_MODE_WRITE | SD_BLOCK_LOOP,&playbuf[0], sizeof(playbuf));

		SOUND_DSPSync(value);	// SPU channel required (to play white noise for syncing purposes)
	    sceSdSetTransIntrHandler (MS_PCM_CORE, (sceSdTransIntrHandler) SOUND_DSPCallback,0);
		SOUND_DSP_CORE_ACTIVE=0;
		return(0);
	}
	return(1);	// Already initialised
}

/**********************************************************************************
SOUND_CloseDSP
	Called by user. Request DSP system to close down

	Requires:	Nothing
	Returns:	Nothing
***********************************************************************************/
void SOUND_CloseDSP(void)
{
	if (SOUND_DSP_ENGINE==0)
	{
		MSERROR(("SOUND_CloseDSP\n"));
		MSERROR(("DSP Engine already closed\n"));
		return;
	}
	SOUND_DSP_ENGINE=2;
	return;
}

/**********************************************************************************
SOUND_ReallyCloseDSP
	Called by MultiStream. Actually perfoms the close down, but MultiStream waits for
	a good moment to do this (when all data from any requested transfer is completed,
	so it doesn't get confused half way through..)

	Requires:	Nothing
	Returns:	Nothing
***********************************************************************************/
void SOUND_ReallyCloseDSP(void)
{
	if (dsp_thid>0)
	{
		sceSdSetParam (MS_PCM_CORE | SD_P_BVOLL,0);
		sceSdSetParam (MS_PCM_CORE | SD_P_BVOLR,0);
	  	sceSdBlockTrans(MS_PCM_CORE, SD_TRANS_MODE_STOP,0,0);
	    sceSdSetTransIntrHandler (MS_PCM_CORE, 0,0);
		DeleteThread(dsp_thid);
		dsp_thid=0;
		SOUND_DSP_ENGINE=0;
		SOUND_DSPInit();
	}
}


/**********************************************************************************
SOUND_DSPSync

	Syncs the two SPU cores.

	Requires:	spuchan		SPU channel 0-47
	Returns:	Nothing

	Notes:

	It works by playing white noise through SPU Core 0, and constantly reading 64
	bytes from the start of the cores output buffer. When this data is different
	to the last time we did the read, we know that this part of the output buffer
	has just been updated, and at this point we kick off the PCM playback. So now
	PCM playback is in sync with the actual output buffer - which means that when
	the PCM callback is called for a half buffer update, playback of the PCM buffer
	is halfway through, we can sucessfully transfer the 1st half of the output buffer
	without any problems of reading buffers which are currently being written to..
	Bad explanation, I know..
***********************************************************************************/
void SOUND_DSPSync(int spuchan)
{
int coreoutbuf[64 / sizeof(int)];
int lastcoreout;
int transferindex;
unsigned int timeout;
int core=SD_CORE_0;
int ch;
sceSdTransIntrHandler OldIntrPointer;
void *OldIntrDataPtr;
short transferpositions[3];
u_int mmix;

	core=spuchan/24;
	ch=spuchan%24;

/* Use noise generator to put random data into output buffers. */

// Turn off core to stop any white noise being heard from the speaker..
	mmix=sceSdGetParam((u_short)(core|SD_P_MMIX));	// Make copy of current settings to restore later..
	sceSdSetParam ((u_short)(core | SD_P_MMIX),0);	// Turn it off

    sceSdSetCoreAttr((u_short)(core | SD_C_NOISE_CLK), 30);
    sceSdSetSwitch((u_short)(core | SD_S_NON), (u_int)(1<<ch));
    sceSdSetParam((u_short)(core | ch<<1 | SD_VP_VOLL), 0xff);
	sceSdSetParam((u_short)(core | ch<<1 | SD_VP_ADSR1), 0xf);
	sceSdSetParam((u_short)(core | ch<<1 |SD_VP_ADSR2), 0x5);
	sceSdSetAddr((u_short)(core| ch<<1 | SD_VA_SSA), 0x5000);
    sceSdSetSwitch((u_short)(core | SD_S_KON), (u_int)(1<<ch));

    lastcoreout = 0;
    transferindex = 0;
    memset(coreoutbuf, 0, sizeof(coreoutbuf));
    timeout = 0;

	SOUND_DSPWaitForVoiceTransComplete();

/* Save current callback settings & then turn off callback */
	OldIntrDataPtr=sceSdGetTransIntrHandlerArgument(MS_SPU_CORE);
	OldIntrPointer=sceSdSetTransIntrHandler (MS_SPU_CORE,0,0);

    do
    {
        /* Repeatedly transfer 64 bytes out of SPU core into IOP buffer */
        /* to test when values change. */

        sceSdVoiceTrans(MS_SPU_CORE, SD_TRANS_MODE_READ | SD_TRANS_BY_DMA,
	        (u_char *)coreoutbuf, (u_int)(0x2000+(core*0x1000)),
	        sizeof(coreoutbuf));

		SOUND_DSPWaitForVoiceTransComplete();	// Wait for transfer completion

// Current offset through buffer for PCM playback..
        transferpositions[transferindex] = (short)((sceSdBlockTransStatus(MS_PCM_CORE, 0) & 0xffffff) - (int)playbuf);


        if (coreoutbuf[0] != lastcoreout)	/* Has the data output changed ?*/
        {
            transferindex++;

// Restart the bitstream DMA to the correct position to give a good sync from now on..

		  	sceSdBlockTrans(MS_PCM_CORE, SD_TRANS_MODE_STOP,0,0);

		  	sceSdBlockTrans(MS_PCM_CORE, SD_TRANS_MODE_WRITE_FROM | SD_BLOCK_LOOP,
		        &playbuf[0], sizeof(playbuf),&playbuf[0]);

            lastcoreout = coreoutbuf[0];
			timeout=0;	// NEW!
        }

// Just incase something, somehow, causes the SPU to not function as expected..
        timeout++;

        if (timeout > 1000)
        {
            timeout = 0;
            transferpositions[transferindex] = 9999;
            transferindex++;
        }
    } while (transferindex < (sizeof(transferpositions) / sizeof(transferpositions[0])));

/* We have the sync, turn the noise generators off. */
    sceSdSetSwitch((u_short)(core | SD_S_NON), 0);
    sceSdSetParam((u_short)(core | ch<<1 | SD_VP_VOLL), 0);
	sceSdSetSwitch((u_short)(core | SD_S_KOFF), (u_int)(1<<ch));
	sceSdSetParam ((u_short)(core | SD_P_MMIX),(u_short)mmix);		// restore mmix

/* Restore callback */
    sceSdSetTransIntrHandler (MS_SPU_CORE,OldIntrPointer,OldIntrDataPtr);
    return;
}


/**********************************************************************************
SOUND_DSPThread
	Sleeps thread.
	Thread is woken by the SOUND_DSPCallback routine, which is called playback of the
	PCM buffer reaches mid and end points of its IOP buffer.
***********************************************************************************/
int SOUND_DSPThread(void)
{
	while (1)
	{
		SleepThread();			// Wait for PCM callback to kick in..
		SOUND_DSPProcess();		// Do all DSP processing..
	}
    return (0);
}

/**********************************************************************************
SOUND_DSPWaitForVoiceTransComplete
	Performs the same function as a sceSdVoiceTransStatus(core,SD_TRANS_STATUS_WAIT)
	BUT..using WAIT will allow other threads to wake up (even if they have a lower priority)
	By constantly polling the SPU with SD_TRANS_STATUS_CHECK, no other threads will run.
	If you don't do it this way, you'll find that your playback may glitch,
	especially if host0 loading (host0 will block all threads until completion).

***********************************************************************************/
int SOUND_DSPWaitForVoiceTransComplete(void)
{
int a;
int t=0;

	do
	{
		a=sceSdVoiceTransStatus(MS_SPU_CORE, SD_TRANS_STATUS_CHECK);
		t++;
	}while(a==0);

	return (t-1);
}

/**********************************************************************************
SOUND_DSPInit
	Initialises the DSP strucutre.

	Requires:	Nothing
	Returns:	Nothing
***********************************************************************************/
void SOUND_DSPInit(void)
{
int i;
	for (i=0;i<SOUND_MAX_DSP;i++)
	{
		SOUND_DSPList[i].Function=SOUND_FLAG_DSP_OFF;	// Turn DSP function off
		SOUND_DSPList[i].Core=0;
		SOUND_DSPList[i].MuteFlag=0;
	}
	SOUND_DSPCount=0;	// No DSP effects enabled..
}

/**********************************************************************************
SOUND_CheckDSPTransfer

	Handles transfering ADPCM data to SPU RAM. Can only transfer small data packets,
	so the DMA channel isn't busy for too long (its also required for sending data
	from SPU RAM to IOP for DSP processing)

	Requires:	Nothing
	Returns:	0			Previous transfer still taking place
				1			OK

	Notes:
	
	It is only possible to transfer a max of approx 4000 bytes of data via DMA inbetween
	each PCM callback. What this means is that each MultiStream request for a transfer is
	handled in short bursts. Only when the final transfer is completed will MultiStream be
	allowed to continue.
***********************************************************************************/
int SOUND_CheckDSPTransfer(void)
{
int a;
#if (MS_LOAD62==0)
u_int size;
#endif

	a=sceSdVoiceTransStatus(MS_SPU_CORE, SD_TRANS_STATUS_CHECK);	// Transfer status
	if (a!=1)
		return(0);		// Transfer still not complete..

#if (MS_LOAD62==0)

	if (SOUND_Transfer.Status==SOUND_TRANSFER)	// Using 5.0 SPU buffer methods (fixed size)
	{
		size=TSIZE;
		if (SOUND_Transfer.Size<=TSIZE)
		{
			size=SOUND_Transfer.Size;
			SOUND_Transfer.Status=SOUND_LAST_TRANSFER;
		}
		else
			SOUND_Transfer.Size-=TSIZE;
		SOUND_Transfer.TranStatus=1;
		sceSdVoiceTrans (MS_SPU_CORE, (SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA),	// Transfer data to correct half of SPU RAM.
		(unsigned char *) SOUND_Transfer.IOPAddr,
		SOUND_Transfer.SPUAddr,size);

		if (SOUND_Transfer.Status!=SOUND_LAST_TRANSFER)
		{
			SOUND_Transfer.IOPAddr+=size;
			SOUND_Transfer.SPUAddr+=size;
		}
	}
#else
	SOUND_UpdateSPUTrans(TSIZE);	// Using 6.0+ SPU buffer methods (resizable)
#endif
	return(1);


}

/**********************************************************************************
SOUND_DSPProcess

	Does everything. Called when half of the current PCM playback buffer is played.

	Requires:	Nothing
	Returns:	Nothing
***********************************************************************************/
void SOUND_DSPProcess(void)
{
struct SysClock filterTime0, filterTime1; // SNG 
int offset;
short *ptr1;
short *ptr2;
short *ptr3;
short *ptr4;
short *ptr5;
short *ptr6;
int j,c,f;
char mixed=0;
char DSPTransferComplete=0;
u_char *tempptr;
int trn=0;


	SOUND_DSP_PROCESSING=1;

#if (MS_SCANLINE_DEBUG==1)

	SOUND_SCANLINE1=JP_IntCount;
	JP_IntCount=0;
	JP_IntCount2=0;

#endif

#if (MS_DSP_DEBUG!=0)
	SOUND_DSP_ERROR_SKIP=0;

	
#if (MS_SCANLINE_DEBUG==1)
	if (SOUND_SCANLINE2>85)
	{
#else
	if (SOUND_DSP_ERROR!=0)
	{
#endif

// This code stops the whole IOP from chugging to a halt if too much CPU is dedicated to DSP effects..
// Causes the sound to break up, but at least you still get access to the IOP!

		SOUND_DSP_ERROR--;	// Yes..
		SOUND_DSP_COUNTER2=SOUND_DEBUG_DSP;
		SOUND_DSP_ERROR_SKIP=1;
		MSERROR(("DSP processing is taking too long!\n"));

#if (MS_SCANLINE_DEBUG==1)
		SOUND_SCANLINE2=0;	//SOUND_SCANLINE1;
		JP_IntCount2=0;
#endif
		return;
	}
	else
		SOUND_DSP_COUNTER2++;	// Increase counter at START of cycle. Used for CPU overload checking!
#endif

// Which half of the buffer has just been played?
	offset =  (int)(1-(sceSdBlockTransStatus (MS_PCM_CORE, 0) >> 24));

    /* Pull data from SPU outputs into IOP buffers. */
    offset*=512;

	SOUND_DSPWaitForVoiceTransComplete();	// Wait for any previous sceSdVoiceTrans to complete

// Now, get the PCM data from either (or both) hardware effect buffers..

	if (((SOUND_DSP_CORE_ACTIVE&SOUND_DSP_CORE0)!=0)||(SOUND_CompressSideChain!=0))
	{
	    sceSdVoiceTrans(MS_SPU_CORE, SD_TRANS_MODE_READ | SD_TRANS_BY_DMA,
		    (u_char *)&spuoutputbufCore0[0+offset], 
		    (u_int)(0x2800 + offset), 512);
		SOUND_DSPWaitForVoiceTransComplete();

	    sceSdVoiceTrans(MS_SPU_CORE, SD_TRANS_MODE_READ | SD_TRANS_BY_DMA,
		    (u_char *)&spuoutputbufCore0[0x400 + offset], 
		    (u_int)(0x2c00 + offset), 512);

		trn++;
	}

	if (((SOUND_DSP_CORE_ACTIVE&SOUND_DSP_CORE1)!=0)||(SOUND_CompressSideChain!=0))
	{
		SOUND_DSPWaitForVoiceTransComplete();

	    sceSdVoiceTrans(MS_SPU_CORE, SD_TRANS_MODE_READ | SD_TRANS_BY_DMA,
		    (u_char *)&spuoutputbufCore1[0+offset], 
		    (u_int)(0x3800 + offset), 512);
		SOUND_DSPWaitForVoiceTransComplete();

	    sceSdVoiceTrans(MS_SPU_CORE, SD_TRANS_MODE_READ | SD_TRANS_BY_DMA,
		    (u_char *)&spuoutputbufCore1[0x400 + offset], 
		    (u_int)(0x3c00 + offset), 512);
		trn++;
	}
	SOUND_DSPWaitForVoiceTransComplete();

	ptr1=(short*)&spuoutputbufCore0[offset];		// Core0 - LEFT
	ptr2=(short*)&spuoutputbufCore0[0x400+offset];	// Core0 - RIGHT
	ptr3=(short*)&spuoutputbufCore1[offset];		// Core1 - LEFT
	ptr4=(short*)&spuoutputbufCore1[0x400+offset];	// Core1 - RIGHT

	SOUND_DSP_IOPCounter=(u_int)SOUND_CheckDSPBlocks();	// Amount of packets in IOP RAM ready to play

	if (SOUND_DSP_IOPCounter==SOUND_DSP_EEPacketSize)
	{
		SOUND_DSP_BuffersReady=1;
	}
	else if (SOUND_DSP_IOPCounter==0)
	{
		SOUND_DSP_BuffersReady=0;
	}

// DSP EE Mode On? (ie. data is sent back to the EE to be processed)

	if ((SOUND_DSPBuffer!=0)&&(SOUND_DSP_EEActive==SOUND_DSP_ON))
	{
		SOUND_DSPCopyCorrectBlock();
		lastoff=offset*2;
	}
	else
	{
		if (lastoff!=-1)
		{
			memcpy(&playbuf[0+lastoff],cptr1,512);	// Copy last processed buffer into PCM buffer for playback
			memcpy(&playbuf[0x200+lastoff],cptr2,512);
		}
		lastoff=offset*2;
	}


// DO DSP EFFECTS HERE!!

#if (MS_DSP_DEBUG!=0)
if (SOUND_DSP_ERROR_SKIP==0)
{
#endif

	if (trn==0)	// Both DSP cores are not active
		DSPTransferComplete=1;

	for (j=0;j<SOUND_DSPCount;j++)	// Max 8 effects (using 4 on both cores will crash everything - no CPU spare!))
	{
		if ((DSPTransferComplete==0)&&(trn==0))
			DSPTransferComplete=(char)SOUND_CheckDSPTransfer();

		if (SOUND_DSPList[j].MuteFlag==0)
		{
			c=SOUND_DSPList[j].Core;

			f=SOUND_DSPList[j].Function;
			if (c==0)
			{
				ptr5=ptr1;
				ptr6=ptr2;
			}
			else
			{
				ptr5=ptr3;
				ptr6=ptr4;
			}
			switch (f)
			{
				case SOUND_FLAG_DSP_OFF:
				break;

				case SOUND_FLAG_DSP_FILTER:

                GetSystemTime(&filterTime0); // SNG
				SOUND_DSP_Filter(0,ptr5,j);
				SOUND_DSP_Filter(1,ptr6,j);
                GetSystemTime(&filterTime1); // SNG
//                printf("[DSP] %i µs\n", DeltaTime(&filterTime0, &filterTime1)); // SNG
				break;

				case SOUND_FLAG_DSP_FLANGE:
				SOUND_DSP_Flange(0,ptr5,(u_int)j);
				SOUND_DSP_Flange(1,ptr6,(u_int)j);
				break;

				case SOUND_FLAG_DSP_DISTORT:
				SOUND_DSP_Distortion(0,ptr5,j);
				SOUND_DSP_Distortion(1,ptr6,j);
				break;

				case SOUND_FLAG_DSP_NOISEGATE:
				SOUND_DSP_NoiseGate(0,ptr5,j);
				SOUND_DSP_NoiseGate(1,ptr6,j);
				break;

				case SOUND_FLAG_DSP_COMPRESS:
				SOUND_DSP_Compressor(0,ptr5,ptr5,j);
				SOUND_DSP_Compressor(1,ptr6,ptr6,j);
				break;

				case SOUND_FLAG_DSP_COMPRESS_SIDE:
				if (c==0)
				{
					SOUND_DSP_Compressor(0,ptr5,ptr3,j);
					SOUND_DSP_Compressor(1,ptr6,ptr4,j);
				}
				else
				{
					SOUND_DSP_Compressor(0,ptr5,ptr1,j);
					SOUND_DSP_Compressor(1,ptr6,ptr2,j);
				}
				break;

				case SOUND_FLAG_DSP_RINGMOD:
				SOUND_DSP_RingMod(0,ptr5,j);
				SOUND_DSP_RingMod(1,ptr6,j);
				break;

				case SOUND_FLAG_DSP_MIXCORES:
				mixed=1;		// Set switch so that no processing is done on Core1 output..
				if (SOUND_DSP_CORE_ACTIVE==SOUND_DSP_BOTH_CORES)
					SOUND_DSP_MixCores(ptr1,ptr2,ptr3,ptr4);
				break;

				case SOUND_FLAG_DSP_PITCH_DETECT:
					SOUND_DSP_DetectPitch(c,ptr5,ptr6,j);
				break;

				default:
				MSERROR(("Unknown DSP effect 0x%x\n",f));
				break;
			}
		}
	}


#if (MS_DSP_DEBUG!=0)
}
#endif

	if (SOUND_DSP_CORE_ACTIVE==0)
	{
		for (j=0;j<256;j++)
		{
			*ptr1++=0;
			*ptr2++=0;
		}
		cptr1=ptr1;
		cptr2=ptr2;
	}

	if (((SOUND_DSP_CORE_ACTIVE&SOUND_DSP_CORE0)!=0)||(SOUND_CompressSideChain!=0))
	{
		cptr1=ptr1;
		cptr2=ptr2;
	}
	else if (((SOUND_DSP_CORE_ACTIVE&SOUND_DSP_CORE1)!=0))
	{
		cptr1=ptr3;
		cptr2=ptr4;
	}

#if (MS_DSP_DEBUG!=0)
if (SOUND_DSP_ERROR_SKIP==0)
{
	if (SOUND_DSP_COUNTER2==SOUND_DEBUG_DSP)
	{
		SOUND_DSP_ERROR+=2;
	}
	else
		SOUND_DSP_ERROR--;
	if (SOUND_DSP_ERROR<0)
		SOUND_DSP_ERROR=0;

}
#endif

#if (MS_SCANLINE_DEBUG==1)
	SOUND_SCANLINE2=JP_IntCount2;
	JP_IntCount2=0;
#endif

	SOUND_DSP_EECounter-=(char)SOUND_DSP_EEReturned;
	SOUND_DSP_EEReturned=0;

	if ((SOUND_DSPBuffer!=0)&&
		(SOUND_DSP_EEActive==SOUND_DSP_ON)&&
		(SOUND_DSP_EECounter5<SOUND_DSP_EEPacketSize))
	{
		tempptr=(u_char*)SOUND_DSP_PCMBufferTemp;
		tempptr+=SOUND_DSP_EECounter4*1024;
		memcpy(tempptr,cptr1,1024);
		SOUND_AddDSPTransAddress((u_int)tempptr,SOUND_DSPBuffer+(SOUND_DSP_EECounter4*1024),(int)SOUND_DSP_EECounter4);
		SOUND_DSP_EECounter+=1;
		SOUND_DSP_EECounter4+=1;
		SOUND_DSP_EECounter4%=SOUND_DSP_EEPacketSize;
		SOUND_DSP_EECounter5++;
	}
	if (DSPTransferComplete==0)
	{
		SOUND_DSPWaitForVoiceTransComplete();
		SOUND_CheckDSPTransfer();
	}

	SOUND_DSP_COUNTER++;
	SOUND_DSP_PROCESSING=0;
}

/**********************************************************************************
SOUND_DSPCopyCorrectBlock

	* If sending data back to EE to process before playback *
	This routine handles copying the correct data packet into the play buffer.
	All this depends on the data actually being back in the IOP when its required.

	Requires:	Nothing
	Returns:	Nothing

***********************************************************************************/
void SOUND_DSPCopyCorrectBlock(void)
{
int j;

	if ((SOUND_DSP_BuffersReady==1)&&(lastoff!=-1))
	{
		j=SOUND_FindDSPBlock((int)SOUND_DSP_IOPCounter2);
		if (j==0)
		{
			cptr1=(short*)SOUND_DSP_PCMBuffer;
			cptr1+=(SOUND_DSP_IOPCounter2*(1024/2));	// LEFT buffer 0,1,2,3..
			cptr2=cptr1+(512/2);						// RIGHT buffer  0,1,2,3..

			SOUND_DSP_IOPCounter2+=1;
			SOUND_DSP_IOPCounter2%=SOUND_DSP_EEPacketSize;
			SOUND_DSP_IOPCounter-=1;

			SOUND_DSP_EECounter5--;
			memcpy(&playbuf[0+lastoff],cptr1,1024);
		}
		else
		{
			MSERROR(("block %d not found\n",SOUND_DSP_IOPCounter2));
		}
	}
}

/**********************************************************************************
SOUND_AddDSPTransAddress
	* If sending data back to EE to process before playback *
	Add IOP and EE address of data packet into list to send to EE.

	Requires:	addr1		IOP Address of data packet
				addr2		EE Address of data packet


***********************************************************************************/
void SOUND_AddDSPTransAddress(u_int addr1,u_int addr2,int counter)
{
u_int *ptr1;
u_int *ptr2;

	while(DoingOther1==1)	/* Currently doing a IOP->EE PCM DMA Transfer? */
	{
		DelayThread(TWO_T*5);		/* Wait around for any transfers to complete */
	};

	ptr1=(u_int *)SOUND_DSP_EEAddressList;
	ptr2=(u_int *)SOUND_DSP_IOPAddressList;

	ptr1+=counter;
	ptr2+=counter;
	*ptr1=addr2;
	*ptr2=addr1;
	return;
}

/**********************************************************************************
SOUND_TransferDSPEE

	* If sending data back to EE to process before playback *
	Transfers and data blocks required over to the EE.
	Requires:	Nothing
	Returns:	Nothin
***********************************************************************************/
void SOUND_TransferDSPEE(void)
{
int i;
u_int *ptr1;
u_int *ptr2;
SPUDSP_TRANS *ptr3;
char *ptr4;

	DoingOther1=1;	// Stops PCM callback from getting a look in during transfer

	ptr1=(u_int *)SOUND_DSP_EEAddressList;
	ptr2=(u_int *)SOUND_DSP_IOPAddressList;

	for (i=0;i<SOUND_DSP_EEPacketSize;i++)
	{
		if (*ptr1!=0)
		{
			ptr3=(SPUDSP_TRANS *)SOUND_DSP_FinalAddressList;
			ptr3+=i;
			if (ptr3->Counter==-1)
			{
				SOUND_TransferDataToEE(*ptr2,*ptr1,1024);	// Transfer data packet to EE
				ptr4=(char *)SOUND_DSP_PCMBuffer;
				ptr4+=i*1024;

				ptr3->Counter=i|0x8000;			// Set top bit of data counter to say that its sent..
				ptr3->IOPAddress=(int)ptr4;	// IOP Address where data is to arrive
				ptr3->EEAddress=(int)(*ptr1);			// EE Address where data is sent to
				*ptr1=0;

			}
			ptr3++;
		}
		ptr1++;
		ptr2++;
	}

	DoingOther1=0;
}

/**********************************************************************************
SOUND_SetDSPCount

	* If sending data back to EE to process before playback *
	Called by user from EE to say that data packet has arrived back in IOP RAM

	Requires:	c			packet number
	Returns:	Nothing
***********************************************************************************/
void SOUND_SetDSPCount(u_int c)
{
int i,j,k;
SPUDSP_TRANS *ptr;

	DoingOther1=1;	// Stops PCM callback from getting a look in during transfer

	ptr=(SPUDSP_TRANS *)SOUND_DSP_FinalAddressList;
	for (i=0;i<SOUND_DSP_EEPacketSize;i++)
	{

		if (ptr->Counter!=-1)
		{
			j=ptr->Counter&0x8000;
			k=ptr->Counter&0xff;
			if ((j!=0)&&(k==c))			// Found correct entry?
			{
				ptr->Counter=(int)c;			// Yes..Clear top bit..
				DoingOther1=0;	// Allow PCM callback to process
				return;
			}
		}
		ptr++;
	}
	DoingOther1=0;	// Allow PCM callback to process
}

/**********************************************************************************
SOUND_FindDSPBlock

	* If sending data back to EE to process before playback *
	Checks if required packet is available to play in IOP RAM. If it is, free it
	from the system

	Requires:	f			Packet number
	Returns:	0			Packet found
				-1			Packet not found
***********************************************************************************/
int SOUND_FindDSPBlock(int f)
{
int i;
SPUDSP_TRANS *ptr;

	ptr=(SPUDSP_TRANS *)SOUND_DSP_FinalAddressList;
	for (i=0;i<SOUND_DSP_EEPacketSize;i++)
	{
		if (ptr->Counter==f)
		{
			ptr->Counter=-1;	// Free block again
			return(0);
		}
		ptr++;
	}
	return(-1);
}

/**********************************************************************************
SOUND_CheckDSPBlocks

	* If sending data back to EE to process before playback *
	Counts blocks in IOP RAM which are ready to play

	Requires:	Nothing
	Returns:	Amount of packets ready to play
***********************************************************************************/
int SOUND_CheckDSPBlocks(void)
{
int i,k;
SPUDSP_TRANS *ptr;

	ptr=(SPUDSP_TRANS *)SOUND_DSP_FinalAddressList;
	k=0;
	for (i=0;i<SOUND_DSP_EEPacketSize;i++)
	{
		if ((ptr->Counter!=-1)&&(ptr->Counter<0x8000))
		{
			k++;	// Amount of packets of data ready to be played (ie. sent back from EE to IOP)
		}
		ptr++;
	}
	return(k);
}

/**********************************************************************************
SOUND_DSPOnOff

	Called by user.
	Turns all DSP processing for specified core ON/OFF.

	Requires:	core		0 or 1 (0=SPU channels 0-23, 1=SPU channels 24-47)
				flag		0 = OFF, 1 = ON
	Returns:	Nothing
***********************************************************************************/
void SOUND_DSPOnOff(int core, int flag)
{
	if (SOUND_DSP_ENGINE==0)
	{
		MSERROR(("Error: SOUND_DSP_OnOff\n"));
		MSERROR(("DSP Engine not initialised\n"));
		MSERROR(("(SOUND_InitDSPEngine not called)\n"));
	}

	if (flag==1)	// ON!
		SOUND_DSP_CORE_ACTIVE|=core+1;
	else			// OFF!
		SOUND_DSP_CORE_ACTIVE&=~(core+1);
}			 

/**********************************************************************************
SOUND_DSPEffect

	Called by user.
	Sets required DSP effect, in correct slot, for correct core.

	Requires:	core			0 or 1
				offset			offset in DSP list for storing DSP effect (0-7)
				type			DSP type to use in slot
	Returns:	Nothing

	Notes:
	There are 8 slots which can be used for DSP processing. Slot 0 is processed first.
	Slot 7 processed last. Each slot can only process data for a single core.

***********************************************************************************/
void SOUND_SetDSPEffect(u_int core,u_int offset, u_int type)
{
int i;
int f=0;
int last;
//short volume;

	if (offset>=SOUND_MAX_DSP)
	{
		MSERROR(("Offset %d too large. Must range from 0-%d\n",offset,SOUND_MAX_DSP-1));
		return;
	}

	last=SOUND_DSPList[offset].Function;
	SOUND_DSPList[offset].Function=SOUND_FLAG_DSP_OFF;	// protect setting up from interrupts..
	SOUND_DSPList[offset].MuteFlag=1;

	if (last==SOUND_FLAG_DSP_FLANGE)		// Slot required is currently playing flange DSP effect?
		SOUND_DSP_CloseFlange((int)offset);		// Yes..So free its IOP RAM buffer.

	SOUND_DSPList[offset].Core=(u_char)core;		// Store core

	if (type==SOUND_FLAG_DSP_FLANGE)		// Init DSP type
		SOUND_DSP_InitFlange((int)offset,0,0);
	else if (type==SOUND_FLAG_DSP_FILTER)
		SOUND_DSP_InitFilter((int)offset);
	else if (type==SOUND_FLAG_DSP_NOISEGATE)
		SOUND_DSP_InitNoiseGate((int)offset);
	else if (type==SOUND_FLAG_DSP_DISTORT)
		SOUND_DSP_InitDistortion((int)offset);
	else if (type==SOUND_FLAG_DSP_COMPRESS)
		SOUND_DSP_InitCompressor((int)offset);
	else if (type==SOUND_FLAG_DSP_COMPRESS_SIDE)
	{
		SOUND_DSP_InitCompressor((int)offset);
		SOUND_CompressSideChain++;
	}
	else if (type==SOUND_FLAG_DSP_RINGMOD)
		SOUND_DSP_InitRingMod((int)offset);
	else if (type==SOUND_FLAG_DSP_PITCH_DETECT)
		SOUND_DSP_InitDetectPitch((int)offset);

	if (last==SOUND_FLAG_DSP_COMPRESS_SIDE)
	{
		SOUND_CompressSideChain--;
	}

	SOUND_DSPList[offset].Function=(u_char)type;
	SOUND_DSPList[offset].MuteFlag=0;

	for (i=0;i<SOUND_MAX_DSP;i++)
	{
		if (SOUND_DSPList[i].Function!=SOUND_FLAG_DSP_OFF)
			f=i+1;
		else
			SOUND_DSP_Result[i]=0;

	}
	SOUND_DSPCount=(u_char)f;	// Last slot with DSP effect active.

#if 0
	printf("--------------------\n");
	printf("DSP Debug:\n");
	for (i=0;i<f;i++)
	{
		printf("%d: Function:%d Core %d\n",i,SOUND_DSPList[i].Function,SOUND_DSPList[i].Core);
	}
	printf("--------------------\n");
#endif
}

/**********************************************************************************
SOUND_ClearDSPEffects
	Turn off all DSP effects on a specified core.

	Required:	core		Core 0 or 1
	Returns:	Nothing
***********************************************************************************/
void SOUND_ClearDSPEffects(u_int core)
{
int i;
	for (i=0;i<SOUND_MAX_DSP;i++)
	{
		if (SOUND_DSPList[i].Core==core)
			SOUND_SetDSPEffect(core,(u_int)i,SOUND_FLAG_DSP_OFF);
	}
}


/**********************************************************************************
SOUND_DSPCallback
	Called when playback of the PCM data reaches mid and end points of its
	IOP buffer
***********************************************************************************/
int SOUND_DSPCallback(int ch, void *common)
{
	(void)ch;
	(void)common;
	/* Wakeup the thread */
	SOUND_DEBUG_DSP=SOUND_DSP_COUNTER2;
	iWakeupThread(dsp_thid);
    return (0);
}

/**********************************************************************************
SOUND_DSP_MixCores

	Mix data from core 0 and core 1 together into core 0..

	Requires:	(short*)ptr1		LEFT data core 0
				(short*)ptr2		RIGHT data core 0
				(short*)ptr3		LEFT data core 1
				(short*)ptr4		RIGHT data core 1

	Returns:	Nothing
***********************************************************************************/
void SOUND_DSP_MixCores(short *ptr1, short *ptr2, short *ptr3, short *ptr4)
{
int i;
int a;
	i=256;
	do{
		a=*ptr1+*ptr3++;
		if (a>0x7fff)
			a=0x7fff;
		else if (a<-0x7fff)
			a=-0x7fff;
		*ptr1++=(short)a;
		a=*ptr2+*ptr4++;
		if (a>0x7fff)
			a=0x7fff;
		else if (a<-0x7fff)
			a=-0x7fff;
		*ptr2++=(short)a;
		i--;
	}while(i);
}

/**************************************************************************
SOUND_DSP_InitFilter
	Initialises Low/High/Band pass filter with resonance

	Requires:	j 		Slot number (0-7)
	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_InitFilter(int j)
{
	SOUND_DSPList[j].Value[0]=FILMAX;
	SOUND_DSPList[j].Value[1]=FILMAX;
	SOUND_DSPList[j].Value[2]=RESMAX;
	SOUND_DSPList[j].Value[3]=SOUND_FILTER_LOWPASS;
	SOUND_DSPList[j].Value[4]=0;	// Old Bandpass Left
	SOUND_DSPList[j].Value[5]=0;	// Old Bandpass Right
	SOUND_DSPList[j].Value[6]=0;	// Old Lowpass Left
	SOUND_DSPList[j].Value[7]=0;	// Old Lowpass Right
	SOUND_DSP_Result[j]=0;
}

/**************************************************************************
SOUND_DSP_Filter
	Low/High/Band pass filter with resonance

	Requires:	LR				Left or Right speakers data
				(short*)data	pointer to sample data
				j				Slot number (0-7)

	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_Filter(int LR, short *data,int j)
{

#if (DSP_FILTER_OPTIMISED==0)
int Samp;
int D4;
int i;
#else
long long ret;
#endif

int *type;
int FilterType;
int LowPass,HiPass,BPass;
int Res;
int JFrq;


	JFrq=(int)SOUND_DSPList[j].Value[1];
	Res=(int)SOUND_DSPList[j].Value[2];

	FilterType=(int)SOUND_DSPList[j].Value[3];

	BPass=(int)SOUND_DSPList[j].Value[4+LR];
	LowPass=(int)SOUND_DSPList[j].Value[6+LR];

 	if (FilterType==SOUND_FILTER_LOWPASS)
	{
 		type=&LowPass;
	}
 	else if (FilterType==SOUND_FILTER_HIPASS)
 		type=&HiPass;
 	else
 		type=&BPass;

#if (DSP_FILTER_OPTIMISED==1)

	ret=_FastFilter(LowPass,BPass,(short *)data,Res|(JFrq<<16));
	LowPass=ret;
	ret>>=32;
	BPass=ret;

#else // Unoptimised

	i=256;
	do
	{
		Samp=*data;
		LowPass=LowPass+((JFrq*BPass)>>9);
		HiPass=Samp-LowPass-((Res*BPass)>>8);
		BPass=((JFrq*HiPass)>>9)+BPass; 

	 	if (FilterType==SOUND_FILTER_LOWPASS)
		{
			if (LowPass<-0x7fff)
				LowPass=-0x7fff; 			// limit sample range.
			else if (LowPass>=0x7fff)
				LowPass=0x7fff; 
		}
	 	else if (FilterType==SOUND_FILTER_HIPASS)
		{
			if (HiPass<-0x7fff)
				HiPass=-0x7fff; 			// limit sample range.
			else if (HiPass>=0x7fff)
				HiPass=0x7fff; 
		}
		else
		{
			if (BPass<-0x7fff)
				BPass=-0x7fff; 			// limit sample range.
			else if (BPass>=0x7fff)
				BPass=0x7fff; 
		}


		D4=*type;
		*data++=(short)D4;		// store new value
		i--;
	}while(i);

#endif // Unoptimised

	SOUND_DSPList[j].Value[4+LR]=(u_int)BPass;		// Copy last values for next time..
	SOUND_DSPList[j].Value[6+LR]=(u_int)LowPass;
}


/**************************************************************************
SOUND_DSP_InitDistortion
	Initialise distortion effect
	Requires:	j 		Slot number (0-7)
	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_InitDistortion(int j)
{
	SOUND_DSPList[j].Value[0]=0;
	SOUND_DSP_Result[j]=0;
}

/**************************************************************************
SOUND_DSP_Distortion
	Process distortion effect
	Requires:	LR				Left or Right speakers data
				(short*)data	pointer to sample data
				j				Slot number (0-7)

	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_Distortion(int LR,short *data,int j)
{
int i,t;
int k;
int JDist;
int t2;

(void)LR;

	if (SOUND_DSPList[j].Value[0]==0)	// No distortion level?
		return;

	k=(int)SOUND_DSPList[j].Value[0]*256;
	JDist=0x2000+k;

	JDist=0x800;	//k;

	k=0x6000-k;	//x4800;
	if (k<0)
		k=0;

	for (i=0;i<256;i++)
	{
		t=*data;
		t2=msabs(t);
		if (t2>k)
		{
			t2-=k;
			t2*=2;
			t2+=k;
			if (t2>0x7fff)
				t2=0x7fff;
		}
		if (t<0)
			t2=-t2;

		*data++=(short)t2;
#if 0
		t*=JDist;
		t/=0x2000;
		if (t>0x7fff)
			t=0x7fff;
		else if (t<-0x7fff)
			t=-0x7fff;
		*data++=t;
#endif
	}
}

/**************************************************************************
SOUND_DSP_InitNoiseGate
	Initialise noise gate effect
	Requires:	j 		Slot number (0-7)
	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_InitNoiseGate(int j)
{
	SOUND_DSPList[j].Value[0]=0;
	SOUND_DSPList[j].Value[1]=0;
	SOUND_DSPList[j].Value[2]=4;		// Gate ramp speed
	SOUND_DSPList[j].Value[3]=4;		// Gate ramp speed
	SOUND_DSPList[j].Value[4]=1024;		// Gate ramp level
	SOUND_DSPList[j].Value[5]=1024;		// Gate ramp level
	SOUND_DSP_Result[j]=0;
}

/**************************************************************************
SOUND_DSP_NoiseGate
	Process NoiseGate effect
	Requires:	LR				Left or Right speakers data
				(short*)data	pointer to sample data
				j				Slot number (0-7)

	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_NoiseGate(int LR,short *data,int j)
{
int i;
int a;
int NoiseGate;
int GateLevel;
int GateSpeed;

	NoiseGate=(int)SOUND_DSPList[j].Value[0+LR];
	if (NoiseGate==0)	// Noise gate level 0 ?
		return;

	GateSpeed=(int)SOUND_DSPList[j].Value[2+LR];

	if (GateSpeed==0)
		GateLevel=1024;
	else
	{
		GateLevel=(int)SOUND_DSPList[j].Value[4+LR];
		for (i=0;i<256;i++)
		{
			a=*data;		// Get sample..
		 	if ((a<0)&&(a>-NoiseGate))		// value is negative and lower than level?
				GateLevel-=GateSpeed;		// fade volume down
			else if ((a>0)&&(a<NoiseGate))	// value is positive and lower than level?
				GateLevel-=GateSpeed;		// fade volume down
			else
				GateLevel+=GateSpeed;		// fade volume up

			if (GateLevel<0)				// limit volume
				GateLevel=0;
			else if (GateLevel>1024)
				GateLevel=1024;

			a*=GateLevel;	// scale PCM sample
			a>>=10;			// Divide by 1024
			*data++=(short)a;
		}
	}

	SOUND_DSPList[j].Value[4+LR]=(u_int)GateLevel;
	SOUND_DSP_Result[j]=GateLevel;
}

/**************************************************************************
SOUND_DSP_InitFlange
	Initialise flange effect

	Requires:	j 		Slot number (0-7)
				Depth	Start depth position
				Speed	Not used

	Returns:	0		OK
				-1		Memory allocation failed
**************************************************************************/
int SOUND_DSP_InitFlange(int j,int Depth, int Speed)
{
int oldstat;
(void)Speed;

	SOUND_DSPList[j].Value[0]=128;	//Depth;
	SOUND_DSPList[j].Value[1]=1;	//Speed;
	SOUND_DSPList[j].Value[2]=0;
	SOUND_DSPList[j].Value[3]=(u_int)Depth;
	SOUND_DSP_Result[j]=0;

    CpuSuspendIntr (&oldstat);	/* No..Suspend interupt around memory allocation */

//Allocate 1024 bytes (256 samples * 2 (For 16 bit) *2 (for both left and right speakers)
	SOUND_DSPList[j].Value[4] = (unsigned int) AllocSysMemory (SMEM_Low, 1024, NULL);
    CpuResumeIntr (oldstat);	/* Resume interrupts after memory allocation */

	if ((void*)SOUND_DSPList[j].Value[4]==NULL)
	{
		MSERROR(("SOUND_DSP_InitFlange\n"));
		MSERROR(("Can not allocate 1024 bytes for flange buffer\n"));
		return(-1);
	}

	SOUND_DSPList[j].Value[5]=0;
	return(0);
}

/**************************************************************************
SOUND_DSP_CloseFlange
	Free memory required for Flange effect
	Requires:	j 		Slot number (0-7)
	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_CloseFlange(int j)
{
int oldstat;

	SOUND_WaitForDSPToComplete();
	if ((void *)SOUND_DSPList[j].Value[4]!=NULL)
	{
	    CpuSuspendIntr (&oldstat);
	 	FreeSysMemory( (void*)SOUND_DSPList[j].Value[4]);	/* Free IOP PCM buffer RAM. */
	    CpuResumeIntr (oldstat);
		SOUND_DSPList[j].Value[4]=0;
	}
}

/**************************************************************************
SOUND_DSP_Flange
	Performs Flange effect
	Requires:	LR				Left or Right speakers data
				(short*)data	pointer to sample data
				j				Slot number (0-7)

	Returns:	Nothing
**************************************************************************/
int a2;
int ttr=0;
void SOUND_DSP_Flange(int LR,short *data,u_int j)
{
int i,a,b;
u_char c,d;

short *ptr;
short *ptr2;
short *ptr3;
int Depth;
int Delay;
int DelayDir;
int count;
short *buffer;

	Depth=(int)SOUND_DSPList[j].Value[0];
	DelayDir=(int)SOUND_DSPList[j].Value[1];
	count=(int)SOUND_DSPList[j].Value[2];
	Delay=(int)SOUND_DSPList[j].Value[3];
	buffer=(short *)SOUND_DSPList[j].Value[4];	// address of IOP buffer
	buffer+=(256*LR);

	c=(u_char)(SOUND_DSPList[j].Value[5]-Delay);
	d=(u_char)SOUND_DSPList[j].Value[5];

	ptr=buffer;
	ptr+=c;
	ptr2=buffer;
	ptr3=buffer;
	ptr3+=d;

	i=256;

	do
	{
		a=*data;
		b=a+*ptr++;
		if (b>0x7fff)
			b=0x7fff;
		else if (b<-0x7fff)
			b=-0x7fff;
		*data++=(short)b;
		*ptr3++=(short)a;
		c++;
		if (!c)	//(c==0)
			ptr=ptr2;
		d++;
		if (!d)	//(d==0)
			ptr3=ptr2;
		i--;
	}while(i!=0);

	SOUND_DSPList[j].Value[5]=(u_int)c+Delay;
	count++;
	if ((count)>6)
	{
		Delay+=DelayDir;
		if (Delay<1)
		{
			DelayDir=-DelayDir;
			Delay=1;
		}
		else if(Delay>Depth)
		{
			DelayDir=-DelayDir;
			Delay=Depth;
		}
		count=0;
	}

	SOUND_DSPList[j].Value[1]=(u_int)DelayDir;
	SOUND_DSPList[j].Value[2]=(u_int)count;
	SOUND_DSPList[j].Value[3]=(u_int)Delay;
}

/**************************************************************************
SOUND_DSP_InitRingMod
	Initialise Ring Modulation effect
	Requires:	j 		Slot number (0-7)
	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_InitRingMod(int j)
{
	SOUND_DSPList[j].Value[0]=1;		// (value added to sawtooth amplitude to create frequency)
	SOUND_DSPList[j].Value[1]=0;		// Current sawtooth waveform level
	SOUND_DSP_Result[j]=0;
}

/**************************************************************************
SOUND_DSP_RingMod
	Performs RingMod effect
	Requires:	LR				Left or Right speakers data
				(short*)data	pointer to sample data
				j				Slot number (0-7)

	Returns:	Nothing
**************************************************************************/

static short s_table [] = {
    0*0x7f,   6*0x7f,  12*0x7f,  18*0x7f,  24*0x7f,  31*0x7f,  37*0x7f,  43*0x7f,
    49*0x7f,  55*0x7f,  61*0x7f,  68*0x7f,  74*0x7f,  79*0x7f,  85*0x7f,  91*0x7f,
    97*0x7f, 103*0x7f, 109*0x7f, 114*0x7f, 120*0x7f, 125*0x7f, 131*0x7f, 136*0x7f,
    141*0x7f, 146*0x7f, 151*0x7f, 156*0x7f, 161*0x7f, 166*0x7f, 171*0x7f, 175*0x7f,
    180*0x7f, 184*0x7f, 188*0x7f, 193*0x7f, 197*0x7f, 201*0x7f, 204*0x7f, 208*0x7f,
    212*0x7f, 215*0x7f, 218*0x7f, 221*0x7f, 224*0x7f, 227*0x7f, 230*0x7f, 233*0x7f,
    235*0x7f, 237*0x7f, 240*0x7f, 242*0x7f, 244*0x7f, 245*0x7f, 247*0x7f, 248*0x7f,
    250*0x7f, 251*0x7f, 252*0x7f, 253*0x7f, 253*0x7f, 254*0x7f, 254*0x7f, 254*0x7f,
    254*0x7f, 254*0x7f, 254*0x7f, 254*0x7f, 253*0x7f, 253*0x7f, 252*0x7f, 251*0x7f,
    250*0x7f, 248*0x7f, 247*0x7f, 245*0x7f, 244*0x7f, 242*0x7f, 240*0x7f, 237*0x7f,
    235*0x7f, 233*0x7f, 230*0x7f, 227*0x7f, 224*0x7f, 221*0x7f, 218*0x7f, 215*0x7f,
    212*0x7f, 208*0x7f, 204*0x7f, 201*0x7f, 197*0x7f, 193*0x7f, 188*0x7f, 184*0x7f,
    180*0x7f, 175*0x7f, 171*0x7f, 166*0x7f, 161*0x7f, 156*0x7f, 151*0x7f, 146*0x7f,
    141*0x7f, 136*0x7f, 131*0x7f, 125*0x7f, 120*0x7f, 114*0x7f, 109*0x7f, 103*0x7f,
    97*0x7f, 91*0x7f, 85*0x7f, 79*0x7f, 74*0x7f, 68*0x7f, 61*0x7f, 55*0x7f,
    49*0x7f, 43*0x7f, 37*0x7f, 31*0x7f, 24*0x7f, 18*0x7f, 12*0x7f, 6*0x7f,
    0*0x7f, -6*0x7f, -12*0x7f, -18*0x7f, -24*0x7f, -31*0x7f, -37*0x7f, -43*0x7f,
    -49*0x7f, -55*0x7f, -61*0x7f, -68*0x7f, -74*0x7f, -79*0x7f, -85*0x7f, -91*0x7f,
    -97*0x7f, -103*0x7f, -109*0x7f, -114*0x7f, -120*0x7f, -125*0x7f, -131*0x7f, -136*0x7f,
    -141*0x7f, -146*0x7f, -151*0x7f, -156*0x7f, -161*0x7f, -166*0x7f, -171*0x7f, -175*0x7f,
    -180*0x7f, -184*0x7f, -188*0x7f, -193*0x7f, -197*0x7f, -201*0x7f, -204*0x7f, -208*0x7f,
    -212*0x7f, -215*0x7f, -218*0x7f, -221*0x7f, -224*0x7f, -227*0x7f, -230*0x7f, -233*0x7f,
    -235*0x7f, -237*0x7f, -240*0x7f, -242*0x7f, -244*0x7f, -245*0x7f, -247*0x7f, -248*0x7f,
    -250*0x7f, -251*0x7f, -252*0x7f, -253*0x7f, -253*0x7f, -254*0x7f, -254*0x7f, -254*0x7f,
    -255*0x7f, -254*0x7f, -254*0x7f, -254*0x7f, -253*0x7f, -253*0x7f, -252*0x7f, -251*0x7f,
    -250*0x7f, -248*0x7f, -247*0x7f, -245*0x7f, -244*0x7f, -242*0x7f, -240*0x7f, -237*0x7f,
    -235*0x7f, -233*0x7f, -230*0x7f, -227*0x7f, -224*0x7f, -221*0x7f, -218*0x7f, -215*0x7f,
    -212*0x7f, -208*0x7f, -204*0x7f, -201*0x7f, -197*0x7f, -193*0x7f, -188*0x7f, -184*0x7f,
    -180*0x7f, -175*0x7f, -171*0x7f, -166*0x7f, -161*0x7f, -156*0x7f, -151*0x7f, -146*0x7f,
    -141*0x7f, -136*0x7f, -131*0x7f, -125*0x7f, -120*0x7f, -114*0x7f, -109*0x7f, -103*0x7f,
    -97*0x7f, -91*0x7f, -85*0x7f, -79*0x7f, -74*0x7f, -68*0x7f, -61*0x7f, -55*0x7f,
    -49*0x7f, -43*0x7f, -37*0x7f, -31*0x7f, -24*0x7f, -18*0x7f, -12*0x7f, -6*0x7f
};


void SOUND_DSP_RingMod(int LR,short *data,int j)
{
int i;
int d;
int s;
u_short w;

	s=(int)SOUND_DSPList[j].Value[0];		// Ringmod freq
	if (s==0)							// Ringmod off?
		return;
	w=(u_short)SOUND_DSPList[j].Value[1];		// Ringmod waveform (sawtooth, generated in this routine)
	
	i=256;
	do
	{
		d=*data;						// Get sample data
		d*=s_table[w>>8];			// Multiple by other waveform
		d>>=15;							// Scale
		if (d>0x7fff)					// Limit
			d=0x7fff;
		else if (d<-0x7fff)
			d=-0x7fff;
		*data++=(short)d;						// Store
		w+=(u_short)s;							// Modify sawtooth waveform
		i--;
	}while(i!=0);
	if (LR==1)							// Just performed DSP on RIGHT data (so LEFT has already been done?)
		SOUND_DSPList[j].Value[1]=w;	// Store current sawtooth sample
}

/**************************************************************************
SOUND_DSP_InitCompressor
	Initialise Compressor effect
	Requires:	j 		Slot number (0-7)
	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_InitCompressor(int j)
{
	SOUND_DSPList[j].Value[0]=16;		// Threshold
	SOUND_DSPList[j].Value[1]=1024;		// Ratio 0-2048 (2048=1:1, 1024 = 2:1 ...)
	SOUND_DSPList[j].Value[2]=8;		// Attack
	SOUND_DSPList[j].Value[3]=1;		// Release
	SOUND_DSPList[j].Value[4]=0;		// Extra gain (0-2048)
	SOUND_DSPList[j].Value[5]=0;		// Current gain LEFT
	SOUND_DSPList[j].Value[6]=0;		// Current gain RIGHT
	SOUND_DSPList[j].Value[7]=0;		// release counter (don't ask..)
	SOUND_DSPList[j].Value[8]=0;		// release counter (don't ask..)
	SOUND_DSP_Result[j]=0;
}

/**************************************************************************
SOUND_DSP_Compressor
	Performs compressor effect
	Requires:	LR				Left or Right speakers data
				(short*)data	pointer to sample data
				(short*)data2	pointer to sample data from other core (if side chain compression)
				j				Slot number (0-7)

	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_Compressor(int LR,short *data,short *data2,int j)
{
int i;
int Ratio;
int Threshold;
int AmpGain;
int ThresholdGain;
int d,d2;
int Attack,Release;
int cntr;

	Threshold=(int)SOUND_DSPList[j].Value[0];
	Ratio=(int)SOUND_DSPList[j].Value[1];
	Attack=(int)SOUND_DSPList[j].Value[2];
	Release=(int)SOUND_DSPList[j].Value[3];
	ThresholdGain=(int)SOUND_DSPList[j].Value[5+LR];
	AmpGain=(int)SOUND_DSPList[j].Value[4];
	cntr=(int)SOUND_DSPList[j].Value[7+LR];

	if (Threshold>=0x8000)	// Compressor off?
		return;				// Yes..

	AmpGain+=COMPRESSOR_TGAIN_MAX;

	i=256;
	if (data!=data2)	// Side Chain Compressor ?
	{
		do
		{
			d=*data2++;				// Get other cores data
			if (d<0)				// Get abs value..(don't trust abs function!)
				d2=-d;
			else
				d2=d;
			if (d2>Threshold)			// Is data larger than threshold?
				ThresholdGain-=Attack;	// Yes..So bring down gain..
			else if (cntr==0)
				ThresholdGain+=Release; // Otherwise increase gain..

			if (ThresholdGain<Ratio)	// Limit gain
				ThresholdGain=Ratio;
			else if (ThresholdGain>2048)	//AmpGain)
				ThresholdGain=2048;	//AmpGain;

			d=*data;		// Apply compression from other cores data to this core.
			d*=ThresholdGain;
			d>>=11;			// divide by 2048

			d*=AmpGain;
			d>>=11;

			if (d>0x7fff)
				d=0x7fff;
			else if (d<-0x7fff)
				d=-0x7fff;

			*data++=(short)d;
			i--;
			cntr++;
			cntr&=31;
		}while(i!=0);
	}
	else // Must be a normal compressor
	{
		do
		{
			d=*data;			// Get sample data..
			if (d<0)			// Get abs value..
				d2=-d;
			else
				d2=d;
			if (d2>Threshold)			// Is data larger than threshold?
				ThresholdGain-=Attack;	// Yes..So bring down the gain..
			else if (cntr==0)
				ThresholdGain+=Release;	// Otherwise increase the gain..

			if (ThresholdGain<Ratio)	// Limit gain
				ThresholdGain=Ratio;
			else if (ThresholdGain>2048)	//AmpGain)
				ThresholdGain=2048;	//AmpGain;
			d*=ThresholdGain;			// Appy compression to sample data

			d>>=11;						// Divide by 2048

			d*=AmpGain;
			d>>=11;

			if (d>0x7fff)				// Check for overflow
				d=0x7fff;
			else if (d<-0x7fff)
				d=-0x7fff;

			*data++=(short)d;
			i--;
			cntr++;
			cntr&=31;
		}while(i!=0);
	}
	SOUND_DSPList[j].Value[7+LR]=(u_int)cntr;
	SOUND_DSPList[j].Value[5+LR]=(u_int)ThresholdGain;
	SOUND_DSP_Result[j]=ThresholdGain;
}

/**************************************************************************
SOUND_DSP_InitDetectPitch
	Initialise Pitch Detection effect
	Requires:	j 		Slot number (0-7)
	Returns:	Nothing
**************************************************************************/
void SOUND_DSP_InitDetectPitch(int j)
{
	SOUND_DSPList[j].Value[0]=0;		// Average pitch over n buffer halfs
	SOUND_DSPList[j].Value[1]=0;		// Counter for how many buffer halfs to average before giving result
	SOUND_DSPList[j].Value[2]=0;		// Last direction flag
	SOUND_DSPList[j].Value[3]=0;		// Copy of last value (1)
	SOUND_DSPList[j].Value[4]=0;		// Copy of last value (2)
	SOUND_DSPList[j].Value[5]=0;		// Current buffer to write last value to (3+this one..)
	SOUND_DSPList[j].Value[6]=0;		// Last freq calculated
	SOUND_DSP_Result[j]=0;
}

/**************************************************************************
SOUND_DSP_DetectPitch
	Performs a very rough/basic Pitch Detection
	Requires:	core			0 or 1
				(short*)data	pointer to LEFT data
				(short*)data2	pointer to RIGHT data
				j				Slot number (0-7)

	Returns:	Nothing

	Notes:		Results will be approx to the actual frequency.
				Example: A 440Hz sample will give results between 434 and 496.

**************************************************************************/
#define PDPackets 3*15	// Approx 3 packets per frame..
void SOUND_DSP_DetectPitch(int core,short *data,short *data2,int j)
{
int i;
int d;
int dir;
int avp,avc;
int frq=0;

(void)core;

	avp=(int)SOUND_DSPList[j].Value[0];	// Average pitch
	avc=(int)SOUND_DSPList[j].Value[1];	// Average counter;
    dir=(int)SOUND_DSPList[j].Value[2];	// Last direction flag

	i=256;
	do
	{
		d=*data++;
		d+=*data2++;	// Mix Left and Right data..

		if ((d<0)&&(dir==0))	// Is value negative (and was positive) ?
		{
			dir=1;				// Yes..Just set flag
			frq++;				// And increase freq counter
		}
		else if ((d>=0)&&(dir==1))	// Is value positive (and was negative) ?
		{
			dir=0;				// Yes..Set flag
			frq++;				// And increase freq counter
		}
		i--;
	}while(i!=0);

	avc++;
	avp+=frq;			// Increase counter

	if (avc==PDPackets)	// 3*256 packets of data read? (approx 1 frames worth of audio..)
	{
		d=((avp+1)/2)*(48000/(PDPackets*256));
		SOUND_DSP_Result[j]=d;
		avc=0;	// Reset counter
		avp=0;	// Reset total frequency
	}

	SOUND_DSPList[j].Value[0]=(u_int)avp;
	SOUND_DSPList[j].Value[1]=(u_int)avc;
	SOUND_DSPList[j].Value[2]=(u_int)dir;
}

/**************************************************************************
SOUND_SetDSP
	Called from the EE.
	Does 99% of the routing from EE to IOP for setting up everything to do
	with DSP.

	Requires:	slot		Required slot to use
				type		effect type
				value		value to set effect to
**************************************************************************/
void SOUND_SetDSP(int slot,int type,int value)
{
u_char Function;
int a;

	Function=SOUND_DSPList[slot].Function;		// Current DSP function is slot

	if (type==SOUND_DSP_MUTE)					// Mute/UnMute DSP effect?
		SOUND_DSPList[slot].MuteFlag=(u_char)value;		// Store Mute/UnMute
	else if (type==SOUND_INIT_DSP)				// Initialise DSP?
	{
		SOUND_InitDSPEngine(value,slot);
	}
	else if (type==SOUND_DSP_ONOFF)				// Turn core n on or off?
	{
		SOUND_DSPOnOff(value&1,(value&2)>>1);
	}
	else if (type==SOUND_DSP_CLEAR)				// Clear all effects for core n?
	{
		SOUND_ClearDSPEffects((u_int)value);			// value = Core (0 or 1)
	}
	else if (Function==SOUND_FLAG_DSP_FILTER)	// Filter?
	{
		if (type==SOUND_FILTER_LOWPASS)			// Modify frequency params?
		{
			SOUND_DSPList[slot].Value[0]=(u_int)value;	// Filter frequency
			if (SOUND_DSPList[slot].Value[3]==SOUND_FILTER_HIPASS)
				SOUND_DSPList[slot].Value[1]=(u_int)(FILMAX-value);	// Reverse frequency if hi pass (so max value is always fully open)
			else
				SOUND_DSPList[slot].Value[1]=(u_int)value;
		}
		else if (type==SOUND_FILTER_RESONANCE)	// Modify resonance?
			SOUND_DSPList[slot].Value[2]=(u_int)(RESMAX-value);	// Store resonance
		else if (type==SOUND_FILTER_TYPE)		// Modify type? (low/hi/band?)
		{
			SOUND_DSPList[slot].Value[3]=(u_int)value;
			if (value==SOUND_FILTER_HIPASS)
				SOUND_DSPList[slot].Value[1]=FILMAX-SOUND_DSPList[slot].Value[0];
		}
	}
	else if (Function==SOUND_FLAG_DSP_DISTORT)	// Distortion?
	{
		if (type==SOUND_DISTORTION_LEVEL)
		{
			SOUND_DSPList[slot].Value[0]=(u_int)value;
		}
	}
	else if (Function==SOUND_FLAG_DSP_FLANGE)	// Flange?
	{
		if (type==SOUND_FLANGE_DEPTH)
			SOUND_DSPList[slot].Value[0]=(u_int)value;
		else if (type==SOUND_FLANGE_SPEED)
		{
			a=(int)SOUND_DSPList[slot].Value[1];
			if (a<0)
				a=-a;
			if (value!=a)
				SOUND_DSPList[slot].Value[1]=(u_int)value;
		}
	}
	else if ((Function==SOUND_FLAG_DSP_COMPRESS)||(Function==SOUND_FLAG_DSP_COMPRESS_SIDE))
	{
		if (type==SOUND_COMPRESS_THRESHOLD)
		{
			SOUND_DSPList[slot].Value[0]=(u_int)value;		// Threshold 0-31 (31=0..limit = 1<<Threshold)
		}
		else if (type==SOUND_COMPRESS_RATIO)
		{
			SOUND_DSPList[slot].Value[1]=(u_int)value;		// Ratio 0-2048 (2048 = 1:1, 1024 = 2:1...)
		}
		else if (type==SOUND_COMPRESS_ATTACK)
		{
			SOUND_DSPList[slot].Value[2]=(u_int)value;		// 0-20
		}
		else if (type==SOUND_COMPRESS_RELEASE)
		{
			SOUND_DSPList[slot].Value[3]=(u_int)value;		// 0-20
		}
		else if (type==SOUND_COMPRESS_GAIN)
		{
			SOUND_DSPList[slot].Value[4]=(u_int)value;		// Gain 0-2048
		}
	}
	else if (Function==SOUND_FLAG_DSP_NOISEGATE)
	{
		if (type==SOUND_NOISEGATE_LEVEL)
		{
			SOUND_DSPList[slot].Value[0]=(u_int)value;		// NoiseGate = 0-0x7fff LEFT
			SOUND_DSPList[slot].Value[1]=(u_int)value;		// NoiseGate = 0-0x7fff RIGHT
		}
		else if (type==SOUND_NOISEGATE_RAMP)
		{
			SOUND_DSPList[slot].Value[2]=(u_int)value;		// RampSpeed = 0-16 (Default 8)
			SOUND_DSPList[slot].Value[3]=(u_int)value;		// RampSpeed = 0-16
		}

	}
	else if (Function==SOUND_FLAG_DSP_RINGMOD)
	{
		if (type==SOUND_RINGMOD_FREQ)
		{
			SOUND_DSPList[slot].Value[0]=(u_int)value;
		}
	}
}

/**************************************************************************
SOUND_WaitForDSPToComplete
	Stalls processing until the DSP thread has completed processing.
	This is used so that if any RPC comes along, it will wait for DSP to
	complete processing before doing anything (helps with syncing and stops
	glitches..)

	Requires:	Nothing
	Returns:	Nothing

**************************************************************************/
void SOUND_WaitForDSPToComplete(void)
{
int a;

	a=SOUND_DSP_COUNTER;
	do{
			DelayThread(TWO_T);	/* Wait two ticks.. */
			DelayThread(TWO_T);	/* Wait two ticks.. */
	}while(a==SOUND_DSP_COUNTER);
}

/**************************************************************************
SOUND_AllocateEEDSP
	* If sending data back to EE to process before playback *
	Allocates IOP RAM to be used for buffering PCM packets to be sent/received
	from EE RAM

	Requires:	type		SOUND_DSP_ON / SOUND_DSP_OFF (this free's the RAM)
				packets		number of packets to use (the more packets, the
							bigger the latency..)

**************************************************************************/
u_int SOUND_AllocateEEDSP(u_int type,u_int packets)
{
int oldstat;
u_int addr;
u_int addr2;
u_int addr3,addr4,addr5;
u_int size;
int i;
u_int *ptr1;
SPUDSP_TRANS *ptr3;


	size=512*2*packets;	// Size of buffer required (single buffer = 512 bytes for L,R = 1024bytes)

	if (type==SOUND_DSP_ON)
	{
		if (SOUND_DSP_PCMBuffer==0)	// Buffer already allocated?
		{
		    CpuSuspendIntr (&oldstat);
			addr = (unsigned int) AllocSysMemory (SMEM_Low, size, NULL);	/* Allocate RAM */
			addr2 = (unsigned int) AllocSysMemory (SMEM_Low, size, NULL);	/* Allocate RAM */
			addr3 = (unsigned int) AllocSysMemory (SMEM_Low, packets*4, NULL);	/* Allocate RAM */
			addr4 = (unsigned int) AllocSysMemory (SMEM_Low, packets*4, NULL);	/* Allocate RAM */

			size=sizeof(SPUDSP_TRANS);			/* Size of single entry */
			size*=(unsigned int)packets;		/* Total size required */
			addr5 = (unsigned int) AllocSysMemory (SMEM_Low, size, NULL);	/* Allocate RAM */
		    CpuResumeIntr (oldstat);

			if (((void*)addr==NULL)|			/* Allocation failed? */
				((void*)addr2==NULL)|			/* Allocation failed? */
				((void*)addr3==NULL)|			/* Allocation failed? */
				((void*)addr4==NULL))			/* Allocation failed? */
			{

			 	FreeSysMemory( (void*)addr);
			 	FreeSysMemory( (void*)addr2);
			 	FreeSysMemory( (void*)addr3);
			 	FreeSysMemory( (void*)addr4);
			 	FreeSysMemory( (void*)addr5);
				return(SOUND_DSP_OFF);
			}

		    CpuResumeIntr (oldstat);


			SOUND_DSP_PCMBuffer=addr;
			SOUND_DSP_PCMBufferTemp=addr2;
			SOUND_DSP_EEAddressList=addr3;
			SOUND_DSP_IOPAddressList=addr4;
			SOUND_DSP_FinalAddressList=(int)addr5;
			SOUND_DSP_EEPacketSize=(u_short)packets;

			ptr1=(u_int *)SOUND_DSP_EEAddressList;
			ptr3=(SPUDSP_TRANS *)SOUND_DSP_FinalAddressList;
			for (i=0;i<packets;i++)
			{
				*ptr1++=0;
				ptr3->Counter=-1;
				ptr3++;
			}
		}
		return(SOUND_DSP_ON);
	}
	else if ((type==SOUND_DSP_OFF)&&(SOUND_DSP_PCMBuffer!=0))
	{
	 	FreeSysMemory( (void*)SOUND_DSP_PCMBuffer);
	 	FreeSysMemory( (void*)SOUND_DSP_PCMBufferTemp);
	 	FreeSysMemory( (void*)SOUND_DSP_EEAddressList);
	 	FreeSysMemory( (void*)SOUND_DSP_IOPAddressList);
	 	FreeSysMemory( (void*)SOUND_DSP_FinalAddressList);
		SOUND_DSP_PCMBuffer=0;
		SOUND_DSP_EEPacketSize=0;
	}
	return(SOUND_DSP_OFF);
}

#endif	// For #if MS_DSP_ACTIVE==1
