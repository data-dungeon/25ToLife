/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "Components/ComponentsPCH.h"
#include "Components/ComponentCreator.h"
#include "Components/weapons/weapstats.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(WeaponStats, WeaponStats, COMPONENT_NEUTRAL_PRIORITY);

/////////////////////////////////////////////////////////////////////////////
WeaponStats::WeaponStats(CCompActor &actor) : CActorComponent(actor)
{
	pStats = NULL;
}

/////////////////////////////////////////////////////////////////////////////
WeaponStats::~WeaponStats(void)
{
	if (pStats)
	{
		delete pStats;
		pStats = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void WeaponStats::Initialize()
{
	if (actor.IsNetMaster())
		pStats = new TrackedStats();

	d_recordHitHandler.Initialize("WeaponStatHit",
		MESSAGE_HANDLE_IF_MASTER,
		actor.GetMessageOwner(), this);
}

/////////////////////////////////////////////////////////////////////////////
int WeaponStats::GetStat(int i_StatType)
{
	switch(i_StatType)
	{
	case USED:
		return (pStats->Used);
		break;
	case HITS:
		return (pStats->Hits);
		break;
	default:
		return (0);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void WeaponStats::IncrementUsed(int count)
{
	if (!actor.IsNetMaster())
		return;

	pStats->Used += count;
#ifdef DEBUG_WEAPONSTATS
	g_console.Echo("WeaponStats::IncrementUsed : %d", pStats->Used);
#endif
}

/////////////////////////////////////////////////////////////////////////////
void WeaponStats::IncrementHits(int count)
{
	if (!actor.IsNetMaster())
		return;

	pStats->Hits += count;
#ifdef DEBUG_WEAPONSTATS
	g_console.Echo("WeaponStats::IncrementHits : %d", pStats->Hits);
#endif
}

void WeaponStats::RecordHit::HandleMessage(void* i_data,
															 ActorHandle i_sender, bool i_posted)
{
	int salvoSize = CMessageDispatcher::DataToInt(i_data);
	ASSERT(salvoSize >= 1);
	d_ref->IncrementHits(salvoSize);
}
