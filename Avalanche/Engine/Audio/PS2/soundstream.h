/***************************************************************************/
// SoundStream, a disk or i/o based sound
/***************************************************************************/
#ifndef SOUNDSTREAM_H
#define SOUNDSTREAM_H

#include "Audio/sstream.h"
#include "Audio/PS2/sdriver.h"

// PS2 soundstream
class SoundStreamPS2 : public SoundStream
{
public:
	// The driver must create the sound, but it can be deleted directly
	~SoundStreamPS2();

	// support a SoundInstance interface
	bool Play();
	bool Stop(bool rewind = false);
	bool IsPlaying();

	// Return the play position, returns -1 if still preloading
	int CurrentPlayPos();

	// Seek
	bool Seek(int pos)
		{ return false; }
	bool Rewind()
		{ return false; }

	// Set the volume [0..1], 0.0 is silent 1.0 is full volume
	bool SetVolume(float wantVolume);

	// Set the pan [-1..1] (stereo streams can't be panned)
	bool SetPan(float pan);

	// Change the pitch of the sound in semitones
	bool SetPitch(float pitchShift);

	// Set the wet/dry mix.  Beware, on the PS2 anything over 0.0
	// will be 100% on
	bool SetWetDry(float wet, float dry);

	// Thanks for the comment
	t_Error StreamError()
		{ return MED_ERR_NONE; }

private:
	// our constructor
	SoundStreamPS2(SoundDriverPS2 &parent, const char *streamName, SoundAttr &attr, MIBStream &mibStream, bool wantRepeat);

	// Convert volume into PS2 space (we only support stereo
	void ComputeVolume(MIBStream::Volume volume[2]);

	// Our parent
	SoundDriverPS2		&driver;

	// Our stream information
 	MIBStream			*stream;

	// state tracking
	bool					queued;
	bool					paused;

	bool					preloaded;
	int					playPos;

	// Our friends
	friend class SoundDriverPS2;
};

#endif
