/***************************************************************************/
// Simple pilot stats
/***************************************************************************/
#include "TTLPCH.h"
#include "avatar/pstatMP.h"
#include "EngineHelper/sfx.h"
#include "Environ/Munition.h"
#include "components/KineControl/BipedKineControl.h"
#include "components/KineControl/BipedKineControl_s.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "Components/ComponentCreator.h"
#include "frontend/savegame.h"
#include "components/affliction.h"
#include "components/painstats.h"
#include "components/loadout.h"
#include "environ/ConfigMgr.h"
#include "weapons/armory.h"
#include "weapons/ttlweapon.h"
#include "weapons/ttlweapstats.h"
#include "avatar/sobj.h"
#include "components/Ai/AiBehavior.h"
#include "frontend/frontend.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(PlayerStatsMP, PlayerStatsMP, COMPONENT_NEUTRAL_PRIORITY);

/***************************************************************************/
// Globals
/***************************************************************************/
PlayerStatsMP::PlayerStatsMP(CCompActor &actor) : PlayerStats(actor)
{
}

/***************************************************************************/
// Reset all the stats to defaults
/***************************************************************************/
void PlayerStatsMP::Initialize()
{
	munitionImpactHandler.Initialize(MUNITION_IMPACT_ACTOR, MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	Reset();
}

/***************************************************************************/
/***************************************************************************/
void PlayerStatsMP::BeginFrame()
{
}

/***************************************************************************/
// Add to Health - set up an incident to add negative damage
/***************************************************************************/
void PlayerStatsMP::AddHealth( float addHealth )
{
}

/***************************************************************************/
/***************************************************************************/
void PlayerStatsMP::DoDamage(
	float damage,
	DamageReason reason,
	int sourceID)
{
	// --- is the match over?
	if (g_referee.GetPhase() != Referee::PLAYING)
		return;

	// sorry but I am currently dead
	if (!IsAlive())
		return;

	// How much damage will this do
	float health = GetHealth();
	float realDamage = ComputeDamage(damage, reason, sourceID);
	health -= realDamage;
	actor.SetProperty("health", health);

	// react to damage
	Effects* effects = (Effects*)actor.GetComponentByName( "Effects" );
	if(effects)
	{
		float hurtPercent = Math::Clamp(realDamage / m_maxHealth);
		effects->HurtEffects(reason, hurtPercent);
	}

	// Need to notify before calling JustDied().
	NotifyHealthChanged();

	// Did we just die?
	if (!IsAlive())
		JustDied(realDamage, reason, sourceID);
}

/***************************************************************************/
/***************************************************************************/
void PlayerStatsMP::Smack(float damage)
{
}

/***************************************************************************/
/***************************************************************************/
void PlayerStatsMP::MunitionImpact::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);

	CMunitionImpact* hit = (CMunitionImpact *)i_data;
	float damage = hit->d_damage;
	bool wasAlive = d_ref->IsAlive();

	CParticipantIF *partIF = (CParticipantIF *)d_ref->GetActor().GetInterface( CParticipantIF::GetID() );
	if (partIF)
		damage *= partIF->GetDamageMultiplier(hit->d_surfaceMaterial);

	// Should we dispute it?

	// Ack it!
	CActor* sender = CActor::FromHandle(i_sender);
	g_ttlMunition.AckMunitionImpact(i_sender, hit, &d_ref->GetActor(), true);

	//save hostagetaker info
	ActorHandle hostageTakerHandle = d_ref->actor.GetProperty( "hostageTaker" )->GetHandle();
	ActorHandle hostageHandle = d_ref->actor.GetProperty( "hostage" )->GetHandle();

	TTLProjectileProperties *projectile = (TTLProjectileProperties *)g_projectileLib.GetProperties( hit->d_munitionID );
	ASSERT(projectile);
	int sourceID = CParticipantIF::GetParticipantIdFromActor(sender);
	if (projectile && projectile->Melee())
		d_ref->DoDamage(damage, MELEEHIT, sourceID);
	else if (projectile && projectile->Stunner())
		d_ref->DoDamage(damage, STUNNERHIT, sourceID);
	else
		d_ref->DoDamage(damage, WEAPONHIT, sourceID);

	// If this is the impact that killed us then record it.
	if (!d_ref->IsAlive() && wasAlive)
	{
		TTLWeaponProperties *weapon = (TTLWeaponProperties *)g_weaponLib.GetProperties( hit->d_weaponID );
		d_ref->TrackWeaponDeath(sourceID, weapon);

		//respond back to my killer
		if(hostageTakerHandle)
		{
			g_messageDispatcher.SendMessageToActor("KilledHostage",NULL,d_ref->actor.GetHandle(),i_sender);
			g_messageDispatcher.SendMessageToActor("IKilledHostage",NULL,i_sender,i_sender);
		}
	}

	if ((sender == NULL) || !sender->IsA(CCombatant::ClassType()))
		return;

	// Weapons stats - a player scored a head shot on a combatant enemy.
	if (!hit->d_victim)
		return;
	CCombatant *combatantSender = (CCombatant *)sender;
	uint8 victim = CParticipantIF::GetParticipantIdFromActor(hit->d_victim);
	if (combatantSender && !combatantSender->GetIsNpc() &&
		hit->d_victim && hit->d_victim->IsA(CCombatant::ClassType()) &&
		g_referee.GetTeamingModel().AreEnemies( sourceID, victim ))
	{
		char *sn = partIF->GetSurfaceName(hit->d_surfaceMaterial);
		if ( sn != NULL )
		{
			if ( !strcmp( sn , "humanHead" ) )
			{
				g_messageDispatcher.SendMessageToActor("WeaponStatHeadShot", NULL, CActor::ToHandle(&d_ref->GetActor()), CActor::ToHandle(sender));
			}
		}
	}
}

/***************************************************************************/
/***************************************************************************/
float PlayerStatsMP::ComputeDamage(
	float damage,
	DamageReason reason,
	int sourceID)
{
	const int myID = CParticipantIF::GetParticipantIdFromActor(&actor);

	if (IsNpc())
	{
		return damage;
	}

	// Can we hurt ourselves?
	bool allowSelfDamage = g_configmgr.TestOptionValue("selfDamage", "1");
	bool shooterIsMe = (myID == sourceID);
	if(shooterIsMe)
	{
		if(allowSelfDamage || reason == FOR_FUN || reason == FALLING)
			return damage;
		else
			return 0.0f;
	}

	bool shooterIsFriend = g_referee.GetTeamingModel().AreFriends(CParticipantIF::GetParticipantIdFromActor(&actor), sourceID);
	bool allowFriendlyFire = g_configmgr.TestOptionValue("friendlyFire", "1");
	if (!allowFriendlyFire && shooterIsFriend)
		return 0.0f;

	return damage;
}

/***************************************************************************/
/***************************************************************************/
CInterface *PlayerStatsMP::GetInterface( uint i_id )
{
	if (i_id == HealthIF::GetID())
		return this;

	return NULL;
}

