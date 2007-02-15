#include "stdafx.h"
#include "GUI/GL3DView.h"
#include "Vertex.h"
#include "Poly.h"
#include "Edge.h"
#include "PropertySet.h"
#include "TransformableObject.h"
#include "..\Utility\Matrix44.h"
#include "Patch.h"
#include "PatchEdge.h"
#include "PolyModel.h"
#include "GUI/GLModelView.h"
#include "Interface/GLHitArray.h"
#include "Bone.h"
#include "gui\Mainfrm.h"
#include "..\Utility\ArrayTemplate.h"

extern CMainFrame *g_pMainFrame;

IMPLEMENT_COMPONENT_CREATE( Vertex, Vertex )

TranslateProperty::Info Vertex::m_vPositionInfo( "Position", "Position" );

Vertex::Vertex()
{
   Init();
}

Vertex::Vertex( UINT nID )
: Component( nID )
{
   Init();
}

void Vertex::Init()
{
   m_nVertexFlags = 0;
   m_pRTVertexHead = NULL;
   m_nNumSurroundingFaces = 0;
   m_nNumMaxSurroundingFaces = 0;
   m_pPrimarySurroundingPolys = NULL;
   m_nNumSurroundingEdges = 0;
   m_nNumMaxSurroundingEdges = 0;
   m_pSurroundingEdges = NULL;
   m_nNumBones = 0;
   m_nNumMaxBones = 0;
   m_pBones = NULL;
}

Vertex::~Vertex()
{
   if (m_pPrimarySurroundingPolys)
   {
      DeconstructArray<Poly*>(m_pPrimarySurroundingPolys,m_nNumSurroundingFaces);
      delete[] (BYTE*)m_pPrimarySurroundingPolys;
   }
   if (m_pSurroundingEdges)
   {
      DeconstructArray<Edge*>(m_pSurroundingEdges,m_nNumSurroundingEdges);
      delete[] (BYTE*)m_pSurroundingEdges;
   }
   if (m_pBones)
   {
      DeconstructArray<Bone*>(m_pBones,m_nNumBones);
      delete[] (BYTE*)m_pBones;
   }
}

void Vertex::PreDestruct()
{
   Component::PreDestruct();
}

void Vertex::PostConstruct()
{
   Component::PostConstruct();
 //  Reference();
   SetDirty(); 
   m_vPosition.Init( &m_vPositionInfo, this );
}

Vector Vertex::GetWorldPos(TransformableObject *pTransformObject)
{
   return pTransformObject->GetObjectToWorldMatrix() * GetPos();
}

void Vertex::SetRTVertexPos( const Vector &vPos )
{
   if (m_pRTVertexHead) m_pRTVertexHead->SetPos( vPos );
   if (GetModel()) GetModel()->SetBoundingSphereDirty( TRUE );
}

const Vector &Vertex::GetPos()
{
   return m_pos;
}

void Vertex::Transform( const Matrix44 &matrix )
{
   if (m_bTransformed || IsLocked())
      return;

   SetPos( matrix * GetPos() );

   m_bTransformed = TRUE;
}

BOOL Vertex::GetWorldPos( Vector &pos )
{
   Vertex *pVertex = (Vertex*)this;//sorry about the hack
   pos = pVertex->GetPos();

   return TRUE;
}

void Vertex::GetViewPos( CGLBaseView *pView, Vector &pos )
{
   GetWorldPos( pos );
   pos *= pView->GetWorldToViewMatrix();
}

Vector Vertex::GetViewPos( CGLBaseView *pView )
{
   Vector pos;
   GetWorldPos( pos );
   pos *= pView->GetWorldToViewMatrix();
   return pos;
}

void Vertex::SetPos( const Vector &pos, BOOL bDoMirror/*=TRUE*/, BOOL bDirty/*=TRUE*/ )
{
   Vertex *pMirror = NULL;
   
   if (!IsConstrained())
      pMirror = (Vertex *)GetMirrorObject(); // Always call!

   if (bDoMirror && pMirror==this)
   {
      Vector vPos(GetPos().x, pos.y, pos.z );
      m_pos = vPos;
      SetRTVertexPos( vPos );
   }
   else
   {
      m_pos = pos;
      SetRTVertexPos( pos );
   }

   m_vPosition.SetValue( pos );

   if (IsSelected())
      g_pMainFrame->GetMainInterface()->GetCurrentSelectionList()->SetBoundsDirty(); // faster to use global

   if (bDirty)
      SetDirty();

   PolyModel *pModel = GetModel();
   if (pModel)
      pModel->SetBoundingSphereDirty();
   if (pMirror && bDoMirror && pMirror!=this && !pMirror->IsLocked())
      pMirror->SetPos( Vector(-pos.x, pos.y, pos.z), FALSE );
}

Vector2 Vertex::GetViewPos(TransformableObject *pTransformObject, CGL3DView *pView)
{
   Vector2 position;
   pView->GetProjectionFromWorld( pTransformObject->GetObjectToWorldMatrix() * GetPos(), position );
   return position;
}

void Vertex::CalcNormal( VertexNormal *pVertexNormal )
{
   Vector n( 0.0f );
   for (UINT i = 0; i < GetNumSurroundingPolys(); i++)
   {
      Poly *pPoly = GetSurroundingPolyAt(i);
      for (UINT j = 0; j < pPoly->GetNumSides(); j++)
      {
         if (pPoly->GetVertexNormalAt(j, FALSE) == pVertexNormal)
         {
            n += *pPoly->GetFaceNormal();
            break;
         }
      }
   }
   n.Normalize();
   pVertexNormal->SetDir(n); // clears dirty bit
}

Edge *Vertex::FindNearestEdge( CGLModelView *pView, const CPoint &pt )
{
   pView->SetDoPickLimitComponent( this );
   PickHitArray pickHitArray;
   pView->DoPick( CRect(pt, pt), pickHitArray,NULL, SOT_Edge );
   pView->SetDoPickLimitComponent( NULL );

   Edge *pEdge = NULL;

   if ( pickHitArray.GetNumHits()==1)
      pEdge = (Edge *)pickHitArray[0][0];

   return pEdge;
}

Poly *Vertex::FindNearestFace( CGLModelView *pView, const CPoint &pt )
{
   pView->SetDoPickLimitComponent( this );
   PickHitArray pickHitArray;
   pView->DoPick( CRect(pt, pt), pickHitArray, NULL, SOT_Poly );
   pView->SetDoPickLimitComponent( NULL );

   Poly *pPoly = NULL;

   if ( pickHitArray.GetNumHits()==1)
      pPoly = (Poly *)pickHitArray[0][0];

   return pPoly;
}

void Vertex::AddSurroundingEdge( Edge *pEdge )
{
   if (!m_bInList)
      ReInsertIntoList();

   UINT nIndex = m_nNumSurroundingEdges;
   UINT nSize = m_nNumSurroundingEdges;
   UINT nMaxSize = m_nNumMaxSurroundingEdges;
   SetArraySize<Edge*>(&m_pSurroundingEdges,nSize+1,nSize,nMaxSize,4);
   ASSERT(nMaxSize<256);
   m_nNumSurroundingEdges = nSize;
   m_nNumMaxSurroundingEdges = nMaxSize;
   m_pSurroundingEdges[nIndex] = pEdge;

   SetDirty();
}

void Vertex::RemoveSurroundingEdgeAt( UINT nIndex )
{
   UINT nSize = m_nNumSurroundingEdges;
   RemoveArrayElements<Edge*>(m_pSurroundingEdges,nIndex,nSize);
   m_nNumSurroundingEdges = nSize;

   SetDirty();

   if (!IsUsed())
      RemoveFromList();
}

void Vertex::AddSurroundingPoly( Poly *pPoly )
{
   UINT nIndex = m_nNumSurroundingFaces;
   UINT nSize = m_nNumSurroundingFaces;
   UINT nMaxSize = m_nNumMaxSurroundingFaces;
   SetArraySize<Poly*>(&m_pPrimarySurroundingPolys,nSize+1,nSize,nMaxSize,4);
   ASSERT(nMaxSize<256);
   m_nNumSurroundingFaces = nSize;
   m_nNumMaxSurroundingFaces = nMaxSize;
   m_pPrimarySurroundingPolys[nIndex] = pPoly;

   // when a poly is added to a vertex, it's normal will need recalculated.
   int nVertexIndex = pPoly->FindVertexFaceRelativeIndex( this );
   if (pPoly->GetVertexNormalAt(nVertexIndex, FALSE))
      pPoly->GetVertexNormalAt(nVertexIndex, FALSE)->SetVertexNormalDirtyFlag( TRUE );
}

void Vertex::RemoveSurroundingPolyAt(UINT nIndex )
{
   UINT nSize = m_nNumSurroundingFaces;
   RemoveArrayElements<Poly*>(m_pPrimarySurroundingPolys,nIndex,nSize);
   m_nNumSurroundingFaces = nSize;
}

void Vertex::AddBone( Bone *pBone )
{
   UINT nIndex = m_nNumBones;
   UINT nSize = m_nNumBones;
   UINT nMaxSize = m_nNumMaxBones;
   SetArraySize<Bone*>(&m_pBones,nSize+1,nSize,nMaxSize);
   ASSERT(nMaxSize<256);
   m_nNumBones = nSize;
   m_nNumMaxBones = nMaxSize;
   m_pBones[nIndex] = pBone;
}

void Vertex::RemoveBoneAt( UINT nIndex )
{
   UINT nSize = m_nNumBones;
   RemoveArrayElements<Bone*>(m_pBones,nIndex,nSize);
   m_nNumBones = nSize;
}

void Vertex::Compact()
{
   UINT nMaxSize = m_nNumMaxSurroundingFaces;
   FreeArrayExtra<Poly*>(&m_pPrimarySurroundingPolys,m_nNumSurroundingFaces,nMaxSize);
   m_nNumMaxSurroundingFaces = nMaxSize;

   nMaxSize = m_nNumMaxSurroundingEdges;
   FreeArrayExtra<Edge*>(&m_pSurroundingEdges,m_nNumSurroundingEdges,nMaxSize);
   m_nNumMaxSurroundingEdges = nMaxSize;

   nMaxSize = m_nNumMaxBones;
   FreeArrayExtra<Bone*>(&m_pBones,m_nNumBones,nMaxSize);
   m_nNumMaxBones = nMaxSize;
}

Vector Vertex::CalcSoftNormal()
{
   Vector n( 0.0f );
   for (UINT i = 0; i < GetNumSurroundingPolys(); i++)
   {
      Poly *pPoly = GetSurroundingPolyAt(i);
      for (UINT j = 0; j < pPoly->GetNumSides(); j++)
         n += *pPoly->GetFaceNormal();
   }
   return n.Normalized();
}

VertexNormal *Vertex::DoesShareVertexNormal(Poly *pPolyA,Poly *pPolyB)
{
   int nIndex0 = pPolyA->FindVertexFaceRelativeIndex(this);	
   int nIndex1 = pPolyB->FindVertexFaceRelativeIndex(this);
   if (nIndex0<0)
      return NULL;
   if (nIndex1<0)
      return NULL;
   VertexNormal *vn0 = pPolyA->GetVertexNormalAt(nIndex0);
   VertexNormal *vn1 = pPolyB->GetVertexNormalAt(nIndex1);
   if (vn0 == vn1)
      return vn0;
   return NULL;
}

Edge *Vertex::GetPrevEdgeClockwise(Edge *pMatchEdge)
{
   UINT nNumEdges = GetNumSurroundingEdges();
   BOOL bUsePolys = TRUE;

   // Check to see if every edge has two polys, if so use polys to walk in order
   for ( UINT i=0; i < nNumEdges; i++ )
   {
      Edge *pEdge = GetSurroundingEdgeAt(i);
      if ( pEdge->GetFirstPoly()==NULL || pEdge->GetSecondPoly()==NULL)
      {
         bUsePolys = FALSE;
         break;
      }
   }

   if (bUsePolys)
   {
      for (UINT i = 0; i < GetNumSurroundingPolys(); i++)
      {
         Poly *pSurroundingPoly = GetSurroundingPolyAt(i);
         Edge *pPrevEdge, *pNextEdge;
         GetEdgePair(pSurroundingPoly,&pPrevEdge,&pNextEdge);
         if (pNextEdge==pMatchEdge)
            return pPrevEdge;
      }
   }
   else
   {
      for ( UINT i=0; i < nNumEdges; i++ )
      {
         Edge *pEdge = GetSurroundingEdgeAt(i);
         if (pEdge==pMatchEdge)
         {
            if (i)
               return GetSurroundingEdgeAt(i-1);
            else
               return GetSurroundingEdgeAt(nNumEdges-1);
         }
      }
   }

   return NULL;
}

Edge *Vertex::GetNextEdgeClockwise(Edge *pMatchEdge)
{
   UINT nNumEdges = GetNumSurroundingEdges();
   BOOL bUsePolys = TRUE;

   // Check to see if every edge has two polys, if so use polys to walk in order
   for ( UINT i=0; i < nNumEdges; i++ )
   {
      Edge *pEdge = GetSurroundingEdgeAt(i);
      if ( pEdge->GetFirstPoly()==NULL || pEdge->GetSecondPoly()==NULL)
      {
         bUsePolys = FALSE;
         break;
      }
   }

   if (bUsePolys)
   {
      for (UINT i = 0; i < GetNumSurroundingPolys(); i++)
      {
         Poly *pSurroundingPoly = GetSurroundingPolyAt(i);
         Edge *pPrevEdge, *pNextEdge;
         GetEdgePair(pSurroundingPoly,&pPrevEdge,&pNextEdge);
         if (pPrevEdge==pMatchEdge)
            return pNextEdge;
      }
   }
   else
   {
      for ( UINT i=0; i < nNumEdges; i++ )
      {
         Edge *pEdge = GetSurroundingEdgeAt(i);
         if (pEdge==pMatchEdge)
         {
            if (i == nNumEdges-1)
               return GetSurroundingEdgeAt(0);
            else
               return GetSurroundingEdgeAt(i+1);
         }
      }
   }

   return NULL;
}

// these might need better logic, but for now it works for me :-) DT
Poly *Vertex::GetPrevFaceClockwise( Poly *pMatchFace )
{
   UINT nNumFaces = GetNumSurroundingPolys();

   for ( UINT i=0; i < nNumFaces; i++ )
   {
      Poly *pFace = GetSurroundingPolyAt(i);
      if (pFace==pMatchFace)
      {
         if (i)
            return GetSurroundingPolyAt(i-1);
         else
            return GetSurroundingPolyAt(nNumFaces-1);
      }
   }
   return NULL;
}

Poly *Vertex::GetNextFaceClockwise( Poly *pMatchFace )
{
   UINT nNumFaces = GetNumSurroundingPolys();

   for ( UINT i=0; i < nNumFaces; i++ )
   {
      Poly *pFace = GetSurroundingPolyAt(i);
      if (pFace==pMatchFace)
      {
         if (i == nNumFaces-1)
            return GetSurroundingPolyAt(0);
         else
            return GetSurroundingPolyAt(i+1);
      }
   }
   return NULL;
}

void Vertex::DirtyDependentFaces()
{
   for (UINT i = 0; i < GetNumSurroundingPolys(); i++)
      GetSurroundingPolyAt(i)->SetDirty();
}

void Vertex::DirtyDependentEdges(Edge *pNotThis/*=NULL*/)
{
   for (UINT i = 0; i < GetNumSurroundingEdges(); i++)
   {
      Edge *pEdge = GetSurroundingEdgeAt(i);
      
      if (pNotThis && pNotThis==pEdge)
         continue;

      pEdge->SetDirty();
   }
}

SnipeObject *Vertex::FindMirrorObject()
{
   PolyModel *pModel = (PolyModel *)GetOwner(); 
   if (!pModel)
      return NULL;

   Vector vMatch = GetPos();
   vMatch.x = -vMatch.x;

   Vector vDelta;
   for (POSITION pos=pModel->GetHeadVertexPosition(); pos; )
   {
      Vertex *pVertex = pModel->GetNextVertex( pos );

      vDelta = pVertex->GetPos();
      vDelta -= vMatch;

      if (fabs(vDelta.x) < MIRRORTOLERANCE
         && fabs(vDelta.y) < MIRRORTOLERANCE
         && fabs(vDelta.z) < MIRRORTOLERANCE)
         return pVertex;
   }
   return NULL;
}

void Vertex::SetDrawColor(const RGBAFloat &color)
{
   glColor4f(color.GetRed(),color.GetGreen(),color.GetBlue(),color.GetAlpha());
}

BOOL Vertex::GetDrawColor( RGBAFloat &color )
{
   if (IsNearestFocusObject()) 
   {
      color.Set(0.0f, 0.5f, 0.0f, color.GetAlpha());
      return TRUE;
   }

   return Component::GetDrawColor( color );
}

void Vertex::Draw()
{
   glBegin(GL_POINTS);
      glVertex3fv(&GetPos().x);
   glEnd();
}

void Vertex::Draw( CGLBaseView *pView )
{
   glBegin( GL_POINTS );
      glVertex3fv( &GetRTVertexHead()->GetPos().x );
   glEnd();
}

void Vertex::DrawPick( CGLModelView *pView )
{
   if (CGLModelView::m_bSinglePick)
   {
      if (pView->IsDisplayWireframe())
         for (UINT i=0; i<GetNumSurroundingEdges(); i++)
            GetSurroundingEdgeAt(i)->DrawPickNearest(this);

      if (pView->IsDisplayShaded())
         for (UINT i=0; i<GetNumSurroundingPolys(); i++)
            GetSurroundingPolyAt(i)->DrawPickNearest(this);
   }
   else
      Draw();
}

Edge *Vertex::FindEdge(Vertex *pVertex, BOOL bUseAttached/*=TRUE*/) const
{
   // uses surrounding edges
   for (UINT i = 0; i < GetNumSurroundingEdges(); i++)
   {
      Edge *pEdge = GetSurroundingEdgeAt(i);
      if (bUseAttached)
      {
         if (pEdge->GetFirstVertex()==pVertex || pEdge->GetSecondVertex()==pVertex)
            return pEdge;
      }
      else
      {
         if (pEdge->GetFirstVertex() == pVertex || pEdge->GetSecondVertex() == pVertex)
            return pEdge;
      }
   }
   return NULL;
}

void Vertex::GetEdgePair(Poly *pPoly,Edge **pEdge0,Edge **pEdge1,UINT *nIndex0/*=NULL*/, UINT *nIndex1/*=NULL*/)
{
   int nFaceRelativeIndex = pPoly->FindVertexFaceRelativeIndex(this);
   ASSERT(nFaceRelativeIndex>=0);

   UINT nIndexPrev,nIndexNext;
   if ((nFaceRelativeIndex-1)<0)
      nIndexPrev = pPoly->GetNumSides()-1;
   else
      nIndexPrev = nFaceRelativeIndex-1;

   if ((nFaceRelativeIndex+1)>=(int)pPoly->GetNumSides())
      nIndexNext = nFaceRelativeIndex;
   else
      nIndexNext = nFaceRelativeIndex;

   if (nIndex0)
      *nIndex0 = nIndexPrev;
   if (nIndex1)
      *nIndex1 = nIndexNext;

   *pEdge0 = pPoly->GetEdgeAt(nIndexPrev);
   *pEdge1 = pPoly->GetEdgeAt(nIndexNext);
}

BOOL Vertex::Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError )
{
   DeleteInfo *&pVertexDeleteInfo = (DeleteInfo *&)pDeleteInfo;

   // Test Validity
   if (pVertexDeleteInfo == NULL)
   {
      UINT nNumSurroundingEdges = GetNumSurroundingEdges();
      if (nNumSurroundingEdges > 2)
      {
         strOutError = (String)"Could not delete Vertex(" + GetMatchName() + ").  Too many surrounding edges.";
         return FALSE;
      }

      Edge *pEdge0 = GetSurroundingEdgeAt(0);
      Edge *pEdge1 = nNumSurroundingEdges < 2 ? NULL : GetSurroundingEdgeAt(1);

      if ((pEdge0->GetFirstPoly()  && pEdge0->GetFirstPoly()->GetNumSides() <4)
      || (pEdge0->GetSecondPoly() && pEdge0->GetSecondPoly()->GetNumSides()<4)
      || (pEdge1 && pEdge1->GetFirstPoly()  && pEdge1->GetFirstPoly()->GetNumSides() <4)
      || (pEdge1 && pEdge1->GetSecondPoly() && pEdge1->GetSecondPoly()->GetNumSides()<4))
      {
         strOutError = (String)"Could not delete Vertex(" + GetMatchName() + ").  Illegal face would be formed.";
         return FALSE;
      }

      pVertexDeleteInfo = SNEW DeleteInfo( this );
      pVertexDeleteInfo->SetOldEdge0(pEdge0);
      pVertexDeleteInfo->SetOldEdge1(pEdge1);
      pVertexDeleteInfo->SetPoly0(pEdge0->GetFirstPoly());
      pVertexDeleteInfo->SetPoly1(pEdge0->GetSecondPoly());
   }

   if (!pVertexDeleteInfo->GetOldEdge1()) // End Dangle
   {
      if (pVertexDeleteInfo->GetOldEdge0())
         pVertexDeleteInfo->GetOldEdge0()->RemoveFromList();
      else
         RemoveFromList(); // Add Vertex mode has a Vertex but no edges
      return Component::Delete( pDeleteInfo, strOutError );
   }

   if (pVertexDeleteInfo->GetNewEdge()==NULL)
   {
      PolyModel *pModel = (PolyModel *)GetOwner();
      Edge *pNewEdge = pModel->AddEdge();
      pNewEdge->SetVisible( pVertexDeleteInfo->GetOldEdge0()->IsVisible() || pVertexDeleteInfo->GetOldEdge1()->IsVisible() );
      pVertexDeleteInfo->SetNewEdge( pNewEdge );

      for (UINT i=0; i<2; i++)
      {
         Poly *pPoly = (i==0) ? pVertexDeleteInfo->GetPoly0() : pVertexDeleteInfo->GetPoly1();

         if (pPoly)
         {
            pVertexDeleteInfo->SetOldPolyVertexIndex( i, pPoly->FindVertexFaceRelativeIndex( this ) );
            pVertexDeleteInfo->SetOldPolyVertexNormal( i, pPoly->GetVertexNormalAt( pVertexDeleteInfo->GetOldPolyVertexIndex(i) ) );

            UINT nNumPropertyPolys = pPoly->CountNumPropertyPolys();
            pVertexDeleteInfo->SetOldPolyNumPropertySets( i, nNumPropertyPolys );
            pVertexDeleteInfo->SetNumOldPropertyEdges0( i, nNumPropertyPolys );
            pVertexDeleteInfo->SetNumOldPropertyEdges1( i, nNumPropertyPolys );

            UINT nEdge0PatchRelativeIndex = pVertexDeleteInfo->GetOldEdge0()->GetFaceRelativeIndex( pPoly );
            UINT nEdge1PatchRelativeIndex = pVertexDeleteInfo->GetOldEdge1()->GetFaceRelativeIndex( pPoly );
            UINT nCount=0;

            pVertexDeleteInfo->SetNumNewPropertyEdges( i, nNumPropertyPolys );
            for (PropertyPoly *pPropertyPoly = pPoly->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
            {
               PropertyEdge *pPropertyOldEdge0 = pPropertyPoly->GetPropertyEdgeAt( nEdge0PatchRelativeIndex );
               PropertyEdge *pPropertyOldEdge1 = pPropertyPoly->GetPropertyEdgeAt( nEdge1PatchRelativeIndex );

               PropertyVertex *pPropertyVertex = pPropertyPoly->GetPropertyVertexAt( pVertexDeleteInfo->GetOldPolyVertexIndex(i) );
               pVertexDeleteInfo->SetOldPolyPropertySetPropertyVertex(i, nCount, pPropertyVertex);
               pVertexDeleteInfo->SetOldPropertyEdge0( i, nCount, pPropertyOldEdge0 );
               pVertexDeleteInfo->SetOldPropertyEdge1( i, nCount, pPropertyOldEdge1 );

               // NewPropertyEdge
               PropertySet *pPropertySet = (PropertySet *)pPropertyPoly->GetOwner();
               PropertyVertex *pPropertyVertex0, *pPropertyVertex1;

               if (pPropertyOldEdge0->GetSecondPropertyVertex() == pPropertyVertex)
               {
                  pPropertyVertex0 = pPropertyOldEdge0->GetConnectedPropertyVertex( pPropertyVertex );
                  pPropertyVertex1 = pPropertyOldEdge1->GetConnectedPropertyVertex( pPropertyVertex );
               }
               else
               {
                  pPropertyVertex0 = pPropertyOldEdge1->GetConnectedPropertyVertex( pPropertyVertex );
                  pPropertyVertex1= pPropertyOldEdge0->GetConnectedPropertyVertex( pPropertyVertex );
               }

               PropertyEdge *pNewPropertyEdge = pPropertySet->AddPropertyEdge( pPropertyPoly, pPropertyVertex0, pPropertyVertex1, pNewEdge, pPropertyOldEdge0->GetID());
               pNewPropertyEdge->SetVisible( pPropertyOldEdge0->IsVisible() || pPropertyOldEdge1->IsVisible() );
               pVertexDeleteInfo->SetNewPropertyEdge( i, nCount, pNewPropertyEdge );
            }
         }
      }
   }

   Poly *pPoly0 = pVertexDeleteInfo->GetPoly0();
   Poly *pPoly1 = pVertexDeleteInfo->GetPoly1();

   BuildPolyInfo *pBuildPolyInfo0 = NULL;
   BuildPolyInfo *pBuildPolyInfo1 = NULL;

   if (pPoly0)
      pBuildPolyInfo0 = pPoly0->RemoveVertex( pVertexDeleteInfo->GetNewEdge(), pVertexDeleteInfo->GetNewPropertyEdges(0), this );

   if (pPoly1)
      pBuildPolyInfo1 = pPoly1->RemoveVertex( pVertexDeleteInfo->GetNewEdge(), pVertexDeleteInfo->GetNewPropertyEdges(1), this );

   if (pVertexDeleteInfo->GetNewEdge()->GetFirstVertex()==NULL)
   {
      Vertex *pV0 = pVertexDeleteInfo->GetOldEdge0()->GetConnectedVertex(this);
      Vertex *pV1 = pVertexDeleteInfo->GetOldEdge1()->GetConnectedVertex(this);

      pVertexDeleteInfo->GetOldEdge0()->RemoveFromList();
      pVertexDeleteInfo->GetOldEdge1()->RemoveFromList();

      // OK it is safe to set our vertices in our new edge since the two old edges have been removed
      pVertexDeleteInfo->GetNewEdge()->SetFirstVertex( pV0 );
      pVertexDeleteInfo->GetNewEdge()->SetSecondVertex( pV1 );
      pVertexDeleteInfo->GetNewEdge()->SafeSetID(pVertexDeleteInfo->GetOldEdge0()->GetID());
   }
   else
   {
      pVertexDeleteInfo->GetOldEdge0()->RemoveFromList();
      pVertexDeleteInfo->GetOldEdge1()->RemoveFromList();
   }

   if (pBuildPolyInfo0)
   {
      pPoly0->BuildPoly( pBuildPolyInfo0 );
      delete pBuildPolyInfo0;
   }

   if (pBuildPolyInfo1)
   {
      pPoly1->BuildPoly( pBuildPolyInfo1 );
      delete pBuildPolyInfo1;
   }

   if (!pVertexDeleteInfo->GetNewEdge()->IsInList()) // Dangle Redo
      pVertexDeleteInfo->GetNewEdge()->ReInsertIntoList();

   return Component::Delete( pDeleteInfo, strOutError );
}

BOOL Vertex::UnDelete( SnipeObject::DeleteInfo *pDeleteInfo )
{
   DeleteInfo *pVertexDeleteInfo = (DeleteInfo *)pDeleteInfo;

   if (!pVertexDeleteInfo->GetOldEdge1()) // Edge Dangle
   {
      if (pVertexDeleteInfo->GetOldEdge0())
         pVertexDeleteInfo->GetOldEdge0()->ReInsertIntoList();
      else
         ReInsertIntoList();
      return Component::UnDelete( pDeleteInfo );
   }
   
   Poly *pPoly0 = pVertexDeleteInfo->GetPoly0();
   Poly *pPoly1 = pVertexDeleteInfo->GetPoly1();

   BuildPolyInfo *pBuildPolyInfo0 = NULL;
   BuildPolyInfo *pBuildPolyInfo1 = NULL;

   if (pPoly1)
      pBuildPolyInfo1 = pPoly1->ReinsertVertex( this, pVertexDeleteInfo->GetOldEdge0(), pVertexDeleteInfo->GetOldPropertyEdges0(1), pVertexDeleteInfo->GetOldEdge1(), pVertexDeleteInfo->GetOldPropertyEdges1(1),
      pVertexDeleteInfo->GetOldPolyVertexIndex(1), pVertexDeleteInfo->GetOldPolyVertexNormal(1), pVertexDeleteInfo->GetOldPolyPropertySet(1) );

   if (pPoly0)
      pBuildPolyInfo0 = pPoly0->ReinsertVertex( this, pVertexDeleteInfo->GetOldEdge0(), pVertexDeleteInfo->GetOldPropertyEdges0(0), pVertexDeleteInfo->GetOldEdge1(), pVertexDeleteInfo->GetOldPropertyEdges1(0), 
      pVertexDeleteInfo->GetOldPolyVertexIndex(0), pVertexDeleteInfo->GetOldPolyVertexNormal(0), pVertexDeleteInfo->GetOldPolyPropertySet(0) );

   pVertexDeleteInfo->GetNewEdge()->RemoveFromList();

   if (pBuildPolyInfo1)
   {
      pPoly1->BuildPoly( pBuildPolyInfo1 );
      delete pBuildPolyInfo1;
   }

   if (pBuildPolyInfo0)
   {
      pPoly0->BuildPoly( pBuildPolyInfo0 );
      delete pBuildPolyInfo0;
   }

   if (!pPoly0 && !pPoly1) // Dangle
   {
      pVertexDeleteInfo->GetOldEdge1()->ReInsertIntoList();
      pVertexDeleteInfo->GetOldEdge0()->ReInsertIntoList();
   }

   if (pVertexDeleteInfo->GetReversedEdge1())
      pVertexDeleteInfo->GetOldEdge1()->ReverseSpline();

   return Component::UnDelete( pDeleteInfo );
}

Vertex::DeleteInfo::~DeleteInfo()
{
   for (UINT i=0; i<2; i++)
   {
      SetOldPolyVertexNormal(i, NULL);

      ASSERT(GetNumOldPropertyEdges0(i) == GetNumOldPropertyEdges1(i) && GetNumOldPropertyEdges0(i) == GetOldPolyNumPropertySets(i) && GetNumOldPropertyEdges0(i) == GetNumNewPropertyEdges(i));
      for (UINT nPropertyPolyIndex=0; nPropertyPolyIndex<GetOldPolyNumPropertySets(i); nPropertyPolyIndex++)
      {
         SetNewPropertyEdge( i, nPropertyPolyIndex, NULL );
         SetOldPropertyEdge0( i, nPropertyPolyIndex, NULL );
         SetOldPropertyEdge1( i, nPropertyPolyIndex, NULL );
         SetOldPolyPropertySetPropertyVertex( i, nPropertyPolyIndex, NULL);
      }
   }

   SetOldEdge0(NULL);
   SetOldEdge1(NULL);

   SetNewEdge(NULL);
}

void Vertex::SetDirty()
{
   DirtyDependentEdges();
}

BOOL Vertex::IsUsed()
{
   return GetNumSurroundingEdges();
}

void Vertex::Disconnect()
{
   for (UINT i=0; i<GetNumBones(); i++)
   {
      Bone *pBone = GetBoneAt(i);
      pBone->RemoveVertex( this, FALSE );
   }

   Component::Disconnect();
}

void Vertex::Reconnect()
{
   for (UINT i=0; i<GetNumBones(); i++)
   {
      Bone *pBone = GetBoneAt(i);
      pBone->AddVertex( this, FALSE );
   }

   Component::Reconnect();
}

static Vertex *GetConnectedVertex(Edge *pEdge, const Vertex *pNotThis ) // Similar to the member function in Edge, but this one can return NULL
{
   if (pEdge->GetFirstVertex()==pNotThis)
      return pEdge->GetSecondVertex();
   else if ( pEdge->GetSecondVertex()==pNotThis)
      return pEdge->GetFirstVertex();
   else
      return NULL;
}

void Vertex::AddToVertexMap( VertexMap &vertexmap )
{
   vertexmap.SetAt( this, this );
}

BOOL Vertex::IsInVertexMap( VertexMap &vertexmap )
{
   Vertex *pTemp;
   return vertexmap.Lookup( this, pTemp );
}

void Vertex::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   Component::Save( bs );

   bs << sp << GetPos();

   if (bWriteHeader)
      bs << nl;
}

BOOL Vertex::Load( BaseStream &bs )
{
   if (!Component::Load( bs ))
      return FALSE;
   
   Vector pos;
   bs >> pos;
   SetPos( pos, FALSE );

   return TRUE;
}

BOOL Vertex::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if ( pProperty==&m_vPosition && GetOwner() )
            SetPos( m_vPosition.GetValue() );
         break;
      }
   }

   return Component::OnObjectNotify( msg, wParam, lParam );
}

Vertex::AttachInfo *Vertex::AttachTo( Vertex *pVertex, String *strError/*=NULL*/ )
{
   Vertex *pFromVertex = NULL;
   Vertex *pToVertex = NULL;
   BOOL bCollapsingEdge = FALSE;
   
   Edge *pEdge = Edge::FindEdge( this, pVertex );
   if (pEdge) // Collapsing an edge by welding both verts of the edge together
      bCollapsingEdge = TRUE;

   pFromVertex = this;
   pToVertex = pVertex;

   if (!bCollapsingEdge)
   {
      for (UINT i=0; i < pFromVertex->GetNumSurroundingPolys(); i++)
      {
         Poly *pFromPoly = pFromVertex->GetSurroundingPolyAt(i);

         for (UINT j=0; j < pToVertex->GetNumSurroundingPolys(); j++)
         {
            if (pToVertex->GetSurroundingPolyAt(j) == pFromPoly)
            {
               if (strError)
               {
                  *strError = "Unable to Attach Vertex(%1) to Vertex(%2), because they are both attached to Face(%3).";
                  strError->Replace("%1", (String)(int)pFromVertex->GetID());
                  strError->Replace("%2", (String)(int)pToVertex->GetID());
                  strError->Replace("%3", (String)(int)pFromPoly->GetID());
               }
               return NULL;
            }
         }
      }
   }

   Vertex::AttachInfo *pAttachInfo = SNEW Vertex::AttachInfo;

   if (!pAttachInfo->Fill( this, pToVertex, strError))
   {
      delete pAttachInfo;
      return NULL;
   }
      
   pAttachInfo->ReAttach();

   static BOOL bInMirror;
   if (!bInMirror)
   {
      bInMirror = TRUE;
      Vertex *pMirrorThis = (Vertex *)GetMirrorObject();
      Vertex *pMirrorVertex = (Vertex *)pVertex->GetMirrorObject();
      if (pMirrorThis && pMirrorVertex)
         pAttachInfo->m_pMirrorInfo = pMirrorThis->AttachTo( pMirrorVertex, strError );
      bInMirror = FALSE;
   }

   return pAttachInfo;
}

Vertex *Vertex::GetTerminatingVertex( CList<Edge *> *pNotInEdgesList/*=NULL*/ )
{
   return this; 
}

////////////////////////////////////////////////////////////////////////////////////
SwapPropertyEdge::SwapPropertyEdge( PropertyPoly *pPropertyPoly, PropertyEdge *pFrom, PropertyEdge *pTo)
{
   m_pFromPropertyPoly = pPropertyPoly;
   m_pFromPropertyEdge = pFrom;
   m_pToPropertyEdge = pTo;

   m_pFromPropertyEdge->Reference();
   m_pToPropertyEdge->Reference();
}

SwapPropertyEdge::~SwapPropertyEdge()
{
   m_pFromPropertyEdge->DeReference();
   m_pToPropertyEdge->DeReference();
}

void SwapPropertyEdge::SwapFromTo()
{
   UINT nIndex = m_pFromPropertyPoly->FindPropertyVertexFaceRelativeIndex( m_pFromPropertyEdge );
   ASSERT( nIndex != -1 );

   m_pFromPropertyPoly->SetPropertyEdgeAt( nIndex, m_pToPropertyEdge );
}

void SwapPropertyEdge::SwapToFrom()
{
   UINT nIndex = m_pFromPropertyPoly->FindPropertyVertexFaceRelativeIndex( m_pToPropertyEdge );
   ASSERT( nIndex != -1 );

   m_pFromPropertyPoly->SetPropertyEdgeAt( nIndex, m_pFromPropertyEdge );
}

////////////////////////////////////////////////////////////////////////////////////
CollapsedPropertyEdge::CollapsedPropertyEdge( PropertyEdge *pFrom, PropertyEdge *pTo, BOOL bAligned )
{
   m_nCollapsedPropertyEdgeFlags = 0;
   m_bAligned = bAligned;
   m_pFromPropertyEdge = pFrom;
   m_pToPropertyEdge = pTo;
   m_pFromPropertyPoly = m_pFromPropertyEdge->GetPropertyPoly();
   m_pToPropertyPoly = m_pToPropertyEdge->GetPropertyPoly();
   m_pFromPropertyEdge->Reference();
   m_pFromPropertyEdge->GetFirstPropertyVertex()->Reference();
   m_pFromPropertyEdge->GetSecondPropertyVertex()->Reference();
   m_pToPropertyEdge->Reference();
   m_pToPropertyEdge->GetFirstPropertyVertex()->Reference();
   m_pToPropertyEdge->GetSecondPropertyVertex()->Reference();

   for (UINT nPropertyVertexIndex=0; nPropertyVertexIndex < 2; nPropertyVertexIndex++)
   {
      PropertyVertex *pPropertyVertex = nPropertyVertexIndex==0 ? m_pFromPropertyEdge->GetFirstPropertyVertex() : m_pFromPropertyEdge->GetSecondPropertyVertex();
      for (UINT i=0; i<pPropertyVertex->GetNumSurroundingPropertyEdges(); i++)
      {
         PropertyEdge *pPropertyEdge = pPropertyVertex->GetSurroundingPropertyEdgeAt(i);
         if (pPropertyEdge == m_pFromPropertyEdge) // No Need swapping them out of FromPropertyEdge because it will just get disconnected anyways
            continue;

         if (pPropertyEdge->GetFirstPropertyVertex() == pPropertyVertex || pPropertyEdge->GetSecondPropertyVertex() == pPropertyVertex)
            m_PropertyEdgesThatSwappedPropertyVertices[nPropertyVertexIndex].AddTail( pPropertyEdge );
      }

      for (UINT i=0; i<pPropertyVertex->GetNumSurroundingPropertyPolys(); i++)
      {
         PropertyPoly *pPropertyPoly = pPropertyVertex->GetSurroundingPropertyPolyAt(i);
         if (pPropertyPoly->FindPropertyVertexFaceRelativeIndex( pPropertyVertex )!=-1)
            m_PropertyPolysThatSwappedPropertyVertices[nPropertyVertexIndex].AddTail( pPropertyPoly );
      }
   }
}

CollapsedPropertyEdge::~CollapsedPropertyEdge()
{
   m_pFromPropertyEdge->GetFirstPropertyVertex()->DeReference();
   m_pFromPropertyEdge->GetSecondPropertyVertex()->DeReference();
   m_pToPropertyEdge->GetFirstPropertyVertex()->DeReference();
   m_pToPropertyEdge->GetSecondPropertyVertex()->DeReference();
   m_pFromPropertyEdge->DeReference();
   m_pToPropertyEdge->DeReference();

   for (POSITION pos = m_PropertyEdgesThatSwappedOutOfPropertyPoly.GetHeadPosition(); pos; )
      delete m_PropertyEdgesThatSwappedOutOfPropertyPoly.GetNext(pos);
}

void CollapsedPropertyEdge::CollapseFromTo()
{
   if (m_pFromPropertyPoly)
   {
      UINT nIndex = m_pFromPropertyPoly->FindPropertyVertexFaceRelativeIndex(m_pFromPropertyEdge);
      ASSERT(nIndex!=-1);
      m_pFromPropertyPoly->SetPropertyEdgeAt( nIndex, m_pToPropertyEdge );

      for (UINT i=0; i<2; i++) // i is each UV of the FromPropertyEdge
      {
         PropertyVertex *pFromPropertyVertex = i==0 ? m_pFromPropertyEdge->GetFirstPropertyVertex() : m_pFromPropertyEdge->GetSecondPropertyVertex();
         PropertyVertex *pToPropertyVertex;
         if (m_bAligned)
            pToPropertyVertex = i==0 ? m_pToPropertyEdge->GetFirstPropertyVertex() : m_pToPropertyEdge->GetSecondPropertyVertex();
         else
            pToPropertyVertex = i==0 ? m_pToPropertyEdge->GetSecondPropertyVertex() : m_pToPropertyEdge->GetFirstPropertyVertex();

         for (POSITION pos = m_PropertyPolysThatSwappedPropertyVertices[i].GetHeadPosition(); pos; )
         {
            PropertyPoly *pPropertyPoly = m_PropertyPolysThatSwappedPropertyVertices[i].GetNext(pos);
            nIndex = pPropertyPoly->FindPropertyVertexFaceRelativeIndex(pFromPropertyVertex);
            ASSERT(nIndex!=-1);

            pPropertyPoly->SetPropertyVertexAt( nIndex, pToPropertyVertex);
         }

         for (POSITION pos = m_PropertyEdgesThatSwappedPropertyVertices[i].GetHeadPosition(); pos; )
         {
            PropertyEdge *pPropertyEdge = m_PropertyEdgesThatSwappedPropertyVertices[i].GetNext(pos);

            if (pPropertyEdge->GetFirstPropertyVertex() == pFromPropertyVertex)
               pPropertyEdge->SetFirstPropertyVertex( pToPropertyVertex );
            else
            {
               ASSERT( pPropertyEdge->GetSecondPropertyVertex() == pFromPropertyVertex );
               pPropertyEdge->SetSecondPropertyVertex( pToPropertyVertex );
            }
         }
      }
   }
   if (!m_bPropertyEdgesThatSwappedOutOfPropertyPolyHaveBeenFound) // Fill it
   {
      m_bPropertyEdgesThatSwappedOutOfPropertyPolyHaveBeenFound = TRUE;
      for (UINT i=0; i<m_pFromPropertyPoly->GetNumSides(); i++)
      {
         PropertyEdge *pPropertyVertexFromEdge = m_pFromPropertyPoly->GetPropertyEdgeAt(i);
         PropertyVertex *pPropertyVertex0 = pPropertyVertexFromEdge->GetAlignedFirstPropertyVertex( m_pFromPropertyPoly );
         PropertyVertex *pPropertyVertex1 = pPropertyVertexFromEdge->GetAlignedSecondPropertyVertex( m_pFromPropertyPoly );
         for (UINT j=0; j<pPropertyVertex0->GetNumSurroundingPropertyEdges(); j++)
         {
            PropertyEdge *pPropertyEdge = pPropertyVertex0->GetSurroundingPropertyEdgeAt(j);
            if (pPropertyEdge == pPropertyVertexFromEdge)
               continue;

            if ((pPropertyEdge->GetFirstPropertyVertex() == pPropertyVertex0 && pPropertyEdge->GetSecondPropertyVertex() == pPropertyVertex1)
             || (pPropertyEdge->GetFirstPropertyVertex() == pPropertyVertex1 && pPropertyEdge->GetSecondPropertyVertex() == pPropertyVertex0))
            {
               m_PropertyEdgesThatSwappedOutOfPropertyPoly.AddTail( SNEW SwapPropertyEdge( m_pFromPropertyPoly, pPropertyVertexFromEdge, pPropertyEdge) );
               break;
            }
         }
      }
      for (UINT i=0; i<m_pToPropertyPoly->GetNumSides(); i++)
      {
         PropertyEdge *pPropertyVertexToEdge = m_pToPropertyPoly->GetPropertyEdgeAt(i);
         PropertyVertex *pPropertyVertex0 = pPropertyVertexToEdge->GetAlignedFirstPropertyVertex( m_pToPropertyPoly );
         PropertyVertex *pPropertyVertex1 = pPropertyVertexToEdge->GetAlignedSecondPropertyVertex( m_pToPropertyPoly );
         for (UINT j=0; j<pPropertyVertex0->GetNumSurroundingPropertyEdges(); j++)
         {
            PropertyEdge *pPropertyEdge = pPropertyVertex0->GetSurroundingPropertyEdgeAt(j);
            if (pPropertyEdge == pPropertyVertexToEdge)
               continue;

            if ((pPropertyEdge->GetFirstPropertyVertex() == pPropertyVertex0 && pPropertyEdge->GetSecondPropertyVertex() == pPropertyVertex1)
               || (pPropertyEdge->GetFirstPropertyVertex() == pPropertyVertex1 && pPropertyEdge->GetSecondPropertyVertex() == pPropertyVertex0))
            {
               m_PropertyEdgesThatSwappedOutOfPropertyPoly.AddTail( SNEW SwapPropertyEdge( m_pToPropertyPoly, pPropertyVertexToEdge, pPropertyEdge) );
               break;
            }
         }
      }
   }

   for (POSITION pos = m_PropertyEdgesThatSwappedOutOfPropertyPoly.GetHeadPosition(); pos; )
   {
      SwapPropertyEdge *pSwapped = m_PropertyEdgesThatSwappedOutOfPropertyPoly.GetNext( pos );
      pSwapped->SwapFromTo();
   }
}

void CollapsedPropertyEdge::CollapseToFrom()
{
   if (m_pFromPropertyPoly)
   {
      for (POSITION pos = m_PropertyEdgesThatSwappedOutOfPropertyPoly.GetTailPosition(); pos; )
      {
         SwapPropertyEdge *pSwapped = m_PropertyEdgesThatSwappedOutOfPropertyPoly.GetPrev( pos );
         pSwapped->SwapToFrom();
      }

      UINT nIndex = m_pFromPropertyPoly->FindPropertyVertexFaceRelativeIndex(m_pToPropertyEdge);
      ASSERT(nIndex!=-1);

      for (int i=1; i>=0; i--) // i is each UV of the FromPropertyEdge
      {
         PropertyVertex *pFromPropertyVertex = i==0 ? m_pFromPropertyEdge->GetFirstPropertyVertex() : m_pFromPropertyEdge->GetSecondPropertyVertex();
         PropertyVertex *pToPropertyVertex;
         if (m_bAligned)
            pToPropertyVertex = i==0 ? m_pToPropertyEdge->GetFirstPropertyVertex() : m_pToPropertyEdge->GetSecondPropertyVertex();
         else
            pToPropertyVertex = i==0 ? m_pToPropertyEdge->GetSecondPropertyVertex() : m_pToPropertyEdge->GetFirstPropertyVertex();

         for (POSITION pos = m_PropertyEdgesThatSwappedPropertyVertices[i].GetTailPosition(); pos; )
         {
            PropertyEdge *pPropertyEdge = m_PropertyEdgesThatSwappedPropertyVertices[i].GetPrev(pos);

            if (pPropertyEdge->GetFirstPropertyVertex() == pToPropertyVertex)
               pPropertyEdge->SetFirstPropertyVertex( pFromPropertyVertex );
            else
            {
               ASSERT( pPropertyEdge->GetSecondPropertyVertex() == pToPropertyVertex );
               pPropertyEdge->SetSecondPropertyVertex( pFromPropertyVertex );
            }
         }

         for (POSITION pos = m_PropertyPolysThatSwappedPropertyVertices[i].GetTailPosition(); pos; )
         {
            PropertyPoly *pPropertyPoly = m_PropertyPolysThatSwappedPropertyVertices[i].GetPrev(pos);
            nIndex = pPropertyPoly->FindPropertyVertexFaceRelativeIndex(pToPropertyVertex);
            ASSERT(nIndex!=-1);

            pPropertyPoly->SetPropertyVertexAt( nIndex, pFromPropertyVertex );
         }
      }

      m_pFromPropertyPoly->SetPropertyEdgeAt( nIndex, m_pFromPropertyEdge );
   }
}

////////////////////////////////////////////////////////////////////////////////////
CollapsedEdge::CollapsedEdge( Edge *pFrom, Edge *pTo, BOOL bAligned, BOOL bCollapsePropertyEdges )
{
   m_bAligned = bAligned;
   m_pFromEdge = pFrom;
   m_pToEdge = pTo;
   m_pFromPoly = m_pFromEdge->GetPoly();
   m_pFromEdge->Reference();
   m_pToEdge->Reference();

   Poly *pToPoly = m_pToEdge->GetPoly();

   if (m_pFromPoly && pToPoly)
   {
      UINT nFromEdgeFaceRelativeIndex = m_pFromEdge->GetFaceRelativeIndex( m_pFromPoly );
      UINT nToEdgeFaceRelativeIndex = m_pToEdge->GetFaceRelativeIndex( pToPoly );

      Vertex *pToV0 = bAligned ? m_pToEdge->GetFirstVertex() : m_pToEdge->GetSecondVertex();
      UINT nIndex = pToPoly->FindVertexFaceRelativeIndex(pToV0);
      ASSERT( nIndex >=0 );
      m_pToVertexNormal[0] = pToPoly->GetVertexNormalAt( nIndex );

      Vertex *pToV1 = bAligned ? m_pToEdge->GetSecondVertex() : m_pToEdge->GetFirstVertex();
      nIndex = pToPoly->FindVertexFaceRelativeIndex(pToV1);
      ASSERT( nIndex >=0 );
      m_pToVertexNormal[1] = pToPoly->GetVertexNormalAt( nIndex );

      Vertex *pFromV0 = m_pFromEdge->GetFirstVertex();
      nIndex = m_pFromPoly->FindVertexFaceRelativeIndex( pFromV0 );
      ASSERT( nIndex >=0 );
      m_pFromVertexNormal[0] = m_pFromPoly->GetVertexNormalAt( nIndex );
      m_pFromVertexNormal[0]->Reference();

      Vertex *pFromV1 = m_pFromEdge->GetSecondVertex();
      nIndex = m_pFromPoly->FindVertexFaceRelativeIndex(pFromV1);
      ASSERT( nIndex >=0 );
      m_pFromVertexNormal[1] = m_pFromPoly->GetVertexNormalAt( nIndex );
      m_pFromVertexNormal[1]->Reference();

      for (UINT nVertex=0; nVertex<2; nVertex++)
      {
         if (m_pFromVertexNormal[nVertex] == m_pToVertexNormal[nVertex])
            continue;

         Vertex *pVertex = nVertex==0 ? pFromV0 : pFromV1;

         for (UINT i=0; i<pVertex->GetNumSurroundingPolys(); i++)
         {
            Poly *pPoly = pVertex->GetSurroundingPolyAt(i);
            if (pPoly->FindVertexNormalFaceRelativeIndex(m_pFromVertexNormal[nVertex])!= -1)
               m_VertexNormalPolyList[nVertex].AddTail( pPoly );
         }
      }
      for( PropertyPoly *pFromPropertyPoly = m_pFromPoly->GetHeadPropertyPoly(); pFromPropertyPoly; pFromPropertyPoly=pFromPropertyPoly->GetNextPropertyPoly())
      {
         PropertyPoly *pToPropertyPoly = NULL;
         PropertySet *pFromPropertySet = pFromPropertyPoly->GetPropertySet();
         if (bCollapsePropertyEdges)
         {
            for (PropertyPoly *pTempPropertyPoly = pToPoly->GetHeadPropertyPoly(); pTempPropertyPoly; pTempPropertyPoly=pTempPropertyPoly->GetNextPropertyPoly())
            {
               if (pTempPropertyPoly->GetPropertySet() == pFromPropertySet)
               {
                  pToPropertyPoly = pTempPropertyPoly;
                  break;
               }
            }
         }

         PropertyEdge *pFromPropertyEdge = pFromPropertyPoly->GetPropertyEdgeAt(nFromEdgeFaceRelativeIndex);

         if (pToPropertyPoly)
         {
            PropertyEdge *pToPropertyEdge = pToPropertyPoly->GetPropertyEdgeAt(nToEdgeFaceRelativeIndex);
            CollapsedPropertyEdge *pCollasedPropertyEdge = SNEW CollapsedPropertyEdge( pFromPropertyEdge, pToPropertyEdge, m_bAligned );
            m_CollapsedPropertyEdges.AddTail( pCollasedPropertyEdge );
         }
         else
            m_PropertyEdgesThatChangedEdgeLists.AddTail( pFromPropertyEdge );
      }
   }
   else
   {
      m_pFromVertexNormal[0] = NULL;
      m_pFromVertexNormal[1] = NULL;
      m_pToVertexNormal[0] = NULL;
      m_pToVertexNormal[1] = NULL;
   }
}

CollapsedEdge::~CollapsedEdge()
{
   m_pFromEdge->DeReference();
   m_pToEdge->DeReference();
   if (m_pFromVertexNormal[0])
      m_pFromVertexNormal[0]->DeReference();
   if (m_pFromVertexNormal[1])
      m_pFromVertexNormal[1]->DeReference();

   for (POSITION pos = m_CollapsedPropertyEdges.GetHeadPosition(); pos;)
      delete m_CollapsedPropertyEdges.GetNext(pos);
}

void CollapsedEdge::CollapseFromTo()
{
   if (m_pFromPoly)
   {
      for (UINT i=0; i<2; i++)
      {
         for (POSITION pos = m_VertexNormalPolyList[i].GetHeadPosition(); pos; )
         {
            Poly *pPoly = m_VertexNormalPolyList[i].GetNext(pos);

            UINT nFromIndex = pPoly->FindVertexNormalFaceRelativeIndex( m_pFromVertexNormal[i] );
            if (nFromIndex!=-1)// It is posible that the collapsing of a different edge changed our Normal Already
               pPoly->SetVertexNormalAt( nFromIndex, m_pToVertexNormal[i] );
         }
      }

      UINT nIndex = m_pFromEdge->GetFaceRelativeIndex( m_pFromPoly );
      ASSERT(nIndex!=-1);
      m_pFromPoly->SetEdgeAt( nIndex, m_pToEdge );

      for (POSITION pos = m_PropertyEdgesThatChangedEdgeLists.GetHeadPosition(); pos;)
      {
         PropertyEdge *pPropertyEdge = m_PropertyEdgesThatChangedEdgeLists.GetNext(pos);

         if (!m_bAligned)
            pPropertyEdge->Reverse();
            
         m_pFromEdge->RemovePropertyEdge( pPropertyEdge );
         m_pToEdge->AddPropertyEdge( pPropertyEdge );
      }

      for (POSITION pos = m_CollapsedPropertyEdges.GetHeadPosition(); pos; )
      {
         CollapsedPropertyEdge *pCollapsedPropertyEdge = m_CollapsedPropertyEdges.GetNext(pos);
         pCollapsedPropertyEdge->CollapseFromTo();
      }
   }
}

void CollapsedEdge::CollapseToFrom()
{
   if (m_pFromPoly)
   {
      for (POSITION pos = m_PropertyEdgesThatChangedEdgeLists.GetTailPosition(); pos;)
      {
         PropertyEdge *pPropertyEdge = m_PropertyEdgesThatChangedEdgeLists.GetPrev(pos);

         m_pToEdge->RemovePropertyEdge( pPropertyEdge );
         m_pFromEdge->AddPropertyEdge( pPropertyEdge );

         if (!m_bAligned)
            pPropertyEdge->Reverse();
      }

      for (POSITION pos = m_CollapsedPropertyEdges.GetTailPosition(); pos; )
      {
         CollapsedPropertyEdge *pCollapsedPropertyEdge = m_CollapsedPropertyEdges.GetPrev(pos);
         pCollapsedPropertyEdge->CollapseToFrom();
      }

      UINT nIndex = m_pToEdge->GetFaceRelativeIndex( m_pFromPoly );
      ASSERT(nIndex!=-1);
      m_pFromPoly->SetEdgeAt( nIndex, m_pFromEdge );

      for (UINT i=0; i<2; i++)
      {
         for (POSITION pos = m_VertexNormalPolyList[i].GetHeadPosition(); pos; )
         {
            Poly *pPoly = m_VertexNormalPolyList[i].GetNext(pos);

            UINT nFromIndex = pPoly->FindVertexNormalFaceRelativeIndex( m_pToVertexNormal[i] );
            if (nFromIndex!=-1)// It is posible that the collapsing of a different edge changed our Normal Already
               pPoly->SetVertexNormalAt( nFromIndex, m_pFromVertexNormal[i] );
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////////
void SwappedVertexInPoly::SwapFromTo()
{
   UINT nIndex = m_pPoly->FindVertexFaceRelativeIndex( m_pFromVertex, FALSE );
   if (nIndex != -1)
      m_pPoly->SetVertexAt(nIndex, m_pToVertex);
}

void SwappedVertexInPoly::SwapToFrom()
{
   UINT nIndex = m_pPoly->FindVertexFaceRelativeIndex( m_pToVertex, FALSE );
   if (nIndex != -1)
      m_pPoly->SetVertexAt(nIndex, m_pFromVertex);
}

////////////////////////////////////////////////////////////////////////////////////
//Vertex::AttachInfo
BOOL Vertex::AttachInfo::Fill( Vertex *pFromVertex, Vertex *pToVertex, String *strError )
{
   SetFromVertex( pFromVertex );
   m_bFromVertexSelected = pFromVertex->IsSelected();
   SetToVertex( pToVertex );
   m_bToVertexSelected = pToVertex->IsSelected();
   SetFocusObject( SelectionList::GetFocusObject() );
   SetNearFocusObject( SelectionList::GetNearestFocusObject() );

   for (UINT i=0; i<pFromVertex->GetNumSurroundingEdges(); i++)
      m_edgeschanged.AddTail(pFromVertex->GetSurroundingEdgeAt( i ));

   // Find Edges that need removed do to collapsing
   for (UINT i=0; i < pFromVertex->GetNumSurroundingEdges(); i++)
   {
      Edge *pFromEdge = pFromVertex->GetSurroundingEdgeAt(i);
      Vertex *pFromV = pFromEdge->GetConnectedVertex( pFromVertex );

      for (UINT k=0; k < pToVertex->GetNumSurroundingEdges(); k++)
      {
         Edge *pToEdge = pToVertex->GetSurroundingEdgeAt(k);
         Vertex *pToV = pToEdge->GetConnectedVertex( pToVertex );

         if (pFromV==pToV) // FromEdge needs to go Bye-Bye because it is being collapsed
         {
            Poly *pCommonPoly = NULL;
            if (pFromEdge->GetFirstPoly() && (pFromEdge->GetFirstPoly()==pToEdge->GetFirstPoly() || pFromEdge->GetFirstPoly()==pToEdge->GetSecondPoly()))
               pCommonPoly = pFromEdge->GetFirstPoly();
            else if (pFromEdge->GetSecondPoly() && (pFromEdge->GetSecondPoly()==pToEdge->GetFirstPoly() || pFromEdge->GetSecondPoly()==pToEdge->GetSecondPoly()))
               pCommonPoly = pFromEdge->GetSecondPoly();

            if (pCommonPoly) // This will be OK later, but not now
            {
               if (strError)
               {
                  *strError = "Edge(%1) can not be collapsed onto Edge(%2) because they share the common Face(%3), delete face first.";
                  strError->Replace("%1", (String)(int)pFromEdge->GetID());
                  strError->Replace("%2", (String)(int)pToEdge->GetID());
                  strError->Replace("%3", (String)(int)pCommonPoly->GetID());
               }
               return FALSE;
            }

            BOOL bAligned = pFromEdge->GetFirstVertex()==pToEdge->GetFirstVertex() || pFromEdge->GetSecondVertex()==pToEdge->GetSecondVertex();
            AddCollapsedEdge( pFromEdge, pToEdge, bAligned );
         }
      }
   }

   for (UINT i=0; i<pFromVertex->GetNumSurroundingPolys(); i++)
   {
      Poly *pPoly = pFromVertex->GetSurroundingPolyAt( i );
      AddSwappedVertexInPoly( pPoly, pFromVertex, pToVertex );
   }
   return TRUE;
}

void Vertex::AttachInfo::ReAttach()
{
   for (POSITION pos = m_SwappedVertexInPoly.GetHeadPosition(); pos; )
   {
      SwappedVertexInPoly *pSwapped = m_SwappedVertexInPoly.GetNext( pos );
      pSwapped->SwapFromTo();
   }

   for (POSITION pos = m_edgeschanged.GetHeadPosition(); pos; )
   {
      Edge *pEdge = m_edgeschanged.GetNext( pos );
      
      if (pEdge->GetFirstVertex() == m_pFromVertex )
         pEdge->SetFirstVertex( m_pToVertex );
      else
         pEdge->SetSecondVertex( m_pToVertex );
   }

   for (POSITION pos = m_CollapsedEdges.GetHeadPosition(); pos; )
   {
      CollapsedEdge *pCollapsedEdge = m_CollapsedEdges.GetNext( pos );
      pCollapsedEdge->CollapseFromTo();
   }

   SetRedoSelection();

   if (m_pMirrorInfo)
      m_pMirrorInfo->ReAttach();
}

void Vertex::AttachInfo::UnAttach()
{
   if (m_pMirrorInfo)
      m_pMirrorInfo->UnAttach();

   for (POSITION pos = m_CollapsedEdges.GetTailPosition(); pos; )
   {
      CollapsedEdge *pCollapsedEdge = m_CollapsedEdges.GetPrev( pos );
      pCollapsedEdge->CollapseToFrom();
   }

   for (POSITION pos = m_edgeschanged.GetTailPosition(); pos; )
   {
      Edge *pEdge = m_edgeschanged.GetPrev( pos );

      if (pEdge->GetFirstVertex() == m_pToVertex )
         pEdge->SetFirstVertex( m_pFromVertex );
      else
         pEdge->SetSecondVertex( m_pFromVertex );
   }

   for (POSITION pos = m_SwappedVertexInPoly.GetTailPosition(); pos; )
   {
      SwappedVertexInPoly *pSwapped = m_SwappedVertexInPoly.GetPrev( pos );
      pSwapped->SwapToFrom();
   }

   SetUndoSelection();
}

void Vertex::AttachInfo::SetUndoSelection()
{
   SelectionList *pSelectionList = GetMainInterface()->GetCurrentSelectionList();

   if ( m_bFromVertexSelected )
      pSelectionList->Add( m_pFromVertex );
   else if (m_pFromVertex->IsSelected())
      pSelectionList->Remove( m_pFromVertex );

   if ( m_bToVertexSelected )
      pSelectionList->Add( m_pToVertex );
   else if (m_pToVertex->IsSelected())
      pSelectionList->Remove( m_pToVertex );

   SelectionList::SetFocusObject( m_pFocusObject, m_pNearFocusObject );
}

void Vertex::AttachInfo::SetRedoSelection()
{
   SelectionList *pSelectionList = GetMainInterface()->GetCurrentSelectionList();

   if (m_bFromVertexSelected && m_pToVertex->IsInList())
   {
      pSelectionList->Add( m_pToVertex );
      SelectionList::SetFocusObject( m_pToVertex, NULL );
   }
}

/////////////////////////////////////////////////////////////////////////
Vertex::WeldInfo *Vertex::AllocWeldInfo()
{
   return SNEW Vertex::WeldInfo();
}

Vertex::WeldInfo *Vertex::WeldTo( Vertex *pToVertex, String *strError/*=NULL*/ )
{
   WeldInfo *pWeldInfo = AllocWeldInfo();

   if (!pWeldInfo->Fill( this, pToVertex, strError ))
   {
      delete pWeldInfo;
      return NULL;
   }

   pWeldInfo->ReWeld();

   static bInMirror;
   if (!bInMirror)
   {
      bInMirror = TRUE;
      Vertex *pMirrorThis = (Vertex *)GetMirrorObject();
      Vertex *pMirrorToVertex = (Vertex *)pToVertex->GetMirrorObject();
      if (pMirrorThis && pToVertex)
         pWeldInfo->m_pMirrorInfo = pMirrorThis->WeldTo( pMirrorToVertex, strError );
      bInMirror = FALSE;
   }

   return pWeldInfo;
}

Vertex::WeldInfo::WeldInfo()
{
   m_pMirrorInfo = NULL;
   m_pWeldedEdge = NULL;
   m_pFromVertex = NULL;
   m_pToVertex = NULL;
   m_pPoly[0] = NULL;
   m_pPoly[1] = NULL;

   m_nWeldInfoFlags = 0;
   m_pFocusObject = NULL;
   m_pNearFocusObject = NULL;

   m_pFromVertexNormal[0] = NULL;
   m_pFromVertexNormal[1] = NULL;

   m_pCollapseFaceInfo[0] = NULL; // whether m_pPoly[0] is being collapsed
   m_pCollapseFaceInfo[1] = NULL; // whether m_pPoly[1] is being collapsed
}

Vertex::WeldInfo::~WeldInfo()
{
   if (m_pWeldedEdge)
      m_pWeldedEdge->DeReference();

   if (m_pFromVertex)
      m_pFromVertex->DeReference();

   for (UINT i=0; i<2; i++)
   {
      if (m_pFromVertexNormal[i])
         m_pFromVertexNormal[i]->DeReference();
   }

   for (POSITION pos = m_weldedPropertyEdges.GetStartPosition(); pos;)
   {
      PropertySet *pPropertySet;
      PropertyEdge *pPropertyEdge;
      m_weldedPropertyEdges.GetNextAssoc( pos, pPropertySet, pPropertyEdge );
      pPropertyEdge->DeReference();
   }

   if (m_pCollapseFaceInfo[0])
      delete m_pCollapseFaceInfo[0];
   if (m_pCollapseFaceInfo[1])
      delete m_pCollapseFaceInfo[1];

   for (POSITION pos = m_vertexinpolyschanged.GetHeadPosition(); pos; )
      delete m_vertexinpolyschanged.GetNext( pos );

   while(!m_CollapsedEdges.IsEmpty())
      delete m_CollapsedEdges.RemoveHead();

   for (POSITION pos = m_PropertyVerticesInPropertyEdgeChanged.GetHeadPosition(); pos; )
      delete m_PropertyVerticesInPropertyEdgeChanged.GetNext( pos );

   for (POSITION pos = m_PropertyVerticesInPropertyPolyChanged.GetHeadPosition(); pos; )
      delete m_PropertyVerticesInPropertyPolyChanged.GetNext( pos );

   if (m_pMirrorInfo)
      delete m_pMirrorInfo;
}

BOOL Vertex::WeldInfo::Fill( Vertex *pFromVertex, Vertex *pToVertex, String *strError/*=NULL*/ )
{
   m_pWeldedEdge = Edge::FindEdge( pFromVertex, pToVertex );
   if (!m_pWeldedEdge)
   {
      if (strError)
      {
         *strError = "Vertex(%1) and Vertex(%2) do not belong to a common edge.";
         strError->Replace("%1", (String)(int)pFromVertex->GetID());
         strError->Replace("%2", (String)(int)pToVertex->GetID());
      }
      return FALSE;
   }

   m_pFromVertex = pFromVertex;
   m_bFromVertexSelected = pFromVertex->IsSelected();
   m_pToVertex = pToVertex;
   m_bToVertexSelected = pToVertex->IsSelected();
   m_pPoly[0] = m_pWeldedEdge->GetFirstPoly();
   m_pPoly[1] = m_pWeldedEdge->GetSecondPoly();
   m_pFocusObject = SelectionList::GetFocusObject();
   m_pNearFocusObject = SelectionList::GetNearestFocusObject();

   //reference components that will be removed
   m_pWeldedEdge->Reference();
   m_pFromVertex->Reference();

   // Store Edge and Vertex face relative index for m_pPoly[0] and m_pPoly[1]
   // Needed for Building and Rebuilding m_pPoly[0] and m_pPoly[1]
   // Store From Vertex normals
   for (int i=0; i<2; i++)
   {
      if (m_pPoly[i])
      {
         m_nVertexFaceRelativeIndex[i] = m_pPoly[i]->FindVertexFaceRelativeIndex( m_pFromVertex );
         m_nEdgeFaceRelativeIndex[i] = m_pWeldedEdge->GetFaceRelativeIndex( m_pPoly[i] );
         m_pFromVertexNormal[i] = m_pPoly[i]->GetVertexNormalAt(m_nVertexFaceRelativeIndex[i]);
         m_pFromVertexNormal[i]->Reference();
         m_pToVertexNormal[i] = m_pPoly[i]->GetVertexNormalAt(m_pPoly[i]->FindVertexFaceRelativeIndex( m_pToVertex ));
      }
      else
      {
         m_nVertexFaceRelativeIndex[i] = NULL;
         m_nEdgeFaceRelativeIndex[i] = NULL;
         m_pFromVertexNormal[i] = NULL;
         m_pToVertexNormal[i] = NULL;
      }
   }

   // Store all edges that need to have this(FromVertex) removed from it and replaced with pToVertex
   for (UINT i=0; i<m_pFromVertex->GetNumSurroundingEdges(); i++)
   {
      Edge *pEdge = m_pFromVertex->GetSurroundingEdgeAt( i );
      if (pEdge == m_pWeldedEdge)
         continue;
      m_edgeschanged.AddTail(pEdge);
   }

   // Store all faces that need pFromVertex removed from it and replaced with pToVertex
   for (UINT i=0; i<m_pFromVertex->GetNumSurroundingPolys(); i++)
   {
      Poly *pPoly = m_pFromVertex->GetSurroundingPolyAt( i );
      if (pPoly == m_pPoly[0] || pPoly == m_pPoly[1]) // Don't need to mess with these two polys because they are getting rebuild without the vertex and edge in them
         continue;

      UINT nRelativeIndex = pPoly->FindVertexFaceRelativeIndex( m_pFromVertex );
      ASSERT(nRelativeIndex!=-1);

      SwappedVertexInPoly *pSwapped = SNEW SwappedVertexInPoly( pPoly, m_pFromVertex, m_pToVertex );
      m_vertexinpolyschanged.AddTail(pSwapped);
   }

   // Store all faces that need m_pFromVertexNormal removed from it and replaced with m_pToVertexNormal
   for (UINT i=0; i<m_pFromVertex->GetNumSurroundingPolys(); i++)
   {
      Poly *pPoly = m_pFromVertex->GetSurroundingPolyAt( i );
      if (pPoly == m_pPoly[0] || pPoly == m_pPoly[1]) // Don't need to mess with these two polys because they are getting rebuild without the fromvertexnormal and weldededge in them
         continue;

      for (UINT nIndex = 0; nIndex<pPoly->GetNumSides(); nIndex++)
      {
         VertexNormal *pVertexNormal = pPoly->GetVertexNormalAt(nIndex);

         if (pVertexNormal == m_pFromVertexNormal[0] )
            m_normalinpolyschanged[0].AddTail(pPoly);
         else if (pVertexNormal == m_pFromVertexNormal[1])
            m_normalinpolyschanged[1].AddTail((pPoly));
      }
   }

   // store and reference UV's and PropertyEdges that will go Bye, Bye
   BOOL bFirstVertex = m_pWeldedEdge->GetFirstVertex() == pFromVertex ? TRUE : FALSE;

   UINT nNumPropertyEdges = m_pWeldedEdge->CountNumPropertyEdges();
   m_fromPropertyVertices.InitHashTable(GetPrimeLargerThan(nNumPropertyEdges * 1.2f));
   m_weldedPropertyEdges.InitHashTable(GetPrimeLargerThan(nNumPropertyEdges * 1.2f));

   for (PropertyEdge *pPropertyEdge = m_pWeldedEdge->GetHeadPropertyEdge(); pPropertyEdge; pPropertyEdge=pPropertyEdge->GetNextPropertyEdge())
   {
      PropertySet *pPropertySet = pPropertyEdge->GetPropertySet();
      PropertyVertex *pFromPropertyVertex = bFirstVertex ? pPropertyEdge->GetFirstPropertyVertex() : pPropertyEdge->GetSecondPropertyVertex();
      PropertyVertex *pToPropertyVertex = bFirstVertex ? pPropertyEdge->GetSecondPropertyVertex() : pPropertyEdge->GetFirstPropertyVertex();

      m_weldedPropertyEdges.SetAt( pPropertySet, pPropertyEdge );
      pPropertyEdge->Reference();

      // Store all PropertyEdges that needs UV changed
      for (UINT i=0; i<pFromPropertyVertex->GetNumSurroundingPropertyEdges(); i++)
      {
         PropertyEdge *pSurroundingPropertyEdge = pFromPropertyVertex->GetSurroundingPropertyEdgeAt(i);
         if (pSurroundingPropertyEdge==pPropertyEdge) // Don't need to change this edge because it is getting removed
            continue;

         m_PropertyVerticesInPropertyEdgeChanged.AddTail( SNEW SwappedPropertyVertexInPropertyEdge( pSurroundingPropertyEdge, pFromPropertyVertex, pToPropertyVertex ) );
      }

      // Store all PropertyPolys that needs UV changed
      for (UINT i=0; i<pFromPropertyVertex->GetNumSurroundingPropertyPolys(); i++)
      {
         PropertyPoly *pSurroundingPropertyPoly = pFromPropertyVertex->GetSurroundingPropertyPolyAt(i);
         if (pSurroundingPropertyPoly==pPropertyEdge->GetFirstPropertyPoly() || pSurroundingPropertyPoly==pPropertyEdge->GetSecondPropertyPoly()) // Don't need to change this PropertyPoly because it is getting rebuilt
            continue;

         m_PropertyVerticesInPropertyPolyChanged.AddTail( SNEW SwappedPropertyVertexInPropertyPoly( pSurroundingPropertyPoly, pFromPropertyVertex, pToPropertyVertex ) );
      }
   }

   for (UINT nPolyIndex=0; nPolyIndex<2; nPolyIndex++)
   {
      Poly *pPoly = m_pPoly[nPolyIndex];
      if (!pPoly)
      {
         // Check to see if edges are collapsing
         for (UINT i=0; i < m_pFromVertex->GetNumSurroundingEdges(); i++)
         {
            Edge *pFromEdge = m_pFromVertex->GetSurroundingEdgeAt(i);
            Vertex *pFromVertex = pFromEdge->GetConnectedVertex( m_pFromVertex );

            for (UINT k=0; k < m_pToVertex->GetNumSurroundingEdges(); k++)
            {
               Edge *pToEdge = m_pToVertex->GetSurroundingEdgeAt(k);
               Vertex *pToVertex = pToEdge->GetConnectedVertex( m_pToVertex );

               if (pFromVertex==pToVertex)
               {
                  if (pFromEdge->GetFirstPoly() && (pFromEdge->GetFirstPoly()==pToEdge->GetFirstPoly() || pFromEdge->GetFirstPoly()==pToEdge->GetSecondPoly()))
                     continue;
                  else if (pFromEdge->GetSecondPoly() && (pFromEdge->GetSecondPoly()==pToEdge->GetFirstPoly() || pFromEdge->GetSecondPoly()==pToEdge->GetSecondPoly()))
                     continue;

                  BOOL bAligned = pFromEdge->GetFirstVertex()==pToEdge->GetFirstVertex() || pFromEdge->GetSecondVertex()==pToEdge->GetSecondVertex();
                  AddCollapsedEdge( pFromEdge, pToEdge, bAligned );
               }
            }
         }
         continue;
      }

      // Has pPoly
      if (pPoly->GetNumSides() > 3)
         continue;

      // pPoly will get collapsed!
      m_pCollapseFaceInfo[nPolyIndex] = SNEW CollapseFace;
      Edge *pFromEdge = NULL;
      for (UINT j=0; j<m_pFromVertex->GetNumSurroundingEdges(); j++)
      {
         Edge *pEdge = m_pFromVertex->GetSurroundingEdgeAt(j);
         if (pEdge == m_pWeldedEdge)
            continue;
         if (pEdge->HasPoly(pPoly))
         {
            pFromEdge = pEdge;
            break;
         }
      }
      ASSERT(pFromEdge);

      Edge *pToEdge = NULL;
      for (UINT j=0; j<m_pToVertex->GetNumSurroundingEdges(); j++)
      {
         Edge *pEdge = m_pToVertex->GetSurroundingEdgeAt(j);
         if (pEdge == m_pWeldedEdge)
            continue;
         if (pEdge->HasPoly(pPoly))
         {
            pToEdge = pEdge;
            break;
         }
      }
      ASSERT(pToEdge);

      m_pCollapseFaceInfo[nPolyIndex]->Fill( pFromEdge, pToEdge, pPoly );
   }

   // Store Original face data for undo, this was much easier than rebuilding
   for (UINT i=0; i<2; i++)
   {
      Poly *pPoly = m_pPoly[i];
      if (!pPoly)
         continue;

      UINT nSides = pPoly->GetNumSides();
      m_oldvertices[i].SetSize( nSides );
      m_oldvertexnormals[i].SetSize( nSides );
      m_oldedges[i].SetSize( nSides );

      UINT nNumPropertyPolys = pPoly->CountNumPropertyPolys();
      m_oldPropertyVertices[i].SetSize(nNumPropertyPolys);
      m_oldPropertyEdges[i].SetSize(nNumPropertyPolys);

      for (UINT nPropertyPolyIndex=0; nPropertyPolyIndex<nNumPropertyPolys; nPropertyPolyIndex++)
      {
         m_oldPropertyVertices[i][nPropertyPolyIndex].SetSize( nSides );
         m_oldPropertyEdges[i][nPropertyPolyIndex].SetSize( nSides );
      }

      for (UINT nIndex =0; nIndex < nSides; nIndex++)
      {
         m_oldvertices[i][nIndex] = pPoly->GetVertexAt( nIndex );
         m_oldvertexnormals[i][nIndex] = pPoly->GetVertexNormalAt( nIndex );
         m_oldedges[i][nIndex] = pPoly->GetEdgeAt( nIndex );

         UINT nPropertyPolyIndex=0; 
         for (PropertyPoly *pPropertyPoly = pPoly->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nPropertyPolyIndex++)
         {
            m_oldPropertyVertices[i][nPropertyPolyIndex][nIndex] = pPropertyPoly->GetPropertyVertexAt( nIndex );
            m_oldPropertyEdges[i][nPropertyPolyIndex][nIndex] = pPropertyPoly->GetPropertyEdgeAt( nIndex );
         }
      }
   }

   return TRUE;
}

void Vertex::WeldInfo::SetUndoSelection()
{
   SelectionList *pSelectionList = GetMainInterface()->GetCurrentSelectionList();

   if ( m_bFromVertexSelected )
      pSelectionList->Add( m_pFromVertex );
   else if (m_pFromVertex->IsSelected())
      pSelectionList->Remove( m_pFromVertex );

   if ( m_bToVertexSelected )
      pSelectionList->Add( m_pToVertex );
   else if (m_pToVertex->IsSelected())
      pSelectionList->Remove( m_pToVertex );

   SelectionList::SetFocusObject( m_pFocusObject, m_pNearFocusObject );
}

void Vertex::WeldInfo::SetRedoSelection()
{
   SelectionList *pSelectionList = GetMainInterface()->GetCurrentSelectionList();

   if (m_bFromVertexSelected && m_pToVertex->IsInList())
   {
      pSelectionList->Add( m_pToVertex );
      SelectionList::SetFocusObject( m_pToVertex, NULL );
   }
}

void Vertex::WeldInfo::ReWeld()
{
   // remove m_pWeldedEdge first so connectivity is correct
   m_pWeldedEdge->RemoveFromList();

   //////////////////////////////////////////////////////////////////////////////////
   // Swap pFromVertex to pToVertex in Edges
   for (POSITION pos = m_edgeschanged.GetHeadPosition(); pos;)
   {
      Edge *pEdge = m_edgeschanged.GetNext( pos );
      if (pEdge->GetFirstVertex() == m_pFromVertex )
         pEdge->SetFirstVertex( m_pToVertex );
      else
         pEdge->SetSecondVertex( m_pToVertex );
   }

   //////////////////////////////////////////////////////////////////////////////////
   // Swap pFromVertex to pToVertex in Polys
   for (POSITION pos = m_vertexinpolyschanged.GetHeadPosition(); pos;)
   {
      SwappedVertexInPoly *pSwapped = m_vertexinpolyschanged.GetNext( pos );
      pSwapped->SwapFromTo();
   }

   //////////////////////////////////////////////////////////////////////////////////
   // Swap pFromVertexNormals to pToVertexNormals in Polys
   for (UINT i=0; i<2; i++)
   {
      for (POSITION pos = m_normalinpolyschanged[i].GetHeadPosition(); pos;)
      {
         Poly *pPoly = m_normalinpolyschanged[i].GetNext( pos );
         int nFaceRelativeIndex = pPoly->FindVertexNormalFaceRelativeIndex( m_pFromVertexNormal[i] );
         ASSERT(nFaceRelativeIndex!=-1);

         pPoly->SetVertexNormalAt( nFaceRelativeIndex, m_pToVertexNormal[i] );
      }
   }

   //////////////////////////////////////////////////////////////////////////////////
   // Swap pFromPropertyVertex to pToPropertyVertex in PropertyEdges
   for (POSITION pos = m_PropertyVerticesInPropertyEdgeChanged.GetHeadPosition(); pos;)
   {
      SwappedPropertyVertexInPropertyEdge *pSwappedPropertyVertexInPropertyEdge = m_PropertyVerticesInPropertyEdgeChanged.GetNext( pos );
      pSwappedPropertyVertexInPropertyEdge->SwapFromTo();
   }

   //////////////////////////////////////////////////////////////////////////////////
   // Swap pFromPropertyVertex to pToPropertyVertex in PropertyPolys
   for (POSITION pos = m_PropertyVerticesInPropertyPolyChanged.GetHeadPosition(); pos;)
   {
      SwappedPropertyVertexInPropertyPoly *pSwappedPropertyVertexInPropertyPoly = m_PropertyVerticesInPropertyPolyChanged.GetNext( pos );
      pSwappedPropertyVertexInPropertyPoly->SwapFromTo();
   }

   /////////////////////////////////////////////////////////////////////
   // Rebuild m_pPoly[0] m_pPoly[1]
   for (UINT i=0; i<2; i++)
   {
      Poly *pPoly =  m_pPoly[i];

      if (!pPoly)
         continue;

      if (m_pCollapseFaceInfo[i]) // Collapse Poly because it will only have 2 edges
         m_pCollapseFaceInfo[i]->ReCollapse();
      else // Rebuild Poly with out m_pWeldedEdge or pFromVertex
      {
         CArray< Vertex * > newvertices;
         CArray< VertexNormal * > newvertexnormals;
         CArray< Edge * > newedges;
         CArray< CArray< PropertyVertex * > > newPropertyVertices;
         CArray< CArray< PropertyEdge * > > newPropertyEdges;

         UINT nNumOldSides = pPoly->GetNumSides();
         UINT nNumNewSides = nNumOldSides - 1;
         ASSERT(nNumNewSides > 2); // Temporary until I handle collapsing of faces

         newvertices.SetSize( nNumNewSides );
         newvertexnormals.SetSize( nNumNewSides );
         newedges.SetSize( nNumNewSides );

         UINT nNumPropertyPolys = pPoly->CountNumPropertyPolys();
         newPropertyVertices.SetSize(nNumPropertyPolys);
         newPropertyEdges.SetSize(nNumPropertyPolys);

         for (UINT nPropertyPolyIndex=0; nPropertyPolyIndex<nNumPropertyPolys; nPropertyPolyIndex++)
         {
            newPropertyVertices[nPropertyPolyIndex].SetSize( nNumNewSides );
            newPropertyEdges[nPropertyPolyIndex].SetSize( nNumNewSides );
         }

         UINT nNewIndex=0;
         for (UINT nIndex=0; nIndex<nNumOldSides; nIndex++ )
         {
            if (nIndex!=m_nVertexFaceRelativeIndex[i])
            {
               UINT nEdgeIndex;
               if (nIndex==m_nEdgeFaceRelativeIndex[i])
                  nEdgeIndex = pPoly->GetNextIndex( nIndex );
               else
                  nEdgeIndex = nIndex;

               newvertices[nNewIndex] = pPoly->GetVertexAt( nIndex );
               newvertexnormals[nNewIndex] = pPoly->GetVertexNormalAt(nIndex, FALSE);
               newedges[nNewIndex] = pPoly->GetEdgeAt(nEdgeIndex);

               UINT nPropertyPolyIndex=0;
               for (PropertyPoly *pPropertyPoly = pPoly->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nPropertyPolyIndex++)
               {
                  newPropertyVertices[nPropertyPolyIndex][nNewIndex] = pPropertyPoly->GetPropertyVertexAt( nIndex );
                  newPropertyEdges[nPropertyPolyIndex][nNewIndex] = pPropertyPoly->GetPropertyEdgeAt( nEdgeIndex );
               }

               nNewIndex++;
            }
         }

         pPoly->BuildPoly( newvertices, newvertexnormals, newedges, newPropertyVertices, newPropertyEdges );
      }
   }

   for (POSITION pos = m_CollapsedEdges.GetHeadPosition(); pos; )
   {
      CollapsedEdge *pCollapsedEdge = m_CollapsedEdges.GetNext( pos );
      pCollapsedEdge->CollapseFromTo();
   }

   SetRedoSelection();

   if (m_pMirrorInfo)
      m_pMirrorInfo->ReWeld();
}

void Vertex::WeldInfo::UnWeld()
{
   if (m_pMirrorInfo)
      m_pMirrorInfo->UnWeld();

   m_pWeldedEdge->m_bAllowReinsert = FALSE;
   /////////////////////////////////////////////////////////////////////
   for (POSITION pos = m_CollapsedEdges.GetTailPosition(); pos; )
   {
      CollapsedEdge *pCollapsedEdge = m_CollapsedEdges.GetPrev( pos );
      pCollapsedEdge->CollapseToFrom();
   }

   // Rebuild m_pPoly[0] m_pPoly[1]
   for (int i=1; i>=0; i--)
   {
      Poly *pPoly =  m_pPoly[i];

      if (!pPoly)
         continue;

      if (m_pCollapseFaceInfo[i]) // UnCollapse Poly because it was disconnected because it was a 3 point patch
         m_pCollapseFaceInfo[i]->UnCollapse();
      else // Rebuild Poly with m_pWeldedEdge and pFromVertex
         pPoly->BuildPoly( m_oldvertices[i], m_oldvertexnormals[i], m_oldedges[i], m_oldPropertyVertices[i], m_oldPropertyEdges[i] );
   }

   //////////////////////////////////////////////////////////////////////////////////
   // Swap pToPropertyVertex to pFromPropertyVertex in PropertyEdges
   for (POSITION pos = m_PropertyVerticesInPropertyEdgeChanged.GetTailPosition(); pos;)
   {
      SwappedPropertyVertexInPropertyEdge *pSwappedPropertyVertexInPropertyEdge = m_PropertyVerticesInPropertyEdgeChanged.GetPrev( pos );
      pSwappedPropertyVertexInPropertyEdge->SwapToFrom();
   }

   //////////////////////////////////////////////////////////////////////////////////
   // Swap pToPropertyVertex to pFromPropertyVertex in PropertyPolys
   for (POSITION pos = m_PropertyVerticesInPropertyPolyChanged.GetTailPosition(); pos;)
   {
      SwappedPropertyVertexInPropertyPoly *pSwappedPropertyVertexInPropertyPoly = m_PropertyVerticesInPropertyPolyChanged.GetPrev( pos );
      pSwappedPropertyVertexInPropertyPoly->SwapToFrom();
   }

   //////////////////////////////////////////////////////////////////////////////////
   // Swap pToVertexNormals to pFromVertexNormals in Polys
   for (int i=1; i>=0; i--)
   {
      for (POSITION pos = m_normalinpolyschanged[i].GetTailPosition(); pos;)
      {
         Poly *pPoly = m_normalinpolyschanged[i].GetPrev( pos );
         int nFaceRelativeIndex = pPoly->FindVertexNormalFaceRelativeIndex( m_pToVertexNormal[i] );
         if (nFaceRelativeIndex==-1)
            continue; // Must have been handled by a collapsed edge

         pPoly->SetVertexNormalAt( nFaceRelativeIndex, m_pFromVertexNormal[i] );
      }
   }

   //////////////////////////////////////////////////////////////////////////////////
   // Swap pToVertex to pFromVertex in Polys
   for (POSITION pos = m_vertexinpolyschanged.GetTailPosition(); pos;)
   {
      SwappedVertexInPoly *pSwapped = m_vertexinpolyschanged.GetPrev( pos );
      pSwapped->SwapToFrom();
   }

   //////////////////////////////////////////////////////////////////////////////////
   // Swap pToVertex to pFromVertex in Edges
   for (POSITION pos = m_edgeschanged.GetTailPosition(); pos;)
   {
      Edge *pEdge = m_edgeschanged.GetPrev( pos );
      if (pEdge->GetFirstVertex() == m_pToVertex )
         pEdge->SetFirstVertex( m_pFromVertex );
      else
         pEdge->SetSecondVertex( m_pFromVertex );
   }

   m_pWeldedEdge->m_bAllowReinsert = TRUE;
   m_pWeldedEdge->ReInsertIntoList();

   SetUndoSelection();
}

////////////////////////////////////////////////////////////////
CollapseFace::~CollapseFace()
{
   if (m_pCollapsedPoly)
      m_pCollapsedPoly->DeReference(); // Referenced because it gets removed
   if (m_pFromEdge)
      m_pFromEdge->DeReference(); // Referenced because it gets removed
   if (m_pToEdge)
      m_pToEdge->DeReference(); // Referenced because it gets removed when the collapsed face gets disconnected and it is a border edge, only temporarily.
   if (m_pFromSharedVertex)
      m_pFromSharedVertex->DeReference();
}

void CollapseFace::Fill( Edge *pFromEdge, Edge *pToEdge, Poly *pCollapsedPoly )
{
   m_pFromEdge = pFromEdge;
   m_pFromEdge->Reference(); // Because it will be removed

   m_pToEdge = pToEdge;
   m_pToEdge->Reference(); // Because it can temporarily get disconnected when the Collapse face gets disconnected

   Vertex *pSharedVertex = m_pFromEdge->GetSharedVertex( m_pToEdge );
   ASSERT(pSharedVertex);

   m_pCollapsedPoly = pCollapsedPoly;
   if (m_pCollapsedPoly)
   {
      m_pCollapsedPoly->Reference(); // Because it will be removed
      m_pConnectedPoly = m_pFromEdge->GetConnectedPoly( m_pCollapsedPoly );

      m_pFromSharedVertex = m_pCollapsedPoly->GetVertexAt( m_pCollapsedPoly->FindVertexFaceRelativeIndex( pSharedVertex ) );
      m_pToSharedVertex = m_pToEdge->GetFirstVertex()==pSharedVertex ? m_pToEdge->GetFirstVertex() : m_pToEdge->GetSecondVertex();
      if (m_pFromSharedVertex == m_pToSharedVertex) // If they are the same then no reason to swap them
      {
         m_pFromSharedVertex=NULL;
         m_pToSharedVertex = NULL;
      }

      if (m_pFromSharedVertex)
         m_pFromSharedVertex->Reference(); // Reference it because it will be removed
   }
   else
      m_pConnectedPoly = m_pFromEdge->GetPoly();

   // Lets figure PropertyEdge swapping now for the connected poly
   // It needs to change all PropertyPolys, FromPropertyEdge to the ToPropertyEdge as long as it isn't a UVBorder.
   // If it is a UVBorder edge then we will keep the PropertyEdge but we need to change its edge pointer to be the new ToEdge and remove it from the FromEdge list
   if (m_pConnectedPoly)
   {
      UINT nRelativeIndex = m_pFromEdge->GetFaceRelativeIndex( m_pConnectedPoly );
      ASSERT( nRelativeIndex != -1 );

      for (PropertyPoly *pPropertyPoly = m_pConnectedPoly->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
      {
         PropertySet *pFromPropertySet = pPropertyPoly->GetPropertySet();
         PropertyEdge *pFromPropertyEdge = pPropertyPoly->GetPropertyEdgeAt( nRelativeIndex );

         PropertyEdge *pToPropertyEdge = NULL;
         for (PropertyEdge *pPropertyEdge = m_pToEdge->GetHeadPropertyEdge(); pPropertyEdge; pPropertyEdge=pPropertyEdge->GetNextPropertyEdge())
         {
            PropertySet *pToPropertySet = pPropertyEdge->GetPropertySet();
            if (pToPropertySet == pFromPropertySet)
            {
               pToPropertyEdge = pPropertyEdge;
               break;
            }
         }

         if (pToPropertyEdge) // None UVBorder
         {
            m_fromPropertyEdges.SetAt( pFromPropertySet, pFromPropertyEdge );
            m_toPropertyEdges.SetAt( pFromPropertySet, pToPropertyEdge ); // FromTex and ToTex coords are the same
         }
      }
   }
}

void CollapseFace::ReCollapse()
{
   if (m_pCollapsedPoly)
      m_pCollapsedPoly->RemoveFromList();

   if (m_pConnectedPoly)
   {
      // Swap pFromSharedVertex for pToSharedVertex in pConnectedPoly
      if (m_pFromSharedVertex)
      {
         UINT nFaceRelativeIndex = m_pConnectedPoly->FindVertexFaceRelativeIndex( m_pFromSharedVertex );
         ASSERT( nFaceRelativeIndex!=-1 );

         m_pConnectedPoly->SetVertexAt( nFaceRelativeIndex, m_pToSharedVertex );
      }

      // Swap pFromEdge for pToEdge in pConnectedPoly
      UINT nFaceRelativeIndex = m_pFromEdge->GetFaceRelativeIndex( m_pConnectedPoly );
      ASSERT(nFaceRelativeIndex!=-1);

      m_pConnectedPoly->SetEdgeAt( nFaceRelativeIndex, m_pToEdge );

      // Handle Connected PropertyPolys
      for (PropertyPoly *pPropertyPoly = m_pConnectedPoly->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
      {
         PropertySet *pPropertySet = pPropertyPoly->GetPropertySet();
         PropertyEdge *pFromPropertyEdge = pPropertyPoly->GetPropertyEdgeAt( nFaceRelativeIndex );
         PropertyEdge *pToPropertyEdge = NULL;

         if (m_toPropertyEdges.Lookup( pPropertySet, pToPropertyEdge)) // Non UVBorder Edge
         {
            ASSERT( pToPropertyEdge );
            pPropertyPoly->SetPropertyEdgeAt( nFaceRelativeIndex, pToPropertyEdge);
         }
         else // Swap Edge pointer in PropertyEdge and change edge lists
         {
            m_pFromEdge->RemovePropertyEdge( pFromPropertyEdge );
            m_pToEdge->AddPropertyEdge( pFromPropertyEdge );
         }
      }
   }
}

void CollapseFace::UnCollapse()
{
   if (m_pConnectedPoly)
   {
      UINT nFaceRelativeIndex = m_pToEdge->GetFaceRelativeIndex( m_pConnectedPoly );
      ASSERT( nFaceRelativeIndex!=-1 );

      // Handle Connected PropertyPolys
      for (PropertyPoly *pPropertyPoly = m_pConnectedPoly->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
      {
         PropertySet *pPropertySet = pPropertyPoly->GetPropertySet();
         PropertyEdge *pFromPropertyEdge = NULL;

         if (m_fromPropertyEdges.Lookup( pPropertySet, pFromPropertyEdge)) // Non UVBorder Edge
         {
            ASSERT( pFromPropertyEdge );
            pPropertyPoly->SetPropertyEdgeAt( nFaceRelativeIndex, pFromPropertyEdge);
         }
         else // Swap Edge pointer in PropertyEdge and change edge lists
         {
            PropertyEdge *pOriginalFromPropertyEdge = pPropertyPoly->GetPropertyEdgeAt( nFaceRelativeIndex );
            
            m_pToEdge->RemovePropertyEdge( pOriginalFromPropertyEdge );
            m_pFromEdge->AddPropertyEdge( pOriginalFromPropertyEdge );
         }
      }

      // Swap ToEdge for FromEdge in Connected Poly
      m_pConnectedPoly->SetEdgeAt( nFaceRelativeIndex, m_pFromEdge );

      // Swap pToSharedVertex for pFromSharedVertex in pConnectedPoly
      if (m_pToSharedVertex)
      {
         UINT nFaceRelativeIndex = m_pConnectedPoly->FindVertexFaceRelativeIndex( m_pToSharedVertex );
         ASSERT( nFaceRelativeIndex!=-1 );

         m_pConnectedPoly->SetVertexAt( nFaceRelativeIndex, m_pFromSharedVertex );
      }
   }

   if (m_pCollapsedPoly)
      m_pCollapsedPoly->ReInsertIntoList();
}

//////////////////////////////////////////////////////////////
void SwappedPropertyVertexInPropertyPoly::SwapFromTo()
{
   UINT nIndex = m_pPropertyPoly->FindPropertyVertexFaceRelativeIndex( m_pFromPropertyVertex );
   ASSERT( nIndex!=-1 );

   m_pPropertyPoly->SetPropertyVertexAt( nIndex, m_pToPropertyVertex );
}

void SwappedPropertyVertexInPropertyPoly::SwapToFrom()
{
   UINT nIndex = m_pPropertyPoly->FindPropertyVertexFaceRelativeIndex( m_pToPropertyVertex );
   ASSERT( nIndex!=-1 );

   m_pPropertyPoly->SetPropertyVertexAt( nIndex, m_pFromPropertyVertex );
}

//////////////////////////////////////////////////////////////
SwappedPropertyVertexInPropertyEdge::SwappedPropertyVertexInPropertyEdge(PropertyEdge *pPropertyEdge, PropertyVertex *pFromPropertyVertex, PropertyVertex *pToPropertyVertex)
{
   m_pPropertyEdge = pPropertyEdge;
   m_pFromPropertyVertex = pFromPropertyVertex;
   m_pToPropertyVertex = pToPropertyVertex;
   m_pFromPropertyVertex->Reference();
   m_pToPropertyVertex->Reference();
}

SwappedPropertyVertexInPropertyEdge::~SwappedPropertyVertexInPropertyEdge()
{
   m_pFromPropertyVertex->DeReference();
   m_pToPropertyVertex->DeReference();
}

void SwappedPropertyVertexInPropertyEdge::SwapFromTo()
{
   if (m_pPropertyEdge->GetFirstPropertyVertex() == m_pFromPropertyVertex)
      m_pPropertyEdge->SetFirstPropertyVertex( m_pToPropertyVertex );
   else
   {
      ASSERT(m_pPropertyEdge->GetSecondPropertyVertex() == m_pFromPropertyVertex);
      m_pPropertyEdge->SetSecondPropertyVertex( m_pToPropertyVertex );
   }
}

void SwappedPropertyVertexInPropertyEdge::SwapToFrom()
{
   if (m_pPropertyEdge->GetFirstPropertyVertex() == m_pToPropertyVertex)
      m_pPropertyEdge->SetFirstPropertyVertex( m_pFromPropertyVertex );
   else
   {
      ASSERT(m_pPropertyEdge->GetSecondPropertyVertex() == m_pToPropertyVertex);
      m_pPropertyEdge->SetSecondPropertyVertex( m_pFromPropertyVertex );
   }
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOL Vertex::IsPeaked( Edge *pEdge )
{
   return pEdge->IsVertexPeaked(this);
}

/*
----------------------------------
|               |                |
|  BorderFace[0]|  BorderFace[1] |
|               |                |
|               |                |
|  BorderEdge[0||  BorderEdge[1] |
|---------------|----------------|
|           this|                |
|               |Edge            |
|               |                |
|   Face[0]     |    Face[1]     |
|               |                |
|---------------|----------------|
*/

VertexPeakInfo *Vertex::Peak( Edge *pEdge, BOOL bPeak )
{
   ASSERT(pEdge);
   Poly *pFace[2];
   pFace[0] = pEdge->GetPoly();
   pFace[1] = pEdge->GetConnectedPoly(pFace[0]);

   Edge *pBorderEdge[2];
   for (UINT i=0; i<2; i++)
   {
      if (!pFace[i])
      {
         pBorderEdge[i] = NULL;
         continue;
      }

      Edge *pTempEdge0 = NULL;
      Edge *pTempEdge1 = NULL;
      GetEdgePair( pFace[i], &pTempEdge0, &pTempEdge1 );

      if (pTempEdge0==pEdge)
         pBorderEdge[i] = pTempEdge1;
      else
      {
         ASSERT(pTempEdge1==pEdge);
         pBorderEdge[i] = pTempEdge0;
      }
   }

   Poly *pBorderFace[2];
   pBorderFace[0] = pBorderEdge[0] ? pBorderEdge[0]->GetConnectedPoly(pFace[0]) : NULL;
   pBorderFace[1] = pBorderEdge[1] ? pBorderEdge[1]->GetConnectedPoly(pFace[1]) : NULL;

   UINT nFaceVertexFaceRelativeIndex[2];
   for (UINT i=0; i<2; i++)
   {
      nFaceVertexFaceRelativeIndex[i] = pFace[i] ? pFace[i]->FindVertexFaceRelativeIndex(this) : NULL;
      ASSERT(nFaceVertexFaceRelativeIndex[i]!=-1);
   }

   UINT nBorderFaceVertexFaceRelativeIndex[2];
   for (UINT i=0; i<2; i++)
   {
      nBorderFaceVertexFaceRelativeIndex[i] = pBorderFace[i] ? pBorderFace[i]->FindVertexFaceRelativeIndex(this) : NULL;
      ASSERT(nBorderFaceVertexFaceRelativeIndex[i]!=-1);
   }

   VertexNormal *pFaceVertexNormal[2];
   for (UINT i=0; i<2; i++)
      pFaceVertexNormal[i] = pFace[i] ? pFace[i]->GetVertexNormalAt(nFaceVertexFaceRelativeIndex[i], FALSE) : NULL;

   VertexNormal *pBorderFaceVertexNormal[2];
   for (UINT i=0; i<2; i++)
      pBorderFaceVertexNormal[i] = pBorderFace[i] ? pBorderFace[i]->GetVertexNormalAt(nBorderFaceVertexFaceRelativeIndex[i], FALSE) : NULL;

   PolyModel *pModel = (PolyModel *)GetOwner();

   VertexPeakInfo *pInfo = SNEW VertexPeakInfo(this, pEdge);

   if (bPeak)
   {
      if (pFace[0] && pFaceVertexNormal[0] == pBorderFaceVertexNormal[0]) // Currently Soft
      {
         VertexNormal *pNewVertexNormal = pModel->AddVertexNormal();
         ChangedNormals *pChanged = SNEW ChangedNormals( pFace[0], nFaceVertexFaceRelativeIndex[0], pFaceVertexNormal[0], pNewVertexNormal );
         pInfo->AddChangedNormals(pChanged);

         if (pFaceVertexNormal[0] == pFaceVertexNormal[1])
         {
            ChangedNormals *pChanged = SNEW ChangedNormals( pFace[1], nFaceVertexFaceRelativeIndex[1], pFaceVertexNormal[1], pNewVertexNormal );
            pInfo->AddChangedNormals(pChanged);
         }
         else
         {
            if (pFace[1] && pFaceVertexNormal[1] == pBorderFaceVertexNormal[1])
            {
               pNewVertexNormal = pModel->AddVertexNormal(); // Alloc new one
               pChanged = SNEW ChangedNormals( pFace[1], nFaceVertexFaceRelativeIndex[1], pFaceVertexNormal[1], pNewVertexNormal );
               pInfo->AddChangedNormals(pChanged);
            }
         }
      }
      else if (pBorderFaceVertexNormal[0]== NULL) // Edge Vertex
      {
         pInfo->m_bChangedPeakFlagOnly = TRUE;
      }
   }
   else // Smooth
   {
      if (pFace[0] && pBorderFaceVertexNormal[0] && pFaceVertexNormal[0] != pBorderFaceVertexNormal[0]) // Currently Hard
      {
         ChangedNormals *pChanged = SNEW ChangedNormals( pFace[0], nFaceVertexFaceRelativeIndex[0], pBorderFaceVertexNormal[0],  pFaceVertexNormal[0] );
         pInfo->AddChangedNormals(pChanged);

         if (pFaceVertexNormal[0] == pFaceVertexNormal[1])
         {
            ChangedNormals *pChanged = SNEW ChangedNormals( pFace[1], nFaceVertexFaceRelativeIndex[1], pBorderFaceVertexNormal[0], pFaceVertexNormal[1] );
            pInfo->AddChangedNormals(pChanged);
         }
         else
         {
            if (pFace[1] && pFaceVertexNormal[1] != pBorderFaceVertexNormal[1])
            {
               pChanged = SNEW ChangedNormals( pFace[1], nFaceVertexFaceRelativeIndex[1], pBorderFaceVertexNormal[1], pFaceVertexNormal[1] );
               pInfo->AddChangedNormals(pChanged);
            }
         }
      }
      else if (pBorderFaceVertexNormal[0]== NULL) // Edge Vertex
      {
         pInfo->m_bChangedPeakFlagOnly = TRUE;
      }
   }

   if (pInfo->GetNumChangedVertexNormals()==0 && !pInfo->m_bChangedPeakFlagOnly)
   {
      delete pInfo;
      return NULL;
   }

   if (bPeak)
      pInfo->Peak();
   else
      pInfo->Smooth();

   static BOOL bInMirror;
   if (!bInMirror)
   {
      bInMirror = TRUE;
      Vertex *pMirrorThis = (Vertex *)GetMirrorObject();
      Edge *pMirrorEdge = (Edge *)pEdge->GetMirrorObject();

      if (pMirrorThis && pMirrorEdge)
         pInfo->m_pMirrorInfo = pMirrorThis->Peak(pMirrorEdge, bPeak);
      bInMirror = FALSE;
   }

   return pInfo;
}

////////////////////////////////////////////////////////////////////////////////////////////
ChangedNormals::ChangedNormals(Poly *pFace, UINT nIndex, VertexNormal *pOldVertexNormal, VertexNormal *pNewVertexNormal)
{
   m_pFace = pFace; ASSERT(m_pFace);
   m_nIndex = nIndex; ASSERT(m_nIndex != -1);
   m_pOldVertexNormal = pOldVertexNormal; ASSERT(m_pOldVertexNormal);
   m_pNewVertexNormal = pNewVertexNormal; ASSERT(m_pNewVertexNormal);

   m_pOldVertexNormal->Reference();
   m_pNewVertexNormal->Reference();
}

ChangedNormals::~ChangedNormals()
{
   m_pOldVertexNormal->DeReference();
   m_pNewVertexNormal->DeReference();
}

BOOL ChangedNormals::Peak()
{
   if (m_pFace->GetVertexNormalAt(m_nIndex, FALSE) == m_pOldVertexNormal)
   {
      m_pFace->SetVertexNormalAt(m_nIndex, m_pNewVertexNormal);
      return TRUE;
   }
   else
   { // Nothing to do, bad call
      ASSERT(FALSE);
      return FALSE;
   }
}

BOOL ChangedNormals::Smooth()
{
   if (m_pFace->GetVertexNormalAt(m_nIndex, FALSE) == m_pNewVertexNormal)
   {
      m_pFace->SetVertexNormalAt(m_nIndex, m_pOldVertexNormal);
      return TRUE;
   }
   else
   { // Nothing to do, bad call
      ASSERT(FALSE);
      return FALSE;
   }
}

////////////////////////////////////////////////////////////////////////////////////////////


VertexPeakInfo::~VertexPeakInfo()
{
   for (POSITION pos = m_changedlist.GetHeadPosition(); pos;)
      delete m_changedlist.GetNext(pos);

   if (m_pMirrorInfo)
      delete m_pMirrorInfo;
}

BOOL VertexPeakInfo::Peak()
{
   BOOL bAnyFailed = FALSE;

   for (POSITION pos = m_changedlist.GetHeadPosition(); pos;)
   {
      ChangedNormals *pChanged = m_changedlist.GetNext(pos);
      if (!pChanged->Peak())
         bAnyFailed = TRUE;
   }

   if (m_pEdge->GetFirstVertex() == m_pVertex)
      m_pEdge->SetFirstVertexPeakedFlag(TRUE);
   else
   {
      ASSERT(m_pEdge->GetSecondVertex() == m_pVertex);
      m_pEdge->SetSecondVertexPeakedFlag(TRUE);
   }

   Edge *pContEdge = m_pEdge->GetContinuousEdge(m_pVertex);
   if (pContEdge)
   {
      if (pContEdge->GetFirstVertex() == m_pVertex)
         pContEdge->SetFirstVertexPeakedFlag( TRUE );
      else
      {
         ASSERT(pContEdge->GetSecondVertex() == m_pVertex);
         pContEdge->SetSecondVertexPeakedFlag( TRUE );
      }
   }

   if (m_pMirrorInfo)
      m_pMirrorInfo->Peak();

   return !bAnyFailed;
}

BOOL VertexPeakInfo::Smooth()
{
   BOOL bAnyFailed = FALSE;

   for (POSITION pos = m_changedlist.GetTailPosition(); pos;)
   {
      ChangedNormals *pChanged = m_changedlist.GetPrev(pos);
      if (!pChanged->Smooth())
         bAnyFailed = TRUE;
   }

   if (m_pEdge->GetFirstVertex() == m_pVertex)
      m_pEdge->SetFirstVertexPeakedFlag(FALSE);
   else
   {
      ASSERT(m_pEdge->GetSecondVertex() == m_pVertex);
      m_pEdge->SetSecondVertexPeakedFlag(FALSE);
   }

   Edge *pContEdge = m_pEdge->GetContinuousEdge(m_pVertex);
   if (pContEdge)
   {
      if (pContEdge->GetFirstVertex() == m_pVertex)
         pContEdge->SetFirstVertexPeakedFlag( FALSE );
      else
      {
         ASSERT(pContEdge->GetSecondVertex() == m_pVertex);
         pContEdge->SetSecondVertexPeakedFlag( FALSE );
      }
   }

   if (m_pMirrorInfo)
      m_pMirrorInfo->Smooth();

   return !bAnyFailed;
}


