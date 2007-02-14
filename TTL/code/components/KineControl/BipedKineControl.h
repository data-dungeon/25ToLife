#ifndef __BIPEDKINECONTROL_H_INCLUDED__
#define __BIPEDKINECONTROL_H_INCLUDED__

#include "Components/KineControl.h"
#include "Interfaces/MoveIF.h"
#include "BipedKineControl_s.h"

class TTLWeapon;
class BipedKinematicController;

//#define DEBUG_VAULTTRIGGER_SETUP

class BipedMoveIF : public CMoveIF
{
  public:
	BipedMoveIF(BipedKinematicController &i_controller) : d_controller(i_controller) {}
	virtual bool CommandTurnRad(float i_turnRad);
	virtual bool CommandHeadingRad(float i_headingRad);
	virtual bool CommandTurnRateRad(float i_turnRateRad, bool i_normalized = false);
	virtual bool CommandMoveDirection(Vector3CRef i_dir, float i_speed, bool i_normalized = false);
	virtual bool CommandMoveDirection(Vector3CRef i_velocity, bool i_normalized = false);
	virtual bool CommandMoveToPoint(Vector3CRef i_point, float i_speed, bool i_normalized = false);
	virtual bool CommandForwardSpeed(float i_forwardSpeed, bool i_normalized = false);
	virtual bool CommandSideSpeed(float i_sideSpeed, bool i_normalized = false);
	virtual float DistanceSquaredToPoint( Vector3CRef i_point );

	virtual bool Crouch( bool crouch = true );
	virtual bool JumpToTarget( Vector3CRef i_point );

  private:
	BipedMoveIF();
	BipedKinematicController &d_controller;
};

class BipedKinematicController : public KinematicController
{
public:
	BipedKinematicController( CCompActor &i_actor );
	virtual ~BipedKinematicController();

	const char *ComponentName() { return "Biped"; }
	const char *ComponentClass() { return "KinematicController"; }

	virtual bool DefaultActive( void ) { return false; }
	virtual bool DefaultInClass( void ) { return true; }

	virtual float GetAxis( int axis ) { ASSERT( axis < Biped_Axis_Count ); return axisPosition[axis]; }
	virtual void SetAxis( int axis, float position ) { axisPosition[axis] = GetAxisRange(axis).Clamp( position ); }
	virtual AxisRange GetAxisRange( int axis ) { ASSERT( axis < Biped_Axis_Count ); return axisRange[axis]; }
	virtual void SetAxisRange( int axis, const AxisRange &range ) { ASSERT( axis < Biped_Axis_Count ); axisRange[axis] = range; }

	virtual void SyncActor( void );

	virtual void Initialize( void );
	virtual void Activate( void );
	virtual void Deactivate( void );

	virtual void BeginFrame( void );
	virtual void BeginUpdate( void );
	virtual void AttemptUpdate( float i_deltaSec );
	virtual void EndUpdate( float i_deltaSec );
	virtual void EndFrame( void );

	virtual CInterface* GetInterface(uint i_id);

	virtual void VelocityDumped( Vector3CRef velChange );

	bool TestOnGround( void );

	void MoveDirection( Vector3CRef vel );
	void MoveDirection( Vector3CRef dir, float speed );
	void MoveDirectionNormalized( Vector3CRef vel );
	void MoveDirectionNormalized( Vector3CRef dir, float speed );

	void MoveToPoint( Vector3CRef dest, float speed );
	void MoveToPointNormalized( Vector3CRef dest, float speed );

	// Anim script interface
	virtual void Launch( Vector3CRef i_vel );
	virtual void Kick( void );

	void SetDefaults( void );

	float GetGravity( void ) {return gravity;}
	
	void SetInteractionActor( ActorHandle i_handle ) { interactionActor = i_handle; }
	void SetInteractionType( InteractionMonitor::InteractionType i_type ) { interactionType = i_type; }

private:
	BipedKinematicController();

	float axisPosition[Biped_Axis_Count];

	AxisRange axisRange[Biped_Axis_Count];
	float maxSidestepSpeed;
	float aimDuration;
	float airAcceleration;
	float airMaxSpeed;
	float gravity;
	float test;
	float maxPeek;
	float fallVelocityMin;
	float fallVelocityMax;
	float sprintFactor;

	bool determineDesiredVelocity;
	Vector3 desiredVelocity;

	bool groundTestCacheValid;
	Vector3 cachePosition;
	Vector3 cacheVelocity;
	bool cachedOnGround;
	Vector3 cachedGroundPosition;
	Vector3 cachedGroundNormal;
	int cachedGroundMaterial;
	u16 cachedGroundColor;

	float aimtime;
	float heading;
	float beginHeading;

	ActorHandle interactionActor;
	InteractionMonitor::InteractionType interactionType;

	float MaxSpeed();
	bool IsSprinting();

	BipedMoveIF d_moveIF;

	// message handling
	#ifdef DEBUG_VAULTTRIGGER_SETUP
		REF_SEND_MESSAGE_HANDLER(VaultTrigger, BipedKinematicController) vaultTrigger;
	#endif
	REF_SEND_MESSAGE_HANDLER(TriggeredVaultHandler, BipedKinematicController) triggeredVaultHandler;
	REF_SEND_MESSAGE_HANDLER(KickHandler, BipedKinematicController) kickHandler;
	REF_BASIC_MESSAGE_HANDLER(MoveToHandler, MESSAGE_STRING, BipedKinematicController) moveToHandler;
	REF_BASIC_MESSAGE_HANDLER(LaunchHandler, MESSAGE_VECTOR3_PTR, BipedKinematicController) launchHandler;
};

class BipedPhysicalControllerDriver : public CActorComponent
{
public:
	BipedPhysicalControllerDriver( CCompActor &i_actor ):CActorComponent( i_actor )
		{ skipControllerRead = false; frameHandle = INVALID_OBJECT_HANDLE; }
	virtual ~BipedPhysicalControllerDriver() {};

	const char *ComponentName() { return "BipedPhysicalController"; }
	const char *ComponentClass() { return "KinematicDriver"; }

	virtual void Initialize( void );

	virtual bool DefaultActive( void ) { return false; }
	virtual bool DefaultInClass( void ) { return true; }

	virtual void Attach( void );
	virtual void Activate( void ) { CActorComponent::Activate(); skipControllerRead = true; }
	virtual void Suspend( void );
	virtual void Deactivate();

	virtual void BeginFrame( void );

	char *GetInteractionAnim( int type );

private:
	BipedPhysicalControllerDriver();
	bool skipControllerRead;

	void StopTagging();
	ActorHandle frameHandle;	// added to handle continuous interaction with tag_frames

	// Utilities
	void CancelAim(TTLWeapon *weapon);

#ifdef DIRECTX_PC
	// control to pop out to 3rd person
	bool lockCamera;
#endif

	REF_BASIC_MESSAGE_HANDLER(ApprehendDone, MESSAGE_VALUE, BipedPhysicalControllerDriver) apprehendDoneHandler;
};

// a very simple component that goes away when the actor is at the target point
class MoveToPointDriver : public CActorComponent
{
public:
	MoveToPointDriver( CCompActor &i_actor ):CActorComponent( i_actor ) { reactivateWhenDone = GetActor().GetActiveComponentInClass( "KinematicDriver" ); if( reactivateWhenDone ) reactivateWhenDone->Deactivate(); }
	virtual ~MoveToPointDriver() { if( reactivateWhenDone ) reactivateWhenDone->Activate(); }

	const char *ComponentName() { return "MoveToPoint"; }

	virtual void BeginFrame( void );
	void MoveToPointAndFacing( Vector3CRef point, const DirCos3x3 &i_orientation, float normalizedSpeed );

private:
	CActorComponent *reactivateWhenDone;
	Vector3 destination;
	DirCos3x3 orientation;
	float speed;
};

#ifdef _DEBUG
class BipedPointToPointDriver : public CActorComponent
{
public:
	BipedPointToPointDriver( CCompActor &i_actor );
	virtual ~BipedPointToPointDriver() {};

	const char *ComponentName() { return "BipedPointToPoint"; }
	const char *ComponentClass() { return "KinematicDriver"; }

	virtual bool DefaultActive( void ) { return false; }

	virtual void Activate( void ) { movingToPoint = 0;CActorComponent::Activate(); }
	virtual void Deactivate( void ) { CActorComponent::Deactivate(); }

	virtual void AttemptUpdate( float dt );

private:
	BipedPointToPointDriver();

	int movingToPoint;
};
#endif

#endif //__BIPEDKINECONTROL_H_INCLUDED__
