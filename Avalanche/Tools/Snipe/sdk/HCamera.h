#pragma once

#include "HSceneObject.h"

class HFloatProperty;
class HBoolProperty;

class SNIPEEXPORT HCamera : public HSceneObject
{
public:
   // Properties
   HBoolProperty  *GetPerspective();
   HFloatProperty *GetFOV();
	HFloatProperty *GetFocusDistance();
   HBoolProperty  *GetManualClip();
   HFloatProperty *GetNearClip();
   HFloatProperty *GetFarClip();
   HFloatProperty *GetAspectRatio();
};