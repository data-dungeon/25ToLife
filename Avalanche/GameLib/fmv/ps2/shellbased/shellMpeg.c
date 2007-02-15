//-----------------------------------------------------------------------------
//                   shellMpeg - IPU/MPEG streaming module
//
// $Workfile: shellMpeg.c $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "shellCore.h"
#include "shellTexture.h"
#include "shellMpeg.h"
#include "shellMpegCh2.h"


//-----------------------------------------------------------------------------
// C O N S T A N T S
//-----------------------------------------------------------------------------

#define READCHUNKSIZE   16384
#define IPU_BUFF_SIZE   (256 * 1024)
#define ADS_HEADER_SIZE 40

// Corresponds to around 16 frames (@ 30fps) for 48kHz audio
#define AUDIO_BUFF_SIZE_PCM   102400
#define AUDIO_BUFF_SIZE_ADPCM 32768
#define AUDIO_BUFF_SIZE_SPDIF 102400

#define DMACHUNKSIZE    8192  // bytes

// Maximum number of macroblocks that fit in a single DMA transfer
#define MAX_DMA_MBCOUNT ((1024*1024 - 16)/sizeof(sceIpuRGB32))

#define IPU_IDLE_TIMEOUT 1000


//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S 
//-----------------------------------------------------------------------------

static ShellMpegStream *_str;


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------
 
static EShellResult _createCore( int create,
					   ShellMpegStream *str,
					   EShellMpegMultiplexType mplex,
					   EShellMpegVideoType video,
					   EShellMpegAudioType audio,
					   int audioChannel,
					   void (*fileInitFn) (int),
					   int (*fileLoadFn) (unsigned char *, int, int),
					   int fileFnParam,
					   int doubleBuffer,
					   u_int dbw,
					   EShellMpegOutputType output );
static EShellResult _getVideoCore( void );
static void _mywaitipuidle( int returnOnOutputBlocked );
static int _mywaitipuidle2( void );
static void _installStrCallbacks( int audioChannel );
static int _videoCallback( sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data );
static int _audioCallback( sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data );
static int _nodataCallback( sceMpeg *mp, sceMpegCbData *cbdata, void *data );
static int _backgroundCallback( sceMpeg *mp, sceMpegCbData *cbdata, void *data );
static int _errorCallback( sceMpeg *mp, sceMpegCbData *cbdata, void *data );
static int _fillBuff( int blocking );


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// shellMpegInit()
// Description: Initialises the ShellMpeg module
// Parameters: None
// Returns: N/A
// Notes: This function initialises the shellMpeg module by calling the
// underlying library intialisation functions, sceIpuInit() and sceMpegInit().
//-----------------------------------------------------------------------------
void shellMpegInit( void )
{
	char __version[]="PsIIShlMpeg 1230";

	asm volatile("nop" : : "r"(__version));

    sceIpuInit();
	sceMpegInit();
	
	shellDmaSetChannelOptions(SHELLDMA_CHANNEL_FROMIPU, 0, 0, 0, 0, 0, 0);
	shellDmaSetChannelOptions(SHELLDMA_CHANNEL_TOIPU, 1, 0, 0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// shellMpegCreate()
// Description: Creates an MPEG/IPU stream player
// Parameters:
//   str          - Uninitialised structure to store decoding parameters
//   mplex        - SHELLMPEG_MULTIPLEX_NONE - Elementary stream input
//                                            (.ipu or .m2v)
//                  SHELLMPEG_MULTIPLEX_PSS  - Multiplexed stream input (.pss)
//   video        - SHELLMPEG_VIDEO_IPU_STANDARD - .ipu format video stream
//                  SHELLMPEG_VIDEO_IPU_COLUMNS  - .ipu format video stream
//                                                but with macroblocks in
//                                                column order
//                  SHELLMPEG_VIDEO_M2V_CSCIPU   - .m2v format video stream
//                                                using IPU for CSC
//   audio        - Type of audio channel to extract for multiplexed stream
//                  types:
//                  SHELLMPEG_AUDIO_NONE  - no audio
//                  SHELLMPEG_AUDIO_PCM   - .ads format PCM
//                  SHELLMPEG_AUDIO_ADPCM - .ads format ADPCM
//                  SHELLMPEG_AUDIO_SPDIF - SPDIF-format channel in a DATA
//                                         stream
//   audioChannel - Channel number within specified audio type to decode
//                  (ignored if audio = SHELLMPEG_AUDIO_NONE)
//   fileInitFn   - Callback function to initialise input stream
//   fileLoadFn   - Callback function to load stream data
//   fileFnParam  - Value to be passed as final argument when calling
//                  fileInitFn and fileLoadFn
//   doubleBuffer - 0 - Single-buffered output
//                  1 - Double-buffered output
//   dbw          - Width of eventual destination buffer, in units of 64 pixels
//   output       - Decoded frame output format:
//                  SHELLMPEG_OUTPUT_PATH3TAGS - shellMpegGetVideo() will be
//                                               used to obtain decoded frames
//                                               (..._16 for PSMCT16 output)
//                  SHELLMPEG_OUTPUT_TEXTURE   - shellMpegGetVideoAsTexture()
//                                               will be used to obtain decoded
//                                               frames
//                                               (..._16 for PSMCT16 output)
//
// Returns: SUCCESS if the player could be initialised successfully, otherwise
//          FAIL
// Notes:
//   Creates a decoder for a single video stream, allocating all memory
//   required for the process on the current heap.
//
//   Parameter 'output' replaces the previously unused 'dpsm'. Values passed
//   in which are not an EShellMpegOutputType value will be assumed to mean
//   SHELLMPEG_OUTPUT_PATH3TAGS.
//
//   On calling this function, the newly-created decoder becomes the
//   currently-active decoder, implicitly being the subject of other shellMpeg
//   calls. The current decoder can re reassigned using shellMpegBind().
//
//   On calling this function, the fileInitFn will be called to request the
//   input stream be initialised. After this, the fileLoadFn is called to read
//   in enough of the file to determine the dimensions of the video and hence
//   allocate the buffers needed in decoding.
//
//   There is no 'destroy' complement for shellMpegCreate since there are no
//   resources used by ShellMpeg which need to be reaped apart from memory,
//   which should be reclaimed by resetting the enclosing heap.
//-----------------------------------------------------------------------------
EShellResult shellMpegCreate( ShellMpegStream *str,
					EShellMpegMultiplexType mplex,
					EShellMpegVideoType video,
					EShellMpegAudioType audio,
					int audioChannel,
					void (*fileInitFn) (int),
					int (*fileLoadFn) (unsigned char *, int, int),
					int	fileFnParam,
					int doubleBuffer,
					u_int dbw,
					EShellMpegOutputType output )
{
	return _createCore(1, str, mplex, video, audio, audioChannel, fileInitFn,
					  fileLoadFn, fileFnParam, doubleBuffer, dbw, output);
}

//-----------------------------------------------------------------------------
// shellMpegGetVideo()
// Description: Decode one video frame
// Parameters: tags - On return, DMA tags for PATH3 transfer of the decoded
//                    image
//             dbp  - VRAM destination to be specified in 'tags', in units
//                    of VRAM word address/64 (or 32ths of a page)
// Returns: SUCCESS if the player could be decoded successfully, otherwise
//          FAIL
// Notes:
//   This function decodes one video frame from the currently active player.
//   On successful completion, variable tags is assigned to a DMA chain which
//   can be used to upload a newly-decoded to VRAM. The dbp specifies the
//   destination buffer pointer (as used in BITBLTBUF) to which the image
//   should be uploaded. The buffer width of the destination buffer however
//   is constant for a stream, and was specified in shellMpeg.
//
//   The fileLoadFn callback specified in shellMpegCreate is repeatedly called
//   during this function to stream in input data.
//
//   The function may repeatedly make use of the shellCore idle callback while
//   waiting for the IPU to complete decoding. It is thus recommended when
//   streaming FMV that this callback be used to recharge the input buffers
//   (see shellMpegRecharge).
//-----------------------------------------------------------------------------
EShellResult shellMpegGetVideo( ShellDmaTag **tags, u_int dbp )
{
	if(_getVideoCore())
	{
		int bufid = ((_str->framesDecoded - 1) % 2) && _str->doubleBuffer;

		*tags = _str->tags[bufid].path3;
		setLoadImageDest(_str->tags[bufid].path3, dbp, _str->dbw,
			(_str->output == SHELLMPEG_OUTPUT_PATH3TAGS_16)
			? SCE_GS_PSMCT16 : SCE_GS_PSMCT32);
		return SHELL_SUCCESS;
	}
	
	return SHELL_FAIL;
}

//-----------------------------------------------------------------------------
// shellMpegGetVideo_Texture()
// Description: Decode one video frame as a shellTexture texture
// Parameters: tex - On return, pointer to ShellTextureInfo structure
// Returns: SUCCESS if the player could be decoded successfully, otherwise
//          FAIL
// Notes:
//   The ShellTextureInfo pointer set by the function may be added to the
//   texture upload queue using shellTextureUpload() or
//   shellTextureUploadToAddr().
//   The fileLoadFn callback specified in shellMpegCreate is repeatedly called
//   during this function to stream in input data.
//
//   The function may repeatedly make use of the shellCore idle callback while
//   waiting for the IPU to complete decoding. It is thus recommended when
//   streaming FMV that this callback be used to recharge the input buffers
//   (see shellMpegRecharge).
//-----------------------------------------------------------------------------
EShellResult shellMpegGetVideoAsTexture( ShellTextureInfo **tex )
{
	if(_getVideoCore())
	{
		int bufid = ((_str->framesDecoded - 1) % 2) && _str->doubleBuffer;
		
		*tex = _str->texture[bufid];
		return SHELL_SUCCESS;
	}

	return SHELL_FAIL;
}

//-----------------------------------------------------------------------------
// shellMpegGetAudio()
// Description: Pass decoded audio data to an output handler
// Parameters: receiver - Audio handler
// Returns: N/A
// Notes:
//   Calling this function does not actually cause any more data to be decoded;
//   rather, it reveals what audio data have been decoded thus far. The audio
//   buffer can be refilled by the shellMpegGetVideo and shellMpegRecharge
//   functions.
//-----------------------------------------------------------------------------
void shellMpegGetAudio( u_int (*receiver) ( u_char *, u_int, u_int, u_int, int ),
					   int param )
{
	u_int bytesUsed;

	if(_str->audio != SHELLMPEG_AUDIO_NONE)
	{
		int used,buffsize,buffpos;

		bytesUsed = receiver(_str->audioBuff, _str->audioBuffSize,
							 (u_int) _str->audioBuffPos,
							 (u_int) _str->audioBuffFullness,
							 param);
		
		used = (int)bytesUsed;
		buffsize = (int) _str->audioBuffSize;
		buffpos = (int)_str->audioBuffPos;

		_str->audioBuffFullness -= (int) bytesUsed;
		_str->audioBuffPos = (buffpos + used) % buffsize;
	}
}

//-----------------------------------------------------------------------------
// shellMpegIsEnd()
// Description:  Determines whether more video frames can be decoded
// Parameters: None
// Returns: 1 - ended, 0 - more frames remain
// Notes:
//-----------------------------------------------------------------------------
int shellMpegIsEnd( void )
{
	if(_str->abort) return 1;

	switch(_str->video)
	{
		case SHELLMPEG_VIDEO_IPU_STANDARD:
		case SHELLMPEG_VIDEO_IPU_COLUMNS:
			return _str->framesDecoded >= _str->ipuNumFrames;
			break;
		
		case SHELLMPEG_VIDEO_M2V_CSCIPU:
			return (sceMpegIsEnd(&_str->mp));
			break;
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
// shellMpegGetRewind()
// Description: Reset a stream back to the start
// Parameters: None
// Returns: N/A
// Notes:
//   Calling this function rewinds a video stream such that the next video
//   frame to be decoded, and the next audio data to be obtained, are from the
//   start of the stream.
//
//   The fileInitFn and fileLoadFn callback parameters specified in
//   shellMpegCreate() are called during this function.
//-----------------------------------------------------------------------------
void shellMpegRewind( void )
{
	_createCore(0, _str, _str->mplex, _str->video, _str->audio, 0,
			    _str->fileInitFn, _str->fileLoadFn, _str->fileFnParam,
			    _str->doubleBuffer, _str->dbw, 0);
}

//-----------------------------------------------------------------------------
// shellMpegRecharge()
// Description: Refill current decoder's input buffer
// Parameters: None
// Returns: N/A
// Notes:
//   Calling this function causes input data to be read in and (depending on
//   type) demultiplexed into video and audio streams, ready for retrieval
//   during shellMpegGetVideo or shellMpegGetAudioBegin calls. Its use is not
//   mandatory, since shellMpegGetVideo performs as much loading as is needed
//   to decode a frame; rather, the function is intended to be used for FMV
//   playback, to be called during periods of inactivity to ensure smooth
//   playback.
//
//   The fileLoadFn callback parameters specified in shellMpegCreate may be
//   called during this function.
//-----------------------------------------------------------------------------
void shellMpegRecharge( void )
{
	_fillBuff(0);
}

//-----------------------------------------------------------------------------
// shellMpegBind()
// Description: Specify a player to be made active
// Parameters: str - Stream player to be made active
// Returns: the previously-active decoder
// Notes:
//-----------------------------------------------------------------------------
ShellMpegStream *shellMpegBind( ShellMpegStream *str )
{
	ShellMpegStream *prev = _str;

	_str = str;
	return prev;
}

//-----------------------------------------------------------------------------
// shellMpegDelete()
// Description: Delete decoder
// Parameters: None
// Returns: N/A
// Notes:
//-----------------------------------------------------------------------------
void shellMpegDelete( void )
{
	if(_str->video == SHELLMPEG_VIDEO_M2V_CSCIPU) sceMpegDelete(&_str->mp);
}

//-----------------------------------------------------------------------------
// shellMpegSingleIpuTexture()
// Description: Decompress a single .ipu texture
// Parameters:
//   ipu      - pointer to .ipu file data
//   size     - size of .ipu file data, in bytes
//   columns  - 0    - row-ordered macroblocks
//              else - column-ordered macroblocks
//   opm      - Output pixel mode
//              SCE_GS_PSMCT32  - output as PSMCT32 texture
//              SCE_GS_PSMCT16  - output as PSMCT16 texture
//   dither   - 0    - don't dither
//              else - dither (only applicable when opm = SCE_GS_PSMCT16)
// Returns: shellTextureInfo * - pointer to image in shellTexture format,
//          or zero if decoding was unsuccessful
// Notes:
//  The IPU data must have been committed to main memory before calling.
//  The shellTextureInfo structure to address the decompressed image, together
//  with the pixels themselves and corresponding DMA upload tags, are placed on
//  the currently bound heap.
//-----------------------------------------------------------------------------
ShellTextureInfo *shellMpegSingleIpuTexture( char *ipu, u_int size,
											 int columns, int opm, int dither )
{
	u_int flags;
	u_int mbcount;
	ShellTextureSetUp setup;
	sceIpuRGB32 *rgb;
	u_int rgbSize;

	setup.width  = (u_short) *(u_short *) (ipu + 8);
	setup.height = (u_short) *(u_short *) (ipu + 10);
 	ipu += 16;  // header is 16 bytes
	size -= 16;

	rgbSize = (setup.width / 16) * (setup.height / 16) *
				 (opm == SCE_GS_PSMCT32 ? sizeof(sceIpuRGB32)
				 						: sizeof(sceIpuRGB16));
	
	if(!(rgb = shellMemoryAllocate(64, rgbSize))) return 0;
	
	InvalidDCache(rgb, (char *) rgb + rgbSize - 1);
		
	// Set up DMA transfer tags for uploading to VRAM
		
	if(columns)
	{
		setup.pImage = shellMemoryAllocate(64, PATH3_TAG_SIZE_COLUMNS
						((u_int) setup.width/16));
				
		setLoadImageTagsColumns((ShellDmaTag *) setup.pImage, rgb,
					(u_int) setup.width / 16, (u_int) setup.height / 16,
					opm);
	}
	
	else
	{
		setup.pImage = shellMemoryAllocate(64, PATH3_TAG_SIZE_ROWS
						((u_int) setup.width/16, (u_int) setup.height/16));

		setLoadImageTagsRows((ShellDmaTag *) setup.pImage, rgb,
					(u_int) setup.width / 16, (u_int) setup.height / 16,
					opm);
	}
	
	sceIpuReset();
	sceIpuSync(0, 0);
	sceIpuBCLR(0);
	sceIpuSync(0, 0);
	
	shellDmaStart(SHELLDMA_CHANNEL_TOIPU, ipu, (u_int) (size + 15) / 16);

	// Get flags
	
	sceIpuFDEC(0);
	if(!_mywaitipuidle2()) return 0;
	
	flags = sceIpuGetFVdecResult();
	flags = (flags >> 24) & 0xff;
		
	DPUT_IPU_CTRL((flags & ~0x00000004) << 16);
	
	// Decode frame
	
	sceIpuIDEC(opm == SCE_GS_PSMCT32
			   		? SCE_IPU_IDEC_RGB32 : SCE_IPU_IDEC_RGB16,
			   dither, 0, (flags >> 2) & 0x1, 1, 8);
			
	mbcount = ((u_int) setup.width / 16) * ((u_int) setup.height / 16);
	
	while(mbcount)
	{
		u_int mb = (mbcount < MAX_DMA_MBCOUNT ) ? mbcount : MAX_DMA_MBCOUNT;
		
		shellDmaStart(SHELLDMA_CHANNEL_FROMIPU,
					  rgb,
					  (mb * (opm == SCE_GS_PSMCT32 ? sizeof(sceIpuRGB32)
						                           : sizeof(sceIpuRGB16))) /16);
				
		if(!_mywaitipuidle2()) return 0;
		
		mbcount -= mb;
		ipu += mb;
	}
		
	setup.imageMode = (u_char) opm;
	setup.swizMode = SHELLTEXTURE_MODE_USERTAGS;

	// Texture structure added to current heap
	
	return shellTextureMemLoadRaw(&setup);
}


//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// _createCore()
// Description: Creates or rewinds an MPEG/IPU stream player
// Parameters:
//   create       - 1 - Create a new stream player
//                  0 - Rewind an existing stream player
//   str          - Structure to store decoding parameters
//   mplex        - SHELLMPEG_MULTIPLEX_NONE - Elementary stream input
//                                            (.ipu or .m2v)
//                  SHELLMPEG_MULTIPLEX_PSS  - Multiplexed stream input (.pss)
//   video        - SHELLMPEG_VIDEO_IPU_STANDARD - .ipu format video stream
//                  SHELLMPEG_VIDEO_IPU_COLUMNS  - .ipu format video stream
//                                                but with macroblocks in
//                                                column order
//                  SHELLMPEG_VIDEO_M2V_CSCIPU   - .m2v format video stream
//                                                using IPU for CSC
//   audio        - Type of audio channel to extract for multiplexed stream
//                  types:
//                  SHELLMPEG_AUDIO_NONE  - no audio
//                  SHELLMPEG_AUDIO_PCM   - .ads format PCM
//                  SHELLMPEG_AUDIO_ADPCM - .ads format ADPCM
//                  SHELLMPEG_AUDIO_SPDIF - SPDIF-format channel in a DATA
//                                         stream
//   audioChannel - Channel number within specified audio type to decode
//                  (ignored if audio = SHELLMPEG_AUDIO_NONE)
//   fileInitFn   - Callback function to initialise input stream
//   fileLoadFn   - Callback function to load stream data
//   fileFnParam  - Value to be passed as final argument when calling
//                  fileInitFn and fileLoadFn
//   doubleBuffer - 0 - Single-buffered output
//                  1 - Double-buffered output
//   dbw          - Width of eventual destination buffer, in units of 64 pixels
//   output       - Decoded frame output format:
//                  SHELLMPEG_OUTPUT_PATH3TAGS - shellMpegGetVideo() will be
//                                               used to obtain decoded frames
//                                               (..._16 for PSMCT16 output)
//                  SHELLMPEG_OUTPUT_TEXTURE   - shellMpegGetVideoAsTexture()
//                                               will be used to obtain decoded
//                                               frames
//                                               (..._16 for PSMCT16 output)
// Returns: SUCCESS if the player could be initialised successfully, otherwise
//          FAIL
// Notes:
//   Creates a decoder for a single video stream, allocating all memory
//   required for the process on the current heap.
//
//   Parameter 'output' replaces the previously unused 'dpsm'. Values passed
//   in which are not an EShellMpegOutputType value will be assumed to mean
//   SHELLMPEG_OUTPUT_PATH3TAGS.
//
//   On calling this function, the newly-created decoder becomes the
//   currently-active decoder, implicitly being the subject of other shellMpeg
//   calls. The current decoder can re reassigned using shellMpegBind().
//
//   On calling this function, , the fileInitFn will be called to request the
//   input stream be initialised. After this, the fileLoadFn is called to read
//   in enough of the file to determine the dimensions of the video and hence
//   allocate the buffers needed in decoding.
//
//   There is no 'destroy' complement for shellMpegCreate since there are no
//   resources used by ShellMpeg which need to be reaped apart from memory,
//   which should be reclaimed by resetting the enclosing heap.
//-----------------------------------------------------------------------------
static EShellResult _createCore( int create,
					   ShellMpegStream *str,
					   EShellMpegMultiplexType mplex,
					   EShellMpegVideoType video,
					   EShellMpegAudioType audio,
					   int audioChannel,
					   void (*fileInitFn) (int),
					   int (*fileLoadFn) (unsigned char *, int, int),
					   int fileFnParam,
					   int doubleBuffer,
					   u_int dbw,
					   EShellMpegOutputType output )
{
	int n;
	int bpp;

	// Call without create to perform a rewind

	_str = str;

	DI();
	*D_ENABLEW = 1 << 16;
	*D3_CHCR = 0;
	*D3_QWC  = 0;
	*D4_CHCR = 0;
	*D4_QWC  = 0;
	*D_ENABLEW = 0 << 16;
	__asm__ volatile("sync.l");
	EI();

	str->abort = 0;

	if(create)
	{
		// Flush any potentially cached data out of the heap prior to
		// allocations
		FlushCache(0);
	
		// Buffer for multiplexed video data (.pss)
		if(!(str->pssBuff = shellMemoryAllocate(64, READCHUNKSIZE)))
			return SHELL_FAIL;

		// Buffer for unmultiplexed video data (.m2v or .ipu)		
		if(!(str->ipuBuff = shellMemoryAllocate(64, IPU_BUFF_SIZE)))
			return SHELL_FAIL;

		str->ipuBuffSize = IPU_BUFF_SIZE;

		str->fileLoadFn = fileLoadFn;
		str->fileInitFn = fileInitFn;
		str->fileFnParam = fileFnParam;	
	
		str->mplex = mplex;
		str->video = video;
		str->audio = audio;
		str->audioChannel = audioChannel;
		str->output = output;

		str->doubleBuffer = doubleBuffer;
		str->dbw = dbw;
	}

	str->ipuBuffPast    = str->ipuBuff + 64;
	str->ipuBuffPresent = str->ipuBuff;
	str->ipuBuffFuture  = str->ipuBuff + 64;

	str->pssBuffFullness = 0;

	if(audio != SHELLMPEG_AUDIO_NONE)
	{
		if(create)
		{
			switch(str->audio)
			{
				case SHELLMPEG_AUDIO_PCM:
					dbgPrint("PCM Stream detected\n");
					str->audioBuffSize = AUDIO_BUFF_SIZE_PCM;
					break;
				
				case SHELLMPEG_AUDIO_ADPCM:
					dbgPrint("ADPCM Stream detected\n");
					str->audioBuffSize = AUDIO_BUFF_SIZE_ADPCM;
					break;
				
				case SHELLMPEG_AUDIO_SPDIF:
					dbgPrint("SPDIF Stream detected\n");
					str->audioBuffSize = AUDIO_BUFF_SIZE_SPDIF;
					break;
					
				default:
					ASSERTS(0, "Unsupported audio type");
					return SHELL_FAIL;
			}
		
			if(!(str->audioBuff = shellMemoryAllocate(64, str->audioBuffSize)))
				return SHELL_FAIL;
		}

		else str->audioBuffSize = 0;

		str->audioBuffPos = 0;
		str->audioBuffFullness = 0;
		str->audioHeaderCount = 0;
	}
	
	if(str->fileInitFn) str->fileInitFn(fileFnParam);
	
	str->framesDecoded = 0;	
	
	// Get video dimensions
	
	if(create && mplex == SHELLMPEG_MULTIPLEX_PSS)
	{
		u_char tempWorkArea[SCE_MPEG_BUFFER_SIZE(0, 0)];

		// Create a temporary MPEG decoder to demux enough video data
		// to read the video header and hence establish dimensions.
		
		sceMpegCreate(&str->mp, tempWorkArea, sizeof(tempWorkArea));
		_installStrCallbacks(str->audioChannel);
	}
	
	if(!_fillBuff(1))
	{
		WARNING(0, "Couldn't read stream header");
		return SHELL_FAIL;
	}
	
	if(0
		|| video == SHELLMPEG_VIDEO_IPU_STANDARD
		|| video == SHELLMPEG_VIDEO_IPU_COLUMNS)
	{
		if(create)
		{
			// IPU format header is documented in sce/doc/format/mvformat.pdf
		
			str->width  = (u_int) *(u_short *) (str->ipuBuffFuture + 8);
			str->height = (u_int) *(u_short *) (str->ipuBuffFuture + 10);
			str->ipuNumFrames = (int) *(u_int *) (str->ipuBuffFuture + 12);
		}
		
		// Advance the demuxed stream past the header, in a roundabout way
		
	 	str->ipuBuffFuture += 16; // header is 16 bytes
	}
	
	// Or get m2v dimensions...
	
	else  // video == SHELLMPEG_VIDEO_M2V_CSCIPU 
	{
		if(create)
		{
			// M2V header is documented in ISO/IEC 13818-2
		
			str->width  = (u_int) (str->ipuBuffFuture[4] << 4)
				| (str->ipuBuffFuture[5] >> 4);
			
			str->height = (u_int) ((str->ipuBuffFuture[5] & 0xf) << 8)
				| str->ipuBuffFuture[6];
		}
	}
	
	// Recreate the sceMpeg structure to replace the temporary one we used
	// just for demuxing which had a zero-sized work area attached.
	// (If we're not playing MPEG video or decoding from an MPEG multiplex
	// there's no need to do this).
	
	if(0
		|| video == SHELLMPEG_VIDEO_M2V_CSCIPU
		|| mplex == SHELLMPEG_MULTIPLEX_PSS)
	{
		// Reinstall the stream callbacks and add the video decoding
		// callbacks if necessary
				
		if(create)
		{
			u_int workAreaSize;
		
			// Set up work area
			
			if(0
				|| video == SHELLMPEG_VIDEO_IPU_STANDARD
				|| video == SHELLMPEG_VIDEO_IPU_COLUMNS)
				workAreaSize = SCE_MPEG_BUFFER_SIZE(0, 0);
			
			else  // video == SHELLMPEG_VIDEO_M2V_CSCIPU
				workAreaSize = SCE_MPEG_BUFFER_SIZE(str->width, str->height);

		
			if(!(str->workArea = shellMemoryAllocate(64, workAreaSize)))
				return SHELL_FAIL;
		
			sceMpegCreate(&str->mp, str->workArea, (int) workAreaSize);

			if(video == SHELLMPEG_VIDEO_M2V_CSCIPU)
			{
				sceMpegAddCallback(&str->mp, sceMpegCbNodata,
									_nodataCallback, 0);
				
				sceMpegAddCallback(&str->mp, sceMpegCbBackground,
								   _backgroundCallback, (void *) str);
			}

			sceMpegAddCallback(&str->mp, sceMpegCbError,
							   _errorCallback, NULL);
			
			if(mplex == SHELLMPEG_MULTIPLEX_PSS)
				_installStrCallbacks(str->audioChannel);
		}

		else sceMpegReset(&str->mp);
	}
	
	if(create)
	{
		switch(output)
		{
		case SHELLMPEG_OUTPUT_PATH3TAGS_16:
		case SHELLMPEG_OUTPUT_TEXTURE_16:
			bpp = 16;
			break;

		default:
			WARNING(0, "shellMpegCreate output type %d no longer supported; using default", output);
			output = SHELLMPEG_OUTPUT_PATH3TAGS;
			
			// Very deliberately no break; here

		case SHELLMPEG_OUTPUT_PATH3TAGS:
		case SHELLMPEG_OUTPUT_TEXTURE:
			bpp = 32;
		}
	}
	
	for(n = 0; create && n < (doubleBuffer ? 2 : 1); n++)
	{
		u_int rgbSize;

		rgbSize = (str->width / 16) * (str->height / 16);
		rgbSize *= (bpp == 16) ? sizeof(sceIpuRGB16) : sizeof(sceIpuRGB32);
						 
		if(!(str->rgb[n] = shellMemoryAllocate(64, rgbSize)))
			return SHELL_FAIL;

		if(((str->output == SHELLMPEG_OUTPUT_PATH3TAGS_16)
			|| (str->output == SHELLMPEG_OUTPUT_TEXTURE_16))
		   && (str->video == SHELLMPEG_VIDEO_M2V_CSCIPU))
		{
			// Decoding an MPEG to PSMCT16 requires a raw8 buffer and an
			// rgb(16) buffer

			if(!(str->raw8[n] = shellMemoryAllocate(64, (str->width / 16)
													* (str->height / 16)
													* sizeof(sceIpuRAW8))))
				return SHELL_FAIL;
		}

		// Set up DMA transfer tags
		
		if(str->video == SHELLMPEG_VIDEO_IPU_STANDARD)
		{
			str->tags[n].path3 = shellMemoryAllocate(64, PATH3_TAG_SIZE_ROWS
											(str->width/16, str->height/16));
											
			setLoadImageTagsRows(str->tags[n].path3,
						str->rgb[n], str->width / 16, str->height / 16,
						(bpp == 16) ? SCE_GS_PSMCT16 : SCE_GS_PSMCT32);
		}
		
		else
		{
			str->tags[n].path3 = shellMemoryAllocate(64, PATH3_TAG_SIZE_COLUMNS
											(str->width/16));
			
			setLoadImageTagsColumns(str->tags[n].path3,
						str->rgb[n], str->width / 16, str->height / 16,
						(bpp == 16) ? SCE_GS_PSMCT16 : SCE_GS_PSMCT32);
		}
		
		if((output == SHELLMPEG_OUTPUT_TEXTURE)
		   || (output == SHELLMPEG_OUTPUT_TEXTURE_16))
		{
			ShellTextureSetUp setup;
		
			setup.width = (u_short) str->width;
			setup.height = (u_short) str->height;
			setup.imageMode = (bpp == 16) ? (u_char) SCE_GS_PSMCT16
										  : (u_char) SCE_GS_PSMCT32;
			setup.swizMode = SHELLTEXTURE_MODE_USERTAGS;
	
			// Texture structures added to current heap
			setup.pImage = (u_char *) str->tags[n].path3;
			
			str->texture[n] = shellTextureMemLoadRaw(&setup);
		}
	}

	sceIpuStopDMA(&str->dmaEnv);
	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// _getVideoCore()
// Description: Decode one video frame
// Parameters: none
// Returns: SUCCESS if the player could be decoded successfully, otherwise
//          FAIL
//-----------------------------------------------------------------------------
static EShellResult _getVideoCore( void )
{
	int bufid = (_str->framesDecoded % 2) && _str->doubleBuffer;
	u_int mbcount = (_str->width / 16) * (_str->height / 16);	

	sceIpuRestartDMA(&_str->dmaEnv);

	if(0
		|| _str->video == SHELLMPEG_VIDEO_IPU_STANDARD
		|| _str->video == SHELLMPEG_VIDEO_IPU_COLUMNS)
	{
		if(_str->framesDecoded == 0)
		{
			sceIpuReset();
			sceIpuSync(0, 0);
			
			sceIpuBCLR(0);
			sceIpuSync(0, 0);
		}

		if(_str->framesDecoded >= _str->ipuNumFrames)
		{
			DI();
			*D_ENABLEW = 1 << 16;
			*D4_CHCR = 0;
			*D4_QWC  = 0;
			*D_ENABLEW = 0 << 16;
			__asm__ volatile("sync.l");
			EI();
			
			return SHELL_FAIL;
		}
	}

	if(_str->video == SHELLMPEG_VIDEO_M2V_CSCIPU)
	{
		if(sceMpegIsEnd(&_str->mp)) return SHELL_FAIL;
		
		if((_str->output == SHELLMPEG_OUTPUT_PATH3TAGS_16)
		   || (_str->output == SHELLMPEG_OUTPUT_TEXTURE_16))
		{
			sceIpuDmaEnv ipuDmaEnv;

			if(sceMpegGetPictureRAW8(&_str->mp, _str->raw8[bufid],
								 (int) (mbcount)) < 0)
				return SHELL_FAIL;

			// Convert from RAW8 to PSMCT16 using the IPU's CSC

			sceIpuStopDMA(&ipuDmaEnv);
			
			*D4_QWC = (sizeof(sceIpuRAW8) * mbcount) / 16;
			*D4_MADR = (u_int) _str->raw8[bufid];
			*D4_CHCR = 1 | (1 << 8);

			*D3_QWC = (sizeof(sceIpuRGB16) * mbcount) / 16;
			*D3_MADR = (u_int) _str->rgb[bufid];
			*D3_CHCR = 0 | (1 << 8);

			sceIpuBCLR(0);
			sceIpuSync(0, 0);

			sceIpuCSC(1, 1, mbcount);
			while(sceIpuSync(1, 0)) shellCoreCallIdleRoutine();

			sceIpuRestartDMA(&ipuDmaEnv);
		}

		else
		{
			if(sceMpegGetPicture(&_str->mp, _str->rgb[bufid],
								 (int) (_str->width * _str->height)) < 0)
				return SHELL_FAIL;
		}
	}
	
	else  // _str->video == SHELLMPEG_VIDEO_IPU_STANDARD or ..._COLUMNS
	{		
		u_int flags;
		int bpp;
		sceIpuRGB32 *cscptr = _str->rgb[bufid];

		switch(_str->output)
		{
		case SHELLMPEG_OUTPUT_PATH3TAGS_16:
		case SHELLMPEG_OUTPUT_TEXTURE_16:
			bpp = 16;
			break;
		case SHELLMPEG_OUTPUT_PATH3TAGS:
		case SHELLMPEG_OUTPUT_TEXTURE:
			bpp = 32;
		}

		// Get flags
		sceIpuFDEC(0);

		_mywaitipuidle(0);
		if(_str->abort) goto fail;

		flags = sceIpuGetFVdecResult();
		flags = (flags >> 24) & 0xff;
		
		sceIpuFDEC(8);  // Advance past the flags

		_mywaitipuidle(0);
		if(_str->abort) goto fail;
		
		DPUT_IPU_CTRL((flags & ~0x00000004) << 16);
		
		sceIpuIDEC((bpp == 16) ? 1 : 0, 1, 0, (flags >> 2) & 0x1, 1, 0);
				   		
		while(mbcount)
		{
			u_int mb = (mbcount < MAX_DMA_MBCOUNT ) ? mbcount : MAX_DMA_MBCOUNT;
			
			shellDmaStart(SHELLDMA_CHANNEL_FROMIPU, cscptr,
						  mb * (bpp == 32 ? sizeof(sceIpuRGB32)
								          : sizeof(sceIpuRGB16)) / 16);
			
			_mywaitipuidle(1);
			if(_str->abort) goto fail;
			
			mbcount -= mb;
			cscptr += mb;
		}
		
		// skip start code
		sceIpuFDEC(32);

		_mywaitipuidle(0);
		if(_str->abort) goto fail;
	}
	
	sceIpuStopDMA(&_str->dmaEnv);
	_str->framesDecoded++;

	return SHELL_SUCCESS;

 fail:

	sceIpuStopDMA(&_str->dmaEnv);

	sceIpuReset();
	sceIpuSync(0, 0);
	
	sceIpuBCLR(0);
	sceIpuSync(0, 0);
	
	return SHELL_FAIL;
}

//-----------------------------------------------------------------------------
// _mywaitipuidle()
// Description: Waits for IPU to be idle, fetching more input data if needed
// Parameters:
//   returnOnOutputBlocked - 0 - Don't exit if fromIPU output DMA stops
//                           1 - Do exit if fromIPU output DMA stops
// Returns: N/A
// Notes:
//   The IPU's contol register is constantly polled until the device is either
//   idle or encounters an error. The toIPU DMA control register is also
//   polled to ensure that there is always an input feed to the device; if this
//   dries up, the _nodataCallback function is called to obtain more.
//-----------------------------------------------------------------------------
static void _mywaitipuidle( int returnOnOutputBlocked )
{
	while(!_str->abort)
	{
		if(DGET_IPU_CTRL() & IPU_CTRL_ECD_M)
		{
			WARNING(0, "IPU error code detected");
			return;
		}
		
		if(!sceIpuIsBusy()) return;
				
		if(!(DGET_D4_CHCR() & D_CHCR_STR_M))
		{
			// DMA stopped
			
			int timeout = 0;
			
			while(sceIpuIsBusy())
				if(timeout++ > IPU_IDLE_TIMEOUT)
				{
					_nodataCallback(0, 0, 0);
					break;
				}
		}

		if(returnOnOutputBlocked && !(DGET_D3_CHCR() & D_CHCR_STR_M)) return;
	}
}

//-----------------------------------------------------------------------------
// _mywaitipuidle2()
// Description: Waits for IPU to be idle or aborts on error
// Parameters: None
// Returns: 1 - IPU completed succesfully, else an error occurred (DMA input
//          dried up or erroneous bitstream data suppled)
//-----------------------------------------------------------------------------
static int _mywaitipuidle2( void )
{
	while(1)
	{
		if(DGET_IPU_CTRL() & IPU_CTRL_ECD_M) return 0;
		if(!sceIpuIsBusy()) return 1;
	
		if(!(DGET_D4_CHCR() & D_CHCR_STR_M))
		{
			// DMA stopped
			
			int timeout = 0;
			
			while(sceIpuIsBusy()) if(timeout++ > IPU_IDLE_TIMEOUT) return 0;
			return 1;
		}
	}
}

//-----------------------------------------------------------------------------
// _installStrCallbacks()
// Description: Installs stream callbacks to decode the required video and
//              audio streams from a multiplex
// Parameters: audioChannel - audio channel number
// Returns: N/A
// Notes: Relies on _str being filled in to determine which channels are
//        required
//-----------------------------------------------------------------------------
static void _installStrCallbacks( int audioChannel )
{
	// Video callbacks
	
	if(_str->video == SHELLMPEG_VIDEO_M2V_CSCIPU)
		sceMpegAddStrCallback(&_str->mp, sceMpegStrM2V, 0,
							  (sceMpegCallback) _videoCallback, 0);
	
	else // video == SHELL_MPEG_VIDEO_IPU_STANDARD or ..._IPU_COLUMNS
		sceMpegAddStrCallback(&_str->mp, sceMpegStrIPU, 0,
							  (sceMpegCallback) _videoCallback, 0);

	// Audio callbacks
	
	if(_str->audio == SHELLMPEG_AUDIO_PCM)
		sceMpegAddStrCallback(&_str->mp, sceMpegStrPCM, audioChannel,
							  (sceMpegCallback) _audioCallback, 0);

	else if(_str->audio == SHELLMPEG_AUDIO_ADPCM)
		sceMpegAddStrCallback(&_str->mp, sceMpegStrADPCM, audioChannel,
							  (sceMpegCallback) _audioCallback, 0);

	else if(_str->audio == SHELLMPEG_AUDIO_SPDIF)
		sceMpegAddStrCallback(&_str->mp, sceMpegStrDATA, audioChannel,
							  (sceMpegCallback) _audioCallback, 0);
}

//-----------------------------------------------------------------------------
// _videoCallback()
// Description: Callback function to accept video data
// Parameters:
//   mp    - Ignored
//   cbstr - Structure containing stream packet
//   data  - Ignored
// Returns: 0       - could not accept more data (buffer full)
//          nonzero - all data accepted
// Notes: This function is called either by _fillBuff()'s sceMpegDemuxPss()
//        call or, when a multiplex is not used, by _fillBuff() itself
//-----------------------------------------------------------------------------
static int _videoCallback( sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data )
{
	int availSpace;
	int spill;

	(void) mp, data;

	// We can arrive at this function either from libmpeg's demuxer,
	// or our own passing raw data straight to it...

	if((0
		 || _str->video == SHELLMPEG_VIDEO_IPU_STANDARD
		 || _str->video == SHELLMPEG_VIDEO_IPU_COLUMNS)
		&& _str->mplex == SHELLMPEG_MULTIPLEX_PSS)
	{
		// Ignore the sub-stream ID
		cbstr->data += 4;
		cbstr->len -= 4;
	}

	availSpace = _str->ipuBuffPresent - _str->ipuBuffPast;    // processed data
	if(availSpace < 0) availSpace += (int) _str->ipuBuffSize;  // ring buffer
	
	if((availSpace == 0) && (_str->ipuBuffPresent == _str->ipuBuffFuture))
		availSpace = (int) _str->ipuBuffSize;    // special case - buffer is empty, not full
	
	if(availSpace < cbstr->len) return 0;
	
	spill = ((int) _str->ipuBuffPast - (int) _str->ipuBuff)
			+ (int) cbstr->len - (int) _str->ipuBuffSize;
	
	
	if(spill > 0)
	{
		memcpy((void *) (((int) _str->ipuBuffPast & 0x0fffffff) | 0x20000000),
			    cbstr->data, cbstr->len - spill);
			    
		memcpy((void *) (((int) _str->ipuBuff & 0x0fffffff) | 0x20000000),
			cbstr->data + (cbstr->len - spill),
			(u_int) spill);
			   
		_str->ipuBuffPast = _str->ipuBuff + spill;
	}
	
	else
	{
		memcpy((void *) (((int) _str->ipuBuffPast & 0x0fffffff) | 0x20000000),
			cbstr->data, cbstr->len);
			
		_str->ipuBuffPast += cbstr->len;
	}
	
	return 1;
}

//-----------------------------------------------------------------------------
// _audioCallback()
// Description: Callback function to accept video data
// Parameters:
//   mp    - Ignored
//   cbstr - Structure containing stream packet
//   data  - Ignored
// Returns: 0       - could not accept more data (buffer full)
//          nonzero - all data accepted
// Notes:
//   This function is called either by _fillBuff()'s sceMpegDemuxPss() call.
//
//   If more data cannot be accommodated in the audio buffer the flag
//   _str->audioFull is raised. This is used to inform _fillBuff() that the
//   reason for the demux failing was an overfull audio buffer and not the
//   .pss stream ending. There is no need for an equivalent _str->ipuFull
//   flag.
//-----------------------------------------------------------------------------
static int _audioCallback( sceMpeg *mp, sceMpegCbDataStr *cbstr, void *data )
{
	int xferStart;
	int spill;

	(void) mp, data;

	cbstr->len  -= 4;  // first four bytes are sub-stream ID
	cbstr->data += 4;

	if((_str->audio != SHELLMPEG_AUDIO_SPDIF)
	   && (_str->audioHeaderCount < ADS_HEADER_SIZE))
	{
		// Note, for these copies the sshd will spill over to the ssbd.
	
		if(cbstr->len > ADS_HEADER_SIZE - _str->audioHeaderCount)
		{
			memcpy((char *) &_str->sshd + _str->audioHeaderCount,
				cbstr->data, ADS_HEADER_SIZE - (u_int) _str->audioHeaderCount);

			cbstr->data += ADS_HEADER_SIZE - _str->audioHeaderCount;
			cbstr->len -= ADS_HEADER_SIZE - (u_int) _str->audioHeaderCount;
			_str->audioHeaderCount = ADS_HEADER_SIZE;
		}

		else
		{
			memcpy((char *) &_str->sshd + _str->audioHeaderCount,
				cbstr->data, cbstr->len);
		
			_str->audioHeaderCount += (int) cbstr->len;
			cbstr->len = 0;
		}
	}
	
	if(_str->audioBuffFullness + cbstr->len > _str->audioBuffSize)
	{
		_str->audioFull = 1;
		return 0;
	}

	xferStart = (_str->audioBuffPos + _str->audioBuffFullness)
		% (int) _str->audioBuffSize;

	spill = xferStart + (int) cbstr->len - (int) _str->audioBuffSize;

	if(spill > 0)
	{
		memcpy((void *) (((int) _str->audioBuff + xferStart) | 0x20000000),
			   cbstr->data,
			   cbstr->len - spill);

		memcpy((void *) ((int) _str->audioBuff | 0x20000000),
			   cbstr->data + (cbstr->len - spill),
			   (u_int) spill);
	}
	
	else
	{
		memcpy((void *) (((int) _str->audioBuff + xferStart) | 0x20000000),
			   cbstr->data,
			   cbstr->len);
	}

	_str->audioBuffFullness += (int) cbstr->len;
	return 1;
}

//-----------------------------------------------------------------------------
// _nodataCallback()
// Description: Callback function to supply the IPU with more video data
// Parameters:
//   mp    - Ignored
//   cbstr - Ignored
//   data  - Ignored
// Returns: 1 (ignored by sceMpegDemuxPss())
// Notes:
//-----------------------------------------------------------------------------
static int _nodataCallback( sceMpeg *mp, sceMpegCbData *cbdata, void *data )
{
	u_int dmaSize;
	int availData;

	(void) mp, cbdata, data;
	
	if(*D4_CHCR & (1 << 8)) return 1;  // If toIPU DMA is still in progress
	
	_str->ipuBuffPresent = _str->ipuBuffFuture;
	
	availData = _str->ipuBuffPast - _str->ipuBuffFuture;    // data ready for processing
	if(availData < 0) availData += (int) _str->ipuBuffSize;  // ring buffer
	
	while(availData < DMACHUNKSIZE)
	{
		if(_fillBuff(1) == 0)
		{
			if(_str->abort)
			{
				if(_str->video == SHELLMPEG_VIDEO_M2V_CSCIPU)
#if SCE_LIBRARY_VERSION >= 0x2600
					sceMpegGetPictureAbort(&_str->mp);
#else
				    WARNING(0, "Cannot abort MPEG decoding with libs < 2.6.0");
#endif

				return 1;
			}
		
			// More data cannot be obtained from the demultiplexer so this
			// must be the end of the stream. Round up the data to ensure
			// there's no remainder from the quadword-multiple transfer,
			// otherwise the decoder won't receive the MPEG end sequence code.

			PRINT("End of stream presumed");
			availData += 15;
			break;
		}  
		
		availData = _str->ipuBuffPast - _str->ipuBuffFuture;   // data ready for processing
		if(availData < 0) availData += (int) _str->ipuBuffSize; // ring buffer
	}

	dmaSize = (u_int) availData & ~0xf;
	if(dmaSize > DMACHUNKSIZE) dmaSize = DMACHUNKSIZE;
	
	if((_str->ipuBuffPresent + dmaSize) > (_str->ipuBuff + _str->ipuBuffSize))
		dmaSize = (u_int) (_str->ipuBuff + _str->ipuBuffSize - _str->ipuBuffPresent);

	shellDmaStart(SHELLDMA_CHANNEL_TOIPU, _str->ipuBuffPresent, dmaSize / 16);

	_str->ipuBuffFuture = _str->ipuBuffPresent + dmaSize;
	
	if(_str->ipuBuffFuture >= (_str->ipuBuff + _str->ipuBuffSize))  // ring buffer
		_str->ipuBuffFuture -= _str->ipuBuffSize;  
	
	return 1;
}

//-----------------------------------------------------------------------------
// _backgroundCallback()
// Description: Callback function for performing background processing while
//              IPU is busy doing colour-space conversion
// Parameters:
//   mp    - Ignored
//   cbstr - Ignored
//   data  - Ignored
// Returns: 1 (ignored by sceMpegDemuxPss())
// Notes:
//   Calculates the amount of time available and calls the core's idle function
//   until this period expires. (User is advised to use the available time to
//   refill buffers with shellMpegRecharge()).
//-----------------------------------------------------------------------------
static int _backgroundCallback( sceMpeg *mp, sceMpegCbData *cbdata, void *data )
{
	(void) mp, cbdata, data;
	
	while(sceIpuSync(1, 0)) shellCoreCallIdleRoutine();
	return 1;
}

//-----------------------------------------------------------------------------
// _errorCallback()
// Description: Callback function for handling MPEG video decoding errors
// Parameters:
//   mp    - Ignored
//   cbstr - Ignored
//   data  - Ignored
// Returns: 1 (ignored by sceMpegDemuxPss())
// Notes: Doesn't attempt recovery
//-----------------------------------------------------------------------------
static int _errorCallback( sceMpeg *mp, sceMpegCbData *cbdata, void *data )
{
	(void) mp, cbdata, data;

	PRINT("MPEG decoding error: '%s'",
		((sceMpegCbDataError *) cbdata)->errMessage);  
	return 1;
}

//-----------------------------------------------------------------------------
// _fillBuff()
// Description: Fill the demultiplex buffers from READCHUNKSIZE worth of
//              input data
// Parameters: blocking - 0 - don't block
//                        1 - block until some data processed or stream has
//                            ended
// Returns: number of bytes put into buffers
// Notes:
//-----------------------------------------------------------------------------
static int _fillBuff( int blocking )
{
	u_char *startPos;
	int procBytes;
	
	if(_str->mplex == SHELLMPEG_MULTIPLEX_NONE)
	{
		// Streaming from an .m2v or .ipu

		sceMpegCbDataStr cbstr;
		
		if(!_str->pssBuffFullness)
		{
			do
			{
				_str->pssBuffFullness = _str->fileLoadFn(_str->pssBuff, READCHUNKSIZE,
												   	   _str->fileFnParam);
				if(_str->pssBuffFullness < 0)
				{
					_str->abort = 1;
					return 0;
				}

				if((_str->pssBuffFullness == 0) && !blocking) return 0;
				
			} while(_str->pssBuffFullness == 0);
		}
		
		cbstr.len = (u_int) _str->pssBuffFullness;
		cbstr.data = _str->pssBuff;

		procBytes = _videoCallback(&_str->mp, &cbstr, 0)
			? (int) cbstr.len : 0;
		
		_str->pssBuffFullness -= procBytes;
		
		return procBytes;
	}
	
	else  // Stream from a multiplex
	{
		if(_str->pssBuffFullness)
			startPos = _str->pssBuff + READCHUNKSIZE - _str->pssBuffFullness;
		
		else
		{
			do
			{
				_str->pssBuffFullness = _str->fileLoadFn(_str->pssBuff, READCHUNKSIZE,
												       _str->fileFnParam);
				
				if(_str->pssBuffFullness < 0)
				{
					_str->abort = 1;
					return 0;
				}

				if((_str->pssBuffFullness == 0) && !blocking) return 0;
				
			} while(_str->pssBuffFullness == 0);
			
			startPos = _str->pssBuff;
		}
		
		_str->audioFull = 0;
		while(1)
		{
			procBytes = sceMpegDemuxPss(&_str->mp, (u_char *) startPos,
										_str->pssBuffFullness);

			if(procBytes == 0 && blocking && _str->audioFull)
			{				
				// Demuxed video data *must* be received (hence blocking), but
				// we can't demux more data from the pss stream because the
				// audio buffer's full. For correctly multiplexed data and a
				// functioning audio decoder this should never happen. However
				// we take care of this case to prevent serious problems just
				// in case.
				
				WARNING(0, "Emptying the full audio buffer to prevent deadlock");
			
				if(_str->audio == SHELLMPEG_AUDIO_PCM)
					_str->audioBuffFullness = _str->audioBuffFullness % 1024;
				
				else if(_str->audio == SHELLMPEG_AUDIO_ADPCM)
					_str->audioBuffFullness = _str->audioBuffFullness %
								  		(_str->sshd.interSize * _str->sshd.ch);

				else if(_str->audio == SHELLMPEG_AUDIO_SPDIF)
					_str->audioBuffFullness = _str->audioBuffFullness % 512;
			}
		
			else break;
		}

		_str->pssBuffFullness -= procBytes;

		return procBytes;
	}
}
