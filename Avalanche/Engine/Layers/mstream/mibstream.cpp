/////////////////////////////////////////////////////////////////////////////
// Multistream wrapper for SPU and file managment
/////////////////////////////////////////////////////////////////////////////
#include "LayersPCH.h"

#include "Layers/mstream/mstream.h"
#include "Layers/mstream/streamirx.h"

/////////////////////////////////////////////////////////////////////////////
// Only the mutlistream class can create us
/////////////////////////////////////////////////////////////////////////////
MIBStream::MIBStream(
const char *streamName)
{
	ASSERT(strlen(streamName) < sizeof(m_streamName) - 1);
	strcpy(m_streamName, streamName);

	m_trackSize = 0;
	m_files = 0;
	m_pitch = 0;
	m_endOffset = 0;
	m_loop = false;

	m_fileID = -1;
	m_offset = 0;
	m_size = 0;

	m_streamID = -1;
	m_childID = -1;
	m_iopSize = 0;
  	m_spuChannel[0] = m_spuChannel[1] = NULL;
	m_spuAddr[0] = m_spuAddr[1] = 0;
	m_spuSize = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MIBStream::~MIBStream()
{
	Close();
}

/////////////////////////////////////////////////////////////////////////////
// Open, given the appropriate info
/////////////////////////////////////////////////////////////////////////////
bool MIBStream::Open(
SOUND_MIH_INFO &header,
int fileID,
int offset,
int size,
bool repeat)
{
	// do we have something valid already?
	ASSERT(m_trackSize == 0);
	if (m_trackSize != 0)
		return false;

	// Copy in the stuff from the MIH header
	m_trackSize = header.TrackSize;						// Size of a single track (one half of a buffer for one stream)
	m_files = header.NumberOfFiles;			  			// Number of interleaved streams in .MIB file
	ASSERT(m_files == 1 || m_files == 2);
	m_pitch = SOUND_HzToSPU(header.Pitch);	// Pitch stored as hz. Convert to SPU value.
	m_endOffset = header.EndOffset;

	// Allocate the stream ID(s)
	m_streamID = MultiStream::AllocateStreamID();
	if (m_streamID < 0)
	{
		//DeleteStreamInfo(info);
		MultiStream::BigFatError("Could not allocate stream 0", m_streamName);
		return false;
	}

	// Allocate the channels
	m_spuChannel[0] = MultiStream::AllocateChannel(m_streamName);
	if (!m_spuChannel[0])
	{
		//DeleteStreamInfo(info);
		MultiStream::BigFatError("Could not allocate base channel", m_streamName);
		MultiStream::FreeStreamID(m_streamID);
		return false;
	}
	if (m_files > 1)
	{
		m_spuChannel[1] = MultiStream::AllocateChannel(m_streamName);
		if (!m_spuChannel[1])
		{
			//DeleteStreamInfo(info);
			MultiStream::BigFatError("Could not allocate base channel", m_streamName);
			MultiStream::FreeChannel(m_spuChannel[0]);
			MultiStream::FreeStreamID(m_streamID);
			return false;
		}
	}

	// Allocate the RAM
	m_spuSize = MultiStream::SizeBy64(m_trackSize * m_files);
	if (m_files > 1)
		m_spuAddr[0] = MultiStream::AllocateSRAM(m_spuSize);
	else
		m_spuAddr[0] = MultiStream::AllocateSRAM(m_spuSize / 2);
	if (!m_spuAddr[0])
	{
		//DeleteStreamInfo(info);
		MultiStream::BigFatError("Could not allocate SPU RAM", m_streamName);
		MultiStream::FreeChannel(m_spuChannel[1]);
		MultiStream::FreeChannel(m_spuChannel[0]);
		MultiStream::FreeStreamID(m_streamID);
		return false;
	}

	// Do the actual allocation
	m_iopSize = SOUND_ReturnMIBBufferSize(m_trackSize, m_files);
	SOUND_AllocateStreamBuffer(m_streamID, m_spuAddr[0], m_iopSize);

	// Setup the spu size
	SOUND_ResizeSPUBuffer(m_streamID, m_spuSize / 2);

	// Allocate the secondary channel
	if (m_files > 1)
		m_spuAddr[1] = m_spuAddr[0] + m_spuSize / 2;

	// Setup the offset and playSize
	m_fileID = fileID;
	m_offset = offset;
	m_size = size;
	m_loop = repeat;
	if (m_loop)
		SOUND_SetFileOffsetAndPlaySize(m_fileID, m_offset - 16, m_size);
	else
		SOUND_SetFileOffsetAndPlaySize(m_fileID, m_offset, m_size);

	// Setup some defaults
	m_currentPitch = m_pitch;
	m_volume[0].left = m_volume[0].right = 0x3fff;
	m_volume[1].left = m_volume[1].right = 0x3fff;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Is this stream legal?
/////////////////////////////////////////////////////////////////////////////
bool MIBStream::Valid()
{
	if (m_fileID < 0 || m_trackSize <= 0 || m_streamID < 0)
		return false;

	for (int f = 0; f < m_files; f++)
	{
		if (!m_spuChannel[f])
			return false;
	}

	// We must be valid...
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Start the sound
/////////////////////////////////////////////////////////////////////////////
bool MIBStream::Start(
const Volume *volume,
int channels)
{
	if (!Valid())
		return false;

	// Setup our play flags
	int repeatFlag = (m_loop ? STREAM_LOOPING : STREAM_ONCE_OFF);

	// Save the volumes
	for (int v = 0; v < m_files; v++)
	{
		m_volume[v].left = GetVolumeL(v, volume, channels);
		m_volume[v].right = GetVolumeR(v, volume, channels);
	}

	// Setup the parent stream
	int ret = SOUND_PlayStream(m_fileID, m_streamID,
											m_spuChannel[0]->GetChannel(),
											m_volume[0].left, m_volume[0].right,
											m_pitch, repeatFlag, 0, 0);
 	ASSERT(ret >= 0);

 	// Tell MultiStream that this is the "Parent", and to use track 0 data.
 	SOUND_SetStreamParent_Int(m_streamID, 0, m_trackSize);

 	// Make the children. Only really supports one right now, but you get the idea
	ASSERT(m_files <= 2);
 	for (int f = 1; f < m_files; f++)
 	{
 		m_childID = MultiStream::AllocateStreamID();
 		ASSERT(m_childID >= 0 && m_childID != m_streamID);
 		SOUND_SetStreamChild_Int(m_childID, m_streamID,
											m_spuChannel[f]->GetChannel(), f,
											m_volume[f].left, m_volume[f].right,
											m_spuAddr[f]);
 	}

 	// Stop bugging me!
 	//SOUND_SetStreamPriority(m_streamID, 0);

	// Weird loop hang workaround (CD LSN based)
 	if (m_loop)
 		SOUND_SetStreamStartPosition(m_streamID, 16);
 	SOUND_SetMIBEndOffset(m_streamID, m_endOffset);

 	// Do I need this?
	MultiStream::SyncIOP();

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool MIBStream::IsPlaying()
{
	// Get the stream info
	SOUND_STREAM_INFO streamInfo;
	int ret = SOUND_GetStreamInfo(m_streamID, &streamInfo);
	ASSERT(ret >= 0);

	// Is the stream reading?
	bool streamReading = (streamInfo.Status != 0);

	// Is the key on?
	int status = SOUND_GetKeyStatus(m_spuChannel[0]->GetChannel());
	bool voicePlaying = (status != 0);

	// What is the play position?
	int playPos = streamInfo.PlayOffset;

	return (m_loop || streamReading || voicePlaying || playPos < m_size);
}

/////////////////////////////////////////////////////////////////////////////
// What is the play position? (-1 means not playing or preloading)
/////////////////////////////////////////////////////////////////////////////
int MIBStream::PlayPosition()
{
	if (!Valid())
		return -1;

	// Get the stream info
	SOUND_STREAM_INFO streamInfo;
	int ret = SOUND_GetStreamInfo(m_streamID, &streamInfo);
	ASSERT(ret >= 0);

	// Are we loaded yet?
	bool preloaded = streamInfo.Active;
	int playPos = streamInfo.PlayOffset;

	if (!IsPlaying() || !preloaded)
		return -1;
	else
		return playPos;
}

/////////////////////////////////////////////////////////////////////////////
// Set the pitch to pause / unpause
/////////////////////////////////////////////////////////////////////////////
void MIBStream::Pause()
{
	if (!Valid())
		return;

	// Stop playback
	for (int f = 0; f < m_files; f++)
	{
		m_spuChannel[f]->SetPitch(0);
		m_spuChannel[f]->SetVolume(0, 0);
	}

	// Send the SOUND_ commands above to the IOP and wait for completion
	MultiStream::SyncIOP();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MIBStream::Unpause()
{
	if (!Valid())
		return;

	// Stop playback
	for (int f = 0; f < m_files; f++)
	{
		m_spuChannel[f]->SetPitch(m_currentPitch);
		m_spuChannel[f]->SetVolume(m_volume[f].left, m_volume[f].right);
	}

	// Send the SOUND_ commands above to the IOP and wait for completion
	MultiStream::SyncIOP();
}

/////////////////////////////////////////////////////////////////////////////
// Set quanties easily to the SPUChannels
/////////////////////////////////////////////////////////////////////////////
bool MIBStream::SetVolume(
Volume *volume,
int channels)
{
  if (!Valid())
		return false;

	// Save the volumes
	for (int v = 0; v < m_files; v++)
	{
		m_volume[v].left = GetVolumeL(v, volume, channels);
		m_volume[v].right = GetVolumeR(v, volume, channels);
	}

	// Just apply it to them all
	for (int f = 0; f < m_files; f++)
		m_spuChannel[f]->SetVolume(m_volume[f].left, m_volume[f].right);

	MultiStream::SyncIOP();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MIBStream::SetPitch(int pitch)
{
  if (!Valid())
		return false;

	// Save it
	m_currentPitch = pitch;

	// Just apply it to them all
	for (int f = 0; f < m_files; f++)
		m_spuChannel[f]->SetPitch(pitch);

	MultiStream::SyncIOP();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MIBStream::SetWetDry(
bool wet,
bool dry)
{
	if (!Valid())
		return false;

	// Just apply it to them all
	for (int f = 0; f < m_files; f++)
		m_spuChannel[f]->SetWetDry(wet, dry);

	MultiStream::SyncIOP();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Close a stream
/////////////////////////////////////////////////////////////////////////////
void MIBStream::Close()
{
	if (m_streamID >= 0)
	{
		SOUND_StopStream(m_streamID);
		SOUND_CloseStreamBuffer(m_streamID);
		MultiStream::FreeStreamID(m_streamID);
	}
	MultiStream::FreeStreamID(m_childID);

	// Release spu channels
	MultiStream::FreeChannel(m_spuChannel[0]);
	m_spuChannel[0] = NULL;
	MultiStream::FreeChannel(m_spuChannel[1]);
	m_spuChannel[1] = NULL;

	// Release spu RAM
	MultiStream::FreeSRAM(m_spuAddr[0]);
	m_spuAddr[1] = 0;

	// We are responsible for always calling this
	MultiStream::SyncIOP();
}

/////////////////////////////////////////////////////////////////////////////
// Get at the SPU channels
/////////////////////////////////////////////////////////////////////////////
SPUChannel *MIBStream::GetChannel(int index)
{
	if (index < 0 || index >= m_files)
		return NULL;
	else
		return m_spuChannel[index];
}

/////////////////////////////////////////////////////////////////////////////
// Dump some debug info
/////////////////////////////////////////////////////////////////////////////
void MIBStream::Debug()
{
	printf(">>>>>> Stream Info for %s (%d tracks)\n", m_streamName, m_files);
	printf("\tFile ID: %d\n", m_fileID);
	printf("\tFile offset/size %d %d\n", m_offset, m_size);
	printf("\tParent stream ID: %d (child: %d)\n", m_streamID, m_childID);
	printf("\tIOP Memory: 0x%x\n", m_iopSize);
	printf("\tSPU Channels: 0x%x 0x%x\n", m_spuChannel[0], m_spuChannel[1]);
	printf("\tSPU Addr: 0x%x 0x%x\n", m_spuAddr[0], m_spuAddr[1]);
	printf("\tSPU size: 0x%x\n", m_spuSize);
}

/////////////////////////////////////////////////////////////////////////////
// Show the header information
/////////////////////////////////////////////////////////////////////////////
void MIBStream::DisplayHeader(
char *fileName,
SOUND_MIH_INFO &headerInfo)
{
	printf("Information for file %s\n", fileName);
	printf("Number of files interleaved in .MIB file: %d\n", headerInfo.NumberOfFiles);
	printf("Pitch of files: %d (SPU 0x%x)\n", headerInfo.Pitch, SOUND_HzToSPU(headerInfo.Pitch));
	printf("Number of tracks per interleaved file: %d\n", headerInfo.Tracks);
	printf("Size of a track: %d\n", headerInfo.TrackSize);
	printf("ID of requested file: %d\n", headerInfo.ID);
	printf("End offset: %d\n", headerInfo.EndOffset);
}

/////////////////////////////////////////////////////////////////////////////
// Utilities
/////////////////////////////////////////////////////////////////////////////
int MIBStream::GetVolumeL(
int channel,
const Volume *volume,
int channels)
{
	if (channel < 0 || !volume || channels == 0)
		return 0x3fff;
	else if (channel < channels)
		return volume[channel].left;
	else
		return volume[channels - 1].left;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int MIBStream::GetVolumeR(
int channel,
const Volume *volume,
int channels)
{
	if (channel < 0 || !volume || channels == 0)
		return 0x3fff;
	else if (channel < channels)
		return volume[channel].right;
	else
		return volume[channels - 1].right;
}


