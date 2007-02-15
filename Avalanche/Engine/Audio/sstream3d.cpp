///////////////////////////////////////////////////////////////////////////////
// SoundStream, a disk or i/o based sound
// Abstract base class
///////////////////////////////////////////////////////////////////////////////
#include "Audio/AudioPCH.h"

#include "math/sfilter.h"
#include "Layers/Media.h"

// Overrides
float SoundStream3D::m_masterPitch = 0.0f;

///////////////////////////////////////////////////////////////////////////////
// Set the overall pitch for all sounds
///////////////////////////////////////////////////////////////////////////////
void SoundStream3D::SetMasterPitch(
float pitch)
{
	m_masterPitch = pitch;
}

///////////////////////////////////////////////////////////////////////////////
// Get the overall pitch for all sound
///////////////////////////////////////////////////////////////////////////////
float SoundStream3D::GetMasterPitch()
{
	return m_masterPitch;
}

///////////////////////////////////////////////////////////////////////////////
// Only the driver should create us
///////////////////////////////////////////////////////////////////////////////
SoundStream3D::SoundStream3D(
SoundDriver &driver,
const char *name,
bool loop,
Priority priority) :
m_driver(driver)
{
	ASSERT(strlen(name) < sizeof(m_name));
	strncpy(m_name, name, sizeof(m_name) - 1);
	m_name[sizeof(m_name) - 1] = '\0';

	m_loop = loop;
	m_priority = priority;

	m_stream = NULL;
	m_streamPos = 0;

	m_watchPos = NULL;
	m_pos.Clear();
	m_watchVel = NULL;
	m_vel.Clear();

	m_fader = NULL;
	m_volume = 1.0f;
	m_minVolume = 0.0f;
	m_pitch = 0.0f;
	m_ambience = 0.0f;
	m_presence = 0.0f;
	m_wet = m_dry = 1.0f;
	m_audibleDistance = 0.0f;

	m_occlusionVolumeFactor = m_occlusionVolumeFactorTarget = 1.0f;
	m_occlusionPanFactor = m_occlusionPanFactorTarget = 1.0f;
	m_nextOcclusionUpdate = 0;
}

///////////////////////////////////////////////////////////////////////////////
// The driver must create the sound, but it can be deleted directly
///////////////////////////////////////////////////////////////////////////////
SoundStream3D::~SoundStream3D()
{
	// Clean up
	Release();
	m_driver.Remove3DStream(this);
}

///////////////////////////////////////////////////////////////////////////////
// Set the position of this sound
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::SetPosition(
Vector3CRef pos)
{
	m_watchPos = NULL;
	m_pos = pos;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Watch this position vector
// Will read from ptr ASYNCHORNOUSLY from your sound calls.
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::WatchPosition(
const Vector3 *pos)
{
	m_watchPos = pos;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Set the velocity of this sound
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::SetVelocity(
Vector3CRef velocity)
{
	m_watchVel = NULL;
	m_vel = velocity;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Watch the velocity of this sound
// Will read from ptr ASYNCHORNOUSLY from your sound calls.
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::WatchVelocity(
const Vector3 *velocity)
{
	m_watchVel = velocity;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::Detach()
{
	if (m_watchPos)
	{
	  	m_pos = *m_watchPos;
		m_watchPos = NULL;
	}
	if (m_watchVel)
	{
		m_vel = *m_watchVel;
		m_watchVel = NULL;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Set the volume [0..1], 0.0 is silent 1.0 is full volume
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::SetVolume(float volume)
{
	m_volume = Math::Clamp(volume);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Set the min volume factor [0..1]
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::SetMinVolume(float volume)
{
	m_minVolume = Math::Clamp(volume);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Set the ambience [0..1], returns false if sound has expired
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::SetAmbience(
float ambience)
{
	m_ambience = Math::Clamp(ambience);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Set the presence [0..1], returns false if sound has expired
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::SetPresence(
float presence)
{
	m_presence = Math::Clamp(presence);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Set the audible distance in meters (0 means default)
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::SetAudibleDistance(
float audibleDistance)
{
	m_audibleDistance = audibleDistance;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::SetPitch(
float shift)
{
	m_pitch = shift;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::SetWetDry(
float wet,
float dry)
{
	m_wet = wet;
	m_dry = dry;
	return m_stream ? m_stream->SetWetDry(m_wet, m_dry) : true;
}

///////////////////////////////////////////////////////////////////////////////
// Playback control
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::Play()
{
	// Make sure we have a stream
	Reload();

	// No stream, abort!
	if (!m_stream)
		return false;

	// Reset occlusions stuff
	m_occlusionVolumeFactor = m_occlusionVolumeFactorTarget = 1.0f;
	m_occlusionPanFactor = m_occlusionPanFactorTarget = 1.0f;
	m_nextOcclusionUpdate = 0;

	// Update all the info
	Update();

	// Play it
	return m_stream->Play();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::Stop(
bool rewind)
{
	if (!m_stream)
		return false;
	m_streamPos = rewind ? 0 : m_stream->CurrentPlayPos();
	return m_stream->Stop(rewind);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::IsPlaying()
{
	return (m_stream && m_stream->IsPlaying());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int SoundStream3D::CurrentPlayPos()
{
	return (m_stream ? m_stream->CurrentPlayPos() : -1);
}

///////////////////////////////////////////////////////////////////////////////
// Reload the hw stream
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::Reload(
bool restorePos)
{
	// Do we already have the stream?
	if (m_stream)
		return true;

	// Can we get a voice?
	if (!m_driver.Allocate3DStream(m_priority, ComputeVolume()))
		return false;

	// We are allowed a voice, so make it
	// Load the stream normally
	m_stream = m_driver.CreateStream(m_name, m_loop, false);
	if (!m_stream)
		return false;

	// Are we trying to restore our position?
	if (restorePos && m_streamPos > 0)
		m_stream->Seek(m_streamPos);

	// Default is not wet, so we must turn it on
	m_stream->SetWetDry(m_wet, m_dry);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Pause
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::Pause()
{
	// Write this you knob
	return (m_stream && m_stream->Pause());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::Unpause()
{
	// Write this you knob
	return (m_stream && m_stream->Unpause());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::Paused()
{
	// Write this you knob
	return (m_stream ? m_stream->Paused() : true);
}

///////////////////////////////////////////////////////////////////////////////
// Get the error
///////////////////////////////////////////////////////////////////////////////
t_Error SoundStream3D::StreamError()
{
	return (m_stream ? m_stream->StreamError() : MED_ERR_NONE);
}

///////////////////////////////////////////////////////////////////////////////
// Release the stream
///////////////////////////////////////////////////////////////////////////////
bool SoundStream3D::Release()
{
	// nothing to do here
	if (!m_stream)
		return false;

	// Tell the driver
	m_driver.Free3DStream(this);

	// We got rid of it!
	delete m_stream;
	m_stream = NULL;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Update our stuff
///////////////////////////////////////////////////////////////////////////////
void SoundStream3D::Update()
{
	// Anything to do here?
	if (!m_stream || m_stream->Paused())
		return;

	// Save our play position
	m_streamPos = Math::Max(m_stream->CurrentPlayPos(), 0);

	// Copy in the stuff
	if (m_watchPos)
		m_pos = *m_watchPos;
	if (m_watchVel)
		m_vel = *m_watchVel;

	// Debugging
#if defined(FOOD) && defined(_DEBUG) && defined(WIN32)
	DrawUtility::Sphere(m_pos, DrawUtility::WHITE, Math::Abs(Math::Sin(g_timer.GetEndSec() * 2.0f) * 1.2f));
#endif

	// get our best listener
	SoundListener &listener = m_driver.BestListener(m_pos);
	if (&listener == NULL)
		return;

	// Figure out our D range
	float maxD = Math::Zero(m_audibleDistance) ? m_params.AudibleDistance() : m_audibleDistance;
	float minD = maxD * m_presence;

	// Compute the pan factor using ambience
	float panFactor = m_params.LeftRightPanFactor() * (1.0f - m_ambience);

	// no pitch yet
	float pitch3D = 0.0f;

	// Update the volume and pan
	float pan3D, volume3D;
	bool audible = PositionalSound::ComputePanVolume(listener.WorldToListener(), m_pos,
																		pan3D, volume3D,
																		panFactor, minD, maxD);

	// Check to see if this sound is occluded
	SoundOcclusionTest *occlusionTest = m_params.GetOcclusionTest();
	if (occlusionTest)
	{
		if (audible)
		{
			// Time to make the call?
			uint now = m_driver.Time();
			if (now == 0 || now > m_nextOcclusionUpdate)
			{
				// Make the call
				occlusionTest->Test(m_pos, listener.Position(), m_occlusionVolumeFactorTarget, m_occlusionPanFactorTarget);

				// Update for the next call
				uint rateMS = m_stream->Looping() ? m_params.LoopOcclusionTestMS() : m_params.OneShotOcclusionTestMS();
				if (rateMS != 0)
					m_nextOcclusionUpdate = now + rateMS;
			}
		}

		// Update the stuff
		const float dt = g_timer.GetFrameSec();
		m_occlusionVolumeFactor = FOFilterImplicit(m_occlusionVolumeFactor, m_occlusionVolumeFactorTarget, dt, m_params.VolumeOcclusionFilter());
		m_occlusionPanFactor = FOFilterImplicit(m_occlusionPanFactor, m_occlusionPanFactorTarget, dt, m_params.PanOcclusionFilter());
		audible = !Math::Zero(m_occlusionVolumeFactor);
	}

	// Compute the pitch
	if (!m_params.DopplerEnabled() || !audible)
		pitch3D = 0.0f;
	else
	{
		// Need to know the base frequency
		float baseFrequency = (float)m_stream->Attr().SampleRate();

		// compute the frequency shift
		float f = Doppler::Compute(baseFrequency,
											m_pos, m_vel,
											listener.Position(), listener.Velocity(),
											m_params.SpeedOfSound());

		// convert to a pitch shift
		pitch3D = SemitoneMath::SemitoneDifference(baseFrequency, f);
	}

	// Apply it!
	float pan = pan3D * m_occlusionPanFactor;
	m_stream->SetPan(pan);

	float volume = m_volume * volume3D * m_occlusionVolumeFactor;
	if (m_fader)
		volume *= m_fader->MixVolume();
	m_stream->SetVolume(Math::Max(volume, m_minVolume * m_volume));
	m_stream->SetPitch(pitch3D + m_pitch + m_masterPitch);
}

///////////////////////////////////////////////////////////////////////////////
// What is our actual volume
///////////////////////////////////////////////////////////////////////////////
float SoundStream3D::ComputeVolume()
{
	// Copy in the stuff
	if (m_watchPos)
		m_pos = *m_watchPos;
	if (m_watchVel)
		m_vel = *m_watchVel;

	// get our best listener
	SoundListener &listener = m_driver.BestListener(m_pos);
	if (&listener == NULL)
		return 0.0f;

	// Figure out our D range
	float maxD = Math::Zero(m_audibleDistance) ? m_params.AudibleDistance() : m_audibleDistance;
	float minD = maxD * m_presence;

	// Compute the pan factor using ambience
	float panFactor = m_params.LeftRightPanFactor() * (1.0f - m_ambience);

	// Update the volume and pan
	float pan3D, volume3D = 0.0f;
	PositionalSound::ComputePanVolume(listener.WorldToListener(), m_pos,
													pan3D, volume3D,
													panFactor, minD, maxD);
	return volume3D;
}

