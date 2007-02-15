#pragma once

#include "HPropertySet.h"
#include "HContainerObject.h"


#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HUV;
class HUVPoly;
class HPoly;

class SNIPEEXPORT HTexCoords : public HPropertySet
{
public:
   // UV
   UINT GetNumUVs();
   HUV *AddUV();
   HUV *AddUV( const Vector2 &pos );
   HUV *GetFirstUV();
   void RemoveUV(HUV *pUV);

   // Poly
   HUVPoly *AddPoly( HPoly *pPoly );
   void RemovePoly( HUVPoly *pUVPoly );

   // UVPoly
   UINT GetNumUVPolys();
};

class SNIPEEXPORT HTexCoordsContainer : public HContainerObject
{
public:
   UINT GetNumTexCoords() const;
   HTexCoords *GetTexCoordsAt( UINT nIndex );
};