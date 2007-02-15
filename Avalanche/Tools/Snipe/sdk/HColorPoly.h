#pragma once

#include "HPropertyPoly.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HColorVertex;

class SNIPEEXPORT HColorPoly : public HPropertyPoly
{
public:
   HColorVertex *GetColorVertexAt( UINT nIndex );
   void SetColorVertexAt( UINT nIndex, HColorVertex *pColorVertex );
};
