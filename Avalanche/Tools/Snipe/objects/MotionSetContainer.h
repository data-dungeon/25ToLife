#pragma once
#include "ContainerObject.h"

class MotionSet;

class MotionSetContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(MotionSetContainer, ContainerObject)

            MotionSetContainer();
   virtual ~MotionSetContainer();

   UINT GetNumMotionSets() const { return GetNumChildren(); }
   MotionSet *GetMotionSetAt( UINT nIndex ) { return (MotionSet *)GetChildAt(nIndex); }

// SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
};