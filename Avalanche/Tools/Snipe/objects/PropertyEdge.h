#pragma once

#include "Component.h"
#include "PropertyVertex.h"
#include "Edge.h"

class PropertySet;

class PropertyEdge : public Component
{
protected:
   Edge *m_pEdge;
   PropertyVertex *m_PropertyVertices[2];
   PropertyPoly *m_propertypolys[2];
   PropertyEdge *m_pNextPropertyEdge;

   union {
      ULONG m_nPropertyEdgeFlags;
      struct {
         ULONG   //m_bSoft	         : 1,
                // m_bSoftDirty       : 1,
///////////////////////////////////  PropertyPatchEdge bits, here for memory sake
                 m_bTangentsDirty   : 1,
                 m_bTessellationDirty : 1;
      };
   };

public:
   DECLARE_COMPONENT_CREATE(PropertyEdge, Component)

   PropertyEdge();
   PropertyEdge( UINT nID );
   virtual ~PropertyEdge();

   void Init();

   Edge *GetEdge() const { return m_pEdge; }
   void SetEdge( Edge *pEdge ) { m_pEdge = pEdge; } // Never call directly, call Edge::AddUVEdge!!!

   PropertyEdge *GetNextPropertyEdge() { return m_pNextPropertyEdge; }
   void SetNextPropertyEdge( PropertyEdge *pNextPropertyEdge ) { m_pNextPropertyEdge = pNextPropertyEdge; }
   PropertyEdge **GetNextPropertyEdgeUplink() { return &m_pNextPropertyEdge; }

   PropertyVertex *GetFirstPropertyVertex() const { return m_PropertyVertices[0]; }
   PropertyVertex *GetSecondPropertyVertex() const { return m_PropertyVertices[1]; }

   PropertyVertex *GetSharedPropertyVertex( PropertyEdge *pPropertyEdge ) const;
   PropertyVertex *GetConnectedPropertyVertex( PropertyVertex *pNotThis );
   BOOL IsInEdge(PropertyVertex *pPropertyVertex) { return GetFirstPropertyVertex() == pPropertyVertex || GetSecondPropertyVertex() == pPropertyVertex; }

   BOOL IsAligned( PropertyPoly *pPropertyPoly ) const;
   PropertyVertex *GetAlignedFirstPropertyVertex( PropertyPoly *pPropertyPoly ) const;
   PropertyVertex *GetAlignedSecondPropertyVertex( PropertyPoly *pPropertyPoly ) const;

   PropertyPoly *GetFirstPropertyPoly();
   void SetFirstPropertyPoly(PropertyPoly *pPropertyPoly);
   PropertyPoly *GetSecondPropertyPoly();
   void SetSecondPropertyPoly(PropertyPoly *pPropertyPoly);
   PropertyPoly *GetPropertyPoly();
   PropertyPoly *GetConnectedPropertyPoly(PropertyPoly *pNotThis);

   BOOL IsSoftEdge();
   BOOL IsBorderEdge() const;
   PropertySet *GetPropertySet();
   BOOL IsPropertyVertexPeaked(PropertyVertex *pPropertyVertex) const;

//   void SetSoftEdgeDirtyFlag(BOOL bFlag) { m_bSoftDirty = bFlag; }

   static PropertyEdge *FindEdge( const PropertyVertex *pPropertyVertex0, const PropertyVertex *pPropertyVertex1 );
   
// Overridables
   virt_base virtual BOOL AddPropertyPoly(PropertyPoly *pPropertyPoly);
   virt_base virtual void RemovePropertyPoly(PropertyPoly *pPropertyPoly);
//   virt_base virtual Vector2 GetPosAlongEdge( float fPosition, PropertyPoly *pPoly );
   virt_base virtual void Reverse();
   virt_base virtual void Draw( BOOL bDrawTess );
   virt_base virtual void SetDirty();
   virt_base virtual void SetFirstPropertyVertex(PropertyVertex *pPropertyVertex);
   virt_base virtual void SetSecondPropertyVertex(PropertyVertex *pPropertyVertex);

// Component Overrrides
   virtual void Disconnect();
   virtual void Reconnect();
   virtual BOOL IsUsed();
};

#include "PropertyEdge.hpp"