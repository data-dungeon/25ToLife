/***************************************************************************/
// Environmental sound triggers
/***************************************************************************/
#ifndef ENVSTRIG_H
#define ENVSTRIG_H

#include "Math/Vector.h"

class SoundListener;

class EnvSoundTrigger
{
public:
	EnvSoundTrigger() {}
	virtual ~EnvSoundTrigger() {}

	// If call trigger succeeds, this returns information about
	// where to trigger the sound
	struct Event
	{
		Vector3		pos;
		Vector3		velocity;
		float			volume;
		float			pitch;
		float			ambience;
		float			presence;
	};

	// Return true with Event filled out to trigger a sound
	virtual bool Trigger(SoundListener &listener, uint time, Event &event) = 0;
};

class InTheTreesEST : public EnvSoundTrigger
{
public:
	InTheTreesEST(float maxHeight, float maxDistance,
						uint now, int minTimeBetween, int maxTimeBetween,
						float baseVolume, float volumeRandPercent,
						float basePitch, float pitchRandPercent,
						float baseAmbience = 0.0f, float basePresence = 0.0f);
	~InTheTreesEST() {}

	// Return true with Event filled out to trigger a sound
	bool Trigger(SoundListener &listener, uint now, Event &event);

private:
	float		height;
	float		distance;
	int		minTime;
	int		maxTime;
	float		volume, volumeRand;
	float		pitch, pitchRand;
	float		ambience;
	float		presence;

	uint		nextTrigger;
};

#endif
