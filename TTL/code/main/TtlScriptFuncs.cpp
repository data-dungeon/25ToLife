#include "TTLPCH.h"

#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"
#include "components/KineControl/BipedAnimScriptIF.h"
#include "Environ/Munition.h"
#include "Components/CollisionInfo.h"
#include "components/Ai/AiBehavior.h"
#include "components/Pickup.h"
#include "components/TagFrame.h"
#include "camera/director.h"
#include "weapons/ttlmunition.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "InterfacesTTL/HealthIF.h"
#include "Game/Misc/ParameterSet.h"

#include "Components/weapons/projlib.h"
#include "components/KineControl/BipedKineControl.h"
#include "Components/InteractionMonitor.h"
#include "Components/Component.h"
#include "components/painstats.h"
#include "components/affliction.h"
#include "components/TriggerBox.h"
#include "Interfaces/ViewIF.h"
#include "Interfaces/Interface.h"
#include "stage/protagonist.h"
#include "stage/CCompActor.h"
#include "Audio/audiosys.h"
#include "Components/KineControl/WallMoves.h"

#include "environ/ActorCreator.h"
#include "environ/ConfigMgr.h"
#include "environ/WorldActorLoader.h"

// cutscene interface includes
#include "cutscene/CutSceneMgr.h"
// Checkpoint interface includes
#include "sim/Referee.h"
#include "sim/TeamingModel.h"
// Weapons pickup interface includes
#include "Components/weapons/weaplib.h"
#include "Components/weapons/weapinv.h"
#include "weapons/armory.h"
#include "weapons/ttlweapinv.h"
#include "components/affliction.h"
#include "weapons/carbomb.h"
#include "weapons/freeproj.h"

#include "Effects/shatter.h"
#include "main/PlayerDB.h"
#include "environ/PlayerSpawnMgr.h"
#include "environ/ObjectiveMgr.h"
#include "InterfacesTTL/ObjectiveIF.h"
#include "Network/Session.h"
#include "avatar/pstat.h"
#include "camera/shootercam.h"
#include "main/TTLLoop.h"
#include "main/TTL.h"
#include "main/CharacterLoader.h"
#include "effects/blood.h"
#include "main/GameEnv.h"

#ifdef DIRECTX_PC
#include "frontend/savegame.h"
#include "frontend/frontend.h"
#endif


//============================================================================

static void Script_GetWorldToScreen( ScriptVM* vm )
{
	Vector3 worldPos = vm->PopVector3();
	int sceneNum = vm->PopInt();

	CameraScene* cameraScene = g_cameraDirector.GetDefaultScene();
	ASSERT_PTR(cameraScene);
	CScene* scene = cameraScene->Scene();
	ASSERT_PTR( scene );
	Vector2 screenPos;
	scene->GetScreenCoords( worldPos, screenPos );

	vm->Push( screenPos.x() );
	vm->Push( screenPos.y() );
}

//============================================================================

static void Script_GetWeaponClass( ScriptVM* vm )
{
	int weaponID = vm->PopInt();

	TTLWeaponProperties *prop = (TTLWeaponProperties *)g_weaponLib.GetProperties( weaponID );
	int weaponClass = WEAPONCLASS_NONE;
	if( prop )
		weaponClass = prop->m_weaponClass;
	vm->Push( weaponClass );
}

//============================================================================

static void Script_GetProjectileClass( ScriptVM* vm )
{
	int munitionType = vm->PopInt();
	if( munitionType == -1 )
		vm->Push( "" );
	else
		vm->Push( (char *)g_projectileLib.GetProperties( munitionType )->GetClass() );
}

//============================================================================

static void Script_GetMunitionImpactType( ScriptVM* vm )
{
	CMunitionImpact* hit = (CMunitionImpact*) vm->PopInt();
	ASSERT_PTR( hit );
	vm->Push( hit->d_munitionID );
}

//============================================================================

static void Script_GetMunitionImpactDamage( ScriptVM* vm )
{
	CMunitionImpact* hit = (CMunitionImpact*) vm->PopInt();
	ASSERT_PTR( hit );
	vm->Push( hit->d_damage );
}

//============================================================================

static void Script_GetMunitionImpactShooter( ScriptVM* vm )
{
	CMunitionImpact* hit = (CMunitionImpact*) vm->PopInt();
	ASSERT_PTR( hit );
//	vm->Push( hit->d_attacker );
	vm->Push( 0 );
}

//============================================================================

static void Script_GetMunitionImpactIntersection( ScriptVM* vm )
{
	CMunitionImpact* hit = (CMunitionImpact*) vm->PopInt();
	ASSERT_PTR( hit );
	vm->Push( hit->d_intersection );
}

//============================================================================

static void Script_GetMunitionMaterial( ScriptVM* vm )
{
	CMunitionImpact* hit = (CMunitionImpact*) vm->PopInt();
	ASSERT_PTR( hit );

	TTLProjectileProperties *projectile = (TTLProjectileProperties *)g_projectileLib.GetProperties(hit->d_munitionID);
	ASSERT(projectile);

	vm->Push( projectile->m_material );
}
//============================================================================

static void Script_GetMunitionImpactMaterial( ScriptVM* vm )
{
	CMunitionImpact* hit = (CMunitionImpact*) vm->PopInt();
	ASSERT_PTR( hit );
	vm->Push( hit->d_surfaceMaterial );
}

//============================================================================

static void Script_GetMunitionFromDir( ScriptVM* vm )
{
	CMunitionImpact* hit = (CMunitionImpact*) vm->PopInt();
	ASSERT_PTR( hit );
	Vector3 fromDir = hit->Direction();
	vm->Push( fromDir );
}

//============================================================================

static void Script_GetMunitionFired_WeaponID( ScriptVM* vm )
{
	CMunitionFired* fired = (CMunitionFired*) vm->PopInt();
	ASSERT_PTR( fired );
	
	int weaponID;
	if ( !fired || !fired->d_firstShot )
		weaponID = -1;
	else
		weaponID = fired->d_weaponID;
	vm->Push( weaponID );
}

//============================================================================

static void Script_GetMunitionFired_WeaponHand( ScriptVM* vm )
{
	CMunitionFired* fired = (CMunitionFired*) vm->PopInt();
	ASSERT_PTR( fired );

	int weapon = 0;
	WeaponInventory *inv = (WeaponInventory *)(((CCompActor *)vm->GetOwner())->GetComponentByName( "WeaponInventory" ));
	if( inv && inv->GetSelected() )
	{
		weapon = inv->GetSelected()->GetActiveModel();
	}
	vm->Push( weapon );
}

//============================================================================

static void Script_CarBomb( ScriptVM* vm )
{
	ASSERT_PTR(vm);

	Vector3 pos = vm->PopVector3();
	CActor *instigator = vm->PopActor();
	g_carBomb.Explode(instigator, (CActor *)vm->GetOwner(),  pos);
}

//============================================================================

static void Script_ExplodeProjectile( ScriptVM* vm )
{
	ASSERT_PTR(vm);

	Vector3 pos = vm->PopVector3();
	CActor *instigator = vm->PopActor();
	const char *projectileName = vm->PopString();

	bool ret = g_freeProjectile.Explode( projectileName, instigator, (CActor *)vm->GetOwner(), pos );
	vm->Push(ret);
}

//============================================================================

static void Script_FireProjectile( ScriptVM* vm )
{
	ASSERT_PTR(vm);

	Vector3 dir = vm->PopVector3();
	Vector3 pos = vm->PopVector3();
	CActor *shooter = vm->PopActor();
	const char *projectileName = vm->PopString();

	bool ret = g_freeProjectile.Fire( projectileName, shooter, pos, dir );
	vm->Push(ret);
}

//============================================================================
static void Script_FireProjectileAt( ScriptVM* vm )
{
	ASSERT_PTR(vm);

	Vector3 pos = vm->PopVector3();
	CActor *victim = vm->PopActor();
	CActor *shooter = vm->PopActor();
	const char *projectileName = vm->PopString();

	bool ret = g_freeProjectile.Fire( projectileName, shooter, victim, pos );
	vm->Push(ret);
}

//============================================================================

static void Script_Kill( ScriptVM* vm )
{
	CCompActor*	actor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	PlayerStats *stats = actor ? (PlayerStats*)actor->GetInterface(PlayerStats::GetID()) : NULL;
	if (stats)
		stats->Kill();
}

//============================================================================

static void Script_GetCollisionImpactIntersection( ScriptVM* vm )
{
	SCollisionInfo* hit = (SCollisionInfo*) vm->PopInt();
	ASSERT_PTR( hit );
	vm->Push( hit->d_intersection );
}

//============================================================================

static void Script_GetCollisionImpactNormal( ScriptVM* vm )
{
	SCollisionInfo* hit = (SCollisionInfo*) vm->PopInt();
	ASSERT_PTR( hit );
	vm->Push( hit->d_normal );
}

//============================================================================

static void Script_GetCollisionSender( ScriptVM* vm )
{
	SCollisionInfo* hit = (SCollisionInfo*) vm->PopInt();
	ASSERT_PTR( hit );
	vm->Push( hit->d_sender );
}

//============================================================================

static void Script_Shatter( ScriptVM* vm )
{
	ASSERT_PTR(vm);

	int colorR = vm->PopInt();
	int colorG = vm->PopInt();
	int colorB = vm->PopInt();
	int colorA = vm->PopInt();

	const char *shatter = vm->PopString();
	CProtagonist *who = (CProtagonist *)vm->PopActor();

	if (!who || !shatter || shatter[0] == '\0')
	{
		vm->Push( (int)false );
		return;
	}

	ShatterObject *object;
	if (colorR == 0 && colorG == 0 && colorB == 0 && colorA == 0)
		object = g_shatter.Create( shatter );
	else
	{
		ts_bRGBA color = { colorR, colorG, colorB, colorA };
		object = g_shatter.Create( shatter, &color );
	}

	if (!object)
		vm->Push( (int)false );
	else
	{
		object->Start( *who );
		vm->Push( (int)true );
	}
}

//============================================================================

static void Script_JukeboxAddSource( ScriptVM* vm )
{
	ASSERT_PTR(vm);
	CActor *who = vm->PopActor();
	ASSERT(who);
	if (who)
		g_audio.jukebox.WatchPlayPosition(&who->GetBodyInWorld());
}

//============================================================================

static void Script_JukeboxRemoveSource( ScriptVM* vm )
{
	ASSERT_PTR(vm);
	CActor *who = vm->PopActor();
	ASSERT(who);
	if (who)
		g_audio.jukebox.RemovePlayPosition(who->GetBodyInWorld());
}

//============================================================================

static void Script_JukeboxNextTrack( ScriptVM* vm )
{
	g_audio.jukebox.Next();
}

//============================================================================
static void Script_Box_InsideMe( ScriptVM* vm )
{
	CActor *boxActor = vm->PopActor();
	ASSERT(boxActor);
	if (boxActor == NULL)
		return;

	// Allow this entity to collide with actors and test for collisions.
	boxActor->SetPositiveInclude( AI_BIT );
	g_collisionSystem.ResetStack();
	CCollisionList* list = g_collisionSystem.Test(boxActor);

	if ( list )
	{
		for ( CCollisionEvent* event = list->GetHeadEvent() ; event ; event = event->GetNext() )
		{
			if(event->GetTargetEntity()->IsA(CActor::ClassType()))
				g_messageDispatcher.SendMessageToActor( "KillInside", NULL, boxActor->GetHandle(), ((CActor*)event->GetTargetEntity())->GetHandle());
		}
	}
	boxActor->SetPositiveInclude( NO_CATEGORY );
}
//============================================================================
static void Script_Pickup_SetAttachableToActor( ScriptVM* vm )
{
	bool toggle = vm->PopBool();
	CActor* actor = (CActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType()  ));
	Pickup *pickup = (Pickup *) ((CCompActor*) actor)->GetComponentByName( "Pickup" );
	if( pickup )
		pickup->SetAttachableToActor(toggle);
}
//============================================================================

static void Script_Pickup_SetDropOnButton( ScriptVM* vm )
{
	bool toggle = vm->PopBool();
	CActor* actor = (CActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType()  ));
	Pickup *pickup = (Pickup *) ((CCompActor*) actor)->GetComponentByName( "Pickup" );
	if( pickup )
		pickup->SetDropOnButton(toggle);
}
//============================================================================
static void Script_Pickup_SetDropOnDeath( ScriptVM* vm )
{
	bool toggle = vm->PopBool();
	CActor* actor = (CActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType()  ));
	Pickup *pickup = (Pickup *) ((CCompActor*) actor)->GetComponentByName( "Pickup" );
	if( pickup )
		pickup->SetDropOnDeath(toggle);
}
//============================================================================
static void Script_Pickup_SetParticleEmitter( ScriptVM* vm )
{
	Vector3 offset = vm->PopVector3();
	const char* emitterName = vm->PopString();
	CActor* actor = (CActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType()  ));
	Pickup *pickup = (Pickup *) ((CCompActor*) actor)->GetComponentByName( "Pickup" );
	if( pickup )
	{
		pickup->SetParticleEmitter( emitterName, offset );
	}
}
//============================================================================
static void Script_Pickup_IsHome( ScriptVM* vm )
{
	CCompActor* pickupActor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( pickupActor );
	ASSERT( pickupActor->IsA( CCompActor::ClassType() ) );
	Pickup* pickup = (Pickup*) pickupActor->GetComponentByName( "Pickup" );
	ASSERT_PTR( pickup );

	bool result = false;

	if ( pickup )
	{
		result = pickup->IsState(((uint8)CObjectiveIF::HOME | (uint8)CObjectiveIF::AVAILABLE));
	}

	vm->Push( result ? 1 : 0 );
}
//============================================================================
static void Script_Pickup_IsDropped( ScriptVM* vm )
{
	CCompActor* pickupActor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( pickupActor );
	ASSERT( pickupActor->IsA( CCompActor::ClassType() ) );
	Pickup* pickup = (Pickup*) pickupActor->GetComponentByName( "Pickup" );
	ASSERT_PTR( pickup );

	bool result = false;

	if ( pickup )
	{
		result = pickup->IsState(((uint8)CObjectiveIF::DROPPED | (uint8)CObjectiveIF::AVAILABLE));
	}

	vm->Push( result ? 1 : 0 );
}
//============================================================================
static void Script_Pickup_IsHeld( ScriptVM* vm )
{
	CCompActor* pickupActor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( pickupActor );
	ASSERT( pickupActor->IsA( CCompActor::ClassType() ) );
	Pickup* pickup = (Pickup*) pickupActor->GetComponentByName( "Pickup" );
	ASSERT_PTR( pickup );

	bool result = false;

	if ( pickup )
	{
		result = pickup->IsState(((uint8)CObjectiveIF::TRANSIT | (uint8)CObjectiveIF::CLAIMED));
	}

	vm->Push( result ? 1 : 0 );
}
//============================================================================
static void Script_Pickup_IsCaptured( ScriptVM* vm )
{
	CCompActor* pickupActor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( pickupActor );
	ASSERT( pickupActor->IsA( CCompActor::ClassType() ) );
	Pickup* pickup = (Pickup*) pickupActor->GetComponentByName( "Pickup" );
	ASSERT_PTR( pickup );

	bool result = false;

	if ( pickup )
	{
		result = pickup->IsState(((uint8)CObjectiveIF::DROPPED | (uint8)CObjectiveIF::OWNED));
	}

	vm->Push( result ? 1 : 0 );
}
//============================================================================
static void Script_Pickup_IsAcquired( ScriptVM* vm )
{
	CCompActor* pickupActor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( pickupActor );
	ASSERT( pickupActor->IsA( CCompActor::ClassType() ) );
	Pickup* pickup = (Pickup*) pickupActor->GetComponentByName( "Pickup" );
	ASSERT_PTR( pickup );

	bool result = false;

	if ( pickup )
	{
		result = !pickup->IsControl(CObjectiveIF::AVAILABLE);
	}

	vm->Push( result ? 1 : 0 );
}
//============================================================================
static void Script_Pickup_GetHolder( ScriptVM* vm )
{
	CCompActor* pickupActor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( pickupActor );
	ASSERT( pickupActor->IsA( CCompActor::ClassType() ) );
	Pickup* pickup = (Pickup*) pickupActor->GetComponentByName( "Pickup" );
	ASSERT_PTR( pickup );

	ActorHandle holder;

	if ( pickup )
	{
		holder = pickup->GetMostRecentHolder();
	}

	vm->Push( CActor::FromHandle( holder ) );
}
//============================================================================
static void Script_Pickup_GetDropper( ScriptVM* vm )
{
	CCompActor* pickupActor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( pickupActor );
	ASSERT( pickupActor->IsA( CCompActor::ClassType() ) );
	Pickup* pickup = (Pickup*) pickupActor->GetComponentByName( "Pickup" );
	ASSERT_PTR( pickup );

	ActorHandle dropper;

	if ( pickup )
	{
		dropper = pickup->GetMostRecentHolder();
	}

	vm->Push( CActor::FromHandle( dropper ) );
}
//============================================================================
static void Script_Pickup_GetTransitionTime( ScriptVM* vm )
{
	CCompActor* pickupActor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( pickupActor );
	ASSERT( pickupActor->IsA( CCompActor::ClassType() ) );
	Pickup* pickup = (Pickup*) pickupActor->GetComponentByName( "Pickup" );
	ASSERT_PTR( pickup );

	int32 time = 0;

	if ( pickup )
	{
		time = pickup->GetTimeOfStateMS();
	}

	vm->Push( time );
}

//============================================================================
static void Script_TagFrame_AssumeRoleOfClosestSpawn( ScriptVM* vm )
{
	bool flag = vm->PopBool();

	CActor* actor = (CActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType()  ));

	CTagFrame *tagframe = (CTagFrame *) ((CCompActor*) actor)->GetComponentByName( "TagFrame" );
	if( tagframe )
		tagframe->AssumeRoleOfClosestSpawn(flag);
}

//============================================================================
static void Script_TagFrame_CanInteract( ScriptVM* vm )
{
	CActor* querier = (CActor*) vm->PopActor();
	ASSERT_PTR( querier );

	CActor* actor = (CActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	bool result = false;
	CTagFrame *tagframe = (CTagFrame *) ((CCompActor*) actor)->GetComponentByName( "TagFrame" );
	if( tagframe )
		result = tagframe->CanInteract(CActor::ToHandle(querier));

	vm->Push( result ? 1 : 0 );
}

//============================================================================
static void Script_TagFrame_Reset( ScriptVM* vm )
{
	CActor* actor = (CActor*)vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	CTagFrame *tagframe = (CTagFrame*)((CCompActor*)actor)->GetComponentByName( "TagFrame" );
	if ( tagframe )
		tagframe->Reset();
}

//============================================================================
static void Script_TagFrame_AnyBlueTags( ScriptVM* vm )
{
	CActor* tag = NULL;
	while((tag = g_objectiveMgr.GetNext(tag)) != NULL)
	{
		if (tag->IsA(CCompActor::ClassType()))
		{
			CTagFrame *tagframe =
					(CTagFrame*)((CCompActor*)tag)->GetComponentByName("TagFrame");
			if ((tagframe != NULL) && (tagframe->GetRole() == CTeamingModel::BLUE))
			{
				vm->Push(1);
				return;
			}
		}
	}
	vm->Push(0);
}

//============================================================================
static void Script_TagFrame_AnyRedTags( ScriptVM* vm )
{
	CActor* tag = NULL;
	while((tag = g_objectiveMgr.GetNext(tag)) != NULL)
	{
		if (tag->IsA(CCompActor::ClassType()))
		{
			CTagFrame *tagframe =
					(CTagFrame*)((CCompActor*)tag)->GetComponentByName("TagFrame");
			if ((tagframe != NULL) && (tagframe->GetRole() == CTeamingModel::RED))
			{
				vm->Push(1);
				return;
			}
		}
	}
	vm->Push(0);
}

//============================================================================
static void Script_ObjectiveMgr_Add( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	bool result = false;
	if (ValidSingleton(CObjectiveMgr))
		result = g_objectiveMgr.Add( actor );

	vm->Push( result ? 1 : 0 );
}

//============================================================================
static void Script_ObjectiveMgr_GetFirstDropped( ScriptVM* vm )
{
	if (ValidSingleton(CObjectiveMgr))
		vm->Push( g_objectiveMgr.GetFirst(CObjectiveIF::DROPPED) );
	else
		vm->Push( 0 );
}

//============================================================================
static void Script_ObjectiveMgr_AllCaptured( ScriptVM* vm )
{
	if (ValidSingleton(CObjectiveMgr))
		vm->Push( g_objectiveMgr.AreAll(CObjectiveIF::OWNED) ? 1 : 0 );
	else
		vm->Push( 0 );
}

//============================================================================
static void Script_Path_SetAtRandomPoint( ScriptVM* vm )
{
	CActor* actor = (CActor*)vm->GetOwner();
	ASSERT( actor->IsA( CCompActor::ClassType()  ));
	CPathFollower* pathFollower = (CPathFollower*)( ((CCompActor*) actor)->GetComponentByName( "PathFollower" ) );
	if( pathFollower )
	{
		float spawnT = g_random.InRange(0.1f,0.9f); //don't want to start them at start or end
		pathFollower->SetDistanceFromT(spawnT);
		actor->Place(pathFollower->GetPath()->PointFromT(spawnT),0.0f);
	}
}

//============================================================================

static void Script_Participant_GetActor( ScriptVM* vm )
{
	int actorId = vm->PopInt();
	CActor* actor = g_referee.GetActor( actorId );
	vm->Push( actor );
}

//============================================================================

static void Script_Participant_GetId( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	int id = CParticipantIF::GetParticipantIdFromActor( actor );
	vm->Push( id );
}

//============================================================================

static void Script_Participant_GetTeam( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	CParticipantIF* myParticipant =
		(CParticipantIF*)actor->GetInterface(CParticipantIF::GetID());

	int team = -1;

	if (myParticipant != NULL)
		team = myParticipant->GetTeamId();

	vm->Push( team );
}

//============================================================================

static void Script_Participant_GetRole( ScriptVM* vm )
{
	CActor* actor = vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	uint8 id = CParticipantIF::GetParticipantIdFromActor( actor );
	vm->Push( g_referee.GetTeamingModel().GetPlayerRole( id ) );
}

//============================================================================

static void Script_Participant_GetSpawnTicket( ScriptVM* vm )
{
	int actorId = vm->PopInt();
	CParticipantIF* participant =
			CParticipantIF::GetInterface( g_referee.GetActor( actorId ) );
	vm->Push( ( ( participant != NULL ) && participant->GetSpawnTicket() ) ? 1 : 0 );
}

//============================================================================

static void Script_DoDamage( ScriptVM* vm )
{
	//int		weaponType	= vm->PopInt();
	int		damageID		= vm->PopInt();
	float		damage		= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	HealthIF* hIF = (HealthIF*) actor->GetInterface( HealthIF::GetID() );
	ASSERT_PTR( hIF );
	if( hIF )
	{
		hIF->DoDamage( damage, HealthIF::FOR_FUN, damageID );
	}
}

//============================================================================

static void Script_AddHealth( ScriptVM* vm )
{
	float		addHealth	= vm->PopFloat();
	CCompActor*	actor		= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	HealthIF* hIF = (HealthIF*) actor->GetInterface( HealthIF::GetID() );
	ASSERT_PTR( hIF );
	if( hIF )
	{
		hIF->AddHealth( addHealth );
	}
}

//============================================================================

static void Script_GetHealth( ScriptVM* vm )
{
	CCompActor*	actor = (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );
	float health = 0.0f;
	if (actor)
	{
		Property* healthProp = actor->GetProperty("health");
		if (healthProp)
			health = healthProp->GetFloat();
	}
	vm->Push( health );
}

//============================================================================

static void Script_GetMaxHealth( ScriptVM* vm )
{
	CCompActor*	actor		= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	HealthIF* hIF = (HealthIF*) actor->GetInterface( HealthIF::GetID() );
	ASSERT_PTR( hIF );
	if( hIF )
	{
		vm->Push( hIF->GetMaxHealth() );
	}
	else
	{
		vm->Push( 0.0f );
	}
}

//============================================================================

//static void Script_Kill( ScriptVM* vm )
//{
//	CCompActor*	actor		= (CCompActor*) vm->PopActor();
//	ASSERT_PTR( actor );
//	ASSERT( actor->IsA( CCompActor::ClassType() ) );
//
//	HealthIF* hIF = (HealthIF*) actor->GetInterface( HealthIF::GetID() );
//	ASSERT_PTR( hIF );
//	if( hIF )
//	{
//		hIF->Kill();
//	}
//}

//============================================================================

static void Script_ConfigMgr_TestOptionValue( ScriptVM* vm )
{
	const char* value = vm->PopString();
	const char* key = vm->PopString();

	vm->Push( ( int )( g_configmgr.TestOptionValue( key, value ) ? 1 : 0 ) );
}

//============================================================================

static void Script_ConfigMgr_GetCurrentLevelName( ScriptVM* vm )
{
	int index = g_configmgr.GetOptionIndexByName( "gameMap" );
	const char* name = "";
	if ( index >= 0 )
		name = g_configmgr.GetDisplayValue( index );
	vm->Push( (char*)name );
}

//============================================================================

static void Script_ConfigMgr_GetChoiceAttributeInt( ScriptVM* vm )
{
	const char* attribute = vm->PopString();
	const char* name = vm->PopString();

	const char* result = g_configmgr.GetChoiceAttribute( name, attribute );
	vm->Push( ( result != NULL ) ? atoi( result ) : 0 );
}

//============================================================================

static void Script_ConfigMgr_GetChoiceAttributeString( ScriptVM* vm )
{
	const char* attribute = vm->PopString();
	const char* name = vm->PopString();

	vm->Push( (char*)g_configmgr.GetChoiceAttribute( name, attribute ) );
}

//============================================================================

static void Script_ActorCreateFromParametersWithPrefix( ScriptVM* vm )
{
	const char* overrideParms = vm->PopString();
	const char*	instanceName  = vm->PopString();
	float       heading = vm->PopFloat();
	Vector3     pos     = vm->PopVector3();
	const char* prefix  = vm->PopString();
	CActor*     actor   = vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	ParameterSet parms;
	parms.SetFromParameterSetWithPrefix( actor->Parm(), prefix );
	if ( overrideParms && overrideParms[0] )
		parms.SetFromString( overrideParms );

	CActor* newActor = ActorCreator::CreateFromParameters( parms, pos, heading );

	if ( newActor->Instance() )
		newActor->Instance()->SetName( instanceName && instanceName[0] ? instanceName : "CREATED" );

	newActor->Script( AI_SCRIPT )->Run( 0.0f );

	char* net = parms.GetString("net");
	if ((net == NULL) || strcmp(net, "local"))
		newActor->DistributeNetActor((net == NULL) || strcmp(net, "nonpersistent"));

	vm->Push( newActor );
}

//============================================================================

static void Script_ActorCreateFromInitString( ScriptVM* vm )
{
	const char*	instanceName = vm->PopString();
	float       heading = vm->PopFloat();
	Vector3     pos     = vm->PopVector3();
	const char* initStr = vm->PopString();
	CActor*     actor   = vm->PopActor();

	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CActor::ClassType() ) );

	ParameterSet parms( initStr );

	CActor* newActor = ActorCreator::CreateFromParameters( parms, pos, heading );

	if ( newActor->Instance() )
		newActor->Instance()->SetName( instanceName && instanceName[0] ? instanceName : "CREATED" );

	newActor->Script( AI_SCRIPT )->Run( 0.0f );

	char* net = parms.GetString("net");
	if ((net == NULL) || strcmp(net, "local"))
		newActor->DistributeNetActor((net == NULL) || strcmp(net, "nonpersistent"));

	vm->Push( newActor );
}

//============================================================================

static void Script_UpdatePain( ScriptVM* vm )
{
	float			damage = vm->PopFloat();
	int			munitionID = vm->PopInt();
	int			location  = vm->PopInt();
	int			direction = vm->PopInt();
	CCompActor*     actor   = (CCompActor*) vm->PopActor();
	ASSERT(actor);

	if (actor)
		CPainStats::HandleHit(*actor, (CPainStats::Location)location, (CPainStats::Direction)direction, munitionID, damage);
}

//============================================================================

static void Script_ShowTextMessage( ScriptVM* vm )
{
	int		override = vm->PopInt();
	int		pauseIt = vm->PopInt();
	float		delay = vm->PopFloat();
	char*		textMessage = vm->PopString();


	GameHUD_ShowMessage msg = { textMessage, delay, (pauseIt != 0), (override != 0) };
	GameHUD::SendMessage(GAMEHUD_SHOWMESSAGE, &msg);
}

//============================================================================

static void Script_HideHud( ScriptVM* vm )
{
	if (ValidSingleton(GameHUD))
		g_hud.ShowOverride(false);
}

//============================================================================

static void Script_ShowHud( ScriptVM* vm )
{
	if (ValidSingleton(GameHUD))
		g_hud.ShowOverride(true);
}

//============================================================================

static void Script_SetupCheckpoint( ScriptVM* vm )
{
	Vector3	facing		= vm->PopVector3();
	Vector3	pos			= vm->PopVector3();
	int		checkPoint	= vm->PopInt();
	// save the checkpoint progress out to the memory card.
#ifdef DIRECTX_PC
	// only save if we haven't saved here before
	if ( g_players.GetPlayer( 0 ) && ( checkPoint > g_GlobalProfile.GetNumValidCheckpoints( g_gameEnvironment.Get() ) ) )
	{
		ts_Checkpoint tempCheckpoint;
		tempCheckpoint.checkpoint = checkPoint;
		tempCheckpoint.facing = facing;
		tempCheckpoint.pos = pos;
		// set up our stats
		// health first
		PlayerStats *stats = (PlayerStats*)g_players.GetPlayer(0)->GetInterface(PlayerStats::GetID());
		if (stats)
			tempCheckpoint.checkpointStats.Health = (int)(stats->GetHealthPercentage()*100.0f);
		// then weapons
		TTLWeaponInventory *inv = (TTLWeaponInventory *)g_players.GetPlayer(0)->GetComponentByName( "WeaponInventory" );
		TTLWeapon* tempWeapon;
		for (int c=0; c < SG_MAX_WEAPONS_PER_PLAYER; ++c)
		{
			tempWeapon = (TTLWeapon*)inv->Get(c);
			if ( tempWeapon )
			{
				strcpy(tempCheckpoint.checkpointWeapons[c].WeaponName,tempWeapon->Name());
				tempCheckpoint.checkpointWeapons[c].Ammo = tempWeapon->Ammo();
			}
		}
		g_GlobalProfile.SetLevelCheckpoint( g_gameEnvironment.Get(), checkPoint, tempCheckpoint );

		// do the actual saving
		g_MemCardManager->SetUsePopups(false);
		g_MemCardManager->SetAutosave( true );
		g_MemCardManager->SetAreYouSure( false );
		g_MemCardManager->SetOverwriteConfirm( false );
		g_MemCardManager->SetNoOkConfirmation( true );
		CSaveGame::AutoSave();
		// give us our focus back (the popup took it away =( )
		g_FrontEnd.ReleaseInputFocus();
	}
#endif
	if (ValidSingleton(PlayerSpawnMgr))
		g_PlayerSpawnMgr.AddBox( pos, facing );
}

//============================================================================

static void Script_ResetAllCheckpointActors( ScriptVM* vm )
{
	int checkpoint	= vm->PopInt();
	CTTLLoop::WorldLoader().RecreateActorsForCheckpoint( checkpoint );
}

//============================================================================

static void Script_IsSinglePlayer( ScriptVM* vm )
{
	vm->Push( (int)g_ttl->IsSinglePlayer() );
}

//============================================================================

static void Script_IsMultiPlayer( ScriptVM* vm )
{
	vm->Push( (int)g_ttl->IsMultiPlayer() );
}

//============================================================================

static void Script_PlayCutScene( ScriptVM* vm )
{
//	CCompActor*	actor		= (CCompActor*) vm->PopActor();
//	ASSERT_PTR( actor );
//	ASSERT( actor->IsA( CCompActor::ClassType() ) );

//	g_cutSceneManager.PlayCutScene( "t:\\cutscenes\\culdesac.cut" );
}

//============================================================================

static void Script_GetWeaponDisplayName( ScriptVM* vm )
{
	const char* name	= vm->PopString();

	TTLWeaponProperties *props = (TTLWeaponProperties *)g_weaponLib.GetProperties(name);
	if ( props )
		vm->Push( (char *)props->m_displayName );
	else
		vm->Push( "" );
}

//============================================================================

static void Script_GetWeaponModelName( ScriptVM* vm )
{
	const char* name	= vm->PopString();
	ASSERT(name && *name);

	TTLWeaponProperties *props = (TTLWeaponProperties *)g_weaponLib.GetProperties(name);
	ASSERT(props);

 	if ( props )
		vm->Push( (char *)props->m_model[0].name );
	else
		vm->Push( "" );
}

//============================================================================

static void Script_GetWeaponSlot( ScriptVM* vm )
{
	const char* name	= vm->PopString();

	TTLWeaponProperties *props = (TTLWeaponProperties *)g_weaponLib.GetProperties(name);
	if ( props )
		vm->Push( props->m_slot );
	else
		vm->Push( -1 );
}

//============================================================================
// Returns -1 if it doesn't exist, or it's slot
//============================================================================

static void Script_FindWeaponSlot( ScriptVM* vm )
{
	const char* name	= vm->PopString();
	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	// they better have one of these
	TTLWeaponInventory *inv = (TTLWeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	if ( !inv )
	{
 		vm->Push( -1 );
		return;
	}

	// Find the weapon
	TTLWeapon *weapon = (TTLWeapon *)inv->Find( name );
	if ( weapon )
 		vm->Push( weapon->GetSlot() );
	else
 		vm->Push( -1 );
}

//============================================================================

static void Script_AddWeapon( ScriptVM* vm )
{
	bool selectIfBetter = vm->PopBool();
	const char* name	= vm->PopString();

	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );
	if ( !inv )
	{
		vm->Push( 0 );
		return;
	}

	// Create it!
	TTLWeapon *weapon = (TTLWeapon *)g_weaponLib.CreateInstance(name);
	ASSERT( weapon );
	if ( !weapon )
	{
		vm->Push( 0 );
		return;
	}

	// Stick in the weapon (inventory will delete the old one)
	int slot = weapon->GetSlot();
	bool ret = inv->Add( weapon, slot );
	ASSERT( ret );

	// Select if better?
	if (ret && selectIfBetter)
	{
		// Do they have a hostage? because if they do we can't change anything
		bool haveHostage = false;
		Property *prop = actor->QueryProperty( "hostage" );
		if (prop)
		{
			CActor *hostage = CActor::FromHandle(prop->GetHandle());
			haveHostage = (hostage != NULL);
		}

		if (!haveHostage)
		{
			int current = inv->GetSelectedIndex();
			if (current == -1 || (slot != 3 && slot > current))
				inv->Select(slot);
		}
	}

	vm->Push( ret );
}

//============================================================================

static void Script_RemoveWeapon( ScriptVM* vm )
{
	const char* name	= vm->PopString();

	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );
	if ( !inv )
	{
		vm->Push( 0 );
		return;
	}

	// Get the weapon
	TTLWeapon *weapon = (TTLWeapon *)inv->Find( name );
	if ( !weapon )
	{
		vm->Push( 0 );
		return;
	}

	// Destory it!
	bool ret = inv->Remove( weapon->GetSlot() );
	ASSERT( ret );
	vm->Push( ret );
}

//============================================================================

static void Script_GetWeaponInSlot( ScriptVM* vm )
{
	int slot	= vm->PopInt();

	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	// Assume nothing
	const char* weaponName = "";

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );

	// there is a weapon in this slot - return what it is
	if ( inv && inv->Get(slot) )
		weaponName = inv->GetName(slot);

	vm->Push( (char *)weaponName );
}

//============================================================================

static void Script_RemoveWeaponInSlot( ScriptVM* vm )
{
	int slot	= vm->PopInt();

	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );
	if( inv )
		inv->Remove( slot );			// remove weapon from slot to free the slot up
}

//============================================================================

static void Script_GetSlotAmmo( ScriptVM* vm )
{
	int slot = vm->PopInt();

	CCompActor* actor		= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	int ammo = 0;
	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );

	if( inv )
	{
		TTLWeapon *weapon = (TTLWeapon *)inv->Get( slot );
		if ( weapon )
			ammo = weapon->Ammo();
	}

	vm->Push( ammo );
}

//============================================================================

static void Script_SlotNeedsAmmo( ScriptVM* vm )
{
	int slot = vm->PopInt();

	CCompActor* actor		= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	bool needsAmmo = false;
	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );
	if ( inv )
	{
		TTLWeapon *weapon = (TTLWeapon *)inv->Get( slot );
		if ( weapon )
			needsAmmo = (weapon->Ammo() != weapon->MaxAmmo());
	}

	vm->Push( needsAmmo );
}

//============================================================================

static void Script_AddSlotAmmo( ScriptVM* vm )
{
	int amount = vm->PopInt();
  	int slot = vm->PopInt();

	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );
	if ( inv )
	{
		TTLWeapon *weapon = (TTLWeapon *)inv->Get( slot );
		if ( weapon )
			weapon->AddAmmo(weapon->GetAmmoType(), amount);
	}
}

//============================================================================

static void Script_GetSlotAmmoCode( ScriptVM* vm )
{
	int slot = vm->PopInt();

	CCompActor* actor		= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	int ammoCode = -1;
	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );

	if( inv )
	{
		TTLWeapon *weapon = (TTLWeapon *)inv->Get( slot );
		if ( weapon )
			ammoCode = weapon->CodifyAmmo();
	}

	vm->Push( ammoCode );
}

//============================================================================

static void Script_ApplySlotAmmoCode( ScriptVM* vm )
{
	int ammoCode = vm->PopInt();
	int slot = vm->PopInt();

	CCompActor* actor		= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );
	if( inv )
	{
		TTLWeapon *weapon = (TTLWeapon *)inv->Get( slot );
		if ( weapon )
			weapon->ApplyAmmoCode(ammoCode);
	}
}

//============================================================================

static void Script_GetSlotSplitAmmo( ScriptVM* vm )
{
	int slot   = vm->PopInt();
	CCompActor* actor = (CCompActor*) vm->PopActor();
	ASSERT_PTR(actor);
	ASSERT(actor->IsA(CCompActor::ClassType()));

	int inBelt = 0;
	int inClip = 0;

	WeaponInventory* inv = (WeaponInventory*) actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR(inv);
	if (inv)
	{
		TTLWeapon* weapon = (TTLWeapon*) inv->Get(slot);
		if (weapon)
			weapon->GetSplitAmmo(inBelt, inClip);
	}

	vm->Push(inBelt);
	vm->Push(inClip);
}

static void Script_WeaponAmmoCompatible( ScriptVM* vm )
{
	const char *weapon1 = vm->PopString();
	const char *weapon2 = vm->PopString();
	bool compatible = false;

	TTLWeaponProperties *weapProps1 = (TTLWeaponProperties *)g_weaponLib.GetProperties(weapon1);
	TTLWeaponProperties *weapProps2 = (TTLWeaponProperties *)g_weaponLib.GetProperties(weapon2);
	if (weapProps1 && weapProps2)
	{
		TTLProjectileProperties *projProps1 = (TTLProjectileProperties *)g_projectileLib.GetProperties(weapProps1->m_projectile);
		TTLProjectileProperties *projProps2 = (TTLProjectileProperties *)g_projectileLib.GetProperties(weapProps2->m_projectile);
		if (projProps1 && projProps2)
		{
			compatible = (strcmpi(projProps1->m_ammoType, projProps2->m_ammoType) == 0);
		}
	}

	vm->Push(compatible);
}

//============================================================================

static void Script_SetSlotSplitAmmo( ScriptVM* vm )
{
	int inClip = vm->PopInt();
	int inBelt = vm->PopInt();
	int slot   = vm->PopInt();
	CCompActor* actor = (CCompActor*) vm->PopActor();
	ASSERT_PTR(actor);
	ASSERT(actor->IsA(CCompActor::ClassType()));

	WeaponInventory* inv = (WeaponInventory*) actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR(inv);
	if (inv)
	{
		TTLWeapon* weapon = (TTLWeapon*) inv->Get(slot);
		if (weapon)
			weapon->SetSplitAmmo(inBelt, inClip);
	}
}

//============================================================================

static void Script_TakeLoadoutSnapshot( ScriptVM* vm )
{
	char* loadoutString = NULL;

	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );
	if ( inv )
	{
		WeaponLoadout loadout(*inv);
		ParameterSet &param = loadout.GetParameterSet();
		loadoutString = (char*) param.GetInitializerString();
	}

	vm->Push( loadoutString );
}

//============================================================================

static void Script_FreeLoadoutSnapshot( ScriptVM* vm )
{
	char* loadout = vm->PopString();
	if ( loadout )
		delete [] loadout;
}

//============================================================================

static void Script_ApplyLoadoutSnapshot( ScriptVM* vm )
{
	char* loadoutString = vm->PopString();

	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	if (loadoutString && actor)
	{
		WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
		ASSERT_PTR( inv );
		if ( inv )
		{
			WeaponLoadout loadout(loadoutString);
			loadout.Build(*inv, true);
		}
	}
}

//============================================================================

static void Script_GetSelectedWeaponSlot( ScriptVM* vm )
{
	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	int slot = -1;

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );
	if ( inv )
		slot = inv->GetSelectedIndex();

	vm->Push( slot );
}

//============================================================================

static void Script_SetSelectedWeaponSlot( ScriptVM* vm )
{
	int slot = vm->PopInt();

	CCompActor* actor	= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	WeaponInventory *inv = (WeaponInventory *)actor->GetComponentByName( "WeaponInventory" );
	ASSERT_PTR( inv );
	if ( inv )
		inv->Select( slot );
}

//============================================================================

static void Script_Referee_IsSetupPhase( ScriptVM* vm )
{
	vm->Push( ( int )( ( g_referee.GetPhase() == Referee::SETUP ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsReadyPhase( ScriptVM* vm )
{
	vm->Push( ( int )( ( g_referee.GetPhase() == Referee::READY ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsPopulatingPhase( ScriptVM* vm )
{
	vm->Push( ( int )( ( g_referee.GetPhase() == Referee::POPULATING ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsPlayingPhase( ScriptVM* vm )
{
	vm->Push( ( int )( ( g_referee.GetPhase() == Referee::PLAYING ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsReportingPhase( ScriptVM* vm )
{
	vm->Push( ( int )( ( g_referee.GetPhase() == Referee::REPORTING ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_SetRoundsInMatch( ScriptVM* vm )
{
	int rounds = vm->PopInt();

	if( ValidSingleton( Referee ) )
		g_referee.SetRoundsInMatch( rounds);
}

//============================================================================

static void Script_Referee_PreventSetupPhaseExit( ScriptVM* vm )
{
	g_referee.PreventSetupPhaseExit();
}

//============================================================================

static void Script_Referee_PreventPopulatingPhaseExit( ScriptVM* vm )
{
	g_referee.PreventPopulatingPhaseExit();
}

//============================================================================

static void Script_Referee_PreventPlayingPhaseExit( ScriptVM* vm )
{
	g_referee.PreventPlayingPhaseExit();
}

//============================================================================

static void Script_Referee_PreventReportingPhaseExit( ScriptVM* vm )
{
	g_referee.PreventReportingPhaseExit();
}

//============================================================================

static void Script_Referee_AllowSetupPhaseExit( ScriptVM* vm )
{
	g_referee.AllowSetupPhaseExit();
}

//============================================================================

static void Script_Referee_AllowPopulatingPhaseExit( ScriptVM* vm )
{
	g_referee.AllowPopulatingPhaseExit();
}

//============================================================================

static void Script_Referee_AllowPlayingPhaseExit( ScriptVM* vm )
{
	g_referee.AllowPlayingPhaseExit();
}

//============================================================================

static void Script_Referee_AllowReportingPhaseExit( ScriptVM* vm )
{
	g_referee.AllowReportingPhaseExit();
}

//============================================================================

static void Script_Referee_CommandSetupPhaseExit( ScriptVM* vm )
{
	g_referee.CommandSetupPhaseExit();
}

//============================================================================

static void Script_Referee_CommandPopulatingPhaseExit( ScriptVM* vm )
{
	g_referee.CommandPopulatingPhaseExit();
}

//============================================================================

static void Script_Referee_CommandPlayingPhaseExit( ScriptVM* vm )
{
	g_referee.CommandPlayingPhaseExit();
}

//============================================================================

static void Script_Referee_CommandReportingPhaseExit( ScriptVM* vm )
{
	g_referee.CommandReportingPhaseExit();
}

//============================================================================

static void Script_Referee_SetSetupPhaseEndTime( ScriptVM* vm )
{
	int time = vm->PopInt();

	g_referee.SetSetupPhaseEndTime( time );
}

//============================================================================

static void Script_Referee_SetPopulatingPhaseEndTime( ScriptVM* vm )
{
	int time = vm->PopInt();

	g_referee.SetPopulatingPhaseEndTime( time );
}

//============================================================================

static void Script_Referee_SetPlayingPhaseEndTime( ScriptVM* vm )
{
	int time = vm->PopInt();

	g_referee.SetPlayingPhaseEndTime( time );
}

//============================================================================

static void Script_Referee_SetReportingPhaseEndTime( ScriptVM* vm )
{
	int time = vm->PopInt();

	g_referee.SetReportingPhaseEndTime( time );
}

//============================================================================

static void Script_Referee_SetSetupPhaseLifetime( ScriptVM* vm )
{
	int time = vm->PopInt();

	g_referee.SetSetupPhaseLifetime( time );
}

//============================================================================

static void Script_Referee_SetPopulatingPhaseLifetime( ScriptVM* vm )
{
	int time = vm->PopInt();

	g_referee.SetPopulatingPhaseLifetime( time );
}

//============================================================================

static void Script_Referee_SetPlayingPhaseLifetime( ScriptVM* vm )
{
	int time = vm->PopInt();

	g_referee.SetPlayingPhaseLifetime( time );
}

//============================================================================

static void Script_Referee_SetReportingPhaseLifetime( ScriptVM* vm )
{
	int time = vm->PopInt();

	g_referee.SetReportingPhaseLifetime( time );
}

//============================================================================

static void Script_Referee_SetRemainingPhaseTime( ScriptVM* vm )
{
	int exceedLifetime = vm->PopInt();
	int time = vm->PopInt();

	g_referee.SetRemainingPhaseTime( time, (exceedLifetime != 0) );
}

//============================================================================

static void Script_Referee_GetElapsedPhaseTime( ScriptVM* vm )
{
	vm->Push( g_referee.GetElapsedPhaseTime() );
}

//============================================================================

static void Script_Referee_GetRemainingPhaseTime( ScriptVM* vm )
{
	vm->Push( g_referee.GetRemainingPhaseTime() );
}

//============================================================================

static void Script_Referee_GetCurrentPhaseStartTime( ScriptVM* vm )
{
	vm->Push( g_referee.GetCurrentPhaseStartTime() );
}

//============================================================================

static void Script_Referee_IsTimedPhase( ScriptVM* vm )
{
	vm->Push( g_referee.IsTimedPhase() ? 1 : 0 );
}

//============================================================================

static void Script_Referee_GetNumStartPlayers( ScriptVM* vm )
{
	vm->Push( g_referee.GetNumStartPlayers() );
}

//============================================================================

static void Script_Referee_GetNumPlayers( ScriptVM* vm )
{
	vm->Push( g_referee.GetNumPlayers() );
}

//============================================================================

static void Script_Referee_GetNumNonPlayers( ScriptVM* vm )
{
	vm->Push( g_referee.GetNumNonPlayers() );
}

//============================================================================

static void Script_Referee_GetNumParticipants( ScriptVM* vm )
{
	vm->Push( g_referee.GetNumParticipants() );
}

//============================================================================

static void Script_Referee_SetUnlimitedSpawnTickets( ScriptVM* vm )
{
	uint8 teamId = (uint8)vm->PopInt();

	g_referee.SetUnlimitedSpawnTickets( teamId );
}

//============================================================================

static void Script_Referee_SetUnlimitedSpawnTicketsForAll( ScriptVM* vm )
{
	g_referee.SetUnlimitedSpawnTicketsForAll();
}

//============================================================================

static void Script_Referee_SetNumSpawnTickets( ScriptVM* vm )
{
	uint16 numSpawnTickets = (uint16)vm->PopInt();
	uint8 teamId = (uint8)vm->PopInt();

	g_referee.SetNumSpawnTickets( teamId, numSpawnTickets );
}

//============================================================================

static void Script_Referee_SetNumSpawnTicketsForAll( ScriptVM* vm )
{
	uint16 numSpawnTickets = (uint16)vm->PopInt();

	g_referee.SetNumSpawnTicketsForAll( numSpawnTickets );
}

//============================================================================

static void Script_Referee_DoesTeamLimitSpawnTickets( ScriptVM* vm )
{
	uint8 teamId = (uint8)vm->PopInt();

	vm->Push( g_referee.DoesTeamLimitSpawnTickets( teamId ) ? 1 : 0 );
}

//============================================================================

static void Script_Referee_DoesTeamLimitSpawnTicketsForParticipant( ScriptVM* vm )
{
	uint8 participantId = (uint8)vm->PopInt();

	vm->Push( g_referee.DoesTeamLimitSpawnTicketsForParticipant( participantId ) ? 1 : 0 );
}

//============================================================================

static void Script_Referee_DoesTeamLimitSpawnTicketsForLocalAvatar( ScriptVM* vm )
{
	vm->Push( g_referee.DoesTeamLimitSpawnTicketsForLocalAvatar() ? 1 : 0 );
}

//============================================================================

static void Script_Referee_RequestSpawnTicket( ScriptVM* vm )
{
	uint8 teamId = (uint8)vm->PopInt();

	vm->Push( g_referee.RequestSpawnTicket( teamId ) ? 1 : 0 );
}

//============================================================================

static void Script_Referee_RequestSpawnTicketForParticipant( ScriptVM* vm )
{
	uint8 participantId = (uint8)vm->PopInt();

	vm->Push( g_referee.RequestSpawnTicketForParticipant( participantId ) ? 1 : 0 );
}

//============================================================================

static void Script_Referee_RequestSpawnTicketForLocalAvatar( ScriptVM* vm )
{
	vm->Push( g_referee.RequestSpawnTicketForLocalAvatar() ? 1 : 0 );
}

//============================================================================

static void Script_Referee_GetLocalPlayerAgentId( ScriptVM* vm )
{
	vm->Push( g_referee.GetLocalPlayerAgentId() );
}

//============================================================================

static void Script_Referee_InstallTeamingModel( ScriptVM* vm )
{
	const char* name = vm->PopString();

	g_referee.InstallTeamingModel( name );
}

//============================================================================

static void Script_Referee_InstallScoringModel( ScriptVM* vm )
{
	const char* name = vm->PopString();

	g_referee.InstallScoringModel( name );
}

//============================================================================

static void Script_Referee_DoAllAgentsHaveAvatars( ScriptVM* vm )
{
	vm->Push( (int)g_referee.DoAllAgentsHaveAvatars() );
}

//============================================================================

static void Script_Referee_GetOverallMVP( ScriptVM* vm )
{
	vm->Push( g_referee.GetScoringModel().GetOverallMVP() );
}

//============================================================================

static void Script_Referee_GetTeamMVP( ScriptVM* vm )
{
	vm->Push( g_referee.GetScoringModel().GetTeamMVP( vm->PopInt() ) );
}

//============================================================================

static void Script_Referee_GetCurrentHighestScore( ScriptVM* vm )
{
	int score;
	g_referee.GetScoringModel().GetBestTeam( g_referee.GetScoringModel().TOTAL_SCORE, score );
	vm->Push( score );
}

//============================================================================

static void Script_Referee_GetCurrentHighestTeamTagCount( ScriptVM* vm )
{
	int tagCount;
	g_referee.GetScoringModel().GetBestTeam( g_referee.GetScoringModel().OBJECTIVES, tagCount );
	vm->Push( tagCount );
}

//============================================================================

static void Script_Referee_GetTeamTagCount( ScriptVM* vm )
{
	vm->Push( (int)g_referee.GetScoringModel().GetTeamCount( g_referee.GetScoringModel().OBJECTIVES, vm->PopInt() ) );
}

//============================================================================

static void Script_Referee_IsBlueTeamWinning( ScriptVM* vm )
{
	CScoringModel& scoring = g_referee.GetScoringModel();
	CTeamingModel& teaming = g_referee.GetTeamingModel();
	vm->Push( (int)(scoring.WINNING == scoring.GetBlueTeamStatus()) );
}

//============================================================================

static void Script_Referee_IsRedTeamWinning( ScriptVM* vm )
{
	CScoringModel& scoring = g_referee.GetScoringModel();
	CTeamingModel& teaming = g_referee.GetTeamingModel();
	vm->Push( (int)(scoring.WINNING == scoring.GetRedTeamStatus()) );
}

//============================================================================

static void Script_Referee_IsBlueTeamWinner( ScriptVM* vm )
{
	CScoringModel& scoring = g_referee.GetScoringModel();
	CTeamingModel& teaming = g_referee.GetTeamingModel();
	vm->Push( (int)(scoring.WON == scoring.GetBlueTeamStatus()) );
}

//============================================================================

static void Script_Referee_IsRedTeamWinner( ScriptVM* vm )
{
	CScoringModel& scoring = g_referee.GetScoringModel();
	CTeamingModel& teaming = g_referee.GetTeamingModel();
	vm->Push( (int)(scoring.WON == scoring.GetRedTeamStatus()) );
}

//============================================================================

static void Script_Referee_CheckTeamDown( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsTeamAllDead( vm->PopInt() ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsPlayerBlue( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsPlayerBlue( vm->PopInt() ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsPlayerRed( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsPlayerRed( vm->PopInt() ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsPlayerFriend( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsPlayerFriend( vm->PopInt() ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsPlayerEnemy( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsPlayerEnemy( vm->PopInt() ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsPlayerNeutral( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsPlayerNeutral( vm->PopInt() ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsAmbient( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsAmbient( vm->PopInt() ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsBlueAllAlive( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsBlueAllAlive( ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsBlueAllDead( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsBlueAllDead( ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsBlueAllPermanentlyDead( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsBlueAllPermanentlyDead( ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsRedAllAlive( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsRedAllAlive( ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsRedAllDead( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsRedAllDead( ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_IsRedAllPermanentlyDead( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().IsRedAllPermanentlyDead( ) ? 1 : 0 ) );
}

//============================================================================

static void Script_Referee_GetTeam( ScriptVM *vm )
{
	int i_id = vm->PopInt();
	vm->Push( g_referee.GetTeam( i_id ) );
}

//============================================================================

static void Script_Referee_GetRedPlayerCount( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().GetRedPlayerCount() ) );
}

//============================================================================

static void Script_Referee_GetBluePlayerCount( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().GetBluePlayerCount() ) );
}

//============================================================================

static void Script_Referee_GetRedAliveCount( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().GetRedAliveCount() ) );
}

//============================================================================

static void Script_Referee_GetBlueAliveCount( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.GetTeamingModel().GetBlueAliveCount() ) );
}

//============================================================================

static void Script_Referee_IsAvatarActive( ScriptVM* vm )
{
	vm->Push( ( int )( g_referee.IsAlive( g_players.GetParticipantId( 0 ) ) ) );
}

//============================================================================

static void Script_Referee_GetRoundCount( ScriptVM* vm )
{
	vm->Push( ValidSingleton( Referee ) ? (int) g_referee.GetRoundCount() : 0 );
}

//============================================================================

static void Script_TeamingModel_ValidTeams( ScriptVM* vm )
{
	vm->Push( ValidSingleton( Referee ) ? (int)g_referee.GetTeamingModel().ValidTeams() : 0 );
}

//============================================================================

static void Script_TeamingModel_AssignAlphaToRedTeam( ScriptVM* vm )
{
	g_referee.AssignAlphaToRedTeam();
}

//============================================================================

static void Script_TeamingModel_AssignAlphaToBlueTeam( ScriptVM* vm )
{
	g_referee.AssignAlphaToBlueTeam();
}

//============================================================================

static void Script_TeamingModel_AssignBetaToRedTeam( ScriptVM* vm )
{
	g_referee.AssignBetaToRedTeam();
}

//============================================================================

static void Script_TeamingModel_AssignBetaToBlueTeam( ScriptVM* vm )
{
	g_referee.AssignBetaToBlueTeam();
}

//============================================================================

static void Script_TeamingModel_GetPlayerProfileVariant( ScriptVM *vm )
{
	int i_id = vm->PopInt();
	vm->Push( (int)g_referee.GetTeamingModel().GetPlayerProfileVariant( i_id ) );
}

//============================================================================

static void Script_TeamingModel_GetTeamProfileVariant( ScriptVM *vm )
{
	int i_id = vm->PopInt();
	vm->Push( (int)g_referee.GetTeamingModel().GetTeamProfileVariant( i_id ) );
}

//============================================================================

static void Script_TeamingModel_GetTeamName( ScriptVM *vm )
{
	vm->Push( (char*)g_referee.GetTeamingModel().GetTeamName( vm->PopInt() ) );
}

//============================================================================

static void Script_TeamingModel_GetRedTeamName( ScriptVM *vm )
{
	vm->Push( (char*)g_referee.GetTeamingModel().GetRedTeamName() );
}

//============================================================================

static void Script_TeamingModel_GetBlueTeamName( ScriptVM *vm )
{
	vm->Push( (char*)g_referee.GetTeamingModel().GetBlueTeamName() );
}

//============================================================================

static void Script_PlayerSpawnMgr_SpawnAvatar( ScriptVM* vm )
{
	bool reArm = (vm->PopInt() != 0);
	bool force = (vm->PopInt() != 0);
//	if (ValidSingleton(PlayerSpawnMgr))
//		vm->Push( ( int )( g_PlayerSpawnMgr.SpawnAvatar( force, reArm ) ? 1 : 0 ) );
//	else
		vm->Push( 0 );
}

//============================================================================

static void Script_PlayerSpawnMgr_ControlSpawn( ScriptVM* vm )
{
	int participant = vm->PopInt();
	Vector3 pos = vm->PopVector3();
	if (ValidSingleton(PlayerSpawnMgr))
		g_PlayerSpawnMgr.ControlSpawn( pos, (uint8)participant );
}

//============================================================================

static void Script_PlayerSpawnMgr_FindRoleOfNearestSpawnPoint( ScriptVM* vm )
{
	Vector3 pos = vm->PopVector3();
	CTeamingModel::ERole role = CTeamingModel::INVALID_ROLE;
	if (ValidSingleton(PlayerSpawnMgr))
		role = g_PlayerSpawnMgr.FindRoleOfNearestSpawnPoint( pos );

	vm->Push( (int)role );
}

//============================================================================

static void Script_PlayerSpawnMgr_HasSpawnPoint( ScriptVM* vm )
{
	if (ValidSingleton(PlayerSpawnMgr))
		vm->Push( ( int ) ( g_PlayerSpawnMgr.HowManySpawnPoints( g_referee.GetLocalPlayerAgentId() ) ? 1 : 0 ) );
	else
		vm->Push( 0 );
}

//============================================================================

static void Script_PlayerSpawnMgr_BlueHasSpawnPoint( ScriptVM* vm )
{
	if (ValidSingleton(PlayerSpawnMgr))
		vm->Push( ( int ) ( g_PlayerSpawnMgr.HowManyRoleSpawnPoints( CTeamingModel::BLUE ) ? 1 : 0 ) );
	else
		vm->Push( 0 );
}

//============================================================================

static void Script_PlayerSpawnMgr_RedHasSpawnPoint( ScriptVM* vm )
{
	if (ValidSingleton(PlayerSpawnMgr))
		vm->Push( ( int ) ( g_PlayerSpawnMgr.HowManyRoleSpawnPoints( CTeamingModel::RED ) ? 1 : 0 ) );
	else
		vm->Push( 0 );
}

//============================================================================

static void Script_Session_GetCurrentTimeMS( ScriptVM* vm )
{
	vm->Push( ( int )g_session.GetSessionTimeMS() );
}

//============================================================================

static void Script_Players_GetActor( ScriptVM* vm )
{
	int iIndex = vm->PopInt();
	vm->Push( ( CActor* )g_players.GetPlayer( iIndex ) );
}

//============================================================================

static void Script_Players_GetActorHandle( ScriptVM* vm )
{
	int iIndex = vm->PopInt();
	vm->Push( g_players.GetPlayer( iIndex )->GetHandle() );
}

//============================================================================

static void Script_Players_GetParticipantId( ScriptVM* vm )
{
	int iIndex = vm->PopInt();
	vm->Push( ( int )g_players.GetParticipantId( iIndex ) );
}

//============================================================================

static void Script_Players_GetAvatar( ScriptVM* vm )
{
	vm->Push( (CActor*)g_players.GetPlayer( 0 ) );
}

//============================================================================

static void Script_Players_GetAvatarId( ScriptVM* vm )
{
	vm->Push( (int)g_players.GetParticipantId( 0 ) );
}

//============================================================================

static void Script_Players_ActivateControls( ScriptVM* vm )
{
	bool activate = vm->PopBool();
	g_players.ActivateControls( activate );
}

//============================================================================

static void Script_Players_GetPlayerDisplayName( ScriptVM* vm )
{
	int iPlayerId = vm->PopInt();
	CPlayerAgent* agent = g_referee.GetPlayerAgent( iPlayerId );
	if ( agent )
		 vm->Push( (char*) agent->GetDisplayName() );
		// vm->Push( (char*) agent->GetAccountName() );
	else
		vm->Push( "Invalid PlayerID" );
}

//============================================================================

static void Script_LevelInit_SetCharacter( ScriptVM* vm )
{
	const char* name = vm->PopString();
	g_playerProfile.SetAllCharacters( PlayerDB::CharacterIndexFromName( name ) );
}

//============================================================================

static void Script_LevelInit_SetWeapons( ScriptVM* vm )
{
	const char* weapons = vm->PopString();
	g_playerProfile.SetAllWeaponLoadouts( weapons );
}

//============================================================================

static void Script_LevelInit_SetTag( ScriptVM* vm )
{
	const char* tag = vm->PopString();
	g_tag.Init( tag );
}
//============================================================================
static void Script_LevelInit_SetVoiceset( ScriptVM* vm )
{
	const char *voiceset = vm->PopString();
	CActor *avatar = g_players.GetPlayer(0);
	if(avatar)
		avatar->Parm().Set("voiceset",voiceset);
}
//============================================================================

static void Script_Affliction_Stunned( ScriptVM* vm )
{
	CCompActor* actor		= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	int stunned = 0;
	Afflictions *afflict = (Afflictions *)actor->GetComponentByName( "Afflictions" );
	if( afflict )
		stunned = afflict->Afflicted( Afflictions::STUNNED );

	vm->Push( stunned );
}

//============================================================================

static void Script_Affliction_OnFire( ScriptVM* vm )
{
	CCompActor* actor		= (CCompActor*) vm->PopActor();
	ASSERT_PTR( actor );
	ASSERT( actor->IsA( CCompActor::ClassType() ) );

	int onfire = 0;
	Afflictions *afflict = (Afflictions *)actor->GetComponentByName( "Afflictions" );
	if( afflict )
		onfire = afflict->Afflicted( Afflictions::ONFIRE );

	vm->Push( onfire );
}

//============================================================================

static void Script_Misc_CatStrings( ScriptVM* vm )
{
	char *szB = vm->PopString();
	char *szA = vm->PopString();

	static char szResult[ 128 ];
	szResult[ 0 ] = '\0';

	strcpy( szResult, szA );
	strcat( szResult, szB );

	vm->Push( szResult );
}

//============================================================================

void Script_SetCameraFirstPersonLock( ScriptVM* vm )
{
	bool iBool = vm->PopBool();

	g_shooterCam.FirstPersonLock( iBool );
}

//============================================================================

static void Script_ChangeAvatarClothes( ScriptVM* vm )
{
	const char* clothesSet = vm->PopString();

	int index = PlayerDB::CharacterIndexFromName( clothesSet );
	ASSERT( index >= 0 );

	CActor* actor = g_players.GetPlayer( 0 );
	if (actor != NULL)
	{
		actor->Parm().SetFromString( PlayerDB::CharacterInitString( index ) );
		ActorCreator::InitGameSpecificGeometry( actor );
	}
}

//============================================================================

static void Script_ResetAvatarClothes( ScriptVM* vm )
{
	CActor* actor = g_players.GetPlayer( 0 );
	if (actor != NULL)
	{
		int index = g_playerProfile.GetCharacter( CPlayerProfile::COP );
		ASSERT( index >= 0 );
		ASSERT( g_playerProfile.GetCharacter( CPlayerProfile::GANG ) == index );	// SP gang & cop should be set the same.
		actor->Parm().SetFromString( PlayerDB::CharacterInitString( index ) );
		ActorCreator::InitGameSpecificGeometry( actor );
	}
}

//============================================================================

void Script_Hud_ShowTeamStatus( ScriptVM* vm )
{
	bool iBoolShow = vm->PopBool();

	// I changed the API so no longer can turn enable or disable
	//  respawn timers independantly, both are either on or off - Dan
	//	pAvatarHud->ShowRespawnTimer( iTeam, iBoolShow );

	GameHUD::SendMessage(GAMEHUD_SHOWTEAMSTATUS, CMessageDispatcher::IntToData(iBoolShow));
}
//============================================================================
void Script_Hud_SetRespawnTimer( ScriptVM* vm )
{
	int iSeconds = vm->PopInt();
	int iTeam = vm->PopInt();

	GameHUD_SetRespawnTimer msg = { iTeam, iSeconds };
	GameHUD::SendMessage(GAMEHUD_SETRESPAWNTIMER, &msg);
}
//============================================================================
void Script_Hud_SetTeamStatus( ScriptVM* vm )
{
	int iTotalPlayers = vm->PopInt();
	int iSeconds = vm->PopInt();
	int iTeam = vm->PopInt();

	GameHUD_SetTeamStatus msg = { iTeam, iSeconds, iTotalPlayers };
	GameHUD::SendMessage(GAMEHUD_SETTEAMSTATUS, &msg);
}
//============================================================================
void Script_TriggerBox_ActorCount( ScriptVM* vm )
{
	TriggerBox* box = (TriggerBox*) vm->PopActor();
	ASSERT( box );
	int count = box ? box->ActorCount() : 0;
	vm->Push( count );
}

//============================================================================

void Script_TriggerBox_GetActor( ScriptVM* vm )
{
	int index = vm->PopInt();
	TriggerBox* box = (TriggerBox*) vm->PopActor();
	ASSERT( box );
	CActor* actor = box ? box->GetActor( index ) : NULL;
	vm->Push( actor );
}

//============================================================================

static void Script_LaunchGhostCamera( ScriptVM* vm )
{
	CActor* actor = g_players.GetPlayer( 0 );
	ASSERT( actor );

	if( actor )
	{
		Camera &camera = g_cameraDirector.CurrentCamera();
		g_cameraDirector.SelectCamera( camera.GetGroup()->Name() , "Ghost" );
		g_ghostCam.setParticipantID( g_referee.GetLocalPlayerAgent()->Id() );
		g_ghostCam.SetFollow( actor );
	}
}

//============================================================================
static void Script_WallFall( ScriptVM* vm )
{
	CActor *actor = (CActor *)vm->GetOwner();
	bool fall = false;
	if( actor->IsA( CCompActor::ClassType() ) )
	{
		WallVault *wall = (WallVault *)(((CCompActor *)actor)->GetComponentByName( "WallVault" ));
		if( wall )
			fall = wall->FarSideFall();
	}
	vm->Push( fall );
}

//============================================================================
static void Script_AbortVault( ScriptVM* vm )
{
	CActor *actor = (CActor *)vm->GetOwner();
	bool fall = false;
	if( actor->IsA( CCompActor::ClassType() ) )
	{
		WallVault *wall = (WallVault *)(((CCompActor *)actor)->GetComponentByName( "WallVault" ));
		if( wall )
			wall->AbortVault();
	}
}

//============================================================================
static void Script_GameEnv_IsWeaponMaster( ScriptVM* vm )
{
	vm->Push( g_gameEnvironment.IsWeaponMaster() );
}

//============================================================================
//============================================================================
//============================================================================
//============================================================================
//============================================================================
//============================================================================

void TTL_RegisterScriptFuncs()
{
	CBipedAnimScriptIF::RegisterCallbacks();
	SCRIPT_REGISTER( GetWorldToScreen );
	SCRIPT_REGISTER( GetWeaponClass );
	SCRIPT_REGISTER( GetProjectileClass );
	SCRIPT_REGISTER( GetMunitionImpactType );
	SCRIPT_REGISTER( GetMunitionImpactDamage );
	SCRIPT_REGISTER( GetMunitionImpactShooter );
	SCRIPT_REGISTER( GetMunitionImpactIntersection );
	SCRIPT_REGISTER( GetMunitionImpactMaterial );
	SCRIPT_REGISTER( GetMunitionMaterial );
	SCRIPT_REGISTER( GetMunitionFromDir );
	SCRIPT_REGISTER( GetMunitionFired_WeaponID );
	SCRIPT_REGISTER( GetMunitionFired_WeaponHand );
	SCRIPT_REGISTER( CarBomb );
	SCRIPT_REGISTER( ExplodeProjectile );
	SCRIPT_REGISTER( FireProjectile );
	SCRIPT_REGISTER( FireProjectileAt );
	SCRIPT_REGISTER( Kill );
	SCRIPT_REGISTER( GetCollisionImpactIntersection );
	SCRIPT_REGISTER( GetCollisionImpactNormal );
	SCRIPT_REGISTER( GetCollisionSender );
	SCRIPT_REGISTER( Shatter );
	SCRIPT_REGISTER( JukeboxAddSource );
	SCRIPT_REGISTER( JukeboxRemoveSource );
	SCRIPT_REGISTER( JukeboxNextTrack );
	SCRIPT_REGISTER( Box_InsideMe );
	SCRIPT_REGISTER( Pickup_SetAttachableToActor );
	SCRIPT_REGISTER( Pickup_SetDropOnButton );
	SCRIPT_REGISTER( Pickup_SetDropOnDeath );
	SCRIPT_REGISTER( Pickup_SetParticleEmitter );
	SCRIPT_REGISTER( Pickup_IsHome );
	SCRIPT_REGISTER( Pickup_IsDropped );
	SCRIPT_REGISTER( Pickup_IsHeld );
	SCRIPT_REGISTER( Pickup_IsCaptured );
	SCRIPT_REGISTER( Pickup_IsAcquired );
	SCRIPT_REGISTER( Pickup_GetHolder );
	SCRIPT_REGISTER( Pickup_GetDropper );
	SCRIPT_REGISTER( Pickup_GetTransitionTime );
	SCRIPT_REGISTER( TagFrame_AssumeRoleOfClosestSpawn );
	SCRIPT_REGISTER( TagFrame_CanInteract );
	SCRIPT_REGISTER( TagFrame_Reset );
	SCRIPT_REGISTER( TagFrame_AnyBlueTags );
	SCRIPT_REGISTER( TagFrame_AnyRedTags );
	SCRIPT_REGISTER( ObjectiveMgr_Add );
	SCRIPT_REGISTER( ObjectiveMgr_GetFirstDropped );
	SCRIPT_REGISTER( ObjectiveMgr_AllCaptured );
	SCRIPT_REGISTER( Path_SetAtRandomPoint );
	SCRIPT_REGISTER( Participant_GetActor );
	SCRIPT_REGISTER( Participant_GetId );
	SCRIPT_REGISTER( Participant_GetTeam );
	SCRIPT_REGISTER( Participant_GetRole );
	SCRIPT_REGISTER( Participant_GetSpawnTicket );
	SCRIPT_REGISTER( DoDamage );
	SCRIPT_REGISTER( AddHealth );
	SCRIPT_REGISTER( GetHealth );
	SCRIPT_REGISTER( GetMaxHealth );
//	SCRIPT_REGISTER( Kill );
	SCRIPT_REGISTER( ConfigMgr_TestOptionValue );
	SCRIPT_REGISTER( ConfigMgr_GetCurrentLevelName );
	SCRIPT_REGISTER( ConfigMgr_GetChoiceAttributeInt );
	SCRIPT_REGISTER( ConfigMgr_GetChoiceAttributeString );
	SCRIPT_REGISTER( ActorCreateFromParametersWithPrefix );
	SCRIPT_REGISTER( ActorCreateFromInitString );
	SCRIPT_REGISTER( UpdatePain );
	SCRIPT_REGISTER( ShowTextMessage );
	SCRIPT_REGISTER( ShowHud );
	SCRIPT_REGISTER( HideHud );
	SCRIPT_REGISTER( SetupCheckpoint );
	SCRIPT_REGISTER( ResetAllCheckpointActors );
	SCRIPT_REGISTER( IsSinglePlayer );
	SCRIPT_REGISTER( IsMultiPlayer );
	SCRIPT_REGISTER( PlayCutScene );
	SCRIPT_REGISTER( GetWeaponDisplayName );
	SCRIPT_REGISTER( GetWeaponModelName );
	SCRIPT_REGISTER( GetWeaponSlot );
	SCRIPT_REGISTER( FindWeaponSlot );
	SCRIPT_REGISTER( AddWeapon );
	SCRIPT_REGISTER( RemoveWeapon );
	SCRIPT_REGISTER( GetWeaponInSlot );
	SCRIPT_REGISTER( RemoveWeaponInSlot );
	SCRIPT_REGISTER( GetSlotAmmo );
	SCRIPT_REGISTER( SlotNeedsAmmo );
	SCRIPT_REGISTER( AddSlotAmmo );
	SCRIPT_REGISTER( GetSlotAmmoCode );
	SCRIPT_REGISTER( ApplySlotAmmoCode );
	SCRIPT_REGISTER( GetSlotSplitAmmo );
	SCRIPT_REGISTER( SetSlotSplitAmmo );
	SCRIPT_REGISTER( WeaponAmmoCompatible );
	SCRIPT_REGISTER( TakeLoadoutSnapshot );
	SCRIPT_REGISTER( FreeLoadoutSnapshot );
	SCRIPT_REGISTER( ApplyLoadoutSnapshot );
	SCRIPT_REGISTER( GetSelectedWeaponSlot );
	SCRIPT_REGISTER( SetSelectedWeaponSlot );
	SCRIPT_REGISTER( LevelInit_SetCharacter );
	SCRIPT_REGISTER( LevelInit_SetWeapons );
	SCRIPT_REGISTER( LevelInit_SetTag );
	SCRIPT_REGISTER( LevelInit_SetVoiceset );
	SCRIPT_REGISTER( Referee_IsSetupPhase );
	SCRIPT_REGISTER( Referee_IsReadyPhase );
	SCRIPT_REGISTER( Referee_IsPopulatingPhase );
	SCRIPT_REGISTER( Referee_IsPlayingPhase );
	SCRIPT_REGISTER( Referee_IsReportingPhase );
	SCRIPT_REGISTER( Referee_SetRoundsInMatch );
	SCRIPT_REGISTER( Referee_PreventSetupPhaseExit );
	SCRIPT_REGISTER( Referee_PreventPopulatingPhaseExit );
	SCRIPT_REGISTER( Referee_PreventPlayingPhaseExit );
	SCRIPT_REGISTER( Referee_PreventReportingPhaseExit );
	SCRIPT_REGISTER( Referee_AllowSetupPhaseExit );
	SCRIPT_REGISTER( Referee_AllowPopulatingPhaseExit );
	SCRIPT_REGISTER( Referee_AllowPlayingPhaseExit );
	SCRIPT_REGISTER( Referee_AllowReportingPhaseExit );
	SCRIPT_REGISTER( Referee_CommandSetupPhaseExit );
	SCRIPT_REGISTER( Referee_CommandPopulatingPhaseExit );
	SCRIPT_REGISTER( Referee_CommandPlayingPhaseExit );
	SCRIPT_REGISTER( Referee_CommandReportingPhaseExit );
	SCRIPT_REGISTER( Referee_SetSetupPhaseEndTime );
	SCRIPT_REGISTER( Referee_SetPopulatingPhaseEndTime );
	SCRIPT_REGISTER( Referee_SetPlayingPhaseEndTime );
	SCRIPT_REGISTER( Referee_SetReportingPhaseEndTime );
	SCRIPT_REGISTER( Referee_SetSetupPhaseLifetime );
	SCRIPT_REGISTER( Referee_SetPopulatingPhaseLifetime );
	SCRIPT_REGISTER( Referee_SetPlayingPhaseLifetime );
	SCRIPT_REGISTER( Referee_SetReportingPhaseLifetime );
	SCRIPT_REGISTER( Referee_SetRemainingPhaseTime );
	SCRIPT_REGISTER( Referee_GetElapsedPhaseTime );
	SCRIPT_REGISTER( Referee_GetRemainingPhaseTime );
	SCRIPT_REGISTER( Referee_GetCurrentPhaseStartTime );
	SCRIPT_REGISTER( Referee_IsTimedPhase );
	SCRIPT_REGISTER( Referee_GetNumStartPlayers );
	SCRIPT_REGISTER( Referee_GetNumPlayers );
	SCRIPT_REGISTER( Referee_GetNumNonPlayers );
	SCRIPT_REGISTER( Referee_GetNumParticipants );
	SCRIPT_REGISTER( Referee_SetUnlimitedSpawnTickets );
	SCRIPT_REGISTER( Referee_SetUnlimitedSpawnTicketsForAll );
	SCRIPT_REGISTER( Referee_SetNumSpawnTickets );
	SCRIPT_REGISTER( Referee_SetNumSpawnTicketsForAll );
	SCRIPT_REGISTER( Referee_DoesTeamLimitSpawnTickets );
	SCRIPT_REGISTER( Referee_DoesTeamLimitSpawnTicketsForParticipant );
	SCRIPT_REGISTER( Referee_DoesTeamLimitSpawnTicketsForLocalAvatar );
	SCRIPT_REGISTER( Referee_RequestSpawnTicket );
	SCRIPT_REGISTER( Referee_RequestSpawnTicketForParticipant );
	SCRIPT_REGISTER( Referee_RequestSpawnTicketForLocalAvatar );
	SCRIPT_REGISTER( Referee_GetLocalPlayerAgentId );
	SCRIPT_REGISTER( Referee_InstallTeamingModel );
	SCRIPT_REGISTER( Referee_InstallScoringModel );
	SCRIPT_REGISTER( Referee_DoAllAgentsHaveAvatars );
	SCRIPT_REGISTER( Referee_GetOverallMVP );
	SCRIPT_REGISTER( Referee_GetTeamMVP );
	SCRIPT_REGISTER( Referee_GetCurrentHighestScore );
	SCRIPT_REGISTER( Referee_GetCurrentHighestTeamTagCount );
	SCRIPT_REGISTER( Referee_GetTeamTagCount );
	SCRIPT_REGISTER( Referee_IsBlueTeamWinning );
	SCRIPT_REGISTER( Referee_IsRedTeamWinning );
	SCRIPT_REGISTER( Referee_IsBlueTeamWinner );
	SCRIPT_REGISTER( Referee_IsRedTeamWinner );
	SCRIPT_REGISTER( Referee_CheckTeamDown );
	SCRIPT_REGISTER( Referee_IsPlayerBlue );
	SCRIPT_REGISTER( Referee_IsPlayerRed );
	SCRIPT_REGISTER( Referee_IsPlayerFriend );
	SCRIPT_REGISTER( Referee_IsPlayerEnemy );
	SCRIPT_REGISTER( Referee_IsPlayerNeutral );
	SCRIPT_REGISTER( Referee_IsAmbient );
	SCRIPT_REGISTER( Referee_IsBlueAllAlive );
	SCRIPT_REGISTER( Referee_IsBlueAllDead );
	SCRIPT_REGISTER( Referee_IsBlueAllPermanentlyDead );
	SCRIPT_REGISTER( Referee_IsRedAllAlive );
	SCRIPT_REGISTER( Referee_IsRedAllDead );
	SCRIPT_REGISTER( Referee_IsRedAllPermanentlyDead );
	SCRIPT_REGISTER( Referee_GetRedPlayerCount );
	SCRIPT_REGISTER( Referee_GetBluePlayerCount );
	SCRIPT_REGISTER( Referee_GetRedAliveCount );
	SCRIPT_REGISTER( Referee_GetBlueAliveCount );
	SCRIPT_REGISTER( Referee_GetTeam );
	SCRIPT_REGISTER( Referee_IsAvatarActive );
	SCRIPT_REGISTER( Referee_GetRoundCount );
	SCRIPT_REGISTER( TeamingModel_ValidTeams );
	SCRIPT_REGISTER( TeamingModel_AssignAlphaToRedTeam );
	SCRIPT_REGISTER( TeamingModel_AssignAlphaToBlueTeam );
	SCRIPT_REGISTER( TeamingModel_AssignBetaToRedTeam );
	SCRIPT_REGISTER( TeamingModel_AssignBetaToBlueTeam );
	SCRIPT_REGISTER( TeamingModel_GetPlayerProfileVariant );
	SCRIPT_REGISTER( TeamingModel_GetTeamProfileVariant );
	SCRIPT_REGISTER( TeamingModel_GetTeamName );
	SCRIPT_REGISTER( TeamingModel_GetRedTeamName );
	SCRIPT_REGISTER( TeamingModel_GetBlueTeamName );
	SCRIPT_REGISTER( PlayerSpawnMgr_SpawnAvatar );
	SCRIPT_REGISTER( PlayerSpawnMgr_ControlSpawn );
	SCRIPT_REGISTER( PlayerSpawnMgr_FindRoleOfNearestSpawnPoint );
	SCRIPT_REGISTER( PlayerSpawnMgr_HasSpawnPoint );
	SCRIPT_REGISTER( PlayerSpawnMgr_BlueHasSpawnPoint );
	SCRIPT_REGISTER( PlayerSpawnMgr_RedHasSpawnPoint );
	SCRIPT_REGISTER( Session_GetCurrentTimeMS );
	SCRIPT_REGISTER( Players_GetActor );
	SCRIPT_REGISTER( Players_GetActorHandle );
	SCRIPT_REGISTER( Players_GetParticipantId );
	SCRIPT_REGISTER( Players_GetAvatar );
	SCRIPT_REGISTER( Players_GetAvatarId );
	SCRIPT_REGISTER( Players_ActivateControls );
	SCRIPT_REGISTER( Players_GetPlayerDisplayName );
	SCRIPT_REGISTER( Affliction_Stunned );
	SCRIPT_REGISTER( Affliction_OnFire );
	SCRIPT_REGISTER( Misc_CatStrings );
	SCRIPT_REGISTER( SetCameraFirstPersonLock );
	SCRIPT_REGISTER( ChangeAvatarClothes );
	SCRIPT_REGISTER( ResetAvatarClothes );
	SCRIPT_REGISTER( Hud_ShowTeamStatus );
	SCRIPT_REGISTER( Hud_SetRespawnTimer );
	SCRIPT_REGISTER( Hud_SetTeamStatus );
	SCRIPT_REGISTER( TriggerBox_ActorCount );
	SCRIPT_REGISTER( TriggerBox_GetActor );
	SCRIPT_REGISTER( GameEnv_IsWeaponMaster );

	SCRIPT_REGISTER( LaunchGhostCamera );
	SCRIPT_REGISTER( WallFall );
	SCRIPT_REGISTER( AbortVault );
}

//============================================================================
