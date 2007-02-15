/***************************************************************************/
// SoundChannel
//
// A sound channel is a structure that maintains the hardware voice
// information
/***************************************************************************/
#include "Audio/AudioPCH.h"
#include "Math/fastrand.h"

/***************************************************************************/
/***************************************************************************/
SoundChannel::SoundChannel(
SoundEmitter &soundEmitter,
SoundDef &soundDef) :
emitter(soundEmitter),
def(soundDef)
{
	// Our intial state
	state = CUED;

	// No voice yet
	voice = NULL;

	// Assume nothing for now
	bankVolume = 1.0f;
	bankPitch = 0.0f;
	volume = 1.0f;
	wet = 0.0f;
	dry = 1.0f;
	pitch = 0.0f;
	paused = false;

	// By default we auto-reclaim, unless it is a loop
	autoReclaim = !Looped();
	autoRestart = Looped();
	autoRestartDelay = 1000;
	nextAutoRestart = 0;
	lock = 0;

	// Nothing set yet
	handle = NULL;

	// No updates needed now
	nextUpdate = 0;

	// Not in any list yet
	prev = next = NULL;
}

/***************************************************************************/
/***************************************************************************/
SoundChannel::~SoundChannel()
{
	Kill();
}

/***************************************************************************/
// What is our emitter?
/***************************************************************************/
SoundEmitter &SoundChannel::Emitter()
{
	return emitter;
}

/***************************************************************************/
// Get at the sound def
/***************************************************************************/
SoundDef &SoundChannel::SoundDefinition()
{
	return def;
}

/***************************************************************************/
// What is the sound's name
/***************************************************************************/
const char *SoundChannel::SoundName()
{
	return def.name;
}

/***************************************************************************/
// Is this sound looped?
/***************************************************************************/
bool SoundChannel::Looped()
{
	return SoundBank::IsSoundLooped(&def);
}

/***************************************************************************/
// Does this sound have a hardware voice?
/***************************************************************************/
bool SoundChannel::HasVoice()
{
	return (voice != NULL);
}

/***************************************************************************/
// Bind a voice into this channel
/***************************************************************************/
void SoundChannel::BindVoice(
SoundInstance *soundInstance)
{
	ASSERT(voice == NULL);
	ASSERT(soundInstance != NULL);

	voice = soundInstance;
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel::StealVoice(
uint now)
{
	// Clean up the hardware voice
	if (voice)
	{
		voice->Stop();
		voice->Parent()->FreeInstance(voice);
		voice = NULL;
	}

	// Say we are stopped
 	state = STOPPED;

	// When do we restart?
	UpdateNextAutoRestart(now);
}

/***************************************************************************/
/***************************************************************************/
uint SoundChannel::GetNextAutoRestart()
{
	if (autoRestart)
		return nextAutoRestart;
	else
		return 0xffffffff;
}

/***************************************************************************/
// When do we restart?
/***************************************************************************/
void SoundChannel::UpdateNextAutoRestart(
uint now)
{
	nextAutoRestart = now + (uint)g_random.InRange((int)(autoRestartDelay / 2), (int)autoRestartDelay);
}

/***************************************************************************/
// Is this sound dead? (not playing, not paused, not cue'd, not looped)
/***************************************************************************/
bool SoundChannel::Dead()
{
	return (state == DEAD);
}

/***************************************************************************/
// Kill this sound
/***************************************************************************/
void SoundChannel::Kill()
{
	// Clean up the hardware voice
	if (voice)
	{
		voice->Stop();
		voice->Parent()->FreeInstance(voice);
		voice = NULL;
	}

	// Mark it as dead
 	state = DEAD;
}

/***************************************************************************/
/***************************************************************************/
bool SoundChannel::UpdateForAquire()
{
	// Refresh the values from the bank
	InitBankValues();

	return true;
}

/***************************************************************************/
// When will this sound be done?
/***************************************************************************/
uint SoundChannel::GetTimeOfCompletion()
{
	return timeOfCompletion;
}

/***************************************************************************/
// Next time to update. 0 means no updates
/***************************************************************************/
uint SoundChannel::NextUpdate()
{
	return nextUpdate;
}

/***************************************************************************/
// The actual update call
// Does nothing by default
/***************************************************************************/
void SoundChannel::Update(uint now)
{
	// If we autoReclaim'ing AND
	if (autoReclaim)
	{
		// Time to check?
		if (now >= timeOfCompletion && !paused && state == PLAYING && !voice->IsPlaying())
			Kill();
	}
}

/***************************************************************************/
// Access the voice settings
// These only set the variables, they don't access the instance
/***************************************************************************/
void SoundChannel::SetVolume(float newVolume)
{
	volume = Math::Clamp(newVolume);
	ApplyVolume();
}

/***************************************************************************/
/***************************************************************************/
float SoundChannel::GetVolume()
{
	return ComputeVolume();
}

/***************************************************************************/
// Set the wet/dry mix
/***************************************************************************/
void SoundChannel::SetWetDry(
float newWet,
float newDry)
{
	wet = Math::Clamp(newWet);
	dry = Math::Clamp(newDry);
	ApplyMix();
}

/***************************************************************************/
/***************************************************************************/
float SoundChannel::GetWet()
{
	return wet;
}

/***************************************************************************/
/***************************************************************************/
float SoundChannel::GetDry()
{
	return dry;
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel::SetPitch(float newPitch)
{
	pitch = newPitch;
	ApplyPitch();
}

/***************************************************************************/
// Access the handle
/***************************************************************************/
void SoundChannel::SetHandle(SoundHandle newHandle)
{
	handle = newHandle;
}

/***************************************************************************/
/***************************************************************************/
SoundHandle SoundChannel::GetHandle()
{
	return handle;
}

/***************************************************************************/
// Play the sound
/***************************************************************************/
bool SoundChannel::Play(
uint now)
{
	// Doh, if they lost the sound, it's gone
	if (state == DEAD || !voice)
	{
		nextUpdate = 0;
		return false;
	}

	// Restart if already playing
	if (state == PLAYING)
		Stop();

	// Start the sound playing
	state = PLAYING;
	ApplySettings();

	// Compute the time of completion
	timeOfCompletion = now + voice->PlayTime();
	if (autoReclaim)
		nextUpdate = timeOfCompletion;

	// Start it playing!
	voice->Play(Looped());

	return true;
}

/***************************************************************************/
// Stop the sound
/***************************************************************************/
void SoundChannel::Stop()
{
	// Signify a stop
	if (state == PLAYING)
	{
		if (voice)
 			voice->Stop();
		state = STOPPED;
	}
}

/***************************************************************************/
// Handle pause/unpause
/***************************************************************************/
void SoundChannel::Pause()
{
	if (state == PLAYING)
		voice->Pause();
	paused = true;
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel::Unpause()
{
	if (state == PLAYING)
		voice->Unpause();
	paused = false;
}

/***************************************************************************/
/***************************************************************************/
bool SoundChannel::Paused()
{
	return paused;
}

/***************************************************************************/
// Force this sound's auto-reclaim state
/***************************************************************************/
void SoundChannel::SetAutoReclaim(bool setAutoReclaim)
{
	// Save the new state
	autoReclaim = setAutoReclaim;

	// If they called this and the sound is playing, update the nextUpdate
	if (state == PLAYING && autoReclaim)
		nextUpdate = timeOfCompletion;
}

/***************************************************************************/
/***************************************************************************/
bool SoundChannel::GetAutoReclaim()
{
	return autoReclaim;
}

/***************************************************************************/
// Will this channel restart if it loses it's voice?
/***************************************************************************/
void SoundChannel::SetAutoRestart(
bool setAutoRestart)
{
	autoRestart = setAutoRestart;
}

/***************************************************************************/
/***************************************************************************/
bool SoundChannel::GetAutoRestart()
{
	return autoRestart;
}

/***************************************************************************/
// Set the auto restart delay
/***************************************************************************/
void SoundChannel::SetAutoRestartDelay(
uint delayMS)
{
	autoRestartDelay = delayMS;
}

/***************************************************************************/
/***************************************************************************/
uint SoundChannel::GetAutoRestartDelay()
{
	return autoRestartDelay;
}

/***************************************************************************/
// Lock/unlock this channel.  Locking will prevent voice from
// ever being reclaimed
/***************************************************************************/
void SoundChannel::Lock()
{
	++lock;
}

/***************************************************************************/
/***************************************************************************/
bool SoundChannel::Unlock()
{
	--lock;
	ASSERT(lock >= 0);

	return (lock > 0);
}

/***************************************************************************/
/***************************************************************************/
bool SoundChannel::Locked()
{
	return (lock > 0);
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel::InitBankValues()
{
	bankVolume = SoundBank::GetSoundVolumeInRange(&def);
	bankPitch = SoundBank::GetSoundPitchInRange(&def);
}

/***************************************************************************/
// These get called to compute volume and pitch
/***************************************************************************/
float SoundChannel::ComputeVolume()
{
	return Math::Clamp(emitter.GetVolume() * bankVolume * volume);
}

/***************************************************************************/
/***************************************************************************/
float SoundChannel::ComputePitch()
{
	return emitter.GetPitch() + bankPitch + pitch;
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel::ApplyVolume()
{
	if (voice)
		voice->SetVolume(ComputeVolume());
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel::ApplyMix()
{
	if (voice)
	{
		if ( !Math::Equal( voice->GetWet(), wet ) ||
			  !Math::Equal( voice->GetDry(), dry ) )
		{
		   voice->SetWetDry(wet, dry);
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel::ApplyPitch()
{
	if (voice)
		voice->SetPitch(ComputePitch());
}

/***************************************************************************/
// Called during a play to apply everything
// only called if state == PLAYING
/***************************************************************************/
void SoundChannel::ApplySettings()
{
	ApplyVolume();
	ApplyMix();
	ApplyPitch();
}

