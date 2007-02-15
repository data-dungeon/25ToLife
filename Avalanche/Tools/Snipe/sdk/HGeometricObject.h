#pragma once

#include "HTransformableObject.h"

class HSurfaceContainer;
class HSurface;
class HCGLBaseView;

class SNIPEEXPORT HGeometricObject : public HTransformableObject
{
public:
   HSurfaceContainer *GetSurfaceContainer();
   HSurface *AddSurface( const char *name = NULL, int nIndex=-1 ); // -1 = last
   void RemoveSurface(HSurface *pSurface);

   void SetGeometryDirtyFlag(BOOL bFlag);
   void SetGeometryCacheDirtyFlag(BOOL bFlag);
   void RebuildGeometry( HCGLBaseView *pBaseView );
   void InvalidateDependantViews();
};
