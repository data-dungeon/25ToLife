#include "stdafx.h"
#include "PolyModel.h"
#include "GUI\GLModelView.h"
#include "ModelDoc.h"
#include "Resource.h"
#include "ModelTranslatorPluginManager.h"
#include "Properties/SurfaceDriver.h"
#include "UVPoly.h"
#include "UVEdge.h"

#define USECALLLIST

///////////////////////////////////////
// PolyModel

BEGIN_MESSAGE_MAP(PolyModel, TransformableObject)
   SNIPEOBJECT_ON_UPDATE_COMMAND_UI(ID_EXPORT_MODEL, OnUpdateExportModel)
   SNIPEOBJECT_ON_COMMAND(ID_EXPORT_MODEL, OnExportModel)
END_MESSAGE_MAP()

void PolyModel::CountPickObjectType( CGLBaseView *pBaseView, UINT &nCount )
{
   SceneObject::CountPickObjectType(pBaseView, nCount);

   switch (pBaseView->m_nPickObjectType)
   {
      case SOT_Vertex :
         nCount += m_vertexlist.GetSize();
      case SOT_Edge :
         nCount += m_edgelist.GetSize();
      case SOT_Poly :
         nCount += m_polylist.GetSize();
   }
}

BOOL PolyModel::DrawPick( CGLBaseView *pBaseView )
{
   CGLModelView *pModelView = (CGLModelView *)pBaseView;

   TransformableObjectAttribsPusher glattribs( this, pBaseView );
   switch (pBaseView->m_nPickObjectType)
   {
      case SOT_Vertex:
         DrawPickVertices( pModelView );
         return TRUE;
      case SOT_Edge:
         DrawPickEdges( pModelView );
         return TRUE;
      case SOT_Poly:
         DrawPickFaces( pModelView );
         return TRUE;
      case SOT_Bone:
         HierObject::Draw( pModelView );
         return TRUE;
   }

   if (IsKindOf(pBaseView->m_nPickObjectType))
   {
      DrawRetainedMode( pModelView );
      return TRUE;
   }

   return FALSE;
}

void PolyModel::Draw( CGLBaseView *pBaseView )
{
   CGLModelView *pModelView = (CGLModelView *)pBaseView;
   GetRootCache()->SetCurrentInstance( this );

   BOOL bDrawTess = IsDrawTess( pModelView );
   PositionVertices( this, bDrawTess );

   BoundingSphere boundingsphere = GetBoundingSphere();
   boundingsphere.m_vCenter *= GetObjectToWorldMatrix();
   if (!pModelView->IsSphereInFrustum( boundingsphere ))
      return;

   if (pBaseView->m_bDoPick)
   {
      DrawPick( pBaseView );
      GetRootCache()->SetCurrentInstance( NULL );
      return;
   }

   TransformableObjectAttribsPusher glattribs( this, pBaseView );

#if 0 // bounding sphere
   glTranslatef( m_vBoundingCenter.x, m_vBoundingCenter.y, m_vBoundingCenter.z );
   glutWireSphere( m_fBoundingRadius, 32, 32 );
   glTranslatef( -m_vBoundingCenter.x, -m_vBoundingCenter.y, -m_vBoundingCenter.z );
#endif

   if (IsCache()) // we are editing at the component level
   {
      if (pModelView->m_bDisplayInRetain)
         DrawRetainedMode( pModelView );
      else
         DrawImmediateMode( pModelView );
   }
   else
   {
      static RGBAFloat selectedcolor( 0.05f, 0.5f, 0.5f, 1.0f );
      static RGBAFloat deselectedcolor( 0.0f, 0.0f, 0.0f, 1.0f );
      if (IsSelected())
         glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat *)&selectedcolor );
      else
         glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat *)&deselectedcolor );
      DrawRetainedMode( pModelView );
   }

   HierObject::Draw( pModelView );

   // Diagnostics
   glDisable( GL_LIGHTING );
   if (pModelView->m_bDisplayVertexIndices)
      DrawVertexIndices( pModelView );
   if (pModelView->m_bDisplayVertexNormalIndices)
      DrawVertexNormalIndices( pModelView );
   if (pModelView->m_bDisplayFaceIndices)
      DrawFaceIndices( pModelView );
   if (pModelView->m_bDisplayVertexNormals)
      DrawVertexNormals( pModelView );
   if (pModelView->m_bDisplayEdgeIndices)
      DrawEdgeIndices( pModelView );
   if (pModelView->m_bDisplayFaceNormals)
      DrawFaceNormals( pModelView );
   if (pModelView->m_bDisplayFirstVertex)
      DrawFirstVertices( pModelView );
   if (pModelView->m_bDisplayUVBoundary)
      DrawUVBoundaries( pModelView );
   if (pModelView->m_bDisplayUVFaceIndices)
      DrawUVFaceIndices( pModelView );
   if (pModelView->m_bDisplayUVIndices)
      DrawUVIndices( pModelView );
   if (pModelView->m_bDisplayUVEdgeIndices)
      DrawUVEdgeIndices( pModelView );

   GetRootCache()->SetCurrentInstance( NULL );
}

void PolyModel::DrawImmediateMode( CGLModelView *pModelView )
{
#ifdef USECALLLIST
   if (pModelView->GetOperation()==CGLBaseView::OP_NAVIGATING)
   {
      if (m_glCallList)
      {
         glCallList( m_glCallList );
         return;
      }
      else
      {
         m_glCallList = glGenLists( 1 );
         glNewList( m_glCallList, GL_COMPILE_AND_EXECUTE );
      }
   }
   else if (m_glCallList)
   {
      glDeleteLists( m_glCallList, 1 );
      m_glCallList = 0;
   }
#endif

   DrawFaces(pModelView);
   if (pModelView->GetCurrentSelectionSOT() == SOT_Vertex)
      DrawVertices();
   DrawEdges( pModelView );

#ifdef USECALLLIST
   if (m_glCallList)
      glEndList();
#endif
}

void PolyModel::ComputeBoundingSphere()
{
   m_BoundingSphere.m_fRadius = 0.0f;

   PolyModel *pCache = GetRootCache();
   if (!pCache->m_pRTVertices)
      pCache->CreateRTVertices( TRUE );

   BoundingBox box;
   for (UINT i=0; i<pCache->m_nNumRTVertices; i++)
   {
      RTVertex *pRTVertex = &pCache->m_pRTVertices[i];
      Vector vPos = pRTVertex->GetPos();
      box.Push( vPos );
   }

   if (box.IsEmpty())
   {
      m_BoundingSphere.Clear();
   }
   else
   {
      m_BoundingSphere.m_vCenter = box.CalculateCenterPoint();
      box.Offset( -m_BoundingSphere.m_vCenter );

      Vector vCorner( box.m_maxx, box.m_maxy, box.m_maxz );
      m_BoundingSphere.m_fRadius = vCorner.Length();
   }

   SetBoundingSphereDirty( FALSE );
}

void PolyModel::DrawRetainedMode( CGLModelView *pModelView )
{
   PolyModel *pCache = GetRootCache();

   BOOL bDrawTess = IsDrawTess( pModelView );
   //PositionVertices( this, bDrawTess );

   //if (IsBoundingRadiusDirty())
   //   ComputeBoundingRadius();

   glEnableClientState( GL_VERTEX_ARRAY );
   glVertexPointer( 3, GL_FLOAT, sizeof(RTVertex), &pCache->m_pRTVertices->GetPos() );

   if (pModelView->IsDisplayShaded() && pCache->m_pRTVertices)
   {
      glEnable( GL_LIGHTING );
#if 0// to see vertex colors
      glDisable( GL_LIGHTING );
#endif
      if (pCache->m_bCompositedSurfacesDirty)
         pCache->BuildCompositedSurfaces();

      glEnableClientState( GL_NORMAL_ARRAY );
      glNormalPointer( GL_FLOAT, sizeof(RTVertex), &pCache->m_pRTVertices->GetNormal() );
      glEnableClientState( GL_TEXTURE_COORD_ARRAY );
      glTexCoordPointer( 2, GL_FLOAT, sizeof(RTVertex), &pCache->m_pRTVertices->GetUV() );
      glEnableClientState( GL_COLOR_ARRAY );
      glColorPointer( 4, GL_FLOAT, sizeof(RTVertex), &pCache->m_pRTVertices->GetColor() );

      Matrix44 viewmatrix;
      for (POSITION pos=pCache->GetHeadCompositedSurfacePosition(); pos; )
      {
         CompositedSurface *pCompositedSurface = pCache->GetNextCompositedSurface( pos );
         pCompositedSurface->DrawFacesRetained( viewmatrix, pModelView, bDrawTess );
      }

      glDisableClientState( GL_COLOR_ARRAY ); 
      glDisableClientState( GL_TEXTURE_COORD_ARRAY );
      glDisableClientState( GL_NORMAL_ARRAY );
   }

   if (pModelView->IsDisplayWireframe() && pCache->m_pRTEdges)
   {
      glDisable( GL_LIGHTING );
//      if (pModelView->IsDisplayShaded())
         glDepthMask( FALSE );

      UINT nNumSoftEdges = pCache->m_nNumRTEdges - pCache->m_nNumBorderEdges - pCache->m_nNumHardEdges;
      // soft
      if (nNumSoftEdges)
      {
         glColor4f( 1.0f, 1.0f, 1.0f, 0.4f*m_fDrawOpacity );
         glLineWidth( 1.4f );
         glDrawElements( GL_LINES, nNumSoftEdges*2, GL_UNSIGNED_INT, pCache->m_pRTEdges );
      }

      // hard
      if (pCache->m_nNumHardEdges)
      {
         glColor4f( 1.0f, 1.0f, 1.0f, 1.0f*m_fDrawOpacity );
         glLineWidth( 1.4f );
         glDrawElements( GL_LINES, pCache->m_nNumHardEdges*2, GL_UNSIGNED_INT, pCache->m_pRTEdges+nNumSoftEdges );
      }

      // border
      if (pCache->m_nNumBorderEdges)
      {
         glColor4f( 1.0f, 1.0f, 1.0f, 1.0f*m_fDrawOpacity );
         glLineWidth( 3.0f );
         glDrawElements( GL_LINES, pCache->m_nNumBorderEdges*2, GL_UNSIGNED_INT, pCache->m_pRTEdges+nNumSoftEdges+pCache->m_nNumHardEdges );
         glLineWidth( 1.0f );
      }

//      if (pModelView->IsDisplayShaded())
         glDepthMask( TRUE );
   }

   if (pModelView->GetCurrentSelectionSOT() == SOT_Vertex)
   {
      glDisable( GL_LIGHTING );
      glPointSize( VERTEXSIZE );
      glColor4f( 1.0f, 0.0f, 0.0f, 1.0f*m_fDrawOpacity );
      glDrawElements( GL_POINTS, pCache->m_nNumVisibleVertices, GL_UNSIGNED_INT, pCache->m_pVisibleVertices );
      //glDrawArrays( GL_POINTS, 0, m_nNumRTVertices );

      glColor4f( 0.5f, 0.5f, 0.5f, 1.0f*m_fDrawOpacity );
      glDrawElements( GL_POINTS, pCache->m_nNumLockedVertices, GL_UNSIGNED_INT, pCache->m_pLockedVertices );
   }

   glDisableClientState( GL_VERTEX_ARRAY );
}

void PolyModel::DrawVertices()
{
   glDisable( GL_LIGHTING );
   glPointSize(VERTEXSIZE);

   for (POSITION pos=GetHeadVertexPosition(); pos; )
   {
      Vertex *pVertex = GetNextVertex(pos);

      if (!pVertex->IsDraw())
         continue;

      RGBAFloat color( 1.0f, 0.0f, 0.0f, 1.0f );
      pVertex->GetDrawColor( color );
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

      pVertex->Draw();
   }
}

inline void DoVertexDrawPick( CGLModelView *pView, PolyModel *pModel, Vertex *pVertex)
{
   if (!pVertex->IsDraw())
      return;

   glLoadName( (GLuint)pVertex );
   pVertex->DrawPick( pView );
}

void PolyModel::DrawPickVertices( CGLModelView *pView )
{
   glPointSize(VERTEXSIZE);//This needs to match the drawer
   if (pView->IsDoPickLimitByComponent())
   {
      Edge *pEdge = pView->IsLimitDoPickByEdge();
      if (pEdge)
      {
         DoVertexDrawPick( pView, this, pEdge->GetFirstVertex());
         DoVertexDrawPick( pView, this, pEdge->GetSecondVertex());
         glLoadName( NULL );
         return;
      }
      Poly *pPoly = pView->IsLimitDoPickByFace();
      if (pPoly)
      {
         for (UINT i=0; i<pPoly->GetNumSides(); i++)
         {
            Vertex *pVertex = pPoly->GetVertexAt(i);
            DoVertexDrawPick( pView, this, pVertex);
         }
         glLoadName( NULL );
         return;
      }
      ASSERT(FALSE);
      return;
   }
   else 
   { // No Component Limitation
      for (POSITION pos=GetHeadVertexPosition(); pos; )
      {
         Vertex *pVertex = GetNextVertex(pos);

         BOOL bExclude = FALSE;
         if (pView->GetDoPickExcludeList() && pView->GetDoPickExcludeList()->Find( pVertex ))
            bExclude = TRUE;

         if (!bExclude)
            DoVertexDrawPick( pView, this, pVertex);
      }
      glLoadName( NULL );
   }
}

void PolyModel::DrawVertexNormals( CGLModelView *pModelView )
{
   float factor = pModelView->GetCamera()->GetCameraDistanceFactor() * 2.0f;
   glColor3fv(colorYellow);
   glLineWidth(2.0f);

   glBegin(GL_LINES);
   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace(pos);

      for (UINT i = 0; i < pPoly->GetNumSides(); i++)
      {
         Vertex *pVertex = pPoly->GetVertexAt(i);

         if (!pVertex->IsVisible())
            continue;

         VertexNormal *pVertexNormal = pPoly->GetVertexNormalAt(i);

         Vector endpos = pVertexNormal->GetDir() * factor;
         endpos += pVertex->GetPos();

         glColor3fv(colorYellow);
         glVertex3f(pVertex->GetPos().x,pVertex->GetPos().y,pVertex->GetPos().z);
         glColor4f(1.0f, 1.0f, 0.0f, 0.0f);
         glVertex3f(endpos.x,endpos.y,endpos.z);
      }
   }
   glEnd();
}

#ifdef _DEBUG
//#define SHOWVREFCOUNT
//#define SHOWVNREFCOUNT
//#define SHOWUVS
//#define SHOWUVREFCOUNT
//#define SHOWPOLYNEIGHBORS
//#define SHOWEDGENEIGHBORS
#endif // _DEBUG

void PolyModel::DrawVertexIndices( CGLModelView *pModelView )
{
   glColor3f(0.0f,1.0f,0.0f);

   String strID;
   for (POSITION pos=GetHeadVertexPosition(); pos; )
   {
      Vertex *pVertex = GetNextVertex(pos);
      if (!pVertex->IsVisible())
         continue;
      Vector vPos = pVertex->GetPos();

      Vector vNormal( 0.0f );
      if (pVertex->GetNumSurroundingPolys())
      {
         for (UINT i=0; i<pVertex->GetNumSurroundingPolys(); i++)
            vNormal += *pVertex->GetSurroundingPolyAt( i )->GetFaceNormal();
         vNormal /= pVertex->GetNumSurroundingPolys();
      }
      else
      {
         vNormal.Set( 0.0f, 0.0, 1.0f );
         pModelView->ViewToWorld( vNormal, TRUE );
      }
      strID = String((ULONG)pVertex->GetID());

   #ifdef SHOWPOLYNEIGHBORS
      strID += "(";
      for (int i = 0; i < (int)pVertex->GetNumSurroundingPolys(); i++)
      {
         if (i!=0)
            strID += ",";
         strID += String((ULONG)pVertex->GetSurroundingPolyAt(i)->GetID());
      }
      strID += ")";
   #endif

   #ifdef SHOWEDGENEIGHBORS
      strID += "(Edges:";
      for (UINT i = 0; i < pVertex->GetNumSurroundingEdges(); i++)
      {
         if (i!=0)
            strID += ",";
         strID += String((ULONG)pVertex->GetSurroundingEdgeAt(i)->GetID());
      }
      strID += ")";
   #endif

   #ifdef SHOWVREFCOUNT
      strID += "(";
      strID += "REF:";
      strID += String((ULONG)pVertex->GetRefCount());
      strID += ")";
   #endif

      pModelView->Draw3DText( strID, vPos, vNormal );
   }
}

#define OFFSET_TOWARD_CENTER 0.30f

void PolyModel::DrawVertexNormalIndices( CGLModelView *pModelView )
{
   glColor3f(1.0f,1.0f,0.0f);
   String strID;

   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace(pos);

      Vector vFaceCenter;
      pPoly->GetWorldPos( vFaceCenter );

      if (!pPoly->IsVisible())
         continue;

      for (UINT i = 0; i < pPoly->GetNumSides(); i++)
      {
         Vertex *pVertex = pPoly->GetVertexAt(i);
         Vector vPos = pVertex->GetPos();
         Vector vDir = vFaceCenter-vPos;
         vDir *= OFFSET_TOWARD_CENTER;
         vPos += vDir;

         VertexNormal *pVertexNormal = pPoly->GetVertexNormalAt(i);
         Vector vNormal = pVertexNormal->GetDir();

         strID = String((ULONG)pVertexNormal->GetID());

#ifdef SHOWVREFCOUNT
         strID += "(";
         strID += "REF:";
         strID += String((ULONG)pVertexNormal->GetRefCount());
         strID += ")";
#endif

         pModelView->Draw3DText( strID, vPos, vNormal );
      }
   }
}

void PolyModel::DrawEdges( CGLModelView *pModelView )
{
   glDisable( GL_LIGHTING );

   float fAlpha;

   BOOL bDrawTess = IsDrawTess( pModelView );

   for (POSITION pos=GetHeadEdgePosition(); pos; )
   {
      Edge *pEdge = GetNextEdge(pos);
      if (!pEdge->IsDraw())
         continue;

      if (pEdge->IsSoftEdge())
      {
         fAlpha = pEdge->IsSelected() ? 0.8f : 0.4f;
         glLineWidth(1.0f);
      }
      else if (pEdge->IsBorderEdge())
      {
         fAlpha = 1.0f;
         glLineWidth(3.0f);
      }
      else // hard edge
      {
         fAlpha = 1.0f;
         glLineWidth(1.0f);
      }

      RGBAFloat color( 1.0f, 1.0f, 1.0f, fAlpha );
      pEdge->GetDrawColor( color );
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

      if (pModelView->IsDisplayWireframe() || pEdge->IsSelected() || pEdge->IsDrawMirrorColor())
         pEdge->Draw( bDrawTess );
   }
}

inline void DoEdgeDrawPick( Edge *pEdge, BOOL bDrawTess, CGLModelView *pView)
{
   if (!pEdge->IsDraw())
      return;
   glLoadName( (GLuint)pEdge );
   pEdge->DrawPick( pView, bDrawTess );
}

void PolyModel::DrawPickEdges( CGLModelView *pView )
{
   BOOL bDrawTess = IsDrawTess( pView );

   if (pView->IsDoPickLimitByComponent())
   {
      Vertex *pVertex = pView->IsLimitDoPickByVertex();
      if (pVertex)
      {
         for (UINT i=0; i<pVertex->GetNumSurroundingEdges(); i++)
         {
            Edge *pEdge = pVertex->GetSurroundingEdgeAt(i);
            if (pEdge->GetFirstVertex()==pVertex || pEdge->GetSecondVertex()==pVertex) // don't want attached
               DoEdgeDrawPick( pEdge, bDrawTess, pView );
         }
         glLoadName( NULL );
         return;
      }
      Poly *pPoly = pView->IsLimitDoPickByFace();
      if (pPoly)
      {
         for ( UINT i=0; i<pPoly->GetNumSides(); i++)
            DoEdgeDrawPick(pPoly->GetEdgeAt(i), bDrawTess, pView);
         glLoadName( NULL );
         return;
      }
      ASSERT(FALSE);
      return;
   }
   else // No Component Limitation
   {
      for (POSITION pos=GetHeadEdgePosition(); pos; )
      {
         Edge *pEdge = GetNextEdge(pos);

         BOOL bExclude = FALSE;
         if (pView->GetDoPickExcludeList() && pView->GetDoPickExcludeList()->Find( pEdge ))
            bExclude = TRUE;

         if (!bExclude)
            DoEdgeDrawPick(pEdge, bDrawTess, pView);
      }
      glLoadName( NULL );
   }
}

//#define SHOWCONNECTEDPOLYS
//#define SHOWUVSOFTFLAGS
//#define SHOWRELATIVEDGEINDEX
#define SHOWALIGNMENT
//#define SHOWFIRSTVERTEX
//#define SHOWREFCOUNT
//#define SHOWDETACHCOUNT
//#define SHOWEDGECENTER
//#define SHOWUVEDGE_UVS

void PolyModel::DrawEdgeIndices( CGLModelView *pModelView )
{
#ifdef USECALLLISTS
   if (m_glEdgeIndiceCallList) {
      glCallList( m_glEdgeIndiceCallList );
      return;
   }
   m_glEdgeIndiceCallList = glGenLists( 1 );
   glNewList( m_glEdgeIndiceCallList, GL_COMPILE_AND_EXECUTE );
#endif

#if 1
   String strText;
   for (POSITION pos=GetHeadEdgePosition(); pos; )
   {
      Edge *pEdge = GetNextEdge(pos);
      if (!pEdge->IsVisible())
         continue;

      Vector normal(0), up(0,1,0);
      Vector edgedir = pEdge->GetSecondVertex()->GetPos() - pEdge->GetFirstVertex()->GetPos();
      Vector edgecenter = (pEdge->GetSecondVertex()->GetPos() + pEdge->GetFirstVertex()->GetPos()) * 0.5f;

      Poly *pPoly0 = pEdge->GetFirstPoly();
      Poly *pPoly1 = pEdge->GetSecondPoly();
      if (pPoly0 || pPoly1)
      {
         if (pPoly0)
            normal += *pPoly0->GetFaceNormal();
         if (pPoly1)
            normal += *pPoly1->GetFaceNormal();
         normal *= 0.5f;
         up = normal.Cross( edgedir );
      }
      else
         normal.Set( 0,0,1 );

//      strText = (int)pEdge->GetID();
      pEdge->GetDisplayIDText(pModelView, strText);

#ifdef SHOWREFCOUNT
      strText += String(" ") + pEdge->GetRefCount();
#endif

      glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
      pModelView->Draw3DText( strText, edgecenter, normal, 22.0f, up );
   }
#else
   String temp;
   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace(pos);

      if (!pPoly->IsVisible())
         continue;
      for (UINT i = 0; i < pPoly->GetNumVertices(); i++)
      {
         glColor4fv(colorWhite);
         Edge *pEdge = pPoly->GetEdgeAt(i);
         temp = String((ULONG)pEdge->GetID());
         Vector v0 = pEdge->GetFirstVertex()->GetPos();
         Vector v1 = pEdge->GetSecondVertex()->GetPos();
         Vector edgecenter = (v0 + v1) * 0.5f;
         Vector faceCenter;
         pPoly->CalcCenter(faceCenter);
         Vector position = ((faceCenter - edgecenter)*0.25) + edgecenter;
#ifdef SHOWCONNECTEDPOLYS
         glColor4fv(colorLtBlue);
         Poly *pConnectedPoly = pEdge->GetConnectedPoly(pPoly);
         if (pConnectedPoly)
            pModelView->DrawText(position.x,position.y,position.z,String((ULONG)pConnectedPoly->GetID()));
#endif

#ifdef SHOWRELATIVEDGEINDEX
         glColor4fv(colorLtBlue);
         pModelView->DrawText(position.x,position.y,position.z,String((ULONG)pEdge->GetFaceRelativeIndex(pPoly)));
#endif

#ifdef SHOWREFCOUNT
         glColor4fv(colorLtBlue);
         pModelView->DrawText(position.x,position.y,position.z,String((ULONG)pEdge->RefCount()));
#endif

#ifdef SHOWDETACHCOUNT
         glColor4fv(colorLtBlue);
         temp += "(R:";
         temp += String((ULONG)pEdge->RefCount());
         temp += ",D:";
         temp += String((ULONG)pEdge->DetachCount());
         temp += ")";

#endif

#ifdef SHOWUVSOFTFLAGS
         String temp = "(";   
         glColor4fv(colorLtBlue);
         CArray<int> *pUVSoftFlags = pEdge->GetUVSoftFlags(pPoly);
         for (int j = 0; j < pUVSoftFlags->GetSize(); j++)
         {
            if (j!=0)
               temp += ",";
            temp += String((ULONG)(*pUVSoftFlags)[j]);      
         }
         temp += ")";
         pModelView->DrawText(position.x,position.y,position.z,temp);
#endif

#ifdef SHOWALIGNMENT
         temp +="(";
         glColor4fv(colorLtBlue);
         temp += String((ULONG)pEdge->IsAligned(pPoly));
         temp +=")";
#endif

#ifdef SHOWFIRSTVERTEX
         glColor4fv(colorLtBlue);
         glBegin(GL_POINTS);
         glVertex3fv(&pEdge->GetFirstVertex()->GetPos().x);
         glEnd();
#endif

#ifdef SHOWEDGECENTER
         glColor4f(0.0f,1.0f,0.0f,1.0f);
         Vector v = (pEdge->GetSecondVertex()->GetPos() - pEdge->GetFirstVertex()->GetPos()) * 0.5f + pEdge->GetFirstVertex()->GetPos();
         glBegin(GL_POINTS);
         glVertex3fv(&v.x);
         glEnd();
#endif

         pModelView->DrawText(position.x,position.y,position.z,temp,2.0f);
      }
   }
#endif

#ifdef USECALLLISTS
   glEndList();
#endif
}

BOOL PolyModel::IsDrawTess( CGLModelView *pModelView ) 
{ 
   return !pModelView->IsDrawAsPolyModel(); 
}

void PolyModel::DrawFirstVertices( CGLModelView *pModelView )
{
   glColor3fv(colorOrange);
   glPointSize(4.0f);

   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace(pos);
      Vertex *pVertex = pPoly->GetVertexAt(0);

      if (!pPoly->IsVisible())
         continue;

      Vector center;
      pPoly->CalcCenter(center);
      Vector v = center-pVertex->GetPos();
      v = v * 0.1f + pVertex->GetPos();
      glBegin(GL_POINTS);
      glVertex3f(v.x,v.y,v.z);
      glEnd();
   }
}

void PolyModel::DrawUVBoundaries( CGLModelView *pModelView )
{
   glDepthMask( FALSE );
   glColor4f( 0.0f, 0.0f, 1.0f, 0.4f );

   for (POSITION pos=GetHeadEdgePosition(); pos; )
   {
      Edge *pEdge = GetNextEdge(pos);
      if (!pEdge->IsDraw() || !pEdge->IsUVBorderEdge())
         continue;

      pEdge->Draw( pModelView );
   }

   glDepthMask( TRUE );
}

void PolyModel::DrawUVFaceIndices( CGLModelView *pModelView )
{
   String strText;
   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace(pos);
      if ( !pPoly->IsVisible() )
         continue;

      UVPoly *pUVPoly = (UVPoly*)pPoly->FindLastPropertyPoly(SOT_TexCoords);
      if (!pUVPoly)
         continue;

      int nID = pUVPoly->GetID();
      strText = nID;
#ifdef SHOWREFCOUNT
      strText += (String)" (Ref=" + pUVPoly->GetRefCount() + ")";
#endif

      glColor4f( 0.5f, 0.5f, 1.0f, 0.6f );
      Vector position;
      pPoly->CalcCenter(position);
      Vector *direction = pPoly->GetFaceNormal();
      Vector up( 0.0f, 1.0f, 0.0f );
      pModelView->ViewToWorld( up, TRUE );
      pModelView->Draw3DText( strText, position, *direction, 25.0f, up );
   }
}

void PolyModel::DrawUVIndices( CGLModelView *pModelView )
{
   glColor3f(1.0f,0.0f,0.0f);
   String strID;

   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace(pos);

      UVPoly *pUVPoly = (UVPoly*)pPoly->FindLastPropertyPoly(SOT_TexCoords);
      if (!pUVPoly)
         continue;

      Vector vFaceCenter;
      pPoly->GetWorldPos( vFaceCenter );

      if (!pPoly->IsVisible())
         continue;

      for (UINT i = 0; i < pPoly->GetNumSides(); i++)
      {
         Vertex *pVertex = pPoly->GetVertexAt(i);
         UV *pUV = pUVPoly->GetUVAt(i);
         Vector vPos = pVertex->GetPos();
         Vector vDir = vFaceCenter-vPos;
         vDir *= OFFSET_TOWARD_CENTER;
         vPos += vDir;

         VertexNormal *pVertexNormal = pPoly->GetVertexNormalAt(i);
         Vector vNormal = pVertexNormal->GetDir();

         strID = String((ULONG)pUV->GetID());

#ifdef SHOWVREFCOUNT
         strID += "(";
         strID += "REF:";
         strID += String((ULONG)pUV->GetRefCount());
         strID += ")";
#endif

         pModelView->Draw3DText( strID, vPos, vNormal );
      }
   }
}

void PolyModel::DrawUVEdgeIndices( CGLModelView *pModelView )
{
   glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
   String strID;

   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace(pos);

      UVPoly *pUVPoly = (UVPoly*)pPoly->FindLastPropertyPoly(SOT_TexCoords);
      if (!pUVPoly)
         continue;

      Vector vFaceCenter;
      pPoly->GetWorldPos( vFaceCenter );

      if (!pPoly->IsVisible())
         continue;

      for (UINT i = 0; i < pPoly->GetNumSides(); i++)
      {
         Edge *pEdge = pPoly->GetEdgeAt(i);
         UVEdge *pUVEdge = pUVPoly->GetUVEdgeAt(i);
         Vector vPos;
         pEdge->GetWorldPos( vPos );
         Vector vDir = vFaceCenter-vPos;
         vDir *= OFFSET_TOWARD_CENTER;
         vPos += vDir;

         VertexNormal *pVertexNormal = pPoly->GetVertexNormalAt(i);
         Vector vNormal = pVertexNormal->GetDir();

         strID = String((ULONG)pUVEdge->GetID());

#ifdef SHOWVREFCOUNT
         strID += "(";
         strID += "REF:";
         strID += String((ULONG)pUVEdge->GetRefCount());
         strID += ")";
#endif
#ifdef SHOWUVEDGE_UVS
         strID += " ";
         strID += (ULONG)pUVEdge->GetFirstUV()->GetID();
         strID += " ";
         strID += (ULONG)pUVEdge->GetSecondUV()->GetID();
#endif

         pModelView->Draw3DText( strID, vPos, *pPoly->GetFaceNormal() );
      }
   }
}

void PolyModel::BuildCompositedSurfaces()
{
   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace( pos );
      pPoly->GetCompositedSurface(); // this will cause it to be made if necessary
   }

   SetCompositedSurfacesDirty( FALSE );
}

void PolyModel::DrawFaces( CGLModelView *pModelView )
{
   if (m_bCompositedSurfacesDirty)
      BuildCompositedSurfaces();

   glEnable( GL_LIGHTING );

   BOOL bDrawTess = IsDrawTess( pModelView );

   const Matrix44 &viewmatrix = pModelView->GetWorldToViewMatrix();

   if (pModelView->IsDisplayShaded())
   {
      for (POSITION pos=GetHeadCompositedSurfacePosition(); pos; )
      {
         CompositedSurface *pCompositedSurface = GetNextCompositedSurface( pos );
         pCompositedSurface->DrawFacesImmediate( viewmatrix, pModelView, bDrawTess );
      }
   }

   // Draw selected and mirror faces
   RGBAFloat color(1.0f, 1.0f, 1.0f, 0.2f);
   glDisable( GL_LIGHTING );
   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace(pos);
      if (!pPoly->GetDrawColor( color )) // this will tell us whether it is default or not
         continue;

      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
      pPoly->Draw( viewmatrix, bDrawTess );
   }
}

inline void DoFaceDrawPick( const Matrix44 &viewmatrix, Poly *pPoly, BOOL bDrawTess )
{
   if (!pPoly->IsDraw())
      return;

   glLoadName( (GLuint)pPoly );
   pPoly->DrawPick( viewmatrix, bDrawTess );
}

void PolyModel::DrawPickFaces( CGLModelView *pView )
{
   BOOL bDrawTess = IsDrawTess( pView );
   const Matrix44 &viewmatrix = pView->GetWorldToViewMatrix();

   if (pView->IsDoPickLimitByComponent())
   {
      Edge *pEdge = pView->IsLimitDoPickByEdge();
      if (pEdge)
      {
         DoFaceDrawPick( viewmatrix, pEdge->GetFirstPoly(), bDrawTess );
         DoFaceDrawPick( viewmatrix, pEdge->GetSecondPoly(), bDrawTess );
         glLoadName( NULL );
         return;
      }

      Vertex *pVertex = pView->IsLimitDoPickByVertex();
      if (pVertex)
      {
         for (UINT i=0; i<pVertex->GetNumSurroundingPolys(); i++)
            DoFaceDrawPick( viewmatrix, pVertex->GetSurroundingPolyAt(i), bDrawTess );
         glLoadName( NULL );
         return;
      }

      ASSERT(FALSE);
      return;
   }
   else // No Component Limitation
   {
      for (POSITION pos=GetHeadFacePosition(); pos; )
      {
         Poly *pPoly = GetNextFace(pos);

         BOOL bExclude = FALSE;
         if (pView->GetDoPickExcludeList() && pView->GetDoPickExcludeList()->Find( pPoly ))
            bExclude = TRUE;

         if (!bExclude)
            DoFaceDrawPick( viewmatrix, pPoly, bDrawTess );
      }
      glLoadName( NULL );
   }
}

void PolyModel::DrawFaceNormals( CGLModelView *pModelView )
{
   float factor = pModelView->GetCamera()->GetCameraDistanceFactor() * 8.0;
   glLineWidth(1.0f);
   glBegin(GL_LINES);

   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace(pos);

      if (!pPoly->IsVisible())
         continue;

      Vector position;
      pPoly->CalcCenter(position);
      Vector *facenormal = pPoly->GetFaceNormal();
      Vector endpos = (*facenormal * factor * 0.1f) + position;
      glColor3fv(colorYellow);
      glVertex3f(position.x,position.y,position.z);
      glColor4f(1.0f, 1.0f, 0.0f, 0.0f );
      glVertex3f(endpos.x,endpos.y,endpos.z);
   }
   glEnd();
}

//#define SHOWNUMVERTS

void PolyModel::DrawFaceIndices( CGLModelView *pModelView )
{
#ifdef USECALLLISTS
   if (m_glFaceIndiceCallList) {
      glCallList( m_glFaceIndiceCallList );
      return;
   }
   m_glFaceIndiceCallList = glGenLists( 1 );
   glNewList( m_glFaceIndiceCallList, GL_COMPILE_AND_EXECUTE );
#endif
   String strText;
   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Poly *pPoly = GetNextFace(pos);
      if ( !pPoly->IsVisible() )
         continue;

      int nID = pPoly->GetID();
      strText = nID;
#ifdef SHOWNUMVERTS
      strText += (String)" (Verts=" + pPoly->GetNumVertices() + ")";
#endif
#ifdef SHOWREFCOUNT
      strText += (String)" (Ref=" + pPoly->GetRefCount() + ")";
#endif
#ifdef SHOWDETACHCOUNT
      strText += (String)" (Detach=" + pPoly->DetachCount() + ")";
#endif

      glColor4f( 0.5f, 0.5f, 1.0f, 0.6f );
      Vector position;
      pPoly->CalcCenter(position);
      Vector *direction = pPoly->GetFaceNormal();
      Vector up( 0.0f, 1.0f, 0.0f );
      pModelView->ViewToWorld( up, TRUE );
      pModelView->Draw3DText( strText, position, *direction, 25.0f, up );
//      pModelView->DrawText( position.x, position.y, position.z, strText, 1.5f );
   }
#ifdef USECALLLISTS
   glEndList();
#endif
}

void PolyModel::OnUpdateExportModel(CCmdUI* pCmdUI) 
{
   PluginList *pPluginList = GetMainInterface()->GetModelTranslatorPluginManager()->GetPluginList();

   BOOL bEnable = FALSE;
   for (POSITION pos = pPluginList->GetHeadPosition(); pos; )
   {
      ModelTranslatorPlugin *pPlugin = (ModelTranslatorPlugin *)pPluginList->GetNext( pos );
      if (pPlugin->GetExportCallback()) {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable(bEnable);
}

void PolyModel::OnExportModel() 
{
   GetMainInterface()->GetModelTranslatorPluginManager()->Export(this);
}
