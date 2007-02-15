///////////////////////////////////////////////////////////////////////////////
// Adaptated from the EZMPEGStr stuff
///////////////////////////////////////////////////////////////////////////////
#ifndef PLAYPSSI_H
#define PLAYPSSI_H

#define DISP_WIDTH  640
#define DISP_HEIGHT_PAL		512
#define DISP_HEIGHT_NTSC	448

#define MAX_WIDTH				720    // See readme
#define MAX_HEIGHT_PAL		512
#define MAX_HEIGHT_NTSC		480

#define MAX_MBX         (MAX_WIDTH/16)
#define MAX_MBY_PAL     (MAX_HEIGHT_PAL/16)
#define MAX_MBY_NTSC    (MAX_HEIGHT_NTSC/16)

#define ZERO_BUFF_SIZE  (0x800)

#define bound(val, x) ((((val) + (x) - 1) / (x))*(x))
//#define min(x, y)	(((x) < (y))? (x): (y))

#define bss_align(val) \
    __attribute__ ((aligned(val))) __attribute__ ((section (".bss")))
#define data_align(val) __attribute__ ((aligned(val)))

#define UNCMASK 0x0fffffff
#define UNCBASE 0x20000000
#define DmaAddr(val) \
    ((void*)((unsigned int)(val) & UNCMASK))
#define UncachedAddr(val) \
    ((void*)(((unsigned int)(val) & UNCMASK) | UNCBASE))

#define PATH3_TAG_SIZE(x, y) (4 * 2 * bound(((x) * (y) * 6 + 10) * 4, 64))

#define MPEG_BUFF_SIZE_PAL	 (SCE_MPEG_BUFFER_SIZE(MAX_WIDTH, MAX_HEIGHT_PAL))
#define MPEG_BUFF_SIZE_NTSC (SCE_MPEG_BUFFER_SIZE(MAX_WIDTH, MAX_HEIGHT_NTSC))

#define DEMUX_BUFF_SIZE (192 * 1024)	//192
#define IOP_BUFF_SIZE   (12288 * 2) // 512 * 48
#define AUDIO_BUFF_SIZE (IOP_BUFF_SIZE * 2)

// Resources required for PSS playback
typedef struct
{
	u_char		*mpegBuff;
	int			mpegBuffSize;
	sceIpuRGB32	*rgb32;
	u_int			*path3tag;
	u_char		*muxBuff;
	int  			muxBuffFullness;
	int			muxBuffSize;
	u_char		*demuxBuff;
	int			demuxBuffSize;
	u_char		*audioBuff;
	int			audioBuffSize;
	int			iopBuff;
	int			iopBuffSize;
	u_char		*zeroBuff;
	int			iopZeroBuff;
	float       volume;

	int 			isStarted;
	int 			(*fileRead) (void *, int);
	int 			bgDuration;
	AudioDec 	audioDec;

	//  Buffer for demultiplexed m2v data (uncached access)
	//  Regions: times are relative to DMA and circular wrt BuffStart, BuffSize
	//  demuxBuffPresent -> demuxBuffFuture   = Area currently being processed
	//  demuxBuffFuture  -> demuxBuffPast     = Demuxed data ready for processing
	//  demuxBuffPast    -> demuxBuffPresent  = Processed data
	u_char 		*demuxBuffPast;
	u_char 		*demuxBuffPresent;
	u_char 		*demuxBuffFuture;
} PlayPSSResources;

// ldimage
void setLoadImageTags(u_int *tags, void *image,	int x, int y, int  width, int height);
void setLoadImageTagsTile(u_int *tags, void *image, int x, int y, int  width, int height);
void loadImage(u_int *tags);

void playPss(int (*cbFileRead) (void *, int), FMVSoundTrack soundTrack, bool &bAborted, FMVDriver::AbortFn abort, void *abortContext);

#endif
