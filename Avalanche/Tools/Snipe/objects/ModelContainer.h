#pragma once
#include "ContainerObject.h"

class PolyModel;

class ModelContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(ModelContainer, ContainerObject)

            ModelContainer();
   virtual ~ModelContainer();

   UINT GetNumModels() const { return GetNumChildren(); }
   PolyModel *GetModelAt( UINT nIndex ) { return (PolyModel *)GetChildAt(nIndex); }

// SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
};