/***************************************************************************/
// Ambient sound manager
/***************************************************************************/
#ifndef AMBIENT_H
#define AMBIENT_H

#include "Audio/Emitter/semit3d.h"
#include "Audio/function/ramp.h"
#include "Audio/Environment/ambsound.h"

class AmbientSoundController
{
public:
	AmbientSoundController();
	~AmbientSoundController();

	// Setup the emitter
	void SetEmitter(SoundEmitter2D *parentEmitter);

	// Reset everything
	void Clear();

	// Queue an ambient sound
	void Queue(const AmbientSound &queueMe);

	// Update
	void Daemon(uint now);

private:
	// Our parent emitter
	SoundEmitter2D *emitter;

	// Handle of playing sound
	SoundHandle		handle[2];
	void Stop();
	void Play(float volume = 1.0f);

	// Fade support
	enum STATE
	{
		SILENT,
		FADEIN,
		PLAYING,
		FADEOUT
	};
	STATE				state;
	RampUp			rampUp;
	RampDown			rampDown;
	void BeginFadeIn(uint now, float volume);
	void BeginFadeOut(uint now);

	// The sounds
	AmbientSound		currentSound;
	AmbientSound		nextSound;
	void ClearQueue()
		{ nextSound.Clear(); }
	bool Queued()
		{ return !nextSound.Empty(); }
	void AdvanceQueue(uint now);

	// Retrigger support
	uint					nextTrigger;
	uint NextTrigger(uint now);
};

#endif
