////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TessEdgeVertices
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline TessEdgeVertices::~TessEdgeVertices()
{
   Clear();
}

inline UINT TessEdgeVertices::GetNumVertices() const
{
   return m_vertices.GetSize();
}

inline void TessEdgeVertices::SetNumVertices( UINT nSize )
{
   Clear();
   m_vertices.SetSize(nSize);
}

inline PatchVertex *TessEdgeVertices::GetVertexAt( UINT nIndex ) const
{
   return (PatchVertex*)m_vertices[nIndex];
}

inline void TessEdgeVertices::SetVertexAtGrow( UINT nIndex, PatchVertex *pVertex )
{
   if (nIndex >= GetNumVertices())
      SetNumVertices( nIndex+1 );
   SetVertexAt( nIndex, pVertex );
}

inline CArray<PatchVertex *> *TessEdgeVertices::GetVertices()
{
   return &m_vertices;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TessEdgeNormals
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline TessEdgeNormals::~TessEdgeNormals()
{
   Clear();
}

inline UINT TessEdgeNormals::GetNumVertexNormals()
{
   return m_vertexnormals.GetSize();
}

inline void TessEdgeNormals::SetNumVertexNormals( UINT nSize )
{
   Clear();
   m_vertexnormals.SetSize(nSize);
}
   
inline VertexNormal *TessEdgeNormals::GetVertexNormalAt( UINT nIndex ) const
{
   return m_vertexnormals[nIndex];
}

inline void TessEdgeNormals::SetVertexNormalAtGrow( UINT nIndex, VertexNormal *pVertexNormal )
{
   if (nIndex >= GetNumVertexNormals())
      SetNumVertexNormals( nIndex+1 );

   SetVertexNormalAt( nIndex, pVertexNormal );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TessPropertyEdge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline TessPropertyEdge::~TessPropertyEdge()
{
   Clear();
}

inline UINT TessPropertyEdge::GetNumPropertyVertices() const
{
   return m_PropertyVertices.GetSize();
}

inline void TessPropertyEdge::SetNumPropertyVertices( UINT nSize )
{
   Clear();
   m_PropertyVertices.SetSize(nSize);
}

inline PropertyVertex *TessPropertyEdge::GetPropertyVertexAt( UINT nIndex, BOOL bReversed ) const
{
   if (bReversed)
      nIndex = m_PropertyVertices.GetCount()-nIndex-1;

   return m_PropertyVertices[nIndex];
}
