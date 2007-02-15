/***************************************************************************/
// Ambient sound holder
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
/***************************************************************************/
AmbientSound::AmbientSound(
const char *soundName,
float useVolume,
bool isStereo)
{
	Clear();
	SetName(soundName);
	volume = useVolume;
	stereo = isStereo;
}

/***************************************************************************/
/***************************************************************************/
AmbientSound::AmbientSound(
ts_EnvSoundField &field)
{
	strcpy(name, field.ambient);
	stereo = (field.flags & ENVSOUNDFIELD_AMBIENTSTEREO) != 0;
	volume = field.volume;

	minRetriggerTime = field.minRetriggerTime;
	maxRetriggerTime = field.maxRetriggerTime;

	fadeInTime = field.fadeInTime;
	fadeOutTime = field.fadeOutTime;
}

/***************************************************************************/
// Clear this out
/***************************************************************************/
void AmbientSound::Clear()
{
	name[0] = '\0';
	stereo = false;
	volume = 1.0f;
	minRetriggerTime = maxRetriggerTime = 0;
	fadeInTime = fadeOutTime = 1.0f;
}

/***************************************************************************/
// Is this empty?
/***************************************************************************/
bool AmbientSound::Empty() const
{
	return (name[0] == '\0');
}

/***************************************************************************/
// Are we equal?
/***************************************************************************/
int AmbientSound::operator==(
const AmbientSound &s) const
{
	return (strcmp(name, s.name) == 0 &&
				stereo == s.stereo &&
				volume == s.volume &&
				minRetriggerTime == s.minRetriggerTime &&
				maxRetriggerTime == s.maxRetriggerTime);
}

/***************************************************************************/
/***************************************************************************/
int AmbientSound::operator!=(
const AmbientSound &s) const
{
	return (strcmp(name, s.name) != 0 ||
				stereo != s.stereo ||
				volume != s.volume ||
				minRetriggerTime != s.minRetriggerTime ||
				maxRetriggerTime != s.maxRetriggerTime);
}

/***************************************************************************/
// Copy
/***************************************************************************/
AmbientSound &AmbientSound::operator=(
const AmbientSound &s)
{
	strcpy(name, s.name);
	stereo = s.stereo;
	volume = s.volume;
	minRetriggerTime = s.minRetriggerTime;
	maxRetriggerTime = s.maxRetriggerTime;
	fadeInTime = s.fadeInTime;
	fadeOutTime = s.fadeOutTime;

	return *this;
}

/***************************************************************************/
// Is this a triggered sound?
/***************************************************************************/
bool AmbientSound::Triggered() const
{
	return !(minRetriggerTime == 0 && maxRetriggerTime == 0);
}

/***************************************************************************/
// Access the members
/***************************************************************************/
void AmbientSound::SetName(
const char *newName)
{
	strncpy(name, newName, sizeof(name) - 1);
	name[sizeof(name) - 1] = '\0';
}

/***************************************************************************/
/***************************************************************************/
const char *AmbientSound::Name() const
{
	return name;
}

/***************************************************************************/
/***************************************************************************/
void AmbientSound::SetStereo(
bool isStereo)
{
	stereo = isStereo;
}

/***************************************************************************/
/***************************************************************************/
bool AmbientSound::Stereo() const
{
	return stereo;
}

/***************************************************************************/
/***************************************************************************/
void AmbientSound::SetVolume(
float newVolume)
{
	volume = newVolume;
}

/***************************************************************************/
/***************************************************************************/
float AmbientSound::Volume() const
{
	return volume;
}

/***************************************************************************/
/***************************************************************************/
void AmbientSound::SetMinRetriggerTime(
int newMinRetriggerTime)
{
	minRetriggerTime = newMinRetriggerTime;
}

/***************************************************************************/
/***************************************************************************/
int AmbientSound::MinRetriggerTime() const
{
	return minRetriggerTime;
}

/***************************************************************************/
/***************************************************************************/
void AmbientSound::SetMaxRetriggerTime(
int newMaxRetriggerTime)
{
	maxRetriggerTime = newMaxRetriggerTime;
}

/***************************************************************************/
/***************************************************************************/
int AmbientSound::MaxRetriggerTime() const
{
	return maxRetriggerTime;
}

/***************************************************************************/
/***************************************************************************/
void AmbientSound::SetFadeInTime(
float newFadeInTime)
{
	fadeInTime = newFadeInTime;
}

/***************************************************************************/
/***************************************************************************/
float AmbientSound::FadeInTime() const
{
	return fadeInTime;
}

/***************************************************************************/
/***************************************************************************/
void AmbientSound::SetFadeOutTime(
float newFadeOutTime)
{
	fadeOutTime = newFadeOutTime;
}

/***************************************************************************/
/***************************************************************************/
float AmbientSound::FadeOutTime() const
{
	return fadeOutTime;
}

