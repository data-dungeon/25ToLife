/***************************************************************************/
// Sound emitter for environmental stuff
/***************************************************************************/
#include "Audio/AudioPCH.h"
#include "Math/fastrand.h"

/***************************************************************************/
	// Don't create this is this returns false!
/***************************************************************************/
bool EnvSourceSound::Valid(
SoundDriver &driver,
const char *name,
EnvSoundSourceType type)
{
	bool valid = false;
	bool validSFXSource, validStreamSource;
	switch (type)
	{
		case ESST_SFX:
		case ESST_SFXDIR:
			{
				SoundBank *bank = NULL;
				validSFXSource = (driver.FindSoundByName(name, &bank) != NULL);
			}
#ifndef PS2
			ASSERTS(validSFXSource, name);
#endif
			valid = validSFXSource;
			break;

		case ESST_STREAMLOOPED:
		case ESST_STREAM:
			validStreamSource = driver.StreamExist(name);
#ifndef PS2
			ASSERTS(validStreamSource, name);
#endif
			valid = validStreamSource;
			break;

		default:
			// unsupported type
			ASSERT(false);
			valid = false;
			break;
	}

	return valid;
}

/***************************************************************************/
// For SFX
/***************************************************************************/
EnvSourceSound::EnvSourceSound(
const char *name,
EnvSoundSourceType type)
{
	// Duh, roger over and out
	ASSERT(name != NULL);
	ASSERT(*name != '\0');

	// Save the name
	ASSERT(strlen(name) < sizeof(m_name));
	strncpy(m_name, name, sizeof(m_name) - 1);
	m_name[sizeof(m_name) - 1] = '\0';

	m_looped = false; // we don't really know this
	switch (type)
	{
		case ESST_SFX:
		case ESST_SFXDIR:
			m_type = ESS_SFX;
			break;

		case ESST_STREAMLOOPED:
			m_looped = true;
		case ESST_STREAM:
			m_type = ESS_STREAM;
			break;

		default:
			// unsupported type
			ASSERT(false);
			break;
	}

	m_handle = NULL;
	m_stream = NULL;
}

/***************************************************************************/
/***************************************************************************/
EnvSourceSound::~EnvSourceSound()
{
	Stop();
}

/***************************************************************************/
/***************************************************************************/
bool EnvSourceSound::Play(
SoundDriver &driver,
SoundEmitter3D &emitter,
uint now,
const Vector3 &pos,
const Vector3 *velocity,
float volume,
float pitch,
float ambience,
float presence,
float audibleDistance)
{
	ASSERT(!IsPlaying());
	if (IsPlaying())
		return false;

	switch (m_type)
	{
		case ESS_SFX:
			// Cue the sound
#if 0//defined(FOOD) && defined(_DEBUG)
			TRACE("Env: Starting SFX %s at [%.1f %.1f %.1f] v %.1f p %.1f a %.1f pr %.1f\n", m_name, (float)pos.X(), (float)pos.Y(), (float)pos.Z(),
																			volume, pitch, ambience, presence);
#endif
			m_handle = emitter.Cue(m_name);
			if (m_handle)
			{
				emitter.SetVolume(m_handle, volume);
				emitter.SetPitch(m_handle, pitch);
				emitter.SetAmbience(m_handle, ambience);
				emitter.SetPresence(m_handle, presence);
				emitter.SetPosition(m_handle, pos);
				if (velocity)
					emitter.SetVelocity(m_handle, *velocity);
				if (!Math::Zero(audibleDistance))
					emitter.SetAudibleDistance(m_handle, audibleDistance);
				emitter.Play(m_handle);

				// Only hold the m_handle if it is looped
				m_looped = emitter.Looped(m_handle);
				if (!m_looped)
				{
					delete m_handle;
					m_handle = NULL;
				}
			}
			break;

		case ESS_STREAM:
			delete m_stream;
#if defined(FOOD) && defined(_DEBUG)
			TRACE("Env: Starting Stream %s at [%.1f %.1f %.1f] v %.1f p %.1f a %.1f pr %.1f\n", m_name, (float)pos.X(), (float)pos.Y(), (float)pos.Z(),
																			volume, pitch, ambience, presence);
#endif
			m_stream = driver.CreateStream3D(m_name, m_looped, SoundStream3D::STREAM3D_PRIORITY_NORMAL);
			if (m_stream)
			{
				m_stream->SetVolume(volume);
				m_stream->SetPitch(pitch);
				m_stream->SetAmbience(ambience);
				m_stream->SetPresence(presence);
				m_stream->SetPosition(pos);
				if (velocity)
					m_stream->SetVelocity(*velocity);
				if (!Math::Zero(audibleDistance))
					m_stream->SetAudibleDistance(audibleDistance);
				m_stream->Play();
			}
			break;

		default:
			ASSERT(false);
			break;
	}

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool EnvSourceSound::Stop()
{
	switch (m_type)
	{
		case ESS_SFX:
			if (m_handle)
			{
				// Get the emitter from the handle
				SoundChannel *channel = m_handle->Channel();
				if (channel)
				{
					SoundEmitter &emitter = channel->Emitter();
					emitter.Kill(m_handle);
				}
				else
				{
					// delete the handle
					delete m_handle;
					m_handle = NULL;
				}
			}
			break;

		case ESS_STREAM:
			delete m_stream;
			m_stream = NULL;
			break;

		default:
			ASSERT(false);
			break;
	}

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool EnvSourceSound::IsPlaying()
{
	switch (m_type)
	{
		case ESS_SFX:
			return (m_handle != NULL);

		case ESS_STREAM:
			return (m_stream ? m_stream->IsPlaying() : false);
			break;

		default:
			ASSERT(false);
			break;
	}

	return false;
}

/***************************************************************************/
/***************************************************************************/
EnvSource::EnvSource(
const char *name,
EnvSoundSourceType type) :
sound(name, type)
{
	// Assume we are active
	active = true;
}

/***************************************************************************/
/***************************************************************************/
EnvSource::~EnvSource()
{
	Stop();
}

/***************************************************************************/
// Play control
/***************************************************************************/
void EnvSource::Stop()
{
	// get the sound to do the work
	sound.Stop();
}

/***************************************************************************/
/***************************************************************************/
EnvSourceStatic::EnvSourceStatic(
int id,
const char *name,
EnvSoundSourceType type,
Vector3CRef pos,
float audibleRange,
float baseVolume,
float basePitch,
float baseAmbience,
float basePresence,
int minTime,
int maxTime) :
EnvSource(name, type),
ZoneSphere(id, pos, audibleRange)
{
	// Sound settings
	volume = baseVolume;
	pitch = basePitch;
	ambience = baseAmbience;
	presence = basePresence;

	// Setup the retrigger stuff
	minTriggerTime = minTime;
	maxTriggerTime = maxTime;

	if (HasTrigger())
	{
		TRACE("Found a min/max trigger time for source %s [%d %d]\n", name, minTriggerTime, maxTriggerTime);
	}

	nextPlay = 0;
}

/***************************************************************************/
// Is this sound ready to play?
// May cause an update if it expires
// Returns 0 if always needs to play
/***************************************************************************/
bool EnvSourceStatic::ReadyToPlay(
uint now)
{
	// Active sounds aren't ready to play
	if (!Active())
		return false;

	// Have we expired?
	if (HasTrigger() && now > nextPlay + maxTriggerTime * 2)
	{
		nextPlay = now + g_random.UpTo(maxTriggerTime);
		return false;
	}

	// We are ready to play
	return now >= nextPlay;
}

/***************************************************************************/
// Play control
/***************************************************************************/
void EnvSourceStatic::Play(
SoundDriver &driver,
SoundEmitter3D &emitter,
uint now)
{
	if (Playing())
		Stop();

	// Play it
	sound.Play(driver, emitter, now, center, NULL, volume, pitch, ambience, presence, radius);

	// This was needed to find unknown sounds on PC builds
#if defined (WIN32) && !defined (_XBOX)
	ASSERTS(sound.Looped() || HasTrigger(), sound.Name());
#else
	ASSERTS(sound.Looped() || HasTrigger(), "Unknown sound name");
#endif //WIN32
	if (!sound.Looped() && !HasTrigger())
	{
		// This is bad, setup some defaults
		minTriggerTime = 5000;
		maxTriggerTime = 10000;
		TRACE("Set a min/max trigger time for unlooped source %s [%d %d]\n", Name(), minTriggerTime, maxTriggerTime);
	}

	// Regenerate the trigger
	if (HasTrigger())
		nextPlay = now + g_random.InRange(minTriggerTime, maxTriggerTime);
}

/***************************************************************************/
// A procedural trigger sound source
/***************************************************************************/
EnvSourceTrigger::EnvSourceTrigger(
const char *name,
EnvSoundSourceType type,
EnvSoundTrigger *theTrigger) :
EnvSource(name, type)
{
	ASSERT(theTrigger);
	trigger = theTrigger;
}

/***************************************************************************/
/***************************************************************************/
EnvSourceTrigger::~EnvSourceTrigger()
{
	delete trigger;
}

/***************************************************************************/
// Update
/***************************************************************************/
void EnvSourceTrigger::Update(
SoundDriver &driver,
SoundEmitter3D &emitter,
uint now)
{
	// Are we active?
	if (!Active())
		return;

	// Pick a random listener
	int l = rand() % driver.Listeners();
	SoundListener *listener = driver.Listener(l);
	ASSERT(listener);

	// Time to trigger?
	if (!trigger->Trigger(*listener, now, event))
	{
		 // Already playing, play until the trigger returned false
		if (Playing())
			Stop();
	}
	else
	{
		// Fire it up
		if (!Playing())
			sound.Play(driver, emitter, now, event.pos, &event.velocity, event.volume, event.pitch, event.ambience, event.presence);
	}
}






















