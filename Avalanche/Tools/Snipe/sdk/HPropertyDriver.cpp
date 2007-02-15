
#include "stdafx.h"
#include "HPropertyDriver.h"
#include "Properties/PropertyDriver.h"

HPropertyPointerProperty *HPropertyDriver::GetProperty()
{
   return (HPropertyPointerProperty *)&((PropertyDriver *)this)->m_pProperty;
}

HSnipeObject *HPropertyDriver::GetObjectDriven()
{
   return (HSnipeObject*)((PropertyDriver *)this)->GetObjectDriven();
}
