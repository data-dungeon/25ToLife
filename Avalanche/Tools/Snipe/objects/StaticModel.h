#pragma once
#include "GeometricObject.h"
#include "ContainerObject.h"

class StaticModel : public GeometricObject
{
public:
   DECLARE_SNIPEOBJECT(StaticModel)

            StaticModel();
   virtual ~StaticModel();

// InstanceObject overrides
   virtual String GetTitle();
   virtual UINT GetIconIndex() { return ICONINDEX_MODEL; };

// HierObject overrides
   virtual CBaseDoc *OnEdit();
};

class ModelContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT(ModelContainer)

            ModelContainer();
   virtual ~ModelContainer();

// SnipeObject overrides
   virtual BOOL OnNotify( NotificationMsg msg, WPARAM wParam, LPARAM lParam );
};