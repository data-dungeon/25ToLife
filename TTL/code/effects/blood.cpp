///////////////////////////////////////////////////////////////////////////////
// The class to make blood...
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "effects/blood.h"
#include "GameHelper/SharedBody.h"
#include "CollideHelper/ObbNode.h"

// This is hot!
DeclareSingleton(Blood);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Blood::Blood() :
m_bloodPool(false)
{
	m_enabled = true;

	m_minUVT = 1.5f;
	m_maxUVT = 2.0f;
	m_colorIntensity = 1.0f;
	m_minColorIntensity = 0.2f;
	m_maxColorIntensity = 1.0f;
	m_bloodAlpha = 255;

	m_bleedBaseIndex = 0;
	m_bleedEmitters = 0;
	m_bleedFXCount = 48;
	m_bleedMinSizeFactor = 0.25f;
	m_bleedBaseSize = 0.4f;
	m_bleedSizeVaryMin = 0.8f;
	m_bleedSizeVaryMax = 1.2f;
	m_bleedAtHealth = 0.4f;
	m_bleedMinRate = 10.0f,
	m_bleedMaxRate = 2.5f;
	m_bleedVariance = 0.33f;
	m_bleedFilterT = 0.1f;

	m_bleedRadiusDivisor = 6.0f;
	m_bleedImpactRangeMin = -1.0f;
	m_bleedImpactRangeMax = 1.0f;
	m_bleedImpactRadius = 0.5f;
	m_bleedImpactEnergyDivisor = 1500.0f;
	m_bleedImpactSpurtsMin = 2;
	m_bleedImpactSpurtsMax = 5;

	m_dieBaseIndex = 1;
	m_dieEmitters = 2;
	m_dieFXCount = 24;
	m_dieBaseSize = 1.0f;
	m_dieFilterT = 0.1f;
	m_bloodyFootsteps = 10;

	m_impactBaseIndex = 2;
	m_impactEmitters = 2;
	m_impactFXCount = 32;
	m_impactBaseSize = 0.25f;
	m_impactMaxSize = 0.5f;
	m_impactMaxRange = 4.0f;
	m_impactEnergyDivisor = 1.0f / 800.0f;
	m_impactFilterT = 0.1f;
	m_impactCone = 0.0f;

	m_ps2FXMultiply = 0.5f;

	for (int i = 0; i < MAX_BLEEDTYPES; i++)
		m_bleedEmitter[i] = NULL;

	for (int i = 0; i < MAX_DIETYPES; i++)
		m_dieEmitter[i] = NULL;

	for (int i = 0; i < MAX_IMPACTTYPES; i++)
		m_impactEmitter[i] = NULL;

	m_onBleed = 0;
	m_onDie = 0;
	m_onImpact = 0;

	strcpy(m_material, "blood");
	m_surfaceType = 0;

	m_checkpointReset.Initialize("KillAllAtCheckpoint",
											MESSAGE_HANDLE_IF_MASTER |	MESSAGE_HANDLE_IF_DUPLICA,
											g_world.GetMessageOwner(), this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Blood::~Blood()
{
	int i;
	for (i = 0; i < m_impactEmitters; i++)
		delete m_impactEmitter[i];

	for (i = 0; i < m_dieEmitters; i++)
		delete m_dieEmitter[i];

	for (i = 0; i < m_bleedEmitters; i++)
		delete m_bleedEmitter[i];
}

///////////////////////////////////////////////////////////////////////////////
// Initialize everything
///////////////////////////////////////////////////////////////////////////////
bool Blood::Load(
const char *fxDBL)
{
	// Create & execute the context
	const char *context = "blood";

	g_console.CreateVar(context, "minUVT", m_minUVT, CONVAR_REAL);
	g_console.CreateVar(context, "maxUVT", m_maxUVT, CONVAR_REAL);
	g_console.CreateVar(context, "colorIntensity", m_colorIntensity, CONVAR_REAL);
	g_console.CreateVar(context, "minColorIntensity", m_minColorIntensity, CONVAR_REAL);
	g_console.CreateVar(context, "maxColorIntensity", m_maxColorIntensity, CONVAR_REAL);
	g_console.CreateVar(context, "bloodAlpha", m_bloodAlpha);

	g_console.CreateVar(context, "bleedBaseIndex", m_bleedBaseIndex);
	g_console.CreateVar(context, "bleedEmitters", m_bleedEmitters);
	g_console.CreateVar(context, "bleedFXCount", m_bleedFXCount);
	g_console.CreateVar(context, "bleedBaseSize", m_bleedBaseSize, CONVAR_REAL);
	g_console.CreateVar(context, "bleedMinSizeFactor", m_bleedMinSizeFactor, CONVAR_REAL);

	g_console.CreateVar(context, "bleedSizeVaryMin", m_bleedSizeVaryMin, CONVAR_REAL);
	g_console.CreateVar(context, "bleedSizeVaryMax", m_bleedSizeVaryMax, CONVAR_REAL);

	g_console.CreateVar(context, "bleedAtHealth", m_bleedAtHealth, CONVAR_REAL);
	g_console.CreateVar(context, "bleedMinRate", m_bleedMinRate, CONVAR_REAL);
	g_console.CreateVar(context, "bleedMaxRate", m_bleedMaxRate, CONVAR_REAL);
	g_console.CreateVar(context, "bleedVariance", m_bleedVariance, CONVAR_REAL);
	g_console.CreateVar(context, "bleedFilterT", m_bleedFilterT, CONVAR_REAL);

	g_console.CreateVar(context, "bleedRadiusDivisor", m_bleedRadiusDivisor, CONVAR_INVREAL);
	g_console.CreateVar(context, "bleedImpactRangeMin", m_bleedImpactRangeMin, CONVAR_REAL);
	g_console.CreateVar(context, "bleedImpactRangeMax", m_bleedImpactRangeMax, CONVAR_REAL);
	g_console.CreateVar(context, "bleedImpactRadius", m_bleedImpactRadius, CONVAR_REAL);
	g_console.CreateVar(context, "bleedImpactEnergyDivisor", m_bleedImpactEnergyDivisor, CONVAR_INVREAL);
	g_console.CreateVar(context, "bleedImpactSpurtsMin", m_bleedImpactSpurtsMin);
	g_console.CreateVar(context, "bleedImpactSpurtsMax", m_bleedImpactSpurtsMax);

	g_console.CreateVar(context, "dieBaseIndex", m_dieBaseIndex);
	g_console.CreateVar(context, "dieEmitters", m_dieEmitters);
	g_console.CreateVar(context, "dieFXCount", m_dieFXCount);
	g_console.CreateVar(context, "dieBaseSize", m_dieBaseSize, CONVAR_REAL);
	g_console.CreateVar(context, "dieFilterT", m_dieFilterT, CONVAR_REAL);
	g_console.CreateVar(context, "bloodyFootsteps", m_bloodyFootsteps, CONVAR_REAL);

	g_console.CreateVar(context, "material", m_material, sizeof(m_material) - 1);

	g_console.CreateVar(context, "impactBaseIndex", m_impactBaseIndex);
	g_console.CreateVar(context, "impactEmitters", m_impactEmitters);
	g_console.CreateVar(context, "impactFXCount", m_impactFXCount);
	g_console.CreateVar(context, "impactBaseSize", m_impactBaseSize, CONVAR_REAL);
	g_console.CreateVar(context, "impactMaxSize", m_impactMaxSize, CONVAR_REAL);
	g_console.CreateVar(context, "impactEnergyDivisor", m_impactEnergyDivisor, CONVAR_INVREAL);
	g_console.CreateVar(context, "impactMaxRange", m_impactMaxRange, CONVAR_REAL);
	g_console.CreateVar(context, "impactFilterT", m_impactFilterT, CONVAR_REAL);
	g_console.CreateVar(context, "impactCone", m_impactCone, CONVAR_DEGREES);

	g_console.CreateVar(context, "ps2FXMultiply", m_ps2FXMultiply);

	g_console.ExecuteContext(context);

#ifndef INTERACTIVE_CONSOLE
	g_console.DeleteContext(context);
#endif //INTERACTIVE_CONSOLE

	// Load the dbl
	if (!m_decalSet.Load(fxDBL))
		return false;

	// The PS2 is slow
#ifdef PS2
	m_bleedFXCount = (int)((float)m_bleedFXCount * m_ps2FXMultiply);
	m_dieFXCount = (int)((float)m_dieFXCount * m_ps2FXMultiply);
	m_impactFXCount = (int)((float)m_impactFXCount * m_ps2FXMultiply);
#endif

	// Create the emitters
	int i;
	for (i = 0; i < m_bleedEmitters; i++)
	{
		m_bleedEmitter[i] = CreateEmitter(m_decalSet, m_bleedBaseIndex + i);
		if (m_bleedEmitter[i])
		{
			m_bleedEmitter[i]->Setup(m_bleedFXCount, -5);
			m_bleedEmitter[i]->SetFilter(m_bleedFilterT);
		}
	}

	for (i = 0; i < m_dieEmitters; i++)
	{
		m_dieEmitter[i] = CreateEmitter(m_decalSet, m_dieBaseIndex + i);
		if (m_dieEmitter[i])
		{
			m_dieEmitter[i]->Setup(m_dieFXCount, -5);
			m_dieEmitter[i]->SetFilter(m_dieFilterT);
		}
	}

	for (i = 0; i < m_impactEmitters; i++)
	{
		m_impactEmitter[i] = CreateEmitter(m_decalSet, m_impactBaseIndex + i);
		if (m_impactEmitter[i])
		{
			m_impactEmitter[i]->Setup(m_impactFXCount, -5);
			m_impactEmitter[i]->SetFilter(m_impactFilterT);
		}
	}

	// Look up our surface type
	m_surfaceType = g_surfaceType.GetValidDynamicIndex((char *)m_material, m_surfaceType, 0);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Is it time to bleed yet?
///////////////////////////////////////////////////////////////////////////////
bool Blood::TimeToBleed(
float health,
float &nextBleed)
{
	// Never time to bleed
	if (health > m_bleedAtHealth || Math::Zero(health))
		return false;

	// Time to bleed?
	float now = g_timer.GetEndSec();
	if (now < nextBleed)
		return false;

	// What is the bleed rate for thier health?
	float rate = Math::LinearMap(health, m_bleedAtHealth, 0.0f, m_bleedMinRate, m_bleedMaxRate);
	rate *= g_random.InRange(1.0f - m_bleedVariance, 1.0f + m_bleedVariance);
	nextBleed = now + rate;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Bleed (place a blood spatter on the ground underneath this point
// Quantity goes [0-1] for the size of the pool
///////////////////////////////////////////////////////////////////////////////
void Blood::Bleed(
CActor &bleeder,
float health,
const char *boneName)
{
	// Anything to do?
	if (!m_enabled || !m_bleedEmitters)
		return;

	// Where is the blood coming from
	Vector3 bone = GetBoneInWorld(bleeder, boneName);
	const AABB &aabb = bleeder.GetAABB();
	float range = Math::Max(aabb.XSize(), aabb.ZSize()) * m_bleedRadiusDivisor;
	Vector2 circle = g_random.InUnitCircle() * range;
   Vector3 from(bone.X() + circle.X(), bone.Y() + 1.0f, bone.Z() + circle.Y());
	float size = Math::LinearMap(health, m_bleedAtHealth, 0.0f, 0.25f, 1.0f);
	size *= g_random.InRange(1.0f - m_bleedVariance, 1.0f + m_bleedVariance);

	Bleed(from, size);
}

///////////////////////////////////////////////////////////////////////////////
// A character has died, place a pool of blood underneath them.
///////////////////////////////////////////////////////////////////////////////
void Blood::DeathPool(
CActor &corpse,
const char *boneName)
{
	// Anything to do?
	if (!m_enabled || !m_dieEmitter)
		return;

	// Where is the blood coming from
	Vector3 from = GetBoneInWorld(corpse, boneName);
	from.Y(from.Y() + 1.0f);

	// Find a spot on the ground
	Collide hit;
	if (!FindGround(from, hit))
		return;

	// place it
	const float radius = m_dieBaseSize;

	AnimatedDecalEmitter *emitter = NextDie();
	if (emitter)
	{
		float uvT[4];
		StartUV(uvT);
		if (emitter->Create(hit.point, hit.normal, radius,
		 						RandomTwist(), uvT, MakeIntensity(hit.color, (u8)m_bloodAlpha),
		 						DECAL_COPLANARCHECK_CHEAP))
			AddBloodPool(hit.point, hit.normal, radius);
	}
}

///////////////////////////////////////////////////////////////////////////////
// A character has been hit by a shot, place blood on the wall behind
///////////////////////////////////////////////////////////////////////////////
void Blood::Impact(
Vector3CRef impact,
Vector3CRef dir,
CActor &victim,
float energy)
{
	if (!m_enabled)
		return;

	// Bleed on a hit
	const AABB &aabb = victim.GetAABB();
	float range = Math::Max(aabb.XSize(), aabb.ZSize()) * m_bleedRadiusDivisor;
	float size = energy * m_bleedImpactEnergyDivisor;

	int spurts = g_random.InRange(m_bleedImpactSpurtsMin, m_bleedImpactSpurtsMax);
	for (int s = 0; s < spurts; s++)
	{
		Vector3 from = impact + dir * g_random.InRange(m_bleedImpactRangeMin, m_bleedImpactRangeMax);

		from.X(from.X() + g_random.InRange(-m_bleedImpactRadius, m_bleedImpactRadius));
		from.Y(from.Y() + 1.0f);
		from.Z(from.Z() + g_random.InRange(-m_bleedImpactRadius, m_bleedImpactRadius));

		Bleed(from, size);
	}

	// Find the spot in the world
	Collide hit;
	Vector3 perturb = g_random.InCone(dir, m_impactCone);
	if (!FindWorld(impact, perturb, m_impactMaxRange, hit))
		return;

	// place it
	float radius = Math::LinearMap(hit.d, 0.0f, m_impactMaxRange, m_impactBaseSize, m_impactMaxSize);
	radius *= energy * m_impactEnergyDivisor;

	// Anything to do?
	AnimatedDecalEmitter *emitter = NextImpact();
	if (emitter)
	{
		float uvT[4];
		StartUV(uvT);
		emitter->Create(hit.point, hit.normal, radius,
		 						RandomTwist(), uvT,
								MakeIntensity(hit.color, (u8)m_bloodAlpha),
								DECAL_COPLANARCHECK_CHEAP);
	}
}

///////////////////////////////////////////////////////////////////////////////
// A AOE impact has occured, bleed!
///////////////////////////////////////////////////////////////////////////////
void Blood::ImpactBleed(
CActor &victim,
const char *boneName,
float energy)
{
	if (!m_enabled)
		return;

	// Bleed on a hit
	Vector3 bone = GetBoneInWorld(victim, boneName);
	const AABB &aabb = victim.GetAABB();
	float range = Math::Max(aabb.XSize(), aabb.ZSize()) * m_bleedRadiusDivisor;
	float size = energy * m_bleedImpactEnergyDivisor;
	int spurts = g_random.InRange(m_bleedImpactSpurtsMin, m_bleedImpactSpurtsMax);
	for (int s = 0; s < spurts; s++)
	{
	   Vector3 from(bone.X() + g_random.InRange(range, range),
							bone.Y() + 1.0f,
							bone.Z() + g_random.InRange(range, range));
		Bleed(from, size);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Blood::Bleed(
Vector3CRef from,
float size)
{
	// Anything to do?
	if (!m_bleedEmitters)
		return;

	// Find a spot on the ground
	Collide hit;
	if (!FindGround(from, hit))
		return;

		// What emttiter to we use
	AnimatedDecalEmitter *emitter = NextBleed();
	if (emitter)
	{
		float uvT[4];
		StartUV(uvT);
		const float radius = m_bleedBaseSize * size;
		emitter->Create(hit.point, hit.normal, radius,
								RandomTwist(), uvT,
								MakeIntensity(hit.color, (u8)m_bloodAlpha),
								DECAL_COPLANARCHECK_CHEAP);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AnimatedDecalEmitter	*Blood::NextBleed()
{
	AnimatedDecalEmitter	*emitter = m_bleedEmitter[m_onBleed++];
	if (m_onBleed >= m_bleedEmitters)
		m_onBleed = 0;

	return emitter;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AnimatedDecalEmitter	*Blood::NextDie()
{
	AnimatedDecalEmitter	*emitter = m_dieEmitter[m_onDie++];
	if (m_onDie >= m_dieEmitters)
		m_onDie = 0;

	return emitter;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AnimatedDecalEmitter	*Blood::NextImpact()
{
	AnimatedDecalEmitter	*emitter = m_impactEmitter[m_onImpact++];
	if (m_onImpact >= m_impactEmitters)
		m_onImpact = 0;

	return emitter;
}

///////////////////////////////////////////////////////////////////////////////
// Find the ground
///////////////////////////////////////////////////////////////////////////////
bool Blood::FindGround(
Vector3CRef start,
Collide &hit)
{
	return FindWorld(start, c_down, 4.0f, hit);
}

///////////////////////////////////////////////////////////////////////////////
// find an intersection w/the world
///////////////////////////////////////////////////////////////////////////////
bool Blood::FindWorld(
Vector3CRef start,
Vector3CRef dir,
float maxRange,
Collide &hit)
{
	// Setup the query
	CLineEntity line;
	line.Line().SetOriginBasis(start, dir);
	line.Line().SetMaxBoundary(maxRange);
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	line.SetPositiveInclude(WORLD_CATEGORY);

	// Do the query
	g_collisionSystem.ResetStack();
	CLineQuery test;
	if (!test.Test(line))
		return false;

	// Is this an okay material?
	// Look it up!
	SCollisionParticleData *impactFX = g_collisionParticle.GetEntry(m_surfaceType, test.material);
	if (impactFX && impactFX->d_decalIndex < 0)
		return false;

	// Give them back the data
 	hit.point = test.intersection;
	hit.normal = test.normal;
	hit.d = test.d;
	hit.color = test.color;
	hit.material = test.material;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Convert colors
///////////////////////////////////////////////////////////////////////////////
ts_bRGBA Blood::MakeColor(
u16 color,
u8 alpha)
{
	ts_bRGBA c;

	c.R = ((color >> 0) & 0x1f) << 3;
	c.G = ((color >> 5) & 0x3f) << 2;
	c.B = ((color >> 11) & 0x1f) << 3;
	c.A = alpha;

	return c;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ts_bRGBA Blood::MakeIntensity(
u16 color,
u8 alpha)
{
	ts_bRGBA c;

	u8 green = ((color >> 5) & 0x3f) << 2;
	float intensity = Math::Clamp((float)green / 255.0f * m_colorIntensity, m_minColorIntensity, m_maxColorIntensity);
	c.R = c.G = c.B = (u8)(intensity * 255.0f);
	c.A = alpha;

	return c;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector3 Blood::GetBoneInWorld(
CActor &forWho,
const char *boneName)
{
	Vector3 pos;

	if (!boneName || !boneName[0])
		pos = forWho.GetBodyInWorld();
	else
	{
		BoneID boneID;
		int ret = boneGetBoneID(forWho.Instance()->Body(), &boneID, (char *)boneName);
		if (!ret)
			pos = forWho.GetBodyInWorld();
		else
		{
			boneGetStartPosition(forWho.Instance()->Body(), boneID, &pos);
			pos = pos * forWho.Instance()->Matrix();
		}
	}

	return pos;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Blood::AddBloodPool(
const Vector3 &pos,
const Vector3 &normal,
float size)
{
	// Delete the oldest
	if (m_bloodPool.Items() == m_dieFXCount * m_dieEmitters)
	{
		BloodPool *die = m_bloodPool.Head();
		m_bloodPool.Unlink(die);
		die->MarkForDeath();
	}

	// Make the new one
	BloodPool *actor = new BloodPool;
	if (actor)
	{
		ts_Geometry *geometry = g_loadSOM.Geometry("objects\\health_pkup.dbl");
		if (!geometry)
			delete actor;
		else
		{
			actor->SetGeometry(0, geometry);

			actor->Initialize(pos, normal, size, m_bloodyFootsteps);
			m_bloodPool.AddTail(actor);

			g_stage.AddProtagonist(actor);
			actor->SetDeleteWhenNoCast(true);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Blood::KillBloodPools()
{
	BloodPool *die = m_bloodPool.Head();
	while (die)
	{
		m_bloodPool.Unlink(die);
		die->MarkForDeath();
		die = m_bloodPool.Head();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AnimatedDecalEmitter *Blood::CreateEmitter(
DecalSet &decalSet,
int textureIndex)
{
#ifdef PS2
	DecalTextureLayer textureLayer;
	textureLayer.Default(textureIndex);
	return new AnimatedDecalEmitter(decalSet, &textureLayer, 1);
#else
	int textures = decalSet.Textures();
	int glossIndex = decalSet.Textures() - 1;
	int bumpIndex = textureIndex + textures / 2;

	DecalTextureLayer textureLayer[3];

	textureLayer[0].DefaultBump(bumpIndex);
	textureLayer[1].DefaultGloss(glossIndex);
	textureLayer[2].DefaultPostBump(textureIndex);

	return new AnimatedDecalEmitter(decalSet, textureLayer, 3);
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Blood::CheckpointReset::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	// Release all of our pools
	d_ref->KillBloodPools();

	// Do all the emitters
	int i;
	for (i = 0; i < d_ref->m_impactEmitters; i++)
	{
		if (d_ref->m_impactEmitter[i])
			d_ref->m_impactEmitter[i]->Clear();
	}

	for (i = 0; i < d_ref->m_dieEmitters; i++)
	{
		if (d_ref->m_dieEmitter[i])
			d_ref->m_dieEmitter[i]->Clear();
	}

	for (i = 0; i < d_ref->m_bleedEmitters; i++)
	{
		if (d_ref->m_bleedEmitter[i])
			d_ref->m_bleedEmitter[i]->Clear();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool BloodPool::Initialize(
const Vector3 &pos,
const Vector3 &normal,
float size,
int bloodyFootsteps)
{
	CProtagonist::Initialize();
	Category() |= NO_SUPPORT_CATEGORY | FIXED_CATEGORY;
	SetPositiveInclude(PLAYER_CATEGORY | AI_CATEGORY | DEBUG_DRAW_CATEGORY);
	d_interactionHint = HINT_IMPOTENT | HINT_JUGGERNAUT;

	// Save this for the message
	d_bloodyFootsteps = bloodyFootsteps;

	DirCos3x3 o;
	Vector3 dir(0.0f, 0.0f, 1.0f);
	Vector3 right = Vector3::Cross(dir, normal).Normalized();
	dir = Vector3::Cross(right, normal);
	o.SetRow0(right);
	o.SetRow1(normal);
	o.SetRow2(dir);
	Place(pos, o);

	// Make an obb!
	// Setup collision
	SetCollisionNode(&d_obbNode, SIMPLE, false);
	Vector3 sizeVector(size, 0.40f, size);
	d_obbNode.SetExtents(-sizeVector, sizeVector);

	// we'll never render, so remove from scene tree
	SceneMgr::RemoveInstance(Instance(0));
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BloodPool::CollisionAccept(
CStageEntity* i_entity,
CCollisionEvent* i_event)
{
	ASSERT(i_entity->IsA(CActor::ClassType()));
	g_messageDispatcher.SendMessageToActor("BloodStep",
															g_messageDispatcher.IntToData(d_bloodyFootsteps),
															GetHandle(), CActor::ToHandle((CActor*)i_entity));
}


