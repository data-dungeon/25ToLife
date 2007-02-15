//-----------------------------------------------------------------------------
//                   shellMpeg - IPU/MPEG streaming module
//
// $Workfile: shellMpeg.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------


#ifndef _SHELLMPEG_H_
#define _SHELLMPEG_H_

#include <libmpeg.h>
#include "shellTexture.h"


//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef enum
{
	SHELLMPEG_MULTIPLEX_NONE,
	SHELLMPEG_MULTIPLEX_PSS

} EShellMpegMultiplexType;


typedef enum
{
	SHELLMPEG_VIDEO_IPU_STANDARD,
	SHELLMPEG_VIDEO_IPU_COLUMNS,
	SHELLMPEG_VIDEO_M2V_CSCIPU

} EShellMpegVideoType;

typedef enum
{
	SHELLMPEG_AUDIO_NONE,
	SHELLMPEG_AUDIO_PCM,
	SHELLMPEG_AUDIO_ADPCM,
	SHELLMPEG_AUDIO_SPDIF

} EShellMpegAudioType;

typedef enum
{
	SHELLMPEG_OUTPUT_PATH3TAGS = 256,
	SHELLMPEG_OUTPUT_TEXTURE,
	SHELLMPEG_OUTPUT_PATH3TAGS_16,
	SHELLMPEG_OUTPUT_TEXTURE_16

} EShellMpegOutputType;

typedef struct
{
	char id[4];		// 'S''S''h''d'
	int size;		// 24
	int type;		// 0: 16bit big endian
					// 1: 16bit little endian
					// 16: SPU2-ADPCM (VAG) 
	int rate;		// sampling rate (Hz)
	int ch;			// number of channels
	int interSize;	// interleave size
	int loopStart;	// loop start block address
	int loopEnd;	// loop end block sddress

} ShellMpegADSHeader;

typedef struct
{
	char id[4];		// 'S''S''b''d'
	int size;		// size of audio data

} ShellMpegADSBody;

typedef struct
{
	sceMpeg mp;

	u_char *pssBuff;
	int pssBuffFullness;

	int ipuNumFrames;
	int framesDecoded;

	u_int width;
	u_int height;

	int (*fileLoadFn) (unsigned char *, int, int);
	void (*fileInitFn) (int);
	int fileFnParam;

	u_int dbw;
	sceIpuDmaEnv dmaEnv;

	sceIpuRGB32 *rgb[2];
	sceIpuRAW8  *raw8[2];

	u_char *workArea;

	union
	{
		ShellDmaTag *path3;      // For IPU images or M2Vs with IPU CSC
		struct
		{
			u_int *prog[2];   // Not currently supported
			u_int *inter[2];  // Not currently supported
			u_int *top[2];    // Not currently supported
			u_int *bot[2];    // Not currently supported
		} path1;
	} tags[2];
	
	ShellTextureInfo *texture[2];

	// ipuBuff - data that will be sent to the IPU, either a .m2v or .ipu.

	// Ring buffer for demultiplexed m2v or ipu data (uncached access)
	//
	//  Regions: times are relative to DMA and circular wrt BuffStart, BuffSize
	//
	//  ipuBuffPresent -> ipuBuffFuture   = Area currently being processed
	//  ipuBuffFuture  -> ipuBuffPast     = Demuxed data ready for processing
	//  ipuBuffPast    -> ipuBuffPresent  = Processed data
	//
	u_char *ipuBuff;
	u_int ipuBuffSize;
	u_char *ipuBuffPast;
	u_char *ipuBuffPresent;
	u_char *ipuBuffFuture;

	// Ring buffer for audio data (audioBuff)
	//
	// Note that audioBuffFullness can wrap
	//
	//  |<----------------------- audioBuffSize ----------------------->|
	//  |<--- audioBuffPos --->|<--- audioBuffFullness --->|
	//
 	//  +----------------------+---------------------------+------------+
	//  |      old data        |         new data          |  old data  |
	//  +----------------------+---------------------------+------------+
	//
	//  ^ audioBuff

	u_char *audioBuff;
	u_int audioBuffSize;
	int audioBuffPos;
	int audioBuffFullness;

	ShellMpegADSHeader sshd;
	ShellMpegADSBody ssbd;

	int audioFull;
	int audioHeaderCount;

	EShellMpegMultiplexType mplex;
	EShellMpegVideoType video;
	EShellMpegAudioType audio;
	int audioChannel;
	int output;
	
	int doubleBuffer;
	int abort;

} ShellMpegStream;


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

void shellMpegInit( void );
EShellResult shellMpegCreate( ShellMpegStream *str,
					EShellMpegMultiplexType mplex,
					EShellMpegVideoType video,
					EShellMpegAudioType audio,
					int audioChannel,
					void (*fileInitFn) ( int ),
					int (*fileLoadFn) ( unsigned char *, int, int ),
					int	fileFnParam,
					int doubleBuffer,
					u_int dbw,
					EShellMpegOutputType output );
EShellResult shellMpegGetVideo( ShellDmaTag **tags, u_int dbp );
EShellResult shellMpegGetVideoAsTexture( ShellTextureInfo **tex );
void shellMpegGetAudio( u_int (*receiver) ( u_char *, u_int, u_int, u_int, int ),
					   int param );
int shellMpegIsEnd( void );
void shellMpegRewind( void );
ShellMpegStream *shellMpegBind( ShellMpegStream *str );
void shellMpegRecharge( void );
void shellMpegDelete( void );
ShellTextureInfo *shellMpegSingleIpuTexture( char *ipu, u_int size,
											 int columns, int opm, int dither );
#ifdef __cplusplus
}
#endif

#endif _SHELLMPEG_H_
