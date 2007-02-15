#include "stdafx.h"
#include "HEdge.h"
#include "Edge.h"

////////////////////////////////////////////////////////////////////////////

BOOL HEdge::IsBorderEdge() const
{
   return ((Edge*)this)->IsBorderEdge();
}

BOOL HEdge::IsDangle() const
{
   return ((Edge*)this)->IsDangle();
}

void HEdge::SetFirstVertex(HVertex *pVertex)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Edge*)this)->SetFirstVertex((Vertex*)pVertex);
}

void HEdge::SetSecondVertex(HVertex *pVertex)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Edge*)this)->SetSecondVertex((Vertex*)pVertex);
}

HVertex *HEdge::GetFirstVertex()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex*)((Edge*)this)->GetFirstVertex();
}

HVertex *HEdge::GetSecondVertex()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex*)((Edge*)this)->GetSecondVertex();
}

HPoly *HEdge::GetPoly()
{
   return (HPoly *)((Edge*)this)->GetPoly();
}

HPoly *HEdge::GetFirstPoly()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPoly *)((Edge*)this)->GetFirstPoly();
}

HPoly *HEdge::GetSecondPoly()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPoly *)((Edge*)this)->GetSecondPoly();
}

HVertex *HEdge::GetConnectedVertex( HVertex *pNotThis ) const
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex *) ((Edge *)this)->GetConnectedVertex( (Vertex *)pNotThis );
}

void HEdge::FindClosestIntersection( HCGLBaseView *pView, const CPoint &point, float &fOutPos )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Edge*)this)->FindClosestIntersection( (CGLBaseView *)pView, point, fOutPos );
}

BOOL HEdge::IsSoftEdge()
{
   return ((Edge *)this)->IsSoftEdge();
}

BOOL HEdge::IsInEdge( HVertex *pVertex )
{
   return ((Edge *)this)->IsInEdge( (Vertex *)pVertex );
}

BYTE HEdge::GetFaceRelativeIndex( HPoly *pPoly )
{
   return ((Edge *)this)->GetFaceRelativeIndex( (Poly *)pPoly );
}

HEdge *HEdge::FindEdge( const HVertex *pVertex0, const HVertex *pVertex1)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HEdge*)Edge::FindEdge((const Vertex *)pVertex0,( const Vertex *)pVertex1);
}

HEdge *HEdge::FindEdgeFromFaces(HVertex *pVertex0, HVertex *pVertex1)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HEdge*)Edge::FindEdgeFromFaces((Vertex*)pVertex0,(Vertex*)pVertex1);
}

HVertex *HEdge::GetSharedVertex( HEdge *pEdge ) const
{
   return (HVertex *) ((Edge *)this)->GetSharedVertex((Edge *)pEdge);
}

BOOL HEdge::IsVertexPeaked( HVertex *pVertex ) const
{
   return ((Edge *)this)->IsVertexPeaked( (Vertex *)pVertex );
}

HPoly *HEdge::GetConnectedPoly(HPoly *pPoly)
{
   return (HPoly *)((Edge *)this)->GetConnectedPoly((Poly*)pPoly);
}

BOOL HEdge::AddPoly(HPoly *pPoly,UINT nEdgeIndex)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Edge*)this)->AddPoly((Poly*)pPoly,nEdgeIndex);
}

HVertex::HDeleteInfo *HEdge::SplitEdge( float fPosition/*=0.5*/, HEdge **pOutEdge0/*=NULL*/, HEdge **pOutEdge1/*=NULL*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex::HDeleteInfo *) ((Edge *)this)->SplitEdge( fPosition, (Edge **)pOutEdge0, (Edge **)pOutEdge1 );
}

Vector HEdge::GetPosAlongEdge( float fPosition )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Edge *)this)->GetPosAlongEdge( fPosition );
}

HEdge *HEdge::GetContinuousEdge( HEdge *pPrev, BOOL bStartOtherDirectionIfNUll/*=FALSE*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HEdge *) ((Edge *)this)->GetContinuousEdge( (Edge *)pPrev, bStartOtherDirectionIfNUll );
}

HEdge *HEdge::GetContinuousEdge( HVertex *pVertex )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HEdge *) ((Edge *)this)->GetContinuousEdge( (Vertex *)pVertex );
}

HEdge *HEdge::GetOppositeEdge( HPoly *&pPoly, BOOL *bOutAligned/*=NULL*/ )
{
   return (HEdge *)((Edge *)this)->GetOppositeEdge( (Poly *&) pPoly, bOutAligned );
}

UINT HEdge::FindAndCreateFaces( HDeleteFaceInfo **&pOutInfos, UINT nMaxSides/*=4*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Edge *)this)->FindAndCreateFaces( (DeleteFaceInfo **&) pOutInfos, nMaxSides );
}

HPolyModel *HEdge::GetModel()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPolyModel*)((Edge *)this)->GetOwner();
}
