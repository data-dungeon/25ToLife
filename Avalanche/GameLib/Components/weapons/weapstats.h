/////////////////////////////////////////////////////////////////////////////
// WeaponStats data and base class.
// Used to track statistics regarding weapons for an individual player.
/////////////////////////////////////////////////////////////////////////////
#ifndef WEAPSTATS_H
#define WEAPSTATS_H

//#define DEBUG_WEAPONSTATS
#ifdef DEBUG_WEAPONSTATS
#include "cmdcon/cmdcon.h"
#endif

#include "Components/Component.h"

// Forwards
class Weapon;

// Base statistics
class TrackedStats
{
public:
	TrackedStats() : Used(0), Hits(0) {};
	virtual ~TrackedStats() {};

	int Used;		// How many times it was used
	int Hits;		// How many times it hit someone
};

class WeaponStats : public CActorComponent
{
public:
	WeaponStats(CCompActor &actor);
	virtual ~WeaponStats(void);

	const char *ComponentName()
		{ return "WeaponStats"; }

	virtual void Initialize();
	virtual int GetStat(int i_StatType);
	virtual void SetStat(int i_StatType, int i_Count) {};
	virtual void IncrementStat(int i_StatType) {};
	virtual void DecrementStat(int i_StatType) {};

	void IncrementUsed(int count = 1);
	void IncrementHits(int count = 1);

	// Weapon stat types
	enum StatTypes
	{
		USED = 0,
		HITS,
		NUM_STAT_TYPES
	};

private:
	virtual int GetNumStatTypes() { return(NUM_STAT_TYPES); }

	TrackedStats* pStats;	// Pointer to our tracked stats.

	// Message handlers
	REF_BASIC_MESSAGE_HANDLER(RecordHit, MESSAGE_VALUE, WeaponStats)
		d_recordHitHandler;
};

#endif //WEAPSTATS_H
