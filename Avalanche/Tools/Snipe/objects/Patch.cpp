#include "stdafx.h"
#include "Patch.h"
#include "PatchModel.h"
#include "PatchEdge.h"
#include "UVPatchEdge.h"
#include "Properties/SurfaceDriver.h"

IMPLEMENT_COMPONENT_CREATE( Patch, Face )

Patch::~Patch()
{
   if (m_pNPatchData)
      delete m_pNPatchData;
   ClearTessellatedPolys();
#if 0
   ClearTessellatedComponents();
#endif
}

Edge *Patch::FindEdgeClosestToPoint( const Vector &point )
{
   Edge *pClosestEdge=NULL;
   float distance, smallestdistance = 0.0;
   Vector i;
   BOOL nFlag = TRUE;
   TransformableObject *pTransformableObject = (TransformableObject*)GetOwner();

   for (UINT nEdgeIndex = 0; nEdgeIndex < GetNumSides(); nEdgeIndex++)
   {
      PatchEdge *pEdge = GetEdgeAt(nEdgeIndex);
      TessEdgeVertices *pTessEdgeVertices = pEdge->GetTessEdgeVertices(this);

      for (UINT j = 0; j < pTessEdgeVertices->GetNumVertices()-1; j++)
      {
         Vertex *pV0 = pTessEdgeVertices->GetVertexAt(j);
         Vertex *pV1 = pTessEdgeVertices->GetVertexAt(j+1);
         Vector p0 = pV0->GetWorldPos(pTransformableObject);
         Vector p1 = pV1->GetWorldPos(pTransformableObject);

         float percent;
         MathTools::PointToLineIntersection( point, p0, p1, i, percent );

         if ( percent<0.0f || percent>1.0f )
            continue;
         distance = (i-point).LengthSq();
   
         if ((nFlag)||(distance<smallestdistance))
         {
            smallestdistance = distance;
            pClosestEdge = pEdge;
            nFlag = FALSE;
         }
      }
   }

   return pClosestEdge;
}

UINT Patch::CalcUSubdivisions()
{
   PatchModel *pPatchModel = (PatchModel*)this->GetOwner();

   int maximumsubdivide = GetMaxSubdivision() + pPatchModel->m_nPatchSubdivisions;

   UINT top = maximumsubdivide;
   UINT bottom = maximumsubdivide;

   if (!pPatchModel->m_bFixedSubdivisions)
   {
      Vector *pCVs = GetCVs(0);
      top = MathTools::CalcBezierCurveSubdivisions(pPatchModel->m_fPatchTolerance,pCVs[0],pCVs[1],pCVs[2],pCVs[3],maximumsubdivide);
      bottom = MathTools::CalcBezierCurveSubdivisions(pPatchModel->m_fPatchTolerance,pCVs[12],pCVs[13],pCVs[14],pCVs[15],maximumsubdivide);
   }

   if (top > bottom)
      return top;
   else
      return bottom;
}

UINT Patch::CalcVSubdivisions()
{
   PatchModel *pPatchModel = (PatchModel*)this->GetOwner();

   int maximumsubdivide = GetMaxSubdivision()+pPatchModel->m_nPatchSubdivisions;

   UINT left = maximumsubdivide;
   UINT right = maximumsubdivide;

   if (!pPatchModel->m_bFixedSubdivisions)
   {
      Vector *pCVs = GetCVs(0);
      left = MathTools::CalcBezierCurveSubdivisions(pPatchModel->m_fPatchTolerance,pCVs[0],pCVs[4],pCVs[8],pCVs[12],maximumsubdivide);
      right = MathTools::CalcBezierCurveSubdivisions(pPatchModel->m_fPatchTolerance,pCVs[3],pCVs[7],pCVs[11],pCVs[15],maximumsubdivide);
   }

   if (left > right)
      return left;
   else
      return right;
}

TessPoly *Patch::AddTessellatedPolys( UINT nCount )
{    
   ClearTessellatedPolys();
   m_nNumTessPolys = nCount;
   PatchModel *pPatchModel = (PatchModel*)GetOwner();
   pPatchModel->m_nNumTessPolys+=nCount;
   m_tesspolyarray = SNEW TessPoly[nCount];
   for (UINT i=0; i<nCount; i++)
      m_tesspolyarray[i].SetPatch( this );
   for (PropertyPatch *pPropertyPatch = (PropertyPatch *)GetHeadPropertyPoly(); pPropertyPatch; pPropertyPatch=(PropertyPatch *)pPropertyPatch->GetNextPropertyPoly() )
      pPropertyPatch->AddTessellatedPropertyPolys( nCount, m_tesspolyarray );
   return m_tesspolyarray;
}

void Patch::ClearTessellatedPolys()
{
   if (!m_tesspolyarray)
      return;
   PatchModel *pPatchModel = (PatchModel*)GetOwner();
   pPatchModel->m_nNumTessPolys-=m_nNumTessPolys;
   delete [] m_tesspolyarray;
   m_tesspolyarray = NULL;
   m_nNumTessPolys = 0;
   // TessPropertyPoly's components get destroyed when TessPoly goes down
   for (PropertyPatch *pPropertyPatch = (PropertyPatch *)GetHeadPropertyPoly(); pPropertyPatch; pPropertyPatch=(PropertyPatch *)pPropertyPatch->GetNextPropertyPoly() )
      pPropertyPatch->ClearTessellatedPropertyPolys();
}

void Patch::ClearTessellatedComponents()
{
   if (GetNumSubPatches()==0)
      return;

   // clear old vertex allocation
   PatchVertex *pHeadVertex;
   VertexNormal *pHeadVertexNormal;
   DynamicMatrix *pVMat = GetVertexMatrix(0);
   DynamicMatrix *pVNMat = GetVertexNormalMatrix(0);
   if ((pVMat->GetNumCols()>0)&&(pVMat->GetNumRows()>0))
   {
      UINT nValue;
      pVMat->Get(0,0,nValue);
      pHeadVertex = (PatchVertex*)nValue;
      pVNMat->Get(0,0,nValue);
      pHeadVertexNormal = (VertexNormal*)nValue;
      UINT nCount = pVMat->GetNumCols() * pVNMat->GetNumRows() * GetNumSubPatches();
      if (pHeadVertex)
      {
         for (UINT i = 0; i < nCount; i++)
            pHeadVertex[i].PreDestruct();
         delete [] pHeadVertex;
      }
      if (pHeadVertexNormal)
      {
         for (UINT i = 0; i < nCount; i++)
            pHeadVertexNormal[i].PreDestruct();
         delete [] pHeadVertexNormal;
      }
   }
}

void Patch::SetTessellationAllocationDirty( BOOL bFlag )
{
   m_bDirtyTessellationAllocation = bFlag;
   SetTessellationDirty( bFlag );

   GetModel()->FreeRTVertices();
   ClearTessellatedPolys();
}

void Patch::SetTessellationDirty( BOOL bFlag )
{
   if (m_bDirtyTessellation == bFlag || (bFlag && !IsInList()))
      return;

   m_bDirtyTessellation = bFlag;

   PatchModel *pModel = GetModel();
   ASSERT( pModel );

   if (bFlag)
   {
      m_pNextDirtyPatch = pModel->m_pDirtyPatchHead;
      m_pPrevDirtyPatch = NULL;
      if (m_pNextDirtyPatch)
         m_pNextDirtyPatch->m_pPrevDirtyPatch = this;
      pModel->m_pDirtyPatchHead = this;
   }
   else
   {
      if (m_pNextDirtyPatch)
         m_pNextDirtyPatch->m_pPrevDirtyPatch = m_pPrevDirtyPatch;

      if (m_pPrevDirtyPatch)
         m_pPrevDirtyPatch->m_pNextDirtyPatch = m_pNextDirtyPatch;
      else
         pModel->m_pDirtyPatchHead = m_pNextDirtyPatch;
   }
}

void Patch::Tessellate()
{
   if (!IsTessellationDirty())
      return;

   SetTessellationDirty( FALSE );

   // This code is for dealing with patches with no subdivisions
   PatchModel *pPatchModel = GetModel();
   if ((pPatchModel->m_nPatchSubdivisions.GetValue() == 0)&&(GetMaxSubdivision()==0))
   {
      if (m_bDirtyTessellationAllocation)
      {
         m_bDirtyTessellationAllocation = FALSE;
         TessellateNoSubdivisions();
      }
      return;
   }

   int nSubU = CalcUSubdivisions();
   int nSubV = CalcVSubdivisions();
   if (IsNPatch())
   {
      nSubU--;
      nSubV = nSubU;
   }
   int nUSubdivisions = 1 << nSubU;
   int nVSubdivisions = 1 << nSubV;

   // this line was added for variable tessellation
   if ((GetVertexMatrix(0)->GetNumRows()!=nUSubdivisions-1)
      &&(GetVertexMatrix(0)->GetNumCols()!=nVSubdivisions-1)
      &&(!m_bDirtyTessellationAllocation))
      m_bDirtyTessellationAllocation = TRUE;

   if (m_bDirtyTessellationAllocation)
   {
      m_bDirtyTessellationAllocation = FALSE;

      AllocateTessellationInternalEdges(nSubU);

      AllocateTessellation(nUSubdivisions-1,nVSubdivisions-1);

      if (IsQuadPatch())
         BuildQuadPatchTessPolys(nUSubdivisions,nVSubdivisions);
      else
         BuildNPatchTessPolys(nUSubdivisions,nVSubdivisions);
   }

   SetTessellationInternalEdges(nSubU);

   SetTessellation(nUSubdivisions,nVSubdivisions);

   for (PropertyPatch *pPropertyPatch=(PropertyPatch*)GetHeadPropertyPoly(); pPropertyPatch; pPropertyPatch=(PropertyPatch*)pPropertyPatch->GetNextPropertyPoly())
      pPropertyPatch->SetTessellation(nUSubdivisions,nVSubdivisions);
}

void Patch::TessellateWithSurrounding()
{
   for (UINT i=0; i < GetNumSides(); i++)
   {
      Vertex *pVertex = GetVertexAt(i);
      for (UINT j=0; j < pVertex->GetNumSurroundingPolys(); j++ )
      {
         Patch *pSurroundingPatch = (Patch *)pVertex->GetSurroundingPolyAt( j );
         pSurroundingPatch->Tessellate();
      }
   }
}

void Patch::TessellateNoSubdivisions()
{
   // ALLOCATE TESSELLATION
   UINT nTotalPatchTessSides = 0;
   for (UINT i = 0; i < GetNumSides(); i++)
      nTotalPatchTessSides += GetEdgeAt(i)->GetTessEdgeVertices(this)->GetNumVertices();

   nTotalPatchTessSides -= GetNumSides();

   TessPoly *pTessPoly = AddTessellatedPolys( 1 );
   pTessPoly->SetNumSides( nTotalPatchTessSides );

   TessEdgeVertices *pTessEdgeVertices;
   TessEdgeNormals *pTessEdgeNormals;

   UINT nSideIndex = 0;
   UINT nUVSideIndex = 0;
   for (UINT i = 0; i < GetNumSides(); i++)
   {
      //	Get tessellation of adjacent edge
      PatchEdge *pEdge = (PatchEdge *)GetEdgeAt(i);

      pTessEdgeVertices = pEdge->GetTessEdgeVertices(this);
      ASSERT( pTessEdgeVertices );
      pTessEdgeNormals = pEdge->GetTessEdgeNormals(this);
      ASSERT( pTessEdgeNormals );

      UINT nNumTessEdgeVerts = pTessEdgeVertices->GetNumVertices();

      for (UINT j = 0; j < nNumTessEdgeVerts-1; j++)
      {
         pTessPoly->SetVertexAt(nSideIndex, pTessEdgeVertices->GetVertexAt(j));
         pTessPoly->SetVertexNormalAt( nSideIndex, pTessEdgeNormals->GetVertexNormalAt(j));
         nSideIndex++;
      }

      TessPropertyPoly *pTessPropertyPoly;
      PropertyPoly *pPropertyPoly;

      UINT nOldUVSideIndex = nUVSideIndex;
      for ( pTessPropertyPoly = pTessPoly->GetHeadTessPropertyPoly(), pPropertyPoly = GetHeadPropertyPoly(); pTessPropertyPoly; pTessPropertyPoly=pTessPropertyPoly->GetNextTessPropertyPoly(), pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
      {
         nUVSideIndex = nOldUVSideIndex;

         PropertyPatchEdge *pPropertyEdge = (PropertyPatchEdge *)pPropertyPoly->GetPropertyEdgeAt( i );

         UINT nNumTessEdgePropertyVertices = pPropertyEdge->GetNumTessPropertyVertices();
         ASSERT( nNumTessEdgePropertyVertices == nNumTessEdgeVerts );
         
         BOOL bReversed = !pPropertyEdge->IsAligned(pPropertyPoly);

         for (UINT j = 0; j < nNumTessEdgePropertyVertices-1; j++) 
         {
            pTessPropertyPoly->SetPropertyVertexAt( nUVSideIndex, pPropertyEdge->GetTessPropertyVertexAt(j, bReversed));
            nUVSideIndex++;
         }
      }
   }

   pTessPoly->SetDirtyFaceNormal(TRUE);
}

void Patch::BuildQuadPatchTessPolys(int nUSubdivisions, int nVSubdivisions)
{
   AddTessellatedPolys( nUSubdivisions * nVSubdivisions );

   // build polygons
   for (int v = 0; v < nVSubdivisions; v++)
   {
      for (int u = 0; u < nUSubdivisions; u++)
      {
         UINT nIndex = v*nVSubdivisions + u;
         TessPoly *pTessPoly = &m_tesspolyarray[ nIndex ];
         BuildQuadTessPoly( pTessPoly, u, v, nUSubdivisions, nVSubdivisions );
         pTessPoly->SetDirtyFaceNormal(TRUE);
      }
   }
}

void Patch::BuildNPatchTessPolys(int nUSubdivisions, int nVSubdivisions)
{
   UINT nNumSubPatches = GetNumSubPatches();

   AddTessellatedPolys( nUSubdivisions * nVSubdivisions * nNumSubPatches );

   for (UINT nSubPatchIndex = 0; nSubPatchIndex < nNumSubPatches; nSubPatchIndex++)
   {
      for (int v = 0; v < nVSubdivisions; v++)
      {
         for (int u = 0; u < nUSubdivisions; u++)
         {
            TessPoly *pTessPoly = pTessPoly = &m_tesspolyarray[(nSubPatchIndex*nVSubdivisions*nUSubdivisions)+(v*nVSubdivisions + u)];
            BuildNPatchTessPoly( pTessPoly, nSubPatchIndex, u, v, nUSubdivisions, nVSubdivisions );
            pTessPoly->SetDirtyFaceNormal(TRUE);
         }
      }
   }
}

void Patch::AllocateTessellation(int nColumns,int nRows)
{
   PatchModel *pPatchModel = (PatchModel*)GetOwner();

   DynamicMatrix *pVMat;
   DynamicMatrix *pVNMat;

   PatchVertex *pVertex;
   VertexNormal *pVertexNormal;

   UINT nNumSubPatches = GetNumSubPatches();

#if 0
   if (( nColumns > 0 ) && ( nRows > 0 ))
   {
      // allocate internal vertices
      PatchVertex *pHeadVertex = pPatchModel->AddTessellatedVertices( nRows * nColumns * nNumSubPatches );
      ASSERT(pHeadVertex);
      pVertex = pHeadVertex;
      VertexNormal *pHeadVertexNormal = pPatchModel->AddTessellatedVertexNormals( nRows * nColumns * nNumSubPatches );
      ASSERT(pHeadVertexNormal);
      pVertexNormal = pHeadVertexNormal;
   }
#endif

   for (UINT nSubPatchIndex = 0; nSubPatchIndex < nNumSubPatches; nSubPatchIndex++)
   {
      pVMat = GetVertexMatrix(nSubPatchIndex);
      pVNMat = GetVertexNormalMatrix(nSubPatchIndex);

      // Initialize matricies
      pVMat->Initialize( nRows, nColumns );
      pVNMat->Initialize( nRows, nColumns );

      for (int nV = 0; nV < nRows; nV++)
      {
         for (int nU = 0; nU < nColumns; nU++)
         {
#if 0
            pVertex->Reference();
            pVertexNormal->Reference();
#else
            pVertex = pPatchModel->AddTessellatedVertex();
            pVertexNormal = pPatchModel->AddTessellatedVertexNormal();
#endif
            pVertex->SetUVWeight(nU,nV);//corner cases are inaccurate do to overwriting

            pVMat->Set( nV, nU, (UINT)pVertex);
            pVNMat->Set( nV, nU, (UINT)pVertexNormal );
         }
#if 0
         pVertex++;
         pVertexNormal++;
#endif
      }
   }
   for (PropertyPatch *pPropertyPatch = (PropertyPatch *)GetHeadPropertyPoly(); pPropertyPatch; pPropertyPatch=(PropertyPatch *)pPropertyPatch->GetNextPropertyPoly())
      pPropertyPatch->AllocateTessellation(nColumns,nRows);
}

void Patch::SetTessellation( int nUSubdivisions, int nVSubdivisions )
{
   for (UINT nSubPatchIndex = 0; nSubPatchIndex < GetNumSubPatches(); nSubPatchIndex++)
   {
      DynamicMatrix *pVMat = GetVertexMatrix(nSubPatchIndex);
      DynamicMatrix *pVNMat = GetVertexNormalMatrix(nSubPatchIndex);

      int nRows = pVMat->GetNumRows();
      int nCols = pVMat->GetNumCols();

      UINT nPTR;

      Vertex *pVertex;
      VertexNormal *pVertexNormal;
      for (int nV = 0; nV < nRows; nV++)
      {
         for (int nU = 0; nU < nCols; nU++)
         {
            float fS = (float)(nU+1)/(float)nUSubdivisions;
            float fT = (float)(nV+1)/(float)nVSubdivisions;

            pVMat->Get(nV,nU,nPTR);
            pVertex = (PatchVertex*)nPTR;
            Vector pos = CalculatePosition(nSubPatchIndex,fS,fT);
            pVertex->SetPos(pos);
            pVNMat->Get(nV,nU,nPTR);
            pVertexNormal = (VertexNormal*)nPTR;
            pVertexNormal->SetVertexNormalDirtyFlag(TRUE);
         }
      }
   }
}

void Patch::BuildTangents()
{
   if (!m_bDirtyTangents)
      return;

   m_bDirtyTangents = FALSE;// this prevents stack overflow

   if (IsQuadPatch())
      BuildQuadPatchTangents();
   else
      BuildNPatchTangents();
}

void Patch::BuildInteriorTangents(UINT nSubPatchIndex)
{
   Vector *pCVs = GetCVs(nSubPatchIndex);

   float twothirds = (2.0f/3.0f);
   float onethird = (1.0f/3.0f);

   Vector A1,A2,B1,B2,C1,C2,D1,D2;

   A1 = pCVs[4] - pCVs[0];
   A2 = pCVs[8] - pCVs[12];
   B1 = pCVs[13] - pCVs[12];
   B2 = pCVs[14] - pCVs[15];
   C1 = pCVs[11] - pCVs[15];
   C2 = pCVs[7] - pCVs[3];
   D1 = pCVs[2] - pCVs[3];
   D2 = pCVs[1] - pCVs[0];

   pCVs[5] = (pCVs[0] + twothirds*A1 + onethird*C2 + twothirds*D2 + onethird*B1);
   pCVs[9] = (pCVs[12] + twothirds*A2 + onethird*C1 + twothirds*B1 + onethird*D2);
   pCVs[10] = (pCVs[15] + twothirds*C1 + onethird*A2 + twothirds*B2 + onethird*D1);
   pCVs[6] = (pCVs[3] + twothirds*C2 + onethird*A1 + twothirds*D1 + onethird*B2);
}

void Patch::BuildQuadPatchTangents()
{
   UINT i;

   // build new vertex list without hooks
   UINT order[] = { 0,1,4, 12,8,13, 15,14,11, 3,7,2 };

   Vector *pCVs = GetCVs(0);
   Vector *pVNCVs = GetVNCVs(0);

   UINT nNumVerts = GetNumSides();

   Vertex *pVertex;
   VertexNormal *pVertexNormal;

   // fill cvs
   UINT nCount = 0;
   for (i = 0; i < nNumVerts; i++)
   {
      pVertex = GetVertexAt(i);
      pCVs[order[nCount*3]] = pVertex->GetPos();
      pVertex->GetTangents(this,pCVs[order[(nCount*3)+1]],pCVs[order[(nCount*3)+2]]);;

      pVertexNormal = GetVertexNormalAt(i);
      pVNCVs[order[nCount*3]] = pVertexNormal->GetDir();
      nCount++;
   }

   BuildInteriorTangents(0);
}

void Patch::BuildNPatchTangents()
{
   Vector center,centernormal;
   CalculateNPatchCenter(center,centernormal);

   for (UINT nSubPatchIndex = 0; nSubPatchIndex < GetNumSubPatches(); nSubPatchIndex++)
   {
      Vector a0,a1,a2,a3;
      Vector b0,b1,b2,b3;

      Vector cv0,cv1;
      Vector notused;

      UINT nPrevSubPatchIndex = GetPrevSubPatchIndex(nSubPatchIndex);

      Vertex *pV0 = GetVertexAt(nPrevSubPatchIndex);
      Vertex *pV1 = GetVertexAt(nSubPatchIndex);

      VertexNormal *pVN0 = GetVertexNormalAt(nPrevSubPatchIndex);
      VertexNormal *pVN1 = GetVertexNormalAt(nSubPatchIndex);

      pV0->GetTangents(this,notused,cv0);
      pV1->GetTangents(this,cv1,notused);

      // calc edge split
      MathTools::SplitBezierCurve(0.5f,pV0->GetPos(),cv0,cv1,pV1->GetPos(),a0,a1,a2,a3,b0,b1,b2,b3);

      Vector *pCVs = GetCVs(nSubPatchIndex);
      Vector *pVNCVs = GetVNCVs(nSubPatchIndex);

      pCVs[0] = center;
      pVNCVs[0] = centernormal;

      // figures out center to vertex tangent
      Vector outvect = b0-center;
      pCVs[4] = (centernormal ^ (outvect ^ centernormal))/3.0f + center;

      // figures split edge to center tangent
      Vector invect = center-b0;

      Vertex *pVertex0 = GetVertexAt(nPrevSubPatchIndex);
      Vector vn0 = GetVertexNormalAt(nPrevSubPatchIndex)->GetDir();
      Vertex *pVertex1 = GetVertexAt(nSubPatchIndex);
      Vector vn1 = GetVertexNormalAt(nSubPatchIndex)->GetDir();

      Vector vn = (vn0 + vn1) * 0.5f;

      pCVs[8] = (vn ^ (invect ^ vn))/3.0f + b0;
      pCVs[12] = b0;
      pCVs[13] = b1;
      pCVs[14] = b2;
      pCVs[15] = b3;
      pVNCVs[12] = (pVN0->GetDir()+pVN1->GetDir())*0.5;
      pVNCVs[15] = pVN1->GetDir();

      Vector2 uv0, uv1, v2notused;
      Vector2 c0, d0, d3;
      Vector2 cuv0,cuv1;

      UINT nNextSubPatchIndex = GetNextSubPatchIndex(nSubPatchIndex);

      pV0 = GetVertexAt(nSubPatchIndex);
      pV1 = GetVertexAt(nNextSubPatchIndex);

      pVN0 = GetVertexNormalAt(nSubPatchIndex);
      pVN1 = GetVertexNormalAt(nNextSubPatchIndex);

      pV0->GetTangents(this,notused,cv0);
      pV1->GetTangents(this,cv1,notused);

      MathTools::SplitBezierCurve(0.5f,pV0->GetPos(),cv0,cv1,pV1->GetPos(),a0,a1,a2,a3,b0,b1,b2,b3);

      pCVs[11] = a1;
      pCVs[7] = a2;
      pCVs[3] = a3;
      pVNCVs[3] = (pVN0->GetDir()+pVN1->GetDir())*0.5;

      // figures split edge to center tangent
      invect = center-a3;
      pVertex0 = GetVertexAt(nSubPatchIndex);
      vn0 = GetVertexNormalAt(nSubPatchIndex)->GetDir();
      pVertex1 = GetVertexAt(nNextSubPatchIndex);
      vn1 = GetVertexNormalAt(nNextSubPatchIndex)->GetDir();

      vn = (vn0 + vn1) *0.5f;

      pCVs[2] = (vn ^ (invect ^ vn))/3.0f + a3;

      // figures out center to vertex tangent
      outvect = a3-center;
      pCVs[1] = (centernormal ^ (outvect ^ centernormal))/3.0f + center;

      BuildInteriorTangents(nSubPatchIndex);
   }
}

void Patch::FillOutVerticesAndVertexNormalsTessPoly(
   TessPoly *pPoly,
   UINT nSubPatchIndex,
   int u, int v,
   int usubdivisions, int vsubdivisions,
   TessEdgeVertices *pLeftTessEdgeVertices,
   TessEdgeVertices *pBottomTessEdgeVertices,
   TessEdgeVertices *pRightTessEdgeVertices,
   TessEdgeVertices *pTopTessEdgeVertices,
   TessEdgeNormals *pLeftTessEdgeNormals,
   TessEdgeNormals *pBottomTessEdgeNormals,
   TessEdgeNormals *pRightTessEdgeNormals,
   TessEdgeNormals *pTopTessEdgeNormals)
{
   DynamicMatrix *vmat = GetVertexMatrix(nSubPatchIndex);
   DynamicMatrix *vnmat = GetVertexNormalMatrix(nSubPatchIndex);

   BOOL bFlag0 = 0, bFlag1 = 0, bFlag2 = 0, bFlag3 = 0;

   int usubdivisions2 = usubdivisions + 1;
   int vsubdivisions2 = vsubdivisions + 1;

   int corner0,corner1,corner2,corner3;

   corner0 = ((v-1) * (usubdivisions-1)) + (u-1);
   corner1 = (v * (usubdivisions-1)) + (u-1);
   corner2 = (v * (usubdivisions-1)) + u;
   corner3 = ((v-1) * (usubdivisions-1)) + u;

   UINT vi[4],vni[4];
   int n0;

   PatchVertex *pV0 = NULL;
   PatchVertex *pV1 = NULL;
   PatchVertex *pV2 = NULL;
   PatchVertex *pV3 = NULL;

   // top left corner
   if ((u!=0)&&(v!=0))
   {
      vmat->Get(corner0,vi[0]);
      vnmat->Get(corner0,vni[0]);
      pV0 = (PatchVertex*)vi[0];
   }

   // bottom left corner
   if ((u!=0)&&(v!=vsubdivisions-1))
   {
      vmat->Get(corner1,vi[1]);
      vnmat->Get(corner1,vni[1]);
      pV1 = (PatchVertex*)vi[1];
   }

   // bottom right corner
   if ((u!=usubdivisions-1)&&(v!=vsubdivisions-1))
   {
      vmat->Get(corner2,vi[2]);
      vnmat->Get(corner2,vni[2]);
      pV2 = (PatchVertex*)vi[2];
   }

   // top right corner
   if ((u!=usubdivisions-1)&&(v!=0))
   {
      vmat->Get(corner3,vi[3]);
      vnmat->Get(corner3,vni[3]);
      pV3 = (PatchVertex*)vi[3];
   }

 
   UINT vcount = 0, vncount = 0;

   int i;

   PatchVertex *pVertex;
   VertexNormal *pVertexNormal;

   if (v == 0)
      bFlag0 = TRUE;

   // fix cracks at left edge
   if (u == 0)
   {
      n0 = 0;
      if (bFlag0)
         n0 = 1;
      int factor = ((int)pLeftTessEdgeVertices->GetNumVertices()-1)/(vsubdivisions);
      for (i = (v*factor)+n0; i < (v*factor)+factor+1; i++)
      {
         pVertex = pLeftTessEdgeVertices->GetVertexAt(i);
         ASSERT(pVertex);

//         pVertex->SetBorderFlag(TRUE);
         pPoly->SetVertexAt( vcount++, pVertex );

         pVertexNormal = pLeftTessEdgeNormals->GetVertexNormalAt(i);
         ASSERT(pVertexNormal);
         pPoly->SetVertexNormalAt( vncount++, pVertexNormal );
      }
      bFlag1 = TRUE;
   }
   else
   {
      if (!bFlag0)
      {
         // top left corner
         pVertex = pV0;
         ASSERT(pVertex);
//         if (u == 0)
//            pVertex->SetBorderFlag(TRUE);
         pVertexNormal = (VertexNormal*)vni[0];
         ASSERT(pVertexNormal);
         pPoly->SetVertexAt(vcount++,pVertex);
         pPoly->SetVertexNormalAt(vncount++,pVertexNormal);
      }
   }

   // fix cracks at bottom edge
   if (v == vsubdivisions-1)
   {
      n0 = 0;
      if (bFlag1)
         n0 = 1;
      int factor = ((int)pBottomTessEdgeVertices->GetNumVertices()-1)/(usubdivisions);
      for (i = (u*factor)+n0; i < (u*factor)+factor+1; i++)
      {
         pVertex = pBottomTessEdgeVertices->GetVertexAt(i);
         ASSERT(pVertex);
//         pVertex->SetBorderFlag(TRUE);
         pPoly->SetVertexAt( vcount++, pVertex );
         pVertexNormal = pBottomTessEdgeNormals->GetVertexNormalAt(i);
         ASSERT(pVertexNormal);
         pPoly->SetVertexNormalAt( vncount++, pVertexNormal );

      }	
      bFlag2 = TRUE;
   }
   else
   {
      if (!bFlag1)
      {
         // bottom left corner
         pVertex = pV1;
         ASSERT(pVertex);

//         if (v == vsubdivisions-1)	
//            pVertex->SetBorderFlag(TRUE);

         pVertexNormal = (VertexNormal *)vni[1];
         ASSERT(pVertexNormal);

         pPoly->SetVertexAt( vcount++, pVertex);
         pPoly->SetVertexNormalAt( vncount++, pVertexNormal);
      }
   }

   // fix cracks at right edge
   if (u == usubdivisions-1)
   {
      n0 = 0;
      if (bFlag2)
         n0 = 1;
      int factor = ((int)pRightTessEdgeVertices->GetNumVertices()-1)/(vsubdivisions);
      for (i = ((vsubdivisions-v-1)*factor)+n0; i < ((vsubdivisions-v-1)*factor)+factor+1; i++)
      {
         pVertex = pRightTessEdgeVertices->GetVertexAt(i);
         ASSERT(pVertex);

//         pVertex->SetBorderFlag( TRUE );
         pPoly->SetVertexAt( vcount++, pVertex );

         pVertexNormal = pRightTessEdgeNormals->GetVertexNormalAt(i);
         ASSERT(pVertexNormal);
         pPoly->SetVertexNormalAt( vncount++, pVertexNormal );
      }
      bFlag3 = TRUE;
   }
   else
   {
      if (!bFlag2)
      {
         // bottom right corner
         pVertex = pV2;
         ASSERT(pVertex);
         
//         if (u == usubdivisions-1)
//            pVertex->SetBorderFlag(TRUE);

         pVertexNormal = (VertexNormal*)vni[2];
         ASSERT(pVertexNormal);

         pPoly->SetVertexAt(vcount++, pVertex);
         pPoly->SetVertexNormalAt(vncount++, pVertexNormal);
      }
   }

   // fix cracks at top edge
   if (v == 0)
   {
      n0 = 0;
      if (bFlag3)
         n0 = 1;
      int factor = ((int)pTopTessEdgeVertices->GetNumVertices()-1)/(usubdivisions);
      for (i = ((usubdivisions-u-1)*factor)+n0; i < ((usubdivisions-u-1)*factor)+factor+1; i++)				
      {
         pVertex = pTopTessEdgeVertices->GetVertexAt(i);
         ASSERT(pVertex);

//         pVertex->SetBorderFlag(TRUE);
         pPoly->SetVertexAt(vcount++, pVertex);

         pVertexNormal = pTopTessEdgeNormals->GetVertexNormalAt(i);
         ASSERT(pVertexNormal);

         pPoly->SetVertexNormalAt(vncount++, pVertexNormal);
      }
   }
   else
   {
      // top right corner
      if (!bFlag3)
      {
         pVertex = pV3;
         ASSERT(pVertex);
//         if (v == 0)
//            pVertex->SetBorderFlag(TRUE);
         pVertexNormal = (VertexNormal*)vni[3];
         ASSERT(pVertexNormal);

         pPoly->SetVertexAt(vcount++,pVertex);
         pPoly->SetVertexNormalAt(vncount++,pVertexNormal);
      }
   }

   ASSERT(vcount>=4);// something's gone horribly wrong!
}

void Patch::BuildQuadTessPoly(TessPoly *pTessPoly,int u, int v,int usubdivisions, int vsubdivisions)
{
   pTessPoly->SetNumSides(4);

   PatchEdge *pLeftEdge = GetEdgeAt(0);
   PatchEdge *pBottomEdge = GetEdgeAt(1);
   PatchEdge *pRightEdge = GetEdgeAt(2);
   PatchEdge *pTopEdge = GetEdgeAt(3);

   FillOutVerticesAndVertexNormalsTessPoly( pTessPoly, 0, u, v, usubdivisions, vsubdivisions,
      pLeftEdge->GetTessEdgeVertices(this), pBottomEdge->GetTessEdgeVertices(this), pRightEdge->GetTessEdgeVertices(this), pTopEdge->GetTessEdgeVertices(this),
      pLeftEdge->GetTessEdgeNormals(this),  pBottomEdge->GetTessEdgeNormals(this),  pRightEdge->GetTessEdgeNormals(this),  pTopEdge->GetTessEdgeNormals(this) );

   for (PropertyPatch *pPropertyPatch = (PropertyPatch*)GetHeadPropertyPoly(); pPropertyPatch; pPropertyPatch=(PropertyPatch*)pPropertyPatch->GetNextPropertyPoly())
      pPropertyPatch->BuildQuadPatchTessPropertyPoly(pTessPoly,u,v,usubdivisions,vsubdivisions);
// Getting tessellated edge components may cause edge retessellation of edge therefore dirtying this patch.
   SetTessellationDirty(FALSE);
}

void Patch::BuildNPatchTessPoly( TessPoly *pTessPoly, UINT nSubPatchIndex, int u, int v, int usubdivisions, int vsubdivisions )
{
   pTessPoly->SetNumSides(4);

   UINT nNumSubPatches = GetNumSubPatches();

   UINT nPreviousSubPatchIndex = GetPrevSubPatchIndex(nSubPatchIndex);

   PatchEdge *pBottomEdge = GetEdgeAt(nPreviousSubPatchIndex);
   PatchEdge *pRightEdge = GetEdgeAt(nSubPatchIndex);

   //Left
   TessEdgeVertices lefttessedgevertices;
   m_pNPatchData->GetTessEdgeVertices(nPreviousSubPatchIndex)->Reverse(lefttessedgevertices);
   TessEdgeNormals lefttessedgenormals;
   m_pNPatchData->GetTessEdgeNormals(nPreviousSubPatchIndex)->Reverse(lefttessedgenormals);

   //Top
   TessEdgeVertices *pTopTessEdgeVertices = m_pNPatchData->GetTessEdgeVertices(nSubPatchIndex);
   TessEdgeNormals *pTopTessEdgeNormals = m_pNPatchData->GetTessEdgeNormals(nSubPatchIndex);

   //Right
   TessEdgeVertices righttessedgevertices;
   pRightEdge->GetTessEdgeVertices(this)->GetFirstHalf(righttessedgevertices);
   TessEdgeNormals righttessedgenormals;
   pRightEdge->GetTessEdgeNormals(this)->GetFirstHalf(righttessedgenormals);

   //Bottom
   TessEdgeVertices bottomtessedgevertices;
   pBottomEdge->GetTessEdgeVertices(this)->GetSecondHalf(bottomtessedgevertices);
   TessEdgeNormals bottomtessedgenormals;
   pBottomEdge->GetTessEdgeNormals(this)->GetSecondHalf(bottomtessedgenormals);

   FillOutVerticesAndVertexNormalsTessPoly( pTessPoly, nSubPatchIndex, u, v, usubdivisions, vsubdivisions,
    &lefttessedgevertices, &bottomtessedgevertices, &righttessedgevertices, pTopTessEdgeVertices,
    &lefttessedgenormals,  &bottomtessedgenormals,  &righttessedgenormals,  pTopTessEdgeNormals);

   for (PropertyPatch *pPropertyPatch = (PropertyPatch*)GetHeadPropertyPoly(); pPropertyPatch; pPropertyPatch=(PropertyPatch*)pPropertyPatch->GetNextPropertyPoly())
      pPropertyPatch->BuildNPatchTessPropertyPoly( pTessPoly, nPreviousSubPatchIndex, nSubPatchIndex, u, v, usubdivisions, vsubdivisions );

// Getting tessellated edge components may cause edge retessellation of edge therefore dirtying this patch.
   SetTessellationDirty(FALSE);
}

void Patch::DrawPickNearest( Edge *pInEdge )
{
   PatchEdge *pEdge = (PatchEdge *)pInEdge;

   Vector vCenter(0.0f);
   for (UINT i=0; i<GetNumSides();i++)
      vCenter += GetVertexAt(i)->GetPos();
   vCenter /= GetNumSides();

   BOOL bOldCullFace = glSet( GL_CULL_FACE, FALSE);

   TessEdgeVertices *pTessVerts = pEdge->GetTessEdgeVertices(this);
   ASSERT(pTessVerts);

   glBegin( GL_POLYGON );
      for (UINT i=0; i<pTessVerts->GetNumVertices(); i++)
      {
         PatchVertex *pVertex = pTessVerts->GetVertexAt(i);
         Vector vVertPos = pVertex->GetPos();
         glVertex3fv(&vVertPos.x);
      }
      glVertex3fv(&vCenter.x);
   glEnd();

   glSet( GL_CULL_FACE, bOldCullFace );
}

void Patch::Draw( const Matrix44 &viewmatrix, BOOL bDrawTess, SurfaceDriver *pSurfaceDriver/*=NULL*/)
{
   if (!bDrawTess)
   {
      Poly::Draw( viewmatrix, bDrawTess, pSurfaceDriver );
      return;
   }

   for (UINT i=0; i<GetNumTessPolys(); i++)
   {
      TessPoly *pTessPoly = GetTessPolyAt(i);

      LPARAM lParam = 0L;
      if (pSurfaceDriver && !pSurfaceDriver->IsValid( pTessPoly, lParam ))
         return;

      UINT nNumVerts = pTessPoly->GetNumSides();
      glBegin( nNumVerts==4 ? GL_QUADS : GL_POLYGON );
      for (UINT i = 0; i < nNumVerts; i++)
      {     
         if (pSurfaceDriver)
         {
            Vector2 uv = pSurfaceDriver->GetUVAt( viewmatrix, i, pTessPoly, lParam );
            glTexCoord2f( uv.x, uv.y );
         }
         glNormal3fv( &pTessPoly->GetVertexNormalAt(i)->GetDir().x );
         glVertex3fv( &pTessPoly->GetVertexAt(i)->GetPos().x );
 
      }
      glEnd();
   }
}

void Patch::Disconnect()
{
   DirtyDependentFaces();
   ClearTessellatedPolys();
   SetTessellationDirty( FALSE );
   Poly::Disconnect();
}

void Patch::Reconnect()
{
   Poly::Reconnect();
   DirtyDependentFaces();
   DirtyTessellationAllocation();
}

void Patch::SetVertexAt( UINT nIndex, Vertex *pVertex )
{
  SetTessellationAllocationDirty( TRUE );
  Poly::SetVertexAt( nIndex, pVertex );
}

BOOL Patch::SetEdgeAt( UINT nIndex, Edge *pEdge )
{
   if (pEdge)
      ((PatchEdge *)pEdge)->SetTessellationAllocationDirty(TRUE);
   return Poly::SetEdgeAt( nIndex, pEdge );
}

void Patch::SetVertexNormalAt( UINT nIndex, VertexNormal *pVertexNormal )
{
   DirtyTessellationAllocation();
   Poly::SetVertexNormalAt( nIndex, pVertexNormal );
}

void Patch::ReverseWinding()
{
   Poly::ReverseWinding();
   SetTessellationAllocationDirty( TRUE );
}

void Patch::AllocateTessellationInternalEdges( int nSub )
{
   if (!m_pNPatchData)
      return;

   if (m_pNPatchData->GetNumSubPatchEdges() != GetNumSubPatches())
      m_pNPatchData->SetNumSubPatchEdges(GetNumSubPatches());

   for (PropertyPatch *pPropertyPatch = (PropertyPatch *)GetHeadPropertyPoly(); pPropertyPatch; pPropertyPatch=(PropertyPatch *)pPropertyPatch->GetNextPropertyPoly())
      pPropertyPatch->SetNumInternalTessPropertyEdges(GetNumSubPatches());

   PatchModel *pPatchModel = (PatchModel*)GetOwner();

   PatchVertex *pCenterVertex = pPatchModel->AddTessellatedVertex();

   VertexNormal *pCenterVertexNormal = VertexNormal::Create();
   pCenterVertexNormal->SetVertexNormalDirtyFlag(TRUE);

   UINT nNPatchHookIndex;

   UINT nNumPropertyPolys = CountNumPropertyPolys();
   PropertyVertex **pCenterPropertyVertices = SNEW PropertyVertex*[nNumPropertyPolys];
   memset(pCenterPropertyVertices,0,sizeof(PropertyVertex*)*nNumPropertyPolys);

   // get edge center vertex
   for (UINT nSubPatchIndex = 0; nSubPatchIndex < m_pNPatchData->GetNumSubPatchEdges(); nSubPatchIndex++)
   {
      PatchEdge *pEdge = GetEdgeAt(nSubPatchIndex);

      TessEdgeVertices *pTessEdgeVertices = pEdge->GetTessEdgeVertices(this);
      TessEdgeNormals *pTessEdgeNormals = pEdge->GetTessEdgeNormals(this);

      nNPatchHookIndex = pTessEdgeVertices->FindNPatchEdgeHook();
      PatchVertex *pEdgeCenterVertex = pTessEdgeVertices->GetVertexAt(nNPatchHookIndex);

      VertexNormal *pEdgeCenterVertexNormal = pTessEdgeNormals->GetVertexNormalAt(nNPatchHookIndex);
      pEdgeCenterVertexNormal->SetVertexNormalDirtyFlag(TRUE);

      AllocateTessellationInternalEdge( nSub,nSubPatchIndex, pEdgeCenterVertex, pCenterVertex, pEdgeCenterVertexNormal, pCenterVertexNormal );

      UINT nCount = 0;
      for (PropertyPatch *pPropertyPatch = (PropertyPatch *)GetHeadPropertyPoly(); pPropertyPatch; pPropertyPatch=(PropertyPatch *)pPropertyPatch->GetNextPropertyPoly(),nCount++)
         pPropertyPatch->AllocateTessellationInternalEdge(nSub,&pCenterPropertyVertices[nCount],nSubPatchIndex,nNPatchHookIndex);
   }
   for (UINT i = 0; i < nNumPropertyPolys; i++)
   {
      if (pCenterPropertyVertices[i])
         pCenterPropertyVertices[i]->SetVisible(FALSE);//hide center vertex
   }

   delete [] pCenterPropertyVertices;
}

void Patch::AllocateTessellationInternalEdge( int nSub,UINT nSubPatchIndex,
                                              PatchVertex *pEdgeCenterVertex,PatchVertex *pCenterVertex,
                                              VertexNormal *pEdgeCenterVertexNormal,VertexNormal *pCenterVertexNormal )
{
   PatchModel *pPatchModel = (PatchModel*)GetOwner();

//   int nSub = CalcUSubdivisions();
//   if (nSub==0)
//      return;

//   int nNumSteps = 1 << (nSub-1);
   int nNumSteps = 1 << nSub;//force all internal edges to the same number of subdivisions

   SubPatchEdge *pSubPatchEdge = m_pNPatchData->GetSubPatchEdgeAt(nSubPatchIndex);

   TessEdgeVertices *pTessEdgeVertices = pSubPatchEdge->GetTessEdgeVertices();
   TessEdgeNormals *pTessEdgeNormals = pSubPatchEdge->GetTessEdgeNormals();

   pTessEdgeVertices->SetNumVertices(nNumSteps+1);
   pTessEdgeNormals->SetNumVertexNormals(nNumSteps+1);

   for (int i=0; i<=nNumSteps; i++)
   {
      if (i==0)
      {
         pTessEdgeVertices->SetVertexAt(i, pEdgeCenterVertex);
         pTessEdgeNormals->SetVertexNormalAt(i, pEdgeCenterVertexNormal);
      }
      else if (i==nNumSteps)
      {
         pTessEdgeVertices->SetVertexAt(i, pCenterVertex);
         pTessEdgeNormals->SetVertexNormalAt(i, pCenterVertexNormal);
      }
      else
      {
         pTessEdgeVertices->SetVertexAt(i, pPatchModel->AddTessellatedVertex());
         pTessEdgeNormals->SetVertexNormalAt(i, VertexNormal::Create());
      }
   }
}

void Patch::SetTessellationInternalEdges(int nSub)
{
   if (!m_pNPatchData)
      return;

   for (UINT nSubPatchIndex = 0; nSubPatchIndex < GetNumSubPatches(); nSubPatchIndex++)
   {
      SetTessellationInternalEdge(nSub, nSubPatchIndex);
      for (PropertyPatch *pPropertyPatch = (PropertyPatch*)GetHeadPropertyPoly(); pPropertyPatch; pPropertyPatch = (PropertyPatch*)pPropertyPatch->GetNextPropertyPoly())
         pPropertyPatch->SetTessellationInternalEdge(nSub, nSubPatchIndex);
   }
}

void Patch::SetTessellationInternalEdge(int nSub,UINT nSubPatchIndex)
{
//   int nSub = CalcUSubdivisions();
//   if (nSub==0)
//      return;
//   int nNumSteps = 1 << (nSub-1);

   int nNumSteps = 1 << nSub;//force all internal edges to the same number of subdivisions

   SubPatchEdge *pSubPatchEdge = m_pNPatchData->GetSubPatchEdgeAt(nSubPatchIndex);

   TessEdgeVertices *pTessEdgeVertices = pSubPatchEdge->GetTessEdgeVertices();
   TessEdgeNormals *pTessEdgeNormals = pSubPatchEdge->GetTessEdgeNormals();

   Vector *pCVs = GetCVs(nSubPatchIndex);

   // set edge centers
   pTessEdgeVertices->GetVertexAt(0)->SetPos(pCVs[3]);
   pTessEdgeNormals->GetVertexNormalAt(0)->SetVertexNormalDirtyFlag(TRUE);

   Vector v;
   double inc = 1.0 / nNumSteps;
   int i=0;
   double t;
   for (t=inc, i=1; i<nNumSteps; t+=inc, i++)
   {
      v = MathTools::Bezier(t,pCVs[3],pCVs[2],pCVs[1],pCVs[0]);
      pTessEdgeVertices->GetVertexAt(i)->SetPos(v);
      pTessEdgeNormals->GetVertexNormalAt(i)->SetVertexNormalDirtyFlag(TRUE);
   }

   // patch center
   pTessEdgeVertices->GetVertexAt(pTessEdgeVertices->GetNumVertices()-1)->SetPos(pCVs[0]);
   pTessEdgeNormals->GetVertexNormalAt(pTessEdgeNormals->GetNumVertexNormals()-1)->SetVertexNormalDirtyFlag(TRUE);
}

void Patch::CalculateNPatchCenter(Vector &center,Vector &centernormal)
{
   UINT nNumVerts = GetNumSides();

   CalcCenter(center);

   UINT i;

   //	move center
   Vector sum(0,0,0);
   for (i = 0; i < nNumVerts; i++)
   {
      Vector vn = GetVertexNormalAt(i)->GetDir();
      Vector invect = GetVertexAt(i)->GetPos() - center;
      float len = invect.Length() * 0.4f;//center raise or lower factor
      invect.Normalize();
      len *= invect.Dot(vn);
      Vector tempn = *GetFaceNormal() * len;
      sum += tempn;
   }
   sum/=nNumVerts;
   center += sum;

   centernormal = *GetFaceNormal();
}

void Patch::DrawInternalEdges()
{
/*

   if (!m_pNPatchData)
      return;

   for (UINT i = 0; i < m_pNPatchData->GetNumSubPatchEdges(); i++)
   {
      SubPatchEdge *pSubPatchEdge = m_pNPatchData->GetSubPatchEdgeAt(i);

      TessEdgeVertices *pTessEdgeVertices = pSubPatchEdge->GetTessEdgeVertices();

      UINT nNumVerts = pTessEdgeVertices->GetNumVertices();
      if (nNumVerts==0)
         return;

      glBegin(GL_POINTS);
      glVertex3fv(&pTessEdgeVertices->GetVertexAt(0)->GetPos().x);
 //     glVertex3fv(&pTessEdgeVertices->GetVertexAt(nNumVerts-1)->GetPos().x);
      glEnd();

      glBegin(GL_LINES);

      for (UINT i = 0; i < nNumVerts-1; i++)
      {
         Vertex *pVertex = pTessEdgeVertices->GetVertexAt(i);
         ASSERT(pVertex);
         glVertex3fv( &pVertex->GetPos().x );
         pVertex = (Vertex*)pTessEdgeVertices->GetVertexAt(i+1);
         ASSERT(pVertex);
         glVertex3fv( &pVertex->GetPos().x );
      }
      glEnd();
   }
*/
}

void Patch::FlipNormal()
{
   Poly::FlipNormal();
   DirtyTessellationAllocation();
   SetTangentsDirty(TRUE);
}

void Patch::DirtyTessellationAllocation()
{
   m_bDirtyTessellationAllocation = TRUE;
   SetTessellationDirty( TRUE );

   for (UINT i = 0; i < GetNumSides();  i++)
   {
      PatchEdge *pEdge = GetEdgeAt(i);
      if (pEdge)
         pEdge->SetTessellationAllocationDirty(TRUE);
   }

   ClearTessellatedPolys();

   GetModel()->FreeRTVertices();
}

Vector Patch::CalculatePosition(UINT nSubPatchIndex, float fS, float fT)
{
   Vector *pCVs = GetCVs(nSubPatchIndex);

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

/*
Vector Patch::CalculateNormal(UINT nSubPatchIndex, float fS, float fT)
{
   Vector *pCVs = GetCVs(nSubPatchIndex);

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

   //   float S[16] = {fS3,fS2,fS,1, 0,1,0,0, 0,0,1,0, 0,0,0,1};
   //   float dS[16] = {3*fS2,2*fS,1,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
   //   float T[16] = {fT3,0,0,0, fT2,1,0,0, fT,0,1,0, 1,0,0,1};
   //   float dT[16] = {3*fT2,0,0,0, 2*fT,1,0,0, 1,0,1,0, 0,0,0,1};
   float S[16] = {fS3,0,0,0, fS2,1,0,0, fS,0,1,0, 1,0,0,1};
   float dS[16] = {3*fS2,0,0,0, 2*fS,1,0,0, 1,0,1,0, 0,0,0,1};
   float T[16] = {fT3,fT2,fT,1, 0,1,0,0, 0,0,1,0, 0,0,0,1};
   float dT[16] = {3*fT2,2*fT2,1,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

   Matrix44 patchx(fPatchx), patchy(fPatchy), patchz(fPatchz),s(S),t(T),bezierbasis(fBezierbasis),ds(dS),dt(dT);

   //   Matrix44 tempads = ds*bezierbasis;
   //   Matrix44 tempbds = bezierbasis*dt;
   //   Matrix44 tempadt = ds*bezierbasis;
   //   Matrix44 tempbdt = bezierbasis*dt;

   Matrix44 tempads = bezierbasis*ds;
   Matrix44 tempbt = t*bezierbasis;
   Matrix44 tempas = bezierbasis*s;
   Matrix44 tempbdt = dt*bezierbasis;

   Matrix44 resultxS = tempbt*patchx*tempads;
   Matrix44 resultyS = tempbt*patchy*tempads;
   Matrix44 resultzS = tempbt*patchz*tempads;
   Matrix44 resultxT = tempbdt*patchx*tempas;
   Matrix44 resultyT = tempbdt*patchy*tempas;
   Matrix44 resultzT = tempbdt*patchz*tempas;

   Vector norm;

   norm.x = ((resultyT.Get(0,0)*resultzS.Get(0,0))-(resultyS.Get(0,0)*resultzT.Get(0,0)));
   norm.y = ((resultzT.Get(0,0)*resultxS.Get(0,0))-(resultzS.Get(0,0)*resultxT.Get(0,0)));
   norm.z = ((resultxT.Get(0,0)*resultyS.Get(0,0))-(resultxS.Get(0,0)*resultyT.Get(0,0)));

   norm.Normalize();

   return norm;
}
*/
////////////////////////////////////////////////////////////////////////////////////
// Property Poly

void Patch::AddPropertyPoly( PropertyPoly *pPropertyPoly )
{
   Poly::AddPropertyPoly( pPropertyPoly );
   PropertyPatch *pPropertyPatch = (PropertyPatch *)pPropertyPoly;
   UINT nNumSubPatches = GetNumSides();
   if (IsQuadPatch())
      nNumSubPatches = 1;
   pPropertyPatch->SetNumBasisMatrices(nNumSubPatches);
   DirtyTessellationAllocation();
   SetDirty();
}

BOOL Patch::RemovePropertyPoly( PropertyPoly *pPropertyPoly )
{
   DirtyTessellationAllocation();
   SetDirty();
   return Poly::RemovePropertyPoly( pPropertyPoly );
}

/////////////////////////////////////////////////////////////////////////////////////

NPatchData::~NPatchData()
{
   if (m_pSubPatchEdges)
      delete [] m_pSubPatchEdges;
}

void NPatchData::SetNumSubPatchEdges(UINT nNumEdges)
{
   if (nNumEdges == m_nNumSubPatches)
      return;

   if (nNumEdges==0)
   {
      delete [] m_pSubPatchEdges;
      m_pSubPatchEdges = NULL;
      return;
   }

   if (nNumEdges > m_nNumSubPatches)
   {
      if (m_pSubPatchEdges)
         delete [] m_pSubPatchEdges;

      m_pSubPatchEdges = SNEW SubPatchEdge[nNumEdges];
   }

   m_nNumSubPatches = nNumEdges;
}

