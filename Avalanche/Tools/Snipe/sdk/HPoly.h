#pragma once

#include "HEdge.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HVertex;
class HVertexNormal;
class HPatch;
class HTexCoords;
class HSurface;
class HPropertyPoly;
class HUVPoly;

class SNIPEEXPORT HDeleteFaceInfo
{
public:
   void Delete();
   void UnDelete();

   void operator delete (void *pVoid);
};

class SNIPEEXPORT HPoly : public HComponent
{
public:
   UINT GetNumSides() const;
   void SetNumSides( UINT nNumSides );

   UINT GetNextIndex( UINT nIndex ) const;
   UINT GetPrevIndex( UINT nIndex ) const;

   void SetVertexAt( UINT nIndex, HVertex *pVertex );
   HVertex *GetVertexAt( UINT nIndex ) const;

   void SetVertexNormalAt( UINT nIndex, HVertexNormal *pVertexNormal );
   HVertexNormal *GetVertexNormalAt( UINT nIndex ) const;

   HEdge *GetEdgeAt( UINT nIndex );
   void SetEdgeAt( UINT nIndex, HEdge *pEdge );

   BOOL IsInFace( HVertex *pVertex );

   HEdge::HDeleteInfo *SplitPoly( HVertex *pV0, HVertex *pV1, HEdge *pPrevEdge=HEDGE_FINDCONTINUOUS, HEdge *pNextEdge=HEDGE_FINDCONTINUOUS );

   int FindVertexFaceRelativeIndex(HVertex *pVertex);

   Vector *GetFaceNormal();
   void CalcCenter(Vector &pos);

   HEdge *FindEdgeClosestToPoint( const Vector &point );

   HPropertyPoly *GetHeadPropertyPoly();

   HUVPoly *FindUVPoly( HTexCoords *pTexCoords );

   HEdge *FindNearestEdge( HCGLModelView *pView, const CPoint &pt );
   HDeleteFaceInfo *DeleteFace();

   void FlipNormal();

   // Surfaces
   UINT GetNumSurfaces() const;
   HSurface *GetSurfaceAt( UINT nIndex ) const;

   HPolyModel *GetModel();
};

//#include "HPoly.h"