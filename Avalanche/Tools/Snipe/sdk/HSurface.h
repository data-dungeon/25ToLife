#pragma once

#include "HContainerObject.h"

class HDecalMapSurfaceDriver;
class HImage;
class HTexCoords;
class HProperty;
class HFloatProperty;
class HColorProperty;
class HVector2Property;
class HUVPoly;
class HPoly;
class HSurfaceDriver;

class SNIPEEXPORT HSurface : public HHierObject
{
public:
   UINT GetNumFaces() const;
   HPoly *GetFaceAt( UINT nIndex ) const;
   void AddFace( HPoly *pFace );
   BOOL RemoveFace( HPoly *pFace );
   void RemoveAllFaces();
   UINT FindFace( HPoly *pFace ) const;

   UINT GetNumSurfaceDrivers();
   int AddSurfaceDriver( HSurfaceDriver *pSurfaceDriver, HProperty *pProperty );
   HSurfaceDriver *GetSurfaceDriverAt( UINT nIndex );
   HSurfaceDriver *GetSurfaceDriver( HProperty *pProperty );

   HColorProperty *GetDiffuseProperty();
   HColorProperty *GetAmbientProperty();
   HColorProperty *GetSpecularProperty();
   HColorProperty *GetEmissionProperty();
   HFloatProperty *GetTransparencyProperty();
   HFloatProperty *GetShininessProperty();
   HVector2Property *GetNormalOffsetProperty();
};

class SNIPEEXPORT HSurfaceContainer : public HContainerObject
{
public:
   UINT GetNumSurfaces();
   HSurface *GetSurfaceAt( UINT nIndex );
   void RemoveSurface( HSurface *pSurface );
   HSurface *FindSurface( const String &strName );
};
