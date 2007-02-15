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
 *                            stream.h
 *                  IOP code of Multistream's API
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      5.00            Oct,31,2001      Jason Page 
 *      6.00            Nov,28,2002      Jason Page 
 *      6.10            Jan,31,2003      Jason Page 
 *      6.20            Feb,25,2003      Jason Page 
 *		7.1				Jan,6,2004		 Jason Page
 */

#ifndef MS5HEADER
#define MS5HEADER

// List of MULTISTREAM command macros. Needs to be the same as the sound.h list, so that
// commands sent from Sound.C can be decoded and executed on the IOP.
// See routine: AdpcmSetParam for details on decoding EE functions.

//#define SOUND_THREAD_PRIORITY 40	// IOP THREAD PRIORITY

#define SND_PLAY_SFX 0		  
#define SND_PLAY_STREAM 1	  
#define SND_SET_VOL_LR 2	  
#define SND_SET_PITCH 3		  
#define SND_STOP_SFX 4		  
#define SND_STOP_STREAM 5	  
#define SND_TRANSFER_SPOTFX 6 
#define SND_INIT_SPU 7		  
#define SND_CLEAR_SPOTFX 8	  
#define SND_GET_CD_STATUS 9	  
#define SND_PAUSE 10
#define SND_RESUME 11
#define SND_INIT_STREAM 12
#define SND_ALLOC_STREAM_BUFFER 13
#define SND_SET_SPU_START_ADDRESS 14
#define SND_CLOSE_STREAM_BUFFER 15
#define SND_SET_MAX_STREAM_LIMIT 16
#define SND_CHECK_STEREO_READY 17
#define SND_CANCEL_CHECK_STEREO_READY 18
#define SND_ENABLE_FX 19
#define SND_DISABLE_FX 20
#define SND_SET_FX_MASTER_VOL 21
#define SND_FX_ALL_CHANS_ON 22
#define SND_FX_ALL_CHANS_OFF 23
#define SND_FX_CHAN_ON 24
#define SND_FX_CHAN_OFF 25
#define SND_PATCH_EXTERNAL_FILE 26
#define SND_CHAIN_STREAMS 27
#define	SND_PATCH_SFX 28	//FLT!
#define SND_GET_FILE_INFO 29
#define SND_SET_FILE_INFO 30
#define SND_CREATE_FILE_INFO 31
#define SND_FREE_FILE_ID 32
#define SND_FREE_ALL_FILE_ID 33
#define SND_STORE_HOST0_FILENAME 34
#define SND_ALLOCATE_FILE_RAM 35
#define SND_FREE_FILE_RAM 36
#define SND_ALLOCATE_SFX_RAM 37
#define SND_FREE_SFX_RAM 38
#define SND_FREE_ALL_SFX_ID 39
#define SND_FREE_SFX_ID 40
#define SND_SET_FILE_OFFSET_AND_SIZE 41
#define SND_INIT_CD 42
#define SND_SET_STREAM_PARENT 43
#define SND_SET_STREAM_CHILD 44
#define SND_GET_MAX_RAM 45
#define SND_ALLOW_KON 46
#define SND_DISABLE_KON 47
#define SND_SET_INT_TRACK 48
#define SND_SET_EE_START_ADDRESS 49
#define SND_SET_IOP_START_ADDRESS 50
#define SND_GET_FILE_ID_INFO 51
#define SND_TRANS_IOP_TO_SPU 52
#define SND_CD_ERROR_RESTART 53
#define SND_CD_ERROR_PAUSE_EX 54
#define SND_CD_CHECK 55
#define SND_CD_ERROR_RESTART_EX 56
#define SND_PLAY_SFX_LOOP 57
#define SND_DISABLE_CD_CALLBACK 58
#define SND_DISABLE_SPU_CALLBACK 59
#define SND_ENABLE_SPU_CALLBACK 60
#define SND_INIT_DTS 61
#define SND_SET_KEY_ON_FLAG 62
#define SND_KEY_ON_CHAN 63
#define SND_STRM_ADD_CH 64
#define SND_SET_OUTPUT 65
#define SND_SET_EE_TRANS_ADDRESS 66
#define SND_APPEND_STREAM 67
#define SND_STREAM_LOAD_SOURCE 68
#define SND_INIT_PCM 69
#define SND_SET_PCM_TYPE 70
#define SND_RESIZE_STREAM_BUFFER 71
#define SND_SET_PCM_VOLUME 72
#define SND_INIT_WAIT 73
#define SND_SET_STREAM_START_POS 74
#define SND_PLAY_PCM 75
#define SND_STOP_PCM 76
#define SND_INIT_PCM_NO_STREAM 77
#define SND_IGNORE_VAG_PITCH 78
#define SND_CLOSE_PCM_PLAYBACK 79
#define SND_SET_IOP_DTS_INFO 80
#define SND_INTERNAL_FASTLOAD 81
#define SND_RESIZE_SPU_BUFFER 82
#define SND_OUT_ALL_CHANS_ON 83	// Thanks to Patrick Bergeron for these commands (83-86)
#define SND_OUT_ALL_CHANS_OFF 84
#define SND_OUT_CHAN_ON 85
#define SND_OUT_CHAN_OFF 86
#define SND_TV_OUTPUT_ON 87
#define SND_TV_OUTPUT_OFF 88
#define SND_PAUSE_PCM 89
#define SND_RESUME_PCM 90
#define SND_SET_SWITCH 91
#define SND_SET_PARAM 92
#define SND_SET_ADDR 93
#define SND_SET_COREATTR 94
#define SND_BLOCK_TRANS 95
#define SND_SET_VOL_SMOOTH 96
#define SND_SET_MULTI_CHAN_PITCH 97
#define SND_SET_THREAD_PRIORITY 98
#define SND_SET_MIB_END 99
#define SND_PRIORITY_STREAM 100
#define SND_PAUSE_SPU_CHANS 101
#define SND_RESUME_SPU_CHANS 102
#define SND_SET_EFFECT_ADDR 103
#define SND_ATWINMON_OPEN 104
#define SND_ATWINMON_CLOSE 105
#define SND_SET_SFX_OFFSET 106
#define SND_SULPHA 107		// SPARE!!!
#define SND_CLOSE_IOP_RESOURCES 108
#define SND_SET_FILTER 109
#define SND_SET_DSP_EFFECT 110
#define SND_SET_DSP_BUFFER 111
#define SND_SET_DSP_COUNT 112
#define SND_CLOSE_DSP 113
#define SND_SET_MULTISTREAM_VOICES 114
#define SND_SET_STREAM_BUFFER 115

#define SOUND_FILTER_FREQ 0
#define SOUND_FILTER_RESONANCE 1
#define SOUND_INIT_DSP 2
#define SOUND_COMPRESSOR_ONOFF 3
#define SOUND_DISTORTION_LEVEL 4
#define SOUND_FILTER_TYPE 5
#define SOUND_DSP_ONOFF 6
#define SOUND_DSP_CLEAR 7
#define SOUND_FLANGE_DEPTH 8
#define SOUND_FLANGE_SPEED 9
#define SOUND_COMPRESS_THRESHOLD 10
#define SOUND_COMPRESS_GAIN 11
#define SOUND_COMPRESS_ATTACK 12
#define SOUND_COMPRESS_RELEASE 13
#define SOUND_COMPRESS_RATIO 14
#define SOUND_NOISEGATE_LEVEL 15
#define SOUND_NOISEGATE_RAMP 16
#define SOUND_DSP_MUTE 17
#define SOUND_RINGMOD_LEVEL 18
#define SOUND_RINGMOD_FREQ 19

#define SOUND_FLAG_DSP_OFF 0
#define SOUND_FLAG_DSP_FILTER 1
#define SOUND_FLAG_DSP_FLANGE 2
#define SOUND_FLAG_DSP_DISTORT 3
#define SOUND_FLAG_DSP_NOISEGATE 4
#define SOUND_FLAG_DSP_COMPRESS 5
#define SOUND_FLAG_DSP_COMPRESS_SIDE 6
#define SOUND_FLAG_DSP_MIXCORES 7
#define SOUND_FLAG_DSP_RINGMOD 8
#define SOUND_FLAG_DSP_PITCH_DETECT 9

#define SOUND_FILTER_LOWPASS 0
#define SOUND_FILTER_HIPASS 1
#define SOUND_FILTER_BANDPASS 2

#define SOUND_DSP_OFF 0
#define SOUND_DSP_ON 1
#define SOUND_MAX_DSP 8

// List of hardware effect types

#define SOUND_FX_OFF 0		// size = 0x80
#define SOUND_FX_ROOM 1		// size = 0x26c0
#define SOUND_FX_STUDIO_A 2	// size = 0x1f40
#define SOUND_FX_STUDIO_B 3	// size = 0x4840
#define SOUND_FX_STUDIO_C 4	// size = 0x6fe0
#define SOUND_FX_HALL 5		// size = 0xade0
#define SOUND_FX_SPACE 6	// size = 0xf6c0
#define SOUND_FX_ECHO 7		// size = 0x18040
#define SOUND_FX_DELAY 8	// size = 0x18040
#define SOUND_FX_PIPE  9	// size = 0x3c00 

// Flag for streaming audio type
#define STREAM_AUDIO_ADPCM 0
#define STREAM_AUDIO_PCM 1

#define SOUND_FASTLOAD_BIND_RPC 0
#define SOUND_FASTLOAD_ON_OFF 1
#define SOUND_FASTLOAD_OFF 0
#define SOUND_FASTLOAD_ON 1
#define SOUND_FASTLOAD_NEXT 5
#define SOUND_FASTLOAD_INVALID_DCACHE_OFF 6
#define SOUND_FASTLOAD_INVALID_DCACHE_ON 7

// Where data is getting loaded to
#define STREAM_LOAD_SPU 0x7f	// Flag if loading data to SPU RAM
#define STREAM_LOAD_IOP 0x7d	// Use if loading data to IOP RAM
#define STREAM_LOAD_EE_LOOP 0x7b		/* Use if loading data to EE RAM and you want the file to loop*/
#define STREAM_LOAD_EE 0x7e		// Use if loading data to EE RAM
#define STREAM_LOAD_MS	0x7c	// Use if MultiStream INTERNAL loading
#define STREAM_RESIDENT 0x7f 	// The same as STREAM_LOAD_SPU

// Audio stream playback type (StreamBuffer[i].LoopInfo)
#define STREAM_LOOPING 0		// Request for stream to be looping
#define STREAM_ONCE_OFF 1		// Request for stream to be once off
#define STREAM_PCM_LOOPING 2
#define STREAM_PCM_ONCE_OFF 3
#define STREAM_READY_TO_DIE 2	// LoopInfo Flag if stream has finished loading data packets for a once-off stream

// Small SPU buffers..
#define STREAM_SPU_NORMAL 0
#define STREAM_SPU_SHORT_LOOP 1
#define STREAM_SPU_READY_TO_DIE 2
#define STREAM_SPU_SFX 3

// PCM settings
#define SOUND_MONO 0
#define SOUND_STEREO 1
#define SOUND_BITSTREAM 2

#define SOUND_PCM 0
#define SOUND_WAV 1
#define SPU_PCM_BLOCK_SIZE 512		// 256 samples L/R (*2 for 16 bit data)
#define SOUND_PCM_OFF 0
#define SOUND_PCM_ON 1
#define SOUND_PCM_BUFFER_NOT_ALLOCATED 2

// SPU DMA settings
#define MS_SPU_DMA 1		// DMA channel for IOP->SPU transfer
#define MS_REVERB_DMA 0		// DMA channel for reverb initialisation
#define MS_USER_DMA 0		// DMA channel for SOUND_TransferIOPToSPU
#define MS_PCM_DMA 0

#define MS_PCM_CORE MS_PCM_DMA
#define MS_SPU_CORE MS_SPU_DMA

// StopStream switch
#define SPU_CH 0x40				// Pass to stop stream (SPU_CH+n) using its SPU channel number

// CD/DVD settings (for calling SOUND_InitCD)
#define SOUND_CD 1				// Passed to SOUND_InitCD if using CD
#define SOUND_DVD 2				// Passed to SOUND_InitCD if using DVD
#define SOUND_SET_CALLBACK 3	// Passed to SOUND_InitCD if CD already initialised
#define SOUND_CD_SPIN_NORMAL 4	// Passed to SOUND_InitCD if using NORMAL loading mode (default)
#define SOUND_CD_SPIN_STREAM 5	// Passed to SOUND_InitCD if using STREAM loading mode
#define SOUND_DISPLAY_INFO 6	/* Passed to SOUND_InitCD (which is a bit nasty!) */

// OpenADPCMFile switches
#define STREAM_LOAD_TO_TEMP 0
#define STREAM_GET_INFO 1
#define STREAM_GET_OTHER_INFO 2
#define STREAM_GET_APPEND_INFO 3	// New 7.21 - Thanks Y_Kousu!!

// MultiStream Thread info.
#define SOUND_SETPARAMS 0x0
#define SOUND_DEV 0x00012345
#define BASE_priority  32

#define SOUND_TRAN_OFF 0
#define SOUND_TRAN_ON 1

// DTS status
#define SOUND_DTS_OFF 0
#define SOUND_DTS_ON 1
#define SOUND_DTS_START 2
#define SOUND_DTS_INIT_THREAD 3
#define SOUND_DTS_CLOSING 4
#define SOUND_DTS_READY_TO_DIE 5
#define SOUND_DTS_START_FAILED 6

// IOP->SPU Transfer flags
#define SOUND_TRANSFER 2
#define SOUND_LAST_TRANSFER 1
#define SOUND_NO_TRANSFER 0

// Stream info
#define MAX_NUMBER_OF_STREAMS 48	// Max number of stream available (one for each SPU channel)
#define STREAM_STATUS_OFF 0		// Stream not active
#define STREAM_STATUS_ON 1		// Stream active
#define SPU_BASE_ADDRESS 0x5010	// Start address of ADPCM data storage
#define SPU_MEMORY_END	0x200000	// End address of SPU RAM

#define STREAM_FIRST_HALF 0		// First half of stream buffer is playing
#define STREAM_SECOND_HALF 1	// Second half of stream buffer is playing
#define STREAM_PLAYING_SHORT_LOOP 2	// Playing a looping stream which is smaller than the stream buffer

#define STREAM_PRELOAD 2		// Whole buffer is being filled with data before key on (or 1st buffer half if interleaved)
#define STREAM_SHORT 3			// First preload is less than the size of the whole buffer.
#define STREAM_PRELOAD2 4		// Second half of buffer preloading before key on (if playing interleaved data)

#define iSTATUS_PLAYING	0		// When "playing" (ie. code checks if half a buffer is played and updates stream)
#define iSTATUS_PLAY	1		// When a stream is preloaded (stream keyon will be performed and half buffer loaded)
#define iSTATUS_INITIALISED	2	// Set to this when SOUND_PlayStream is called (interrupt will setup, and change to preTRANSFER2)
#define iSTATUS_WAIT 3			// When a stream is stopped / initialised
#define iSTATUS_RESIDENT 4		// Resident sample (SFX)
#define iSTATUS_preTRANSFER1 5	// Set to this mode once first transfer starts
#define iSTATUS_preTRANSFER2 6	// Set to this mode to request second transfer (if using interleaved data)
#define iSTATUS_KEYED_ON 7		// Stream keyed on. (once the stream has keyed on, it will change status to PLAYING)

#define SOUND_PARENT 1			// Mark stream as a PARENT
#define SOUND_CHILD 2			// Mark stream as a CHILD of a PARENT

// CD information
#define CD_STAGE_DONE 0			// This mode is set once all seeking and loading is completed
#define CD_STAGE_SEEK 1			// Set when seeking
#define CD_STAGE_READ 2			// Set when reading
#define CD_SECTOR_SIZE 2048		// Obvious..
#define CD_STAGE_CDERROR 244

// SPU KeyStatus
#define SPU_KEY_STATUS_OFF 0			// SPU channel not active
#define SPU_KEY_STATUS_ON 1				// SPU channel active
#define SPU_KEY_STATUS_USED_BY_STREAM 2	// SPU channel not active, but will be once preloaded data is ready..
#define SPU_KEY_STATUS_JUST_KEYED_ON 4	// SPU channel just keyed on (gives a bit of time before reading hardware)

// Source of data (host or cd)
#define ADPCM_DATA_ON_HOST 0	// If data is loading from host 0
#define ADPCM_DATA_ON_CD 1		// If data is loading from CD

// Loading information
#define USE_INTERNAL_LOADING 0	// Passed by user, if using MultiStreams own CD loading code
#define USE_EXTERNAL_LOADING 1	// Passed by user, if using users own cd loading code (data sent to IOP from EE)
#define CD_GET_FILE_INFO 0		// If using EXTERNAL, this flag is set to calculate loading info (then sent to EE)
#define CD_TRANSFER_DATA 1		// If using EXTERNAL, this flag is set to transfer data from IOP->SPU

#define SOUND_MIB_END_NOT_SET 1	// Used to check if MultiStream should check for end of MIB playback data.

#define SOUND_VAG_PITCH_OFF 0
#define SOUND_VAG_PITCH_ON 1

#define SOUND_SPU_UPDATE_HALF_SIZE 0
#define SOUND_SPU_UPDATE_HALF_REMAIN 1
#define SOUND_SPU_PRELOAD 2
#define SOUND_SPU_SFX 3

// SOUND_AppendStream flags
#define SOUND_APPEND_NOW 0
#define SOUND_APPEND_AT_LOOP 1

// Priority loading 
#define SOUND_FORCE_BEFORE_AUDIO 0
#define SOUND_FORCE_BEFORE_EE_STREAMS 1
#define SOUND_FORCE_SINGLE_TRANSFER 0
#define SOUND_FORCE_UNTIL_STREAM_DIES 1

#define SOUND_PRIORITY_INIT 128
#define SOUND_PRIORITY_AUDIO 64
#define SOUND_PRIORITY_DATA 128+64
#define SOUND_STREAM_PRIORITY_DEFAULT 0x100

#define SOUND_LOOP_TO_START 1		/* Required for SOUND_SetSFXPlayOffset */
#define SOUND_LOOP_TO_OFFSET 2		/* Required for SOUND_SetSFXPlayOffset */ 

//------------------------------------------------------------------
// Structures
//------------------------------------------------------------------

typedef struct SPUDSP_TRANS
{
	int	IOPAddress;
	int	EEAddress;
	int	Counter;
} SPUDSP_TRANS;

// SPU DMA structure.

typedef struct SPUDMA_TRANS
{
	u_int	IOPAddress;
	u_int	SPUAddress;
	u_int	DataSize;
	u_int	TranSize;
	u_char	Stream;
	u_char	Status;
	u_char	TranType;
	u_char	TransFlag;
} SPUDMA_TRANS;

typedef struct SPUDMA
{
	u_int	IOPAddress[2];		// IOP Address of ADPCM buffer
	int	IOPSize[2];		// Size of IOP buffer
	u_int	SPUAddress;		// SPU address of ADPCM buffer
	u_int	SPUSize;		// Size of SPU buffer
	u_int	Packets[2];		// Number of packets to transfer
	u_char	Status[2];			// Current status
	u_char	TransType[2];		// Preload etc..
	u_char	Stream;			// Stream channel which has requested transfer
	u_char	StreamType[2];		// Looping, Once off (so we know where to store loop/final markers..)
	u_int	TransCount;		// Amount of transfers completed
	u_int	TotalSize;
	u_int	FinalEnd[2];
	u_int	CurrentBuffer;
	u_int	UsedBuffer;
	u_int	SizeCount;
	u_int 	PlayHalf;
	u_int	InitIOPAddress;
	u_int	InitIOPSize;
	u_int	InitSize;
	u_int	PacketSize[2];

	u_char	CurrentStat[2];
	u_int	TranSize;
	u_int	TranRemain;
	u_int	TranPos;
	u_int	ForceSwap;
	u_int	TranHalfSize;
	u_int	TempCheck;
	u_char	Initialised;
	u_int	JP;
} SPUDMA;

typedef struct SOUND_USER_TRANSFER
{
	u_int	SPUAddress;
	u_int	IOPAddress;
	u_int	Size;
	u_char	Status;
} SOUND_USER_TRANSFER;

// Streaming data structure.

typedef struct {

	u_int	RequestLoad;
	unsigned char	NoMoreLoading;
	unsigned int	SPUAddress;			// Address of SPU data buffer
	unsigned int	IOPAddress;			// Address of IOP data buffer
	unsigned int	IOPAllocAddress; 	// Address of IOP data buffer when first allocated
	unsigned int	IOPBufferSize;		// Size of IOP data buffer
	unsigned int	IOPBufferHalfSize;	// Half size of IOP data buffer
	unsigned int	IOPCurrentRemainder; // Amount of data required to fill IOP buffer
	unsigned int	IOPCurrentAddress;		 // Address to load data from CD to IOP
	unsigned int	FileSize;			// Total file size
	unsigned int	FileStart;			// Offset into file to start reading data
	unsigned int	FileRemainder;		// Amount of data left to read in file
	unsigned int	FileSeek;			// Position through file for loading data


	unsigned int	AP_FileSize;			// Total file size
	unsigned int	AP_FileStart;			// Offset into file to start reading data
	unsigned int	AP_FileRemainder;		// Amount of data left to read in file
	unsigned int	AP_FileSeek;			// Position through file for loading data
	int				AP_FileID;				// Stream file ID
	u_int			AP_FilePlaySize;		// Size of data to play from file

	unsigned int	PlayOffset;			// Start position to start playing data
	unsigned char	Status;				// stream status (active/etc..)
	unsigned char	iStatus;			// internal playing status (preloading, playing..)
	unsigned char	ResidentFlag;		// 0 = Stream, otherwise, STREAM_LOAD_SPU,STREAM_LOAD_EE or STREAM_LOAD_IOP
	unsigned char	PlayingHalf;		// which half is being played (0 or 1). 3 used for final load (STREAM_SHORT)
	unsigned char	SPUChannel;			// SPU channel to play stream on
	unsigned char	LoopInfo;			// Looping/Onceoff/ReadyToDie(final load complete)/Resident (SFX)
	short			VolL;				// Left Volume
	short			VolR;				// Right Volume
	short			Pitch;				// Stream pitch
	int				FileID;				// Stream file ID
	unsigned int	SectorStart;		// lsn of the first sector of this file
	int				SPUCurrent;			// Current SPU playback address
	int				KeyStat;			// Current SPU keystatus
	int				AR;					// Attack/Release
	int				USR_SPUChannel;		// User values (these are then copied into other areas on initialisation)
	int				USR_StrID;			// File ID
	int				USR_Pitch;			// Pitch
	int				USR_LInfo;			// Loop info
	int				USR_AR;				// Attack / Release
	unsigned int	AllocSize;			// Stream buffer size
	unsigned int	AllocHalfSize;		// Stream buffer half size
	unsigned int	MaxSize;			// Original Stream buffer half size
	unsigned int	AllocSPUAddress;	// Original Stream SPU buffer address (so Child can point to it)
	int				Error;				// Stream CD ERROR flag (0=no error,1=error)
	int				Source;				// Either ADPCM_DATA_ON_HOST or ADPCM_DATA_ON_CD
//	u_int			handle;				// File ID
	int				ChainList[9];		// Holds FileID to stream
	u_int			ChainFileSeek[9];	// Holds offset into file to start streaming
	u_int			ChainFileSize[9];	// Holds size of data to stream
	u_int			ChainPlaySize[9];	// Holds size of data to stream
	u_short			ChainPos;			// Current position through chain
	u_char			ChainCount;			// Number of requested files to chain
	u_char			ParentChildFlag;	// Set to SOUND_PARENT or SOUND_CHILD if either..(never both)
	u_char			TransCounter;		// Increase every SPU transfer (keep parent/child synced)
	int				ParentStream;		// Parent stream channel of a child stream.
	u_int			IntTransOffset;		// Interleaved data offset into IOP track to copy correct data to SPU
	u_int			IntTransSize;		// Size of data to copy from IOPBuffer+IntTransOffset to SPU address
	u_char			IntKeyOn;			// KeyOn Flag, set if all interleaved streams are keyed on at the same time.
	u_int			core0;				// core0 KeyON status
	u_int			core1;				// core1 KeyON Status
	char			AllowKON;			// 1=NoKeyOn after stream preloaded (wait for SOUND_AllowKeyOn)
	u_int			LastTranSize;		// Size of data left to be loaded for stream
	u_int			LastBufferSize;		// Size of data currently loaded by stream
	u_int			FilePlaySize;		// Size of data to play from file
	u_int			TempSeek;			// Seek offset into file (set in OpenADPCMFile)
	u_int			TempFileSize;		// Total file size (set in OpenADPCMFile)
	u_int			TempPlaySize;		// Total play size (set in OpenADPCMFile)
	u_char			MemoryAllocated;	// Flag (0=no StreamBuffer memory allocated, 1=alloced)
	char			KeyOffFlag;			// 1=Current SPU channel will "loop" to start of stream data
	char			LoadSource;			// USE_INTERNAL_LOADING or USE_EXTERNAL_LOADING
	char			LoadSource2;			// USE_INTERNAL_LOADING or USE_EXTERNAL_LOADING
	u_char			MIBEnd;				// 1=Last data packet before looping, 0 = not last packet
	u_int			MIBLoop;			// Offset through data for setting loop marker
	u_int			MIBAddress;			// IOP start address of last MIB packet before looping
	u_int			LastLoadAddress;	// Last IOP address data was loaded to
	u_short			SPUChannel2[3];		// 2nd SPU channel number (see SOUND_StreamAddSPUChannel)
	u_short			VolL2[3];				// 2nd SPU channel volume (Left)
	u_short			VolR2[3];				// 2nd SPU channel volume (Right)
	u_short			Ch2Flag;			// 1=2nd SPU channel active
	u_int			TranSize;			// Total data currently transferred to SPU RAM from IOP
	u_char			VAGPitch;			// Flag (1=Use VAG header info for stream pitch, 0 =don't)
	u_int			TotalTranSize;	// Total data currently transferred to EE from IOP
	u_char			EEAllow;			// For FAST INTERNAL loading
	u_char			Priority;			// Streams priority
	u_int			Priority2;			// Streams priority
	u_char			PriorityRequest;	// Streams priority Flag. 0 = No user priority request made, 1= request made..
	SPUDMA			DMAInfo;			// Info for small SPU buffer handling
	u_char			Chld;
	u_char			ChldWait;
	u_int			SPUBufferSize;		// Size of SPU RAM buffer (initial value = half IOP buffer)
	u_int			LSAXLowAddr;
	u_int			LSAXHiAddr;
	u_int			LSAXLowAddr_Old;
	u_int			LSAXHiAddr_Old;
	u_char			LSAXSwap;
	u_char			LSAXSwapOld;
	u_int			LastSPUAddr;
	u_int			SamplesPlayed;
	u_int			FastLoadCounter;
	u_int			MIBEndOffset;
	u_int			ATWinFile;

	u_char			NextLoad;
	u_int			NextFileID;
	u_int			NextAddress;
	u_int			NextFileID_FOAPS;
	u_int			NextFileOffset_FOAPS;
	u_int			NextFileSize_FOAPS;

	char			AppendFlag;
} STREAMDATA;

#define SOUND_FLAG_KEYOFF 1			// KeyOffFlag
#define SOUND_FLAG_ALLOW_KEYON 2	// AllowKON
#define SOUND_FLAG_MIB_END 4		// MIBEnd
#define SOUND_FLAG_CH2 8			// Ch2Flag
#define SOUND_FLAG_VAG_PITCH 16		// VAGPitch
#define SOUND_FLAG_CD_ERROR 32		// Error
#define SOUND_FLAG_SOURCE 64 		// Source

// Smooth volume changing..
typedef struct SOUND_SMOOTH_VOL_FADE
{
	int		CurrentVol;
	int		DestVol;
	u_char	Status;
	u_char	RevPhase;
	u_char	FadeDir;
} SOUND_SMOOTH_VOL_FADE;


// File info structure.

typedef struct ADPCMFILE
{
	int		ID;							// File ID
	u_int	size;						// Total size of file
	u_int	offset;						// V5 (Original offset set in SOUND_SetFileInfo)
	u_int	PlayOffset;					// V5 (Offset into file to start playing stream / Loading data)
	u_int	PlaySize;					// V5 (Size of data to play / stream)
	u_int	lsn;						// LSN of data (if on CD)
	int		Source;						// Host or CDROM...
	char	*FileName;					// V5 If HOST Loading, pointer to address of filename string
	u_short	MIBTracks;
	u_char	OpenFlag;					// V6 ATWINMON speedup
	int	OpenID;							// V6 ATWINMON speedup
} ADPCMFILE;

// Spot FX info structure.

typedef struct ADPCM_SPOTINFO
{
	int	ID;				// Copied from main structure (ie. ID 0=BallKick, 1=GoalCheer...)
	u_int	SPUAddress;		// SPU address of sample
	u_int	PlayOffset;
	u_short	LoopFlag;
} ADPCM_SPOTINFO;

// IOP->SPU Transfer
typedef struct SOUND_TRANS
{
	u_int IOPAddr;			// IOP Address
	u_int Size;				// Size of transfer
	u_int SPUAddr;			// SPU Address
	u_char TranMode;		// DTS on/off
	u_char Status;
volatile	u_char TranStatus;		// 1 = DTS transfering MultiStream data, 0 = DTS transfer complete
} SOUND_TRANS;

// Timer structure.
typedef struct TimerCtx {
    int thread_id;
    int timer_id;
    int count;
} TimerCtx;

typedef void (*cdFunc) (int n);

extern volatile int SPU_DMA_TRANS_FLAG;	//=0;	// Halts checking of doublebuffering as SPU DMA transfer currently in operation.

extern int MS_RPCThreadID;

int TimerFunc( void *common);
void startTimer( TimerCtx* timer );
void setTimer( TimerCtx* timer );
void SOUND_ChainStreams(u_int Chan,int Count,u_short *data);
void SOUND_EnableEffects(int core, int type, int depl, int depr, int delay, int feedback );
void SOUND_DisableEffects(int core);
void SOUND_SetEffectMasterVolume(int core, int voll,int volr);
void SOUND_AllChannelEffectsOff(int core);
void SOUND_AllChannelEffectsOn(int core);
void SOUND_SetChannelEffectOff(int channel);
void SOUND_SetChannelEffectOn(int channel);
void SOUND_PatchSFX(int ID,u_int SPUAddr);
void SetIOPRequest(int stream);
int sce_adpcm_loop (void);
void* ProcessEECommand (unsigned int command, void *data_);
void SOUND_InitSPU ();
void SOUND_AllocateStreamBuffer(int i,u_int spuaddr,u_int size);
void SOUND_SetStreamBuffer(int strm, u_int iopaddr, u_int iopsize, u_int spuaddr, u_int spusize);
void AdpcmSetMasterVolume (int core, unsigned int vol);
int OpenADPCMFile(int Stream,int StreamID,int tempflag);
int LoadDataFromDisk(int i,int Size, cdFunc endfunc);
int makeMyThread(u_int priority);
int SOUND_InitStreamData(int type,int maxfiles, int maxsfx,u_short priority);
int StartStream(int Stream,int SPUChannel,int FileID, int LoopInfo,int AR);
int SOUND_PlayStream(int SPUChannel,int VolL,int VolR,int FileID, int Pitch, int LoopInfo,int AR);
int _AdpcmDmaInt (int ch, void *common);
int _CdStreamInt ();
int StreamADPCM();
void UpdateStream (int i,int Size,int BufferHalf, cdFunc endfunc);
int SOUND_GetStatus(void);
void SetVoiceData(int Channel,int Pitch,int VolL,int VolR,int SPUAddress, int AR,int flag);
int GetFileID(int ID);
void SOUND_PlaySFX(int ID,int Channel,int VolL,int VolR,int Pitch,int AR,int Loop);
ADPCM_SPOTINFO* GetSFXSlot(int ID);
void SOUND_SetChannelVolume(int Channel,int VolL,int VolR,int flag);
void SOUND_SetChannelPitch(int Channel,int Pitch);
void SOUND_Resume(void);
void SOUND_Pause(void);
void translate_cderror(int cderror);
void CDError_ResumeStreams(void);
void CDError_PauseStreams(void);
int HandleEERequest(int i, int child);
void CheckTwoStreamsStatus(int channel1,int channel2,u_int Pitch);
void InitCheckStereoStatus(int channel1,int channel2,u_int pitch);
void ClearCheckStereoStatus(void);
void GetChannelStatus(void);
void StopStream(int Ch,int KeyOffFlag);
void SOUND_StopStream(int i,int KeyOffFlag);
void AdpcmSetParam(short *jdata);
void SOUND_GetMaxIOPRAM(void);
int SOUND_GetFileInfo(u_short count,u_short *filename);
int SOUND_SetFileInfo(int id,int lsn,u_int size,int source, u_int offset);
void SOUND_CreateFileInfo(int id, u_int offset, u_short *filename);
void SOUND_FreeFileID(int ID);
void SOUND_FreeAllFileIDs(void);
void SOUND_AllocateFileMemory(int maxfiles);
void SOUND_FreeFileMemory(void);
void SOUND_FreeSpotFXMemory(void);
void SOUND_AllocateSpotFXMemory(int maxfx);
void SOUND_FreeAllSpotFXIDs(void);
void SOUND_FreeSpotFXID(int ID);
int SOUND_GetSpotFXSlot(int id);
void SOUND_StoreHOST0FileName(int id,u_short *filename);
void SOUND_SetFileOffsetAndPlaySize(int ID, u_int offset, u_int size);
void SOUND_InitCD(u_short type);
void SOUND_SetStreamParent_Int(int S,int Track,u_int TrackSize);
void SOUND_SetStreamChild_Int(int CP, int SPUChan,int Track,int VolL,int VolR,u_int SPUAddr);
int SOUND_PerformKeyOn(int i);
void SOUND_UpdateMemoryEndPosition(int i);
int SOUND_CheckForEERequest(void);
void SOUND_HandleCDErrorChecking(void);
void SOUND_GetActiveStreamInfo(void);
void SOUND_HandlePreTransfer(int i);
void SOUND_CheckForHalfBufferPlayed(int i);
void SOUND_CheckStreamSPUStatus(int i);
void cdend_PreloadDataNowInIOP();	// Preloaded stream loaded to IOP.
void cdend_PreloadedDataNowInSPU();	// Preloaded stream transfered to SPU.
void cdend_KeyedOn();	// First half loaded to IOP. Keyon Stream.
void cdend_FirstHalfPlayed();	// First half played. Half transfered to SPU, New half loaded to IOP.
void cdend_SecondHalfPlayed();	// Second half played. Half transfered to SPU, New half loaded to IOP.
void cdend_InterleavedSecondTransfer();	// Interleaved preload.
void cdend_IntPreloadedDataNowInSPU();	// Interleaved data now fully preloaded
int SOUND_GetCore(int Channel);
int SOUND_GetSPUChannel(int Channel);
int SOUND_CheckSPUMemory(u_int Address,u_int Size);
int SOUND_CheckStream(int stream);
void SOUND_UpdateParentChild(int i);
void SOUND_StreamTransferToSPU(int i,u_int Size,u_int BufferHalf);
void SOUND_StreamPreLoading(int i, u_int Size);
void SOUND_StreamReadyToDie(int i,int BufferHalf,u_int Size,int flag);
void SOUND_StreamAllowKeyOn(u_int i);
void SOUND_StreamDisableKeyOn(u_int i);
void SOUND_SetStreamTrack_Int(int i,int track);
u_int SOUND_GetFileLSN(int i);
int SOUND_TransferDataToEE(unsigned int source,unsigned int dest, unsigned int size);
void SOUND_GetFileIDInfo(u_int count, int FileID);
int SOUND_TransIOPToSPU(u_int IOPAddress,u_int SPUAddress,u_int Size);
int SOUND_CheckUpdate(void);
void SOUND_RestartFromCDError(void);
void SOUND_CopyValues(int i,int dir);
void SOUND_CheckCDError(void);
void SOUND_EXTERNAL_CDErrorPause(void);
void SOUND_EXTERNAL_CDErrorRestart(void);
void SOUND_FastPitch0(void);
void SOUND_UpdateLoopingSFX(void);
void SOUND_DisableCDCallback(void);
void SOUND_DisableSPUCallback(void);
void SOUND_EnableSPUCallback(void);
int DTS_start(void);
void SOUND_HandleSPUTransfer(int i,u_int IOPAddr, u_int SPUAddr, u_int Size,int TranType);
void SOUND_InitDTSMode(void);
void SOUND_KeyOnChannel(int Channel);
void SOUND_StreamAddSPUChannel(int StrmChan,int SPUChan,int VolL,int VolR);
void SOUND_StopDTS(int flag);
void SOUND_SetDTSMode(int mode,int flag);
void SOUND_SetOutput(int flag);
void SOUND_EnablePCM(void);
void SOUND_EnableBitStream(void);
void SOUND_ModifyEETransferAddress(int i,u_int addr);
int SOUND_AppendStream(int i, int FileID,int flag);
void SOUND_SetStreamLoadSource(int stream,int source);
void SOUND_CopyEXTERNALData(int i);
void SOUND_SetPCMType(int Format, int Stereo);
void SOUND_InitPCMPlayback(int stream,int format,int stereo);
void SOUND_TransferPCM (u_int source, u_int dest,u_int size);
void SOUND_StartPCM(void);
void SOUND_ClosePCMPlayback(void);
void SOUND_StopPCM(void);
void SOUND_PausePCM(void);
void SOUND_PCMOnceOff(int i);
void SOUND_ResumePCM(void);
void SOUND_ResizeStreamBuffer(int i,u_int size);
void _BgmRaw2Spu (u_long *src, u_long *dst,  u_long block_count);
void SOUND_SetPCMVolume(int VolL,int VolR);
void SOUND_HandleEEWait(void);
void SOUND_SetStreamStartPosition(int i, u_int offset);
void SOUND_InitPCMPlaybackNoStream(u_int size, int mode);
u_int CheckVAGHeader(int i,u_int IOPAddr);
void SOUND_StreamIgnoreVAGPitch(int i);
void SOUND_DelayKeyOnOff(void);
void SOUND_HandleCDEvent(void);
void SOUND_InitINTERNALFastLoad(int type,int flag);
u_int SOUND_FastINTERNAL_RPC(int i,u_int totalsize,u_int lastsize,u_int status);
void USR_HALT_MULTISTREAM_CLR(void);
void USR_HALT_MULTISTREAM_SET(void);
void SOUND_StartLoad(int i,cdFunc endfunc);
void cdend_LoadedSecondHalf(void);
void cdend_LoadedFirstHalf(void);
void SOUND_InitLoadRequest(int i);
void SOUND_SetLoadRequest(int i,u_int IOPAddr,u_int Size,u_int FinalEnd);
void SOUND_SmallSPUTrans(void);
void SOUND_SetChannelOutputOn(int channel);
void SOUND_SetChannelOutputOff(int channel);
void SOUND_AllChannelOutputOn(int core);
void SOUND_AllChannelOutputOff(int core);
void SOUND_SetTVOutputOn(void);
void SOUND_SetTVOutputOff(void);

void SOUND_SPUFinalEndHalf(int i,int b, SPUDMA *ptr);
void SOUND_SPUEndHalfRemain(int i,int b, SPUDMA *ptr);
void SOUND_SPUHalfRemainTransInit(int i,int b,SPUDMA *ptr);
void SOUND_SPUEndHalfSize(int i,int b, SPUDMA *ptr);
void SOUND_SPUHalfSizeTransInit(int i,int b,SPUDMA *ptr);
void SOUND_SPUTransferComplete(int i,int b, SPUDMA *ptr);
int SOUND_SPUCheckForHalfUpdate(int i,int b, SPUDMA *ptr);
void SOUND_SPUCheckForOutOfRange(int i, SPUDMA *ptr);
u_int SOUND_SPUGetStreamAddress(int i);
int SOUND_SPUCheckValidPlaying(int i, SPUDMA *ptr);
int SOUND_SPUGetTranSizeAndRemain(int b, SPUDMA *ptr);
int SOUND_SPUCheckParentChild(int i);
int SOUND_SPUInitSFXTransfer(int i,int b, SPUDMA *ptr);
int SOUND_SPUInitHalfRemain(int i,int b, SPUDMA *ptr);
int SOUND_SPUInitHalfSize(int i,int b, SPUDMA *ptr);
int SOUND_SPUInitPreload(int i,int b, SPUDMA *ptr);
void SOUND_SPUEndSFX(int i,int b, SPUDMA *ptr);
void SOUND_SPUPreloadEnd(int i,int b, SPUDMA *ptr);
void SOUND_SPUHandleEndTransfer(int i,int b, SPUDMA *ptr);
int SOUND_SPUProcessUpdate(int i,int b, SPUDMA *ptr);
void SOUND_UpdateSPUTrans(u_int size);
int SOUND_CheckSPUTransfer(void);
void SOUND_SetSPUTransRequest(u_int IOPAddr,u_int SPUAddr,u_int Size,u_int i);
void cdend_LoadedNonAudio(void);
void SOUND_CalcPlayingTime(void);
ADPCMFILE* SOUND_FindFile(int FileID);

void SOUND_SetSwitch(u_short coreANDcommand, u_int bitmask);
void SOUND_SetParam(u_short coreANDcommandANDchannel,u_short value);
void SOUND_SetAddr(u_short coreANDcommandANDchannel,u_int address);
void SOUND_SetCoreAttr(u_short coreANDcommand,u_short value);
void SOUND_BlockTrans(u_short DMAChan,u_short Flags, u_int IOPAddr,u_int Size,u_int Offset);

int SOUND_CheckSdrOutOfRange(u_short com,u_int low,u_int high);

int SOUND_SmoothFindClosest(int val);
int SOUND_ModifySmoothVol(int i,int flag);
void SOUND_SetChannelVolumeSmooth(int i,int voll,int volr);
void SOUND_UpdateSmoothVols(void);
void SOUND_InitSmoothFade(void);
void SOUND_SmoothFadeOff(int i);
void SOUND_SetMultiChannelPitch(u_short Pitch,u_int core0,u_int core1);
void SOUND_SetThreadPriority(int priority);
void SOUND_SetMIBEndOffset(int i, u_int Offset);
void SOUND_PauseSPUChannels(u_int First,u_int Last);
void SOUND_ResumeSPUChannels(u_int First,u_int Last);
void SOUND_SetEffectBufferAddress(u_int address1,u_int address2);
int SOUND_GetNextStreamPriority(u_int j,u_int end);
int SOUND_SortStreamPriorities(void);
void SOUND_SetStreamPriority(u_int i,u_int priority);
void SOUND_ATWinMonOpenFile(u_int id);
void SOUND_ATWinMonCloseFile(u_int id);
void cdend_StreamFullBuffer();
void SOUND_SetSFXPlayOffset(u_int ID, u_short LoopFlag,u_int Offset);

void SOUND_CloseIOPResources(void);
void SOUND_SetCalcPlayingTime(int i, u_int low,u_int hi);
void SOUND_UpdateStreamTimer(int i);
int SOUND_GetStreamSPUChan(int i);
void SOUND_DisplayStats(void);
void SOUND_CheckForNextLoad(int i);

void SOUND_SetMultiStreamVoices(u_int core0, u_int core1);

int _AdpcmSetMarkClearEND(int i,u_int addr, u_int size);
int _AdpcmSetMarkRESTART(int i,u_int addr, u_int size);
int _AdpcmSetMarkRESTART2(int i,u_int addr, u_int size);
int _AdpcmSetMarkEND(int i,u_int addr, u_int size);
int _AdpcmSetMarkSTART(int i,u_int addr);
int _AdpcmSetMarkFINAL(int i,u_int addr, u_int size);
int SOUND_CheckADPCMSize(int i,u_int addr);
int SOUND_GetParent(int i);

int msabs (int a);
void CheckToWakeRPC(void);
//void CheckToWakeRPC(int flag);
void SOUND_CloseStreamBuffer(int i);

void SOUND_EnableRPC(void);
void SOUND_DisableRPC(void);


#endif

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADSTUFF AFTER THIS */