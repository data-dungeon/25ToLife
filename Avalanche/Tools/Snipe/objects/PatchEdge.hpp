
inline BOOL PatchEdge::IsVertexConstrained(Vertex *pVertex) const
{
   if (GetFirstVertex() == pVertex)
      return IsFirstVertexConstrained();
   else
   {
      ASSERT(GetSecondVertex() == pVertex);
      return IsSecondVertexConstrained();
   }
}

inline VertexHandle *PatchEdge::GetFirstVertexHandle(BOOL bBuildTangentIfDirty/*=TRUE*/)
{
   if (bBuildTangentIfDirty && m_bTangentsDirty)
      BuildTangents();

   return &m_firstVertexHandle;
}

inline VertexHandle *PatchEdge::GetSecondVertexHandle(BOOL bBuildTangentIfDirty/*=TRUE*/)
{
   if (bBuildTangentIfDirty && m_bTangentsDirty)
      BuildTangents();

   return &m_secondVertexHandle;
}

inline VertexHandle *PatchEdge::GetVertexHandleForVertex(Vertex *pVertex, BOOL bBuildTangentIfDirty/*=TRUE*/)
{
   if (GetFirstVertex()==pVertex)
      return GetFirstVertexHandle(bBuildTangentIfDirty);
   else
   {
      ASSERT(GetSecondVertex()==pVertex);
      return GetSecondVertexHandle(bBuildTangentIfDirty);
   }
}

inline VertexHandle *PatchEdge::GetAlignedFirstVertexHandle( Poly *pPoly )
{
   if (IsAligned( pPoly ))
      return GetSecondVertexHandle();

   return GetFirstVertexHandle();
}

inline VertexHandle *PatchEdge::GetAlignedSecondVertexHandle( Poly *pPoly )
{
   if (IsAligned( pPoly ))
      return GetFirstVertexHandle();

   return GetSecondVertexHandle();
}

inline Patch *PatchEdge::GetPatch()
{
   return (Patch*)GetPoly();
}

inline Patch *PatchEdge::GetConnectedPatch(Patch *pPatch)
{
   return (Patch*)GetConnectedPoly((Poly*)pPatch);
}

inline EdgePatchInfo *PatchEdge::GetEdgePatchInfo( Poly *pPoly)
{
   return (EdgePatchInfo *) Edge::GetEdgePolyInfo( pPoly );
}

inline PatchVertex *PatchEdge::GetPrevContVertex() const
{
   if (m_pPrevContEdge)
      return (PatchVertex *)GetSharedVertex(m_pPrevContEdge);
   else if (m_pNextContEdge)
      return (PatchVertex *)GetConnectedVertex((Vertex *)GetNextContVertex());
   else
      return (PatchVertex *)GetFirstVertex();
}

inline PatchVertex *PatchEdge::GetNextContVertex() const
{
   if (m_pNextContEdge)
      return (PatchVertex *)GetSharedVertex(m_pNextContEdge);
   else if (m_pPrevContEdge)
      return (PatchVertex *)GetConnectedVertex((Vertex *)GetPrevContVertex());
   else
      return (PatchVertex *)GetSecondVertex();
}

inline PatchEdge *PatchEdge::GetPrevContEdge(BOOL bLoop)
{
   if (bLoop || (m_pPrevContEdge && !m_pPrevContEdge->IsContinuityLoop()))
      return m_pPrevContEdge;
   else
      return NULL;
}

inline PatchEdge *PatchEdge::GetNextContEdge(BOOL bLoop)
{
   if (bLoop || !IsContinuityLoop())
      return m_pNextContEdge;
   else
      return NULL;
}

inline void PatchEdge::SetContEdge( PatchVertex *pVertex, PatchEdge *pEdge )
{
   if (GetPrevContVertex() == pVertex)
      SetPrevContEdge(pEdge);
   else
   {
      ASSERT(GetNextContVertex() == pVertex);
      SetNextContEdge(pEdge);
   }
}

inline BOOL PatchEdge::IsAlignedWithContinuity() const
{
   return GetFirstVertex() == (Vertex *)GetPrevContVertex();
}

inline VertexHandle *PatchEdge::GetVertexHandle(PatchVertex *pPatchVertex)
{
   if (pPatchVertex == (PatchVertex*)GetFirstVertex())
      return &m_firstVertexHandle;
   else if (pPatchVertex == (PatchVertex*)GetSecondVertex())
      return &m_secondVertexHandle;
   else
      return NULL;
}