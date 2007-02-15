/*---------------------------------------------------------------------------*
  Project:  Dolphin
  File:     THPRead.c

  Copyright 2002 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/GameLib/fmv/gcn/THPRead.c $
 * 
 * 1     4/03/02 10:14a Todd Blackburn
    
    1     02/01/16 10:53a Akagi
    Initial revision made by Suzuki-san (IRD).
  $NoKeywords: $  

 *---------------------------------------------------------------------------*/

#include <dolphin.h>
#include <string.h>
#include "THPPlayer.h"
#include "THPRead.h"
#include <stdlib.h>

/*---------------------------------------------------------------------------*
   External function
 *---------------------------------------------------------------------------*/
 
extern void PrepareReady(BOOL flag);

/*---------------------------------------------------------------------------*
   Static function
 *---------------------------------------------------------------------------*/
 
static void *Reader(void *ptr);

/*---------------------------------------------------------------------------*
   Global variable
 *---------------------------------------------------------------------------*/
 
extern THPPlayer ActivePlayer;

/*---------------------------------------------------------------------------*
   Static variable
 *---------------------------------------------------------------------------*/
 
static s32            ReadThreadCreated = 0;
static OSMessageQueue FreeReadBufferQueue;
static OSMessageQueue ReadedBufferQueue;
static OSMessageQueue ReadedBufferQueue2;
static OSMessage      FreeReadBufferMessage[READ_BUFFER_NUM];
static OSMessage      ReadedBufferMessage[READ_BUFFER_NUM];
static OSMessage      ReadedBufferMessage2[READ_BUFFER_NUM];
static OSThread       ReadThread;
static u8             ReadThreadStack[4*1024];

/*---------------------------------------------------------------------------*
    Name:           CreateReadThread

    Description:    Create read thread

    Arguments:      priority  Thread priority

    Returns:        If succeed in creating thread return TRUE.  If fail, FALSE.
 *---------------------------------------------------------------------------*/
 
BOOL CreateReadThread(OSPriority priority)
{
    if (OSCreateThread(&ReadThread,
                       Reader,
                       NULL,
                       ReadThreadStack + sizeof(ReadThreadStack),
                       sizeof(ReadThreadStack),
                       priority,
                       OS_THREAD_ATTR_DETACH) == FALSE)
    {
#ifdef _DEBUG
        OSReport("Can't create read thread\n");
#endif
        return FALSE;
    }
    
    OSInitMessageQueue(&FreeReadBufferQueue,
                       FreeReadBufferMessage,
                       READ_BUFFER_NUM);
    
    OSInitMessageQueue(&ReadedBufferQueue,
                       ReadedBufferMessage,
                       READ_BUFFER_NUM);
    
    OSInitMessageQueue(&ReadedBufferQueue2,
                       ReadedBufferMessage2,
                       READ_BUFFER_NUM);
    
    ReadThreadCreated = 1;
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
    Name:           ReadThreadStart

    Description:    Start of read thread.

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void ReadThreadStart(void)
{
    if (ReadThreadCreated)
    {
        OSResumeThread(&ReadThread);
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           ReadThreadCancel

    Description:    Cancel of read thread.

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void ReadThreadCancel(void)
{
    if (ReadThreadCreated)
    {
        OSCancelThread(&ReadThread);
    
        ReadThreadCreated = 0;
    }
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           Reader

    Description:    Read process for streaming playback

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
 
static void *Reader(void *ptr)
{
#pragma unused(ptr)
    THPReadBuffer *readBuffer;
    s32           offset;
    s32           size;
    s32           readFrame;
    s32           result;
    s32           frameNumber;
    
    readFrame = 0;
    offset    = ActivePlayer.initOffset;
    size      = ActivePlayer.initReadSize;
    
    while(1)
    {      
        readBuffer = PopFreeReadBuffer();
        
        result = DVDRead(&ActivePlayer.fileInfo, readBuffer->ptr, size, offset);
        
        // FATAL error during cancel
        if (result != size)
        {
            if (result == DVD_RESULT_FATAL_ERROR)
            {
                ActivePlayer.dvdError = DVD_RESULT_FATAL_ERROR;
            }
            
            if (readFrame == 0)
            {
                PrepareReady(FALSE);
            }
            
            OSSuspendThread(&ReadThread);
        }
        
        readBuffer->frameNumber = readFrame;
        
        PushReadedBuffer(readBuffer);
                        
        offset += size;
        size    = NEXT_READ_SIZE(readBuffer);
        
        // Calculate current frame # from quantity read
        frameNumber = (s32)((readFrame + ActivePlayer.initReadFrame)
                    % ActivePlayer.header.numFrames);
            
        // Check THP movie end
        if (frameNumber == ActivePlayer.header.numFrames - 1)
        {
            // If loop playback, at beginning of movie data
            if (ActivePlayer.playFlag & LOOP)
            {
                offset = (s32)(ActivePlayer.header.movieDataOffsets);
            }
            // Stop decoding if one shot playback
            else
            {
                OSSuspendThread(&ReadThread);
            }
        }
        
        readFrame++;    
    }
    
    return NULL;
}

/*---------------------------------------------------------------------------*
    Name:           PopReadedBuffer

    Description:    Get buffer where THP frame stored.

    Arguments:      None

    Returns:        Pointer for buffer where THP frame stored.
 *---------------------------------------------------------------------------*/
 
THPReadBuffer *PopReadedBuffer()
{
    OSMessage msg;
    
    OSReceiveMessage(&ReadedBufferQueue, &msg, OS_MESSAGE_BLOCK);
    
    return (THPReadBuffer *)msg;
}

/*---------------------------------------------------------------------------*
    Name:           PushReadedBuffer

    Description:    Pushes buffer where THP frame stored to queue

    Arguments:      buffer   Pointer for buffer where read completed

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void PushReadedBuffer(THPReadBuffer *buffer)
{
    OSSendMessage(&ReadedBufferQueue, buffer, OS_MESSAGE_BLOCK);
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           PopFreeReadBuffer

    Description:    Get free read buffers

    Arguments:      None

    Returns:        Pointer for read buffer
 *---------------------------------------------------------------------------*/
 
THPReadBuffer *PopFreeReadBuffer()
{
    OSMessage msg;
    
    OSReceiveMessage(&FreeReadBufferQueue, &msg, OS_MESSAGE_BLOCK);
    
    return (THPReadBuffer *)msg;
}

/*---------------------------------------------------------------------------*
    Name:           PushFreeReadBuffer

    Description:    Free used read buffers

    Arguments:      buffer  Pointer for free read buffers

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void PushFreeReadBuffer(THPReadBuffer *buffer)
{
    OSSendMessage(&FreeReadBufferQueue, buffer, OS_MESSAGE_BLOCK);
    
    return;
}

/*---------------------------------------------------------------------------*
    Name:           PopReadedBuffer2

    Description:    Get read buffer passed from audio decode thread
                    Called from video decode thread.

    Arguments:      None

    Returns:        Pointer for read buffer
 *---------------------------------------------------------------------------*/
 
THPReadBuffer *PopReadedBuffer2()
{
    OSMessage msg;
    
    OSReceiveMessage(&ReadedBufferQueue2, &msg, OS_MESSAGE_BLOCK);
    
    return (THPReadBuffer *)msg;
}

/*---------------------------------------------------------------------------*
    Name:           PushReadedBuffer2

    Description:    Passes read buffer to video decode thread.

    Arguments:      Pointer for read buffer.

    Returns:        None
 *---------------------------------------------------------------------------*/
 
void PushReadedBuffer2(THPReadBuffer *buffer)
{
    OSSendMessage(&ReadedBufferQueue2, buffer, OS_MESSAGE_BLOCK);
    
    return;
}
