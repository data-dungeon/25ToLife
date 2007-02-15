
inline void UVPoly::Init()
{
   m_pNextUVPoly = NULL;
   m_pPoly = NULL;
   m_pUVs = NULL;
   m_pUVEdges = NULL;
}

inline UVPoly::UVPoly()
{
   Init();
}

inline UVPoly::UVPoly( UINT nID )
: Component( nID )
{
   Init();
}

inline int UVPoly::FindUVFaceRelativeIndex( const UV *pFind) const
{
   for (UINT i = 0; i < GetNumSides(); i++)
   {
      UV *pUV = GetUVAt(i);
      if (pUV && pUV == pFind)
         return i;
   }
   return -1;
}

inline int UVPoly::FindUVFaceRelativeIndex( const UVEdge *pFind ) const
{
   for (UINT i = 0; i < GetNumSides(); i++)
   {
      UVEdge *pUVEdge = GetUVEdgeAt(i);
      if (pUVEdge && pUVEdge == pFind)
         return i;
   }
   return -1;
}

inline BOOL UVEdge::IsAligned( UVPoly *pUVPoly ) const
{
   int nIndex = pUVPoly->FindUVFaceRelativeIndex( this );
   ASSERT(nIndex != -1);

   UV *pUV0 = pUVPoly->GetUVAt( nIndex );
   UV *pUV1 = pUVPoly->GetUVAt( pUVPoly->GetNextIndex( nIndex) );
   
   return GetFirstUV() == pUV0 && GetSecondUV() == pUV1;
}