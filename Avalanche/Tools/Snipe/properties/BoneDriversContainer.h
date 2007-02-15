#pragma once
#include "ContainerObject.h"

class MotionChannel;

class BoneDriversContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(BoneDriversContainer, ContainerObject)

   BoneDriversContainer();
   virtual ~BoneDriversContainer();

   // SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
};