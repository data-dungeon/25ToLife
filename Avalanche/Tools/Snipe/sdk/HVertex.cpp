#include "stdafx.h"
#include "HVertex.h"
#include "Vertex.h"
#include "PatchVertex.h"

//////////////////////////////////////////////////////////////

void HVertex::HAttachInfo::ReAttach()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Vertex::AttachInfo *)this)->ReAttach();
}

void HVertex::HAttachInfo::UnAttach()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Vertex::AttachInfo *)this)->UnAttach();
}

void HVertex::HAttachInfo::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   Vertex::AttachInfo *pAttachInfo = (Vertex::AttachInfo *) pVoid;
   delete pAttachInfo;
}

//////////////////////////////////////////////////////////////////////////////

void HVertex::HWeldInfo::ReWeld()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Vertex::WeldInfo *)this)->ReWeld();
}

void HVertex::HWeldInfo::UnWeld()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Vertex::WeldInfo *)this)->UnWeld();
}

void HVertex::HWeldInfo::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   Vertex::WeldInfo *pWeldInfo = (Vertex::WeldInfo *) pVoid;
   delete pWeldInfo;
}

//////////////////////////////////////////////////////////////////////////////

HVectorProperty *HVertex::GetPositionPoperty() const
{
   return (HVectorProperty *)&((Vertex *)this)->m_vPosition;
}

const Vector &HVertex::GetPos() const 
{ 
   return ((Vertex *)this)->GetPos();
}

void HVertex::SetPos( const Vector &pos)
{
   ((Vertex *)this)->SetPos(pos);
}

UINT HVertex::GetNumSurroundingEdges() const
{
   return ((Vertex*)this)->GetNumSurroundingEdges();
}

HEdge *HVertex::GetSurroundingEdgeAt( UINT nIndex ) const
{
   return (HEdge*)((Vertex*)this)->GetSurroundingEdgeAt(nIndex);
}

void HVertex::GetEdgePair(HPoly *pPoly, HEdge **pEdge0, HEdge **pEdge1, UINT *nIndex0/*=NULL*/, UINT *nIndex1/*=NULL*/)
{
   ((Vertex *)this)->GetEdgePair((Poly *)pPoly, (Edge **)pEdge0, (Edge **)pEdge1, nIndex0, nIndex1);
}

HEdge *HVertex::GetPrevEdgeClockwise(HEdge *pEdge)
{
   return (HEdge *) ((Vertex *)this)->GetPrevEdgeClockwise((Edge *) pEdge );
}

HEdge *HVertex::GetNextEdgeClockwise(HEdge *pEdge)
{
   return (HEdge *) ((Vertex *)this)->GetNextEdgeClockwise((Edge *) pEdge );
}

HPoly *HVertex::GetPrevFaceClockwise(HPoly *pFace)
{
   return (HPoly *) ((Vertex *)this)->GetPrevFaceClockwise((Poly *) pFace );
}

HPoly *HVertex::GetNextFaceClockwise(HPoly *pFace)
{
   return (HPoly *) ((Vertex *)this)->GetNextFaceClockwise((Poly *) pFace );
}

UINT HVertex::GetNumSurroundingPolys() const
{
   return ((Vertex*)this)->GetNumSurroundingPolys();
}

HPoly *HVertex::GetSurroundingPolyAt( UINT nIndex ) const
{
   return (HPoly*)((Vertex*)this)->GetSurroundingPolyAt(nIndex);
}

UINT HVertex::FindSurroundingPoly( HPoly *pPoly ) const
{
   return ((Vertex*)this)->FindSurroundingPoly( (Poly *)pPoly );
}

HEdge *HVertex::FindNearestEdge( HCGLModelView *pView, const CPoint &pt )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HEdge *)((Vertex*)this)->FindNearestEdge( (CGLModelView *)pView, pt );
}

HPoly *HVertex::FindNearestFace( HCGLModelView *pView, const CPoint &pt )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPoly *)((Vertex*)this)->FindNearestFace( (CGLModelView *)pView, pt );
}

HVertex::HAttachInfo *HVertex::AttachTo( HVertex *pAttachToVertex, String *strError/*=NULL*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex::HAttachInfo *) ((Vertex *)this)->AttachTo( (Vertex *)pAttachToVertex, strError );
}

HVertex::HWeldInfo *HVertex::WeldTo( HVertex *pWeldToVertex, String *strError/*=NULL*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertex::HWeldInfo *) ((Vertex *)this)->WeldTo( (Vertex *)pWeldToVertex, strError );
}

UINT HVertex::FindAndCreateFaces( HDeleteFaceInfo **&pOutInfos, UINT nMaxSides/*=4*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Vertex *)this)->FindAndCreateFaces( (DeleteFaceInfo **&)pOutInfos, nMaxSides );
}

HPolyModel *HVertex::GetModel()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPolyModel*)((Vertex *)this)->GetOwner();
}

HVertexPeakInfo *HVertex::Peak( HEdge *pEdge, BOOL bPeak )
{
   return (HVertexPeakInfo *) ((Vertex *)this)->Peak( (Edge *)pEdge, bPeak );
}

BOOL HVertex::IsPeaked( HEdge *pEdge )
{
   return ((Vertex *)this)->IsPeaked( (Edge *)pEdge);
}

/////////////////////////////////////////////////////////////////////

void HVertexPeakInfo::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   delete (VertexPeakInfo *)pVoid;
}

UINT HVertexPeakInfo::GetNumChangedVertexNormals() const 
{ 
   return ((VertexPeakInfo *)this)->GetNumChangedVertexNormals();
}

BOOL HVertexPeakInfo::Peak()
{
   return ((VertexPeakInfo *)this)->Peak();
}

BOOL HVertexPeakInfo::Smooth()
{
   return ((VertexPeakInfo *)this)->Smooth();
}


