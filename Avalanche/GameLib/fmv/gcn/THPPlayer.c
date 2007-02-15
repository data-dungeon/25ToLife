/*---------------------------------------------------------------------------*
  Project:  Dolphin
  File:     THPPlayer.c

  Copyright 2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/GameLibnew/fmv/gcn/THPPlayer.c $
 * 
 * 3     5/14/03 2:25p Todd Blackburn
 * 
 * 2     10/13/02 9:51p Todd Blackburn
 * 
 * 1     4/03/02 10:14a Todd Blackburn
    
    1     02/01/16 10:53a Akagi
    Initial revision made by Suzuki-san (IRD).
  $NoKeywords: $  

 *---------------------------------------------------------------------------*/

#include <dolphin.h>
#include <string.h>
#include <math.h>
#include "THPPlayer.h"
#include "THPVideoDecode.h"
#include "THPAudioDecode.h"
#include "THPRead.h"
#include "THPDraw.h"

/*---------------------------------------------------------------------------*
   Global Function
 *---------------------------------------------------------------------------*/
 
void PrepareReady(BOOL flag);

/*---------------------------------------------------------------------------*
   Static Function
 *---------------------------------------------------------------------------*/
 
static void          PlayControl(u32 retraceCount);
static BOOL          ProperTimingForStart(void);
static BOOL          ProperTimingForGettingNextFrame(void);
static void          PushUsedTextureSet(THPTextureSet *buffer);
static THPTextureSet *PopUsedTextureSet(void);
static void          THPAudioMixCallback(void);
static void          MixAudio(s16 *destination, s16 *source, u32 sample);

/*---------------------------------------------------------------------------*
   Global Variable
 *---------------------------------------------------------------------------*/
 
THPPlayer ActivePlayer;

/*---------------------------------------------------------------------------*
   Static Variable
 *---------------------------------------------------------------------------*/
 
// 32768 * ((vol * vol) / (127 * 127)) 
static u16 VolumeTable[] =
{
      0,     2,     8,    18,    32,    50,    73,    99,
    130,   164,   203,   245,   292,   343,   398,   457,
    520,   587,   658,   733,   812,   895,   983,  1074,
   1170,  1269,  1373,  1481,  1592,  1708,  1828,  1952,
   2080,  2212,  2348,  2488,  2632,  2781,  2933,  3090,
   3250,  3415,  3583,  3756,  3933,  4114,  4298,  4487,
   4680,  4877,  5079,  5284,  5493,  5706,  5924,  6145,
   6371,  6600,  6834,  7072,  7313,  7559,  7809,  8063,
   8321,  8583,  8849,  9119,  9394,  9672,  9954, 10241,
  10531, 10826, 11125, 11427, 11734, 12045, 12360, 12679,
  13002, 13329, 13660, 13995, 14335, 14678, 15025, 15377,
  15732, 16092, 16456, 16823, 17195, 17571, 17951, 18335,
  18723, 19115, 19511, 19911, 20316, 20724, 21136, 21553,
  21974, 22398, 22827, 23260, 23696, 24137, 24582, 25031,
  25484, 25941, 26402, 26868, 27337, 27810, 28288, 28769,
  29255, 29744, 30238, 30736, 31238, 31744, 32254, 32768
};

static BOOL              Initialized = 0;
static s32               WorkBuffer[16] ATTRIBUTE_ALIGN(32);
static OSMessageQueue    PrepareReadyQueue;
static OSMessageQueue    UsedTextureSetQueue;
static OSMessage         PrepareReadyMessage;
static OSMessage         UsedTextureSetMessage[DECODE_BUFFER_NUM];
static VIRetraceCallback OldVIPostCallback = NULL;
static s16               SoundBuffer[2][SAMPLES_PER_AUDIO_FRAME * 2] ATTRIBUTE_ALIGN(32);
static s32               SoundBufferIndex;
static AIDCallback       OldAIDCallback = NULL;
static s16               *LastAudioBuffer;
static s16               *CurAudioBuffer;
static s32               AudioSystem = 0;

/*---------------------------------------------------------------------------*
    Name:           THPPlayerInit

    Description:    Enables locked-cache, and initializes player and THP 
                    library.
                    Register AI FIFO DMA call back for player

    Arguments:      None

    Returns:        Returns TRUE if succeeds, and FALSE if fails.
 *---------------------------------------------------------------------------*/

BOOL THPPlayerInit(s32 audioSystem)
{
    BOOL old;
    
    ASSERTMSG(audioSystem >= 0 && audioSystem <= WITH_MUSYX, "audioSystem flag is invalid\n");
    
    memset(&ActivePlayer, 0, sizeof(THPPlayer));
    
    LCEnable();
    
    OSInitMessageQueue(&UsedTextureSetQueue,
                       UsedTextureSetMessage,
                       DECODE_BUFFER_NUM);
                       
    if (THPInit() == FALSE)
    {
        return FALSE;
    }
  
    old = OSDisableInterrupts();
    
    AudioSystem      = audioSystem;
    SoundBufferIndex = 0;
    LastAudioBuffer  = NULL;
    CurAudioBuffer   = NULL;
    
    OldAIDCallback = AIRegisterDMACallback(THPAudioMixCallback);
        
    if (OldAIDCallback == NULL && AudioSystem != ALONE)
    {
        AIRegisterDMACallback(NULL);
            
        OSRestoreInterrupts(old);
            
#ifdef _DEBUG
        OSReport("Pleae call AXInit or sndInit before you call THPPlayerInit\n");
#endif
        return FALSE;
    }
    
    OSRestoreInterrupts(old);
    
    if (AudioSystem == ALONE)
    {
        memset(SoundBuffer, 0, BYTES_PER_AUDIO_FRAME << 1);
    
        DCFlushRange(SoundBuffer, BYTES_PER_AUDIO_FRAME << 1);
    
        AIInitDMA((u32)SoundBuffer[SoundBufferIndex], BYTES_PER_AUDIO_FRAME);
         
        AIStartDMA();
    }
    
    Initialized = TRUE;
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerQuit

    Description:    Disables locked cache.
                    Restores AI FIFO DMA callback to the status before 
                    THPPlayerInit is called. 

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/

void THPPlayerQuit(void)
{
    BOOL old;
    
    LCDisable();
    
    old = OSDisableInterrupts();

    if (OldAIDCallback)
    {
        AIRegisterDMACallback(OldAIDCallback);
    }

    OSRestoreInterrupts(old);
        

    Initialized = FALSE;
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerOpen

    Description:    Opens THP movie file. Loads required data.

    Arguments:      fileName   Filename of THP movie.
                    onMemory   Flag to do OnMemory playback or not.

    Returns:        Returns TRUE if succeeds, and FALSE if fails.
 *---------------------------------------------------------------------------*/
 
BOOL THPPlayerOpen(char *fileName, BOOL onMemory, s32 volume)
{
    s32 offset, i;
    
    if (!Initialized)
    {
#ifdef _DEBUG
        OSReport("You must call THPPlayerInit before you call this function\n");
#endif
        return FALSE;
    }
    
    if (ActivePlayer.open)
    {
#ifdef _DEBUG
        OSReport("Can't open %s. Because thp file have already opened.\n");
#endif        
        return FALSE;
    }
    
    // Clears current video data and audio data
    memset(&ActivePlayer.videoInfo, 0, sizeof(THPVideoInfo));
    memset(&ActivePlayer.audioInfo, 0, sizeof(THPAudioInfo));
    
    if (DVDOpen(fileName, &ActivePlayer.fileInfo) == FALSE)
    {
#ifdef _DEBUG
        OSReport("Can't open %s.\n", fileName);
#endif
        return FALSE;
    }
    
    // Get THP header
    if (DVDRead(&ActivePlayer.fileInfo, WorkBuffer, 64, 0) < 0)
    {           
#ifdef _DEBUG
        OSReport("Fail to read the header from THP file.\n");
#endif
        DVDClose(&ActivePlayer.fileInfo);
        return FALSE;
    }
    
    memcpy(&ActivePlayer.header, WorkBuffer, sizeof(THPHeader));
    
    // Check if THP movie file
    if (strcmp(ActivePlayer.header.magic, "THP") != 0)
    {
#ifdef _DEBUG
        OSReport("This file is not THP file.\n");
#endif
        DVDClose(&ActivePlayer.fileInfo);
        return FALSE;
    }
    
    // Check version
    if (ActivePlayer.header.version != THP_VERSION)
    {
#ifdef _DEBUG
        OSReport("invalid version.\n");
#endif
        DVDClose(&ActivePlayer.fileInfo);
        return FALSE;
    }
    
    offset = (s32)ActivePlayer.header.compInfoDataOffsets;
    
    // Get component data in frame
    if (DVDRead(&ActivePlayer.fileInfo, WorkBuffer, 32, offset) < 0)
    {
#ifdef _DEBUG
        OSReport("Fail to read the frame component infomation from THP file.\n");
#endif
        DVDClose(&ActivePlayer.fileInfo);
        return FALSE;
    }
    
    memcpy(&ActivePlayer.compInfo, WorkBuffer, sizeof(THPFrameCompInfo));
        
    offset += sizeof(THPFrameCompInfo);
        
    ActivePlayer.audioExist = 0;
    
    // Check components in frame
    for(i = 0 ; i < ActivePlayer.compInfo.numComponents ; i++)
    {   
        switch(ActivePlayer.compInfo.frameComp[i])
        {
            case VIDEO_COMP: // Get video data of components
                if (DVDRead(&ActivePlayer.fileInfo, WorkBuffer, 32, offset) < 0)
                {
#ifdef _DEBUG
                    OSReport("Fail to read the video infomation from THP file.\n");
#endif
                    DVDClose(&ActivePlayer.fileInfo);
                    return FALSE;
                }
                memcpy(&ActivePlayer.videoInfo, WorkBuffer, sizeof(THPVideoInfo));
                offset += sizeof(THPVideoInfo);
                break;
            case AUDIO_COMP: // Get audio data of components
                if (DVDRead(&ActivePlayer.fileInfo, WorkBuffer, 32, offset) < 0)
                {
#ifdef _DEBUG
                    OSReport("Fail to read the video infomation from THP file.\n");
#endif
                    DVDClose(&ActivePlayer.fileInfo);
                    return FALSE;
                }
                memcpy(&ActivePlayer.audioInfo, WorkBuffer, sizeof(THPAudioInfo));
                ActivePlayer.audioExist = 1;
                offset += sizeof(THPAudioInfo);
                    
                break;
            default:
#ifdef _DEBUG
                OSReport("Unknow frame components.\n");
#endif
                return FALSE;
        }
    }
    
    ActivePlayer.state        = ActivePlayer.internalState = STOP;
    ActivePlayer.playFlag     = 0;
    ActivePlayer.onMemory     = onMemory;
    ActivePlayer.open         = TRUE;
    ActivePlayer.curVolume    = (float)volume;
    ActivePlayer.targetVolume = ActivePlayer.curVolume;
    ActivePlayer.rampCount    = 0;
        
    return TRUE;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerClose

    Description:    Close THP movie file.

    Arguments:      None

    Returns:        Returns TRUE if succeeds, and FALSE if fails.
 *---------------------------------------------------------------------------*/
 
BOOL THPPlayerClose(void)
{
    if (ActivePlayer.open)
    {
        if (ActivePlayer.state == STOP)
        {
            ActivePlayer.open = FALSE;
            DVDClose(&ActivePlayer.fileInfo);
            
            return TRUE;
        }
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerCalcNeedMemory

    Description:    Calculates needed memory for THP movie playback

    Arguments:      None

    Returns:        Returns needed memory size if succeeds, and 0 if fails.
 *---------------------------------------------------------------------------*/
 
u32 THPPlayerCalcNeedMemory(void)
{   
    u32 size;
    
    if (ActivePlayer.open)
    {
        // Buffer size for read
        if (ActivePlayer.onMemory)
        {
            size = OSRoundUp32B(ActivePlayer.header.movieDataSize);
        }
        else
        {
            size = OSRoundUp32B(ActivePlayer.header.bufSize) * READ_BUFFER_NUM;
        }

        // Size of texture buffer
        size += OSRoundUp32B(ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize) 
              * DECODE_BUFFER_NUM; //Y
        size += OSRoundUp32B(ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize / 4)
              * DECODE_BUFFER_NUM; //U
        size += OSRoundUp32B(ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize / 4)
              * DECODE_BUFFER_NUM; //V
        
        // Size of audio buffer
        if (ActivePlayer.audioExist)
        {
            size += (OSRoundUp32B(ActivePlayer.header.audioMaxSamples * 4) * DECODE_BUFFER_NUM);
        }
        
        size += THP_WORK_SIZE;
        
        return size;
    }
    
    return 0;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerSetBuffer

    Description:    Allocate required memory for movie playback to THPPlayer
                    structure.

    Arguments:      buffer   Pointer to memory area for THP movie set aside 
                             externally.

    Returns:        Returns TRUE if succeeds, and FALSE if fails.
 *---------------------------------------------------------------------------*/
 
BOOL THPPlayerSetBuffer(u8 *buffer)
{
    u32 i;
    u8  *ptr;
    u32 ysize, uvsize;
    
    ASSERTMSG(buffer != NULL, "buffer is NULL");
    
    if (ActivePlayer.open && (ActivePlayer.state == STOP))
    {
        ptr = buffer;
        
        // Set buffer for read
        if (ActivePlayer.onMemory)
        {
            ActivePlayer.movieData = ptr;
            ptr += ActivePlayer.header.movieDataSize;
        }
        else
        {
            for (i = 0 ; i < READ_BUFFER_NUM ; i++)
            {
                ActivePlayer.readBuffer[i].ptr = ptr;
                ptr += OSRoundUp32B(ActivePlayer.header.bufSize);
            }
        }
        
        ysize  = OSRoundUp32B(ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize);
        uvsize = OSRoundUp32B(ActivePlayer.videoInfo.xSize * ActivePlayer.videoInfo.ySize / 4);
        
        // Set texture buffer
        for (i = 0 ; i < DECODE_BUFFER_NUM ; i++)
        {
            ActivePlayer.textureSet[i].ytexture = ptr;
            DCInvalidateRange(ptr, ysize);
            ptr += ysize;
            ActivePlayer.textureSet[i].utexture = ptr;
            DCInvalidateRange(ptr, uvsize);
            ptr += uvsize;
            ActivePlayer.textureSet[i].vtexture = ptr;
            DCInvalidateRange(ptr, uvsize);
            ptr += uvsize;
        }
        
        // Set audio buffer
        if (ActivePlayer.audioExist)
        {
            for (i = 0 ; i < DECODE_BUFFER_NUM ; i++)
            {
                ActivePlayer.audioBuffer[i].buffer = (s16 *)ptr;
                ActivePlayer.audioBuffer[i].curPtr = (s16 *)ptr;
                ActivePlayer.audioBuffer[i].validSample = 0;
                ptr += OSRoundUp32B(ActivePlayer.header.audioMaxSamples * 4);
            }
        }
        
        ActivePlayer.thpWork = (void *)ptr;
        
        return TRUE;
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           InitAllMessageQueue

    Description:    Initializes queue used with video decode thread, audio 
                    thread, and read thread.

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void InitAllMessageQueue(void)
{
    s32 i;
    THPReadBuffer  *readBuffer;
    THPTextureSet  *textureSet;
    THPAudioBuffer *audioBuffer;
    
    // Push all read buffers to free read buffer queue
    if (!ActivePlayer.onMemory)
    {
        for (i = 0 ; i < READ_BUFFER_NUM ; i++)
        {
            readBuffer = &ActivePlayer.readBuffer[i];
            PushFreeReadBuffer(readBuffer);
        }
    }
    
    // Push all texture buffers for storing decoded THP video data to
    // free texture buffer queue.
    for (i = 0 ; i < DECODE_BUFFER_NUM ; i++)
    {
        textureSet = &ActivePlayer.textureSet[i];
        PushFreeTextureSet(textureSet);
    }
    
    // Push all audio buffers for storing decoded THP audio data to
    // free audio buffer queue.
    if (ActivePlayer.audioExist)
    {
        for (i = 0 ; i < DECODE_BUFFER_NUM ; i++)
        {
            audioBuffer = &ActivePlayer.audioBuffer[i];
            PushFreeAudioBuffer(audioBuffer);
        }
    }
    
    OSInitMessageQueue(&PrepareReadyQueue,
                       &PrepareReadyMessage,
                       1);
}

/*---------------------------------------------------------------------------*
    Name:           WaitUntilPrepare

    Description:    Waits until playback preparations completed.

    Arguments:      None

    Returns:        If playback preparations complete, returns TRUE. If 
                    preparations fail, returns FALSE.
 *---------------------------------------------------------------------------*/
 
static BOOL WaitUntilPrepare(void)
{
    OSMessage msg;
    
    OSReceiveMessage(&PrepareReadyQueue, &msg, OS_MESSAGE_BLOCK);
    
    if ((BOOL)msg)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
    Name:           PrepareReady

    Description:    Sends message about whether or not playback preparations
                    are completed.

    Arguments:      flag   If playback preparations completed, TRUE.  If 
                    preparations fail, FALSE.

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void PrepareReady(BOOL flag)
{
    OSSendMessage(&PrepareReadyQueue, (OSMessage)flag, OS_MESSAGE_BLOCK);
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerPrepare

    Description:    Carries out preparations for movie playback. Does not 
                    return until THP movie playback preparations completed.

    Arguments:      frameNum  Specify frame number to start movie playback
                              If the THP movie file does not have 
                              THPFrameOffsetData, everything but 0 is an error.
                    playFlag  Specify playback flag.
                              You can ombine LOOP ODD_INT EVEN_INT and specify.
                              Use ODD_INT and EVEN_INT with interlaced movie.
                              You cannot specify ODD_INT and EVEN_INT at same
                               time.

    Returns:        If playback preparations complete, returns TRUE. If 
                    preparations fail, returns FALSE.
 *---------------------------------------------------------------------------*/
 
BOOL THPPlayerPrepare(s32 frameNum, s32 playFlag)
{
    s32 offset;
    u8  *ptr;
    
    if (ActivePlayer.open && (ActivePlayer.state == STOP))
    {
        // Check if specified starting frame number is appropriate
        if (frameNum > 0)
        {
            // Does THP movie file have offset data?
            if (!ActivePlayer.header.offsetDataOffsets)
            {
#ifdef _DEBUG
                OSReport("This thp file doesn't have the offset data\n");
#endif
                return FALSE;
            }
            
            // Does starting frame number exceed total frames?
            if (ActivePlayer.header.numFrames > frameNum)
            {
                offset = (s32)(ActivePlayer.header.offsetDataOffsets + (frameNum - 1) * 4);
                
                if (DVDRead(&ActivePlayer.fileInfo,
                            WorkBuffer,
                            32,
                            offset) < 0)
                {
#ifdef _DEBUG
                    OSReport("Fail to read the offset data from THP file.\n");
#endif
                    return FALSE;
                }
                
                //  Set starting file offset, frame size, and frame number
                ActivePlayer.initOffset    = (s32)(ActivePlayer.header.movieDataOffsets
                                           + WorkBuffer[0]);
                ActivePlayer.initReadFrame = frameNum;
                ActivePlayer.initReadSize  = (s32)(WorkBuffer[1] - WorkBuffer[0]);
            }
            else
            {
#ifdef _DEBUG
                OSReport("Specified frame number is over total frame number\n");
#endif
                return FALSE;
            }
        }
        // If 0, from beginning
        else
        {
            ActivePlayer.initOffset    = (s32)ActivePlayer.header.movieDataOffsets;
            ActivePlayer.initReadSize  = (s32)ActivePlayer.header.firstFrameSize;
            ActivePlayer.initReadFrame = frameNum;
        }
        
        ActivePlayer.playFlag   = (u8)playFlag;
        ActivePlayer.videoAhead = 0;
        
        // If On Memory playback, load all THP movie data to memory
        if (ActivePlayer.onMemory)
        {
            if (DVDRead(&ActivePlayer.fileInfo,
                        ActivePlayer.movieData,
                        (s32)ActivePlayer.header.movieDataSize,
                        (s32)ActivePlayer.header.movieDataOffsets) < 0)
            {
#ifdef _DEBUG
                OSReport("Fail to read all movie data from THP file\n");
#endif
                return FALSE;
            }
            
            ptr = ActivePlayer.movieData + ActivePlayer.initOffset - ActivePlayer.header.movieDataOffsets;
            
            // Create video decode thread
            CreateVideoDecodeThread(VIDEO_THREAD_PRIORITY, ptr);
        
            // Create audio decode thread if required
            if (ActivePlayer.audioExist)
            {
                CreateAudioDecodeThread(AUDIO_THREAD_PRIORITY, ptr);
            }
        }
        // Not On Memory playback
        else
        {
            // Create video decode thread
            CreateVideoDecodeThread(VIDEO_THREAD_PRIORITY, NULL);
            
            // Create audio decode thread if required
            if (ActivePlayer.audioExist)
            {
                CreateAudioDecodeThread(AUDIO_THREAD_PRIORITY, NULL);
            }
            
            // Create read thread
            CreateReadThread(READ_THREAD_PRIORITY);
        }
        
        // Initialize queues used with various threads
        InitAllMessageQueue();
        
        VideoDecodeThreadStart();
        
        if (ActivePlayer.audioExist)
        {
            AudioDecodeThreadStart();
        }
        
        if (!ActivePlayer.onMemory)
        {
            ReadThreadStart();
        }
        
        // Wait until thread preparation completed.
        if (WaitUntilPrepare() == FALSE)
        {
            return FALSE;
        }
        
        // If preparations complete, state goes to preparations complete
        ActivePlayer.state = PREPARE;
        ActivePlayer.internalState = STOP;
        
        // Initialize variables
        ActivePlayer.dispTextureSet  = NULL;
        ActivePlayer.playAudioBuffer = NULL;
        
        ActivePlayer.curVideoNumber = 0;
        ActivePlayer.curAudioNumber = 0;
        
        // Register VI post callback that controls playback
        OldVIPostCallback = VISetPostRetraceCallback(PlayControl);
        
        return TRUE;
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerPlay

    Description:    Start of movie playback.

    Arguments:      None

    Returns:        Returns TRUE if succeeds, and FALSE if fails.
 *---------------------------------------------------------------------------*/
 
BOOL THPPlayerPlay(void)
{
    if (ActivePlayer.open && ((ActivePlayer.state == PREPARE) || (ActivePlayer.state == PAUSE)))
    {
        ActivePlayer.state        = PLAY;
        ActivePlayer.prevCount    = 0;
        ActivePlayer.curCount     = 0;
        ActivePlayer.retraceCount = -1;
        
        return TRUE;
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerStop

    Description:    Stop for movie playback.
                    Returns VI post callback to original state and stops 
                    threads. 

    Arguments:      None

    Returns:        Returns TRUE if succeeds, and FALSE if fails.
 *---------------------------------------------------------------------------*/
 
void THPPlayerStop(void)
{
    THPTextureSet *texture;
    
    if (ActivePlayer.open && !(ActivePlayer.state == STOP))
    {
        ActivePlayer.state = ActivePlayer.internalState = STOP;
        
        // Return VI post callback
        VISetPostRetraceCallback(OldVIPostCallback);
        
        // Cancel if stopping threads and loading data
        if (!ActivePlayer.onMemory)
        {
            DVDCancel(&ActivePlayer.fileInfo.cb);
            ReadThreadCancel();
        }
        
        VideoDecodeThreadCancel();
        
        if (ActivePlayer.audioExist)
        {
            AudioDecodeThreadCancel();
        }
        
        // Empty played back texture queues.
        while (1)
        {
            texture = PopUsedTextureSet();
            if (texture == NULL)
            {
                break;
            }
        }

        ActivePlayer.curVolume = ActivePlayer.targetVolume;
        ActivePlayer.rampCount = 0;
        
        // Clear errors
        ActivePlayer.dvdError   = FALSE;
        ActivePlayer.videoError = FALSE;
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerPause

    Description:    Pause movie playback

    Arguments:      None

    Returns:        Returns TRUE if succeeds, and FALSE if fails.
 *---------------------------------------------------------------------------*/
 
BOOL THPPlayerPause(void)
{
    if (ActivePlayer.open && (ActivePlayer.state == PLAY))
    {
        ActivePlayer.state = ActivePlayer.internalState = PAUSE;
        
        return TRUE;
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerSkip

    Description:    Skip movie ahead one frame.

    Arguments:      None

    Returns:        Returns TRUE if succeeds, and FALSE if fails.
 *---------------------------------------------------------------------------*/
 
BOOL THPPlayerSkip(void)
{
    BOOL          old;
    s32           frameNumber, audioGet, videoGet;
    
    if (ActivePlayer.open && (ActivePlayer.state == PAUSE))
    {
        // Block in function until get decoded THP video data so
        // release played back THP video data in advance.
        THPPlayerDrawDone();
        
        // If have audio
        if (ActivePlayer.audioExist)
        {
            frameNumber = (s32)((ActivePlayer.curAudioNumber + ActivePlayer.initReadFrame)
                        % ActivePlayer.header.numFrames);
                        
            // Check if one shot and also if audio has reached end.
                        if (!(ActivePlayer.playFlag & LOOP) && (frameNumber == ActivePlayer.header.numFrames - 1))
            {
                if (ActivePlayer.playAudioBuffer)
                {
                    PushFreeAudioBuffer(ActivePlayer.playAudioBuffer);
                    ActivePlayer.playAudioBuffer = NULL;
                }
                
                audioGet = 0;
            }
            else
            {
                // Release current audio buffer
                if (ActivePlayer.playAudioBuffer)
                {
                    PushFreeAudioBuffer(ActivePlayer.playAudioBuffer);
                }
                
                // Wait until get next audio buffer
                ActivePlayer.playAudioBuffer = PopDecodedAudioBuffer(OS_MESSAGE_BLOCK);
                ActivePlayer.curAudioNumber = ActivePlayer.playAudioBuffer->frameNumber;
                
                audioGet = 1;
            }
        }
        
        frameNumber = (s32)((ActivePlayer.curVideoNumber + ActivePlayer.initReadFrame)
                    % ActivePlayer.header.numFrames);
                        
        // Check if one shot and also if video has reached end.
                if (!(ActivePlayer.playFlag & LOOP) && (frameNumber == ActivePlayer.header.numFrames - 1))
        {
            videoGet = 0;
        }
        else
        {
            // Release current texture buffer
            if (ActivePlayer.dispTextureSet)
            {
                PushFreeTextureSet(ActivePlayer.dispTextureSet);
            }
                
            // Wait until get next texture buffer
            ActivePlayer.dispTextureSet = PopDecodedTextureSet(OS_MESSAGE_BLOCK);
            ActivePlayer.curVideoNumber = ActivePlayer.dispTextureSet->frameNumber;
            
            if (ActivePlayer.audioExist)
            {
                old = OSDisableInterrupts();
                ActivePlayer.videoAhead--;
                OSRestoreInterrupts(old);
            }
            
            videoGet = 1;
        }
        
        if (audioGet || videoGet)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           PlayControl

    Description:    Controls movie playback. Gets decoded THP video data at 
                    appropriate timing.

    Arguments:      retraceCount  Current retrace count

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void PlayControl(u32 retraceCount)
{
    s32 diff, frameNumber;
    THPTextureSet *textureSet;
    
    if (OldVIPostCallback)
    {
        OldVIPostCallback(retraceCount);
    }
    
    textureSet = (THPTextureSet *)0xFFFFFFFF;
   
    if (ActivePlayer.open && (ActivePlayer.state == PLAY))
    {
        // If an error has occurred, change state to error.
        if (ActivePlayer.dvdError || ActivePlayer.videoError)
        {
            ActivePlayer.state = ActivePlayer.internalState = ERROR;
            
            return;
        }
        
        // Allow audio output 1 vsync after get starting THP video data
        // May be that movie rendering loop has looped with 1 vsync
        // The reason for this follows,
        //
        // "Flow from THPPlayerPlay to display of staring frame"
        //
        //           <Call THPPlayerPlay>
        //  -----------------vsync---------------------------
        //  <Get starting THP video data in VI post callback>
        //  <Render starting THP video data, and call 
        //  VISetNextFrameBuffer and VIFlush with user
        //  draw loop>
        // ------------------vsync----------------------------
        // From this point the movie is displayed on TV. Audio output is allowed
        // with this timing.
        if (ActivePlayer.retraceCount == 0 
                &&
           (ActivePlayer.internalState == STOP || ActivePlayer.internalState == PAUSE))
        {
            ActivePlayer.internalState = PLAY;
        }
        
        ActivePlayer.retraceCount++;
        
        // When start THP movie playback and when end pause
        if (ActivePlayer.internalState == STOP || ActivePlayer.internalState == PAUSE)
        {
            // Appropriate timing for start of playback?
            if (ProperTimingForStart())
            {
                // If THP movie has audio
                if (ActivePlayer.audioExist)
                {
                    // Calculate difference between current audio playback frames
                    // and current video playback frames
                    diff = ActivePlayer.curVideoNumber - ActivePlayer.curAudioNumber;
                    
                    // If audio is not slower than video, move video ahead
                    if (diff <= 1)
                    {
                        // Get decoded THP video data
                        textureSet = PopDecodedTextureSet(OS_MESSAGE_NOBLOCK);
                        
                        // Decrement decoded THP video data count
                        if (ActivePlayer.videoAhead > diff)
                        {
                            ActivePlayer.videoAhead--;
                        }
                    }
                    // Allow audio output if slow
                    else
                    {
                        ActivePlayer.internalState = PLAY;
                    }
                }
                // If THP movie has no audio
                else
                {
                    textureSet = PopDecodedTextureSet(OS_MESSAGE_NOBLOCK);
                    ActivePlayer.internalState = PLAY;
                }
            }
            // If not appropriate timing, wait for next Vsync.
            else
            {
                ActivePlayer.retraceCount = -1;
            }
        }
        // During playback
        else
        {
            if (ProperTimingForGettingNextFrame())
            {
                // If THP movie has audio
                if (ActivePlayer.audioExist)
                {
                    // Calculate difference between current audio playback frames
                    // and current video playback frames
                    diff = ActivePlayer.curVideoNumber - ActivePlayer.curAudioNumber;
                    
                    // If audio is not slower than video, move video ahead
                    if (diff <= 1)
                    {
                        // Get decoded THP video data
                        textureSet = PopDecodedTextureSet(OS_MESSAGE_NOBLOCK);
                        
                        // Decrement decoded THP video data count
                        if (ActivePlayer.videoAhead > diff)
                        {
                            ActivePlayer.videoAhead--;
                        }
                    }
                }
                // If THP movie has no audio
                else
                {
                    // Get decoded video data
                    textureSet = PopDecodedTextureSet(OS_MESSAGE_NOBLOCK);
                }
            }
        }
        
        // If can get decoded THP video data, push THP video data held until
        // that point to cache for data played back.
        if (textureSet && (textureSet != (THPTextureSet *)0xFFFFFFFF))
        {
            ActivePlayer.curVideoNumber = textureSet->frameNumber;
            if (ActivePlayer.dispTextureSet)
            {
                // If you call PushFreeTextureSet here, newly decoded THP video
                // data may be written to the texture buffer the graphics
                // processor is accessing, so the data is pushed to a temporary
                // cache. After verifying with THPPlayerDrawDone() that access from
                // the graphics processor is done, the actual release is done.
                PushUsedTextureSet(ActivePlayer.dispTextureSet);
            }
            ActivePlayer.dispTextureSet = textureSet;
        }
        
        // Check if playback has reached end during one shot playback
        if (!(ActivePlayer.playFlag & LOOP))
        {
            // If THP movie has audio, check if video and audio has reached end.
            if (ActivePlayer.audioExist)
            {
                frameNumber = (s32)((ActivePlayer.curAudioNumber + ActivePlayer.initReadFrame)
                            % ActivePlayer.header.numFrames);
                    
                // If reached end, set state to PLAYED.
                if ((frameNumber == ActivePlayer.header.numFrames - 1)
                     && 
                    (ActivePlayer.playAudioBuffer == NULL))
                {
                    frameNumber = (s32)((ActivePlayer.curVideoNumber + ActivePlayer.initReadFrame)
                                % ActivePlayer.header.numFrames);
                    
                    if ((frameNumber == ActivePlayer.header.numFrames - 1)
                         &&
                        (textureSet == NULL))
                    {
                        ActivePlayer.state = ActivePlayer.internalState = PLAYED;
                    }
                }
            }
            // If THP movie has audio, check if video has reached end.
            else
            {
                frameNumber = (s32)((ActivePlayer.curVideoNumber + ActivePlayer.initReadFrame)
                            % ActivePlayer.header.numFrames);
                         
                if ((frameNumber == ActivePlayer.header.numFrames - 1)
                     &&
                    (textureSet == NULL))
                {       
                    ActivePlayer.state = ActivePlayer.internalState = PLAYED;
                }
            }
        }
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           ProperTimingForStart

    Description:    Check if appropriate timing for movie playback start.
                    Movie rendering loop may be looping with 1 vsync.

    Arguments:      None

    Returns:        If appropriate timing returns TRUE. If inappropriate
                    returns FALSE.
 *---------------------------------------------------------------------------*/
 
static BOOL ProperTimingForStart(void)
{
    if (ActivePlayer.playFlag & ODD_INT)
    {
        if (VIGetNextField() == VI_FIELD_BELOW)
        {
            return TRUE;
        }
    }
    else if (ActivePlayer.playFlag & EVEN_INT)
    {
        if (VIGetNextField() == VI_FIELD_ABOVE)
        {
            return TRUE;
        }
    }
    else 
    {
        return TRUE;
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           ProperTimingForGettingNextFrame

    Description:    Checks if appropriate timing to get decoded THP video data.
    
    Arguments:      None

    Returns:        If appropriate timing returns TRUE. If inappropriate
                    returns FALSE.
 *---------------------------------------------------------------------------*/
 
static BOOL ProperTimingForGettingNextFrame(void)
{
    s32 frameRate;
    
    if (ActivePlayer.playFlag & ODD_INT)
    {
        if (VIGetNextField() == VI_FIELD_BELOW)
        {
            return TRUE;
        }
    }
    else if (ActivePlayer.playFlag & EVEN_INT)
    {
        if (VIGetNextField() == VI_FIELD_ABOVE)
        {
            return TRUE;
        }
    }
    else 
    {
        // Convert framerate to an integer
        frameRate = (s32)((ActivePlayer.header.frameRate) * 100.0f);
        
        if (VIGetTvFormat() == VI_PAL)
        {
            ActivePlayer.curCount = (s32)((ActivePlayer.retraceCount * frameRate) / PAL_RATE);
        }
        else
        {        
            ActivePlayer.curCount = (s32)((ActivePlayer.retraceCount * frameRate) / NTSC_RATE);
        }
                    
        if (ActivePlayer.prevCount != ActivePlayer.curCount)
        {
            ActivePlayer.prevCount = ActivePlayer.curCount;
            return TRUE;
        }
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerDrawCurrentFrame

    Description:    Draw currently acquired decoded THP video data.
                    If none acquired, not displayed.
                    
    Arguments:      rmode     Pointer for currently set GXRenderModeObj
                    x         X coordinate of upper left TV screen for THP 
                              movie display
                    y         Y coordinate of upper left TV screen for THP 
                              movie display
                    polygonW  Width of polygon for THP movie display
                    polygonH  Height of polygon for THP movie display

    Returns:        If could draw return drawn frame number. If could not draw
                    return -1.
 *---------------------------------------------------------------------------*/
 
s32 THPPlayerDrawCurrentFrame(GXRenderModeObj *rmode, u32 x, u32 y, u32 polygonW, u32 polygonH)
{
    s32 currentFrameNumber;
    
    if (ActivePlayer.open && !(ActivePlayer.state == STOP) && ActivePlayer.dispTextureSet)
    {
        THPGXYuv2RgbSetup(rmode);
        THPGXYuv2RgbDraw(ActivePlayer.dispTextureSet->ytexture,
                         ActivePlayer.dispTextureSet->utexture, 
                         ActivePlayer.dispTextureSet->vtexture,
                         (s16)x, (s16)y,
                         (s16)ActivePlayer.videoInfo.xSize, (s16)ActivePlayer.videoInfo.ySize,
                         (s16)polygonW, (s16)polygonH);
        
        THPGXRestore();
                               
        currentFrameNumber = (s32)((ActivePlayer.dispTextureSet->frameNumber + ActivePlayer.initReadFrame)
                           % ActivePlayer.header.numFrames);
        
        return currentFrameNumber;
    }
    
    return -1;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerGetVideoInfo

    Description:    Get THP movie video data.
                    
    Arguments:      videoInfo  Pointer for THPVideoInfo structure.

    Returns:        If succeeds returns TRUE. If fails returns FALSE.
 *---------------------------------------------------------------------------*/
 
BOOL THPPlayerGetVideoInfo(THPVideoInfo *videoInfo)
{
    if (ActivePlayer.open)
    {
        memcpy(videoInfo, &ActivePlayer.videoInfo, sizeof(THPVideoInfo));
        
        return TRUE;
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerGetAudioInfo

    Description:    Get THP movie audio data.
                    
    Arguments:      audioInfo  Pointer for THPAudioInfo structure.

    Returns:        If succeeds returns TRUE. If fails returns FALSE.
 *---------------------------------------------------------------------------*/
 
BOOL THPPlayerGetAudioInfo(THPAudioInfo *audioInfo)
{
    if (ActivePlayer.open)
    {
        memcpy(audioInfo, &ActivePlayer.audioInfo, sizeof(THPAudioInfo));
        
        return TRUE;
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerGetFrameRate

    Description:    Get THP movie frame rate
                    
    Arguments:      None

    Returns:        If succeeds returns THP movie frame rate. If fails, 0.0f.
 *---------------------------------------------------------------------------*/
 
f32 THPPlayerGetFrameRate(void)
{
    if (ActivePlayer.open)
    {
        
        return ActivePlayer.header.frameRate;
    }
    
    return 0.0f;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerGetTotalFrame

    Description:    Get total frames for THP movie
                    
    Arguments:      None

    Returns:        If succeeds returns THP movie total frames. If fails, 0.
 *---------------------------------------------------------------------------*/
 
u32 THPPlayerGetTotalFrame(void)
{
    if (ActivePlayer.open)
    {
        return ActivePlayer.header.numFrames;
    }
    
    return 0;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerGetState

    Description:    Get current player status.
                    
    Arguments:      None

    Returns:        Player status
                    STOP    stopped
                    PREPARE preparations completed
                    PLAY    playing
                    PLAYED  played THP movie to end
                    ERROR   error occured
 *---------------------------------------------------------------------------*/
 
s32 THPPlayerGetState(void)
{
    return ActivePlayer.state;
}

/*---------------------------------------------------------------------------*
    Name:           PushUsedTextureSet

    Description:    Push played back THP video data to played back THP video
                    data queue.
                    
    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void PushUsedTextureSet(THPTextureSet *buffer)
{
    OSSendMessage(&UsedTextureSetQueue, buffer, OS_MESSAGE_NOBLOCK);
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           PopUsedTextureSet

    Description:    Remove played back THP video data from played back THP
                    video data queue.
                    
    Arguments:      None

    Returns:        Pointer for played back THP video data.
 *---------------------------------------------------------------------------*/
 
static THPTextureSet *PopUsedTextureSet(void)
{
    OSMessage msg;
    
    if (OSReceiveMessage(&UsedTextureSetQueue, &msg, OS_MESSAGE_NOBLOCK) == TRUE)
    {
        return (THPTextureSet *)msg;
    }
    else
    {
        return NULL;
    }
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerDrawDone

    Description:    Function used instead of GXDrawDone to synchronize with
                    the graphics processor during movie playback. It waits for
                    the graphics processor to complete processing and frees
                    internally THP video data determined to be played back.
                    
    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void THPPlayerDrawDone(void)
{
    THPTextureSet *textureSet;
    
    GXDrawDone();
    
    if (Initialized)
    {
        while(1)
        {
            textureSet = PopUsedTextureSet();
            if (textureSet == NULL)
            {
                break;
            }
            else
            {
                PushFreeTextureSet(textureSet);
            }
        }
    }
}


/*---------------------------------------------------------------------------*
    Name:           THPAudioMixCallback

    Description:    AI callback function for player. 
                    Call callback functions of AX and MusyX internally when 
                    used with AX and MusyX, and mix output data of AX and 
                    MusyX with THP audio data.  
                    
    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void THPAudioMixCallback(void)
{
    BOOL old;

    if (AudioSystem == ALONE)
    {
        SoundBufferIndex ^= 1;
        
        AIInitDMA((u32)SoundBuffer[SoundBufferIndex], BYTES_PER_AUDIO_FRAME);
        
        old = OSEnableInterrupts();
        
        MixAudio(SoundBuffer[SoundBufferIndex], NULL, SAMPLES_PER_AUDIO_FRAME);
            
        DCFlushRange(SoundBuffer[SoundBufferIndex], BYTES_PER_AUDIO_FRAME);
        
        OSRestoreInterrupts(old);
    }
    else
    {
        if (AudioSystem == WITH_AX)
        {
            if (LastAudioBuffer)
            {
                CurAudioBuffer = LastAudioBuffer;
            }
            
            OldAIDCallback();
             
            LastAudioBuffer = (s16 *)OSPhysicalToCached(AIGetDMAStartAddr());
        }
        else
        {
            OldAIDCallback();
                
            CurAudioBuffer = (s16 *)OSPhysicalToCached(AIGetDMAStartAddr());
        }
        
        SoundBufferIndex ^= 1;
        
        AIInitDMA((u32)SoundBuffer[SoundBufferIndex], BYTES_PER_AUDIO_FRAME);
        
        old = OSEnableInterrupts();
        
        if (CurAudioBuffer)
        {
            DCInvalidateRange(CurAudioBuffer, BYTES_PER_AUDIO_FRAME);
        }
        
        MixAudio(SoundBuffer[SoundBufferIndex], CurAudioBuffer, SAMPLES_PER_AUDIO_FRAME);
        
        DCFlushRange(SoundBuffer[SoundBufferIndex], BYTES_PER_AUDIO_FRAME);
        
        OSRestoreInterrupts(old);
    }
}

/*---------------------------------------------------------------------------*
    Name:           MixAudio

    Description:    Mix THP audio data in the specified buffer.
                    
    Arguments:      destination Buffer in which mixed data is stored. 
                    source      Output buffer from audio library 
                    sample      Sample number of audio data to be mixed. 
                                Specified by stereo sample.

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void MixAudio(s16 *destination, s16 *source, u32 sample)
{
    u32 sampleNum, requestSample, i;
    s32 mix;
    s16 *dst, *libsrc, *thpsrc;
    u16 attenuation;
    
    // When mixed with audio library output 
    if (source)
    {
        if (ActivePlayer.open && (ActivePlayer.internalState == PLAY) && ActivePlayer.audioExist)
        {
            requestSample = sample;
            dst           = destination;
            libsrc        = source;
            
            while (1)
            {
                if (ActivePlayer.playAudioBuffer == NULL)
                {
                    if ((ActivePlayer.playAudioBuffer = PopDecodedAudioBuffer(OS_MESSAGE_NOBLOCK)) == NULL)
                    {
                        memcpy(dst, libsrc, requestSample << 2);
                        break;
                    }
                    ActivePlayer.curAudioNumber = ActivePlayer.playAudioBuffer->frameNumber;
                }
                
                if (ActivePlayer.playAudioBuffer->validSample)
                {
                    if (ActivePlayer.playAudioBuffer->validSample >= requestSample)
                    {
                        sampleNum = requestSample;
                    }
                    else
                    {
                        sampleNum = ActivePlayer.playAudioBuffer->validSample;
                    }
                    
                    thpsrc = ActivePlayer.playAudioBuffer->curPtr;
                    
                    // Mixing
                    for (i = 0 ; i < sampleNum ; i++)
                    {   
                        if (ActivePlayer.rampCount)
                        {
                            ActivePlayer.rampCount--;
                            ActivePlayer.curVolume += ActivePlayer.deltaVolume;
                        }
                        else
                        {
                            ActivePlayer.curVolume = ActivePlayer.targetVolume;
                        }
                        
                        attenuation = VolumeTable[(s32)ActivePlayer.curVolume];
                        
                        // Right
                        mix = (*libsrc) + ((attenuation * (*thpsrc)) >> 15);
                        
                        if (mix < -32768)
                        {
                            mix = -32768;
                        }
                        if (mix > 32767)
                        {
                            mix = 32767;
                        }
                        
                        *dst = (s16)mix;
                        dst++;
                        libsrc++;
                        thpsrc++;
                        
                        // Left
                        mix = (*libsrc) + ((attenuation * (*thpsrc)) >> 15);
                        
                        if (mix < -32768)
                        {
                            mix = -32768;
                        }
                        if (mix > 32767)
                        {
                            mix = 32767;
                        }
                        
                        *dst = (s16)mix;
                        dst++;
                        libsrc++;
                        thpsrc++;
                    }
                    
                    requestSample -= sampleNum;
                    
                    ActivePlayer.playAudioBuffer->validSample -= sampleNum;
                    ActivePlayer.playAudioBuffer->curPtr       = thpsrc;
                    
                    if (ActivePlayer.playAudioBuffer->validSample == 0)
                    {
                        // Release used THP audio data
                        PushFreeAudioBuffer(ActivePlayer.playAudioBuffer);
                        ActivePlayer.playAudioBuffer = NULL;
                    }
                    
                    if (!requestSample)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            memcpy(destination, source, sample << 2);
        }
    }
    // When not mixing with audio library output 
    else
    {
        if (ActivePlayer.open && (ActivePlayer.internalState == PLAY) && ActivePlayer.audioExist)
        {
            requestSample = sample;
            dst           = destination;
            
            while (1)
            {
                if (ActivePlayer.playAudioBuffer == NULL)
                {
                    if ((ActivePlayer.playAudioBuffer = PopDecodedAudioBuffer(OS_MESSAGE_NOBLOCK)) == NULL)
                    {
                        memset(dst, 0, requestSample << 2);
                        break;
                    }
                    ActivePlayer.curAudioNumber = ActivePlayer.playAudioBuffer->frameNumber;
                }
                
                if (ActivePlayer.playAudioBuffer->validSample)
                {
                    if (ActivePlayer.playAudioBuffer->validSample >= requestSample)
                    {
                        sampleNum = requestSample;
                    }
                    else
                    {
                        sampleNum = ActivePlayer.playAudioBuffer->validSample;
                    }
                    
                    thpsrc = ActivePlayer.playAudioBuffer->curPtr;
                    
                    // Mixing
                    for (i = 0 ; i < sampleNum ; i++)
                    {   
                        if (ActivePlayer.rampCount)
                        {
                            ActivePlayer.rampCount--;
                            ActivePlayer.curVolume += ActivePlayer.deltaVolume;
                        }
                        else
                        {
                            ActivePlayer.curVolume = ActivePlayer.targetVolume;
                        }
                        
                        attenuation = VolumeTable[(s32)ActivePlayer.curVolume];
                        
                        // Right
                        mix = (attenuation * (*thpsrc)) >> 15;
                        
                        if (mix < -32768)
                        {
                            mix = -32768;
                        }
                        if (mix > 32767)
                        {
                            mix = 32767;
                        }
                        
                        *dst = (s16)mix;
                        dst++;
                        thpsrc++;
                        
                        // Left
                        mix = (attenuation * (*thpsrc)) >> 15;
                        
                        if (mix < -32768)
                        {
                            mix = -32768;
                        }
                        if (mix > 32767)
                        {
                            mix = 32767;
                        }
                        
                        *dst = (s16)mix;
                        dst++;
                        thpsrc++;
                    }
                    
                    requestSample -= sampleNum;
                    
                    ActivePlayer.playAudioBuffer->validSample -= sampleNum;
                    ActivePlayer.playAudioBuffer->curPtr       = thpsrc;
                    
                    if (ActivePlayer.playAudioBuffer->validSample == 0)
                    {
                        // Free used THP audio data 
                        PushFreeAudioBuffer(ActivePlayer.playAudioBuffer);
                        ActivePlayer.playAudioBuffer = NULL;
                    }
                    
                    if (!requestSample)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            memset(destination, 0, sample << 2);
        }
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerSetVolume

    Description:    Set volume of player. Volume will be changed in the 
                    specified time. 
                    
    Arguments:      vol  volume to be set (0 - 127)
                    time Specify the time required to go from current volume to
                         specified volume in units of milliseconds (0 - 60000)

    Returns:        Returns TRUE if succeeds, and FALSE if fails.
 *---------------------------------------------------------------------------*/
 
BOOL THPPlayerSetVolume(s32 vol, s32 time)
{
    BOOL old;
    
    if (ActivePlayer.open && ActivePlayer.audioExist)
    {
        if (vol > 127)
        {
            vol = 127;
        }
        
        if (vol < 0)
        {
            vol = 0;
        }
        
        if (time > 60000)
        {
            time = 60000;
        }
        
        if (time < 0)
        {
            time = 0;
        }
        
        old = OSDisableInterrupts();
        
        ActivePlayer.targetVolume = (f32)vol;

        if (time)
        {
            ActivePlayer.rampCount   = 32 * time;
            ActivePlayer.deltaVolume = (ActivePlayer.targetVolume - ActivePlayer.curVolume)
                                     / (f32)ActivePlayer.rampCount;
        }
        else
        {
            ActivePlayer.rampCount = 0;
            ActivePlayer.curVolume = ActivePlayer.targetVolume;
        }
        
        OSRestoreInterrupts(old);
        
        return TRUE;
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
    Name:           THPPlayerGetVolume

    Description:    Get current volume of player 
                    
    Arguments:      None

    Returns:        Returns Current playback volume if succeeds, and -1 if fails. 
 *---------------------------------------------------------------------------*/
 
s32 THPPlayerGetVolume(void)
{
    if (ActivePlayer.open)
    {
        return (s32)ActivePlayer.curVolume;
    }
    
    return -1;
}
