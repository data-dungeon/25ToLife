/**********************************************/
/*                                            */
/* PathFollower.h                             */
/* big juju							                */
/* originally by shon love                    */
/* cleaned by ned martin  avalanche software  */
/* visual c++ 6.0                             */
/*                                            */
/**********************************************/

#ifndef _PATHFOLLOWER_H
#define _PATHFOLLOWER_H

#include "Components/Component.h"
#include "Math/Vector.h"
#include "Math/Quatern.h"
#include "linearAlgebra/cquat.h"
#include "path/PathCollection.h"

/* forward declarations */

class CCompActor;

class CPathFollower : public CActorComponent
{
public:
	CPathFollower( CCompActor& i_actor );
	virtual     ~CPathFollower();

	const char  *ComponentName() { return "PathFollower"; }
	const char  *ComponentClass() { return "KinematicDriver"; }

	void         Setup();

	// place the actor at the current path position
	void         PlaceActor( void );

	// move path follower along path
	bool         FollowPath( float i_deltaSec );

	// move path follower along path with acceleration
	bool         FollowAccelPath( float i_deltaSec);

	// move path follower along arbitrary direction vector
	Vector3      FollowDirection( float i_deltaSec, Vector3 &i_direction );

	// drop the path follower to the terrain - returns surface normal
	Vector3      DropOnTerrain( void );

	// orient the object
	void         Orient( void );

	// proceed on the path for a fixed distance	
	void         ProceedAlongPathFixed( float i_distance );

	// proceed on the path with the current parameters
	void         ProceedAlongPath( float i_deltaSec );

	// adjust our speed for turns
	void         AdjustSpeed( float i_deltaSec );

	// change the path follower's path to the new path at point k,t
	bool               SetPath( SplinePath *i_path );
	inline SplinePath* GetPath( void )                  { return( d_pPath ); };

	// reset the path to the given values
	void         SetPathDistance( float newDistance )   { d_distance = newDistance; }
	float        GetPathDistance( void )                { return(d_distance); }

	// reset the distance from T (between 0 and 1)
	void         SetDistanceFromT( float i_T );
	float        GetPathDistanceFromT( void )           { return( d_distance / d_pPath->Length() ); }

	// set to the closest position along the path to the given point
	void         SetPosition( Vector3 &pos )            { d_distance = d_pPath->NearestDistance(pos); }
	Vector3      GetPosition()                          { return(d_vPosition); }

	Vector3      GetDirectionCurrent()                  { return(d_vDirectionCurrent); }
	void         SetDirectionCurrent(Vector3 i_dc)      { d_vDirectionCurrent = i_dc; }

	float        GetOrgSpeedTarget()                    { return(d_orgSpeedTarget); }

	float        GetSpeedTarget()                       { return(d_fSpeedTarget); }
	void         SetSpeedTarget(float i_speedtarget)    { d_fSpeedTarget = i_speedtarget; }

	float        GetSpeedCurrent()                      { return(d_fSpeedCurrent); }
	void         SetSpeedCurrent(float i_speedcurrent)  { d_fSpeedCurrent = i_speedcurrent; }

	float        GetAccelerationMaximum()               { return(d_fAccelerationMaximum); }
	void         SetAccelerationMaximum(float i_aclMax) { d_fAccelerationMaximum = i_aclMax; }

	float        GetTurningGeesMaximum()                { return(d_fTurningGeesMaximum); }

	Quat         GetOrientation()                       { return(d_Orientation); }
	void         SetOrientation(Quat i_orientation)     { d_Orientation = i_orientation; }

	float        GetResetDelay()                        { return(d_fResetDelay); }

	// attempt to speed up as fast as possible
	float        SpeedUp( float i_deltaSec );
	float        SlowDown( float i_deltaSec );

	// move along path
	void         Go( float i_deltaSec );

	// stop at current position
	void         Stop( float i_deltaSec );

	// reverse speed
	void         Reverse( float i_deltaSec );

	// build a quaternion that represents making the normal vector the up vector in x,z plane
	static ts_Quatvw     QuatFromNormalAsPlane( Vector3 i_normal );
	static ts_Quatvw     Quat_FromZToDirection( Vector3 i_direction );

	// proceed on the path with the current parameters
	void         ProceedAlongPathClosed( float i_deltaSec );

	// randomize the original spline path follower is assigned to
	void         RandomizePath();

	// create/delete a new path from scratch for the path follower to follow
	void         CreatePathFromScratch(Vector3 *pvPoints, int nPointCount, bool bForceNoLoop = false);
	void         DeletePathFromScratch();

	// checks cushion around path follower for potential collisions and redirects if necessary
	bool         FindObstacle( Vector3 &o_point, Vector3 &o_normal );
	void         TakeDetour( float i_deltaSec, Vector3 i_point, Vector3 i_normal );
	void         ReturnToPath( float i_deltaSec );

	// hooks to update the actor
	virtual void Activate( void );
	virtual void Deactivate( void );
	virtual void BeginUpdate( void );
	virtual void AttemptUpdate( float i_deltaSec );

	/* flag setting/testing */

	inline const void SetCircuitComplete(bool bState)			{ d_bCircuitComplete = bState; }
	inline const bool IsCircuitComplete() const					{ return(d_bCircuitComplete); }

	inline const void SetAtEndpoint(bool bState)					{ d_bAtEndpoint = bState; }
	inline const bool IsAtEndpoint() const							{ return(d_bAtEndpoint); }

	inline const void SetKeepOriginalOrient(bool bState)		{ d_bKeepOriginalOrient = bState; }
	inline const bool IsKeepOriginalOrient() const				{ return(d_bKeepOriginalOrient); }

	inline const void SetReverseAtEnd(bool bState)				{ d_bReverseAtEnd = bState; }
	inline const bool IsReverseAtEnd() const						{ return(d_bReverseAtEnd); }

	inline const void SetFaceMovementDirection(bool bState)	{ d_bFaceMovementDirection = bState; }
	inline const bool IsFaceMovementDirection() const			{ return(d_bFaceMovementDirection); }

	inline const void SetSegmentPathMode(bool bState)			{ d_bSegmentPathMode = bState; }
	inline const bool IsSegmentPathMode() const					{ return(d_bSegmentPathMode); }

	inline const void SetDropOnGround(bool bState)				{ d_bDropOnGround = bState; }
	inline const bool IsDropOnGround() const						{ return(d_bDropOnGround); }

	inline const void SetRemoveAtPathEnd(bool bState)        { d_bRemoveAtPathEnd = bState; }
	inline const bool IsRemoveAtPathEnd()                    { return(d_bRemoveAtPathEnd); }

	inline const void SetIsEvacuator(bool bState)            { d_bIsEvacuator = bState; }
	inline const bool IsEvacuator()                          { return(d_bIsEvacuator); }

	// cushioning flag setting/testing

	inline const void SetIsOnPath( bool bState )					{ d_isOnPath = bState; }
	inline const bool IsOnPath() const								{ return (d_isOnPath); }

	inline const void SetIsCushioned( bool bState )				{ d_isCushioned = bState; }
	inline const bool IsCushioned() const							{ return (d_isCushioned); }

	// state of AiBehavior for use in dead-reckoned model

	inline const void SetState(int i_state)                  { d_state = i_state; }
	inline const int GetState()                              { return (d_state); }

#ifndef CONSUMER_BUILD
	// debug
	inline void	Draw3D()	{ d_pPath->Draw(); }
#endif //CONSUMER_BUILD

protected:

	// can this path follower make the next turn at current velocity?
	bool						CanMakeTurn( float i_deltaSec );

	// the path we're following
	SplinePath				*d_pPath;

	// when a path follower randomizes a path, he is responsible for deleting it. only ONE path can be randomized,
	// note that path follower may be following another path, e.g. d_pPath != d_pRandomizedPath
	SplinePath				*d_pRandomizedPath;

	// a path follower can be told to create a new path from scratch, too, at which point he is responsible for i
	SplinePath				*d_pPathFromScratch;

	// position along spline path
	float						d_distance;

	// count down to restarting path
	float						d_timeToReset;

	// record the original speedTarget
	float						d_orgSpeedTarget;

	/* these fields used to be in the ts_PathFollower structure. ts_PathFollower records used to be able to be defined */
	/* in the .dbl file, but no longer-- now they are just initialization parameters, so we can set them directly */

	float						d_fSpeedTarget;			// desired speed					meters/second
	float						d_fSpeedCurrent;			// current speed					meters/second
	float						d_fAccelerationMaximum;	// maximum acceleration rate	meters/second/second
	float						d_fTurningGeesMaximum;	// maximum turning rate	in Gs	meters/second/second
	Vector3					d_vDirectionCurrent;		// current direction				normalized velocity vector
	Vector3					d_vPosition;				// current position				meters
	ts_Quatvw				d_Orientation;				// current orientation			radians
	float						d_fResetDelay;				// how long to wait between path traversals - in seconds; 0.0 = looping

	float						d_stopDistance;
	float						d_slowingSpeed;
	float						d_pauseCounter;

	enum ePathFollowerState { pPAUSE, pFOLLOWPATH, pSTOPFORWARD, pFOLLOWREVERSE, pSTOPREVERSE };
	unsigned int			d_pathFollowerState;

	// flags-- compiler will collect these into an int

	bool						d_bCircuitComplete;			// track path roll-over
	bool						d_bAtEndpoint;					// track endpoint arrivals
	bool						d_bKeepOriginalOrient;		// keep original orientation-- don't align to path
	bool						d_bReverseAtEnd;				// reverse path when we reach the end
	bool						d_bFaceMovementDirection;	// always face in direction of travel
	bool						d_bSegmentPathMode;			// on/off screen - dummy down path follower
	bool						d_bDropOnGround;				// place on ground below path
	bool                 d_bRemoveAtPathEnd;        // remove the actor when it reaches the path end
	bool                 d_bIsEvacuator;            // is the actor an evacuator

	// variables to handle frame updating
	float						d_beginDistance;
	float						d_beginSpeedCurrent;
	Vector3					d_beginBodyInWorld;
	Vector3					d_beginVelocity;
	DirCos3x3				d_beginBodyToWorld;

	// variables to be used with detours
	bool						d_isOnPath;                // currently moving around obstructive object
	bool						d_isCushioned;             // use padding for this actor

	// variable used by dead-reckoned model to know state of AiBehavior
	int                  d_state;                   // state of the pathfollower

// operations

public:

	/* overload = operator to make copying easy */
	CPathFollower &operator=( CPathFollower & );

	// ----------- Save/Load Game -------------
	struct pathfollowerSaveGameData
	{
		float					d_distance;
		float					d_timeToReset;
		float					d_orgSpeedTarget;

		float					d_fSpeedTarget;			// desired speed					meters/second
		float					d_fSpeedCurrent;			// current speed					meters/second
		float					d_fAccelerationMaximum;	// maximum acceleration rate	meters/second/second
		float					d_fTurningGeesMaximum;	// maximum turning rate	in Gs	meters/second/second
		Vector3				d_vDirectionCurrent;		// current direction				normalized velocity vector
		Vector3				d_vPosition;				// current position				meters
		ts_Quatvw			d_Orientation;				// current orientation			radians
		float					d_fResetDelay;				// how long to wait between path traversals - in seconds; 0.0 = looping

		float					d_stopDistance;
		float					d_slowingSpeed;
		float					d_pauseCounter;
		unsigned int		d_pathFollowerState;

		bool					d_bCircuitComplete;			// track path roll-over
		bool					d_bAtEndpoint;					// track endpoint arrivals
		bool					d_bKeepOriginalOrient;		// keep original orientation-- don't align to path
		bool					d_bReverseAtEnd;				// reverse path when we reach the end
		bool					d_bFaceMovementDirection;	// always face in direction of travel
		bool					d_bSegmentPathMode;			// on/off screen - dummy down path follower
		bool					d_bDropOnGround;				// place on ground below path
		bool					d_bRemoveAtPathEnd;			// remove the actor when it reaches the path end
		bool					d_bIsEvacuator;				// is the actor an evacuator
		bool					d_isOnPath;						// currently moving around obstructive object
		bool					d_isCushioned;					// use padding for this actor
	};

	u32 GetSaveGameMemoryNeeded( );
	u32 SerializeSaveGame( char *pBuf, bool bDirection );

	friend class DRPathFollowerModel;
	friend class DRPathFollowerComponent;
};

#endif
