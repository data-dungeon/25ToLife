
inline void PropertyPoly::Init()
{
   m_pNextPropertyPoly = NULL;
   m_pPoly = NULL;
   m_pPropertyVertices = NULL;
   m_pPropertyEdges = NULL;
}

inline PropertyPoly::PropertyPoly()
{
   Init();
}

inline PropertyPoly::PropertyPoly( UINT nID )
: Component( nID )
{
   Init();
}

inline int PropertyPoly::FindPropertyVertexFaceRelativeIndex( const PropertyVertex *pFind) const
{
   for (UINT i = 0; i < GetNumSides(); i++)
   {
      PropertyVertex *pPropertyVertex = GetPropertyVertexAt(i);
      if (pPropertyVertex && pPropertyVertex == pFind)
         return i;
   }
   return -1;
}

inline int PropertyPoly::FindPropertyVertexFaceRelativeIndex( const PropertyEdge *pFind ) const
{
   for (UINT i = 0; i < GetNumSides(); i++)
   {
      PropertyEdge *pPropertyEdge = GetPropertyEdgeAt(i);
      if (pPropertyEdge && pPropertyEdge == pFind)
         return i;
   }
   return -1;
}
