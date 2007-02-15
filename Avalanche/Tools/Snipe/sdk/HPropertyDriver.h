#pragma once

#include "HHierObject.h"

class HPropertyPointerProperty;

class SNIPEEXPORT HPropertyDriver : public HHierObject
{
public:
   HPropertyPointerProperty *GetProperty();
   HSnipeObject *GetObjectDriven();
};
