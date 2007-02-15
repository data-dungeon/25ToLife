#include "Components/ComponentsPCH.h"
#include "Components/InteractionMonitor.h"
#include "Components/ComponentCreator.h"
#include "collide/collsys.h"
#include "CollideHelper/LineEntity.h"
#include "CollideHelper/LineQuery.h"
#include "CollideHelper/SphereEntity.h"
#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"
#include "Interfaces/ViewIF.h"
#include "path/PathCollection.h"
#include "stage/SurfaceType.h"
#include "Interfaces/CollisionIF.h"
#include "collide/SphereToCapsule.h"
#include "components/weapons/weapinv.h"
#include "components/weapons/aimsim.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(InteractionMonitor, InteractionMonitor,
		COMPONENT_NEUTRAL_PRIORITY);

uint InteractionIF::s_interactionID = INTERFACE_ID_INVALID;

InteractionMonitor::InteractionMonitor( CCompActor &actor ) :
	CActorComponent( actor )
{
	currentTarget = NULL;
	currentTargetHandle = INVALID_OBJECT_HANDLE;
	targetType = OBJECT_NONE;
	
	// set up default search mode
	SearchForwardWedge( Vector3( 0.0f, 1.0f, 0.0f ), 3.5f, Math::Deg2Rad( 45.0f ) );

	fenceMaterial = g_surfaceType.GetIndex("fence");
	ladderMaterial = g_surfaceType.GetIndex("ladder");
	vaultMaterial = g_surfaceType.GetIndex("vaultable");
	climbMaterial = g_surfaceType.GetIndex("climbable");

	int i;
	for( i = 0; i < INTERACTION_COUNT; i++ )
		pQuery[i] = NULL;
}

InteractionMonitor::~InteractionMonitor()
{
}

float InteractionMonitor::InteractionRangeSquared( InteractionType type )
{
	float range = 1.5f;

	if( type == INTERACTION_VAULT || type == INTERACTION_CLIMB )
		range = 2.0f;
	if( type == INTERACTION_LADDER || type == INTERACTION_FENCE )
		range = 1.25f;

	return range * range;
}
InteractionMonitor::InteractionType ITypeFromName( const char *type )
{
	InteractionMonitor::InteractionType iType = InteractionMonitor::INTERACTION_NONE;
	if( stricmp( type, "Door" ) == 0 )
		iType = InteractionMonitor::INTERACTION_DOOR;
	else if( stricmp( type, "DoorLocked" ) == 0 )
		iType = InteractionMonitor::INTERACTION_DOOR_LOCKED;
	else if( stricmp( type, "DoorKickOpen" ) == 0 )
		iType = InteractionMonitor::INTERACTION_DOOR_KICK_OPEN;
	else if( stricmp( type, "DoorKickClose" ) == 0 )
		iType = InteractionMonitor::INTERACTION_DOOR_KICK_CLOSE;
	else if( stricmp( type, "DoorPullClose" ) == 0 )
		iType = InteractionMonitor::INTERACTION_DOOR_PULL_CLOSE;	
	else if( stricmp( type, "DoorSlideOpen" ) == 0 )
		iType = InteractionMonitor::INTERACTION_DOOR_SLIDE_OPEN;
	else if( stricmp( type, "DoorSlideClose" ) == 0 )
		iType = InteractionMonitor::INTERACTION_DOOR_SLIDE_CLOSE;
	else if( stricmp( type, "TagFrame" ) == 0 )
		iType = InteractionMonitor::INTERACTION_TAG_FRAME;
	else if( stricmp( type, "LightSwitchOn" ) == 0 )
		iType = InteractionMonitor::INTERACTION_LIGHT_SWITCH_ON;
	else if( stricmp( type, "LightSwitchOff" ) == 0 )
		iType = InteractionMonitor::INTERACTION_LIGHT_SWITCH_OFF;
	else if( stricmp( type, "SwitchOpen" ) == 0 )
		iType = InteractionMonitor::INTERACTION_SWITCH_OPEN;
	else if( stricmp( type, "SwitchClose" ) == 0 )
		iType = InteractionMonitor::INTERACTION_SWITCH_CLOSE;
	else if( stricmp( type, "ElevatorLocked" ) == 0 )
		iType = InteractionMonitor::INTERACTION_ELEVATOR_LOCKED;
	else if( stricmp( type, "ElevatorSwitch" ) == 0 )
		iType = InteractionMonitor::INTERACTION_ELEVATOR_SWITCH;
	else if( stricmp( type, "Hostage" ) == 0 )
		iType = InteractionMonitor::INTERACTION_HOSTAGE;
	else if( stricmp( type, "Apprehend" ) == 0 )
		iType = InteractionMonitor::INTERACTION_APPREHEND;
	else if( stricmp( type, "SwapWeapon" ) == 0 )
		iType = InteractionMonitor::INTERACTION_SWAP_WEAPON;
	else if( stricmp( type, "SwapLoadout" ) == 0 )
		iType = InteractionMonitor::INTERACTION_SWAP_LOADOUT;
	else if( stricmp( type, "JukeBoxAudioSelect" ) == 0 )
		iType = InteractionMonitor::INTERACTION_JUKEBOX_AUDIO_TRAC;
	else if( stricmp( type, "Vaultable" ) == 0 )
		iType = InteractionMonitor::INTERACTION_VAULT;
	else if( stricmp( type, "Ladder" ) == 0 )
		iType = InteractionMonitor::INTERACTION_LADDER;
	else if( stricmp( type, "Fence" ) == 0 )
		iType = InteractionMonitor::INTERACTION_FENCE;

	return iType;
}

void InteractionMonitor::SetInteractionText( CProtagonist *actor )
{
	if( actor )
	{
		ScriptVM *script = actor->Script( AI_SCRIPT );

		if( script )
		{
			// check too see if there are any custom text messages (input in script) for display
			if( script->HasFunction( "QueryTextMessage" ) )
			{
				script->Push( &GetActor() );
				script->CallFunction( "QueryTextMessage" );
				currentInteractionText = script->PopString();
			}
			if( script->HasFunction( "QueryTextMessage2" ) )
			{
				script->Push( &GetActor() );
				script->CallFunction( "QueryTextMessage2" );
				currentInteractionText2 = script->PopString();
			}
		}
	}
	else
	{
		currentInteractionText = NULL;
		currentInteractionText2 = NULL;
	}
}

InteractionMonitor::InteractionType InteractionMonitor::InteractionPossible( CProtagonist *actor )
{
	// if this actor cannot interact with us, don't try to.
	ScriptVM *script = actor->Script( AI_SCRIPT );
	InteractionType iType = INTERACTION_NONE;
	if( script )
	{
		if( script->HasFunction( "QueryInteractionType" ) )
		{
			script->Push( &GetActor() );
			script->CallFunction( "QueryInteractionType" );
			char *type = script->PopString();
			
			if( type == NULL || strlen( type ) == 0 )
				iType = INTERACTION_NONE;
			else			
				iType = ITypeFromName( type );
		}
	}

	// if we couldn't get an interaction type from script, check by interface
	if( iType == INTERACTION_NONE )
	{
		InteractionIF *interact = (InteractionIF *)actor->GetInterface( InteractionIF::GetID() );
		if( interact )
			iType = interact->QueryInteractionType( &GetActor() );
	}

	if( iType != INTERACTION_NONE )
	{
		// if we only care about things in front, make sure this one's in front
		if( searchType == INTERACT_FORWARDRADIUS || searchType == INTERACT_FORWARDWEDGE )
		{
			CCollisionIF *collisionIF = (CCollisionIF *)actor->GetInterface( CCollisionIF::GetID() );
			if( ( collisionIF && !InFront( collisionIF->GetCollisionInWorld() ) ) ||
					( !collisionIF && !InFront( actor->GetBodyInWorld() ) ) )
			{
				iType = INTERACTION_NONE;
			}
		}
	}

	return iType;
}

bool InteractionMonitor::ActorInteractionClear( CProtagonist &interactor, CProtagonist &interactee )
{
	CLineQuery query;
	CLineEntity entity;
	Vector3 origin, end;
	origin = interactor.GetBodyInWorld() + Vector3( 0.0f, 0.25f, 0.0f );
	end = interactee.GetBodyInWorld() + Vector3( 0.0f, 0.25f, 0.0f );
	entity.SetIgnoreEntity( &interactor );
	entity.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	entity.Line().SetOriginEnd( origin, end );
	if( query.Test( entity ) )
	{
		// did we hit something other than the interactee?
		if( query.target != (CStageEntity *)&interactee )
			return false;
	}
	origin = interactor.GetBodyInWorld() + Vector3( 0.0f, 1.25f, 0.0f );
	end = interactee.GetBodyInWorld() + Vector3( 0.0f, 1.25f, 0.0f );
	entity.Line().SetOriginEnd( origin, end );
	if( query.Test( entity ) )
	{
		// did we hit something other than the interactee?
		if( query.target != (CStageEntity *)&interactee )
			return false;
	}
	return true;
}

bool InteractionMonitor::InFront( Vector3CRef p )
{
	Vector3 forward;

	ViewIF *viewIF = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );
	if( viewIF )
		viewIF->GetDirection( forward );
	else
		forward = GetActor().GetBodyToWorld().GetRow2();

	Vector3 toP = p - (GetActor().GetBodyInWorld() + searchOffset);
	if( toP.Dot( forward ) <= 0.0f )
	{
		return false;
	}
	return true;
}

// record the first entity under the view cursor that is interactable,
// and use that as our interaction object.
void InteractionMonitor::ViewCursorEntity( CStageEntity *entity, u8 material )
{
	if( viewCursorEntity )
		return;

	InteractionType iType = INTERACTION_NONE;
	if( entity->IsA( CProtagonist::ClassType() ) )
	{
		CProtagonist *actor = (CProtagonist *)(entity);
		iType = InteractionPossible( actor );
	}
	else if( entity->IsA( CStaticEntity::ClassType() ) )
	{
		if( material == ladderMaterial )
			iType = INTERACTION_LADDER;
		else if( material == fenceMaterial )
			iType = INTERACTION_FENCE;
		else if( material == vaultMaterial )
			iType = INTERACTION_VAULT;
		else if( material == climbMaterial )
			iType = INTERACTION_VAULT;
	}
	if( iType != INTERACTION_NONE )
		viewCursorEntity = entity;
}

void InteractionMonitor::BeginFrame( void )
{
	g_collisionSystem.ResetStack();

	InteractionType closestIType = INTERACTION_NONE;
	InteractionObjectType closestObjectType = OBJECT_NONE;
	void *interactionObject = NULL;
	CCollisionList *list = NULL;
	float toClosestSquared = 0.0f, toObjectSquared;
	Vector3 toObject;
	Vector3 forward;
	bool interactionOverride = false;

	// I hate to put this here, but the only other alternative would be to have some
	// sort of "Interaction Override", which is just as ugly.  Since this is the easier ugly,
	// this is what I'm doing.  Maybe at some point in the future, I'll have a flash of intuition
	// that shows me a better way.  Until then, I willingly admit my suckiness.

	// If we've got a hostage (yeah, cause game-independant code should definitely have the concept
	// of a hostage, right?), disallow all other types of interaction.
	Property *prop = GetActor().QueryProperty( "hostage" );
	if( prop )
	{
		CActor* hostage = CActor::FromHandle( prop->GetHandle() );
		if( hostage != NULL)
		{
			bool allowRelease = true;

			// don't allow hostage release if we don't have a clear area in front of us
			CLineQuery query;
			CLineEntity entity;
			Vector3 origin, end;
			origin = GetActor().GetBodyInWorld() + Vector3( 0.0f, 0.25f, 0.0f );
			end = origin + (GetActor().GetBodyToWorld().GetRow2() * 2.0f);
			entity.SetIgnoreEntity( hostage );
			entity.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BACK_SIDE);
			entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
			entity.Line().SetOriginEnd( end, origin );
			//DrawUtility::Line( end, origin, DrawUtility::GREEN );
			if( query.Test( entity ) )
			{
				//DrawUtility::Point( query.intersection, DrawUtility::GREEN, 0.1f );
				//if( query.target != hostage )
					allowRelease = false;
			}
			origin = GetActor().GetBodyInWorld() + Vector3( 0.0f, 1.25f, 0.0f );
			end = origin + (GetActor().GetBodyToWorld().GetRow2() * 2.0f);
			entity.Line().SetOriginEnd( end, origin );
			//DrawUtility::Line( end, origin, DrawUtility::GREEN );
			if( query.Test( entity ) )
			{
				//DrawUtility::Point( query.intersection, DrawUtility::GREEN, 0.1f );
				//if( query.target != hostage )
					allowRelease = false;
			}

			interactionOverride = true;
			if( allowRelease )
			{
				closestIType = INTERACTION_RELEASEHOSTAGE;
				closestObjectType = OBJECT_ACTOR;
				interactionObject = hostage;
			}
		}
	}

	if( !interactionOverride )
	{
		ViewIF *viewIF = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );
		if( viewIF )
			viewIF->GetDirection( forward );
		else
			forward = GetActor().GetBodyToWorld().GetRow2();
			
		if( searchType == INTERACT_FORWARDVECTOR )
		{
			CLineEntity line;

			line.Line().SetOriginBasis( GetActor().GetBodyInWorld() + searchOffset, forward );
			line.Line().SetMaxBoundary( searchDistance );
			line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
			line.Line().SetFindMode(CLineCollisionObject::FIND_ALL);
			line.SetIgnoreEntity( &(GetActor()) );

			list = g_collisionSystem.Test( &line );
		}
		// CONE NOTES: since there's not actually a cone collision query, do a sphere check, and then
		// use the closest point to determine if the object is in the sphere or not
		else if( searchType == INTERACT_RADIUS || searchType == INTERACT_FORWARDRADIUS || searchType == INTERACT_FORWARDWEDGE )
		{
			CSphereEntity sphere;

			sphere.Sphere().Set( GetActor().GetBodyInWorld() + searchOffset, searchDistance );
			sphere.SetPositiveExclude( TERRAIN_CATEGORY );
			sphere.SetIgnoreEntity( &(GetActor()) );
			list = g_collisionSystem.Test( &sphere );
		}

		// find the closest interaction entity
		bool fullAbort = false;
		if( list )
		{
			CCollisionEvent* event = list->GetHeadEvent();
			while (event && !fullAbort)
			{
				CStageEntity *currentEntity = event->GetTargetEntity();
				if( currentEntity->IsA( CProtagonist::ClassType() ) )
				{
					CProtagonist *actor = (CProtagonist *)(currentEntity);
					InteractionType iType = InteractionPossible( actor );
					if( iType != INTERACTION_NONE )
						if( pQuery[iType] )
							iType = pQuery[iType]( GetActor(), actor, OBJECT_ACTOR, iType );
					CCollisionPair *pair = event->GetHeadPair();
					while( pair )
					{
						Vector3 p;
						if( pair->GetIntersectionType() == CCollisionIntersection::SPHERE_TO_CAPSULE )
							p = ((CSphereToCapsuleIntersection *)(pair->GetIntersection()))->ClosestPoint();
						else if( pair->GetIntersectionType() == CCollisionIntersection::SPHERE_TO_OBB )
							p = ((CSphereToObbIntersection *)(pair->GetIntersection()))->ClosestPoint();
						else
							ASSERT( 0 && "Unknown collision type" );

						toObject = p - (GetActor().GetBodyInWorld() + searchOffset);
						toObjectSquared = toObject.LengthSquared();
						
						if( iType == INTERACTION_LADDER )
						{
							ASSERT( 0 && "The actor-based version of the ladder is deprecated, and should no longer be used. Please change to a static OBB instead." );
							// don't allow ladder interaction if we're behind the ladder
							if( toObject.Dot( actor->GetBodyToWorld().GetRow2() ) <= 0.0f )
								iType = INTERACTION_NONE;
						}

						// make sure we're within the interaction range for this interaction type
						if( toObjectSquared > InteractionRangeSquared( iType ) )
							iType = INTERACTION_NONE;

						// cone check.  see the comment above labeled CONE NOTE
						if( searchType == INTERACT_FORWARDWEDGE )
						{
							Vector3 check = toObject;
							Vector3 flatforward = forward;
							check.y( 0.0f );
							check.SafeNormalize();
							flatforward.y( 0.0f );
							flatforward.SafeNormalize();
							if( check.Dot( flatforward ) < searchArcCos && !Math::Zero( toObjectSquared ) )
								iType = INTERACTION_NONE;
						}

						if( iType != INTERACTION_NONE )
						{
							if( interactionObject == NULL || toObjectSquared < toClosestSquared || currentEntity == viewCursorEntity )
							{
								if( iType == INTERACTION_HOSTAGE || iType == INTERACTION_APPREHEND )
								{
									// make sure there's nothing between us and them, so we don't pull them through geometry
									if( !ActorInteractionClear( GetActor(), *(actor) ) )
										iType = INTERACTION_NONE;

								}
								if( iType != INTERACTION_NONE )
								{
									interactionObject = actor;
									toClosestSquared = toObjectSquared;
									closestIType = iType;
									closestObjectType = OBJECT_ACTOR;
									SetInteractionText( actor );
									if( currentEntity == viewCursorEntity ) // found the guy under the cursor.  screw everybody else
									{
										fullAbort = true;
										break;
									}
								}
							}
						}
						pair = pair->GetNext();
					}
				}
				else if( currentEntity->IsA( CStaticEntity::ClassType() ) )
				{
					CStaticEntity *entity = (CStaticEntity *)(currentEntity);
					CCollisionPair *pair = event->GetHeadPair();
					InteractionType iType = INTERACTION_NONE;
					while( pair )
					{
						ASSERT( pair->GetIntersectionType() == CCollisionIntersection::SPHERE_TO_OBB );
						OBBStaticInstance* obb = (OBBStaticInstance*)(pair->GetTargetObject());
						Vector3 p = ((CSphereToObbIntersection *)(pair->GetIntersection()))->ClosestPoint();
						toObject = p - (GetActor().GetBodyInWorld() + searchOffset);
						toObjectSquared = toObject.LengthSquared();
						if( obb->Material() == ladderMaterial )
							iType = INTERACTION_LADDER;
						else if( obb->Material() == fenceMaterial )
							iType = INTERACTION_FENCE;
						else if( obb->Material() == vaultMaterial )
							iType = INTERACTION_VAULT;
						else if( obb->Material() == climbMaterial )
							iType = INTERACTION_VAULT;
						
						// make sure we're within the interaction range for this interaction type
						if( toObjectSquared > InteractionRangeSquared( iType ) )
							iType = INTERACTION_NONE;

						if( iType == INTERACTION_LADDER )
						{
							// don't allow ladder interaction if we're behind the ladder, unless we're at
							// the top of it.
							Vector3 boxForward = obb->Matrix()->GetRow2();
							if( toObject.y() >= -0.1f && toObject.Dot( boxForward ) <= 0.0f )
								iType = INTERACTION_NONE;
						}

						// if we only care about things in front, make sure this one's in front
						if( searchType == INTERACT_FORWARDRADIUS )
						{
							if( toObject.Dot( forward ) < 0.0f )
								iType = INTERACTION_NONE;
						}
						// cone check.  see the comment above labeled CONE NOTE
						else if( searchType == INTERACT_FORWARDWEDGE )
						{
							Vector3 check = toObject;
							Vector3 flatforward = forward;
							check.y( 0.0f );
							check.SafeNormalize();
							flatforward.y( 0.0f );
							flatforward.SafeNormalize();
							// for ladders, we may need to check the back wedge as well
							if( iType == INTERACTION_LADDER )
							{
								bool forwardWedge = true;
								bool backWedge = true;
								float dot = check.Dot( flatforward );
								if( dot < searchArcCos )
									forwardWedge = false;
								if( dot > -searchArcCos && toObject.y() > -0.99f )
									backWedge = false;
								// only allow back wedge if we're above it
								if( backWedge && toObject.y() > -0.1f )
									backWedge = false;

								if( !forwardWedge && !backWedge && !Math::Zero( toObjectSquared ) )
									iType = INTERACTION_NONE;
							}
							else
							{
								float dot = check.Dot( flatforward );
								if( dot < searchArcCos && !Math::Zero( toObjectSquared ) )
									iType = INTERACTION_NONE;
							}							
						}


						if( iType != INTERACTION_NONE )
						{
							if( pQuery[iType] )
								iType = pQuery[iType]( GetActor(), obb, OBJECT_STATICOBB, iType );

							if( interactionObject == NULL || toObjectSquared < toClosestSquared || currentEntity == viewCursorEntity )
							{
								interactionObject = obb;
								toClosestSquared = toObjectSquared;
								closestIType = iType;
								closestObjectType = OBJECT_STATICOBB;
								if( currentEntity == viewCursorEntity ) // found the guy under the cursor.  screw everybody else
								{
									fullAbort = true;
									break;
								}
							}
						}
						pair = pair->GetNext();
					}
				}
				event = event->GetNext();
			}
		}
	}
	// turn off interaction?
	if( interactionObject == NULL && currentTarget != NULL )
	{
		currentTarget = NULL;
		targetType = OBJECT_NONE;
		currentInteractionType = INTERACTION_NONE;
	}
	else
	{
		if( closestObjectType == OBJECT_ACTOR )
		{
			currentTarget = &(currentTargetHandle);
			currentTargetHandle = ((CProtagonist *)interactionObject)->GetHandle();
		}
		else
		{
			currentTarget = interactionObject;
		}
		targetType = closestObjectType;
		currentInteractionType = closestIType;
	}
}

void InteractionMonitor::BeginUpdate( void )
{
#ifdef _DEBUG
	if( currentTarget )
	{
		if( targetType == OBJECT_ACTOR )
		{
			CProtagonist *pProtag = CProtagonist::FromHandle(currentTargetHandle);
			if( pProtag->IsA( CProtagonist::ClassType() ) )
				pProtag->Draw( NULL );
		}
	}
#endif
}
