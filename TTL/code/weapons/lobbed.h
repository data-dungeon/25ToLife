/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a lobbed weapon
/////////////////////////////////////////////////////////////////////////////
#ifndef LOBBED_H
#define LOBBED_H

#include "Components/Component.h"
#include "weapons/ttlweapon.h"

// The creator
class LobbedCreator : public WeaponCreator
{
public:
	// Do the work
	LobbedCreator();

	// Create
	Weapon *CreateInstance(const VarObjProperties<Weapon> &properties);

protected:
	// Subclass this to provide new
	WeaponProperties *NewProperties(int index);
};

// Our properties
class LobbedProperties : public TTLWeaponProperties
{
public:
	// Weee
	LobbedProperties(WeaponCreator &creator, int index);
	~LobbedProperties();

	// For parsing the data
	void RegisterContext(const char *context);
	void PostLoadSetup();

	// Sound parameteres
	char						m_pullTriggerSound[24];
	char 						m_lobSound[24];

	// Ammo and firing
	float 					m_rateOfFire;
	int						m_totalAmmo;

	// Launch info
	float						m_launchYFactor;
	float						m_launchHoldTime;

	float						m_launchSpeedMin, m_launchSpeedMax;
	Vector3Packed			m_launchOmegaMin, m_launchOmegaMax;

	// Our geometry
	char 						m_inAirModel[24];

	// Sound while we are live!
	char						m_liveSound[24];
	char						m_liveParticleEmitter[24];

	// Sound parameteres
	float						m_impactSpeedDivisor;

	// fuse time (all weapons should have a fuse)
	float						m_fuse;

	// Impact info (if exceded, will explode)
	float						m_maxImpactSpeed;

	// Physics params
	float						m_mass;
	Vector3Packed			m_inertiaTensor;
	char						m_material[WEAPONSTRING_SIZE];
};

// The actual implementation
class Lobbed : public TTLWeapon
{
public:
	Lobbed(const LobbedProperties &properties);
	~Lobbed();

	// Set the owner
	void SetOwner(CActor *owner);

	// Ammo
	int Ammo()
		{ return m_ammo; }
	int MaxAmmo()
		{ return m_lProps.m_totalAmmo; }
	bool SetAmmo(const char *type, int amount);

	// A single integer code for ammo
	// Per-class specific BUT -1 means full, 0 means none
	int CodifyAmmo();
	void ApplyAmmoCode(int ammoCode);
	void GetSplitAmmo(int& inBelt, int& inClip);
	void SetSplitAmmo(int inBelt, int inClip);

	// Reset the state of this weapon
	void Reset();

	// The state of the gun
	State GetState() const;

	// Request a fire (returns true if it worked)
	bool PullTrigger();
	bool ReleaseTrigger();
	bool Cancel();

	// AI simplified fire (strength goes 0 - 1)
	Vector3 AIGetLaunchPos();
	float AIGetMinLaunchSpeed()
		{ return m_lProps.m_launchSpeedMin; }
	float AIGetMaxLaunchSpeed()
		{ return m_lProps.m_launchSpeedMax; }
	bool AIFire(const Vector3 &launchDir, float strength);

	// Get the throw strength < 0.0 means not throwing
	float GetThrowStrength();

	// Request a reload
	bool Reload();

	// Make gun not ready for a time
	bool Unready(float time);

	// Get the aim deviation ([0.0 - 1.0])
	float GetAimDeviationRatio();

	// Other weapon info
	float MaxRange();
	float RateOfAttack();

	// Activate/deactivate this weapon
	void Activate();
	void Deactivate();

	// Call this once per frame
	void Update();

protected:
	// The Lobbeds propers
	const LobbedProperties	&m_lProps;

	// Our current state
	State							m_state;

	// Current ammo
	int							m_ammo;

	// when can we fire again?
	float							m_nextFire;

	// How long has the trigger been down?
	float							m_triggerDownTime;

	// Some AI stuff
	bool							m_aiThrowOverride;
	Vector3						m_aiLaunchDir;

	// The acutal release
	bool							m_waitingForRelease;
	float							m_releaseTimeout;
	float							m_releaseStrength;
	void Release(bool dead = false);
	bool ShouldDeathRelease();
	void DeathRelease();

	// Our own sophisticated show/hide
	bool	m_inHand;
	void PlaceInHand(bool shouldBeInHand);

	// Our message handler
	void ReleaseHandler(void* i_data, ActorHandle i_sender, bool i_posted);
	MESSAGE_VALUE_CallbackMessageHandler<Lobbed> m_releaseHandler;

	// Create the actor
	bool CreateActor(const Graphics4x4 &placement, const Vector3 &velocity, const Vector3 &angularVelocity);
};

class LobbedFXComponent : public CActorComponent
{
public:
	LobbedFXComponent(CCompActor &actor, const LobbedProperties &props);
	~LobbedFXComponent();

	const char *ComponentName()
		{ return "LobbedFX"; }
	const char *ComponentClass()
		{ return "LobbedFXComponent"; }

	// Wee!
	void Initialize();

	// Get the updates
	void EndUpdate(float i_deltaSec);

	const LobbedProperties &GetProperties()
		{ return d_props; }

private:
	// Our properties
	const	LobbedProperties 	&d_props;

	// Our attach points!
	AttachPoints				d_attachPoints;

	// The emitter
	EmitterWrapper				d_liveEmitter;

	// Sound stuff
	SoundHandle					d_liveSound;
};

class LobbedActor : public CCompActor
{
public:
	LobbedActor(Lobbed &owner, const LobbedProperties &props);
	virtual ~LobbedActor();

	const char* GetClassName() { return("Lobbed");}
	uint8 Material(CCollisionPair* i_pair);

	struct InitialState
	{
		Graphics4x4 placement;
		Vector3		velocity;
		Vector3		omega;
	};
	bool Initialize(const InitialState &initialState);

	// Get the updates
	void EndUpdate(float i_deltaSec);

	// A collision was detected.
	uint CollisionDetect(CStageEntity* i_entity, CCollisionEvent* i_event,
								bool i_mateDisplaced);

	// Accept a collision.
	void CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event);

	// Network
	virtual CNetActor* CreateNetActor(void);

	static int ActiveTotal()
		{ return s_active; }

private:
	// How many of us are in the world?
	static int					s_active;

	// Who are we attached to?
	Lobbed						*d_lobbed;
	ActorHandle					d_owner;

	// Our properties
	const	LobbedProperties 	&d_props;

	// Our collision node
	CObbNode		  				d_obbNode;

	// The surface
	int 							d_surfaceType;

	// Our attach points!
	AttachPoints				d_attachPoints;

	// When do we die?
	float							d_die;

	// We have to disable the ignore at somepoint
	uint32						d_clearIgnore;
	Vector3						d_lastPos;
	// Sound stuff
	SoundHandle					d_impactSound;

	// used so that a single alert message can be sent to AI
	bool							m_aiNotification;

	// Wee!
	bool Explode(const Vector3 &pos, const Vector3 &normal);

	// Build in something to handle the weapon dying
	REF_BASIC_MESSAGE_HANDLER(WeaponDied, MESSAGE_VALUE, LobbedActor) m_weaponDiedHandler;
	REF_SEND_MESSAGE_HANDLER(StartCinematic, LobbedActor) m_startCinematic;
	REF_BASIC_MESSAGE_HANDLER(CheckpointReset, MESSAGE_VALUE, LobbedActor) m_checkpointReset;
};

#endif
