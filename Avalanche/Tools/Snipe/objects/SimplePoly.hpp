inline SimpleTriangle::SimpleTriangle()
{ 
   m_pVertex[0] = NULL; m_pVertex[1] = NULL; m_pVertex[2] = NULL;
   m_pVertexNormal[0] = NULL; m_pVertexNormal[1] = NULL; m_pVertexNormal[2] = NULL;
   m_nSimpleTriangleFlags = 0;
}

inline SimpleTriangle::~SimpleTriangle()
{
}

inline Vertex *SimpleTriangle::GetVertexAt(const UINT nIndex) const
{
   return m_pVertex[nIndex];
}

inline void SimpleTriangle::SetVertexAt(const UINT nIndex,Vertex *pVertex)
{
   ASSERT(pVertex);
   m_pVertex[nIndex] = pVertex;
}

inline VertexNormal *SimpleTriangle::GetVertexNormalAt(const UINT nIndex) const
{
   return m_pVertexNormal[nIndex];
}

inline void SimpleTriangle::SetVertexNormalAt(const UINT nIndex,VertexNormal *pVertexNormal)
{
   ASSERT(pVertexNormal);
   m_pVertexNormal[nIndex] = pVertexNormal;
}

inline UV *SimpleTriangle::GetUVAt(const UINT nIndex, UINT nUVSetIndex)
{
   return m_uvsets[nUVSetIndex].GetUVAt(nIndex);
} 

inline void SimpleTriangle::SetUVAt(const UINT nIndex, UV *pUV, UINT nUVSetIndex)
{
   ASSERT(pUV);
   m_uvsets[nUVSetIndex].SetUVAt(nIndex,pUV);
}

inline void SimpleTriangle::AddSecondaryNeighbors()
{
   for (UINT i = 0; i < 3; i++)
      ((SubDVertex*)GetVertexAt(i))->AddSecondarySurroundingPoly(this);
}

inline void SimpleTriangle::RemoveSecondaryNeighbors()
{
   for (UINT i = 0; i < 3; i++)
      ((SubDVertex*)GetVertexAt(i))->RemoveSecondaryNeighbors(this);
}

inline int SimpleTriangle::FindVertexFaceRelativeIndex(Vertex *pVertex)
{
   for (UINT i = 0; i < 3; i++)
      if (GetVertexAt(i)->GetVertexData() == pVertex->GetVertexData())
         return i;
   return -1;
}

inline void SimpleTriangle::BuildFaceNormal()
{
   Vector normal(0,0,0);
   Vector A,B;
   A = m_pVertex[0]->GetPos() - m_pVertex[3]->GetPos();
   B = m_pVertex[1]->GetPos() - m_pVertex[0]->GetPos();
   A.Normalize();
   B.Normalize();
   normal += A ^ B;

   A = m_pVertex[1]->GetPos() - m_pVertex[0]->GetPos();
   B = m_pVertex[2]->GetPos() - m_pVertex[1]->GetPos();
   A.Normalize();
   B.Normalize();
   normal += A ^ B;

   A = m_pVertex[2]->GetPos() - m_pVertex[1]->GetPos();
   B = m_pVertex[3]->GetPos() - m_pVertex[2]->GetPos();
   A.Normalize();
   B.Normalize();
   normal += A ^ B;

   normal.Normalize();

   m_faceNormal = normal;

   m_bDirtyFaceNormal = FALSE;
}

inline Vector *SimpleTriangle::GetFaceNormal()
{
   if (m_bDirtyFaceNormal)
      BuildFaceNormal();
   return &m_faceNormal;
}

inline void SimpleTriangle::SetDirtyFaceNormal( BOOL bDirtyFaceNormal )
{
   if (m_bDirtyFaceNormal == bDirtyFaceNormal)
      return;

   m_bDirtyFaceNormal = bDirtyFaceNormal;

   if (bDirtyFaceNormal) // dirty the vertex normals too
   {
      for (UINT i=0; i< 3; i++)
      {
         VertexNormal *pVertexNormal = m_pVertexNormal[i];
         if (pVertexNormal)
            pVertexNormal->SetVertexNormalDirtyFlag( TRUE );
      }
   }
}

inline SimplePolyUVSet::SimplePolyUVSet()
{
   m_uvs.SetSize(0,8);
}

inline SimplePolyUVSet::~SimplePolyUVSet()
{
}

inline UINT SimplePolyUVSet::GetNumUVs() const
{
   return m_uvs.GetSize();
}

inline void SimplePolyUVSet::SetNumUVs(const UINT nSize)
{
   m_uvs.RemoveAll();
   m_uvs.SetSize(nSize);
}

inline UV *SimplePolyUVSet::GetUVAt(const UINT nIndex) const
{
   ASSERT(nIndex<(UINT)m_uvs.GetSize());
   return m_uvs.GetAt( nIndex );
}

inline void SimplePolyUVSet::SetUVAt(const UINT nIndex,UV *pUV)
{
   ASSERT(nIndex<(UINT)m_uvs.GetSize());
   ASSERT(pUV);
   m_uvs.SetAt( nIndex, pUV );
}

inline SimplePoly::SimplePoly()
{
   m_nSimplePolyFlags = 0;
}

inline SimplePoly::~SimplePoly()
{
}

inline UINT SimplePoly::GetNumVertices() const
{
   return m_vertices.GetSize();
}

inline void SimplePoly::SetNumVertices( UINT nSize )
{
   m_vertices.RemoveAll();
   m_vertices.SetSize( nSize );
}

inline Vertex *SimplePoly::GetVertexAt( UINT nIndex ) const
{
   ASSERT(nIndex<(UINT)m_vertices.GetSize());
   return m_vertices.GetAt(nIndex);
}

inline void SimplePoly::SetVertexAt( UINT nIndex, Vertex *pVertex )
{
   ASSERT(nIndex<(UINT)m_vertices.GetSize());
   m_vertices.SetAt( nIndex, pVertex );
}

inline UINT SimplePoly::GetNumVertexNormals() const
{
   return m_vertexnormals.GetSize();
}

inline void SimplePoly::SetNumVertexNormals( UINT nSize )
{
   m_vertexnormals.RemoveAll();
   m_vertexnormals.SetSize( nSize );
}

inline VertexNormal *SimplePoly::GetVertexNormalAt( UINT nIndex ) const
{
   ASSERT(nIndex<(UINT)m_vertexnormals.GetSize());

   VertexNormal *pVertexNormal = m_vertexnormals.GetAt(nIndex);
   if (pVertexNormal->IsVertexNormalDirty())
   {
      pVertexNormal->SetVertexNormalDirtyFlag(FALSE);
      ((PatchVertex*)m_vertices[nIndex])->CalcSecondaryNormal( pVertexNormal );  
   }

   return m_vertexnormals.GetAt(nIndex);
}

inline void SimplePoly::SetVertexNormalAt( UINT nIndex, VertexNormal *pVertexNormal )
{
   ASSERT(nIndex<(UINT)m_vertexnormals.GetSize());
   m_vertexnormals.SetAt( nIndex, pVertexNormal );
}

inline UINT SimplePoly::GetNumUVSets() const
{
   return m_uvsets.GetSize();
}

inline void SimplePoly::SetNumUVSets(UINT nSize)
{
   m_uvsets.SetSize(nSize);
}

inline UINT SimplePoly::GetNumUVs(UINT nSetIndex) const
{
   return m_uvsets[nSetIndex].GetNumUVs();
}

inline UV *SimplePoly::GetUVAt(UINT nIndex,UINT nSetIndex) const
{
   return m_uvsets[nSetIndex].GetUVAt(nIndex);
}

inline void SimplePoly::SetUVAt(UINT nIndex,UV *pUV,UINT nSetIndex)
{
   m_uvsets[nSetIndex].SetUVAt(nIndex,pUV);
}
/*
//inline void SimplePoly::AddSecondaryNeighbors()
//{
//   for (UINT i = 0; i < GetNumVertices(); i++)
//      ((PatchVertex*)GetVertexAt(i))->AddSecondarySurroundingPoly(this);
//}
//
//inline void SimplePoly::RemoveSecondaryNeighbors()
//{
//   for (UINT i = 0; i < GetNumVertices(); i++)
//      ((PatchVertex*)GetVertexAt(i))->RemoveSecondaryNeighbors(this);
//}
*/
inline int SimplePoly::FindVertexFaceRelativeIndex(Vertex *pVertex)
{
   int i;
   for (i = 0; i < (int)GetNumVertices(); i++)
      if (GetVertexAt(i)->GetVertexData() == pVertex->GetVertexData())
         return i;
   return -1;
}

inline void SimplePoly::BuildFaceNormal()
{
   Vector normal(0,0,0);
   int numVerts = (int)GetNumVertices();
   Vector A,B;
   int i;
   for (i = 0; i < numVerts; i++)
   {
      int prev = i - 1;
      if (prev<0)
         prev = numVerts-1;
      int next = i + 1;
      if (next>numVerts-1)
         next = 0;
      A = m_vertices[i]->GetPos() - m_vertices[prev]->GetPos();
      B = m_vertices[next]->GetPos() - m_vertices[i]->GetPos();
      A.Normalize();
      B.Normalize();
      //     normal += B ^ A;
      normal += A ^ B;
   }
   m_faceNormal = normal.Normalize();
   m_bDirtyFaceNormal = FALSE;
}

inline Vector *SimplePoly::GetFaceNormal()
{
   if (m_bDirtyFaceNormal)
      BuildFaceNormal();
   return &m_faceNormal;
}

inline void SimplePoly::SetDirtyFaceNormal( BOOL bDirtyFaceNormal )
{
   if (m_bDirtyFaceNormal == bDirtyFaceNormal)
      return;

   m_bDirtyFaceNormal = bDirtyFaceNormal;

   if (bDirtyFaceNormal) // dirty the vertex normals too
   {
      for (UINT i=0; i<GetNumVertexNormals(); i++)
      {
         VertexNormal *pVertexNormal = m_vertexnormals[i];
         if (pVertexNormal)
            pVertexNormal->SetVertexNormalDirtyFlag( TRUE );
      }
   }
}


