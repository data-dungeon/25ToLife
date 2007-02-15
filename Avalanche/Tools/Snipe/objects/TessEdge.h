#pragma once

#include "Vertex.h"

class UV;
class PropertyVertex;

class TessPropertyEdge
{
protected:
   CArray<PropertyVertex *> m_PropertyVertices;

public:
   TessPropertyEdge() {}
   ~TessPropertyEdge();

   UINT GetNumPropertyVertices() const;
   void SetNumPropertyVertices( UINT nSize );
   void SetPropertyVertexAt( UINT nIndex, PropertyVertex *pPropertyVertex );
   PropertyVertex *GetPropertyVertexAt( UINT nIndex, BOOL bReversed ) const;

   void Clear();
   void GetFirstHalf( TessPropertyEdge &tesspropertyedge );
   void GetSecondHalf( TessPropertyEdge &tesspropertyedge );
};

class TessEdgeUVs : public TessPropertyEdge
{
public:
   TessEdgeUVs() {}
   ~TessEdgeUVs() {}

   UINT GetNumUVs() const { return GetNumPropertyVertices(); }
   void SetNumUVs( UINT nSize ) { SetNumPropertyVertices(nSize); }
   void SetUVAt( UINT nIndex, UV *pUV ) { SetPropertyVertexAt(nIndex,(PropertyVertex*)pUV); }
   UV *GetUVAt( UINT nIndex, BOOL bReversed ) const { return (UV*)GetPropertyVertexAt(nIndex,bReversed); }
};

class Vertex;
class VertexNormal;
class PatchEdge;
class PatchVertex;

class TessEdgeVertices
{
protected:
   CArray<PatchVertex *> m_vertices;
public:
   TessEdgeVertices() {}
   ~TessEdgeVertices();

   UINT GetNumVertices() const;
   void SetNumVertices( UINT nSize );
   void SetVertexAt( UINT nIndex, PatchVertex *pVertex );
   void SetVertexAtGrow( UINT nIndex, PatchVertex *pVertex );
   PatchVertex *GetVertexAt( UINT nIndex ) const;
   CArray<PatchVertex *> *GetVertices();
   void Reverse(TessEdgeVertices &result);
   void GetHalves(TessEdgeVertices &edge1,TessEdgeVertices &edge2);
   UINT FindNPatchEdgeHook();
   void Clear();
   void FindClosestIntersection( CGLBaseView *pView, TransformableObject *pTransformableObject, const CPoint &point, float &fPos );
   void GetFirstHalf(TessEdgeVertices &tessedgevertices);
   void GetSecondHalf(TessEdgeVertices &tessedgevertices);
};

class TessEdgeNormals
{
protected:
   CArray<VertexNormal *> m_vertexnormals;

public:
   TessEdgeNormals() {}
   ~TessEdgeNormals();
 
   UINT GetNumVertexNormals();
   void SetNumVertexNormals( UINT nSize );
   void SetVertexNormalAt( UINT nIndex, VertexNormal *pVertexNormal );
   void SetVertexNormalAtGrow( UINT nIndex, VertexNormal *pVertexNorml );
   VertexNormal *GetVertexNormalAt( UINT nIndex ) const;

   void Reverse( TessEdgeNormals &result );
   void Clear();
   void GetFirstHalf( TessEdgeNormals &tessedgenormals );
   void GetSecondHalf( TessEdgeNormals &tessedgenormals );
};

#include "TessEdge.hpp"