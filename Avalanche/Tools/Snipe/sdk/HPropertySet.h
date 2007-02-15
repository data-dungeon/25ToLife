#pragma once

#include "HHierObject.h"
#include "HContainerObject.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HUV;
class HUVPoly;
class HPoly;
class HPropertyPoly;

class SNIPEEXPORT HPropertySet : public HHierObject
{
public:
   POSITION GetHeadPropertyPolyPosition();
   HPropertyPoly *GetNextPropertyPoly(POSITION &pos);
};
