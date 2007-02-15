///////////////////////////////////////////////////////////////////////////////
// SoundStream, a disk or i/o based sound
// Abstract base class
///////////////////////////////////////////////////////////////////////////////
#ifndef SSTREAM3D_H
#define SSTREAM3D_H

#include "Math/Vector.h"
#include "Audio/Emitter/e3dparam.h"

class SoundDriver;
class SoundStream;
class Fader;

class SoundStream3D
{
public:
	// Setup global defaults for sounds
	static void SetMasterPitch(float masterPitch);
	static float GetMasterPitch();

	enum Priority
	{
		STREAM3D_PRIORITY_LOW,
		STREAM3D_PRIORITY_NORMAL,
		STREAM3D_PRIORITY_HIGH
	};

	// Only the driver should create us. We own this stream
	// and will call delete on it when done
	SoundStream3D(SoundDriver &driver,
						const char *name,
						bool loop = false,
						Priority priority = STREAM3D_PRIORITY_NORMAL);
	~SoundStream3D();

	// Get our name
	const char *Name()
		{ return m_name; }

	// Are we looping
	bool Looping()
		{ return m_loop; }

	// What is our priority
	Priority GetPriority()
		{ return	m_priority; }

	// Set up our basic 3D playback information
	void SetParams(SoundEmitter3DParams &params)
		{ m_params = params; }
	SoundEmitter3DParams &GetParams()
		{ return m_params; }

	// Set the position of this sound
	bool SetPosition(Vector3CRef pos);

	// Watch this position vector
	// Will read from ptr ASYNCHORNOUSLY from your sound calls.
	bool WatchPosition(const Vector3 *pos);

	// Set the velocity of this sound
	bool SetVelocity(Vector3CRef velocity);

	// Watch the velocity of this sound
	// Will read from ptr ASYNCHORNOUSLY from your sound calls.
	bool WatchVelocity(const Vector3 *velocity);

	// Stop watching pos/velocity
	bool Detach();

	// Assign a fader to this stream (there can be only one!)
	void BindFader(Fader *fader)
		{ m_fader = fader; }
	void ClearFader()
		{ BindFader(NULL); }
	Fader *GetFader()
		{ return m_fader; }

	// Set the volume [0..1], 0.0 is silent 1.0 is full volume
	bool SetVolume(float volume);
	float GetVolume()
		{ return m_volume; }

	// Set the min volume factor [0..1]
	bool SetMinVolume(float volume);
	float GetMinVolume()
		{ return m_minVolume; }

	// Set the ambience [0..1], returns false if sound has expired
	bool SetAmbience(float ambience);

	// Set the presence [0..1], returns false if sound has expired
	bool SetPresence(float presence);

	// Set the audible distance in meters (0 means default)
	bool SetAudibleDistance(float audibleDistance);

	bool SetPitch(float shift);
	float GetPitch()
		{ return m_pitch; }

	// Set the mix for this sound stream
	bool SetWetDry(float wet, float dry);
	float GetWet()
		{ return m_wet; }
	float GetDry()
		{ return m_dry; }

	// Playback control
	bool Play();
	bool Stop(bool rewind = false);
  	bool IsPlaying();

	int CurrentPlayPos();

	// Pause
	bool Pause();
	bool Unpause();
	bool Paused();

	// Get the error
	t_Error StreamError();

	// Is the HW stream lost?
	bool IsLost()
		{ return !m_stream; }

	// Reload the hw stream
	bool Reload(bool restorePos = true);

	// Release the stream
	bool Release();

	// Update our stuff
	void Update();

	// What is our actual volume (ignores the fact if we have a stream or not)
	float ComputeVolume();

protected:
	// The master
	SoundDriver				&m_driver;

	// Our name
	char 						m_name[64];

	// Are we not men? We are devo.
	bool						m_loop;

	// Our priority
	Priority					m_priority;

	// Our stream!
	SoundStream				*m_stream;
	int						m_streamPos;

	// Our basic paramsters
	SoundEmitter3DParams	m_params;

	// Our state in the world
	const Vector3			*m_watchPos;
	Vector3					m_pos;
	const Vector3			*m_watchVel;
	Vector3					m_vel;

	// Overrides for this stuff
	Fader						*m_fader;
	float						m_volume;
	float						m_minVolume;
	float						m_pitch;
	float						m_ambience;
	float						m_presence;
	float						m_audibleDistance;
	float						m_wet, m_dry;

	// Occlusion support
	float						m_occlusionVolumeFactor;
	float						m_occlusionPanFactor;
	float						m_occlusionVolumeFactorTarget;
	float						m_occlusionPanFactorTarget;
	uint 						m_nextOcclusionUpdate;

 	// Global overrides
	static float			m_masterPitch;

public:
	// We are in a list
	SoundStream3D			*prev, *next;
};

#endif
