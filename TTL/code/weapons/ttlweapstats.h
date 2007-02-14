/////////////////////////////////////////////////////////////////////////////
// TTLWeaponStats data and derived class.
/////////////////////////////////////////////////////////////////////////////
#ifndef TTLWEAPSTATS_H
#define TTLWEAPSTATS_H

//#define DEBUG_WEAPONSTATS
#ifdef DEBUG_WEAPONSTATS
#include "cmdcon/cmdcon.h"
#endif

#include "components\weapons\weapstats.h"
#include "container\darray.h"

// Weapon class specific stats
class WeaponTypeStats
{
public:
	WeaponTypeStats() : Kills(0), DeathHolding(0), DeathBy(0) {};

	int Kills;			// How many kills using this weapon class
	int DeathHolding;	// How many deaths holding this weapon class
	int DeathBy;		// How many deaths caused by this weapon class
};

// TTL weapon specific statistics.
class TTLTrackedStats : public TrackedStats
{
public:
	TTLTrackedStats(int numTypes);
	virtual ~TTLTrackedStats();

	int HeadShots;			// How many head shots

	DynamicArray<WeaponTypeStats*> pTypeStats;
};

// Our derived weapon stat class.
class TTLWeaponStats : public WeaponStats
{
public:
	TTLWeaponStats(CCompActor &actor);
	virtual ~TTLWeaponStats();

	const char *ComponentName()
		{ return "TTLWeaponStats"; }

	virtual void Initialize();
	virtual int GetStat(int i_StatType, int i_WeaponType = -1);
	virtual void SetStat(int i_StatType, int i_Count) {};
	virtual void IncrementStat(int i_StatType) {};
	virtual void DecrementStat(int i_StatType) {};

	void IncrementHeadShots();
	void IncrementKills(int i_WeaponType);
	void IncrementDeathHolding(int i_WeaponType);
	void IncrementDeathBy(int i_WeaponType);

	// TTL specific weapon stat types
	enum TTLStatTypes
	{
		HEAD_SHOTS = NUM_STAT_TYPES,
		KILLS,
		DEATH_HOLDING,
		DEATH_BY,
		NUM_TTL_STAT_TYPES
	};

	// Stats can be grouped by weapon classes.
	enum WeaponStatTypes
	{
		WST_MELEE = 0,
		WST_GRENADE,
		WST_SPECIAL,
		WST_SUB_MACHINEGUN,
		WST_HEAVY_MACHINEGUN,
		WST_SHOTGUN,
		WST_NUM_WEAPON_TYPES
	};

	static TTLWeaponStats *GetComponent(ActorHandle actorHandle)
	{
		CCompActor *actor = CCompActor::FromHandle(actorHandle);
		if (!actor)
			return NULL;
		else
			return (TTLWeaponStats *)actor->GetComponentByName("TTLWeaponStats");
	}

private:
	virtual int GetNumStatTypes() { return(NUM_TTL_STAT_TYPES); }
	virtual int GetNumWeaponTypes() { return(WST_NUM_WEAPON_TYPES); }

	TTLTrackedStats* pStats;	// Pointer to our tracked stats

	// Message handlers
	REF_SEND_MESSAGE_HANDLER(RecordHeadShot, TTLWeaponStats)
		d_recordHeadShotHandler;
	REF_BASIC_MESSAGE_HANDLER(RecordKill, MESSAGE_VALUE, TTLWeaponStats)
		d_recordKillHandler;
};

#endif //TTLWEAPSTATS_H