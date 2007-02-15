#include "stdafx.h"
#include "HUVMapSurfaceDriver.h"
#include "Properties/UVMapSurfaceDriver.h"
#include "HTexCoords.h"

HTexCoordsPointerProperty *HUVMapSurfaceDriver::GetTexCoords()
{
   return (HTexCoordsPointerProperty *)& ((UVMapSurfaceDriver *)this)->m_pTexCoords;
}
