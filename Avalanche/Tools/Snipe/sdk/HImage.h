#pragma once

#include "HHierObject.h"

class SNIPEEXPORT HImage : public HHierObject
{
public:
   int GetWidth();
   int GetHeight();

   String GetFileName();
};