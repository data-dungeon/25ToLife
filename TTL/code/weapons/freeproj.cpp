/////////////////////////////////////////////////////////////////////////////
// Car bomb is used to generate a projectile-like explosion
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Components/weapons/projlib.h"
#include "weapons/aoeproj.h"
#include "weapons/freeproj.h"

DeclareSingleton(FreeProjectile);
volatile FreeProjectileDependency g_FreeProjectileDependency;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FreeProjectile::FreeProjectile() :
m_list(8, 4, true)
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FreeProjectile::~FreeProjectile()
{
	for (int i = 0; i < m_list.Count(); i++)
		delete m_list[i].projectile;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool FreeProjectile::Explode(
const char *projectileName,
CActor *instigator,
CActor *generator,
const Vector3 &pos)
{
	// Wee!
	Instance *instance = Find(projectileName);
	if (!instance)
		return false;

	// This is very dangerous (this should be improved someday, but we
	// are upcasting without any real verification.)
	AreaOfEffectProjectile *projectile = (AreaOfEffectProjectile *)instance->projectile;
	ASSERT(strcmpi(projectile->GetProperties().GetClass(), "areaofeffect") == 0 ||
				strcmpi(projectile->GetProperties().GetClass(), "fireaoe") == 0 ||
				strcmpi(projectile->GetProperties().GetClass(), "shockaoe") == 0);

	// do the work
	projectile->SetOwner(instigator);
	projectile->SetGenerator(generator);
	Vector3 up(0.0f, 1.0f, 0.0f);
	return projectile->Fire(NULL, pos, up);
}

/////////////////////////////////////////////////////////////////////////////
// Fire shoots the projectile in the specified direction
/////////////////////////////////////////////////////////////////////////////
bool FreeProjectile::Fire(
const char *projectileName,
CActor *shooter,
const Vector3 &pos,
const Vector3 &dir)
{
	// Wee!
	Instance *instance = Find(projectileName);
	if (!instance)
		return false;

	// Do it!
	instance->projectile->SetOwner(shooter);
	return instance->projectile->Fire(NULL, pos, dir);
}

/////////////////////////////////////////////////////////////////////////////
// Fire specifially targets a victim.
/////////////////////////////////////////////////////////////////////////////
bool FreeProjectile::Fire(
const char *projectileName,
CActor *shooter,
CActor *victim,
const Vector3 &pos)
{
	if (!victim)
		return false;

	Vector3 dir = (victim->GetBodyInWorld() - pos).Normalized();
	return Fire(projectileName, shooter, pos, dir);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FreeProjectile::Instance *FreeProjectile::Find(
const char *name)
{
	// Already exists?
	for (int i = 0; i < m_list.Count(); i++)
	{
		if (strcmpi(name, m_list[i].name) == 0)
			return &m_list[i];
	}

	// Create it
	Instance instance;
	instance.projectile = (TTLProjectile *)g_projectileLib.CreateInstance(name);
	if (!instance.projectile)
		return NULL;
	strcpy(instance.name, name);

	// Add it
	int count = m_list.Count();
	m_list.Add(instance, count);
	return &m_list[count];
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void FreeProjectileDependency::InitializeEvery()
{
	NewSingleton(FreeProjectile);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void FreeProjectileDependency::TerminateEvery()
{
	DeleteSingleton(FreeProjectile);
}

