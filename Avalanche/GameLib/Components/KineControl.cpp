#include "Components/ComponentsPCH.h"

#include "KineControl.h"

AxisRange::AxisRange( float i_max )
{ 
	mirrored = true; 
	max = i_max; 
	min = -i_max; 
	centerAtZero = true; 
}

AxisRange::AxisRange( float i_min, float i_max, bool i_centerAtZero ) 
{ 
	mirrored = false; 
	max = i_max; 
	min = i_min; 
	centerAtZero = i_centerAtZero; 
}

float AxisRange::DeNormalize( float f ) 
{ 
	float denormalized;
	if( centerAtZero ) 
	{
		if( f < 0.0f ) 
			denormalized = f * -Min(); 
		else 
			denormalized = f * Max(); 
	}
	else
	{
		denormalized = Min() + ( (Max() - Min()) * f );
	}
	return denormalized;
}

void KinematicController::SetAxisNormalized( int axis, float position )
{
	// Assuming that the minimum is negative and the max is positive.
	// They don't both have to be the same value, but zero should always
	// normalize to zero, so a centered stick position does nothing
	ASSERT( GetAxisRange( axis ).Max() >= 0.0f && GetAxisRange( axis ).Min() <= 0.0f );
	SetAxis( axis, GetAxisRange( axis ).DeNormalize( position ) );
}
