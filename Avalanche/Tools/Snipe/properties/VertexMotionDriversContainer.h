#pragma once
#include "ContainerObject.h"

class VertexMotionDriversContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(VertexMotionDriversContainer, ContainerObject)

   VertexMotionDriversContainer();
   virtual ~VertexMotionDriversContainer();

   // SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
};