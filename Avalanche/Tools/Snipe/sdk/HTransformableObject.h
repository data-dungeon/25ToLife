#pragma once

#include "HHierObject.h"

class HColorProperty;
class HTranslateProperty;
class HRotateProperty;
class HFloatProperty;
class HTransformProperty;
class HBoolProperty;
class HTypeProperty;

class SNIPEEXPORT HTransformableObject : public HHierObject
{
public:
   HColorProperty     *GetBoneColor();
   HBoolProperty      *GetChained();
   HBoolProperty      *GetLockIK();
   HTranslateProperty *GetStart();
   HTranslateProperty *GetEnd();
   HRotateProperty    *GetRotate();
   HFloatProperty     *GetLength();
   HTypeProperty      *GetRollMethod(); enum { RM_ZSINGULARITY, RM_YPOLESSINGULARITY, RM_ROLLHISTORY };
   HTypeProperty      *GetManipulationMethod(); enum { MM_STANDARD, MM_TRANSLATEONLY, MM_ROTATEONLY };
   HBoolProperty      *GetLimitManipulators();
   HBoolProperty      *GetAllowXTranslate();
   HBoolProperty      *GetAllowYTranslate();
   HBoolProperty      *GetAllowZTranslate();
   HBoolProperty      *GetAllowXScale();
   HBoolProperty      *GetAllowYScale();
   HBoolProperty      *GetAllowZScale();
   HBoolProperty      *GetAllowXRotate();
   HBoolProperty      *GetAllowYRotate();
   HBoolProperty      *GetAllowZRotate();
   HTransformProperty *GetTransform();
   HBoolProperty      *GetVisible();
   BOOL IsBone();
   UINT GetNumTransformableObjects();
   HTransformableObject *GetTransformableObjectAt(UINT nIndex);
};