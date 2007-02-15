#include "stdafx.h"
#include "HMotionSet.h"
#include "MotionSet.h"

// HMotionPlaceholder
HMotion *HMotionPlaceholder::GetMotion()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotion*)((MotionPlaceholder*)this)->GetMotion();
}

UINT HMotionPlaceholder::GetNumMotionChannelBiasObjects()
{
   return ((MotionPlaceholder*)this)->GetNumMotionChannelBiasObjects();
}

HMotionChannelBiasObject *HMotionPlaceholder::GetMotionChannelBiasObjectAt( UINT nIndex )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotionChannelBiasObject*)((MotionPlaceholder*)this)->GetMotionChannelBiasObjectAt(nIndex);
}

// HMotionChannelBiasObject
HFloatProperty *HMotionChannelBiasObject::GetTranslationBiasProperty()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HFloatProperty*)((MotionChannelBiasObject*)this)->GetTranslationBiasProperty();
}

HFloatProperty *HMotionChannelBiasObject::GetRotationBiasProperty()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HFloatProperty*)((MotionChannelBiasObject*)this)->GetRotationBiasProperty();
}

HFloatProperty *HMotionChannelBiasObject::GetScaleBiasProperty()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HFloatProperty*)((MotionChannelBiasObject*)this)->GetScaleBiasProperty();
}

UINT HMotionChannelBiasObject::GetNumMotionCurveDriverBiasObjects()
{
   return ((MotionChannelBiasObject*)this)->GetNumMotionCurveDriverBiasObjects();
}

HMotionCurveDriver *HMotionChannelBiasObject::GetMotionCurveDriverBiasObjectAt( UINT nIndex )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotionCurveDriver*)((MotionChannelBiasObject*)this)->GetMotionCurveDriverBiasObjectAt(nIndex);
}

// HMotionSet
HFloatProperty *HMotionSet::GetTranslationToleranceProperty()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HFloatProperty*)((MotionSet*)this)->GetTranslationToleranceProperty();
}

HFloatProperty *HMotionSet::GetRotationToleranceProperty()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HFloatProperty*)((MotionSet*)this)->GetRotationToleranceProperty();
}

HFloatProperty *HMotionSet::GetScaleToleranceProperty()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HFloatProperty*)((MotionSet*)this)->GetScaleToleranceProperty();
}

UINT HMotionSet::GetNumMotionPlaceholders()
{
   return ((MotionSet*)this)->GetNumMotionPlaceholders();
}

HMotionPlaceholder *HMotionSet::GetMotionPlaceholderAt(UINT nIndex)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotionPlaceholder*)((MotionSet*)this)->GetMotionPlaceholderAt(nIndex);
}

HHierObject *HMotionSet::AddMotion( HMotion *pMotion )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HHierObject*)((MotionSet*)this)->AddMotion((Motion*)pMotion);
}

UINT HMotionSet::GetNumMotionChannelBiasObjects()
{
   return ((MotionSet*)this)->GetNumMotionChannelBiasObjects();
}

HMotionChannelBiasObject *HMotionSet::GetMotionChannelBiasObjectAt( UINT nIndex )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotionChannelBiasObject*)((MotionSet*)this)->GetMotionChannelBiasObjectAt(nIndex);
}

HMotionChannelBiasObject *HMotionSet::GetMotionChannelBiasObject( const String &strName )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotionChannelBiasObject*)((MotionSet*)this)->GetMotionChannelBiasObject(strName);
}
