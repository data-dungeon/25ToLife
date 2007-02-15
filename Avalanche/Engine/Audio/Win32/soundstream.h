/***************************************************************************/
// SoundStream, a disk or i/o based sound
/***************************************************************************/
#ifndef SOUNDSTREAM_H
#define SOUNDSTREAM_H

#include "Audio/sstream.h"
#include "Audio/Win32/sound.h"
#include "Audio/Win32/sdriver.h"
#include "Audio/Win32/wavefile.h"
#ifdef DIRECTX_PC
#include "audio/win32/OggFile.h"
#endif
// Class for ecapsulating stream I/O for the SoundStream
class StreamIOSystem;

class SoundStreamWin32 : public SoundStream, public SoundData
{
public:
	// The driver must create the sound, but it can be deleted directly
	~SoundStreamWin32();

	// Move instance functions up, as most platforms won't
	// support a SoundInstance interface
	bool Play();
	bool Stop(bool rewind = false);
	bool IsPlaying();

	// Return the play position, returns -1 if still preloading
	int CurrentPlayPos();

	// Seek to a position
	bool Seek(int pos)
		{ return false; }

	// Rewind the sound
	bool Rewind();

	// Set the volume [0..1], 0.0 is silent 1.0 is full volume
	bool SetVolume(float volume);

	// Set the pan [-1..1] (stereo streams can't be panned)
	virtual bool SetPan(float pan);

	// Change the pitch of the sound in semitones
	virtual bool SetPitch(float pitchShift);

	// Set the mix for this sound stream
	bool SetWetDry(float wet, float dry)
		{ return true; }

	// Get the pointer to our instance (for control)
	// This is platform specific, most devices won't support
	// a stream sound as an instance
	SoundInstance *Instance()
		{ return instance; }

private:
	// our constructor
	SoundStreamWin32(SoundDriverServices &driverServices, const char *streamName, SoundAttr &bufferAttr, bool loop, LPDIRECTSOUNDBUFFER theBuffer, int bufferSize);

	// Our name
	StreamIOSystem	*ioSystem;
	int				buffersPlayed;
	SoundInstance	*instance;

	// Set the i/o methodology (we will delete object when done)
	bool SetIOSystem(StreamIOSystem *streamIOSystem);

	#define STREAMEVENT_START		0
	#define STREAMEVENT_HALFWAY   1
	#define STREAMEVENTS				2
	DSBPOSITIONNOTIFY 				posNotify[STREAMEVENTS];
	HANDLE								hEvent[STREAMEVENTS];
	LPDIRECTSOUNDNOTIFY				notify;
	DWORD									threadHandle;
	BOOL  								threadExited;
	BOOL									exitThread;
	CRITICAL_SECTION 					criticalSection;

	// Setup/destroy the thread
	void InitThread();
	bool StartThread();
	void ShutdownThread();
	inline void BlockThread();
	inline void UnblockThread();
	static void __cdecl ThreadFunction(void *argList);

	int stopEvent;
	void StreamToBuffer(int event);

	// Our friends
	friend class SoundDriverWin32;
};

// Abstract base class for a stream I/O system
// Assumes first read comes from offset 0 of the sound data
class StreamIOSystem
{
public:
	StreamIOSystem()	{}
	virtual ~StreamIOSystem() {}

	// Is this I/O functional?
	virtual bool ReadValid() = 0;

	// The read command
	virtual bool Read(unsigned long request, BYTE *data, unsigned long *read) = 0;

	// Rewind to the beginning
	virtual bool Rewind() = 0;

	// Return the current seek position/size
	virtual unsigned long Position() = 0;
};

// Stream I/O system for wave file
class WaveStreamIOSystem : public StreamIOSystem
{
public:
	// Specify the wave file
	WaveStreamIOSystem(char *waveFile);
	~WaveStreamIOSystem();

	// Is this I/O functional?
	bool ReadValid()
		{ return valid; }

	// Get the header for this bad boy
	const WAVEFORMATEX *WaveFormat()
		{ return waveRead.WaveFormat(); }

	// The read command
	bool Read(unsigned long request, BYTE *data, unsigned long *read);

	// Rewind to the beginning
	bool Rewind();

	// Return the current seek position/size
	unsigned long Position()
		{ return seek; }

private:
	bool				valid;		// Valid file?
	unsigned int 	seek;			// current seek position
	WaveFile			waveRead;	// wave reading handy thingie

	// Open/close the wave file
	bool Prepare(char *file);
	void Close();
};

#ifdef DIRECTX_PC
// Stream I/O system for Ogg files
class OggStreamIOSystem : public StreamIOSystem
{
public:
	// specify the ogg file
	OggStreamIOSystem(char* oggFile);
	~OggStreamIOSystem();

	// Is this I/O functional?
	bool ReadValid() { return valid; }

	// Get the header for this bad boy
	const WAVEFORMATEX *WaveFormat()
		{ return oggRead.WaveFormat(); }

	// the read command
	bool Read(unsigned long request, BYTE *data, unsigned long *read);

	// Rewind to the beginning
	bool Rewind();
	
	// Return the current seek position/size
	unsigned long Position() { return seek; }

private:
	bool				valid;
	unsigned int		seek;
	OggFile				oggRead;

	// open/close the ogg file
	bool Prepare(char *file);
	void Close();
};
#endif // DIRECTX_PC

#endif
