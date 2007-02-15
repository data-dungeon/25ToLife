
inline PropertyVertex *PropertyEdge::GetAlignedFirstPropertyVertex( PropertyPoly *pPropertyPoly ) const
{
   if (IsAligned( pPropertyPoly ))
      return GetFirstPropertyVertex();

   return GetSecondPropertyVertex();
}

inline PropertyVertex *PropertyEdge::GetAlignedSecondPropertyVertex( PropertyPoly *pPropertyPoly ) const
{
   if (IsAligned( pPropertyPoly ))
      return GetSecondPropertyVertex();

   return GetFirstPropertyVertex();
}

inline PropertyPoly *PropertyEdge::GetFirstPropertyPoly()
{
   return m_propertypolys[0];
}

inline PropertyPoly *PropertyEdge::GetSecondPropertyPoly()
{
   return m_propertypolys[1];
}

inline void PropertyEdge::SetFirstPropertyPoly(PropertyPoly *pPropertyPoly)
{
   m_propertypolys[0] = pPropertyPoly;
}

inline void PropertyEdge::SetSecondPropertyPoly(PropertyPoly *pPropertyPoly)
{
   m_propertypolys[1] = pPropertyPoly;
}

inline PropertyPoly *PropertyEdge::GetPropertyPoly()
{
   if (m_propertypolys[0])
      return m_propertypolys[0];

   else if (m_propertypolys[1])
      return m_propertypolys[1];

   return NULL;
}

inline PropertyPoly *PropertyEdge::GetConnectedPropertyPoly(PropertyPoly *pNotThis)
{
   if (m_propertypolys[0] && m_propertypolys[0] != pNotThis)
      return m_propertypolys[0];

   if (m_propertypolys[1] && m_propertypolys[1] != pNotThis)
      return m_propertypolys[1];

   return NULL;
}

inline BOOL PropertyEdge::IsSoftEdge()
{
   if (m_propertypolys[0] && m_propertypolys[1])
      return TRUE;

   return FALSE;
}

inline BOOL PropertyEdge::IsBorderEdge() const
{ 
   return (m_propertypolys[0] && !m_propertypolys[1])
      || (m_propertypolys[1] && !m_propertypolys[0]);
}

inline PropertyVertex *PropertyEdge::GetSharedPropertyVertex( PropertyEdge *pPropertyEdge ) const
{
   if (GetFirstPropertyVertex() == pPropertyEdge->GetFirstPropertyVertex() || GetFirstPropertyVertex() == pPropertyEdge->GetSecondPropertyVertex())
      return GetFirstPropertyVertex();
   if (GetSecondPropertyVertex() == pPropertyEdge->GetFirstPropertyVertex() || GetSecondPropertyVertex() == pPropertyEdge->GetSecondPropertyVertex())
      return GetSecondPropertyVertex();
   return NULL;
}

inline PropertyVertex *PropertyEdge::GetConnectedPropertyVertex( PropertyVertex *pNotThis )
{
   if (GetFirstPropertyVertex()==pNotThis)
      return GetSecondPropertyVertex();
   else
   {
      ASSERT(GetSecondPropertyVertex()==pNotThis);
      return GetFirstPropertyVertex();
   }
}

inline BOOL PropertyEdge::IsPropertyVertexPeaked(PropertyVertex *pPropertyVertex) const
{
   if (GetFirstPropertyVertex() == pPropertyVertex)
      return GetEdge()->IsFirstVertexPeaked();
   else
   {
      ASSERT(GetSecondPropertyVertex() == pPropertyVertex);
      return GetEdge()->IsSecondVertexPeaked();
   }
}