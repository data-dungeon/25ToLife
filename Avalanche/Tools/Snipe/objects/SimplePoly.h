#include "VertexNormal.h"
#include "UV.h"
#include "SubDVertex.h"
#include "PatchVertex.h"
#include "../Utility/simplelist.h"

class SimpleTriangleUVSet
{
protected:
   UV *m_pUV[3];
public:
   SimpleTriangleUVSet() { m_pUV[0] = NULL; m_pUV[1] = NULL; m_pUV[2] = NULL; }
   UV *GetUVAt(const UINT nIndex) const { return m_pUV[nIndex]; }
   void SetUVAt(const UINT nIndex,UV *pUV) { m_pUV[nIndex] = pUV; }
};

class SimpleTriangle : public SimpleListEntry
{
protected:
   Vertex *m_pVertex[3];
   VertexNormal *m_pVertexNormal[3];
   CArray<SimpleTriangleUVSet> m_uvsets;
   Vector m_faceNormal;

   union {
      ULONG m_nSimpleTriangleFlags;
      struct {
         ULONG m_bDirtyFaceNormal               : 1;
      };
   };
public:
   SimpleTriangle();
   ~SimpleTriangle();

   // SimpleListEntry
   SimpleTriangle *Next() { return (SimpleTriangle *)SimpleListEntry::Next(); }
   SimpleTriangle *Previous() { return (SimpleTriangle *)SimpleListEntry::Previous(); }

   Vertex *GetVertexAt(const UINT nIndex) const;
   void SetVertexAt(const UINT nIndex,Vertex *pVertex);
   VertexNormal *GetVertexNormalAt(const UINT nIndex) const;
   void SetVertexNormalAt(const UINT nIndex,VertexNormal *pVertexNormal);
   UV *GetUVAt(const UINT nIndex, UINT nUVSetIndex = 0);
   void SetUVAt(const UINT nIndex, UV *pUV, UINT nUVSetIndex = 0);

   // neighbors
   void AddSecondaryNeighbors();
   void RemoveSecondaryNeighbors();

   // face normals
   void BuildFaceNormal();
   Vector *GetFaceNormal();
   void SetDirtyFaceNormal( BOOL bDirtyFaceNormal );

   // misc
   int FindVertexFaceRelativeIndex(Vertex *pVertex);
};

class SimplePolyUVSet
{
private:
   CArray<UV *> m_uvs;
public:
   SimplePolyUVSet();
   ~SimplePolyUVSet();
   UINT GetNumUVs() const;
   void SetNumUVs(const UINT nSize);
   UV *GetUVAt(const UINT nIndex) const;
   void SetUVAt(const UINT nIndex,UV *pUV);
};

class SimplePoly : public SimpleListEntry
{
protected:
   CArray<Vertex *> m_vertices;
   CArray<VertexNormal *> m_vertexnormals;
   CArray<SimplePolyUVSet> m_uvsets;
   Vector m_faceNormal;

   union {
      ULONG m_nSimplePolyFlags;
      struct {
         ULONG m_bDirtyFaceNormal               : 1;
      };
   };

public:
   SimplePoly();
   ~SimplePoly();

   // SimpleListEntry
   SimplePoly *Next() { return (SimplePoly *)SimpleListEntry::Next(); }
   SimplePoly *Previous() { return (SimplePoly *)SimpleListEntry::Previous(); }

   // vertices
   UINT GetNumVertices() const;
   void SetNumVertices( UINT nSize );
   Vertex *GetVertexAt( UINT nIndex ) const;
   void SetVertexAt( UINT nIndex, Vertex *pVertex );

   // vertexnormals
   UINT GetNumVertexNormals() const;
   void SetNumVertexNormals( UINT nSize );
   VertexNormal *GetVertexNormalAt( UINT nIndex ) const;
   void SetVertexNormalAt( UINT nIndex, VertexNormal *pVertexNormal );

   // uvs
   UINT GetNumUVSets() const;
   void SetNumUVSets(UINT nSize);
   void SetNumUVs(const UINT nSize,UINT nUVSetIndex = 0) { m_uvsets[nUVSetIndex].SetNumUVs(nSize); }
   UINT GetNumUVs(UINT nSetIndex = 0) const;
   UV *GetUVAt(UINT nIndex,UINT nSetIndex = 0) const;
   void SetUVAt(UINT nIndex,UV *pUV,UINT nSetIndex = 0);

   // neighbors
//   void AddSecondaryNeighbors();
//   void RemoveSecondaryNeighbors();

   // face normals
   void BuildFaceNormal();
   Vector *GetFaceNormal();
   void SetDirtyFaceNormal( BOOL bDirtyFaceNormal );

   // misc
   int FindVertexFaceRelativeIndex(Vertex *pVertex);

};

#include "SimplePoly.hpp"
