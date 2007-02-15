///////////////////////////////////////////////////////////////////////////////
// SpecialFX distortions!
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#include "Effects/sfx/sfxdistort.h"

int SFXDistort::s_distortions = 0;
const int SFXDistort::s_maxDistortions = 3;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXDistort::~SFXDistort()
{
	if (m_handle)
	{
		WorldSpaceDistort::Remove(m_handle);
		--s_distortions;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SFXDistort::ParamDefaults(
Params &params,
float size,
float attack,
float sustain,
float decay)
{
	// N8: could potentially compute the number of slices/rings depending on
	// N8: the screen space size of the effect...
	params.priority = SFXDISTORT_LOW_PRIORITY;
	params.hasNormal = false;
	params.normal.Clear();

	params.frequency = 25.0f;  // frequency of sin signal.
	params.amplitude = 1.0f;  	// amplitude of sin signal.

	params.distort.slices = 10;  // number of (pie) slices.
	params.distort.rings = 10;  // number of concentric rings.
	params.distort.aspectRatio = 1.0f;  // squash/stretch circle.
	params.distort.innerradius = size * 0.2f;  // "middle" ring of the circle.
	params.distort.outerradius = size;  // outer ring of the circle.
	params.distort.attacktime = attack;  // amount of time for effect to appear.
	params.distort.sustaintime = sustain;  // amount of time to hold effect.
	params.distort.decaytime = decay;  // amount of time to disappear.

	params.distort.useOmega = false;
	params.distort.signalScale = 3.0f;  // scale of signal.

	params.distort.insideColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	params.distort.outerColor.Set(1.0f, 1.0f, 1.0f, 0.0f);
	params.distort.midColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

	params.distort.spiralStrength = 0.0f;  // strength of the spiral vortex.
	params.distort.spiralMaxAge = 0.0f;  // strength of the spiral vortex.
	params.distort.spiralInitialAngle = 0.0f;  // start angle (twist) of the vortex.
	params.distort.fixedSpiralAngle = 0.0f;  // start angle (twist) of the vortex.
	params.distort.conicDepth = 0.0f;  // start angle (twist) of the vortex.
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXDistort *SFXDistort::Create(
const char *name,
const Vector3 &pos,
const Params &params)
{
	// Extreme case
	if (!ValidSingleton(SpecialFXSystem))
		return NULL;

	// Compute the ttl
	float ttl = params.distort.attacktime + params.distort.sustaintime + params.distort.decaytime;

	// Create the object
	SFXDistort *sfx = new SFXDistort(g_sfx, name, ttl);
	if (!sfx)
		return NULL;

	// Setup the sticky
	sfx->Sticky().SetInWorld(NULL, pos);

	// Create the emitter
	if (!sfx->Create(params))
	{
		delete sfx;
		return NULL;
	}

	g_sfx.Add(sfx);
	return sfx;
}

///////////////////////////////////////////////////////////////////////////////
// These create and add!
///////////////////////////////////////////////////////////////////////////////
SFXDistort *SFXDistort::Create(
const char *name,
CActor &actor,
const char *bone,
const Params &params)
{
	// Extreme case
	if (!ValidSingleton(SpecialFXSystem))
		return NULL;

	// Compute the ttl
	float ttl = params.distort.attacktime + params.distort.sustaintime + params.distort.decaytime;

	// Create the object
	SFXDistort *sfx = new SFXDistort(g_sfx, name, ttl);
	if (!sfx)
		return NULL;

	// Setup the sticky
	const Vector3 origin(0.0f, 1.0f, 0.0f);
	sfx->Sticky().SetInBody(&actor, origin);

	// Create the emitter
	if (!sfx->Create(params))
	{
		delete sfx;
		return NULL;
	}

	g_sfx.Add(sfx);
	return sfx;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SFXDistort::Destroy(SFXDistort *sfx)
{
	ASSERT(ValidSingleton(SpecialFXSystem));
	if (!ValidSingleton(SpecialFXSystem))
		return;

	g_sfx.Delete(sfx);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXDistort::SFXDistort(
SpecialFXSystem &sfx,
const char *name,
float ttl) :
SpecialFXSticky(sfx, name, ttl)
{
	m_priority = SFXDISTORT_LOW_PRIORITY;
	m_handle = 0;
	m_hasNormal = false;
}

/////////////////////////////////////////////////////////////////////////////
// Setup the emitter
/////////////////////////////////////////////////////////////////////////////
bool SFXDistort::Create(
const Params &params)
{
	// Can we fit?
	if (s_distortions == s_maxDistortions)
	{
		if (params.priority == SFXDISTORT_LOW_PRIORITY ||
				(params.priority == SFXDISTORT_LOW_PRIORITY && !Reclaim()))
			return false;
	}
	ASSERT(s_distortions < s_maxDistortions);

	// set this bad boy up
	m_priority = params.priority;
	m_hasNormal = params.hasNormal;
	m_normal = params.normal;
	Vector3 pos = m_sticky.GetLocation();
	Vector3 normal;
	if (m_hasNormal)
		normal = m_normal;
	else
		normal = FaceTheScreen(pos);
	SigGen *signal = new SigGen(SigGen::SIN, params.frequency, params.amplitude);

#if defined(PS2) || defined(_XBOX)
	m_handle = WorldSpaceDistort::AddCircle(pos, normal, params.distort, signal, false, false);
#else
	m_handle = 0;
#endif

	if (!m_handle)
		return false;

	++s_distortions;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool SFXDistort::Reclaim()
{
	SFXDistort *distort = (SFXDistort *)m_sfx.FindFirstByClass(Class());
	while (distort)
	{
		if (distort->GetPriority() == SFXDISTORT_LOW_PRIORITY)
		{
			m_sfx.Delete(distort);
			return true;
		}

		distort = (SFXDistort *)m_sfx.FindNextByClass(Class(), distort);
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SFXDistort::Update(
float now,
float deltaT)
{
	// Update the quantities
	if (m_handle)
	{
		Vector3 pos = m_sticky.GetLocation();
		Vector3 normal;
		if ( m_hasNormal )
			normal = m_normal;
		else
			normal = FaceTheScreen( pos );
		WorldSpaceDistort::MoveDistortion(m_handle, pos, normal);
	}

	SpecialFXSticky::Update(now, deltaT);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 SFXDistort::FaceTheScreen(
const Vector3 &pos)
{
	return (g_cameraDirector.CurrentCamera().Eye() - pos).Normalized();
}
