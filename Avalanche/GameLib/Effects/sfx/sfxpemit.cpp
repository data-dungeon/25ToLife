///////////////////////////////////////////////////////////////////////////////
// SpecialFX particle emitter!
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#include "Effects/sfx/sfxsys.h"
#include "Effects/sfx/sfxpemit.h"

const Vector3 SFXParticleEmitter::s_up(0.0f, 1.0f, 0.0f);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXParticleEmitter::~SFXParticleEmitter()
{
	DestroyEmitter();
}

///////////////////////////////////////////////////////////////////////////////
// Reset the TTL
///////////////////////////////////////////////////////////////////////////////
void SFXParticleEmitter::ResetTTL(
float newTTL,
bool includeSiblings)
{
	// Do the real work
	SpecialFXSticky::ResetTTL(newTTL, includeSiblings);

	DestroyEmitter();
	CreateEmitter();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXParticleEmitter *SFXParticleEmitter::Create(
const char *emitterName,
const Vector3 &pos,
float ttl)
{
	// Extreme case
	if (!ValidSingleton(SpecialFXSystem))
		return NULL;

	// Create the object
	SFXParticleEmitter *sfx = new SFXParticleEmitter(g_sfx, emitterName, ttl);
	if (!sfx)
		return NULL;

	// Setup the sticky
	sfx->Sticky().SetInWorld(NULL, pos);

	// Create the emitter
	if (!sfx->CreateEmitter())
	{
		delete sfx;
		return NULL;
	}

	bool ret = g_sfx.Add(sfx);
	ASSERT(ret);
	return sfx;
}

///////////////////////////////////////////////////////////////////////////////
// These create and add!
///////////////////////////////////////////////////////////////////////////////
SFXParticleEmitter *SFXParticleEmitter::Create(
const char *emitterName,
CActor &actor,
const char *bone,
float ttl)
{
	// Extreme case
	if (!ValidSingleton(SpecialFXSystem))
		return NULL;

	// Create the object
	SFXParticleEmitter *sfx = new SFXParticleEmitter(g_sfx, emitterName, ttl);
	if (!sfx)
		return NULL;

	// Setup the sticky
	const Vector3 origin(0.0f, 1.0f, 0.0f);
	sfx->Sticky().SetInBody(&actor, origin);

	// Create the emitter
	if (!sfx->CreateEmitter())
	{
		delete sfx;
		return NULL;
	}

	bool ret = g_sfx.Add(sfx);
	ASSERT(ret);
	return sfx;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXParticleEmitter::SFXParticleEmitter(
SpecialFXSystem &sfx,
const char *name,
float ttl) :
SpecialFXSticky(sfx, name, ttl)
{
	m_handle = INVALID_HANDLE;
	m_pos.Clear();
	m_velocity.Clear();
}

/////////////////////////////////////////////////////////////////////////////
// Setup the emitter
/////////////////////////////////////////////////////////////////////////////
bool SFXParticleEmitter::CreateEmitter()
{
	m_pos = m_sticky.GetLocation();
	m_velocity = m_sticky.GetVelocity();
	bool stationary = Math::Zero(m_velocity.LengthSquared());

	Update(0.0f, 0.0f);
	m_handle = ParticleEmitter__CreateFromRecord(Name(),
																	&m_pos,
																	&m_velocity,
																	NULL,
																	false,
																	&s_up,
																	stationary);

	// Wee
	if (m_handle == INVALID_HANDLE)
		return false;

	// Setup the ttl
	if (m_ttl < 0.0f)
		m_ttl = ParticleEmitter__GetLifetime(m_handle) + 0.5f;
	else
		ParticleEmitter__SetLifetime(m_handle, m_ttl);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool SFXParticleEmitter::DestroyEmitter()
{
	if (m_handle == INVALID_HANDLE)
		return false;

	ParticleEmitter__Kill(m_handle);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SFXParticleEmitter::Update(
float now,
float deltaT)
{
	// Update the quantities
	m_pos = m_sticky.GetLocation();
	m_velocity = m_sticky.GetVelocity();

	if (m_handle != INVALID_HANDLE)
		ParticleEmitter__SetPosition(m_handle, m_pos);

	SpecialFXSticky::Update(now, deltaT);
}

