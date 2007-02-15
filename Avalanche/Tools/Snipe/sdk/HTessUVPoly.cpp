#include "stdafx.h"
#include "HTessUVPoly.h"
#include "TessPoly.h"

HUV *HTessUVPoly::GetUVAt( UINT nIndex )
{
   return (HUV *) ((TessUVPoly *)this)->GetUVAt( nIndex );
}

HUVPatch *HTessUVPoly::GetUVPatch() const
{
   return (HUVPatch *) ((TessUVPoly *)this)->GetUVPatch();
}

