//============================================================================
//=
//= GameHelperScriptFunc.cpp - 
//=    Script Interface routines to module "game"
//=
//============================================================================

#include "GameHelper/GameHelperPCH.h"
#include "GameHelper/SharedBody.h"
#include "GameHelper/GameLocks.h"
#include "math/fastrand.h"

#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"

//============================================================================

static void Script_GetRandomInt( ScriptVM* vm )
{
	int maxValue = vm->PopInt();
	int minValue = vm->PopInt();
	int result   = g_random.InRange( minValue, maxValue );
	vm->Push( result );
}

//============================================================================

static void Script_GetRandomReal( ScriptVM* vm )
{
	float maxValue = vm->PopFloat();
	float minValue = vm->PopFloat();
	float result   = g_random.InRange( minValue, maxValue );
	vm->Push( result );
}

//============================================================================

static void Script_GetRandomRealExc( ScriptVM* vm )
{
	float maxValue = vm->PopFloat();
	float minValue = vm->PopFloat();
	float result   = g_random.InRangeExcludeHigh( minValue, maxValue );
	vm->Push( result );
}

//============================================================================

static void Script_SendMessageToActor( ScriptVM* vm )
{
	ActorHandle	receiver	= vm->PopActorHandle();
	void*			data		= vm->PopTypeless();
	const char*	message  = vm->PopString();

	if ( message && message[0] )
	{
		ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( vm->GetScriptFileNameOfPC(), vm->GetScriptLineNumberOfPC() );
		GetSingleton(CMessageDispatcher).SendMessageToActor( message, data, CActor::ToHandle((CActor*) vm->GetOwner()), receiver );
	}
}

//============================================================================

static void Script_SendMessageVector3ToActor( ScriptVM* vm )
{
	ActorHandle receiver = vm->PopActorHandle();
	Vector3     vec      = vm->PopVector3();
	const char* message  = vm->PopString();

	if ( message && message[0] )
	{
		ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( vm->GetScriptFileNameOfPC(), vm->GetScriptLineNumberOfPC() );
		GetSingleton(CMessageDispatcher).SendMessageToActor( message, &vec, CActor::ToHandle((CActor*) vm->GetOwner()), receiver );
	}
}

//============================================================================

static void Script_SendMessageToAll( ScriptVM* vm )
{
	void*       data    = vm->PopTypeless();
	const char*	message = vm->PopString();
	if ( message && message[0] )
	{
		ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( vm->GetScriptFileNameOfPC(), vm->GetScriptLineNumberOfPC() );
		GetSingleton(CMessageDispatcher).SendMessageToAll( message, data, CActor::ToHandle((CActor*) vm->GetOwner()) );
	}
}

//============================================================================

static void Script_SendMessageToAllFromActor( ScriptVM* vm )
{
	ActorHandle sender  = vm->PopActorHandle();
	void*       data    = vm->PopTypeless();
	const char*	message = vm->PopString();
	if ( message && message[0] )
	{
		ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( vm->GetScriptFileNameOfPC(), vm->GetScriptLineNumberOfPC() );
		GetSingleton(CMessageDispatcher).SendMessageToAll( message, data, sender );
	}
}

//============================================================================

static void Script_SendStringMessageToSession( ScriptVM* vm )
{
	void*       data    = vm->PopTypeless();
	const char*	message = vm->PopString();
	if ( message && message[0] )
	{
		ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( vm->GetScriptFileNameOfPC(), vm->GetScriptLineNumberOfPC() );
		GetSingleton(CMessageDispatcher).SendMessageToSession( message, data, INVALID_OBJECT_HANDLE );
	}
}

//============================================================================

static void Script_SetMessageDataTranslator( ScriptVM* vm )
{
	int type            = vm->PopInt();
	const char* message = vm->PopString();
	if ( message && message[0] )
	{
		ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( vm->GetScriptFileNameOfPC(), vm->GetScriptLineNumberOfPC() );
		GetSingleton(CMessageDispatcher).InstallBasicDataTypeTranslator( message, (EMessageBasicDataType) type );
	}
}

//============================================================================

static void Script_FindInstanceByName( ScriptVM* vm )
{
	const char* instanceName = vm->PopString();
	CInstance* instance = NULL;
	if ( instanceName && instanceName[ 0 ] && stricmp( instanceName, "none" ) != 0 )
	{
		instance = g_loadSOM.FindInstanceByName( instanceName );
		ASSERT(instance);
	}
	vm->Push( (int)instance );
}

//============================================================================

static void Script_GetVector3( ScriptVM* vm )
{
	Vector3Packed *v = (Vector3Packed *) vm->PopInt();
	ASSERT_PTR( v );
	vm->Push( *v );
}

//============================================================================

static void Script_GetNamedPoint( ScriptVM* vm )
{
	const char* pointName = vm->PopString();
	ASSERT_PTR( pointName );
	ASSERT( *pointName );

	ts_NamedPosition* point = g_loadSOM.FindNamedPoint( pointName );
	if ( point )
	{
		vm->Push( point->vPos );
	}
	else
	{
		ASSERTF( false, ( "named point '%s' not found", pointName ) );
		vm->Push( Vector3( -999999.0f, -999999.0f, -999999.0f ) );
	}
}

//============================================================================
// Do these really belong here?
//============================================================================

static void Script_EnableCollisionLayer( ScriptVM* vm )
{
	int layer = vm->PopInt();
	ASSERT( layer != 0 );

	g_collisionSystem.EnableLayer(layer);
}

//============================================================================

static void Script_DisableCollisionLayer( ScriptVM* vm )
{
	int layer = vm->PopInt();
	ASSERT( layer != 0 );

	g_collisionSystem.DisableLayer(layer);
}

//============================================================================

static void Script_SharedBody_Hidden( ScriptVM* vm )
{
	const char* slot = vm->PopString();

	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	ASSERT_PTR( actor->Instance( 0 ) );
	SharedBody* body = (SharedBody*) actor->Instance( 0 )->DynamicModel();
	ASSERT_PTR( body );

	vm->Push( body->Hidden( slot ) );
}

//============================================================================

static void Script_SharedBody_Hide( ScriptVM* vm )
{
	bool flag = vm->PopBool();
	const char* slot = vm->PopString();

	CActor* actor = (CActor*) vm->GetOwner();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	ASSERT_PTR( actor->Instance( 0 ) );
	SharedBody* body = (SharedBody*) actor->Instance( 0 )->DynamicModel();
	ASSERT_PTR( body );

	body->Hide( slot, flag );
}

//============================================================================

static void Script_GameLocks_Unlock( ScriptVM* vm )
{
	const char* key = vm->PopString();
	if ( key && key[0] )
	{
		GameLocks::Unlock( key );
	}
}

//============================================================================

static void Script_GameLocks_IsUnlocked( ScriptVM* vm )
{
	const char* key = vm->PopString();
	vm->Push( key && key[0] && GameLocks::IsUnlocked( key ) );
}

//============================================================================

void GameHelper_RegisterScriptFuncs()
{
	SCRIPT_REGISTER( GetRandomInt        );
	SCRIPT_REGISTER( GetRandomReal       );
	SCRIPT_REGISTER( GetRandomRealExc    );
	SCRIPT_REGISTER( SendMessageToActor  );
	SCRIPT_REGISTER( SendMessageVector3ToActor );
	SCRIPT_REGISTER( SendMessageToAll    );
	SCRIPT_REGISTER( SendMessageToAllFromActor );
	SCRIPT_REGISTER( SendStringMessageToSession );
	SCRIPT_REGISTER( SetMessageDataTranslator );
	SCRIPT_REGISTER( FindInstanceByName  );
	SCRIPT_REGISTER( GetVector3 );
	SCRIPT_REGISTER( GetNamedPoint );
	SCRIPT_REGISTER( EnableCollisionLayer );
	SCRIPT_REGISTER( DisableCollisionLayer );
	SCRIPT_REGISTER( SharedBody_Hidden );
	SCRIPT_REGISTER( SharedBody_Hide );
	SCRIPT_REGISTER( GameLocks_Unlock );
	SCRIPT_REGISTER( GameLocks_IsUnlocked );
}

//============================================================================
