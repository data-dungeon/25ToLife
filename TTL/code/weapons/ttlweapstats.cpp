/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "Components/ComponentCreator.h"
#include "weapons\ttlweapstats.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(TTLWeaponStats, TTLWeaponStats, COMPONENT_NEUTRAL_PRIORITY);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TTLTrackedStats::TTLTrackedStats(int numTypes) : 
	HeadShots(0)
{
	pTypeStats.Clear();
	for (int i = 0; i < numTypes; i++)
	{
		pTypeStats.Expand();
		pTypeStats[i] = new WeaponTypeStats();
	}
}

/////////////////////////////////////////////////////////////////////////////
TTLTrackedStats::~TTLTrackedStats()
{
	while (pTypeStats.Count())
	{
		delete pTypeStats[pTypeStats.Count() - 1];
		pTypeStats.Delete(pTypeStats.Count() - 1);
	}
	pTypeStats.Reset();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TTLWeaponStats::TTLWeaponStats(CCompActor &actor) : WeaponStats(actor)
{
	pStats = NULL;
}

/////////////////////////////////////////////////////////////////////////////
TTLWeaponStats::~TTLWeaponStats()
{
	if (pStats)
	{
		delete pStats;
		pStats = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void TTLWeaponStats::Initialize()
{
	WeaponStats::Initialize();

	if (actor.IsNetMaster())
		pStats = new TTLTrackedStats(WST_NUM_WEAPON_TYPES);

	d_recordHeadShotHandler.Initialize("WeaponStatHeadShot",
		MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
		actor.GetMessageOwner(), this);
	d_recordKillHandler.Initialize("WeaponStatKill",
		MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
		actor.GetMessageOwner(), this);
}

/////////////////////////////////////////////////////////////////////////////
int TTLWeaponStats::GetStat(int i_StatType, int i_WeaponType /*=-1*/)
{
	if (!actor.IsNetMaster())
		return 0;

	switch(i_StatType)
	{
	case HEAD_SHOTS:
		return (pStats->HeadShots);
		break;
	case KILLS:
		return (pStats->pTypeStats[i_WeaponType]->Kills);
		break;
	case DEATH_HOLDING:
		return (pStats->pTypeStats[i_WeaponType]->DeathHolding);
		break;
	case DEATH_BY:
		return (pStats->pTypeStats[i_WeaponType]->DeathBy);
		break;
	default:
		return WeaponStats::GetStat(i_StatType);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void TTLWeaponStats::IncrementHeadShots()
{
	if (!actor.IsNetMaster())
		return;

	pStats->HeadShots++;
#ifdef DEBUG_WEAPONSTATS
	g_console.Echo("TTLWeaponStats::IncrementHeadShots : %d", pStats->HeadShots); 
#endif
}

/////////////////////////////////////////////////////////////////////////////
void TTLWeaponStats::IncrementKills(int i_WeaponType)
{
	if (!actor.IsNetMaster())
		return;

	switch (i_WeaponType)
	{
	case WEAPONTYPE_MELEE:
		pStats->pTypeStats[WST_MELEE]->Kills++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementKills WST_MELEE : %d", pStats->pTypeStats[WST_MELEE]->Kills); 
#endif
		break;
	case WEAPONTYPE_LOBBED:
	case WEAPONTYPE_LAUNCHER:
		pStats->pTypeStats[WST_GRENADE]->Kills++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementKills WST_GRENADE : %d", pStats->pTypeStats[WST_GRENADE]->Kills); 
#endif
		break;
	case WEAPONTYPE_PISTOL:
	case WEAPONTYPE_SNIPER:
		pStats->pTypeStats[WST_SPECIAL]->Kills++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementKills WST_SPECIAL : %d", pStats->pTypeStats[WST_SPECIAL]->Kills); 
#endif
		break;
	case WEAPONTYPE_SUB:
		pStats->pTypeStats[WST_SUB_MACHINEGUN]->Kills++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementKills WST_SUB_MACHINEGUN : %d", pStats->pTypeStats[WST_SUB_MACHINEGUN]->Kills); 
#endif
		break;
	case WEAPONTYPE_ASSAULT:
		pStats->pTypeStats[WST_HEAVY_MACHINEGUN]->Kills++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementKills WST_HEAVY_MACHINEGUN : %d", pStats->pTypeStats[WST_HEAVY_MACHINEGUN]->Kills); 
#endif
		break;
	case WEAPONTYPE_SHOTGUN:
		pStats->pTypeStats[WST_SHOTGUN]->Kills++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementKills WST_SHOTGUN : %d", pStats->pTypeStats[WST_SHOTGUN]->Kills); 
#endif
		break;
	case WEAPONTYPE_NONE:
	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void TTLWeaponStats::IncrementDeathHolding(int i_WeaponType)
{
	if (!actor.IsNetMaster())
		return;

	switch (i_WeaponType)
	{
	case WEAPONTYPE_MELEE:
		pStats->pTypeStats[WST_MELEE]->DeathHolding++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathHolding WST_MELEE : %d", pStats->pTypeStats[WST_MELEE]->DeathHolding); 
#endif
		break;
	case WEAPONTYPE_LOBBED:
	case WEAPONTYPE_LAUNCHER:
		pStats->pTypeStats[WST_GRENADE]->DeathHolding++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathHolding WST_GRENADE : %d", pStats->pTypeStats[WST_GRENADE]->DeathHolding); 
#endif
		break;
	case WEAPONTYPE_PISTOL:
	case WEAPONTYPE_SNIPER:
		pStats->pTypeStats[WST_SPECIAL]->DeathHolding++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathHolding WST_SPECIAL : %d", pStats->pTypeStats[WST_SPECIAL]->DeathHolding); 
#endif
		break;
	case WEAPONTYPE_SUB:
		pStats->pTypeStats[WST_SUB_MACHINEGUN]->DeathHolding++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathHolding WST_SUB_MACHINEGUN : %d", pStats->pTypeStats[WST_SUB_MACHINEGUN]->DeathHolding); 
#endif
		break;
	case WEAPONTYPE_ASSAULT:
		pStats->pTypeStats[WST_HEAVY_MACHINEGUN]->DeathHolding++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathHolding WST_HEAVY_MACHINEGUN : %d", pStats->pTypeStats[WST_HEAVY_MACHINEGUN]->DeathHolding); 
#endif
		break;
	case WEAPONTYPE_SHOTGUN:
		pStats->pTypeStats[WST_SHOTGUN]->DeathHolding++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathHolding WST_SHOTGUN : %d", pStats->pTypeStats[WST_SHOTGUN]->DeathHolding); 
#endif
		break;
	case WEAPONTYPE_NONE:
	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void TTLWeaponStats::IncrementDeathBy(int i_WeaponType)
{
	if (!actor.IsNetMaster())
		return;

	switch (i_WeaponType)
	{
	case WEAPONTYPE_MELEE:
		pStats->pTypeStats[WST_MELEE]->DeathBy++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathBy WST_MELEE : %d", pStats->pTypeStats[WST_MELEE]->DeathBy); 
#endif
		break;
	case WEAPONTYPE_LOBBED:
	case WEAPONTYPE_LAUNCHER:
		pStats->pTypeStats[WST_GRENADE]->DeathBy++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathBy WST_GRENADE : %d", pStats->pTypeStats[WST_GRENADE]->DeathBy); 
#endif
		break;
	case WEAPONTYPE_PISTOL:
	case WEAPONTYPE_SNIPER:
		pStats->pTypeStats[WST_SPECIAL]->DeathBy++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathBy WST_SPECIAL : %d", pStats->pTypeStats[WST_SPECIAL]->DeathBy); 
#endif
		break;
	case WEAPONTYPE_SUB:
		pStats->pTypeStats[WST_SUB_MACHINEGUN]->DeathBy++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathBy WST_SUB_MACHINEGUN : %d", pStats->pTypeStats[WST_SUB_MACHINEGUN]->DeathBy); 
#endif
		break;
	case WEAPONTYPE_ASSAULT:
		pStats->pTypeStats[WST_HEAVY_MACHINEGUN]->DeathBy++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathBy WST_HEAVY_MACHINEGUN : %d", pStats->pTypeStats[WST_HEAVY_MACHINEGUN]->DeathBy); 
#endif
		break;
	case WEAPONTYPE_SHOTGUN:
		pStats->pTypeStats[WST_SHOTGUN]->DeathBy++;
#ifdef DEBUG_WEAPONSTATS
		g_console.Echo("TTLWeaponStats::IncrementDeathBy WST_SHOTGUN : %d", pStats->pTypeStats[WST_SHOTGUN]->DeathBy); 
#endif
		break;
	case WEAPONTYPE_NONE:
	default:
		break;
	}
}

void TTLWeaponStats::RecordHeadShot::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->IncrementHeadShots();
}

void TTLWeaponStats::RecordKill::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->IncrementKills((int)i_data);
}
