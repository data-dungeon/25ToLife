#include "stdafx.h"
#include "MotionCurveDrivers.h"
#include "GUI/GLBaseView.h"

/////////////////////////////////////////////////////////
// Float Driver

IMPLEMENT_SNIPEOBJECT_CREATE( FloatMotionCurveDriver )

void FloatMotionCurveDriver::OnPropertyValueStored( const Time &time, BOOL bStoreUndo )
{
   StoreValue( GetProperty()->GetCurrentValue(), time, bStoreUndo );
}

/////////////////////////////////////////////////////////
// Vector Driver

IMPLEMENT_SNIPEOBJECT_CREATE( VectorMotionCurveDriver )

void VectorMotionCurveDriver::OnPropertyValueStored( const Time &time, BOOL bStoreUndo )
{
   VectorProperty *pProperty = GetProperty();
   if (pProperty)
      StoreValue( pProperty->GetCurrentValue(), time, bStoreUndo );
}

/////////////////////////////////////////////////////////
// Vector2 Driver

IMPLEMENT_SNIPEOBJECT_CREATE( Vector2MotionCurveDriver )


/////////////////////////////////////////////////////////
// Euler Driver

IMPLEMENT_SNIPEOBJECT_CREATE( EulerMotionCurveDriver )

/////////////////////////////////////////////////////////
// Quaternion Driver

IMPLEMENT_SNIPEOBJECT_CREATE( QuaternionMotionCurveDriver )

/////////////////////////////////////////////////////////
// String Driver

IMPLEMENT_SNIPEOBJECT_CREATE( StringMotionCurveDriver )