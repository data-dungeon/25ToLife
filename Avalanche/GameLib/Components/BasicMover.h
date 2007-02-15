//============================================================================
//=
//= BasicMover.h - A component to handle rudamentary things such as rotating,
//=                scaling, point to point movement, etc.
//=
//============================================================================

#ifndef _BASIC_MOVER_H
#define _BASIC_MOVER_H

#include "Components/Component.h"
#include "Interfaces/BasicMoverIF.h"

#define BASICMOVER_ROTATE_X										0x0001
#define BASICMOVER_ROTATE_Y										0x0002
#define BASICMOVER_ROTATE_Z										0x0004
#define BASICMOVER_TRANSLATE_X									0x0010
#define BASICMOVER_TRANSLATE_Y									0x0020
#define BASICMOVER_TRANSLATE_Z									0x0040
#define BASICMOVER_ROTATE_AT_VELOCITY_X						0x0100
#define BASICMOVER_ROTATE_AT_VELOCITY_Y						0x0200
#define BASICMOVER_ROTATE_AT_VELOCITY_Z						0x0400
#define BASICMOVER_TRANSLATE_AT_VELOCITY_X					0x1000
#define BASICMOVER_TRANSLATE_AT_VELOCITY_Y					0x2000
#define BASICMOVER_TRANSLATE_AT_VELOCITY_Z					0x4000

//============================================================================

class CBasicMover : public CActorComponent, public CBasicMoverIF
{
public:
	// component base stuff
	CBasicMover( CCompActor& i_actor );
	virtual ~CBasicMover();
	const char *ComponentName()  { return "BasicMover"; }
	const char *ComponentClass() { return "KinematicDriver"; }
	virtual void Activate( void );
	virtual void SyncActor( void );
	virtual void BeginUpdate();
	virtual void AttemptUpdate( float i_deltaSec );
	virtual void EndUpdate( float i_deltaSec );

	// Get at interfaces
	virtual CInterface* GetInterface(uint i_id);

	// translational movement to absolute position
	virtual void MoveTo( Vector3CRef pos, float duration );
	virtual void MoveToX( float pos, float duration );
	virtual void MoveToY( float pos, float duration );
	virtual void MoveToZ( float pos, float duration );

	// translational movement to relative position
	virtual void MoveRel( Vector3CRef pos, float duration );
	virtual void MoveRelX( float pos, float duration );
	virtual void MoveRelY( float pos, float duration );
	virtual void MoveRelZ( float pos, float duration );

	// rotate to absolute position
	virtual void RotateTo( Vector3CRef rot, float duration );
	virtual void RotateToX( float rot, float duration );
	virtual void RotateToY( float rot, float duration );
	virtual void RotateToZ( float rot, float duration );

	// rotate to relative position
	virtual void RotateRel( Vector3CRef rot, float duration );
	virtual void RotateRelX( float rot, float duration );
	virtual void RotateRelY( float rot, float duration );
	virtual void RotateRelZ( float rot, float duration );

	// apply constant velocities
	virtual void MoveAtConstantVelocity( Vector3CRef vel );
	virtual void RotateAtConstantVelocity( Vector3CRef vel );

private:
	void AttemptTranslate( float i_deltaSec );
	void AttemptRotate( float i_deltaSec );

	int		d_beginActionFlags;
	Vector3	d_beginRotateTime;
	Vector3	d_beginTranslateTime;
	Vector3	d_beginRotate;
	Vector3	d_beginBodyInWorld;

	int		d_actionFlags;

	Vector3	d_rotate;
	Vector3	d_rotateStart;
	Vector3	d_rotateEnd;
	Vector3	d_rotateTime;
	Vector3	d_rotateEndTime;

	Vector3	d_translateStart;
	Vector3	d_translateEnd;
	Vector3	d_translateTime;
	Vector3	d_translateEndTime;
};

//============================================================================

#endif //_BASIC_MOVER_H
