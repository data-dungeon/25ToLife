
/////////////////////////////////////////////////////

inline TessPoly::TessPoly()
{
   m_nTessPolyFlags = 0;
   m_pVertices = NULL;
   m_pRTVertexIndices = NULL;
   m_pVertexNormals = NULL;
   m_pPatch = NULL;
   m_pHeadTessPropertyPoly = NULL;
}

inline PatchVertex *TessPoly::GetVertexAt( UINT nIndex ) const
{ 
   return m_pVertices[nIndex];
}

inline VertexNormal *TessPoly::GetVertexNormalAt( UINT nIndex, BOOL bCalculateIfNecessary/*=TRUE*/ ) const
{
   VertexNormal *pVertexNormal = m_pVertexNormals[nIndex];

   ASSERT( pVertexNormal );

   if (bCalculateIfNecessary && pVertexNormal->IsVertexNormalDirty())
   {
      // OK, if our normal is dirty then we most make sure that all surrounding tess uv polys are built.
      // Tessellate patch will do this for us.  It was made into a function so
      // this function could remain inline, whereas TessellatePatch can not
      // be inline because it uses patch functions.
      TessellatePatch();
      GetVertexAt(nIndex)->CalcSecondaryNormal( pVertexNormal );
   }

   return pVertexNormal;
}

inline int TessPoly::FindVertexFaceRelativeIndex( PatchVertex *pVertex )
{
   for (int i = 0; i < (int)GetNumSides(); i++)
   {
      if (GetVertexAt(i)==pVertex)
         return i;
   }
   return -1;
}

inline Vector *TessPoly::GetFaceNormal()
{
   if (m_bDirtyFaceNormal)
      BuildFaceNormal();

   return &m_vFaceNormal;
}

inline UINT TessPoly::GetRTVertexAt( UINT nIndex ) const
{
   ASSERT(nIndex < GetNumSides());
   return m_pRTVertexIndices[nIndex];
}

inline void TessPoly::SetRTVertexAt( UINT nIndex, UINT nRTVertex )
{
   ASSERT(nIndex < GetNumSides());
   m_pRTVertexIndices[nIndex] = nRTVertex;
}

inline UINT TessPoly::GetNextIndex( UINT nIndex ) const
{
   nIndex++;
   if (nIndex >= GetNumSides())
      nIndex = 0;

   return nIndex;
}

inline UINT TessPoly::GetPrevIndex( UINT nIndex ) const
{
   if (nIndex == 0)
      nIndex = GetNumSides();

   nIndex--;
   return nIndex;
}
