/*---------------------------------------------------------------------------*
  Project:  Dolphin
  File:     THPVideoDecode.c

  Copyright 2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/GameLib/fmv/gcn/THPVideoDecode.c $
 * 
 * 1     4/03/02 10:14a Todd Blackburn
    
    1     02/01/16 10:53a Akagi
    Initial revision made by Suzuki-san (IRD).
  $NoKeywords: $  

 *---------------------------------------------------------------------------*/

#include <dolphin.h>
#include "THPPlayer.h"
#include "THPVideoDecode.h"
#include "THPRead.h"

/*---------------------------------------------------------------------------*
   External Function
 *---------------------------------------------------------------------------*/
 
extern void PrepareReady(BOOL flag);

/*---------------------------------------------------------------------------*
   Static Function
 *---------------------------------------------------------------------------*/
 
static void *VideoDecoder(void *ptr);
static void *VideoDecoderForOnMemory(void *ptr);
static void VideoDecode(THPReadBuffer *readBuffer);

/*---------------------------------------------------------------------------*
   Global Variable
 *---------------------------------------------------------------------------*/
 
extern THPPlayer ActivePlayer;

/*---------------------------------------------------------------------------*
   Static Variable
 *---------------------------------------------------------------------------*/

static s32            VideoDecodeThreadCreated = 0;
static OSThread       VideoDecodeThread;
static u8             VideoDecodeThreadStack[4*1024];
static OSMessageQueue FreeTextureSetQueue;
static OSMessageQueue DecodedTextureSetQueue;
static OSMessage      FreeTextureSetMessage[DECODE_BUFFER_NUM];
static OSMessage      DecodedTextureSetMessage[DECODE_BUFFER_NUM];
static s32            First;

/*---------------------------------------------------------------------------*
    Name:           CreateVideoDecodeThread

    Description:    Create video decode thread

    Arguments:      priority  thread priority
                    ptr       Pointer to memory where THP movie start data is
                              for OnMemory playback.

    Returns:        If thread creation succeeds returns TRUE. If fails, FALSE.
 *---------------------------------------------------------------------------*/
 
BOOL CreateVideoDecodeThread(OSPriority priority, u8 *ptr)
{
    if (ptr)
    {
        if (OSCreateThread(&VideoDecodeThread,
                           VideoDecoderForOnMemory,
                           ptr,
                           VideoDecodeThreadStack + sizeof(VideoDecodeThreadStack),
                           sizeof(VideoDecodeThreadStack),
                           priority,
                           OS_THREAD_ATTR_DETACH) == FALSE)
        {
#ifdef _DEBUG
            OSReport("Can't create video decode thread\n");
#endif
            return FALSE;
        }
    }
    else
    {
        if (OSCreateThread(&VideoDecodeThread,
                           VideoDecoder,
                           NULL,
                           VideoDecodeThreadStack + sizeof(VideoDecodeThreadStack),
                           sizeof(VideoDecodeThreadStack),
                           priority,
                           OS_THREAD_ATTR_DETACH) == FALSE)
        {
#ifdef _DEBUG
            OSReport("Can't create video decode thread\n");
#endif
            return FALSE;
        }
    }
    
    OSInitMessageQueue(&FreeTextureSetQueue,
                       FreeTextureSetMessage,
                       DECODE_BUFFER_NUM);
                       
    OSInitMessageQueue(&DecodedTextureSetQueue,
                       DecodedTextureSetMessage,
                       DECODE_BUFFER_NUM);
                       
    VideoDecodeThreadCreated = 1;
    
    First = 1;
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
    Name:           VideoDecodeThreadStart

    Description:    Start of video decode thread

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void VideoDecodeThreadStart(void)
{
    if (VideoDecodeThreadCreated)
    {
        OSResumeThread(&VideoDecodeThread);
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           VideoDecodeThreadCancel

    Description:    Cancel video decode thread

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void VideoDecodeThreadCancel(void)
{
    if (VideoDecodeThreadCreated)
    {
        OSCancelThread(&VideoDecodeThread);
    
        VideoDecodeThreadCreated = 0;
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           VideoDecoder

    Description:    Video decoder streaming playback

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void *VideoDecoder(void *ptr)
{
    #pragma unused(ptr)
    THPReadBuffer *readBuffer;
    BOOL          old;
    s32           frameNumber;
    
    while (1)
    {
        if (ActivePlayer.audioExist)
        {
            // Skip decoding if video decoding is delayed
            while (ActivePlayer.videoAhead < 0)
            {
                readBuffer = PopReadedBuffer2();
                
                frameNumber = (s32)((readBuffer->frameNumber + ActivePlayer.initReadFrame)
                            % ActivePlayer.header.numFrames);
                
                // Always decode last frame with one shot playback
                if (frameNumber == ActivePlayer.header.numFrames - 1)
                {
                    if (!(ActivePlayer.playFlag & LOOP))
                    {
                        VideoDecode(readBuffer);
                    }
                }
                
                PushFreeReadBuffer(readBuffer);
                        
                old = OSDisableInterrupts();
                ActivePlayer.videoAhead++;
                OSRestoreInterrupts(old);
            }
        }
             
        if (ActivePlayer.audioExist)
        {   
            readBuffer = PopReadedBuffer2();
        }
        else
        {
            readBuffer = PopReadedBuffer();
        }
            
        VideoDecode(readBuffer);
            
        PushFreeReadBuffer(readBuffer);
    }
    
    return NULL;
}       

/*---------------------------------------------------------------------------*
    Name:           VideoDecoderForOnMemory

    Description:    Video decoder for OnMemory playback

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void *VideoDecoderForOnMemory(void *ptr)
{
    THPReadBuffer readBuffer;
    BOOL          old;
    s32           tmp, size, readFrame, frameNumber;
    
    size           = ActivePlayer.initReadSize;
    readBuffer.ptr = (u8 *)ptr;
    readFrame      = 0;
    
    while(1)
    {
        if (ActivePlayer.audioExist)
        {
            // Skip decoding if video decoding is delayed
            while (ActivePlayer.videoAhead < 0)
            {
                old = OSDisableInterrupts();
                ActivePlayer.videoAhead++;
                OSRestoreInterrupts(old);
                
                frameNumber = (s32)((readFrame + ActivePlayer.initReadFrame)
                            % ActivePlayer.header.numFrames);
                    
                // Check THP movie end
                if (frameNumber == ActivePlayer.header.numFrames - 1)
                {
                    // If loop playback, at beginning of THP movie
                    if (ActivePlayer.playFlag & LOOP)
                    {
                        size           = *(s32 *)(readBuffer.ptr);
                        readBuffer.ptr = ActivePlayer.movieData;
                    }
                    // Always decode last frame with one shot playback
                    else
                    {
                        break;
                    }
                }
                // If not last, move pointer to next frame
                else
                {
                    tmp             = *(s32 *)(readBuffer.ptr);
                    readBuffer.ptr += size;
                    size            = tmp;
                }
                
                readFrame++;
            }
        }
            
        readBuffer.frameNumber = readFrame;
            
        VideoDecode(&readBuffer);
        
        frameNumber = (s32)((readFrame + ActivePlayer.initReadFrame)
                            % ActivePlayer.header.numFrames);
                    
        // Check THP movie end
        if (frameNumber == ActivePlayer.header.numFrames - 1)
        {
            // If loop playback, at beginning of THP movie
            if (ActivePlayer.playFlag & LOOP)
            {
                size           = *(s32 *)(readBuffer.ptr);
                readBuffer.ptr = ActivePlayer.movieData;
            }
            // If one shot playback, stop decoding
            else
            {
                OSSuspendThread(&VideoDecodeThread);
            }
        }
        // If not last, move pointer to next frame
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
    Name:           VideoDecode

    Description:    Decode THP video data

    Arguments:      readBuffer  Pointer to buffer where THP frame stored.

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void VideoDecode(THPReadBuffer *readBuffer)
{
    THPTextureSet *textureSet;
    u32           i;
    u32           *compSizePtr;
    u8            *ptr;
    BOOL          old;
    
    compSizePtr = (u32 *)(readBuffer->ptr + 8);
    ptr         = readBuffer->ptr + ActivePlayer.compInfo.numComponents * 4 + 8;
    
    textureSet  = PopFreeTextureSet();
    
    for (i = 0 ; i < ActivePlayer.compInfo.numComponents ; i++)
    {
        switch (ActivePlayer.compInfo.frameComp[i])
        {
            case VIDEO_COMP:
                if ((ActivePlayer.videoError = THPVideoDecode(ptr,
                                                              textureSet->ytexture,
                                                              textureSet->utexture,
                                                              textureSet->vtexture,
                                                              ActivePlayer.thpWork)) != THP_OK)
                {
                    if (First)
                    {
                        PrepareReady(FALSE);
                        First = 0;
                    }
                    
                    OSSuspendThread(&VideoDecodeThread);
                }
                
                textureSet->frameNumber = readBuffer->frameNumber;
                PushDecodedTextureSet(textureSet);
                
                // Increment decoded THP video data count
                old = OSDisableInterrupts();
                ActivePlayer.videoAhead++;
                OSRestoreInterrupts(old);
                break;
        }
        ptr += *compSizePtr;
        compSizePtr++;
    }
    
    if (First)
    {
        PrepareReady(TRUE);
        First = 0;
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           PopFreeTextureSet

    Description:    Get free texture buffers

    Arguments:      None

    Returns:        Pointer for texture buffers not used.
 *---------------------------------------------------------------------------*/

THPTextureSet *PopFreeTextureSet()
{
    OSMessage msg;
    
    OSReceiveMessage(&FreeTextureSetQueue, &msg, OS_MESSAGE_BLOCK);
    
    return (THPTextureSet *)msg;
}

/*---------------------------------------------------------------------------*
    Name:           PushFreeTextureSet

    Description:    Free played back THP video data.

    Arguments:      buffer  Pointer for played back THP video data.

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void PushFreeTextureSet(THPTextureSet *buffer)
{
    OSSendMessage(&FreeTextureSetQueue, buffer, OS_MESSAGE_NOBLOCK);
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           PopDecodedTextureSet

    Description:    Get decoded THP video data

    Arguments:      flag  If OS_MESSAGE_BLOCK, block until can get decoded
                          THP video data.
                          If OS_MESSAGE_NOBLOCK, do not block.

    Returns:        If can get decoded THP video data return pointer to
                    texture buffer. If cannot get return NULL.
 *---------------------------------------------------------------------------*/
 
THPTextureSet *PopDecodedTextureSet(s32 flag)
{
    OSMessage msg;
    
    if (OSReceiveMessage(&DecodedTextureSetQueue, &msg, flag) == TRUE)
    {
        return (THPTextureSet *)msg;
    }
    else
    {
        return NULL;
    }
}

/*---------------------------------------------------------------------------*
    Name:           PushDecodedTextureSet

    Description:    Push decoded THP video data to queue.

    Arguments:      buffer  Pointer for decoded THP video data.

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void PushDecodedTextureSet(THPTextureSet *buffer)
{
    OSSendMessage(&DecodedTextureSetQueue, buffer, OS_MESSAGE_BLOCK);
    
    return;
}