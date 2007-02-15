#include "stdafx.h"
#include "Edge.h"
#include "Poly.h"
#include "PolyModel.h"
#include "..\Utility\MathTools.h"
#include "..\Utility\AMacros.h"
#include "GUI/GLModelView.h"
#include "gui\Mainfrm.h"

extern CMainFrame *g_pMainFrame;

Edge::DeleteInfo::~DeleteInfo()
{
   if (m_pVertexDeleteInfo0)
      delete m_pVertexDeleteInfo0;

   if (m_pVertexDeleteInfo1)
      delete m_pVertexDeleteInfo1;

   SetOldPoly0(NULL);
   SetOldPoly1(NULL);
   SetNewPoly(NULL); // This will Dereference
}

////////////////////////////////////////////////////////////////////

IMPLEMENT_COMPONENT_CREATE( Edge, Edge )

Edge::~Edge()
{
   if (m_pEdgePolyInfo[0])
      delete m_pEdgePolyInfo[0];
   if (m_pEdgePolyInfo[1])
      delete m_pEdgePolyInfo[1];

   if (GetFirstVertex())
      GetFirstVertex()->DeReference( TRUE );
   if (GetSecondVertex())
      GetSecondVertex()->DeReference( TRUE );

   PropertyEdge *pNext;
   for (PropertyEdge *pPropertyEdge = GetHeadPropertyEdge(); pPropertyEdge; pPropertyEdge=pNext)
   {
      pNext = pPropertyEdge->GetNextPropertyEdge();
      pPropertyEdge->DeReference();
   }
}

BOOL Edge::GetDrawColor( RGBAFloat &color )
{
   if (IsManifold()) 
   {
      color.Set(1.0f, 0.0f, 0.0f, color.GetAlpha());
      return TRUE;
   }
   if (Component::GetDrawColor( color ))
      return TRUE;

   return FALSE;
}

void Edge::Draw( BOOL bDrawTess )
{
   glBegin(GL_LINES);

   glVertex3fv(&GetFirstVertex()->GetPos().x);
   glVertex3fv(&GetSecondVertex()->GetPos().x);

   if (IsNearestFocusObject())
   {
      SnipeObject *pFocusObject = SelectionList::GetFocusObject();
      ASSERT(pFocusObject);
      if (pFocusObject->IsKindOf( SOT_Vertex ))
      {
         Vertex *pVertex = (Vertex *)pFocusObject;
         RGBAFloat color;
         pVertex->GetDrawColor( color );

         glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), 0.8f );
         glVertex3fv(&pVertex->GetPos().x);

         glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), 0.0f );
         glVertex3fv(&GetConnectedVertex(pVertex)->GetPos().x);
      }
      else if (pFocusObject->IsKindOf( SOT_Poly ))
      {
         DrawPickNearest( (Poly *)pFocusObject );
      }
      else
      {
         ASSERT(FALSE);
      }
   }

   glEnd();
}

void Edge::Draw( CGLBaseView *pView )
{
   glLineWidth( IsBorderEdge() ? 3.0f : 1.4f );
   glBegin( GL_LINES );
      glVertex3fv( &GetFirstVertex()->GetRTVertexHead()->GetPos().x );
      glVertex3fv( &GetSecondVertex()->GetRTVertexHead()->GetPos().x );
   glEnd();
}

void Edge::DrawSelected( CGLBaseView *pView )
{
Component::DrawSelected( pView );
return;

   float fAlpha = IsSoftEdge() ? 0.6f : 1.0f;

   glDisable( GL_DEPTH_TEST );
      glColor4f( g_selectedcolor.GetRed(), g_selectedcolor.GetGreen(), g_selectedcolor.GetBlue(), g_selectedcolor.GetAlpha()*0.15f );
      Draw( pView );
   glEnable( GL_DEPTH_TEST );

   glColor4f( g_selectedcolor.GetRed(), g_selectedcolor.GetGreen(), g_selectedcolor.GetBlue(), g_selectedcolor.GetAlpha()*fAlpha );
   Draw( pView );
}

void Edge::DrawMirrored( CGLBaseView *pView )
{
Component::DrawMirrored( pView );
return;

   glDisable( GL_DEPTH_TEST );
      glColor4f( g_mirrorcolor.GetRed(), g_mirrorcolor.GetGreen(), g_mirrorcolor.GetBlue(), g_mirrorcolor.GetAlpha()*0.15f );
      Draw( pView );
   glEnable( GL_DEPTH_TEST );

   float fAlpha = IsSoftEdge() ? 0.6f : 1.0f;
   glColor4f( g_mirrorcolor.GetRed(), g_mirrorcolor.GetGreen(), g_mirrorcolor.GetBlue(), g_mirrorcolor.GetAlpha()*fAlpha );
   Draw( pView );
}

void Edge::DrawFocused( CGLBaseView *pView )
{
Component::DrawFocused( pView );
return;

   glDisable( GL_DEPTH_TEST );
      glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), g_focuscolor.GetAlpha()*0.3f );
      Draw( pView );
   glEnable( GL_DEPTH_TEST );

   glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), g_focuscolor.GetAlpha() );
   Draw( pView );
}

void Edge::DrawNearestFocused( CGLBaseView *pView, SnipeObject *pFocusObject )
{
   if (pFocusObject->IsKindOf( SOT_Vertex ))
   {
      Vertex *pVertex = (Vertex *)pFocusObject;

      glDisable( GL_DEPTH_TEST );
      glBegin( GL_LINES );
         glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), g_focuscolor.GetAlpha()*0.2f );
         glVertex3fv( &pVertex->GetRTVertexHead()->GetPos().x );
         glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), 0.0f );
         glVertex3fv( &GetConnectedVertex( pVertex )->GetRTVertexHead()->GetPos().x);
      glEnd();

      glEnable( GL_DEPTH_TEST );
      glBegin( GL_LINES );
         glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), g_focuscolor.GetAlpha()*0.6f );
         glVertex3fv( &pVertex->GetRTVertexHead()->GetPos().x );
         glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), 0.0f );
         glVertex3fv( &GetConnectedVertex( pVertex )->GetRTVertexHead()->GetPos().x);
      glEnd();
   }
}

void Edge::GetVector(Vector &vect)
{
   vect = GetSecondVertex()->GetPos() - GetFirstVertex()->GetPos();
}

BOOL Edge::GetWorldPos( Vector &pos )
{
   pos = GetFirstVertex()->GetPos();
   pos += GetSecondVertex()->GetPos();
   pos *= 0.5;

   return TRUE;
}

void Edge::PreTransform()
{
   m_bTransformed = FALSE;

   GetFirstVertex()->PreTransform();
   GetSecondVertex()->PreTransform();
}

void Edge::Transform( const Matrix44 &matrix )
{
   if (m_bTransformed || IsLocked())
      return;

   GetFirstVertex()->Transform( matrix );
   GetSecondVertex()->Transform( matrix );

   SetDirty();
   if (IsSelected())
      g_pMainFrame->GetMainInterface()->GetCurrentSelectionList()->SetBoundsDirty(); // master to use global

   m_bTransformed = TRUE;
}

BOOL Edge::GetAngle(Edge *pNextEdge,float &angle)
{
   Vector A,B;
   GetVector(A);
   pNextEdge->GetVector(B);
   A.Normalize();
   B.Normalize();
   angle = A.GetAngle(B);
   return TRUE;
}

// figures out vertexnormal and uv connectivity
BOOL Edge::AddPoly( Poly *pPoly, UINT nEdgeIndex )
{
   if ((m_pEdgePolyInfo[0])&&(m_pEdgePolyInfo[1]))
   {
//      ASSERT(FALSE);
      return FALSE;//already full
   }

   if (!m_bInList)
      ReInsertIntoList();

   SetDirty();

   if (( !m_pEdgePolyInfo[0] &&  m_pEdgePolyInfo[1] && m_pEdgePolyInfo[1]->m_pPoly!=pPoly ) 
    || ( !m_pEdgePolyInfo[0] && !m_pEdgePolyInfo[1] ))
      SetFirstPoly(pPoly, nEdgeIndex);

   if (( !m_pEdgePolyInfo[1] &&  m_pEdgePolyInfo[0] && m_pEdgePolyInfo[0]->m_pPoly!=pPoly ) 
    || ( !m_pEdgePolyInfo[0] && !m_pEdgePolyInfo[1] ))
      SetSecondPoly(pPoly, nEdgeIndex);

   SetSoftEdgeDirtyFlag( TRUE );

   return TRUE;
}

void Edge::FindClosestIntersection( CGLBaseView *pView, const CPoint &point, float &fPos )
{
   TransformableObject *pTransformableObject = (TransformableObject*)GetOwner();
   Vector p0 = GetFirstVertex()->GetWorldPos(pTransformableObject);
   Vector p1 = GetSecondVertex()->GetWorldPos(pTransformableObject);

   pView->WorldToScreen( p0 );
   pView->WorldToScreen( p1 );

   Vector2 vMouse(point.x-p0.x, point.y-p0.y);
   Vector2 vEdge(p1.x-p0.x, p1.y-p0.y);
   float fLen = vEdge.Length();

   fPos = vEdge.Dot( vMouse ) / (fLen * fLen);

   fPos = min(1.0f, max(0.0f, fPos));
}

void Edge::DirtyDependentFaces()
{
   GetFirstVertex()->DirtyDependentFaces();
   GetSecondVertex()->DirtyDependentFaces();
}

// static
Edge *Edge::FindEdge( const Vertex *pVertex0, const Vertex *pVertex1)
{
   // uses surrounding edges
   for (UINT i = 0; i < pVertex0->GetNumSurroundingEdges(); i++)
   {
      Edge *pEdge = pVertex0->GetSurroundingEdgeAt(i);
      if (pEdge->GetFirstVertex() == pVertex1 || pEdge->GetSecondVertex() == pVertex1)
         return pEdge;
   }
   return NULL;
}

Edge *Edge::FindEdgeFromFaces(Vertex *pVertex0, Vertex *pVertex1)
{
   //uses surrounding faces
   for (UINT i = 0; i < pVertex0->GetNumSurroundingPolys(); i++)
   {
      Poly *pSurroundingPoly = pVertex0->GetSurroundingPolyAt(i);
      Edge *pPrevEdge, *pNextEdge;
      pVertex0->GetEdgePair(pSurroundingPoly,&pPrevEdge,&pNextEdge);
      Vertex *pV0 = pPrevEdge->GetFirstVertex();
      Vertex *pV1 = pPrevEdge->GetSecondVertex();
      if ((pVertex0==pV1 && pVertex1==pV0) || (pVertex0==pV0 && pVertex1==pV1))
         return pPrevEdge;
      pV0 = pNextEdge->GetFirstVertex();
      pV1 = pNextEdge->GetSecondVertex();
      if ((pVertex0==pV1 && pVertex1==pV0) || (pVertex0==pV0 && pVertex1==pV1))
         return pNextEdge;
   }
   return NULL;
}

SnipeObject *Edge::FindMirrorObject()
{
   PolyModel *pModel = (PolyModel *)GetOwner(); 
   if (!pModel)
      return NULL;

   Vertex *pMirroredVertexArray[2];
   pMirroredVertexArray[0] = (Vertex *)GetFirstVertex()->GetMirrorObject();
   if (pMirroredVertexArray[0]==NULL)
      return NULL;
   pMirroredVertexArray[1] = (Vertex *)GetSecondVertex()->GetMirrorObject();
   if (pMirroredVertexArray[1]==NULL)
      return NULL;

   return Edge::FindEdge(pMirroredVertexArray[0], pMirroredVertexArray[1]);
}

BOOL Edge::IsUsed()
{
   return GetFirstPoly() || GetSecondPoly();
}

void Edge::Disconnect()
{
   if (GetSecondVertex())
      GetSecondVertex()->RemoveSurroundingEdge( this );
 
   if (GetFirstVertex())
      GetFirstVertex()->RemoveSurroundingEdge( this );

   Component::Disconnect();
}

void Edge::Reconnect()
{
   Component::Reconnect();

   if (GetFirstVertex())
      GetFirstVertex()->AddSurroundingEdge( this );

   if (GetSecondVertex())
      GetSecondVertex()->AddSurroundingEdge(this);
}

BOOL Edge::Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError )
{
   DeleteInfo *&pEdgeDeleteInfo = (DeleteInfo *&)pDeleteInfo;

   if (pEdgeDeleteInfo) // Must be redo
   {
      if (pEdgeDeleteInfo->m_pVertexDeleteInfo0 || pEdgeDeleteInfo->m_pVertexDeleteInfo1)
      {
         if (pEdgeDeleteInfo->m_pVertexDeleteInfo0)
            pEdgeDeleteInfo->m_pVertexDeleteInfo0->Delete();

         if (pEdgeDeleteInfo->m_pVertexDeleteInfo1)
            pEdgeDeleteInfo->m_pVertexDeleteInfo1->Delete();
         return TRUE;
      }
      else if (pEdgeDeleteInfo->GetNewPoly()) // Must Be Redo
      {
         pEdgeDeleteInfo->GetOldPoly0()->RemoveFromList();
         pEdgeDeleteInfo->GetOldPoly1()->RemoveFromList();
         pEdgeDeleteInfo->GetNewPoly()->ReInsertIntoList();

         if (pEdgeDeleteInfo->GetReversedWindingPoly0())
            pEdgeDeleteInfo->GetOldPoly0()->ReverseWinding();
      }
      else // Must be dangle
         ((Edge *)pEdgeDeleteInfo->GetSnipeObject())->RemoveFromList();

      if (pEdgeDeleteInfo->GetReversedEdge())
         pEdgeDeleteInfo->GetReversedEdge()->ReverseSpline();

      return Component::Delete( pDeleteInfo, strOutError );
   }

   // Test Validity
   if (IsBorderEdge())
   {
      strOutError = (String) "Could not delete Edge(" + GetMatchName() + ").  Edge is on a border.";
      return FALSE;
   }

   Poly *pPoly = GetPoly(); 
   if ( pPoly && !IsBorderEdge() && IsUVBorderEdge() )
   {
      strOutError = (String) "Could not delete Edge("  + GetMatchName() + ").  Edge is on a uv border.";
      return FALSE;
   }

   pEdgeDeleteInfo = SNEW DeleteInfo( this );

   if (IsDangle())
   {
      RemoveFromList();
      return Component::Delete( pDeleteInfo, strOutError );
   }

   Vertex *pV0 = GetFirstVertex();
   Vertex *pV1 = GetSecondVertex();

   if (pV0->GetNumSurroundingEdges()==2)
   {
      String strError;
      if (!pV0->Delete((SnipeObject::DeleteInfo *&)pEdgeDeleteInfo->m_pVertexDeleteInfo0, strError ))
      {
         strOutError = (String) "Could not delete Edge("  + GetMatchName() + ").  Illegal face would be formed.";
         return FALSE;
      }
      Edge *pNewEdge = pEdgeDeleteInfo->m_pVertexDeleteInfo0->GetNewEdge();
      //Set ID Here
      if (pNewEdge->GetID() == GetID())// it used the wrong ID
      {
         Edge *pOldEdge0 = pEdgeDeleteInfo->m_pVertexDeleteInfo0->GetOldEdge0();
         Edge *pOldEdge1 = pEdgeDeleteInfo->m_pVertexDeleteInfo0->GetOldEdge1();
         UINT nNewID = pOldEdge0->GetID()==pNewEdge->GetID() ? pOldEdge1->GetID() : pOldEdge0->GetID();
         pNewEdge->SafeSetID( nNewID );
      }
   }

   if (pV1->GetNumSurroundingEdges()==2)
   {
      String strError;
      if (!pV1->Delete((SnipeObject::DeleteInfo *&)pEdgeDeleteInfo->m_pVertexDeleteInfo1, strError))
      {
         strOutError = (String) "Could not delete Edge("  + GetMatchName() + ").  Illegal face would be formed.";
         return FALSE;
      }
      Edge *pNewEdge = pEdgeDeleteInfo->m_pVertexDeleteInfo1->GetNewEdge();
      //Set ID Here
      if (pNewEdge->GetID() == GetID())// it used the wrong ID
      {
         Edge *pOldEdge0 = pEdgeDeleteInfo->m_pVertexDeleteInfo1->GetOldEdge0();
         Edge *pOldEdge1 = pEdgeDeleteInfo->m_pVertexDeleteInfo1->GetOldEdge1();
         UINT nNewID = pOldEdge0->GetID()==pNewEdge->GetID() ? pOldEdge1->GetID() : pOldEdge0->GetID();
         pNewEdge->SafeSetID( nNewID );
      }
   }

   if (pEdgeDeleteInfo->m_pVertexDeleteInfo0 || pEdgeDeleteInfo->m_pVertexDeleteInfo1)
      return TRUE;

   Poly *pPoly0 = m_pEdgePolyInfo[0]->m_pPoly;
   Poly *pPoly1 = m_pEdgePolyInfo[1]->m_pPoly;

   pEdgeDeleteInfo->SetOldPoly0( pPoly0 );
   pEdgeDeleteInfo->SetOldPoly1( pPoly1 );

   if (!pPoly0->IsAdjacentPolyWindingTheSame( this ))
   {
      pEdgeDeleteInfo->SetReversedWindingPoly0( TRUE );
      pPoly0->ReverseWinding();
   }

   UINT nNumPropertyPolys = pPoly->CountNumPropertyPolys(); // we can use either patch because they will identical
   CArray< CArray<PropertyVertex*> > PropertyVertices;
   CArray< CArray<PropertyEdge*> > propertyedges;
   PropertyVertices.SetSize(nNumPropertyPolys);
   propertyedges.SetSize( nNumPropertyPolys );

   PolyModel *pModel = (PolyModel *)pPoly0->GetOwner();

   Vertex *pVertex0 = GetFirstVertex();
   Vertex *pVertex1 = GetSecondVertex();

   CArray<Vertex*> vertices;
   CArray<VertexNormal*> vertexnormals;
   CArray<Edge*> edges;

   // walk poly0
   // first figure which direction to walk
   int nStartIndex = pPoly0->FindVertexFaceRelativeIndex(pVertex0);
   int nEndIndex = pPoly0->FindVertexFaceRelativeIndex(pVertex1);
   ASSERT(nStartIndex>=0);
   ASSERT(nEndIndex>=0);

   BOOL bStartWithVertex0 = TRUE;
   if (pPoly0->GetNextIndex(nStartIndex)==nEndIndex) // need to start on other side because we always want to walk incrementing index
   {
      UINT nTemp = nStartIndex;
      nStartIndex = nEndIndex;
      nEndIndex = nTemp;
      bStartWithVertex0=FALSE;
   }

   for ( UINT i=nStartIndex; i!=nEndIndex; i=pPoly0->GetNextIndex(i))
   {
      Edge *pEdge = pPoly0->GetEdgeAt(i);
      Vertex *pVertex = pPoly0->GetVertexAt(i);
      ASSERT(pVertex);

      vertices.Add( pVertex );
      vertexnormals.Add( pPoly0->GetVertexNormalAt(i) );
      edges.Add( pEdge );

      UINT nCount=0;
      for ( PropertyPoly *pPropertyPoly = pPoly0->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++ )
      {
         propertyedges[nCount].Add( pPropertyPoly->GetPropertyEdgeAt(i) );
         PropertyVertices[nCount].Add( pPropertyPoly->GetPropertyVertexAt(i) );
      }
   }

   // walk poly1
   // first figure which direction to walk
   if (bStartWithVertex0)
   {
      nStartIndex = pPoly1->FindVertexFaceRelativeIndex(pVertex0);
      nEndIndex = pPoly1->FindVertexFaceRelativeIndex(pVertex1);
   }
   else
   {
      nStartIndex = pPoly1->FindVertexFaceRelativeIndex(pVertex1);
      nEndIndex = pPoly1->FindVertexFaceRelativeIndex(pVertex0);
   }

   nStartIndex = pPoly1->GetNextIndex(nStartIndex);
   nEndIndex = pPoly1->GetPrevIndex(nEndIndex);

   for ( UINT i=nStartIndex; i!=nEndIndex; i=pPoly1->GetNextIndex(i))
   {
      Edge *pEdge = pPoly1->GetEdgeAt(i);
      Vertex *pVertex = pPoly1->GetVertexAt(i);
      ASSERT(pVertex);

      vertices.Add( pVertex );
      vertexnormals.Add( pPoly1->GetVertexNormalAt(i) );
      edges.Add( pEdge );

      UINT nCount=0;
      for ( PropertyPoly *pPropertyPoly = pPoly1->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++ )
      {
         propertyedges[nCount].Add( pPropertyPoly->GetPropertyEdgeAt(i) );
         PropertyVertices[nCount].Add( pPropertyPoly->GetPropertyVertexAt(i) );
      }
   }

   RemoveFromList();
   pPoly0->RemoveFromList();
   pPoly1->RemoveFromList();

   Poly *pNewPoly = pModel->AddPoly( pPoly0->GetID() ); // This must be after Poly0 is removed from the list
   pNewPoly->SetVisible( pPoly0->IsVisible() || pPoly1->IsVisible() );
   pEdgeDeleteInfo->SetNewPoly( pNewPoly );
   pNewPoly->SetNumSides( vertices.GetSize() );

   // We need to reinsert these in the reverse order that we removed them
   for (UINT nPass=0; nPass<3; nPass++)
   {
      for (int nPoly = 0; nPoly < 2; nPoly++)
      {
         Poly *pPoly = nPoly==0 ? pPoly1 : pPoly0;
         for ( UINT i=0; i < pPoly->GetNumSides(); i++)
         {
            Edge *pEdge = pPoly->GetEdgeAt(i);
            if (pEdge == this)
               continue;

            for (int j=0; j < edges.GetSize(); j++)
            {
               if (edges.GetAt(j) == pEdge)
               {
                  switch(nPass)
                  {
                  case 0:
                     pNewPoly->SetEdgeAt( j, edges[j] );
                     break;
                  case 1:
                     pNewPoly->SetVertexNormalAt( j, vertexnormals[j] );
                     break;
                  case 2:
                     pNewPoly->SetVertexAt( j,vertices[j] );
                     break;
                  }
                  break;
               }
            }
         }
      }
   }

   // we need all the edges inserted first before we insert the PropertyEdges
   for (int nPoly = 0; nPoly < 2; nPoly++)
   {
      Poly *pPoly = nPoly==0 ? pPoly1 : pPoly0;
      for ( UINT i=0; i < pPoly->GetNumSides(); i++)
      {
         Edge *pEdge = pPoly->GetEdgeAt(i);
         if (pEdge == this)
            continue;

         for (int j=0; j < edges.GetSize(); j++)
         {
            if (edges.GetAt(j) == pEdge)
            {
               UINT nCount = 0;
               for ( PropertyPoly *pPropertyPoly = pPoly->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++ )
               {
                  PropertySet *pPropertySet = pPropertyPoly->GetPropertySet();
                  PropertyPoly *pNewPropertyPoly = pPropertySet->AddPoly( pNewPoly );
                  pNewPropertyPoly->SetPropertyVertexAt( j, PropertyVertices[nCount][j] );
                  pNewPropertyPoly->SetPropertyEdgeAt( j, propertyedges[nCount][j] );
               }
               break;
            }
         }
      }
   }

   // create texture info on new polys
   for (UINT nSurfaceIndex=0; nSurfaceIndex<pPoly0->GetNumSurfaces(); nSurfaceIndex++)
   {
      Surface *pSurface = pPoly0->GetSurfaceAt( nSurfaceIndex );
      pSurface->AddFace( pNewPoly );
   }

   return Component::Delete( pDeleteInfo, strOutError );
}

BOOL Edge::UnDelete( SnipeObject::DeleteInfo *pDeleteInfo )
{
   DeleteInfo *pEdgeDeleteInfo = (DeleteInfo *)pDeleteInfo;

   if (pEdgeDeleteInfo->m_pVertexDeleteInfo0 || pEdgeDeleteInfo->m_pVertexDeleteInfo1)
   {
      if (pEdgeDeleteInfo->m_pVertexDeleteInfo1)
         pEdgeDeleteInfo->m_pVertexDeleteInfo1->UnDelete();

      if (pEdgeDeleteInfo->m_pVertexDeleteInfo0)
         pEdgeDeleteInfo->m_pVertexDeleteInfo0->UnDelete();

      return TRUE;
   }

   if (pEdgeDeleteInfo->GetReversedEdge())
      pEdgeDeleteInfo->GetReversedEdge()->ReverseSpline();

   if ( !pEdgeDeleteInfo->GetOldPoly0() ) // Must be dangle
   {
      ASSERT (!pEdgeDeleteInfo->GetOldPoly1());
      ((Edge *)pEdgeDeleteInfo->GetSnipeObject())->ReInsertIntoList();

      return Component::UnDelete( pDeleteInfo );
   }

   pEdgeDeleteInfo->GetNewPoly()->RemoveFromList();
   pEdgeDeleteInfo->GetOldPoly0()->ReInsertIntoList(); // Don't want these reversed because we want them to get the same poly pointer in the edge, Poly0, Poly1
   pEdgeDeleteInfo->GetOldPoly1()->ReInsertIntoList();

   if (pEdgeDeleteInfo->GetReversedWindingPoly0())
      pEdgeDeleteInfo->GetOldPoly0()->ReverseWinding();

   return Component::UnDelete( pDeleteInfo );
}

float Edge::CalculateDot( Edge *pEdge )
{
   Vertex *pSharedVertex = GetSharedVertex(pEdge);

   Vector A;
   A=pSharedVertex->GetPos() - GetConnectedVertex(pSharedVertex)->GetPos();
   A.Normalize();

   Vector B;
   B=pEdge->GetConnectedVertex(pSharedVertex)->GetPos() - pSharedVertex->GetPos();
   B.Normalize();

   return A.Dot(B);
}

Edge *Edge::GetContinuousEdgeByDot( Vertex *pVertex, float fMinDot, Edge *pNoCallBackEdge/*=NULL*/ )
{
   Edge *pClosesEdge = NULL;
   float fOriginalMinDot = fMinDot;

   for (UINT i=0; i < pVertex->GetNumSurroundingEdges(); i++)
   {
      Edge *pEdge = pVertex->GetSurroundingEdgeAt(i);
      if (pEdge == this)
         continue;

      float fDot = CalculateDot( pEdge );
      if ( fDot > fMinDot )
      {
         if (pEdge != pNoCallBackEdge && pEdge->GetContinuousEdgeByDot( pVertex, fOriginalMinDot, this ) != this) // Make sure I can gt Back
            continue; // This isn't the closes edge because we can't get back

         pClosesEdge = pEdge;
         fMinDot = fDot;
      }
   }
   return pClosesEdge;
}

Edge *Edge::GetContinuousEdge( Vertex *pVertex, float fAngleTolerance/*=50.0f*/ )
{
   // Do Smartest test using polys
   UINT nNumSurroundingPolys = pVertex->GetNumSurroundingPolys();
   UINT nNumSurroundingEdges = pVertex->GetNumSurroundingEdges();
   if ( nNumSurroundingEdges == nNumSurroundingPolys && !ISODD(nNumSurroundingEdges) )
   {
      // We want to return opposite edge of this now;
      UINT nNumSteps = nNumSurroundingEdges/2;
      Edge *pEdge = this;
      for (UINT i=0; i < nNumSteps; i++)
         pEdge = pVertex->GetNextEdgeClockwise(pEdge);
      return pEdge;
   }

   // Border Edge T corner case
   if (nNumSurroundingPolys == 2 && nNumSurroundingEdges == 3)
   {
      if (!IsBorderEdge()) // Don't want to test by angle
         return NULL;

      for (UINT i=0; i < nNumSurroundingEdges; i++)
      {
         Edge *pEdge = pVertex->GetSurroundingEdgeAt(i);
         if (pEdge != this && pEdge->IsBorderEdge())
            return pEdge;
      }
   }

   // OK we must find edge by angle/dot now (Odd number of surrounding polys or dangles (Edges missing faces)
   float fMinDot = cos(fAngleTolerance*DEGTORAD);
   return GetContinuousEdgeByDot( pVertex, fMinDot );
}

Edge *Edge::GetContinuousEdge( Edge *pPrev, BOOL bStartOtherDirectionIfNUll/*=FALSE*/, float fAngleTolerance/*=50.0f*/ )
{
   Vertex *pSharedVertex;
   if (pPrev)
      pSharedVertex = GetSharedVertex(pPrev);
   else
   {
      if (bStartOtherDirectionIfNUll)
         pSharedVertex = GetFirstVertex();
      else
         pSharedVertex = GetSecondVertex();
   }

   Vertex *pNextVertex = GetConnectedVertex( pSharedVertex );
   return GetContinuousEdge(pNextVertex, fAngleTolerance);
}

void Edge::DrawPick( CGLModelView *pView, BOOL bDrawTess ) 
{ 
   if (CGLModelView::m_bSinglePick && pView->IsDisplayShaded() && !IsDangle())
   {
      Poly *pPoly = GetFirstPoly();
      if (pPoly)
      {
         if (bDrawTess)
            pPoly->DrawPickNearest(this);
         else
            pPoly->Poly::DrawPickNearest(this);
      }

      pPoly = GetSecondPoly();
      if (pPoly)
      {
         if (bDrawTess)
            pPoly->DrawPickNearest(this);
         else
            pPoly->Poly::DrawPickNearest(this);
      }
   }
   else
      Draw( bDrawTess );
}


void Edge::DrawPickNearest( Vertex *pVertex )
{
   glBegin( GL_LINES );
      Vector vPos = pVertex->GetPos();
      glVertex3fv(&vPos.x);

      vPos += GetConnectedVertex(pVertex)->GetPos();
      vPos /= 2.0f;
      glVertex3fv(&vPos.x);
   glEnd();
}

void Edge::DrawPickNearest( Poly *pFace )
{
}

void Edge::RemoveVertex( Vertex *pVertex )
{
   if (GetFirstVertex() == pVertex)
      SetFirstVertex( NULL );
   else if (GetSecondVertex() == pVertex)
      SetSecondVertex( NULL );
   else
      return; // vertex not used in this edge.  this can happen in spline models.

   SetDirty();
}

void Edge::RemovePoly(Poly *pPoly, BOOL bRemoveIfNotUsed/*=TRUE*/)
{
   if (GetFirstPoly() == pPoly)
      SetFirstPoly( NULL, 0);
   else if (GetSecondPoly() == pPoly)
      SetSecondPoly( NULL, 0);
   else
   {
//      ASSERT(FALSE);// poly doesn't exist anywhere
      return;
   }

   SetSoftEdgeDirtyFlag(TRUE);
   SetDirty();

   if (bRemoveIfNotUsed && !IsUsed())
      RemoveFromList();
}

void Edge::SetDirty()
{
   Poly *pPoly = GetPoly();
   if (pPoly)
   {
      pPoly->SetDirty();
      Poly *pConnectedPoly = GetConnectedPoly(pPoly);
      if (pConnectedPoly)
         pConnectedPoly->SetDirty();
   }
}

BOOL Edge::IsAligned( Poly *pPoly )
{
   EdgePolyInfo *pEdgePolyInfo = GetEdgePolyInfo(pPoly);
   if (pEdgePolyInfo) {
      UINT nEdgeIndex = pEdgePolyInfo->m_nEdgeIndex;
      Vertex *pVertex0 = pPoly->GetVertexAt( nEdgeIndex );
      Vertex *pVertex1 = pPoly->GetVertexAt( pPoly->GetNextIndex(nEdgeIndex) );
      return GetFirstVertex()==pVertex0 && GetSecondVertex()==pVertex1;
   }
   else
      return FALSE;
}

Vector Edge::GetPosAlongEdge( float fPosition, Poly *pPoly/*=NULL*/ )
{
   ASSERT(fPosition >= 0.0 && fPosition <= 1.0f);
   if (pPoly)
      return (GetAlignedSecondVertex(pPoly)->GetPos() - GetAlignedFirstVertex(pPoly)->GetPos()) * fPosition + GetAlignedFirstVertex(pPoly)->GetPos();
   return (GetSecondVertex()->GetPos() - GetFirstVertex()->GetPos()) * fPosition + GetFirstVertex()->GetPos();
}

Vertex::DeleteInfo *Edge::SplitEdge( float fPosition/*=0.5*/, Edge **pOutEdge0/*=NULL*/, Edge **pOutEdge1/*=NULL*/   )
{
   Vertex::DeleteInfo *pVertexDeleteInfo = SNEW Vertex::DeleteInfo(NULL);

   pVertexDeleteInfo->SetNewEdge( this );

   BOOL bIsSoft = IsSoftEdge();

   Poly *pPoly0 = GetPoly();
   PolyModel *pModel = (PolyModel *)GetOwner();

   Vertex *pNewVertex = pModel->AddVertex();
   pVertexDeleteInfo->SetSnipeObject( pNewVertex );
   pVertexDeleteInfo->SetPoly0( pPoly0 );

   if (pPoly0)
   {
      Poly *pPoly1 = GetConnectedPoly(pPoly0);
      pVertexDeleteInfo->SetPoly1( pPoly1 );

      Vertex *pV0 = GetAlignedFirstVertex(pPoly0);
      Vertex *pV1 = GetAlignedSecondVertex(pPoly0);

      VertexNormal *pNewVertexNormal = pModel->AddVertexNormal();
      pVertexDeleteInfo->SetOldPolyVertexNormal( 0, pNewVertexNormal );

      CArray< PropertyVertex *> newPropertyVertices[2];

      UINT nNumPoly0PropertyPolys = pPoly0 ? pPoly0->CountNumPropertyPolys() : 0;
      UINT nNumPoly1PropertyPolys = pPoly1 ? pPoly1->CountNumPropertyPolys() : 0;

      newPropertyVertices[0].SetSize(nNumPoly0PropertyPolys);
      pVertexDeleteInfo->SetNumNewPropertyEdges( 0, nNumPoly0PropertyPolys );

      if (pPoly1)
      {
         newPropertyVertices[1].SetSize(nNumPoly1PropertyPolys);
         pVertexDeleteInfo->SetNumNewPropertyEdges( 1, nNumPoly1PropertyPolys );
      }

      CMap<PropertySet *, PropertySet *, PropertyVertex *, PropertyVertex *> m_sharedPropertyVertices;
      m_sharedPropertyVertices.InitHashTable( 17 );

      //Can't walk edges here because they are not guaranteed to be in the same order as PropertyPolys, which is used in InsertVertex 
      for (UINT i=0; i<2; i++)
      {
         Poly *pPoly = i==0 ? pPoly0 : pPoly1;
         if (!pPoly)
            continue;

         UINT nFaceRelativeIndex = GetFaceRelativeIndex( pPoly );
         ASSERT(nFaceRelativeIndex!=-1);

         UINT nUVIndex = 0;
         for (PropertyPoly *pPropertyPoly = pPoly->GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nUVIndex++)
         {
            PropertySet *pPropertySet = pPropertyPoly->GetPropertySet();
            PropertyVertex *pPropertyVertex = NULL;
            if (i>0)
               m_sharedPropertyVertices.Lookup(pPropertySet, pPropertyVertex);
            if (!pPropertyVertex)
            {
               pPropertyVertex = pPropertySet->AddPropertyVertex();
               m_sharedPropertyVertices.SetAt(pPropertySet, pPropertyVertex);
            }

            newPropertyVertices[i][nUVIndex] = pPropertyVertex;
            PropertyEdge *pPropertyEdge = pPropertyPoly->GetPropertyEdgeAt( nFaceRelativeIndex );
            pVertexDeleteInfo->SetNewPropertyEdge( i, nUVIndex, pPropertyEdge );

         }
      }

      pVertexDeleteInfo->SetOldPolyNumPropertySets( 0, nNumPoly0PropertyPolys );
      for (UINT i=0; i < nNumPoly0PropertyPolys; i++)
         pVertexDeleteInfo->SetOldPolyPropertySetPropertyVertex( 0, i, newPropertyVertices[0][i] );

      Edge *pNewEdge0 = NULL;
      Edge *pNewEdge1 = NULL;
      CArray<PropertyEdge *> newPropertyEdges0;
      CArray<PropertyEdge *> newPropertyEdges1;

      pPoly0->InsertVertex( pNewVertex, pNewVertexNormal, &newPropertyVertices[0], pV0, fPosition, pNewEdge0, newPropertyEdges0, pNewEdge1, newPropertyEdges1 );

      pVertexDeleteInfo->SetOldEdge0(pNewEdge0);
      pVertexDeleteInfo->SetOldEdge1(pNewEdge1);
      pVertexDeleteInfo->SetOldPolyVertexIndex( 0, pPoly0->FindVertexFaceRelativeIndex( pNewVertex ) );

      ASSERT( nNumPoly0PropertyPolys == newPropertyEdges0.GetCount() && nNumPoly0PropertyPolys == newPropertyEdges1.GetCount());

      pVertexDeleteInfo->SetNumOldPropertyEdges0( 0, nNumPoly0PropertyPolys );
      pVertexDeleteInfo->SetNumOldPropertyEdges1( 0, nNumPoly0PropertyPolys );
      for (UINT i=0; i < nNumPoly0PropertyPolys; i++)
      {
         pVertexDeleteInfo->SetOldPropertyEdge0( 0, i, newPropertyEdges0[i] );
         pVertexDeleteInfo->SetOldPropertyEdge1( 0, i, newPropertyEdges1[i] );
      }

      if (pPoly1)
      {
         if (!bIsSoft)
            pNewVertexNormal = pModel->AddVertexNormal();
         pVertexDeleteInfo->SetOldPolyVertexNormal( 1, pNewVertexNormal );

         pVertexDeleteInfo->SetOldPolyNumPropertySets( 1, newPropertyVertices[1].GetCount() );
         for (int i=0; i < newPropertyVertices[1].GetCount(); i++)
            pVertexDeleteInfo->SetOldPolyPropertySetPropertyVertex( 1, i, newPropertyVertices[1][i] );

         newPropertyEdges0.RemoveAll(); // These can be different on each poly, so lets erase them and not reuse them
         newPropertyEdges1.RemoveAll(); // These can be different on each poly, so lets erase them and not reuse them

         pPoly1->InsertVertex( pNewVertex, pNewVertexNormal, &newPropertyVertices[1], pV1, fPosition, pNewEdge0, newPropertyEdges0, pNewEdge1, newPropertyEdges1 );

         pVertexDeleteInfo->SetOldPolyVertexIndex( 1, pPoly1->FindVertexFaceRelativeIndex( pNewVertex ) );

         ASSERT( nNumPoly1PropertyPolys == newPropertyEdges0.GetCount() && nNumPoly1PropertyPolys == newPropertyEdges1.GetCount());

         pVertexDeleteInfo->SetNumOldPropertyEdges0( 1, nNumPoly1PropertyPolys );
         pVertexDeleteInfo->SetNumOldPropertyEdges1( 1, nNumPoly1PropertyPolys );
         for (UINT i=0; i < nNumPoly1PropertyPolys; i++)
         {
            pVertexDeleteInfo->SetOldPropertyEdge0( 1, i, newPropertyEdges0[i] );
            pVertexDeleteInfo->SetOldPropertyEdge1( 1, i, newPropertyEdges1[i] );
         }
      }
   }
   else // Splitting a dangle;
   {
      Vector pos = GetPosAlongEdge(fPosition);
      pNewVertex->SetPos(pos);
      RemoveFromList(); // Must be after GetPosALongEdge and before AddEdge belows

      // pNewEdge0 and pNewEdge1 are always aligned with pEdge
      Edge *pNewEdge0 = pModel->AddEdge( GetFirstVertex(), pNewVertex );
      Edge *pNewEdge1 = pModel->AddEdge( pNewVertex, GetSecondVertex() );

      pVertexDeleteInfo->SetOldEdge0(pNewEdge0);
      pVertexDeleteInfo->SetOldEdge1(pNewEdge1);
   }

   Edge *pMirrorEdge = (Edge *)GetMirrorObject();
   if ( pMirrorEdge && pMirrorEdge!= this )
   {
      Vertex *pMirrorV0 = pMirrorEdge->GetFirstVertex();
      float fMirrorPos = fPosition;

      ASSERT(GetFirstVertex()->GetMirrorObject());

      if (!(((Vertex *)GetFirstVertex()->GetMirrorObject())==pMirrorV0)) // Mirrored edge run in opposite direction
         fMirrorPos = 1.0f-fMirrorPos;

      pVertexDeleteInfo->m_pMirrorDeleteInfo = pMirrorEdge->SplitEdge( fMirrorPos );

      // this is a hack for splitting polys that border the mirror seam
      Vector vPos = ((Vertex *)pVertexDeleteInfo->GetSnipeObject())->GetPos();
      vPos.x = -vPos.x;
      ((Vertex *)pVertexDeleteInfo->m_pMirrorDeleteInfo->GetSnipeObject())->SetPos( vPos );

      // Do we need to do similar with UV's.  I don't think so because I don't think it is common for UVs to mirror because they will usually be in a different TexCoord
   }

   if (pOutEdge0)
      *pOutEdge0 = pVertexDeleteInfo->GetOldEdge0();
   if (pOutEdge1)
      *pOutEdge1 = pVertexDeleteInfo->GetOldEdge1();

   return pVertexDeleteInfo;
}

void Edge::PushBounds( BoundingBox &box, CGLBaseView *pView/*=NULL*/ )
{
   Vector vPos;

   vPos = GetFirstVertex()->GetPos();
   if (pView)
      pView->WorldToView( vPos );
   box.Push( vPos );

   vPos = GetSecondVertex()->GetPos();
   if (pView)
      pView->WorldToView( vPos );
   box.Push( vPos );
}

UINT Edge::GetSaveFlags()
{
   UINT nFlags = SF_NONE;

   if (IsFirstVertexPeaked())
      nFlags |= SF_V0PEAKED;

   if (IsSecondVertexPeaked())
      nFlags |= SF_V1PEAKED;

   return nFlags;
}

void Edge::ApplySaveFlags( UINT nFlags )
{
   if (nFlags & SF_V0PEAKED)
      Edge::SetFirstVertexPeakedFlag( TRUE );

   if (nFlags & SF_V1PEAKED)
      Edge::SetSecondVertexPeakedFlag( TRUE );
}

void Edge::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   Component::Save( bs );

   bs << sp << GetFirstVertex()->GetID() << sp << GetSecondVertex()->GetID() << sp  << GetSaveFlags();

   if (bWriteHeader)
      bs << nl;
}

UINT Edge::LoadGuts( BaseStream &bs, UINT &nFlags )
{
   if (!Component::Load( bs ))
      return FALSE;

   PolyModel *pPolyModel = (PolyModel *)GetOwner();

   UINT nFirstVertexID;
   bs >> nFirstVertexID;
   Vertex *pFirstVertex = pPolyModel->FindVertex( nFirstVertexID );
   SetFirstVertex( pFirstVertex );

   UINT nSecondVertexID;
   bs >> nSecondVertexID;
   Vertex *pSecondVertex = pPolyModel->FindVertex( nSecondVertexID );
   SetSecondVertex( pSecondVertex );

   bs >> nFlags;
   ApplySaveFlags( nFlags );

   return TRUE;
}

BOOL Edge::Load( BaseStream &bs )
{
   UINT nFlags;
   return LoadGuts( bs, nFlags );
}

BOOL Edge::IsUVBorderEdge()
{
   for (PropertyEdge *pPropertyEdge = GetHeadPropertyEdge(); pPropertyEdge; pPropertyEdge=pPropertyEdge->GetNextPropertyEdge())
   {
      if (pPropertyEdge->GetPropertySet()->GetSOT()==SOT_TexCoords)
      {
         if (pPropertyEdge->IsBorderEdge())
            return TRUE;
      }
   }
   return FALSE;
}

void Edge::AddPropertyEdge( PropertyEdge *pNewPropertyEdge )
{
   pNewPropertyEdge->SetEdge( this );

   PropertyEdge **pUplink, *pPropertyEdge;
   for (pUplink = &m_pHeadPropertyEdge, pPropertyEdge = *pUplink; pPropertyEdge; pUplink = pPropertyEdge->GetNextPropertyEdgeUplink(), pPropertyEdge = *pUplink )
      if (pPropertyEdge == pNewPropertyEdge) // already in here, but be from a propertyset::reconnect (OnObjectNotify - NOTIFYOBJ_ADDEDTOPROJECT)
         return;

   *pUplink = pNewPropertyEdge;
   pNewPropertyEdge->SetNextPropertyEdge( NULL );
   pNewPropertyEdge->Reference();
}

PropertyEdge *Edge::FindPropertyEdge( PropertyVertex *pPropertyVertex0, PropertyVertex *pPropertyVertex1)
{
   for (PropertyEdge *pPropertyEdge = GetHeadPropertyEdge(); pPropertyEdge; pPropertyEdge=pPropertyEdge->GetNextPropertyEdge())
   {
      if ( pPropertyEdge->IsInEdge( pPropertyVertex0 ) && pPropertyEdge->IsInEdge( pPropertyVertex1 ))
         return pPropertyEdge;
   }

   return NULL;
}

BOOL Edge::RemovePropertyEdge( PropertyEdge *pMatchPropertyEdge )
{
   PropertyEdge **pUplink, *pPropertyEdge;
   for (pUplink = &m_pHeadPropertyEdge, pPropertyEdge = *pUplink; pPropertyEdge && pPropertyEdge!=pMatchPropertyEdge; pUplink = pPropertyEdge->GetNextPropertyEdgeUplink(), pPropertyEdge = *pUplink )
      ;

   if( pPropertyEdge==NULL )
      return FALSE;

   *pUplink = pPropertyEdge->GetNextPropertyEdge();
   pMatchPropertyEdge->DeReference();
   return TRUE;
}

UINT Edge::CountNumPropertyEdges()
{
   UINT nCount=0;
   for (PropertyEdge *pPropertyEdge=GetHeadPropertyEdge(); pPropertyEdge; pPropertyEdge=pPropertyEdge->GetNextPropertyEdge())
      nCount++;

   return nCount;
}

PropertyEdge *Edge::FindPropertyEdge( PropertySet *pMatchPropertySet, PropertyVertex *pMatchPropertyVertex/* = NULL */ )
{
   for ( PropertyEdge *pPropertyEdge = GetHeadPropertyEdge(); pPropertyEdge; pPropertyEdge=pPropertyEdge->GetNextPropertyEdge())
   {
      if (pPropertyEdge->GetPropertySet() == pMatchPropertySet)
      {
         if (pMatchPropertyVertex)
            if (pPropertyEdge->IsInEdge(pMatchPropertyVertex))
               return pPropertyEdge;
            else
               continue;
         return pPropertyEdge;
      }
   }
   return NULL;
}

Edge *Edge::GetOppositeEdge( Poly *&pPoly, BOOL *bOutAligned/*=NULL*/ )
{
   if (!pPoly)
      return NULL;

   UINT nConstrainedCount = 0;
   for (UINT i=0; i<pPoly->GetNumSides(); i++)
   {
      Vertex *pVertex = pPoly->GetVertexAt(i);
      if (pVertex->IsKindOf(SOT_PatchVertex) && ((PatchVertex *)pVertex)->IsConstrained())
      {
         PatchVertex *pPatchVertex = (PatchVertex *)pVertex;
         PatchEdge *pEdge0, *pEdge1;

         if (pPatchVertex->GetConstrainedBetween(pEdge0, pEdge1))
         {
            // we want to know whether this poly uses the vertex for an edge other than the constrained between edge,
            // if so we don't need to consider it constrained for our check
            Edge *pEdgePair0 = NULL, *pEdgePair1 = NULL;
            pVertex->GetEdgePair( pPoly, &pEdgePair0, &pEdgePair1);
            if ((pEdgePair0 == (Edge *)pEdge0 || pEdgePair0 == (Edge *)pEdge1)
               && (pEdgePair1 == (Edge *)pEdge0 || pEdgePair1 == (Edge *)pEdge1))
               nConstrainedCount++;
         }
      }
   }

   int nIndex = GetFaceRelativeIndex( pPoly );
   Edge *pNewEdge = NULL;

   if (nConstrainedCount==0)
   {
      if (ISODD( pPoly->GetNumSides() ))
         return NULL;

      int nSteps = pPoly->GetNumSides()/2;
      int nNewIndex = nIndex >= nSteps ? nIndex-nSteps : nIndex+nSteps;
      pNewEdge = pPoly->GetEdgeAt(nNewIndex);
   }
   else // Has Constrained
   {
      int nNumSides = pPoly->GetNumSides() - nConstrainedCount;
      if (ISODD( nNumSides ))
         return NULL;

      int nSteps = nNumSides/2;

      int nPrevIndex = nIndex;
      Edge *pPrevEdge = this; 
      for (int i=0; i<nSteps; i++)
      {
         UINT nNextIndex = pPoly->GetNextIndex(nPrevIndex);
         Edge *pNextEdge = pPoly->GetEdgeAt(nNextIndex);

         PatchVertex *pSharedVertex = (PatchVertex *)pPrevEdge->GetSharedVertex(pNextEdge);// Cast is OK because we know it is a patchmodel in this if
         if (pSharedVertex->IsConstrained())
            i--;

         pPrevEdge = pNextEdge;
         nPrevIndex = nNextIndex;
      }
      pNewEdge = pPrevEdge;
   }

   ASSERT(pNewEdge);

   if (bOutAligned && pNewEdge)
   {
      BOOL bThisAligned = IsAligned(pPoly);
      BOOL bNewAligned = pNewEdge->IsAligned(pPoly);
      *bOutAligned = bThisAligned != bNewAligned;
   }

   pPoly = pNewEdge->GetConnectedPoly(pPoly);

   if (pNewEdge->IsKindOf(SOT_PatchEdge) && nConstrainedCount && ((PatchVertex *)pNewEdge->GetFirstVertex())->IsConstrained() || ((PatchVertex *)pNewEdge->GetSecondVertex())->IsConstrained())
      return NULL;

   return pNewEdge;
}
