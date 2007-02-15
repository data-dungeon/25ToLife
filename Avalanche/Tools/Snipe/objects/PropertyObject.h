#pragma once
#include "SnipeObject.h"
#include "Properties/PropertyArray.h"
#include "Plugin/CommandUIPlugin.h"

class Property;

class PropertyObject : public SnipeObject
{
protected:
   PropertyArray m_propertyarray;

public:
   DECLARE_SNIPEOBJECT_PURE( PropertyObject, SnipeObject )

            PropertyObject();
   virtual ~PropertyObject();

// Operations
   void AddProperty( Property *pProperty ) ;
   void ShowProperties( BOOL bVisible );
   BOOL HasVisibleProperty();
   UINT GetNumProperties() const { return m_propertyarray.GetCount(); }

// SnipeObject overrides
   virtual void PreDestruct();
   virtual PropertyArray *GetPropertyArray() { return &m_propertyarray; };
   virtual void MarkSaveDependents();
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
};
