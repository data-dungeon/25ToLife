/*
**
**  File:   Combatant.cpp
**  Date:   January 6, 2004
**  By:     Bryant Collard
**  Desc:   A temporary class that prepares for that design and implementation
**          of a more general dynamically configurable actor.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/ActorsTTL/Combatant.cpp $
**  $Revision: #137 $
**  $DateTime: 2005/08/23 17:38:35 $
**    $Author: Bryant_Collard $
**
*/

#include "TTLPCH.h"

#ifdef NETZ
#include "NetZTTL/DOCombatant.h"
#endif

#include "components/KineControl/BipedModel.h"
#include "components/KineControl/ViewDriver.h"
#include "Components/weapons/weaplib.h"
#include "Components/KineControl.h"
#include "Components/GroundHeight.h"
#include "weapons/firearm.h"
#include "weapons/lobbed.h"
#include "weapons/supersonic.h"
#include "EngineHelper/sfx.h"
#include "avatar/pstatMP.h"
#include "avatar/pstatSP.h"
#include "main/TTL.h"
#include "components/painstats.h"
#include "components/affliction.h"
#include "components/onfire.h"
#include "components/footfall.h"
#include "components/WaterWake.h"
#include "weapons/armoryview.h"

#include "data_h_files/players/players_s.h"

#include "Math/sfilter.h"
#include "stage/ActorCreator.h"
#include "CollideHelper/ObbBonesNode.h"
#include "CollideHelper/CapsuleBonesNode.h"
#include "weapons/armory.h"
#include "components/quicktag.h"
#include "Components/ComponentCreator.h"
#include "Script/ScriptVM.h"

#define COLLISION_CAPSULE

// Define creators and add them to the creator list.
CUSTOM_ACTOR_CREATOR(CAICombatant, combatant)::Create(void)
{
	CCombatant* combatant = new CCombatant;

	combatant->Category() |= AI_CATEGORY;
	combatant->SetIsNpc(true);
	if (g_ttl->IsMultiPlayer())
		combatant->AttachComponent( "PlayerStatsMP" );
	else
		combatant->AttachComponent( "PlayerStatsSP" );
	combatant->AttachComponent( "Biped" );
	combatant->AttachComponent( "GroundFollower" );
	combatant->InitializeComponents();

	return(combatant);
}

//CUSTOM_ACTOR_CREATOR(CPlayerCombatant, player)::Create(void)
//{
//	CCombatant *combatant = new CCombatant;
//	ASSERT_PTR(combatant);
//
//	combatant->SetIsNpc(false);
//	combatant->AttachComponent( "Biped" );
//	combatant->AttachComponent( "GroundFollower" );
//	combatant->AttachComponent( "BipedPhysicalController" );
//	combatant->AttachComponent( "PlayerStats" );
//	combatant->AttachComponent( "InteractionMonitor" );
//
//	combatant->InitializeComponents();
//
//	return(combatant);
//}

// Names of bones to which to attach collision objects.
static const char *childBoneNames[] =
{
	"head",
	"spine",
	"rightarm",
	"rightforearm",
	"rightupleg",
	"rightleg",
	"leftarm",
	"leftforearm",
	"leftupleg",
	"leftleg",
	"rightshoulder",
	"leftshoulder",
	""
};

// ID changes made by console or watch.
enum
{
	PARAM_CHANGE_SIZE,
	PARAM_CHANGE_POS,
	PARAM_CHANGE_SURFACE_NAME
};

#define COMBATANT_SURFACE_NAME_LENGTH  32

STANDARD_COMPONENT_CREATOR(CCombatantFocus, CombatantFocus, COMPONENT_STATE_OVERRIDE_PRIORITY);
CCombatantFocus::CCombatantFocus( CCompActor &i_actor ) : CActorComponent( i_actor )
{
	focusOffset.Set( 0.0f, 1.8f, 0.0f );
}

void CCombatantFocus::RunAnimation( void )
{
	Vector3 target;
	CCombatant *owner = (CCombatant *)&actor;

	// Set the focus target if dead.
	if (owner->GetProperty("health")->GetFloat() <= 0.0f)
	{
		Vector3 pos;
		boneGetPositionFromBone(owner->hipsBone, &pos);
		pos = pos * owner->GetBodyToWorld();
		target = pos;
	}
	else if( owner->AnimAction() == ACTION_ROAM_VAULT || ACTION_TYPE_MASK( owner->AnimAction() ) == ACTION_ROAM_LADDER )
	{
		Vector3 pos;
		boneGetPositionFromBone(owner->headBone, &pos);
		pos = pos * owner->GetBodyToWorld();
		target = pos;
	}
	else
	{
		float y = 1.8f;

		BipedModel *model = (BipedModel *)(owner->GetComponentByName( "BipedModel" ));
		if( model && model->Crouching() )
			y = 1.3f;

		target.Set( 0.0f, y, 0.0f );
		focusOffset = FOFilterImplicit( focusOffset, target, g_timer.GetFrameSec(), 0.25 );
		target = focusOffset;
	}

	owner->d_focusInWorld = target + owner->d_bodyInWorld;
}

void CCombatant::CommonConstruct( void )
{
	// updateLevel -
	// 0 = no terrain collision, one ground vector.
	// 1 = terrain collision, 3 ground vectors, 40fps update rate
	AddProperty( "updateLevel", Property_int );
	SetProperty( "updateLevel", 0 );
	AddProperty( "headPosition", Property_Vector3 );
	SetProperty( "headPosition", Vector3( 0.0f, 0.0f, 0.0f ) );

	d_collisionParam = NULL;
}

CCombatant::CCombatant() :
	CCompActor(s_combatantClass)
{
	CommonConstruct();
	isNpc = false;
}

CCombatant::CCombatant(const CEntityType &i_type) : CCompActor(i_type)
{
	CommonConstruct();
}

CCombatant::~CCombatant()
{
}

CCombatant* CCombatant::FromHandle(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if ((actor != NULL) && actor->IsA(s_combatantClass))
		return((CCombatant*)actor);
	return(NULL);
}

bool CCombatant::Initialize(void)
{
	// Get the child obb parameters.
	d_collisionParam =
			CParam<CCombatantCollisionParam>::GetParam("HitBox", "var");
	ASSERT_PTR(d_collisionParam);
	d_collisionChangeHandler.Initialize("CombatantCollisionChange",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA,
			d_messageOwner, this);
	d_iKilledHostageHandler.Initialize("IKilledHostage", 
		MESSAGE_HANDLE_IF_MASTER|MESSAGE_ROUTE_TO_MASTER, d_messageOwner, this);

	CCapsuleBonesNode *node = new CCapsuleBonesNode;
	SetCollisionNode( node, COMPLEX, true );
	node->SetScaleUsed(false);
	node->SetFitToAttached(true);
	node->SetFixedOffset( Vector3( 0.0f, 0.0f, 0.15f ) );
	node->SetupAttached(childBoneNames);
	node->SetSurfaceName("humanBody");
	node->SetSizeInBones((Vector3Packed*)d_collisionParam->d_size,
		sizeof(Vector3Packed));
	node->SetObbInBones((Vector3Packed*)d_collisionParam->d_pos,
		sizeof(Vector3Packed));
	node->SetSurfaceNames(d_collisionParam->d_surfaceName,
		COMBATANT_SURFACE_NAME_LENGTH);
	// set up a default capsule
	Vector3 origin, end;
	origin.Set( 0.0f, 0.48f, 0.0f );
	end.Set( 0.0f, 2.0f-0.48f, 0.0f );
	node->SetExtents( origin, end, 0.48f );
	((CCapsuleBonesNode*)d_collisionNode)->SetAxisExtents( 0.25f, MAXFLOAT );
	((CCapsuleBonesNode*)d_collisionNode)->SetRadiusExtents( 0.48f, 0.48f );

	d_simpleNode.Set(node->GetCollisionObject());
	d_capsuleWrapperNode.Set(node, &d_bodyInWorld);

	d_resetWeaponsHandler.Initialize("ResetWeapons", MESSAGE_HANDLE_IF_MASTER | 
		MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_DUPLICAS, d_messageOwner, this);

	if (!CCompActor::Initialize())
		return(false);

	d_accelerationInWorld.Clear();

	d_category |= NO_SUPPORT_CATEGORY;

//	SetSurfaceType( "Default" );

	// gotta have animations.
	char *role = Parm().GetString( "role" );
	bool isGang = ( role && strcmp( role, "gang" ) == 0 );
	bool isCop = ( role && strcmp( role, "cop" ) == 0 );
	if( isGang )
		LoadMotion( "avatars/gangster.mdb", 0, 0 );
	else if( isCop )
		LoadMotion( "avatars/police.mdb", 0, 0 );
	else
	{
		LoadMotion( "avatars/npc.mdb", 0, 0 );
		LoadMotion( "avatars/police.mdb", 0, 2);  //need to pick motion set to load based on weapon they will have?
	}
	LoadMotion( "avatars/common.mdb", 0, 1 );
	if (!ValidSingleton(CSession) || !g_session.IsNetworked())
		LoadMotion("avatars/intros.mdb", 0, 3);

	LoadAnimScript( "avatars/avatar.sx", ANIM1_SCRIPT );
	LoadAnimScript( "avatars/avatar2.sx", ANIM2_SCRIPT );
	LoadAnimScript( "avatars/avatar3.sx", ANIM3_SCRIPT );
	LoadAnimScript( "avatars/avatar-eye.sx", ANIM4_SCRIPT );

	AttachComponent( "BipedModel" );
	AttachComponent( "WeaponInventory" );
	AttachComponent( "Loadout" );
	AttachComponent( "Say" );
	AttachComponent( "Effects" );
	AttachComponent( "Participant" );
	AttachComponent( "PainStats" );
	AttachComponent( "Afflictions" );
	AttachComponent( "OnFire" );
	AttachComponent( "Shadow" );
	AttachComponent( "Footfall" );
	AttachComponent( "WaterWake" );
	AttachComponent( "CombatantFocus" );
	AttachComponent( "GroundHeight" );

	// register the animation interface
	BipedModel *model = (BipedModel *)GetComponentByName( "BipedModel" );
	if( model )
	{
		model->RegisterInterface( Script(ANIM1_SCRIPT) );
		model->RegisterInterface( Script(ANIM2_SCRIPT) );
		model->RegisterInterface( Script(ANIM3_SCRIPT) );
	}

	if (!GetIsNpc())
	{
#ifdef IT_GOT_BETTER
      // Someday this will be much, much better!
		const char *gangTaunt[] = { "frta_001", "frta_002", "frta_010", "frta_011", "frta_015", "frta_016", "frta_020",
											"frta_021", "frta_030", "frta_031", "frta_040", "frta_041", "frta_050", "frta_051",
											"frta_060", "frta_061", "frta_070", "frta_071", "frta_072", "frta_080", "frta_081",
											"frta_090", "frta_095", "frta_100", "frta_120", "frta_130", "frta_140", "frta_150",
											"frta_155", "frta_160", "frta_165", "frta_170", "frta_175", "frta_180", "frta_181",
											"frta_182", "frta_183", "frta_184", "frta_188", "frta_190", NULL };
		const char *copTaunt[] = { "WIAP_031", "WIAP_032", "WIIB7", "WIIB8", "WIIB9", "WIIB_002", "WIIB_003", 
											"WIIB_005", "WIIB_006", "WISEE1", "WISEE2", "wita_031", NULL };

		const char **taunt;
		if (isCop)
			taunt = copTaunt;
		else
			taunt = gangTaunt;
		int onTaunt = 0;
		ParameterSet &paramSet = Parm();
		while (taunt[onTaunt])
		{
			char keyValue[64];
			sprintf(keyValue, "taunt%d=%s", onTaunt, taunt[onTaunt]);
			paramSet.SetFromString(keyValue);
			++onTaunt;
		}
#endif
		g_messageDispatcher.SendMessageToAll("SetAvatarVoiceset",NULL,INVALID_OBJECT_HANDLE);

		AttachComponent( "TTLWeaponStats" );
		//AttachComponent( "Taunt" );
		AttachComponent( "QuickTag" );
	}

	// create the water wake emitters
	CWaterWake* waterWake = (CWaterWake*)GetComponentByName("WaterWake");
	if (waterWake != NULL)
		waterWake->Create("WaterWake");

	InitializeComponents();

	ResetWeaponInventory(true);

	Reset();

	// all combatants should receive point lights
	for ( int i = 0 ; i < MAX_ACTOR_INSTANCES ; i++ )
		if ( Instance( i ) )
			Instance( i )->SetDynamicLightFlags( INSTANCE_LIGHTING_ALL );

	hipsBone = GetBone("hips1");
	headBone = GetBone("head");

	SetProperty( "headPosition", GetStartPosition( headBone->boneID ) * d_bodyToWorld + d_bodyInWorld );

	return(true);
}

void CCombatant::PlaceOnGround( void )
{
	CLineQuery groundCollision;

	CLineEntity entity;

	Vector3 origin, end;
	origin = GetBodyInWorld() + Vector3( 0.0f, 1.5f, 0.0f );
	end = GetBodyInWorld() + Vector3( 0.0f, -5.0f, 0.0f );
	g_collisionSystem.ResetStack();
	entity.SetPositiveExclude( AVATAR_IGNORE_CATEGORIES );
	entity.SetIgnoreEntity( this );
	entity.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	entity.Line().SetOriginEnd( origin, end );

	if( groundCollision.Test( entity ) )
	{
		GetBodyInWorld() = groundCollision.intersection;
		SetProperty( "onGround", true );
	}
	else
		SetProperty( "onGround", false );
}

void CCombatant::Reset()
{
	// Activate the model.
	BipedModel *model = (BipedModel *)GetComponentByName( "BipedModel" );
	if( model )
		model->Activate();

	// Activate first kinematic controller...
	KinematicController *controller = (KinematicController *)GetDefaultComponentInClass( "kinematicController" );
	if( controller )
		controller->Activate();

	// and the driver.
	CActorComponent *driver = GetDefaultComponentInClass( "KinematicDriver" );
	if( driver )
		driver->Activate();
}

CCombatantCollisionParam::CCombatantCollisionParam()
{
	d_size = NULL;
	d_pos = NULL;
	d_surfaceName = NULL;
}

CCombatantCollisionParam::~CCombatantCollisionParam()
{
	delete[] d_size;
	delete[] d_pos;
	delete[] d_surfaceName;
}

void CParam<CCombatantCollisionParam>::RegisterVariables(void)
{
	int numBones = 0;
	while((childBoneNames[numBones] != NULL) &&
			(childBoneNames[numBones][0] != '\0'))
		numBones++;

	if (numBones == 0)
		return;
	ASSERT((d_data.d_size == NULL) && (d_data.d_pos == NULL));
	d_data.d_size = new Vector3Packed[numBones];
	d_data.d_pos = new Vector3Packed[numBones];
	d_data.d_surfaceName = new char[numBones * COMBATANT_SURFACE_NAME_LENGTH];

	char title[64];
	for (int i = 0; i < numBones; i++)
	{
		sprintf(title, "%s_Size", childBoneNames[i]);
		RegisterVariable(d_data.d_size + i, title, 0.5f, 0.5f, 0.5f,
				0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, (void*)PARAM_CHANGE_SIZE);

		sprintf(title, "%s_Pos", childBoneNames[i]);
		RegisterVariable(d_data.d_pos + i, title, 0.0f, 0.0f, 0.0f,
				-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, (void*)PARAM_CHANGE_POS);

		sprintf(title, "%s_Surface", childBoneNames[i]);
		RegisterVariable(d_data.d_surfaceName + i * COMBATANT_SURFACE_NAME_LENGTH,
				title, "humanBody", COMBATANT_SURFACE_NAME_LENGTH,
				(void*)PARAM_CHANGE_SURFACE_NAME);
	}
}

void CParam<CCombatantCollisionParam>::CParamMessageHandler::HandleMessage(
		void* i_data, ActorHandle i_sender, bool i_posted)
{
	g_messageDispatcher.SendMessageToAll("CombatantCollisionChange", i_data,
			i_sender);
}

void CCombatant::CCollisionChangeHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	switch((uint)i_data)
	{
		case PARAM_CHANGE_SIZE:
			#ifdef COLLISION_CAPSULE
			#else
				((CObbBonesNode*)d_ref->d_collisionNode)->SetSizeInBones(
						(Vector3Packed*)d_ref->d_collisionParam->d_size, sizeof(Vector3Packed));
			#endif
			break;
		case PARAM_CHANGE_POS:
			#ifdef COLLISION_CAPSULE
			#else
				((CObbBonesNode*)d_ref->d_collisionNode)->SetObbInBones(
						(Vector3Packed*)d_ref->d_collisionParam->d_pos, sizeof(Vector3Packed));
			#endif
			break;
		case PARAM_CHANGE_SURFACE_NAME:
			#ifdef COLLISION_CAPSULE
			#else
				((CObbBonesNode*)d_ref->d_collisionNode)->SetSurfaceNames(
						d_ref->d_collisionParam->d_surfaceName,
						COMBATANT_SURFACE_NAME_LENGTH);
			#endif
			break;
	}
}

void CCombatant::CResetWeapons::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	FromHandle(i_sender)->ResetWeaponInventory();
}

void CCombatant::EndUpdate(float i_deltaSec)
{
	CCompActor::EndUpdate( i_deltaSec );
}

void CCombatant::RunAnimation( bool i_forceUpdate )
{
	CCompActor::RunAnimation( i_forceUpdate );

	SetProperty( "headPosition", GetStartPosition( headBone->boneID ) * d_bodyToWorld + d_bodyInWorld );
}

void CCombatant::UpdateCapsuleSize( void )
{
#ifdef COLLISION_CAPSULE
	float minAxis = -MAXFLOAT, maxAxis = MAXFLOAT;
	float minRadius = 0.05f, maxRadius = MAXFLOAT;
	bool fixedPoint = true;
	CCapsuleBonesNode *node = ((CCapsuleBonesNode*)d_collisionNode);

	if( node == NULL )
		return;

	bool noCap = false;
	if( AnimAction() == ACTION_ROAM_DIE )
		noCap = true;


	// I couldn't get the slave interaction to work, so just let both of their capsules
	// roam free, so that the attacker's will mostly enclose the hostage.
	Property *prop = QueryProperty( "hostage" );
	if( prop && ((ActorHandle)prop->GetHandle()).Valid() )
		noCap = true;

	// don't cap the hostage's capsule
	prop = QueryProperty( "hostageTaker" );
	if( prop && ((ActorHandle)prop->GetHandle()).Valid() )
		noCap = true;


	if( noCap )
	{
		minAxis = 0.05f;
		fixedPoint = false;
	}
	else if( AnimAction() == ACTION_ROAM_VAULT || ACTION_TYPE_MASK( AnimAction() ) == ACTION_ROAM_LADDER )
	{
		fixedPoint = false;
		maxRadius = 0.48f;
	}
	else if( AnimAction() == ACTION_ROAM_PEEK )
	{
		minAxis = 0.25f;
		maxRadius = 0.48f;
		fixedPoint = true;
		// how far into the peek anim are we?
		ts_MotnInf *inf = GetMotnInf();
		float ratio = inf->currentTime / CMotion__GetAnimTime( inf->pMotionHeader );
		if( ratio > 1.0f )
			ratio = 1.0f;

		// the only ways to know which way we're peeking is to check the vars in the anim script,
		// but "peekleft" and "peekright" are set to zero while un-peeking, so that we don't get
		// stuck in peek state.  Unfortunately, this means that we need to engage in a bit of
		// tomfoolery here, and rely on the current offset to determine which direction we're really going
		if( Script( MOTION1 )->GetInt( "peekright" ) )
			ratio = -ratio;
		else if( !Script( MOTION1 )->GetInt( "peekleft" ) )
		{
			if( node->GetFixedOffset().x() < 0.0f )
				ratio = -ratio;
		}

		((CCapsuleBonesNode*)d_collisionNode)->SetFixedOffset( Vector3( 0.5f * ratio, 0.0f, 0.15f ) );
	}
	else
	{
		minAxis = 0.25f;
		maxRadius = 0.48f;
		((CCapsuleBonesNode*)d_collisionNode)->SetFixedOffset( Vector3( 0.0f, 0.0f, 0.15f ) );
	}


	((CCapsuleBonesNode*)d_collisionNode)->SetAxisExtents( minAxis, maxAxis );
	((CCapsuleBonesNode*)d_collisionNode)->SetRadiusExtents( minRadius, maxRadius );
	((CCapsuleBonesNode*)d_collisionNode)->UseFixedOffset( fixedPoint );

#else
	Vector3 &clampMin = ((CObbBonesNode*)d_collisionNode)->GetClampMin();
	Vector3 &clampMax = ((CObbBonesNode*)d_collisionNode)->GetClampMax();

	if( AnimAction() == ACTION_ROAM_DIE )
		clampMin.Y( 0.05f );
	else if( GetProperty( "onGround" )->GetBool() )
		clampMin.Y( 0.4f );
	else
		clampMin.Y( 0.3f );

	// cap the front/back and side extents, unless we're dying
	if( AnimAction() != ACTION_ROAM_DIE )
	{
		clampMin.X(-0.5f);
		clampMax.X(0.5f);
		clampMin.Z(-0.45f);
		clampMax.Z(0.6f);
	}
	else
	{
		clampMin.X(-MAXFLOAT);
		clampMax.X(MAXFLOAT);
		clampMin.Z(-MAXFLOAT);
		clampMax.Z(MAXFLOAT);
	}
#endif
}

void CCombatant::EndFrame( void )
{
	UpdateCapsuleSize();
	CCompActor::EndFrame();

	// set the ambient lighting color multipler from the ground if on screen
	float w;
	Vector2 screen;
	Camera &camera = g_cameraDirector.CurrentCamera();
	if (camera.ToScreen(GetFocusInWorld(), screen, w, Vector2(1.1f, 1.1f)))
	{
		CGroundHeight* comp = (CGroundHeight*)GetComponentByName("GroundHeight");
		ASSERT_PTR(comp);
		CGroundHeightIF::SCheckResults* results;
		comp->Get(0.5f, -1.0f, results);
		SetLightColor(results->d_color);
	}
}

Vector3& CCombatant::GetFocusInWorld(void)
{
	// if we're standing up, focus on a fixed point.  Don't track the head,
	// because doing so makes the head look like it's on a rail.  The camera
	// bounces around instead of the head.

	// If we are crouching, though, we need to give at least some consideration to
	// where the head is, since in some views it goes down far enough to go
	// offscreen.  In that case, figure the vertical distance from the feet to the head,
	// and go halfway between that distance and the normal standing up distance

	// To do this, track the y offset, and use the handy FOFilter func during our update

	return d_focusInWorld;
}

CCollisionNode* CCombatant::BeginCollisionTest(CStageEntity* i_entity,
		ESelect i_select)
{
	if (i_entity->Category().PositiveMatch(WATER_CATEGORY))
		return(&d_capsuleWrapperNode);
	if (i_select == USE_COMPLEX)
		return(d_collisionNode);
	if (IgnoreEntity(i_entity))
		return(NULL);
	return(&d_simpleNode);
}

uint CCombatant::CollisionDetect(CStageEntity* i_entity,
		CCollisionEvent* i_event, bool i_mateDisplaced)
{
	if (i_entity->Category().PositiveMatch(WATER_CATEGORY))
		return(EFFECT_NONE);
	return (CCompActor::CollisionDetect(i_entity, i_event, i_mateDisplaced));
}

void CCombatant::SyncActor( void )
{
	bool simple = true;
	if( GetProperty( "updateLevel" )->GetInt() == 1 )
		simple = false;

	//Property *prop = QueryProperty( "hostageTaker" );
	//if( prop && ((ActorHandle)prop->GetHandle()).Valid() )
	//	simple = false;
	Property *prop = QueryProperty( "stopPushing" );
	if( prop && prop->GetInt() > 0 )
		simple = false;

	prop = QueryProperty( "terrainCollide" );
	if( prop && prop->GetInt() > 0 )
		simple = false;

	if( simple )
	{
		SetPositiveExclude( TERRAIN_CATEGORY | STATIC_CATEGORY );
		SetMaxUpdateSec( 100.0f );
	}
	else
	{
		SetPositiveExclude( NO_CATEGORY );
		SetMaxUpdateSec( 1.0f / 20.0f );
	}
	d_focusInWorld = d_bodyInWorld + Vector3( 0.0f, 1.8f, 0.0f );

	UpdateCapsuleSize();
	if (d_collisionNode != NULL)
		d_collisionNode->SyncWithAnimation();

	CCompActor::SyncActor();
}

static void SelectBestWeapon(
WeaponInventory *inv)
{
	if (!inv)	
		return;
	
	// Select the best weapon
	TTLWeapon::Slot tryWeapon[4] = { TTLWeapon::PRIMARY, TTLWeapon::SECONDARY, TTLWeapon::LOBBED, TTLWeapon::MELEE };
	for (int t = 0; t < 4; t++)
	{
		inv->Select(tryWeapon[t]);
		Weapon *weapon = inv->GetSelected();
		if (weapon)
			break;
	}
}

void CCombatant::Respawn(void)
{
	ResetAllComponents();

	WeaponInventory *inv = (WeaponInventory *)GetComponentByName( "WeaponInventory" );
	if( inv )
		SelectBestWeapon(inv);	
}

void CCombatant::Placed( void )
{
	PlaceOnGround();
	StartAnimation( "IdleNoInterp", true, true );
	CCompActor::Placed();
}

void CCombatant::ResetWeaponInventory( bool legalize )
{
	WeaponInventory *inv = (WeaponInventory *)GetComponentByName( "WeaponInventory" );
	if( inv )
	{
		WeaponLoadout loadout(Parm());
		if (legalize && GetIsNpc() && !g_gameEnvironment.BonusLevel())	
		{
			// Make sure the loadout is legal!
			const char *role = Parm().GetString( "role" );
			const bool isCop = ( role && strcmp( role, "cop" ) == 0 );
			ArmoryView::PlayerType playerType;
			if (isCop)
				playerType = GetIsNpc() ? ArmoryView::AICop : ArmoryView::HumanCop;
			else
				playerType = GetIsNpc() ? ArmoryView::AIGangster : ArmoryView::HumanGangster;
			g_armoryView.Legalize(playerType, loadout);
		}
		loadout.Build(*inv, true);
		SelectBestWeapon(inv);
	}
}

CNetActor* CCombatant::CreateNetActor(void)
{
#ifdef NETZ
	return((CNetActor*)DOCombatant::Create());
#else
	return(NULL);
#endif
}

CInterface *CCombatant::GetInterface( uint i_id )
{
	// if we can't fulfil the request, pass it back up the chain
	return CCompActor::GetInterface( i_id );
}

uint CCombatant::GetTailoredInteractionHint(CProtagonist* i_actor)
{
	//if I am a npc
	if(isNpc && i_actor->IsA(CCombatant::ClassType()))
	{
		CCombatant *pComb = (CCombatant *)i_actor;

		if(!pComb->GetIsNpc())
		{
			// don't push the other guy around if he's telling us to stop pushing, or if he's on a ladder
			CActorComponent *comp = pComb->GetComponentByName( "BipedLadder" );

			Property *prop = pComb->GetProperty( "stopPushing" );
			if( prop == NULL || prop->GetInt() == 0 )
				if( comp == NULL || !comp->IsActive() )
					return CProtagonist::HINT_JUGGERNAUT;
		}
	}

	// don't let other people bump us around while we're on a ladder
	CActorComponent *comp = GetComponentByName( "BipedLadder" );
	if( comp && comp->IsActive() )
		return CProtagonist::HINT_JUGGERNAUT;

	return(((CProtagonist*)this)->GetInteractionHint());
}

CCapsuleWrapperNode::CCapsuleWrapperNode()
{
	d_capsuleNode = NULL;
	d_base = NULL;
}

void CCapsuleWrapperNode::Set(CCapsuleBonesNode* i_capsuleNode,
		const Vector3* i_base)
{
	d_capsuleNode = i_capsuleNode;
	d_base = i_base;
}

CCollisionObject* CCapsuleWrapperNode::BeginObjectTest(void)
{
	if (d_capsuleNode)
	{
		Capsule* capsule = (Capsule*)d_capsuleNode->GetCollisionObject();
		if (d_base)
		{
			Vector3 axis = capsule->End() - *d_base;
			float length = axis.Length();
			if (Math::DivByZero(1.0f, length))
				d_sphere.Set(capsule->Center(), capsule->SphericalRadius());
			else
			{
				axis *= 1.0f / length;
				// Find the center. Move the center down a little bit.
				Vector3 center = 0.5f * (capsule->End() + *d_base +
						(capsule->Radius() - 0.2f) * axis);
				float radius = 0.5f * (length + capsule->Radius());
				d_sphere.Set(center, radius);
			}
		}
		else
			d_sphere.Set(capsule->Center(), capsule->SphericalRadius());
		return(&d_sphere);
	}
	return(NULL);
}

void CCapsuleWrapperNode::EndObjectTest (ETestResult i_result,
		CCollisionPair* i_pair)
{
	if (d_capsuleNode)
		d_capsuleNode->EndObjectTest(i_result, i_pair);
}

CCollisionNode* CCapsuleWrapperNode::GetChild(void)
{
	if (d_capsuleNode)
		return(d_capsuleNode->GetChild());
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CCombatant::CIKilledHostage::HandleMessage(void* i_data, ActorHandle i_sender,
															 bool i_posted)
{
	if( g_ttl->IsSinglePlayer() ) return;
	Property* prop = d_ref->QueryProperty("KilledHostage");
	if( prop )
	{
		prop->Set( true );
	}
}