#include "stdafx.h"
#include "HPatch.h"
#include "Patch.h"

HPatchEdge *HPatch::GetEdgeAt(UINT nIndex)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPatchEdge*)((Patch*)this)->GetEdgeAt(nIndex);
}

// misc
void HPatch::SetMaxSubdivision(BYTE nMaxSubdivision)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((Patch*)this)->SetMaxSubdivision(nMaxSubdivision);
}

BYTE HPatch::GetMaxSubdivision()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Patch*)this)->GetMaxSubdivision();
}

UINT HPatch::GetNumTessPolys()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Patch*)this)->GetNumTessPolys();
}

HTessPoly *HPatch::GetTessPolyAt(UINT nIndex)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HTessPoly*)((Patch*)this)->GetTessPolyAt(nIndex);
}

HPatchModel *HPatch::GetModel()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPatchModel*)((Poly *)this)->GetOwner();
}