#include "stdafx.h"
#include "HMotionCurveDrivers.h"
#include "Properties/MotionCurveDrivers.h"

// Float Driver
/////////////////////////////////////////////////////////

HFloatMotionKey *HFloatMotionCurveDriver::StoreValue( float value, const Time &time, BOOL bStoreUndo/*=TRUE*/ )
{
   return (HFloatMotionKey*)((FloatMotionCurveDriver*)this)->StoreValue(value,time,bStoreUndo);
}

float HFloatMotionCurveDriver::GetValue( const Time &time) const
{
   return ((FloatMotionCurveDriver*)this)->GetValue(time);
}

// Vector Driver
/////////////////////////////////////////////////////////

void HVectorMotionCurveDriver::StoreValue( Vector value, const Time &time, BOOL bStoreUndo/*=TRUE*/ )
{
   ((VectorMotionCurveDriver*)this)->StoreValue(value,time,bStoreUndo);
}

Vector HVectorMotionCurveDriver::GetValue( const Time &time) const
{
   return ((VectorMotionCurveDriver*)this)->GetValue(time);
}

// Vector2 Driver
/////////////////////////////////////////////////////////

void HVector2MotionCurveDriver::StoreValue( Vector2 value, const Time &time, BOOL bStoreUndo/*=TRUE*/ )
{
   ((Vector2MotionCurveDriver*)this)->StoreValue(value,time,bStoreUndo);
}

Vector2 HVector2MotionCurveDriver::GetValue( const Time &time) const
{
   return ((Vector2MotionCurveDriver*)this)->GetValue(time);
}

// Euler Driver
/////////////////////////////////////////////////////////

// Quaternion Driver
/////////////////////////////////////////////////////////

void HQuaternionMotionCurveDriver::StoreValue( Quaternion value, const Time &time, BOOL bStoreUndo/*=TRUE*/ )
{
   ((QuaternionMotionCurveDriver*)this)->StoreValue(value,time,bStoreUndo);
}

Quaternion HQuaternionMotionCurveDriver::GetValue( const Time &time) const
{
   return ((QuaternionMotionCurveDriver*)this)->GetValue(time);
}
