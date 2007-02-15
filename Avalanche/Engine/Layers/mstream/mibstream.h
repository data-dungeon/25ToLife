/////////////////////////////////////////////////////////////////////////////
// Wrapper for a MIB stream (a multistream ADPCM stream)
/////////////////////////////////////////////////////////////////////////////
#ifndef MIBSTREAM_H
#define MIBSTREAM_H

#include "Layers/mstream/streamirx.h"
#include "container/pool.h"

// Forward declare us
class MultiStream;

class MIBStream
{
public:
	// Open, given the appropriate info
	bool Open(SOUND_MIH_INFO &header, int fileID, int offset, int size, bool repeat = false);

	// Is this stream legal?
	bool Valid();

	// Start playback
	struct Volume
	{
		int left, right;
	};
	bool Start(const Volume *volume, int channels);

	// Is the stream playing?
	bool IsPlaying();

	// What is the play position? (-1 means not playing or preloading)
	int PlayPosition();

	// Set the pitch to pause / unpause (does no state tracking)
	void Pause();
	void Unpause();

	// Set quanties easily to the SPUChannels
	bool SetVolume(Volume *volume, int channels);
	bool SetPitch(int pitch);
	bool SetWetDry(bool wet, bool dry);

	// Close a stream
	void Close();

	// Get at the SPU channels
	int Channels()
		{ return m_files; }
	SPUChannel *GetChannel(int index);

	// Get the base pitch (PS2 units)
	int BasePitch()
		{ return m_pitch; }

	// Dump some debug info on this stream
	void Debug();

	// Stream support
	static void DisplayHeader(char *fileName, SOUND_MIH_INFO &headerInfo);

private:
	// Our name
	char			m_streamName[64];	// Our stream name

	// Stuff from the header
	int			m_trackSize;		// Size of of single track
	int			m_files;				// number of files (must be 1 or 2)
	int			m_pitch; 			// Pitch (in SPU) required
	int			m_endOffset;		// End offset
	bool			m_loop;				// Loop

	// The open stream file
	int 			m_fileID;		  	// file ID
	int			m_offset, m_size;	// offset and size

	// The PS2 resources
	int			m_streamID;  		// Base stream ID
	int			m_childID;			// child stream ID
	uint			m_iopSize;			// Memory on the IOP
	SPUChannel 	*m_spuChannel[2];	// Base SPU channel
	uint			m_spuAddr[2];  	// Address in SPU RAM
	uint 			m_spuSize;			// Size of SPU

	// Some tracking
	int			m_currentPitch;
	Volume		m_volume[2];

	// Utilities
	int GetVolumeL(int channel, const Volume *volume, int channels);
	int GetVolumeR(int channel, const Volume *volume, int channels);

	// Only the mutlistream class can create us
	MIBStream(const char *streamName);
	~MIBStream();
	friend class MultiStream;
	friend class Pool<MIBStream>;
};

#endif


