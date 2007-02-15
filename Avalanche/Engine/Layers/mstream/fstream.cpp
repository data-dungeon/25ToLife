/////////////////////////////////////////////////////////////////////////////
// Multistream wrapper for SPU and file managment
/////////////////////////////////////////////////////////////////////////////
#include "LayersPCH.h"

#include <sifrpc.h>
#include <sifdev.h>
#include <sdmacro.h>	// HAS defines for SD_P_MVOL etc..etc..

#include "Layers/mstream/mstream.h"
#include "Layers/mstream/streamirx.h"

#ifndef CONSUMER_BUILD
//#define FILESTREAM_DEBUG
#endif

/////////////////////////////////////////////////////////////////////////////
// How long is the file
/////////////////////////////////////////////////////////////////////////////
int PS2FileStream::Length()
{
#ifdef FILESTREAM_DEBUG
	TRACE("Asking length of file %d (stream %d) = %d\n", m_fileID, m_streamID, m_length);
#endif

	if (m_streamID >= 0)
		return m_length;
	else
		return -1;
}

/////////////////////////////////////////////////////////////////////////////
// Seek to the specified positon
/////////////////////////////////////////////////////////////////////////////
bool PS2FileStream::Seek(int pos)
{
#ifdef FILESTREAM_DEBUG
	TRACE("Seeking to %d (of %d) of file %d (stream %d)\n", pos, m_length, m_fileID, m_streamID);
#endif

	// Duh
	if (m_streamID < 0 || m_pendingRead || pos > m_length)
		return false;

	// We are there!
	m_filePos = pos;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Read using the default mode
/////////////////////////////////////////////////////////////////////////////
bool PS2FileStream::Read(
void *destination,
int length,
FileDestination fileDestination)
{
	if (m_async)
		return ReadAsync(destination, length, fileDestination);
	else
		return ReadSync(destination, length, fileDestination);
}

/////////////////////////////////////////////////////////////////////////////
// Read overriding the mode
/////////////////////////////////////////////////////////////////////////////
bool PS2FileStream::ReadAsync(
void *destination,
int length,
FileDestination fileDestination)
{
#ifdef FILESTREAM_DEBUG
	TRACE("Async read of %d (pos/size %d/%d) to 0x%x of file %d (stream %d)\n", length, m_filePos, m_length, destination, m_fileID, m_streamID);
#endif

	ASSERT(destination);
	ASSERT(length > 0);
	if (fileDestination == FILEDEST_EE)
	{
		ASSERT(((unsigned int)destination % 64) == 0);
	}

	// Are we alive?
	if (m_streamID < 0)
		return false;

	// We have a pending read!
	m_pendingRead = true;
	m_pendingReadSize = length;
	m_pendingReadEnd = m_filePos + length;
	if (m_pendingReadEnd > m_length)
	{
		m_pendingReadEnd = m_length;
		length = m_length - m_filePos;
	}

	// Setup the area to read
	SOUND_SetFileOffsetAndPlaySize(m_fileID, m_filePos, length);
	SOUND_LoadFile(StreamDestination(fileDestination), m_streamID, m_fileID, (int)destination);
	MultiStream::SyncIOP();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool PS2FileStream::ReadSync(
void *destination,
int length,
FileDestination fileDestination)
{
#ifdef FILESTREAM_DEBUG
	TRACE("Sync read of %d (pos/size %d/%d) to 0x%x of file %d (stream %d)\n", length, m_filePos, m_length, destination, m_fileID, m_streamID);
#endif

	// Issue the read!
	if (!ReadAsync(destination, length, fileDestination))
		return false;

	// Use our own wait
	int errorCode = 0;
#if 1
	bool done = false;
	while (!done)
	{
		// Are we done yet?
		done = CompletionCheck(NULL, &errorCode);
		if (errorCode != 0)
			break;

		// We are blocking so let other stuff update
		MultiStream::CallBlockingCallback();
	}
#else
	SOUND_WaitForFileToLoad(m_streamID, SOUND_WAIT_HOT);
	DonePendingRead(true);
#endif

	// Clear the pending io
	return (errorCode == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Do we have an async call?
/////////////////////////////////////////////////////////////////////////////
bool PS2FileStream::PendingIO()
{
	return m_pendingRead;
}

/////////////////////////////////////////////////////////////////////////////
// Are we done with an async call?
/////////////////////////////////////////////////////////////////////////////
bool PS2FileStream::CompletionCheck(
int *ioSize,
int *error)
{
	// Need to fill this out
	if (error)
		*error = 0;

	if (!m_pendingRead)
		return true;

	// Try to throttle
	SOUND_SetWaitUpdate();
	while (SOUND_CheckForUpdate() == 0);
	MultiStream::SyncIOP();

#ifdef FILESTREAM_DEBUG
	TRACE("CompletionCheck for file %d (stream %d): ", m_fileID, m_streamID);
#endif

	SOUND_FASTLOAD_INFO info;
	memset(&info, '\0', sizeof(info));
	bool moreData = (SOUND_GetFastLoadInfo(&info) != 0);
	if (!moreData)
	{
#ifdef FILESTREAM_DEBUG
		TRACE("no moreData on GetFastLoadInfo...");
#endif
		return false;
	}

	// is this the stream we care about
	if (info.StreamChannel != m_streamID)
	{
#ifdef FILESTREAM_DEBUG
		TRACE("whoa! different streamid (%d) THIS SHOULD NEVER HAPPEN\n", info.StreamChannel);
#endif
		return false;
	}

	// Update this
	if (ioSize)
		*ioSize = info.TotalSize;

#ifdef FILESTREAM_DEBUG
	TRACE("read %d of %d (status = %d needs to be 1 to be done)\n", info.TotalSize, m_pendingReadSize, info.Status);
#endif

	// Are we done yet?
	if (info.TotalSize < m_pendingReadSize)
		return false;

	// Error
#ifdef ACTUALLYREPORTERRORS
	if (error && info.Status != 0 && info.Status != 1)
	{
		switch (info.Status)
		{
			case 2:
				*error = SCECdErTRMOPN;
				break;
			case 3:
				*error = SCECdErREAD;
				break;
			default:
				*error = SCECdErFAIL;
				break;
		}
	}
#else
	// Don't report errors, but recover
	if (info.Status != 0 && info.Status != 1)
	{
		SOUND_HandleCDErrors();
		return false;
	}
#endif

	// Not done until this goes to 1
	if (info.Status != 1)
		return false;

	// We are done!
	DonePendingRead(true);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void PS2FileStream::DonePendingRead(bool success)
{
	ASSERT(m_pendingRead);

	if (success)
		m_filePos = m_pendingReadEnd;

	m_pendingRead = false;

#ifdef FILESTREAM_DEBUG
	TRACE("Completed pending read file %d (stream %d) success: %d\n", m_fileID, m_streamID, success);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Convert to the MStream dest type
/////////////////////////////////////////////////////////////////////////////
int PS2FileStream::StreamDestination(
FileDestination fileDestination)
{
	switch (fileDestination)
	{
		case FILEDEST_IOP:
			return STREAM_LOAD_IOP;

		case FILEDEST_SPU:
			return STREAM_LOAD_SPU;

		case FILEDEST_EE:
		default:
			return STREAM_LOAD_EE;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
PS2FileStream::PS2FileStream(
int fileID,
int length,
bool async,
bool ownsFileID)
{
	// Save off everything
	ASSERT(fileID >= 0);
	ASSERT(length > 0);
	m_ownsFileID = ownsFileID;
	m_fileID = fileID;
	m_length = length;
	m_async = async;

	// Allocate the stream ID
	m_streamID = MultiStream::AllocateStreamID();
	ASSERT(m_streamID >= 0);
	if (m_streamID < 0)
		MultiStream::BigFatError("Could not allocate file streamID");
	else
	{
		const int iopBufferSize = 64 * 1024;
		if (SOUND_AllocateStreamBuffer(m_streamID, SOUND_DATA_STREAM, iopBufferSize) < 0)
		{
			MultiStream::FreeStreamID(m_streamID);
			MultiStream::BigFatError("Could not allocate IOP ram for file");
		}
	}

	// We start at 0
	m_filePos = 0;

	// No pending io
	m_pendingRead = false;

#ifdef FILESTREAM_DEBUG
	TRACE("Opening fileID %d (stream %d), async: %d ownsFileID: %d\n", m_fileID, m_streamID, async, ownsFileID);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Delete me to close
/////////////////////////////////////////////////////////////////////////////
PS2FileStream::~PS2FileStream()
{
#ifdef FILESTREAM_DEBUG
	TRACE("Closing fileID %d (stream %d)\n", m_fileID, m_streamID);
#endif

	if (m_streamID >= 0)
	{
		SOUND_CloseStreamBuffer(m_streamID);
		MultiStream::FreeStreamID(m_streamID);
	}

	if (m_ownsFileID)
		MultiStream::CloseFileID(m_fileID);
	MultiStream::SyncIOP();
}

