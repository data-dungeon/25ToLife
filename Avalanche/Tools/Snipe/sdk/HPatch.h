#pragma once

#include "HPoly.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HPoly;
class HEdge;
class HVertex;
class HVertexNormal;
class HUV;
class HPatchEdge;
class HTessPoly;
class HPatchModel;

class SNIPEEXPORT HPatch : public HPoly
{
public:
   HPatch *Next();
   HPatch *Previous();

   HPatchEdge *GetEdgeAt(UINT nIndex);

   // kept around temporarily, be warned!
   HPoly *GetOriginalPoly();

   // subdivisions
   void SetMaxSubdivision(BYTE nMaxSubdivision);
   BYTE GetMaxSubdivision();

   UINT GetNumTessPolys();
   HTessPoly *GetTessPolyAt(UINT nIndex);

   HPatchModel *GetModel();
};