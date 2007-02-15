#include "stdafx.h"
#include "HRotoscope.h"
#include "Rotoscope.h"

HImagePointerProperty *HRotoscope::GetImage() const
{
   return (HImagePointerProperty *) &((Rotoscope *)this)->m_pImage;
}

HTransform2Property *HRotoscope::GetTransform()
{
   return (HTransform2Property *)&((Rotoscope *)this)->m_transform;
}

HFloatProperty *HRotoscope::GetOpacity()
{
   return (HFloatProperty *)&((Rotoscope *)this)->m_fOpacity;
}
