#pragma once
#include "PatchVertex.h"
#include "VertexNormal.h"
#include "UV.h"

class TessUVPoly;
class Patch;
class TessPropertyPoly;
class PropertyVertex;
class PropertyPatch;
class PropertySet;

class TessPoly
{
protected:
   Patch         *m_pPatch;
   PatchVertex  **m_pVertices;
   UINT          *m_pRTVertexIndices;
   VertexNormal **m_pVertexNormals;
   Vector         m_vFaceNormal;
   TessPropertyPoly *m_pHeadTessPropertyPoly;

   union {
      ULONG       m_nTessPolyFlags;
      struct {
         ULONG    m_nNumSides        : 8,
                  m_bDirtyFaceNormal : 1;
      };
   };

public:
    TessPoly();
   ~TessPoly();

   UINT GetNumSides() const { return m_nNumSides; }
   void SetNumSides( UINT nNumSides );

   // vertices
   PatchVertex *GetVertexAt( UINT nIndex ) const;
   void SetVertexAt( UINT nIndex, PatchVertex *pVertex );

   // rtvertices
   UINT GetRTVertexAt( UINT nIndex ) const;
   void SetRTVertexAt( UINT nIndex, UINT nRTVertex );

   // vertexnormals
   VertexNormal *GetVertexNormalAt( UINT nIndex, BOOL bCalculateIfNecessary=TRUE ) const;
   void SetVertexNormalAt( UINT nIndex, VertexNormal *pVertexNormal );

   // UVs
   TessUVPoly *FindTessUVPoly( TexCoords *pTexCoords );

   // Face normals
   void BuildFaceNormal();
   Vector *GetFaceNormal();
   void SetFaceNormal(const Vector &vFaceNormal) { m_vFaceNormal = vFaceNormal; }
   void SetDirtyFaceNormal( BOOL bDirtyFaceNormal );

   // TessPropertyPolys
   UINT CountNumTessPropertyPolys() const;
   TessPropertyPoly *GetHeadTessPropertyPoly() const { return m_pHeadTessPropertyPoly; }
   void SetHeadTessPropertyPoly( TessPropertyPoly *pTessPropertyPoly ) { m_pHeadTessPropertyPoly = pTessPropertyPoly; }
   void AddTessPropertyPoly( TessPropertyPoly *pTessPropertyPoly );
   BOOL RemoveTessPropertyPoly( TessPropertyPoly *pTessPropertyPoly );
   TessPropertyPoly *FindTessPropertyPoly( PropertySet *pPropertySet );

   // misc
   int FindVertexFaceRelativeIndex(PatchVertex *pVertex);
   UINT GetNextIndex( UINT nIndex ) const;
   UINT GetPrevIndex( UINT nIndex ) const;

   void TessellatePatch() const;
   void SetPatch( Patch *pPatch ) { m_pPatch = pPatch; }
   Patch *GetPatch() const { return m_pPatch; }

   void Disconnect();
};

class TessPropertyPoly
{
public:
   TessPropertyPoly *m_pNextTessPropertyPoly;
   PropertyPatch *m_pPropertyPatch;
   PropertyVertex **m_pPropertyVertices; // size is GetNumSides of the TessPoly

public:
   TessPropertyPoly()
   {
      m_pPropertyVertices = NULL;
      m_pNextTessPropertyPoly = NULL;
      m_pPropertyPatch = NULL;
   }

   void Init( PropertyPatch *pPropertyPatch )
   {
      m_pPropertyPatch = pPropertyPatch;
   }

   TessPropertyPoly *GetNextTessPropertyPoly() const { return m_pNextTessPropertyPoly; }
   TessPropertyPoly **GetNextTessPropertyPolyUplink() { return &m_pNextTessPropertyPoly; }
   void SetNextTessPropertyPoly( TessPropertyPoly *pTessPropertyPoly ) { m_pNextTessPropertyPoly = pTessPropertyPoly; }

   PropertyVertex *GetPropertyVertexAt(UINT nIndex) const { return m_pPropertyVertices[nIndex]; }
   void SetPropertyVertexAt(UINT nIndex, PropertyVertex *pPropertyVertex);

   PropertyPatch *GetPropertyPatch() const { return m_pPropertyPatch; }
};

class TessUVPoly : public TessPropertyPoly
{
public:
   TessUVPoly() {}

   TessUVPoly *GetNextTessUVPoly() const { return (TessUVPoly*)GetNextTessPropertyPoly(); }
   TessUVPoly **GetNextTessUVPolyUplink() { return (TessUVPoly**)GetNextTessPropertyPolyUplink(); }
   void SetNextTessUVPoly( TessUVPoly *pTessUVPoly ) { SetNextTessPropertyPoly((TessPropertyPoly*)pTessUVPoly); }

   UV *GetUVAt(UINT nIndex) const { return (UV*)GetPropertyVertexAt(nIndex); }
   void SetUVAt(UINT nIndex, UV *pUV) { SetPropertyVertexAt(nIndex,(PropertyVertex*)pUV); }

   UVPatch *GetUVPatch() const { return (UVPatch*)GetPropertyPatch(); }
};


#include "TessPoly.hpp"
