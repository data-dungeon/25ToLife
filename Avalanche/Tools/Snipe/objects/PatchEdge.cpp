#include "stdafx.h"
#include "PatchEdge.h"
#include "Patch.h"
#include "PatchModel.h"
#include "PatchVertex.h"
#include "UVPatchEdge.h"
#include "GUI\GLBaseView.h"
#include "GUI\GLModelView.h"

Property::Info PatchEdge::m_firstVertexHandleInfo( "FirstBias", "FirstBias" );
Property::Info PatchEdge::m_secondVertexHandleInfo( "SecondBias", "SecondBias" );
 FloatProperty::Info VertexHandle::m_fMagnitudeInfo( "Magnitude", "Magnitude", 1.0f, 0.0f, 200.0f);
 FloatProperty::Info VertexHandle::m_fAlphaInfo( "Alpha", "Alpha", 0.0f, 0.0f, 90.0f);
 FloatProperty::Info VertexHandle::m_fGammaInfo( "Gamma", "Gamma", 0.0f, 0.0f, 90.0f);

VertexHandle::VertexHandle()
{
}

VertexHandle::VertexHandle(const VertexHandle &other)
{
   m_vTangent = other.m_vTangent;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_COMPONENT_CREATE( PatchEdge, Edge )

PatchEdge::PatchEdge()
{
   Init();
}

PatchEdge::PatchEdge( UINT nID )
: Edge( nID )
{
   Init();
}

PatchEdge::~PatchEdge()
{
}

void PatchEdge::Init()
{
   m_pNextDirtyDangle = NULL;
   m_pPrevDirtyDangle = NULL;
   m_pPrevContEdge = NULL;
   m_pNextContEdge = NULL;

   m_firstVertexHandle.Init( &m_firstVertexHandleInfo, this );
   m_firstVertexHandle.m_fMagnitude.Init(&VertexHandle::m_fMagnitudeInfo,&m_firstVertexHandle);
   m_firstVertexHandle.m_fAlpha.Init(&VertexHandle::m_fAlphaInfo,&m_firstVertexHandle);
   m_firstVertexHandle.m_fGamma.Init(&VertexHandle::m_fGammaInfo,&m_firstVertexHandle);
   m_secondVertexHandle.Init( &m_secondVertexHandleInfo, this );
   m_secondVertexHandle.m_fMagnitude.Init(&VertexHandle::m_fMagnitudeInfo,&m_secondVertexHandle);
   m_secondVertexHandle.m_fAlpha.Init(&VertexHandle::m_fAlphaInfo,&m_secondVertexHandle);
   m_secondVertexHandle.m_fGamma.Init(&VertexHandle::m_fGammaInfo,&m_secondVertexHandle);

   m_firstVertexHandle.SetVisible(FALSE);
   m_secondVertexHandle.SetVisible(FALSE);

   SetTessellationDirty(TRUE);
}

UINT PatchEdge::GetSaveFlags()
{
   UINT nFlags = Edge::GetSaveFlags();

   if (IsContinuityLoop())
      nFlags |= SF_LOOP;

   if (IsFirstVertexConstrained())
      nFlags |= SF_V0CONSTRAINED;
   if (IsSecondVertexConstrained())
      nFlags |= SF_V1CONSTRAINED;

   VertexHandle *pFirstVertexHandle = GetFirstVertexHandle(FALSE);
   if (pFirstVertexHandle->m_fAlpha!=0.0f || pFirstVertexHandle->m_fGamma!=0.0f || pFirstVertexHandle->m_fMagnitude!=1.0f)
      nFlags |= SF_V0BIAS;

   // don't write out bias for v1 if v1 is continuous and is smooth,
   // the loader will set it from the next edge's v0 bias
   if (IsSecondVertexPeaked() || !GetContinuousEdge( GetSecondVertex() ))
   {
      VertexHandle *pSecondVertexHandle = GetSecondVertexHandle(FALSE);
      if (pSecondVertexHandle->m_fAlpha!=0.0f || pSecondVertexHandle->m_fGamma!=0.0f || pSecondVertexHandle->m_fMagnitude!=1.0f)
         nFlags |= SF_V1BIAS;
   }

   return nFlags;
}

void PatchEdge::ApplySaveFlags( UINT nFlags )
{
   Edge::ApplySaveFlags( nFlags );

   if (nFlags & SF_V0CONSTRAINED)
      SetFirstVertexConstrainedFlag( TRUE );
   if (nFlags & SF_V1CONSTRAINED)
      SetSecondVertexConstrainedFlag( TRUE );

   if (nFlags & SF_LOOP)
      SetContinuityLoop( TRUE );
}

void PatchEdge::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   Edge::Save( bs );

   UINT nPrevContEdgeID = 0;
   PatchEdge *pPrevContEdge = GetPrevContEdge( TRUE );
   if (pPrevContEdge)
      nPrevContEdgeID = pPrevContEdge->GetID();

   UINT nNextContEdgeID = 0;
   PatchEdge *pNextContEdge = GetNextContEdge( TRUE );
   if (pNextContEdge)
      nNextContEdgeID = pNextContEdge->GetID();

   bs << sp << nPrevContEdgeID << sp << nNextContEdgeID;

   UINT nFlags = GetSaveFlags();
   if (nFlags & SF_V0BIAS)
   {
      VertexHandle *pFirstVertexHandle = GetFirstVertexHandle(FALSE);
      bs << sp << pFirstVertexHandle->m_fAlpha << sp << pFirstVertexHandle->m_fGamma << sp << pFirstVertexHandle->m_fMagnitude;
   }
   if (nFlags & SF_V1BIAS)
   {
      VertexHandle *pSecondVertexHandle = GetSecondVertexHandle(FALSE);
      bs << sp << pSecondVertexHandle->m_fAlpha << sp << pSecondVertexHandle->m_fGamma << sp << pSecondVertexHandle->m_fMagnitude;
   }

   if (bWriteHeader)
      bs << nl;
}

BOOL PatchEdge::Load( BaseStream &bs )
{
   UINT nFlags;
   if (!Edge::LoadGuts( bs, nFlags ))
      return FALSE;

   if (bs.Eof()) // probably promoting a lower class model
      return TRUE;

   UINT nPrevContEdgeID;
   UINT nNextContEdgeID;

   bs >> nPrevContEdgeID >> nNextContEdgeID;

   m_pPrevContEdge = (PatchEdge *)nPrevContEdgeID; // temporarily store the edge id in the pointer until OnEndObjectLoad
   m_pNextContEdge = (PatchEdge *)nNextContEdgeID; // ""

   if (nFlags & SF_V0BIAS)
   {
      float fAlpha, fGamma, fMagnitude;
      bs >> fAlpha >> fGamma >> fMagnitude;

      VertexHandle *pFirstVertexHandle = GetFirstVertexHandle(FALSE);
      pFirstVertexHandle->m_fAlpha.StoreValue( fAlpha, 0, FALSE );
      pFirstVertexHandle->m_fGamma.StoreValue( fGamma, 0, FALSE );
      pFirstVertexHandle->m_fMagnitude.StoreValue( fMagnitude, 0, FALSE );
   }

   if (nFlags & SF_V1BIAS)
   {
      float fAlpha, fGamma, fMagnitude;
      bs >> fAlpha >> fGamma >> fMagnitude;

      VertexHandle *pSecondVertexHandle = GetSecondVertexHandle(FALSE);
      pSecondVertexHandle->m_fAlpha.StoreValue( fAlpha, 0, FALSE );
      pSecondVertexHandle->m_fGamma.StoreValue( fGamma, 0, FALSE );
      pSecondVertexHandle->m_fMagnitude.StoreValue( fMagnitude, 0, FALSE );
   }

   return TRUE;
}

void PatchEdge::SetFirstVertex(Vertex *pVertex)
{
   SetTessellationAllocationDirty(TRUE);
   Edge::SetFirstVertex(pVertex);
}

void PatchEdge::SetSecondVertex(Vertex *pVertex)
{
   SetTessellationAllocationDirty(TRUE);
   Edge::SetSecondVertex(pVertex);
}

void PatchEdge::FindClosestIntersection(CGLBaseView *pView, const CPoint &point, float &fPos )
{
   TessEdgeVertices *pTessEdgeVertices = GetAlignedTessEdgeVertices();

   TransformableObject *pTransformableObject = (TransformableObject*)GetOwner();

   pTessEdgeVertices->FindClosestIntersection( pView, pTransformableObject, point, fPos );

   if (!(pTessEdgeVertices->GetVertexAt(0) == GetFirstVertex()) )
      fPos = 1.0f - fPos;
}

TessEdgeVertices *PatchEdge::GetTessEdgeVertices(Patch *pPatch)
{
   Tessellate();
   if (IsAligned(pPatch))
      return &m_alignedTessedgeVertices;
   else
      return &m_nonAlignedTessedgeVertices;
}

void PatchEdge::Tessellate()
{
   if (!IsTessellationDirty())
      return;

   SetTessellationDirty( FALSE );

   AllocateTessellation();

   SetTessellation();
}

void PatchEdge::AllocateTessellation()
{
   PatchModel *pPatchModel = (PatchModel*)GetOwner();

   Patch *pPatch;
   int nSub = CalcEdgeSubdivisions(&pPatch);
   int nNumSteps = 1 << nSub;
   nNumSteps += 1;

   //////////////////////////////////
   // Vertices
   //////////////////////////////////

   if (m_alignedTessedgeVertices.GetNumVertices() != nNumSteps)
   {
      TessEdgeVertices *pTessEdgeVertices = &m_alignedTessedgeVertices;
      TessEdgeVertices *pFlippedTessEdgeVertices = &m_nonAlignedTessedgeVertices;
      if ( pPatch && !IsAligned(pPatch) )
      {
         pTessEdgeVertices = &m_nonAlignedTessedgeVertices;
         pFlippedTessEdgeVertices = &m_alignedTessedgeVertices;
      }

      PatchVertex *pVertex;

#if 0
      int nCount = nNumSteps-2;
      if (nCount>0)
         pVertex = pPatchModel->AddTessellatedVertices(nCount);
#endif

      pTessEdgeVertices->SetNumVertices( nNumSteps );
      pFlippedTessEdgeVertices->SetNumVertices( nNumSteps );

      for (int i=0; i<nNumSteps; i++)
      {

         if (i==0)// steal first vertex
         {
            if (pPatch)
               pVertex = (PatchVertex *)GetAlignedFirstVertex(pPatch);
            else
               pVertex = (PatchVertex *)GetFirstVertex();
         }
         else if (i==nNumSteps-1)// steal last vertex
         {
            if (pPatch)
               pVertex = (PatchVertex *)GetAlignedSecondVertex(pPatch);
            else
               pVertex = (PatchVertex *)GetSecondVertex();
         }
#if 0
         else
         {
            pVertex->Reference();
            pTessEdgeVertices->SetVertexAt( i, pVertex );
            pFlippedTessEdgeVertices->SetVertexAt( pTessEdgeVertices->GetNumVertices()-i-1, pVertex );
            pVertex++;
            continue;
         }
#else
         else // allocate middle vertices
            pVertex = pPatchModel->AddTessellatedVertex();
#endif
         pTessEdgeVertices->SetVertexAt( i, pVertex );
         pFlippedTessEdgeVertices->SetVertexAt( pTessEdgeVertices->GetNumVertices()-i-1, pVertex );
      }
   }

   //////////////////////////////////
   // Normals
   //////////////////////////////////

   TessEdgeNormals *pTessEdgeNormals0 = NULL;
   TessEdgeNormals *pTessEdgeNormals1 = NULL;
   EdgePatchInfo *pEdgePatchInfo0 = NULL, *pEdgePatchInfo1 = NULL;
   Patch *pConnectedPatch = NULL;

   if (pPatch)
   {
      pEdgePatchInfo0 = GetEdgePatchInfo(pPatch);
      if (pEdgePatchInfo0)
         pTessEdgeNormals0 = &pEdgePatchInfo0->m_tessedgenormals;

      pConnectedPatch = GetConnectedPatch(pPatch);
      if (pConnectedPatch)
      {
         pEdgePatchInfo1 = GetEdgePatchInfo(pConnectedPatch);
         if (pEdgePatchInfo1)
            pTessEdgeNormals1 = &pEdgePatchInfo1->m_tessedgenormals;
      }
   }

   if ( (pTessEdgeNormals0 && pTessEdgeNormals0->GetNumVertexNormals() != nNumSteps)
    || ( pTessEdgeNormals1 && pTessEdgeNormals1->GetNumVertexNormals() != nNumSteps ))
   {
      int nFirstIndex0=-1,nFirstIndex1=-1;
      int nSecondIndex0=-1,nSecondIndex1=-1;

      // get face relative indices
      if (pPatch)
      {
         nFirstIndex0 = pPatch->FindVertexFaceRelativeIndex(GetAlignedFirstVertex(pPatch));
         if (nFirstIndex0<0)
            pPatch = NULL;

         if (pPatch)
            nSecondIndex0 = pPatch->FindVertexFaceRelativeIndex(GetAlignedSecondVertex(pPatch));
      }

      if (pPatch)
      {
         if (pConnectedPatch)
         {
            nFirstIndex1 = pConnectedPatch->FindVertexFaceRelativeIndex(GetAlignedSecondVertex(pConnectedPatch));
            ASSERT(nFirstIndex1>=0);
            nSecondIndex1 = pConnectedPatch->FindVertexFaceRelativeIndex(GetAlignedFirstVertex(pConnectedPatch));
            ASSERT(nSecondIndex1>=0);
         }

         pTessEdgeNormals0->SetNumVertexNormals( nNumSteps );
         if (pTessEdgeNormals1)
            pTessEdgeNormals1->SetNumVertexNormals( nNumSteps );

         VertexNormal *pVertexNormal0;
         VertexNormal *pVertexNormal1;
#if 0
         int nCount = nNumSteps-2;
         if (nCount>0)
         {
            pVertexNormal0 = pPatchModel->AddTessellatedVertexNormals(nCount);
            if (IsSoftEdge())
               pVertexNormal1 = pPatchModel->AddTessellatedVertexNormals(nCount);
         }
#endif

         for (int i=0; i<nNumSteps; i++)
         {
 
            if (i==0) // steal first normal
            {
               pVertexNormal0 = pPatch->GetVertexNormalAt(nFirstIndex0, FALSE);
               if (pTessEdgeNormals1)
                  pVertexNormal1 = pConnectedPatch->GetVertexNormalAt(nFirstIndex1, FALSE);
            }
            else if (i==nNumSteps-1)// steal last normal
            {
               pVertexNormal0 = pPatch->GetVertexNormalAt(nSecondIndex0, FALSE);
               if (pTessEdgeNormals1)
                  pVertexNormal1 = pConnectedPatch->GetVertexNormalAt(nSecondIndex1, FALSE);
            }
#if 0
            else // allocate middle normal
            {
               pTessEdgeNormals0->SetVertexNormalAt( i, pVertexNormal0 );
               if (pTessEdgeNormals1)
                  pTessEdgeNormals1->SetVertexNormalAt( nNumSteps-i-1, pVertexNormal1 );
               pVertexNormal0->Reference();
               pVertexNormal0++;
               if (IsSoftEdge())
               {
                  pVertexNormal1->Reference();
                  pVertexNormal1++;
               }
               continue;
            }
#else
            else // allocate middle normal
            {
               pVertexNormal0 = VertexNormal::Create();
               if (pTessEdgeNormals1)
               {
                  if (IsSoftEdge())
                     pVertexNormal1 = pVertexNormal0;
                  else
                     pVertexNormal1 = VertexNormal::Create();
               }
            }
#endif
            pTessEdgeNormals0->SetVertexNormalAt( i, pVertexNormal0 );
            if (pTessEdgeNormals1)
               pTessEdgeNormals1->SetVertexNormalAt( nNumSteps-i-1, pVertexNormal1 );
         }
      }
   }
   for (PropertyPatchEdge *pPropertyPatchEdge = (PropertyPatchEdge*)GetHeadPropertyEdge(); pPropertyPatchEdge; pPropertyPatchEdge = (PropertyPatchEdge*)pPropertyPatchEdge->GetNextPropertyEdge())
      pPropertyPatchEdge->AllocateTessellation();
}

void PatchEdge::SetTessellation()
{
   Patch *pPatch;
   int nSub = CalcEdgeSubdivisions(&pPatch);
   int nNumSteps = 1 << nSub;

   TessEdgeNormals *pTessEdgeNormals0 = NULL;
   TessEdgeNormals *pTessEdgeNormals1 = NULL;
   EdgePatchInfo *pEdgePatchInfo0 = NULL, *pEdgePatchInfo1 = NULL;

   Patch *pConnectedPatch = NULL;
   if (pPatch)
   {
      pEdgePatchInfo0 = GetEdgePatchInfo(pPatch);
      if (!pEdgePatchInfo0)
         return;

      pTessEdgeNormals0 = &pEdgePatchInfo0->m_tessedgenormals;

      pConnectedPatch = GetConnectedPatch(pPatch);
      if (pConnectedPatch)
      {
         pEdgePatchInfo1 = GetEdgePatchInfo(pConnectedPatch);
         if (!pEdgePatchInfo1)
            return;
         pTessEdgeNormals1 = &pEdgePatchInfo1->m_tessedgenormals;
      }
   }

   // Add NPatch Edge Hook Vertex
   BOOL bIsNPatchHookEdge = FALSE; 
   if ((pPatch && pPatch->IsNPatch()) || (pConnectedPatch && pConnectedPatch->IsNPatch()))
      bIsNPatchHookEdge = TRUE;

   TessEdgeVertices *pTessEdgeVertices = GetAlignedTessEdgeVertices();
   Vector p0, p1, p2, p3;

   p0 = GetFirstVertex()->GetPos();
   p1 = GetFirstVertexHandle()->GetTangent();
   p2 = -GetSecondVertexHandle()->GetTangent();
   p3 = GetSecondVertex()->GetPos();

   Vector v;
   double inc = 1.0 / nNumSteps;
   int i=0;
   double t;
   for (t=inc, i=1; i<nNumSteps; t+=inc, i++)
   {
      v = (( 2*t*t*t)-(3*t*t)+1)*p0
        + ((-2*t*t*t)+(3*t*t)  )*p3
        + ((   t*t*t)-(2*t*t)+t)*p1
        + ((   t*t*t)-(  t*t)  )*p2;

      pTessEdgeVertices->GetVertexAt(i)->SetPos(v);

      if ((bIsNPatchHookEdge)&&(t==0.5f))
         pTessEdgeVertices->GetVertexAt(i)->SetNPatchHook(TRUE);

      if (pPatch)
      {
         pTessEdgeNormals0->GetVertexNormalAt(i)->SetVertexNormalDirtyFlag(TRUE);
         if (pConnectedPatch && !IsSoftEdge())// don't set opposite of shared edge
            pTessEdgeNormals1->GetVertexNormalAt(i)->SetVertexNormalDirtyFlag(TRUE);
      }
   }

   if ((pPatch)&&(pTessEdgeNormals0->GetNumVertexNormals()>0))
   {
      pTessEdgeNormals0->GetVertexNormalAt(0)->SetVertexNormalDirtyFlag(TRUE);
      pTessEdgeNormals0->GetVertexNormalAt( pTessEdgeNormals0->GetNumVertexNormals()-1)->SetVertexNormalDirtyFlag(TRUE);

      if ((pConnectedPatch)&&(pTessEdgeNormals1->GetNumVertexNormals()>0))
      {
         pTessEdgeNormals1->GetVertexNormalAt(0)->SetVertexNormalDirtyFlag(TRUE);
         pTessEdgeNormals1->GetVertexNormalAt( pTessEdgeNormals1->GetNumVertexNormals()-1 )->SetVertexNormalDirtyFlag(TRUE);
      }
   }
   for (PropertyPatchEdge *pPropertyPatchEdge = (PropertyPatchEdge*)GetHeadPropertyEdge(); pPropertyPatchEdge; pPropertyPatchEdge = (PropertyPatchEdge*)pPropertyPatchEdge->GetNextPropertyEdge())
      pPropertyPatchEdge->SetTessellation();
}

UINT PatchEdge::CalcEdgeSubdivisions(Patch **pHighOrderPatch/*=NULL*/)
{
   PatchModel *pPatchModel = (PatchModel *)GetOwner();

   int nMaxSubdivide = pPatchModel->m_nPatchSubdivisions;
   Patch *pPatch = GetPatch();
   if (pPatch)
   {  
      if (pHighOrderPatch)
         *pHighOrderPatch = pPatch;
      Patch *pConnectedPatch = GetConnectedPatch(pPatch);
      if ((pConnectedPatch)&&(pConnectedPatch->GetMaxSubdivision()>pPatch->GetMaxSubdivision()))
      {
         nMaxSubdivide += pConnectedPatch->GetMaxSubdivision();
         if (pHighOrderPatch)
            *pHighOrderPatch = pPatch;
      }
      else
         nMaxSubdivide += pPatch->GetMaxSubdivision();
      if (pPatchModel->m_bFixedSubdivisions)
         return nMaxSubdivide;
   }

   if (pHighOrderPatch)
      *pHighOrderPatch = NULL;

   if (pPatchModel->m_bFixedSubdivisions)
      return nMaxSubdivide;

   // calculate new fPosition
   Vertex *pVertex = GetFirstVertex();
   Vertex *pNextVertex = GetSecondVertex();

   Vector v0 = pVertex->GetPos();
   Vector v1 = GetFirstTangentPos();
   Vector v2 = GetSecondTangentPos();
   Vector v3 = pNextVertex->GetPos();

   return MathTools::CalcBezierCurveSubdivisions(pPatchModel->m_fPatchTolerance,v0,v1,v2,v3,nMaxSubdivide);
}

BOOL PatchEdge::AddPoly(Poly *pPoly, UINT nEdgeIndex)
{
   BOOL bResult = Edge::AddPoly(pPoly, nEdgeIndex);
   ((Patch *)pPoly)->SetTessellationAllocationDirty(TRUE);
   Patch *pConnectedPatch = GetConnectedPatch( (Patch *)pPoly );
   if (pConnectedPatch)
      pConnectedPatch->SetTessellationAllocationDirty(TRUE);
   SetTangentsDirty(TRUE);
   SetPropertyEdgeListDirty( TRUE );
   return bResult;
}

void PatchEdge::RemovePoly(Poly *pPoly, BOOL bRemoveIfNotUsed/*=TRUE*/)
{
   Edge::RemovePoly(pPoly, bRemoveIfNotUsed);
   SetPropertyEdgeListDirty( TRUE );
}
   
EdgePolyInfo *PatchEdge::CreateNewEdgePolyInfo() 
{ 
   SetTessellationDirty( TRUE ); 
   return SNEW EdgePatchInfo; 
}

Vertex::DeleteInfo *PatchEdge::SplitEdge( float fPosition/*=0.5f*/, Edge **pOutEdge0/*=NULL*/, Edge **pOutEdge1/*=NULL*/  )
{

   PatchEdge *pV0ContEdge = (PatchEdge *)GetContinuousEdge(GetFirstVertex());
   BOOL bV0PrevCont = pV0ContEdge ? pV0ContEdge->GetPrevContVertex() == GetFirstVertex() : FALSE;

   PatchEdge *pV1ContEdge = (PatchEdge *)GetContinuousEdge(GetSecondVertex());
   BOOL bV1PrevCont = pV1ContEdge ? pV1ContEdge->GetPrevContVertex() == GetSecondVertex() : FALSE;

   Vertex::DeleteInfo *pDeleteInfo = Edge::SplitEdge( fPosition, pOutEdge0, pOutEdge1 );

   if (pDeleteInfo)
   {
      PatchEdge *pNewEdge0 = (PatchEdge *)pDeleteInfo->GetOldEdge0();
      PatchEdge *pNewEdge1 = (PatchEdge *)pDeleteInfo->GetOldEdge1();
      ASSERT( pNewEdge0 );
      ASSERT( pNewEdge1 );

      if (pV0ContEdge && !pV0ContEdge->IsInList()) // Mirror mode next to axis
         pV0ContEdge = (PatchEdge *)pNewEdge0->Edge::GetContinuousEdge(pNewEdge0->GetFirstVertex());

      if (pV0ContEdge)
      {
         if (bV0PrevCont)
            pV0ContEdge->SetPrevContEdge(pNewEdge0);
         else
            pV0ContEdge->SetNextContEdge(pNewEdge0);
      }

      if (pV1ContEdge && !pV1ContEdge->IsInList()) // Mirror mode next to axis
         pV1ContEdge = (PatchEdge *)pNewEdge1->Edge::GetContinuousEdge(pNewEdge1->GetSecondVertex());

      if (pV1ContEdge)
      {
         if (bV1PrevCont)
            pV1ContEdge->SetPrevContEdge(pNewEdge1);
         else
            pV1ContEdge->SetNextContEdge(pNewEdge1);
      }

      pNewEdge0->SetContEdge((PatchVertex *)pNewEdge0->GetSecondVertex(), pNewEdge1);
   }

   return pDeleteInfo;
}

Edge *PatchEdge::GetContinuousEdge( Vertex *pVertex, float fAngleTolerance/*=50.0f*/ )
{
   if (m_pPrevContEdge && m_pPrevContEdge->IsInEdge(pVertex))
      return m_pPrevContEdge;
   else if (m_pNextContEdge && m_pNextContEdge->IsInEdge(pVertex))
      return m_pNextContEdge;
   else
      return NULL;
}

static Edge *FindContinuousEdge( PatchEdge *pEdge, Edge *pPrev, BOOL bStartOtherDirectionIfNUll/*=FALSE*/, float fAngleTolerance/*=50.0f*/ )
{
   Vertex *pSharedVertex;
   if (pPrev)
      pSharedVertex = pEdge->GetSharedVertex(pPrev);
   else
   {
      if (bStartOtherDirectionIfNUll)
         pSharedVertex = pEdge->GetFirstVertex();
      else
         pSharedVertex = pEdge->GetSecondVertex();
   }

   Vertex *pNextVertex = pEdge->GetConnectedVertex( pSharedVertex );
   return pEdge->Edge::GetContinuousEdge( pNextVertex, fAngleTolerance ); // Don't want Patch::GetContinuousEdge because we are setting the cached pointers and it returns them
}

void PatchEdge::BuildContinuity()
{
   if (!IsContinuityDirty())
      return;

   PatchModel *pPatchModel = (PatchModel*)GetOwner();
   float fTolerance = pPatchModel->m_fContinuityTolerance.GetValue();
 
   // walk backwards
   PatchEdge *pCurrEdge = this;
   PatchEdge *pLastEdge = NULL;

   while( TRUE )
   {
      PatchEdge *pPrevEdge = (PatchEdge *)FindContinuousEdge( pCurrEdge, pLastEdge, FALSE, fTolerance );
      if (pPrevEdge==NULL)
         break;

      pCurrEdge->SetPrevContEdge(pPrevEdge);
      if (pPrevEdge == this)
         return;

      pLastEdge = pCurrEdge;
      pCurrEdge = pPrevEdge;
   }
   pCurrEdge->SetPrevContEdge(NULL);

   pCurrEdge = this;
   pLastEdge = NULL;

   // walk forwards
   while( TRUE )
   {
      PatchEdge *pNextEdge = (PatchEdge *)FindContinuousEdge( pCurrEdge, pLastEdge, TRUE, fTolerance );
      if (pNextEdge==NULL)
         break;

      pCurrEdge->SetNextContEdge(pNextEdge);
      if (pNextEdge == this)
         return;

      pLastEdge = pCurrEdge;
      pCurrEdge = pNextEdge;
   }
   pCurrEdge->SetNextContEdge(NULL);
}

void BuildEdgeCoefficients(Vector vV0, Vector vV1,
                           Vector vVN0, Vector vVN1,
                           Vector &vCoeff0, Vector &vCoeff1)
{
   float tmpf;
   Vector tangent1,tangent2;
   Vector edge = vV1 - vV0;

   //E - (E.N)N
   tangent1 = edge;
   tmpf = tangent1.Dot(vVN0);
   tangent1 -= vVN0 * tmpf;

   vCoeff0 = vV0 + (tangent1 * 0.3333f);

   //E - (E.N)N
   tangent2 = edge;
   tmpf = tangent2.Dot(vVN1);
   tangent2 -= vVN1 * tmpf;

   vCoeff1 = vV1 - (tangent2 * 0.3333f);
}

void PatchEdge::BuildTangents()
{
   if (!m_bTangentsDirty)
      return;

   m_bTangentsDirty = FALSE;

   PatchEdge *pPrevContEdge = GetPrevContEdge(TRUE);
   PatchEdge *pNextContEdge = GetNextContEdge(TRUE);

   const Vector *pV0;
   if (pPrevContEdge && !IsVertexPeaked(GetPrevContVertex()))
      pV0 = &pPrevContEdge->GetPrevContVertex()->GetPos();
   else
      pV0 = NULL;

   const Vector &pV1 = GetPrevContVertex()->GetPos();
   const Vector &pV2 = GetNextContVertex()->GetPos();

   const Vector *pV3;
   if (pNextContEdge && !IsVertexPeaked(GetNextContVertex()))
      pV3 = &pNextContEdge->GetNextContVertex()->GetPos();
   else
      pV3 = NULL;

   VertexHandle *pHandle1 = GetVertexHandle(GetPrevContVertex());
   VertexHandle *pHandle2 = GetVertexHandle(GetNextContVertex());

   Vector vTangent0;
   Vector vTangent1;
   MathTools::ComputeTangents(pV0, pV1, pV2, pV3, &vTangent0, &vTangent1,
    pHandle1->m_fAlpha, pHandle1->m_fGamma, pHandle1->m_fMagnitude,
    pHandle2->m_fAlpha, pHandle2->m_fGamma, pHandle2->m_fMagnitude);

   if ( GetPrevContVertex() == GetFirstVertex() )
   {
      m_firstVertexHandle.SetTangent(vTangent0);
      m_secondVertexHandle.SetTangent(vTangent1);
   }
   else if (GetPrevContVertex()==GetSecondVertex())
   {
      m_firstVertexHandle.SetTangent(vTangent1);
      m_secondVertexHandle.SetTangent(vTangent0);
   }
}

Vector PatchEdge::CalculateTangent( PatchVertex *pPatchVertex, Vector &vAlphaPlane, Vector &vGammaPlane )
{
   if ( (GetFirstVertex() != pPatchVertex) && (GetSecondVertex() != pPatchVertex) )
   {
      ASSERT(FALSE);//vertex doesn't belong to edge
   }

   PatchEdge *pPrevContEdge = GetPrevContEdge(TRUE);
   PatchEdge *pNextContEdge = GetNextContEdge(TRUE);

   const Vector *pV0;
   if (pPrevContEdge && !IsVertexPeaked(GetPrevContVertex()))
      pV0 = &pPrevContEdge->GetPrevContVertex()->GetPos();
   else
      pV0 = NULL;

   const Vector &pV1 = GetPrevContVertex()->GetPos();
   const Vector &pV2 = GetNextContVertex()->GetPos();

   const Vector *pV3;
   if (pNextContEdge && !IsVertexPeaked(GetNextContVertex()))
      pV3 = &pNextContEdge->GetNextContVertex()->GetPos();
   else
      pV3 = NULL;

   Vector vTangent0;
   Vector vTangent1;
   MathTools::ComputeTangents(pV0, pV1, pV2, pV3, &vTangent0, &vTangent1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);

   if ( GetPrevContVertex() == pPatchVertex )
   {
      vGammaPlane = Cross( vTangent0, (pV2 - pV1).Normalized() );
      vAlphaPlane = Cross( vTangent0, vGammaPlane );
      return vTangent0;
   }
   else
   {
      vGammaPlane = Cross( vTangent1,(pV1 - pV2).Normalized() );
      vAlphaPlane = Cross( vTangent1, vGammaPlane );
      return vTangent1;
   }
}

void PatchEdge::BuildTangentsFromVertexNormals()
{
   if (!m_bTangentsDirty)
      return;

   m_bTangentsDirty = FALSE;

   PatchModel *pPatchModel = (PatchModel*)GetOwner();

   Patch *pPatch = (Patch*)GetPoly();
   UINT nIndex,nNext;
   nIndex = (UINT)GetFaceRelativeIndex((Poly*)pPatch);
   nNext = pPatch->GetNextIndex(nIndex);

   Vector vCoeff0,vCoeff1;

   BuildEdgeCoefficients(pPatch->GetVertexAt(nIndex)->GetPos(),pPatch->GetVertexAt(nNext)->GetPos(),
      pPatch->GetVertexNormalAt(nIndex)->GetDir(),pPatch->GetVertexNormalAt(nNext)->GetDir(),
      vCoeff0,vCoeff1);

   // gets previous continuous edge to round out patch
   Vector edgevector;
   GetVector(edgevector);
   float edgevectorlength = edgevector.Length();

   Vector tangent;
   tangent = vCoeff0 - pPatch->GetVertexAt(nIndex)->GetPos();
   m_firstVertexHandle.SetTangent(tangent);

   tangent = vCoeff1 - pPatch->GetVertexAt(nNext)->GetPos();
   m_secondVertexHandle.SetTangent(tangent);
}


Vector PatchEdge::GetFirstTangentPos()
{
   return GetFirstVertexHandle()->GetTangent() * 0.3333f + GetFirstVertex()->GetPos();
}

Vector PatchEdge::GetSecondTangentPos()
{
   return GetSecondVertexHandle()->GetTangent() * 0.3333f + GetSecondVertex()->GetPos();
}

Vector PatchEdge::GetAlignedFirstTangentPos( Poly *pPoly )
{
   if (IsAligned( pPoly ))
      return GetFirstTangentPos();

   return GetSecondTangentPos();
}

Vector PatchEdge::GetAlignedSecondTangentPos( Poly *pPoly )
{
   if (IsAligned( pPoly ))
      return GetSecondTangentPos();

   return GetFirstTangentPos();
}

void PatchEdge::SetDirty()
{
   Edge::SetDirty();

   SetTangentsDirty(TRUE);
}

void PatchEdge::SetTangentsDirty( BOOL bFlag )
{
   m_bTangentsDirty = bFlag;

   SetTessellationDirty( bFlag );
   Patch *pPatch = GetPatch();
   if (!pPatch)
      return;
   pPatch->SetTangentsDirty(bFlag);
   Patch *pConnectedPatch = GetConnectedPatch(pPatch);
   if (pConnectedPatch)
      pConnectedPatch->SetTangentsDirty(bFlag);

   for (PropertyPatchEdge *pPropertyPatchEdge = (PropertyPatchEdge*)GetHeadPropertyEdge(); pPropertyPatchEdge; pPropertyPatchEdge = (PropertyPatchEdge*)pPropertyPatchEdge->GetNextPropertyEdge())
      pPropertyPatchEdge->SetTangentsDirty(bFlag);
}

void PatchEdge::ClearTessellation()
{
   m_alignedTessedgeVertices.Clear();
   m_nonAlignedTessedgeVertices.Clear(); // We really don't need to clear all the tesselations like the non aligned and normals because we only check against aligned to whether we rebuild all the tessellation

   for (PropertyPatchEdge *pPropertyPatchEdge = (PropertyPatchEdge*)GetHeadPropertyEdge(); pPropertyPatchEdge; pPropertyPatchEdge = (PropertyPatchEdge*)pPropertyPatchEdge->GetNextPropertyEdge())
      pPropertyPatchEdge->ClearTessellation();

   Patch *pPatch = GetPatch();

   if (pPatch)
   {
      EdgePatchInfo *pInfo = GetEdgePatchInfo(pPatch);
      ASSERT( pInfo );
      pInfo->m_tessedgenormals.Clear();

      pPatch = GetConnectedPatch(pPatch);
      if (pPatch)
      {
         pInfo = GetEdgePatchInfo(pPatch);
         ASSERT( pInfo );
         pInfo->m_tessedgenormals.Clear();
      }
   }
}

void PatchEdge::Disconnect()
{
   ClearTessellation();
   SetTessellationDirty( FALSE );

   for (PatchEdge *pEdge = this; pEdge; pEdge = pEdge->m_pNextContEdge)
      if (pEdge->IsContinuityLoop())
      {
         pEdge->SetContinuityLoop(FALSE);
         break;
      }

   if (m_pPrevContEdge)
      m_pPrevContEdge->m_pNextContEdge = NULL;
   if (m_pNextContEdge)
      m_pNextContEdge->m_pPrevContEdge = NULL;
   Edge::Disconnect();
}

void PatchEdge::Reconnect()
{
   Edge::Reconnect();

   if (m_pPrevContEdge)
      m_pPrevContEdge->m_pNextContEdge = this;

   if (m_pNextContEdge)
      m_pNextContEdge->m_pPrevContEdge = this;

   for (PatchEdge *pEdge = this->m_pNextContEdge; pEdge; pEdge = pEdge->m_pNextContEdge)
   {
      if (pEdge->IsContinuityLoop())
         pEdge->SetContinuityLoop(FALSE);

      if (pEdge == this)
      {
         pEdge->SetContinuityLoop(TRUE);
         break;
      }
   }

   SetTessellationAllocationDirty( TRUE );
}

void PatchEdge::SetTessellationAllocationDirty(BOOL bFlag)
{
   SetTessellationDirty( bFlag );
   ClearTessellation();

   Patch *pPatch = GetPatch();
   if (!pPatch)
      return;
   pPatch->SetTessellationAllocationDirty(bFlag);
   Patch *pConnectedPatch = GetConnectedPatch(pPatch);
   if (pConnectedPatch)
      pConnectedPatch->SetTessellationAllocationDirty(bFlag);
}

TessEdgeNormals *PatchEdge::GetTessEdgeNormals(Patch *pPatch)
{
   EdgePatchInfo *pEdgePatchInfo = GetEdgePatchInfo(pPatch);
   if (!pEdgePatchInfo)
      return NULL;

   Tessellate();
   return &pEdgePatchInfo->m_tessedgenormals; 
}

Vector PatchEdge::GetPosAlongEdge( float fPosition, Poly *pPoly/*=NULL*/  )
{
   ASSERT(fPosition >= 0.0 && fPosition <= 1.0f);

   Patch *pPatch = (Patch*)pPoly;

   // calculate new fPosition
   Vector v0, cv0, cv1, v3;
   Vertex *pVertex, *pNextVertex;

   if (pPatch)
   {
      pVertex = GetAlignedFirstVertex( pPatch );
      pNextVertex = GetAlignedSecondVertex( pPatch );
      cv0 = GetAlignedFirstTangentPos( pPatch );
      cv1 = GetAlignedSecondTangentPos( pPatch );
   }
   else
   {
      pVertex = GetFirstVertex();
      pNextVertex = GetSecondVertex();
      cv0 = GetFirstTangentPos();
      cv1 = GetSecondTangentPos();
   }

   v0 = pVertex->GetPos();
   v3 = pNextVertex->GetPos();

   return MathTools::Bezier( fPosition, v0, cv0, cv1, v3 );// along curve
}

void PatchEdge::SetTessellationDirty( BOOL bFlag )
{
   if (m_bTessellationDirty == bFlag)
      return;

   if (bFlag && !IsInList())
      return;

   m_bTessellationDirty = bFlag;

   //if (IsDangle())
   //{
      PatchModel *pModel = GetModel();
      ASSERT( pModel );

      if (bFlag)
      {
         m_pNextDirtyDangle = pModel->m_pDirtyDangleHead;
         m_pPrevDirtyDangle = NULL;
         if (m_pNextDirtyDangle)
            m_pNextDirtyDangle->m_pPrevDirtyDangle = this;
         pModel->m_pDirtyDangleHead = this;
      }
      else
      {
         if (m_pNextDirtyDangle)
            m_pNextDirtyDangle->m_pPrevDirtyDangle = m_pPrevDirtyDangle;

         if (m_pPrevDirtyDangle)
            m_pPrevDirtyDangle->m_pNextDirtyDangle = m_pNextDirtyDangle;
         else
            pModel->m_pDirtyDangleHead = m_pNextDirtyDangle;
      }
   //}
}

void PatchEdge::ReverseSpline()
{
   PatchEdge *pStartEdge;
   for (pStartEdge=this; pStartEdge->GetPrevContEdge(FALSE); pStartEdge = pStartEdge->GetPrevContEdge(FALSE))
      ;

   PatchEdge *pNextEdge, *pLastEdge = NULL;
   for (PatchEdge *pEdge=pStartEdge; pEdge; pEdge=pNextEdge)
   {
      pLastEdge = pEdge;
      pNextEdge=pEdge->GetNextContEdge(FALSE);

      PatchEdge *pTemp=pEdge->m_pNextContEdge; 
      pEdge->m_pNextContEdge = pEdge->m_pPrevContEdge;
      pEdge->m_pPrevContEdge = pTemp;
   }

   if (pLastEdge && pLastEdge->IsContinuityLoop())
   {
      pLastEdge->SetContinuityLoop(FALSE);
      pLastEdge->m_pPrevContEdge->SetContinuityLoop(TRUE);
   }
}

PatchEdge::RedirectInfo *PatchEdge::Redirect( PatchVertex *pTermVertex, String *pOutError/*=NULL*/ )
{
   if (!IsInEdge(pTermVertex))
   {
      if (pOutError)
      {
         *pOutError = "TERM Vertex does not belong to FROM Edge(%1).";
         pOutError->Replace("%1", (ULONG) GetID());
      }
      return NULL;
   }

   RedirectInfo *pInfo = SNEW RedirectInfo(this, NULL, pTermVertex);

   pInfo->Redo();

   static BOOL bInMirror;
   if (!bInMirror)
   {
      bInMirror = TRUE;
      PatchEdge *pMirrorThis = (PatchEdge *)GetMirrorObject();
      PatchVertex *pMirrorTermVertex = (PatchVertex *)pTermVertex->GetMirrorObject();
      if (pMirrorThis && pMirrorTermVertex)
         pInfo->m_pMirrorInfo = pMirrorThis->Redirect( pMirrorTermVertex, pOutError );
      bInMirror = FALSE;
   }

   return pInfo;
}

PatchEdge::RedirectInfo *PatchEdge::Redirect( PatchEdge *pToEdge, String *pOutError/*=NULL*/ )
{
   ASSERT( pToEdge);

   if (pToEdge == this)
   {
      if (pOutError)
         *pOutError = "FROM Edge and TO Edge can not be the same";
      return NULL;
   }

   PatchVertex *pSharedVertex = (PatchVertex *)GetSharedVertex( pToEdge );
   if (!pSharedVertex)
   {
      if (pOutError)
      {
         *pOutError = "Edge(%1) is not attached to Edge(%2)";
         pOutError->Replace("%1", (ULONG)GetID());
         pOutError->Replace("%2", (ULONG)pToEdge->GetID());
      }
      return NULL;
   }

   RedirectInfo *pInfo = SNEW RedirectInfo(this, pToEdge, pSharedVertex);
   if (pToEdge == pInfo->m_pOriginalToEdge)
   {
      delete pInfo;
      if (pOutError)
      {
         *pOutError = "Edge(%1) is already directed through Edge(%2), nothing to do";
         pOutError->Replace("%1", (ULONG)GetID());
         pOutError->Replace("%2", (ULONG)pToEdge->GetID());
      }
      return NULL;
   }

   if (pInfo->m_bReverseTo)
   {
      // Test to make sure that this and ToEdge don't belong to the same spline.
      Edge *pPrevEdge = NULL;
      Edge *pCurrEdge = this;
      do
      {
         if (pCurrEdge == pToEdge)
         {
            if (pOutError)
            {
               *pOutError = "Edge(%1) belongs to the same spline as Edge(%2).  Terminate one of the edges first";
               pOutError->Replace("%1", (ULONG)GetID());
               pOutError->Replace("%2", (ULONG)pToEdge->GetID());
            }
            return NULL;
         }
         Edge *pTempEdge = pCurrEdge->GetContinuousEdge(pPrevEdge);
         pPrevEdge = pCurrEdge;
         pCurrEdge = pTempEdge;
      }
      while(pCurrEdge && pCurrEdge!=this);

      pPrevEdge = NULL;
      pCurrEdge = this;
      do
      {
         if (pCurrEdge == pToEdge)
         {
            if (pOutError)
            {
               *pOutError = "Edge(%1) belongs to the same spline as Edge(%2).  Terminate one of the edges first";
               pOutError->Replace("%1", (ULONG)GetID());
               pOutError->Replace("%2", (ULONG)pToEdge->GetID());
            }
            return NULL;
         }
         Edge *pTempEdge = pCurrEdge->GetContinuousEdge(pPrevEdge, TRUE);
         pPrevEdge = pCurrEdge;
         pCurrEdge = pTempEdge;
      }
      while(pCurrEdge && pCurrEdge!=this);
   }

   pInfo->Redo();

   static BOOL bInMirror;
   if (!bInMirror)
   {
      bInMirror = TRUE;
      PatchEdge *pMirrorThis = (PatchEdge *)GetMirrorObject();
      PatchEdge *pMirrorToEdge = (PatchEdge *)pToEdge->GetMirrorObject();
      if (pMirrorThis && pMirrorToEdge)
         pInfo->m_pMirrorInfo = pMirrorThis->Redirect( pMirrorToEdge, pOutError );
      bInMirror = FALSE;
   }
   return pInfo;
}

PatchEdge::RedirectInfo::RedirectInfo( PatchEdge *pFromEdge, PatchEdge *pToEdge, PatchVertex *pSharedVertex )
{
   m_pMirrorInfo = NULL;
   m_pFromEdge=pFromEdge;
   m_pToEdge=pToEdge;
   m_pSharedVertex=pSharedVertex;
   m_pOriginalToEdge = (PatchEdge *)pFromEdge->GetContinuousEdge(pSharedVertex);
   m_bReverseTo = FALSE;

   if (pToEdge)
   {
      if (pToEdge->IsInEdge(pFromEdge->GetPrevContVertex()))
      {
         if (pToEdge->GetNextContVertex()!=m_pSharedVertex)
            m_bReverseTo = TRUE;
      }
      else
      {
         ASSERT(pToEdge->IsInEdge(pFromEdge->GetNextContVertex()));
         if (pToEdge->GetPrevContVertex()!=m_pSharedVertex)
            m_bReverseTo = TRUE;
      }
   }
}

PatchEdge::RedirectInfo::~RedirectInfo()
{
   if (m_pMirrorInfo)
      delete m_pMirrorInfo;
}

void PatchEdge::RedirectInfo::Redo()
{
   if (m_bReverseTo)
      m_pToEdge->ReverseSpline();

   if (m_pFromEdge->GetPrevContVertex() == m_pSharedVertex)
      m_pFromEdge->SetPrevContEdge(m_pToEdge);
   else
   {
      ASSERT(m_pFromEdge->GetNextContVertex() == m_pSharedVertex);
      m_pFromEdge->SetNextContEdge(m_pToEdge);
   }

   if (m_pMirrorInfo)
      m_pMirrorInfo->Redo();
}

void PatchEdge::RedirectInfo::Undo()
{
   if (m_pMirrorInfo)
      m_pMirrorInfo->Undo();

   if (m_pFromEdge->GetPrevContVertex() == m_pSharedVertex)
      m_pFromEdge->SetPrevContEdge(m_pOriginalToEdge);
   else
   {
      ASSERT(m_pFromEdge->GetNextContVertex() == m_pSharedVertex);
      m_pFromEdge->SetNextContEdge(m_pOriginalToEdge);
   }

   if (m_bReverseTo)
      m_pToEdge->ReverseSpline();
}

////////////////////////////////////////////////////////////////////////////////

void PatchEdge::SetPrevContEdge(PatchEdge *pEdge)
{
   if (pEdge && pEdge->m_pPrevContEdge && pEdge->m_pNextContEdge && GetPrevContVertex()!=pEdge->GetNextContVertex()) // edges continuity are running in opposite directions
   {
      ASSERT(FALSE);
      return;
   }

   // clears previous
   if (m_pPrevContEdge)
   {
      for (PatchEdge *pCurrentEdge=this; pCurrentEdge; pCurrentEdge=pCurrentEdge->m_pPrevContEdge)
      {
         if (pCurrentEdge->IsContinuityLoop())
         {
            pCurrentEdge->SetContinuityLoop(FALSE);
            break;
         }
      }

      m_pPrevContEdge->m_pNextContEdge = NULL;
      m_pPrevContEdge->SetNextContinuityDirty( FALSE );
      m_pPrevContEdge->SetTangentsDirty( TRUE );
   }

   if (pEdge && pEdge->m_pNextContEdge)
   {
      for (PatchEdge *pCurrentEdge=pEdge; pCurrentEdge; pCurrentEdge=pCurrentEdge->m_pPrevContEdge)
      {
         if (pCurrentEdge->IsContinuityLoop())
         {
            pCurrentEdge->SetContinuityLoop( FALSE );
            break;
         }
      }
      pEdge->m_pNextContEdge->m_pPrevContEdge = NULL;
      pEdge->m_pNextContEdge->SetPrevContinuityDirty( FALSE );
      pEdge->m_pNextContEdge->SetTangentsDirty( TRUE );
   }

   // assigns
   m_pPrevContEdge = pEdge;
   SetPrevContinuityDirty( FALSE );
   SetTangentsDirty( TRUE );

   if (pEdge)
   {
      pEdge->m_pNextContEdge = this;
      pEdge->SetNextContinuityDirty( FALSE );
      pEdge->SetTangentsDirty( TRUE );
   }

   for (PatchEdge *pCurrentEdge=m_pPrevContEdge; pCurrentEdge; pCurrentEdge=pCurrentEdge->m_pPrevContEdge)
   {
      if (pCurrentEdge==this)
      {
         pCurrentEdge->SetContinuityLoop(TRUE);
         break;
      }
   }
}

void PatchEdge::SetNextContEdge(PatchEdge *pEdge)
{
   if (pEdge && pEdge->m_pPrevContEdge && pEdge->m_pNextContEdge && GetNextContVertex()!=pEdge->GetPrevContVertex()) // edges continuity are running in opposite directions
   {
      ASSERT(FALSE);
      return;
   }

   // clears previous
   if (m_pNextContEdge)
   {
      for (PatchEdge *pCurrentEdge=this; pCurrentEdge; pCurrentEdge=pCurrentEdge->m_pNextContEdge)
      {
         if (pCurrentEdge->IsContinuityLoop())
         {
            pCurrentEdge->SetContinuityLoop(FALSE);
            break;
         }
      }
      m_pNextContEdge->m_pPrevContEdge = NULL;
      m_pNextContEdge->SetPrevContinuityDirty( FALSE );
      m_pNextContEdge->SetTangentsDirty( TRUE );
   }

   if (pEdge && pEdge->m_pPrevContEdge)
   {
      for (PatchEdge *pCurrentEdge=pEdge; pCurrentEdge; pCurrentEdge=pCurrentEdge->m_pNextContEdge)
      {
         if (pCurrentEdge->IsContinuityLoop())
         {
            pCurrentEdge->SetContinuityLoop(FALSE);
            break;
         }
      }
      pEdge->m_pPrevContEdge->m_pNextContEdge = NULL;
      pEdge->m_pPrevContEdge->SetNextContinuityDirty( FALSE );
      pEdge->m_pPrevContEdge->SetTangentsDirty( TRUE );
   }

   // assigns
   m_pNextContEdge = pEdge;
   SetNextContinuityDirty( FALSE );
   SetTangentsDirty( TRUE );

   if (pEdge)
   {
      pEdge->m_pPrevContEdge = this;
      pEdge->SetPrevContinuityDirty( FALSE );
      pEdge->SetTangentsDirty( TRUE );
   }

   for (PatchEdge *pCurrentEdge=m_pNextContEdge; pCurrentEdge; pCurrentEdge=pCurrentEdge->m_pNextContEdge)
   {
      if (pCurrentEdge==this)
      {
         pCurrentEdge->SetContinuityLoop(TRUE);
         break;
      }
   }
}

void DrawFin( Vector pivot, Vector target,CGLBaseView *pBaseView,bool bFilled = TRUE)
{
   glPushMatrix();

   Matrix44 matrix = pBaseView->GetWorldToViewMatrix();
   GLfloat glmatrix[16];
   matrix.GetFloatMatrix( glmatrix, true );
   glLoadMatrixf( glmatrix );

   Vector dir = target - pivot;


   Vector start = dir * 0.1f + pivot;
   pBaseView->WorldToScreen(start);
   start.z = 0.0f;

   Vector end = target - dir *0.1f;
   target = end;

   pBaseView->WorldToScreen(end);
   end.z = 0.0f;

   if (bFilled)
      glBegin(GL_POLYGON);
   else
      glBegin(GL_LINE_LOOP);

   Vector finvect = end - start;
   finvect *= 0.05f;
   Vector v;
   v.y = (sin(90.0f * DEGTORAD)*finvect.x) + (cos(90.0f * DEGTORAD)*finvect.y);
   v.x = (cos(90.0f * DEGTORAD)*finvect.x) - (sin(90.0f * DEGTORAD)*finvect.y);
   v.z = 0.0f;

   Vector leftcorner;
   leftcorner = start + v;
   Vector rightcorner;
   rightcorner = start - v;

   glVertex3fv(&target.x);

   v = leftcorner;
   v.z = 0.0f;
   pBaseView->ScreenToWorld(v);  
   glVertex3fv(&v.x);

   v = rightcorner;
   v.z = 0.0f;
   pBaseView->ScreenToWorld(v);  
   glVertex3fv(&v.x);

   glEnd();

   glPopMatrix();
}

void PatchEdge::DrawEdgeContinuity( CGLBaseView *pBaseView )
{
   CGLModelView *pModelView = (CGLModelView*)pBaseView;
   if (!pModelView->m_bDisplayContinuity)
      return;

   if (GetPrevContEdge(TRUE) && !GetNextContEdge(TRUE))
      glColor4f(1.0f,0.0f,0.0f,1.0f);//red
   else if (GetNextContEdge(TRUE) && !GetPrevContEdge(TRUE))
      glColor4f(0.0f,0.0f,1.0f,1.0f);//blue
   else if (!GetNextContEdge(TRUE) && !GetPrevContEdge(TRUE))
      glColor4fv(colorOrange);
   else if (IsContinuityLoop())
      glColor4fv(colorYellow);
   else
      glColor4f(0.0f,1.0f,0.0f,1.0f);

   Vector edgevect;
   GetVector(edgevect);

   Vertex *pVertex0,*pVertex1;
   pVertex0 = GetPrevContVertex();
   pVertex1 = GetNextContVertex();

   Vector v0 = pVertex0->GetPos();
   Vector v1 = pVertex1->GetPos();

   DrawFin(v0,v1,pBaseView,TRUE);
}

void PatchEdge::SetFirstVertexPeakedFlag(BOOL bPeaked) // Don't call directly, call Vertex::Peak
{ 
   Edge::SetFirstVertexPeakedFlag(bPeaked);

   SetTangentsDirty(TRUE);

   // This case is for edge vetices like the cap on a cylinder.  The problem is the cap never got the tesselated polys
   // cleared because there is no spline running across it if it is an ngon
   Poly *pPoly = GetPoly();
   Edge *pContEdge = GetContinuousEdge(GetFirstVertex());
   if (!pContEdge && pPoly)
   {
      Edge *pEdge1, *pEdge2;
      GetFirstVertex()->GetEdgePair(pPoly, &pEdge1, &pEdge2);
      Edge *pEdge = (pEdge1==this)?pEdge2:pEdge1;//Border Edge for Edge knots
      
      Patch *pConnectedPatch = (Patch *)pEdge->GetConnectedPoly(pPoly);
      if (pConnectedPatch)
      {
         pConnectedPatch->SetTessellationAllocationDirty(TRUE);
         pConnectedPatch->SetTangentsDirty(TRUE);
      }
   }
}

void PatchEdge::SetSecondVertexPeakedFlag(BOOL bPeaked) // Don't call directly, call Vertex::Peak
{ 
   Edge::SetSecondVertexPeakedFlag(bPeaked);

   SetTangentsDirty(TRUE);

   // This case is for edge vertices like the cap on a cylinder.  The problem is the cap never got the tesselated polys
   // cleared because there is no spline running across it if it is an ngon
   Poly *pPoly = GetPoly();
   Edge *pContEdge = GetContinuousEdge(GetSecondVertex());
   if (!pContEdge && pPoly)
   {
      Edge *pEdge1, *pEdge2;
      GetSecondVertex()->GetEdgePair(pPoly, &pEdge1, &pEdge2);
      Edge *pEdge = (pEdge1==this)?pEdge2:pEdge1;//Border Edge for Edge knots

      Patch *pConnectedPatch = (Patch *)pEdge->GetConnectedPoly(pPoly);
      if (pConnectedPatch)
      {
         pConnectedPatch->SetTessellationAllocationDirty(TRUE);
         pConnectedPatch->SetTangentsDirty(TRUE);
      }
   }
}

BOOL PatchEdge::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
   case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if ( pProperty==m_firstVertexHandle.GetMagnitude() && GetFirstVertex() )
            GetFirstVertex()->SetDirty(); // We Dirty the vertex rather than the edge to handle constrained vertices properly
         if ( pProperty==m_secondVertexHandle.GetMagnitude() && GetSecondVertex() )
            GetSecondVertex()->SetDirty(); // We Dirty the vertex rather than the edge to handle constrained vertices properly
         break;
      }
   }

   return Component::OnObjectNotify( msg, wParam, lParam );
}


