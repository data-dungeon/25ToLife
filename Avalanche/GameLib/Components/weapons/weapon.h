/////////////////////////////////////////////////////////////////////////////
// Weapon data and base class
/////////////////////////////////////////////////////////////////////////////
#ifndef WEAPON_H
#define WEAPON_H

#include "Components/weapons/aimsim.h"
#include "Components/weapons/aimsight.h"
#include "Components/weapons/projectile.h"
#include "game/animation/attachp.h"
#include "EngineHelper/simpleinst.h"
#include "Effects/widget.h"
#include "Environ/Munition.h"
#include "GameHelper/Message.h"

// Forwards
class Weapon;
class WeaponCreator;

// Base string size
#define WEAPONSTRING_SIZE			20
#define WEAPONBONESTRING_SIZE		12

// max models and attach points per weapon
#define WEAPON_MAX_MODELS 			2

// The attach points in the weapon
enum WeaponBone
{
	BONE_MUZZLE ,
	BONE_BREECH,
	BONE_SIGHT,

	WEAPONBONES
};

///////////////////////////////////////
// The model information for the weapon
///////////////////////////////////////
struct WeaponModel
{
	char 				name[WEAPONSTRING_SIZE];

	char 				attachBone[WEAPONBONESTRING_SIZE];
	int				geometry;

	bool				weaponAttachPoint[WEAPONBONES];
};

// The script driven definition of a weapon
class WeaponProperties : public VarObjProperties<Weapon>
{
public:
	WeaponProperties(WeaponCreator &creator, int index);
	virtual ~WeaponProperties();

	// For parsing the data
	virtual void RegisterContext(const char *context);

	virtual void PostLoadSetup();

	// Our classification
	char				m_type[WEAPONSTRING_SIZE];

	// What projectile does this hold?
	char 				m_projectile[WEAPONSTRING_SIZE];

	// What aim sight does this have?
	char				m_aimSight[WEAPONSTRING_SIZE];

	// Nice name
	char				m_displayName[WEAPONSTRING_SIZE];

	// Icon ID (for hud/etc)
	char				m_icon[WEAPONSTRING_SIZE];

	// The models
	WeaponModel		m_model[WEAPON_MAX_MODELS];
	int				m_models;

	// These must be implemented by your subclass
	virtual const WeaponAimProps *GetWeaponAimProps() const
		{ return NULL; }
	virtual int GetStances() const
		{ return 0; }

	// Let everyone see this
	static bool FixupModelName(char *model, int size = WEAPONSTRING_SIZE);
};

// The actual weapon
class Weapon
{
public:
	Weapon(const WeaponProperties &properties);
	virtual ~Weapon();

	// Access our properties
	const WeaponProperties &GetProperties() const
		{ return m_properties; }

	// What is our name?
	const char *Name() const;

	// Display name
	const char *DisplayName() const;

	// What is our icon ID
	const char *Icon() const
		{ return m_properties.m_icon; }

	// Get our general index
	int GetID() const
		{ return m_properties.GetIndex(); }

	// Get at the aimsim, cannot be null
	AimingSimulation &GetAimSim()
		{ return m_aimSim; }

	// Monkey with the aim
	virtual void SetAim(const Vector3 &eye, const Vector3 &dir)
		{ m_aimSim.Aim(eye, dir); }
	virtual void SetAim(const Vector3 &target, float deviance = 0.0f)
		{ m_aimSim.Aim(target, deviance); }
	virtual void ClearAim()
		{ m_aimSim.Aim(); }

	// What is our aimsight, could be null
	AimSight *GetSight();

	// Get our projectile, could be null
	Projectile *GetProjectile();

	// Set the owner
	virtual void SetOwner(CActor *owner);
	CActor *GetOwner()
		{ return CActor::FromHandle(m_owner); }

	// Ask for a specific subclass, returns NULL if not here
	virtual Weapon *GetInterface(const char *type);

	// Access weapon models
	inline int Models() const
		{ return m_properties.m_models; }
	const WeaponModel &GetModel(int index);

	// Attach/detach from an actor
	bool Attach(CActor &actor);
	bool Detach(CActor &actor);

	// Reset the state of this weapon
	virtual void Reset();

	// Does the weapon have ammo?
	virtual bool HasAmmo() = 0;

	// The states of the gun
	enum State
	{
		READY,
		NOTREADY,
		FIRING,
		RELOADING
	};
	virtual State GetState() const = 0;

	// Request a fire (returns true if it worked)
	virtual bool PullTrigger() = 0;
	virtual bool ReleaseTrigger() = 0;

	// Request a reload
	virtual bool Reload()
		{ return false; }

	// Cancel the current state if possible
	virtual bool Cancel()
		{ return false; }

	// Make gun not ready for a time
	virtual bool Unready(float time) = 0;

	// Activate/deactivate this weapon
	virtual void Activate();
	virtual void Deactivate();

	virtual void Show();
	virtual void Hide();
	virtual bool IsHidden()
		{ return m_isHidden; }

	virtual void HideLock( bool hide );

	// Call this once per frame
	virtual void Update();

	// What is the currently active model?
	virtual int GetActiveModel()
		{ return 0; }

	// Get some info about the gun
	// in world coordinates
	Vector3 GetWeaponInWorld(int model);
	const Graphics4x4 &GetWeaponToWorld(int model);
	Vector3 GetWeaponDir(int model);
	Graphics4x4 GetWeaponPlacement(int model);

	// Get some info about specific parts of the gun
	// in world coordinates
	Vector3 GetBoneInWorld(int model, WeaponBone forBone);
	const Graphics4x4 &GetBoneToWorld(int model, WeaponBone forBone);
	DirCos3x3 GetBoneOrientation(int model, WeaponBone forBone);
	Vector3 GetBoneDir(int model, WeaponBone forBone);

protected:
	// Our baes data
	const WeaponProperties	&m_properties;

	// Our owner
	ActorHandle					m_owner;

	// Get at the aimsim, cannot be null
	AimingSimulation			m_aimSim;

	// Our aimsight
	AimSight						*m_aimSight;

	// Our projectile
	Projectile					*m_projectile;

	// Are we hidden?  We need to remember, because CActor overwrites our hidden flag
	// at will
	bool							m_isHidden;

	// info about the weapon model
	t_Handle						m_dblHandle[WEAPON_MAX_MODELS];
	ts_Geometry					m_geometry[WEAPON_MAX_MODELS];
	AttachPoints				m_attachPoints[WEAPON_MAX_MODELS];
	AttachPointID				m_attachPoint[WEAPON_MAX_MODELS][WEAPONBONES];
	Graphics4x4					m_subTransform[WEAPON_MAX_MODELS][WEAPONBONES];
	bool LoadModels();

	struct BoneTransform
	{
		BoneID				boneID;
		Graphics4x4			boneToWorld;
	};
	BoneTransform	m_transform[WEAPON_MAX_MODELS];
	bool				m_bonesBound;

	void BindBones();
	void UpdateTransforms();
	Vector3 GetWeaponVelocity(int model);

public:
	// We appear in a list
	Weapon		*prev, *next;
};

#endif
