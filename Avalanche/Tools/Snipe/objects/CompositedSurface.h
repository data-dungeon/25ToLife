//

#include "../Utility/RGBColor.h"
#include "../Utility/Vector2.h"
#include "Properties/Property.h"

class Surface;

class SurfaceArray
{
public:
   Surface **m_pSurfaces; // array of surface *'s that were composited to make this
   UINT      m_nNumSurfaces;
   BOOL      m_bAutoDelete;

   SurfaceArray()
   {
      m_pSurfaces = NULL;
      m_nNumSurfaces = 0;
      m_bAutoDelete = TRUE;
   }
   SurfaceArray( Surface **pSurfaces, UINT nCount )
   {
      m_pSurfaces = pSurfaces;
      m_nNumSurfaces = nCount;
      m_bAutoDelete = FALSE;
   }
   ~SurfaceArray()
   {
      Empty();
   }

   void Empty()
   {
      m_nNumSurfaces = 0;
      if (m_pSurfaces)
      {
         if (m_bAutoDelete)
            delete [] m_pSurfaces;
         m_pSurfaces = NULL;
      }
   }

   BOOL operator == ( const SurfaceArray &other ) const
   {
      if (m_nNumSurfaces != other.m_nNumSurfaces)
         return FALSE;

      for (UINT i=0; i<m_nNumSurfaces; i++)
      {
         if (m_pSurfaces[i] != other.m_pSurfaces[i])
            return FALSE;
      }

      return TRUE;
   }
   void operator = ( const SurfaceArray &other )
   {
      Empty();

      m_nNumSurfaces = other.m_nNumSurfaces;
      m_pSurfaces = SNEW Surface *[m_nNumSurfaces];

      for (UINT i=0; i<m_nNumSurfaces; i++)
         m_pSurfaces[i] = other.m_pSurfaces[i];
   }
};

class SurfaceDriver;

template< typename TYPE >
class SurfaceProperty
{
public:
   TYPE            m_value;
   SurfaceDriver **m_pSurfaceDrivers;
   ULONG           m_nNumSurfaceDrivers;

   SurfaceProperty()
   {
      m_pSurfaceDrivers = NULL;
      m_nNumSurfaceDrivers = 0;
   }
   ~SurfaceProperty()
   {
      Empty();
   }

   void Empty()
   {
      if (m_pSurfaceDrivers)
      {
         delete [] m_pSurfaceDrivers;
         m_pSurfaceDrivers = NULL;
      }
      m_nNumSurfaceDrivers = 0;
   }
   void AddSurfaceDrivers( const Property &other )
   {
      UINT nNumSurfaceDriversAdded = other.GetNumPropertyDrivers( SOT_SurfaceDriver );

      if (nNumSurfaceDriversAdded)
      {
         UINT nPrevNumSurfaceDrivers = m_nNumSurfaceDrivers;
         m_nNumSurfaceDrivers += nNumSurfaceDriversAdded;

         SurfaceDriver **pSurfaceDrivers = SNEW SurfaceDriver *[m_nNumSurfaceDrivers];

         for (UINT i=0; i<nPrevNumSurfaceDrivers; i++)
            pSurfaceDrivers[i] = m_pSurfaceDrivers[i];
         for (UINT i=0; i<nNumSurfaceDriversAdded; i++)
            pSurfaceDrivers[nPrevNumSurfaceDrivers + i] = (SurfaceDriver *)other.GetPropertyDriverAt( i, SOT_SurfaceDriver );

         if (m_pSurfaceDrivers)
            delete [] m_pSurfaceDrivers;
         m_pSurfaceDrivers = pSurfaceDrivers;
      }
   }

   operator const TYPE &() const { return m_value; }
   const TYPE &GetValue() const { return m_value; }
   void operator = ( const TYPE &other )
   {
      m_value = other;
   }
   void operator = ( BasicProperty<TYPE> &other )
   {
      m_value = other;

      Empty();

      AddSurfaceDrivers( other );
   }
   void operator += ( const TYPE &other )
   {
      m_value += other;
   }
   void operator -= ( const TYPE &other )
   {
      m_value -= other;
   }
   TYPE operator * ( float fFactor )
   {
      return m_value * fFactor;
   }
   TYPE operator * ( const TYPE &other ) const
   {
      return m_value * other;
   }
};

class PolyModel;
class Poly;
class RTVertex;
class RTQuad;
class CGLModelView;

class CompositedSurface
{
public:
   PolyModel                *m_pModelCache;
   SurfaceArray              m_SurfaceArray; // array of surface *'s that were composited to make this
   CArray<Poly *>            m_facearray;    // the faces that use this composited surface
   SurfaceProperty<RGBFloat> m_cAmbientColor;
   SurfaceProperty<RGBFloat> m_cDiffuseColor;
   SurfaceProperty<RGBFloat> m_cSpecularColor;
   SurfaceProperty<RGBFloat> m_cEmissionColor;
   SurfaceProperty<float>    m_fTransparency;
   SurfaceProperty<float>    m_fShininess;
   SurfaceProperty<Vector2>  m_vNormalOffset;
   SurfaceProperty<int>      m_nCullFaceMode;

   RTQuad                   *m_pRTQuads;
   UINT                      m_nNumRTQuads;

   GLuint                    m_glBufferObjectID;

   ULONG                     m_bDirty       : 1;

   CompositedSurface( PolyModel *pModel )
   {
      m_pModelCache = pModel;
      m_bDirty = TRUE;
      m_pRTQuads = NULL;
      m_nNumRTQuads = 0L;
      m_glBufferObjectID = 0L;
   }
   ~CompositedSurface();

// Operations
   void SetDirty( BOOL bDirty=TRUE ) { m_bDirty = bDirty; }

   void Compute();
   void Bind();
   void UnBind();

   void FreeRTQuads();
   BOOL CreateRTQuads( BOOL bTessellated );

   // Surfaces
   UINT GetNumSurfaces() const { return m_SurfaceArray.m_nNumSurfaces; }
   Surface *GetSurfaceAt( UINT nIndex ) const
   {
      ASSERT(nIndex < m_SurfaceArray.m_nNumSurfaces);
      return m_SurfaceArray.m_pSurfaces[nIndex];
   }
   void SetFromSurface( Surface *pSurface );
   void ReplaceSurface( Surface *pSurface );
   void AddSurface( Surface *pSurface );
   void SubtractSurface( Surface *pSurface );
   void MultiplySurface( Surface *pSurface );

   // Faces
   UINT GetNumFaces() const { return m_facearray.GetCount(); }
   Poly *GetFaceAt( UINT nIndex ) const { return m_facearray.GetAt( nIndex ); }
   UINT AddFace( Poly *pFace );
   BOOL RemoveFace( Poly *pFace );
   UINT FindFace( Poly *pFace ) const;
   void DrawFacesImmediateBase( const Matrix44 &viewmatrix, BOOL bDrawTess, SurfaceDriver *pSurfaceDriver=NULL );
   void DrawFacesImmediate( const Matrix44 &viewmatrix, CGLModelView *pModelView, BOOL bDrawTess );
   void DrawFacesRetained( const Matrix44 &viewmatrix, CGLModelView *pModelView, BOOL bDrawTess );
};

