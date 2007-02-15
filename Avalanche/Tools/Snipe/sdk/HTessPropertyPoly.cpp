#include "stdafx.h"
#include "HTessPropertyPoly.h"
#include "TessPoly.h"
#include "PropertySet.h"
#include "HPropertyPatch.h"

//////////////////////////////////////////////////////////////////////////////////

HTessPropertyPoly *HTessPropertyPoly::GetNextTessPropertyPoly()
{
   return (HTessPropertyPoly *) ((TessPropertyPoly *)this)->GetNextTessPropertyPoly();
}

HPropertyPatch *HTessPropertyPoly::GetPropertyPatch()
{
   return (HPropertyPatch*) ((TessPropertyPoly*)this)->GetPropertyPatch();
}