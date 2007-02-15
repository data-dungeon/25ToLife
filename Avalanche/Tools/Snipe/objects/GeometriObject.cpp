#include "stdafx.h"
#include "GeometricObject.h"
#include "../Utility/BoundingBox.h"
#include "Interface/MainInterface.h"
#include "Vertex.h"

IMPLEMENT_SNIPEOBJECT_PURE( GeometricObject )

GeometricObject::GeometricObject()
{
   m_nNumQuads = m_nNumNGons = 0;
   m_nGeometricObjectFlags = 0;
   m_glCallList = 0;
   m_pSurfaceContainer = NULL;
}

GeometricObject::~GeometricObject()
{
}

void GeometricObject::Empty()
{
   m_vertexarray.RemoveAll();
   m_hardedgearray.RemoveAll();
   m_softedgearray.RemoveAll();
   m_borderedgearray.RemoveAll();
   m_surfacearray.RemoveAll();
   m_controlpointarray.RemoveAll();
   m_splineedgearray.RemoveAll();
   m_hiddenedgearray.RemoveAll();

   m_nNumQuads = 0;
   m_nNumNGons = 0;
}

BoundingBox GeometricObject::CalculateBoundingBox(CoordinateSpace nSpace, CGLModelView *pView/*=NULL*/)
{
   return BoundingBox();
}

GeometricObjectInformation *GeometricObject::GetDisplayInfo(CGLModelView *pModelView,GeometricObjectInformation *pInfo/*=NULL*/)
{
   pInfo->m_nNumSurfaceVertices += m_vertexarray.GetSize();
   pInfo->m_nNumSurfaceEdges += m_hardedgearray.GetSize() + m_softedgearray.GetSize() + m_borderedgearray.GetSize();
   pInfo->m_nNumSurfaces += m_surfacearray.GetSize();
   pInfo->m_nNumSurfaceQuads += m_nNumQuads;
   pInfo->m_nNumSurfaceNGons += m_nNumNGons;

   return pInfo;
}

SurfaceContainer *GeometricObject::CreateSurfaceContainer()
{
   if (!m_pSurfaceContainer)
   {
      m_pSurfaceContainer = SurfaceContainer::Create();
      AddChildAtTail(m_pSurfaceContainer);
   }
   return m_pSurfaceContainer;
}

Surface *GeometricObject::AddSurface( const char *name/*=NULL*/, int nIndex/*=-1*/ )
{
   SurfaceContainer *pSurfaceContainer = CreateSurfaceContainer();
   Surface *pSurface = Surface::Create();
   pSurfaceContainer->InsertChildAt( nIndex, pSurface );
   if (name)
      pSurface->SetName(name);
   return pSurface;
}

void GeometricObject::RemoveSurface(Surface *pSurface)
{
   SurfaceContainer *pSurfaceContainer = GetSurfaceContainer();
   pSurfaceContainer->RemoveSurface(pSurface);
}

// SnipeObject overrides
BOOL GeometricObject::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYOBJ_ADDEDCHILD:
      {
         SnipeObject *pChild = (SnipeObject *)lParam;
         if (pChild->GetSOT() == SOT_SurfaceContainer)
            m_pSurfaceContainer = (SurfaceContainer *)pChild;
         break;
      }
      case NOTIFYOBJ_REMOVEDCHILD:
      {
         SnipeObject *pChild = (SnipeObject *)lParam;
         if (pChild->GetSOT() == SOT_SurfaceContainer)
            m_pSurfaceContainer = NULL;
         break;
      }
      case NOTIFYOBJ_REBUILDGEOMETRY:
         m_bGeometryDirty = TRUE;
         break;
   }

   return TransformableObject::OnObjectNotify( msg, wParam, lParam );
}


