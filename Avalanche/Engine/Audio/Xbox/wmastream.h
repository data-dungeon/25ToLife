/////////////////////////////////////////////////////////////////////////////
// WMA based streaming sound, supports soundtracks
/////////////////////////////////////////////////////////////////////////////
#ifndef WMASTREAM_H
#define WMASTREAM_H

#include "Audio/Xbox/soundstream.h"
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"

struct ImbeddedStreamInfo;

// A sound stream based on a WMA file/soundtrack
class WMAStream : public SoundStreamXbox
{
public:
#ifdef _DEBUG
	static bool Test(ImbeddedStreamInfo &info, const char *fileName);
#else
	static bool Test(ImbeddedStreamInfo &info, const char *fileName) { return true; }
#endif

	// The driver must create the sound, but it can be deleted directly
	~WMAStream();

	// Simple controls
	bool Play();
	bool Stop(bool rewind = false);
	bool IsPlaying();

	// Return the play position, returns -1 if still preloading
	int CurrentPlayPos();

	// Seek to a position
	bool Seek(int pos);

	// Rewind the sound
	bool Rewind();

	// Set the volume from [0..1]
	bool SetVolume(float volume);

	// Set the pan [-1..1] (stereo streams can't be panned)
	bool SetPan(float pan);

	// Change the pitch of the sound in semitones
	bool SetPitch(float pitchShift);

	// Set the mix for this sound stream
	bool SetWetDry(float newWet, float newDry);

private:
	// our constructor
	WMAStream(SoundDriverServices &driverServices, const char *fileName, bool loop);
	WMAStream(SoundDriverServices &driverServices, ImbeddedStreamInfo &info, const char *fileName, bool loop);
	WMAStream(SoundDriverServices &driverServices, DWORD soundTrackID, bool loop);

	// What type of stream are we?
	enum CreationType
	{
		BYFILENAME,
		BYFILEOFFSET,
		BYSOUNDTRACK
	};
	CreationType	type;
	char 				streamFileName[64];	// only saved for type == BYFILENAME
	HANDLE			streamFile;				// only saved for type == BYFILEOFFSET
	DWORD				streamOffset;			// only saved for type == BYFILEOFFSET
	DWORD				streamSoundTrackID;  // only saved for type == BYSOUNDTRACK
	HANDLE			soundTrackHandle;    // only saved for type == BYSOUNDTRACK

	// Are we going to repeat this track?
	bool repeat;

	// Are we paused?
	bool paused;

	// Apply mixbins
	bool ApplyMixBins();

	// the wma decoder will work properly in async mode, with all formats
	// only when using a lookahead size of at least 64k. We are using it in
	// synchronous mode but for good measure we still pass 64k
	#define WMASTRM_LOOKAHEAD_SIZE (64 * 1024)

	// Define the maximum amount of packets we will ever submit to the renderer
#ifdef DEBUG
	#define WMASTRM_PACKET_COUNT 8
#else
	#define WMASTRM_PACKET_COUNT 4
#endif

	// Define the source packet size:
	// This value is hard-coded assuming a WMA file of stero, 16bit resolution.  If
	// this Value can by dynamically set based on the wma format, keeping in mind
	// that wma needs enough buffer for a minimum of 2048 samples worth of PCM data
	#define WMASTRM_SOURCE_PACKET_BYTES (2048 * 2 * 2)

	// The stuff we need to play a WMA
	XWmaFileMediaObject	*inputStream;									// Source (wma file)
	IDirectSoundStream	*outputStream;						 			// Render (DirectSoundStream)
	BYTE						sourceBuffer[WMASTRM_SOURCE_PACKET_BYTES * WMASTRM_PACKET_COUNT];									// Source filter data buffer
	struct Packet
	{
		DWORD					status;
		DWORD					size;
	};
	Packet					packet[WMASTRM_PACKET_COUNT];

	DWORD			 			filePos;											// File progress, in bytes
	DWORD						fileSize;

	DWORD						lastDecoderStatus;              			// last known wma decoder status
	HRESULT					openResult;                     			// Result of opening file

	DWORD						startingDataOffset;							// offset into wma file were data begins
	WAVEFORMATEX			wfxSourceFormat;

	// Format of the WMA
#ifdef WMACONTENTDESC
	WMAXMOFileContDesc	wmaContentDescription;						// text information about author, title, description, etc
#endif
	WMAXMOFileHeader		wmaFileHeader;									// basic information about the wma file

	// Are we done yet?
	bool						doneRead;

	// Are we an active process?
	bool						processActive;

	// Clear our state
	void Initiailize();

	// Startup/terminate the stream
	bool Open();
	void Close();

	// Create the stream
	bool CreateStream();

	// Processing (must call this repeatedly)
	void Process();

	// Packet processing
	bool FindFreePacket(int &packetIndex);
	bool ProcessSource(int packetIndex);
	bool ProcessRenderer(int packetIndex);

	// Play control
	void Pause(bool pause);

	friend class SoundDriverXbox;
};

#endif
