/***************************************************************************/
// SoundChannel
//
// A sound channel is a structure that maintains the hardware voice
// information
/***************************************************************************/
#ifndef SCHANNEL_H
#define SCHANNEL_H

#include "platform/BaseType.h"
#include "Math/Vector.h"
#include "Audio/group/shandle.h"

class SoundEmitter;

class SoundChannel
{
public:
	SoundChannel(SoundEmitter &soundEmitter, SoundDef &soundDef);
	virtual ~SoundChannel();

	// What kind of sound are we 2D or 3D
	enum TYPE
	{
		CHANNEL2D,
		CHANNEL3D
	};
	virtual TYPE Type() = 0;

	// What is our emitter?
	SoundEmitter &Emitter();

	// Get at the sound def
	SoundDef &SoundDefinition();

	// What is the sound's name
	const char *SoundName();

	// Is this sound looped?
	bool Looped();

	// Does this sound have a hardware voice?
	bool HasVoice();

	// Bind a voice into this channel
	void BindVoice(SoundInstance *soundInstance);
	void StealVoice(uint now);
	uint GetNextAutoRestart();
	void UpdateNextAutoRestart(uint now);

	// Is this sound dead? (not playing, not paused, not cue'd, not looped)
	bool Dead();

	// Kill this sound
	void Kill();

	// This is called when the hardware voice is about to be allocated
	// and the channel is about to go through reclaim  checks
	// Return false if this channel doesn't deserve a voice.
	virtual bool UpdateForAquire();

	// When will this sound be done?
	uint GetTimeOfCompletion();

	// Next time to update. 0 means no updates
	uint NextUpdate();

	// The actual update call, decides when to auto reclaim
	virtual void Update(uint now);

	// Access the voice settings
	// These only set the variables, they don't access the instance
	void SetVolume(float newVolume);
	float GetVolume();

	// Set the wet/dry mix
	void SetWetDry(float newWet, float newDry);
	float GetWet();
	float GetDry();

	// Set the pitch
	void SetPitch(float newPitch);

	// Access the handle
	void SetHandle(SoundHandle newHandle);
	SoundHandle GetHandle();

	// Play the sound
	virtual bool Play(uint now);

	// Stop the sound
	void Stop();

	// Handle pause/unpause
	void Pause();
	void Unpause();
	bool Paused();

	// Force this sound's auto-reclaim state
	void SetAutoReclaim(bool setAutoReclaim);
	bool GetAutoReclaim();

	// Will this channel restart if it loses it's voice?
	void SetAutoRestart(bool setAutoRestart);
	bool GetAutoRestart();

	// Set the auto restart delay
	void SetAutoRestartDelay(uint delayMS);
	uint GetAutoRestartDelay();

	// Lock/unlock this channel.  Locking will prevent voice from
	// ever being reclaimed
	void Lock();
	bool Unlock();
	bool Locked();

protected:
	SoundEmitter	&emitter;				// Our spawning emitter
	SoundDef			&def;						// The sound def
	SoundInstance	*voice;					// Hardware voice

	// Do we auto-reclaim this voice (mark it as dead when it's done)
	bool				autoReclaim;

	// Auto restart support
	bool				autoRestart;
	uint				autoRestartDelay;
	uint				nextAutoRestart;

	int				lock;

	// What is our state?
	enum STATE
	{
		CUED,
		PLAYING,
		STOPPED,
		DEAD
	};
	STATE				state;

	// When should the next update occur?
	uint				nextUpdate;

	// When will this sound be done?
	uint				timeOfCompletion;

	// The bank values
	float 			bankVolume;
	float				bankPitch;
	virtual void InitBankValues();

	float				volume;					// volume
	float				wet, dry;				// wet, dry mix
	float				pitch;               // pitch
	bool				paused;					// paused

	SoundHandle		handle;					// Handle to this sound

	// These get called to compute volume and pitch
	virtual float ComputeVolume();
	virtual float ComputePitch();
	virtual void ApplyVolume();
	virtual void ApplyMix();
	virtual void ApplyPitch();

	// Called during a play to apply everything
	// only called if state == PLAYING
	virtual void ApplySettings();

public:
	// We are going to appear in a doubly-linked list
	SoundChannel	*prev, *next;
};

#endif
