//-----------------------------------------------------------------------------
//                               FMV Player
//
// $Workfile: main.c $
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

#include <stdlib.h>
#include "shellCore.h"
#include "shellMpeg.h"
#include "fmvdisplay.h"
#include "fmvaudio.h"


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

static void parseCommandLine( int argc, char *argv[],
							  EFMVDisplayMode *pMode, int *pUpperFieldFirst,
							  EFMVDisplayWidescreen *pWidescreen,
							  EShellMpegMultiplexType *pMplexType,
							  EShellMpegVideoType *pVideoType,
							  EShellMpegAudioType *pAudioType,
							  int *pAudioChannel, char *name );
static void fileInitFn( int fileFnParam );
static int fileLoadFn( u_char *buff, int size, int fileFnParam );
static void idleFn( int data );


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	ShellMpegStream str;
	int iopStrBuf;
	u_int tbw, tbp;

	
	char filename[128];
	EFMVDisplayMode mode;
	EFMVDisplayWidescreen widescreen;
	int upperFieldFirst;
	EShellMpegMultiplexType mplexType;
	EShellMpegVideoType videoType;
	EShellMpegAudioType audioType;
	int audioChannel;

	parseCommandLine(argc, argv, &mode, &upperFieldFirst, &widescreen,
					 &mplexType, &videoType, &audioType, &audioChannel,
					 filename);

	shellMemoryInit(0);
	shellDmaInit();
	shellFileInit(SHELLFILE_MEDIUM_HOST0, 0, "data_ps2/irx", NULL);

	FMVDisplayInit(mode, upperFieldFirst, &tbw, &tbp);
	FMVAudioInit();

	sceSifInitRpc(0);
	sceSifInitIopHeap();

	iopStrBuf = (int) sceSifAllocIopHeap((2048 * 80) + 16);

	if(!shellFileStInit( 80, 5, ((iopStrBuf + 15) / 16) * 16))
		ASSERT(0, "Couldn't initiate streaming");

			
	shellMpegInit();
	shellMemoryBindHeap(SHELLMEMORY_HEAP1);


	while(1)
	{
		ShellDmaTag *tagsA, *tagsB;
		
		if(!shellMpegCreate(&str, mplexType, videoType,
						   audioType, audioChannel,
						   fileInitFn, fileLoadFn, (int) filename,
						   1, tbw, SHELLMPEG_OUTPUT_PATH3TAGS))
			ASSERT(0, "Couldn't initialise stream");

		shellCoreSetIdleRoutine(idleFn, (int) &str);

		FMVAudioCreate(audioType, &str.sshd);
		FMVDisplayCreate(str.width, str.height, widescreen);

		// Queue up two video frames and a full audio buffer in advance to
		// ensure a smooth start
		
		if(!shellMpegGetVideo(&tagsB, tbp)) goto abort;
		if(!shellMpegGetVideo(&tagsA, tbp)) goto abort;
		FMVDisplayAddFrame(tagsB);


		while(!shellMpegIsEnd() && !FMVAudioIsStartable())
		{
			shellMpegRecharge();
			shellMpegGetAudio(FMVAudioSend, 0);
		}

		// Simultaneously begin audio and video

		FMVDisplayStart();
		FMVAudioStart();
		
		// Loop

		
		while(!shellMpegIsEnd())
		{
			FMVDisplayAddFrame(tagsA);
		
			if(!shellMpegGetVideo(&tagsA, tbp)) goto abort;
			shellMpegGetAudio(FMVAudioSend, 0);
		}

		
		// Streaming finished; flush through video buffer
		
		FMVDisplayAddFrame(tagsA);


	abort:

		while(!FMVDisplayFlush()) shellMpegGetAudio(FMVAudioSend, 0);
		
		// Close
		
		shellMpegDelete();

		FMVAudioDestroy();
		FMVDisplayDestroy();
		
		shellFileStStop();
		shellMemoryResetHeap();
	}
}

//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

static void parseCommandLine( int argc, char *argv[],
							  EFMVDisplayMode *pMode, int *pUpperFieldFirst,
							  EFMVDisplayWidescreen *pWidescreen,
							  EShellMpegMultiplexType *pMplexType,
							  EShellMpegVideoType *pVideoType,
							  EShellMpegAudioType *pAudioType,
							  int *pAudioChannel, char *name )
{
	int n, err = 0;

	// Defaults
	*pMode = FMV_NTSC;
	*pUpperFieldFirst = 1;
	*pWidescreen = FMV_WS_NONE;
	*pMplexType = SHELLMPEG_MULTIPLEX_PSS;
	*pVideoType = SHELLMPEG_VIDEO_M2V_CSCIPU;
	*pAudioType = SHELLMPEG_AUDIO_PCM;
	*pAudioChannel = 0;

	name[0] = 0;

	for (n = 1; !err && n < argc; n++)
	{
		if     (!strcmp(argv[n], "-pal"))         *pMode = FMV_PAL;
		else if(!strcmp(argv[n], "-ntsc"))        *pMode = FMV_NTSC;
		else if(!strcmp(argv[n], "-palfromntsc")) *pMode = FMV_PAL_FROM_NTSC;
		else if(!strcmp(argv[n], "-480p"))        *pMode = FMV_480P;
		else if(!strcmp(argv[n], "-lowerfirst"))  *pUpperFieldFirst = 0;
		else if(!strcmp(argv[n], "-hcrop"))       *pWidescreen = FMV_WS_H_CROP;
		else if(!strcmp(argv[n], "-hletterbox"))  *pWidescreen = FMV_WS_H_LETTERBOX;
		else if(!strcmp(argv[n], "-vcrop"))       *pWidescreen = FMV_WS_V_CROP;
		else if(!strcmp(argv[n], "-vletterbox"))  *pWidescreen = FMV_WS_V_LETTERBOX;

		else if(!strcmp(argv[n], "-ipu"))
		{	
			*pMplexType = SHELLMPEG_MULTIPLEX_NONE;
			*pVideoType = SHELLMPEG_VIDEO_IPU_STANDARD;
			*pAudioType = SHELLMPEG_AUDIO_NONE;
		}

		else if(!strcmp(argv[n], "-ipucolumns"))
		{	
			*pMplexType = SHELLMPEG_MULTIPLEX_NONE;
			*pVideoType = SHELLMPEG_VIDEO_IPU_COLUMNS;
			*pAudioType = SHELLMPEG_AUDIO_NONE;
		}

		else if(!strcmp(argv[n], "-m2v"))
		{	
			*pMplexType = SHELLMPEG_MULTIPLEX_NONE;
			*pVideoType = SHELLMPEG_VIDEO_M2V_CSCIPU;
			*pAudioType = SHELLMPEG_AUDIO_NONE;
		}

		else if(!strcmp(argv[n], "-pss"))
		{	
			*pMplexType = SHELLMPEG_MULTIPLEX_PSS;
			*pVideoType = SHELLMPEG_VIDEO_M2V_CSCIPU;
		}

		else if(!strcmp(argv[n], "-noaudio"))
			*pAudioType = SHELLMPEG_AUDIO_NONE;

		else if(!strcmp(argv[n], "-pcm"))
		{
			*pAudioType = SHELLMPEG_AUDIO_PCM;
			if(++n == argc) err = 1;
			else *pAudioChannel = atoi(argv[n]);			
		}

		else if(!strcmp(argv[n], "-adpcm"))
		{
			*pAudioType = SHELLMPEG_AUDIO_ADPCM;
			if(++n == argc) err = 1;
			else *pAudioChannel = atoi(argv[n]);			
		}

		else if(!strcmp(argv[n], "-spdif"))
		{
			*pAudioType = SHELLMPEG_AUDIO_SPDIF;
			if(++n == argc) err = 1;
			else *pAudioChannel = atoi(argv[n]);			
		}

		else if(argv[n][0] == '-') err = 1;
		else strcpy(name, argv[n]);
	}
	
	if (err || (!name[0]))
	{
		PRINT("Usage: <program> <input> <video> <audio> fileName",
			  argv[0]);

		PRINT("Input options:");
		PRINT(" -ipu");
		PRINT(" -ipucolumns");
		PRINT(" -m2v");
		PRINT(" -pss");
		PRINT("");
		PRINT("Display options:");
		PRINT(" -pal [-lowerfirst]");
		PRINT(" -ntsc  [-lowerfirst]");
		PRINT(" -palfromntsc");
		PRINT(" -480p");
		PRINT("");
		PRINT("Widescreen options:");
		PRINT(" -hcrop");
		PRINT(" -hletterbox");
		PRINT(" -vcrop");
		PRINT(" -vletterbox");
		PRINT("");
		PRINT("Audio options:   (n is channel number)");
		PRINT(" -noaudio");
		PRINT(" -pcm n");
		PRINT(" -adpcm n");
		PRINT(" -spdif n");
		PRINT("");
		
		PRINT("Default options are '-pss -ntsc -pcm 0'");
		
		while(1)
			;
	}
}

static void idleFn( int data )
{
	(void) data;

	shellMpegRecharge();
}

static void fileInitFn( int fileFnParam )
{
	char *filename = (char *) fileFnParam;

	if(!shellFileStStart(filename, 0))
		ASSERT(0, "Failed to start file streaming");
}

static int fileLoadFn( u_char *buff, int size, int fileFnParam )
{
	(void) fileFnParam;

	return 2048 * shellFileStRead((char *) buff, size / 2048, 0 );
}
