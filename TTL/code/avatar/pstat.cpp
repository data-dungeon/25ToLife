/***************************************************************************/
// Simple pilot stats
/***************************************************************************/
#include "TTLPCH.h"
#include "avatar/pstat.h"
#include "avatar/sobj.h"
#include "components/affliction.h"
#include "components/loadout.h"
#include "components/painstats.h"
#include "components/Ai/AiBehavior.h"
#include "components/KineControl/BipedKineControl.h"
#include "components/KineControl/BipedKineControl_s.h"
#include "EngineHelper/sfx.h"
#include "environ/ConfigMgr.h"
#include "Environ/Munition.h"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "weapons/armory.h"
#include "weapons/ttlweapon.h"
#include "weapons/ttlweapstats.h"

#define DEFAULT_MAX_HEALTH 100.0f
#define RESPAWN_INVULN_TIME 2.5f

/***************************************************************************/
// Globals
/***************************************************************************/
PlayerStats::PlayerStats(CCompActor &actor) : CActorComponent(actor)
{
	actor.AddProperty( "health", Property_float );
	actor.AddProperty( "ApprehendTime", Property_float );
	actor.AddProperty( "KilledHostage", Property_bool );
	SetMaxHealth(DEFAULT_MAX_HEALTH);
	Reset();
}

/***************************************************************************/
// Reset all the stats to defaults
/***************************************************************************/
void PlayerStats::Reset()
{
	SetHealthToFull();
	actor.SetProperty( "ApprehendTime", 0.0f );
	actor.SetProperty( "KilledHostage", false );
	NotifyHealthChanged();
	m_pauseDamage = false;
	if(actor.IsA(CCombatant::ClassType()) && ((CCombatant*)&actor)->GetIsNpc())
		m_invulnTimer = 0.0f;
	else
		m_invulnTimer = RESPAWN_INVULN_TIME;
	m_invincible = false;
	m_stunnerID = INVALID_PARTICIPANT;
}

/***************************************************************************/
/***************************************************************************/
void PlayerStats::SetMaxHealth(float amount)
{
	if(amount > 0.0f && amount < 10000.0f)
		m_maxHealth = amount;
}

/***************************************************************************/
// Reset all the stats to defaults
/***************************************************************************/
void PlayerStats::SetHealthToFull()
{
	actor.SetProperty( "health", m_maxHealth );
	NotifyHealthChanged();
}

/***************************************************************************/
/***************************************************************************/
float PlayerStats::GetHealth()
{
	return actor.GetProperty( "health" )->GetFloat();
}

/***************************************************************************/
/***************************************************************************/
float PlayerStats::GetHealthPercentage()
{
	//PLAYER_MAX_HEALTH will never be zero
	return Math::Clamp(GetHealth() / m_maxHealth);
}

/***************************************************************************/
// Kill player immediately
// - currently used for Players/NPCs falling thru level
// - Player death will fadeout screen and bypass normal death animations & any loot drops
// - NPC death will prevent NPC from spawning anything
/***************************************************************************/
void PlayerStats::Kill()
{
	// --- is the match over?
	if( g_referee.GetPhase() != Referee::PLAYING )
		return;

	//sorry but I am currently dead
	if (!IsAlive())
		return;

	//if I'm the avatar - set weapon dropped so I don't drop it upon Death-NOW (player falling thru Level)
	if( &actor == g_players.GetPlayer(0) )
	{
		CPainStats *stats = (CPainStats *)g_players.GetPlayer(0)->GetComponentByName( "PainStats" );
		if (stats)
			stats->SetWeaponDropped();
	}

	// How much damge will this do
	float health = 0.0f;
	actor.SetProperty("health", health);

	// Need to notify before calling JustDied().
	NotifyHealthChanged();

	JustDied(1000.0f, FOR_FUN, INVALID_PARTICIPANT);
}

/***************************************************************************/
/***************************************************************************/
void PlayerStats::JustDied(float damage, DamageReason reason, int sourceID)
{
	ASSERT(!IsAlive());

	// BET - only record a kill right now if the player can't be apprehended.
	// Note: you can't apprehend NPCs in networked games.
	Afflictions *afflictions = (Afflictions *)GetActor().GetComponentByName("Afflictions");
	if (afflictions && afflictions->Afflicted(Afflictions::STUNNED) && (!g_session.IsNetworked() || !IsNpc()))
	{
		GetActor().SetProperty( "ApprehendTime", 10.0f );
		m_stunnerID = sourceID;
	}
	else
	{
		uint32 data = ((0xFF & sourceID) << 8) | (0xFF & CParticipantIF::GetParticipantIdFromActor(&actor));
		g_messageDispatcher.SendMessageToOwner("RecordKill", (void*)data, actor.GetHandle(), g_referee.GetMessageOwner());
	}

	// BET - deactivate the interaction monitor until we respawn.
	InteractionMonitor* interact = (InteractionMonitor*)GetActor().GetComponentByName("InteractionMonitor");
	if (interact)
		interact->Deactivate();

	//react to damage
	Effects* effects = (Effects*)actor.GetComponentByName( "Effects" );
	if(effects)
		effects->DeathEffects(reason);

	g_messageDispatcher.SendMessageToAll("ManDown", NULL, actor.GetHandle());
	if (!IsNpc())
	{
		PlayerLoadout* loadout = (PlayerLoadout*)GetActor().GetComponentByName("Loadout");
		if (loadout)
		{
			loadout->BuildPickupLoadout();
			loadout->Activate();
		}
	}
}

/***************************************************************************/
/***************************************************************************/
CInterface *PlayerStats::GetInterface( uint i_id )
{
	if( i_id == HealthIF::GetID() )
		return this;

	return NULL;
}

/***************************************************************************/
/***************************************************************************/
void PlayerStats::NotifyHealthChanged()
{
	bool alive = IsAlive();
	CParticipantIF* participant = CParticipantIF::GetInterface(&actor);
	if ((participant != NULL) && (alive != participant->GetAlive()))
	{
		participant->SetSpawnTicket(!alive && g_referee.RequestSpawnTicketForParticipant(participant->GetParticipantId()));
		participant->SetAlive(alive);
	}
}

/***************************************************************************/
/***************************************************************************/
void PlayerStats::TrackWeaponDeath( int sourceID, TTLWeaponProperties *weapon )
{
	if (sourceID != INVALID_PARTICIPANT)
	{
		uint8 victim = CParticipantIF::GetParticipantIdFromActor( actor );
		if (!g_referee.GetTeamingModel().AreEnemies( sourceID, victim ))
			return;

		if (weapon != NULL)
		{
			// Weapon stats - tell killer what weapon he used to do the deed (ignore suicide and team kills).
			CActor* sourceActor = g_referee.GetActor(sourceID);
			if (sourceActor && CActor::ToHandle(&actor) != sourceActor->GetHandle())
				g_messageDispatcher.SendMessageToActor("WeaponStatKill", (void*)weapon->m_weaponType, ((CActor*)&actor)->GetHandle(), sourceActor->GetHandle());

			// Weapons stats - what weapon did we die by.
			TTLWeaponStats* stats = (TTLWeaponStats*)actor.GetComponentByName("TTLWeaponStats");
			if (stats)
				stats->IncrementDeathBy(weapon->m_weaponType);
		}

		// Weapons stats - what weapon did we die holding.
		WeaponInventory* inv = (WeaponInventory*)actor.GetComponentByName("WeaponInventory");
		if (inv)
		{
			TTLWeapon* selectedWeapon = (TTLWeapon*)inv->GetSelected();
			if (selectedWeapon)
			{
				TTLWeaponStats* stats = (TTLWeaponStats*)actor.GetComponentByName("TTLWeaponStats");
				if (stats)
					stats->IncrementDeathHolding(selectedWeapon->GetWeaponType());
			}
		}
	}
}

