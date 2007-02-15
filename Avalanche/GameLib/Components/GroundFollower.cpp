#include "Components/ComponentsPCH.h"

#include "Components/GroundFollower.h"
#include "Components/ComponentCreator.h"
#include "collide/CapsuleToCapsule.h"
#include "collide/CapsuleToObb.h"
#include "collide/CapsuleToTerrain.h"
#include "Components/KineControl.h"
#include "components/slave.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(GroundFollower, GroundFollower,
		COMPONENT_NEUTRAL_PRIORITY);

#define COLLISION_CAPSULE

GroundFollower::GroundFollower( CCompActor &i_actor ) :
	CActorComponent( i_actor )
{
}

GroundFollower::~GroundFollower()
{
}

void GroundFollower::Initialize( void )
{
	GetActor().AddProperty( "stopPushing", Property_int );
	GetActor().SetProperty( "stopPushing", 0 );
	GetActor().AddProperty( "terrainCollide", Property_int );
	GetActor().SetProperty( "terrainCollide", 0 );
}

void GroundFollower::Activate( void )
{
	CActorComponent::Activate();
}

void GroundFollower::BeginFrame( void )
{
	OBB *obb = (OBB *)GetActor().GetCollisionObject();
	Vector3 movementDirection = GetActor().GetVelocityInWorld();

	if( movementDirection.SafeNormalize() )
	{
		#ifdef DEBUG_LEADING_CORNERS
			int corner[4];
			Vector3 cornerV[8];

			obb->Transform( &cornerV[0] );
			corner[0] = obb->GetWorldFrontCornerIndex( movementDirection );
			OBB::GetLeadingCorners( corner[0], corner[1], corner[2], corner[3] );
			
			for( int i = 0; i < 4; i++ )
			{
				DrawUtility::COLOR c = DrawUtility::MAGENTA;
				if( i == 0 ) c = DrawUtility::RED;
				DrawUtility::Point( cornerV[ corner[i] ], c, 0.1f );
			}
		#endif
	}

	lastPosition = GetActor().GetBodyInWorld();
	movementDetermined = false;
	anyCollisionsThisFrame = false;
}

void GroundFollower::BeginUpdate( void )
{
}

uint GroundFollower::CollisionDetect(CStageEntity* i_entity, CCollisionEvent* i_event, bool i_mateDisplaced)
{
	bool collideWithTerrain = false;

	//exit now if you are a Juggernaut or the other guy is impotent.
	if (i_entity->IsA(CProtagonist::ClassType()))
	{
		CProtagonist* protagonist = (CProtagonist*)i_entity;
		if( ( GetActor().GetTailoredInteractionHint(protagonist) & CProtagonist::HINT_JUGGERNAUT ) &&
				!( protagonist->GetTailoredInteractionHint( (CProtagonist*)(&actor) ) & CProtagonist::HINT_JUGGERNAUT ) )
			return CProtagonist::EFFECT_NONE;

		if( ( protagonist->GetTailoredInteractionHint((CProtagonist*)(&actor)) & CProtagonist::HINT_IMPOTENT ) &&
			!( GetActor().GetTailoredInteractionHint(protagonist) & CProtagonist::HINT_IMPOTENT ) )
			return CProtagonist::EFFECT_NONE;
	}

	if( !IsActive() )
		return CProtagonist::EFFECT_NONE;

	// Retest the pair of entities if the other entity has already displaced
	// in response to this collision.
	if (i_mateDisplaced)
	{
		i_event = CCollisionEvent::TestEntityPair((CStageEntity*)&actor,
			i_entity, &g_collisionSystem.GetStack());
		if (i_event == NULL)
			return(CProtagonist::EFFECT_NONE);
	}

	/*
	*	Try to describe the collision by a plane.
	*/
	uint effect = CProtagonist::EFFECT_NONE;
	Vector3 displacement;
	Vector3 point, normal;
	float queryDepth = 0.0f, targetDepth = 0.0f;

	if( !movementDetermined )
	{
		movementDetermined = true;
		movementDirection = GetActor().GetBodyInWorld() - lastPosition;
		movementDirection.SafeNormalize();
	}

	bool dead = false;
	if( GetActor().QueryProperty( "health" ) && GetActor().GetProperty( "health" )->GetFloat() <= 0.0f )
		dead = true;
	bool onGround = GetActor().GetProperty( "onGround" )->GetBool();
	Vector3 groundNormal = GetActor().GetProperty( "groundNormal" )->GetVector3();

	bool result = false;
	bool selected = true;

	Vector3 v = -movementDirection;
	if( !v.SafeNormalize() )
		selected = false;

	#ifdef COLLISION_CAPSULE
		CCollisionPair* pair = i_event->GetHeadPair();
		while( pair != NULL )
		{
			if( pair->GetIntersectionType() == CCollisionIntersection::CAPSULE_TO_CAPSULE )
			{
				CCapsuleToCapsuleIntersection *c2c = (CCapsuleToCapsuleIntersection *)pair->GetIntersection();
				Capsule *query = (Capsule *)pair->GetQueryObject();
				Capsule *target = (Capsule *)pair->GetTargetObject();
				float rsum = query->Radius() + target->Radius();
				normal = c2c->ClosestPointOnA() - c2c->ClosestPointOnB();
				point = c2c->ClosestPointOnB();
				queryDepth = normal.Length();
				if( queryDepth > 0.0f )
				{
					normal = normal / queryDepth;
					queryDepth = rsum - queryDepth;
				}
				else
				{
					queryDepth = rsum;
					normal = query->Axis().Cross( target->Axis() );
					if( !normal.SafeNormalize() )
					{
						// that failed too!  Which means the capsules axes are parallel as well as
						// intersecting, so pick any random normal that's perpendicular to the  axes
						// the capsule's orientation matrix gives a very convenient way to pick one
						normal = query->Orientation().GetRow0();
					}
				}
				// split the difference of the displacement
				targetDepth = queryDepth * 0.5f;
				queryDepth = targetDepth;
			}
			else if( pair->GetIntersectionType() == CCollisionIntersection::CAPSULE_TO_OBB )
			{
				CCapsuleToObbIntersection *c2o = (CCapsuleToObbIntersection *)pair->GetIntersection();
				//normal.Set( 0.0f, 1.0f, 0.0f );
				//queryDepth = 0.0f;
				c2o->GetResolutionNormal( normal, queryDepth );

				//c2o->GetCapsule().Draw( DrawUtility::GREEN );
				//c2o->GetOBB().Draw( DrawUtility::BLUE );
				//DrawUtility::NormalPlane( GetActor().GetBodyInWorld(), normal );
				
				//if( selected )
				//{
				//	if( !c2o->DirectionalFeatureTest( v, normal, queryDepth ) )
				//	{
				//		queryDepth = 0.0f;
				//		normal = v;
				//	}
				//}
				//else
				//	c2o->GetResolutionNormal( normal, queryDepth );
			}
			else if( pair->GetIntersectionType() == CCollisionIntersection::CAPSULE_TO_TERRAIN )
			{
				CCapsuleToTerrainIntersection *c2t = (CCapsuleToTerrainIntersection *)pair->GetIntersection();

				normal = c2t->GetResolutionInfo( queryDepth );
				if( !Math::Zero( queryDepth ) )
					collideWithTerrain = true;
			}
			else
			{
				ASSERT( 0 && "Unexpected collision type.  Let Travis know.  And this time, he really does want to know.  Promise.  :)" );
			}
			if( onGround )
			{
				// constrain displacement to the ground normal
				Vector3 newNormal = normal - ( groundNormal * groundNormal.Dot( normal ) );
				if( newNormal.SafeNormalize() )
				{
					float ratio = newNormal.Dot( normal );
					queryDepth *= ratio;
					normal = newNormal;
				}
			}
			pair = pair->GetNext();
		}
	#else
		if( 1 )//i_entity->IsA( CTerrainEntity::ClassType() ) )
		{
			if( selected )
			{
				if( !dead && onGround && Math::Abs( groundNormal.Dot( Vector3( 0.0f, 1.0f, 0.0f ) ) > Math::CosDeg( 15 ) ) )
					result = CEventMiner::SelectedPlaneFit( i_event, (CStageEntity*)(&actor), v, 0.051f, groundNormal, point, normal );
				else
					result = CEventMiner::SelectedPlaneFit( i_event, (CStageEntity*)(&actor), v, 0.05f, point, normal );
			}
			else
			{
				if( !dead && onGround && Math::Abs( groundNormal.Dot( Vector3( 0.0f, 1.0f, 0.0f ) ) > Math::CosDeg( 15 ) ) )
					result = CEventMiner::PlaneFit( i_event, groundNormal, point, normal );
				else
					result = CEventMiner::PlaneFit( i_event, point, normal );
			}

			if( !result )
				return( CProtagonist::EFFECT_NONE );

			CEventMiner::Depths( i_event, point, normal, queryDepth, targetDepth );
		}
		else
		{
			CCollisionPair* pair = i_event->GetHeadPair();
			while( pair != NULL )
			{
				if( pair->GetIntersectionType() == CCollisionIntersection::OBB_TO_OBB )
				{
					OBB *first, *second;
					CStageEntity *otherEntity;
					Vector3 movement;
					// OBBToOBBFirstPointOfCollision checks for first moving into second,
					// and we want to consider the case of our actor moving into the other
					// object, so make sure we get first and second right.  First is us,
					// second is the other guy
					if( i_event->GetTargetEntity() == (CStageEntity*)(&actor) )
					{
						first = (OBB *)(pair->GetTargetObject());
						second = (OBB *)(pair->GetQueryObject());
						otherEntity = i_event->GetQueryEntity();
					}
					else
					{
						second = (OBB *)(pair->GetTargetObject());
						first = (OBB *)(pair->GetQueryObject());
						otherEntity = i_event->GetTargetEntity();
					}
					// reduce movement to a normalized vector, but if the thing we're colliding
					// with is moving also, make our velocity relative to its first
					//if( otherEntity->IsA( CActor::ClassType() ) )
					//{
					//	CActor *otherActor = (CActor *)otherEntity;
					//	movement = movement - otherActor->GetVelocityInWorld();
					//}
					//movement = movement * g_timer.GetFrameSec();
					//if( !movement.SafeNormalize() )
					//{
						//result = CEventMiner::PlaneFit( i_event, point, normal );
						//if( !result )
						//	return( CProtagonist::EFFECT_NONE );
						//break;
					//}
					//else
					{
						result = CEventMiner::OBBToOBBFirstPointOfCollision( i_event, first, second, movementDirection, point, normal );
						if( !result )
							return( CProtagonist::EFFECT_NONE );
					}
					queryDepth = CEventMiner::OBBSimpleDepth( first, point, normal );
					targetDepth = 0.0f;
				}
				else
				{
					ASSERT( 0 && "Unexpected collision type.  Let Travis know.  And this time, he really does want to know.  Promise.  :)" );
				}
				pair = pair->GetNext();
			}
		}
	#endif
	
	uint hint = 0;
	if (i_entity->IsA(CProtagonist::ClassType()))
		hint = ((CProtagonist*)i_entity)->GetTailoredInteractionHint(&actor);

	// if we're the target, flip the normal so it points toward us
	if ( i_event->GetTargetEntity() == (CStageEntity*)(&actor) )
		normal.Negate();

	float depth = queryDepth;

	// apply displacement to us and our obb
	// Find the penetration depths. If the other entity has already displaced in
	// response to this collision or if the other entity will not displace, make
	// this entity perform all of the displacement needed to prevent
	// interpenetration.
	if ( i_mateDisplaced || (hint & CProtagonist::HINT_JUGGERNAUT) )
		depth += targetDepth;
	displacement = depth * normal;

	//ASSERTF( displacement.LengthXZSquared() <= 1.0f, ( "large displacement (%f), show travis", displacement.LengthXZSquared() ) );

	if( !Math::Zero( displacement.LengthSquared() ) )
	{
		// cancel out any velocity that is in the opposite direction
		// of the displacement
		Vector3 dV = -( normal * actor.GetVelocityInWorld().Dot( normal ) );
		actor.GetVelocityInWorld() += dV;
		KinematicController *controller = (KinematicController *)(actor.GetActiveComponentInClass( "KinematicController" ));
		if( controller )
			controller->VelocityDumped( dV );
		
		// do the displacement
		actor.GetBodyInWorld() += displacement;
		
		actor.SyncState();
		effect = CProtagonist::EFFECT_DISPLACEMENT;

		if( collideWithTerrain )
			GetActor().SetProperty( "stopPushing", 2 );

		// if we are slaved to another actor, push him too, so he can't shove us through stuff
		// I couldn't make this work easily, so I'm handling it differently
		//Slave *slave = (Slave *)GetActor().GetComponentByName( "Slave" );
		//if( slave )
		//{
		//	slave->GetMaster()->GetBodyInWorld() += displacement;
		//	slave->GetMaster()->SyncState();
		//}
		
		anyCollisionsThisFrame = true;
	}

	return effect;
}

void GroundFollower::EndFrame( void )
{
	lastPosition = GetActor().GetBodyInWorld();

	bool sync = false;
	int count = GetActor().GetProperty( "stopPushing" )->GetInt();
	if( count > 0 )
	{
		count--;
		GetActor().SetProperty( "stopPushing", count );
		if( count == 0 )
			sync = true;
	}
	count = GetActor().GetProperty( "terrainCollide" )->GetInt();
	if( count > 0 && !anyCollisionsThisFrame )
	{
		count--;
		GetActor().SetProperty( "terrainCollide", count );
		if( count == 0 )
			sync = true;
	}

	if( sync )
		SyncActor();
}
