///////////////////////////////////////////////////////////////////////////////
// Avatar Special Effects
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "avatar/pstat.h"
#include "camera/shootercam.h"
#include "Math/sfilter.h"
#include "Environ/Munition.h"
#include "Effects/scenesetup.h"
#include "components/affliction.h"
#include "weapons/zoomsight.h"
#include "Effects/fullscreendistort.h"
#include "effects/blood.h"
#include "frontend/frontend.h"
#include "avatar/avatarfx.h"

// This is hot!
DeclareSingleton(AvatarFX);

#ifdef PS2
extern bool gDOFOn;
#endif

///////////////////////////////////////////////////////////////////////////////
// Stuff for the stuff
///////////////////////////////////////////////////////////////////////////////
AvatarFX::AvatarFX() :
m_smoke("smoke"),
m_blind("blind"),
m_shock("shock"),
m_fire("fire"),
#ifdef DIRECTX_PC
m_flashSprite("flash"),
m_shockSprite("shock"),
m_fireSprite("fire"),
m_tearGasSprite("tearGas"),
#endif
m_tearGas("tearGas")
{
	m_currentActor = NULL;
	m_health = 100.0f;
 	m_bloodSplat = NULL;
	m_controller = NULL;

	m_baseBlur = 0.0f;
	m_baseTrip = 0.0f;
	m_baseBlurFilter = 0.1f;
	m_healthStartBlur = 40.0f;
	m_healthStartTrip = 20.0f;
	m_healthBlurMin = 8.0f;
	m_healthBlurMax = 64.0f;
	m_healthTripMin = 0.0f;
	m_healthTripMax = 4.0f;

	m_healthStartHeart = 40.0f;
	m_heartMinRate = 2.0f;
	m_heartMaxRate = 0.5f;

	m_beat1Duration = 500;
	m_beat1Intensity = 128;
 	m_beat1HUDPulse = 1.5f;
	m_beat2Duration = 250;
	m_beat2Intensity = 64;
 	m_beat2HUDPulse = 1.2f;
	m_beat2Delay = 0.75f;

	m_explodeMaxD = 8.0f;
	m_explodeBlur.blur = 64.0f;
	m_explodeBlur.trip = 8.0f;
	m_explodeBlur.filter = 0.0f;
	m_explodeBlur.duration = 3.0f;

	m_explodeVibrate.duration = 2500;
	m_explodeVibrate.intensity = ControllerInputDriver::RUMBLELOW;

	m_explodeCameraShake.duration = 2.5f;
	m_explodeCameraShake.frequency = 250.0f;
	m_explodeCameraShake.amplitude = 0.15f;

#ifdef DIRECTX_PC
	m_hitBlurEnergy = 500.0f;
#else
	m_hitBlurEnergy = 750.0f;
#endif
	m_hitBlur.blur = 64.0f;
	m_hitBlur.trip = 8.0f;
	m_hitBlur.filter = 0.0f;
	m_hitBlur.duration = 0.4f;

	m_hitEnergyBase = 600.0f;
	m_hitVibrate.duration = 500;
	m_hitVibrate.intensity = ControllerInputDriver::RUMBLEMEDIUM;

	m_hitCameraShake.duration = 0.5f;
	m_hitCameraShake.frequency = 250.0f;
	m_hitCameraShake.amplitude = 0.07f;

	// Other full screen effects
	m_smoke.SetDefaults(Vector4(128.0f, 128.0f, 128.0f, 127.0f), 0.5f, 0.25f);

	m_blind.SetDefaults(Vector4(255.0f, 255.0f, 255.0f, 128.0f));
	m_baseBlindBlur = 0.0f;
	m_blindBlur = 0.0f;
	m_baseBlindTrip = 0.0f;
	m_blindTrip = 0.0f;
	m_blindFilterUp = 0.5f;
	m_blindFilterDown = 1.0f;

	m_fire.SetDefaults(Vector4(255.0f, 128.0f, 128.0f, 127.0f), 0.5f, 0.25f);
	m_shock.SetDefaults(Vector4(0.0f, 25.0f, 255.0f, 127.0f), 0.25f, 0.1f);
	m_tearGas.SetDefaults(Vector4(255.0f, 255.0f, 128.0f, 127.0f), 0.5, 0.25f);
#ifdef DIRECTX_PC
	m_flashSprite.SetDefaults(Vector4(255.0f, 255.0f, 255.0f, 255.0f));
	m_fireSprite.SetDefaults(Vector4(255.0f, 128.0f, 128.0f, 127.0f), 0.5f, 0.25f);
	m_shockSprite.SetDefaults(Vector4(0.0f, 25.0f, 255.0f, 127.0f), 0.25f, 0.1f);
	m_tearGasSprite.SetDefaults(Vector4(255.0f, 255.0f, 128.0f, 127.0f), 0.5, 0.25f);
#endif

	m_fsdPos.Clear();

	m_deafLoop = NULL;
	m_deafVolume = 0.0f;
	m_deafVolumeTimeConstant = 0.8f;
	m_deafPitch = 0.0f;

	m_shockAmp = 1.0f;
	m_shockFreq = 2.0f;
	m_shockOmega = 20.0f;

	m_splatDBL[0] = '\0';
	m_splatStartTexture = 0;
	m_splatTextures = 0;
	m_splats = 8;
	m_splatMinRadius = 0.2f;
	m_splatMaxRadius = 0.7f;
	m_splatStartAlpha = 200;
	m_splatTimeConstant = 0.1f;

	Reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AvatarFX::~AvatarFX()
{
	Sfx::Stop(m_deafLoop);
 	delete [] m_bloodSplat;
}

///////////////////////////////////////////////////////////////////////////////
// Intialize stuff
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::Setup()
{
	// Load in our shizna from the bizna
	const char *context = "avatarfx";

	g_console.CreateVar(context, "baseBlur", m_baseBlur, CONVAR_REAL);
	g_console.CreateVar(context, "baseTrip", m_baseTrip, CONVAR_REAL);

	g_console.CreateVar(context, "baseBlurFilter", m_baseBlurFilter, CONVAR_REAL);

	g_console.CreateVar(context, "healthStartBlur", m_healthStartBlur, CONVAR_REAL);
	g_console.CreateVar(context, "healthStartTrip", m_healthStartTrip, CONVAR_REAL);

	g_console.CreateVar(context, "healthBlurMin", m_healthBlurMin, CONVAR_REAL);
	g_console.CreateVar(context, "healthBlurMax", m_healthBlurMax, CONVAR_REAL);

	g_console.CreateVar(context, "healthTripMin", m_healthTripMin, CONVAR_REAL);
	g_console.CreateVar(context, "healthTripMax", m_healthTripMax, CONVAR_REAL);

	g_console.CreateVar(context, "healthStartHeart", m_healthStartHeart, CONVAR_REAL);
	g_console.CreateVar(context, "heartMinRate", m_heartMinRate, CONVAR_REAL);
	g_console.CreateVar(context, "heartMaxRate", m_heartMaxRate, CONVAR_REAL);

	g_console.CreateVar(context, "beat1Duration", m_beat1Duration);
	g_console.CreateVar(context, "beat1Intensity", m_beat1Intensity);
	g_console.CreateVar(context, "beat1HUDPulse", m_beat1HUDPulse, CONVAR_REAL);
	g_console.CreateVar(context, "beat2Duration", m_beat1Duration);
	g_console.CreateVar(context, "beat2Intensity", m_beat1Intensity);
	g_console.CreateVar(context, "beat2HUDPulse", m_beat2HUDPulse, CONVAR_REAL);
	g_console.CreateVar(context, "beat2Delay", m_beat2Delay, CONVAR_REAL);

	g_console.CreateVar(context, "explodeMaxD", m_explodeMaxD, CONVAR_REAL);
	g_console.CreateVar(context, "explodeBlurTime", m_explodeBlur.duration, CONVAR_REAL);
	g_console.CreateVar(context, "explodeBlurBlur", m_explodeBlur.blur, CONVAR_REAL);
	g_console.CreateVar(context, "explodeBlurTrip", m_explodeBlur.trip, CONVAR_REAL);
	g_console.CreateVar(context, "explodeBlurFilter", m_explodeBlur.filter, CONVAR_REAL);

	g_console.CreateVar(context, "explodeVibrateTime", m_explodeVibrate.duration, CONVAR_REAL);
	g_console.CreateVar(context, "explodeVibrateIntensity", m_explodeVibrate.intensity, CONVAR_REAL);

	g_console.CreateVar(context, "explodeCameraShakeTime", m_explodeCameraShake.duration, CONVAR_REAL);
	g_console.CreateVar(context, "explodeCameraShakeAmp", m_explodeCameraShake.amplitude, CONVAR_REAL);
	g_console.CreateVar(context, "explodeCameraShakeFreq", m_explodeCameraShake.frequency, CONVAR_REAL);

#ifndef DIRECTX_PC
	// Do what you need to do, but don't take my shit out
	g_console.CreateVar(context, "hitBlurEnergy", m_hitBlurEnergy, CONVAR_REAL);
#endif
	g_console.CreateVar(context, "hitBlurTime", m_hitBlur.duration, CONVAR_REAL);
	g_console.CreateVar(context, "hitBlurBlur", m_hitBlur.blur, CONVAR_REAL);
	g_console.CreateVar(context, "hitBlurTrip", m_hitBlur.trip, CONVAR_REAL);
	g_console.CreateVar(context, "hitBlurFilter", m_hitBlur.filter, CONVAR_REAL);

	g_console.CreateVar(context, "hitVibrateTime", m_hitVibrate.duration, CONVAR_REAL);
	g_console.CreateVar(context, "hitVibrateIntensity", m_hitVibrate.intensity, CONVAR_REAL);

	g_console.CreateVar(context, "hitCameraShakeTime", m_hitCameraShake.duration, CONVAR_REAL);
	g_console.CreateVar(context, "hitCameraShakeAmp", m_hitCameraShake.amplitude, CONVAR_REAL);
	g_console.CreateVar(context, "hitCameraShakeFreq", m_hitCameraShake.frequency, CONVAR_REAL);

	g_console.CreateVar(context, "deafVolumeTC", m_deafVolumeTimeConstant, CONVAR_REAL);
	g_console.CreateVar(context, "deafPitch", m_deafPitch, CONVAR_REAL);

	g_console.CreateVar(context, "shockAmp", m_shockAmp, CONVAR_REAL);
	g_console.CreateVar(context, "shockFreq", m_shockFreq, CONVAR_REAL);
	g_console.CreateVar(context, "shockOmega", m_shockOmega, CONVAR_REAL);

	// Other full screen effects
	m_smoke.RegisterContext(context);

	m_blind.RegisterContext(context);
	g_console.CreateVar(context, "baseBlindBlur", m_baseBlindBlur, CONVAR_REAL);
	g_console.CreateVar(context, "blindBlur", m_blindBlur, CONVAR_REAL);
	g_console.CreateVar(context, "baseBlindTrip", m_baseBlindTrip, CONVAR_REAL);
	g_console.CreateVar(context, "blindTrip", m_blindTrip, CONVAR_REAL);
	g_console.CreateVar(context, "blindFilterUp", m_blindFilterUp, CONVAR_REAL);
	g_console.CreateVar(context, "blindFilterDown", m_blindFilterDown, CONVAR_REAL);

	m_shock.RegisterContext(context);
	m_fire.RegisterContext(context);
	m_tearGas.RegisterContext(context);

	g_console.CreateVar(context, "splatDBL", m_splatDBL, sizeof(m_splatDBL) - 1);
	g_console.CreateVar(context, "splatStartTexture", m_splatStartTexture);
	g_console.CreateVar(context, "splatTextures", m_splatTextures);
	g_console.CreateVar(context, "splats", m_splats);
	g_console.CreateVar(context, "splatMinRadius", m_splatMinRadius);
	g_console.CreateVar(context, "splatMaxRadius", m_splatMaxRadius);
	g_console.CreateVar(context, "splatStartAlpha", m_splatStartAlpha);
	g_console.CreateVar(context, "splatTimeConstant", m_splatTimeConstant);

	g_console.ExecuteContext(context);

#ifndef INTERACTIVE_CONSOLE
	g_console.DeleteContext(context);
#endif //INTERACTIVE_CONSOLE

	// Setup the full screen stuff
	m_smoke.Initialize();
	m_blind.Initialize();
	m_shock.Initialize();
	m_fire.Initialize();
	m_tearGas.Initialize();
#ifdef DIRECTX_PC
	m_flashSprite.RegisterContext(context);
	m_shockSprite.RegisterContext(context);
	m_fireSprite.RegisterContext(context);
	m_tearGasSprite.RegisterContext(context);

	m_flashSprite.Initialize();
	m_shockSprite.Initialize();
	m_fireSprite.Initialize();
	m_tearGasSprite.Initialize();
#endif


	// Setup the blood splats
	CreateBloodSplats();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::StartFrame()
{
	// Save this
	if (!g_shooterCamPtr)
		m_currentActor = NULL;
	else
 		m_currentActor = g_shooterCam.GetFollow();
	m_health = 0.0f;

	if (!m_controller)
		m_controller = g_controlFocus.Find("Avatar");

	// Clear the requests
	ClearBlurRequest();
	ClearVibrateRequest();
	ClearCameraShakeRequest();
}

///////////////////////////////////////////////////////////////////////////////
// Call this once per frame
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::EndFrame()
{
	// Anything to do?
	bool inFrontEnd = ((g_FrontEnd.GetActiveScreen() != NULL) || g_FrontEnd.ScoreBoardUp());

	if (inFrontEnd || !g_shooterCamPtr || !g_shooterCam.Active())
	{
		Reset();
		return;
	}

	// Update the position
	m_fsdPos = m_currentActor->GetBodyInWorld();
	m_fsdPos.Y(m_fsdPos.Y() + 2.0f);

	// Get our health
	m_health = GetHealth(*m_currentActor);

	// Turn health into a base blur
	if (m_health > m_healthStartBlur)
		SetBaseBlur(m_baseBlur, m_baseTrip);
	else
	{
		float blur = Math::LinearMap(m_health, 0.0f, m_healthStartBlur, m_healthBlurMax, m_healthBlurMin);
		float trip = 0.0f;
		if (m_health < m_healthStartTrip)
			trip = Math::LinearMap(m_health, 0.0f, m_healthStartTrip, m_healthTripMax, m_healthTripMin);
		SetBaseBlur(m_baseBlur + blur, m_baseTrip + trip);
	}

	// Do the heart beat
	if (m_health > m_healthStartHeart)
		SetHeartBeatRate(0.0f);
	else
		SetHeartBeatRate(Math::LinearMap(m_health, 0.0f, m_healthStartHeart, m_heartMaxRate, m_heartMinRate));

	// Handle the afflictions
	ApplyDeafness(GetAffliction(*m_currentActor, Afflictions::DEAF));

	m_smoke.Update(GetAffliction(*m_currentActor, Afflictions::INSMOKE), &m_fsdPos);

	float blind = Math::Clamp(GetAffliction(*m_currentActor, Afflictions::BLIND));
	if (!Math::Equal(blind, m_currentBlind))
	{
		if (m_currentBlind < blind)
			m_currentBlind = FOFilterImplicit(m_currentBlind, blind, g_timer.GetFrameSec(), m_blindFilterUp);
		else
		{
			m_currentBlind = FOFilterImplicit(m_currentBlind, blind, g_timer.GetFrameSec(), m_blindFilterDown);
			if (m_currentBlind < 0.01f)
				m_currentBlind = 0.0f;
		}
	}

	float shock = Math::Max(GetAffliction(*m_currentActor, Afflictions::STUNNED), GetAffliction(*m_currentActor, Afflictions::SHOCK));
	float fire = GetAffliction(*m_currentActor, Afflictions::ONFIRE);
	float tearGas = GetAffliction(*m_currentActor, Afflictions::TEARGAS);
	if (blind > 0.0f)
		fire = shock = tearGas = 0.0f;
	else if (shock > 0.0f)
		fire = tearGas = 0.0f;
	else if (fire > 0.0f)
		tearGas = 0.0f;
	if (!Math::Zero(m_currentBlind))
	{
		// special stuff for blind
		m_fsdPos = GetAfflictionSource(*m_currentActor, Afflictions::BLIND);
		float blur = m_baseBlindBlur + m_currentBlind * m_blindBlur;
		float trip = m_baseBlindTrip + m_currentBlind * m_blindTrip;
		SetBaseBlur(m_baseBlur + blur, m_baseTrip + trip);
	}

	m_blind.Update(m_currentBlind, &m_fsdPos);

#ifdef DIRECTX_PC
	if (RenderToTexture::isRenderingToTextureOn)
	{
//		m_flash.Update(blind, &m_fsdPos);
		m_shock.Update(shock, &m_fsdPos);
		m_fire.Update(fire, &m_fsdPos);
		m_tearGas.Update(tearGas, &m_fsdPos);
	}
	else
	{
		m_flashSprite.Update(blind, &m_fsdPos);
		m_shockSprite.Update(shock, &m_fsdPos);
		m_fireSprite.Update(fire, &m_fsdPos);
		m_tearGasSprite.Update(tearGas, &m_fsdPos);
	}
#else
	m_shock.Update(shock, &m_fsdPos);
	m_fire.Update(fire, &m_fsdPos);
	m_tearGas.Update(tearGas, &m_fsdPos);
#endif // DIRECTX_PC

	// Now do the updates
	bool forceNoBlur = m_fire.OnScreen() || m_tearGas.OnScreen() || m_shock.OnScreen();
	UpdateBlur(g_timer.GetFrameSec(), forceNoBlur);
	if (forceNoBlur)
		ResetCameraShake();
	UpdateCameraShake();
	UpdateHeartBeat(g_timer.GetEndSec());
	UpdateVibrate();

	// Splats!
	UpdateBloodSplats(g_timer.GetFrameSec());

#ifdef PS2
	//gDOFOn = (GetZoomLevel(*m_currentActor) != 0);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Reset all effects
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::Reset()
{
	ResetBlur();
	ResetVibrate();
	ResetHeartBeat();
	ResetCameraShake();

	m_smoke.Reset();
	m_blind.Reset();
	m_currentBlind = 0.0f;
	m_shock.Reset();
	m_fire.Reset();
	m_tearGas.Reset();
#ifdef DIRECTX_PC
	m_flashSprite.Reset();
	m_shockSprite.Reset();
	m_fireSprite.Reset();
	m_tearGasSprite.Reset();
#endif

	RestoreEars();
	ResetBloodSplats();
	ClearController();
}

///////////////////////////////////////////////////////////////////////////////
// Clear out the cache to the controller
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::ClearController()
{
	m_controller = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Tell me what is going on
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::NotifyExplosion(
Vector3CRef explosionPos)
{
	// How far away are we
	Vector3 delta = g_cameraDirector.CurrentCamera().Eye() - explosionPos;
	if (delta.LengthSquared() > Math::Square(m_explodeMaxD))
		return;

	// Do the thingie
	float factor = 1.0f - Factor(delta.Length(), m_explodeMaxD);
	RequestBlur(m_explodeBlur.blur * factor, m_explodeBlur.trip, m_explodeBlur.filter, m_explodeBlur.duration);
	RequestVibrate((int)((float)m_explodeVibrate.duration * factor), m_explodeVibrate.intensity);
	RequestCameraShake(m_explodeCameraShake.duration * factor, m_explodeCameraShake.amplitude * factor, m_explodeCameraShake.frequency);
}

///////////////////////////////////////////////////////////////////////////////
// Tell me what is going on
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::NotifyWeaponFire(
CActor &who,
float duration,
float intensity)
{
	if (!CareAbout(who))
		return;

	// Do the thingie
	int ms = (int)(duration * 1000.0f);
	int i = Math::Clamp((int)(intensity * 255.0f), 0, 255);
	if (ms > 0)
		RequestVibrate((uint)ms, (uint)i);
}

///////////////////////////////////////////////////////////////////////////////
// Tell me what is going on
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::NotifyWeaponHit(
CActor &who,
const CMunitionImpact &hit)
{
	if (!CareAbout(who))
		return;

	// Do the thingie
	if (hit.d_energy >= m_hitBlurEnergy)
		RequestBlur(m_hitBlur.blur, m_hitBlur.trip, m_hitBlur.filter, m_hitBlur.duration);

	// Do a vibrate
	float factor = Factor(hit.d_energy, m_hitEnergyBase);
	int duration = (int)(m_hitVibrate.duration * factor);
	RequestVibrate((uint)duration, m_hitVibrate.intensity);

	// Shake the camera
	RequestCameraShake(m_hitCameraShake.duration, m_hitCameraShake.amplitude * factor, m_hitCameraShake.frequency);

	// Splat!
	if (g_blood.Enabled())
	{
		static Vector2 clipFactor(1.1f, 1.1f);
		Vector2 screen;
		float w;
		if (g_cameraDirector.CurrentCamera().ToScreen(hit.d_shotOrigin, screen, w, clipFactor))
		{
			float scale = hit.d_damage / 100.0f;
			NewBloodSplat(screen, scale);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::NotifyDeath(
CActor &who)
{
	if (!CareAbout(who))
		return;
}


///////////////////////////////////////////////////////////////////////////////
// Get some aim modification! (pass in x/y from avatarcontrolmap)
///////////////////////////////////////////////////////////////////////////////
bool AvatarFX::AimModify(float &x, float &y)
{
	float shock = Math::Max(GetAffliction(*m_currentActor, Afflictions::STUNNED), GetAffliction(*m_currentActor, Afflictions::SHOCK));
	if (Math::Zero(shock))
		return false;

	const float theta = g_timer.GetEndSec() * m_shockFreq;
	x = Math::Clamp(x + g_perlinNoise.Noise(theta) * m_shockAmp, -1.0f, 1.0f);
	y = Math::Clamp(y + g_perlinNoise.Noise(theta + m_shockOmega) * m_shockAmp, -1.0f, 1.0f);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::ClearBlurRequest()
{
	m_blur.haveRequest = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::RequestBlur(
float blur,
float trip,
float filter,
float duration)
{
	if (!m_blur.haveRequest || blur > m_blur.request.blur)
	{
		m_blur.request.blur = blur;
		m_blur.request.trip = trip;
		m_blur.request.filter = filter;
		m_blur.request.duration = duration;
		m_blur.haveRequest = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::SetBaseBlur(
float blur,
float trip)
{
	m_blur.base.blur = blur;
	m_blur.base.trip = trip;
	m_blur.base.duration = -1.0f;
	m_blur.base.filter = m_baseBlurFilter;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::UpdateBlur(
float deltaT,
bool forceNoBlur)
{
	// What is our blur target?
	if (m_blur.haveRequest)
		m_blur.target = m_blur.request;

	// Has the current target expired?
	if (m_blur.target.duration <= 0.0f)
		m_blur.target = m_blur.base;

	// Do the work
	m_blur.current.blur = FOFilterImplicit(m_blur.current.blur, m_blur.target.blur, deltaT, m_blur.target.filter);
	m_blur.current.trip = FOFilterImplicit(m_blur.current.trip, m_blur.target.trip, deltaT, m_blur.target.filter);

	if (forceNoBlur)
		gfxMotionBlur(0, 0);
	else
	{
		int blur = Math::Clamp((int)m_blur.current.blur, 0, 255);
		int trip = Math::Clamp((int)m_blur.current.trip, 0, 255);
		gfxMotionBlur(blur, trip);
	}

	// Update duration
	if (m_blur.target.duration >= 0.0f)
		m_blur.target.duration -= deltaT;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::ResetBlur()
{
	memset(&m_blur, '\0', sizeof(m_blur));
	gfxMotionBlur(0, 0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::ClearVibrateRequest()
{
	m_vibrateRequest = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::RequestVibrate(
uint duration,
uint intensity)
{
	if (!m_vibrateRequest || intensity > m_vibrate.intensity)
	{
		m_vibrate.duration = duration;
		m_vibrate.intensity = intensity;
		m_vibrateRequest = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::UpdateVibrate()
{
	if (!m_vibrateRequest)
		return;

	// Do the thingie
	if (m_controller)
	{
		uint currentIntensity;
		bool vibrating = m_controller->GetVibration(currentIntensity);

		uint newIntensity = Math::Clamp(m_vibrate.intensity, 0, 255);
		if (vibrating && currentIntensity > newIntensity)
			return;

		if (vibrating)
			m_controller->ClearVibration(true);
		m_controller->SetVibrationDuration(m_vibrate.duration, newIntensity);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::ResetVibrate()
{
	m_vibrateRequest = false;
	if (m_controller)
		m_controller->ClearVibration(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::ResetHeartBeat()
{
	m_beatQueueTop = -1;
	m_beatRate = 0.0f;
	m_lastBeat = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::SetHeartBeatRate(
float rate)
{
	m_beatRate = rate;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::RequestHeartBeat(
float now)
{
	// Fill out the queue
	m_beatQueue[0].when = now + m_beat2Delay;
	m_beatQueue[0].duration = m_beat2Duration;
	m_beatQueue[0].intensity = m_beat2Intensity;
	m_beatQueue[0].hudPulse = m_beat2HUDPulse;

	m_beatQueue[1].when = now;
	m_beatQueue[1].duration = m_beat1Duration;
	m_beatQueue[1].intensity = m_beat1Intensity;
	m_beatQueue[1].hudPulse = m_beat1HUDPulse;

	m_beatQueueTop = 1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::UpdateHeartBeat(
float time)
{
	// Request a beat?
	if (!Math::Zero(m_beatRate) && time > m_lastBeat + m_beatRate)
	{
		RequestHeartBeat(time);
		m_lastBeat = time;
	}

	// Time to do a beat?
	if (m_beatQueueTop >= 0 && time >= m_beatQueue[m_beatQueueTop].when)
	{
	 	RequestVibrate(m_beatQueue[m_beatQueueTop].duration, m_beatQueue[m_beatQueueTop].intensity);
		PulseHealthHUD(m_beatQueue[m_beatQueueTop].hudPulse, (float)m_beatQueue[m_beatQueueTop].duration / 1000.0f);
		--m_beatQueueTop;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::ClearCameraShakeRequest()
{
	m_requestCameraShake = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::RequestCameraShake(
float duration,
float amplitude,
float frequency)
{
	if (!m_requestCameraShake || amplitude > m_cameraShake.amplitude)
	{
		m_cameraShake.duration = duration;
		m_cameraShake.amplitude = amplitude;
		m_cameraShake.frequency = frequency;
		m_requestCameraShake = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::UpdateCameraShake()
{
	// Shake!
	if (m_requestCameraShake && g_shooterCamPtr)
		g_shooterCam.SetEyePointShake(m_cameraShake.amplitude, m_cameraShake.frequency, m_cameraShake.duration);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::ResetCameraShake()
{
	if (g_shooterCamPtr)
		g_shooterCam.ClearEyePointShake();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AvatarFX::CareAbout(
CActor &who)
{
 	return (m_currentActor && &who == m_currentActor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float AvatarFX::GetHealth(
CActor &who)
{
	float health = 100.0f;

	// Force the health somewhere
	static float debugHealth = 0.0f;
	if (!Math::Zero(debugHealth))
		return debugHealth;

	if (who.IsA(CCompActor::ClassType()))
	{
		CCompActor &comp = (CCompActor &)who;
		health = comp.GetProperty("health")->GetFloat();
	}

	return health;
}

///////////////////////////////////////////////////////////////////////////////
// Get the affliction
///////////////////////////////////////////////////////////////////////////////
float AvatarFX::GetAffliction(
CActor &who,
Afflictions::Affliction affliction)
{
 	CCompActor &comp = (CCompActor &)who;
	Afflictions *afflictions = (Afflictions *)comp.GetComponentByName("Afflictions");
	return afflictions ? afflictions->GetStrength(affliction) : 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Get the affliction
///////////////////////////////////////////////////////////////////////////////
Vector3 AvatarFX::GetAfflictionSource(
CActor &who,
Afflictions::Affliction affliction)
{
 	CCompActor &comp = (CCompActor &)who;
	Afflictions *afflictions = (Afflictions *)comp.GetComponentByName("Afflictions");
	return afflictions ? afflictions->GetSource(affliction) : m_fsdPos;
}

///////////////////////////////////////////////////////////////////////////////
// Get the zoomlevel
///////////////////////////////////////////////////////////////////////////////
int AvatarFX::GetZoomLevel(
CActor &who)
{
 	CCompActor &comp = (CCompActor &)who;
	WeaponInventory *inv = (WeaponInventory *)comp.GetComponentByName("WeaponInventory");
	if (!inv)
		return 0;
	Weapon *weapon = (Weapon *)inv->GetSelected();
	if (!weapon)
		return 0;
	ZoomSight *aimSight = (ZoomSight *)weapon->GetSight();
	if (!aimSight)
		return 0;
	return aimSight->ZoomLevel();
}

///////////////////////////////////////////////////////////////////////////////
// Interact with the hud
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::PulseHealthHUD(
float strength,
float duration)
{
	if (!m_currentActor)
		return;

	if (m_currentActor->IsA(CCompActor::ClassType()))
	{
		CCompActor *comp = (CCompActor *)m_currentActor;
		GameHUD_PulseHealth msg = { strength, duration };
		g_hud.SendMessage(GAMEHUD_PULSEHEALTH, &msg, m_currentActor);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::ApplyDeafness(
float intensity)
{
	const float maxDeafVolume = 0.9f;

	if (Math::Zero(intensity))
	{
		if (m_deafLoop)
		{
			if (Math::Zero(m_deafVolume))
				Sfx::Stop(m_deafLoop);
			else
			{
				float deltaT = g_timer.GetFrameSec();
				m_deafVolume = FOFilterImplicit(m_deafVolume, 0.0f, deltaT, m_deafVolumeTimeConstant * 0.5f);
				if (m_deafVolume < 0.005f)
					m_deafVolume = 0.0f;
				Sfx::SetVolume(m_deafLoop, m_deafVolume);
				g_audio.SetVolumeFactor(Audio::SFX3D, 1.0f - m_deafVolume);
			}
		}
	}
	else // Still ramping up
	{
		if (!m_deafLoop)
		{
			m_deafVolume = 0.15f;
			Sfx::Play2D("earring", m_deafVolume, m_deafPitch, &m_deafLoop);
		}
		if (m_deafLoop)
		{
			float deltaT = g_timer.GetFrameSec();
			m_deafVolume = FOFilterImplicit(m_deafVolume, maxDeafVolume, deltaT, m_deafVolumeTimeConstant);
			Sfx::SetVolume(m_deafLoop, m_deafVolume);
			g_audio.SetVolumeFactor(Audio::SFX3D, 1.0f - m_deafVolume);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::RestoreEars()
{
	Sfx::Stop(m_deafLoop);
	g_audio.SetVolumeFactor(Audio::SFX3D, 1.0f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::CreateBloodSplats()
{
	// Create the array
	if (m_splats == 0)
		return;

	// Load the dbl
	t_Handle textureDBL = dblLoad(m_splatDBL);
	if (textureDBL == INVALID_HANDLE)
		return;
	SpriteMgr__SetTextureDbl(textureDBL);

	// How many textures are there?
	TextureDatabase *textureDB = dblGetTextures(textureDBL, DEFAULT_CHUNK_KEY_STRING);
	if (!textureDB)
		return;
	int textures = textureDB->TextureCount();

	// Create the array
	m_bloodSplat = new BloodSplat[m_splats];
	ASSERT(m_bloodSplat);

	// Some defaults
	Vector3 v[4];
	v[0].Set(0.0f, 0.0f, 0.0f);
	v[1].Set(0.0f, 0.0f, 0.0f);
	v[2].Set(0.0f, 0.0f, 0.0f);
	v[3].Set(0.0f, 0.0f, 0.0f);

	Vector2 uv[4];
	uv[0].Set(0.0f, 0.0f);
	uv[1].Set(1.0f, 0.0f);
	uv[2].Set(1.0f, 1.0f);
	uv[3].Set(0.0f, 1.0f);

	ts_bRGBA white = { 255, 255, 255, 255 };
	ts_bRGBA	color[4];
	color[0] = color[1] = color[2] = color[3] = white;

	// Create the sprites
	for (int i = 0; i < m_splats; i++)
	{
		int texture = m_splatStartTexture + (i % m_splatTextures);
		ASSERT(texture < textures);

		m_bloodSplat[i].visible = false;
		m_bloodSplat[i].alpha = 0.0f;
		m_bloodSplat[i].sprite = SpriteMgr__RegisterSprite(texture, 100, SPRITE_DEFAULT_SORT,
					  					 									SPRITE_ALL_ONE_COLOR, v, uv, color);

		SpriteMgr__DisableSprite(m_bloodSplat[i].sprite);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::NewBloodSplat(
Vector2 center,
float scale)
{
	// Find an open splat
	BloodSplat *splat = NULL;
	for (int i = 0; i < m_splats; i++)
	{
		if (!m_bloodSplat[i].visible)
		{
			splat = &m_bloodSplat[i];
			break;
		}
	}
	if (!splat)
		return;

	// What is our radius?
	float radius = m_splatMinRadius + (m_splatMaxRadius - m_splatMinRadius) * scale;

	// Make the canter little jittery
	Vector2 pos(center.X() + g_random.InRange(-m_splatMinRadius, m_splatMinRadius),
					center.Y() + g_random.InRange(-m_splatMinRadius, m_splatMinRadius));
	float radiusX = 3.0f / 4.0f * radius;
	float radiusY = radius;
	Vector3 v[4];
	v[0].Set(pos.X() - radiusX, pos.Y() + radiusY, 0.0f);
	v[1].Set(pos.X() + radiusX, pos.Y() + radiusY, 0.0f);
	v[2].Set(pos.X() + radiusX, pos.Y() - radiusY, 0.0f);
	v[3].Set(pos.X() - radiusX, pos.Y() - radiusY, 0.0f);

	// Start with a standard color
	ts_bRGBA c[4];
	c[0].R = c[0].G = c[0].B = 255;
	c[0].A = (unsigned char)Math::Clamp(m_splatStartAlpha, 0, 255);
#ifdef PS2
	c[0].A /= 2;
#endif
	c[1] = c[2] = c[3] = c[0];

	// Rotate it!
	static Vector2 baseUV[4] =
	{
		Vector2(0.0f, 0.0f),
		Vector2(1.0f, 0.0f),
		Vector2(1.0f, 1.0f),
		Vector2(0.0f, 1.0f)
	};
	Vector2 uv[4];
	float cosTheta, sinTheta;
	Math::SinCos(g_random.InRange(-Math::Pi, Math::Pi), sinTheta, cosTheta);
	float a = (sinTheta * 0.5f - cosTheta * 0.5f);
	float b = -1.0f * (sinTheta * 0.5f + cosTheta * 0.5f);
	for (uint i = 0; i < 4; i++)
	{
		uv[i].Set(cosTheta * baseUV[i].x() - sinTheta * baseUV[i].y() + a + 0.5f,
					 sinTheta * baseUV[i].x() + cosTheta * baseUV[i].y() + b + 0.5f);
	}

	// Wee!
	splat->visible = true;
	splat->alpha = (float)Math::Clamp((float)m_splatStartAlpha / 255.0f);
	SpriteMgr__EnableSprite(splat->sprite);
	SpriteMgr__UpdateSprite(splat->sprite, 100,
									SPRITE_DEFAULT_SORT, SPRITE_ALL_ONE_COLOR,
									v, uv, c);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::UpdateBloodSplats(
float deltaT)
{
	for (int i = 0; i < m_splats; i++)
	{
		if (m_bloodSplat[i].visible)
		{
			m_bloodSplat[i].alpha = FOFilterImplicit(m_bloodSplat[i].alpha, 0.0f, deltaT, m_splatTimeConstant);
			if (m_bloodSplat[i].alpha < 0.005f)
			{
				m_bloodSplat[i].visible = false;
				SpriteMgr__DisableSprite(m_bloodSplat[i].sprite);
			}
			else
			{
				// Update the alpha
				ts_bRGBA c[4];
				c[0].R = c[0].G = c[0].B = 255;
				c[0].A = (unsigned char)Math::Clamp(m_bloodSplat[i].alpha * 255.0f, 0.0f, 255.0f);
#ifdef PS2
				c[0].A /= 2;
#endif
				c[1] = c[2] = c[3] = c[0];

				SpriteMgr__UpdateSprite(m_bloodSplat[i].sprite, 100,
												SPRITE_DEFAULT_SORT, SPRITE_ALL_ONE_COLOR,
												NULL, NULL, c);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AvatarFX::ResetBloodSplats()
{
	if (!m_bloodSplat)
		return;

	for (int i = 0; i < m_splats; i++)
	{
		m_bloodSplat[i].visible = false;
		SpriteMgr__DisableSprite(m_bloodSplat[i].sprite);
	}
}

