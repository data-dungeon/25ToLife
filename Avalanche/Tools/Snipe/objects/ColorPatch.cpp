#include "stdafx.h"
#include "ColorPatch.h"
#include "../Utility/Matrix44.h"
#include "TessPoly.h"
#include "TessEdge.h"
#include "ColorPatchEdge.h"
#include "Patch.h"
#include "ColorPatchVertex.h"
#include "TessColorEdge.h"

IMPLEMENT_COMPONENT_CREATE( ColorPatch, Face )

void ColorPatch::BuildQuadPatchTangents()
{
   UINT order[] = { 0,1,4, 12,8,13, 15,14,11, 3,7,2 };

   UINT nNumVerts = GetPatch()->GetNumSides();

   Vector4 *pCVs = GetCVs(0);
   int nCount = 0;
   for (UINT i = 0; i < nNumVerts; i++)
   {
      ColorPatchVertex *pColorVertex = (ColorPatchVertex*)GetColorVertexAt(i);
      pColorVertex->GetTangents(this, pCVs[order[(nCount*3)+1]], pCVs[order[(nCount*3)+2]] );
      pCVs[order[nCount*3]] = pColorVertex->GetPos();
      nCount++;
   }
   CalculateInteriorTangents(0);
}

Vector4 ColorPatch::CalculateNPatchCenter()
{
   UINT numColorVertices = GetPatch()->GetNumSides();

   Vector4 centerColorVertex(0,0,0,0);
   for (UINT i = 0; i < numColorVertices; i++)
   {
      Vector4 vect = GetColorVertexAt(i)->GetPos();
      centerColorVertex += vect;
   }
   centerColorVertex /= numColorVertices;
   return centerColorVertex;
}

void ColorPatch::BuildNPatchTangents()
{
   Vector4 centerColorVertex = CalculateNPatchCenter();

   Vector4 ColorVertex0, ColorVertex1, v2notused;
   Vector4 c0, d0, d3;
   Vector4 cColorVertex0,cColorVertex1;

   for (UINT nSubPatchIndex = 0; nSubPatchIndex < GetPatch()->GetNumSubPatches(); nSubPatchIndex++)
   {
      UINT nPrevSubPatchIndex = GetPatch()->GetPrevSubPatchIndex(nSubPatchIndex);

      Vector4 *pCVs = GetCVs(nSubPatchIndex);

      ColorPatchVertex *pColorVertex0 = (ColorPatchVertex*)GetColorVertexAt( nPrevSubPatchIndex );
      ColorPatchVertex *pColorVertex1 = (ColorPatchVertex*)GetColorVertexAt( nSubPatchIndex );

      pColorVertex0->GetTangents( this, v2notused, cColorVertex1 );
      pColorVertex1->GetTangents( this, cColorVertex1, v2notused );

      c0 = pColorVertex0->GetPos();
      d3 = pColorVertex1->GetPos();
      d0 = (d3-c0)*0.5f + c0;

      pCVs[0] = centerColorVertex;
      pCVs[4] = (d0-centerColorVertex)/3.0f + centerColorVertex;
      pCVs[8] = d0+(centerColorVertex-d0)/3.0f;
      pCVs[12] = d0;
      pCVs[13] = (d3-d0)/3.0f + d0;
      pCVs[14] = d3 + (d0-d3)/3.0f;
      pCVs[15] = d3;

      UINT nNextSubPatchIndex = GetPatch()->GetNextSubPatchIndex(nSubPatchIndex);

      pColorVertex0 = (ColorPatchVertex*)GetColorVertexAt( nSubPatchIndex );
      pColorVertex1 = (ColorPatchVertex*)GetColorVertexAt( nNextSubPatchIndex );

      c0 = pColorVertex0->GetPos();
      d3 = pColorVertex1->GetPos();
      d0 = (d3-c0)*0.5f + c0;

      pCVs[11]= (d0-c0)/3.0f + c0;
      pCVs[7]= d0 + (c0-d0)/3.0f;
      pCVs[3]= d0;
      pCVs[2]= (centerColorVertex-d0)/3.0f + d0;
      pCVs[1]= centerColorVertex + (d0-centerColorVertex)/3.0f;

      CalculateInteriorTangents(nSubPatchIndex);
   }
}

void ColorPatch::SetTessellation( int nUSubdivisions, int nVSubdivisions )
{
   for (UINT nSubPatchIndex = 0; nSubPatchIndex < GetPatch()->GetNumSubPatches(); nSubPatchIndex++)
   {
      DynamicMatrix *pColorVertexMat = GetMatrix(nSubPatchIndex);

      int nRows = pColorVertexMat->GetNumRows();
      int nCols = pColorVertexMat->GetNumCols();

      for (int nV = 0; nV < nRows; nV++)
      {
         for (int nU = 0; nU < nCols; nU++)
         {
            float fS = (float)(nU+1)/(float)nUSubdivisions;
            float fT = (float)(nV+1)/(float)nVSubdivisions;

            ColorVertex *pColorVertex;
            pColorVertexMat->Get( nV, nU, (UINT &)pColorVertex );

            Vector4 pos = CalculateColor( fS, fT, nSubPatchIndex );
            pColorVertex->SetPos(pos);
         }
      }
   }
}

void ColorPatch::SetTessellationInternalEdge( int nSub, UINT nSubPatchIndex )
{
   double t;
   Vector4 v4ColorVertex;
   int nNumSteps = 1 << nSub;//force all internal edges to the same number of subdivisions
   double inc = 1.0 / nNumSteps;
 
   TessColorEdge *pTessColorEdge = (TessColorEdge*)&m_pInternalTessPropertyEdges[nSubPatchIndex];

   Vector4 *pCVs = GetVector4CVs(nSubPatchIndex);

   ColorPatchVertex *pColorVertex = (ColorPatchVertex*)pTessColorEdge->GetColorVertexAt(0, FALSE);

   pTessColorEdge->GetColorVertexAt(0, FALSE)->SetPos(pCVs[3]);

   int i;
   for ( t=inc, i=1; i<nNumSteps; t+=inc, i++ )
   {
      v4ColorVertex = MathTools::Bezier( t, pCVs[3], pCVs[2], pCVs[1], pCVs[0] );
      pTessColorEdge->GetColorVertexAt(i, FALSE)->SetPos(v4ColorVertex);
   }

   pTessColorEdge->GetColorVertexAt(pTessColorEdge->GetNumColorVertices()-1, FALSE)->SetPos(pCVs[0]);
}


