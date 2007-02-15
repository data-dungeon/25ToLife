//============================================================================
//=
//= BasicMover.cpp - A component to handle rudamentary things such as rotating,
//=                  scaling, point to point movement, etc.
//=
//============================================================================

#include "Components/ComponentsPCH.h"
#include "Components/ComponentCreator.h"
#include "Components/BasicMover.h"
#include "mathUtil/Interpolate.h"

#include "Script/ScriptSystem.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CBasicMover, BasicMover, COMPONENT_NEUTRAL_PRIORITY);

//============================================================================

CBasicMover::CBasicMover( CCompActor& i_actor ) :
	CActorComponent( i_actor )
{
	d_actionFlags = 0;
}

//============================================================================

CBasicMover::~CBasicMover()
{
}

//============================================================================
void CBasicMover::Activate( void )
{
	GetActor().SyncActor();
	CActorComponent::Activate();
}

//============================================================================

CInterface* CBasicMover::GetInterface( uint i_id )
{
	if ( i_id == CBasicMoverIF::GetID() )
		return (CInterface*) this;
	else
		return NULL;
}

//============================================================================

void CBasicMover::SyncActor(void)
{
	DirCos3x3 &bodyToWorld = actor.GetBodyToWorld();
	d_rotate.X(Math::ArcSin(-bodyToWorld.e21));
	d_rotate.Y(Math::ArcTan2(bodyToWorld.e20, bodyToWorld.e22));
	d_rotate.Z(Math::ArcTan2(bodyToWorld.e01, bodyToWorld.e11));
}

//============================================================================

void CBasicMover::BeginUpdate()
{
	d_beginActionFlags = d_actionFlags;
	d_beginTranslateTime = d_translateTime;
	d_beginRotateTime  = d_rotateTime;
	d_beginBodyInWorld = actor.GetBodyInWorld();
	d_beginRotate      = d_rotate;
}

//============================================================================

void CBasicMover::AttemptTranslate( float i_deltaSec )
{
	if ( d_beginActionFlags & BASICMOVER_TRANSLATE_AT_VELOCITY_X )
	{
		actor.GetBodyInWorld().x( d_beginBodyInWorld.x() + i_deltaSec * actor.GetVelocityInWorld().x() );
	}
	else if ( d_beginActionFlags & BASICMOVER_TRANSLATE_X )
	{
		d_translateTime.x( d_beginTranslateTime.x() + i_deltaSec );
		if ( d_translateTime.x() >= d_translateEndTime.x() )
		{
			d_translateTime.x( d_translateEndTime.x() );
			d_actionFlags &= ~BASICMOVER_TRANSLATE_X;
		}
		if ( d_translateEndTime.x() != 0.0f )
		{
			actor.GetBodyInWorld().x( Interpolate::Cubic( d_translateStart.x(), d_translateEnd.x(), d_translateTime.x() / d_translateEndTime.x() ) );
			actor.GetVelocityInWorld().x( Interpolate::CubicDerivative( d_translateStart.x(), d_translateEnd.x(), 0.0f, 0.0f, d_translateTime.x() / d_translateEndTime.x() ) / d_translateEndTime.x() );
		}
		else
		{
			actor.GetBodyInWorld().x( d_translateEnd.x() );
		}
	}

	if ( d_beginActionFlags & BASICMOVER_TRANSLATE_AT_VELOCITY_Y )
	{
		actor.GetBodyInWorld().y( d_beginBodyInWorld.y() + i_deltaSec * actor.GetVelocityInWorld().y() );
	}
	else if ( d_beginActionFlags & BASICMOVER_TRANSLATE_Y )
	{
		d_translateTime.y( d_beginTranslateTime.y() + i_deltaSec );
		if ( d_translateTime.y() >= d_translateEndTime.y() )
		{
			d_translateTime.y( d_translateEndTime.y() );
			d_actionFlags &= ~BASICMOVER_TRANSLATE_Y;
		}
		if ( d_translateEndTime.y() != 0.0f )
		{
			actor.GetBodyInWorld().y( Interpolate::Cubic( d_translateStart.y(), d_translateEnd.y(), d_translateTime.y() / d_translateEndTime.y() ) );
			actor.GetVelocityInWorld().y( Interpolate::CubicDerivative( d_translateStart.y(), d_translateEnd.y(), 0.0f, 0.0f, d_translateTime.y() / d_translateEndTime.y() ) / d_translateEndTime.y() );
		}
		else
		{
			actor.GetBodyInWorld().y( d_translateEnd.y() );
		}
	}

	if ( d_beginActionFlags & BASICMOVER_TRANSLATE_AT_VELOCITY_Z )
	{
		actor.GetBodyInWorld().z( d_beginBodyInWorld.z() + i_deltaSec * actor.GetVelocityInWorld().z() );
	}
	else if ( d_beginActionFlags & BASICMOVER_TRANSLATE_Z )
	{
		d_translateTime.z( d_beginTranslateTime.z() + i_deltaSec );
		if ( d_translateTime.z() >= d_translateEndTime.z() )
		{
			d_translateTime.z( d_translateEndTime.z() );
			d_actionFlags &= ~BASICMOVER_TRANSLATE_Z;
		}
		if ( d_translateEndTime.z() != 0.0f )
		{
			actor.GetBodyInWorld().z( Interpolate::Cubic( d_translateStart.z(), d_translateEnd.z(), d_translateTime.z() / d_translateEndTime.z() ) );
			actor.GetVelocityInWorld().z( Interpolate::CubicDerivative( d_translateStart.z(), d_translateEnd.z(), 0.0f, 0.0f, d_translateTime.z() / d_translateEndTime.z() ) / d_translateEndTime.z() );
		}
		else
		{
			actor.GetBodyInWorld().z( d_translateEnd.z() );
		}
	}
}

//============================================================================

void CBasicMover::AttemptRotate( float i_deltaSec )
{
	Vector3 vel(0.0f, 0.0f, 0.0f);

	if ( d_beginActionFlags & BASICMOVER_ROTATE_AT_VELOCITY_X )
	{
		vel.x( actor.GetAngVelocityInBody().x() );
		d_rotate.x( d_beginRotate.x() + i_deltaSec * actor.GetAngVelocityInBody().x() );
	}
	else if ( d_beginActionFlags & BASICMOVER_ROTATE_X )
	{
		d_rotateTime.x( d_beginRotateTime.x() + i_deltaSec );
		if ( d_rotateTime.x() >= d_rotateEndTime.x() )
		{
			d_rotateTime.x( d_rotateEndTime.x() );
			d_actionFlags &= ~BASICMOVER_ROTATE_X;
		}

		// check for 0 time.. instant change
		if (d_rotateEndTime.x() != 0.0f)
		{
			d_rotate.x( Math::NormalAngle( Interpolate::Cubic( d_rotateStart.x(), d_rotateEnd.x(), d_rotateTime.x() / d_rotateEndTime.x() ) ) );
			vel.X( Interpolate::CubicDerivative( d_rotateStart.x(), d_rotateEnd.x(), 0.0f, 0.0f, d_rotateTime.x() / d_rotateEndTime.x() ) / d_rotateEndTime.X() );
		}
		else
			d_rotate.x( Math::NormalAngle(d_rotateEnd.x()) );
	}

	if ( d_beginActionFlags & BASICMOVER_ROTATE_AT_VELOCITY_Y )
	{
		vel.y( actor.GetAngVelocityInBody().y() );
		d_rotate.y( d_beginRotate.y() + i_deltaSec * actor.GetAngVelocityInBody().y() );
	}
	else if ( d_beginActionFlags & BASICMOVER_ROTATE_Y )
	{
		d_rotateTime.y( d_beginRotateTime.y() + i_deltaSec );
		if ( d_rotateTime.y() >= d_rotateEndTime.y() )
		{
			d_rotateTime.y( d_rotateEndTime.y() );
			d_actionFlags &= ~BASICMOVER_ROTATE_Y;
		}
		// check for 0 time.. instant change
		if (d_rotateEndTime.y() != 0.0f)
		{
			d_rotate.y( Math::NormalAngle( Interpolate::Cubic( d_rotateStart.y(), d_rotateEnd.y(), d_rotateTime.y() / d_rotateEndTime.y() ) ) );
			vel.Y( Interpolate::CubicDerivative( d_rotateStart.y(), d_rotateEnd.y(), 0.0f, 0.0f, d_rotateTime.y() / d_rotateEndTime.y() ) / d_rotateEndTime.Y() );
		}
		else
			d_rotate.y( Math::NormalAngle(d_rotateEnd.y()) );
	}

	if ( d_beginActionFlags & BASICMOVER_ROTATE_AT_VELOCITY_Z )
	{
		vel.z( actor.GetAngVelocityInBody().z() );
		d_rotate.z( d_beginRotate.z() + i_deltaSec * actor.GetAngVelocityInBody().z() );
	}
	else if ( d_beginActionFlags & BASICMOVER_ROTATE_Z )
	{
		d_rotateTime.z( d_beginRotateTime.z() + i_deltaSec );
		if ( d_rotateTime.z() >= d_rotateEndTime.z() )
		{
			d_rotateTime.z( d_rotateEndTime.z() );
			d_actionFlags &= ~BASICMOVER_ROTATE_Z;
		}
		// check for 0 time.. instant change
		if (d_rotateEndTime.z() != 0.0f)
		{
			d_rotate.z( Math::NormalAngle( Interpolate::Cubic( d_rotateStart.z(), d_rotateEnd.z(), d_rotateTime.z() / d_rotateEndTime.z() ) ) );
			vel.Z( Interpolate::CubicDerivative( d_rotateStart.z(), d_rotateEnd.z(), 0.0f, 0.0f, d_rotateTime.z() / d_rotateEndTime.z() ) / d_rotateEndTime.Z() );
		}
		else
			d_rotate.z (Math::NormalAngle (d_rotateEnd.z()) );
	}

	actor.d_omegaInBody = vel.Z();

	actor.d_bodyToWorld.SetZRotation( d_rotate.z() );	// third applied, roll
	actor.d_omegaInBody += vel.X() * actor.d_bodyToWorld.GetColumn0();

	actor.d_bodyToWorld.CatXRotation( d_rotate.x() );	// second applied, pitch
	actor.d_omegaInBody += vel.Y() * actor.d_bodyToWorld.GetColumn1();

	actor.d_bodyToWorld.CatYRotation( d_rotate.y() );	// first applied, facing
}

//============================================================================

void CBasicMover::AttemptUpdate( float i_deltaSec )
{
	if ( d_beginActionFlags )
	{
		AttemptTranslate( i_deltaSec );
		AttemptRotate( i_deltaSec );
		GetActor().SyncState();
	}
}

//============================================================================

void CBasicMover::EndUpdate( float i_deltaSec )
{
	if (d_actionFlags != d_beginActionFlags)
	{
		actor.SetDiscontinuity(DISCONTINUITY_VELOCITY);
		actor.SetDiscontinuity(DISCONTINUITY_ANGULAR_VELOCITY);
	}
}

//============================================================================

void CBasicMover::RotateTo( Vector3CRef rot, float duration )
{
	RotateToX( rot.X(), duration );
	RotateRelY( rot.Y() - d_rotate.Y(), duration );
	RotateRelZ( rot.Z() - d_rotate.Z(), duration );
}

//============================================================================

void CBasicMover::RotateToX( float rot, float duration )
{
	// x rotations can only go between -pi/2 and pi/2
	ASSERT(Math::Abs(rot) <= Math::HalfPi);
	rot = Math::NormalAngle( rot );
	if (rot > Math::HalfPi)
		rot = Math::HalfPi;
	else if (rot < -Math::HalfPi)
		rot = -Math::HalfPi;

	d_rotateStart.x ( d_rotate.X() );
	d_rotateEnd.x( rot );
	d_rotateTime.x( 0.0f );
	d_rotateEndTime.x( duration );
	d_actionFlags |= BASICMOVER_ROTATE_X;
	actor.SetDiscontinuity(DISCONTINUITY_ANGULAR_VELOCITY);
}

//============================================================================

void CBasicMover::RotateToY( float rot, float duration )
{
	RotateRelY( rot - d_rotate.Y(), duration );
}

//============================================================================

void CBasicMover::RotateToZ( float rot, float duration )
{
	RotateRelZ( rot - d_rotate.Z(), duration );
}

//============================================================================

void CBasicMover::RotateRel( Vector3CRef rot, float duration )
{
	RotateToX( d_rotate.X() + rot.X(), duration );
	RotateRelY( rot.Y(), duration );
	RotateRelZ( rot.Z(), duration );
}

//============================================================================

void CBasicMover::RotateRelX( float rot, float duration )
{
	RotateToX( d_rotate.X() + rot, duration );
}

//============================================================================

void CBasicMover::RotateRelY( float rot, float duration )
{
	rot = Math::NormalAngle( rot );

	d_rotateStart.y( d_rotate.Y() );
	d_rotateEnd.y( d_rotate.Y() + rot );
	d_rotateTime.y( 0.0f );
	d_rotateEndTime.y( duration );
	d_actionFlags |= BASICMOVER_ROTATE_Y;
	actor.SetDiscontinuity(DISCONTINUITY_ANGULAR_VELOCITY);
}	

//============================================================================

void CBasicMover::RotateRelZ( float rot, float duration )
{
	rot = Math::NormalAngle( rot );

	d_rotateStart.z( d_rotate.Z() );
	d_rotateEnd.z( d_rotate.Z() + rot );
	d_rotateTime.z( 0.0f );
	d_rotateEndTime.z( duration );
	d_actionFlags |= BASICMOVER_ROTATE_Z;
	actor.SetDiscontinuity(DISCONTINUITY_ANGULAR_VELOCITY);
}

//============================================================================

void CBasicMover::MoveTo( Vector3CRef pos, float duration )
{
	d_translateStart = actor.GetBodyInWorld();
	d_translateEnd   = pos;
	d_translateTime.Set( 0.0f, 0.0f, 0.0f );
	d_translateEndTime.Set( duration, duration, duration );
	d_actionFlags |= BASICMOVER_TRANSLATE_X | BASICMOVER_TRANSLATE_Y | BASICMOVER_TRANSLATE_Z;
	actor.SetDiscontinuity( DISCONTINUITY_VELOCITY );
}

//============================================================================

void CBasicMover::MoveToX( float pos, float duration )
{
	d_translateStart.x( actor.GetBodyInWorld().x() );
	d_translateEnd.x( pos );
	d_translateTime.x( 0.0f );
	d_translateEndTime.x( duration );
	d_actionFlags |= BASICMOVER_TRANSLATE_X;
	actor.SetDiscontinuity( DISCONTINUITY_VELOCITY );
}

//============================================================================

void CBasicMover::MoveToY( float pos, float duration )
{
	d_translateStart.y( actor.GetBodyInWorld().y() );
	d_translateEnd.y( pos );
	d_translateTime.y( 0.0f );
	d_translateEndTime.y( duration );
	d_actionFlags |= BASICMOVER_TRANSLATE_Y;
	actor.SetDiscontinuity( DISCONTINUITY_VELOCITY );
}

//============================================================================

void CBasicMover::MoveToZ( float pos, float duration )
{
	d_translateStart.z( actor.GetBodyInWorld().z() );
	d_translateEnd.z( pos );
	d_translateTime.z( 0.0f );
	d_translateEndTime.z( duration );
	d_actionFlags |= BASICMOVER_TRANSLATE_Z;
	actor.SetDiscontinuity( DISCONTINUITY_VELOCITY );
}

//============================================================================

void CBasicMover::MoveRel( Vector3CRef pos, float duration )
{
	Vector3 world = ( pos * actor.GetBodyToWorld() ) + actor.GetBodyInWorld();
	MoveTo( world, duration );
}

//============================================================================

void CBasicMover::MoveRelX( float pos, float duration )
{
	Vector3 rel = Vector3( pos, 0.0f, 0.0f );
	Vector3 world = ( rel * actor.GetBodyToWorld() ) + actor.GetBodyInWorld();
	MoveTo( world, duration );
}

//============================================================================

void CBasicMover::MoveRelY( float pos, float duration )
{
	Vector3 rel = Vector3( 0.0f, pos, 0.0f );
	Vector3 world = ( rel * actor.GetBodyToWorld() ) + actor.GetBodyInWorld();
	MoveTo( world, duration );
}

//============================================================================

void CBasicMover::MoveRelZ( float pos, float duration )
{
	Vector3 rel = Vector3( 0.0f, 0.0f, pos );
	Vector3 world = ( rel * actor.GetBodyToWorld() ) + actor.GetBodyInWorld();
	MoveTo( world, duration );
}

//============================================================================

void CBasicMover::MoveAtConstantVelocity( Vector3CRef vel )
{
	if ( !Math::Zero( vel.x() ) )
	{
		actor.GetVelocityInWorld().x( vel.x() );
		d_actionFlags |= BASICMOVER_TRANSLATE_AT_VELOCITY_X;
	}
	else
	{
		d_actionFlags &= ~BASICMOVER_TRANSLATE_AT_VELOCITY_X;
	}

	if ( !Math::Zero( vel.y() ) )
	{
		actor.GetVelocityInWorld().y( vel.y() );
		d_actionFlags |= BASICMOVER_TRANSLATE_AT_VELOCITY_Y;
	}
	else
	{
		d_actionFlags &= ~BASICMOVER_TRANSLATE_AT_VELOCITY_Y;
	}

	if ( !Math::Zero( vel.z() ) )
	{
		actor.GetVelocityInWorld().z( vel.z() );
		d_actionFlags |= BASICMOVER_TRANSLATE_AT_VELOCITY_Z;
	}
	else
	{
		d_actionFlags &= ~BASICMOVER_TRANSLATE_AT_VELOCITY_Z;
	}

	actor.SetDiscontinuity( DISCONTINUITY_VELOCITY );
}

//============================================================================

void CBasicMover::RotateAtConstantVelocity( Vector3CRef vel )
{
	if ( !Math::Zero( vel.x() ) )
	{
		actor.GetAngVelocityInBody().x( vel.x() );
		d_actionFlags |= BASICMOVER_ROTATE_AT_VELOCITY_X;
	}
	else
	{
		d_actionFlags &= ~BASICMOVER_ROTATE_AT_VELOCITY_X;
	}

	if ( !Math::Zero( vel.y() ) )
	{
		actor.GetAngVelocityInBody().y( vel.y() );
		d_actionFlags |= BASICMOVER_ROTATE_AT_VELOCITY_Y;
	}
	else
	{
		d_actionFlags &= ~BASICMOVER_ROTATE_AT_VELOCITY_Y;
	}

	if ( !Math::Zero( vel.z() ) )
	{
		actor.GetAngVelocityInBody().z( vel.z() );
		d_actionFlags |= BASICMOVER_ROTATE_AT_VELOCITY_Z;
	}
	else
	{
		d_actionFlags &= ~BASICMOVER_ROTATE_AT_VELOCITY_Z;
	}

	actor.SetDiscontinuity( DISCONTINUITY_ANGULAR_VELOCITY );
}

//============================================================================
