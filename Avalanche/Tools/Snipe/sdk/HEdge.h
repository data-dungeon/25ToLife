#pragma once

#include "HVertex.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HVertexNormal;
class HPoly;
class HCGLBaseView;

class SNIPEEXPORT HEdge : public HComponent
{
public:
   class SNIPEEXPORT HWeldInfo
   {
   public:
      void operator delete (void *pVoid);

      void ReWeld();
      void UnWeld();
   };

   BOOL IsBorderEdge() const;
   BOOL IsDangle() const;

   void SetFirstVertex(HVertex *pVertex);
   void SetSecondVertex(HVertex *pVertex);
   HVertex *GetFirstVertex();
   HVertex *GetSecondVertex();

   HPoly *GetPoly();
   HPoly *GetFirstPoly();
   HPoly *GetSecondPoly();

   BOOL IsSoftEdge();
   BOOL IsInEdge( HVertex *pVertex );

   void SetFirstVertexNormal(HVertexNormal *pVertexNormal);
   void SetSecondVertexNormal(HVertexNormal *pVertexNormal);
   HVertexNormal *GetFirstVertexNormal();
   HVertexNormal *GetSecondVertexNormal();

   HVertex *GetConnectedVertex( HVertex *pNotThis ) const;
   HVertex *GetSharedVertex( HEdge *pEdge ) const;

   BOOL IsVertexPeaked( HVertex *pVertex ) const;

   void FindClosestIntersection( HCGLBaseView *pView, const CPoint &point, float &fOutPos );

   BYTE GetFaceRelativeIndex( HPoly *pPoly );
   static HEdge *FindEdge( const HVertex *pVertex0, const HVertex *pVertex1 );
   static HEdge *FindEdgeFromFaces( HVertex *pVertex0, HVertex *pVertex1 );
   HEdge *GetContinuousEdge( HEdge *pPrev, BOOL bStartOtherDirectionIfNUll=FALSE );
   HEdge *GetContinuousEdge( HVertex *pVertex );
   HEdge *GetOppositeEdge( HPoly *&pPoly, BOOL *bOutAligned=NULL ); // Whether the opposite edge is running V0 - V1 in the same direction

   Vector GetPosAlongEdge( float fPosition );
   HVertex::HDeleteInfo *SplitEdge( float fPosition=0.5f, HEdge **pOutEdge0=NULL, HEdge **pOutEdge1=NULL  );

   BOOL AddPoly(HPoly *pPoly,UINT nEdgeIndex);
   
   UINT FindAndCreateFaces( HDeleteFaceInfo **&pOutInfos, UINT nMaxSides=4 );
   HPoly *GetConnectedPoly(HPoly *pPoly);

   HPolyModel *GetModel();
};