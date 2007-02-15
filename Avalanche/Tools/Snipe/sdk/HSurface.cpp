#include "stdafx.h"
#include "HSurface.h"
#include "Surface.h"
#include "..\Properties\Property.h"

UINT HSurface::GetNumSurfaceDrivers()
{
   return ((Surface *)this)->GetNumSurfaceDrivers();
}

int HSurface::AddSurfaceDriver( HSurfaceDriver *pSurfaceDriver, HProperty *pProperty )
{
   return ((Surface *)this)->AddSurfaceDriver( (SurfaceDriver *)pSurfaceDriver, (Property *)pProperty );
}

HSurfaceDriver *HSurface::GetSurfaceDriverAt( UINT nIndex )
{
   return (HSurfaceDriver *)((Surface *)this)->GetSurfaceDriverAt( nIndex );
}

HSurfaceDriver *HSurface::GetSurfaceDriver( HProperty *pProperty )
{
   return (HSurfaceDriver *)((Surface *)this)->GetSurfaceDriver( (Property *)pProperty );
}

HColorProperty *HSurface::GetDiffuseProperty()
{
   return (HColorProperty *)((Surface *)this)->GetDiffuseProperty();
}

HColorProperty *HSurface::GetAmbientProperty()
{
   return (HColorProperty *)((Surface *)this)->GetAmbientProperty();
}

HColorProperty *HSurface::GetSpecularProperty()
{
   return (HColorProperty *)((Surface *)this)->GetSpecularProperty();
}

HColorProperty *HSurface::GetEmissionProperty()
{
   return (HColorProperty *)((Surface *)this)->GetEmissionProperty();
}

HFloatProperty *HSurface::GetTransparencyProperty()
{
   return (HFloatProperty *)((Surface *)this)->GetTransparencyProperty();
}

HFloatProperty *HSurface::GetShininessProperty()
{
   return (HFloatProperty *)((Surface *)this)->GetShininessProperty();
}

HVector2Property *HSurface::GetNormalOffsetProperty()
{
   return (HVector2Property *)((Surface *)this)->GetNormalOffsetProperty();
}

UINT HSurface::GetNumFaces() const
{
   return ((Surface *)this)->GetNumFaces();
}

HPoly *HSurface::GetFaceAt( UINT nIndex ) const
{
   return (HPoly *)((Surface *)this)->GetFaceAt( nIndex );
}

void HSurface::AddFace( HPoly *pFace )
{
   ((Surface *)this)->AddFace( (Poly *)pFace );
}

BOOL HSurface::RemoveFace( HPoly *pFace )
{
   return ((Surface *)this)->RemoveFace( (Poly *)pFace );
}

UINT HSurface::FindFace( HPoly *pFace ) const
{
   return ((Surface *)this)->FindFace( (Poly *)pFace );
}

void HSurface::RemoveAllFaces()
{
   ((Surface *)this)->RemoveAllFaces();
}

/////////////////////////////////////////////////////////////////
// HSurfaceContainer

UINT HSurfaceContainer::GetNumSurfaces()
{
   return ((SurfaceContainer *)this)->GetNumSurfaces();
}

HSurface *HSurfaceContainer::GetSurfaceAt( UINT nIndex )
{
   return (HSurface *)((SurfaceContainer *)this)->GetSurfaceAt( nIndex );
}

void HSurfaceContainer::RemoveSurface( HSurface *pSurface )
{
   ((SurfaceContainer *)this)->RemoveSurface( (Surface *)pSurface );
}

HSurface *HSurfaceContainer::FindSurface( const String &strName )
{
   return (HSurface *)((SurfaceContainer *)this)->FindObject( strName );
}
