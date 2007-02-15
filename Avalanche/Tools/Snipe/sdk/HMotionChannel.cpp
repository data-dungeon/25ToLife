#include "stdafx.h"
#include "HMotionChannel.h"
#include "Properties/MotionChannel.h"
#include "Properties/MotionCurveDriver.h"

UINT HMotionChannel::GetNumMotionCurveDrivers()
{
   return ((MotionChannel *)this)->GetNumMotionCurveDrivers();
}

int HMotionChannel::AddMotionCurveDriver( HMotionCurveDriver *pMotionCurveDriver )
{
   return ((MotionChannel *)this)->AddMotionCurveDriver( (MotionCurveDriver *)pMotionCurveDriver );
}

HChannelDriver *HMotionChannel::GetMotionCurveDriverAt( UINT nIndex )
{
   return (HChannelDriver *)((MotionChannel *)this)->GetMotionCurveDriverAt( nIndex );
}

HChannelDriver *HMotionChannel::GetMotionCurveDriver( HProperty *pProperty )
{
   return (HChannelDriver *)((MotionChannel *)this)->GetMotionCurveDriver( (Property *)pProperty );
}

HChannelDriver *HMotionChannel::GetMotionCurveDriver( const char *name )
{
   return (HChannelDriver *)((MotionChannel *)this)->GetMotionCurveDriver( name );
}

void HMotionChannel::ComputeParentToBone(Vector bonedirection,Vector rolldirection,Vector parentbonedirection,Vector parentrolldirection)
{
   ((MotionChannel*)this)->ComputeParentToBone(bonedirection,rolldirection,parentbonedirection,parentrolldirection);
}