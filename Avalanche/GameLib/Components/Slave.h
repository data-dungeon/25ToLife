#ifndef __SLAVE_H_INCLUDED__
#define __SLAVE_H_INCLUDED__

#include "Components/Component.h"

// a component that slaves an actor to another.  Similar to rider, except that
// rider only affects things that you're standing on, and once you're no longer
// standing on them stops working, where Slave will keep you attached to the other
// actor until you explicitly unattach

// another difference between the two is that rider just pulls you along with the
// other actor, where slave will keep a consistent offset and orientation
class Slave : public CActorComponent
{
public:
	Slave( CCompActor &i_actor );

	const char *ComponentName() { return "Slave"; }

	void Initialize( void );
	void AttemptUpdate( float i_deltaSec );
	void EndUpdate( float i_deltaSec );

	void AttachToActor( CProtagonist *i_master );
	void AttachToActor( CProtagonist *i_master, Vector3CRef posOffset, const DirCos3x3 &orientOffset );
	void DetachFromActor( void ) { master = NULL; }
	CProtagonist *GetMaster( void ) { return master; }

private:
	CProtagonist *master;
	Vector3 positionOffset;
	DirCos3x3 orientationOffset;
};

#endif // __SLAVE_H_INCLUDED__