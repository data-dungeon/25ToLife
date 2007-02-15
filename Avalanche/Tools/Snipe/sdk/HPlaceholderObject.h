#pragma once

#include "HHierObject.h"

class String;

class SNIPEEXPORT HPlaceholderObject : public HHierObject
{
public:
   const String &GetFileText() const;
};