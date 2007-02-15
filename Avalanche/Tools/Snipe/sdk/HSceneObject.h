#pragma once

#include "HTransformableObject.h"

class HMotionInstance;

class SNIPEEXPORT HSceneObject : public HTransformableObject
{
public:
   // Properties
//   HSceneObjectPointerProperty *GetSceneObjectCache();

   // Operations
   void AddMotionInstance( HMotionInstance *pMotion );
};