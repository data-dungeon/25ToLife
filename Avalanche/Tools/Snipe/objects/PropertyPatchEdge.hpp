//

inline void PropertyPatchEdge::Init()
{
    SetTessellationDirty( TRUE );
    m_pFirstPropertyVertexHandle = NULL;
    m_pSecondPropertyVertexHandle = NULL;

}

inline PropertyVertexHandle *PropertyPatchEdge::GetFirstPropertyVertexHandle()
{
   if (IsTangentsDirty())
      BuildTangents();
   return m_pSecondPropertyVertexHandle;
}

inline PropertyVertexHandle *PropertyPatchEdge::GetSecondPropertyVertexHandle()
{
   if (IsTangentsDirty())
      BuildTangents();
   return m_pFirstPropertyVertexHandle;
}

inline PropertyVertexHandle *PropertyPatchEdge::GetAlignedFirstPropertyVertexHandle( PropertyPoly *pPropertyPoly )
{
   if (IsAligned( pPropertyPoly ))
      return GetFirstPropertyVertexHandle();
   return GetSecondPropertyVertexHandle();
}

inline PropertyVertexHandle *PropertyPatchEdge::GetAlignedSecondPropertyVertexHandle( PropertyPoly *pPropertyPoly )
{
   if (IsAligned( pPropertyPoly ))
      return GetSecondPropertyVertexHandle();
   return GetFirstPropertyVertexHandle();
}

inline PropertyVertexHandle *PropertyPatchEdge::GetPropertyVertexHandle( PropertyVertex *pPropertyVertex )
{
   if (pPropertyVertex == (PropertyVertex*)GetFirstPropertyVertex())
      return GetFirstPropertyVertexHandle();
   else if (pPropertyVertex == (PropertyVertex*)GetSecondPropertyVertex())
      return GetSecondPropertyVertexHandle();
   else
      return NULL;
}

inline void PropertyPatchEdge::SetNumTessPropertyVertices( UINT nNumUVs )
{
   m_tessPropertyVertices.SetNumPropertyVertices( nNumUVs );
   SetTessellationDirty( TRUE );
}

inline void PropertyPatchEdge::SetTessPropertyVertexAt( UINT nIndex, PropertyVertex *pPropertyVertex )
{
   m_tessPropertyVertices.SetPropertyVertexAt( nIndex, pPropertyVertex );
}
