#pragma once

#include "Component.h"

class PropertySet;
class PropertyPoly;
class PropertyEdge;

class PropertyComponent : public Component
{
private:
   PropertyPoly **m_pSurroundingPropertyPolys;
   PropertyEdge **m_pSurroundingPropertyEdges;
   char m_nNumSurroundingPropertyFaces, m_nNumMaxSurroundingPropertyFaces;
   char m_nNumSurroundingPropertyEdges, m_nNumMaxSurroundingPropertyEdges;

public:
   DECLARE_COMPONENT_CREATE(PropertyComponent, Component)

            PropertyComponent();
            PropertyComponent( UINT nID );
   virtual ~PropertyComponent();

   // Operations
   PropertySet *GetPropertySet();

   void SetDirty();
   void DirtyDependentEdges();

//   void GetTangents( PropertyPatch *pPropertyComponentPatch, Vector2 &PropertyComponent0, Vector2 &PropertyComponent1 );

   // Neighbors
   UINT GetNumSurroundingPropertyEdges() const  { return m_nNumSurroundingPropertyEdges; }
   void AddSurroundingPropertyEdge( PropertyEdge *pPropertyComponentEdge );
   PropertyEdge *GetSurroundingPropertyEdgeAt( UINT nIndex ) const;
   void RemoveSurroundingPropertyEdgeAt( UINT nIndex );
   void RemoveSurroundingPropertyEdge( PropertyEdge *pPropertyEdge );

   UINT GetNumSurroundingPropertyPolys() const { return m_nNumSurroundingPropertyFaces; }
   void AddSurroundingPropertyPoly( PropertyPoly *pPropertyPoly );
   PropertyPoly *GetSurroundingPropertyPolyAt( UINT nIndex ) const;
   void RemoveSurroundingPropertyPolyAt( const UINT nIndex );
   void RemoveSurroundingPropertyPoly( PropertyPoly *pPropertyPoly );

   void Compact();

   void GetEdgePair(PropertyPoly *pPoly, PropertyEdge **pEdge0, PropertyEdge **pEdge1, UINT *nIndex0=NULL, UINT *nIndex1=NULL);

// Component overrides
   virtual BOOL IsUsed();
   virtual void Reconnect();
   virtual void Disconnect();

// SnipeObject Overrides
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
};


