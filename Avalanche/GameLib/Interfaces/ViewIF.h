#ifndef __VIEWIF_H_INCLUDED__
#define __VIEWIF_H_INCLUDED__

#include "Interfaces/Interface.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class ViewIF : public CInterface
{
public:
	ViewIF() { viewLocked = false; };
	virtual ~ViewIF() {};

	static uint GetID(void) {return(CInterface::GetValidID(s_viewID));}
	static ViewIF* GetInterface(CActor* i_actor);
	static ViewIF* GetInterface(ActorHandle i_handle);

	// yes, you really do need to implement every single one of these
	// functions.  sorry.
	virtual void GetDirection(Vector3 &o_direction) = 0;
	virtual void GetUp(Vector3 &o_up) = 0;
	virtual void GetEyepoint(Vector3 &o_eyepoint) = 0;
	virtual DirCos3x3 SightMatrix( void ) = 0;

	// these ones you don't have to implement if you don't want to
	
	virtual void GetOrientation(Vector3 &o_direction, Vector3 &o_up) { GetDirection( o_direction ); GetUp( o_up ); }

	// ViewLocked should return true if it wants to keep looking the
	// direction it is looking.  If it doesn't care what direction it looks,
	// then return false.  The reason you might want to unlock is so that
	// a kinematic controller knows whether it should turn the actor to move
	// the desired direction, or keep the actor facing the same direction and
	// move sideways or backward instead.
	virtual bool ViewLocked( void ) { return viewLocked; }
	virtual void LockView( void ) { viewLocked = true; }
	virtual void UnlockView( void ) { viewLocked = false; }
	virtual void SetSightMatrix( const DirCos3x3 &matrix ) {};
	virtual void SetEyepoint( Vector3CRef eye ) {};
	
	// these you should not override.  They're just helpers that
	// construct a sight matrix for you
	void LookAt( Vector3CRef lookAtPoint, Vector3CRef up );
	void LookDirection( Vector3CRef lookDir, Vector3CRef up );

private:
	static uint s_viewID;
	
	bool viewLocked;
};

#endif //__VIEWIF_H_INCLUDED__
