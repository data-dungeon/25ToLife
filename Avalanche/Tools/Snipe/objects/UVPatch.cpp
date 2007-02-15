#include "stdafx.h"
#include "UVPatch.h"
#include "../Utility/Matrix44.h"
#include "TessPoly.h"
#include "TessEdge.h"
#include "UVPatchEdge.h"
#include "Patch.h"
#include "PatchUV.h"

IMPLEMENT_COMPONENT_CREATE( UVPatch, Face )

void UVPatch::BuildQuadPatchTangents()
{
   UINT order[] = { 0,1,4, 12,8,13, 15,14,11, 3,7,2 };

   UINT nNumVerts = GetPatch()->GetNumSides();

   Vector2 *pUVCVs = GetUVCVs(0);
   int nCount = 0;
   for (UINT i = 0; i < nNumVerts; i++)
   {
      PatchUV *pUV = (PatchUV*)GetUVAt(i);
      pUV->GetTangents(this, pUVCVs[order[(nCount*3)+1]], pUVCVs[order[(nCount*3)+2]] );
      pUVCVs[order[nCount*3]] = pUV->GetPos();
      nCount++;
   }
   CalculateInteriorTangents(0);
}

Vector2 UVPatch::CalculateNPatchCenter()
{
   UINT numUVs = GetPatch()->GetNumSides();

   Vector2 centeruv(0,0);
   for (UINT i = 0; i < numUVs; i++)
   {
      Vector2 vect = GetUVAt(i)->GetPos();
      centeruv += vect;
   }
   centeruv /= numUVs;
   return centeruv;
}

void UVPatch::BuildNPatchTangents()
{
   Vector2 centeruv = CalculateNPatchCenter();

   Vector2 uv0, uv1, v2notused;
   Vector2 c0, d0, d3;
   Vector2 cuv0,cuv1;

   for (UINT nSubPatchIndex = 0; nSubPatchIndex < GetPatch()->GetNumSubPatches(); nSubPatchIndex++)
   {
      UINT nPrevSubPatchIndex = GetPatch()->GetPrevSubPatchIndex(nSubPatchIndex);

      Vector2 *pUVCVs = GetUVCVs(nSubPatchIndex);

      PatchUV *pUV0 = (PatchUV*)GetUVAt( nPrevSubPatchIndex );
      PatchUV *pUV1 = (PatchUV*)GetUVAt( nSubPatchIndex );

      pUV0->GetTangents( this, v2notused, cuv1 );
      pUV1->GetTangents( this, cuv1, v2notused );

      c0 = pUV0->GetPos();
      d3 = pUV1->GetPos();
      d0 = (d3-c0)*0.5f + c0;

      pUVCVs[0] = centeruv;
      pUVCVs[4] = (d0-centeruv)/3.0f + centeruv;
      pUVCVs[8] = d0+(centeruv-d0)/3.0f;
      pUVCVs[12] = d0;
      pUVCVs[13] = (d3-d0)/3.0f + d0;
      pUVCVs[14] = d3 + (d0-d3)/3.0f;
      pUVCVs[15] = d3;

      UINT nNextSubPatchIndex = GetPatch()->GetNextSubPatchIndex(nSubPatchIndex);

      pUV0 = (PatchUV*)GetUVAt( nSubPatchIndex );
      pUV1 = (PatchUV*)GetUVAt( nNextSubPatchIndex );

      c0 = pUV0->GetPos();
      d3 = pUV1->GetPos();
      d0 = (d3-c0)*0.5f + c0;

      pUVCVs[11]= (d0-c0)/3.0f + c0;
      pUVCVs[7]= d0 + (c0-d0)/3.0f;
      pUVCVs[3]= d0;
      pUVCVs[2]= (centeruv-d0)/3.0f + d0;
      pUVCVs[1]= centeruv + (d0-centeruv)/3.0f;

      CalculateInteriorTangents(nSubPatchIndex);
   }
}

void UVPatch::SetTessellation( int nUSubdivisions, int nVSubdivisions )
{
   for (UINT nSubPatchIndex = 0; nSubPatchIndex < GetPatch()->GetNumSubPatches(); nSubPatchIndex++)
   {
      DynamicMatrix *pUVMat = GetMatrix(nSubPatchIndex);

      int nRows = pUVMat->GetNumRows();
      int nCols = pUVMat->GetNumCols();

      for (int nV = 0; nV < nRows; nV++)
      {
         for (int nU = 0; nU < nCols; nU++)
         {
            float fS = (float)(nU+1)/(float)nUSubdivisions;
            float fT = (float)(nV+1)/(float)nVSubdivisions;

            UV *pUV;
            pUVMat->Get( nV, nU, (UINT &)pUV );

            Vector2 pos = CalculateUV( fS, fT, nSubPatchIndex );
            pUV->SetPos(pos);
         }
      }
   }
}

void UVPatch::SetTessellationInternalEdge( int nSub, UINT nSubPatchIndex )
{
   double t;
   Vector2 uv;
   int nNumSteps = 1 << nSub;//force all internal edges to the same number of subdivisions
   double inc = 1.0 / nNumSteps;
 
   TessEdgeUVs *pTessEdgeUVs = (TessEdgeUVs*)&m_pInternalTessPropertyEdges[nSubPatchIndex];

   Vector2 *pUVCVs = GetVector2CVs(nSubPatchIndex);

   UV *pUV = pTessEdgeUVs->GetUVAt(0, FALSE);

   pTessEdgeUVs->GetUVAt(0, FALSE)->SetPos(pUVCVs[3]);

   int i;
   for ( t=inc, i=1; i<nNumSteps; t+=inc, i++ )
   {
      uv = MathTools::Bezier( t, pUVCVs[3], pUVCVs[2], pUVCVs[1], pUVCVs[0] );
      pTessEdgeUVs->GetUVAt(i, FALSE)->SetPos(uv); // COMEBACK DAN
   }

   pTessEdgeUVs->GetUVAt(pTessEdgeUVs->GetNumUVs()-1, FALSE)->SetPos(pUVCVs[0]);
}


