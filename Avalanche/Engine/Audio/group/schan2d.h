/***************************************************************************/
// SoundChannel2D
/***************************************************************************/
#ifndef SCHAN2D_H
#define SCHAN2D_H

#include "Audio/group/schannel.h"

// This is basically the same as the base class
class SoundChannel2D : public SoundChannel
{
public:
	SoundChannel2D(SoundEmitter &soundEmitter, SoundDef &soundDef);
	~SoundChannel2D();

	// Return we are 2D
	TYPE Type();

	// Allow them to set the pan
	void SetPan(float newPan);

private:
	float				pan;

	// Apply the pan settings
	void ApplyPan();

	// Called during a play to apply everything
	void ApplySettings();
};

#endif
