#ifndef __VIEWCOMPONENT_H_INCLUDED__
#define __VIEWCOMPONENT_H_INCLUDED__

#include "Components/Component.h"
#include "Interfaces/ViewIF.h"

// a very simple view component, that does nothing but keep track
// of the view, and let you lock/unlock it.
class ViewComponent : public CActorComponent, public ViewIF
{
public:
	ViewComponent( CCompActor &i_actor );

	const char *ComponentName() { return "ViewComponent"; }

	virtual void GetDirection(Vector3 &o_direction) { o_direction = SightMatrix().GetRow2(); }
	virtual void GetUp(Vector3 &o_up) { o_up = SightMatrix().GetRow1(); }
	virtual void GetEyepoint(Vector3 &o_eyepoint);
	virtual DirCos3x3 SightMatrix( void );

	virtual void SetSightMatrix( const DirCos3x3 &matrix ) {sightMatrix = matrix;}
	virtual void SetEyepoint( Vector3CRef eye ) {eyePoint = eye;}

	virtual CInterface* GetInterface(uint i_id);

private:
	DirCos3x3 sightMatrix;
	Vector3 eyePoint;
	bool eyeSpecified;
};

#endif //__VIEWCOMPONENT_H_INCLUDED__