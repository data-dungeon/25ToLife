#include "stdafx.h"
#include "HCamera.h"
#include "Camera.h"

// Properties
HBoolProperty *HCamera::GetPerspective()
{
   return (HBoolProperty *) &((Camera *)this)->m_bPerspective;
}

HFloatProperty *HCamera::GetFOV()
{
   return (HFloatProperty *) &((Camera *)this)->m_fFOV;
}

HFloatProperty *HCamera::GetFocusDistance()
{
   return (HFloatProperty *) &((Camera *)this)->m_fFocusDistance;
}

HBoolProperty *HCamera::GetManualClip()
{
   return (HBoolProperty *) &((Camera *)this)->m_bManualClip;
}

HFloatProperty *HCamera::GetNearClip()
{
   return (HFloatProperty *) &((Camera *)this)->m_fNearClip;
}

HFloatProperty *HCamera::GetFarClip()
{
   return (HFloatProperty *) &((Camera *)this)->m_fFarClip;
}

HFloatProperty *HCamera::GetAspectRatio()
{
   return (HFloatProperty *) &((Camera *)this)->m_fAspectRatio;
}

