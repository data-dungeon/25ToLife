#pragma once

#include "HInstanceObject.h"

class SNIPEEXPORT HParentableObject : public HInstanceObject
{
public:
   void SetParent( HSnipeObject *pParent );
};