/***************************************************************************/
// Environmental sound triggers
/***************************************************************************/
#include "Audio/AudioPCH.h"
#include "Math/fastrand.h"

/***************************************************************************/
/***************************************************************************/
InTheTreesEST::InTheTreesEST(
float maxHeight,
float maxDistance,
uint now,
int minTimeBetween,
int maxTimeBetween,
float baseVolume,
float volumeRandPercent,
float basePitch,
float pitchRandPercent,
float baseAmbience,
float basePresence)
{
	height = maxHeight;
	distance = maxDistance;
	minTime = minTimeBetween;
	maxTime = maxTimeBetween;
	volume = baseVolume;
	volumeRand = volumeRandPercent;
	pitch = basePitch;
	pitchRand = pitchRandPercent;
	ambience = baseAmbience;
	presence = basePresence;

	nextTrigger = now + (uint)g_random.InRange(minTime, maxTime);
}

/***************************************************************************/
// Return true with Event filled out to trigger a sound
/***************************************************************************/
bool InTheTreesEST::Trigger(
SoundListener &listener,
uint now,
Event &event)
{
	// Time yet?
	if (now < nextTrigger)
		return false;

	// Setup the next trigger time
	nextTrigger = now + (uint)g_random.InRange(minTime, maxTime);

	// Fill out the event
	event.pos = listener.Position();
	event.pos += Vector3(g_random.NegPos() * distance,
								g_random.Unit() * height,
								g_random.NegPos() * distance);
	event.velocity.Clear();
	event.volume = Math::Clamp(volume + g_random.NegPos() * volumeRand * volume);
	event.pitch = pitch + g_random.NegPos() * pitchRand * pitch;
	event.ambience = ambience;
	event.presence = presence;

	return true;
}
