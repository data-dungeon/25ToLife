/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                    - easy mpeg streaming -
 *
 *                         Version 0.20
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            strfile.c
 *                   functions for file streaming
 *
 *       Version        Date            Design   Log
 *  --------------------------------------------------------------------
 *       0.10           02.29.2000      umemura  the first version
 *       0.20           08.21.2001      mikedean non-blocking
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <eekernel.h>
#include <libdev.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <sif.h>

#include "strfile.h"
#include "Audio/PS2/sound.h"

static StrFile infile;

// ////////////////////////////////////////////////////////////////
//
//  Open a file to read and check its size
//
//	< file name conversion >
//	   = from HD =
//	    dir/file.pss           -> host0:dir/file.pss
//	    host0:dir/file.pss     -> host0:dir/file.pss
//
//	   = from CD/DVD =
//	    cdrom0:\dir\file.pss;1 -> cdrom0:\DIR\FILE.PSS;1
//	    cdrom0:/dir/file.pss;1 -> cdrom0:\DIR\FILE.PSS;1
//	    cdrom0:/dir/file.pss   -> cdrom0:\DIR\FILE.PSS;1
//
int strFileOpen(char *filename)
{
	// Init everything
	infile.streamID = -1;
	infile.size = 0;
	infile.streamStarted = false;
	infile.loaded = 0;
	infile.nextTransfer = false;

	// Override the fastload mode!
	MultiStream::OverrideFastLoadMode(SOUND_FASTLOAD_STOP);

	// Open the file
	infile.fileID = MultiStream::OpenFileID(filename, &infile.size);

	// Did it work?
	return (infile.fileID < 0 ? 0 : 1);
}

// ////////////////////////////////////////////////////////////////
//
//  Close a file
//
int strFileClose(void)
{
	if (infile.streamID >= 0)
	{
		MultiStream::StopStream(infile.streamID);
		MultiStream::CloseStreamBuffer(infile.streamID);
		MultiStream::FreeStreamID(infile.streamID);
	}

	if (infile.fileID >= 0)
		MultiStream::CloseFileID(infile.fileID);
	MultiStream::SyncIOP();

	MultiStream::RestoreFastLoadMode();

	return 1;
}

// ////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////
int strFirstRead(uint eeAddress, int size)
{
	ASSERT(!infile.streamStarted);

	// Setup the stream
	infile.streamID = MultiStream::AllocateStreamID();
	ASSERT(infile.streamID >= 0);
	MultiStream::OpenStreamBuffer(infile.streamID, SOUND_DATA_STREAM, size);
	MultiStream::StreamLoadEE(infile.streamID, infile.fileID, eeAddress);

	infile.loaded = 0;
	SOUND_FASTLOAD_INFO fastLoadInfo;
	while (infile.loaded != size)
	{
		// Wait for readyness
		MultiStream::BeginReadyCheck();
		while (!MultiStream::IsReady());

		MultiStream::GetFastLoadInfo(fastLoadInfo);

		// Make sure we get data for the start of the stream,
		// rather than any other other data.
		if (fastLoadInfo.Counter == 1)	// First transfer from IOP->EE?
			infile.loaded = fastLoadInfo.TotalSize;	// Yes.
	}

	// okay we are rolling
	infile.streamStarted = true;
	MultiStream::BeginReadyCheck();
	infile.nextTransfer = true;
	infile.loaded = fastLoadInfo.TotalSize;
	return infile.loaded;
}

// ////////////////////////////////////////////////////////////////
//
//  Read data
//
int strFileRead(void *buff, int size)
{
	int loadsize = 0;
	SOUND_FASTLOAD_INFO fastLoadInfo;
	u_int eeAddress=(u_int)(u_int *)buff;

	// Have we started the stream yet?
	if (!infile.streamStarted)
		return strFirstRead(eeAddress, size);

	// Get the info
	MultiStream::GetFastLoadInfo(fastLoadInfo);
	if (fastLoadInfo.TotalSize != infile.loaded)
	{
		loadsize = fastLoadInfo.LoadSize;
		infile.loaded = fastLoadInfo.TotalSize;
		infile.nextTransfer = true;
		MultiStream::BeginReadyCheck();
	}
	else
	{
		// We are done with the block wait for ready then update the ee address
		if (MultiStream::IsReady())
		{
			if (infile.nextTransfer)
			{
				MultiStream::StreamSetEEAddress(infile.streamID, eeAddress);
				MultiStream::SetFastLoadNext(infile.streamID);
				infile.nextTransfer = false;
				MultiStream::SyncIOP();
			}
		}
	}

	return(loadsize);
}

