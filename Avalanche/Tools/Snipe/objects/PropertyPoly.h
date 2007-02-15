#pragma once

#include "Component.h"

class PropertySet;
class Poly;
class PropertyVertex;
class PropertyEdge;

class PropertyPoly : public Component
{
public:
   PropertyPoly *m_pNextPropertyPoly;
   Poly *m_pPoly;

   PropertyVertex **m_pPropertyVertices;
   PropertyEdge **m_pPropertyEdges;

   DECLARE_COMPONENT_CREATE( PropertyPoly, Component )

            PropertyPoly();
            PropertyPoly( UINT nID );
   virtual ~PropertyPoly();

   void Init();
   PropertyPoly *GetNextPropertyPoly() const { return m_pNextPropertyPoly; }
   PropertyPoly **GetNextPropertyPolyUplink() { return &m_pNextPropertyPoly; }

   void SetNextPropertyPoly( PropertyPoly *pPropertyPoly ) { m_pNextPropertyPoly = pPropertyPoly; }

   UINT GetNumSides() const;

   // Property Components
   PropertyVertex *GetPropertyVertexAt( UINT nIndex ) const { ASSERT( nIndex < GetNumSides() ); return m_pPropertyVertices[nIndex]; }
   void SetPropertyVertexAt( UINT nIndex, PropertyVertex *pPropertyVertex );
   int FindPropertyVertexFaceRelativeIndex( const PropertyVertex *pFind) const;

   // Property Edges
   void SetPropertyEdgeAt( UINT nIndex, PropertyEdge *pPropertyEdge );
   PropertyEdge *GetPropertyEdgeAt( UINT nIndex ) const;
   int FindPropertyVertexFaceRelativeIndex( const PropertyEdge *pFind ) const;
   void BuildPropertyEdges(); 

   PropertySet *GetPropertySet();

   Poly *GetPoly() { return m_pPoly; }
   void SetPoly( Poly *pPoly ) { m_pPoly = pPoly; }

   UINT GetPrevIndex( UINT nIndex ) const;
   UINT GetNextIndex( UINT nIndex ) const;

   PropertyPoly *FindConnectedPropertyPoly(PropertyVertex *pPropertyVertex0, PropertyVertex *pPropertyVertex1);

// Overridables
   virt_base virtual void ReverseWinding();

// Component overrides
   virtual BOOL IsUsed();
   virtual void Reconnect();
   virtual void Disconnect();

// SnipeObject Overrides
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
};

#include "PropertyPoly.hpp"