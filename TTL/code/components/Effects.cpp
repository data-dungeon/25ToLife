#include "TTLPCH.h"

#include "components/Effects.h"
#include "EngineHelper/sfx.h"
#include "Environ/Munition.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "effects/blood.h"
#include "components/Ai/AiBehavior.h"
#include "avatar/pstat.h"
#include "avatar/avatarfx.h"
#include "data_h_files/players/players_s.h"
#include "weapons/firearm.h"
#include "Components/weapons/projlib.h"
#include "Components/ComponentCreator.h"
#include "effects/pain_decal.h"

#define CAR_EXPLOSION_MAX_DIST 300.0f
#define CAR_EXPLOSION_Y_FACTOR 1.0f
#define CAR_EXPLODED_MIN_ENERGY_TO_REACT 0.05f
#define CAR_EXPLOSION_MAX_DAMAGE 0.5f
#define DEATH_SILILOQUY_CHANCE 0.40f

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(Effects, Effects, COMPONENT_NEUTRAL_PRIORITY);

//============================================================================
Effects::Effects( CCompActor &i_actor ) :
	CActorComponent( i_actor )
{
	voiceHandle = NULL;
	lastBledTime = 0.0f;
}
//============================================================================
Effects::~Effects()
{
	Destroy();
}

//============================================================================
void Effects::Initialize( void )
{
	munitionImpactHandler.Initialize(MUNITION_IMPACT_ACTORFX,
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA,
			actor.GetMessageOwner(), this);

	deathEndedHandler.Initialize("DeathEnded",
		MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA,
		actor.GetMessageOwner(), this);

	d_startaffliction.Initialize("startaffliction",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	healthIF = (HealthIF*)actor.GetInterface( HealthIF::GetID() );
	sayComponent = (Say*)( actor.GetComponentByName( "Say" ) );
	Reset();
}

//============================================================================
void Effects::Reset(void)
{
	lastBledTime = 0.0f;
	Sfx::Stop(voiceHandle);
}

//============================================================================
void Effects::Destroy( void )
{
	Sfx::Stop(voiceHandle);
}

//============================================================================
void Effects::BeginFrame()
{
	if(healthIF)
	{
		float health = healthIF->GetHealthPercentage();
		if (g_blood.TimeToBleed(health,lastBledTime))
			g_blood.Bleed(actor,health);
	}
}
//============================================================================
void Effects::MunitionImpact( const CMunitionImpact& i_data )
{
	// A character has been hit by a shot, place blood on the wall behind
	Vector3 dir = (i_data.d_intersection - i_data.d_shotOrigin).Normalized();
	g_blood.Impact(i_data.d_intersection, dir, actor, i_data.d_energy);

	// Tell them about it
	g_avatarFX.NotifyWeaponHit(actor, i_data);

	// Do the other stuff
	TTLProjectileProperties *projectile = (TTLProjectileProperties *)g_projectileLib.GetProperties( i_data.d_munitionID );
	if (projectile->Melee())
		HurtEffects(HealthIF::MELEEHIT, Math::Clamp(i_data.d_energy / 1500.0f));
	else
		HurtEffects(HealthIF::WEAPONHIT, Math::Clamp(i_data.d_energy / 1000.0f));
}

//============================================================================
void Effects::HurtEffects(HealthIF::DamageReason reason, float intensityFromZeroToOne)
{
	//check to see if it can be hurt
	if(healthIF == NULL || !healthIF->IsAlive())
		return;

	switch(reason)
	{
		case HealthIF::ADD_HEALTH:
			break;
		case HealthIF::FOR_FUN:
			if(sayComponent)
				sayComponent->PlayVoiceType(Say::VOICETYPE_SFX_HURT);
			break;
		case HealthIF::WEAPONHIT:
			{
			if(sayComponent)
			{
				sayComponent->PlayVoiceType(Say::VOICETYPE_SFX_HURT);
				if(&actor != g_players.GetPlayer(0))
					sayComponent->PlayVoiceType(Say::VOICETYPE_SHOT,0.2f,1.0f,false);
			}
			Firearm *weapon = NULL;
			WeaponInventory *inv = (WeaponInventory *)actor.GetComponentByName( "WeaponInventory" );
			if( inv )
				weapon = (Firearm *)inv->GetSelected();
			if(weapon)
				weapon->GetAimSim().TriggerEvent("hit",intensityFromZeroToOne);
			}
			break;
		case HealthIF::MELEEHIT:
			if(sayComponent)
			{
				sayComponent->PlayVoiceType(Say::VOICETYPE_SFX_MELEE_HURT);
				sayComponent->PlayVoiceType(Say::VOICETYPE_HURT,0.1f,0.8f,false);
			}
			break;
		case HealthIF::STUNNERHIT:
			if(sayComponent)
				sayComponent->PlayVoiceType(Say::VOICETYPE_SFX_HURT);
			break;
		case HealthIF::FALLING:
			if(sayComponent)
				sayComponent->PlayVoiceType(Say::VOICETYPE_SFX_HURT);
			break;
		case HealthIF::EXPLOSION:
			{
			if(sayComponent)
				sayComponent->PlayVoiceType(Say::VOICETYPE_SFX_HURT);
			Firearm *weapon = NULL;
			WeaponInventory *inv = (WeaponInventory *)actor.GetComponentByName( "WeaponInventory" );
			if( inv )
				weapon = (Firearm *)inv->GetSelected();
			if(weapon)
			weapon->GetAimSim().TriggerEvent("explode",intensityFromZeroToOne);
			break;
			}
		case HealthIF::ONFIRE:
			{
				Firearm *weapon = NULL;
				WeaponInventory *inv = (WeaponInventory *)actor.GetComponentByName( "WeaponInventory" );
				if( inv )
					weapon = (Firearm *)inv->GetSelected();
				if(weapon)
					weapon->GetAimSim().TriggerEvent("explode",intensityFromZeroToOne);
				if(&actor == g_players.GetPlayer(0) && sayComponent)
					sayComponent->PlayVoiceType(Say::VOICETYPE_ON_FIRE,0.0f,0.0f,false);
				break;
			}
	}
}

//============================================================================
//============================================================================
void Effects::DeathEffects(HealthIF::DamageReason reason)
{
	// make sure avatar is visible.  the avatar is hidded when in first person mode and
	// when a sniper gun is used
	bool isShooter = (g_shooterCamPtr && &actor == (CCompActor *)g_shooterCam.GetFollow());
	if (isShooter)
		g_shooterCam.HideShooter(false);

	g_avatarFX.NotifyDeath(actor);

	if( reason != HealthIF::MELEEHIT)
	{
		if(sayComponent)
		{
			if(Stumpy::Decapitated(actor))
				sayComponent->PlayVoiceType(Say::VOICETYPE_DEATH_DECAP);
			else if(g_random.Unit() < DEATH_SILILOQUY_CHANCE)
				sayComponent->PlayVoiceType(Say::VOICETYPE_DEATH_SILILOQUY);
			else
				sayComponent->PlayVoiceType(Say::VOICETYPE_DEATH);
		}
	}
	else
	{
		if(sayComponent)
		{
			if(Stumpy::Decapitated(actor))
				sayComponent->PlayVoiceType(Say::VOICETYPE_DEATH_DECAP);
			else if(sayComponent)
				sayComponent->PlayVoiceType(Say::VOICETYPE_DEATH_MELEE);
		}

		if (isShooter)
		{
			int select = g_random.UpTo(3);
			if (select == 0)
			{
				CMasterTimer::ETimerSpeed speed = CMasterTimer::ONE_FOURTH;
				uint32 duration = 400;
				int data = (duration << 16) | (int)speed | 0x80000000;
				g_messageDispatcher.SendMessageToAll("TimeSpeed", CMessageDispatcher::IntToData(data), INVALID_OBJECT_HANDLE);
			}
			if (select == 1)
			{
				CMasterTimer::ETimerSpeed speed = CMasterTimer::ONE_HALF;
				uint32 duration = 600;
				int data = (duration << 16) | (int)speed | 0x80000000;
				g_messageDispatcher.SendMessageToAll("TimeSpeed", CMessageDispatcher::IntToData(data), INVALID_OBJECT_HANDLE);
			}
		}
	}
}

//============================================================================
//EVENTS
//============================================================================
void Effects::MunitionImpact::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	CMunitionImpact* impact = (CMunitionImpact *) i_data;
	d_ref->MunitionImpact( *impact );
}

//============================================================================
void Effects::DeathEnded::HandleMessage(
void* i_data,
ActorHandle i_sender,
bool i_posted)
{
	AiBehavior *aiBehavior = (AiBehavior*)( d_ref->actor.GetComponentByName( "AiBehavior" ) );
	if(aiBehavior && aiBehavior->GetAfflication() == Afflictions::STUNNED)
		return;

	bool decapitated = Stumpy::Decapitated(d_ref->actor);
	g_blood.DeathPool(d_ref->actor, decapitated ? "head" : "spine");
}
//============================================================================
void Effects::startaffliction::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	int i_affliction = (int) i_data;

	//check to see for higher priority affltion
	switch(i_affliction)
	{
		case Afflictions::BLIND:
		case Afflictions::TEARGAS:
			Say *sayComponent = (Say *)d_ref->actor.GetComponentByName( "Say" );
			if (sayComponent)
				sayComponent->PlayVoiceType(Say::VOICETYPE_BLINDED,0.2f,0.3f,false);
		break;
	}
}
//==================================eof=======================================

