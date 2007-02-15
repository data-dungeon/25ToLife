#include "stdafx.h"
#include "PatchExtras.h"
#include "PatchModel.h"

BOOL TessEdge::FindClosestIntersection( const Vector &point, Vector &intersection, float &param )
{
   TransformableObject *pTransformableObject = (TransformableObject*)m_pEdge->GetOwner();
   Matrix44 transfrommatrix = pTransformableObject->GetObjectToWorldMatrix();
   Vector i;
   float percent;
   UINT nNumVertsMinusOne = GetNumVertices()-1;
   for (UINT j = 0; j < nNumVertsMinusOne; j++)
   {
      Vertex *pV0 = GetVertexAt(j);
      Vertex *pV1 = GetVertexAt(j+1);
      Vector p0 = pV0->GetWorldPos(pTransformableObject);
      Vector p1 = pV1->GetWorldPos(pTransformableObject);
      MathTools::PointToLineIntersection(point,p0,p1,i,percent);
      if ((percent >= 0.0f)&&(percent <= 1.0f))
      {
         intersection = i;
         float ratio = (float)j / (float)nNumVertsMinusOne;
         param = (percent/(float)nNumVertsMinusOne) + ratio;
         return TRUE;
      }
   }
   return FALSE;
}
