#include "stdafx.h"
#include "HMotionKey.h"
#include "MotionKey.h"

void HMotionKey::SetTime(const Time &time)
{
   ((MotionKey*)this)->SetTime(time);
}

const Time &HMotionKey::GetTime()
{
   return ((MotionKey*)this)->GetTime();
}

HMotionKey *HMotionKey::GetNextKey()
{
   return (HMotionKey*)((MotionKey*)this)->GetNextKey();
}

HMotionKey *HMotionKey::GetPrevKey()
{
   return (HMotionKey*)((MotionKey*)this)->GetPrevKey();
}

/////////////////////////////////////
// HFloatMotionKey

void HFloatMotionKey::SetValue( float value )
{
   ((FloatMotionKey *)this)->SetValue( value );
}

float HFloatMotionKey::GetValue()
{
   return ((FloatMotionKey*)this)->GetValue();
}

void HFloatMotionKey::SetInTangent( const Vector2 &tangent )
{
   // TODO: compute bias and magnitude and set them
   ((FloatMotionKey *)this)->SetInTangent( tangent );
}

Vector2 HFloatMotionKey::GetInTangent()
{
   return ((FloatMotionKey *)this)->GetInTangent();
}

void HFloatMotionKey::SetOutTangent( const Vector2 &tangent )
{
   // TODO: compute bias and magnitude and set them
   ((FloatMotionKey *)this)->SetOutTangent( tangent );
}

Vector2 HFloatMotionKey::GetOutTangent()
{
   return ((FloatMotionKey *)this)->GetOutTangent();
}

void HFloatMotionKey::SetInterpolationMethod( InterpolationMethod interpolationmethod )
{
   ((FloatMotionKey *)this)->SetInterpolationMethod( (FloatMotionKey::InterpolationMethod)interpolationmethod );
}

HFloatMotionKey::InterpolationMethod HFloatMotionKey::GetInterpolationMethod()
{
   return (HFloatMotionKey::InterpolationMethod)((FloatMotionKey *)this)->GetInterpolationMethod();
}

/////////////////////////////////////
// HStringMotionKey

void HStringMotionKey::SetValue( const String &value )
{
   ((StringMotionKey *)this)->SetValue( value );
}

String HStringMotionKey::GetValue()
{
   return ((StringMotionKey*)this)->GetValue();
}

