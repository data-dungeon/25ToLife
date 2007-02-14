/***************************************************************************/
// Simple pilot stats
/***************************************************************************/
#include "TTLPCH.h"
#include "avatar/pstatSP.h"
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
#include "Interfaces/ViewIF.h"
#include "frontend/frontend.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(PlayerStatsSP, PlayerStatsSP, COMPONENT_NEUTRAL_PRIORITY);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<DifficultySettings>::RegisterVariables()
{
	RegisterVariable(d_data.ai2PlayerDamageFactor, "AI2PlayerDamage", 0.33f, 0.0f, 5.0f);
	RegisterVariable(d_data.player2AIDamageFactor, "Player2AIDamage", 1.2f, 0.0f, 5.0f);
	RegisterVariable(d_data.aiFallingFactor, "aiFalling", 0.2f, 0.0f, 5.0f);
	RegisterVariable(d_data.playerFallingFactor, "playerFalling", 1.0f, 0.0f, 5.0f);
	RegisterVariable(d_data.aiSameTeamFactor, "aiSameTeam", 0.1f, 0.0f, 5.0f);
	RegisterVariable(d_data.aiMeleeFactor, "aiMelee", 0.5f, 0.0f, 5.0f);
	RegisterVariable(d_data.forceHealth, "forceHealth", 50.0f, -1.0f, 100.0f);
	RegisterVariable(d_data.fromBehindAngle, "fromBehindAngle", 0.3f, 0.0f, 1.0f);
}

/***************************************************************************/
// Globals
/***************************************************************************/
PlayerStatsSP::PlayerStatsSP(CCompActor &actor) : PlayerStats(actor)
{
}

/***************************************************************************/
// Reset all the stats to defaults
/***************************************************************************/
void PlayerStatsSP::Initialize()
{
	// Holy magic
	switch(g_GlobalProfile.GameSettings.Difficulty)
	{
		case 0: //easy
			m_difficulty = CParam<DifficultySettings>::GetParam("easy", "var");
			break;
		default:
		case 1: //normal
			m_difficulty = CParam<DifficultySettings>::GetParam("normal", "var");
			break;
		case 2: //hard
			m_difficulty = CParam<DifficultySettings>::GetParam("hard", "var");
			break;
		case 3: //nightmare
			m_difficulty = CParam<DifficultySettings>::GetParam("omfg", "var");
			break;
	}
	ASSERT(m_difficulty);

	munitionImpactHandler.Initialize(MUNITION_IMPACT_ACTOR, MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	startCinematicHandler.Initialize("StartCinematic", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	endCinematicHandler.Initialize("EndCinematic", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	Reset();
}

/***************************************************************************/
/***************************************************************************/
void PlayerStatsSP::BeginFrame()
{
	float frameSec = g_timer.GetFrameSec();
	Math::MoveToZero(&m_invulnTimer,frameSec);
}

/***************************************************************************/
// Add to Health - set up an incident to add negative damage
/***************************************************************************/
void PlayerStatsSP::AddHealth( float addHealth )
{
	if (GetHealth() > 0.0f)
	{
		if (m_difficulty->forceHealth >= 0.0f)
			addHealth = m_difficulty->forceHealth;
		float health = Math::Min(GetHealth() + addHealth, m_maxHealth);
		actor.SetProperty( "health", health );
		NotifyHealthChanged();
	}
}

/***************************************************************************/
/***************************************************************************/
void PlayerStatsSP::DoDamage(
float damage,
DamageReason reason,
int sourceID)
{
	// hehe!
	if (Cheats::Enabled(Cheats::INVULNERABLE, &actor) || m_pauseDamage || m_invincible || m_invulnTimer > 0.0f)
		return;

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
void PlayerStatsSP::Smack(float damage)
{
	DoDamage(damage, FOR_FUN, INVALID_PARTICIPANT);
}

/***************************************************************************/
/***************************************************************************/
void PlayerStatsSP::MunitionImpact::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
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

	// did we get hit from behind?
	int sourceID = CParticipantIF::GetParticipantIdFromActor(sender);
	if (g_referee.IsNonPlayer(sourceID) && !d_ref->IsNpc() && !g_gameEnvironment.BonusLevel())
	{
		ViewIF *viewIF = (ViewIF *)d_ref->GetActor().GetInterface(ViewIF::GetID());
		if (viewIF)
		{
			Vector3 myDir;
			viewIF->GetDirection(myDir);
			if (Vector3::Dot(myDir, hit->Direction()) > d_ref->m_difficulty->fromBehindAngle)
			{
#if defined(FOOD) && defined(_DEBUG) && defined(WIN32)
				DrawUtility::Sphere(hit->d_intersection, DrawUtility::RED);
#endif
				damage *= 2.0f;
			}
		}
	}

	//save hostagetaker info
	ActorHandle hostageTakerHandle = d_ref->actor.GetProperty( "hostageTaker" )->GetHandle();
	ActorHandle hostageHandle = d_ref->actor.GetProperty( "hostage" )->GetHandle();

	TTLProjectileProperties *projectile = (TTLProjectileProperties *)g_projectileLib.GetProperties( hit->d_munitionID );
	ASSERT(projectile);
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

		//if I am a hostage tell my hostage-taker
		if (hostageTakerHandle)
			g_messageDispatcher.SendMessageToActor("KilledHostage", NULL, d_ref->actor.GetHandle(), i_sender);
	}

	if ((sender == NULL) || !sender->IsA(CCombatant::ClassType()))
		return;

	// make note of head shots
	CCombatant *combatantSender = (CCombatant *)sender;
	CCombatant *victim = (hit->d_victim && hit->d_victim->IsA(CCombatant::ClassType())) ? (CCombatant*)hit->d_victim : NULL;
	if( victim && victim->GetIsNpc() && !combatantSender->GetIsNpc() )
	{
		// don't count friendly npc hits
		int npc_team = victim->Parm().GetInt( "npc_team" );
		if( npc_team==AiBehavior::OBJNPCTEAM_FRIENDLY ||
			npc_team==AiBehavior::OBJNPCTEAM_AMBIENT ||
			npc_team==AiBehavior::OBJNPCTEAM_NEUTRAL )
		{
			g_secondaryObjective.DecShotsFired();
			return;
		}

		g_secondaryObjective.IncNumHits();

		char *sn = partIF->GetSurfaceName(hit->d_surfaceMaterial);
		if( sn != NULL )
		{
			if( !strcmp( sn , "humanHead" ) )
			{
				g_secondaryObjective.IncHeadshots();
			}
		}
	}

	// Weapons stats - a player scored a head shot on a combatant.
	if (combatantSender && !combatantSender->GetIsNpc() && victim && victim->IsA(CCombatant::ClassType()))
	{
		char *sn = partIF->GetSurfaceName(hit->d_surfaceMaterial);
		if ( sn != NULL )
		{
			if ( !strcmp( sn , "humanHead" ) )
			{
				g_messageDispatcher.SendMessageToActor("WeaponStatHeadShot", NULL, CActor::ToHandle(&d_ref->GetActor()), CActor::ToHandle(sender));

#if 0
				// send off a display message -- this headshot resutled in a death.
				if( d_ref->GetHealth() <= 0 )
				{
					char				szTalkText[ 128 ];
					uint8				attacker = CParticipantIF::GetParticipantIdFromActor(i_sender);
					CPlayerAgent	*pAttacker = g_referee.GetPlayerAgent( attacker );
					uint8				victim = CParticipantIF::GetParticipantIdFromActor( CActor::ToHandle( hit->d_victim ) );
					CPlayerAgent	*pVictim = g_referee.GetPlayerAgent( victim );

					if ( pAttacker == NULL )
					{
						sprintf( szTalkText, "%s died from a head shot", pVictim->GetDisplayName() );
					}
					else
					{
						sprintf( szTalkText, "%s killed %s with a head shot", pAttacker->GetDisplayName(), pVictim->GetDisplayName() );
					}

					g_messageDispatcher.SendMessageToSession( "TextChat", szTalkText, i_sender );
				}
#endif
			}
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void PlayerStatsSP::StartCinematic::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	BipedModel* model = (BipedModel*)d_ref->GetActor().GetComponentByName("BipedModel");

	ASSERT_PTR(d_ref);
	d_ref->m_pauseDamage = true;
	model->StopCrouch();
	g_FrontEnd.SetCinematicInProgress(true);
}

/***************************************************************************/
/***************************************************************************/
void PlayerStatsSP::EndCinematic::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	if(d_ref->actor.Parm().GetInt("invincible") == 1)
		d_ref->m_pauseDamage = true;
	else
		d_ref->m_pauseDamage = false;
	g_FrontEnd.SetCinematicInProgress(false);
}

/***************************************************************************/
/***************************************************************************/
float PlayerStatsSP::ComputeDamage(
float damage,
DamageReason reason,
int sourceID)
{
	const int myID = CParticipantIF::GetParticipantIdFromActor(&actor);

	if (IsNpc())
	{
		//bots don't get full damage
		if (reason == FALLING)
			damage *= m_difficulty->aiFallingFactor;
		else if (reason == STUNNERHIT)
			damage = Math::Max(100.5f, GetHealth() + 1.0f);
		else if (g_referee.IsPlayer(sourceID))
			damage *= m_difficulty->player2AIDamageFactor;
		else if (g_referee.GetTeamingModel().AreTeammates(myID, sourceID))
			damage *= m_difficulty->aiSameTeamFactor;
		else
		{
			bool shooterIsTeammate = g_referee.GetTeamingModel().AreFriends(myID, sourceID);
			if(shooterIsTeammate && reason == MELEEHIT)
				return 0.0f;

			bool allowFriendlyFire = g_configmgr.TestOptionValue("friendlyFire", "1");
			if (!allowFriendlyFire && shooterIsTeammate)
				return 0.0f;
		}

		return damage;
	}

	if(g_referee.IsNonPlayer(sourceID)) 	// DIFFICULTY LEVEL STUFF!!!
	{
		if(reason == MELEEHIT)
			damage *= m_difficulty->aiMeleeFactor;
		else if (reason == FALLING)
			damage = m_difficulty->playerFallingFactor;
		else
			damage *= m_difficulty->ai2PlayerDamageFactor;

		// Stop cheesing the AI you deviant testers
		if (g_referee.IsPlayer(myID))
		{
			// If you are holding melee, you aget 2x
			WeaponInventory *inv = (WeaponInventory *)actor.GetComponentByName("WeaponInventory");
			if (inv)
			{
				TTLWeapon *weapon = (TTLWeapon*)inv->GetSelected();
				if (weapon && weapon->GetWeaponClass() == WEAPONCLASS_MELEE)
					damage *= 2.0f;
			}
		}
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
CInterface *PlayerStatsSP::GetInterface( uint i_id )
{
	if (i_id == HealthIF::GetID())
		return this;

	return NULL;
}

