#pragma once

#include "HierObject.h"
#include "ContainerObject.h"
#include "Component.h"
#include "ComponentList.h"
#include "PropertyVertex.h"
#include "PropertyEdge.h"
#include "PropertyPoly.h"

//#include "PropertyPatchComponent.h"
#include "PropertyPatchEdge.h"
#include "PropertyPatch.h"

class PolyModel;

class PropertySet : public HierObject
{
protected:
   CComponentList<PropertyEdge> m_propertyedgelist;
   CComponentList<PropertyVertex> m_propertyvertexlist;
   CComponentList<PropertyPoly> m_propertypolys;

public:
//   static CMap<PropertyVertex*,PropertyVertex*,BOOL,BOOL> m_memoryleakmap;   

public:
   DECLARE_SNIPEOBJECT_CREATE( PropertySet, HierObject )
   DECLARE_COMMANDOBJECT()

            PropertySet();
   virtual ~PropertySet();

   PolyModel *GetModel() const;

   // Property Components
   UINT GetNumPropertyVertices() { return m_propertyvertexlist.GetSize(); }
   PropertyVertex *AddPropertyVertex( UINT nID=-1 );
   PropertyVertex *AddTessellatedPropertyVertex( UINT nID=-1 );
   PropertyVertex *AddTessellatedPropertyVertices( UINT nSize );

   void RemovePropertyVertex( PropertyVertex *pPropertyVertex );
   PropertyVertex *FindPropertyVertex( UINT nID ) { return m_propertyvertexlist.Find( nID ); }
   POSITION GetHeadPropertyVertexPosition() { return m_propertyvertexlist.GetHeadPosition(); }
   PropertyVertex *GetNextPropertyVertex( POSITION &pos ) { return m_propertyvertexlist.GetNext( pos ); }

   // Property Polys
   UINT GetNumPropertyPolys() { return m_propertypolys.GetSize(); }
   POSITION GetHeadPropertyPolyPosition() { return m_propertypolys.GetHeadPosition(); }
   PropertyPoly *GetNextPropertyPoly( POSITION &pos ) { return m_propertypolys.GetNext( pos ); }
   PropertyPoly *AddPoly( Poly *pPoly );
   void RemovePoly( PropertyPoly *pPropertyPoly );

   // Property Edges
   PropertyEdge *AddPropertyEdge( Edge *pEdge, UINT nID=-1 );
   PropertyEdge *AddPropertyEdge( PropertyPoly *pPropertyPoly, PropertyVertex *pPropertyVertex0, PropertyVertex *pPropertyVertex1, Edge *pEdge, UINT nID=-1 );
   POSITION GetHeadPropertyEdgePosition() { return m_propertyedgelist.GetHeadPosition(); }
   PropertyEdge *GetNextPropertyEdge( POSITION &pos ) { return m_propertyedgelist.GetNext( pos ); }
   BOOL HasPropertyEdges() { return m_propertyedgelist.GetCount() != 0; }

   // Overridables
   virt_base virtual PropertyVertex *CreatePropertyVertex(UINT nID=-1);
   virt_base virtual PropertyEdge *CreatePropertyEdge(UINT nID=-1);
   virt_base virtual PropertyPoly *CreatePropertyPoly(UINT nID=-1);

   virt_base virtual PropertyVertex *CreatePropertyPatchComponent(UINT nID=-1);
   virt_base virtual PropertyPatchEdge *CreatePropertyPatchEdge(UINT nID=-1);
   virt_base virtual PropertyPatch *CreatePropertyPatch(UINT nID=-1);

   virt_base virtual PropertyVertex *DuplicateComponent(PropertyVertex *pPropertyVertex) { return AddPropertyVertex(); }

   // SnipeObject overrides
   virtual void CountPickObjectType( CGLBaseView *pView, UINT &nCount );
   virtual SimpleComponentList *FindComponentList( const String &strComponentType );
   virtual Component *CreateComponent( SnipeObjectType nSOT );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );

};

////////////////////////////////////////////////////////////////////

class PropertySetContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(PropertySetContainer, ContainerObject)

            PropertySetContainer();
   virtual ~PropertySetContainer();

   UINT GetNumPropertySets() const { return GetNumChildren(); }
   PropertySet *GetPropertySetAt( UINT nIndex ) { return (PropertySet*)GetChildAt(nIndex); }

// SnipeObject overrides
   virtual void Draw( CGLBaseView *pView ) {} // don't recurse, it's a waiste of time
};