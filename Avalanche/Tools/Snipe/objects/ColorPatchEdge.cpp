#include "stdafx.h"
#include "ColorPatchEdge.h"
#include "ColorPatchVertex.h"
#include "PatchEdge.h"
#include "../Utility/MathTools.h"

IMPLEMENT_COMPONENT_CREATE( ColorPatchEdge, Edge )

void ColorPatchEdge::SetTessellation()
{
   PatchEdge *pPatchEdge = GetEdge();
   int nSub = pPatchEdge->CalcEdgeSubdivisions();

   int nNumSteps = 1 << nSub;
   if (nNumSteps==1)
      return;

   double inc = 1.0 / nNumSteps;

   Vector4 p0( GetFirstColorVertex()->GetPos() );
   Vector4 p1( GetFirstColorVertexHandle()->GetTangent() );
   Vector4 p2( -1 * GetSecondColorVertexHandle()->GetTangent() );
   Vector4 p3( GetSecondColorVertex()->GetPos() );

   Vector4 v;
   for (double t=inc, i=1; i<nNumSteps; t+=inc, i++)
   {
      v = (( 2*t*t*t)-(3*t*t)+1)*p0
         + ((-2*t*t*t)+(3*t*t)  )*p3
         + ((   t*t*t)-(2*t*t)+t)*p1
         + ((   t*t*t)-(  t*t)  )*p2;

      GetTessColorVertexAt( i, FALSE )->SetPos( Vector4(v.x,v.y,v.z,v.w) );
   }
}

void ColorPatchEdge::BuildTangents()
{
   ASSERT(IsTangentsDirty());

   m_bTangentsDirty = FALSE;

   ColorPatchEdge *pPrevContColorEdge = GetPrevContColorEdge(TRUE);
   ColorPatchEdge *pNextContColorEdge = GetNextContColorEdge(TRUE);

   ColorVertex *pPrevContColorVertex = GetPrevContColorVertex();
   ColorVertex *pNextContColorVertex = GetNextContColorVertex();

   if (IsPropertyVertexPeaked(pPrevContColorVertex))
      pPrevContColorEdge = NULL;

   if (IsPropertyVertexPeaked(pNextContColorVertex))
      pNextContColorEdge = NULL;

   Vector4 prevtan;
   float tangentlength;
   if (pPrevContColorEdge)
   {
      prevtan = pNextContColorVertex->GetPos() - pPrevContColorEdge->GetPrevContColorVertex()->GetPos();
      float fPrevLength = (pPrevContColorEdge->GetPrevContColorVertex()->GetPos() - pPrevContColorVertex->GetPos()).Length();
      float fNextLength = (pNextContColorVertex->GetPos() - pPrevContColorVertex->GetPos()).Length();
      float fRatio = fNextLength / (fPrevLength + fNextLength);
      tangentlength = prevtan.Length() * fRatio;
   }
   else
   {
      prevtan = pNextContColorVertex->GetPos() - pPrevContColorVertex->GetPos();
      tangentlength = prevtan.Length() * 0.5f;
   }

   prevtan.Normalize();
   prevtan *= tangentlength;

   Vector4 nexttan;
   if (pNextContColorEdge)
   {
      nexttan = pNextContColorEdge->GetNextContColorVertex()->GetPos() - pPrevContColorVertex->GetPos();
      float fPrevLength = (pPrevContColorVertex->GetPos() - pNextContColorVertex->GetPos()).Length();
      float fNextLength = (pNextContColorEdge->GetNextContColorVertex()->GetPos() - pNextContColorVertex->GetPos()).Length();
      float fRatio = fPrevLength / (fPrevLength + fNextLength);
      tangentlength = nexttan.Length() * fRatio;
   }
   else
   {
      nexttan = pNextContColorVertex->GetPos() - pPrevContColorVertex->GetPos();
      tangentlength = nexttan.Length() * 0.5f;
   }

   nexttan.Normalize();
   nexttan *= tangentlength;

   if ( pPrevContColorVertex == GetFirstColorVertex() )
   {
      GetFirstColorVertexHandle()->SetTangent(prevtan);
      GetSecondColorVertexHandle()->SetTangent(-1 * nexttan);
   }
   else if (pPrevContColorVertex == GetSecondColorVertex())
   {
      GetFirstColorVertexHandle()->SetTangent(-1 * nexttan);
      GetSecondColorVertexHandle()->SetTangent(prevtan);
   }
}

Vector4 ColorPatchEdge::GetPosAlongEdge( float fPosition, ColorPoly *pColorVertexPoly )
{
   ASSERT(fPosition >= 0.0 && fPosition <= 1.0f);

   Vector4 v0,cv0,cv1,v1;
   v0 = GetAlignedFirstColorVertex( pColorVertexPoly )->GetPos();
   cv0 = GetAlignedFirstColorVertexTangentPos( pColorVertexPoly );
   cv1 = GetAlignedSecondColorVertexTangentPos( pColorVertexPoly );
   v1 = GetAlignedSecondColorVertex( pColorVertexPoly )->GetPos();

   Vector4 result;
   result.x = MathTools::Bezier( fPosition, v0.x, cv0.x, cv1.x, v1.x);
   result.y = MathTools::Bezier( fPosition, v0.y, cv0.y, cv1.y, v1.y);
   result.z = MathTools::Bezier( fPosition, v0.z, cv0.z, cv1.z, v1.z);
   result.w = MathTools::Bezier( fPosition, v0.w, cv0.w, cv1.w, v1.w); 
   return result;// along curve
}

Vector4 ColorPatchEdge::GetFirstColorVertexTangentPos()
{
   return GetFirstColorVertexHandle()->GetTangent() * 0.3333f + GetFirstColorVertex()->GetPos();
}

Vector4 ColorPatchEdge::GetSecondColorVertexTangentPos()
{
   return GetSecondColorVertexHandle()->GetTangent() * 0.3333f + GetSecondColorVertex()->GetPos();
}