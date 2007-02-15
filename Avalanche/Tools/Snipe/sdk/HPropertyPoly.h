#pragma once

#include "HComponent.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HPropertySet;
class HPoly;

class SNIPEEXPORT HPropertyPoly : public HComponent
{
public:
   HPropertyPoly *GetNextPropertyPoly();

   HPoly *GetPoly();

   HPropertySet *GetPropertySet();
};
