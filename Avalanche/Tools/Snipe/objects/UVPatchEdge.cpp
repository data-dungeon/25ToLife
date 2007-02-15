#include "stdafx.h"
#include "UVPatchEdge.h"
#include "PatchEdge.h"
#include "../Utility/MathTools.h"
#include "PatchUV.h"

IMPLEMENT_COMPONENT_CREATE( UVPatchEdge, Edge )

void UVPatchEdge::SetTessellation()
{
   PatchEdge *pPatchEdge = GetEdge();
   int nSub = pPatchEdge->CalcEdgeSubdivisions();

   int nNumSteps = 1 << nSub;
   if (nNumSteps==1)
      return;

   double inc = 1.0 / nNumSteps;

   Vector p0( GetFirstUV()->GetPos() );
   Vector p1( GetFirstUVHandle()->GetTangent() );
   Vector p2( -GetSecondUVHandle()->GetTangent() );
   Vector p3( GetSecondUV()->GetPos() );

   Vector v;
   for (double t=inc, i=1; i<nNumSteps; t+=inc, i++)
   {
      v = (( 2*t*t*t)-(3*t*t)+1)*p0
         + ((-2*t*t*t)+(3*t*t)  )*p3
         + ((   t*t*t)-(2*t*t)+t)*p1
         + ((   t*t*t)-(  t*t)  )*p2;

      GetTessUVAt( i, FALSE )->SetPos( Vector2(v.x,v.y) );
   }
}

void UVPatchEdge::BuildTangents()
{
   if (!m_bTangentsDirty)
      return;

   m_bTangentsDirty = FALSE;

   UVPatchEdge *pPrevContUVEdge = GetPrevContUVEdge(TRUE);
   UVPatchEdge *pNextContUVEdge = GetNextContUVEdge(TRUE);

   UV *pPrevContUV = GetPrevContUV();
   UV *pNextContUV = GetNextContUV();

   const Vector2 *pV0;
   if (pPrevContUVEdge && !IsPropertyVertexPeaked(pPrevContUV))
      pV0 = &pPrevContUVEdge->GetPrevContUV()->GetPos();
   else
      pV0 = NULL;

   const Vector2 &pV1 = GetPrevContUV()->GetPos();
   const Vector2 &pV2 = GetNextContUV()->GetPos();

   const Vector2 *pV3;
   if (pNextContUVEdge && !IsPropertyVertexPeaked(pNextContUV))
      pV3 = &pNextContUVEdge->GetNextContUV()->GetPos();
   else
      pV3 = NULL;

   VertexHandle *pHandle1 = GetEdge()->GetVertexHandle(GetEdge()->GetPrevContVertex());
   VertexHandle *pHandle2 = GetEdge()->GetVertexHandle(GetEdge()->GetNextContVertex());

   Vector2 vTangent0;
   Vector2 vTangent1;
   MathTools::ComputeTangents(pV0, pV1, pV2, pV3, &vTangent0, &vTangent1,
      pHandle1->m_fAlpha, pHandle1->m_fMagnitude,
      pHandle2->m_fAlpha, pHandle2->m_fMagnitude);

   if ( GetPrevContUV() == GetFirstUV() )
   {
      GetFirstUVHandle()->SetTangent(vTangent0);
      GetSecondUVHandle()->SetTangent(vTangent1);
   }
   else if (GetPrevContUV()==GetSecondUV())
   {
      GetFirstUVHandle()->SetTangent(vTangent1);
      GetSecondUVHandle()->SetTangent(vTangent0);
   }
}

Vector2 UVPatchEdge::GetPosAlongEdge( float fPosition, UVPoly *pUVPoly )
{
   ASSERT(fPosition >= 0.0 && fPosition <= 1.0f);

   Vector2 v0,cv0,cv1,v1;
   v0 = GetAlignedFirstUV( pUVPoly )->GetPos();
   cv0 = GetAlignedFirstUVTangentPos( pUVPoly );
   cv1 = GetAlignedSecondUVTangentPos( pUVPoly );
   v1 = GetAlignedSecondUV( pUVPoly )->GetPos();

   Vector2 result;
   result.x = MathTools::Bezier( fPosition, v0.x, cv0.x, cv1.x, v1.x);
   result.y = MathTools::Bezier( fPosition, v0.y, cv0.y, cv1.y, v1.y);

   return result;// along curve
}

void UVPatchEdge::Draw( BOOL bDrawTess )
{
   if (!bDrawTess)
   {
      glBegin( GL_LINES );
         glVertex2fv( &GetFirstUV()->GetPos().x );
         glVertex2fv( &GetSecondUV()->GetPos().x );
      glEnd();
      return;
   }

#if 0
   glPushAttrib(GL_CURRENT_BIT);
   glColor3f(0.0f,1.0f,0.0f);
   Vector tangent;
   glBegin( GL_POINTS );
      tangent = this->GetFirstUVTangentPos();
      glVertex2fv( &tangent.x );
      tangent = this->GetSecondUVTangentPos();
      glVertex2fv( &tangent.x );
   glEnd();
   glPopAttrib();
#endif

   UINT nNumVerts = GetNumTessUVs();
   if (nNumVerts==0)
      return;

   glBegin(GL_LINES);
      for (UINT i = 0; i < nNumVerts-1; i++)
      {
         UV *pUV = GetTessUVAt(i);
         ASSERT(pUV);
         UV *pNextUV = GetTessUVAt(i+1);
         ASSERT(pNextUV);
         glVertex2fv( &pUV->GetPos().x );
         glVertex2fv( &pNextUV->GetPos().x );
      }
   glEnd();
}

Vector2 UVPatchEdge::GetFirstUVTangentPos()
{
   return GetFirstUVHandle()->GetTangent() * 0.3333f + GetFirstUV()->GetPos();
}

Vector2 UVPatchEdge::GetSecondUVTangentPos()
{
   return GetSecondUVHandle()->GetTangent() * 0.3333f + GetSecondUV()->GetPos();
}
