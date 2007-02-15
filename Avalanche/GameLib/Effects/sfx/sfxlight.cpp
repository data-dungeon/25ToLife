///////////////////////////////////////////////////////////////////////////////
// SpecialFX particle emitter!
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#include "Effects/sfx/sfxlight.h"

int SFXLight::s_lights = 0;
#ifdef PS2
const int SFXLight::s_maxLights = 1;
#else
const int SFXLight::s_maxLights = 3;
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXLight::~SFXLight()
{
	if (m_inLightSet)
	{
		g_lightSet.removeDynamicLight(m_light);
		m_light = NULL;
		--s_lights;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXLight *SFXLight::Create(
const char *name,
const Vector3 &pos,
const Params &params)
{
	// Extreme case
	if (!ValidSingleton(SpecialFXSystem))
		return NULL;

	// Create the object
	SFXLight *sfx = new SFXLight(g_sfx, name, params.ttl);
	if (!sfx)
		return NULL;

	// Setup the sticky
	sfx->Sticky().SetInWorld(NULL, pos);

	// Create the emitter
	if (!sfx->CreateLight(params))
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
SFXLight *SFXLight::Create(
const char *name,
CActor &actor,
const char *bone,
const Params &params)
{
	// Extreme case
	if (!ValidSingleton(SpecialFXSystem))
		return NULL;

	// Create the object
	SFXLight *sfx = new SFXLight(g_sfx, name, params.ttl);
	if (!sfx)
		return NULL;

	// Setup the sticky
	const Vector3 origin(0.0f, 1.0f, 0.0f);
	sfx->Sticky().SetInBody(&actor, origin);

	// Create the emitter
	if (!sfx->CreateLight(params))
	{
		delete sfx;
		return NULL;
	}

	g_sfx.Add(sfx);
	return sfx;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SFXLight::Destroy(SFXLight *light)
{
	ASSERT(ValidSingleton(SpecialFXSystem));
	if (!ValidSingleton(SpecialFXSystem))
		return;

	g_sfx.Delete(light);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXLight::SFXLight(
SpecialFXSystem &sfx,
const char *name,
float ttl) :
SpecialFXSticky(sfx, name, ttl)
{
	m_light = new CLightRecord;
	m_priority = SFXLIGHT_LOW_PRIORITY;
	m_inLightSet = false;
}

/////////////////////////////////////////////////////////////////////////////
// Setup the emitter
/////////////////////////////////////////////////////////////////////////////
bool SFXLight::CreateLight(
const Params &params)
{
	// Can we fit?
	if (s_lights == s_maxLights)
	{
		if (params.priority == SFXLIGHT_LOW_PRIORITY ||
				(params.priority == SFXLIGHT_LOW_PRIORITY && !Reclaim()))
			return false;
	}
	ASSERT(s_lights < s_maxLights);

	// set this bad boy up
	Vector4 color;
	color.X(params.color.X());
	color.Y(params.color.Y());
	color.Z(params.color.Z());
	color.W(0.0f);

	m_light->setColor(color);
	m_light->setType(params.type);
	m_light->setAttenuation(params.attenuation);
	g_lightSet.addDynamicLight(m_light);

	// These seem to work better after the add
	m_light->setIntensity(params.intensity);
	m_light->enableLight(true);

	// Make sure we set it's position
	m_light->setPosition(m_sticky.GetLocation());

	// Set our state
	m_inLightSet = true;
	m_priority = params.priority;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool SFXLight::Reclaim()
{
	SFXLight *light = (SFXLight *)m_sfx.FindFirstByClass(Class());
	while (light)
	{
		if (light->GetPriority() == SFXLIGHT_LOW_PRIORITY)
		{
			m_sfx.Delete(light);
			return true;
		}

		light = (SFXLight *)m_sfx.FindNextByClass(Class(), light);
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SFXLight::Update(
float now,
float deltaT)
{
	// Update the quantities
	if (m_inLightSet)
		m_light->setPosition(m_sticky.GetLocation());

	SpecialFXSticky::Update(now, deltaT);
}

