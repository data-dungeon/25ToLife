#pragma once

#include "HPropertyDriver.h"
//#include "../Utility/ATime.h"

class HMotionCurve;
class HFloatProperty;

class SNIPEEXPORT HMotionCurveDriver : public HPropertyDriver
{
public:
   class SNIPEEXPORT HRebuildMotionCurveDriverInfo
   {
   public:
      enum HRebuildMotionCurveType
      {
         LINEAR, CUBIC,
      };
      void Redo();
      void Undo();
      void operator delete (void *pVoid);
   };

   UINT GetNumMotionCurves();;
   HMotionCurve *GetMotionCurveAt( UINT nIndex );
   Time GetStartTime();
   Time GetStopTime();

   void AddModifiedMotionCurves();
   void RemoveModifiedMotionCurves();

   void RebuildMotionCurveDriverIn( float fTolerance );

   HFloatProperty *GetReductionBiasProperty();

   HRebuildMotionCurveDriverInfo *RebuildCurves( float fTolerance, HRebuildMotionCurveDriverInfo::HRebuildMotionCurveType nType );
};

