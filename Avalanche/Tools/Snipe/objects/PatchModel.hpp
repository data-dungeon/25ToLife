inline UINT PatchModel::GetNumPatches()
{
   return m_polylist.GetSize();
}

inline Patch *PatchModel::AddPatch()
{
   return (Patch*)AddPoly();
}

inline Patch *PatchModel::AddPatch(UINT nID)
{
   return (Patch*)AddPoly(nID);
}

inline void PatchModel::RemovePatch( Patch *pPatch )
{
   RemovePoly( pPatch );
}

// tessellation access functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline PatchVertex *PatchModel::AddTessellatedVertex()
{
   return PatchVertex::Create();
}

inline PatchVertex *PatchModel::AddTessellatedVertex( const Vector &pos )
{
   PatchVertex *pVertex = AddTessellatedVertex();
   pVertex->SetPos(pos);
   return pVertex;
}

inline PatchVertex *PatchModel::AddTessellatedVertices( UINT nSize )
{
   return PatchVertex::CreateArray(nSize);
}


inline VertexNormal *PatchModel::AddTessellatedVertexNormal()
{
   return VertexNormal::Create();
}
/*
inline VertexNormal *PatchModel::AddTessellatedVertexNormal( const Vector &dir )
{
   VertexNormal *pVertexNormal = AddTessellatedVertexNormal();
   pVertexNormal->SetDir(dir);
   return pVertexNormal;
}
*/

inline VertexNormal *PatchModel::AddTessellatedVertexNormals( UINT nSize )
{
   return VertexNormal::CreateArray(nSize);
}