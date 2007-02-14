#ifndef __BIPEDLADDER_H_INCLUDED__
#define __BIPEDLADDER_H_INCLUDED__

#include "Components/KineControl.h"
#include "Interfaces/MoveIF.h"
#include "Interfaces/ViewIF.h"
#include "BipedKineControl_s.h"

class BipedLadderController : public KinematicController, public CMoveIF
{
public:
	BipedLadderController( CCompActor &i_actor );
	virtual ~BipedLadderController();

	const char *ComponentName() { return "BipedLadder"; }
	const char *ComponentClass() { return "KinematicController"; }

	virtual bool DefaultActive( void ) { return false; }

	virtual float GetAxis( int axis ) { ASSERT( axis < Ladder_Axis_Count ); return axisPosition[axis]; }
	virtual void SetAxis( int axis, float position ) { axisPosition[axis] = GetAxisRange(axis).Clamp( position ); }
	virtual AxisRange GetAxisRange( int axis ) { ASSERT( axis < Ladder_Axis_Count ); return axisRange[axis]; }
	virtual void SetAxisRange( int axis, const AxisRange &range ) { ASSERT( axis < Ladder_Axis_Count ); axisRange[axis] = range; }

	void SetDefaults( void );

	virtual void Initialize( void );
	virtual void Activate( void );
	virtual void Deactivate( void );

	virtual void AttemptUpdate( float i_deltaSec );
	virtual void EndUpdate( float i_deltaSec );
	virtual void EndFrame( void );

	virtual CInterface* GetInterface(uint i_id);
	virtual bool CommandVerticalSpeed( float i_speed, bool i_normalized = false );

	void GetOffLadder( int reason );

	void LockToLadder( void );

	void SetLadder( OBB *ladder, bool i_oneSided = false ) {currentLadder = ladder; oneSided = i_oneSided;}

private:
	BipedLadderController();

	float axisPosition[Ladder_Axis_Count];

	AxisRange axisRange[Ladder_Axis_Count];

	float slideVel;
	float ladderPosition;
	float distFromLadder;
	float topClimbOff;
	float bottomClimbOff;
	u8 ladderMaterial;
	OBB *currentLadder;
	bool oneSided;

	REF_SEND_MESSAGE_HANDLER(OffLadder, BipedLadderController) d_offLadderHandler;
	REF_SEND_MESSAGE_HANDLER(ManDown, BipedLadderController) d_manDownHandler;
};

class LadderPhysicalControllerDriver : public CActorComponent
{
public:
	LadderPhysicalControllerDriver( CCompActor &i_actor ):CActorComponent( i_actor ){ skipControllerRead = false; pressOnUs = false; }
	virtual ~LadderPhysicalControllerDriver() {};

	const char *ComponentName() { return "LadderPhysicalController"; }
	const char *ComponentClass() { return "KinematicDriver"; }

	virtual bool DefaultActive( void ) { return false; }

	virtual void Activate( void ) { CActorComponent::Activate(); skipControllerRead = true; }
	
	virtual void BeginFrame( void );

private:
	bool skipControllerRead;
	bool pressOnUs;
};

#endif //__BIPEDLADDER_H_INCLUDED__
