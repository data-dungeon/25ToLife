/*---------------------------------------------------------------------------*
  Project:  Dolphin
  File:     THPAudioDecode.c

  Copyright 2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/GameLib/fmv/gcn/THPAudioDecode.c $
 * 
 * 1     4/03/02 10:14a Todd Blackburn
    
    1     02/01/16 10:52a Akagi
    Initial revision made by Suzuki-san (IRD).
  $NoKeywords: $  

 *---------------------------------------------------------------------------*/

#include <dolphin.h>
#include "THPPlayer.h"
#include "THPAudioDecode.h"
#include "THPRead.h"

/*---------------------------------------------------------------------------*
   Static Function
 *---------------------------------------------------------------------------*/
 
static void *AudioDecoder(void *ptr);
static void *AudioDecoderForOnMemory(void *ptr);
static void AudioDecode(THPReadBuffer *readBuffer);

/*---------------------------------------------------------------------------*
   Global Variable
 *---------------------------------------------------------------------------*/
 
extern THPPlayer ActivePlayer;

/*---------------------------------------------------------------------------*
   Static Variable
 *---------------------------------------------------------------------------*/
 
static s32            AudioDecodeThreadCreated = 0;
static OSThread       AudioDecodeThread;
static u8             AudioDecodeThreadStack[4*1024];
static OSMessageQueue FreeAudioBufferQueue;
static OSMessageQueue DecodedAudioBufferQueue;
static OSMessage      FreeAudioBufferMessage[DECODE_BUFFER_NUM];
static OSMessage      DecodedAudioBufferMessage[DECODE_BUFFER_NUM];

/*---------------------------------------------------------------------------*
    Name:           CreateAudioDecodeThread

    Description:    Creation of audio decode thread

    Arguments:      priority   thread priority
                    ptr        Pointer to memory where the start data of movie 
                               is with OnMemory playback.

    Return Values:  If creation of thread succeeds, TRUE. If creation fails, 
                    FALSE.
 *---------------------------------------------------------------------------*/
 
BOOL CreateAudioDecodeThread(OSPriority priority, u8 *ptr)
{
    if (ptr)
    {
        if (OSCreateThread(&AudioDecodeThread,
                           AudioDecoderForOnMemory,
                           ptr,
                           AudioDecodeThreadStack + sizeof(AudioDecodeThreadStack),
                           sizeof(AudioDecodeThreadStack),
                           priority,
                           OS_THREAD_ATTR_DETACH) == FALSE)
        {
#ifdef _DEBUG
            OSReport("Can't create audio decode thread\n");
#endif
            return FALSE;
        }
    }
    else
    {
        if (OSCreateThread(&AudioDecodeThread,
                           AudioDecoder,
                           NULL,
                           AudioDecodeThreadStack + sizeof(AudioDecodeThreadStack),
                           sizeof(AudioDecodeThreadStack),
                           priority,
                           OS_THREAD_ATTR_DETACH) == FALSE)
        {
#ifdef _DEBUG
            OSReport("Can't create audio decode thread\n");
#endif
            return FALSE;
        }
    }
    
    OSInitMessageQueue(&FreeAudioBufferQueue,
                       FreeAudioBufferMessage,
                       DECODE_BUFFER_NUM);
                       
    OSInitMessageQueue(&DecodedAudioBufferQueue,
                       DecodedAudioBufferMessage,
                       DECODE_BUFFER_NUM);
    
    AudioDecodeThreadCreated = 1;
                 
    return TRUE;
}

/*---------------------------------------------------------------------------*
    Name:           AudioDecodeThreadStart

    Description:    Start of audio decode thread.

    Arguments:      None

    Return Values:  None
 *---------------------------------------------------------------------------*/
 
void AudioDecodeThreadStart(void)
{
    if (AudioDecodeThreadCreated)
    {
        OSResumeThread(&AudioDecodeThread);
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           AudioDecodeThreadCancel

    Description:    Cancel audio decode thread

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void AudioDecodeThreadCancel(void)
{
    if (AudioDecodeThreadCreated)
    {
        OSCancelThread(&AudioDecodeThread);
    
        AudioDecodeThreadCreated = 0;
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           AudioDecoder

    Description:    Audio decoder for streaming playback

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void *AudioDecoder(void *ptr)
{
    #pragma unused(ptr)
    THPReadBuffer *readBuffer;
    
    while(1)
    {
        readBuffer = PopReadedBuffer();
            
        AudioDecode(readBuffer);
        
        PushReadedBuffer2(readBuffer);
    }
    
    return NULL;
}

/*---------------------------------------------------------------------------*
    Name:           AudioDecoderForOnMemory

    Description:    Audio decoder for OnMemory playback.

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void *AudioDecoderForOnMemory(void *ptr)
{
    THPReadBuffer readBuffer;
    s32           tmp, size, readFrame, frameNumber;
    
    size           = ActivePlayer.initReadSize;
    readBuffer.ptr = (u8 *)ptr;
    readFrame      = 0;
    
    while(1)
    {
        readBuffer.frameNumber = readFrame;
            
        AudioDecode(&readBuffer);
            
        frameNumber = (s32)((readFrame + ActivePlayer.initReadFrame)
                    % ActivePlayer.header.numFrames);
                        
        // Check end of THP movie data
        if (frameNumber == ActivePlayer.header.numFrames - 1)
        {
            // If loop playback, at beginning of movie data
            if (ActivePlayer.playFlag & LOOP)
            {
                size           = *(s32 *)(readBuffer.ptr);
                readBuffer.ptr = ActivePlayer.movieData;
            }
            // Stop decode if one shot playback
            else
            {
                OSSuspendThread(&AudioDecodeThread);
            }
        }
        // Move next frame pointer if not end
        else
        {
            tmp             = *(s32 *)(readBuffer.ptr);
            readBuffer.ptr += size;
            size            = tmp;
        }
        
        readFrame++;
    }
    
    return NULL;
}

/*---------------------------------------------------------------------------*
    Name:           AudioDecode

    Description:    Decoding of THP audio data.

    Arguments:      readBuffer Pointer to buffer where THP frame stored.

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void AudioDecode(THPReadBuffer *readBuffer)
{
    THPAudioBuffer *audioBuffer;
    u32            i, sample;
    u32            *compSizePtr;
    u8             *ptr;

    compSizePtr = (u32 *)(readBuffer->ptr + 8);
    ptr         = readBuffer->ptr + ActivePlayer.compInfo.numComponents * 4 + 8;
    
    audioBuffer  = PopFreeAudioBuffer();
    
    for (i = 0 ; i < ActivePlayer.compInfo.numComponents ; i++)
    {
        switch (ActivePlayer.compInfo.frameComp[i])
        {
            case AUDIO_COMP:
                sample = THPAudioDecode(audioBuffer->buffer,
                                        ptr,
                                        THP_AUDIO_INTERLEAVE);
                audioBuffer->validSample = sample;
                audioBuffer->curPtr      = audioBuffer->buffer;
                audioBuffer->frameNumber = readBuffer->frameNumber;
                PushDecodedAudioBuffer(audioBuffer);
                break;
        }
        ptr += *compSizePtr;
        compSizePtr++;
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           PopFreeAudioBuffer

    Description:    Get audio buffers not used.

    Arguments:      None

    Returns:        Pointer for audio buffers not used.
 *---------------------------------------------------------------------------*/
 
THPAudioBuffer *PopFreeAudioBuffer()
{
    OSMessage msg;
    
    OSReceiveMessage(&FreeAudioBufferQueue, &msg, OS_MESSAGE_BLOCK);
    
    return (THPAudioBuffer *)msg;
}

/*---------------------------------------------------------------------------*
    Name:           PushFreeAudioBuffer

    Description:    Free THP audio data finished playing back.

    Arguments:      buffer  Pointer for THP audio data finished playing back.

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void PushFreeAudioBuffer(THPAudioBuffer *buffer)
{
    OSSendMessage(&FreeAudioBufferQueue, buffer, OS_MESSAGE_NOBLOCK);
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           PopDecodedAudioBuffer

    Description:    Get decoded THP audio data.

    Arguments:      Block until can get decoded THP audio data
                    if flag OS_MESSAGE_BLOCK.  Do not block if
                    OS_MESSAGE_NOBLOCK.

    Returns:        If get decoded THP audio data, return audio buffer pointer.
                    If could not get, NULL is returned.
 *---------------------------------------------------------------------------*/
 
THPAudioBuffer *PopDecodedAudioBuffer(s32 flag)
{
    OSMessage msg;
    
    if (OSReceiveMessage(&DecodedAudioBufferQueue, &msg, flag) == TRUE)
    {
        return (THPAudioBuffer *)msg;
    }
    else
    {
        return NULL;
    }
}

/*---------------------------------------------------------------------------*
    Name:           PushDecodedAudioBuffer

    Description:    Push THP audio that has finished decoding into queue

    Arguments:      buffer  Pointer for decoded THP audio data.

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void PushDecodedAudioBuffer(THPAudioBuffer *buffer)
{
    OSSendMessage(&DecodedAudioBufferQueue, buffer, OS_MESSAGE_BLOCK);
    
    return;
}



