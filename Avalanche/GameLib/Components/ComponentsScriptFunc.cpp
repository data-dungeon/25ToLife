//============================================================================
//=
//= ComponentsScriptFunc.cpp - 
//=    Script Interface routines to module "component"
//=
//============================================================================

#include "Components/ComponentsPCH.h"

#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"

#include "Components/weapons/weapinv.h"
#include "Components/weapons/weapon.h"
#include "Components/ActorList.h"

#include "camera/CameraPCH.h"
#include "Components/PathFollowerCam.h"

//============================================================================

static void Script_InitializeComponents( ScriptVM* vm )
{
	CCompActor* actor   = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );
	actor->InitializeComponents();
}

//============================================================================

static void Script_AttachComponent( ScriptVM* vm )
{
	const char* name = vm->PopString();
	ASSERT_PTR( name );
	ASSERT( *name );

	CCompActor* actor   = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	actor->AttachComponent( name );
}

//============================================================================

static void Script_HasComponent( ScriptVM* vm )
{
	int hasComp = 0;
	const char* name = vm->PopString();

	CCompActor* actor   = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	if( actor->IsA( CCompActor::ClassType() ) )
	{
		CActorComponent* component = actor->GetComponentByName( name );
		if( component )
			hasComp = 1;
	}
	vm->Push( hasComp );
}

//============================================================================

static void Script_IsComponentActive( ScriptVM* vm )
{
	const char* name = vm->PopString();

	CCompActor* actor   = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CActorComponent* component = actor->GetComponentByName( name );
	if( component )
		vm->Push( (int)(component->IsActive()) );
	else
		vm->Push( 0 );
}

//============================================================================

static void Script_ActivateComponent( ScriptVM* vm )
{
	const char* name = vm->PopString();

	CCompActor* actor   = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CActorComponent* component = actor->GetComponentByName( name );
	if( component )
		component->Activate();
}

//============================================================================

static void Script_DeactivateComponent( ScriptVM* vm )
{
	const char* name = vm->PopString();

	CCompActor* actor   = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CActorComponent* component = actor->GetComponentByName( name );
	if( component )
		component->Deactivate();
}

static void Script_ActivateComponentInClass( ScriptVM* vm )
{
	const char* name = vm->PopString();

	CCompActor* actor   = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CActorComponent* component = actor->GetDefaultComponentInClass( name );
	if( component )
		component->Activate();
}

static void Script_DeactivateComponentInClass( ScriptVM* vm )
{
	const char* name = vm->PopString();

	CCompActor* actor   = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CActorComponent* component = actor->GetActiveComponentInClass( name );
	if( component )
		component->Deactivate();
}


//============================================================================

static void Script_ShowCurrentWeapon( ScriptVM* vm )
{
	CCompActor* actor   = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	if( inv )
	{
		Weapon *weapon = inv->GetSelected();
		if( weapon )
			weapon->Show();
	}
}

//============================================================================

static void Script_HideCurrentWeapon( ScriptVM* vm )
{
	CCompActor* actor   = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	if( inv )
	{
		Weapon *weapon = inv->GetSelected();
		if( weapon )
			weapon->Hide();
	}
}

//============================================================================

static void Script_ActorList_Count( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	int count = 0;

	CActorList* actorList = (CActorList*) me->GetComponentByName( "ActorList" );
	ASSERT_PTR( actorList );
	if ( actorList )
		count = actorList->Count();

	vm->Push( count );
}

//============================================================================

static void Script_ActorList_Add( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();

	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	bool success = false;

	CActorList* actorList = (CActorList*) me->GetComponentByName( "ActorList" );
	ASSERT_PTR( actorList );
	if ( actorList )
		success = actorList->Add( actor );

	vm->Push( success );
}

//============================================================================

static void Script_ActorList_Remove( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();

	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	bool success = false;

	CActorList* actorList = (CActorList*) me->GetComponentByName( "ActorList" );
	ASSERT_PTR( actorList );
	if ( actorList )
		success = actorList->Remove( actor );

	vm->Push( success );
}

//============================================================================

static void Script_ActorList_Contains( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();

	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	bool success = false;

	CActorList* actorList = (CActorList*) me->GetComponentByName( "ActorList" );
	ASSERT_PTR( actorList );
	if ( actorList )
		success = actorList->Contains( actor );

	vm->Push( success );
}

//============================================================================

static void Script_ActorList_Get( ScriptVM* vm )
{
	int index = vm->PopInt();

	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	CActor* actor = NULL;

	CActorList* actorList = (CActorList*) me->GetComponentByName( "ActorList" );
	ASSERT_PTR( actorList );
	if ( actorList )
		actor = actorList->Get( index );

	vm->Push( actor);
}

//============================================================================

static void Script_ActorList_Clear( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	CActorList* actorList = (CActorList*) me->GetComponentByName( "ActorList" );
	ASSERT_PTR( actorList );
	if ( actorList )
		actorList->Clear();
}

//============================================================================

static void Script_GetPropertyFloat( ScriptVM* vm )
{
	const char *name = vm->PopString();
	CCompActor* me = (CCompActor*) vm->PopActor();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	float f = 0.0f;
	Property *prop = me->QueryProperty( name );
	if( prop )
		f = prop->GetFloat();

	vm->Push( f );
}

//============================================================================

static void Script_GetPropertyInt( ScriptVM* vm )
{
	const char *name = vm->PopString();
	CCompActor* me = (CCompActor*) vm->PopActor();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	int i = 0;
	Property *prop = me->QueryProperty( name );
	if( prop )
		i = prop->GetInt();

	vm->Push( i );
}

//============================================================================

static void Script_GetPropertyPointer( ScriptVM* vm )
{
	const char *name = vm->PopString();
	CCompActor* me = (CCompActor*) vm->PopActor();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	void *p = NULL;
	Property *prop = me->QueryProperty( name );
	if( prop )
		p = prop->GetPointer();

	vm->Push( (int)p );
}

//============================================================================

static void Script_GetPropertyHandle( ScriptVM* vm )
{
	const char *name = vm->PopString();
	CCompActor* me = (CCompActor*) vm->PopActor();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	int i = 0;
	Property *prop = me->QueryProperty( name );
	if( prop )
		i = prop->GetHandle();

	vm->Push( i );
}

//============================================================================

static void Script_GetPropertyVector( ScriptVM* vm )
{
	const char *name = vm->PopString();
	CCompActor* me = (CCompActor*) vm->PopActor();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	Vector3 v( 0.0f, 0.0f, 0.0f );
	Property *prop = me->QueryProperty( name );
	if( prop )
		v = prop->GetVector3();

	vm->Push( v );
}

//============================================================================

static void Script_SetPropertyFloat( ScriptVM* vm )
{
	float f = vm->PopFloat();
	const char *name = vm->PopString();
	CCompActor* me = (CCompActor*) vm->PopActor();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	Property *prop = me->QueryProperty( name );
	if( !prop )
		me->AddProperty( name, Property_float );
	prop = me->QueryProperty( name );
	prop->Set( f );
}

//============================================================================

static void Script_SetPropertyInt( ScriptVM* vm )
{
	int i = vm->PopInt();
	const char *name = vm->PopString();
	CCompActor* me = (CCompActor*) vm->PopActor();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	Property *prop = me->QueryProperty( name );
	if( !prop )
		me->AddProperty( name, Property_int );
	prop = me->QueryProperty( name );
	prop->Set( i );
}

//============================================================================

static void Script_SetPropertyPointer( ScriptVM* vm )
{
	void *p = (void *)(vm->PopInt());
	const char *name = vm->PopString();
	CCompActor* me = (CCompActor*) vm->PopActor();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	Property *prop = me->QueryProperty( name );
	if( !prop )
		me->AddProperty( name, Property_pointer );
	prop = me->QueryProperty( name );
	prop->Set( p );
}

//============================================================================

static void Script_SetPropertyVector( ScriptVM* vm )
{
	Vector3 v = vm->PopVector3();
	const char *name = vm->PopString();
	CCompActor* me = (CCompActor*) vm->PopActor();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	Property *prop = me->QueryProperty( name );
	if( !prop )
		me->AddProperty( name, Property_Vector3 );
	prop = me->QueryProperty( name );
	prop->Set( v );
}

//============================================================================

static void Script_SetPropertyHandle( ScriptVM* vm )
{
	uint32 i = (uint32)vm->PopInt();
	const char *name = vm->PopString();
	CCompActor* me = (CCompActor*) vm->PopActor();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	Property *prop = me->QueryProperty( name );
	if( !prop )
		me->AddProperty( name, Property_handle );
	prop = me->QueryProperty( name );
	prop->Set( i );
}

void Script_SetPathCamTarget( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	CActor *actor = vm->PopActor();
	pfc->SetTarget( actor );
}

void Script_SetPathCamTargetPoint( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	Vector3 v = vm->PopVector3();
	pfc->SetTargetPoint( &v );

}

void Script_SetPathCamSpeed( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetSpeed( vm->PopFloat() );
}

void Script_SetPathCamTargetSpeed( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetTargetSpeed( vm->PopFloat() );
}

void Script_SetPathCamEndDelay( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetEndDelay( vm->PopFloat() );
}

void Script_SetPathCamAccel( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetAccel( vm->PopFloat() );
}

void Script_SetPathCamDecel( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetDecel( vm->PopFloat() );
}

void Script_SetPathCamTimeActive( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetTimeActive( vm->PopFloat() );
}

void Script_SetPathTextMessage( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetTextMessage( vm->PopString() );
}

void Script_SetPathCamTargetingBehavior( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetTargetingBehavior( vm->PopInt() );
}

void Script_SetCameraType( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetCameraType( vm->PopInt() );
}


void Script_SetPathCamFadeIn( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetFadeIn( vm->PopInt() != 0 );
}

void Script_SetPathCamFadeOut( ScriptVM* vm )
{
	CCompActor* me = (CCompActor*) vm->GetOwner();
	ASSERT_PTR( me );
	ASSERT( me->IsA( CCompActor::ClassType() ) );

	// get component
	CPathFollowerCam *pfc= (CPathFollowerCam *)me->GetComponentByName( "PathFollowerCam" );
	ASSERT_PTR( pfc );

	 pfc->SetFadeOut( vm->PopInt() != 0 );
}

//============================================================================

void Components_RegisterScriptFuncs()
{
	SCRIPT_REGISTER( AttachComponent );
	SCRIPT_REGISTER( InitializeComponents );
	SCRIPT_REGISTER( ActivateComponent );
	SCRIPT_REGISTER( HasComponent );
	SCRIPT_REGISTER( IsComponentActive );
	SCRIPT_REGISTER( DeactivateComponent );
	SCRIPT_REGISTER( ActivateComponentInClass );
	SCRIPT_REGISTER( DeactivateComponentInClass );
	SCRIPT_REGISTER( ShowCurrentWeapon );
	SCRIPT_REGISTER( HideCurrentWeapon );
	SCRIPT_REGISTER( ActorList_Count );
	SCRIPT_REGISTER( ActorList_Add );
	SCRIPT_REGISTER( ActorList_Remove );
	SCRIPT_REGISTER( ActorList_Contains );
	SCRIPT_REGISTER( ActorList_Get );
	SCRIPT_REGISTER( ActorList_Clear );
	SCRIPT_REGISTER( GetPropertyFloat );
	SCRIPT_REGISTER( GetPropertyInt );
	SCRIPT_REGISTER( GetPropertyPointer );
	SCRIPT_REGISTER( GetPropertyVector );
	SCRIPT_REGISTER( GetPropertyHandle );
	SCRIPT_REGISTER( SetPropertyFloat );
	SCRIPT_REGISTER( SetPropertyInt );
	SCRIPT_REGISTER( SetPropertyPointer );
	SCRIPT_REGISTER( SetPropertyVector );
	SCRIPT_REGISTER( SetPropertyHandle );
	SCRIPT_REGISTER( SetPathCamTargetingBehavior );
	SCRIPT_REGISTER( SetPathCamFadeOut );
	SCRIPT_REGISTER( SetPathCamFadeIn );
	SCRIPT_REGISTER( SetCameraType );
	SCRIPT_REGISTER( SetPathCamTargetSpeed );
	SCRIPT_REGISTER( SetPathCamSpeed );
	SCRIPT_REGISTER( SetPathCamEndDelay );
	SCRIPT_REGISTER( SetPathCamAccel );
	SCRIPT_REGISTER( SetPathCamDecel );
	SCRIPT_REGISTER( SetPathCamTimeActive );
	SCRIPT_REGISTER( SetPathTextMessage );
	SCRIPT_REGISTER( SetPathCamTargetPoint );
	SCRIPT_REGISTER( SetPathCamTarget );

}

//============================================================================
