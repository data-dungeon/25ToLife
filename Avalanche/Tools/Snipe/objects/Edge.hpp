inline void Edge::Init()
{
   m_vertexarray[0] = NULL;
   m_vertexarray[1] = NULL;
   m_nEdgeFlags = 0;
   m_pEdgePolyInfo[0] = NULL;
   m_pEdgePolyInfo[1] = NULL;
   m_bSoftDirty = TRUE;
   m_pHeadPropertyEdge = NULL;
   m_bPropertyEdgeListDirty = TRUE;
}

inline Edge::Edge()
{
   Init();
}

inline Edge::Edge( UINT nID )
: Component( nID )
{
   Init();
}

inline Edge &Edge::operator = ( const Edge &other )
{
   SetFirstVertex( other.GetFirstVertex());
   SetSecondVertex( other.GetSecondVertex());

   m_nEdgeFlags = other.m_nEdgeFlags;

   return *this;
}

inline void Edge::SetFirstVertex(Vertex *pVertex)
{
   if (GetFirstVertex())
   {
      GetFirstVertex()->RemoveSurroundingEdge( this );
      GetFirstVertex()->DeReference();
   }

   m_vertexarray[0] = pVertex;
   if (pVertex)
   {
      pVertex->Reference();
      pVertex->AddSurroundingEdge((Edge*)this);
   }
}

inline void Edge::SetSecondVertex(Vertex *pVertex)
{
   if (GetSecondVertex())
   {
      GetSecondVertex()->RemoveSurroundingEdge( this );
      GetSecondVertex()->DeReference();
   }

   m_vertexarray[1] = pVertex;
   if (pVertex)
   {
      pVertex->Reference();
      pVertex->AddSurroundingEdge((Edge*)this);
   }
   SetDirty();
}

inline BOOL Edge::IsVertexPeaked(Vertex *pVertex) const
{
   if (GetFirstVertex() == pVertex)
      return IsFirstVertexPeaked();
   else
   {
      ASSERT(GetSecondVertex() == pVertex);
      return IsSecondVertexPeaked();
   }
}

inline Vertex *Edge::GetSharedVertex( Edge *pEdge ) const
{
   if (GetFirstVertex() == pEdge->GetFirstVertex() || GetFirstVertex() == pEdge->GetSecondVertex())
      return GetFirstVertex();
   if (GetSecondVertex() == pEdge->GetFirstVertex() || GetSecondVertex() == pEdge->GetSecondVertex())
      return GetSecondVertex();
   return NULL;
}

inline Vertex *Edge::GetVertexOnEdge( Vertex *pVertex )
{
   if (GetFirstVertex()==pVertex)
      return GetFirstVertex();
   else if (GetSecondVertex()==pVertex)
      return GetSecondVertex();

   return NULL;
}

inline Poly *Edge::GetFirstPoly()
{
   if (m_pEdgePolyInfo[0])
      return m_pEdgePolyInfo[0]->m_pPoly;
   return NULL;
}

inline Poly *Edge::GetSecondPoly()
{
   if (m_pEdgePolyInfo[1])
      return m_pEdgePolyInfo[1]->m_pPoly;
   return NULL;
}

inline void Edge::SetFirstPoly(Poly *pPoly,UINT nEdgeIndex)
{
   if (!pPoly)
   {
      if (m_pEdgePolyInfo[0])
      {
         delete m_pEdgePolyInfo[0];
         m_pEdgePolyInfo[0] = NULL;
      }
      return;
   }
   if (!m_pEdgePolyInfo[0])
      m_pEdgePolyInfo[0] = CreateNewEdgePolyInfo();
   m_pEdgePolyInfo[0]->m_pPoly = pPoly;
   m_pEdgePolyInfo[0]->m_nEdgeIndex = nEdgeIndex;
}

inline void Edge::SetSecondPoly(Poly *pPoly,UINT nEdgeIndex)
{
   if (!pPoly)
   {
      if (m_pEdgePolyInfo[1])
         delete m_pEdgePolyInfo[1];
      m_pEdgePolyInfo[1]=NULL;
      return;
   }
   if (!m_pEdgePolyInfo[1])
      m_pEdgePolyInfo[1] = CreateNewEdgePolyInfo();
   m_pEdgePolyInfo[1]->m_pPoly = pPoly;
   m_pEdgePolyInfo[1]->m_nEdgeIndex = nEdgeIndex;
}

inline Poly *Edge::GetPoly()
{
   if (m_pEdgePolyInfo[0])
      return m_pEdgePolyInfo[0]->m_pPoly;
   if (m_pEdgePolyInfo[1])
      return m_pEdgePolyInfo[1]->m_pPoly;
   return NULL;
}

inline Poly *Edge::GetConnectedPoly(Poly *pNotThis)
{
   if ( m_pEdgePolyInfo[0] && m_pEdgePolyInfo[0]->m_pPoly != pNotThis )
      return m_pEdgePolyInfo[0]->m_pPoly;

   if ( m_pEdgePolyInfo[1] && m_pEdgePolyInfo[1]->m_pPoly != pNotThis )
      return m_pEdgePolyInfo[1]->m_pPoly;
      
   return NULL;
}

inline Vertex *Edge::GetConnectedVertex( Vertex *pNotThis ) const
{
   if (GetFirstVertex()==pNotThis)
      return GetSecondVertex();
   else
   {
      ASSERT(GetSecondVertex()==pNotThis);
      return GetFirstVertex();
   }
}

inline EdgePolyInfo *Edge::GetEdgePolyInfo(Poly *pPoly)
{
   if (m_pEdgePolyInfo[0] && m_pEdgePolyInfo[0]->m_pPoly==pPoly)
      return m_pEdgePolyInfo[0];
   else if (m_pEdgePolyInfo[1] && m_pEdgePolyInfo[1]->m_pPoly==pPoly)
      return m_pEdgePolyInfo[1];
   else
   {
      //ASSERT(FALSE);// POLY COULD NOT BE FOUND
      return NULL;
   }
}

inline BYTE Edge::GetFaceRelativeIndex(Poly *pPoly)
{
   EdgePolyInfo *pEdgePolyInfo = GetEdgePolyInfo(pPoly);
   if (pEdgePolyInfo)
      return pEdgePolyInfo->m_nEdgeIndex;
   return -1;
}

inline BOOL Edge::SetFaceRelativeIndex(Poly *pPoly,BYTE nIndex)
{
   EdgePolyInfo *pEdgePolyInfo = GetEdgePolyInfo(pPoly);
   if (pEdgePolyInfo)
      pEdgePolyInfo->m_nEdgeIndex = nIndex;
   return FALSE;
}

inline BOOL Edge::IsSoftEdge()
{
   if (m_bSoftDirty)
   {
      Poly *pPoly0 = GetFirstPoly();
      Poly *pPoly1 = GetSecondPoly();
      m_bSoft = pPoly0 && pPoly1 && GetFirstVertex()->DoesShareVertexNormal( pPoly0, pPoly1 ) && GetSecondVertex()->DoesShareVertexNormal( pPoly0, pPoly1 );
      SetSoftEdgeDirtyFlag( FALSE );
   }
   return m_bSoft;
}

inline BOOL Edge::IsBorderEdge() const
{ 
   return ((m_pEdgePolyInfo[0] && !m_pEdgePolyInfo[1])
        || (m_pEdgePolyInfo[1] && !m_pEdgePolyInfo[0]));
}

inline BOOL Edge::IsDangle() const
{ 
   return !m_pEdgePolyInfo[0] && !m_pEdgePolyInfo[1];
}

inline Vertex *Edge::GetAlignedFirstVertex(Poly *pPoly)
{
   if (IsAligned(pPoly))
      return GetFirstVertex();
   return GetSecondVertex();
}

inline Vertex *Edge::GetAlignedSecondVertex(Poly *pPoly)
{
   if (IsAligned(pPoly))
      return GetSecondVertex();
   return GetFirstVertex();
}

inline BOOL Edge::HasPoly(Poly *pPoly)
{
   if ( m_pEdgePolyInfo[0] && m_pEdgePolyInfo[0]->m_pPoly==pPoly )
      return TRUE;

   if ( m_pEdgePolyInfo[1] && m_pEdgePolyInfo[1]->m_pPoly==pPoly )
      return TRUE;

   return FALSE;
}
