#include "stdafx.h"
//#include "UVBezierSplineEdge.h"
//#include "BezierSplineEdge.h"
//
//IMPLEMENT_COMPONENT_CREATE( UVBezierSplineEdge, UVPatchEdge )
//
//void UVBezierSplineEdge::BuildUVTangents()
//{
//#if 0
//   UVPatchEdge::BuildUVTangents();
//#else
//   if (!m_bUVTangentsDirty)
//      return;
//
//   m_bUVTangentsDirty = FALSE;
//
//   BezierSplineEdge *pEdge = (BezierSplineEdge*)GetEdge();
//
//   BezierKnot *pFirstKnot = (BezierKnot*)pEdge->GetFirstVertex();
//   BezierKnot *pSecondKnot = (BezierKnot*)pEdge->GetSecondVertex();
//   
//   TexCoords *pTexCoords = GetTexCoords();
//
//   GetFirstUVHandle()->SetTangent(GetFirstUV()->CalcUVTangent( pFirstKnot, pTexCoords, FALSE ) /3.0f);
//   GetSecondUVHandle()->SetTangent((GetSecondUV()->CalcUVTangent( pSecondKnot, pTexCoords, TRUE )/3.0f).Negate());
//#endif
//}
