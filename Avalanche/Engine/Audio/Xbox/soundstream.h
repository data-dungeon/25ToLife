/***************************************************************************/
// SoundStream, a disk or i/o based sound
/***************************************************************************/
#ifndef SOUNDSTREAM_H
#define SOUNDSTREAM_H

#include "Audio/sstream.h"
#include "Audio/Xbox/sound.h"
#include "Audio/Xbox/sdriver.h"

class SoundStreamXbox : public SoundStream
{
public:
	// our constructor
	SoundStreamXbox(SoundDriverServices &driverServices, const char *name, bool loop) :
		SoundStream(name, loop), driver(driverServices) {}

	// The driver must create the sound, but it can be deleted directly
	virtual ~SoundStreamXbox() {}

	// Call the process (register via SoundDriverServices)
	virtual void Process() {}

protected:
	// The sound driver services are always available to subclasses
	SoundDriverServices		&driver;

public:
	// We can appear in a list managed by the driver (not for your use)
	SoundStreamXbox			*prev, *next;
};

#endif
