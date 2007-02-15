#pragma once
#include "HierObject.h"

class ContainerObject : public HierObject
{
public:
   DECLARE_SNIPEOBJECT_PURE(ContainerObject, HierObject)

            ContainerObject();
   virtual ~ContainerObject();

// InstanceObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_CONTAINER; };
};
