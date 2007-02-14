/***************************************************************************/
// Simple player stats, made to be obscure
/***************************************************************************/
#ifndef PLAYER_STATS_SP_H
#define PLAYER_STATS_SP_H

#include "avatar/pstat.h"

class PlayerStatsSP : public PlayerStats
{
public:
	PlayerStatsSP(CCompActor &actor);
	const char *ComponentName()
		{ return "PlayerStatsSP"; }

	virtual void Initialize();
	virtual void BeginFrame();

	virtual void AddHealth( float addHealth );
	virtual void DoDamage( float damage, DamageReason reason, int sourceID );
	virtual void Smack(float damage);

	CInterface *GetInterface( uint i_id );

	static PlayerStatsSP *GetComponent(ActorHandle actorHandle)
	{
		CCompActor *actor = CCompActor::FromHandle(actorHandle);
		if (!actor)
			return NULL;
		else
			return (PlayerStatsSP *)actor->GetComponentByName("PlayerStatsSP");
	}

private:
	virtual float ComputeDamage(float damage, DamageReason reason, int sourceID);

	REF_SEND_MESSAGE_HANDLER(MunitionImpact, PlayerStatsSP) munitionImpactHandler;
	REF_SEND_MESSAGE_HANDLER(StartCinematic, PlayerStatsSP) startCinematicHandler;
	REF_SEND_MESSAGE_HANDLER(EndCinematic, PlayerStatsSP) endCinematicHandler;
};

#endif
