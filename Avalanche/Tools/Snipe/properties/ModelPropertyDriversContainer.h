#pragma once
#include "ContainerObject.h"

class ModelPropertyDriversContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(ModelPropertyDriversContainer, ContainerObject)

   ModelPropertyDriversContainer();
   virtual ~ModelPropertyDriversContainer();

   // SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
};