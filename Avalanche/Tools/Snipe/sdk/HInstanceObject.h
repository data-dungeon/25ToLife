#pragma once

#include "HSnipeObject.h"

class HPropertyArray;
class HProperty;

class SNIPEEXPORT HInstanceObject : public HSnipeObject
{
public:
   HPropertyArray *GetPropertyArray(); // will never be NULL
   void AddProperty(HProperty *pProperty);

   BOOL IsCache() const;
   BOOL IsInstance() const;
   HInstanceObject *GetRootCache(); // Note: returns this if it is the cache
   HInstanceObject *GetCache(); // Note: returns this if it is the cache (only returns one cache level up)
};