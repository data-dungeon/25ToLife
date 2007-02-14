/////////////////////////////////////////////////////////////////////////////
// Aimsim information
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Effects/sfx/sfxlight.h"
#include "Effects/sfx/sfxdistort.h"
#include "Effects/sfx/sfxsys.h"
#include "weapons/explodeFX.h"

// Our singleton
DeclareSingleton(ExplodeFXLibrary);
volatile ExplodeFXDependency g_explodeFXDependency;

// Our array of creation functions
VarObjCreator<ExplodeFX> *VarObjLibrary<ExplodeFX>::m_creatorList = NULL;

// Our only creator right now
ExplodeFXCreator g_explodeFXCreator;

/////////////////////////////////////////////////////////////////////////////
// Weee
/////////////////////////////////////////////////////////////////////////////
ExplodeFXProperties::ExplodeFXProperties(
ExplodeFXCreator &creator,
int index) :
VarObjProperties<ExplodeFX>(creator, index)
{
	// Assume nothing

 	m_sound1[0] = '\0';
 	m_sound1Pitch = 0.0f;
 	m_sound2[0] = '\0';
 	m_sound2Pitch = 0.0f;

 	m_particle[0] = '\0';
 	m_particleScale = 1.0f;

	// The light!
	m_wantLight = true;
	m_lightColor.Set(255.0f, 255.0f, 255.0f);
	m_lightAttenuation = 1.0f;
	m_lightIntensity = 1.0f;
	m_lightDuration = 0.1f;

	// Distortion settings
	m_wantDistort = true;
	m_distortInnerRadius = 2.0f;  // "middle" ring of the circle.
	m_distortOuterRadius = 7.0f;  // outer ring of the circle.
	m_distortAttack = 0.0f;
	m_distortSustain = 0.0f;
	m_distortDecay = 3.0f;
	m_distortFrequency = 25.0f;  // frequency of sin signal.
	m_distortAmplitude = 1.0f;  	// amplitude of sin signal.

	m_distortSlices = 10;  // number of (pie) slices.
	m_distortRings = 10;  // number of concentric rings.
	m_distortAspectRatio = 1.0f;  // squash/stretch circle.

	m_distortUseOmega = false;
	m_distortSignalScale = 3.0f;  // scale of signal.

	m_distortInsideColor.Set(255.0f, 255.0f, 255.0f, 255.0f);
	m_distortOuterColor.Set(255.0f, 255.0f, 255.0f, 255.0f);
	m_distortMidColor.Set(255.0f, 255.0f, 255.0f, 255.0f);

	m_distortSpiralStrength = 0.0f;  // strength of the spiral vortex.
	m_distortSpiralMaxAge = 0.0f;  // strength of the spiral vortex.
	m_distortSpiralInitialAngle = 0.0f;  // start angle (twist) of the vortex.
	m_distortFixedSpiralAngle = 0.0f;  // start angle (twist) of the vortex.
	m_distortConicDepth = 0.0f;  // start angle (twist) of the vortex.

	// Should this shake the avatar's controller
	m_avatarShake = true;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void ExplodeFXProperties::RegisterContext(
const char *context)
{
	// Call the parent
	VarObjProperties<ExplodeFX>::RegisterContext(context);

	// Sound
	g_console.CreateVar(context, "sound1", m_sound1, sizeof(m_sound1) - 1);
	g_console.CreateVar(context, "pitch1", m_sound1Pitch);
	g_console.CreateVar(context, "sound2", m_sound2, sizeof(m_sound2) - 1);
	g_console.CreateVar(context, "pitch2", m_sound2Pitch);

	// Particle
	g_console.CreateVar(context, "particle", m_particle, sizeof(m_particle) - 1);
	g_console.CreateVar(context, "particleScale", m_particleScale);

	// Shake the avatar
	g_console.CreateVar(context, "avatarShake", m_avatarShake);

	// Light!
	g_console.CreateVar(context, "wantLight", m_wantLight);
	g_console.CreateVar(context, "lightColor", &m_lightColor);
	g_console.CreateVar(context, "lightAttenuation", m_lightAttenuation);
	g_console.CreateVar(context, "lightIntensity", m_lightIntensity);
	g_console.CreateVar(context, "lightDuration", m_lightDuration);

	// Distortion settings
	g_console.CreateVar(context, "wantDistort", m_wantDistort);
	g_console.CreateVar(context, "innerRadius", m_distortInnerRadius);
	g_console.CreateVar(context, "outerRadius", m_distortOuterRadius);
	g_console.CreateVar(context, "attack", m_distortAttack);
	g_console.CreateVar(context, "sustain", m_distortSustain);
	g_console.CreateVar(context, "decay", m_distortDecay);
	g_console.CreateVar(context, "frequency", m_distortFrequency);
	g_console.CreateVar(context, "amplitude", m_distortAmplitude);
	g_console.CreateVar(context, "slices", m_distortSlices);
	g_console.CreateVar(context, "rings", m_distortRings);
	g_console.CreateVar(context, "aspectRatio", m_distortAspectRatio);
	g_console.CreateVar(context, "useOmega", m_distortUseOmega);
	g_console.CreateVar(context, "signalScale", m_distortSignalScale);
	g_console.CreateVar(context, "insideColor", &m_distortInsideColor);
	g_console.CreateVar(context, "outerColor", &m_distortOuterColor);
	g_console.CreateVar(context, "midColor", &m_distortMidColor);
	g_console.CreateVar(context, "spiralStrength", m_distortSpiralStrength);
	g_console.CreateVar(context, "spiralMaxAge", m_distortSpiralMaxAge);
	g_console.CreateVar(context, "spiralInitialAngle", m_distortSpiralInitialAngle);
	g_console.CreateVar(context, "fixedSpiralAngle", m_distortFixedSpiralAngle);
	g_console.CreateVar(context, "conicDepth", m_distortConicDepth);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ExplodeFXProperties::PostLoadSetup()
{
	VarObjProperties<ExplodeFX>::PostLoadSetup();

	const float div = 1.0f / 255.0f;
	m_lightColor *= div;
	m_distortInsideColor *= div;
	m_distortOuterColor *= div;
	m_distortMidColor *= div;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool ExplodeFXProperties::Go(const Vector3 &pos, const Vector3 &dir) const
{
	// Make the sounds
	if (m_sound1[0])
		Sfx::PlayStationary(m_sound1, pos, 1.0f, m_sound1Pitch);
	if (m_sound2[0])
		Sfx::PlayStationary(m_sound2, pos, 1.0f, m_sound2Pitch);

	// Tell the avatar
	if (m_avatarShake)
		g_avatarFX.NotifyExplosion(pos);

	// Check visibility
	const Cartesian4x4 &camera = g_cameraDirector.GetCameraMatrix();
	Vector3 inCamera = pos * camera;
	const bool onScreen = (inCamera.Z() < 0.0f) && (Math::Abs(inCamera.Z()) > Math::Abs(inCamera.X())) && (Math::Abs(inCamera.Z()) > Math::Abs(inCamera.Y()));

	// Particles
	if (m_particle[0] && onScreen)
		ParticleMgr__CreateEffect(m_particle, &pos, &dir, NULL, NULL, 0.0f, m_particleScale, 0);

	// Do they even want a light?
	if (m_wantLight && onScreen)
	{
		int howMany = g_sfx.CountByClass("light");
		const int maxLights = 2;
		if (howMany < maxLights)
		{
			SFXLight::Params params;

			params.priority = SFXLight::SFXLIGHT_LOW_PRIORITY;
			params.type = CLightRecord::LIGHT_OMNI;
			params.color = m_lightColor;
			params.attenuation = m_lightAttenuation;
			params.intensity = m_lightIntensity;
			params.ttl = m_lightDuration;

			SFXLight::Create(GetName(), pos, params);
		}
	}

	// Do the distortion
	const bool decentFrameRate = (g_timer.GetFilteredFrameSec() < 0.042f);
	const int fsdRunning = ValidSingleton(CFSDSystem) && g_fsdSystem.IsBusy();
	if (m_wantDistort && onScreen && decentFrameRate && !fsdRunning)
	{
		// Are there too many?
		int howMany = g_sfx.CountByClass("distort");
		const int maxDistorts = 2;
		if (howMany >= maxDistorts)
			g_sfx.DeleteOldestByClass("distort");

		SFXDistort::Params params;

		params.priority = SFXDistort::SFXDISTORT_LOW_PRIORITY;
		params.hasNormal = false;
		params.normal.Clear();
		params.frequency = m_distortFrequency;
		params.amplitude = m_distortAmplitude;
		params.distort.slices = m_distortSlices;
		params.distort.rings = m_distortRings;
		params.distort.aspectRatio = m_distortAspectRatio;
		params.distort.innerradius = m_distortInnerRadius;
		params.distort.outerradius = m_distortOuterRadius;
		params.distort.attacktime = m_distortAttack;
		params.distort.sustaintime = m_distortSustain;
		params.distort.decaytime = m_distortDecay;
		params.distort.useOmega = m_distortUseOmega;
		params.distort.signalScale = m_distortSignalScale;
		params.distort.insideColor = m_distortInsideColor;
		params.distort.outerColor = m_distortOuterColor;
		params.distort.midColor = m_distortMidColor;
		params.distort.spiralStrength = m_distortSpiralStrength;
		params.distort.spiralMaxAge = m_distortSpiralMaxAge;
		params.distort.spiralInitialAngle = m_distortSpiralInitialAngle;
		params.distort.fixedSpiralAngle = m_distortFixedSpiralAngle;
		params.distort.conicDepth = m_distortConicDepth;

		SFXDistort::Create(GetName(), pos, params);
	}

	// Work!
	return (m_sound1[0] || m_sound2[0] || m_particle[0] || m_wantLight || m_wantDistort || m_avatarShake);
}

/////////////////////////////////////////////////////////////////////////////
// Trigger the FX
/////////////////////////////////////////////////////////////////////////////
bool ExplodeFX::Trigger(
ActorHandle from,
int projectileType,
const Vector3 &pos,
const Vector3 &dir)
{
	SMunitionExplodeFX fx;
	fx.d_munitionID = projectileType;
	fx.d_pos = pos;
	fx.d_dir = dir;
	fx.d_id = m_properties.GetIndex();
	g_ttlMunition.SendExplodeFX(from, fx);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Run an explosion
/////////////////////////////////////////////////////////////////////////////
void ExplodeFXLibrary::HandleTrigger(
const SMunitionExplodeFX &fx)
{
	ExplodeFXProperties *props = (ExplodeFXProperties *)GetProperties(fx.d_id);
	ASSERT(props);
	if (props)
		props->Go(fx.d_pos, fx.d_dir);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ExplodeFXDependency::InitializeOnce()
{
	NewSingleton(ExplodeFXLibrary);
	g_explodeFXLibrary.InitializeOnce();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ExplodeFXDependency::TerminateOnce()
{
	g_explodeFXLibrary.TerminateOnce();
	DeleteSingleton(ExplodeFXLibrary);
}
