#pragma once

#include "HierObject.h"
#include "ContainerObject.h"
#include "PropertySet.h"

class TexCoords : public PropertySet
{
public:
   DECLARE_SNIPEOBJECT_CREATE( TexCoords, PropertySet )

   SnipeObject *m_pCurInstance; // the one chosen for editing in the UV editor

            TexCoords();
   virtual ~TexCoords();

   // UVs
   UINT GetNumUVs() { return GetNumPropertyVertices(); }
   UV *AddUV() { return (UV*)AddPropertyVertex(); }
   UV *AddUV( const Vector2 &pos ) { UV *pUV = AddUV(); pUV->SetPos(pos); return pUV; } 
   void RemoveUV(UV *pUV) { RemovePropertyVertex((PropertyVertex*)pUV); }
   UV *FindUV( UINT nID ) {  return (UV*)FindPropertyVertex( nID ); }
   UV *GetNextUV( POSITION &pos ) { GetNextPropertyVertex(pos); }

   BOOL HasUVEdges() { return HasPropertyEdges(); }

   // UVPolys
   UINT GetNumUVPolys() { return GetNumPropertyPolys();  }
   UVPoly *GetNextUVPoly( POSITION &pos ) { return (UVPoly*)GetNextPropertyPoly(pos); }
   UVPoly *AddPoly( Poly *pPoly ) { return (UVPoly*)PropertySet::AddPoly(pPoly); }
   void RemovePoly( UVPoly *pUVPoly ) { PropertySet::RemovePoly((PropertyPoly*)pUVPoly); }

   void DrawUVs( CGLTexCoordView *pView );

   // Edges
   UVEdge *AddUVEdge( Edge *pEdge, UINT nID=-1 ) { return (UVEdge*)AddPropertyEdge(pEdge,nID); }
   UVEdge *AddUVEdge( UVPoly *pUVPoly, UV *pUV0, UV *pUV1, Edge *pEdge, UINT nID=-1 )
      { return (UVEdge*)AddPropertyEdge( (PropertyPoly*)pUVPoly, (PropertyVertex*)pUV0, (PropertyVertex*)pUV1, pEdge, nID ); }
   UVEdge *GetNextUVEdge( POSITION &pos ) { return (UVEdge*)GetNextPropertyEdge(pos); }

   // PropertySet overrides
   virtual PropertyVertex *CreatePropertyVertex(UINT nID=-1);
   virtual PropertyEdge *CreatePropertyEdge(UINT nID=-1);
   virtual PropertyPoly *CreatePropertyPoly(UINT nID=-1);

   virtual PropertyVertex *CreatePropertyPatchComponent(UINT nID=-1);
   virtual PropertyPatchEdge *CreatePropertyPatchEdge(UINT nID=-1);
   virtual PropertyPatch *CreatePropertyPatch(UINT nID=-1);

   virtual PropertyVertex *DuplicateComponent(PropertyVertex *pPropertyVertex);
  
   // HierObject overrides
   virtual CSnipeDocTemplate *GetDocTemplate();

   // SnipeObject overrides
   virtual void CountPickObjectType( CGLBaseView *pView, UINT &nCount );
   virtual SimpleComponentList *FindComponentList( const String &strComponentType );
   virtual Component *CreateComponent( SnipeObjectType nSOT );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
   virtual int GetItemHeight();
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

////////////////////////////////////////////////////////////////////

class TexCoordsContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(TexCoordsContainer, ContainerObject)

            TexCoordsContainer();
   virtual ~TexCoordsContainer();

   UINT GetNumTexCoords() const { return GetNumChildren(); }
   TexCoords *GetTexCoordsAt( UINT nIndex ) { return (TexCoords*)GetChildAt(nIndex); }

// SnipeObject overrides
   virtual void Draw( CGLBaseView *pView ) {} // don't recurse, it's a waiste of time
};

#include "TexCoords.hpp"