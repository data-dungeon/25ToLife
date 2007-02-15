/////////////////////////////////////////////////////////////////////////////
// Aiming simulation
/////////////////////////////////////////////////////////////////////////////
#ifndef AIMSIM_H
#define AIMSIM_H

#include "EngineHelper/Singleton.h"
#include "container/Darray.h"

// These are the global properties for the aiming
class AimingSimProps
{
public:
	AimingSimProps();
  	~AimingSimProps();

	void Reset();

	// We have special events defined
	enum ImpulseType
	{
		IMPULSE_ADD = 0,
		IMPULSE_REPLACE
	};

	// Query the speed stuff
	float GetVelocityImpulse(Vector3CRef currentV, float multiplier) const;
	float GetMaxVelocityImpulse() const
		{ return m_vMaxImpulse; }

	// angular impulse
	float GetAngularImpulse(float deltaA, float multiplier) const;

	// Add an inpulse event
	void AddEvent(const char *eventName, float impulse, ImpulseType type = IMPULSE_ADD);

	// Find an event
	struct ImpulseEvent
	{
		char 			name[16];
		float			impulse;
		ImpulseType	type;
	};
	const ImpulseEvent *FindEvent(const char *lookFor) const;

	// Load the stuff from the var (also looks up your events!)
	bool LoadScript(const char *script);

private:
	// our Props
	Vector3Packed						m_maxV;
	float									m_vImpulse;
	float									m_vMaxImpulse;
	float									m_maxA;
	float									m_aImpulse;
	DynamicArray<ImpulseEvent>		m_impulseEvent;
};


// These are set per weapon
struct WeaponAimProps
{
	float 			minFireCone;
	float 			minAimFireCone;
	float				maxFireCone;

	float				movementImpulseMultiplier;
	float				turnImpulseMultiplier;

	float				aimingBonus;
	float				aimingImpulse;

	float				recoilImpulse;

	float				timeConstant;
};

#define MAX_AIMSIM_MATERIALS 	5

class AimingSimulation
{
public:
	AimingSimulation(const AimingSimProps &aimProps);
	~AimingSimulation() {}

	// Set the weapon info
	void SetWeapon(const WeaponAimProps *weaponProps, int stances);

	// Who is our owner?
	void SetOwner(CActor *owner)
		{ m_owner = CActor::ToHandle(owner); }

	// Reset everything
	void Reset();

	// The weapon should update this frequently
	void SetMuzzle(const Vector3 &muzzleEnd, const Vector3 &muzzleDir);
	const Vector3 &GetMuzzleEnd()
		{ return m_muzzleEnd; }
	const Vector3 &GetMuzzleDir()
		{ return m_muzzleDir; }

	// Set the aiming type
	enum CameraAimType
	{
		CAT_SIGHTLINE,
		CAT_TRIANGULATE
	};
	static void SetDefaultAimType(CameraAimType type)
		{ s_defaultAimType = type; }
	static CameraAimType GetDefaultAimType()
		{ return s_defaultAimType; }
	void SetAimType(CameraAimType type)
		{ m_aimType = type; }
	void RestoreAimType()
		{ SetAimType(GetDefaultAimType()); }

	// Register a callback for aiming pass-thru on triangulate
	// Return true for a penetration, false to stop
	typedef bool (*PenetrateCallback)(void *context, u8 material, int depth);
	void RegisterPenetrateCallback(PenetrateCallback callback, void *context)
		{ m_penetrateCB = callback; m_penetrateCBContext = context; }
	void ClearPenetrateCallback()
		{ m_penetrateCB = NULL; m_penetrateCBContext = NULL; }

	// Aim using the camera
	void Aim(const Vector3 &eye, const Vector3 &dir);

	// Aim at a target point in the world (for AI)
	void Aim(const Vector3 &target, float deviance = 0.0f);

	// Aim using the physical gun model (i.e. owner is dead)
	void Aim();

	// Get quantities about the aiming
	const Vector3 &AimOrigin();
	Vector3 AimDir();
	const Vector3 &AimPoint();
	const Vector3 &FilteredAimPoint();

	// Don't use this unless you want to ignore the
	// effects of this simulation
	const Vector3 &TrueAimDir();

	// Get information about what the aim is on
	const Vector3 &TargetPoint();
	CActor *TargetActor();
	float TargetRange();
	Vector3 TargetDir();

	// The material (-1 mean no material)
	int TargetMaterial();
	int *TargetMaterials(int &count);

	// What is the point on the bullet trajectory at d
	Vector3 PointOnTrajectory(float d);

	// Are they aiming? (i.e. zooming in, getting the bonus)
	void SetAiming(bool aiming);
	bool GetAiming()
		{ return m_aiming; }

	// What stance are we in?
	void SetStance(int newStance);
	int GetStance()
		{ return m_stance; }

	// Weapon was fired (add in recoil!)
	void Recoil();

	// Trigger an event
	bool TriggerEvent(const char *event, float strength = 1.0f);

	// call this to update (only once per frame please)
	void Update();

	// Get the sizes for the stuff
	float GetMaxSpread();
	float GetMinSpread();

	// Get the value of the aiming sim in 0-1
	float GetError()
		{ return Math::Clamp(m_error + m_vError); }

	// Setup the max scan range
	static void ScanRange(float scanRange)
		{ s_scanRange = scanRange; }
	static float ScanRange()
		{ return s_scanRange; }

private:
	// Our parent
	const AimingSimProps	&m_sim;

	// Who owns us?
	ActorHandle				m_owner;
	BoneID 					m_referenceBoneID;

	// Info for this weapon
	const WeaponAimProps	*m_weapon;
	int						m_maxStances;

	// Our current stance
	bool						m_aiming;
	int						m_stance;

	// Targeting info
	enum AimStyle
	{
		AIM_LOOKAT,
		AIM_ATPOINT,
		AIM_USEGUN
	};
	CameraAimType			m_aimType;
	AimStyle					m_aimStyle;
	Vector3					m_muzzleEnd, m_muzzleDir;
	Vector3					m_eye, m_dir;
	Vector3					m_target;
	float						m_deviance;
	Vector3 RawAimDir();

	// Stuff to support the penetrate callback
	PenetrateCallback		m_penetrateCB;
	void						*m_penetrateCBContext;

	// The computed info
	bool						m_aimValid, m_targetValid;
	Vector3					m_aimOrigin, m_aimDir;
	Vector3					m_aimPoint, m_hitPoint;
	ActorHandle				m_hitActor;
	float						m_hitRange;
	int						m_hitMaterial[MAX_AIMSIM_MATERIALS];
	int						m_hitMaterials;
	void InvalidateAim()
		{ m_aimValid = false; m_targetValid = false; }
	void ComputeAim();
	void ComputeTarget();

	// Update & angular stuff
	float						m_lastUpdate;
	int						m_onUpdate;
	Vector3					m_lastDir;
	float ComputeAngle(const Vector3 &currentDir, const Vector3 &lastDir);

	// Filtered aim point
	bool						m_wantFilteredAimPoint;
	bool						m_haveFilteredAimPoint;
	Vector3					m_filteredAimPoint;

	// Our current value
	float						m_error;
	float						m_vError;

	// Add in an impulse
	void AddImpulse(float impulse, AimingSimProps::ImpulseType type = AimingSimProps::IMPULSE_ADD);

	// Are we ready to do anything?
	inline bool Ready()
		{ return (m_weapon != NULL); }
	// Get the current weapon
	inline const WeaponAimProps &GetProps()
		{ return m_weapon[m_stance]; }

	// Return the MinFireCone
	float MinFireCone();

	// return max fire cone w/bonus
	float MaxFireCone();

	// How far do we compute target intersections
	static float s_scanRange;

	// The deafault aim type
	static CameraAimType	s_defaultAimType;
};

// Weapon singleton
DefineSingleton(AimingSimProps)
#define g_aimSimProps GetSingleton(AimingSimProps)

#endif
