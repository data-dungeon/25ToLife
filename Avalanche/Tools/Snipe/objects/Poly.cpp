#include "stdafx.h"
#include "PropertyPoly.h"
#include "PolyModel.h"
#include "../Utility/MathTools.h"
#include "GUI/GLModelView.h"
#include "Interface/GLHitArray.h"
#include "Properties/SurfaceDriver.h"
#include "CreateFace.h"
#include "gui\Mainfrm.h"
#include "PropertyPoly.h"

IMPLEMENT_COMPONENT_CREATE( Poly, Face )

extern CMainFrame *g_pMainFrame;

Poly::~Poly()
{
   if (m_pVertices)
   {
      PropertyPoly *pNextPropertyPoly;
      for (PropertyPoly *pPropertyPoly = m_pHeadPropertyPoly; pPropertyPoly; pPropertyPoly = pNextPropertyPoly )
      {
         pNextPropertyPoly = pPropertyPoly->GetNextPropertyPoly();
         pPropertyPoly->DeReference();
      }

      delete [] m_pVertices;

      ASSERT(m_pRTVertexIndices);
      delete [] m_pRTVertexIndices;

      for (UINT i=0; i<GetNumSides(); i++)
      {
         Edge *pEdge = GetEdgeAt(i);
         if (pEdge)
            pEdge->DeReference( TRUE );

         VertexNormal *pVertexNormal = GetVertexNormalAt(i, FALSE);
         if (pVertexNormal)
            pVertexNormal->DeReference( TRUE );
      }
      ASSERT(m_pEdges);
      delete [] m_pEdges;

      ASSERT(m_pVertexNormals);
      delete [] m_pVertexNormals;
   }

   if (m_pSurfaces)
      delete [] m_pSurfaces;
}

void Poly::SetNumSides( UINT nNumSides )
{
   if (m_nNumSides == nNumSides)
      return;

   for (UINT nPass=0; nPass < 4; nPass++)
   {
      for (int i = m_nNumSides-1; i >=0; i--)
      {
         // Polys don't reference vertices, only edges do since they are the ones in charge of destroying them
         switch (nPass)
         {
            case 0:
               SetVertexAt(i, NULL);
               break;
            case 1:
               SetVertexNormalAt(i, NULL);
               break;
            case 2:
               SetEdgeAt(i, NULL);
               break;
            case 3:
            {
               for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly = pPropertyPoly->GetNextPropertyPoly())
               {
                  pPropertyPoly->SetPropertyVertexAt(i, NULL); 
                  pPropertyPoly->SetPropertyEdgeAt(i, NULL);
               }

               break;
            }
         }
      }
   }

   if (nNumSides > m_nNumSides || nNumSides == 0)
   {
      if (m_pVertices)
      {
         delete [] m_pVertices;
         m_pVertices = NULL;

         delete [] m_pRTVertexIndices;
         m_pRTVertexIndices = NULL;

         ASSERT(m_pVertexNormals);
         delete [] m_pVertexNormals ;
         m_pVertexNormals = NULL;

         ASSERT(m_pEdges);
         delete [] m_pEdges;
         m_pEdges = NULL;

         for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly = pPropertyPoly->GetNextPropertyPoly())
         {
            ASSERT(pPropertyPoly->m_pPropertyVertices);
            delete [] pPropertyPoly->m_pPropertyVertices;
            pPropertyPoly->m_pPropertyVertices = NULL;

            ASSERT(pPropertyPoly->m_pPropertyEdges);
            delete [] pPropertyPoly->m_pPropertyEdges;
            pPropertyPoly->m_pPropertyEdges = NULL;
         }

      }

      if (nNumSides > m_nNumSides )
      {
         m_pVertices = SNEW Vertex *[nNumSides];
         memset( m_pVertices, 0, nNumSides * sizeof(Vertex *));

         m_pRTVertexIndices = SNEW UINT [nNumSides];
         memset( m_pRTVertexIndices, 0, nNumSides * sizeof(UINT));

         m_pVertexNormals = SNEW VertexNormal *[nNumSides];
         memset( m_pVertexNormals, 0, nNumSides * sizeof(VertexNormal *));

         m_pEdges = SNEW Edge *[nNumSides];
         memset( m_pEdges, 0, nNumSides * sizeof(Edge *));

         for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly = pPropertyPoly->GetNextPropertyPoly())
         {
            ASSERT(pPropertyPoly->m_pPropertyVertices==NULL);
            pPropertyPoly->m_pPropertyVertices = SNEW PropertyVertex *[nNumSides];
            memset( pPropertyPoly->m_pPropertyVertices, 0, nNumSides * sizeof(PropertyVertex *));

            ASSERT(pPropertyPoly->m_pPropertyEdges==NULL);
            pPropertyPoly->m_pPropertyEdges = SNEW PropertyEdge *[nNumSides];
            memset( pPropertyPoly->m_pPropertyEdges, 0, nNumSides * sizeof(PropertyEdge *));
         }
      }
   }

   m_nNumSides = nNumSides;
}

void Poly::SetVertexAt( UINT nIndex, Vertex *pVertex )
{
   ASSERT(nIndex < GetNumSides());

   if (m_pVertices[nIndex])
   {
      if (m_pVertices[nIndex] == pVertex)
         return;

      m_pVertices[nIndex]->RemoveSurroundingPoly( this );
   }

   m_pVertices[nIndex] = pVertex;

   // Polys don't reference vertices, only edges do since they are the ones in charge of destroying them
   if (pVertex)
      pVertex->AddSurroundingPoly( this );
}

BOOL Poly::SetEdgeAt( UINT nIndex, Edge *pEdge )
{
   ASSERT(nIndex < GetNumSides());

   BOOL bResult = TRUE;
   if (m_pEdges[nIndex])
   {
      if (m_pEdges[nIndex] == pEdge)
         return TRUE;

      m_pEdges[nIndex]->RemovePoly( this );
      m_pEdges[nIndex]->DeReference();
   }

   if (pEdge)
   {
      bResult = pEdge->AddPoly( this, nIndex );
      if (bResult)
         pEdge->Reference();
      else
         g_outputWindow << "<W>Warning: Manifold Edge(" << pEdge->GetID() << ") on Face(" << GetID() << "). V1=" << pEdge->GetFirstVertex()->GetID() << ", V2=" << pEdge->GetSecondVertex()->GetID() << "\n";

      SetDirty();
   }

   m_pEdges[nIndex] = pEdge;

   return bResult;
}

void Poly::SetVertexNormalAt( UINT nIndex, VertexNormal *pVertexNormal )
{
   ASSERT(nIndex < GetNumSides());

   if (m_pVertexNormals[nIndex])
   {
      if (m_pVertexNormals[nIndex] == pVertexNormal)
         return;
      m_pVertexNormals[nIndex]->RemovePoly( this );
      m_pVertexNormals[nIndex]->DeReference();
   }

   m_pVertexNormals[nIndex] = pVertexNormal;

   if (pVertexNormal)
   {
      pVertexNormal->Reference();
      pVertexNormal->AddPoly( this );
      pVertexNormal->SetVertexNormalDirtyFlag( TRUE );
      SetDirty();
   }

   Vertex *pVertex = GetVertexAt( nIndex );
   if (pVertex)
   {
      Edge *pEdge0 = NULL, *pEdge1 = NULL;
      pVertex->GetEdgePair( this, &pEdge0, &pEdge1 );
      if (pEdge0)
         pEdge0->SetSoftEdgeDirtyFlag( TRUE );
      if (pEdge1)
         pEdge1->SetSoftEdgeDirtyFlag( TRUE );
   }
}

void Poly::Isolate()
{
   g_outputWindow << "<W>Warning: Isolating Face(" << GetID() << ").\n";

   PolyModel *pPolyModel = (PolyModel*)GetOwner();

   UINT nNumSides = GetNumSides();
   UINT nNumPropertyPolys = CountNumPropertyPolys();
   CArray< Vertex * > newvertices;
   CArray< VertexNormal * > newnormals;
   CArray< Edge * > newedges;
   CArray< CArray< PropertyVertex * > > newPropertyVertices;
   CArray< CArray< PropertyEdge * > > newpropertyedges;
   newvertices.SetSize( nNumSides );
   newnormals.SetSize( nNumSides );
   newedges.SetSize( nNumSides );
   newPropertyVertices.SetSize( nNumPropertyPolys );
   newpropertyedges.SetSize( nNumPropertyPolys );
   for (UINT nPropertyPolyIndex=0; nPropertyPolyIndex<nNumPropertyPolys; nPropertyPolyIndex++)
   {
      newPropertyVertices[nPropertyPolyIndex].SetSize( nNumSides );
      newpropertyedges[nPropertyPolyIndex].SetSize( nNumSides );
   }

   Edge *pPrevNewEdge = NULL;
   for (UINT i=0; i<nNumSides; i++)
   {
      Vertex *pOldVertex = GetVertexAt(i);
      Vertex *pNewVertex = pPolyModel->AddVertex( pOldVertex->GetPos() );
      newvertices[i] = pNewVertex;
      VertexNormal *pOldVertexNormal = GetVertexNormalAt(i);
      if( pOldVertexNormal )
         newnormals[i] = pPolyModel->AddVertexNormal( pOldVertexNormal->GetDir() );
      Edge *pNewEdge = pPolyModel->AddEdge();
      newedges[i] = pNewEdge;

      pNewEdge->SetManifoldFlag(TRUE);
      pNewEdge->SetFirstVertex( pNewVertex );
      if (pPrevNewEdge)
         pPrevNewEdge->SetSecondVertex( pNewVertex );

      pPrevNewEdge = pNewEdge;
   }
   if (pPrevNewEdge)
      pPrevNewEdge->SetSecondVertex( newvertices[0] );

   nPropertyPolyIndex = 0;
   for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly = pPropertyPoly->GetNextPropertyPoly(), nPropertyPolyIndex++)
   {
      PropertySet *pPropertySet = pPropertyPoly->GetPropertySet();
      PropertyEdge *pPrevNewPropertyEdge = NULL;
      for (UINT i=0; i<nNumSides; i++)
      {
//         PropertyVertex *pNewPropertyVertex = pPropertySet->AddPropertyVertex( pPropertyPoly->GetPropertyVertexAt(i)->GetPos() );
         PropertyVertex *pNewPropertyVertex = pPropertySet->DuplicateComponent( pPropertyPoly->GetPropertyVertexAt(i) );

         newPropertyVertices[nPropertyPolyIndex][i] = pNewPropertyVertex;

         PropertyEdge *pNewPropertyEdge = pPropertySet->AddPropertyEdge( newedges[i] );
         newpropertyedges[nPropertyPolyIndex][i] = pNewPropertyEdge;

         pNewPropertyEdge->SetFirstPropertyVertex( pNewPropertyVertex );
         if (pPrevNewPropertyEdge)
            pPrevNewPropertyEdge->SetSecondPropertyVertex( pNewPropertyVertex );

         pPrevNewPropertyEdge = pNewPropertyEdge;
      }
      if (pPrevNewPropertyEdge)
         pPrevNewPropertyEdge->SetSecondPropertyVertex( newPropertyVertices[nPropertyPolyIndex][0] );
   }

   BuildPoly( newvertices, newnormals, newedges, newPropertyVertices, newpropertyedges );
}

BOOL Poly::RemoveSurface( Surface *pSurface )
{
   UINT nIndex = FindSurface( pSurface );
   if (nIndex == -1)
      return FALSE;

   RemoveSurfaceAt( nIndex );
   return TRUE;
}

void Poly::RemoveSurfaceAt( UINT nIndex )
{
   ASSERT(nIndex < m_nNumSurfaces);

   if (m_nNumSurfaces==1)
   {
      delete [] m_pSurfaces;
      m_pSurfaces = NULL;
      m_nNumSurfaces = 0;
      return;
   }

   Surface **pNewSurfaces = SNEW Surface *[m_nNumSurfaces-1];

   UINT d=0;
   for (UINT i=0; i<m_nNumSurfaces; i++)
   {
      if (i==nIndex)
         continue;

      pNewSurfaces[d++] = m_pSurfaces[i]; 
   }

   delete [] m_pSurfaces;
   m_pSurfaces = pNewSurfaces;

   m_nNumSurfaces--;
}

Edge *Poly::GetEdgeAt( UINT nIndex )
{
   ASSERT(nIndex < GetNumSides());
   return m_pEdges[nIndex];
}

BOOL Poly::GetDrawColor( RGBAFloat &color )
{
   if (IsNearestFocusObject()) 
   {
      color.Set(0.0f, 0.5f, 0.0f, color.GetAlpha());
      return TRUE;
   }
   if (Component::GetDrawColor( color ))
      return TRUE;

   return FALSE;
}

BOOL Poly::Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError )
{
   if (pDeleteInfo == NULL)
      pDeleteInfo = SNEW DeleteInfo( this );

   RemoveFromList();
   
   return Component::Delete( pDeleteInfo, strOutError );
}

BOOL Poly::UnDelete( SnipeObject::DeleteInfo *pDeleteInfo )
{
   if (!Component::UnDelete( pDeleteInfo ))
      return FALSE;

   ReInsertIntoList();

   return TRUE;
}

BOOL Poly::GetWorldPos( Vector &pos )
{
   pos.Set(0);

   for (UINT i=0; i<GetNumSides(); i++)
      pos += m_pVertices[i]->GetPos();

   pos /= GetNumSides();

   return TRUE;
}

void Poly::PreTransform()
{
   m_bTransformed = FALSE;

   for (UINT i=0; i< GetNumSides(); i++)
      m_pVertices[i]->PreTransform();
}

void Poly::Transform( const Matrix44 &matrix )
{
   if (m_bTransformed || IsLocked())
      return;

   for (UINT i=0; i<GetNumSides(); i++)
      m_pVertices[i]->Transform( matrix );

   if (IsSelected())
      g_pMainFrame->GetMainInterface()->GetCurrentSelectionList()->SetBoundsDirty(); // faster to use global

   m_bTransformed = TRUE;
}

UINT Poly::FindEdge( Edge *pFind )
{
   for (UINT nEdgeIndex = 0; nEdgeIndex < GetNumSides(); nEdgeIndex++)
   {
      Edge *pEdge = GetEdgeAt(nEdgeIndex);

      if (pEdge == pFind)
         return nEdgeIndex;
   }

   return -1;
}

Edge *Poly::FindEdgeClosestToPoint( const Vector &point )
{
   Edge *pClosestEdge=NULL;
   float distance, smallestdistance = 0.0;
   Vector i;
   BOOL nFlag = TRUE;
   TransformableObject *pTransformableObject = (TransformableObject*)GetOwner();

   for (UINT nEdgeIndex = 0; nEdgeIndex < GetNumSides(); nEdgeIndex++)
   {
      Edge *pEdge = GetEdgeAt(nEdgeIndex);
      Vertex *pV0 = pEdge->GetFirstVertex();
      Vertex *pV1 = pEdge->GetSecondVertex();
      Vector p0 = pV0->GetWorldPos(pTransformableObject);
      Vector p1 = pV1->GetWorldPos(pTransformableObject);
      float percent;
      MathTools::PointToLineIntersection(point,p0,p1,i,percent);
      if ((percent<0.0f)||(percent>1.0f))
         continue;
      distance = (i-point).LengthSq();
      if ((nFlag)||(distance<smallestdistance))
      {
         smallestdistance = distance;
         pClosestEdge = pEdge;
         nFlag = FALSE;
      } 
   }
   return pClosestEdge;
}

void Poly::DirtyDependentFaces()
{
   for (UINT i = 0; i < GetNumSides(); i++)
   {
      Edge *pEdge = GetEdgeAt( i );
      if (pEdge)
         pEdge->SetDirty();
   }
}

void Poly::SetDirty()
{
   SetDirtyFaceNormal( TRUE );
}


void Poly::Draw( const Matrix44 &viewmatrix, BOOL bDrawTess, SurfaceDriver *pSurfaceDriver/*=NULL*/)
{
   LPARAM lParam = 0L;
   if (pSurfaceDriver && !pSurfaceDriver->IsValid( this, lParam ))
      return;

   Vector2 uv;
   UINT numVerts = GetNumSides();
   glBegin( numVerts==4 ? GL_QUADS : GL_POLYGON );
      for (UINT i = 0; i < numVerts; i++)
      {
         if (pSurfaceDriver)
         {
            uv = pSurfaceDriver->GetUVAt( viewmatrix, i, this, lParam );
            glTexCoord2f( uv.x, uv.y );
         }
         glNormal3fv( &GetVertexNormalAt(i)->GetDir().x );
         glVertex3fv( &GetVertexAt(i)->GetPos().x );
      }
      //if (IsNearestFocusObject())
      //{
      //   SnipeObject *pFocusObject = SelectionList::GetFocusObject();
      //   ASSERT(pFocusObject);
      //   if (pFocusObject->IsKindOf( SOT_Vertex ))
      //   {
      //      Vertex *pFocusVertex = (Vertex *)pFocusObject;
      //      RGBAFloat color;
      //      pFocusVertex->GetDrawColor( color );
      //      for (UINT i = 0; i < numVerts; i++)
      //      {
      //         Vertex *pVertex = GetVertexAt(i);
      //         glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), pVertex==pFocusVertex? 0.8f: 0.0f);
      //         glVertex3fv( &pVertex->GetPos().x );
      //      }
      //   }
      //   else if (pFocusObject->IsKindOf( SOT_Edge ))
      //   {

      //   }
      //}
   glEnd();
}

void Poly::Draw( CGLBaseView *pView )
{
   UINT numVerts = GetNumSides();
   glBegin( numVerts==4 ? GL_QUADS : GL_POLYGON );
      for (UINT i = 0; i < numVerts; i++)
         glVertex3fv( &GetVertexAt(i)->GetRTVertexHead()->GetPos().x );
   glEnd();
}

void Poly::DrawSelected( CGLBaseView *pView )
{
   glDisable( GL_DEPTH_TEST );
      glColor4f( g_selectedcolor.GetRed(), g_selectedcolor.GetGreen(), g_selectedcolor.GetBlue(), g_selectedcolor.GetAlpha()*0.1f );
      Draw( pView );
   glEnable( GL_DEPTH_TEST );

   glColor4f( g_selectedcolor.GetRed(), g_selectedcolor.GetGreen(), g_selectedcolor.GetBlue(), g_selectedcolor.GetAlpha()*0.35f );
   Draw( pView );
}

void Poly::DrawMirrored( CGLBaseView *pView )
{
   glDisable( GL_DEPTH_TEST );
      glColor4f( g_mirrorcolor.GetRed(), g_mirrorcolor.GetGreen(), g_mirrorcolor.GetBlue(), g_mirrorcolor.GetAlpha()*0.1f );
      Draw( pView );
   glEnable( GL_DEPTH_TEST );

   glColor4f( g_mirrorcolor.GetRed(), g_mirrorcolor.GetGreen(), g_mirrorcolor.GetBlue(), g_mirrorcolor.GetAlpha()*0.35f );
   Draw( pView );
}

void Poly::DrawFocused( CGLBaseView *pView )
{
   glDisable( GL_DEPTH_TEST );
      glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), g_focuscolor.GetAlpha()*0.15f );
      Draw( pView );
   glEnable( GL_DEPTH_TEST );

   glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), g_focuscolor.GetAlpha()*0.35f );
   Draw( pView );
}

void Poly::DrawNearestFocused( CGLBaseView *pView, SnipeObject *pFocusObject )
{
   if (pFocusObject->IsKindOf( SOT_Vertex ))
   {
      Vertex *pVertex = (Vertex *)pFocusObject;

      UINT numVerts = GetNumSides();
      UINT nIndex = FindVertexFaceRelativeIndex( pVertex );
      glDisable( GL_DEPTH_TEST );
         glBegin( numVerts==4 ? GL_QUADS : GL_POLYGON );
            for (UINT i = 0; i < numVerts; i++)
            {
               float fAlpha = (i == nIndex) ? 0.2f : 0.0f;
               glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), g_focuscolor.GetAlpha() * fAlpha );
               glVertex3fv( &GetVertexAt(i)->GetRTVertexHead()->GetPos().x );
            }
         glEnd();
      glEnable( GL_DEPTH_TEST );
   }
}

// static
Poly *Poly::FindFace(UINT nVertexCount, Vertex **pVertexArray)
{
   UINT nMatchPolyCount = pVertexArray[0]->GetNumSurroundingPolys();

   Poly *pMatchPoly = NULL;
   for (UINT i=0; i<nMatchPolyCount; i++)
   {
      BOOL bFound=FALSE;
      pMatchPoly = pVertexArray[0]->GetSurroundingPolyAt(i);
      for (UINT j=1; j<nVertexCount; j++)
      {
         bFound = FALSE;
         UINT nPolyCount = pVertexArray[j]->GetNumSurroundingPolys();
         for (UINT k=0; k<nPolyCount; k++)
         {
            if (pVertexArray[j]->GetSurroundingPolyAt(k) == pMatchPoly)
            {
               bFound = TRUE;
               break;
            }
         }
         if (!bFound)
            break;
      }
      if (bFound)
         return pMatchPoly;
   }
   return NULL;
}

SnipeObject *Poly::FindMirrorObject()
{
   PolyModel *pModel = (PolyModel *)GetOwner(); 
   if (!pModel)
      return NULL;

   Vertex *pVertexArray3[3];
   Vertex *pVertexArray4[4];
   CArray<Vertex *, Vertex *> vertexarrayN;

   Vertex **pVertexArray;

   UINT nVertexCount = GetNumSides();
   switch( nVertexCount )
   {
   case 0:
   case 1:
   case 2:
      return NULL;
   case 3:
      pVertexArray = pVertexArray3;
      break;
   case 4:
      pVertexArray = pVertexArray4;
      break;
   default:
      vertexarrayN.SetSize( nVertexCount );
      pVertexArray = vertexarrayN.GetData();
      break;
   }

   for (UINT i=0; i<nVertexCount; i++)
   {
      pVertexArray[i] = (Vertex *)GetVertexAt(i)->GetMirrorObject();
      if (pVertexArray[i]==NULL)
         return NULL;
   }

   return Poly::FindFace( nVertexCount, pVertexArray );
}

void Poly::BuildFaceNormal()
{
   Vector normal(0,0,0);
   int numVerts = (int)GetNumSides();
   Vector A,B;
   int i;
   for (i = 0; i < numVerts; i++)
   {
      int prev = i - 1;
      if (prev<0)
         prev = numVerts-1;
      int next = i + 1;
      if (next>numVerts-1)
         next = 0;
      A = m_pVertices[i]->GetPos() - m_pVertices[prev]->GetPos();
      B = m_pVertices[next]->GetPos() - m_pVertices[i]->GetPos();
      A.Normalize();
      B.Normalize();
      normal += A ^ B;
   }
   m_faceNormal = normal.Normalize();

   SetDirtyFaceNormal( FALSE );
}

void Poly::SetDirtyFaceNormal( BOOL bDirtyFaceNormal )
{
   if (m_bDirtyFaceNormal == bDirtyFaceNormal)
      return;

   m_bDirtyFaceNormal = bDirtyFaceNormal;

   if (bDirtyFaceNormal) // dirty the vertex normals too
   {
      for (UINT i=0; i< GetNumSides(); i++)
      {
         VertexNormal *pVertexNormal = GetVertexNormalAt(i, FALSE);
         if (pVertexNormal)
            pVertexNormal->SetVertexNormalDirtyFlag( TRUE );
      }
   }
}

void Poly::DrawPickNearest( Vertex *pVertex )
{
   Edge *pEdge0, *pEdge1;

   pVertex->GetEdgePair( this, &pEdge0, &pEdge1 );
   glBegin( GL_QUADS );
      Vector vCenter(0.0f);
      for (UINT i=0; i<GetNumSides();i++)
         vCenter += GetVertexAt(i)->GetPos();
      vCenter /= GetNumSides();

      Vector vVertPos = pVertex->GetPos();
      glVertex3fv(&vVertPos.x);

      Vector vEdgePos = vVertPos + pEdge0->GetConnectedVertex(pVertex)->GetPos();
      vEdgePos /= 2.0f;
      glVertex3fv(&vEdgePos.x);

      glVertex3fv(&vCenter.x);

      vEdgePos = vVertPos + pEdge1->GetConnectedVertex(pVertex)->GetPos();
      vEdgePos /= 2.0f;
      glVertex3fv(&vEdgePos.x);
   glEnd();
}

void Poly::DrawPickNearest( Edge *pEdge )
{
   BOOL bOldCullFace = glSet( GL_CULL_FACE, FALSE);

   glBegin( GL_TRIANGLES );
      Vector vCenter(0.0f);
      for (UINT i=0; i<GetNumSides();i++)
         vCenter += GetVertexAt(i)->GetPos();
      vCenter /= GetNumSides();

      Vector vVertPos = pEdge->GetFirstVertex()->GetPos();
      glVertex3fv(&vVertPos.x);
      glVertex3fv(&vCenter.x);

      vVertPos = pEdge->GetSecondVertex()->GetPos();
      glVertex3fv(&vVertPos.x);
   glEnd();

   glSet( GL_CULL_FACE, bOldCullFace );
}

BOOL Poly::IsUsed()
{
   return FALSE;
}

void Poly::Disconnect()
{
   for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
      pPropertyPoly->RemoveFromList();      

   // TODO COMBACK possibly put all in one loop, all except Edges, they must be done after all vertices have been done because when an edge gets detached
   // it can possibly detach the vertex which could set the vertex data to null which would be bad for GetVertexAt(i)->RemoveSurroundingPoly( this ); because
   // the surrounding list is stored in the Vertex Data 
   for (int i=GetNumSides()-1; i>=0; i--)
   {
      Vertex *pVertex = GetVertexAt(i);
      if (pVertex)
         pVertex->RemoveSurroundingPoly( this );
   }

   for (int i=GetNumSides()-1; i>=0; i--)
   {
      VertexNormal *pVertexNormal = GetVertexNormalAt(i, FALSE);
      if (pVertexNormal)
         pVertexNormal->RemovePoly( this );
   }

   for (int i=GetNumSides()-1; i>=0; i--)
   {
      Edge *pEdge = GetEdgeAt(i);
      if (pEdge)
         pEdge->RemovePoly( this, m_bRemoveEdgesOnDisconnectIfNotUsed );
   }

   for (int i=GetNumSurfaces()-1; i>=0; i--)
      GetSurfaceAt(i)->RemoveFace( this, FALSE );

   SetDirty();
   SetCompositedSurface( NULL );
   GetModel()->SetCompositedSurfacesDirty( TRUE );

   Component::Disconnect();
}

void Poly::Reconnect()
{
   Component::Reconnect();

   for (UINT i=0; i<GetNumSurfaces(); i++)
      GetSurfaceAt(i)->AddFace( this );

   for (UINT i=0; i<GetNumSides(); i++)
      GetEdgeAt(i)->AddPoly( this, i );

   for (UINT i=0; i<GetNumSides(); i++)
      GetVertexNormalAt(i, FALSE)->AddPoly( this );

   for (UINT i=0; i<GetNumSides(); i++)
      GetVertexAt(i)->AddSurroundingPoly( this );

   for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
      pPropertyPoly->ReInsertIntoList();      

   SetDirty();
   GetModel()->SetCompositedSurfacesDirty( TRUE );
}

Edge *Poly::GetSharedEdge( Poly *pPoly )
{
   for (UINT i = 0; i < GetNumSides(); i++)
   {
      Edge *pMatchEdge = GetEdgeAt(i);
      for (UINT j = 0; j < pPoly->GetNumSides(); j++)
      {
         Edge *pEdge = pPoly->GetEdgeAt(j);
         if (pEdge == pMatchEdge)
            return pMatchEdge;
      }
   }
   return NULL;
}

Edge::DeleteInfo *Poly::SplitPoly( Vertex *pVertex0, Vertex *pVertex1, Edge *pPrevEdge/*=EDGE_FINDCONTINUOUS*/, Edge *pNextEdge/*=EDGE_FINDCONTINUOUS*/ )
{
   int nIndex0 = FindVertexFaceRelativeIndex(pVertex0);
   int nIndex1 = FindVertexFaceRelativeIndex(pVertex1);
   ASSERT(nIndex0>=0);
   ASSERT(nIndex1>=0);

   UINT nNumPropertyPolys = CountNumPropertyPolys();

   CArray<Vertex*> vertices0;
   CArray<Vertex*> vertices1;

   CArray<VertexNormal*> vertexnormals0;
   CArray<VertexNormal*> vertexnormals1;

   CArray< CArray<PropertyVertex*> > PropertyVertices0;
   CArray< CArray<PropertyVertex*> > PropertyVertices1;

   CArray< CArray<PropertyEdge *> > PropertyEdges0;
   CArray< CArray<PropertyEdge *> > PropertyEdges1;

   PropertyVertices0.SetSize(nNumPropertyPolys);
   PropertyVertices1.SetSize(nNumPropertyPolys);
   PropertyEdges0.SetSize(nNumPropertyPolys);
   PropertyEdges1.SetSize(nNumPropertyPolys);

   CArray<Edge*> edges0;
   CArray<Edge*> edges1;

   Vertex *pVertex;
   VertexNormal *pVertexNormal;
   Edge *pEdge;
   int nPrevIndex = -1;
   int i = nIndex1;

   while(nPrevIndex != nIndex0)
   {
      pVertex = GetVertexAt(i);
      pVertexNormal = GetVertexNormalAt(i);
      pEdge = GetEdgeAt(i);
      vertices0.Add(pVertex);
      vertexnormals0.Add(pVertexNormal);
      if (i != nIndex0)
         edges0.Add(pEdge);

      UINT nCount=0;
      for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
      {
         PropertyVertices0[nCount].Add( pPropertyPoly->GetPropertyVertexAt(i) );
         if (i != nIndex0)
            PropertyEdges0[nCount].Add( pPropertyPoly->GetPropertyEdgeAt(i) );
      }
   
      nPrevIndex = i;
      i++;
      if ( i>= (int)GetNumSides())
         i = 0;
   }

   PolyModel *pModel = (PolyModel *)GetOwner();

   Edge *pReversedEdge = NULL;
   Edge *pNewEdge = pModel->AddEdgeBetween( pVertex0, pVertex1, &pReversedEdge, pPrevEdge, pNextEdge );
   ASSERT( pNewEdge );

   CArray< PropertyEdge *> newpropertyedges;
   newpropertyedges.SetSize(nNumPropertyPolys);

   UINT nUVIndex0 = FindVertexFaceRelativeIndex( pNewEdge->GetFirstVertex() );
   UINT nUVIndex1 = FindVertexFaceRelativeIndex( pNewEdge->GetSecondVertex() );

   UINT nCount=0;
   for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
   {
      PropertySet *pPropertySet = pPropertyPoly->GetPropertySet();

      PropertyVertex *pPropertyVertex0 = pPropertyPoly->GetPropertyVertexAt( nUVIndex0 );
      PropertyVertex *pPropertyVertex1 = pPropertyPoly->GetPropertyVertexAt( nUVIndex1 );

      PropertyEdge *pNewPropertyEdge = pPropertySet->AddPropertyEdge( pPropertyPoly, pPropertyVertex0, pPropertyVertex1, pNewEdge );
      newpropertyedges[nCount] = pNewPropertyEdge;
      PropertyEdges0[nCount].Add( pNewPropertyEdge );
   }

   Edge::DeleteInfo *pEdgeDeleteInfo = SNEW Edge::DeleteInfo( pNewEdge );

   pEdgeDeleteInfo->SetReversedEdge(pReversedEdge);
   pEdgeDeleteInfo->SetNewPoly( this );

   Poly *pMirrorPoly = (Poly *)GetMirrorObject();
   RemoveFromList();

   Poly *pPoly0 = pModel->AddPoly();
   pEdgeDeleteInfo->SetOldPoly0( pPoly0 );

   Poly *pPoly1 = pModel->AddPoly();
   pEdgeDeleteInfo->SetOldPoly1( pPoly1 );

   edges0.Add(pNewEdge);

   nPrevIndex = -1;
   i = nIndex0;
   while(nPrevIndex != nIndex1)
   {
      pVertex = GetVertexAt(i);
      pVertexNormal = GetVertexNormalAt(i);
      pEdge = GetEdgeAt(i);
      vertices1.Add(pVertex);
      vertexnormals1.Add(pVertexNormal);
      if (i != nIndex1)
         edges1.Add(pEdge);

      UINT nCount=0;
      for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
      {
         PropertyVertex *pPropertyVertex = pPropertyPoly->GetPropertyVertexAt(i);
         PropertyVertices1[nCount].Add(pPropertyVertex);

         if (i != nIndex1)
            PropertyEdges1[nCount].Add( pPropertyPoly->GetPropertyEdgeAt(i) );
      }
      nPrevIndex = i;
      i = GetNextIndex(i);
   }

   edges1.Add(pNewEdge);
   
   nCount=0;
   for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
   {
      PropertyEdges1[nCount].Add( newpropertyedges[nCount] );
      PropertySet *pPropertySet = pPropertyPoly->GetPropertySet();
      PropertyPoly *pPropertyPoly0 = pPropertySet->AddPoly( pPoly0 );
      PropertyPoly *pPropertyPoly1 = pPropertySet->AddPoly( pPoly1 );
   }

   pPoly0->BuildPoly( vertices0, vertexnormals0, edges0, PropertyVertices0, PropertyEdges0 );
   pPoly1->BuildPoly( vertices1, vertexnormals1, edges1, PropertyVertices1, PropertyEdges1 );

   // create texture info on new polys
   for (UINT nSurfaceIndex=0; nSurfaceIndex<GetNumSurfaces(); nSurfaceIndex++)
   {
      Surface *pSurface = GetSurfaceAt( nSurfaceIndex );
      pSurface->AddFace( pPoly0 );
      pSurface->AddFace( pPoly1 );
   }

   if (pMirrorPoly)
   {
      Vertex *pMirrorVertex0 = (Vertex *)pVertex0->GetMirrorObject();
      if (!pMirrorVertex0)
         goto endmirror;

      Vertex *pMirrorVertex1 = (Vertex *)pVertex1->GetMirrorObject();
      if (!pMirrorVertex1)
         goto endmirror;

      Edge *pMirrorPrev;
      if (pPrevEdge!=(Edge *)EDGE_FINDCONTINUOUS && pPrevEdge != (Edge *)EDGE_TERMINATE)
      {
         pMirrorPrev = (Edge *)pPrevEdge->GetMirrorObject();
         if (!pMirrorPrev)
            goto endmirror;
      }
      else
         pMirrorPrev = pPrevEdge;

      Edge *pMirrorNext;
      if (pNextEdge!=(Edge *)EDGE_FINDCONTINUOUS && pNextEdge != (Edge *)EDGE_TERMINATE)
      {
         pMirrorNext = (Edge *)pNextEdge->GetMirrorObject();
         if (!pMirrorNext)
            goto endmirror;
      }
      else
         pMirrorNext = pNextEdge;

      pEdgeDeleteInfo->m_pMirrorDeleteInfo = pMirrorPoly->SplitPoly( pMirrorVertex0, pMirrorVertex1, pMirrorPrev, pMirrorNext );
   }

endmirror:


   return pEdgeDeleteInfo;
}

void Poly::BuildPoly( BuildPolyInfo *pInfo )
{
   BuildPoly( pInfo->m_newvertices, pInfo->m_newvertexnormals, pInfo->m_newedges, pInfo->m_newPropertyVertices, pInfo->m_newpropertyedges );
}

void Poly::BuildPoly( CArray<Vertex *> &newvertices, CArray<VertexNormal *> &newvertexnormals, CArray<Edge *> &newedges,
 CArray< CArray<PropertyVertex *> > &newPropertyVertices, CArray< CArray<PropertyEdge *> > &newpropertyedges )
{
   CArray< Vertex * > oldvertices;
   CArray< VertexNormal * > oldvertexnormals;
   CArray< Edge * > oldedges;
   CArray< CArray< PropertyVertex * > > olduvs;
   CArray< CArray< PropertyEdge * > > oldPropertyEdges;

   UINT nNumOldSides = GetNumSides();
   oldvertices.SetSize( nNumOldSides );
   oldvertexnormals.SetSize( nNumOldSides );
   oldedges.SetSize( nNumOldSides );

   UINT nNumPropertyPolys = CountNumPropertyPolys();
   olduvs.SetSize( nNumPropertyPolys );
   oldPropertyEdges.SetSize( nNumPropertyPolys );
   for (UINT nPropertyPolyIndex=0; nPropertyPolyIndex<nNumPropertyPolys; nPropertyPolyIndex++)
   {
      olduvs[nPropertyPolyIndex].SetSize( nNumOldSides );
      oldPropertyEdges[nPropertyPolyIndex].SetSize( nNumOldSides );
   }

   for ( UINT i = 0; i < nNumOldSides; i++)
   {
      oldvertices[i] = GetVertexAt(i);
      if (oldvertices[i])
         oldvertices[i]->Reference();
      
      oldvertexnormals[i] = GetVertexNormalAt(i, FALSE);
      if (oldvertexnormals[i])
         oldvertexnormals[i]->Reference();
      
      oldedges[i] = GetEdgeAt(i);
      if (oldedges[i])
         oldedges[i]->Reference();
   
      UINT nCount=0;
      for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
      {
         olduvs[nCount][i] = pPropertyPoly->GetPropertyVertexAt(i); 
         if (olduvs[nCount][i])
            olduvs[nCount][i]->Reference(); 

         oldPropertyEdges[nCount][i] = pPropertyPoly->GetPropertyEdgeAt(i);

         if (oldPropertyEdges[nCount][i])
            oldPropertyEdges[nCount][i]->Reference();
      }
   }

   // copy new poly data into poly, edges need to be last
   SetNumSides(newvertices.GetSize());

   for (UINT nPass=0; nPass<4; nPass++ )
   {
      for (i = 0; i < (UINT)newvertices.GetSize(); i++)
      {
         switch (nPass)
         {
            case 0:
            {
               UINT nCount=0;
               for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
               {
                  pPropertyPoly->SetPropertyVertexAt( i, newPropertyVertices[nCount][i] ); 
                  pPropertyPoly->SetPropertyEdgeAt( i, newpropertyedges[nCount][i] );
               }
               break;
            }
            case 1:
               SetEdgeAt( i, newedges[i] );
               break;
            case 2:
               SetVertexNormalAt( i, newvertexnormals[i] );
               break;
            case 3:
               SetVertexAt( i, newvertices[i] );
               break;
         }
      }
   }

   for ( UINT i = 0; i < nNumOldSides; i++)
   {
      if (oldvertices[i])
         oldvertices[i]->DeReference();
      if (oldvertexnormals[i])
         oldvertexnormals[i]->DeReference();
      if (oldedges[i])
         oldedges[i]->DeReference();

      UINT nCount=0;
      for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
      {
         if (olduvs[nCount][i])
            olduvs[nCount][i]->DeReference(); 

         if (oldPropertyEdges[nCount][i])
            oldPropertyEdges[nCount][i]->DeReference();
      }
   }
}

//pnewpropertyedges is an array of [2] face specific
BuildPolyInfo *Poly::RemoveVertex(Edge *pNewEdge, CArray< PropertyEdge *> *pnewpropertyedges, Vertex *pAtVertex)
{
   BuildPolyInfo *pBuildPolyInfo = SNEW BuildPolyInfo;

   UINT i;
   UINT nNumPropertyPolys = CountNumPropertyPolys();

   UINT nNumVerts = GetNumSides();
   UINT nNewSize = nNumVerts-1;

   // build new poly data
   pBuildPolyInfo->m_newvertices.SetSize(nNewSize);
   pBuildPolyInfo->m_newvertexnormals.SetSize(nNewSize);
   pBuildPolyInfo->m_newedges.SetSize(nNewSize);
   pBuildPolyInfo->m_newPropertyVertices.SetSize(nNumPropertyPolys);
   pBuildPolyInfo->m_newpropertyedges.SetSize(nNumPropertyPolys);

   ASSERT(pnewpropertyedges->GetSize()==nNumPropertyPolys);

   UINT nPropertyPolyIndex;
   for (nPropertyPolyIndex = 0; nPropertyPolyIndex < nNumPropertyPolys; nPropertyPolyIndex++)
   {
      pBuildPolyInfo->m_newPropertyVertices[nPropertyPolyIndex].SetSize(nNewSize);
      pBuildPolyInfo->m_newpropertyedges[nPropertyPolyIndex].SetSize(nNewSize);
   }

   i = 0;
   for (UINT j = 0; j < nNumVerts; j++)
   {
      Vertex *pVertex = GetVertexAt(j);
      if (pVertex==pAtVertex)
         continue;
      pBuildPolyInfo->m_newvertices[i] = pVertex;

      VertexNormal *pVertexNormal = GetVertexNormalAt(j, FALSE);
      pBuildPolyInfo->m_newvertexnormals[i] = pVertexNormal;

      UINT nCount=0;
      for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
      {
         PropertyVertex *pPropertyVertex = pPropertyPoly->GetPropertyVertexAt(j);
         pBuildPolyInfo->m_newPropertyVertices[nCount][i] = pPropertyVertex;
      }

      Edge *pEdge = GetEdgeAt(j);

      UINT nNext = GetNextIndex(j);
      Vertex *pNextVertex = GetVertexAt(nNext);

      if (pNextVertex==pAtVertex)
      {
         pBuildPolyInfo->m_newedges[i] = pNewEdge;
         for (UINT nPropertyPolyIndex=0; nPropertyPolyIndex < nNumPropertyPolys; nPropertyPolyIndex++)
            pBuildPolyInfo->m_newpropertyedges[nPropertyPolyIndex][i] = pnewpropertyedges->GetAt(nPropertyPolyIndex);
      }
      else
      {
         pBuildPolyInfo->m_newedges[i] = pEdge;
         UINT nCount=0;
         for ( PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
            pBuildPolyInfo->m_newpropertyedges[nCount][i] = pPropertyPoly->GetPropertyEdgeAt(j);
      }

      i++;
   }
   
   return pBuildPolyInfo;
}

BuildPolyInfo *Poly::ReinsertVertex( Vertex *pOldVertex, Edge *pOldEdge0, CArray<PropertyEdge *> *pOldPropertyEdges0, Edge *pOldEdge1, CArray<PropertyEdge *> *pOldPropertyEdges1, UINT nOldIndex, VertexNormal *pOldVertexNormal, CArray<PropertyVertex *> *pOldUVs )
{
   UINT nNumVerts = GetNumSides();
   int i;

   UINT nNumPropertyPolys = CountNumPropertyPolys();

   BuildPolyInfo *pBuildInfo = SNEW BuildPolyInfo;

   UINT nNewSize = nNumVerts+1;

   pBuildInfo->m_newvertices.SetSize(nNewSize);
   pBuildInfo->m_newvertexnormals.SetSize(nNewSize);
   pBuildInfo->m_newedges.SetSize(nNewSize);
   pBuildInfo->m_newPropertyVertices.SetSize(nNumPropertyPolys);
   pBuildInfo->m_newpropertyedges.SetSize(nNumPropertyPolys);

   ASSERT( pOldPropertyEdges0->GetSize() == nNumPropertyPolys && pOldPropertyEdges1->GetSize() == nNumPropertyPolys );

   UINT nPropertyPolyIndex;
   for (nPropertyPolyIndex = 0; nPropertyPolyIndex < nNumPropertyPolys; nPropertyPolyIndex++)
   {
      pBuildInfo->m_newPropertyVertices[nPropertyPolyIndex].SetSize(nNewSize);
      pBuildInfo->m_newpropertyedges[nPropertyPolyIndex].SetSize(nNewSize);
   }

   Vertex *pPrevVertex = GetVertexAt( GetPrevIndex(nOldIndex) );
   BOOL bForward = pOldEdge0->IsInEdge(pPrevVertex);
   UINT nPrevOldIndex = nOldIndex==0 ? nNewSize-1 : nOldIndex-1;

   i = 0;
   for (UINT j = 0; j < nNewSize; j++)
   {
      if (j==nOldIndex)
      {
         // insert old vertex, vertexnormal, uv, edge
         pBuildInfo->m_newvertices[j] = pOldVertex;
         pBuildInfo->m_newvertexnormals[j] = pOldVertexNormal;

         pBuildInfo->m_newedges[nPrevOldIndex] = bForward ? pOldEdge0 : pOldEdge1;
         pBuildInfo->m_newedges[j] = bForward ? pOldEdge1 : pOldEdge0;

         UINT nPropertyPolyIndex=0;
         for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nPropertyPolyIndex++)
         {
            PropertyVertex *pPrevUV = pPropertyPoly->GetPropertyVertexAt( GetPrevIndex(nOldIndex) );
            BOOL bUVForward = pOldPropertyEdges0->GetAt(nPropertyPolyIndex)->IsInEdge( pPrevUV );
            pBuildInfo->m_newpropertyedges[nPropertyPolyIndex][nPrevOldIndex] = bUVForward ? pOldPropertyEdges0->GetAt(nPropertyPolyIndex) : pOldPropertyEdges1->GetAt(nPropertyPolyIndex);
            pBuildInfo->m_newpropertyedges[nPropertyPolyIndex][j] = bUVForward ? pOldPropertyEdges1->GetAt(nPropertyPolyIndex) : pOldPropertyEdges0->GetAt(nPropertyPolyIndex);
            pBuildInfo->m_newPropertyVertices[nPropertyPolyIndex][j] = pOldUVs->GetAt(nPropertyPolyIndex);
         }
      }
      else
      {      
         // Copy Existing Vertex, Normal, Edge...
         pBuildInfo->m_newvertices[j] = GetVertexAt(i);
         pBuildInfo->m_newvertexnormals[j] =  GetVertexNormalAt(i, FALSE);

         if (j!=nPrevOldIndex)
            pBuildInfo->m_newedges[j] = GetEdgeAt(i);

         UINT nCount=0;
         for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
         {
            pBuildInfo->m_newPropertyVertices[nCount][j] = pPropertyPoly->GetPropertyVertexAt(i);
            if (j!=nPrevOldIndex)
               pBuildInfo->m_newpropertyedges[nCount][j] = pPropertyPoly->GetPropertyEdgeAt(i); 
         }
         i++;
      }
   }

   return pBuildInfo;
}

void Poly::InsertVertex( Vertex *pNewVertex, VertexNormal *pNewVertexNormal, CArray<PropertyVertex *> *pnewPropertyVertices, Vertex *pAtVertex, float fPosition, 
 Edge *&pOutNewEdge0, CArray<PropertyEdge *> &pOutnewpropertyedges0, Edge *&pOutNewEdge1, CArray<PropertyEdge *> &pOutnewpropertyedges1 )
{
   ASSERT(pOutnewpropertyedges0.IsEmpty() && pOutnewpropertyedges1.IsEmpty());

   UINT nNumVerts = GetNumSides();
   UINT i;

   UINT nNumPropertyPolys = CountNumPropertyPolys();

   CArray<Vertex*> newvertices;
   CArray<VertexNormal*> newvertexnormals;
   CArray<Edge*> newedges;
   CArray< CArray<PropertyVertex*> > newPropertyVertices;
   CArray< CArray<PropertyEdge *> > newpropertyedges;

   UINT nNewSize = nNumVerts+1;

   // build new poly data
   newvertices.SetSize(nNewSize);
   newvertexnormals.SetSize(nNewSize);
   newedges.SetSize(nNewSize);
   newPropertyVertices.SetSize(nNumPropertyPolys);
   newpropertyedges.SetSize(nNumPropertyPolys);

   PolyModel *pPolyModel = (PolyModel *)GetOwner();

   UINT nPropertyPolyIndex;
   for (nPropertyPolyIndex = 0; nPropertyPolyIndex < nNumPropertyPolys; nPropertyPolyIndex++)
   {
      newPropertyVertices[nPropertyPolyIndex].SetSize(nNewSize);
      newpropertyedges[nPropertyPolyIndex].SetSize(nNewSize);
   }

   UINT nNewEdgeIndex, nNewEdgeNextIndex;

   i = 0;
   for (UINT j = 0; j < nNumVerts; j++, i++)
   {
      Vertex *pVertex = GetVertexAt(j); 
      Edge *pEdge = GetEdgeAt(j);
      VertexNormal *pVertexNormal = GetVertexNormalAt(j, FALSE);

      // Copy Existing Vertex, Normal, Edge...
      newvertices[i] = pVertex;
      newvertexnormals[i] = pVertexNormal;
    
      UINT nCount=0;
      for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
      {
         PropertyVertex *pPropertyVertex = pPropertyPoly->GetPropertyVertexAt(j);
         newPropertyVertices[nCount][i] = pPropertyVertex;
      }

      // Insert After
      if (pVertex == pAtVertex )
      {
         // insert new vertex, vertexnormal, uv, edge
         if (!(pEdge->GetFirstVertex()==pVertex))
            fPosition = 1.0 - fPosition;

         UINT nNext = GetNextIndex(j);

         if (pEdge->IsInList())
         {
            Vector pos = pEdge->GetPosAlongEdge(fPosition,this);
            pNewVertex->SetPos(pos);
            pEdge->RemoveFromList(); // Must be after GetPosALongEdge and before AddEdge belows
         }

         if (pOutNewEdge0==NULL)
         {
            ASSERT( pOutNewEdge1 == NULL );
            // pNewEdge0 and pNewEdge1 are always aligned with pEdge
            pOutNewEdge0 = pPolyModel->AddEdge( pEdge->GetFirstVertex(), pNewVertex );
            pOutNewEdge1 = pPolyModel->AddEdge( pNewVertex, pEdge->GetSecondVertex());
         }

         if (nNumPropertyPolys)
         {
            pOutnewpropertyedges0.SetSize( nNumPropertyPolys );
            pOutnewpropertyedges1.SetSize( nNumPropertyPolys );
         }

         UINT nPropertyPolyIndex=0;
         for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nPropertyPolyIndex++)
         {
            PropertyEdge *pPropertyEdge = pPropertyPoly->GetPropertyEdgeAt(j);

            PropertyVertex *pNewPropertyVertex = pnewPropertyVertices->GetAt(nPropertyPolyIndex);
            if (pPropertyEdge->IsInList())
            {
               PropertyVertex *pNextUV = pPropertyPoly->GetPropertyVertexAt(nNext);
 //              Vector2 pos = pPropertyEdge->GetPosAlongEdge(fPosition, pPropertyPoly);
 //              pNewPropertyVertex->SetPos(pos);

               pNewPropertyVertex->SetPosFromPointAlongEdge(fPosition, pPropertyEdge, pPropertyPoly);

               pPropertyEdge->RemoveFromList(); // Test
            }

            PropertySet *pPropertySet = pPropertyPoly->GetPropertySet();

            // This will return a shared edge if it exists
            // pNewPropertyEdge0 and pNewPropertyEdge1 are always aligned with pPropertyEdge
            pOutnewpropertyedges0[nPropertyPolyIndex] = pPropertySet->AddPropertyEdge( pPropertyPoly, pPropertyEdge->GetFirstPropertyVertex(), pNewPropertyVertex, pOutNewEdge0 );
            pOutnewpropertyedges1[nPropertyPolyIndex] = pPropertySet->AddPropertyEdge( pPropertyPoly, pNewPropertyVertex, pPropertyEdge->GetSecondPropertyVertex(), pOutNewEdge1 );
         }

         PolyModel *pPolyModel = (PolyModel *)GetOwner();

         BOOL bAligned = pEdge->IsAligned( this );
         newedges[i] = bAligned ? pOutNewEdge0 : pOutNewEdge1;
         nNewEdgeIndex = i;

         i++;
         nNewEdgeNextIndex = i;
         newvertices[i] = pNewVertex;
         newvertexnormals[i] = pNewVertexNormal;
         newedges[i] = bAligned ? pOutNewEdge1 : pOutNewEdge0;

         nCount=0;
         for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
         {
            PropertyVertex *pNewPropertyVertex = pnewPropertyVertices->GetAt(nCount);
            newPropertyVertices[nCount][i] = pNewPropertyVertex;

            newpropertyedges[nCount][i-1] = bAligned ? pOutnewpropertyedges0[nCount] : pOutnewpropertyedges1[nCount];
            newpropertyedges[nCount][i] = bAligned ? pOutnewpropertyedges1[nCount] : pOutnewpropertyedges0[nCount];
         }
         continue;
      }

      newedges[i] = pEdge;
      UINT nPropertyPolyIndex=0;
      for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nPropertyPolyIndex++)
         newpropertyedges[nPropertyPolyIndex][i] = pPropertyPoly->GetPropertyEdgeAt(j);
   }

   BuildPoly( newvertices, newvertexnormals, newedges, newPropertyVertices, newpropertyedges );
}

void Poly::PushBounds( BoundingBox &box, CGLBaseView *pView/*=NULL*/ )
{
   Vector vPos;

   for (UINT i=0; i<GetNumSides(); i++)
   {
      vPos = GetVertexAt(i)->GetPos();
      if (pView)
         pView->WorldToView( vPos );
      box.Push( vPos );
   }
}

/*
Example 1
*************************
*                       *
*        Poly 1         *
*                       *
*     ************      *
*     *  Poly 2  *      *
*     *          *      *
*************************
*/
//static 
Poly *Poly::FindPoly( Vertex *pV0, Vertex *pV1 )
{
   Poly *pBestPoly = NULL;

   for ( UINT i=0; i<pV0->GetNumSurroundingPolys(); i++ )
   {
      Poly *pMatchPoly = pV0->GetSurroundingPolyAt(i);
      for ( UINT j=0; j<pV1->GetNumSurroundingPolys(); j++ )
      {
         Poly *pPoly = pV1->GetSurroundingPolyAt(j);
         if (pPoly == pMatchPoly && pPoly != pBestPoly)
         {
            if (pBestPoly && pPoly->GetNumSides() > pBestPoly->GetNumSides()) // Case where one poly surrounds another poly, see example 1 above.  I know this case can still fail, but I think it would be rare
               continue;

            pBestPoly = pPoly;
         }  
      }
   }

   return pBestPoly;
}

void Poly::FindAndCreateNormals()
{
   PolyModel *pModel = GetModel();

   for (UINT i=0; i<GetNumSides(); i++)
   {
      if (GetVertexNormalAt(i))
         continue;

      Vertex *pVertex = GetVertexAt(i);

      Poly *pPoly = NULL;
      for (UINT j=0; j<pVertex->GetNumSurroundingEdges(); j++)
      {
         Edge *pEdge = pVertex->GetSurroundingEdgeAt(j);
         if (pEdge->GetFirstPoly()==this )
            pPoly = pEdge->GetSecondPoly();
         else if (pEdge->GetSecondPoly()==this)
            pPoly = pEdge->GetFirstPoly();

         if (pPoly)
            break;
      }

      if (pPoly)
      {
         UINT nIndex = pPoly->FindVertexFaceRelativeIndex(pVertex);
         ASSERT( nIndex!=-1 );
         VertexNormal *pVertexNormal = pPoly->GetVertexNormalAt(nIndex);
         SetVertexNormalAt( i, pVertexNormal );
      }
      else
      {
         VertexNormal *pVertexNormal = pModel->AddVertexNormal();
         SetVertexNormalAt( i, pVertexNormal );
      }
   }
}

void Poly::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   Component::Save( bs );

   UINT nNumVertices = GetNumSides();
   bs << sp << nNumVertices;

   for (UINT i=0; i<nNumVertices; i++)
      bs << sp << GetVertexAt(i)->GetID();

   //for (UINT i=0; i<nNumVertices; i++)
   //   bs << sp << GetVertexNormalAt(i)->GetID();

   if (bWriteHeader)
      bs << nl;
}

BOOL Poly::Load( BaseStream &bs )
{
   if (!Component::Load( bs ))
      return FALSE;

   UINT nNumVertices;
   bs >> nNumVertices;

   PolyModel *pPolyModel = (PolyModel *)GetOwner();

   // VERTICES
   SetNumSides( nNumVertices );
   for (UINT nIndex=0; nIndex<nNumVertices; nIndex++)
   {
      UINT nVertexID;
      bs >> nVertexID;
      Vertex *pVertex = pPolyModel->FindVertex( nVertexID );
      if (!pVertex)
         return FALSE;
      SetVertexAt( nIndex, pVertex );
   }

   // NORMALS
   //for (UINT nIndex=0; nIndex<nNumVertices; nIndex++)
   //{
   //   UINT nVertexNormalID;
   //   bs >> nVertexNormalID;
   //   VertexNormal *pVertexNormal = pPolyModel->FindVertexNormal( nVertexNormalID );
   //   if (!pVertexNormal)
   //      pVertexNormal = pPolyModel->AddVertexNormal( nVertexNormalID );
   //   SetVertexNormalAt( nIndex, pVertexNormal );
   //}

   // EDGES
   //for (UINT nIndex=0; nIndex<nNumVertices; nIndex++)
   //{
   //   Vertex *pVertex0 = GetVertexAt( nIndex );
   //   Vertex *pVertex1 = GetVertexAt( GetNextIndex( nIndex ));
   //   Edge *pEdge = pPolyModel->AddEdge( pVertex0, pVertex1 );
   //   SetEdgeAt( nIndex, pEdge );
   //}

   return TRUE;
}

// uses face neighbors, assumes pV0 and pV1 are aligned with this
Poly *Poly::FindConnectedPoly(Vertex *pV0, Vertex *pV1)
{
   UINT nIndex;
   Poly *pSurroundingFace;
   for (UINT i = 0; i < pV0->GetNumSurroundingPolys(); i++)
   {
      pSurroundingFace = pV0->GetSurroundingPolyAt(i);
      nIndex = pSurroundingFace->FindVertexFaceRelativeIndex(pV1);
      Vertex *pNextVertex = pSurroundingFace->GetVertexAt(GetNextIndex(nIndex));
      if (pV0 == pNextVertex)
         return pSurroundingFace;
   }
   return NULL;
}

BOOL Poly::BuildEdges()
{
   PolyModel *pPolyModel = (PolyModel*)GetOwner();

   BOOL bResult = TRUE;

   UINT nNumSides = GetNumSides();
   for (UINT i = 0; i < nNumSides; i++)
   {
      Vertex *pV0 = GetVertexAt(i);
      Vertex *pV1 = GetVertexAt(GetNextIndex(i));

      Edge *pEdge = pPolyModel->AddEdge(pV0,pV1);
      if (!GetEdgeAt(i))
      {
         if (!SetEdgeAt(i,pEdge))
            bResult = FALSE;
      }
   }

   return bResult;
}

BOOL Poly::IsAdjacentPolyWindingTheSame( Edge *pEdge )
{
   Poly *pPoly = pEdge->GetConnectedPoly( this );
   if (!pPoly)
      return FALSE;

   Vertex *pVertex0 = GetVertexAt(pEdge->GetFaceRelativeIndex( this ));
   Vertex *pVertex1 = pPoly->GetVertexAt(pEdge->GetFaceRelativeIndex( pPoly ));

   return !(pVertex0 == pVertex1) ;
}

void Poly::ReverseWinding()
{
   UINT halfnumsides = m_nNumSides / 2;
   for (UINT i=1; i<halfnumsides; i++)
   {
      UINT nNewIndex = m_nNumSides - i;
      SwapPointers( (void *&)m_pVertices[i], (void *&)m_pVertices[nNewIndex] );
      SwapPointers( (void *&)m_pVertexNormals[i], (void *&)m_pVertexNormals[nNewIndex] );
   }

   for (UINT i=0; i<halfnumsides; i++) // This has to swap even 0
   {
      UINT nNewIndex = m_nNumSides - i - 1;
      SwapPointers( (void *&)m_pEdges[i], (void *&)m_pEdges[nNewIndex] );

      EdgePolyInfo *pInfo = m_pEdges[i]->GetEdgePolyInfo( this );
      if (pInfo)
         pInfo->m_nEdgeIndex = i;

      pInfo = m_pEdges[nNewIndex]->GetEdgePolyInfo( this );
      if (pInfo)
         pInfo->m_nEdgeIndex = nNewIndex;
   }

   SetDirtyFaceNormal( TRUE );

   for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
      pPropertyPoly->ReverseWinding();
}

Edge *Poly::FindNearestEdge( CGLModelView *pView, const CPoint &pt )
{
   pView->SetDoPickLimitComponent( this );
   PickHitArray pickHitArray;
   pView->DoPick( CRect(pt, pt), pickHitArray, NULL, SOT_Edge );
   pView->SetDoPickLimitComponent( NULL );

   Edge *pEdge = NULL;

   if ( pickHitArray.GetNumHits()==1)
      pEdge = (Edge *)pickHitArray[0][0];

   return pEdge;
}

UINT Poly::AddSurface( Surface *pSurface )
{
   UINT nIndex = FindSurface( pSurface );

   if (nIndex == -1)
   {
      Surface **pNewSurfaces = SNEW Surface *[m_nNumSurfaces+1];

      for (nIndex=0; nIndex < m_nNumSurfaces; nIndex++)
         pNewSurfaces[nIndex] = m_pSurfaces[nIndex];

      if (m_pSurfaces) 
         delete [] m_pSurfaces;

      m_pSurfaces = pNewSurfaces;
      m_nNumSurfaces++;

      m_pSurfaces[nIndex] = pSurface;

      SetCompositedSurface( NULL );
      GetModel()->SetCompositedSurfacesDirty( TRUE );
      SetDirty();
   }

   return nIndex;
}

CompositedSurface *Poly::GetCompositedSurface()
{
   if (!m_pCompositedSurface)
      SetCompositedSurface( GetModel()->FindCompositedSurface( this ) );

   return m_pCompositedSurface;
}

void Poly::SetCompositedSurface( CompositedSurface *pCompositedSurface, BOOL bRemoveFromPrev/*=TRUE*/ )
{
   if (m_pCompositedSurface && bRemoveFromPrev)
      m_pCompositedSurface->RemoveFace( this );

   m_pCompositedSurface = pCompositedSurface;

   if (m_pCompositedSurface)
      m_pCompositedSurface->AddFace( this );
}

DeleteFaceInfo *Poly::DeleteFace()
{
   DeleteFaceInfo *pInfo = SNEW DeleteFaceInfo(this);

   Poly *pMirrorThis = (Poly *)GetMirrorObject();
   if (pMirrorThis && pMirrorThis != this)
      pInfo->m_pMirrorDeleteFaceInfo = SNEW DeleteFaceInfo(pMirrorThis);

   pInfo->Delete();
   return pInfo;
}

void Poly::FlipNormal()
{
   CArray<Vertex *> vertexarray;
   CArray<Edge *> edgearray;
   CArray<VertexNormal *> vertexnormalarray;
   CArray< CArray<PropertyVertex *> > uvarray;
   CArray< CArray<PropertyEdge *> > PropertyEdgearray;

   vertexarray.SetSize(GetNumSides());
   edgearray.SetSize(GetNumSides());
   vertexnormalarray.SetSize(GetNumSides());
   
   UINT nPropertyPolys = CountNumPropertyPolys();
   uvarray.SetSize( nPropertyPolys );
   PropertyEdgearray.SetSize( nPropertyPolys );

   for (UINT i=0; i<nPropertyPolys; i++)
   {
      uvarray[i].SetSize( GetNumSides() );
      PropertyEdgearray[i].SetSize( GetNumSides() );
   }

   for (UINT i=0; i<GetNumSides(); i++)
   {
      vertexarray[i] = GetVertexAt(i);
      vertexnormalarray[i] = GetVertexNormalAt(i);
      edgearray[i] = GetEdgeAt(i);

      UINT nCount=0;
      for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
      {
         uvarray[nCount][i] = pPropertyPoly->GetPropertyVertexAt(i); 
         PropertyEdgearray[nCount][i] = pPropertyPoly->GetPropertyEdgeAt(i); 
      }
   }

   for (UINT i=0; i<GetNumSides(); i++)
   {
      UINT nVertexIndex = i==0 ? 0 : GetNumSides()-i;
      UINT nEdgeIndex = GetNumSides()-i-1;

      m_pVertices[nVertexIndex] = vertexarray[i];
      m_pVertexNormals[nVertexIndex] = vertexnormalarray[i];
      m_pEdges[nEdgeIndex] = edgearray[i];
      
      EdgePolyInfo *pInfo = m_pEdges[nEdgeIndex]->GetEdgePolyInfo(this);
      pInfo->m_nEdgeIndex = nEdgeIndex;

      UINT nCount=0;
      for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly(), nCount++)
      {
         pPropertyPoly->m_pPropertyVertices[nVertexIndex] = uvarray[nCount][i]; 
         pPropertyPoly->m_pPropertyEdges[nEdgeIndex] = PropertyEdgearray[nCount][i]; 
      }
   }

   static bInMirror;
   if (!bInMirror)
   {
      bInMirror = TRUE;
      Poly *pMirrorThis = (Poly *)GetMirrorObject();
      if (pMirrorThis && pMirrorThis != this)
         pMirrorThis->FlipNormal();
      bInMirror = FALSE;
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Property Poly

UINT Poly::CountNumPropertyPolys()
{
   UINT nCount = 0;
   for ( PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
      nCount++;
   return nCount;
}

void Poly::AddPropertyPoly( PropertyPoly *pPropertyPoly )
{
   pPropertyPoly->SetPoly( this );

   PropertyPoly **pUpLink, *pCurrPropertyPoly;
   for (pUpLink = &m_pHeadPropertyPoly, pCurrPropertyPoly=*pUpLink; pCurrPropertyPoly; pUpLink = pCurrPropertyPoly->GetNextPropertyPolyUplink(), pCurrPropertyPoly = *pUpLink)
      ;

   *pUpLink = pPropertyPoly;
   pPropertyPoly->SetNextPropertyPoly( NULL );

   pPropertyPoly->Reference();

   UINT nNumSides = GetNumSides();

   if (pPropertyPoly->m_pPropertyVertices==NULL && nNumSides)
   {
      ASSERT(pPropertyPoly->m_pPropertyVertices==NULL);
      pPropertyPoly->m_pPropertyVertices = SNEW PropertyVertex *[nNumSides];
      memset( pPropertyPoly->m_pPropertyVertices, 0, nNumSides * sizeof( PropertyVertex *) );

      ASSERT(pPropertyPoly->m_pPropertyEdges==NULL);
      pPropertyPoly->m_pPropertyEdges = SNEW PropertyEdge *[nNumSides];
      memset( pPropertyPoly->m_pPropertyEdges, 0, nNumSides * sizeof( PropertyEdge *) );

   }

   for (UINT i=0; i < nNumSides; i++)
   {
      Edge *pEdge = GetEdgeAt(i );
      if (pEdge)
         pEdge->SetPropertyEdgeListDirty( TRUE );
   }
}

BOOL Poly::RemovePropertyPoly( PropertyPoly *pPropertyPolyMatch )
{
   PropertyPoly *pPropertyPoly, **ppPropertyPoly;

   for ( ppPropertyPoly = &m_pHeadPropertyPoly, pPropertyPoly = m_pHeadPropertyPoly; pPropertyPoly; ppPropertyPoly = &pPropertyPoly->m_pNextPropertyPoly, pPropertyPoly = pPropertyPoly->m_pNextPropertyPoly ) 
   {
      if (pPropertyPoly == pPropertyPolyMatch)
      {
         *ppPropertyPoly = pPropertyPoly->m_pNextPropertyPoly;
         pPropertyPoly->m_pNextPropertyPoly = NULL;
         pPropertyPoly->DeReference();
         return TRUE;
      }
   }

   return FALSE;
}

PropertyPoly *Poly::FindPropertyPoly( PropertySet *pPropertySet )
{
   for ( PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
   {
      if ( pPropertyPoly->GetPropertySet() == pPropertySet )
         return pPropertyPoly;
   }

   return NULL;
}


// UVPoly

/*
UINT Poly::CountNumUVPolys()
{
   UINT nCount=0;
   for ( PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
   {
      if (pPropertyPoly->GetPropertySet()->GetSOT()!=SOT_TexCoords)
         continue;
      nCount++;
   }
   return nCount;
}
*/

PropertyPoly *Poly::FindLastPropertyPoly(SnipeObjectType nPropertySetSOT)
{
   PropertyPoly *pLastPropertyPoly = NULL;
   for (PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly = pPropertyPoly->GetNextPropertyPoly())
   {
      SnipeObjectType nSOT = pPropertyPoly->GetPropertySet()->GetSOT();
      if (pPropertyPoly->GetPropertySet()->GetSOT() == nPropertySetSOT)
         pLastPropertyPoly = pPropertyPoly;
   }
   return pLastPropertyPoly;
}

UVPoly *Poly::FindUVPoly( TexCoords *pTexCoords )
{
   for ( PropertyPoly *pPropertyPoly = GetHeadPropertyPoly(); pPropertyPoly; pPropertyPoly=pPropertyPoly->GetNextPropertyPoly())
   {
      if (pPropertyPoly->GetPropertySet()->GetSOT()!=SOT_TexCoords)
         continue;

      if ( pPropertyPoly->GetPropertySet() == (PropertySet*)pTexCoords )
         return (UVPoly*)pPropertyPoly;
   }

   return NULL;
}

