/***************************************************************************/
// An environmental source
/***************************************************************************/
#ifndef ENVSOURCE_H
#define ENVSOURCE_H

#include "CollideHelper/zone.h"
#include "Audio/Environment/envstrig.h"

// Forwards
class SoundStream3D;

// Abstraction for SFX and streams
class EnvSourceSound
{
public:
	// Don't create this is this returns false!
	static bool Valid(SoundDriver &driver, const char *name, EnvSoundSourceType type);

	EnvSourceSound(const char *name, EnvSoundSourceType type);
	~EnvSourceSound();

	// Our name
	const char *Name()
		{ return m_name; }

	// Our type
	enum TYPE
	{
		ESS_SFX,
		ESS_STREAM
	};
	TYPE Type()
		{ return m_type;}

	// Are we looped (inaccurate until the first play call)
	bool Looped()
		{ return m_looped; }

	// Play control
	bool Play(SoundDriver &driver, SoundEmitter3D &emitter, uint now,
					const Vector3 &pos, const Vector3 *velocity,
					float volume = 1.0f, float pitch = 0.0f, float ambience = 0.0f, float presence = 0.0f, float audibleDistance = 0.0f);
	bool Stop();
	bool IsPlaying();

private:
	// Reference to our name
	char 				m_name[32];

	// Our type
	TYPE				m_type;

	// Are we looped?
	bool				m_looped;

	// The handle
	SoundHandle		m_handle;			// Handle for playing sound
	SoundStream3D	*m_stream;			// Stream
};

// Basic environmental source
class EnvSource
{
public:
	EnvSource(const char *name, EnvSoundSourceType type);
	virtual ~EnvSource();

	// What type are we?
	enum TYPE
	{
		STATIC,
		TRIGGERED
	};
	virtual TYPE Type() = 0;

	// Access info
	const char *Name()
		{ return sound.Name(); }

	// Activate/deactivate
	void Activate()
		{ active = true; }
	void Deactivate()
		{ Stop(); active = false; }
	bool Active()
		{ return active; }

	// Play control
	virtual void Stop();
	bool Playing()
		{ return sound.IsPlaying(); }

protected:
	EnvSourceSound	sound;			// The sound
	bool				active;			// Is this sound active?
};

class EnvSourceStatic : public EnvSource, public ZoneSphere
{
public:
	EnvSourceStatic(int id, const char *name, EnvSoundSourceType type,
							Vector3CRef pos, float audibleRange,
							float baseVolume, float basePitch,
							float baseAmbience, float basePresence,
							int minTime, int maxTime);
	~EnvSourceStatic() {}

	// We are the staic implementation
	TYPE Type()
		{ return STATIC; }

	// Is this sound ready to play?
	// May cause an update if it expires
	bool ReadyToPlay(uint now);

	// Play control
	void Play(SoundDriver &driver, SoundEmitter3D &emitter, uint now);

	// Is this sound active?
	bool Active()
		{ return EnvSource::Active(); }

	// Does this sound have retrigger logic?
	bool HasTrigger()
		{ return (minTriggerTime != 0 && maxTriggerTime != 0); }

protected:
	// Sound settings
	float				volume;
	float 			pitch;
	float				ambience;
	float				presence;

	// Does this sound have a trigger?
	int				minTriggerTime, maxTriggerTime;
	uint				nextPlay;
};

// A procedural trigger sound source
class EnvSourceTrigger : public EnvSource
{
public:
	EnvSourceTrigger(const char *name, EnvSoundSourceType type, EnvSoundTrigger *theTrigger);
	~EnvSourceTrigger();

	// We are the triggered implementation
	TYPE Type()
		{ return TRIGGERED; }

	// Update
	void Update(SoundDriver &driver, SoundEmitter3D &emitter, uint now);

protected:
	// Next time we need to update
	uint 							nextUpdate;
	bool							initialized;

	// Stuff for the trigger
	EnvSoundTrigger 			*trigger;
	EnvSoundTrigger::Event 	event;

public:
	EnvSourceTrigger	*prev, *next;
};

#endif
