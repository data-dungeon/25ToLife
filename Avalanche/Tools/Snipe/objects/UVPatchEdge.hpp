//

inline UVPatchEdge::UVPatchEdge()
{
   m_pFirstPropertyVertexHandle = SNEW UVHandle;
   m_pSecondPropertyVertexHandle = SNEW UVHandle;
}

inline UVPatchEdge::UVPatchEdge( UINT nID )
: PropertyPatchEdge( nID )
{
   m_pFirstPropertyVertexHandle = SNEW UVHandle;
   m_pSecondPropertyVertexHandle = SNEW UVHandle;
}

inline UVPatchEdge::~UVPatchEdge()
{
}

inline Vector2 UVPatchEdge::GetAlignedFirstUVTangentPos( UVPoly *pUVPoly )
{
   if (IsAligned( pUVPoly ))
      return GetFirstUVTangentPos();

   return GetSecondUVTangentPos();
}

inline Vector2 UVPatchEdge::GetAlignedSecondUVTangentPos( UVPoly *pUVPoly )
{
   if (IsAligned( pUVPoly ))
      return GetSecondUVTangentPos();

   return GetFirstUVTangentPos();
}
