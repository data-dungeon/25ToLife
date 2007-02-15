#include "stdafx.h"
#include "HPatchModel.h"
#include "PatchModel.h"

HPatchEdge *HPatchModel::GetNextEdge( POSITION &pos )
{
   return (HPatchEdge *)((PatchModel *)this)->GetNextEdge( pos );
}

UINT HPatchModel::GetNumPatches()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((PatchModel *)this)->GetNumPatches();
}

HPatch *HPatchModel::GetNextFace( POSITION &pos )
{
   return (HPatch *)((PatchModel *)this)->GetNextFace( pos );
}

HIntProperty *HPatchModel::GetPatchSubdivisions()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HIntProperty *) &((PatchModel *)this)->m_nPatchSubdivisions;
}
