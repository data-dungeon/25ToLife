#pragma once
#include "ContainerObject.h"

class CustomPropertyDriversContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(CustomPropertyDriversContainer, ContainerObject)

   CustomPropertyDriversContainer();
   virtual ~CustomPropertyDriversContainer();

   // SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
};