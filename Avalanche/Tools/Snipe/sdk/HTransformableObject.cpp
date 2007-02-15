
#include "stdafx.h"
#include "HTransformableObject.h"
#include "TransformableObject.h"

HColorProperty *HTransformableObject::GetBoneColor()
{
   return (HColorProperty *)&((TransformableObject *)this)->m_cBoneColor;
}

HBoolProperty *HTransformableObject::GetChained()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bChained;
}

HBoolProperty *HTransformableObject::GetLockIK()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bLockIK;
}

HTranslateProperty *HTransformableObject::GetStart()
{
   return (HTranslateProperty *)&((TransformableObject *)this)->m_vStart;
}

HTranslateProperty *HTransformableObject::GetEnd()
{
   return (HTranslateProperty *)&((TransformableObject *)this)->m_vEnd;
}

HRotateProperty *HTransformableObject::GetRotate()
{
   return (HRotateProperty *)&((TransformableObject *)this)->m_vRotate;
}

HFloatProperty *HTransformableObject::GetLength()
{
   return (HFloatProperty *)&((TransformableObject *)this)->m_fLength;
}

HTypeProperty *HTransformableObject::GetRollMethod()
{
   return (HTypeProperty *)&((TransformableObject *)this)->m_nRollMethod;
}

HTypeProperty *HTransformableObject::GetManipulationMethod()
{
   return (HTypeProperty *)&((TransformableObject *)this)->m_nManipulationMethod;
}

HBoolProperty *HTransformableObject::GetLimitManipulators()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bLimitManipulators;
}

HBoolProperty *HTransformableObject::GetAllowXTranslate()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bAllowXTranslate;
}

HBoolProperty *HTransformableObject::GetAllowYTranslate()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bAllowYTranslate;
}

HBoolProperty *HTransformableObject::GetAllowZTranslate()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bAllowZTranslate;
}

HBoolProperty *HTransformableObject::GetAllowXScale()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bAllowXScale;
}

HBoolProperty *HTransformableObject::GetAllowYScale()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bAllowYScale;
}

HBoolProperty *HTransformableObject::GetAllowZScale()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bAllowZScale;
}

HBoolProperty *HTransformableObject::GetAllowXRotate()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bAllowXRotate;
}

HBoolProperty *HTransformableObject::GetAllowYRotate()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bAllowYRotate;
}

HBoolProperty *HTransformableObject::GetAllowZRotate()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bAllowZRotate;
}

HTransformProperty *HTransformableObject::GetTransform()
{
   return (HTransformProperty *)&((TransformableObject *)this)->m_transform;
}

HBoolProperty *HTransformableObject::GetVisible()
{
   return (HBoolProperty *)&((TransformableObject *)this)->m_bVisible;
}

BOOL HTransformableObject::IsBone()
{
   return ((TransformableObject*)this)->IsBone();
}

UINT HTransformableObject::GetNumTransformableObjects()
{
   return ((TransformableObject*)this)->GetNumTransformableObjects();
}

HTransformableObject *HTransformableObject::GetTransformableObjectAt(UINT nIndex)
{
   return (HTransformableObject*)((TransformableObject*)this)->GetTransformableObjectAt(nIndex);
}

