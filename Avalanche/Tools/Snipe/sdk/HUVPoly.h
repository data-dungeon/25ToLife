#pragma once

#include "HPropertyPoly.h"

class HPoly;
class HUV;
class HTexCoords;

class SNIPEEXPORT HUVPoly : public HPropertyPoly
{
public:
   UINT GetNumSides();

   HUV *GetUVAt( UINT nIndex );
   void SetUVAt( UINT nIndex, HUV *pUV );
   int FindUVFaceRelativeIndex( HUV *pUV );

   HTexCoords *GetTexCoords();
};
