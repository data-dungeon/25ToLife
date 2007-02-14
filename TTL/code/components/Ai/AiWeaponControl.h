#ifndef AI_WEAPONCONTROL_H
#define AI_WEAPONCONTROL_H

#include "Components/Component.h"
#include "Math/Vector.h"
#include "components/Ai/AiPerception.h"
#include "weapons/aiweaphelper.h"

#define MAX_AI_WEAPONS 4
#define WEAPON_REACTION_TIME 0.5f

class CCompActor;
class ParamaterSet;
class CActor;
class Firearm;
class TTLWeapon;

class AiWeaponControl : public CActorComponent
{
public:
	AiWeaponControl( CCompActor &i_actor );
	~AiWeaponControl();

	const char *ComponentName()
		{ return "AiWeaponControl"; }

	// Initialize the component
	void Initialize();

	// reset the full state
	void Reset();

	// Update
	void Update(float frameSec);

	// Disable/eneable
	void SetWeaponControlEnabled(bool enabled);

	// Update the target aim
	void AimAtTarget();

	// Fire at will!
	void FireAtWill(bool toggle);

	// Force some firing
	void ForceSomeFiring(float seconds);

	// Select no weapon
	void SelectNoWeapon();

	// Set the trailing timer
	void SetTrailingTimer(float time)
		{ trailingTimer = time; }

	// What is the selected class of weapon
	WeaponClass GetSelectedClass()
		{ return weaponHelper.GetClass(); }

	// For hostage taking, I need to know if you have a pistol, and you need to use it
	bool SelectHostageWeapon(bool selectIt);

	// Immediate cease fire, no trailing shots
	void CeaseFire(float minInterludeTime = WEAPON_REACTION_TIME);

private:
	bool PrimaryFeasible(float distanceSqrd);
	bool SecondaryFeasible(float distanceSqrd);
	bool MeleeFeasible(float distanceSqrd);
	bool LobbedFeasible(float distanceSqrd);
	bool CheatForAmmo();

	bool					weaponControlEnabled;

	WeaponInventory	*weaponInventory;
	AiPerception		*aiPerception;
	AIWeaponHelper		weaponHelper;

	// States
	bool					fireAtWill;
	float					reactionTimer;
	float					trailingTimer;
	float					weaponChoiceTimer;
	float					nadeInterludeTimer;
	float					forceSomeFiring;

	// Per weapon parameters
	struct WeaponControl
	{
		int				burstCountMin, burstCountMax;
		float				gapTime;
		float				devianceMin, devianceMax;
	};
	WeaponControl			weaponControl[WEAPONTYPES];
	static WeaponControl	baseWeaponControl[WEAPONTYPES];

	// Utilities
	void DecideBestWeapon(float frameSec);
	void SelectWeapon(int weapon);
	void QueueFire(int bursts = 1);
	bool Dead();

	bool aimingAnim;
	float lobTimer;
	float gap;
	float burstModifier;
	const char *storeWeapon;
};

#endif // AI_WEAPONCONTROL_H
