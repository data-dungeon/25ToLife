#pragma once

#include "RTVertex.h"
#include "Surface.h"
#include "Properties/MapSurfaceDriver.h"

class GeometricObjectSurface
{
public:
   CArray<RTQuad> m_quadarray;
   CArray<RTPoly> m_polyarray;
   MapSurfaceDriver *m_pSurfaceDriver;
   Surface       *m_pSurface;
   int            m_nCullFaceMode; enum { CFM_NONE, CFM_FRONT, CFM_BACK };

   UINT           m_nNumQuads;
   UINT           m_nNumNGons;

   GeometricObjectSurface()
   {
      m_pSurfaceDriver = NULL;
      m_pSurface = NULL;
      m_nCullFaceMode = CFM_NONE;
      m_nNumQuads = 0;
      m_nNumNGons = 0;
   }
   virtual ~GeometricObjectSurface() {}

   GeometricObjectSurface(const GeometricObjectSurface &other);

   BOOL operator == ( const GeometricObjectSurface &other ) const;

   // Operations
   void Empty()
   {
      m_quadarray.RemoveAll();
      m_polyarray.RemoveAll();
      m_pSurfaceDriver = NULL;
      m_pSurface = NULL;
   }

   void Bind();
   void UnBind();

   GeometricObjectSurface &operator = (const GeometricObjectSurface &other);
};

inline void GeometricObjectSurface::Bind()
{
   glPushAttrib( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT );

   if (m_pSurface)
   {
      m_pSurface->Bind();
      if (m_pSurfaceDriver)
         m_pSurfaceDriver->Bind();
   }
   else
      glColor3f(0.6f,0.6f,0.6f);
}

inline void GeometricObjectSurface::UnBind()
{
   if (m_pSurface)
      m_pSurface->UnBind();

   if (m_pSurfaceDriver)
      m_pSurfaceDriver->UnBind();

   glPopAttrib();
}

inline BOOL GeometricObjectSurface::operator == ( const GeometricObjectSurface &other ) const
{
   if (m_pSurfaceDriver==other.m_pSurfaceDriver && m_pSurface==other.m_pSurface)
      return TRUE;
   return FALSE;
}

#ifdef __AFXTEMPL_H__
template<> AFX_INLINE UINT AFXAPI HashKey<const GeometricObjectSurface &>( const GeometricObjectSurface &key )
{
   return (DWORD)((DWORD_PTR)HashKey<const MapSurfaceDriver &>(*key.m_pSurfaceDriver));
}
#endif

inline GeometricObjectSurface::GeometricObjectSurface(const GeometricObjectSurface &other)
{
/* // these loops should never happen
   int i;


   m_polyarray.SetSize(other.m_polyarray.GetSize());
   for (i = 0; i < other.m_polyarray.GetSize(); i++)
      m_polyarray[i] = other.m_polyarray[i];

   m_quadarray.SetSize(other.m_quadarray.GetSize());
   for (i = 0; i < other.m_quadarray.GetSize(); i++)
      m_quadarray[i] = other.m_quadarray[i];
*/

   m_pSurfaceDriver = other.m_pSurfaceDriver;
   m_pSurface = other.m_pSurface;
   m_nCullFaceMode = other.m_nCullFaceMode;
}

inline GeometricObjectSurface &GeometricObjectSurface::operator = (const GeometricObjectSurface &other)
{
/* // these loops should never happen
   int i;

   m_polyarray.SetSize(other.m_polyarray.GetSize());
   for (i = 0; i < other.m_polyarray.GetSize(); i++)
      m_polyarray[i] = other.m_polyarray[i];

   m_quadarray.SetSize(other.m_quadarray.GetSize());
   for (i = 0; i < other.m_quadarray.GetSize(); i++)
      m_quadarray[i] = other.m_quadarray[i];
*/

   m_pSurfaceDriver = other.m_pSurfaceDriver;
   m_pSurface = other.m_pSurface;
   m_nCullFaceMode = other.m_nCullFaceMode;

   return *this;
}
