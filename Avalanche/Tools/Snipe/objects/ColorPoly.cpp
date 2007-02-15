#include "stdafx.h"
#include "ColorPoly.h"

IMPLEMENT_COMPONENT_CREATE( ColorPoly, Face )

void ColorPoly::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   PropertyPoly::Save( bs );
}

BOOL ColorPoly::Load( BaseStream &bs )
{
   if (!PropertyPoly::Load( bs ))
      return FALSE;
   return TRUE;
}
