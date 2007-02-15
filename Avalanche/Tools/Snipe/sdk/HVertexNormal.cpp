#include "stdafx.h"
#include "HVertexNormal.h"
#include "VertexNormal.h"

void HVertexNormal::SetDir( const Vector &direction )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((VertexNormal *)this)->SetDir( direction );
}

const Vector &HVertexNormal::GetDir() const
{
   return ((VertexNormal *)this)->GetDir();
}

HPolyModel *HVertexNormal::GetModel()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPolyModel*)((VertexNormal *)this)->GetOwner();
}
