#pragma once

#include "HPolyModel.h"

class HIntProperty;
class HPatch;
class HPatchEdge;

class SNIPEEXPORT HPatchModel : public HPolyModel
{
public:
   enum BuildFlags
   {
      BuildFlags_FaceNormals =         1 << 0,
      BuildFlags_Edges =               1 << 1,
      BuildFlags_VertexNormals =       1 << 2,
      BuildFlags_Tessellation =        1 << 3,
   };

   // Edges
   HPatchEdge *GetNextEdge( POSITION &pos );

   // Faces
   UINT GetNumPatches();
   HPatch *GetNextFace( POSITION &pos );

   HIntProperty *GetPatchSubdivisions();
};