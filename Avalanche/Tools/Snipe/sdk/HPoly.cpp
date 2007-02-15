#include "stdafx.h"
#include "HPoly.h"
#include "Poly.h"
#include "CompositedSurface.h"
#include "CreateFace.h"

void HDeleteFaceInfo::Delete()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((DeleteFaceInfo *)this)->Delete();
}

void HDeleteFaceInfo::UnDelete()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((DeleteFaceInfo *)this)->UnDelete();
}

void HDeleteFaceInfo::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   DeleteFaceInfo *pInfo = (DeleteFaceInfo *) pVoid;
   delete pInfo;
}

///////////////////////////////////////////////////////////////////////////////////

UINT HPoly::GetNumSides() const
{
   return ((Poly *)this)->GetNumSides();
}

void HPoly::SetNumSides( UINT nNumSides )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Poly *)this)->SetNumSides(nNumSides);
}

UINT HPoly::GetNextIndex( UINT nIndex ) const
{
   return ((Poly *)this)->GetNextIndex(nIndex);
}

UINT HPoly::GetPrevIndex( UINT nIndex ) const
{
   return ((Poly *)this)->GetPrevIndex(nIndex);
}

HVertex *HPoly::GetVertexAt( UINT nIndex ) const
{
   return (HVertex *)((Poly *)this)->GetVertexAt( nIndex );
}

void HPoly::SetVertexAt( UINT nIndex, HVertex *pVertex )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Poly *)this)->SetVertexAt(nIndex,(Vertex*)pVertex);
}

HVertexNormal *HPoly::GetVertexNormalAt( UINT nIndex ) const
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HVertexNormal *)((Poly *)this)->GetVertexNormalAt( nIndex );
}

void HPoly::SetVertexNormalAt( UINT nIndex, HVertexNormal *pVertexNormal )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Poly *)this)->SetVertexNormalAt(nIndex,(VertexNormal*)pVertexNormal);
}

HEdge *HPoly::GetEdgeAt( UINT nIndex )
{
   return (HEdge *)((Poly *)this)->GetEdgeAt(nIndex);
}

void HPoly::SetEdgeAt( UINT nIndex, HEdge *pEdge )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Poly *)this)->SetEdgeAt(nIndex,(Edge*)pEdge);
}

BOOL HPoly::IsInFace( HVertex *pVertex )
{
   return ((Poly *)this)->IsInFace( (Vertex *)pVertex );
}

HEdge::HDeleteInfo *HPoly::SplitPoly( HVertex *pVertex0, HVertex *pVertex1, HEdge *pPrevEdge/*=HEDGE_FINDCONTINUOUS*/, HEdge *pNextEdge/*=HEDGE_FINDCONTINUOUS*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HEdge::HDeleteInfo *)((Poly *)this)->SplitPoly( (Vertex *)pVertex0, (Vertex *)pVertex1, (Edge *)pPrevEdge, (Edge *)pNextEdge );
}

int HPoly::FindVertexFaceRelativeIndex( HVertex *pVertex )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Poly *)this)->FindVertexFaceRelativeIndex( (Vertex*)pVertex );
}

Vector *HPoly::GetFaceNormal()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Poly *)this)->GetFaceNormal();
}

void HPoly::CalcCenter( Vector &pos )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Poly *)this)->CalcCenter( pos );
}

HEdge *HPoly::FindEdgeClosestToPoint( const Vector &point )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HEdge *)((Poly *)this)->FindEdgeClosestToPoint( point );
}

HPropertyPoly *HPoly::GetHeadPropertyPoly()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPropertyPoly *)((Poly*)this)->GetHeadPropertyPoly();
}

HUVPoly *HPoly::FindUVPoly( HTexCoords *pTexCoords )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HUVPoly *)((Poly *)this)->FindUVPoly( (TexCoords *)pTexCoords );
}

HEdge *HPoly::FindNearestEdge( HCGLModelView *pView, const CPoint &pt )
{
   return ( HEdge *) ((Poly *)this)->FindNearestEdge( (CGLModelView *) pView, pt );
}

void HPoly::FlipNormal()
{
   ((Poly*)this)->FlipNormal();
}

UINT HPoly::GetNumSurfaces() const
{
   return ((Poly*)this)->GetNumSurfaces();
}

HSurface *HPoly::GetSurfaceAt(UINT nIndex) const
{
   return (HSurface*)((Poly*)this)->GetSurfaceAt(nIndex);
}

HDeleteFaceInfo *HPoly::DeleteFace()
{
   return (HDeleteFaceInfo *)((Poly *)this)->DeleteFace();
}

HPolyModel *HPoly::GetModel()
{
   return (HPolyModel*)((Poly *)this)->GetOwner();
}