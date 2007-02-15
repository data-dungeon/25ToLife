//============================================================================
//=
//= ActorList.h - A component that keeps track of a list of actors
//=
//============================================================================

#ifndef _ACTOR_LIST_H
#define _ACTOR_LIST_H

#include "Components/Component.h"
#include "container/Darray.h"
#include "GameHelper/Handle.h"

handledef(CActor) ActorHandle;

//============================================================================

class CActorList : public CActorComponent
{
public:
	// component base stuff
	CActorList( CCompActor& i_actor );
	virtual ~CActorList();
	const char *ComponentName()  { return "ActorList"; }

	int		Count();
	bool		Add( CActor* i_actor );
	bool		Remove( CActor* i_actor );
	bool		Contains( CActor* i_actor );
	CActor*	Get( int index );
	void		Clear();
	void		Compress();

private:
	DynamicArray<ActorHandle>	d_actors;
};

//============================================================================

#endif //_ACTOR_LIST_H
