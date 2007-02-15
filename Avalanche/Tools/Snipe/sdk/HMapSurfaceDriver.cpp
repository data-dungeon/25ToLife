
#include "stdafx.h"
#include "HMapSurfaceDriver.h"
#include "Properties/MapSurfaceDriver.h"

HImagePointerProperty *HMapSurfaceDriver::GetImage() const
{
   return (HImagePointerProperty *) &((MapSurfaceDriver *)this)->m_pImage;
}

HTypeProperty *HMapSurfaceDriver::GetBlendStyle()
{
   return (HTypeProperty *)&((MapSurfaceDriver *)this)->m_nBlendStyle;
}

HFloatProperty *HMapSurfaceDriver::GetOpacity()
{
   return (HFloatProperty *)&((MapSurfaceDriver *)this)->m_fOpacity;
}
