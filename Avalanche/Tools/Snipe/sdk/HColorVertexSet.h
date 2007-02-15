#pragma once

#include "HPropertySet.h"

class HColorVertex;
class HColorPoly;

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class SNIPEEXPORT HColorVertexSet : public HPropertySet
{
public:
   // UV
   HColorVertex *AddColorVertex();
   HColorVertex *AddColorVertex( const RGBAFloat &color );

   // Poly
   HColorPoly *AddPoly( HPoly *pPoly );
};
