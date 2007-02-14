/***************************************************************************/
// Simple player stats, made to be obscure
/***************************************************************************/
#ifndef PLAYER_STATS_MP_H
#define PLAYER_STATS_MP_H

#include "avatar/pstat.h"

class PlayerStatsMP : public PlayerStats
{
public:
	PlayerStatsMP(CCompActor &actor);
	const char *ComponentName()
		{ return "PlayerStatsMP"; }

	virtual void Initialize();
	virtual void BeginFrame();

	virtual void AddHealth( float addHealth );
	virtual void DoDamage( float damage, DamageReason reason, int sourceID );
	virtual void Smack(float damage);

	CInterface *GetInterface( uint i_id );

	static PlayerStatsMP *GetComponent(ActorHandle actorHandle)
	{
		CCompActor *actor = CCompActor::FromHandle(actorHandle);
		if (!actor)
			return NULL;
		else
			return (PlayerStatsMP *)actor->GetComponentByName("PlayerStatsMP");
	}

private:
	virtual float ComputeDamage(float damage, DamageReason reason, int sourceID);

	REF_SEND_MESSAGE_HANDLER(MunitionImpact, PlayerStatsMP) munitionImpactHandler;
};

#endif
