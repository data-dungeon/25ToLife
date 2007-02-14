#ifndef VISION_H
#define VISION_H

#include "Components/Component.h"

class Vision : public CActorComponent
{
public:
	Vision( CCompActor &i_actor );
	~Vision();

	const char *ComponentName( void ) { return "Vision"; }

	virtual void Initialize( void );

	bool CameraToEye(CActor *toActor, float &distSquared);  //a cord from current camera to head

	float DistanceSquaredToEye(CActor *toActor); // get the distance from current camera to head.

private:

};

#endif // VISION_H
