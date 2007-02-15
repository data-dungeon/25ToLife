#pragma once
#include "TransformableObject.h"
#include "GeometricObjectSurface.h"
#include "Surface.h"

class BoundingBox;
class CGLModelView;

class GeometricObjectInformation
{
public:
   GeometricObjectInformation()
   {
      m_nNumSurfaceVertices = m_nNumSurfaceEdges = m_nNumSurfaces = m_nNumSurfaceQuads = m_nNumSurfaceNGons = 0;
   }

   UINT m_nNumSurfaceVertices;
   UINT m_nNumSurfaceEdges;
   UINT m_nNumSurfaces;
   UINT m_nNumSurfaceQuads;
   UINT m_nNumSurfaceNGons;

public:
   virtual void DrawInfo(CGLModelView *pView);
};

class GeometricObject : public TransformableObject
{
protected:
   SurfaceContainer              *m_pSurfaceContainer;

   // display arrays
   CArray<GeometricObjectSurface> m_surfacearray;
   CArray<RTVertex>               m_vertexarray;
   CArray<UINT>                   m_controlpointarray;

   CArray<RTEdge>                 m_hardedgearray;
   CArray<RTEdge>                 m_softedgearray;
   CArray<RTEdge>                 m_borderedgearray;
   CArray<RTEdge>                 m_splineedgearray;
   CArray<RTEdge>                 m_hiddenedgearray;

   //cached info
   UINT                           m_nNumQuads;
   UINT                           m_nNumNGons;
   GLuint                         m_glCallList;

   //temporary build maps
   CMap<RTEdge, const RTEdge &, UINT, UINT > m_softedgemap;
   CMap<RTEdge, const RTEdge &, UINT, UINT > m_hardedgemap;
   CMap<RTEdge, const RTEdge &, UINT, UINT > m_borderedgemap;
   CMap<RTVertex, const RTVertex &, UINT, UINT > m_vertexmap;
   CMap<GeometricObjectSurface, const GeometricObjectSurface &, UINT, UINT > m_surfacemap;
   CMap<Vector, const Vector &, UINT, UINT> m_vertexindexmap;
   CMap<RTEdge, const RTEdge &, UINT, UINT > m_hiddenedgemap;

   union {
      ULONG m_nGeometricObjectFlags;
      struct {
         ULONG m_bGeometryDirty      : 1,//everything needs to be rebuilt
               m_bGeometryCacheDirty : 1;//only cache needs rebuilding
      };
   };

public:
   DECLARE_SNIPEOBJECT_PURE( GeometricObject, TransformableObject )

            GeometricObject();
   virtual ~GeometricObject();

   void Empty();

   GeometricObject *GetCache() { return (GeometricObject *)TransformableObject::GetCache(); } // Note: returns this if it is the cache
   GeometricObject *GetRootCache() { return (GeometricObject *)TransformableObject::GetRootCache(); } // Note: returns this if it is the cache
   virtual BOOL IsGeometry() { return TRUE; }
   void SetGeometryDirtyFlag(BOOL bFlag) { m_bGeometryDirty = bFlag; }
   void SetGeometryCacheDirtyFlag(BOOL bFlag) { m_bGeometryCacheDirty = bFlag; }

// draw
   void DrawSurfaces();
   void DrawControlPoints();

// Operations
   // Surface
   SurfaceContainer *CreateSurfaceContainer();
   SurfaceContainer *GetSurfaceContainer() { return m_pSurfaceContainer; }
   Surface *AddSurface( const char *name=NULL, int nIndex=-1 );
   void RemoveSurface( Surface *pSurface );

// Overridables
   virt_base virtual BoundingBox CalculateBoundingBox(CoordinateSpace nSpace, CGLModelView *pView=NULL);
   virt_base virtual UINT GetSurfaceType() { return SOT_GeometricObjectSurface; }
   virt_base virtual GeometricObjectInformation *GetDisplayInfo(CGLModelView *pModelView,GeometricObjectInformation *pInfo=NULL);
   virt_base virtual BOOL RebuildGeometry( CGLBaseView *pBaseView ) { return TRUE; }
   virt_base virtual BOOL RebuildSurface() { return TRUE; }
   virt_base virtual BOOL RebuildControlPoints() { return TRUE; }
   virt_base virtual BOOL DrawPick(CGLBaseView *pBaseView) { return TRUE; }
   virt_base virtual void DrawImmediateMode( CGLModelView *pModelView );
   virt_base virtual void DrawRetainedMode( CGLModelView *pModelView );

// InstanceObject overrides
   virtual void Draw( CGLBaseView *pView );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};





										
