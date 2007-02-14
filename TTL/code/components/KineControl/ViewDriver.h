#ifndef __VIEWDRIVER_H_INCLUDED__
#define __VIEWDRIVER_H_INCLUDED__

#include "Components/Component.h"
#include "Interfaces/MoveIF.h"
#include "Interfaces/ViewIF.h"
#include "Components/KineControl.h"
#include "BipedKineControl_s.h"

class ViewDriver : public CActorComponent, public ViewIF
{
public:
	ViewDriver( CCompActor &i_actor );
	virtual ~ViewDriver();

	const char *ComponentName() { return "ViewDriver"; }

	virtual float GetAxis( int axis ) { ASSERT( axis < View_Axis_Count ); return axisPosition[axis]; }
#ifndef DIRECTX_PC
	virtual void SetAxis( int axis, float position )  { axisPosition[axis] = GetAxisRange(axis).Clamp( position ); }
#else
	virtual void SetAxis( int axis, float position )		{ axisPosition[axis] = position ; }
	virtual void SetAxisClamped( int axis, float position )	{ axisPosition[axis] = GetAxisRange(axis).Clamp( position ); }
#endif
	virtual AxisRange GetAxisRange( int axis ) { ASSERT( axis < View_Axis_Count ); return axisRange[axis]; }
	virtual void SetAxisRange( int axis, const AxisRange &range ) { ASSERT( axis < View_Axis_Count ); axisRange[axis] = range; }
	virtual void SetAxisNormalized( int axis, float position );

	virtual void Deactivate( void );
	virtual void SyncActor( void );
	virtual void Initialize( void );

	virtual void BeginFrame( void );
	virtual void EndUpdate( float i_deltaSec );

	virtual CInterface* GetInterface(uint i_id);

	void SetDefaults( void );

	// View interface overrides
	virtual void GetDirection(Vector3 &o_direction);
	virtual void GetUp(Vector3 &o_up);
	virtual void GetOrientation(Vector3 &o_direction, Vector3 &o_up);
	virtual void GetEyepoint(Vector3 &o_eyepoint);
	virtual DirCos3x3 SightMatrix( void );

	virtual bool ViewLocked( void ) { if( IsActive() ) return true; else return false; }
	virtual void SetSightMatrix( const DirCos3x3 &matrix );

	void UpdateControllerSettings( void );

	// Slow down aiming over a target
	virtual void OnTarget(bool crosshairOnTarget)	
		{ onTarget = crosshairOnTarget; }

private:
	ViewDriver();

	float axisPosition[View_Axis_Count];
	float cameraAccelerationTime;

	AxisRange axisRange[View_Axis_Count];

	DirCos3x3 sightMatrix;

	float eyePointOffset;
	float peekDistance;

	AxisRange controllerAcceleration;
	AxisRange controllerSpeed;

	float			aimModifier;

	bool			onTarget;
	float			onTargetSlowdown;

	REF_SEND_MESSAGE_HANDLER(ControlSettingsHandler, ViewDriver) controlSettingsHandler;
};

#endif //__VIEWDRIVER_H_INCLUDED__
