#include "stdafx.h"
#include "PatchVertex.h"
#include "Patch.h"
#include "PatchModel.h"
#include "..\Utility\MathTools.h"
#include "PatchEdge.h"
#include "UVPatchEdge.h"
#include "..\Utility\ArrayTemplate.h"
#include "GUI\GLVertexHandleViewControl.h"
#include "GUI/BaseDoc.h"
#include "GUI/Snipe.h"
#include "GUI/GLBaseView.h"
#include "GUI/GLModelView.h"

IMPLEMENT_COMPONENT_CREATE( PatchVertex, Vertex )

PatchVertex::PatchVertex()
{
   m_pSecondarySurroundingPolys = NULL;
   m_nNumSecondarySurroundingFaces = 0;
   m_nNumMaxSecondarySurroundingFaces = 0;
   m_fUWeight = 0.0f;
   m_fVWeight = 0.0f;
}

PatchVertex::PatchVertex( UINT nID )
: Vertex( nID )
{
   m_pSecondarySurroundingPolys = NULL;
   m_nNumSecondarySurroundingFaces = 0;
   m_nNumMaxSecondarySurroundingFaces = 0;
   m_fUWeight = 0.0f;
   m_fVWeight = 0.0f;
}

PatchVertex::~PatchVertex()
{
   if (m_pSecondarySurroundingPolys)
   {
      DeconstructArray<TessPoly*>(m_pSecondarySurroundingPolys,m_nNumSecondarySurroundingFaces);
      delete[] (BYTE*)m_pSecondarySurroundingPolys;
   }
}

BOOL PatchVertex::Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError )
{
   PatchEdge *pV0ContEdge = NULL;
   PatchEdge *pV1ContEdge = NULL;
   PatchVertex *pV0 = NULL;
   PatchVertex *pV1 = NULL;
   BOOL bV0ContIsPrev;
   BOOL bV1ContIsPrev;

   if (pDeleteInfo==NULL && GetNumSurroundingEdges() == 2)
   {
      PatchEdge *pEdge0 = (PatchEdge *)GetSurroundingEdgeAt(0);
      PatchEdge *pEdge1 = (PatchEdge *)GetSurroundingEdgeAt(1);

      pV0 = (PatchVertex *)pEdge0->GetConnectedVertex(this);
      pV1 = (PatchVertex *)pEdge1->GetConnectedVertex(this);

      pV0ContEdge = (PatchEdge *)pEdge0->GetContinuousEdge(pV0);
      if (pV0ContEdge)
         bV0ContIsPrev = pEdge0->GetPrevContVertex() == pV0;

      if (pEdge0->GetPrevContVertex() != pEdge1->GetPrevContVertex()
         && pEdge0->GetNextContVertex() != pEdge1->GetNextContVertex()) // Not two separate splines running opposite directions
      {
         pV1ContEdge = (PatchEdge *)pEdge1->GetContinuousEdge(pV1);
         if (pV1ContEdge)
            bV1ContIsPrev = pEdge1->GetPrevContVertex() == pV1;
      }
   }

   if (!Vertex::Delete( pDeleteInfo, strOutError ))
      return FALSE;

   PatchEdge *pNewEdge = (PatchEdge *)((Vertex::DeleteInfo *)pDeleteInfo)->GetNewEdge();
   if (pNewEdge)
   {
      if (pV0ContEdge && !pV0ContEdge->IsInList()) // Mirror mode next to axis
         pV0ContEdge = (PatchEdge *)pNewEdge->Edge::GetContinuousEdge(pV0);

      if (pV0ContEdge)
      {
         if (bV0ContIsPrev)
            pNewEdge->SetPrevContEdge(pV0ContEdge);
         else
            pNewEdge->SetNextContEdge(pV0ContEdge);
      }

      if (pV1ContEdge && !pV1ContEdge->IsInList()) // Mirror mode next to axis
         pV1ContEdge = (PatchEdge *)pNewEdge->Edge::GetContinuousEdge(pV1);

      if (pV1ContEdge)
      {
         if (bV1ContIsPrev)
            pNewEdge->SetPrevContEdge(pV1ContEdge);
         else
            pNewEdge->SetNextContEdge(pV1ContEdge);
      }
   }

   return TRUE;
}

void PatchVertex::Draw( CGLBaseView *pView )
{
   Vertex::Draw( pView );

   if (IsConstrained())
   {
      glPointSize( VERTEXSIZE-2 );
      glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
      glBegin( GL_POINTS );
      glVertex3fv( &GetPos().x );
      glEnd();
      glPointSize( VERTEXSIZE );
   }
}

const Vector &PatchVertex::GetPos()
{
   if (IsConstrained())
      ComputeConstrainedPos();

   m_bSetDirtyCalled = FALSE;

   return m_pos;
}

void PatchVertex::DirtyDependentEdges(Edge *pNotThis/*=NULL*/)
{
   for (UINT i = 0; i < GetNumSurroundingEdges(); i++)
   {
      PatchEdge *pEdge = (PatchEdge *)GetSurroundingEdgeAt(i);
      if (pNotThis && pNotThis==pEdge)
         continue;

      pEdge->SetDirty();

      Vertex *pVertex = (PatchVertex *)pEdge->GetConnectedVertex(this);
      if (pVertex && pVertex->IsConstrained())
         pVertex->SetDirty();
   }
}

BOOL PatchVertex::GetTwoContinuousUnConstrainedVertices( PatchEdge *pEdge, PatchVertex *&pOutPrevVertex, PatchVertex *&pOutNextVertex, PatchEdge *&pOutPrevEdge, PatchEdge *&pOutNextEdge, float &pOutConstrainedPos )
{
   int nConstrainedCount = 1; // consider ourselves constrained
   int nPos = 1;

   if (pEdge->GetPrevContVertex() == this)
   {
      pOutNextEdge = pEdge;
      pOutPrevEdge = pEdge->GetPrevContEdge(TRUE);
   }
   else
   {
      pOutNextEdge = pEdge->GetNextContEdge(TRUE);
      pOutPrevEdge = pEdge;
   }

   if (!pOutPrevEdge || !pOutNextEdge)
      return FALSE;

   pOutPrevVertex = (PatchVertex *)pOutPrevEdge->GetConnectedVertex( this );
   while(pOutPrevVertex && pOutPrevVertex->IsConstrained())
   {
      pOutPrevEdge = pOutPrevEdge->GetPrevContEdge(TRUE);
      if (!pOutPrevEdge)
      {
         pOutPrevVertex = NULL;
         break;
      }
      pOutPrevVertex = (PatchVertex *)pOutPrevEdge->GetConnectedVertex( pOutPrevVertex );
      nConstrainedCount++;
      nPos++;
   }

   pOutNextVertex = (PatchVertex *)pOutNextEdge->GetConnectedVertex( this );
   while(pOutNextVertex && pOutNextVertex->IsConstrained())
   {
      pOutNextEdge = pOutNextEdge->GetNextContEdge(TRUE);
      if (!pOutNextEdge)
      {
         pOutNextVertex = NULL;
         break;
      }
      pOutNextVertex = (PatchVertex *)pOutNextEdge->GetConnectedVertex( pOutNextVertex );
      nConstrainedCount++;
   }

   if (!pOutPrevVertex || !pOutNextVertex)
      return FALSE;

   pOutConstrainedPos = (1.0f / (nConstrainedCount+1)) * nPos;

   return TRUE;
}

BOOL PatchVertex::CanConstrain( PatchEdge *pEdge )
{
   return pEdge->GetContinuousEdge(this)!=NULL;
}

BOOL PatchVertex::SetConstrained( PatchEdge *pEdge )
{
   if (IsConstrained())
   {
      PatchEdge *pEdge0, *pEdge1;
      BOOL bResult = GetConstrainedBetween(pEdge0, pEdge1);
      ASSERT(bResult);

      if (pEdge0->GetFirstVertex() == this)
         pEdge0->SetFirstVertexConstrainedFlag(FALSE);
      else
      {
         ASSERT(pEdge0->GetSecondVertex() == this);
         pEdge0->SetSecondVertexConstrainedFlag(FALSE);
      }

      if (pEdge1->GetFirstVertex() == this)
         pEdge1->SetFirstVertexConstrainedFlag(FALSE);
      else
      {
         ASSERT(pEdge1->GetSecondVertex() == this);
         pEdge1->SetSecondVertexConstrainedFlag(FALSE);
      }
      m_bContrained = FALSE;
   }

   if (pEdge)
   {
      PatchEdge *pEdge0 = pEdge;
      PatchEdge *pEdge1 = (PatchEdge *)pEdge0->GetContinuousEdge(this); // should realy use property tolerance

      if (!pEdge1)
         return FALSE;

      if (pEdge0->GetFirstVertex() == this)
         pEdge0->SetFirstVertexConstrainedFlag(TRUE);
      else
      {
         ASSERT(pEdge0->GetSecondVertex() == this);
         pEdge0->SetSecondVertexConstrainedFlag(TRUE);
      }

      if (pEdge1->GetFirstVertex() == this)
         pEdge1->SetFirstVertexConstrainedFlag(TRUE);
      else
      {
         ASSERT(pEdge1->GetSecondVertex() == this);
         pEdge1->SetSecondVertexConstrainedFlag(TRUE);
      }
   }

   m_bContrained = pEdge!=NULL;
   ((PatchModel *)GetModel())->CreateConstrainedRTVertices();

   SetDirty();

   return TRUE;
}

BOOL PatchVertex::GetConstrainedBetween( PatchEdge *&pEdge0, PatchEdge *&pEdge1)
{
   if (!IsConstrained())
      return FALSE;

   for (UINT i=0; i<GetNumSurroundingEdges(); i++)
   {
      PatchEdge *pEdge = (PatchEdge *)GetSurroundingEdgeAt(i);
      if (pEdge->IsVertexConstrained(this))
      {
         pEdge0 = pEdge;
         pEdge1 = (PatchEdge *)pEdge->GetContinuousEdge(this); // should use model tolerance here
         return pEdge0 && pEdge1;
      }
   }

   pEdge0=NULL;
   pEdge1=NULL;
   return FALSE;
}

BOOL PatchVertex::GetConstrainedBetween( PatchVertex *&pVertex0, PatchVertex *&pVertex1)
{
   PatchEdge *pEdge0, *pEdge1;
   if (!GetConstrainedBetween(pEdge0, pEdge1))
      return FALSE;

   pVertex0 = (PatchVertex *)pEdge0->GetConnectedVertex(this);
   pVertex1 = (PatchVertex *)pEdge1->GetConnectedVertex(this);

   return TRUE;
}

BOOL PatchVertex::ComputeConstrainedPos()
{
   ASSERT( IsConstrained() );

   PatchVertex *pCV0, *pCV1;
   PatchEdge *pCEdge0, *pCEdge1;
   float fConstrainedPos;

   if (!GetConstrainedBetween(pCEdge0, pCEdge1))
      return FALSE;

   if (!GetTwoContinuousUnConstrainedVertices( pCEdge0, pCV0, pCV1, pCEdge0, pCEdge1, fConstrainedPos ))
      return FALSE;

   ASSERT(!pCV0->IsConstrained());
   ASSERT(!pCV1->IsConstrained());

   float fUnused;
   PatchVertex *pV0, *pTempVertex;
   PatchEdge *pEdge0, *pTempEdge;
   if (!pCV0->GetTwoContinuousUnConstrainedVertices( pCEdge0, pV0, pTempVertex, pEdge0, pTempEdge, fUnused ))
      pV0=NULL;

   PatchVertex *pV1;
   PatchEdge *pEdge1;
   if (!pCV1->GetTwoContinuousUnConstrainedVertices( pCEdge1, pTempVertex, pV1, pTempEdge, pEdge1, fUnused ))
      pV1=NULL;

   // pV0  pCV0  (this)  pCV1  pV1  (All are unconstrained vertices except this)
   if (pCEdge0->IsVertexPeaked(pCV0))
      pV0 = NULL;

   if (pCEdge1->IsVertexPeaked(pCV1))
      pV1 = NULL;

   Vector vPrevTangent;
   float fTangentLength;

   if (pV0)
   {
      vPrevTangent = pCV1->GetPos() - pV0->GetPos(); 
      float fPrevLength = (pV0->GetPos() - pCV0->GetPos()).Length();
      float fNextLength = (pCV1->GetPos() - pCV0->GetPos()).Length();
      float fRatio = fNextLength / (fPrevLength + fNextLength);
      fTangentLength = vPrevTangent.Length() * fRatio;
   }
   else
   {
      vPrevTangent = pCV1->GetPos() - pCV0->GetPos();
      fTangentLength = vPrevTangent.Length() * 0.5f;
   }

   VertexHandle *pHandle1 = pCEdge0->GetVertexHandle(pCV0);
   VertexHandle *pHandle2 = pCEdge1->GetVertexHandle(pCV1);

   Vector vTangent0;
   Vector vTangent1;
   MathTools::ComputeTangents(pV0 ? &pV0->GetPos() : NULL, pCV0->GetPos(), pCV1->GetPos(), pV1 ? &pV1->GetPos() : NULL, 
    &vTangent0, &vTangent1,
    pHandle1->m_fAlpha, pHandle1->m_fGamma, pHandle1->m_fMagnitude,
    pHandle2->m_fAlpha, pHandle2->m_fGamma, pHandle2->m_fMagnitude);

   Vector v0 = pCV0->GetPos();
   Vector cv0 = v0 + (vTangent0 * .33333333);
   Vector v1 = pCV1->GetPos();
   Vector cv1 = v1 + (vTangent1 * .33333333);

   SetPos( MathTools::Bezier( fConstrainedPos, v0, cv0, cv1, v1 ), FALSE, FALSE );

   return TRUE;
}

void PatchVertex::AddSecondarySurroundingPoly( TessPoly *pPoly )
{
   UINT nIndex = m_nNumSecondarySurroundingFaces;
   UINT nSize = m_nNumSecondarySurroundingFaces;
   UINT nMaxSize = m_nNumMaxSecondarySurroundingFaces;
   SetArraySize<TessPoly*>(&m_pSecondarySurroundingPolys,nSize+1,nSize,nMaxSize,4);
   ASSERT(nMaxSize<256);
   m_nNumSecondarySurroundingFaces = nSize;
   m_nNumMaxSecondarySurroundingFaces = nMaxSize;
   m_pSecondarySurroundingPolys[nIndex] = pPoly;
}

void PatchVertex::RemoveSecondarySurroundingPolyAt( UINT nIndex )
{
   UINT nSize = m_nNumSecondarySurroundingFaces;
   RemoveArrayElements<TessPoly*>(m_pSecondarySurroundingPolys,nIndex,nSize);
   m_nNumSecondarySurroundingFaces = nSize;
}

void PatchVertex::Compact()
{
   Vertex::Compact();
   UINT nMaxSize = m_nNumMaxSecondarySurroundingFaces;
   FreeArrayExtra<TessPoly*>(&m_pSecondarySurroundingPolys,m_nNumSecondarySurroundingFaces,nMaxSize);
   m_nNumMaxSecondarySurroundingFaces = nMaxSize;
}

void PatchVertex::CalcSecondaryNormal(VertexNormal *pVertexNormal)
{
   Vector n(0,0,0);
   UINT i,j;
   TessPoly *pPoly;
   for (i = 0; i < GetNumSecondarySurroundingPolys(); i++)
   {
      pPoly = GetSecondarySurroundingPolyAt(i);
      for (j = 0; j < pPoly->GetNumSides(); j++)
      {
         if (pPoly->GetVertexNormalAt( j, FALSE ) == pVertexNormal) // to see if it is shared
         {
            n += *pPoly->GetFaceNormal();
            break;
         }
      }
   }
   n.Normalize();
   pVertexNormal->SetDir(n);
}
/*
// this function iterates through surrounding faces gathering uvs
void PatchVertex::GatherSecondaryUVs(CArray< CArray<UV*> > &uvs)
{
   uvs.RemoveAll();

   UINT numFaces = GetNumSecondarySurroundingPolys();
   if (numFaces == 0)
      return;

   uvs.SetSize(numFaces);

   UINT nFaceIndex;
   for (nFaceIndex = 0; nFaceIndex < numFaces; nFaceIndex++)
   {
      TessPoly *pPoly = GetSecondarySurroundingPolyAt(nFaceIndex);
      int facerelativeindex = pPoly->FindVertexFaceRelativeIndex(this);
      if (facerelativeindex<0)
         continue;
      UINT nNumTessUVPolys = pPoly->CountNumTessUVPolys();

      uvs[nFaceIndex].SetSize(nNumTessUVPolys);

      UINT nCount = 0;
      for (TessUVPoly *pTessUVPoly = pPoly->GetHeadTessUVPoly(); pTessUVPoly; pTessUVPoly=pTessUVPoly->GetNextTessUVPoly(), nCount++)
      {
         UV *pUV = pTessUVPoly->GetUVAt(facerelativeindex);
         uvs[nFaceIndex].SetAt(nCount, pUV);
      }
   }
}
*/
BOOL PatchVertex::GetTangents(Poly *pPoly,Vector &v0,Vector &v1)
{
   PatchEdge *pPrevEdge, *pNextEdge;
   GetEdgePair(pPoly,(Edge**)&pPrevEdge,(Edge**)&pNextEdge);

   if (pPrevEdge->GetFirstVertex() == this)
      v0 = pPrevEdge->GetFirstTangentPos();
   else
      v0 = pPrevEdge->GetSecondTangentPos();

   if (pNextEdge->GetFirstVertex() == this)
      v1 = pNextEdge->GetFirstTangentPos();
   else
      v1 = pNextEdge->GetSecondTangentPos();

   return TRUE;
}

void PatchVertex::SetDirty()
{
   if (m_bSetDirtyCalled)
      return;

   m_bSetDirtyCalled = TRUE;

   for (UINT i = 0; i < GetNumSurroundingEdges(); i++)
   {
      Edge *pEdge = GetSurroundingEdgeAt(i);

      if (pEdge->GetFirstVertex()) // yes this is possible when SetDirty gets called in Edge::SetFirstVertex and SetSecondVertex, it is only bad for a few lines of code, but appropriate... I think.. DAN
         pEdge->GetFirstVertex()->DirtyDependentEdges();

      if (pEdge->GetSecondVertex())
         pEdge->GetSecondVertex()->DirtyDependentEdges(pEdge);
   }

   for (UINT i=0; i<GetNumSecondarySurroundingPolys(); i++)
   {
      TessPoly *pTessPoly = GetSecondarySurroundingPolyAt( i );
      pTessPoly->SetDirtyFaceNormal( TRUE );
   }
}

void PatchVertex::OnFocusChanged( BOOL bFocus )
{
   Vertex::OnFocusChanged( bFocus );

   SnipeObject *pSnipeObject = SelectionList::GetNearestFocusObject();
   if (!pSnipeObject)
      return;

   if (pSnipeObject->GetSOT()!=SOT_PatchEdge)
      return;

   PatchEdge *pPatchEdge = (PatchEdge*)pSnipeObject;

   for (POSITION docpos=GetApp().GetDocList().GetHeadPosition(); docpos; )
   {
      CBaseDoc *pDoc = GetApp().GetDocList().GetNext( docpos );

      if (pDoc->UsesObject( this ))
      {
         for (POSITION viewpos=pDoc->m_viewlist.GetHeadPosition(); viewpos; )
         {
            CGLBaseView *pView = pDoc->m_viewlist.GetNext( viewpos );

            if (!((CGLModelView*)pView)->m_bDisplayControlHandles.GetValue())
               continue;

            if (bFocus)
            {
               SNEW CGLVertexHandleViewControl( pView, pPatchEdge, this );
            }
            else
            {
               POSITION controlpos=pView->GetViewControlList().GetHeadPosition();
               while (controlpos)
               {
                  for (controlpos=pView->GetViewControlList().GetHeadPosition(); controlpos; )
                  {
                     CGLViewControl *pControl = pView->GetViewControlList().GetNext( controlpos );
                     if (pControl->IsFor( pPatchEdge->GetVertexHandle(this) ))
                     {
                        delete pControl;
                        break;
                     }
                  }
               }
            }
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
Vertex::WeldInfo *PatchVertex::AllocWeldInfo()
{
   return SNEW PatchVertex::WeldInfo();
}

void PatchVertex::WeldInfo::ReWeld()
{
   PatchEdge *pEdge = (PatchEdge *)m_pWeldedEdge;
   PatchEdge *pPrevEdge = pEdge->GetPrevContEdge(TRUE);
   PatchEdge *pNextEdge = pEdge->GetNextContEdge(TRUE);

   Vertex::WeldInfo::ReWeld();

   if (pPrevEdge && pNextEdge && pPrevEdge->IsInList() && pNextEdge->IsInList())
      pPrevEdge->SetNextContEdge( pNextEdge );
}

void PatchVertex::WeldInfo::UnWeld()
{
//   PatchEdge *pEdge = (PatchEdge *)m_pWeldedEdge;
//   PatchEdge *pPrevEdge = pEdge->GetPrevContEdge(TRUE);
//   PatchEdge *pNextEdge = pEdge->GetNextContEdge(TRUE);
//
//   if (pPrevEdge && pNextEdge && pPrevEdge->IsInList() && pNextEdge->IsInList())
//   {
//      pPrevEdge->SetNextContEdge( pEdge );
//      pNextEdge->SetPrevContEdge( pEdge );
//   }
   // The reconnect of m_pWeldedEdge will handle continuity

   Vertex::WeldInfo::UnWeld();
}
