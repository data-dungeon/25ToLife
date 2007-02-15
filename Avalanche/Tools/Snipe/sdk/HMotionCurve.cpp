#include "stdafx.h"
#include "HMotionCurve.h"
#include "MotionCurve.h"

// MotionCurve
///////////////////////////////////////////////

Time HMotionCurve::GetStartTime()
{
   return ((MotionCurve*)this)->GetStartTime();
}

Time HMotionCurve::GetStopTime()
{
  return ((MotionCurve*)this)->GetStopTime();
}

UINT HMotionCurve::CountNumKeys()
{
   return ((FloatMotionCurve*)this)->CountNumKeys();
}

HMotionKey *HMotionCurve::GetHeadKey() const
{
   return (HMotionKey*)((MotionCurve*)this)->GetHeadKey();
}

HMotionKey *HMotionCurve::GetTailKey() const
{
   return (HMotionKey*)((MotionCurve*)this)->GetTailKey();
}

HMotionKey *HMotionCurve::GetLastKey() const
{
   return (HMotionKey*)((MotionCurve*)this)->GetLastKey();
}

// FloatMotionCurve
///////////////////////////////////////////////

HFloatMotionKey *HFloatMotionCurve::StoreValue( const float &value, const Time &time, BOOL bStoreUndo )
{
   return (HFloatMotionKey*)((FloatMotionCurve*)this)->StoreValue(value,time,bStoreUndo);
}

float HFloatMotionCurve::GetValue( const Time &time )
{
   return ((FloatMotionCurve*)this)->GetValue(time);
}

// StringMotionCurve
///////////////////////////////////////////////

HStringMotionKey *HStringMotionCurve::StoreValue( const String &value, const Time &time, BOOL bStoreUndo )
{
   return (HStringMotionKey*)((StringMotionCurve*)this)->StoreValue(value,time,bStoreUndo);
}

String HStringMotionCurve::GetValue( const Time &time )
{
   return ((StringMotionCurve*)this)->GetValue(time);
}

