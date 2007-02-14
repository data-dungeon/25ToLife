#include "TTLPCH.h"

#include "components/KineControl/ViewDriver.h"
#include "Components/ComponentCreator.h"
#include "platform/CMouse.h"
#include "Math/sfilter.h"
#include "components/KineControl/BipedModel.h"
#include "avatar/avatarfx.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(ViewDriver, ViewDriver, COMPONENT_DRIVER_PRIORITY);

typedef struct tag_ViewDefaultParams
{
	float maxCameraTurnRate;
	float maxCameraPitchRate;
	float minCameraPitch;
	float maxCameraPitch;
	float cameraAccelerationTime;
	float onTargetSlowdown;
	float cameraAimModifier;
	float peekDistance;
} ViewDefaultParams;

typedef struct tag_ControllerDefaultParams
{
	float maxControllerSpeed;
	float minControllerSpeed;
	float maxControllerAccelerationTime;
	float minControllerAccelerationTime;
} ControllerDefaultParams;

ViewDefaultParams ViewDefaults;
ControllerDefaultParams ControllerDefaults;

void CParam<ViewDefaultParams>::RegisterVariables()
{
	RegisterVariable( d_data.maxCameraTurnRate, "maxCameraTurnRate", 5.0f, 0.0f, 20.0f );
	RegisterVariable( d_data.maxCameraPitchRate, "maxCameraPitchRate", 2.5f, 0.0f, 20.0f );
	RegisterVariable( d_data.minCameraPitch, "minCameraPitch", -60.0f, -180.0f, 180.0f );
	RegisterVariable( d_data.maxCameraPitch, "maxCameraPitch", 60.0f, -180.0f, 180.0f );
	RegisterVariable( d_data.cameraAccelerationTime, "cameraAccelerationTime", 0.3f, 0.0f, 10.0f );
	RegisterVariable( d_data.onTargetSlowdown, "onTargetSlowdown", 0.7f, 0.0f, 5.0f );
	RegisterVariable( d_data.cameraAimModifier, "cameraAimModifier", 1.0f, 0.0f, 5.0f );
	RegisterVariable( d_data.peekDistance, "peekDistance", 0.55f, 0.0f, 2.0f );
}

void CParam<ControllerDefaultParams>::RegisterVariables()
{
	RegisterVariable( d_data.minControllerSpeed, "minControllerSpeed", 1.0f, 0.0f, 20.0f );
	RegisterVariable( d_data.maxControllerSpeed, "maxControllerSpeed", 5.0f, 0.0f, 20.0f );
	RegisterVariable( d_data.minControllerAccelerationTime, "minControllerAccelerationTime", 0.0f, 0.0f, 10.0f );
	RegisterVariable( d_data.maxControllerAccelerationTime, "maxControllerAccelerationTime", 0.6f, 0.0f, 10.0f );
}

void ViewDriver::ControlSettingsHandler::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->UpdateControllerSettings();
}

void ViewDriver::UpdateControllerSettings( void )
{
	AvatarControlMapper *avatarControl = (AvatarControlMapper *)GetActor().GetProperty("ControlMapper")->GetPointer();

	// adjust the aim axis ranges according to the controlmapper's specified sensitivity
	if( avatarControl )
	{
		float speed = controllerSpeed.DeNormalize( avatarControl->GetControllerSpeed() );
		float sensitivity = controllerAcceleration.DeNormalize( 1.0f - avatarControl->GetControllerSensitivity() );

		float turnpitchratio = GetAxisRange( View_Axis_CameraPitchRate ).Max() / GetAxisRange( View_Axis_CameraTurnRate ).Max();
		SetAxisRange( View_Axis_CameraTurnRate, AxisRange( speed ) );
		SetAxisRange( View_Axis_CameraPitchRate, AxisRange( speed * turnpitchratio ) );
		cameraAccelerationTime = sensitivity;
	}

#ifdef DIRECTX_PC
	// --- look to override var file if we have a mouse and keyboard
	if( g_CMouse.isMouseActive( ) )
	{
		axisRange[View_Axis_CameraTurnRate] = AxisRange( 7.0f );
		axisRange[View_Axis_CameraPitchRate] = AxisRange( 7.0f );
		cameraAccelerationTime = 0.001f;
	}
#endif
}

void ViewDriver::SetDefaults( void )
{
	// these are not modifiable in the var file, but should stay at the -1.0 to 1.0 range
	axisRange[View_Axis_CameraTurnRateTarget] = AxisRange( 1.0f );
	axisRange[View_Axis_CameraPitchRateTarget] = AxisRange( 1.0f );

	ViewDefaultParams *defaults = CParam<ViewDefaultParams>::GetParam( "View", "var" );

	axisRange[View_Axis_CameraTurnRate] = AxisRange( defaults->maxCameraTurnRate );
	axisRange[View_Axis_CameraPitchRate] = AxisRange( defaults->maxCameraPitchRate );
	axisRange[View_Axis_CameraPitch] = AxisRange( defaults->minCameraPitch, defaults->maxCameraPitch );
	cameraAccelerationTime = defaults->cameraAccelerationTime;
	onTargetSlowdown = defaults->onTargetSlowdown;
	aimModifier = defaults->cameraAimModifier;
	peekDistance = defaults->peekDistance;
}

#pragma warning(disable : 4355)  // Allow this pointer in initializer list
ViewDriver::ViewDriver(CCompActor &i_actor) : CActorComponent( i_actor )
{
	int i;

	for( i = 0; i < View_Axis_Count; i++ )
		axisPosition[ i ] = 0.0f;

	SetDefaults();

	ControllerDefaultParams *defaults = CParam<ControllerDefaultParams>::GetParam( "Controller", "var" );
	controllerAcceleration = AxisRange( defaults->minControllerAccelerationTime, defaults->maxControllerAccelerationTime, false );
	controllerSpeed = AxisRange( defaults->minControllerSpeed, defaults->maxControllerSpeed, false );

	sightMatrix.Identity();

	onTarget = false;
}
#pragma warning(default : 4355)

ViewDriver::~ViewDriver()
{
}

void ViewDriver::Deactivate( void )
{
	CActorComponent::Deactivate();
	
	SetAxis( View_Axis_CameraPitch, 0.0f );
}

void ViewDriver::SetAxisNormalized( int axis, float position )
{
	// Assuming that the minimum is negative and the max is positive.
	// They don't both have to be the same value, but zero should always
	// normalize to zero, so a centered stick position does nothing
	ASSERT( GetAxisRange( axis ).Max() >= 0.0f && GetAxisRange( axis ).Min() <= 0.0f );
	SetAxis( axis, GetAxisRange( axis ).DeNormalize( position ) );
}

void ViewDriver::SyncActor( void )
{
	sightMatrix = actor.GetBodyToWorld();
}

void ViewDriver::Initialize( void )
{
	controlSettingsHandler.Initialize( "UpdateControllerSettings", MESSAGE_HANDLE_IF_MASTER,
		actor.GetMessageOwner(), this);
	onTarget = false;
	eyePointOffset = 0.0f;
}
#ifdef DIRECTX_PC
void ViewDriver::BeginFrame( void )
{
	if( !IsActive() )
		return;

	float i_deltaSec = g_timer.GetFrameSec();

	AvatarControlMapper *avatarControl = (AvatarControlMapper *)GetActor().GetProperty("ControlMapper")->GetPointer();

	float y = avatarControl->GetAxis( AvatarControlMapper::PITCH );
	float x = avatarControl->GetAxis( AvatarControlMapper::YAW );
	if (ValidSingleton(AvatarFX))
		g_avatarFX.AimModify(x, y);

	//if (onTarget)
	//{
	//	x *= onTargetSlowdown;
	//	y *= onTargetSlowdown;
	//}
	
	// slow down if aiming
	bool aiming = false;
	WeaponInventory *inv = (WeaponInventory *)actor.GetComponentByName( "WeaponInventory" );
	if( inv && inv->GetSelected() )
	{
		AimingSimulation &aimSim = inv->GetSelected()->GetAimSim();
		if( aimSim.GetAiming() )
			aiming = true;
	}
	if( aiming )
	{
		float modifier;
		Property *prop = actor.QueryProperty( "aimTurning" );
		if( prop )
			modifier = prop->GetFloat();
		else
			modifier = aimModifier;
		x *= modifier;
		y *= modifier;
	}
	
	SetAxisNormalized( View_Axis_CameraTurnRateTarget, x );
	SetAxisNormalized( View_Axis_CameraPitchRateTarget, y );

	//// filter turnrate towards turnratetarget
	float target = axisPosition[View_Axis_CameraTurnRateTarget] * GetAxisRange( View_Axis_CameraTurnRate ).Max();
	//// don't filter when the target is lower than the current rate
	//if( (target < axisPosition[View_Axis_CameraTurnRate] && axisPosition[View_Axis_CameraTurnRate] > 0.0f) ||
	//	(target > axisPosition[View_Axis_CameraTurnRate] && axisPosition[View_Axis_CameraTurnRate] < 0.0f) )
	//{
		axisPosition[View_Axis_CameraTurnRate] = target;
	//}
	//else
	//{
	//	axisPosition[View_Axis_CameraTurnRate] = FOFilterImplicit( axisPosition[View_Axis_CameraTurnRate], 
	//		target, i_deltaSec, cameraAccelerationTime );
	//}

	//// now do the same for pitch
	target = axisPosition[View_Axis_CameraPitchRateTarget] * GetAxisRange( View_Axis_CameraPitchRate ).Max();
	//// don't filter when the target is lower than the current rate
	//if( (target < axisPosition[View_Axis_CameraPitchRate] && axisPosition[View_Axis_CameraPitchRate] > 0.0f) ||
	//	(target > axisPosition[View_Axis_CameraPitchRate] && axisPosition[View_Axis_CameraPitchRate] < 0.0f) )
	//{
		axisPosition[View_Axis_CameraPitchRate] = target;
	//}
	//else
	//{
	//	axisPosition[View_Axis_CameraPitchRate] = FOFilterImplicit( axisPosition[View_Axis_CameraPitchRate], 
	//		target, i_deltaSec, cameraAccelerationTime );
	//}

	// check to see if we're centering the view or not
	if ( g_inputPtr && g_inputPtr->GetController(0)->IsCenteringView())
		SetAxis( View_Axis_CameraPitch, 0.0f );
	else
		SetAxis( View_Axis_CameraPitch, axisPosition[View_Axis_CameraPitch] + Math::Rad2Deg( axisPosition[View_Axis_CameraPitchRate] * i_deltaSec ) );

	// if we're turning the sight matrix, turn the entire body the same amount
	if( !Math::Zero( axisPosition[View_Axis_CameraTurnRate] ) )
	{
		float dh = axisPosition[View_Axis_CameraTurnRate] * i_deltaSec;
		CMoveIF *move = (CMoveIF *)GetActor().GetInterface( CMoveIF::GetID() );
		if( move )
			move->CommandTurnRad( dh );
	}

	float desiredOffset = 0.0f;

	Property *prop;
	bool sprint = false;
	prop = actor.QueryProperty( "sprint" );
	if( prop )
		sprint = prop->GetBool();
	
	// add and subtract just to make sure the view is centered if both are held
	if( avatarControl->ButtonDown( AvatarControlMapper::PEEKLEFT ) && !sprint )
	{
		desiredOffset += peekDistance;
		CMoveIF *move = (CMoveIF *)GetActor().GetInterface( CMoveIF::GetID() );
		if( move )
			move->CommandTurnToView( GetActor() );
	}
	if( avatarControl->ButtonDown( AvatarControlMapper::PEEKRIGHT ) && !sprint)
	{
		desiredOffset -= peekDistance;
		CMoveIF *move = (CMoveIF *)GetActor().GetInterface( CMoveIF::GetID() );
		if( move )
			move->CommandTurnToView( GetActor() );
	}

	eyePointOffset = FOFilterImplicit( eyePointOffset, desiredOffset, i_deltaSec, 0.2f );
}
#else
void ViewDriver::BeginFrame( void )
{
	if( !IsActive() )
		return;

	float i_deltaSec = g_timer.GetFrameSec();

	AvatarControlMapper *avatarControl = (AvatarControlMapper *)GetActor().GetProperty("ControlMapper")->GetPointer();

	float y = avatarControl->GetAxis( AvatarControlMapper::PITCH );
	float x = avatarControl->GetAxis( AvatarControlMapper::YAW );
	if (ValidSingleton(AvatarFX))
		g_avatarFX.AimModify(x, y);
		
	if (onTarget)
	{
		x *= onTargetSlowdown;
		y *= onTargetSlowdown;
	}
	
	// slow down if aiming
	bool aiming = false;
	WeaponInventory *inv = (WeaponInventory *)actor.GetComponentByName( "WeaponInventory" );
	if( inv && inv->GetSelected() )
	{
		AimingSimulation &aimSim = inv->GetSelected()->GetAimSim();
		if( aimSim.GetAiming() )
			aiming = true;
	}
	if( aiming )
	{
		float modifier;
		Property *prop = actor.QueryProperty( "aimTurning" );
		if( prop )
			modifier = prop->GetFloat();
		else
			modifier = aimModifier;
		x *= modifier;
		y *= modifier;
	}

	SetAxisNormalized( View_Axis_CameraTurnRateTarget, x );
	SetAxisNormalized( View_Axis_CameraPitchRateTarget, y );

	// filter turnrate towards turnratetarget
	float target = axisPosition[View_Axis_CameraTurnRateTarget] * GetAxisRange( View_Axis_CameraTurnRate ).Max();
	// don't filter when the target is lower than the current rate
	if( (target < axisPosition[View_Axis_CameraTurnRate] && axisPosition[View_Axis_CameraTurnRate] > 0.0f) ||
		(target > axisPosition[View_Axis_CameraTurnRate] && axisPosition[View_Axis_CameraTurnRate] < 0.0f) )
	{
		axisPosition[View_Axis_CameraTurnRate] = target;
	}
	else
	{
		axisPosition[View_Axis_CameraTurnRate] = FOFilterImplicit( axisPosition[View_Axis_CameraTurnRate], 
			target, i_deltaSec, cameraAccelerationTime );
	}

	// now do the same for pitch
	target = axisPosition[View_Axis_CameraPitchRateTarget] * GetAxisRange( View_Axis_CameraPitchRate ).Max();
	// don't filter when the target is lower than the current rate
	if( (target < axisPosition[View_Axis_CameraPitchRate] && axisPosition[View_Axis_CameraPitchRate] > 0.0f) ||
		(target > axisPosition[View_Axis_CameraPitchRate] && axisPosition[View_Axis_CameraPitchRate] < 0.0f) )
	{
		axisPosition[View_Axis_CameraPitchRate] = target;
	}
	else
	{
		axisPosition[View_Axis_CameraPitchRate] = FOFilterImplicit( axisPosition[View_Axis_CameraPitchRate], 
			target, i_deltaSec, cameraAccelerationTime );
	}

	SetAxis( View_Axis_CameraPitch, axisPosition[View_Axis_CameraPitch] + Math::Rad2Deg( axisPosition[View_Axis_CameraPitchRate] * i_deltaSec ) );

	// if we're turning the sight matrix, turn the entire body the same amount
	if( !Math::Zero( axisPosition[View_Axis_CameraTurnRate] ) )
	{
		float dh = axisPosition[View_Axis_CameraTurnRate] * i_deltaSec;
		CMoveIF *move = (CMoveIF *)GetActor().GetInterface( CMoveIF::GetID() );
		if( move )
			move->CommandTurnRad( dh );
	}

	float desiredOffset = 0.0f;

	Property *prop;
	bool sprint = false;
	prop = actor.QueryProperty( "sprint" );
	if( prop )
		sprint = prop->GetBool();

	// add and subtract just to make sure the view is centered if both are held
	if( avatarControl->ButtonDown( AvatarControlMapper::PEEKLEFT ) && !sprint )
	{
		desiredOffset += peekDistance;
		CMoveIF *move = (CMoveIF *)GetActor().GetInterface( CMoveIF::GetID() );
		if( move )
			move->CommandTurnToView( GetActor() );
	}
	if( avatarControl->ButtonDown( AvatarControlMapper::PEEKRIGHT ) && !sprint )
	{
		desiredOffset -= peekDistance;
		CMoveIF *move = (CMoveIF *)GetActor().GetInterface( CMoveIF::GetID() );
		if( move )
			move->CommandTurnToView( GetActor() );
	}

	eyePointOffset = FOFilterImplicit( eyePointOffset, desiredOffset, i_deltaSec, 0.2f );
}
#endif

void ViewDriver::EndUpdate( float i_deltaSec )
{
	// if we're suspended rather than deactivated, we still need to give back the correct sight matrix,
	// so don't clobber it
	if( !IsActive() )
	{
		if( !Suspended() )
			sightMatrix = GetActor().GetBodyToWorld();
		return;
	}

	// rotate the sight matrix?
	sightMatrix.CatYRotation( -axisPosition[View_Axis_CameraTurnRate] * i_deltaSec );
	
	// Build the sight vector
	Vector3 sightVector = sightMatrix.GetRow2();

	// Reflect only heading (I think this is already true but am doing this
	// out of paranoia).
	sightVector.Y(0.0f);
	sightVector.Normalize();

	// Add in ptich.
	float sPitch, cPitch;
#ifdef DIRECTX_PC
#define MAX_PITCH 85.0f
	axisPosition[View_Axis_CameraPitch] = Math::Clamp(axisPosition[View_Axis_CameraPitch], -MAX_PITCH, MAX_PITCH);
#endif //DIRECTX_PC
	Math::SinCos(Math::Deg2Rad(axisPosition[View_Axis_CameraPitch]),
		sPitch, cPitch);
	sightVector *= cPitch;
	sightVector.Y(sPitch);
}

void ViewDriver::GetDirection(Vector3 &o_direction)
{
	const DirCos3x3 &sightMatrix = SightMatrix();
	o_direction = sightMatrix.GetRow2();
}

void ViewDriver::GetUp(Vector3 &o_up)
{
	const DirCos3x3 &sightMatrix = SightMatrix();
	o_up = sightMatrix.GetRow1();
}

void ViewDriver::GetOrientation(Vector3 &o_direction, Vector3 &o_up)
{
	const DirCos3x3 &sightMatrix = SightMatrix();
	o_direction = sightMatrix.GetRow2();
	o_up = sightMatrix.GetRow1();
}

void ViewDriver::GetEyepoint(Vector3 &o_eyepoint)
{
	Vector3 offset( eyePointOffset, 0.0f, 0.0f );
	offset = offset * SightMatrix();
	o_eyepoint = GetActor().GetFocusInWorld() + offset;
}

DirCos3x3 ViewDriver::SightMatrix( void )
{
	DirCos3x3 matrix = sightMatrix;
	DirCos3x3 rot;

	// give back the real sight matrix even if we're suspended, since that almost certainly means simply paused.
	if( IsActive() || Suspended() )
	{
		rot.SetXRotation( -Math::Deg2Rad( axisPosition[View_Axis_CameraPitch] ) );

		matrix = rot * matrix;
		return matrix;
	}

	return GetActor().GetBodyToWorld();
}

void ViewDriver::SetSightMatrix( const DirCos3x3 &matrix )
{ 
	// if somebody is trying to set our matrix directly, we need to ignore any lean or tilt
	// component, and flatten it out.
	Vector3 forward, side, up;

	forward = matrix.GetRow2();
	side = matrix.GetRow0();
	forward.y( 0.0f );
	side.y( 0.0f );
	
	// if forward or side is straight up, we're going to have stress.  Bail and fail
	if( forward.SafeNormalize() && side.SafeNormalize() )
	{
		sightMatrix.SetRow0( side );
		sightMatrix.SetRow2( forward );
		up.Set( 0.0f, 1.0f, 0.0f );
		sightMatrix.SetRow1( up );
	}
	else
	{
		sightMatrix = matrix;
	}

	// rebuild the sight vector
	Vector3 sightVector = sightMatrix.GetRow2();

	// Reflect only heading (I think this is already true but am doing this
	// out of paranoia).
	sightVector.Y(0.0f);
	sightVector.Normalize();

	// Add in ptich.
	float sPitch, cPitch;
	Math::SinCos(Math::Deg2Rad(axisPosition[View_Axis_CameraPitch]),
		sPitch, cPitch);
	sightVector *= cPitch;
	sightVector.Y(sPitch);
}

CInterface* ViewDriver::GetInterface(uint i_id)
{
	if (i_id == ViewIF::GetID())
		return( this );
	return(NULL);
}

