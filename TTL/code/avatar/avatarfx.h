///////////////////////////////////////////////////////////////////////////////
// Avatar Special Effects
///////////////////////////////////////////////////////////////////////////////
#ifndef AVATARFX_H
#define AVATARFX_H

#include "Math/Vector.h"
#include "EngineHelper/Singleton.h"
#include "Environ/Munition.h"
#include "components/affliction.h"
#include "effects/fsdfx.h"

class CActor;
class ControlMapper;

class AvatarFX
{
public:
	// Stuff for the stuff
	AvatarFX();
	~AvatarFX();

	// Intialize stuff
	void Setup();

	// Call this once per frame
	void StartFrame();
	void EndFrame();

	// Reset all effects
	void Reset();

	// Remove the cached pointer to our controller
	void ClearController();

	// Tell me what is going on?
	void NotifyExplosion(Vector3CRef explosionPos);
	void NotifyWeaponFire(CActor &who, float duration, float intensity);
	void NotifyWeaponHit(CActor &who, const CMunitionImpact &hit);
	void NotifyDeath(CActor &who);

	// Get some aim modification! (pass in x/y from avatarcontrolmap)
	bool AimModify(float &x, float &y);

private:
	// Who are we?
	CActor			*m_currentActor;
	ControlMapper 	*m_controller;
	float				m_health;

	//////////////////////////////////
	// The utilties that make it work!
	//////////////////////////////////
	struct BlurSetting
	{
		float blur, trip, filter, duration;
	};

	// Stuff for motion blur
	struct Blur
	{
		bool				haveRequest;
		BlurSetting		request;
		BlurSetting		target;
		BlurSetting		base;
		BlurSetting		current;
	};
	Blur m_blur;
	void ClearBlurRequest();
	void RequestBlur(float blur, float trip, float filter, float duration);
	void SetBaseBlur(float blur, float trip);
	void UpdateBlur(float deltaT, bool forceNoBlur);
	void ResetBlur();

	// Vibration stuff
	struct Vibrate
	{
		uint	duration;
		uint	intensity;
	};
	bool		m_vibrateRequest;
	Vibrate	m_vibrate;
	void ClearVibrateRequest();
	void RequestVibrate(uint duration, uint intensity);
	void UpdateVibrate();
	void ResetVibrate();

	// Stuff for heartbeat
	struct Beat
	{
		float		when;
		uint		duration;
		uint		intensity;
		float		hudPulse;
	};
	Beat	m_beatQueue[2];
	int	m_beatQueueTop;
	float	m_lastBeat;
	float	m_beatRate;
	void SetHeartBeatRate(float rate);
	void RequestHeartBeat(float now);
	void UpdateHeartBeat(float time);
	void ResetHeartBeat();

	struct CameraShake
	{
		float 	duration;
		float		amplitude;
		float		frequency;
	};
	bool m_requestCameraShake;
	CameraShake	m_cameraShake;
	void ClearCameraShakeRequest();
	void RequestCameraShake(float duration, float amplitude, float frequency);
	void UpdateCameraShake();
	void ResetCameraShake();

	// Do we care about this dude?
	bool CareAbout(CActor &who);

	// Get the health
	float GetHealth(CActor &who);

	// Get at the afflictions
	float GetAffliction(CActor &who, Afflictions::Affliction affliction);
	Vector3 GetAfflictionSource(CActor &who, Afflictions::Affliction affliction);

	// Get the current zoom level
	int GetZoomLevel(CActor &who);

	inline float Factor(float input, float base)
		{ return (Math::Zero(base) ? 1.0f : input / base); }

	// Interact with the hud
	void PulseHealthHUD(float strength, float duration);

	// Deafness
	SoundHandle		m_deafLoop;
	float				m_deafVolume;
	void ApplyDeafness(float intensity);
	void RestoreEars();

	// The afflictions
	FullScreenFXSprite	m_smoke;
	FullScreenFXSprite	m_blind;
	FullScreenFX			m_shock;
	FullScreenFX			m_fire;
	FullScreenFX			m_tearGas;
#ifdef DIRECTX_PC
	// for now, since the decision to use
	// backbuffer effects happens at runtime,
	// we need to create both kinds of distortion
	// effects.
	FullScreenFXSprite		m_flashSprite;
	FullScreenFXSprite		m_shockSprite;
	FullScreenFXSprite		m_fireSprite;
	FullScreenFXSprite		m_tearGasSprite;
#endif
	Vector3					m_fsdPos;

	// Blood splats
	struct BloodSplat
	{
		bool				visible;
		float				alpha;
		t_Handle			sprite;
	};
	BloodSplat			*m_bloodSplat;
	void CreateBloodSplats();
	void NewBloodSplat(Vector2 center, float scale);
	void UpdateBloodSplats(float deltaT);
	void ResetBloodSplats();

	/////////////////////
	// Var file settings
	/////////////////////

	// Health blur
	float				m_baseBlur, m_baseTrip;
	float				m_baseBlurFilter;
	float				m_healthStartBlur, m_healthStartTrip;
	float				m_healthBlurMin, m_healthBlurMax;
	float				m_healthTripMin, m_healthTripMax;

	// heartbeat rate stuff
	float				m_healthStartHeart;
	float				m_heartMinRate, m_heartMaxRate;

	// heartbeat vibration settings
	int 				m_beat1Duration, m_beat1Intensity;
	float				m_beat1HUDPulse;
	int 				m_beat2Duration, m_beat2Intensity;
	float				m_beat2HUDPulse;
	float				m_beat2Delay;

	// Explosion settings
	float				m_explodeMaxD;
	BlurSetting		m_explodeBlur;
	Vibrate			m_explodeVibrate;
	CameraShake		m_explodeCameraShake;

	// Hit settings
	float				m_hitBlurEnergy;
	BlurSetting		m_hitBlur;
	float				m_hitEnergyBase;
	Vibrate			m_hitVibrate;
	CameraShake		m_hitCameraShake;

	// Deafness
	float				m_deafVolumeTimeConstant;
	float				m_deafPitch;

	// Shocking!
	float				m_shockAmp, m_shockFreq, m_shockOmega;

	// Blindness
	float				m_baseBlindBlur, m_blindBlur;
	float				m_baseBlindTrip, m_blindTrip;
	float				m_blindFilterUp, m_blindFilterDown;
	float				m_currentBlind;

	// Blood splats
	char				m_splatDBL[32];
	int				m_splatStartTexture;
	int				m_splatTextures;
	int				m_splats;
	float				m_splatMinRadius;
	float				m_splatMaxRadius;
	int				m_splatStartAlpha;
	float				m_splatTimeConstant;
};

DefineSingleton(AvatarFX)
#define g_avatarFX GetSingleton(AvatarFX)

#endif
