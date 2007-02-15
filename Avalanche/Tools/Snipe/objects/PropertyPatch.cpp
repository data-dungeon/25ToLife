#include "stdafx.h"
#include "PropertyPatch.h"
#include "Patch.h"
#include "TessEdge.h"
#include "PropertyPatchEdge.h"
#include "PatchModel.h"

IMPLEMENT_COMPONENT_CREATE( PropertyPatch, "Face" )

PropertyPatch::~PropertyPatch()
{
   ClearInternalTessPropertyEdges();
   ClearBezierBasisMatrices();
}

void PropertyPatch::Disconnect()
{
   ClearInternalTessPropertyEdges();
   PropertyPoly::Disconnect();
}

TessPropertyPoly *PropertyPatch::AddTessellatedPropertyPolys( UINT nCount, TessPoly *pTessPolyArray )
{ 
   m_nNumTessPropertyPolys = nCount;
   m_pTessPropertyPolyArray = SNEW TessPropertyPoly[nCount];

   ASSERT(pTessPolyArray);

   for (UINT i=0; i<nCount; i++)
   {
      TessPoly &pTessPoly = pTessPolyArray[i];
      TessPropertyPoly &pTessPropertyPoly = m_pTessPropertyPolyArray[i];

      pTessPropertyPoly.Init(this);
      pTessPoly.AddTessPropertyPoly( &pTessPropertyPoly );
   }

   return m_pTessPropertyPolyArray;
}

//////////////////////////////////////////////////////////////////////////////////

Vector VectorBezierBasisMatrix::Calculate(float fS, float fT)
{
   Vector *pCVs = GetCVs();

   float fBezierbasis[16] = { -1,3,-3,1, 3,-6, 3, 0, -3,3,0,0 , 1,0,0,0};

   float fPatchx[16] = { pCVs[0].x,pCVs[1].x,pCVs[2].x,pCVs[3].x,
      pCVs[4].x,pCVs[5].x,pCVs[6].x,pCVs[7].x,
      pCVs[8].x,pCVs[9].x,pCVs[10].x,pCVs[11].x,
      pCVs[12].x,pCVs[13].x,pCVs[14].x,pCVs[15].x };
   float fPatchy[16] = { pCVs[0].y,pCVs[1].y,pCVs[2].y,pCVs[3].y,
      pCVs[4].y,pCVs[5].y,pCVs[6].y,pCVs[7].y,
      pCVs[8].y,pCVs[9].y,pCVs[10].y,pCVs[11].y,
      pCVs[12].y,pCVs[13].y,pCVs[14].y,pCVs[15].y };
   float fPatchz[16] = { pCVs[0].z,pCVs[1].z,pCVs[2].z,pCVs[3].z,
      pCVs[4].z,pCVs[5].z,pCVs[6].z,pCVs[7].z,
      pCVs[8].z,pCVs[9].z,pCVs[10].z,pCVs[11].z,
      pCVs[12].z,pCVs[13].z,pCVs[14].z,pCVs[15].z };

   float fS2 = fS*fS;
   float fS3 = fS2*fS;
   float fT2 = fT*fT;
   float fT3 = fT2*fT;

   float S[16] = {fS3,0,0,0, fS2,1,0,0, fS,0,1,0, 1,0,0,1};
   float T[16] = {fT3,fT2,fT,1, 0,1,0,0, 0,0,1,0, 0,0,0,1};

   Matrix44 patchx(fPatchx), patchy(fPatchy), patchz(fPatchz),s(S),t(T),bezierbasis(fBezierbasis);

   Matrix44 tempA = bezierbasis*s;
   Matrix44 tempB = t*bezierbasis;

   Matrix44 resultx = tempB*patchx*tempA;
   Matrix44 resulty = tempB*patchy*tempA;
   Matrix44 resultz = tempB*patchz*tempA;

   Vector pos;
   pos.x = resultx.Get(0,0);
   pos.y = resulty.Get(0,0);
   pos.z = resultz.Get(0,0);
   return pos;
}

Vector2 Vector2BezierBasisMatrix::Calculate(float fS, float fT)
{
   Vector2 *pCVs = GetCVs();

   float fBezierbasis[16] = { -1,3,-3,1, 3,-6, 3, 0, -3,3,0,0 , 1,0,0,0};

   float fPatchx[16] = { pCVs[0].x,pCVs[1].x,pCVs[2].x,pCVs[3].x,
      pCVs[4].x,pCVs[5].x,pCVs[6].x,pCVs[7].x,
      pCVs[8].x,pCVs[9].x,pCVs[10].x,pCVs[11].x,
      pCVs[12].x,pCVs[13].x,pCVs[14].x,pCVs[15].x };
   float fPatchy[16] = { pCVs[0].y,pCVs[1].y,pCVs[2].y,pCVs[3].y,
      pCVs[4].y,pCVs[5].y,pCVs[6].y,pCVs[7].y,
      pCVs[8].y,pCVs[9].y,pCVs[10].y,pCVs[11].y,
      pCVs[12].y,pCVs[13].y,pCVs[14].y,pCVs[15].y };

   float fS2 = fS*fS;
   float fS3 = fS2*fS;
   float fT2 = fT*fT;
   float fT3 = fT2*fT;

   float S[16] = {fS3,0,0,0, fS2,1,0,0, fS,0,1,0, 1,0,0,1};
   float T[16] = {fT3,fT2,fT,1, 0,1,0,0, 0,0,1,0, 0,0,0,1};

   Matrix44 patchx(fPatchx), patchy(fPatchy),s(S),t(T),bezierbasis(fBezierbasis);

   Matrix44 tempA = bezierbasis*s;
   Matrix44 tempB = t*bezierbasis;

   Matrix44 resultx = tempB*patchx*tempA;
   Matrix44 resulty = tempB*patchy*tempA;

   Vector2 pos;
   pos.x = resultx.Get(0,0);
   pos.y = resulty.Get(0,0);
   return pos;
}

Vector4 Vector4BezierBasisMatrix::Calculate(float fS, float fT)
{
   Vector4 *pCVs = GetCVs();

   float fBezierbasis[16] = { -1,3,-3,1, 3,-6, 3, 0, -3,3,0,0 , 1,0,0,0};

   float fPatchx[16] = { pCVs[0].x,pCVs[1].x,pCVs[2].x,pCVs[3].x,
      pCVs[4].x,pCVs[5].x,pCVs[6].x,pCVs[7].x,
      pCVs[8].x,pCVs[9].x,pCVs[10].x,pCVs[11].x,
      pCVs[12].x,pCVs[13].x,pCVs[14].x,pCVs[15].x };
   float fPatchy[16] = { pCVs[0].y,pCVs[1].y,pCVs[2].y,pCVs[3].y,
      pCVs[4].y,pCVs[5].y,pCVs[6].y,pCVs[7].y,
      pCVs[8].y,pCVs[9].y,pCVs[10].y,pCVs[11].y,
      pCVs[12].y,pCVs[13].y,pCVs[14].y,pCVs[15].y };
   float fPatchz[16] = { pCVs[0].z,pCVs[1].z,pCVs[2].z,pCVs[3].z,
      pCVs[4].z,pCVs[5].z,pCVs[6].z,pCVs[7].z,
      pCVs[8].z,pCVs[9].z,pCVs[10].z,pCVs[11].z,
      pCVs[12].z,pCVs[13].z,pCVs[14].z,pCVs[15].z };
   float fPatchw[16] = { pCVs[0].w,pCVs[1].w,pCVs[2].w,pCVs[3].w,
      pCVs[4].w,pCVs[5].w,pCVs[6].w,pCVs[7].w,
      pCVs[8].w,pCVs[9].w,pCVs[10].w,pCVs[11].w,
      pCVs[12].w,pCVs[13].w,pCVs[14].w,pCVs[15].w };

   float fS2 = fS*fS;
   float fS3 = fS2*fS;
   float fT2 = fT*fT;
   float fT3 = fT2*fT;

   float S[16] = {fS3,0,0,0, fS2,1,0,0, fS,0,1,0, 1,0,0,1};
   float T[16] = {fT3,fT2,fT,1, 0,1,0,0, 0,0,1,0, 0,0,0,1};

   Matrix44 patchx(fPatchx), patchy(fPatchy), patchz(fPatchz), patchw(fPatchw), s(S),t(T),bezierbasis(fBezierbasis);

   Matrix44 tempA = bezierbasis*s;
   Matrix44 tempB = t*bezierbasis;

   Matrix44 resultx = tempB*patchx*tempA;
   Matrix44 resulty = tempB*patchy*tempA;
   Matrix44 resultz = tempB*patchz*tempA;
   Matrix44 resultw = tempB*patchw*tempA;

   Vector4 pos;
   pos.x = resultx.Get(0,0);
   pos.y = resulty.Get(0,0);
   pos.z = resultz.Get(0,0);
   pos.w = resultz.Get(0,0);
   return pos;
}

void PropertyPatch::FillOutTessPropertyPolysPropertyVertices( TessPropertyPoly *pTessPropertyPoly, UINT nSubPatchIndex, int u, int v, int usubdivisions, int vsubdivisions,
 TessPropertyEdge *pLeftTessPropertyEdge,  BOOL bLeftReversed,  TessPropertyEdge *pBottomTessPropertyEdge, BOOL bBottomReversed,
 TessPropertyEdge *pRightTessPropertyEdge, BOOL bRightReversed, TessPropertyEdge *pTopTessPropertyEdge,    BOOL bTopReversed )
{
   CArray<PropertyVertex *> *pPropertyVertices = NULL;

   BOOL bFlag0, bFlag1, bFlag2, bFlag3;

   int usubdivisions2 = usubdivisions - 1;
   int vsubdivisions2 = vsubdivisions - 1;

   int corner0, corner1, corner2, corner3;

   corner0 = ((v-1) * (usubdivisions-1)) + (u-1);
   corner1 = (v * (usubdivisions-1)) + (u-1);
   corner2 = (v * (usubdivisions-1)) + u;
   corner3 = ((v-1) * (usubdivisions-1)) + u;

   UINT nv[4];
   int n0;
   int i;
   PropertyVertex *pPropertyVertex;

   // build data for multiple uv sets
   DynamicMatrix *pMat = GetMatrix(nSubPatchIndex);
   PropertySet *pPropertySet = GetPropertySet();

   // top left corner
   if ((u!=0)&&(v!=0))
      pMat->Get( corner0, nv[0] );

   // bottom left corner
   if ((u!=0)&&(v!=vsubdivisions-1))
      pMat->Get( corner1, nv[1] );

   // bottom right corner
   if ((u!=usubdivisions-1)&&(v!=vsubdivisions-1))
      pMat->Get( corner2, nv[2] );

   // top right corner
   if ((u!=usubdivisions-1)&&(v!=0))
      pMat->Get( corner3, nv[3] );

   UINT uvcount = 0;

   bFlag0 = 0;
   bFlag1 = 0;
   bFlag2 = 0;
   bFlag3 = 0;

   if (v == 0)
      bFlag0 = 1;

   // fix cracks at left edge
   if (u == 0)
   {
      n0 = 0;
      if (bFlag0)
         n0 = 1;
      int factor = ((int)pLeftTessPropertyEdge->GetNumPropertyVertices()-1)/(vsubdivisions);
      for (i = (v*factor)+n0; i < (v*factor)+factor+1; i++)
      {
         pPropertyVertex = pLeftTessPropertyEdge->GetPropertyVertexAt( i, bLeftReversed );
         ASSERT(pPropertyVertex);
         pTessPropertyPoly->SetPropertyVertexAt(uvcount++, pPropertyVertex );
      }
      bFlag1 = TRUE;
   }
   else
   {
      if (!bFlag0)
      {
         // top left corner
         pPropertyVertex = (PropertyVertex *)nv[0];
         ASSERT(pPropertyVertex);
         pTessPropertyPoly->SetPropertyVertexAt( uvcount++, pPropertyVertex );
      }
   }

   // fix cracks at bottom edge
   if (v == vsubdivisions-1)
   {
      n0 = 0;
      if (bFlag1)
         n0 = 1;
      int factor = ((int)pBottomTessPropertyEdge->GetNumPropertyVertices()-1)/(usubdivisions);
      for (i = (u*factor)+n0; i < (u*factor)+factor+1; i++)
      {
         pPropertyVertex = pBottomTessPropertyEdge->GetPropertyVertexAt( i, bBottomReversed );
         ASSERT(pPropertyVertex);
         pTessPropertyPoly->SetPropertyVertexAt( uvcount++, pPropertyVertex );
      }
      bFlag2 = TRUE;	
   }
   else
   {
      if (!bFlag1)
      {
         // bottom left corner
         pPropertyVertex = (PropertyVertex *)nv[1];
         ASSERT(pPropertyVertex);
         pTessPropertyPoly->SetPropertyVertexAt( uvcount++, pPropertyVertex );
      }
   }

   // fix cracks at right edge
   if (u == usubdivisions-1)
   {
      n0 = 0;
      if (bFlag2)
         n0 = 1;
      int factor = ((int)pRightTessPropertyEdge->GetNumPropertyVertices()-1)/(vsubdivisions);
      for (i = ((vsubdivisions-v-1)*factor)+n0; i < ((vsubdivisions-v-1)*factor)+factor+1; i++)
      {
         pPropertyVertex = pRightTessPropertyEdge->GetPropertyVertexAt( i, bRightReversed );
         ASSERT(pPropertyVertex);
         pTessPropertyPoly->SetPropertyVertexAt( uvcount++, pPropertyVertex );
      }
      bFlag3 = TRUE;
   }
   else
   {
      if (!bFlag2)
      {
         // bottom right corner
         pPropertyVertex = (PropertyVertex *)nv[2];
         ASSERT(pPropertyVertex);
         pTessPropertyPoly->SetPropertyVertexAt( uvcount++, pPropertyVertex );
      }
   }

   // fix cracks at top edge
   if (v == 0)
   {
      n0 = 0;
      if (bFlag3)
         n0 = 1;
      int factor = ((int)pTopTessPropertyEdge->GetNumPropertyVertices()-1)/(usubdivisions);
      for (i = ((usubdivisions-u-1)*factor)+n0; i < ((usubdivisions-u-1)*factor)+factor+1; i++)				
      {
         pPropertyVertex = pTopTessPropertyEdge->GetPropertyVertexAt( i, bTopReversed );
         ASSERT(pPropertyVertex);
         pTessPropertyPoly->SetPropertyVertexAt( uvcount++, pPropertyVertex );
      }
   }
   else
   {
      // top right corner
      if (!bFlag3)
      {
         pPropertyVertex = (PropertyVertex *)nv[3];
         ASSERT(pPropertyVertex);
         pTessPropertyPoly->SetPropertyVertexAt( uvcount++, pPropertyVertex );
      }
   }
}

void PropertyPatch::ClearTessellatedPropertyVertices()
{
   UINT nNumSubPatches = GetPatch()->GetNumSubPatches();

   if (nNumSubPatches==0)
      return;

   // clear old vertex allocation
   PropertyVertex *pHeadPropertyVertex;
   DynamicMatrix *pPVMat = GetMatrix(0);
   if (pPVMat->GetNumCols()>0&&pPVMat->GetNumRows()>0)
   {
      UINT nValue;
      pPVMat->Get(0,0,nValue);
      pHeadPropertyVertex = (PropertyVertex*)nValue;

      UINT nCount = pPVMat->GetNumCols() * pPVMat->GetNumRows() * nNumSubPatches;

      if (pHeadPropertyVertex)
      {
         for (UINT i = 0; i < nCount; i++)
            pHeadPropertyVertex[i].PreDestruct();

         delete [] pHeadPropertyVertex;
      }
   }
}

void PropertyPatch::AllocateTessellation( int nColumns, int nRows )
{
   UINT nNumSubPatches = GetPatch()->GetNumSubPatches();
   PropertyVertex *pPropertyVertex = NULL;

#if 0
   if (( nColumns > 0 ) && ( nRows > 0 ))
   {
      // allocate internal vertices
      PropertyVertex *pHeadPropertyVertex = GetPropertySet()->AddTessellatedPropertyVertices( nRows * nColumns * nNumSubPatches );
      ASSERT(pHeadPropertyVertex);
      pPropertyVertex = pHeadPropertyVertex;
   }
#endif

   for (UINT nSubPatchIndex = 0; nSubPatchIndex < GetPatch()->GetNumSubPatches(); nSubPatchIndex++)
   {
      DynamicMatrix *pMat = GetMatrix(nSubPatchIndex);
      pMat->Initialize(nRows,nColumns);
      for (int nV = 0; nV < nRows; nV++)
      {
         for (int nU = 0; nU < nColumns; nU++)
         {
#if 0
            pPropertyVertex->Reference();
#else
            pPropertyVertex = GetPropertySet()->AddTessellatedPropertyVertex();
#endif
            pMat->Set(nV,nU,(UINT)pPropertyVertex);
#if 0
            pPropertyVertex++;
#endif
         }
      }
   }
}

void PropertyPatch::AllocateTessellationInternalEdge( int nSub, PropertyVertex **pCenterPropertyVertex, UINT nSubPatchIndex, UINT nNPatchHookIndex )
{
   PropertySet *pPropertySet = GetPropertySet();

   if (!*pCenterPropertyVertex)
      *pCenterPropertyVertex = pPropertySet->AddPropertyVertex();

   TessPropertyEdge *pTessPropertyEdge = &m_pInternalTessPropertyEdges[nSubPatchIndex];

   int nNumSteps = 1 << nSub;//force all internal edges to the same number of subdivisions

   pTessPropertyEdge->SetNumPropertyVertices(nNumSteps+1);

   PropertyPatchEdge *pPropertyPatchEdge = (PropertyPatchEdge *)GetPropertyEdgeAt( nSubPatchIndex );

   PropertyVertex *pEdgeCenterPropertyVertex = pPropertyPatchEdge->GetTessPropertyVertexAt(nNPatchHookIndex,FALSE);

   for (int i=0; i<=nNumSteps; i++)
   {
      if (i==0)
         pTessPropertyEdge->SetPropertyVertexAt(i, pEdgeCenterPropertyVertex);     
      else if (i==nNumSteps)
         pTessPropertyEdge->SetPropertyVertexAt(i, *pCenterPropertyVertex);
      else
         pTessPropertyEdge->SetPropertyVertexAt(i, pPropertySet->AddPropertyVertex());;          
   }
}

void PropertyPatch::ClearInternalTessPropertyEdges()
{
   if (!m_pInternalTessPropertyEdges)
      return;
   delete [] m_pInternalTessPropertyEdges;
   m_pInternalTessPropertyEdges = NULL;
}

// this should only be called by ClearTessellatedPolys(), otherwise components don't get destroyed
void PropertyPatch::ClearTessellatedPropertyPolys()
{
   if (!m_pTessPropertyPolyArray)
      return;
   delete [] m_pTessPropertyPolyArray;
   m_pTessPropertyPolyArray = NULL;
}

void PropertyPatch::ClearBezierBasisMatrices()
{
   if (!m_pBezierBasisArray)
      return;
#if 0
  ClearTessellatedPropertyVertices();
#endif
   for (UINT i = 0; i < GetPatch()->GetNumSubPatches(); i++)
      delete m_pBezierBasisArray[i];
   delete [] m_pBezierBasisArray;
   m_pBezierBasisArray = NULL;
}

void PropertyPatch::SetNumInternalTessPropertyEdges(UINT nSize)
{
   ClearInternalTessPropertyEdges();
   m_pInternalTessPropertyEdges = SNEW TessPropertyEdge[nSize];
}

void PropertyPatch::ReverseWinding()
{
   PropertyPoly::ReverseWinding();
   // Do I need to dirty any tess stuff?
}

void PropertyPatch::BuildQuadPatchTessPropertyPoly( TessPoly *pTessPoly, int u, int v, int usubdivisions, int vsubdivisions )
{
   PropertyPatchEdge *pLeftPropertyEdge   = (PropertyPatchEdge *)GetPropertyEdgeAt(0);
   PropertyPatchEdge *pBottomPropertyEdge = (PropertyPatchEdge *)GetPropertyEdgeAt(1);
   PropertyPatchEdge *pRightPropertyEdge  = (PropertyPatchEdge *)GetPropertyEdgeAt(2);
   PropertyPatchEdge *pTopPropertyEdge    = (PropertyPatchEdge *)GetPropertyEdgeAt(3);

   BOOL bLeftReversed   = !pLeftPropertyEdge->IsAligned( this ); 
   BOOL bBottomReversed = !pBottomPropertyEdge->IsAligned( this ); 
   BOOL bRightReversed  = !pRightPropertyEdge->IsAligned( this ); 
   BOOL bTopReversed    = !pTopPropertyEdge->IsAligned( this ); 

   for (TessPropertyPoly *pTessPropertyPoly = pTessPoly->GetHeadTessPropertyPoly(); pTessPropertyPoly; pTessPropertyPoly = pTessPropertyPoly->GetNextTessPropertyPoly())
   {
      FillOutTessPropertyPolysPropertyVertices( pTessPropertyPoly, 0, u, v, usubdivisions, vsubdivisions,
         pLeftPropertyEdge->GetTessPropertyEdge(),  bLeftReversed,  pBottomPropertyEdge->GetTessPropertyEdge(), bBottomReversed,
         pRightPropertyEdge->GetTessPropertyEdge(), bRightReversed, pTopPropertyEdge->GetTessPropertyEdge(),    bTopReversed);
   }
}

void PropertyPatch::BuildNPatchTessPropertyPoly( TessPoly *pTessPoly, UINT nPreviousSubPatchIndex,UINT nSubPatchIndex, int u, int v, int usubdivisions, int vsubdivisions )
{
   PropertyPatchEdge *pBottomPropertyPatchEdge = (PropertyPatchEdge *)GetPropertyEdgeAt( nPreviousSubPatchIndex );
   PropertyPatchEdge *pRightPropertyPatchEdge  = (PropertyPatchEdge *)GetPropertyEdgeAt( nSubPatchIndex );

   //Left
   TessPropertyEdge *pLeftTessPropertyEdge = &m_pInternalTessPropertyEdges[nPreviousSubPatchIndex];

   //Top
   TessPropertyEdge *pTopTessPropertyEdge = &m_pInternalTessPropertyEdges[nSubPatchIndex];

   TessPropertyEdge *pRightTessPropertyEdge = pRightPropertyPatchEdge->GetTessPropertyEdge();

   //Right
   TessPropertyEdge rightTessPropertyEdge;
   BOOL bReversedRight;

   if (pRightPropertyPatchEdge->IsAligned( this ))
   {
      bReversedRight = FALSE;
      pRightPropertyPatchEdge->GetTessPropertyEdge()->GetFirstHalf(rightTessPropertyEdge);
   }
   else
   {
      bReversedRight = TRUE;
      pRightPropertyPatchEdge->GetTessPropertyEdge()->GetSecondHalf(rightTessPropertyEdge);
   }

   //Bottom
   TessPropertyEdge bottomTessPropertyEdge;
   BOOL bReversedBottom;
   if (pBottomPropertyPatchEdge->IsAligned( this ))
   {
      bReversedBottom = FALSE;
      pBottomPropertyPatchEdge->GetTessPropertyEdge()->GetSecondHalf(bottomTessPropertyEdge);
   }
   else
   {
      bReversedBottom = TRUE;
      pBottomPropertyPatchEdge->GetTessPropertyEdge()->GetFirstHalf(bottomTessPropertyEdge);
   }

   for (TessPropertyPoly *pTessPropertyPoly = pTessPoly->GetHeadTessPropertyPoly(); pTessPropertyPoly; pTessPropertyPoly = pTessPropertyPoly->GetNextTessPropertyPoly())
   {
      FillOutTessPropertyPolysPropertyVertices( pTessPropertyPoly, nSubPatchIndex, u, v, usubdivisions, vsubdivisions,
         pLeftTessPropertyEdge, TRUE, &bottomTessPropertyEdge, bReversedBottom, &rightTessPropertyEdge, bReversedRight, pTopTessPropertyEdge, FALSE );
   }
}

void PropertyPatch::SetTangentsDirty( BOOL bDirty )
{
   m_bDirtyTangents = bDirty;
   GetPatch()->SetTessellationDirty( bDirty );
}

void PropertyPatch::BuildTangents()
{
   if (!m_bDirtyTangents)
      return;

   m_bDirtyTangents = FALSE;

   if (GetPatch()->IsQuadPatch())
      BuildQuadPatchTangents();
   else
      BuildNPatchTangents();
}

TessPropertyPoly *PropertyPatch::GetTessPropertyPolyAt( UINT nIndex )
{
   GetPatch()->Tessellate();
   return &m_pTessPropertyPolyArray[nIndex];
}

Vector *PropertyPatch::GetVectorCVs( UINT nIndex )
{
   if (m_bDirtyTangents)
      BuildTangents();
   return ((VectorBezierBasisMatrix*)m_pBezierBasisArray[nIndex])->GetCVs();
}

Vector2 *PropertyPatch::GetVector2CVs( UINT nIndex )
{
   if (m_bDirtyTangents)
      BuildTangents();
   return ((Vector2BezierBasisMatrix*)m_pBezierBasisArray[nIndex])->GetCVs();
}

Vector4 *PropertyPatch::GetVector4CVs( UINT nIndex )
{
   if (m_bDirtyTangents)
      BuildTangents();
   return ((Vector4BezierBasisMatrix*)m_pBezierBasisArray[nIndex])->GetCVs();
}

Vector PropertyPatch::CalculateVector( float fS, float fT, UINT nIndex )
{
   if (m_bDirtyTangents)
      BuildTangents();
  GetPatch()->Tessellate();
  return ((VectorBezierBasisMatrix*)m_pBezierBasisArray[nIndex])->Calculate( fS, fT );
}

Vector2 PropertyPatch::CalculateVector2( float fS, float fT, UINT nIndex )
{
   if (m_bDirtyTangents)
      BuildTangents();
   GetPatch()->Tessellate();
   return ((Vector2BezierBasisMatrix*)m_pBezierBasisArray[nIndex])->Calculate( fS, fT );
}

Vector4 PropertyPatch::CalculateVector4( float fS, float fT, UINT nIndex )
{
   if (m_bDirtyTangents)
      BuildTangents();
   GetPatch()->Tessellate();
   return ((Vector4BezierBasisMatrix*)m_pBezierBasisArray[nIndex])->Calculate( fS, fT );
}