#include "stdafx.h"
#include "HMotionCurveDriver.h"
#include "Properties/MotionCurveDriver.h"

UINT HMotionCurveDriver::GetNumMotionCurves()
{
   return ((MotionCurveDriver*)this)->GetNumMotionCurves();
}

HMotionCurve *HMotionCurveDriver::GetMotionCurveAt( UINT nIndex )
{
   return (HMotionCurve*)((MotionCurveDriver*)this)->GetMotionCurveAt(nIndex);
}

Time HMotionCurveDriver::GetStartTime()
{
   return ((MotionCurveDriver*)this)->GetStartTime();
}

Time HMotionCurveDriver::GetStopTime()
{
   return ((MotionCurveDriver*)this)->GetStopTime();
}

void HMotionCurveDriver::AddModifiedMotionCurves()
{
   ((MotionCurveDriver*)this)->AddModifiedMotionCurves();
}

void HMotionCurveDriver::RemoveModifiedMotionCurves()
{
   ((MotionCurveDriver*)this)->RemoveModifiedMotionCurves();
}

HFloatProperty *HMotionCurveDriver::GetReductionBiasProperty()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HFloatProperty*)((MotionCurveDriver*)this)->GetReductionBiasProperty();
}

HMotionCurveDriver::HRebuildMotionCurveDriverInfo *HMotionCurveDriver::RebuildCurves( float fTolerance, HRebuildMotionCurveDriverInfo::HRebuildMotionCurveType nType )
{
   return (HMotionCurveDriver::HRebuildMotionCurveDriverInfo*)((MotionCurveDriver*)this)->RebuildCurves(fTolerance,(MotionCurveDriver::RebuildMotionCurveDriverInfo::RebuildMotionCurveType)nType);
}

void HMotionCurveDriver::HRebuildMotionCurveDriverInfo::Redo()
{
   ((MotionCurveDriver::RebuildMotionCurveDriverInfo *)this)->Redo();
}

void HMotionCurveDriver::HRebuildMotionCurveDriverInfo::Undo()
{
   ((MotionCurveDriver::RebuildMotionCurveDriverInfo *)this)->Undo();
}

void HMotionCurveDriver::HRebuildMotionCurveDriverInfo::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   MotionCurveDriver::RebuildMotionCurveDriverInfo *pRebuildMotionCurveDriverInfo = (MotionCurveDriver::RebuildMotionCurveDriverInfo *) pVoid;
   delete pRebuildMotionCurveDriverInfo;
}
