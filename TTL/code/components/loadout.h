/////////////////////////////////////////////////////////////////////////////
// Loadout management component
/////////////////////////////////////////////////////////////////////////////
#ifndef PLAYERLOADOUT_H
#define PLAYERLOADOUT_H

#include "Components/Component.h"

class PlayerLoadout : public CActorComponent
{
public:
	// Pass in a list (terminated with a NULL string)
	// List must be static
	PlayerLoadout(CCompActor &actor);
	~PlayerLoadout();

	// Our interface support
	const char *ComponentName()
		{ return "Loadout"; }

	// The initialize
	void Initialize();

	void BuildPickupLoadout();

	// The update
	void EndUpdate(float i_deltaSec);

	// Activate/Deactivate
	void Activate(int msToLive = 0);
	void Deactivate();

private:
	CActor*	m_loadout;
	uint		m_timeToDeactivate;
	uint8		m_source;

	// The message handlers
	REF_BASIC_MESSAGE_HANDLER(SwapLoadout, MESSAGE_STRING, PlayerLoadout) m_swapLoadout;
	REF_BASIC_MESSAGE_HANDLER(SwapDuplicaLoadout, MESSAGE_STRING, PlayerLoadout) m_swapDuplicaLoadout;
};

#endif
