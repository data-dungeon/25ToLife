#include "stdafx.h"
#include "HGeometricObject.h"
#include "GeometricObject.h"
#include "GUI\GLBaseView.h"

HSurfaceContainer *HGeometricObject::GetSurfaceContainer()
{
   return (HSurfaceContainer *)((GeometricObject *)this)->GetSurfaceContainer();
}

HSurface *HGeometricObject::AddSurface( const char *name/*=NULL*/, int nIndex/*=-1*/ )
{
   return (HSurface*)((GeometricObject *)this)->AddSurface( name, nIndex );
}

void HGeometricObject::RemoveSurface(HSurface *pSurface)
{
   ((GeometricObject*)this)->RemoveSurface((Surface*)pSurface);
}

void HGeometricObject::SetGeometryDirtyFlag(BOOL bFlag)
{
   ((GeometricObject*)this)->SetGeometryDirtyFlag(bFlag);
}

void HGeometricObject::SetGeometryCacheDirtyFlag(BOOL bFlag)
{
   ((GeometricObject*)this)->SetGeometryCacheDirtyFlag(bFlag);
}

void HGeometricObject::RebuildGeometry(HCGLBaseView *pBaseView)
{
   ((GeometricObject*)this)->RebuildGeometry((CGLBaseView*)pBaseView);
}

void HGeometricObject::InvalidateDependantViews()
{
   ((GeometricObject*)this)->InvalidateDependantViews();
}