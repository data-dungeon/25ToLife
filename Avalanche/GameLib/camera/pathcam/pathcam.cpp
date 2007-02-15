///////////////////////////////////////////////////////////////////////////////
// A simple Target cam
///////////////////////////////////////////////////////////////////////////////
#include "camera/CameraPCH.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PathWTVC::PathWTVC()
{
	Setup ( Vector3 (2.0f, 1.0f, 0.0f),
			  Vector3 (0.0f, 1.0f, 0.0f) );

	m_focalPoint = m_Target;
	m_eye = m_Pos;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PathWTVC::Setup(Vector3 Pos, Vector3 Target)
{
	m_Pos = Pos;
	m_Target = Target;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PathWTVC::Update( Camera &parent )
{
	Vector3		vTarget;
	PathCamera	&pathCam = ((PathCamera &)parent);
	SplinePath	*path = pathCam.GetPath();
	float			distance = pathCam.GetDistance();;

	// update the distance along the track here.
	pathCam.SetDistance( pathCam.GetDistance() + pathCam.GetSpeed() * g_timer.GetFrameSec() );
	if( pathCam.GetDistance() >= path->Length() )
	{
		//If this is a closed loop, then we don't want to stop following
		if ( path->ClosedLoop() )
		{
			pathCam.SetDistance( path->WrapDistance( pathCam.GetDistance() ) ); 
			pathCam.SetFollowingPath( true );
		} else
		{
			pathCam.SetFollowingPath( false );
			pathCam.SetDistance( path->Length() );
		}
	}
	else
	{
		pathCam.SetFollowingPath( true );
	}

	// Compute the camera matrix.
	m_eye = path->PointFromDistance( pathCam.GetDistance() );
	m_eye.y( m_eye.y() + pathCam.GetHeightAbovePath() );

	//See if we're at the end of our path
	if( pathCam.IsFollowingPath() )
	{
		//See if we have a specific End Target
		if( pathCam.GetEndTarget() != NULL )
		{
			vTarget = pathCam.GetEndTarget()->GetBodyInWorld();

			//Do a smooth transition to the new target
			if( pathCam.GetTransitioned() && ( pathCam.GetCurTarget() != vTarget ) )
			{
				Vector3 vTransVec;
				Segment sSeg;

				vTransVec = vTarget - pathCam.GetCurTarget();
				vTransVec.SafeNormalize();
				vTarget = pathCam.GetCurTarget() + ( vTransVec * 0.20f );

				sSeg.start = vTarget;
				sSeg.end = pathCam.GetCurTarget();
				if( sSeg.Length() < 0.20f )
				{
					pathCam.SetTransitioned( true );
				}
			}
		}
		else if( pathCam.GetTarget() != NULL )
		{
			vTarget = pathCam.GetTarget()->GetBodyInWorld();
		}
		else if( pathCam.isTargetingPoint() )
		{
			vTarget = *m_pTargetPoint;
		}
		else
		{
			vTarget = m_eye + path->TangentFromDistance( pathCam.GetDistance() );
		}
	}
	else
	{
		pathCam.SetTransitioned( false );
		if( pathCam.GetTarget() != NULL )
		{
			vTarget = pathCam.GetTarget()->GetBodyInWorld();
		}
		else if( pathCam.isTargetingPoint() )
		{
			vTarget = *m_pTargetPoint;
		}
		else
		{
			vTarget = m_eye + path->TangentFromDistance( pathCam.GetDistance() );
		}
	}

//	Vector3 up( 0.0f, 1.0f, 0.0f );
	m_focalPoint = vTarget + *pathCam.GetTargetOffset();
	pathCam.SetCurTarget( m_focalPoint );
}

void PathCamera::Reset( void )
{
	m_HeightAbovePath	= 0.0f;
	targetOffset = Vector3( 0 , 0 , 0 );
	distance = 0.0f;
	bTargetAtEnd = false;
	bFollowing = true;
	bTransitioned = false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PathProjection::PathProjection()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PathProjection::Update( Camera &parent )
{
//	SetHither( .1 , true);

	DefaultProjectionController::Update(parent);
}

///////////////////////////////////////////////////////////////////////////////
// Cameras must have at least a name and w2v controller
///////////////////////////////////////////////////////////////////////////////

PathCamera::PathCamera( const char *name , ViewportController *viewportC) 
								: Camera( name , &m_target , &m_pathProj , viewportC ) , m_pathProj()
{
	Reset();
	bTargetingPoint = false;
	m_Speed = 0.0f;
	m_HeightAbovePath = 0.0f;
	targetOffset = Vector3( 0 , 0 , 0 );
	path = NULL;
	pTarget = NULL;
	pEndTarget = NULL;
	SetEyePointFilter(0.0f);
	SetFocalPointFilter(0.0f);
	m_target.ClearTarget();
	vCurTarget.Clear();
}

//PathCamera::PathCamera( const char *name , ProjectionController *projectionC , ViewportController *viewportC) 
//								: Camera( name , &m_target , projectionC , viewportC )
//{
//	Reset();
//	bTargetingPoint = false;
//	m_Speed = 0.0f;
//	m_HeightAbovePath = 0.0f;
//	targetOffset = Vector3( 0 , 0 , 0 );
//	path = NULL;
//	pTarget = NULL;
//	pEndTarget = NULL;
//	SetEyePointFilter(0.0f);
//	SetFocalPointFilter(0.0f);
//	m_target.ClearTarget();
//	vCurTarget.Clear();
//}

