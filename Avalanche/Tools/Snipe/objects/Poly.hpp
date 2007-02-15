
inline void Poly::Init()
{
   m_nPolyFlags = 0;
   m_pVertices = NULL;
   m_pRTVertexIndices = NULL;
   m_pVertexNormals = NULL;
   m_pEdges = NULL;
   m_pSurfaces = NULL;
   m_pCompositedSurface = NULL;
   m_bRemoveEdgesOnDisconnectIfNotUsed = TRUE;
   m_pHeadPropertyPoly = NULL;
}

inline Poly::Poly()
{
   Init();
   SetDirty();
}

inline Poly::Poly( UINT nID )
: Component( nID )
{
   Init();
}

/////// VERTICES

inline UINT Poly::GetNumSides() const
{
   return m_nNumSides;
}

inline Vertex *Poly::GetVertexAt( UINT nIndex ) const
{
   ASSERT(nIndex < GetNumSides());
   return m_pVertices[nIndex];
}

inline UINT Poly::GetRTVertexAt( UINT nIndex ) const
{
   ASSERT(nIndex < GetNumSides());
   return m_pRTVertexIndices[nIndex];
}

inline void Poly::SetRTVertexAt( UINT nIndex, UINT nRTVertex )
{
   ASSERT(nIndex < GetNumSides());
   m_pRTVertexIndices[nIndex] = nRTVertex;
}

inline BOOL Poly::IsInFace( Vertex *pVertex )
{
  for (UINT i=0; i<GetNumSides(); i++)
     if (GetVertexAt(i)==pVertex)
        return TRUE;
  return FALSE;
}

/////// VERTEXNORMALS
inline VertexNormal *Poly::GetVertexNormalAt( UINT nIndex, BOOL bCalculateIfNecessary/*=TRUE*/ )
{
   ASSERT(nIndex < GetNumSides());

   VertexNormal *pVertexNormal = m_pVertexNormals[nIndex];
   if (bCalculateIfNecessary && pVertexNormal && pVertexNormal->IsVertexNormalDirty())
   {
      pVertexNormal->SetVertexNormalDirtyFlag(FALSE);
      m_pVertices[nIndex]->CalcNormal( pVertexNormal );
   }

   return pVertexNormal;
}

inline UINT Poly::GetNextIndex( UINT nIndex ) const
{
   nIndex++;
   if (nIndex >= GetNumSides())
      nIndex = 0;

   return nIndex;
}

inline UINT Poly::GetPrevIndex( UINT nIndex ) const
{
   if (nIndex == 0)
      nIndex = GetNumSides();
   
   nIndex--;
   return nIndex;
}

inline int Poly::FindVertexFaceRelativeIndex(Vertex *pFind, BOOL bIncludeAttached/*=TRUE*/)
{
   for (UINT i = 0; i < GetNumSides(); i++)
   {
      Vertex *pVertex = GetVertexAt(i);
      if (pVertex)
      {
         if (bIncludeAttached)
         {
            if (pVertex==pFind)
               return i;
         }
         else 
         {
            if (pVertex == pFind)
               return i;
         }
      }
   }
   return -1;
}

inline int Poly::FindVertexNormalFaceRelativeIndex(VertexNormal *pFind)
{
   for (UINT i = 0; i < GetNumSides(); i++)
   {
      VertexNormal *pVertexNormal = GetVertexNormalAt(i, FALSE);
      if (pVertexNormal == pFind)
         return i;
   }
   return -1;
}

inline void Poly::CalcCenter(Vector &pos)
{
   pos = Vector(0,0,0);
   UINT i;
   for (i = 0; i < GetNumSides(); i++)
      pos += GetVertexAt(i)->GetPos();
   pos /= GetNumSides();
}

inline Vector *Poly::GetFaceNormal()
{
   if (IsDirtyFaceNormal())
      BuildFaceNormal();
   return &m_faceNormal;
}

inline UINT Poly::GetNumSurfaces() const
{
   return m_nNumSurfaces;
}

inline void Poly::SetNumSurfaces(UINT nNumSurfaces)
{
   ASSERT( m_nNumSurfaces==0 ); // if we want to allow resetting, we need to dereference the old ones

   m_nNumSurfaces = nNumSurfaces;
   m_pSurfaces = SNEW Surface *[nNumSurfaces];
   memset( m_pSurfaces, 0, nNumSurfaces * sizeof( Surface *) );
}

inline Surface *Poly::GetSurfaceAt(UINT nIndex) const
{
   ASSERT(nIndex < m_nNumSurfaces);
   return m_pSurfaces[nIndex];
}

inline void Poly::SetSurfaceAt(UINT nIndex,Surface *pSurface)
{
   ASSERT(nIndex < m_nNumSurfaces);
   ASSERT(GetSurfaceAt(nIndex) == NULL);

   m_pSurfaces[nIndex] = pSurface;
   SetDirty();
}

// the more layered textures the slower this gets
inline UINT Poly::FindSurface( Surface *pSurface )
{
   for (UINT i = 0; i < GetNumSurfaces(); i++)
   {
      if (GetSurfaceAt(i) == pSurface)
         return i;
   }

   return UINT(-1);
}
