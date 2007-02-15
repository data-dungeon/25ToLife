#ifndef __RIDER_H_INCLUDED__
#define __RIDER_H_INCLUDED__

#include "Components/Component.h"

// ride on any actor that we end up standing on top of, for as long as we're standing on top of it.
// depends on the groundEntity property to know who or what we're standing on
class Rider : public CActorComponent
{
public:
	Rider( CCompActor &i_actor );

	const char *ComponentName() { return "Rider"; }

	void Initialize( void );
	void BeginFrame( void );
	void AttemptUpdate( float i_detaSec );
	void EndUpdate( float i_deltaSec );
	void EndFrame( void );

private:
	CCompActor *riding;
	Vector3 lastRiderInBody; // last known position, in the thing being ridden's coordinate space
	Vector3 lastRiderInWorld; // where our last known position was, in world coordinates.
	DirCos3x3 lastBodyToWorld; // the thing being ridden's last body to world
};

#endif //__RIDER_H_INCLUDED__