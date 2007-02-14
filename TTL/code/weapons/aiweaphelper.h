/////////////////////////////////////////////////////////////////////////////
// AI Weapon helper class!
/////////////////////////////////////////////////////////////////////////////
#ifndef AIWEAPHELPER_H
#define AIWEAPHELPER_H

#include "container/pool.h"
#include "container/dllist.h"
#include "weapons/ttlweapon.h"
#include "weapons/aiweaptarget.h"
#include "weapons/aiweapfire.h"

class AIWeaponHelper
{
public:
	AIWeaponHelper();
	~AIWeaponHelper();

	// Initialize stuff
	// We will work your inventory! (don't call things directly)
	void Initialize(WeaponInventory &inventory, CCompActor &shooter);

	// how many slots are there?
	int Slots();

	// Select a weapon. Pass -1 to select no weapon
	bool SelectWeapon(int slot);
	int SelectedWeapon();

	// Get info about the weapon. If slot = -1, use the current
	TTLWeapon *GetWeapon(int slot = -1);
	float GetRange(int slot = -1);
	float GetRateOfAttack(int slot = -1);
	bool HasAmmo(int slot = -1);
	float AmmoRatio(int slot = -1);
	bool CheatForMoreAmmo(int slot = -1);

	// What is the "class" of the weapon?
	WeaponClass GetClass(int slot = -1);

	// What is the type of the weapon
	WeaponType GetType(int slot = -1);

	// Get the subtype of the weapon
	WeaponSubType GetSubType(int slot = -1);

	// Select the "best" weapon (meaning it exists and has ammo)
	void SelectBest();

	// Set the current target
	void SetTarget(const Vector3 &target);
	void SetTarget(CActor &actor);
	void SetTarget(CActor &actor, const Vector3 &offset);
	void ClearTarget();

	// Could this weapon even hit based on range?
	bool InRange();

	// Setup a delay between firing
	bool SetFireGap(float gapTime);
	float FireGap()
		{ return m_fireGap; }

	// Fire the selected weapon!
	// Deviance if 0 will fire *exactly* at target (assuming viewIF is right)
	//		whereas 1.0 will miss by a lot
	// if rateOfAttack is zero it will use the rate of attack that is in the weapon
	bool Fire(int times = 1, float deviance = 0.0f, float rateOfAttack = 0.0f);

	// How many shots have been fired in this salvo?
	int ShotsFired();

	// Abort firing the current fire command (returns true if actually was firing)
	bool AbortFire();

	// How may fire events are queued?
	int FireQueued();

	// Clear the queue and abort firing
	void ClearFireQueue();

	// Call this once per frame to manage firing, etc.
	void Update();

	// Override nextfireevent time.  This is necessary so that gap time prevents them from firing, when starting.
	void SetNexFireEvent(float gameTime) {m_nextFireEvent = gameTime;}

private:
	// The information
	WeaponInventory		*m_inventory;
	CCompActor				*m_shooter;

	// The target!
	enum TargetType
	{
		AITARGET_NONE,
		AITARGET_POINT,
		AITARGET_ACTOR
	};
	TargetType				m_targetType;
	AIWeaponTarget			*m_target;

	// Set Aiming
	void SetAim(bool aim);

	// The current firing info
	struct FireEvent
	{
		int 			times;
		float 		deviance;
		float 		rateOfAttack;

		FireEvent	*prev, *next;
	};
	DoublyLinkedList<FireEvent>	m_fireEvent;
	Pool<FireEvent>					m_firePool;
	float									m_fireGap, m_nextFireEvent;
	AIWeaponFire						*m_fire;

	FireEvent *NewFireEvent()
		{ return m_firePool.New(); }
	void DeleteFireEvent(FireEvent *fireEvent);
	bool QueueFireEvent(FireEvent *fireEvent);
	FireEvent *NextFireEvent()
		{ return m_fireEvent.Head(); }
	bool CreateWeaponFire(const FireEvent &fireEvent);

	// Work the gap timer
	void ClearGapTimer();
	void SetGapTimer();
};

#endif
