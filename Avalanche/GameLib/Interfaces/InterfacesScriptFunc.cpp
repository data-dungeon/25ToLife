//============================================================================
//=
//= InterfacesScriptFunc.cpp - 
//=    Script Interface routines to module "game"
//=
//============================================================================

#include "Interfaces/InterfacesPCH.h"

#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"
#include "stage/cactor.h"
#include "stage/CCompActor.h"

#include "Interfaces/MoveIF.h"
#include "Interfaces/ViewIF.h"
#include "Interfaces/AnimActionIF.h"
#include "Interfaces/RigidBodyIF.h"
#include "Interfaces/BasicMoverIF.h"
#include "Interfaces/InventoryListIF.h"

// PersistentData interface includes
#include "GameHelper/PersistentData.h"


//============================================================================

static CMoveIF* GetMoveIF( ScriptVM* vm )
{
	ASSERT_PTR( vm );

	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );

	return (CMoveIF *)actor->GetInterface( CMoveIF::GetID() );
}

//============================================================================

static void Script_MoveHeading( ScriptVM* vm )
{
	float degrees = vm->PopFloat();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandHeadingDeg( degrees );
}

//============================================================================

static void Script_MoveTurnRate( ScriptVM* vm )
{
	float degreesPerSec = vm->PopFloat();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandTurnRateDeg( degreesPerSec, false );
}

//============================================================================

static void Script_MoveTurnRateNormalized( ScriptVM* vm )
{
	float fracPerSec = vm->PopFloat();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandTurnRateRad( fracPerSec, true );
}

//============================================================================

static void Script_MoveDirection( ScriptVM* vm )
{
	float speed = vm->PopFloat();
	Vector3 dir = vm->PopVector3();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandMoveDirection( dir, speed, false );
}

//============================================================================

static void Script_MoveDirectionNormalized( ScriptVM* vm )
{
	float speed = vm->PopFloat();
	Vector3 dir = vm->PopVector3();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandMoveDirection( dir, speed, true );
}

//============================================================================

static void Script_MoveToPoint( ScriptVM* vm )
{
	float speed = vm->PopFloat();
	Vector3 pos = vm->PopVector3();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandMoveToPoint( pos, speed, false );
}

//============================================================================

static void Script_MoveToPointNormalized( ScriptVM* vm )
{
	float speed = vm->PopFloat();
	Vector3 pos = vm->PopVector3();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandMoveToPoint( pos, speed, true );
}

//============================================================================

static void Script_MoveForward( ScriptVM* vm )
{
	float speed = vm->PopFloat();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandForwardSpeed( speed, false );
}

//============================================================================

static void Script_MoveForwardNormalized( ScriptVM* vm )
{
	float speed = vm->PopFloat();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandForwardSpeed( speed, true );
}

//============================================================================

static void Script_MoveSideways( ScriptVM* vm )
{
	float speed = vm->PopFloat();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandSideSpeed( speed, false );
}

//============================================================================

static void Script_MoveSidewaysNormalized( ScriptVM* vm )
{
	float speed = vm->PopFloat();

	CMoveIF* moveIF = GetMoveIF( vm );
	if ( moveIF )
		moveIF->CommandSideSpeed( speed, true );
}

//============================================================================

static CAnimActionIF* GetAnimActionIF( ScriptVM* vm )
{
	ASSERT_PTR( vm );

	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );

	return (CAnimActionIF *)actor->GetInterface( CAnimActionIF::GetID() );
}

//============================================================================

static void Script_StartAction( ScriptVM* vm )
{
	int action = vm->PopInt();

	CAnimActionIF *animaction = GetAnimActionIF( vm );
	if( animaction )
		animaction->StartAction( action );
}

//============================================================================

static void Script_EndAction( ScriptVM* vm )
{
	int action = vm->PopInt();

	CAnimActionIF *animaction = GetAnimActionIF( vm );
	if( animaction )
		animaction->EndAction( action );
}

//============================================================================

static void Script_ActionInProgress( ScriptVM* vm )
{
	int action = vm->PopInt();

	CAnimActionIF *animaction = GetAnimActionIF( vm );
	if( animaction )
		vm->Push( animaction->ActionInProgress( action ) );
}

//============================================================================

static ViewIF* GetViewIF( ScriptVM* vm )
{
	ASSERT_PTR( vm );

	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );

	return (ViewIF *)actor->GetInterface( ViewIF::GetID() );
}

//============================================================================

static void Script_ViewLock( ScriptVM *vm )
{
	ViewIF *view = GetViewIF( vm );
	if( view )
		view->LockView();
}

//============================================================================

static void Script_ViewUnlock( ScriptVM *vm )
{
	ViewIF *view = GetViewIF( vm );
	if( view )
		view->UnlockView();
}

//============================================================================

static void Script_ViewLookAtPoint( ScriptVM *vm )
{
	Vector3 v = vm->PopVector3();
	Vector3 up( 0.0f, 1.0f, 0.0f );
	ViewIF *view = GetViewIF( vm );
	if( view )
		view->LookAt( v, up );
}

//============================================================================

static void Script_ViewLookDirection( ScriptVM *vm )
{
	Vector3 v = vm->PopVector3();
	Vector3 up( 0.0f, 1.0f, 0.0f );
	ViewIF *view = GetViewIF( vm );
	if( view )
		view->LookDirection( v, up );
}

//============================================================================

static void Script_ViewGetDirection( ScriptVM *vm )
{
	Vector3 v( 0.0f, 0.0f, 1.0f );
	ViewIF *view = GetViewIF( vm );
	if( view )
		view->GetDirection( v );
	vm->Push( v );
}

//============================================================================

static void Script_ViewGetHeadingOffset( ScriptVM *vm )
{
	float offsetDegrees = 0.0f;
	ViewIF *view = GetViewIF( vm );
	if( view )
	{
		CActor* actor = (CActor*) vm->GetOwner();
		ASSERT_PTR( actor );
		Vector3 v;

		view->GetDirection( v );
		v = v / actor->GetBodyToWorld();
		v.y( 0.0f );
		v.SafeNormalize();
		offsetDegrees = Math::ArcTan2Deg( v.x(), v.z() );
	}
	vm->Push( offsetDegrees );
}

//============================================================================

static void Script_RigidBodySetMass( ScriptVM* vm )
{
	float			mass      = vm->PopFloat();
	CCompActor* actor     = (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CRigidBodyIF* rigidBodyIF = (CRigidBodyIF*) actor->GetInterface( CRigidBodyIF::GetID() );
	ASSERT_PTR( rigidBodyIF );

	rigidBodyIF->SetMass( mass );
}

//============================================================================

static void Script_RigidBodySetInertia( ScriptVM* vm )
{
	float			Izz      = vm->PopFloat();
	float			Iyy      = vm->PopFloat();
	float			Ixx      = vm->PopFloat();
	CCompActor* actor     = (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CRigidBodyIF* rigidBodyIF = (CRigidBodyIF*) actor->GetInterface( CRigidBodyIF::GetID() );
	ASSERT_PTR( rigidBodyIF );

	rigidBodyIF->SetInertia( Ixx, Iyy, Izz );
}

//============================================================================

static void Script_RigidBodySetBodyAcceleration( ScriptVM* vm )
{
	bool        autoApply = vm->PopBool();
	Vector3     accel     = vm->PopVector3();
	CCompActor* actor     = (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CRigidBodyIF* rigidBodyIF = (CRigidBodyIF*) actor->GetInterface( CRigidBodyIF::GetID() );
	ASSERT_PTR( rigidBodyIF );

	rigidBodyIF->SetBodyAcceleration( accel, autoApply );
}

//============================================================================

static void Script_RigidBodySetBodyVelocity( ScriptVM* vm )
{
	Vector3     velocity	= vm->PopVector3();
	CCompActor* actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CRigidBodyIF* rigidBodyIF = (CRigidBodyIF*) actor->GetInterface( CRigidBodyIF::GetID() );
	ASSERT_PTR( rigidBodyIF );

	rigidBodyIF->SetCGVelocityInWorld( velocity );
}

//============================================================================

static void Script_RigidBodySetBodyAngularVelocity( ScriptVM* vm )
{
	Vector3     velocity	= vm->PopVector3();
	CCompActor* actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CRigidBodyIF* rigidBodyIF = (CRigidBodyIF*) actor->GetInterface( CRigidBodyIF::GetID() );
	ASSERT_PTR( rigidBodyIF );

	rigidBodyIF->SetAngVelocityInCG( velocity );
}

//============================================================================

static void Script_RigidBodyGetKineticEnergy( ScriptVM* vm )
{
	CCompActor* actor		= (CCompActor*) vm->PopActor();
	float kineticEnergy;

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CRigidBodyIF* rigidBodyIF = (CRigidBodyIF*) actor->GetInterface( CRigidBodyIF::GetID() );
	ASSERT_PTR( rigidBodyIF );

	kineticEnergy = rigidBodyIF->ComputeKineticEnergy();
	vm->Push( kineticEnergy );
}

//============================================================================

static void Script_MoveTo( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	Vector3		pos		= vm->PopVector3();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->MoveTo( pos, duration );
}

//============================================================================

static void Script_MoveToX( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->MoveToX( pos, duration );
}

//============================================================================

static void Script_MoveToY( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->MoveToY( pos, duration );
}

//============================================================================

static void Script_MoveToZ( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->MoveToZ( pos, duration );
}

//============================================================================

static void Script_MoveRel( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	Vector3		pos		= vm->PopVector3();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->MoveRel( pos, duration );
}

//============================================================================

static void Script_MoveRelX( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->MoveRelX( pos, duration );
}

//============================================================================

static void Script_MoveRelY( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->MoveRelY( pos, duration );
}

//============================================================================

static void Script_MoveRelZ( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->MoveRelZ( pos, duration );
}

//============================================================================

static void Script_RotateTo( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	Vector3		pos		= vm->PopVector3();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->RotateTo( pos, duration );
}

//============================================================================

static void Script_RotateToX( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->RotateToX( pos, duration );
}

//============================================================================

static void Script_RotateToY( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->RotateToY( pos, duration );
}

//============================================================================

static void Script_RotateToZ( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->RotateToZ( pos, duration );
}

//============================================================================

static void Script_RotateRel( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	Vector3		pos		= vm->PopVector3();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->RotateRel( pos, duration );
}

//============================================================================

static void Script_RotateRelX( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->RotateRelX( pos, duration );
}

//============================================================================

static void Script_RotateRelY( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->RotateRelY( pos, duration );
}

//============================================================================

static void Script_RotateRelZ( ScriptVM* vm )
{
	float			duration = vm->PopFloat();
	float			pos		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->RotateRelZ( pos, duration );
}

//============================================================================

static void Script_MoveAtConstantVelocity( ScriptVM* vm )
{
	Vector3		vel		= vm->PopVector3();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->MoveAtConstantVelocity( vel );
}

//============================================================================

static void Script_RotateAtConstantVelocity( ScriptVM* vm )
{
	Vector3		vel		= vm->PopVector3();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CBasicMoverIF* moverIF = (CBasicMoverIF*) actor->GetInterface( CBasicMoverIF::GetID() );
	ASSERT_PTR( moverIF );
	if ( moverIF )
		moverIF->RotateAtConstantVelocity( vel );
}

//============================================================================

static void Script_AddInventory( ScriptVM* vm )
{
	int	value = vm->PopInt();
	const char *key = vm->PopString();
	CCompActor*	actor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );
	ASSERT_PTR( key );

	CInventoryListIF* iIF = CInventoryListIF::GetInterface( actor );
	if( iIF )
	{
		iIF->AddInventory( key, value );
	}
}

//============================================================================

static void Script_GetInventory( ScriptVM* vm )
{
	const char *key = vm->PopString();
	CActor*	actor = (CActor*) vm->PopActor();
	ASSERT_PTR( key );

	CInventoryListIF* iIF = CInventoryListIF::GetInterface( actor );
	vm->Push( iIF ? iIF->GetInventory(key) : 0 );
}

//============================================================================

static void Script_RemoveInventory( ScriptVM* vm )
{
	int	value = vm->PopInt();
	const char *key = vm->PopString();
	CCompActor*	actor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );
	ASSERT_PTR( key );

	CInventoryListIF* iIF = CInventoryListIF::GetInterface( actor );
	if( iIF )
	{
		iIF->RemoveInventory( key, value );
	}
}

//============================================================================

static void Script_InventoryCheckpointHit( ScriptVM* vm )
{
	CCompActor* actor = (CCompActor*) vm->PopActor();
	CInventoryListIF* iIF = CInventoryListIF::GetInterface( actor );
	if ( iIF )
	{
		iIF->CheckpointHit();
	}
}

//============================================================================

static void Script_InventoryCheckpointReset( ScriptVM* vm )
{
	CCompActor* actor = (CCompActor*) vm->PopActor();
	CInventoryListIF* iIF = CInventoryListIF::GetInterface( actor );
	if ( iIF )
	{
		iIF->CheckpointReset();
	}
}

//============================================================================

static void Script_SetGlobal( ScriptVM* vm )
{
	int			value	= vm->PopInt();
	const char* name	= vm->PopString();
	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );
}

//============================================================================

void Interfaces_RegisterScriptFuncs()
{
	SCRIPT_REGISTER( MoveHeading					);
	SCRIPT_REGISTER( MoveTurnRate					);
	SCRIPT_REGISTER( MoveTurnRateNormalized	);
	SCRIPT_REGISTER( MoveDirection				);
	SCRIPT_REGISTER( MoveDirectionNormalized	);
	SCRIPT_REGISTER( MoveToPoint					);
	SCRIPT_REGISTER( MoveToPointNormalized		);
	SCRIPT_REGISTER( MoveForward					);
	SCRIPT_REGISTER( MoveForwardNormalized		);
	SCRIPT_REGISTER( MoveSideways					);
	SCRIPT_REGISTER( MoveSidewaysNormalized	);
	SCRIPT_REGISTER( StartAction					);
	SCRIPT_REGISTER( EndAction						);
	SCRIPT_REGISTER( ActionInProgress			);
	SCRIPT_REGISTER( ViewLock );
	SCRIPT_REGISTER( ViewUnlock );
	SCRIPT_REGISTER( ViewLookAtPoint );
	SCRIPT_REGISTER( ViewLookDirection );
	SCRIPT_REGISTER( ViewGetDirection );
	SCRIPT_REGISTER( ViewGetHeadingOffset );
	SCRIPT_REGISTER( RigidBodySetMass );
	SCRIPT_REGISTER( RigidBodySetInertia );
	SCRIPT_REGISTER( RigidBodySetBodyAcceleration );
	SCRIPT_REGISTER( RigidBodySetBodyVelocity );
	SCRIPT_REGISTER( RigidBodySetBodyAngularVelocity );
	SCRIPT_REGISTER( RigidBodyGetKineticEnergy );
	SCRIPT_REGISTER( MoveTo );
	SCRIPT_REGISTER( MoveToX );
	SCRIPT_REGISTER( MoveToY );
	SCRIPT_REGISTER( MoveToZ );
	SCRIPT_REGISTER( MoveRel );
	SCRIPT_REGISTER( MoveRelX );
	SCRIPT_REGISTER( MoveRelY );
	SCRIPT_REGISTER( MoveRelZ );
	SCRIPT_REGISTER( RotateTo );
	SCRIPT_REGISTER( RotateToX );
	SCRIPT_REGISTER( RotateToY );
	SCRIPT_REGISTER( RotateToZ );
	SCRIPT_REGISTER( RotateRel );
	SCRIPT_REGISTER( RotateRelX );
	SCRIPT_REGISTER( RotateRelY );
	SCRIPT_REGISTER( RotateRelZ );
	SCRIPT_REGISTER( MoveAtConstantVelocity );
	SCRIPT_REGISTER( RotateAtConstantVelocity );
	SCRIPT_REGISTER( AddInventory );
	SCRIPT_REGISTER( GetInventory );
	SCRIPT_REGISTER( RemoveInventory );
	SCRIPT_REGISTER( InventoryCheckpointHit );
	SCRIPT_REGISTER( InventoryCheckpointReset );
	SCRIPT_REGISTER( SetGlobal );
}


//=================================eof========================================
