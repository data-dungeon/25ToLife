#include "stdafx.h"
#include "HPropertyPoly.h"
#include "PropertyPoly.h"
#include "PropertySet.h"

//////////////////////////////////////////////////////////////////////////////////

HPropertyPoly *HPropertyPoly::GetNextPropertyPoly()
{
   return (HPropertyPoly*)((PropertyPoly*)this)->GetNextPropertyPoly();
}

HPoly *HPropertyPoly::GetPoly()
{
   return (HPoly *)((PropertyPoly *)this)->GetPoly();
}

HPropertySet *HPropertyPoly::GetPropertySet()
{
   return (HPropertySet*) ((PropertyPoly*)this)->GetPropertySet();
}