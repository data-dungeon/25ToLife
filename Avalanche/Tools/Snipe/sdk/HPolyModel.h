#pragma once

#include "HSceneObject.h"
#include "HEdge.h"
#include "HSurface.h"
/*!
A class that is for polygonal models.  Including n-gons from 3
to as many vertices as you want.
*/

class HVertex;
class HVertexNormal;
class HPoly;
class HImage;
class HUV;
class HTexCoords;
class HUV;
class Vector;
class Vector2;
class HBoneContainer;
class HTexCoordsContainer;
class HSurfaceContainer;
class HColorVertexSet;

class SNIPEEXPORT HPolyModel : public HSceneObject
{
public:
   enum BuildFlags
   {
      BuildFlags_FaceNormals =         1 << 0,
      BuildFlags_Edges =               1 << 1,
      BuildFlags_VertexNormals =       1 << 2,
   };

   // Vertices
   UINT GetNumVertices();
   POSITION GetHeadVertexPosition();
   HVertex *GetNextVertex( POSITION &pos );
   HVertex *AddVertex();
   HVertex *AddVertex( UINT nID );
   HVertex *AddVertex( const Vector &pos, HVertex::HDeleteInfo **pOutVertexDeleteInfo=NULL, BOOL bMirrorIfInMirrorMode=FALSE );
   HVertex *FindVertex( UINT nID );

   // VertexNormals
   void BuildVertexNormals();
   UINT GetNumVertexNormals();
   POSITION GetHeadVertexNormalPosition();
   HVertexNormal *GetNextVertexNormal( POSITION &pos );
   HVertexNormal *AddVertexNormal();
   void SetVertexNormals(const CArray<UINT> &faceindicies,const CArray<Vector> &vertexnormals);

   // UVs
   UINT GetNumUVs();
   HUV *AddUV( const Vector2 &pos );

   // Edges
   void BuildEdges();
   HEdge *AddEdge();
   UINT GetNumEdges();
   POSITION GetHeadEdgePosition();
   HEdge *GetNextEdge( POSITION &pos );
   int AddEdges( HSnipeObjectList *pVertices, HEdge::HDeleteInfo **&pEdgeDeleteInfos, HEdge *pPrevEdge=HEDGE_TERMINATE, HEdge *pNextEdge=HEDGE_TERMINATE );
   HEdge::HDeleteInfo *AddEdge( HVertex *pV0, HVertex *pV1, HEdge *pPrevEdge=HEDGE_FINDCONTINUOUS, HEdge *pNextEdge=HEDGE_FINDCONTINUOUS );

   // Polygons
   UINT GetNumPolys();
   POSITION GetHeadFacePosition();
   HPoly *GetNextFace( POSITION &pos );
   HPoly *AddPoly();
   void RemovePoly(HPoly *pPoly);

   void BuildPolys();

   // Decals
   HTexCoords *AddTexCoords(const char *name = NULL);
   HTexCoordsContainer *GetTexCoordsContainer();

   // Vertex Color Sets
   HColorVertexSet *AddColorVertexSet(const char *name = NULL);

   // Bones
   HBoneContainer *GetBoneContainer();
   void AddBone( HTransformableObject *pBone, HTransformableObject *pParent );

   // Surfaces
   HSurfaceContainer *GetSurfaceContainer();
   HSurface *AddSurface( const char *name = NULL, int nIndex=-1, HSurface::HDeleteInfo **pDeleteInfo=NULL ); // -1 = last
   void RemoveSurface(HSurface *pSurface);
};


