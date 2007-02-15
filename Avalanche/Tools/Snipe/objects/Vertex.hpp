//
inline Vertex &Vertex::operator = ( Vertex &other )
{
   SetPos( other.GetPos());
   return *this;
}

inline BOOL Vertex::operator == ( Vertex &other )
{
   return GetPos() == other.GetPos();
}

inline BOOL Vertex::operator != ( Vertex &other ) 
{
   return !(*this == other);
}

template<> AFX_INLINE UINT AFXAPI HashKey<Vertex &>( Vertex &key )
{
   double x = (int)(key.GetPos().x * 1000.0f + SIGN(key.GetPos().x) * .5f)/1000.0*12.3;
   double y = (int)(key.GetPos().y * 1000.0f + SIGN(key.GetPos().y) * .5f)/1000.0*23.4;
   double z = (int)(key.GetPos().z * 1000.0f + SIGN(key.GetPos().z) * .5f)/1000.0*34.5;
   return (int) (pow(x*x + y*y + z*z, 0.5 ) * 9.87);  
}

// Vertex POLY NEIGHBORS
inline UINT Vertex::GetNumSurroundingPolys() const
{
   return m_nNumSurroundingFaces;
};

inline Poly *Vertex::GetSurroundingPolyAt( UINT nIndex ) const
{
   return m_pPrimarySurroundingPolys[nIndex];
}

inline UINT Vertex::FindSurroundingPoly( Poly *pPoly ) const
{
   for (UINT i = 0; i < GetNumSurroundingPolys(); i++)
   {
      if (GetSurroundingPolyAt(i) == pPoly)
         return i;
   }

   return -1;
}

inline void Vertex::RemoveSurroundingPoly( Poly *pPoly )
{
   UINT nIndex = FindSurroundingPoly( pPoly );
   if (nIndex != -1)
      RemoveSurroundingPolyAt( nIndex );
}

// Vertex Edge NEIGHBORS

inline UINT Vertex::GetNumSurroundingEdges() const
{
   return m_nNumSurroundingEdges;
};

inline Edge *Vertex::GetSurroundingEdgeAt( UINT nIndex ) const
{
   return m_pSurroundingEdges[nIndex];
}

inline void Vertex::RemoveSurroundingEdge(Edge *pEdge)
{
   for (UINT i = 0; i < GetNumSurroundingEdges(); i++)
   {
      if (GetSurroundingEdgeAt(i)==pEdge)
      {
         RemoveSurroundingEdgeAt(i);
         return;
      }
   }
}

// Vertex Bone Information

inline Bone *Vertex::GetBoneAt( UINT nIndex )
{
   return m_pBones[nIndex];
}

inline void Vertex::RemoveBone( Bone *pBone )
{
   ASSERT(pBone);
   for (UINT i=0; i < GetNumBones(); i++)
   {
      if (pBone == GetBoneAt(i))
         RemoveBoneAt(i);
   }
}