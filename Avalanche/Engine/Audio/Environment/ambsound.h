/***************************************************************************/
// Ambient sound holder
/***************************************************************************/
#ifndef AMBSOUND_H
#define AMBSOUND_H

#include "Audio/Environment/envdbl.h"

class AmbientSound
{
public:
	AmbientSound() {}
	AmbientSound(const char *soundName, float useVolume = 1.0f, bool isStereo = false);
	AmbientSound(ts_EnvSoundField &field);
	~AmbientSound() {}

	// Clear this out
	void Clear();

	// Is this empty?
	bool Empty() const;

	// Are we equal?
	int operator==(const AmbientSound &s) const;
	int operator!=(const AmbientSound &s) const;

	// Copy
	AmbientSound &operator=(const AmbientSound &s);

	// Is this a triggered sound?
	bool Triggered() const;

	// Access the members
	void SetName(const char *newName);
	const char *Name() const;

	void SetStereo(bool isStereo);
	bool Stereo() const;

	void SetVolume(float newVolume);
  	float Volume() const;

	void SetMinRetriggerTime(int newMinRetriggerTime);
	int MinRetriggerTime() const;

	void SetMaxRetriggerTime(int newMaxRetriggerTime);
	int MaxRetriggerTime() const;

	void SetFadeInTime(float newFadeInTime);
	float FadeInTime() const;

	void SetFadeOutTime(float newFadeOutTime);
	float FadeOutTime() const;

private:
	char	name[MAX_SOUND_NAME_SIZE];		// ambient effect name from MOD
	bool	stereo;								// In stereo?
	float	volume;								// ambient volume

	int	minRetriggerTime;		 			// minimum trigger time in ms
	int	maxRetriggerTime;					// maximum trigger time in ms

	float	fadeInTime;							// Fade in time
	float	fadeOutTime;						// Fade out time
};

#endif
