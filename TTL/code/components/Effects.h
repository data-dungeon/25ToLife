//These are effects related to a human character whether player or npc

#ifndef EFFECTS_H
#define EFFECTS_H

#include "Components/Component.h"
#include "InterfacesTTL/HealthIF.h"
#include "Components/Say.h"

class Effects : public CActorComponent
{
public:
	Effects( CCompActor &i_actor );
	~Effects();

	const char *ComponentName( void ) { return "Effects"; }

	virtual void Initialize( void );
	void Reset(void);
	virtual void BeginFrame();

	void Destroy( void );

	void MunitionImpact( const CMunitionImpact& i_data );
	void HurtEffects(HealthIF::DamageReason reason, float intensityFromZeroToOne);
	void DeathEffects(HealthIF::DamageReason reason = HealthIF::WEAPONHIT);

private:
	float lastBledTime;
	SoundHandle	voiceHandle;
	HealthIF *healthIF;
	Say *sayComponent;

	REF_SEND_MESSAGE_HANDLER(MunitionImpact, Effects) munitionImpactHandler;
	REF_SEND_MESSAGE_HANDLER(DeathEnded, Effects) deathEndedHandler;
	REF_SEND_MESSAGE_HANDLER(startaffliction, Effects) d_startaffliction;
};

#endif // EFFECTS_H
