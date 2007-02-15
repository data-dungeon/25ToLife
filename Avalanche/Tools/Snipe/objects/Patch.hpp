inline void Patch::Init()
{
   m_pNextDirtyPatch = NULL;
   m_pPrevDirtyPatch = NULL;
   m_nNumTessPolys = 0;
   m_tesspolyarray = NULL;
   m_nMaxSubdivision = 0;
   m_pNPatchData = NULL;
   m_bDirtyTangents = TRUE;
   m_bDirtyTessellation = TRUE;
   m_bDirtyTessellationAllocation = TRUE;
}

inline Patch::Patch()
{
   Init();
}

inline Patch::Patch( UINT nID )
: Poly( nID )
{
   Init();
}

inline BOOL Patch::IsQuadPatch()
{
   UINT nNumVerts = GetNumSides();
   if (nNumVerts==4)
      return TRUE;
   return FALSE;
}

inline BOOL Patch::IsNPatch()
{
   if (GetNumSides()>4 || GetNumSides()==3)
      return TRUE;

   return FALSE;
}

inline void Patch::SetNumSides( UINT nNumSides )
{
   Poly::SetNumSides(nNumSides);

   UINT nNumSubPatches;

   if (IsQuadPatch())
      nNumSubPatches = 1;
   else
      nNumSubPatches = nNumSides;

   SetNumSubPatches(nNumSubPatches);
}

inline void Patch::SetNumSubPatches(UINT nSize)
{
   ASSERT( nSize > 0 );

   if (m_quads.GetSize() == nSize)
      return;

#if 0
   ClearTessellatedComponents();
#endif

   for ( PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly = pPropertyPoly->GetNextPropertyPoly() )
      ((PropertyPatch *)pPropertyPoly)->SetNumBasisMatrices( nSize );

   // npatch shrinks to quadpatch
   if ( m_pNPatchData && m_quads.GetSize()>1 && nSize == 1 )
   {
      delete m_pNPatchData;
      m_pNPatchData = NULL;
   }

   m_quads.RemoveAll();
   m_quads.SetSize(nSize);

   // if npatch then allocate npatch data
   if ( nSize>1 && !m_pNPatchData )
      m_pNPatchData = SNEW NPatchData;
}

inline UINT Patch::GetNumSubPatches() const
{
   return m_quads.GetSize();
}

inline Vector *Patch::GetCVs(UINT nSubPatchIndex)
{
   ASSERT(nSubPatchIndex<(UINT)m_quads.GetSize());
   if (m_bDirtyTangents)
      BuildTangents();
   return m_quads[nSubPatchIndex].m_cvs;	
}

inline Vector *Patch::GetVNCVs(UINT nSubPatchIndex)
{
   ASSERT(nSubPatchIndex<(UINT)m_quads.GetSize());
   if (m_bDirtyTangents)
      BuildTangents();
   return m_quads[nSubPatchIndex].m_vncvs;	
}

inline UINT Patch::GetNumTessPolys( BOOL bTessellateIfNeeded/*=TRUE*/ )
{
   if (bTessellateIfNeeded)
      Tessellate();
   return m_nNumTessPolys;
}

inline TessPoly *Patch::GetTessPolyAt( UINT nIndex )
{
   ASSERT(nIndex < m_nNumTessPolys);
   return &m_tesspolyarray[nIndex];
}

inline BYTE Patch::GetMaxSubdivision()
{
   return m_nMaxSubdivision;
}

inline void Patch::SetMaxSubdivision(BYTE nMaxSubdivision)
{
  m_nMaxSubdivision = nMaxSubdivision;
  DirtyTessellationAllocation();
}

inline void Patch::CalcPatchSubdivisions(UINT &nSubU,UINT &nSubV)
{
   nSubU = CalcUSubdivisions();
   nSubV = CalcVSubdivisions();

   if (!IsQuadPatch())
   {
      if (nSubU > nSubV)
         nSubV = nSubU-2;//ngons start off 1 less than everyone else
      else
         nSubU = nSubV-2;
   }
}

inline void Patch::SetTangentsDirty( BOOL bFlag )
{
   m_bDirtyTangents = bFlag;
   SetTessellationDirty( bFlag );

   for (PropertyPatch *pPropertyPatch = (PropertyPatch*)GetHeadPropertyPoly(); pPropertyPatch; pPropertyPatch = (PropertyPatch*)pPropertyPatch->GetNextPropertyPoly())
      pPropertyPatch->SetTangentsDirty(bFlag);
}

inline NPatchData *Patch::GetNPatchData()
{
   return m_pNPatchData;
}

inline PatchEdge *Patch::GetEdgeAt( UINT nIndex )
{
   return (PatchEdge *)Poly::GetEdgeAt( nIndex );
}

inline UINT Patch::GetNextSubPatchIndex( UINT nIndex ) const
{
   UINT nNumSubPatches = GetNumSubPatches();
   if (nNumSubPatches==1)
      return 1;
   nIndex++;
   if (nIndex >= nNumSubPatches)
      nIndex = 0;
   return nIndex;
}

inline UINT Patch::GetPrevSubPatchIndex( UINT nIndex ) const
{
   UINT nNumSubPatches = GetNumSubPatches();
   if (nNumSubPatches==1)
      return 1;
   if (nIndex == 0)
      nIndex = nNumSubPatches;
   nIndex--;
   return nIndex;
}


