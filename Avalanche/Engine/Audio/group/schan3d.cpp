/***************************************************************************/
// SoundChannel3D
/***************************************************************************/
#include "Audio/AudioPCH.h"

#include "math/sfilter.h"

/***************************************************************************/
/***************************************************************************/
SoundChannel3D::SoundChannel3D(
SoundEmitter &soundEmitter,
SoundDef &soundDef) :
SoundChannel(soundEmitter, soundDef)
{
	pos.Clear();
	vel.Clear();
	cookie = NULL;
	watchPos = NULL;
	watchVel = NULL;
	worldPos.Clear();
	worldVel.Clear();

	audibleDistance = 0.0f;

	volume3D = 0.0f;
	pan3D = 0.0f;
	pitch3D = 0.0f;

	// Occlusion support!
	occlusionVolumeFactor = occlusionVolumeFactorTarget = 1.0f;
	occlusionPanFactor = occlusionPanFactorTarget = 1.0f;
	nextOcclusionUpdate = 1;

	// Override these settings
	wet = Emitter3D().GetParams().GetWetMix();
	dry = Emitter3D().GetParams().GetDryMix();

	ambience = 0.0f;
	presence = 0.0f;
}

/***************************************************************************/
// Return we are 3D
/***************************************************************************/
SoundChannel::TYPE SoundChannel3D::Type()
{
	return CHANNEL3D;
}

/***************************************************************************/
// The pos & velocity functions
/***************************************************************************/
void SoundChannel3D::SetPosition(Vector3CRef newPos)
{
	pos = newPos;
	watchPos = NULL;
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel3D::WatchPosition(const Vector3 *newPos)
{
	watchPos = newPos;
	UpdateAndApply();
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel3D::SetVelocity(Vector3CRef newVelocity)
{
	vel = newVelocity;
	watchVel = NULL;
	UpdateAndApply();
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel3D::SetCookie(void *newCookie)
{
	cookie = newCookie;
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel3D::WatchVelocity(const Vector3 *newVelocity)
{
	watchVel = newVelocity;
	UpdateAndApply();
}

/***************************************************************************/
// Detach this sound from all asynchronous memory read upates
/***************************************************************************/
void SoundChannel3D::Detach()
{
	watchPos = NULL;
	watchVel = NULL;
}

/***************************************************************************/
/***************************************************************************/
Vector3CRef SoundChannel3D::Position()
{
	return worldPos;
}

/***************************************************************************/
/***************************************************************************/
Vector3CRef SoundChannel3D::Velocity()
{
	return worldVel;
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel3D::SetAmbience(float newAmbience)
{
	ambience = Math::Clamp(newAmbience);
	UpdateAndApply();
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel3D::SetPresence(float newPresence)
{
	presence = Math::Clamp(newPresence);
	UpdateAndApply();
}

/***************************************************************************/
// Set the audible distance
/***************************************************************************/
void SoundChannel3D::SetAudibleDistance(
float maxAudibleDistance)
{
	audibleDistance = maxAudibleDistance;
	UpdateAndApply();
}

/***************************************************************************/
// Play the sound
/***************************************************************************/
bool SoundChannel3D::Play(uint now)
{
	// set our position for EAX
#ifdef DIRECTX_PC
	if (EAX::usingEAX)
	{
		SoundEmitter3D &emitter3D = Emitter3D();
		// Put sound pos into world space
		Vector3 worldPos = emitter3D.PosToWorld(pos);
		if ( voice->EAXBufferInfo.lpDS3DB8 )
		{
			voice->EAXBufferInfo.lpDS3DB8->SetPosition(worldPos.X(), worldPos.Y(), worldPos.Z(), DS3D_IMMEDIATE);
		}
	}
#endif
	// Actually get it playing
	bool playing = SoundChannel::Play(now);

	// We update faster than normal
	if (playing)
	{
		// Set the next update (stagger it with the rand)
		const uint halfUpdateRate = max(Emitter3D().GetParams().GetUpdateRate() / 2, 1);
		nextUpdate = halfUpdateRate + (rand() % halfUpdateRate);
	}

	return playing;
}

/***************************************************************************/
// Update everything as we might get reclaimed
/***************************************************************************/
bool SoundChannel3D::UpdateForAquire()
{
	// Refresh the values from the bank
	InitBankValues();

	// Is it audible?
	bool audible = Update3D(0);
#if 0//0defined(FOOD) && defined(_DEBUG) && defined(WIN32)
	if (!audible)
	{
		TRACE("Not playing sound \"%s\" because it is not audible (more than %d away).\n", def.name, def.audibleDist);
	}
	else
	{
		TRACE("Playing sound \"%s\"!!!\n", def.name);
	}
#endif
	return audible;
}

/***************************************************************************/
// The update call
/***************************************************************************/
void SoundChannel3D::Update(uint now)
{
	// Do the basic update
	SoundChannel::Update(now);

	// Update our 3D stuff
	if (state != PLAYING)
		nextUpdate = 0;	// Cancel updates (they are expensive)
	else
	{
		// Update and apply
		Update3D(now);
		ApplySettings();
		nextUpdate = now + Emitter3D().GetParams().GetUpdateRate();
	}
}

/***************************************************************************/
// Initialize the bank values
/***************************************************************************/
void SoundChannel3D::InitBankValues()
{
	// Call the parent
	SoundChannel::InitBankValues();

	// Override audible distance?
	if (Math::Zero(audibleDistance))
		audibleDistance = SoundBank::GetSoundAudibleDistance(&def);
}

/***************************************************************************/
// These get called to compute volume and pitch
/***************************************************************************/
float SoundChannel3D::ComputeVolume()
{
	return SoundChannel::ComputeVolume() * volume3D * occlusionVolumeFactor;
}

/***************************************************************************/
/***************************************************************************/
float SoundChannel3D::ComputePitch()
{
	return SoundChannel::ComputePitch() + pitch3D * occlusionPanFactor;
}

/***************************************************************************/
/***************************************************************************/
float SoundChannel3D::ComputeAmbience()
{
	return Math::Clamp(ambience + SoundBank::GetSoundAmbience(&def));
}

/***************************************************************************/
/***************************************************************************/
float SoundChannel3D::ComputePresence()
{
	return Math::Clamp(presence + SoundBank::GetSoundPresence(&def));
}

/***************************************************************************/
// Someone is trying to change the volume of this sound...
/***************************************************************************/
void SoundChannel3D::ApplyVolume()
{
	UpdateAndApply();
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel3D::ApplyPitch()
{
	UpdateAndApply();
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel3D::ApplyPan()
{
	// But do the pan too!
	if (voice)
		voice->SetPan(pan3D);
}

/***************************************************************************/
// Called during a play to apply everything
// only called if state == PLAYING
/***************************************************************************/
void SoundChannel3D::ApplySettings()
{
	if (voice)
		voice->SetVolumeAndPan(ComputeVolume(), pan3D);
	SoundChannel::ApplyMix();
	SoundChannel::ApplyPitch();
}

/***************************************************************************/
// The actual update3d call
/***************************************************************************/
bool SoundChannel3D::Update3D(
uint now)
{
	// Alias the emitter
	SoundEmitter3D &emitter3D = Emitter3D();

	// Alias this for convienice
	const SoundEmitter3DParams &params = emitter3D.params;

	// Auto detach this sound if need be
	if (!params.VerifyCookie(cookie))
	{
		Detach();
		cookie = NULL;
	}
	else
	{
		// Update our position and velocity
		if (watchPos)
			pos = *watchPos;
		if (watchVel)
			vel = *watchVel;
	}

	// Put into world space
	worldPos = emitter3D.PosToWorld(pos);
	worldVel = emitter3D.VelToWorld(vel);

	// update EAX world pos information
#ifdef DIRECTX_PC
	if (EAX::usingEAX)
	{
		if ( voice )
			if (voice->EAXBufferInfo.lpDS3DB8 )
				voice->EAXBufferInfo.lpDS3DB8->SetPosition(worldPos.X(), worldPos.Y(), worldPos.Z(), DS3D_IMMEDIATE);
	}
#endif
	// get our best listener
	SoundListener &listener = emitter3D.driver.BestListener(worldPos);
	if (&listener == NULL)
		return false;

	// Figure out our D range
	float maxD;
	if (Math::Zero(audibleDistance))
		maxD = params.AudibleDistance();
	else
		maxD = audibleDistance;
	float minD = maxD * ComputePresence();

	// Compute the pan factor using ambience
	float panFactor = params.LeftRightPanFactor() * (1.0f - ComputeAmbience());

	// Update the volume and pan
	bool audible = PositionalSound::ComputePanVolume(listener.WorldToListener(), worldPos,
																		pan3D, volume3D,
																		panFactor, minD, maxD);

	// Check to see if this sound is occluded
	SoundOcclusionTest *occlusionTest = params.GetOcclusionTest();
	if (occlusionTest)
	{
		// Time to make the call?
		if (audible && (now == 0 || now > nextOcclusionUpdate))
		{
			// Make the call
			occlusionTest->Test(worldPos, listener.Position(), occlusionVolumeFactor, occlusionPanFactor);

			// Update for the next call
			uint rateMS = Looped() ? params.LoopOcclusionTestMS() : params.OneShotOcclusionTestMS();
			if (rateMS != 0)
				nextOcclusionUpdate = now + rateMS;
		}

		// Update the stuff
		const float dt = g_timer.GetFrameSec();
		occlusionVolumeFactor = FOFilterImplicit(occlusionVolumeFactor, occlusionVolumeFactorTarget, dt, params.VolumeOcclusionFilter());
		occlusionPanFactor = FOFilterImplicit(occlusionPanFactor, occlusionPanFactorTarget, dt, params.PanOcclusionFilter());

		// Update the audible flag
		audible = !Math::Zero(occlusionVolumeFactor);
	}

	// Compute the pitch
	if (!params.DopplerEnabled() || !voice || !audible)
		pitch3D = 0.0f;
	else
	{
		// Need to know the base frequency
		float baseFrequency = (float)voice->Parent()->Attr().SampleRate();

		// compute the frequency shift
		float f = Doppler::Compute(baseFrequency,
											worldPos, worldVel,
											listener.Position(), listener.Velocity(),
											params.SpeedOfSound());

		// convert to a pitch shift
		pitch3D = SemitoneMath::SemitoneDifference(baseFrequency, f);
	}

	return audible;
}

/***************************************************************************/
/***************************************************************************/
void SoundChannel3D::UpdateAndApply()
{
	if (state == PLAYING)
	{
		Update3D(0);
		ApplySettings();
	}
}

/***************************************************************************/
/***************************************************************************/
SoundEmitter3D &SoundChannel3D::Emitter3D()
{
	// Upcast our emitter
	ASSERT(emitter.Type() == SoundEmitter::EMITTER3D);

	// Put these into world space
	return (SoundEmitter3D &)emitter;
}
