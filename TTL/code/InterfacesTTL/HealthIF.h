#ifndef __HEALTHIF_H_INCLUDED__
#define __HEALTHIF_H_INCLUDED__

#include "Interfaces/Interface.h"
#include "weapons/weaponanim.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class HealthIF : public CInterface
{
public:
	typedef enum
	{
		ADD_HEALTH,	// used to add health - not damage
		FOR_FUN,		// no reason in particular
		WEAPONHIT,
		MELEEHIT,
		STUNNERHIT,
		FALLING,
		EXPLOSION,
		ONFIRE
	} DamageReason;

	static uint GetID(void) {return(CInterface::GetValidID(s_healthID));}
	static HealthIF* GetInterface(CActor* i_actor);
	static HealthIF* GetInterface(ActorHandle i_handle);

	virtual bool IsAlive(void) { return GetHealth() > 0.0f; }
	virtual void AddHealth( float value ) {}
	virtual float GetHealth() { return 0.0f; }
	virtual float GetMaxHealth() {return 0.0f; }
	virtual float GetHealthPercentage() {return 0.0f; }
	virtual void DoDamage( float damage, DamageReason reason, int sourceID ) {}
	virtual void Kill(void) {}

private:
	static uint s_healthID;
};

#endif // __HEALTHIF_H_INCLUDED__
