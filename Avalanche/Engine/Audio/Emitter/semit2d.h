/***************************************************************************/
// 2D SoundEmitter
/***************************************************************************/
#ifndef SEMIT2D_H
#define SEMIT2D_H

#include "Audio/Emitter/semitter.h"
#include "Audio/group/schan2d.h"

class Sound;

// 2D Sound group
class SoundEmitter2D : public SoundEmitter
{
public:
	// Return our type
	TYPE Type()
		{ return EMITTER2D; }

	// check semitter.h, as this inherits all the functions

	// Set the pan [-1..1], returns false if sound has expired
	bool SetPan(SoundHandle handle, float pan);

	// Set the wet/dry mix
	bool SetWetDry(SoundHandle handle, float wet, float dry);

protected:
	// set up
	SoundEmitter2D(const char *name,
						SoundDriver &soundDriver,
						SoundGroup &soundGroup);

	// Resolve and upcast
	SoundChannel2D *ResolveHandle2D(SoundHandle handle);

	// The audio system creates us
	friend class Audio;

public:
	// Must be public for list management
	virtual ~SoundEmitter2D();
};

#endif
