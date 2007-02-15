///////////////////////////////////////////////////////////////////////////////
// Adaptated from the EZMPEGStr stuff
///////////////////////////////////////////////////////////////////////////////
#include <eekernel.h>
#include <libgraph.h>
#include <libipu.h>
#include <libmpeg.h>
#include <stdio.h>
#include <string.h>
#include <sdmacro.h>
#include <sifdev.h>

#include "Layers/Memory.h"
#include "Layers/mstream/streamirx.h"

#include "Layers/Debug.h"

#include "strfile.h"
#include "audiodec.h"
#include "playpss.h"

//  Definitions
#define READCHUNKSIZE   16384 * 2
#define DMACHUNKSIZE    4096          // bytes
#define PK_SIZE_PAL     (MAX_MBX * MAX_MBY_PAL * 6 + 10)
#define PK_SIZE_NTSC    (MAX_MBX * MAX_MBY_NTSC * 6 + 10)
#define P3TAGBUFF1      0
#define P3TAGBUFF2_PAL	(bound(PK_SIZE_PAL * 4, 64))
#define P3TAGBUFF2_NTSC (bound(PK_SIZE_NTSC * 4, 64))
#define FBP1_PAL        ((bound(MAX_WIDTH, 64) * bound(MAX_HEIGHT_PAL, 32)) >> 11)
#define FBP1_NTSC       ((bound(MAX_WIDTH, 64) * bound(MAX_HEIGHT_NTSC, 32)) >> 11)
#define CSCPERPIX       (0.000151f)   // number of scanlines to CSC one pixel
#define FILLBUFFMAXTIME (25)          // max scanlines that fillbuff can need
                                      // (empirical)

//  Variables
PlayPSSResources *s_mpeg = NULL;
extern bool g_wantPAL;

//  Function declarations
static int videoCallback(sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data);
static int audioCallback(sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data);
static int nodataCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data);
static int backgroundCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data);
static int errorCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data);

static int fillBuff(sceMpeg *mp,int blocking);
static void clearBackground(u_int val);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MPEGSESSION StartMpegSession(float volume)
{
	// Allocate the session
	PlayPSSResources *resources = (PlayPSSResources *)memAlloc(sizeof(PlayPSSResources), 64);
	if (!resources)
		return NULL;
	memset(resources, '\0', sizeof(PlayPSSResources));

	// Allocate resources required for playback
	MEM_SET_ALLOC_NAME("FMV - MPEG work area");
	if (g_wantPAL)
	{
		resources->mpegBuff		= (unsigned char *)memAlloc(MPEG_BUFF_SIZE_PAL, 64);
		resources->mpegBuffSize	= MPEG_BUFF_SIZE_PAL;
	}
	else
	{
		resources->mpegBuff		= (unsigned char *)memAlloc(MPEG_BUFF_SIZE_NTSC, 64);
		resources->mpegBuffSize	= MPEG_BUFF_SIZE_NTSC;
	}

	MEM_SET_ALLOC_NAME("FMV - MPEG rgb32 buffer");
	if (g_wantPAL)
		resources->rgb32 = (sceIpuRGB32 *) memAlloc(MAX_MBX * MAX_MBY_PAL * sizeof(sceIpuRGB32), 64);
	else
		resources->rgb32 = (sceIpuRGB32 *) memAlloc(MAX_MBX * MAX_MBY_NTSC * sizeof(sceIpuRGB32), 64);

	MEM_SET_ALLOC_NAME("FMV - MPEG path 3 tags");
	if (g_wantPAL)
		resources->path3tag		= (unsigned int *)memAlloc(PATH3_TAG_SIZE(MAX_MBX, MAX_MBY_PAL), 64);
	else
		resources->path3tag		= (unsigned int *)memAlloc(PATH3_TAG_SIZE(MAX_MBX, MAX_MBY_NTSC), 64);

	MEM_SET_ALLOC_NAME("FMV - MPEG read buffer");
	resources->muxBuff		= (u_char *)memAlloc(READCHUNKSIZE + 64, 64);
	resources->muxBuffSize	= READCHUNKSIZE;
	resources->muxBuffFullness = 0;

	MEM_SET_ALLOC_NAME("FMV - MPEG demux buffer");
	resources->demuxBuff		= (u_char *)memAlloc(DEMUX_BUFF_SIZE, 64);
	resources->demuxBuffSize= DEMUX_BUFF_SIZE;

	MEM_SET_ALLOC_NAME("FMV - MPEG audio buffer");
	resources->audioBuff		= (u_char *)memAlloc(AUDIO_BUFF_SIZE, 64);
	resources->audioBuffSize= AUDIO_BUFF_SIZE;

	MEM_SET_ALLOC_NAME("FMV - MPEG audio buffer");
	resources->iopBuff		= (int)sceSifAllocIopHeap(IOP_BUFF_SIZE);
	resources->iopBuffSize	= IOP_BUFF_SIZE;

	MEM_SET_ALLOC_NAME("FMV - MPEG zero buffer");
	resources->zeroBuff		= (u_char *)memAlloc(ZERO_BUFF_SIZE, 64);
	resources->iopZeroBuff	= (int)sceSifAllocIopHeap(ZERO_BUFF_SIZE);

	resources->volume = volume;

	return (MPEGSESSION)resources;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void EndMpegSession(
MPEGSESSION session)
{
	// We never got started
	if (!session)
		return;

	// Deallocate resources
	PlayPSSResources *resources = (PlayPSSResources *)session;
	memFree(resources->mpegBuff);
	memFree(resources->rgb32);
	memFree(resources->path3tag);
	memFree(resources->muxBuff);
	memFree(resources->demuxBuff);
	memFree(resources->audioBuff);
	sceSifFreeIopHeap((void *)resources->iopBuff);
	memFree(resources->zeroBuff);
	sceSifFreeIopHeap((void *)resources->iopZeroBuff);

	// Free the session
	memFree(resources);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void PlayMpegFile(
MPEGSESSION session,
char *filename,
FMVSoundTrack soundTrack,
bool &aborted,
FMVDriver::AbortFn abort,
void *abortContext)
{
	// Something is wrong
	if (!session)
		return;

	// Get at the resources
	s_mpeg = (PlayPSSResources *)session;

	// Playback mpeg file
	if (strFileOpen(filename))
	{
		playPss(strFileRead, soundTrack, aborted, abort, abortContext);
		strFileClose();
	}

	//  Clear background
	clearBackground(0x80000000);
	sceGsSyncV(0);
}

///////////////////////////////////////////////////////////////////////////////
//  Play back pss file
///////////////////////////////////////////////////////////////////////////////
void playPss(
int (*cbFileRead) (void *, int),
FMVSoundTrack soundTrack,
bool &bAborted,
FMVDriver::AbortFn abort,
void *abortContext)
{
	sceMpeg mp;
	int drawid = 1;
	sceGsDispEnv disp;

	//  Initialize IPU and MPEG library
	sceMpegInit();

	//  Initialize audio decoder
	audioDecCreate(&s_mpeg->audioDec,
							s_mpeg->audioBuff, s_mpeg->audioBuffSize,
 	 						s_mpeg->iopBuff, s_mpeg->iopBuffSize,
 	 						s_mpeg->zeroBuff, s_mpeg->iopZeroBuff, ZERO_BUFF_SIZE,
						   s_mpeg->volume);

	s_mpeg->isStarted = 0;

	//  Clear background
	clearBackground(0x80000000);

	//  Create MPEG decoder
	sceMpegCreate(&mp, s_mpeg->mpegBuff, s_mpeg->mpegBuffSize);

	sceMpegAddStrCallback(&mp, sceMpegStrM2V, 0, (sceMpegCallback) videoCallback, NULL);

	sceMpegAddStrCallback(&mp, sceMpegStrPCM, soundTrack, (sceMpegCallback) audioCallback, NULL);

	sceMpegAddCallback(&mp, sceMpegCbNodata, nodataCallback, NULL);
	sceMpegAddCallback(&mp, sceMpegCbBackground, backgroundCallback, NULL);
	sceMpegAddCallback(&mp, sceMpegCbError, errorCallback, NULL);

	//  Initialise read buffer for multiplexed pss data
	s_mpeg->muxBuffFullness = 0;

	//  Initialise buffer for demultiplexed m2v data
	s_mpeg->demuxBuffPast    = s_mpeg->demuxBuff + 16;
	s_mpeg->demuxBuffPresent = s_mpeg->demuxBuff;
	s_mpeg->demuxBuffFuture  = s_mpeg->demuxBuff + 16;

	//  Prebuffer (bounded by which of the m2v and pcm buffers fills first)
	s_mpeg->fileRead = cbFileRead;
	while (fillBuff(&mp, 1));

	//  Initilize display
	if (g_wantPAL)
	{
		sceGsSetDefDispEnv(&disp, SCE_GS_PSMCT32, DISP_WIDTH, DISP_HEIGHT_PAL, 0, 0);
		if (MAX_HEIGHT_PAL > DISP_HEIGHT_PAL)
  			disp.dispfb.DBY = (MAX_HEIGHT_PAL - DISP_HEIGHT_PAL) / 2;
	}
	else
	{
		sceGsSetDefDispEnv(&disp, SCE_GS_PSMCT32, DISP_WIDTH, DISP_HEIGHT_NTSC, 0, 0);
		if (MAX_HEIGHT_NTSC > DISP_HEIGHT_NTSC)
	  		disp.dispfb.DBY = (MAX_HEIGHT_NTSC - DISP_HEIGHT_NTSC) / 2;
	}

	if (MAX_WIDTH > DISP_WIDTH)
		disp.dispfb.DBX = (MAX_WIDTH - DISP_WIDTH) / 2;

	disp.dispfb.FBW = bound(MAX_WIDTH, 64) / 64;
	sceGsPutDispEnv(&disp);

	//  Wait for even field
	while (sceGsSyncV(0) == 1);

	//  Main loop to decode MPEG bitstream
	while ( !sceMpegIsEnd(&mp) )
	{
		bAborted = (abort && abort(abortContext) && (mp.frameCount > 10));
		if (bAborted)
			break;

		*T0_MODE  = 1 << 7 | 3;  // Count hsyncs
		*T0_COUNT = 0;

		//  Get decoded picture in sceIpuRGB32 format
		int mba = g_wantPAL ? (MAX_MBX * MAX_MBY_PAL) : (MAX_MBX * MAX_MBY_NTSC);
		while (sceMpegGetPicture(&mp, s_mpeg->rgb32, mba) < 0)
		{
    		dbgPrint("sceMpegGetPicture failed\n");
			ASSERT(false);
  		}

  		//  Initialize DMA tags for path3 (uncached access)
		if (mp.frameCount == 0)
		{
			int origin_x = (MAX_WIDTH - mp.width)/2;
			int origin_y = g_wantPAL ? ((MAX_HEIGHT_PAL - mp.height)/2) : ((MAX_HEIGHT_NTSC - mp.height)/2);

			setLoadImageTags(&s_mpeg->path3tag[P3TAGBUFF1], s_mpeg->rgb32, origin_x, origin_y, mp.width, mp.height);
			if (g_wantPAL)
				setLoadImageTags(&s_mpeg->path3tag[P3TAGBUFF2_PAL], s_mpeg->rgb32, origin_x, origin_y + MAX_HEIGHT_PAL, mp.width, mp.height);
			else
				setLoadImageTags(&s_mpeg->path3tag[P3TAGBUFF2_NTSC], s_mpeg->rgb32, origin_x, origin_y + MAX_HEIGHT_NTSC, mp.width, mp.height);

			s_mpeg->bgDuration = (int) ((float) mp.width * (float) mp.height * CSCPERPIX);

			// TJC - print out some information if we are in debug mode.
			if (g_wantPAL)
				dbgPrint("Movie: %d x %d, Hardware: %d x %d\n", mp.width, mp.height, DISP_WIDTH, DISP_HEIGHT_PAL);
			else
				dbgPrint("Movie: %d x %d, Hardware: %d x %d\n", mp.width, mp.height, DISP_WIDTH, DISP_HEIGHT_NTSC);
		}

  		//  Load decoded image to GS via path3
  		sceGsSyncPath(0, 0);
		if (g_wantPAL)
  			loadImage(drawid ? &s_mpeg->path3tag[P3TAGBUFF2_PAL] : &s_mpeg->path3tag[P3TAGBUFF1]);
		else
  			loadImage(drawid ? &s_mpeg->path3tag[P3TAGBUFF2_NTSC] : &s_mpeg->path3tag[P3TAGBUFF1]);

		//  Send audio data to IOP
		audioDecSendToIOP(&s_mpeg->audioDec);

		if (!s_mpeg->isStarted && audioDecIsPreset(&s_mpeg->audioDec))
		{
			audioDecStart(&s_mpeg->audioDec);
			s_mpeg->isStarted = 1;
		}

		int goTo = g_wantPAL ? (625 - FILLBUFFMAXTIME) : (525 - FILLBUFFMAXTIME);

		while (*T0_COUNT < goTo)
		{
			fillBuff(&mp, 0);
		}

		//  Wait until two vblank happen
		while (sceGsSyncV(0) != 1);

		//  Switch display
		if (drawid)
		{
			disp.dispfb.FBP = g_wantPAL ? FBP1_PAL : FBP1_NTSC;
			drawid = 0;
		}
		else
		{
			disp.dispfb.FBP = 0x00;
			drawid = 1;
		}
		sceGsPutDispEnv(&disp);
	}

	audioDecReset(&s_mpeg->audioDec);
	audioDecDelete(&s_mpeg->audioDec);

	//  Reset MPEG decoder to the initial state
	sceMpegReset(&mp);

	// Delete MPEG decoder
	sceMpegDelete(&mp);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int videoCallback(sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data)
{
	int availSpace;
	int spill;

	availSpace = s_mpeg->demuxBuffPresent - s_mpeg->demuxBuffPast;    // processed data
	if (availSpace < 0)
		availSpace += s_mpeg->demuxBuffSize;  // ring buffer

	if ((availSpace == 0) && (s_mpeg->demuxBuffPresent == s_mpeg->demuxBuffFuture))
	  availSpace = s_mpeg->demuxBuffSize;    // special case - buffer is empty, not full

	if (availSpace < cbstr->len)
		return 0;

	spill = (s_mpeg->demuxBuffPast - s_mpeg->demuxBuff) + cbstr->len - s_mpeg->demuxBuffSize;

	if (spill > 0)
	{
		memcpy(UncachedAddr(s_mpeg->demuxBuffPast), cbstr->data, cbstr->len - spill);
		memcpy(UncachedAddr(s_mpeg->demuxBuff), cbstr->data + (cbstr->len - spill), spill);
		s_mpeg->demuxBuffPast = s_mpeg->demuxBuff + spill;
	}
	else
	{
		memcpy(UncachedAddr(s_mpeg->demuxBuffPast), cbstr->data, cbstr->len);
		s_mpeg->demuxBuffPast += cbstr->len;
	}

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int audioCallback(sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data)
{
	u_char *pd0, *pd1;
	int d0, d1;
	int spill;

	audioDecBeginPut(&s_mpeg->audioDec, &pd0, &d0, &pd1, &d1);

	cbstr->len  -= 4;  // first four bytes are sub-stream ID
	cbstr->data += 4;

	if (cbstr->len > (d0 + d1)) return 0;

	spill = cbstr->len - d0;

	if (spill > 0)
	{
		memcpy(pd0, cbstr->data, cbstr->len - spill);
		memcpy(pd1, cbstr->data + (cbstr->len - spill), spill);
	}
	else
		memcpy(pd0, cbstr->data, cbstr->len);

	audioDecEndPut(&s_mpeg->audioDec, cbstr->len);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int nodataCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data)
{
	int dmaSize;
	int availData;

	s_mpeg->demuxBuffPresent = s_mpeg->demuxBuffFuture;

	availData = s_mpeg->demuxBuffPast - s_mpeg->demuxBuffFuture;    // data ready for processing
	if (availData < 0) availData += s_mpeg->demuxBuffSize;  // ring buffer

	while (availData < DMACHUNKSIZE)
	{
		if (fillBuff(mp, 1) == 0)
		{
			//  More data cannot be obtained from the demultiplexer so this must
			//  be the end of the stream. Round up the data to ensure there's no
			//  remainder from the quadword-multiple transfer, otherwise the
			//  decoder won't receive the MPEG end sequence code.
			availData += 15;
			break;
		}

  		availData = s_mpeg->demuxBuffPast - s_mpeg->demuxBuffFuture;   // data ready for processing
  		if (availData < 0)
			availData += s_mpeg->demuxBuffSize; // ring buffer
	}

	dmaSize = availData & ~0xf;
	if (dmaSize > DMACHUNKSIZE) dmaSize = DMACHUNKSIZE;

	if ((s_mpeg->demuxBuffPresent + dmaSize) > (s_mpeg->demuxBuff + s_mpeg->demuxBuffSize))
  		dmaSize = s_mpeg->demuxBuff + s_mpeg->demuxBuffSize - s_mpeg->demuxBuffPresent;

	//  DMA must have stopped so these fields are safe to write to
	*D4_QWC  = dmaSize / 16;
	*D4_MADR = (int) s_mpeg->demuxBuffPresent;
	*D4_CHCR = 1 | 1 << 8;               // start, normal, from memory

	s_mpeg->demuxBuffFuture = s_mpeg->demuxBuffPresent + dmaSize;

	if (s_mpeg->demuxBuffFuture >= (s_mpeg->demuxBuff + s_mpeg->demuxBuffSize))  // ring buffer
  		s_mpeg->demuxBuffFuture -= s_mpeg->demuxBuffSize;

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int backgroundCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data)
{
	int dueTime = *T0_COUNT + s_mpeg->bgDuration;

	while (*T0_COUNT < dueTime)
		fillBuff(mp, 0);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int errorCallback(sceMpeg *mp, sceMpegCbData *cbstr, void *data)
{
	sceMpegCbDataError *cberr= (sceMpegCbDataError *) cbstr;

	dbgPrint("MPEG decoding error: '%s'\n", cberr->errMessage);
	ASSERT(false);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
//  Fill the demultiplex buffers from at most READCHUNKSIZE worth of
//  multiplexed data. Returns number of bytes put into buffers.
/////////////////////////////////////////////////////////////////////////////
int old[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int cnt1=0;
static int fillBuff(sceMpeg *mp, int blocking)
{
	u_char *startPos;
	int procBytes;
	int i;

	int g;
	char f;

	f=0;
	g=0;

	if (s_mpeg->muxBuffFullness)
		startPos = s_mpeg->muxBuff + s_mpeg->muxBuffSize - s_mpeg->muxBuffFullness;
	else
	{
		do
		{
			s_mpeg->muxBuffFullness = s_mpeg->fileRead(s_mpeg->muxBuff, s_mpeg->muxBuffSize);
			if ((s_mpeg->muxBuffFullness == 0) && !blocking)
				return 0;
		} while (s_mpeg->muxBuffFullness == 0);

		i=1;
		//	startPos = muxBuff;
		//	for (i=0;i<32;i++){
		//	dbgPrint ("%x,",*startPos++);}
		//	dbgPrint("\n");

		startPos = s_mpeg->muxBuff;
	}

	procBytes = sceMpegDemuxPss(mp, (u_char *) startPos, s_mpeg->muxBuffFullness);
	s_mpeg->muxBuffFullness -= procBytes;
	//dbgPrint("preload complete2 %x\n",resources->muxBuffFullness);

	return procBytes;
}

/////////////////////////////////////////////////////////////////////////////
//  Clear background of display
/////////////////////////////////////////////////////////////////////////////
static void clearBackground(u_int val)
{
	int i;
	static u_int backTile[16*16] bss_align(64);
	u_int *p = (u_int*) UncachedAddr(backTile);

	for (i = 0; i < 16*16; i++) p[i] = val;

	sceGsSyncV(0);
	if (g_wantPAL)
		setLoadImageTagsTile(s_mpeg->path3tag, backTile, 0, 0, MAX_WIDTH, MAX_HEIGHT_PAL * 2);
	else
		setLoadImageTagsTile(s_mpeg->path3tag, backTile, 0, 0, MAX_WIDTH, MAX_HEIGHT_NTSC * 2);
	loadImage(s_mpeg->path3tag);
	sceGsSyncPath(0, 0);
}
