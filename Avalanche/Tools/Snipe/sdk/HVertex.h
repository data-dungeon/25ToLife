#pragma once

#include "HComponent.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

#define HEDGE_FINDCONTINUOUS (HEdge *)0x00000000
#define HEDGE_TERMINATE      (HEdge *)0x00000001

class HPoly;
class HEdge;
class HBezierKnot;
class HPatchEdge;
class HCGLModelView;
class HDeleteFaceInfo;
class HPolyModel;
class HVectorProperty;
class HVertexPeakInfo;

class SNIPEEXPORT HVertex : public HComponent
{
public:
   class SNIPEEXPORT HAttachInfo
   {
   public:
      void ReAttach();
      void UnAttach();

      void operator delete (void *pVoid);
   };

   class SNIPEEXPORT HWeldInfo
   {
   public:
      void ReWeld();
      void UnWeld();

      void operator delete (void *pVoid);
   };

   HVectorProperty *GetPositionPoperty() const;

   const Vector &GetPos() const;
   void SetPos( const Vector &pos);

   UINT GetNumSurroundingEdges() const;
   HEdge *GetSurroundingEdgeAt( UINT nIndex ) const;
   void GetEdgePair(HPoly *pPoly, HEdge **pEdge0, HEdge **pEdge1, UINT *nIndex0=NULL, UINT *nIndex1=NULL);

   UINT GetNumSurroundingPolys() const;
   HPoly *GetSurroundingPolyAt( UINT nIndex ) const;
   UINT FindSurroundingPoly( HPoly *pPoly ) const;

   BOOL IsConstrained() const { return FALSE; } // Added this back so that i could compile, sorry Tony

   HAttachInfo *AttachTo( HVertex *pAttachToVertex, String *strError=NULL );
   HWeldInfo *WeldTo( HVertex *pToVertex, String *strError=NULL);
   UINT FindAndCreateFaces( HDeleteFaceInfo **&pOutInfos, UINT nMaxSides=4 );

   HVertexPeakInfo *Peak( HEdge *pEdge, BOOL bPeak );
   BOOL IsPeaked( HEdge *pEdge );

   HEdge *GetPrevEdgeClockwise(HEdge *pEdge);
   HEdge *GetNextEdgeClockwise(HEdge *pEdge);

   HPoly *GetPrevFaceClockwise(HPoly *pPoly);
   HPoly *GetNextFaceClockwise(HPoly *pPoly);

   HEdge *FindNearestEdge( HCGLModelView *pView, const CPoint &pt );
   HPoly *FindNearestFace( HCGLModelView *pView, const CPoint &pt );

   static HVertex *GetNearestVertex(); // From Focus Object, not this object
   static BOOL SetNearestVertex( HVertex *pVertex, HVertex **pPrevVertex=NULL ); // From Focus Object, not this object

   HPolyModel *GetModel();
};

class SNIPEEXPORT HVertexPeakInfo
{
public:
   void operator delete (void *pVoid);

   UINT GetNumChangedVertexNormals() const;

   BOOL Peak();
   BOOL Smooth();
};
