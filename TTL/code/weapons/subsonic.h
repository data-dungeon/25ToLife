/////////////////////////////////////////////////////////////////////////////
// Projectile instantiation class for subsonic
/////////////////////////////////////////////////////////////////////////////
#ifndef SUBSONIC_H
#define SUBSONIC_H

#include "Components/Component.h"
#include "weapons/ttlweapon.h"
#include "weapons/ttlprojectile.h"
#include "weapons/aoeproj.h"

// Forwards
class SubsonicProjectile;

// The object factory
class SubsonicProjectileCreator : public ProjectileCreator
{
public:
	// Do the work
	SubsonicProjectileCreator();

	// Create
	Projectile *CreateInstance(const VarObjProperties<Projectile> &properties);

protected:
	ProjectileProperties *NewProperties(int index);
};

/////////////////////////////////////////////////////////////////////////////
// The properties
/////////////////////////////////////////////////////////////////////////////
class SubsonicProjectileProperties : public TTLProjectileProperties
{
public:
	SubsonicProjectileProperties(ProjectileCreator &creator, int index);
	virtual ~SubsonicProjectileProperties() {}

	// For parsing the data
	virtual void RegisterContext(const char *context);
	virtual void PostLoadSetup();

	// Launch offset
	float						m_launchOffset;

	// Energy
	float						m_energy;

	// Launch parameters
	float						m_launchSpeed;
	Vector3Packed			m_launchOmegaMin, m_launchOmegaMax;

	// Our geometry
	char 						m_inAirModel[24];

	// Sound while we are live!
	char						m_liveSound[24];
	char						m_liveParticleEmitter[24];

	// Sound parameteres
	float						m_impactSpeedDivisor;

	// fuse time
	float						m_fuse;

	// Impact info (if exceded, will explode)
	float						m_maxImpactSpeed;

	// Physics params
	Vector3Packed			m_gravity;
	float						m_thrustTime;
	float						m_thrust;
	float						m_mass;
	Vector3Packed			m_inertiaTensor;

	// Spawn on explode
	char						m_spawnProjectile[WEAPONSTRING_SIZE];
};

/////////////////////////////////////////////////////////////////////////////
// The mighty class
/////////////////////////////////////////////////////////////////////////////
class SubsonicProjectile : public TTLProjectile
{
public:
	SubsonicProjectile(const SubsonicProjectileProperties &properties);
	~SubsonicProjectile() {}

	// Fire!
	bool Fire(Weapon *weapon, const Vector3 &origin, const Vector3 &dir);

protected:
	// Alias our special properties
	const SubsonicProjectileProperties &m_ssProps;
};

class SubsonicFXComponent : public CActorComponent
{
public:
	SubsonicFXComponent(CCompActor &actor, const SubsonicProjectileProperties &props);
	~SubsonicFXComponent();

	const char *ComponentName()
		{ return "SubsonicFX"; }
	const char *ComponentClass()
		{ return "SubsonicFXComponent"; }

	// Wee!
	void Initialize();

	// Get the updates
	void EndUpdate(float i_deltaSec);

	const SubsonicProjectileProperties &GetProperties()
		{ return d_props; }

private:
	// Our properties
	const	SubsonicProjectileProperties 	&d_props;

	// The emitter
	EmitterWrapper					d_liveEmitter;

	// Sound stuff
	SoundHandle						d_liveSound;
};

class SubsonicActor : public CCompActor
{
public:
	SubsonicActor(const SubsonicProjectileProperties &props);
	~SubsonicActor();

	const char* GetClassName()
		{ return("Subsonic"); }

	uint8 Material(CCollisionPair* i_pair)
		{ return d_props.m_surfaceType; }

	bool Initialize(ActorHandle shooter, int weaponID,
							const Vector3 &origin, const Vector3 &dir,
							const Vector3 &velocity, const Vector3 &omega);

	// Get the updates
	void BeginUpdate();

	// Attempt update (duh)
	void AttemptUpdate(float i_deltaSec);

	// Get the updates
	void EndUpdate(float i_deltaSec);

	// A collision was detected.
	uint CollisionDetect(CStageEntity* i_entity, CCollisionEvent* i_event,
								bool i_mateDisplaced);

	// Accept a collision.
	void CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event);

	// Network
	virtual CNetActor* CreateNetActor(void);

private:
	// Our properties
	const	SubsonicProjectileProperties	&d_props;

	ActorHandle	 					d_shooter;
	int								d_weaponID;
	Vector3							d_shotOrigin;

	// Collision stuff
	Vector3							d_lastPos;

	// thrust stuff
	float								d_thrustTime;

	// Sound stuff
	SoundHandle						d_impactSound;

	// When do we die?
	float								d_die;

	// Our collision node
	CObbNode							d_obbNode;

	// Wee!
	Vector3 ComputeForces();
	void DoDamage(CActor *victim, const Vector3 &intersection, const Vector3 &normal, u8 surfaceType, u16 color);
	void BounceFX(SCollisionParticleData *impactFX, const Vector3 &intersection, const Vector3 &normal, float speed);
	bool Explode(const Vector3 &pos, const Vector3 &normal);

	REF_SEND_MESSAGE_HANDLER(StartCinematic, SubsonicActor) m_startCinematic;
	REF_BASIC_MESSAGE_HANDLER(CheckpointReset, MESSAGE_VALUE, SubsonicActor) m_checkpointReset;
};

#endif
