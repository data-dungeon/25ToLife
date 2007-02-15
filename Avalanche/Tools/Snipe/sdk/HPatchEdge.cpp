#include "stdafx.h"
#include "HPatchEdge.h"
#include "PatchEdge.h"

HFloatProperty *HVertexHandle::GetMagnitudeProperty()
{
   return (HFloatProperty *)&((VertexHandle *)this)->m_fMagnitude;
}

HFloatProperty *HVertexHandle::GetAlphaProperty()
{
   return (HFloatProperty *)&((VertexHandle *)this)->m_fAlpha;
}

HFloatProperty *HVertexHandle::GetGammaProperty()
{
   return (HFloatProperty *)&((VertexHandle *)this)->m_fGamma;
}

Vector HVertexHandle::GetTangent() const
{
   return ((VertexHandle *)this)->GetTangent();
}

///////////////////////////////////////////////////////////////////////////////////////

void HPatchEdge::HRedirectInfo::Undo()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((PatchEdge::RedirectInfo *)this)->Undo();
}

void HPatchEdge::HRedirectInfo::Redo()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((PatchEdge::RedirectInfo *)this)->Redo();
}

void HPatchEdge::HRedirectInfo::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   delete (PatchEdge::RedirectInfo *)pVoid;
}

///////////////////////////////////////////////////////////////////////////////////////

HPatchVertex *HPatchEdge::GetPrevContVertex() const
{
   return (HPatchVertex *)((PatchEdge *)this)->GetPrevContVertex();
}

HPatchVertex *HPatchEdge::GetNextContVertex() const
{
   return (HPatchVertex *)((PatchEdge *)this)->GetNextContVertex();
}

HPatchEdge *HPatchEdge::GetPrevContEdge( BOOL bLoop )
{
   return (HPatchEdge *)((PatchEdge *)this)->GetPrevContEdge( bLoop );
}

void HPatchEdge::SetPrevContEdge( HPatchEdge *pEdge )
{
   ((PatchEdge *)this)->SetPrevContEdge( (PatchEdge *)pEdge );
}

HPatchEdge *HPatchEdge::GetNextContEdge( BOOL bLoop )
{
   return (HPatchEdge *)((PatchEdge *)this)->GetNextContEdge( bLoop );
}

void HPatchEdge::SetNextContEdge( HPatchEdge *pEdge )
{
   ((PatchEdge *)this)->SetNextContEdge( (PatchEdge *)pEdge );
}

HVertexHandle *HPatchEdge::GetFirstVertexHandle( BOOL bBuildTangentIfDirty/*=TRUE*/ )
{
   return (HVertexHandle *)((PatchEdge *)this)->GetFirstVertexHandle( bBuildTangentIfDirty );
}

HVertexHandle *HPatchEdge::GetSecondVertexHandle( BOOL bBuildTangentIfDirty/*=TRUE*/ )
{
   return (HVertexHandle *)((PatchEdge *)this)->GetSecondVertexHandle( bBuildTangentIfDirty );
}

BOOL HPatchEdge::IsVertexConstrained( HVertex *pVertex ) const
{
   return ((PatchEdge *)this)->IsVertexConstrained( (Vertex *)pVertex );
}

HPatchEdge::HRedirectInfo *HPatchEdge::Redirect( HPatchVertex *pTermVertex, String *pOutError/*=NULL*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPatchEdge::HRedirectInfo *) ((PatchEdge *)this)->Redirect((PatchVertex *)pTermVertex, pOutError);
}

HPatchEdge::HRedirectInfo *HPatchEdge::Redirect( HPatchEdge *pToEdge, String *pOutError/*=NULL*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPatchEdge::HRedirectInfo *) ((PatchEdge *)this)->Redirect((PatchEdge *)pToEdge, pOutError);
}

HPatchModel *HPatchEdge::GetModel()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPatchModel*)((PatchEdge *)this)->GetOwner();
}