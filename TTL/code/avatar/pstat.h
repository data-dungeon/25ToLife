/***************************************************************************/
// Simple player stats, made to be obscure
/***************************************************************************/
#ifndef PSTAT_H
#define PSTAT_H

#include "Components/Component.h"
#include "container/dllist.h"
#include "GameHelper/Message.h"
#include "InterfacesTTL/HealthIF.h"
#include "weapons/weaponanim.h"

class TTLWeaponProperties;

// The parameters for the difficulty level
struct DifficultySettings
{
	float ai2PlayerDamageFactor;
	float player2AIDamageFactor;
	float aiFallingFactor;
	float playerFallingFactor;
	float aiSameTeamFactor;
	float aiMeleeFactor;
	float forceHealth;
	float fromBehindAngle;
};

class PlayerStats : public CActorComponent, public HealthIF
{
public:
	PlayerStats(CCompActor &actor);
	const char *ComponentName()
		{ return "PlayerStats"; }

	virtual void Initialize() = 0;
	void Reset();
	virtual void BeginFrame() = 0;

	float GetHealth();
	float GetMaxHealth()
		{ return m_maxHealth; }
	float GetHealthPercentage();

	bool IsAlive()
		{ return GetHealth() > 0.0f; }

	virtual void AddHealth(float addHealth) = 0;
	virtual void DoDamage(float damage, DamageReason reason, int sourceID) = 0;
	void SetHealthToFull();
	virtual void Smack(float damage) = 0;

	void SetHealth(float amount)
		{ actor.SetProperty( "health", amount); }
	void SetMaxHealth(float amount);
	void SetInvincible(bool setting) {m_invincible = true;}

	void Kill();

	bool IsNpc()
		{ return ((CCombatant*)&actor)->GetIsNpc(); }

	int GetStunner()
		{ return (m_stunnerID); }

	CInterface *GetInterface( uint i_id );

	static PlayerStats *GetComponent(ActorHandle actorHandle)
	{
		CCompActor *actor = CCompActor::FromHandle(actorHandle);
		if (!actor)
			return NULL;
		else
			return (PlayerStats *)actor->GetComponentByName("PlayerStats");
	}

protected:
	virtual float ComputeDamage(float damage, DamageReason reason, int sourceID) = 0;
	void JustDied(float damage, DamageReason reason, int sourceID);
	void NotifyHealthChanged();
	void TrackWeaponDeath(int sourceID, TTLWeaponProperties *weapon);

	DifficultySettings	*m_difficulty;
	float						m_maxHealth;
	bool						m_pauseDamage;  //used during cinematic to withhold new damage
	float						m_invulnTimer;
	bool						m_invincible;
	int						m_stunnerID;    //player who stunned us to death

private:
	static uint s_playerStatsID;
};

#endif
