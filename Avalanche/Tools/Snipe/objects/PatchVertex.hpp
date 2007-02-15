
// Secondary Neighbors ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline UINT PatchVertex::GetNumSecondarySurroundingPolys() const
{
   return m_nNumSecondarySurroundingFaces;
}

inline TessPoly *PatchVertex::GetSecondarySurroundingPolyAt( UINT nIndex ) const
{
   return m_pSecondarySurroundingPolys[nIndex];
}

inline void PatchVertex::RemoveSecondarySurroundingPoly( TessPoly *pPoly )
{
   for (UINT i = 0; i < GetNumSecondarySurroundingPolys(); i++)
   {
      if (GetSecondarySurroundingPolyAt(i)==pPoly)
      {
         RemoveSecondarySurroundingPolyAt(i);
         return;
      }
   }
}
