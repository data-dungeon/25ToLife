#pragma once

#include "HHierObject.h"

class HChannelDriver;
class HProperty;
class HMotionCurveDriver;

class SNIPEEXPORT HMotionChannel : public HHierObject
{
public:
   UINT GetNumMotionCurveDrivers();
   int AddMotionCurveDriver( HMotionCurveDriver *pMotionCurveDriver );
   HChannelDriver *GetMotionCurveDriverAt(UINT nIndex);
   HChannelDriver *GetMotionCurveDriver( HProperty *pProperty );
   HChannelDriver *GetMotionCurveDriver( const char *name );

   void ComputeParentToBone(Vector bonedirection,Vector rolldirection,Vector parentbonedirection,Vector parentrolldirection);
};
