/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a supersonic projectile
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Effects/sfx/sfxpemit.h"
#include "Effects/sfx/sfxtrigger.h"
#include "Effects/sfx/sfxsys.h"
#include "weapons/emitproj.h"

// Create this
EmitterProjectileCreator g_emitterCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
EmitterProjectileCreator::EmitterProjectileCreator() :
ProjectileCreator("emitproj")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
Projectile *EmitterProjectileCreator::CreateInstance(
const VarObjProperties<Projectile> &properties)
{
	return new EmitterProjectile((const EmitterProjectileProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ProjectileProperties *EmitterProjectileCreator::NewProperties(
int index)
{
	return new EmitterProjectileProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
EmitterProjectileProperties::EmitterProjectileProperties(
ProjectileCreator &creator,
int index) :
AreaOfEffectProjectileProperties(creator, index)
{
	m_emitter[0] = '\0';
	m_ttl = 0.0f;

	m_triggerMessage[0] = '\0';
	m_triggerSize.Clear();
	m_triggerDuration = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void EmitterProjectileProperties::RegisterContext(
const char *context)
{
	// Call the parent
	AreaOfEffectProjectileProperties::RegisterContext(context);

	g_console.CreateVar(context, "emitter", m_emitter, sizeof(m_emitter) - 1);
	g_console.CreateVar(context, "ttl", m_ttl, CONVAR_REAL);

	g_console.CreateVar(context, "triggerMessage", m_triggerMessage, sizeof(m_triggerMessage) - 1);
	g_console.CreateVar(context, "triggerSize", &m_triggerSize);
	g_console.CreateVar(context, "triggerDuration",  m_triggerDuration);
}

/////////////////////////////////////////////////////////////////////////////
// Handle explosions!
/////////////////////////////////////////////////////////////////////////////
void EmitterProjectileProperties::HandleExplodeFX(
ActorHandle owner,
const SMunitionExplodeFX &fx)
{
	// Doh!
	if (!ValidSingleton(SpecialFXSystem))
		return;

	// Find the ground
	Vector3 ground;
	bool foundGround = FindGround(fx.d_pos, 3.0f, ground);
	if (!foundGround)
		return;

	// Are we inside of another one?
	SFXParticleEmitter *insideOf = InsideOf(m_emitter, fx.d_pos);
	if (insideOf && MergeSFX(insideOf, ground))
		return;

	// How many are in the word?
	int effectsInWorld = g_sfx.CountByClass("pemitter");
	const int maxInWorld = 5;
	if (effectsInWorld >= maxInWorld)
		g_sfx.DeleteOldestByClass("pemitter");

	// Create the emitter
	SFXParticleEmitter *emitter = NULL;
	if (m_emitter[0])
		emitter = SFXParticleEmitter::Create(m_emitter, ground, m_ttl);
	ASSERT(emitter);

	// Attach the trigger
	if (m_triggerMessage[0] && emitter)
	{
		CSticky sticky;
		sticky.SetInWorld(NULL, ground);
		SFXTriggerBox *trigger = new SFXTriggerBox(g_sfx, sticky, m_triggerMessage, m_triggerSize, m_triggerDuration);
		ASSERT(trigger);
		if (trigger)
			emitter->AddSibling(trigger);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool EmitterProjectileProperties::FindGround(
const Vector3 &pos,
float maxD,
Vector3 &ground)
{
	// What are we landing on...
	CLineEntity line;
	const float upStart = 0.25f;
	const Vector3 start = pos + Vector3(0.0f, upStart, 0.0f);
	const Vector3 down(0.0f, -1.0f, 0.0f);
	line.Line().SetOriginBasis(start, down);
	line.Line().SetMaxBoundary(maxD + upStart);
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	line.SetPositiveInclude(WORLD_CATEGORY);

	// Now cast it
	CLineQuery test;
	if (!test.Test(line))
		return false;

	ground = test.intersection;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SFXParticleEmitter *EmitterProjectileProperties::InsideOf(
const char *name,
const Vector3 &pos)
{
	SFXParticleEmitter *emitter = (SFXParticleEmitter *)g_sfx.FindFirstByName(name);
	while (emitter)
	{
		SFXTriggerBox *trigger = (SFXTriggerBox *)emitter->GetFirstSibling();
		if (trigger && trigger->Inside(pos))
			return emitter;

 		emitter = (SFXParticleEmitter *)g_sfx.FindNextByName(name, emitter);
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool EmitterProjectileProperties::MergeSFX(
SFXParticleEmitter *mergeWith,
const Vector3 &pos)
{
	ASSERT(mergeWith);
	SFXTriggerBox *trigger = (SFXTriggerBox *)mergeWith->GetFirstSibling();
	if (!trigger || !trigger->Active())
	{
		// this guy is dead anyway, kill him
		g_sfx.Delete(mergeWith);
		return false;
	}

	// Set the new position
	const Vector3 newPos = (mergeWith->GetPosition() + pos) * 0.5f;
	mergeWith->MoveTo(newPos);
	mergeWith->ResetTTL(m_ttl, false);
	trigger->ResetTTL(m_triggerDuration, false);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Reset due to a checkpoint
/////////////////////////////////////////////////////////////////////////////
void EmitterProjectileProperties::CheckpointReset() const
{
	TTLProjectileProperties::CheckpointReset();

	if (ValidSingleton(SpecialFXSystem))
		g_sfx.DeleteAllByClass("pemitter");
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
EmitterProjectile::EmitterProjectile(
const EmitterProjectileProperties &properties) :
AreaOfEffectProjectile(properties),
m_emitProps(properties)
{
}
































