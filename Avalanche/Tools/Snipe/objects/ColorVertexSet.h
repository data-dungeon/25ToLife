#pragma once

#include "HierObject.h"
#include "ContainerObject.h"
#include "PropertySet.h"
#include "ColorVertex.h"

class ColorPoly;
class ColorEdge;

class ColorVertexSet : public PropertySet
{
public:
   DECLARE_SNIPEOBJECT_CREATE( ColorVertexSet, PropertySet )
   DECLARE_COMMANDOBJECT()

            ColorVertexSet();
   virtual ~ColorVertexSet();

   // ColorVertexs
   UINT GetNumColorVertexs() { return GetNumPropertyVertices(); }
   ColorVertex *AddColorVertex() { return (ColorVertex*)AddPropertyVertex(); }
   ColorVertex *AddColorVertex( const RGBAFloat &color ) { ColorVertex *pColorVertex = AddColorVertex(); pColorVertex->SetColor(color); return pColorVertex; } 
   void RemoveColorVertex(ColorVertex *pColorVertex) { RemovePropertyVertex((PropertyVertex*)pColorVertex); }
   ColorVertex *FindColorVertex( UINT nID ) {  return (ColorVertex*)FindPropertyVertex( nID ); }
   ColorVertex *GetNextColorVertex( POSITION &pos ) { GetNextPropertyVertex(pos); }

   BOOL HasColorEdges() { return HasPropertyEdges(); }

   // ColorPolys
   UINT GetNumColorPolys() { return GetNumPropertyPolys();  }
   ColorPoly *GetNextColorPoly( POSITION &pos ) { return (ColorPoly*)GetNextPropertyPoly(pos); }
   ColorPoly *AddPoly( Poly *pPoly ) { return (ColorPoly*)PropertySet::AddPoly(pPoly); }
   void RemovePoly( ColorPoly *pColorPoly ) { PropertySet::RemovePoly((PropertyPoly*)pColorPoly); }

   void DrawColorVertices( CGLTexCoordView *pView );

   // Edges
   ColorEdge *AddColorEdge( Edge *pEdge, UINT nID=-1 ) { return (ColorEdge*)AddPropertyEdge(pEdge,nID); }
   ColorEdge *AddColorEdge( ColorPoly *pColorPoly, ColorVertex *pColorVertex0, ColorVertex *pColorVertex1, Edge *pEdge, UINT nID=-1 )
      { return (ColorEdge*)AddPropertyEdge( (PropertyPoly*)pColorPoly, (PropertyVertex*)pColorVertex0, (PropertyVertex*)pColorVertex1, pEdge, nID ); }
   ColorEdge *GetNextColorEdge( POSITION &pos ) { return (ColorEdge*)GetNextPropertyEdge(pos); }

   // PropertySet overrides
   virtual PropertyVertex *CreatePropertyVertex(UINT nID=-1);
   virtual PropertyEdge *CreatePropertyEdge(UINT nID=-1);
   virtual PropertyPoly *CreatePropertyPoly(UINT nID=-1);

   virtual PropertyVertex *CreatePropertyPatchComponent(UINT nID=-1);
   virtual PropertyPatchEdge *CreatePropertyPatchEdge(UINT nID=-1);
   virtual PropertyPatch *CreatePropertyPatch(UINT nID=-1);

   virtual PropertyVertex *DuplicateComponent(PropertyVertex *pPropertyVertex);

   // SnipeObject overrides
   virtual void CountPickObjectType( CGLBaseView *pView, UINT &nCount );
   virtual SimpleComponentList *FindComponentList( const String &strComponentType );
   virtual Component *CreateComponent( SnipeObjectType nSOT );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

////////////////////////////////////////////////////////////////////

class ColorVertexSetContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(ColorVertexSetContainer, ContainerObject)

            ColorVertexSetContainer();
   virtual ~ColorVertexSetContainer();

   UINT GetNumColorVertexSet() const { return GetNumChildren(); }
   ColorVertexSet *GetColorVertexSetAt( UINT nIndex ) { return (ColorVertexSet*)GetChildAt(nIndex); }

// SnipeObject overrides
   virtual void Draw( CGLBaseView *pView ) {} // don't recurse, it's a waiste of time
};
