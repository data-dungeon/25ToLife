#pragma once

#include "HComponent.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HUVPoly;

class SNIPEEXPORT HUV : public HComponent
{
public:
   const Vector2 &GetPos() const;
   void SetPos( const Vector2 &pos );

   UINT GetNumSurroundingUVPolys() const;
   HUVPoly *GetSurroundingUVPolyAt( UINT nIndex ) const;
};