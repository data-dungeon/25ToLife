//
inline ColorPatchEdge::ColorPatchEdge()
{
   m_pFirstPropertyVertexHandle = SNEW ColorVertexHandle;
   m_pSecondPropertyVertexHandle = SNEW ColorVertexHandle;
}

inline ColorPatchEdge::ColorPatchEdge( UINT nID )
: PropertyPatchEdge( nID )
{
   m_pFirstPropertyVertexHandle = SNEW ColorVertexHandle;
   m_pSecondPropertyVertexHandle = SNEW ColorVertexHandle;
}

inline ColorPatchEdge::~ColorPatchEdge()
{
}


inline Vector4 ColorPatchEdge::GetAlignedFirstColorVertexTangentPos( ColorPoly *pColorPoly )
{
   if (IsAligned( pColorPoly ))
      return GetFirstColorVertexTangentPos();

   return GetSecondColorVertexTangentPos();
}

inline Vector4 ColorPatchEdge::GetAlignedSecondColorVertexTangentPos( ColorPoly *pColorPoly )
{
   if (IsAligned( pColorPoly ))
      return GetSecondColorVertexTangentPos();

   return GetFirstColorVertexTangentPos();
}
