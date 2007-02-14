/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a lobbed weapon
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "EngineHelper/timer.h"
#include "Effects/widget.h"
#include "Math/sfilter.h"
#include "Math/fastrand.h"
#include "Components/RigidBody.h"
#include "Components/weapons/projlib.h"
#include "CollideHelper/ObbNode.h"
#include "weapons/aoeproj.h"
#include "weapons/lobbed.h"
#include "environ/glass.h"
#include "weapons/ttlweapstats.h"

#ifdef NETZ
#include "NetZTTL/dolobbed.h"
#endif

// Create this
LobbedCreator g_lobbedCreator;

// How many of us are in the world?
int LobbedActor::s_active = 0;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
LobbedCreator::LobbedCreator() :
WeaponCreator("lobbed")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
Weapon *LobbedCreator::CreateInstance(
const VarObjProperties<Weapon> &properties)
{
	Lobbed *lobTest = NULL;

	return new Lobbed((const LobbedProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
// Subclass this to provide new
/////////////////////////////////////////////////////////////////////////////
WeaponProperties *LobbedCreator::NewProperties(
int index)
{
	return new LobbedProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
// Weee
/////////////////////////////////////////////////////////////////////////////
LobbedProperties::LobbedProperties(
WeaponCreator &creator,
int index) : TTLWeaponProperties(creator, index)
{
	m_pullTriggerSound[0] = '\0';
	m_lobSound[0] = '\0';

	m_rateOfFire = 0.0f;
	m_totalAmmo = 0;

	m_launchYFactor = 0.0f;
	m_launchHoldTime = 0.0f;

	m_launchSpeedMin = m_launchSpeedMax = 0.0f;

	m_launchOmegaMin.Clear();
	m_launchOmegaMax.Clear();

	m_inAirModel[0] = '\0';

	m_liveSound[0] = '\0';
	m_liveParticleEmitter[0] = '\0';

	m_impactSpeedDivisor = 1.0f;

	m_fuse = 0.0f;

	m_maxImpactSpeed = 0.0f;

	m_mass = 0.0f;
	m_inertiaTensor.Set(10.0f, 10.0f, 10.0f);
	m_material[0] = '\0';
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LobbedProperties::~LobbedProperties()
{
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void LobbedProperties::RegisterContext(
const char *context)
{
	// Call the parent
	TTLWeaponProperties::RegisterContext(context);

	g_console.CreateVar(context, "pullTriggerSound", m_pullTriggerSound, sizeof(m_pullTriggerSound) - 1);
	g_console.CreateVar(context, "lobSound", m_lobSound, sizeof(m_lobSound) - 1);

	g_console.CreateVar(context, "rateOfFire", m_rateOfFire, CONVAR_REAL);
	g_console.CreateVar(context, "totalAmmo", m_totalAmmo);

	g_console.CreateVar(context, "launchYFactor", m_launchYFactor);
	g_console.CreateVar(context, "launchHoldTime", m_launchHoldTime);

	g_console.CreateVar(context, "launchSpeedMin", m_launchSpeedMin);
	g_console.CreateVar(context, "launchSpeedMax", m_launchSpeedMax);

	g_console.CreateVar(context, "launchOmegaMin", &m_launchOmegaMin);
	g_console.CreateVar(context, "launchOmegaMax", &m_launchOmegaMax);

	g_console.CreateVar(context, "inAirModel", m_inAirModel, sizeof(m_inAirModel) - 1);

	g_console.CreateVar(context, "liveSound", m_liveSound, sizeof(m_liveSound) - 1);
	g_console.CreateVar(context, "liveParticleEmitter", m_liveParticleEmitter, sizeof(m_liveParticleEmitter) - 1);

	g_console.CreateVar(context, "impactSpeedDivisor", m_impactSpeedDivisor, CONVAR_INVREAL);

	g_console.CreateVar(context, "fuse", m_fuse);

	g_console.CreateVar(context, "maxImpactSpeed", m_maxImpactSpeed);

	g_console.CreateVar(context, "mass", m_mass);
	g_console.CreateVar(context, "inertiaTensor", &m_inertiaTensor);
	g_console.CreateVar(context, "material", m_material, sizeof(m_material) - 1);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LobbedProperties::PostLoadSetup()
{
	TTLWeaponProperties::PostLoadSetup();
	WeaponProperties::FixupModelName(m_inAirModel, sizeof(m_inAirModel));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Lobbed::Lobbed(
const LobbedProperties &properties) :
TTLWeapon(properties),
m_lProps(properties)
{
	// We must have a projectile to work
	ASSERT(m_projectile);

	// Assume not ammo until we are loaded
	m_ammo = 0;

	// Not in hand
	m_inHand = false;

	// Get ready to go
	Reset();
}

/////////////////////////////////////////////////////////////////////////////
// Clean up
/////////////////////////////////////////////////////////////////////////////
Lobbed::~Lobbed()
{
}

/////////////////////////////////////////////////////////////////////////////
// Set the owner
/////////////////////////////////////////////////////////////////////////////
void Lobbed::SetOwner(CActor *owner)
{
	// CAll the parent
	TTLWeapon::SetOwner(owner);

	// Init our handler
	if (owner)
		m_releaseHandler.Initialize("LobbedRelease", MESSAGE_HANDLE_IF_MASTER, owner->GetMessageOwner(), this, &Lobbed::ReleaseHandler);
}

/////////////////////////////////////////////////////////////////////////////
// Reset the state of this weapon
/////////////////////////////////////////////////////////////////////////////
void Lobbed::Reset()
{
	// Call the parent
	TTLWeapon::Reset();

	// Prepare to fire
	m_nextFire = 0;
	m_triggerDownTime = 0.0f;

	// We are ready!
	m_state = READY;

	m_aiThrowOverride = false;

	// Ready again
	m_waitingForRelease = false;
	m_releaseTimeout = 0.0f;

	// Not in hand
	PlaceInHand(false);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Lobbed::State Lobbed::GetState() const
{
	return m_state;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Lobbed::SetAmmo(
const char *type,
int amount)
{
	if (strcmpi(type, GetAmmoType()) != 0)
		return false;

	m_ammo = Math::Clamp(amount, 0, m_lProps.m_totalAmmo);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// A single integer code for ammo
// Per-class specific BUT -1 means full, 0 means none
/////////////////////////////////////////////////////////////////////////////
int Lobbed::CodifyAmmo()
{
	return m_ammo;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Lobbed::ApplyAmmoCode(int ammoCode)
{
	if (ammoCode == -1)
		m_ammo = m_lProps.m_totalAmmo;
	else
		m_ammo = Math::Clamp(ammoCode, 0, m_lProps.m_totalAmmo);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Lobbed::GetSplitAmmo(int& inBelt, int& inClip)
{
	inBelt = m_ammo;
	inClip = -1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Lobbed::SetSplitAmmo(int inBelt, int inClip)
{
	if (inBelt == -1)
		m_ammo = m_lProps.m_totalAmmo;
	else
		m_ammo = Math::Clamp(inBelt, 0, m_lProps.m_totalAmmo);
}

/////////////////////////////////////////////////////////////////////////////
// Request a fire (returns true if it worked)
/////////////////////////////////////////////////////////////////////////////
bool Lobbed::PullTrigger()
{
	// Can we fire?
	if (m_state != READY || m_waitingForRelease)
		return false;

	// State
	m_state = FIRING;

	// Save the time
	m_triggerDownTime = Now();

	// We are go
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Lobbed::ReleaseTrigger()
{
	// Are we even firing?
	if (m_state != FIRING)
		return false;

	// Weapon stats - avatar used weapon.
	// BET: Lobbed projectiles need to be tracked here since
	// AreaOfEffectProjectile::Fire covers all aoe damagers.
	TTLWeaponStats* stats = TTLWeaponStats::GetComponent(m_owner);
	if (stats)
		stats->IncrementUsed();

	// What is the power!
	float triggerDown = Now() - m_triggerDownTime;

	// What is the speed they want?
	if (!Math::Zero(m_lProps.m_launchHoldTime))
		m_releaseStrength = Math::Clamp(triggerDown / m_lProps.m_launchHoldTime);
	else
		m_releaseStrength = 1.0f;

	// controller throw
	m_aiThrowOverride = false;

	// Wait for the release
	m_waitingForRelease = true;
	m_releaseTimeout = Now() + 1.0f;

	// Launch!
	FireSound(m_lProps.m_pullTriggerSound);
	FireSound(m_lProps.m_lobSound);

	// Show it
	PlaceInHand(true);

	// Reset the state
	m_state = RELOADING;
	m_nextFire = Now() + m_lProps.m_rateOfFire;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Lobbed::Cancel()
{
	if (ShouldDeathRelease())
		DeathRelease();
	else if (m_state == FIRING)
	{
		m_state = READY;
		m_triggerDownTime = 0.0f;
		m_waitingForRelease = false;
		PlaceInHand(false);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// AI simplified fire (strength goes 0 - 1)
/////////////////////////////////////////////////////////////////////////////
Vector3 Lobbed::AIGetLaunchPos()
{
	return GetWeaponInWorld(0) + Vector3(0.0f, 0.35f, 0.0f);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Lobbed::AIFire(
const Vector3 &launchDir,
float strength)
{
	// Pull the trigger
	if (!PullTrigger())
		return false;

	// Force this
	m_releaseStrength = Math::Clamp(strength);

	// the ai ownz!
	m_aiThrowOverride = true;
	m_aiLaunchDir = launchDir;

	// Wait for the release
	m_waitingForRelease = true;
	m_releaseTimeout = Now() + 1.0f;

	// Launch!
	FireSound(m_lProps.m_pullTriggerSound);
	FireSound(m_lProps.m_lobSound);

	// Reset the state
	m_state = RELOADING;
	m_nextFire = Now() + m_lProps.m_rateOfFire;

	// What is this for?
	g_messageDispatcher.SendMessageToActor("BeginThrow", NULL, m_owner, m_owner);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Get the throw strength < 0.0 means not throwing
/////////////////////////////////////////////////////////////////////////////
float Lobbed::GetThrowStrength()
{
	if (m_state != FIRING)
		return -1.0f;
	else
	{
		float triggerDown = Now() - m_triggerDownTime;
		return Math::Clamp(triggerDown / m_lProps.m_launchHoldTime);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Request a reload
/////////////////////////////////////////////////////////////////////////////
bool Lobbed::Reload()
{
	// We don't reload
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// Make gun not ready for a time
/////////////////////////////////////////////////////////////////////////////
bool Lobbed::Unready(
float time)
{
	m_state = NOTREADY;
	m_nextFire = Now() + time;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Get the aim deviation (in radians)
/////////////////////////////////////////////////////////////////////////////
float Lobbed::GetAimDeviationRatio()
{
	return 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float Lobbed::MaxRange()
{
	// Assuming 45 degree launch on flat plane
	const float g = 9.8f;
	float range = Math::Square(m_lProps.m_launchSpeedMax) / g;

	// This is a bit of a hack, it will go farther than first bounce
	return 1.7f * range;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float Lobbed::RateOfAttack()
{
	return m_lProps.m_rateOfFire;
}

/////////////////////////////////////////////////////////////////////////////
// Activate/deactivate this weapon
/////////////////////////////////////////////////////////////////////////////
void Lobbed::Activate()
{
	// Call the parent
	TTLWeapon::Activate();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Lobbed::Deactivate()
{
	// Call the parent
	TTLWeapon::Deactivate();
}

/////////////////////////////////////////////////////////////////////////////
// Call this once per frame
/////////////////////////////////////////////////////////////////////////////
void Lobbed::Update()
{
	// Let the parent update
	TTLWeapon::Update();

#if 0
	// Are they being a pussy?
	if (Now() > m_triggerDownTime + Math::Max(2.5f, m_lProps.m_launchHoldTime * 2.0f))
		Cancel();
#endif

	// Did the release timeout
	if (m_waitingForRelease && Now() > m_releaseTimeout)
	{
 		m_waitingForRelease = false;
		PlaceInHand(false);
	}

	// Did they die with the grenade pin pulled?
	if (ShouldDeathRelease())
		DeathRelease();

	// Update to the ready state
	if (m_ammo == 0)
		m_state = NOTREADY;
	else if (m_state != FIRING && m_state != READY && Now() >= m_nextFire)
		m_state = READY;
}

/////////////////////////////////////////////////////////////////////////////
// The release
/////////////////////////////////////////////////////////////////////////////
void Lobbed::Release(
bool dead)
{
	// Are we even firing?
	if (!m_waitingForRelease)
		return;

	// Gather aiming info
#if 0
	Vector3 aimOrigin = m_aimSim.AimOrigin();
	Vector3 aimDir = m_aimSim.AimDir();
#else
	BoneID boneID = INVALID_BONEID;
	CActor *actor = CActor::FromHandle(m_owner);
	ASSERT(actor);
	if (!actor)
		return;

	int ret = boneGetBoneID(actor->Instance()->Body(), &boneID, "head");
	ASSERT(ret >=0);

	Graphics4x4 boneToModel;
	boneGetCurrentBoneToModel(actor->Instance()->Body(), boneID, &boneToModel);
	Graphics4x4 boneToWorld = boneToModel * actor->Instance()->Matrix();

	const Vector3 offset = Vector3(0.0f, -0.7f, -0.10f);
	Vector3 aimOrigin = boneToWorld.GetRow3() + offset.BasisTransform(boneToWorld);
	Vector3 aimDir = m_aimSim.TargetPoint() - aimOrigin;
	aimDir.SafeNormalize();
#endif

#if 0
	DrawUtility::Point(aimOrigin, DrawUtility::WHITE, 1.0f);
	DrawUtility::Point(m_aimSim.TargetPoint(), DrawUtility::YELLOW, 1.0f);
	DrawUtility::Normal(aimOrigin, aimDir * 2.0f, DrawUtility::CYAN);
#endif

	// What is the launch speed?
	float launchSpeed;
	if (dead)
		launchSpeed = 0.0f;
	else
	{
		if (!m_aiThrowOverride)
			launchSpeed = m_lProps.m_launchSpeedMin + (m_lProps.m_launchSpeedMax - m_lProps.m_launchSpeedMin) * m_releaseStrength;
		else
			launchSpeed = m_lProps.m_launchSpeedMax * m_releaseStrength;
	}

	// What is the launch direction?
	Vector3 launchDir;
	if (!m_aiThrowOverride)
		launchDir  = (aimDir + Vector3(0.0f, 1.0f, 0.0f) * m_lProps.m_launchYFactor).Normalized();
	else
		launchDir = m_aiLaunchDir;

	//DrawUtility::Line(aimOrigin, launchDir * 2.0f, DrawUtility::RED);

	// Munge in our velocity
	Vector3 v = launchDir * launchSpeed;
	const float velocityFactor = 0.75f;
	if (!m_aiThrowOverride && !dead)
	{
		v += (actor->GetVelocityInWorld() * velocityFactor);
		//DrawUtility::Line(aimOrigin, v, DrawUtility::WHITE);
	}

	// Figure out the launch omega
	Vector3 omega;
	omega.X(g_random.InRange(m_lProps.m_launchOmegaMin.X(), m_lProps.m_launchOmegaMax.X()));
	omega.Y(g_random.InRange(m_lProps.m_launchOmegaMin.Y(), m_lProps.m_launchOmegaMax.Y()));
	omega.Z(g_random.InRange(m_lProps.m_launchOmegaMin.Z(), m_lProps.m_launchOmegaMax.Z()));

	// Launch the actor
	Graphics4x4 body = GetWeaponToWorld(0);
#if 1
	body.PokeTranslation(aimOrigin);
#endif
	CreateActor(body, v, omega);
	PlaceInHand(false);

	// Ready again
	m_waitingForRelease = false;

#ifdef ILOVEGRENADES
	if (!m_aiThrowOverride)
#endif
	if (!Cheats::Enabled(Cheats::INFINITEAMMO, m_owner))
		--m_ammo;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Lobbed::ShouldDeathRelease()
{
	return (IsDead(m_owner) && m_waitingForRelease);
}

/////////////////////////////////////////////////////////////////////////////
// The release (whilst dead)
/////////////////////////////////////////////////////////////////////////////
void Lobbed::DeathRelease()
{
	ASSERT(IsDead(m_owner));

	// The agony!
	m_releaseStrength = 0.0f;

	// Wait for the release
	m_waitingForRelease = true;
	Release(true);

	// Reset the state
	m_state = RELOADING;
	m_nextFire = Now() + m_lProps.m_rateOfFire;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Lobbed::PlaceInHand(
bool shouldBeInHand)
{
	// Are we done?
	if (m_inHand == shouldBeInHand)
		return;
	m_inHand = shouldBeInHand;

	if (m_inHand)
	{
		// Show it
		CActor *ownerActor = CActor::FromHandle(m_owner);
		if (ownerActor)
			Attach(*ownerActor);
		Show();
	}
	else
	{
		Hide();
		CActor *ownerActor = CActor::FromHandle(m_owner);
		if (ownerActor)
			Detach(*ownerActor);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Lobbed::CreateActor(
const Graphics4x4 &placement,
const Vector3 &velocity,
const Vector3 &angularVelocity)
{
	// Create it
	LobbedActor *actor = new LobbedActor(*this, m_lProps);
	ASSERT(actor);
	if (!actor)
		return false;

	// set up the geometry
	const char *model = m_lProps.m_inAirModel[0] ? m_lProps.m_inAirModel : m_lProps.m_model[0].name;
	ts_Geometry *geometry = g_loadSOM.Geometry(model);
	if (!geometry)
	{
		delete actor;
		return false;
	}
	actor->SetGeometry(0, geometry);
	actor->Parm().Set("model_name", model);

	// Setup initial conditions
	LobbedActor::InitialState initialState;
	initialState.placement = placement;
	initialState.velocity = velocity;
	initialState.omega = angularVelocity;
	actor->Initialize(initialState);

	// put into the world - is not persistent
	g_stage.AddProtagonist(actor);
	actor->SetDeleteWhenNoCast(true);
	actor->DistributeNetActor(false);

	// We can now go away
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Lobbed::ReleaseHandler(void *i_data, ActorHandle i_sender, bool i_posted)
{
	// We we sure this is for us?
	ASSERT(i_sender == m_owner);
	if (i_sender == m_owner)
		Release();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LobbedFXComponent::LobbedFXComponent(
CCompActor &actor,
const LobbedProperties &props) :
CActorComponent(actor),
d_props(props)
{
	d_liveSound = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LobbedFXComponent::~LobbedFXComponent()
{
	Sfx::Stop(d_liveSound);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LobbedFXComponent::Initialize()
{
	CActorComponent::Initialize();

	// Create the attach points
	if (d_props.m_liveParticleEmitter)
	{
		// Bind our attach points (this is dumb!)
		ts_Geometry geometry;
		ASSERT(actor.Instance());
		if (actor.Instance())
		{
			actor.Instance()->GetGeometry(&geometry);
			d_attachPoints.BindSlowBoatToChina(geometry);
		}

		// The emitter
		AttachPointID ap = d_attachPoints.Find("muzzle");
		if (ap != INVALID_ATTACH_POINT)
			d_liveEmitter.Create(&actor, d_props.m_liveParticleEmitter, &d_attachPoints, ap);
		else
			d_liveEmitter.Create(&actor, d_props.m_liveParticleEmitter);
	}

	// Start up the band!
	if (d_props.m_liveSound[0])
		Sfx::Play(d_props.m_liveSound, actor, Sfx::ACTORPLAYPOS_BODY, &d_liveSound);
}

/////////////////////////////////////////////////////////////////////////////
// Get the updates
/////////////////////////////////////////////////////////////////////////////
void LobbedFXComponent::EndUpdate(
float i_deltaSec)
{
	// Call the parent
	CActorComponent::EndUpdate(i_deltaSec);

	// Update our emitter
	d_liveEmitter.Update();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LobbedActor::LobbedActor(
Lobbed &owner,
const LobbedProperties &props) :
d_props(props)
{
	d_lobbed = &owner;
	d_owner = d_lobbed->GetOwner();
	d_surfaceType = 0;
	d_die = 0.0f;
	d_impactSound = NULL;
	d_clearIgnore = 0;
	m_aiNotification = true;

	// one more in the world!
	++s_active;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LobbedActor::~LobbedActor()
{
	// How many of us are in the world?
	ASSERT(s_active > 0);
	--s_active;

	Sfx::Stop(d_impactSound);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uint8 LobbedActor::Material(
CCollisionPair* i_pair)
{
	return d_surfaceType;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool LobbedActor::Initialize(
const InitialState &initialState)
{
	// place in world
	Place(initialState.placement);
	d_lastPos = initialState.placement.GetRow3();

	// Setup collision
	SetCollisionNode(&d_obbNode, SIMPLE, false);
	d_obbNode.FitObbToActor();
	Vector3 size(d_obbNode.GetXSize(), d_obbNode.GetYSize(), d_obbNode.GetZSize());
	size *= 2.0f;
	Vector3 maxExtent = d_obbNode.GetCollisionInActor() + 0.5f * size;
	d_obbNode.SetExtents(maxExtent - size, maxExtent);

	if (!CCompActor::Initialize())
		return false;

	// Look up our surface type
	d_surfaceType = g_surfaceType.GetValidDynamicIndex((char *)d_props.m_material, d_surfaceType, 0);

	// Create the rigid body
	CRigidBody *body = (CRigidBody*)AttachComponent("RigidBody");
	ASSERT_PTR(body);

	body->SetMass(d_props.m_mass);
	body->SetInertia(d_props.m_inertiaTensor.X(), d_props.m_inertiaTensor.Y(), d_props.m_inertiaTensor.Z());

	body->SetCG(d_obbNode.GetCollisionInActor(), d_obbNode.GetCollisionToActor());
	body->SetBodyAcceleration(Vector3(0.0f, -9.81f, 0.0f), true);

	// Make sure we ignore outselves
	ASSERT(d_lobbed);
	if (d_lobbed)
		SetIgnoreEntity(d_lobbed->GetOwner());
	d_clearIgnore = g_timer.GetEndMS() + 750;

	DirCos3x3 m;
	m.Identity();
	body->SetCGToWorld(m);
	body->SetCGVelocityInWorld(initialState.velocity);
	body->SetAngVelocityInCG(initialState.omega);

	// Setup the FX component
	if (d_props.m_liveSound[0] || d_props.m_liveParticleEmitter[0])
	{
		LobbedFXComponent *fx = new LobbedFXComponent(*this, d_props);
		ASSERT_PTR(fx);
		AttachComponent(fx);
	}

	// Initialize all the components
	InitializeComponents();

	// Run Forrest, run!
	SetMaxUpdateSec(1.0f / 50.0f);

	// when do we die?
	if (!Math::Zero(d_props.m_fuse))
		d_die = g_timer.GetEndSec() + d_props.m_fuse;

	// Travis, where are my dogs?
	d_category |= (NO_SUPPORT_CATEGORY | NO_WEAPON_HIT_CATEGORY | SUBSONIC_CATEGORY);

	// Make sure we know if our root weapon died
	m_weaponDiedHandler.Initialize("WeaponDied",
											MESSAGE_HANDLE_IF_MASTER,
											GetMessageOwner(), this);

	// Only do this for non-networked
	if (!ValidSingleton(CSession) || !g_session.IsNetworked())
	{
		m_startCinematic.Initialize("StartCinematic", MESSAGE_HANDLE_IF_MASTER, GetMessageOwner(), this);
		m_checkpointReset.Initialize("KillAllAtCheckpoint", MESSAGE_HANDLE_IF_MASTER, GetMessageOwner(), this);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Get the updates
/////////////////////////////////////////////////////////////////////////////
void LobbedActor::EndUpdate(
float i_deltaSec)
{
	// Call the parent
	CCompActor::EndUpdate(i_deltaSec);

	// Clear the ignore
	if (d_clearIgnore && g_timer.GetEndMS() > d_clearIgnore)
	{
		d_clearIgnore = 0;
		SetIgnoreEntity(NULL);
	}

	// Time to die?
	if (!Math::Zero(d_props.m_fuse) && g_timer.GetEndSec() > d_die)
		Explode(GetBodyInWorld(), Vector3(0.0f, 1.0f, 0.0f));

	//once the nade gets close to explosion, tell people to run.
	if(m_aiNotification && (d_die - g_timer.GetEndSec()) < 0.8f)
	{
		g_messageDispatcher.SendMessageToAll( "LobbedExplosionWarning", NULL, GetHandle() );
		m_aiNotification = false;
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
		if (!d_ignoreEntity || test.target != d_ignoreEntity)
		{
			CRigidBody *body = (CRigidBody*)GetComponentByName("RigidBody");
			ASSERT_PTR(body);
			if (body)
			{
				Vector3 pos = test.intersection + test.normal * 0.20f;
				body->SetCGInWorld(pos);
				const Vector3 zero(0.0f, 0.0f, 0.0f);
				body->CollideWithMassive(test.intersection, test.normal, zero, -0.5f, 0.5f);
			}

			//Explode(test.intersection, test.normal);
		}
	}

	// next!
	d_lastPos = d_bodyInWorld;
}

/////////////////////////////////////////////////////////////////////////////
// A collision was detected.
/////////////////////////////////////////////////////////////////////////////
uint LobbedActor::CollisionDetect(
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
void LobbedActor::CollisionAccept(
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

	// How fast are we going
	float speed = GetVelocityInWorld().Length();
	if (!Math::Zero(d_props.m_maxImpactSpeed) && speed > d_props.m_maxImpactSpeed)
		Explode(GetBodyInWorld(), normal);

	// Look it up!
	SCollisionParticleData *impactFX = g_collisionParticle.GetEntry(querySurface, targetSurface);
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
CNetActor* LobbedActor::CreateNetActor(void)
{
#ifdef NETZ
	const int maxActorsToDistribute = 8;
	if (s_active <= maxActorsToDistribute)
		return(DOLobbed::Create());
	else
		return(NULL);
#else
	return(NULL);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Wee!
/////////////////////////////////////////////////////////////////////////////
bool LobbedActor::Explode(
const Vector3 &pos,
const Vector3 &normal)
{
	if (MarkedForDeath())
		return false;

	// Make us go away
	Hide();
	MarkForDeath();

	// What is the damage projectile?
	AreaOfEffectProjectile *projectile;
	if (d_lobbed)
		projectile = (AreaOfEffectProjectile *)d_lobbed->GetProjectile();
	else
	{
		projectile = (AreaOfEffectProjectile *)g_projectileLib.CreateInstance(d_props.m_projectile);
		projectile->SetOwner(CActor::FromHandle(d_owner));
	}

	// Spawn the damage projectile
	if (projectile)
	{
		projectile->SetGenerator(this);
		projectile->Fire(d_lobbed, pos, normal);
	}

	// The weapon that spawned us went away
	if (!d_lobbed)
		delete projectile;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbedActor::WeaponDied::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	Weapon *weapon = (Weapon *)i_data;
	ASSERT(weapon);

	// Is this our weapon?
	if (weapon == d_ref->d_lobbed)
		d_ref->d_lobbed = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LobbedActor::StartCinematic::HandleMessage(
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
void LobbedActor::CheckpointReset::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	// Make us go away
	d_ref->Hide();
	d_ref->MarkForDeath();
}


