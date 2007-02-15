#pragma once

#include "HTransformableObject.h"

class HFileInfoProperty;

class SNIPEEXPORT HScene : public HTransformableObject
{
public:
   HFileInfoProperty *GetFileInfoProperty();
   HHierObject *AddInstance(HHierObject *pHierObject);
};