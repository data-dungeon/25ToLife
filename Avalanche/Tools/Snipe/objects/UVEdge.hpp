
inline UV *UVEdge::GetAlignedFirstUV( UVPoly *pUVPoly ) const
{
   if (IsAligned( pUVPoly ))
      return GetFirstUV();

   return GetSecondUV();
}

inline UV *UVEdge::GetAlignedSecondUV( UVPoly *pUVPoly ) const
{
   if (IsAligned( pUVPoly ))
      return GetSecondUV();

   return GetFirstUV();
}

inline UVPoly *UVEdge::GetFirstUVPoly()
{
   return m_uvpolys[0];
}

inline UVPoly *UVEdge::GetSecondUVPoly()
{
   return m_uvpolys[1];
}

inline void UVEdge::SetFirstUVPoly(UVPoly *pUVPoly)
{
   m_uvpolys[0] = pUVPoly;
}

inline void UVEdge::SetSecondUVPoly(UVPoly *pUVPoly)
{
   m_uvpolys[1] = pUVPoly;
}

inline UVPoly *UVEdge::GetUVPoly()
{
   if (m_uvpolys[0])
      return m_uvpolys[0];

   else if (m_uvpolys[1])
      return m_uvpolys[1];

   return NULL;
}

inline UVPoly *UVEdge::GetConnectedUVPoly(UVPoly *pNotThis)
{
   if (m_uvpolys[0] && m_uvpolys[0] != pNotThis)
      return m_uvpolys[0];

   if (m_uvpolys[1] && m_uvpolys[1] != pNotThis)
      return m_uvpolys[1];

   return NULL;
}

inline BOOL UVEdge::IsSoftEdge()
{
   if (m_uvpolys[0] && m_uvpolys[1])
      return TRUE;

   return FALSE;
}

inline BOOL UVEdge::IsBorderEdge() const
{ 
   return (m_uvpolys[0] && !m_uvpolys[1])
      || (m_uvpolys[1] && !m_uvpolys[0]);
}

inline Vector2 UVEdge::GetPosAlongEdge( float fPosition, UVPoly *pUVPoly )
{
   ASSERT(fPosition >= 0.0 && fPosition <= 1.0f);

   UV *pFirstUV = GetAlignedFirstUV(pUVPoly);
   UV *pSecondUV = GetAlignedSecondUV(pUVPoly);

   return (pSecondUV->GetPos() - pFirstUV->GetPos()) * fPosition + pFirstUV->GetPos();
}

inline UV *UVEdge::GetSharedUV( UVEdge *pUVEdge ) const
{
   if (GetFirstUV() == pUVEdge->GetFirstUV() || GetFirstUV() == pUVEdge->GetSecondUV())
      return GetFirstUV();
   if (GetSecondUV() == pUVEdge->GetFirstUV() || GetSecondUV() == pUVEdge->GetSecondUV())
      return GetSecondUV();
   return NULL;
}

inline UV *UVEdge::GetConnectedUV( UV *pNotThis )
{
   if (GetFirstUV()==pNotThis)
      return GetSecondUV();
   else
   {
      ASSERT(GetSecondUV()==pNotThis);
      return GetFirstUV();
   }
}