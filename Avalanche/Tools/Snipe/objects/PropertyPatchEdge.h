#pragma once

#include "PropertyEdge.h"
#include "TessEdge.h"

class PropertyPatch;

class PropertyVertexHandle
{
public:
            PropertyVertexHandle() {}
   virtual ~PropertyVertexHandle() {}
};

/////////////////////////////////////////////////////////////////

class PropertyPatchEdge : public PropertyEdge
{
protected:
   TessPropertyEdge m_tessPropertyVertices;
   PropertyVertexHandle *m_pFirstPropertyVertexHandle;
   PropertyVertexHandle *m_pSecondPropertyVertexHandle;

public:
   DECLARE_COMPONENT_CREATE(PropertyPatchEdge, PropertyEdge)

   PropertyPatchEdge() { Init(); }
   PropertyPatchEdge( UINT nID ) : PropertyEdge( nID ) { Init(); }
   virtual ~PropertyPatchEdge();

   void Init();
   PatchEdge *GetEdge() { return (PatchEdge *) m_pEdge; }

   BOOL IsTessellationDirty() { return m_bTessellationDirty; }
   void SetTessellationDirty( BOOL bDirty ) { m_bTessellationDirty = bDirty; }

   UINT GetNumTessPropertyVertices();
   void SetNumTessPropertyVertices( UINT nNumUVs );
   void SetTessPropertyVertexAt( UINT nIndex, PropertyVertex *pPropertyVertex );
   PropertyVertex *GetTessPropertyVertexAt( UINT nIndex, BOOL bReversed=FALSE );

   TessPropertyEdge *GetTessPropertyEdge();

   BOOL IsTangentsDirty() { return m_bTangentsDirty; }
   void SetTangentsDirty( BOOL bDirty );

   PropertyVertexHandle *GetFirstPropertyVertexHandle();
   PropertyVertexHandle *GetSecondPropertyVertexHandle();

   PropertyVertexHandle *GetAlignedFirstPropertyVertexHandle( PropertyPoly *pPropertyPoly );
   PropertyVertexHandle *GetAlignedSecondPropertyVertexHandle( PropertyPoly *pPropertyPoly );

   PropertyVertexHandle *GetPropertyVertexHandle( PropertyVertex *pPropertyVertex );

   void ClearTessellation();

   PropertyPatch *GetPropertyPatch() { return (PropertyPatch *)GetPropertyPoly(); }
   PropertyPatch *GetConnectedPropertyPatch( PropertyPatch *pNotThis ) { return (PropertyPatch *)GetConnectedPropertyPoly( (PropertyPoly *)pNotThis ); }

   PropertyPatchEdge *GetPrevContPropertyEdge(BOOL bLoop);
   PropertyPatchEdge *GetNextContPropertyEdge(BOOL bLoop);
   PropertyVertex *GetPrevContPropertyVertex();
   PropertyVertex *GetNextContPropertyVertex();

   void AllocateTessellation();

// Overridables
   virt_base virtual void BuildTangents() {}
   virt_base virtual void SetTessellation() {}

// PropertyEdge overrides
   virtual BOOL AddPropertyPoly(PropertyPoly *pPropertyPoly);
   virtual void Reverse();
 //  virtual void Draw( BOOL bDrawTess );
   virtual void SetDirty();
   virtual void SetFirstPropertyVertex(PropertyVertex *pPropertyVertex);
   virtual void SetSecondPropertyVertex(PropertyVertex *pPropertyVertex);

// Component overrides
   virtual void Disconnect();
};

#include "PropertyPatchEdge.hpp"