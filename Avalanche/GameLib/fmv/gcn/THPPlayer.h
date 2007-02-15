/*---------------------------------------------------------------------------*
  Project:  Dolphin
  File:     THPPlayer.h

  Copyright 2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/GameLibnew/fmv/gcn/THPPlayer.h $
 * 
 * 3     5/14/03 2:25p Todd Blackburn
 * 
 * 2     10/13/02 9:51p Todd Blackburn
 * 
 * 1     4/03/02 10:14a Todd Blackburn
    
    1     02/01/16 11:03a Akagi
    Initial revision made by Suzuki-san (IRD).
  $NoKeywords: $  

 *---------------------------------------------------------------------------*/

#ifndef __THPPLAYER_H__
#define __THPPLAYER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <Dolphin/types.h>
#include <dolphin/thp.h>

#define THP_VERSION (0x10000)

#define NTSC_RATE (5994)
#define PAL_RATE  (5000)

#define SAMPLE2BYTE(x) ((x) << 2)
#define BYTE2SAMPLE(x) ((x) >> 2)

#define READ_THREAD_PRIORITY  8
#define AUDIO_THREAD_PRIORITY 12
#define VIDEO_THREAD_PRIORITY 20

#define READ_BUFFER_NUM    10
#define DECODE_BUFFER_NUM  3

#define STOP    0x0
#define PREPARE 0x1
#define PLAY    0x2
#define PLAYED  0x3
#define PAUSE   0x4
#define ERROR   0x5

#define NONE     0x0000
#define LOOP     0x0001
#define ODD_INT  0x0002
#define EVEN_INT 0x0004

#define SAMPLES_PER_AUDIO_FRAME 160 // 5msec (32khz)
#define BYTES_PER_AUDIO_FRAME   (SAMPLES_PER_AUDIO_FRAME * 4)

#define ALONE      0x00
#define WITH_AX    0x01
#define WITH_MUSYX 0x02

typedef enum
{
    VIDEO_COMP,
    AUDIO_COMP
} THPComponent;

typedef struct
{
    char magic[4];
    u32 version;
    u32 bufSize;
    u32 audioMaxSamples;
    f32 frameRate;
    u32 numFrames;
    u32 firstFrameSize;
    u32 movieDataSize;
    u32 compInfoDataOffsets;
    u32 offsetDataOffsets;
    u32 movieDataOffsets;
    u32 finalFrameDataOffsets;
} THPHeader;

typedef struct
{
    u32 numComponents;
    u8  frameComp[16];
} THPFrameCompInfo;

typedef struct
{
    u32 xSize;
    u32 ySize;
} THPVideoInfo;

typedef struct
{
    u32 sndChannels;
    u32 sndFrequency;
    u32 sndNumSamples;
} THPAudioInfo;

typedef struct
{
    u8  *ptr;
    s32 frameNumber;
} THPReadBuffer;

typedef struct
{
    u8  *ytexture;
    u8  *utexture;
    u8  *vtexture;
    s32 frameNumber;
} THPTextureSet;

typedef struct
{
    s16 *buffer;
    s16 *curPtr;
    u32 validSample;
    s32 frameNumber;
} THPAudioBuffer;

typedef struct
{
    DVDFileInfo         fileInfo;
    THPHeader           header;
    THPFrameCompInfo    compInfo;
    THPVideoInfo        videoInfo;
    THPAudioInfo        audioInfo;
    void                *thpWork;
    BOOL                open;
    u8                  state;
    u8                  internalState;    
    u8                  playFlag;
    u8                  audioExist;
    s32                 dvdError;
    s32                 videoError;
    BOOL                onMemory;
    u8                  *movieData;
    s32                 initOffset;
    s32                 initReadSize;
    s32                 initReadFrame;
    s64                 retraceCount;
    s32                 prevCount;
    s32                 curCount;
    s32                 videoAhead;
    f32                 curVolume;
    f32                 targetVolume;
    f32                 deltaVolume;
    s32                 rampCount;
    s32                 curVideoNumber;
    s32                 curAudioNumber;
    THPTextureSet       *dispTextureSet;
    THPAudioBuffer      *playAudioBuffer;
    THPReadBuffer       readBuffer[READ_BUFFER_NUM];
    THPTextureSet       textureSet[DECODE_BUFFER_NUM];
    THPAudioBuffer      audioBuffer[DECODE_BUFFER_NUM];
} THPPlayer;

extern BOOL THPPlayerInit(s32 audioSystem);
extern void THPPlayerQuit(void);
extern BOOL THPPlayerOpen(char *fileName, BOOL onMemory, s32 volume);
extern BOOL THPPlayerClose(void);
extern u32  THPPlayerCalcNeedMemory(void);
extern BOOL THPPlayerSetBuffer(u8 *buffer);
extern BOOL THPPlayerPrepare(s32 frameNum, s32 playFlag);
extern BOOL THPPlayerPlay(void);
extern void THPPlayerStop(void);
extern BOOL THPPlayerPause(void);
extern BOOL THPPlayerSkip(void);
extern s32  THPPlayerDrawCurrentFrame(GXRenderModeObj *rmode, 
                                      u32 x, 
                                      u32 y, 
                                      u32 polygonW,
                                      u32 polygonH);
extern BOOL THPPlayerGetVideoInfo(THPVideoInfo *videoInfo);
extern BOOL THPPlayerGetAudioInfo(THPAudioInfo *audioInfo);
extern f32  THPPlayerGetFrameRate(void);
extern u32  THPPlayerGetTotalFrame(void);
extern s32  THPPlayerGetState(void);
extern void THPPlayerDrawDone(void);
extern BOOL THPPlayerSetVolume(s32 vol, s32 time);
extern s32  THPPlayerGetVolume(void);

#ifdef __cplusplus
}
#endif

#endif // __THPPLAYER_H__
