#include "stdafx.h"
#include "UVPoly.h"

IMPLEMENT_COMPONENT_CREATE( UVPoly, Face )

void UVPoly::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   PropertyPoly::Save( bs );
}

BOOL UVPoly::Load( BaseStream &bs )
{
   if (!PropertyPoly::Load( bs ))
      return FALSE;
   return TRUE;
}
