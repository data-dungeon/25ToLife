#include "Interfaces/InterfacesPCH.h"
#include "Interfaces/ViewIF.h"
#include "stage/cactor.h"

uint ViewIF::s_viewID = INTERFACE_ID_INVALID;

ViewIF* ViewIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((ViewIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

ViewIF* ViewIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((ViewIF*)actor->GetInterface(GetID()));
	return(NULL);
}

void ViewIF::LookAt( Vector3CRef lookAtPoint, Vector3CRef up )
{
	DirCos3x3 matrix;
	Vector3 z, eye;

	GetEyepoint( eye );
	z = lookAtPoint - eye;
	if( !z.SafeNormalize() )
	{
		ASSERT( 0 && "Please don't look at the eye point" );
		return;
	}
	if (Math::Zero((z - up).LengthSquared()))
	{
		ASSERT( 0 && "Trying to look straight up" );
		LookDirection( z, Vector3(1.0f, 0.0f, 0.0f) );
		return;
	}
	LookDirection( z, up );
}

void ViewIF::LookDirection( Vector3CRef lookDir, Vector3CRef up )
{
	DirCos3x3 matrix;
	Vector3 x, y, z;

	z = lookDir;
	if( !z.SafeNormalize() )
	{
		ASSERT( 0 && "Specify a direction to look in!" );
		return;
	}
	x = up.Cross( lookDir );
	x.Normalize();
	y = lookDir.Cross( x );
	y.Normalize();

	matrix.SetRow0( x );
	matrix.SetRow1( y );
	matrix.SetRow2( lookDir );

	SetSightMatrix( matrix );
}
