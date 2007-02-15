
#include "stdafx.h"
#include "HInstanceObject.h"
#include "InstanceObject.h"
#include "Properties/Property.h"

HPropertyArray *HInstanceObject::GetPropertyArray()
{
   return (HPropertyArray *) ((InstanceObject *)this)->GetPropertyArray();
}

void HInstanceObject::AddProperty(HProperty *pProperty)
{
   ((InstanceObject *)this)->AddProperty((Property *)pProperty);
}

BOOL HInstanceObject::IsCache() const
{
   return ((InstanceObject *)this)->IsCache();
}

BOOL HInstanceObject::IsInstance() const
{
   return ((InstanceObject *)this)->IsInstance();
}

HInstanceObject *HInstanceObject::GetRootCache()
{
   return (HInstanceObject *)((InstanceObject *)this)->GetRootCache();
}

HInstanceObject *HInstanceObject::GetCache()
{
   return (HInstanceObject *)((InstanceObject *)this)->GetCache();
}
