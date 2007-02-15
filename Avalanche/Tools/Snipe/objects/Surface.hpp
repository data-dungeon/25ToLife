// Surface ///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline UINT Surface::GetNumSurfaceDrivers()
{
   return GetNumChildren();
}

inline SurfaceDriver *Surface::GetSurfaceDriverAt(UINT nIndex)
{
   return (SurfaceDriver *)GetChildAt(nIndex);
}

inline UINT Surface::GetNumFaces() const
{
   return m_facearray.GetCount();
}

inline Poly *Surface::GetFaceAt( UINT nIndex ) const
{
   return m_facearray.GetAt( nIndex );
}

inline UINT Surface::FindFace( Poly *pFace ) const
{
   for (UINT i=0; i<GetNumFaces(); i++)
      if (GetFaceAt(i) == pFace)
         return i;
   return -1;
}

// SurfaceContainer /////////////////////////////////////////////////////////////////////////////////////////////////////

inline UINT SurfaceContainer::GetNumSurfaces()
{
   return GetNumChildren();
}

inline Surface *SurfaceContainer::GetSurfaceAt( UINT nIndex )
{
   return (Surface*)GetChildAt( nIndex );
}

inline void SurfaceContainer::RemoveSurface( Surface *pSurface )
{
   pSurface->DestroyChildren();
   RemoveChild( (HierObject*)pSurface );
}

