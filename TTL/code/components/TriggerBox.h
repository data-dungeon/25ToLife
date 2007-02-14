//============================================================================
//=
//= TriggerBox.h -
//=    This actor class watches for entry/exit of any other actor from its
//=    collision box, and then transmits those events to the handling
//=    script.
//=
//============================================================================

#ifndef _TRIGGERBOX_H
#define _TRIGGERBOX_H

#include "stage/protagonist.h"
#include "container/Darray.h"

//============================================================================

class TriggerBox : public CProtagonist
{
public:
	TriggerBox();
	~TriggerBox();
	virtual bool Initialize();

	virtual void BeginUpdate( void );
	virtual void CollisionAccept( CStageEntity* i_entity, CCollisionEvent* i_event );
	virtual void EndUpdate( float i_deltaSec );
	virtual void RunAnimation( bool forceUpdate );

	int		ActorCount() const;
	CActor*	GetActor( int index ) const;

private:
	DynamicArray<ActorHandle> d_containedActors;
	int							  d_numSeen;
};

//============================================================================

#endif //_TRIGGERBOX_H
