/***************************************************************************/
// SoundChannel3D
/***************************************************************************/
#ifndef SCHAN3D_H
#define SCHAN3D_H

#include "Audio/group/schannel.h"

class SoundEmitter3D;

// The 3D version adds a lot of data
class SoundChannel3D : public SoundChannel
{
public:
	SoundChannel3D(SoundEmitter &soundEmitter, SoundDef &soundDef);

	// Return we are 3D
	TYPE Type();

	// The pos & velocity functions
	void SetPosition(Vector3CRef newPosition);
	void SetVelocity(Vector3CRef newVelocity);

	void SetCookie(void *newCookie);
	void WatchPosition(const Vector3 *newPosition);
	void WatchVelocity(const Vector3 *newVelocity);

	// Detach this sound from all asynchronous memory read upates
	void Detach();

	// Return the current pos/velocity
	Vector3CRef Position();
	Vector3CRef Velocity();

	// Ambience and presence
	void SetAmbience(float newAmbience);
	void SetPresence(float newPresence);

	// Set the audible distance
	void SetAudibleDistance(float maxAudibleDistance);

	// Play the sound
	bool Play(uint now);

	// Update everything as we might get reclaimed
	// Return false if this channel doesn't deserve a hardware voice
	bool UpdateForAquire();

	// The update call
	void Update(uint now);

private:
	// The stuff we are tracking
	Vector3			pos;
	Vector3			vel;
	void				*cookie;
	const Vector3	*watchPos;
	const Vector3	*watchVel;
	Vector3			worldPos;
	Vector3			worldVel;

	// Ambience and presence settings
	float 			ambience;
	float				presence;

	// The audible distance for this sound
	float 			audibleDistance;

	// The 3D values we compute
	float				volume3D;
	float				pan3D;
	float				pitch3D;

	// The occlusion stuff
	float				occlusionVolumeFactor;
	float				occlusionPanFactor;
	float				occlusionVolumeFactorTarget;
	float				occlusionPanFactorTarget;
	uint				nextOcclusionUpdate;

	// Initialize the bank values
	void InitBankValues();

	// These get called to compute volume and pitch
	float ComputeVolume();
	float ComputePitch();
	float ComputeAmbience();
	float ComputePresence();
	void ApplyVolume();
	void ApplyPitch();
	void ApplyPan();

	// Called during a play to apply everything
	// only called if state == PLAYING
	void ApplySettings();

	// Compute 3D values...can be slow so don't overdo it
	// Returns true if audible (pass 0 for now to force!)
	bool Update3D(uint now);

	// Apply settings, but only if playing
	void UpdateAndApply();

	// Utility to get at a 3D emitter
	SoundEmitter3D &Emitter3D();
};

#endif
