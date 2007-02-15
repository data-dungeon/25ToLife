/**********************************************/
/*                                            */
/* PathFollower.cpp                           */
/* big juju							                */
/* originally by shon love                    */
/* cleaned by ned martin  avalanche software  */
/* visual c++ 6.0                             */
/*                                            */
/**********************************************/

#include "Components/ComponentsPCH.h"

#include "Interfaces/MoveIF.h"
#include "Components/PathFollower.h"
#include "Components/ComponentCreator.h"
#include "Game/Misc/ParameterSet.h"
#include "collide/collsys.h"
#include "CollideHelper/ObbEntity.h"
#include "CollideHelper/EventMiner.h"
#include "mathUtil/mathcons.h"

//-------------------------------------------------------------------------
//	Gravity
//-------------------------------------------------------------------------
const float constGeesToAccel = 9.8f;	// meters/second^2

// Define a creator and add it to the creator list.
CUSTOM_COMPONENT_CREATOR(CPathFollower, PathFollower, COMPONENT_DRIVER_PRIORITY
		)::Create(CCompActor &i_actor)
{
	CPathFollower *path = new CPathFollower( i_actor );

	if( !path->GetPath() )
	{
		// no path was attached, don't really return the component
		delete path;
		path = NULL;
	}
	return path;
}
 
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CPathFollower::CPathFollower( CCompActor& i_actor) : CActorComponent( i_actor )
{
	d_pPath = NULL;
	d_pRandomizedPath = NULL;
	d_pPathFromScratch = NULL;
	d_distance = 0.0f;
	d_orgSpeedTarget = 0.0f;

	SetCircuitComplete(false);
	SetAtEndpoint(false);
	SetKeepOriginalOrient(false);
	SetReverseAtEnd(true);
	SetFaceMovementDirection(true);
	SetSegmentPathMode(false);
	SetDropOnGround(false);
	SetRemoveAtPathEnd(false);
	SetIsEvacuator(false);

	SetIsOnPath( true );
	SetIsCushioned( false );

	d_state = 0;

	d_Orientation.V.Clear();
	d_Orientation.W = 1.0;

	d_fSpeedCurrent = 1.0f;
	d_fSpeedTarget = 1.0f;
	d_orgSpeedTarget = d_fSpeedTarget;
	d_fAccelerationMaximum = 1.0f;
	d_fTurningGeesMaximum = 1.0f;
	d_fResetDelay = 0.0f;

	d_pathFollowerState = pPAUSE;
	d_pauseCounter = 0;

	d_vPosition.Clear();
	d_vDirectionCurrent.Clear();

	Setup();
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CPathFollower::~CPathFollower()
{
	if (d_pRandomizedPath)
		delete d_pRandomizedPath;
	if (d_pPathFromScratch)
		delete d_pPathFromScratch;
}

//-------------------------------------------------------------------------
// setup from environmental actor record
// ----------------------------------------------------------------------------------
void CPathFollower::Setup()
{
	// check path name
	d_pPath = NULL;
	const char *pPathName = actor.Parm().GetString("path_name");
	if ((pPathName == NULL) || (*pPathName == 0))
		return;
	if (stricmp(pPathName, "none") == 0)
		return;
	if (stricmp(pPathName, "random") == 0)
	{
		/* make a random path object, add it to the global paths, and set it up for use here */
	}
	else
	{
		/* we have a path that should be in the collection */

		d_pPath = PathCollection::Collection()->Named(pPathName);
		if (d_pPath == NULL)
			d_pPath = PathCollection::Collection()->Main();	// not found, default to main path
	}
	if (d_pPath == NULL)
		return;

	/* path parameters */

	float fValue;
	int nValue;

	if ((fValue = actor.Parm().GetFloat("path_init_speed")) != MAXFLOAT)
		d_fSpeedCurrent = fValue;

	if ((fValue = actor.Parm().GetFloat("path_target_speed")) != MAXFLOAT)
	{
		d_fSpeedTarget = fValue;
		d_orgSpeedTarget = d_fSpeedTarget;
	}

	if ((fValue = actor.Parm().GetFloat("path_accel")) != MAXFLOAT)
		d_fAccelerationMaximum = fValue;

	if ((fValue = actor.Parm().GetFloat("path_max_g")) != MAXFLOAT)
		d_fTurningGeesMaximum = fValue;

	if ((fValue = actor.Parm().GetFloat("path_reset_delay")) != MAXFLOAT)
		d_fResetDelay = fValue;

	if ((nValue = actor.Parm().GetInt("path_drop_on_ground")) != MAXINT)
		SetDropOnGround(nValue != 0);

	if ((nValue = actor.Parm().GetInt("path_keep_orient")) != MAXINT)
		SetKeepOriginalOrient(nValue != 0);

	if ((nValue = actor.Parm().GetInt("path_reverse_at_end")) != MAXINT)
		SetReverseAtEnd(nValue != 0);

	if ((nValue = actor.Parm().GetInt("path_face_movement_direction")) != MAXINT)
		SetFaceMovementDirection(nValue != 0);

	if ((nValue = actor.Parm().GetInt("remove_at_path_end")) != MAXINT)
		SetRemoveAtPathEnd(nValue != 0);

	if ((nValue = actor.Parm().GetInt("evacuator")) != MAXINT)
		SetIsEvacuator(nValue != 0);

	// TODO: activate when this has ben added to ObjAttr.cfg and all the levels are updated
	//if ((nValue = params->GetInt("path_follower_is_cushioned")) != MAXINT)
	//	SetIsCushioned(nValue != 0);

	// initial position and direction
	PlaceActor();
}

//-------------------------------------------------------------------------
//	Set which path to follow
//-------------------------------------------------------------------------
bool CPathFollower::SetPath( SplinePath* i_path )
{
	if (i_path)
	{
		d_pPath = i_path;
		return(true);
	}
	else
		return(false);
}

//-------------------------------------------------------------------------
// ------------------------------------------------------------------------
void CPathFollower::PlaceActor( void )
{
	if (d_pPath != NULL)
	{
		d_vPosition = d_pPath->PointFromDistance( d_distance );
		d_vDirectionCurrent = d_pPath->TangentFromDistance( d_distance );
		Orient();
		Quat q = d_Orientation;

		actor.d_bodyInWorld = d_vPosition;
		actor.d_velocityInWorld = d_fSpeedCurrent * d_vDirectionCurrent;
		actor.d_bodyToWorld = q.ConvertToD3x3();
		actor.d_omegaInBody.Clear();

		actor.SyncState();
	}
}

//-------------------------------------------------------------------------
// Set distance along path from 0 to 1
//-------------------------------------------------------------------------
void CPathFollower::SetDistanceFromT( float i_T )
{
	ASSERT(i_T <= 1.0f);
	ASSERT(d_pPath);
	if (d_pPath == 0)
	{
		return;
	}

	d_distance = i_T * d_pPath->Length();
}

//-------------------------------------------------------------------------
//	adjust our speed for turns
//-------------------------------------------------------------------------
void CPathFollower::AdjustSpeed( float i_deltaSec )
{
	ASSERT(d_pPath);

	// slow down if we can't make the turn
	if (IsSegmentPathMode() == false)
	{
		// can we make the turn at current speed
		if ( CanMakeTurn( i_deltaSec ) )
		{
			// attempt to speed up
			d_fSpeedCurrent = SpeedUp( i_deltaSec );
		}
		else
		{
			// we can't make the turn at current speed, slow down
			d_fSpeedCurrent -= (d_fAccelerationMaximum * i_deltaSec );

			// min 1 m/s?
			if (d_fSpeedCurrent <= 0.0f)
				d_fSpeedCurrent = 1.0f;
		}
	}
	else
	{
		d_fSpeedCurrent = SpeedUp( i_deltaSec );
	}
}

//-------------------------------------------------------------------------
//	fall to earth
//-------------------------------------------------------------------------
Vector3 CPathFollower::DropOnTerrain( )
{
	Vector3		result( 0.0f, 1.0f, 0.0f );

	ASSERT(d_pPath);

	if ((IsSegmentPathMode() == false) && (IsDropOnGround()))
	{
//Profiler__Start(6);
#if 1
		// Get a point that is (about) on the ground.
		d_vPosition.y(d_vPosition.y() - d_pPath->AverageHeightAboveGround());
#else
		bool hit = false;
		SingleCollision hitResult;

		// attempt to drop on the ground
		// if we're following the ground, calculate/assign the appropriate y-coordinate for position here
		hit = g_collisionSystem->GetHeight(
			d_vPosition.x(),
			d_vPosition.z(),
			hitResult,
			CollisionSystem::WORLD,
			d_vPosition.y()
		);

		// maybe we're under the terrain?
		if ( !hit )
		{
			hit =  g_collisionSystem->GetHeight(
				d_vPosition.x(),
				d_vPosition.z(),
				hitResult,
				CollisionSystem::WORLD,
				REALLYLARGEY
			);
		}

		// OK, got a hit and groundFollowing, so - adjust the y coordinate
		if ( hit )
		{
			d_vPosition.y( hitResult.intersection.y());
			result = hitResult.normal;
		}
#endif
//Profiler__Stop(6);
	}

	return( result );
}

//-------------------------------------------------------------------------
//	Orient the object
//-------------------------------------------------------------------------
void CPathFollower::Orient()
{
	ASSERT(d_pPath);
	if ((IsKeepOriginalOrient()) || (IsSegmentPathMode()))
	{
		/* either not aligning to path at all or off-screen in simple path mode */

		Quat qVelocity;
		qVelocity.Identity();
		d_Orientation = qVelocity;
	}
	else
	{
		// the direction vector is the tangent to the current curve, or something close
		// so it is OK to use it for our orientation
		Vector3 dir = (Vector3 &) d_vDirectionCurrent;

		/* may need to reverse direction if traveling backwards on path */

		if ((IsFaceMovementDirection()) && (d_fSpeedCurrent < 0.0f) )
			dir *= -1.0f;

		DirCos3x3 mtxVelocity;
		mtxVelocity.BuildFromUnitVector(dir, 2, 1, 1);
		Quat qVelocity = mtxVelocity;
		qVelocity.Normalize();
		d_Orientation = qVelocity;
	}
}


//-------------------------------------------------------------------------
//  Proceed on the path a set distance, rather than speed/time based
//-------------------------------------------------------------------------
void CPathFollower::ProceedAlongPathFixed( float i_distance )
{
	ASSERT(d_pPath);

	//If we only want to move a fixed distance, kill the current speed.
	d_fSpeedCurrent=0;

	if (IsAtEndpoint() == false)
		d_distance += i_distance;

	// check for completed circuits
	if ( d_distance > d_pPath->Length() )
	{
		// Wrap (for circuits)
		if ( d_pPath->ClosedLoop() )
		{
			d_distance -= d_pPath->Length();

			if (d_fResetDelay != 0.0f )
				SetCircuitComplete(true);
		}
		else
		{
			d_fSpeedCurrent = 0.0f;
		}
		SetAtEndpoint(true);
	}
	else if ((d_distance <= 0.0f) && (i_distance < 0.0f))
	{
		d_distance = 0.0f;
		d_fSpeedCurrent = 0.0f;
		SetAtEndpoint(true);
	}
	else
	{
		SetAtEndpoint(false);
	}

	// process the appropriate path following routine
	if (IsSegmentPathMode())
	{
		d_vPosition = d_pPath->SegmentPath::PointFromDistance( d_distance );
		d_vDirectionCurrent = d_pPath->SegmentPath::TangentFromDistance( d_distance );
	}
	else
	{
		d_vPosition = d_pPath->PointFromDistance( d_distance );

/*		Some pathFollowers need to back up along their path.  If you have an object
		that needs to always face the direction it's going, place the orient code
		in that object's update code. */

		d_vDirectionCurrent = (d_pPath->TangentFromDistance( d_distance ));
	}
}

//-------------------------------------------------------------------------
//	Proceed on the path with the current parameters
//-------------------------------------------------------------------------
void CPathFollower::ProceedAlongPathClosed( float i_deltaSec )
{
	ASSERT(d_pPath);

	// proceed along the path with the current speed
	d_distance += d_fSpeedCurrent * i_deltaSec;

	// check for completed circuits
	if ( d_distance > d_pPath->Length() )
	{
		// Wrap (for circuits)
		if ( d_pPath->ClosedLoop() )
		{
			d_distance -= d_pPath->Length();

			if (d_fResetDelay != 0.0f )
			{
				SetCircuitComplete(true);
			}
		}
	}
	else if ((d_distance <= 0.0f) && (d_fSpeedCurrent < 0.0f))
	{
		d_distance += d_pPath->Length();
	}

	d_vPosition = d_pPath->PointFromDistance( d_distance );
	d_vDirectionCurrent = (d_pPath->TangentFromDistance( d_distance ));
}

//-------------------------------------------------------------------------
//	Proceed on the path with the current parameters
//-------------------------------------------------------------------------
void CPathFollower::ProceedAlongPath( float i_deltaSec )
{
	ASSERT(d_pPath);

	// proceed along the path with the current speed
	d_distance += d_fSpeedCurrent * i_deltaSec;

	// See if we've already reached an endpoint
	if ( d_distance == d_pPath->Length() )
	{
		if( !d_pPath->ClosedLoop() )
		{
			if( !IsReverseAtEnd() )
			{
				SetAtEndpoint(true);
			}
		}
	} else if ( d_distance > d_pPath->Length() ) // check for completed circuits
	{
		// Wrap (for circuits)
		if ( d_pPath->ClosedLoop() )
		{
			d_distance -= d_pPath->Length();

			if ( d_fResetDelay != 0.0f )
			{
				SetCircuitComplete(true);
			}
		}
		else
		{
			//BHY:  Bounce back and forth between beginning and end of path
//			d_fSpeedCurrent = 0.0f;
			if (IsReverseAtEnd())
			{
				d_distance = d_pPath->Length();
				Reverse(i_deltaSec);
			}
			else
			{
				//-- at end of path, don't reverse... full stop.
				d_fAccelerationMaximum = 0;
				d_fSpeedCurrent = 0;
				d_distance = d_pPath->Length();
			}

			/* send message to possible script for this actor */

			g_messageDispatcher.SendMessageToActor( "PathfollowerAtEnd", NULL, actor.GetHandle(), actor.GetHandle() );
		}
		SetAtEndpoint(true);
	}
	else if ((d_distance <= 0.0f) && (d_fSpeedCurrent < 0.0f))
	{
		//brian was here////////////////
		if (!IsReverseAtEnd() && d_fSpeedTarget < 0.0f)  //if you are going backward in a loop, continue looping backward
		{
			d_distance = d_pPath->Length() - 0.0001f;
		}
		////////////////////////////////
		else
		{
			d_distance = 0.0f;
			//d_fSpeedCurrent = d_orgSpeedTarget;
			//d_fSpeedTarget = d_orgSpeedTarget;
			Reverse(i_deltaSec);
			SetAtEndpoint(true);
		}
	}
	else
	{
		SetAtEndpoint(false);
	}

	// process the appropriate path following routine
	if (IsSegmentPathMode())
	{
		d_vPosition = d_pPath->SegmentPath::PointFromDistance( d_distance );
		d_vDirectionCurrent = d_pPath->SegmentPath::TangentFromDistance( d_distance );
	}
	else
	{
		d_vPosition = d_pPath->PointFromDistance( d_distance );

/*		Some pathFollowers need to back up along their path.  If you have an object
		that needs to always face the direction it's going, place the orient code
		in that object's update code. */

		d_vDirectionCurrent = (d_pPath->TangentFromDistance( d_distance ));
	}
}

//-------------------------------------------------------------------------
//	Move path follower along path
//-------------------------------------------------------------------------
bool CPathFollower::FollowPath( float i_deltaSec )
{
	ASSERT(d_pPath);

	// speed up, if possible
	AdjustSpeed( i_deltaSec );

	// detect the distance to the path from current position
	ProceedAlongPath( i_deltaSec );

	// maybe drop to the terrain
	DropOnTerrain( );

	// set the orientation
	Orient( );

	return( true );
}

// ----------------------------------------------------------------------------------
bool CPathFollower::FollowAccelPath( float i_deltaSec)
{
	ASSERT(d_pPath);

	switch( d_pathFollowerState)
	{
	case pPAUSE:
		d_pauseCounter+=i_deltaSec;
		if(d_pauseCounter>0.0f)  //Set the d_pauseCounter > 0 if you want to delay pathfollowing
			d_pathFollowerState = pFOLLOWPATH;
		break;
	case pFOLLOWPATH:
		Go(i_deltaSec);
		d_distance += d_fSpeedCurrent * i_deltaSec;
		if (d_distance > d_pPath->Length() - 0.5f)
		{
			//If the path being followed is a closed circuit path, remain in this state
			if ( d_pPath->ClosedLoop() )
			{
				d_distance -= d_pPath->Length();
				SetCircuitComplete(true);
			}
			else
			{
				d_pathFollowerState = pSTOPFORWARD;
			}
		}
		break;
	case pSTOPFORWARD:
		Stop(i_deltaSec);
		d_distance += d_fSpeedCurrent * i_deltaSec;
		if (d_fSpeedCurrent==0.0f)
		{
			d_pathFollowerState = pFOLLOWREVERSE;
		}
		break;
	case pFOLLOWREVERSE:
		Reverse(i_deltaSec);
		d_distance += d_fSpeedCurrent * i_deltaSec;
		if (d_distance < 0.5f)
		{
			d_pathFollowerState = pSTOPREVERSE;
		}
		break;
	case pSTOPREVERSE:
		Stop(i_deltaSec);
		d_distance += d_fSpeedCurrent * i_deltaSec;
		if (d_fSpeedCurrent==0.0f)
		{
			d_pathFollowerState = pFOLLOWPATH;
		}
		break;
	default:
		break;
	}
	// process the appropriate path following routine
	if (IsSegmentPathMode())
	{
		d_vPosition = d_pPath->SegmentPath::PointFromDistance( d_distance );
		if(d_fSpeedCurrent<0.0f)
			d_vDirectionCurrent = (-1.0f * d_pPath->SegmentPath::TangentFromDistance( d_distance ));
		else
			d_vDirectionCurrent = d_pPath->SegmentPath::TangentFromDistance( d_distance );
	}
	else
	{
		d_vPosition = d_pPath->PointFromDistance( d_distance );
		if(d_fSpeedCurrent<0.0f)
			d_vDirectionCurrent = (-1.0f * d_pPath->TangentFromDistance( d_distance ));
		else
			d_vDirectionCurrent = (d_pPath->TangentFromDistance( d_distance ));
	}
	// set the orientation
	Orient( );

	return( true );
}

//-------------------------------------------------------------------------
//	Move path follower along normalized arbitrary direction vector
//-------------------------------------------------------------------------
Vector3 CPathFollower::FollowDirection( float i_deltaSec, Vector3 &i_direction )
{
	Vector3		result, pos;

	ASSERT(d_pPath);
	ASSERT(i_deltaSec >= 0.0f);

	result = i_direction * ( d_fSpeedCurrent * i_deltaSec );

	pos = (Vector3 &)d_vPosition;
	pos = pos + result;

	d_vPosition = pos;

	return( pos );
}

//-------------------------------------------------------------------------
//	Test to see if we can handle the coming turn
//-------------------------------------------------------------------------
bool CPathFollower::CanMakeTurn( float i_deltaSec )
{

	ASSERT(d_pPath);

	if (IsSegmentPathMode())
		return( true );
	else
	{
		if ( d_fSpeedCurrent < ( MPHToMPS( 30.0f ) ) )
		{	// anything traveling under 30 miles/hour can take the turn! :)
			return( true );
		}
		else
		{
			float temp, minR, currR, deltaRadians;
			Vector3 tangentOld, tangentNew, positionOld, positionNew;
			Segment radialDistance;

			tangentOld.Set(0.0f, 0.0f, 0.0f);

			// calculate minimum radius of circle able to drive
			// r = v^2/a;
			temp = d_fSpeedCurrent;
			minR = ( ( temp * temp ) / (d_fTurningGeesMaximum * constGeesToAccel ) ) * i_deltaSec;

			// record the current orientation and position
			tangentOld = (Vector3 &)d_vDirectionCurrent;
			positionOld = (Vector3 &)d_vPosition;

			// calc new orientation and position
			// distance we want to travel this frame
			float d = d_distance + ( i_deltaSec * d_fSpeedCurrent );

			positionNew = d_pPath->PointFromDistance( d );
			tangentNew = d_pPath->TangentFromDistance( d );

			// find difference in radians ( about the y-axis ), and the distance traveled
			// find the distance between the two vectors
			radialDistance.start = positionNew;
			radialDistance.end = positionOld;

			float cosRadians = tangentNew.Dot( tangentOld );
			if (cosRadians > 1.0f)
				cosRadians = 1.0f;
			else if (cosRadians < -1.0f)
				cosRadians = -1.0f;
			deltaRadians = Math::ArcCos( cosRadians );

			// calc radius of desired circle
			// ( ( distance ) / ( dTheta ) )
			currR = ( radialDistance.Length( ) / deltaRadians );

			// if circleR is < minR, can't make it, slow down
			if ( currR >= minR )
			{
				return( true );
			}
			else
			{
				return( false );
			}
		}
	}
}

//-------------------------------------------------------------------------
//	Speed up to maximum velocity
//-------------------------------------------------------------------------
float CPathFollower::SpeedUp( float i_deltaSec )
{
	ASSERT(d_pPath);

	if (IsSegmentPathMode())
		return( d_fSpeedTarget );
	else
	{
		if ( Math::Zero( d_fAccelerationMaximum ) )
			return( d_fSpeedCurrent );

		float accel = d_fAccelerationMaximum * i_deltaSec;
		float newSpeed = d_fSpeedCurrent;

		// try to accelerate or deccelerate to target speed
		if(d_fSpeedTarget>0.0f)
		{
			if ( newSpeed < d_fSpeedTarget)
			{
				if ( (newSpeed + accel ) > d_fSpeedTarget )
				{
					newSpeed = d_fSpeedTarget;
				}
				else
				{
					newSpeed += accel;
				}
			}
			else if (newSpeed > d_fSpeedTarget)
			{
				if ( (newSpeed - accel ) < d_fSpeedTarget )
				{
					newSpeed = d_fSpeedTarget;
				}
				else
				{
					newSpeed -= accel;
				}
			}
		}
		else
		{
			if ( newSpeed > d_fSpeedTarget)
			{
				if ( (newSpeed - accel ) < d_fSpeedTarget )
				{
					newSpeed = d_fSpeedTarget;
				}
				else
				{
					newSpeed -= accel;
				}
			}
			else if (newSpeed < d_fSpeedTarget)
			{
				newSpeed = d_fSpeedTarget;
			}
		}

		return( newSpeed );
	}
}

//-------------------------------------------------------------------------
//	Speed down to minimum velocity
//-------------------------------------------------------------------------
float CPathFollower::SlowDown( float i_deltaSec )
{
	ASSERT(d_pPath);

	if (IsSegmentPathMode())
		return( d_fSpeedTarget );
	else
	{
		if ( Math::Zero( d_fAccelerationMaximum ) )
			return( d_fSpeedCurrent );

		float accel = d_fAccelerationMaximum * i_deltaSec;
		float newSpeed = d_fSpeedCurrent;

		// try to slow down to target speed
		if (newSpeed == d_fSpeedTarget)
		{
			return (newSpeed);
		}
		if ( newSpeed < d_fSpeedTarget)
		{
			if (( newSpeed + accel ) > d_fSpeedTarget )
			{
				newSpeed = d_fSpeedTarget;
			}
			else
			{
				newSpeed += accel;
			}
		}
		else if (newSpeed > d_fSpeedTarget)
		{
			if (( newSpeed - accel ) < d_fSpeedTarget )
			{
				newSpeed = d_fSpeedTarget;
			}
			else
			{
				newSpeed -= accel;
			}
		}
		return( newSpeed );
	}
}

//-------------------------------------------------------------------------
// move along path
//-------------------------------------------------------------------------
void CPathFollower::Go( float i_deltaSec )
{
	d_fSpeedTarget = d_orgSpeedTarget;
	d_fSpeedCurrent = SpeedUp(i_deltaSec);
	//SpeedUp( i_deltaSec );
}

//-------------------------------------------------------------------------
// stop at current position
//-------------------------------------------------------------------------
void CPathFollower::Stop( float i_deltaSec )
{
	d_fSpeedTarget = 0.0f;
	d_fSpeedCurrent = SlowDown( i_deltaSec );
	//SpeedUp( i_deltaSec );
}

//-------------------------------------------------------------------------
// reverse speed
//-------------------------------------------------------------------------
void CPathFollower::Reverse( float i_deltaSec )
{
	d_fSpeedTarget = -d_fSpeedCurrent;
	d_fSpeedCurrent = 0.0f;
	d_fSpeedCurrent = SpeedUp( i_deltaSec );
}

//-------------------------------------------------------------------------
//	Given a normal vector, find the quaternion for rotation into that plane
// i_direction should be an xz-only vector for rotation around i_normal
//-------------------------------------------------------------------------
ts_Quatvw CPathFollower::QuatFromNormalAsPlane( Vector3 i_normal )
{
	const Vector3	yAxis( 0.0f, 1.0f, 0.0f );
	Quat			result, direction;
	float			angle;

	i_normal.Normalize( );

	angle = Math::ArcCos( i_normal.Dot( yAxis ) );

	if ( angle > 0.01f )
	{
		Vector3			right, up, dir;
		DirCos3x3		mtx;

		up = i_normal;

		dir = yAxis.Cross( i_normal );
		dir.Normalize( );

		right = dir.Cross( i_normal );
		right.Normalize( );

		mtx.SetColumn1( up );
		mtx.SetColumn0( right );
		mtx.SetColumn2( dir );

		result = mtx;
		result.Normalize( );
	}
	else
	{
		result.Identity( );
	}

	return( result );
}

// ----------------------------------------------------------------------------------
/* randomize the spline path. note that this should be called at initialize time, since it is possible for the path
follower to change paths. typically, the path you want to randomize is the path follower's initial path */
// ----------------------------------------------------------------------------------
void CPathFollower::RandomizePath()
{
	/* already randomized? */

	if (d_pRandomizedPath)
	{
		ASSERT(false);	// randomizing more than once???
		return;
	}

	/* make a copy-- we are now responsible for it */

	ASSERT(d_pPath);
	d_pRandomizedPath = new SplinePath(d_pPath);

	/* randomize and make it our current path */

	d_pRandomizedPath->Randomize();
	d_pPath = d_pRandomizedPath;
}

// ----------------------------------------------------------------------------------
/* create a new path from scratch for the path follower to follow. we assume that the passed point array is in
"point/control point/control point, repeat" format */
// ----------------------------------------------------------------------------------
void CPathFollower::CreatePathFromScratch(Vector3 *pvPoints, int nPointCount, bool bForceNoLoop /*= false*/)
{
	ASSERT(pvPoints);

	/* get rid of old one, if any */

	if (d_pPathFromScratch)
		delete d_pPathFromScratch;

	/* create a new one */

	d_pPathFromScratch = new SplinePath("From scratch", pvPoints, nPointCount, bForceNoLoop);
	ASSERT(d_pPathFromScratch);

	/* follow new path */

	d_pPath = d_pPathFromScratch;
	d_vPosition = pvPoints[0];
	d_distance = 0.0f;
}

// ----------------------------------------------------------------------------------
// delete path from scratch-- but not if they are currently on it!
// ----------------------------------------------------------------------------------
void CPathFollower::DeletePathFromScratch()
{
	if (d_pPathFromScratch)
	{
		ASSERT(d_pPathFromScratch != d_pPath);	// currently on it!
		if (d_pPathFromScratch != d_pPath)
		{
			delete d_pPathFromScratch;
			d_pPathFromScratch = NULL;
		}
	}
}

//-------------------------------------------------------------------------
// Check for possible collisions in way of path follower
//-------------------------------------------------------------------------
bool CPathFollower::FindObstacle( Vector3 &o_point, Vector3 &o_normal )
{
	// size of ObbEntity for actor
	static const float BOX_WIDTH = 1.0f;
	static const float BOX_HEIGHT = 0.5f;

	bool isFound = false;

	// setup ObbEntity around the path follower
	CObbEntity obbEntity;
	AABB aabb;
	aabb.Set( Vector3(-BOX_WIDTH, -BOX_HEIGHT, -BOX_WIDTH), Vector3(BOX_WIDTH, BOX_HEIGHT, BOX_WIDTH) );
	obbEntity.Obb().SetBoundingVolume( &aabb );
	obbEntity.Obb().SetOffsetTransform( d_beginBodyToWorld, d_vPosition, Vector3(0.0f, 1.5f, 0.0f) );

	// check against terrain and static actors
	obbEntity.SetPositiveInclude( ALL_CATEGORIES );

	// reset collision stack and test for collisions with obbEntity
	g_collisionSystem.ResetStack();
	CCollisionList* list = g_collisionSystem.Test( &obbEntity );

	if (list)
	{ // collision(s) found, check for affect on actor
		CCollisionEvent* event = list->GetHeadEvent();
		Vector3 point;
		Vector3 normal;
		float distance;
		float range = 0;

		Vector3 vDirection = d_vDirectionCurrent;
		if (GetSpeedCurrent() < 0.f)
		{ // actor is traveling the reverse path route, use the correct direction
			vDirection = vDirection * -1.f;
		}

		while (event)
		{
			CEventMiner::PlaneFit( event, point, normal );

			// check that the ObbEntity isn't colliding with our PathFollower
			if ((CActor*)event->GetTargetEntity() != (CActor*)(&this->GetActor()))
			{
				// angle between the direction and collision normals
				float diffAngle = Math::ArcCos( Vector3::Dot( vDirection, normal ) / 
					(vDirection.Length() * normal.Length()) );

				distance = Math::Sqrt( Math::Pow( point.x() - d_vPosition.x(), 2 ) 
						+ 0.0f + Math::Pow( point.z() - d_vPosition.z(), 2 ) );

				if (diffAngle >= (Math::Pi / 2.0f) && distance < 1.0f) 
				{ // the collision is in front of the follower
					if (range < (diffAngle / distance))
					{ // closest collision found so far
						isFound = true;
						range = diffAngle / distance;
						o_point = point;
						o_normal = normal;
					}
				}
			}

			event = event->GetNext();
		}
		event = NULL;
	}

	// cleanup collision stuff
	list = NULL;
	g_collisionSystem.ResetStack();

	return isFound;
}

//-------------------------------------------------------------------------
// Move to avoid a collision
//-------------------------------------------------------------------------
void CPathFollower::TakeDetour( float i_deltaSec, Vector3 i_point, Vector3 i_normal )
{
	SetIsOnPath( false );
	Vector3 zAxisVec = Vector3(0.0f, 0.0f, 1.0f);
	Vector3 point = i_point;
	Vector3 normal = i_normal;

	// if actor has reversed their direction on the path then inverse the current direction
	Vector3 vDirection = d_vDirectionCurrent;
	if (GetSpeedCurrent() < 0.f)
	{
		vDirection = vDirection * -1.f;
	}

	// angle between the direction normal and the +z-axis unit vector
	float actAngle = Math::ArcCos( Vector3::Dot( vDirection, zAxisVec )
			/ (vDirection.Length() * zAxisVec.Length()) );

	// inverse collision normal so it's within Pi/2 degrees of direction normal
	normal = normal * -1.0f;
	// angle between the collision normal and the +z-axis unit vector
	float colAngle = Math::ArcCos( Vector3::Dot( normal, zAxisVec )
			/ (normal.Length() * zAxisVec.Length()) );

	// direction vector of collision from actor position
	Vector3 colVec = point - d_vPosition;

	// use a rotation matrix to get the new direction vector
	DirCos3x3 rotate;
	float rotAngle = 0.0f;

	// calculate rotAngle to rotate direction vector to get detour direction vector
	if (vDirection.x() >= 0.0f)
	{
		if (colVec.x() >= 0.0f)
		{
			if (actAngle >= colAngle)
			{ // turn left
				rotAngle = Math::Pi - (actAngle - colAngle);
			}
			else //if (actAngle < colAngle)
			{ // turn right
				rotAngle = -Math::Pi + (colAngle - actAngle);
			}
		}
		else if (colVec.x() < 0.0f)
		{
			if (actAngle + colAngle < Math::Pi)
			{ // turn left
				rotAngle = Math::Pi - (actAngle + colAngle);
			}
			else //if (actAngle + colAngle >= Math::Pi)
			{ // turn right
				rotAngle = -Math::Pi + ((Math::Pi * 2.0f) - (colAngle + actAngle));
			}
		}
	}
	else
	{ // direction colVec right of +z unit vector
		if (colVec.x() < 0.0f)
		{
			if (actAngle < colAngle)
			{ // turn left
				rotAngle = Math::Pi - (colAngle - actAngle);
			}
			else //if (actAngle >= colAngle)
			{ // turn right
				rotAngle = -Math::Pi + (actAngle - colAngle);
			}
		}
		else //if (colVec.x() >= 0.0f)
		{
			if (actAngle + colAngle >= Math::Pi)
			{ // turn left
				rotAngle = Math::Pi - ((Math::Pi * 2.0f) - (colAngle + actAngle));
			}
			else //if (actAngle + colAngle < Math::Pi)
			{ // turn right
				rotAngle = -Math::Pi + (actAngle + colAngle);
			}
		}
	}

	// distance from collision changes how quickly we do the rotation
	float distance = Math::Sqrt( Math::Pow( point.x() - d_vPosition.x(), 2 )
			+ 0.0f + Math::Pow( point.z() - d_vPosition.z(), 2 ) );
	float distFactor = (1.25f / distance);
	rotAngle *= distFactor * Math::Abs(GetSpeedCurrent()) * i_deltaSec;

	// rotate direction by calculated angle
	rotate.SetYRotation( rotAngle );
	d_vDirectionCurrent = vDirection * rotate;
	if (GetSpeedCurrent() < 0.f)
	{
		d_vDirectionCurrent = d_vDirectionCurrent * -1.0f;
	}

	// speed up, if possible
	AdjustSpeed( i_deltaSec );

	// send path follower in the calculated direction
	FollowDirection( i_deltaSec, d_vDirectionCurrent );

	// maybe drop to the terrain
	DropOnTerrain();

	// set the orientation
	Orient();
}

//-------------------------------------------------------------------------
// Return to the path
//-------------------------------------------------------------------------
void CPathFollower::ReturnToPath( float i_deltaSec )
{
	float distOnPath = d_pPath->NearestDistance( d_vPosition );
	Vector3 posOnPath = d_pPath->PointFromDistance( distOnPath );
	Vector3 tanOnPath = d_pPath->TangentFromDistance( distOnPath );
	Vector3 zAxisVec = Vector3(0.0f, 0.0f, 1.0f);

	// vector normal from actor to path position
	Vector3 dirToPath = posOnPath - d_vPosition;
	dirToPath = dirToPath / dirToPath.Length();

	// if actor has reversed their direction on the path then inverse the current direction
	Vector3 vDirection = d_vDirectionCurrent;
	if (GetSpeedCurrent() < 0.f)
	{
		vDirection = vDirection * -1.0f;
	}

	//  angle between actor to path position normal and +z-axis unit vector
	float dirAngle = Math::ArcCos( Vector3::Dot( dirToPath, zAxisVec )
			/ (dirToPath.Length() * zAxisVec.Length()) );

	// angle between direction normal and +z-axis unit vector
	float actAngle = Math::ArcCos( Vector3::Dot( vDirection, zAxisVec )
			/ (vDirection.Length() * zAxisVec.Length()) );

	// use a rotation matrix to get the new direction vector
	DirCos3x3 rotate;
	float rotAngle = 0.0f;

	// distance between position of actor and closest position on path
	float distance = Math::Sqrt( Math::Pow( posOnPath.x() - d_vPosition.x(), 2 )
			+ 0.0f + Math::Pow( posOnPath.z() - d_vPosition.z(), 2 ) );

	if (distance < 0.1f)
	{ // back on path
		SetIsOnPath( true );
		SetPathDistance( distOnPath );
	}
	else if (distance < 0.5f)
	{
		// angle between path tangent and +z-axis unit vector
		float pathAngle = Math::ArcCos( Vector3::Dot( tanOnPath, zAxisVec )
				/ (tanOnPath.Length() * zAxisVec.Length()) );

		// calculate rotAngle to rotate direction vector to exponentially parallel the path tangent
		if (vDirection.x() >= 0.0f)
		{
			if (tanOnPath.x() >= 0.0f)
			{
				rotAngle = pathAngle - actAngle;
			}
			else if (actAngle + pathAngle >= Math::Pi)
			{ // turn left
				rotAngle = (Math::Pi * 2.0f) - (actAngle + pathAngle);
			}
			else
			{ // turn right
				rotAngle = -Math::Pi + (actAngle + pathAngle);
			}
		}
		else
		{
			if (tanOnPath.x() < 0.0f)
			{
				rotAngle = actAngle - pathAngle;
			}
			else if (actAngle + pathAngle < Math::Pi)
			{ // turn left
				rotAngle = Math::Pi - (actAngle + pathAngle);
			}
			else
			{ // turn right
				rotAngle = -(Math::Pi * 2.0f) + (actAngle + pathAngle);
			}
		}

		//rotAngle *= ((0.1f / distance) * i_deltaSec);
	}
	else
	{
		// calculate rotAngle to rotate direction vector to get detour direction vector
		if (vDirection.x() >= 0.0f)
		{
			if (dirToPath.x() >= 0.0f)
			{
				rotAngle = dirAngle - actAngle;
			}
			else if (dirAngle + actAngle >= Math::Pi)
			{ // turn left
				rotAngle = (Math::Pi * 2.0f) - (dirAngle + actAngle);
			}
			else
			{ // turn right
				rotAngle = -(dirAngle + actAngle);
			}
		}
		else
		{
			if (dirToPath.x() < 0.0f)
			{
				rotAngle = actAngle - dirAngle;
			}
			else if (dirAngle + actAngle < Math::Pi)
			{ // turn left
				rotAngle = dirAngle + actAngle;
			}
			else
			{ // turn right
				rotAngle = -((Math::Pi * 2.0f) - (dirAngle + actAngle));
			}
		}

		//rotAngle *= i_deltaSec;
	}

	// rotate direction by calculated angle
	rotAngle *=  (Math::Abs(GetSpeedCurrent()) * i_deltaSec);
	rotate.SetYRotation( rotAngle );
	d_vDirectionCurrent = vDirection * rotate;
	if (GetSpeedCurrent() < 0.f)
	{
		d_vDirectionCurrent = d_vDirectionCurrent * -1.0f;
	}

	// speed up, if possible
	AdjustSpeed( i_deltaSec );

	// send path follower in the calculated direction
	FollowDirection( i_deltaSec, d_vDirectionCurrent );

	// maybe drop to the terrain
	DropOnTerrain();

	// set the orientation
	Orient();
}

// ----------------------------------------------------------------------------------
//#define IDENTIFY_PATHFOLLOWER
#ifdef IDENTIFY_PATHFOLLOWER
	#define ACTORCOLORS 7
	struct ActorColor
	{
		CActor *actor;
		DrawUtility::COLOR color;
	};

	ActorColor ActorColors[] =
	{
		{ NULL, DrawUtility::RED },
		{ NULL, DrawUtility::GREEN },
		{ NULL, DrawUtility::BLUE },
		{ NULL, DrawUtility::CYAN },
		{ NULL, DrawUtility::MAGENTA },
		{ NULL, DrawUtility::YELLOW },
		{ NULL, DrawUtility::WHITE }
	};
#endif //IDENTIFY_PATHFOLLOWER

// ----------------------------------------------------------------------------------
void CPathFollower::BeginUpdate()
{
	#ifdef IDENTIFY_PATHFOLLOWER
		int i;
		for( i = 0; i < ACTORCOLORS; i++ )
		{
			if( ActorColors[i].actor == &actor )
				break;
			if( ActorColors[i].actor == NULL )
			{
				ActorColors[i].actor = &actor;
				break;
			}
		}
		if( i < ACTORCOLORS )
			DrawUtility::Point( actor.GetBodyInWorld(), ActorColors[i].color );
	#endif //IDENTIFY_PATHFOLLOWER

	if( !IsActive() )
		return;

	d_beginSpeedCurrent = d_fSpeedCurrent;
	d_beginDistance     = d_distance;
	d_beginBodyInWorld  = actor.GetBodyInWorld();
	d_beginBodyToWorld  = actor.GetBodyToWorld();
}

// ----------------------------------------------------------------------------------
void CPathFollower::AttemptUpdate( float i_deltaSec )
{
	if( !IsActive() )
		return;

	// set state to as it was at BeginUpdate
	d_vPosition     = d_beginBodyInWorld;
	d_fSpeedCurrent = d_beginSpeedCurrent;
	d_distance      = d_beginDistance;

	// --- BET: cushions actor from obstacles in its path if padding is on
	if (IsCushioned())
	{
		Vector3 point;
		Vector3 normal;
		if (FindObstacle( point, normal ))
		{	// avoid an obstacle in our path
			TakeDetour( i_deltaSec, point, normal );
		}
		else if (!IsOnPath())
		{	// return to the path
			ReturnToPath( i_deltaSec );
		}
		else
		{	// move our target point along the path
			FollowPath( i_deltaSec );
		}
	}
	else
	{	// move our target point along the path
		FollowPath( i_deltaSec );
	}

	CMoveIF *moveIF = (CMoveIF *)actor.GetInterface( CMoveIF::GetID() );
	if (moveIF && IsOnPath())
		moveIF->CommandMoveToPoint( d_vPosition, Math::Abs( d_fSpeedCurrent ) );
	else
	{
		// place the actor at the new location
		actor.GetBodyInWorld() = d_vPosition;
		// orient
		if ( !IsKeepOriginalOrient() )
		{
			Quat q = d_Orientation;
			actor.GetBodyToWorld() = q.ConvertToD3x3();
		}

		// velocity
		actor.d_velocityInWorld = d_vPosition - d_beginBodyInWorld;
		if ( !Math::Zero( i_deltaSec ) )
			actor.d_velocityInWorld /= i_deltaSec;

		// angular velocity
		DirCos3x3 bodyToBegin = actor.GetBodyToWorld() / d_beginBodyToWorld;
		Quat q = bodyToBegin;
		actor.d_omegaInBody = q;
		float sinHalfAngle = actor.d_omegaInBody.Length();
		if (Math::DivByZero(1.0f, sinHalfAngle))
			actor.d_omegaInBody.Set(0.0f, 1.0f, 0.0f);
		else
			actor.d_omegaInBody *= (1.0f / sinHalfAngle);
		float angle = 2.0f * Math::ArcTan2(sinHalfAngle, q.e[3]);
		if (Math::DivByZero(angle, i_deltaSec))
			actor.d_omegaInBody.Clear();
		else
			actor.d_omegaInBody *= (angle / i_deltaSec);

		// update collision
		actor.SyncState();
	}
}

// ----------------------------------------------------------------------------------
void CPathFollower::Activate( void )
{
	CActorComponent::Activate();
}

// ----------------------------------------------------------------------------------
void CPathFollower::Deactivate( void )
{
	CActorComponent::Deactivate();
}
// ----------------------------------------------------------------------------------
/* copy one path follower to another. like CPathFollower::Randomize, typically this routine should be used as part of
initialization. path followers can move from path to path, and you probably want to copy the path follower's original
state, when he is on his original path */
// ----------------------------------------------------------------------------------
CPathFollower &CPathFollower::operator=( CPathFollower &Source )
{
	/* memcpy */

	memcpy(this, &Source, sizeof(CPathFollower));

	/* we don't own randomized path (for memory purposes), if any */

	d_pRandomizedPath = NULL;

	/* done */

	return *this;
}

// --------------------------- Save/Load Game Functions -----------------------------
u32 CPathFollower::GetSaveGameMemoryNeeded( )
{
	return( Align_RoundTo16( sizeof( pathfollowerSaveGameData ) ) );
}

// ----------------------------------------------------------------------------------
u32 CPathFollower::SerializeSaveGame( char *pBuf, bool bDirection )
{
	// --- Save Data?
	if( bDirection )
	{
		pathfollowerSaveGameData *pSaveTo = ( pathfollowerSaveGameData * )pBuf;

		pSaveTo->d_distance						= d_distance;
		pSaveTo->d_timeToReset					= d_timeToReset;
		pSaveTo->d_orgSpeedTarget					= d_orgSpeedTarget;
		pSaveTo->d_fSpeedTarget					= d_fSpeedTarget;
		pSaveTo->d_fSpeedCurrent				= d_fSpeedCurrent;
		pSaveTo->d_fAccelerationMaximum			= d_fAccelerationMaximum;
		pSaveTo->d_fTurningGeesMaximum			= d_fTurningGeesMaximum;
		pSaveTo->d_vDirectionCurrent			= d_vDirectionCurrent;
		pSaveTo->d_vPosition					= d_vPosition;
		pSaveTo->d_Orientation					= d_Orientation;
		pSaveTo->d_fResetDelay					= d_fResetDelay;
		pSaveTo->d_stopDistance					= d_stopDistance;
		pSaveTo->d_slowingSpeed					= d_slowingSpeed;
		pSaveTo->d_pauseCounter					= d_pauseCounter;
		pSaveTo->d_pathFollowerState				= d_pathFollowerState;
		pSaveTo->d_bCircuitComplete				= d_bCircuitComplete;
		pSaveTo->d_bAtEndpoint					= d_bAtEndpoint;
		pSaveTo->d_bKeepOriginalOrient			= d_bKeepOriginalOrient;
		pSaveTo->d_bReverseAtEnd				= d_bReverseAtEnd;
		pSaveTo->d_bFaceMovementDirection		= d_bFaceMovementDirection;
		pSaveTo->d_bSegmentPathMode				= d_bSegmentPathMode;
		pSaveTo->d_bDropOnGround				= d_bDropOnGround;
		pSaveTo->d_bRemoveAtPathEnd				= d_bRemoveAtPathEnd;
		pSaveTo->d_bIsEvacuator					= d_bIsEvacuator;
		pSaveTo->d_isOnPath						= d_isOnPath;
		pSaveTo->d_isCushioned					= d_isCushioned;
	}

	// -- Load Data?
	else
	{
		pathfollowerSaveGameData *pLoadFrom = ( pathfollowerSaveGameData * )pBuf;

		d_distance						= pLoadFrom->d_distance;
		d_timeToReset						= pLoadFrom->d_timeToReset;
		d_orgSpeedTarget					= pLoadFrom->d_orgSpeedTarget;
		d_fSpeedTarget					= pLoadFrom->d_fSpeedTarget;
		d_fSpeedCurrent					= pLoadFrom->d_fSpeedCurrent;
		d_fAccelerationMaximum			= pLoadFrom->d_fAccelerationMaximum;
		d_fTurningGeesMaximum			= pLoadFrom->d_fTurningGeesMaximum;
		d_vDirectionCurrent				= pLoadFrom->d_vDirectionCurrent;
		d_vPosition						= pLoadFrom->d_vPosition;
		d_Orientation					= pLoadFrom->d_Orientation;
		d_fResetDelay					= pLoadFrom->d_fResetDelay;
		d_stopDistance					= pLoadFrom->d_stopDistance;
		d_slowingSpeed					= pLoadFrom->d_slowingSpeed;
		d_pauseCounter					= pLoadFrom->d_pauseCounter;
		d_pathFollowerState				= pLoadFrom->d_pathFollowerState;
		d_bCircuitComplete				= pLoadFrom->d_bCircuitComplete;
		d_bAtEndpoint					= pLoadFrom->d_bAtEndpoint;
		d_bKeepOriginalOrient			= pLoadFrom->d_bKeepOriginalOrient;
		d_bReverseAtEnd					= pLoadFrom->d_bReverseAtEnd;
		d_bFaceMovementDirection		= pLoadFrom->d_bFaceMovementDirection;
		d_bSegmentPathMode				= pLoadFrom->d_bSegmentPathMode;
		d_bDropOnGround					= pLoadFrom->d_bDropOnGround;
		d_bRemoveAtPathEnd				= pLoadFrom->d_bRemoveAtPathEnd;
		d_bIsEvacuator					= pLoadFrom->d_bIsEvacuator;
		d_isOnPath						= pLoadFrom->d_isOnPath;
		d_isCushioned					= pLoadFrom->d_isCushioned;
	}

	return( CPathFollower::GetSaveGameMemoryNeeded( ) );
}
