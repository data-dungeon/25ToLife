#pragma once

#include "HHierObject.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HDecalMap;
class HDecalStamp;
class HImage;

class SNIPEEXPORT HDecal : public HHierObject
{
public:
#if 0
   HDecalMap *AddMap(const char *name);
   HDecalMap *AddMap(HImage *pImage);
#endif
   HDecalStamp *AddStamp(const char *name);
};