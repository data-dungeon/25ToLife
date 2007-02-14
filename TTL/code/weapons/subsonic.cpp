/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a subsonic projectile
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "EngineHelper/sfx.h"
#include "Environ/Munition.h"
#include "Components/RigidBody.h"
#include "CollideHelper/ObbNode.h"
#include "weapons/subsonic.h"
#include "environ/glass.h"
#include "avatar/pstat.h"
#include "weapons/ttlweapstats.h"

#ifdef NETZ
#include "NetZTTL/dosubsonic.h"
#endif

// Create this
SubsonicProjectileCreator g_subsonicCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
SubsonicProjectileCreator::SubsonicProjectileCreator() :
ProjectileCreator("subsonic")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
Projectile *SubsonicProjectileCreator::CreateInstance(
const VarObjProperties<Projectile> &properties)
{
	return new SubsonicProjectile((const SubsonicProjectileProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ProjectileProperties *SubsonicProjectileCreator::NewProperties(
int index)
{
	return new SubsonicProjectileProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SubsonicProjectileProperties::SubsonicProjectileProperties(
ProjectileCreator &creator,
int index) :
TTLProjectileProperties(creator, index)
{
	m_energy = 0.0f;

	m_launchOffset = 0.2f;

	m_launchSpeed = 0.0f;

	m_launchOmegaMin.Clear();
	m_launchOmegaMax.Clear();

	m_inAirModel[0] = '\0';

	m_liveSound[0] = '\0';
	m_liveParticleEmitter[0] = '\0';

	m_impactSpeedDivisor = 1.0f;

	m_fuse = 0.0f;

	m_maxImpactSpeed = 0.0f;

	m_thrustTime = 0.0f;
	m_thrust = 0.0f;
	m_gravity.Set(0.0f, -9.81f, 0.0f);

	m_mass = 0.0f;
	m_inertiaTensor.Set(10.0f, 10.0f, 10.0f);

	m_spawnProjectile[0] = '\0';
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void SubsonicProjectileProperties::RegisterContext(
const char *context)
{
	// Call the parent
	TTLProjectileProperties::RegisterContext(context);

	g_console.CreateVar(context, "energy", m_energy);

	g_console.CreateVar(context, "launchOffset", m_launchOffset);

	g_console.CreateVar(context, "launchSpeed", m_launchSpeed);
	g_console.CreateVar(context, "launchOmegaMin", &m_launchOmegaMin);
	g_console.CreateVar(context, "launchOmegaMax", &m_launchOmegaMax);

	g_console.CreateVar(context, "inAirModel", m_inAirModel, sizeof(m_inAirModel) - 1);

	g_console.CreateVar(context, "liveSound", m_liveSound, sizeof(m_liveSound) - 1);
	g_console.CreateVar(context, "liveParticleEmitter", m_liveParticleEmitter, sizeof(m_liveParticleEmitter) - 1);

	g_console.CreateVar(context, "impactSpeedDivisor", m_impactSpeedDivisor, CONVAR_INVREAL);

	g_console.CreateVar(context, "fuse", m_fuse);

	g_console.CreateVar(context, "maxImpactSpeed", m_maxImpactSpeed);

	g_console.CreateVar(context, "thrustTime", m_thrustTime);
	g_console.CreateVar(context, "thrust", m_thrust);
	g_console.CreateVar(context, "gravity", &m_gravity);

	g_console.CreateVar(context, "mass", m_mass);
	g_console.CreateVar(context, "inertiaTensor", &m_inertiaTensor);

	g_console.CreateVar(context, "spawnProjectile", m_spawnProjectile, sizeof(m_spawnProjectile) - 1);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SubsonicProjectileProperties::PostLoadSetup()
{
	TTLProjectileProperties::PostLoadSetup();
	WeaponProperties::FixupModelName(m_inAirModel, sizeof(m_inAirModel));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SubsonicProjectile::SubsonicProjectile(
const SubsonicProjectileProperties &properties) :
TTLProjectile(properties),
m_ssProps(properties)
{
}

/////////////////////////////////////////////////////////////////////////////
// Fire!
/////////////////////////////////////////////////////////////////////////////
bool SubsonicProjectile::Fire(
Weapon *weapon,
const Vector3 &origin,
const Vector3 &dir)
{
	CCombatant *actorOwner = CCombatant::FromHandle(m_owner);
	bool isAI = (actorOwner && actorOwner->GetIsNpc());

	// Figure out our real info
	Vector3 muzzle, muzzleDir;
	if (!weapon)
	{
		muzzle = origin;
		muzzleDir = dir;
	}
	else if (!isAI)
	{
		AimingSimulation &aim = weapon->GetAimSim();
		muzzle = aim.GetMuzzleEnd();
		Vector3 toTarget = aim.TargetPoint() - muzzle;
		float d = toTarget.Length();
		if (Math::Zero(d))
			return false;
		muzzleDir = toTarget/ d;
	}
	else
	{
		AimingSimulation &aim = weapon->GetAimSim();
		muzzle = aim.GetMuzzleEnd();
		Vector3 toTarget = aim.AimPoint() - muzzle;
		float d = toTarget.Length();
		if (Math::Zero(d))
			return false;
		muzzleDir = toTarget/ d;
	}

	// Create it
	SubsonicActor *actor = new SubsonicActor(m_ssProps);
	ASSERT(actor);
	if (!actor)
		return false;

	// set up the geometry
	ts_Geometry *geometry = g_loadSOM.Geometry(m_ssProps.m_inAirModel);
	if (!geometry)
	{
		delete actor;
		return false;
	}
	actor->SetGeometry(0, geometry);
	actor->Parm().Set("model_name", m_ssProps.m_inAirModel);

	// Setup initial conditions
	Vector3 velocity = m_ssProps.m_launchSpeed * muzzleDir;

	// Figure out the launch omega
	Vector3 omega;
	omega.X(g_random.InRange(m_ssProps.m_launchOmegaMin.X(), m_ssProps.m_launchOmegaMax.X()));
	omega.Y(g_random.InRange(m_ssProps.m_launchOmegaMin.Y(), m_ssProps.m_launchOmegaMax.Y()));
	omega.Z(g_random.InRange(m_ssProps.m_launchOmegaMin.Z(), m_ssProps.m_launchOmegaMax.Z()));

	// Okay, the poor bastard ai need as much help as they can get
	float launchOffset = m_ssProps.m_launchOffset;
	if (isAI)
		launchOffset += 2.0f;

	ASSERT(weapon);
	actor->Initialize(m_owner,
							weapon ? weapon->GetID() : -1,
							muzzle + muzzleDir * launchOffset, muzzleDir,
							velocity, omega);

	// put into the world - is not persistent
	g_stage.AddProtagonist(actor);
	actor->SetDeleteWhenNoCast(true);
	actor->DistributeNetActor(false);

	// Ignore random firing caused by the avatar's death.
	PlayerStats* pStats = (PlayerStats*)(CActor::FromHandle(m_owner))->GetInterface(PlayerStats::GetID());
	if (pStats && pStats->IsAlive())
	{
		// Weapon stats - avatar used weapon.
		TTLWeaponStats* stats = TTLWeaponStats::GetComponent(m_owner);
		if (stats)
			stats->IncrementUsed();
	}

	// We can now go away
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SubsonicFXComponent::SubsonicFXComponent(
CCompActor &actor,
const SubsonicProjectileProperties &props) :
CActorComponent(actor),
d_props(props)
{
	d_liveSound = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SubsonicFXComponent::~SubsonicFXComponent()
{
	Sfx::Stop(d_liveSound);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SubsonicFXComponent::Initialize()
{
	CActorComponent::Initialize();

	// Dthe emitter
	d_liveEmitter.Create(&actor, d_props.m_liveParticleEmitter);

	// Start up the band!
	if (d_props.m_liveSound[0])
		Sfx::Play(d_props.m_liveSound, actor, Sfx::ACTORPLAYPOS_BODY, &d_liveSound);
}

/////////////////////////////////////////////////////////////////////////////
// Get the updates
/////////////////////////////////////////////////////////////////////////////
void SubsonicFXComponent::EndUpdate(
float i_deltaSec)
{
	// Call the parent
	CActorComponent::EndUpdate(i_deltaSec);

	// Update our emitter
	d_liveEmitter.Update();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SubsonicActor::SubsonicActor(
const SubsonicProjectileProperties &props) :
d_props(props)
{
	d_shooter = (ActorHandle)INVALID_OBJECT_HANDLE;
	d_impactSound = NULL;
	d_die = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SubsonicActor::~SubsonicActor()
{
 	Sfx::Stop(d_impactSound);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool SubsonicActor::Initialize(
ActorHandle shooter,
int weaponID,
const Vector3 &origin,
const Vector3 &dir,
const Vector3 &velocity,
const Vector3 &omega)
{
	// save some of this for later
	d_shooter = shooter;
	d_weaponID = weaponID;
	d_shotOrigin = origin;
	d_lastPos = origin;

	// Thrust time
	d_thrustTime = d_props.m_thrustTime;

	// place in world
	DirCos3x3 o;
	o.BuildFromAxis(dir, 2, 1);
	Place(origin, o);

	// Setup collision
	SetCollisionNode(&d_obbNode, SIMPLE, false);
	d_obbNode.FitObbToActor();
	Vector3 size(d_obbNode.GetXSize(), d_obbNode.GetYSize(), d_obbNode.GetZSize());
	size *= 3.0f;
	Vector3 maxExtent = d_obbNode.GetCollisionInActor() + 0.5f * size;
	d_obbNode.SetExtents(maxExtent - size, maxExtent);

	if (!CCompActor::Initialize())
		return false;

	// Create the rigid body
	CRigidBody *body = (CRigidBody*)AttachComponent("RigidBody");
	ASSERT_PTR(body);

	body->SetMass(d_props.m_mass);
	body->SetInertia(d_props.m_inertiaTensor.X(), d_props.m_inertiaTensor.Y(), d_props.m_inertiaTensor.Z());

	body->SetCG(d_obbNode.GetCollisionInActor(), d_obbNode.GetCollisionToActor());
	body->SetBodyAcceleration(d_props.m_gravity, true);

	// Make sure we ignore outselves
	SetIgnoreEntity(CActor::FromHandle(d_shooter));

	DirCos3x3 m;
	m.Identity();
	body->SetCGToWorld(m);
	body->SetCGVelocityInWorld(velocity);
	body->SetAngVelocityInCG(omega);

	// Setup the FX component
	if (d_props.m_liveSound[0] || d_props.m_liveParticleEmitter[0])
	{
		SubsonicFXComponent *fx = new SubsonicFXComponent(*this, d_props);
		ASSERT_PTR(fx);
		AttachComponent(fx);
	}

	// Initialize all the components
	InitializeComponents();

	// Run Forrest, run!
	SetMaxUpdateSec(1.0f / 60.0f);

	// when do we die?
	if (!Math::Zero(d_props.m_fuse))
		d_die = g_timer.GetEndSec() + d_props.m_fuse;

	// Travis, where are my dogs?
	d_category |= (NO_SUPPORT_CATEGORY | NO_WEAPON_HIT_CATEGORY | SUBSONIC_CATEGORY);

	// Only do this for non-networked
	if (!ValidSingleton(CSession) || !g_session.IsNetworked())
	{
		m_startCinematic.Initialize("StartCinematic", MESSAGE_HANDLE_IF_MASTER, GetMessageOwner(), this);
		m_checkpointReset.Initialize("KillAllAtCheckpoint", MESSAGE_HANDLE_IF_MASTER, GetMessageOwner(), this);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SubsonicActor::BeginUpdate()
{
	if (!Math::Zero(d_thrustTime))
	{
		float speed = GetVelocityInWorld().Length();
		if (!Math::Zero(speed))
		{
			Vector3 dir = GetVelocityInWorld() / speed;
			Vector3 force = dir * d_props.m_thrust;
			CRigidBody* body = (CRigidBody*)GetComponentByName("RigidBody");
			ASSERT_PTR(body);
			body->ApplyForceCG(force);
		}
	}

	CCompActor::BeginUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// Attempt update (duh)
/////////////////////////////////////////////////////////////////////////////
void SubsonicActor::AttemptUpdate(
float i_deltaSec)
{
	// Call the parent
	CCompActor::AttemptUpdate(i_deltaSec);

	// Align it
	float length = d_velocityInWorld.Length();
	if (!Math::Zero(length))
	{
		Vector3 dir = d_velocityInWorld / length;
		d_bodyToWorld.BuildFromAxis(dir, 2, 1);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Get the updates
/////////////////////////////////////////////////////////////////////////////
void SubsonicActor::EndUpdate(
float i_deltaSec)
{
	// Call the parent
	CCompActor::EndUpdate(i_deltaSec);

	// Time to die?
	if (!Math::Zero(d_props.m_fuse) && g_timer.GetEndSec() > d_die)
	{
		Vector3 normal = GetVelocityInWorld().Normalized();
		Explode(d_bodyInWorld, normal);
	}

	// Check for an intersection
	CLineEntity entity;
	entity.Line().SetOriginEnd(d_lastPos, d_bodyInWorld);
	entity.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	entity.SetIgnoreEntity((CStageEntity*)this);
	entity.SetPositiveExclude(NO_WEAPON_HIT_CATEGORY);
	g_collisionSystem.ResetStack();
	CLineQuery test;
	if (test.Test(entity, CStageEntity::USE_COMPLEX))
	{
		// How fast are we going?
		float speed = GetVelocityInWorld().Length();
		bool bounce = Math::Zero(d_props.m_maxImpactSpeed) || (speed  < d_props.m_maxImpactSpeed);
		if (!bounce)
			Explode(test.intersection, GetVelocityInWorld().Normalized());
		else
		{
			// this is bad, the Rigid body didn't get the bounce
			BounceFX(g_collisionParticle.GetEntry(test.material, d_props.m_surfaceType), test.intersection, test.normal, speed);
		}

		// do damage based on a hit
		if (!bounce || test.actor)
			DoDamage(test.actor, test.intersection, test.normal, test.material, test.color);
	}

	// next!
	d_lastPos = d_bodyInWorld;

	// Update thrust time
	Math::MoveToZero(&d_thrustTime, i_deltaSec);
}

/////////////////////////////////////////////////////////////////////////////
// A collision was detected.
/////////////////////////////////////////////////////////////////////////////
uint SubsonicActor::CollisionDetect(
CStageEntity *i_entity,
CCollisionEvent *i_event,
bool i_mateDisplaced)
{
	if (i_entity->IsA(CGlass::ClassType()))
		return(CProtagonist::EFFECT_NONE);
	return(CCompActor::CollisionDetect(i_entity, i_event, i_mateDisplaced));
}

/////////////////////////////////////////////////////////////////////////////
// Accept a collision.
/////////////////////////////////////////////////////////////////////////////
void SubsonicActor::CollisionAccept(
CStageEntity *i_entity,
CCollisionEvent *i_event)
{
	// Call the parent
	CCompActor::CollisionAccept(i_entity, i_event);

	// Shatter glass
	if (i_entity->IsA(CGlass::ClassType()))
	{
		g_messageDispatcher.SendMessageToActor("Shatter", NULL, GetHandle(),
				((CActor*)i_entity)->GetHandle());
		return;
	}

	// What is the collision?
	Vector3 intersection, normal;
	CEventMiner::PlaneFit(i_event, intersection, normal);
	u8 querySurface, targetSurface;
	CEventMiner::SurfaceType(i_event, intersection, normal, querySurface, targetSurface);
	u16 color = 0; // Can't get this

	// How fast are we going to explode?
	float speed = GetVelocityInWorld().Length();
	bool bounce = Math::Zero(d_props.m_maxImpactSpeed) || (speed < d_props.m_maxImpactSpeed);
	if (!bounce)
		Explode(d_bodyInWorld, GetVelocityInWorld() / speed);
	else	// Look it up!
		BounceFX(g_collisionParticle.GetEntry(querySurface, targetSurface), intersection, normal, speed);

	// Is this an actor?
	if (i_entity->IsA(CActor::ClassType()))
		DoDamage((CActor *)i_entity, intersection, normal, targetSurface, color);
	else if (!bounce)
		DoDamage(NULL, intersection, normal, targetSurface, color);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SubsonicActor::DoDamage(
CActor *victim,
const Vector3 &intersection,
const Vector3 &normal,
u8 surfaceType,
u16 color)
{
	// Don't do any work if this doesn't do any damage on intersection
	if (Math::Zero(d_props.m_damage))
		return;

	CMunitionImpactSurface impact;

	impact.d_weaponID = d_weaponID;
	impact.d_firstShot = true;
	impact.d_deflected = false;
	impact.d_munitionID = d_props.GetIndex();
	impact.d_shotOrigin = d_shotOrigin;
	impact.d_victim = victim;
	impact.d_intersection = intersection;
	impact.d_surfaceNormal = normal;
	impact.d_surfaceMaterial = surfaceType;

#if 1
	impact.d_surfaceShade = 64;
#else
	if (!victim)
	{
		int r = ((color >> 0) & 0x1f) << 3;
		int g = ((color >> 5) & 0x3f) << 2;
		int b = ((color >> 11) & 0x1f) << 3;
		impact.d_surfaceShade = Math::Clamp((r + g + b) / 3, 0, 255);
	}
#endif

	// The real meat!
	impact.d_energy = d_props.m_energy;
	impact.d_damage = d_props.m_damage;

	// Wee!
	g_ttlMunition.SendMunitionImpact(d_shooter, impact);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SubsonicActor::BounceFX(
SCollisionParticleData *impactFX,
const Vector3 &intersection,
const Vector3 &normal,
float speed)
{
	if (!impactFX)
		return;

	// Figure out our scale factor
	float scale = Math::Clamp(speed * d_props.m_impactSpeedDivisor);

	// Play the sound
	if (impactFX->d_sound[0])
	{
		Sfx::Stop(d_impactSound);
		Sfx::PlayStationary(impactFX->d_sound, intersection, scale, 0.0f, &d_impactSound);
	}

	// Do the particle
	if (impactFX->d_particle[0])
	{
		const float offset = 0.02f;
		Vector3Padded pos = intersection + normal * offset;
		Vector3Padded velUnitVector = normal;
		const int randomScalePercent = 15;
		ParticleMgr__CreateEffect(impactFX->d_particle, &pos,
											&velUnitVector, NULL, NULL,
											0.0f, impactFX->d_particleScale * scale, randomScalePercent);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CNetActor* SubsonicActor::CreateNetActor(void)
{
#ifdef NETZ
	return(DOSubsonic::Create());
#else
	return(NULL);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Wee!
/////////////////////////////////////////////////////////////////////////////
bool SubsonicActor::Explode(
const Vector3 &pos,
const Vector3 &normal)
{
	if (MarkedForDeath())
		return false;

	// Make us go away
	Hide();
	MarkForDeath();

	// Spawn the damage projectile
	if (!d_props.m_spawnProjectile[0])
		return true;

	AreaOfEffectProjectile *spawn = (AreaOfEffectProjectile *)g_projectileLib.CreateInstance(d_props.m_spawnProjectile);
	if (spawn)
	{
		spawn->SetGenerator(this);
		spawn->SetOwner(CActor::FromHandle(d_shooter));
		spawn->Fire(NULL, pos, normal);
		delete spawn;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SubsonicActor::StartCinematic::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	// Make us go away
	d_ref->Hide();
	d_ref->MarkForDeath();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SubsonicActor::CheckpointReset::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	// Make us go away
	d_ref->Hide();
	d_ref->MarkForDeath();
}









