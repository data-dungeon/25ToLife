///////////////////////////////////////////////////////////////////////////////
// A path following cam
///////////////////////////////////////////////////////////////////////////////
#ifndef PATHCAM_H
#define PATHCAM_H

#include "camera/camera.h"

#include "path/SplinePath.h"
//class SplinePath;

class PathWTVC : public FlexTargetW2VC
{
public:
	PathWTVC();
	~PathWTVC() {}

	// Setup the parameters
	void Setup(Vector3 Pos, Vector3 Target);

  	// Update 
	void Update(Camera &parent);

	// Focal point
	Vector3 FocalPoint()	{ return m_focalPoint; }

  // Eye position
	Vector3 Eye() { return m_eye; }

	void	SetTargetPoint( Vector3 *v ) { m_pTargetPoint = v; }

private:
	Vector3 		m_eye;
	Vector3		m_focalPoint;
	Vector3		m_Pos;
	Vector3		m_Target;
	Vector3		*m_pTargetPoint;
};

////////////////////////////////////////////////////////////////
// General projection controller
////////////////////////////////////////////////////////////////
class PathProjection : public DefaultProjectionController
{
public:
	PathProjection();
	~PathProjection() {}

  	// Update
	void Update(Camera &parent);
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class PathCamera : public Camera
{
public:
	// Cameras must have at least a name and w2v controller
	//PathCamera( const char *name , ProjectionController *projectionController = NULL , ViewportController *viewportController = NULL );
	PathCamera( const char *name , ViewportController *viewportC = NULL); 

	// Set the parameters
	bool			IsFollowingPath( void ) { return( bFollowing ); };
	void			SetFollowingPath( bool f ) { bFollowing = f; };

	void			SetTarget( CActor *pActor ) { pTarget = pActor; };
	CActor		*GetTarget( void ) { return pTarget; }

	void			SetTargetOffset( Vector3CRef v ) { targetOffset = v; };
	Vector3		*GetTargetOffset( void ) { return &targetOffset; }

	void			SetEndTarget( CActor *pActor) { pEndTarget = pActor; };
	CActor		*GetEndTarget( void ) { return pEndTarget; }

	void			SetCurTarget( Vector3 &v ) { vCurTarget = v; };
	Vector3		&GetCurTarget( void ) { return vCurTarget; }

	void			Setup( Vector3 Pos , Vector3 Target ) { m_target.Setup( Pos , Target ); }

	bool			SetPath( SplinePath *sp ) { ASSERT( sp ); path = sp; distance = path->Length(); return true; }

	float			GetDistance( void ) { return distance; }
	void			SetDistance( float d ) { distance = d; }

	float			GetSpeed( void ) { return m_Speed; }
	void			SetSpeed( float d ) { m_Speed = d; }

	SplinePath	*GetPath( void ) { return path; }

	float			GetHeightAbovePath( void ) { return m_HeightAbovePath; }
	void			SetHeightAbovePath( float d ) { m_HeightAbovePath = d; }

	bool			GetTransitioned( void ) { return bTransitioned; };
	void			SetTransitioned( bool f ) { bTransitioned = f; };

	void			Reset( void );

	void			SetTargetPoint( Vector3 *v ) { m_target.SetTargetPoint( v ); }
	bool			isTargetingPoint( void ) { return bTargetingPoint; }
	void			SetTargetingPoint( void ) { bTargetingPoint = true; }
	void			ClearTargetingPoint( void ) { bTargetingPoint = false; }

	void			SetHither( float hither ) { m_pathProj.SetHither( hither ); }
	float			GetHither( void ) { return m_pathProj.Hither(); }

protected:
	PathWTVC		m_target;
	PathProjection m_pathProj;
	SplinePath	*path;
	float			distance;

	float			m_HeightAbovePath;	// meters
	float			m_Speed;					// meters/second
	CActor		*pTarget;				// Target camera points at (NULL is valid - will stay along path)
	CActor		*pEndTarget;			// Target camera points at when it gets to the end of the path
	Vector3		vCurTarget;				// Current Target
	Vector3		targetOffset;
	bool			bTransitioned;
	bool			bTargetAtEnd;			// Trace the path until the end, then look at the target
	bool			bFollowing;				// Whether or not we're currently following a path
	bool			bTargetingPoint;
};

#endif
