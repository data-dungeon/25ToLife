#include "stdafx.h"
#include "HColorVertexSet.h"
#include "ColorVertexSet.h"

HColorVertex *HColorVertexSet::AddColorVertex()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HColorVertex*)((ColorVertexSet *)this)->AddColorVertex();
}

HColorVertex *HColorVertexSet::AddColorVertex( const RGBAFloat &color )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HColorVertex*)((ColorVertexSet *)this)->AddColorVertex(color);
}

HColorPoly *HColorVertexSet::AddPoly( HPoly *pPoly )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HColorPoly *)((ColorVertexSet *)this)->AddPoly( (Poly*)pPoly );
}
