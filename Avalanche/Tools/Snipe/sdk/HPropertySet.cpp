#include "stdafx.h"
#include "HPropertySet.h"
#include "PropertySet.h"

POSITION HPropertySet::GetHeadPropertyPolyPosition()
{
   return ((PropertySet*)this)->GetHeadPropertyPolyPosition();
}

HPropertyPoly *HPropertySet::GetNextPropertyPoly(POSITION &pos)
{
   return (HPropertyPoly*)((PropertySet*)this)->GetNextPropertyPoly(pos);
}