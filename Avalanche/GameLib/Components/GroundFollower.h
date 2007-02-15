#ifndef __GROUNDFOLLOWER_H_INCLUDED__
#define __GROUNDFOLLOWER_H_INCLUDED__

/******************************************
GroundFollower - a collision behavior for things that are intended to move
along the ground.

Don't go through walls, try to stay on the floor.
******************************************/

#include "Components/Component.h"
#include "Math/Vector.h"

class GroundFollower : public CActorComponent
{
public:
	GroundFollower( CCompActor &i_actor );
	virtual ~GroundFollower();

	const char *ComponentName() { return "GroundFollower"; }
	const char *ComponentClass() { return "CollisionBehavior"; }

	virtual void Initialize( void );
	virtual void Activate( void );
	virtual void BeginFrame( void );
	virtual void BeginUpdate( void );
	virtual void EndFrame( void );

	virtual uint CollisionDetect(CStageEntity* i_entity, CCollisionEvent* i_event, bool i_mateDisplaced);

private:
	// remember the velocity at the beginning of the update, because collision responses cancel
	// part of the velocity, but we want to know where the actor is coming from, so we can
	// resolve OBB collisions correctly
	Vector3 movementDirection;
	Vector3 lastPosition;
	bool movementDetermined;
	bool anyCollisionsThisFrame;
};

#endif // __GROUNDFOLLOWER_H_INCLUDED__
