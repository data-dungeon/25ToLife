//============================================================================
//=
//= StageScriptFunc.cpp -
//=    Script Interface routines to module "Stage"
//=
//============================================================================

#include "stage/StagePCH.h"

#include "stage/ActorCreator.h"
#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"
#include "Interfaces/CollisionIF.h"

//#include "Effects/footstep.h"
//#include "CollideHelper/XZEntity.h"
#include "CollideHelper/LineQuery.h"

//============================================================================

void Script_SetIgnoreEntity( ScriptVM *vm )
{
	CProtagonist* ignore = (CProtagonist *)vm->PopActor();
	CProtagonist* actor = (CProtagonist *)vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CProtagonist::ClassType() ) );
	ASSERT( ignore == NULL || ignore->IsA( CProtagonist::ClassType() ) );

	actor->SetIgnoreEntity( ignore );
}

//============================================================================

void Script_ActorDeactivatePathFollower( ScriptVM *vm )
{
	CCompActor* actor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CActorComponent *component = actor->GetComponentByName( "PathFollower" );
	if( component )
		component->Deactivate();
}

//============================================================================

void Script_ActorDeactivatePathFollowerCam( ScriptVM *vm )
{
	CCompActor* actor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CActorComponent *component = actor->GetComponentByName( "PathFollowerCam" );
	if( component )
	{
		component->Deactivate();
	}
}

//============================================================================

void Script_ActorActivatePathFollower( ScriptVM *vm )
{
	CCompActor* actor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CActorComponent *component = actor->GetComponentByName( "PathFollower" );
	if( component )
		component->Activate();
}

//============================================================================

void Script_ActorActivatePathFollowerCam( ScriptVM *vm )
{
	CCompActor* actor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CActorComponent *component = actor->GetComponentByName( "PathFollowerCam" );
	if( component )
	{
		component->Activate();
	}
}

//============================================================================

void Script_ActorGetPosition( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	vm->Push( actor->GetBodyInWorld() );
}

//============================================================================

void Script_ActorMatchPositionAndOrientation( ScriptVM* vm )
{
	CActor *target = vm->PopActor();
	CActor *actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT_PTR( target );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	ASSERT( target->IsA( CActor::ClassType() ) );

	if (actor && target)
	{
		actor->GetBodyInWorld() = target->GetBodyInWorld();
		actor->GetBodyToWorld() = target->GetBodyToWorld();
		actor->SetDiscontinuity( DISCONTINUITY_POSITION | DISCONTINUITY_ORIENTATION );
	}
}

//============================================================================

void Script_ActorGetVelocity( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	vm->Push( actor->GetVelocityInWorld() );
}

//============================================================================

void Script_ActorSetVelocity( ScriptVM* vm )
{
	Vector3 vel = vm->PopVector3();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->GetVelocityInWorld() = vel;
}

//============================================================================

void Script_ActorGetAngularVelocity( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	vm->Push( actor->GetAngVelocityInBody() );
}

//============================================================================

void Script_ActorGetFacingVector( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	vm->Push( actor->GetBodyToWorld().GetRow2() );
}

//============================================================================

void Script_ActorGetUpVector( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	vm->Push( actor->GetBodyToWorld().GetRow1() );
}

//============================================================================

void Script_ActorGetRotationX( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	DirCos3x3 &bodyToWorld = actor->GetBodyToWorld();
	vm->Push( Math::ArcSin( -bodyToWorld.e21 ) );
}

//============================================================================

void Script_ActorGetRotationY( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	DirCos3x3 &bodyToWorld = actor->GetBodyToWorld();
	vm->Push( Math::ArcTan2( bodyToWorld.e20, bodyToWorld.e22 ) );
}

//============================================================================

void Script_ActorGetRotationZ( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	DirCos3x3 &bodyToWorld = actor->GetBodyToWorld();
	vm->Push( Math::ArcTan2( bodyToWorld.e01, bodyToWorld.e11 ) );
}

//============================================================================

void Script_ActorMarkForDeath( ScriptVM* vm )
{
	float time = vm->PopFloat();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->MarkForDeath( time );
}

//============================================================================

void Script_ActorHide( ScriptVM* vm )
{
	bool hide = vm->PopBool();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	actor->Hide( hide );
}

//============================================================================

void Script_FindActorByInstanceName( ScriptVM* vm )
{
	const char* instanceName = vm->PopString();
	CActor* actor = g_stage.MasterCast().FindActorByInstanceName( instanceName );
	//this assert is bad, since there are things that don't spawn til later and things that see if there is one yet
	//ASSERTF( actor, ( "cannot find actor with instance name %s", instanceName ) );
	vm->Push( actor );
}

//============================================================================

void Script_ActorEnableInteraction( ScriptVM* vm )
{
	int		enable = vm->PopInt();
	CActor*	actor    = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->EnableInteraction( enable != 0 );
}

//============================================================================

void Script_ActorSetCategoryBits( ScriptVM* vm )
{
	int		category = vm->PopInt();
	CActor*	actor    = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->Category() |= category;
}

//============================================================================

void Script_ActorClearCategoryBits( ScriptVM* vm )
{
	int		category = vm->PopInt();
	CActor*	actor    = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->Category() &= ~category;
}

//============================================================================

void Script_ActorSetPositiveInclude( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->SetPositiveInclude( bits );
}

//============================================================================

void Script_ActorSetPositiveIncludeBits( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->PositiveInclude() |= bits;
}

//============================================================================

void Script_ActorClearPositiveIncludeBits( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->PositiveInclude() &= ~bits;
}

//============================================================================

void Script_ActorSetNegativeInclude( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->SetNegativeInclude( bits );
}

//============================================================================

void Script_ActorSetNegativeIncludeBits( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->NegativeInclude() |= bits;
}

//============================================================================

void Script_ActorClearNegativeIncludeBits( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->NegativeInclude() &= ~bits;
}

//============================================================================

void Script_ActorSetPositiveExclude( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->SetPositiveExclude( bits );
}

//============================================================================

void Script_ActorSetPositiveExcludeBits( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->PositiveExclude() |= bits;
}

//============================================================================

void Script_ActorClearPositiveExcludeBits( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->PositiveExclude() &= ~bits;
}

//============================================================================

void Script_ActorSetNegativeExclude( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->SetNegativeExclude( bits );
}

//============================================================================

void Script_ActorSetNegativeExcludeBits( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->NegativeExclude() |= bits;
}

//============================================================================

void Script_ActorClearNegativeExcludeBits( ScriptVM* vm )
{
	int	bits = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->NegativeExclude() &= ~bits;
}

//============================================================================

void Script_ActorLoadMotion( ScriptVM* vm )
{
	int motionIndex = vm->PopInt();
	const char* motionFile = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->LoadMotion( motionFile, motionIndex, 0 );
}

//============================================================================

void Script_ActorLoadAnimScript( ScriptVM* vm )
{
	int scriptIndex = vm->PopInt();
	const char* scriptFile = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->LoadAnimScript( scriptFile, scriptIndex );
}

//============================================================================

void Script_ActorSetAlpha( ScriptVM* vm )
{
	float alpha = vm->PopFloat();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	for ( int i = 0 ; i < MAX_ACTOR_INSTANCES ; i++ )
		if ( actor->Instance( i ) )
			actor->Instance( i )->SetAlpha( alpha );
}

//============================================================================

void Script_ActorSetScriptFloat( ScriptVM* vm )
{
	CActor *actor = vm->PopActor();
	int script = vm->PopInt();
	char *name = (char *)vm->PopString();
	float f = vm->PopFloat();
	actor->Script( script )->SetVar( name, f );
}

//============================================================================

void Script_ActorSetScriptInt( ScriptVM* vm )
{
	CActor *actor = vm->PopActor();
	int script = vm->PopInt();
	char *name = (char *)vm->PopString();
	int i = vm->PopInt();
	actor->Script( script )->SetVar( name, i );
}

//============================================================================

void Script_ActorGetScriptFloat( ScriptVM* vm )
{
	CActor *actor = vm->PopActor();
	int script = vm->PopInt();
	char *name = (char *)vm->PopString();
	float f = actor->Script( script )->GetFloat( name );
	vm->Push( f );
}

//============================================================================

void Script_ActorGetScriptInt( ScriptVM* vm )
{
	CActor *actor = vm->PopActor();
	int script = vm->PopInt();
	char *name = (char *)vm->PopString();
	int i = actor->Script( script )->GetInt( name );
	vm->Push( i );
}

//============================================================================

void Script_ActorSetGeometryFromWorldInstance( ScriptVM* vm )
{
	char* instanceName = vm->PopString();
	int geomIndex = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	actor->SetGeometryFromWorldInstance( geomIndex, instanceName );
	actor->InitializeGeometry( geomIndex );
}

//============================================================================

void Script_ActorSetGeometry( ScriptVM* vm )
{
	char* geomFile = vm->PopString();
	ASSERT(geomFile && *geomFile);
	int geomIndex = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	ts_Geometry* geom = g_loadSOM.Geometry( geomFile );
	ASSERT_PTR( geom );
	actor->SetGeometry( geomIndex, geom );
	actor->InitializeGeometry( geomIndex );
}

//============================================================================

void Script_ActorSetGeometryCullRadius( ScriptVM* vm )
{
	float radius = vm->PopFloat();
	int geomIndex = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	ASSERT_PTR( actor->Instance( geomIndex ) );
	actor->Instance( geomIndex )->SetCullRadius( radius );
}

//============================================================================

void Script_ActorHideGeometry( ScriptVM* vm )
{
	bool hideFlag = vm->PopBool();
	int geomIndex = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	if ( actor && actor->Instance( geomIndex ) )
		actor->Instance( geomIndex )->Hide( hideFlag );
}

//============================================================================

void Script_ActorGetNetMaster( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	vm->Push( actor->IsNetMaster() );
}

//============================================================================

void Script_ActorSetStateDistributed( ScriptVM* vm )
{
	bool stateDistributed = vm->PopBool();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->SetStateDistributed(stateDistributed);
}

//============================================================================

void Script_ActorRequestNetDataSlot( ScriptVM* vm )
{
	int maxBufferSize = vm->PopInt();
	const char* slotName = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	vm->Push( actor->RequestNetDataSlot( slotName, maxBufferSize ) );
}

//============================================================================

void Script_ActorWriteNetDataSlot( ScriptVM* vm )
{
	int bufferSize = vm->PopInt();
	void* buffer = vm->PopTypeless();
	const char* slotName = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	vm->Push( actor->WriteNetDataSlot( slotName, buffer, bufferSize ) );
}

//============================================================================

void Script_ActorSendNetData( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	actor->SendNetData();
}

//============================================================================

void Script_ActorOpenNetDataSlot( ScriptVM* vm )
{
	const char* slotName = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	vm->Push( (int) actor->OpenNetDataSlot( slotName ) );
}

//============================================================================

void Script_ActorReadNetDataSlot( ScriptVM* vm )
{
	const char* slotName = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	vm->Push( (int) actor->ReadNetDataSlot( slotName ) );
}

//============================================================================

void Script_ActorSetDiscontinuity( ScriptVM* vm )
{
	uint32 discontinuity = vm->PopInt();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );
	actor->SetDiscontinuity(discontinuity);
}

//============================================================================

void Script_ActorSetCollisionSurfaceName( ScriptVM* vm )
{
	const char* name = vm->PopString();
	ASSERT_PTR( name );
	ASSERT( *name );

	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	CCollisionIF* collisionIF = (CCollisionIF*)actor->GetInterface(
			CCollisionIF::GetID());
	if (collisionIF != NULL)
	{
		// Lets be really anal, and help out the scripts
		// which tend to be dumb...
		char dynamicname[32];
		strncpy(dynamicname, name, sizeof(dynamicname));
		dynamicname[sizeof(dynamicname) - 1] = '\0';

		int surfaceType = g_surfaceType.GetValidDynamicIndex(dynamicname, -1, sizeof(dynamicname));

		// This assert means that a non-dynamic material was used to mark
		// this ObbNode *or* the material does not exist in the database
		// In either case, this is something you should fix
		if (strcmpi(dynamicname, name) != 0)
		{
			strcpy(dynamicname, "nd");
			strcat(dynamicname, name);
			TRACE("Non-dynamic material assigned to actor %s, reassigned to %s\n", actor->Name(), dynamicname);
		}

		collisionIF->SetSurfaceName( dynamicname );
	}
}

//============================================================================

void Script_ActorGetCollisionXSize( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	COrientedCollisionIF* collisionIF = (COrientedCollisionIF*)
			actor->GetInterface(COrientedCollisionIF::GetID());
	if (collisionIF != NULL)
		vm->Push( collisionIF->GetXSize() );
	else
		vm->Push( 0.0f );
}

//============================================================================

void Script_ActorGetCollisionYSize( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	COrientedCollisionIF* collisionIF = (COrientedCollisionIF*)
			actor->GetInterface(COrientedCollisionIF::GetID());
	if (collisionIF != NULL)
		vm->Push( collisionIF->GetYSize() );
	else
		vm->Push( 0.0f );
}

//============================================================================

void Script_ActorGetCollisionZSize( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	COrientedCollisionIF* collisionIF = (COrientedCollisionIF*)
			actor->GetInterface(COrientedCollisionIF::GetID());
	if (collisionIF != NULL)
		vm->Push( collisionIF->GetZSize() );
	else
		vm->Push( 0.0f );
}

//============================================================================

void Script_ActorSetCollisionXSize( ScriptVM* vm )
{
	float size          = vm->PopFloat();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	COrientedCollisionIF* collisionIF = (COrientedCollisionIF*)
			actor->GetInterface(COrientedCollisionIF::GetID());
	if (collisionIF != NULL)
		collisionIF->SetXSize( size );
}

//============================================================================

void Script_ActorSetCollisionYSize( ScriptVM* vm )
{
	float size          = vm->PopFloat();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	COrientedCollisionIF* collisionIF = (COrientedCollisionIF*)
			actor->GetInterface(COrientedCollisionIF::GetID());
	if (collisionIF != NULL)
		collisionIF->SetYSize( size );
}

//============================================================================

void Script_ActorSetCollisionZSize( ScriptVM* vm )
{
	float size          = vm->PopFloat();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	COrientedCollisionIF* collisionIF = (COrientedCollisionIF*)
			actor->GetInterface(COrientedCollisionIF::GetID());
	if (collisionIF != NULL)
		collisionIF->SetZSize( size );
}

//============================================================================

void Script_ActorScaleCollisionXSize( ScriptVM* vm )
{
	float size          = vm->PopFloat();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	COrientedCollisionIF* collisionIF = (COrientedCollisionIF*)
			actor->GetInterface(COrientedCollisionIF::GetID());
	if (collisionIF != NULL)
		collisionIF->ScaleXSize( size );
}

//============================================================================

void Script_ActorScaleCollisionYSize( ScriptVM* vm )
{
	float size          = vm->PopFloat();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	COrientedCollisionIF* collisionIF = (COrientedCollisionIF*)
			actor->GetInterface(COrientedCollisionIF::GetID());
	if (collisionIF != NULL)
		collisionIF->ScaleYSize( size );
}

//============================================================================

void Script_ActorScaleCollisionZSize( ScriptVM* vm )
{
	float size          = vm->PopFloat();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	COrientedCollisionIF* collisionIF = (COrientedCollisionIF*)
			actor->GetInterface(COrientedCollisionIF::GetID());
	if (collisionIF != NULL)
		collisionIF->ScaleZSize( size );
}

//============================================================================

void Script_ActorCreate( ScriptVM* vm )
{
	const char* className = vm->PopString();
	ASSERT_PTR( className );
	ASSERT( className[0] );
	CActor* actor = CActorCreator::Create( className );
	ASSERT_PTR( actor );
	vm->Push( actor );
}

//============================================================================

void Script_ActorSetInstanceName( ScriptVM* vm )
{
	const char* name = vm->PopString();
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	ASSERT( actor->Instance( 0 ) );
	actor->Instance( 0 )->SetName( name );
}

//============================================================================

void Script_GetSurfaceIndex( ScriptVM* vm )
{
	const char *surfaceName = vm->PopString();
	ASSERT_PTR( surfaceName );
	vm->Push( g_surfaceType.GetIndex( surfaceName ) );
}

//============================================================================

static void Script_ActorFindFloor( ScriptVM* vm )
{
	CActor* actor	= (CActor*) vm->PopActor();
	ASSERT_PTR( actor );

	CLineEntity test;
	float scanHeight = 1.0f;
	Vector3 pos = actor->GetBodyInWorld();
	Vector3 start = pos;
	start.Y(start.Y() + scanHeight);
	const Vector3 down(0.0f, -1.0f, 0.0f);
	test.Line().SetOriginBasis(start, down);
	test.Line().SetMaxBoundary(3.0f);
	test.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	test.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	test.SetPositiveInclude(WORLD_CATEGORY);

	// Do the query
	g_collisionSystem.ResetStack();
	CLineQuery query;
	if (!query.Test(test))
	{
		vm->Push( pos.y() );						// return original y-pos
	}
	else
	{
		// Give them back the data
		vm->Push( query.intersection.Y() );
	}
}

//============================================================================

void Script_ActorValid( ScriptVM* vm )
{
	// Actor ptrs are stored as handles in script.  PopActor does the
	// handle to ptr translation and sets to NULL if it isn't valid.
	CActor* actor = vm->PopActor();
	int valid = (actor != NULL);
	vm->Push(valid);
}

//============================================================================

void Script_ActorGetInstancePtr( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	vm->Push( (int) actor->Instance(0) );
}

//============================================================================

void Script_ActorGetInstanceName( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->Instance(0) );
	vm->Push( (char*) actor->Instance(0)->Name() );
}

//============================================================================

void Script_ActorSetImpotent( ScriptVM* vm )
{
	int flag = vm->PopInt();
	CProtagonist* actor = (CProtagonist*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CProtagonist::ClassType() ) );
	if ( actor )
	{
		if ( flag )
			actor->SetInteractionHint( actor->GetInteractionHint() | CProtagonist::HINT_IMPOTENT );
		else
			actor->SetInteractionHint( actor->GetInteractionHint() & ~CProtagonist::HINT_IMPOTENT );
	}
}

//============================================================================

void Script_ActorSetJuggernaut( ScriptVM* vm )
{
	int flag = vm->PopInt();
	CProtagonist* actor = (CProtagonist*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CProtagonist::ClassType() ) );
	if ( actor )
	{
		if ( flag )
			actor->SetInteractionHint( actor->GetInteractionHint() | CProtagonist::HINT_JUGGERNAUT );
		else
			actor->SetInteractionHint( actor->GetInteractionHint() & ~CProtagonist::HINT_JUGGERNAUT );
	}
}

//============================================================================

void Stage_RegisterScriptFuncs()
{
	SCRIPT_REGISTER( SetIgnoreEntity );
	SCRIPT_REGISTER( ActorDeactivatePathFollower );
	SCRIPT_REGISTER( ActorDeactivatePathFollowerCam );
	SCRIPT_REGISTER( ActorActivatePathFollower );
	SCRIPT_REGISTER( ActorActivatePathFollowerCam );
	SCRIPT_REGISTER( ActorGetPosition );
	SCRIPT_REGISTER( ActorMatchPositionAndOrientation );
	SCRIPT_REGISTER( ActorGetVelocity );
	SCRIPT_REGISTER( ActorSetVelocity );
	SCRIPT_REGISTER( ActorGetAngularVelocity );
	SCRIPT_REGISTER( ActorGetFacingVector );
	SCRIPT_REGISTER( ActorGetUpVector );
	SCRIPT_REGISTER( ActorGetRotationX );
	SCRIPT_REGISTER( ActorGetRotationY );
	SCRIPT_REGISTER( ActorGetRotationZ );
	SCRIPT_REGISTER( ActorMarkForDeath );
	SCRIPT_REGISTER( ActorHide );
	SCRIPT_REGISTER( FindActorByInstanceName );
	SCRIPT_REGISTER( ActorEnableInteraction );
	SCRIPT_REGISTER( ActorSetCategoryBits );
	SCRIPT_REGISTER( ActorClearCategoryBits );
	SCRIPT_REGISTER( ActorSetPositiveInclude );
	SCRIPT_REGISTER( ActorSetPositiveIncludeBits );
	SCRIPT_REGISTER( ActorClearPositiveIncludeBits );
	SCRIPT_REGISTER( ActorSetNegativeInclude );
	SCRIPT_REGISTER( ActorSetNegativeIncludeBits );
	SCRIPT_REGISTER( ActorClearNegativeIncludeBits );
	SCRIPT_REGISTER( ActorSetPositiveExclude );
	SCRIPT_REGISTER( ActorSetPositiveExcludeBits );
	SCRIPT_REGISTER( ActorClearPositiveExcludeBits );
	SCRIPT_REGISTER( ActorSetNegativeExclude );
	SCRIPT_REGISTER( ActorSetNegativeExcludeBits );
	SCRIPT_REGISTER( ActorClearNegativeExcludeBits );
	SCRIPT_REGISTER( ActorLoadMotion );
	SCRIPT_REGISTER( ActorLoadAnimScript );
	SCRIPT_REGISTER( ActorSetAlpha );
	SCRIPT_REGISTER( ActorSetScriptFloat );
	SCRIPT_REGISTER( ActorSetScriptInt );
	SCRIPT_REGISTER( ActorGetScriptFloat );
	SCRIPT_REGISTER( ActorGetScriptInt );
	SCRIPT_REGISTER( ActorSetGeometryFromWorldInstance );
	SCRIPT_REGISTER( ActorSetGeometry );
	SCRIPT_REGISTER( ActorSetGeometryCullRadius );
	SCRIPT_REGISTER( ActorHideGeometry );
	SCRIPT_REGISTER( ActorGetNetMaster );
	SCRIPT_REGISTER( ActorSetStateDistributed );
	SCRIPT_REGISTER( ActorRequestNetDataSlot );
	SCRIPT_REGISTER( ActorWriteNetDataSlot );
	SCRIPT_REGISTER( ActorSendNetData );
	SCRIPT_REGISTER( ActorOpenNetDataSlot );
	SCRIPT_REGISTER( ActorReadNetDataSlot );
	SCRIPT_REGISTER( ActorSetDiscontinuity );
	SCRIPT_REGISTER( ActorSetCollisionSurfaceName );
	SCRIPT_REGISTER( ActorGetCollisionXSize );
	SCRIPT_REGISTER( ActorGetCollisionYSize );
	SCRIPT_REGISTER( ActorGetCollisionZSize );
	SCRIPT_REGISTER( ActorSetCollisionXSize );
	SCRIPT_REGISTER( ActorSetCollisionYSize );
	SCRIPT_REGISTER( ActorSetCollisionZSize );
	SCRIPT_REGISTER( ActorScaleCollisionXSize );
	SCRIPT_REGISTER( ActorScaleCollisionYSize );
	SCRIPT_REGISTER( ActorScaleCollisionZSize );
	SCRIPT_REGISTER( ActorCreate );
	SCRIPT_REGISTER( ActorSetInstanceName );
	SCRIPT_REGISTER( GetSurfaceIndex );
	SCRIPT_REGISTER( ActorFindFloor );
	SCRIPT_REGISTER( ActorGetInstancePtr );
	SCRIPT_REGISTER( ActorGetInstanceName );
	SCRIPT_REGISTER( ActorSetImpotent );
	SCRIPT_REGISTER( ActorSetJuggernaut );
	SCRIPT_REGISTER( ActorValid );
}

//============================================================================
