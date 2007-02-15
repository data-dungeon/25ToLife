#include "stdafx.h"
#include "PatchModel.h"
#include "GLBaseView.h"
#include "Gui\GLModelView.h"
#include "Properties/SurfaceDriver.h"
#include "..\Utility\MathTools.h"

// GeometryObject Overrides

void PatchModel::Draw( CGLBaseView *pBaseView )
{
   PolyModel::Draw(pBaseView);

   CGLModelView *pModelView = (CGLModelView*)pBaseView;
   if (!IsDrawTess( pModelView ) || pBaseView->m_bDoPick)
      return;

   // Diagnostics
   glDisable( GL_LIGHTING );
   if (/*pModelView->IsDisplayWireframe() && */pModelView->m_bDisplayHiddenEdges)
      DrawHiddenEdges(pModelView);
   if (pModelView->m_bDisplayVertexNormals && pModelView->m_bDisplayHiddenEdges)
      DrawVertexNormals( pModelView );
   if (pModelView->m_bDisplayControlVertices)
      DrawControlVertices();
   if (pModelView->m_bDisplayContinuity)
      DrawContinuity(pModelView);
}

void PatchEdge::GetDisplayIDText( CGLModelView *pView, String &strText )
{
   strText.Empty();

   if (pView->m_bDisplayContinuityIDs)
   {
      String strPrev;
      if (GetPrevContEdge(TRUE))
         strPrev = (int)GetPrevContEdge(TRUE)->GetID();

      String strID = (int)GetID();

      String strNext;
      if (GetNextContEdge(TRUE))
         strNext = (int)GetNextContEdge(TRUE)->GetID();

      if (IsAlignedWithContinuity())
         strText = strPrev + " " + strID + " " + strNext;
      else
         strText = strNext + " " + strID + " " + strPrev;
   }
   else
      Edge::GetDisplayIDText(pView, strText);
}

void PatchEdge::Draw( BOOL bDrawTess )
{
   if (!bDrawTess)
   {
      Edge::Draw( bDrawTess );
      return;
   }

   UINT nNumVerts = GetAlignedTessEdgeVertices()->GetNumVertices();
   if (nNumVerts==0)
      return;

   glBegin(GL_LINES);

   if (IsManifold())
      glColor3f(1.0f,0.0f,0.0f);

   for (UINT i = 0; i < nNumVerts-1; i++)
   {
      Vertex *pVertex = GetAlignedTessEdgeVertices()->GetVertexAt(i);
      ASSERT(pVertex);
      glVertex3fv( &pVertex->GetPos().x );
      pVertex = (Vertex*)GetAlignedTessEdgeVertices()->GetVertexAt(i+1);
      ASSERT(pVertex);
      glVertex3fv( &pVertex->GetPos().x );
   }

   if (IsNearestFocusObject())
   {
      SnipeObject *pFocusObject = SelectionList::GetFocusObject();
      ASSERT(pFocusObject);
      if (pFocusObject->IsKindOf( SOT_Vertex ))
      {
         Vertex *pFocusVertex = (Vertex *)pFocusObject;
         RGBAFloat color;
         pFocusVertex->GetDrawColor( color );

         float fStep = 0.8f/(nNumVerts-1);
         float pAlpha;

         if (GetAlignedTessEdgeVertices()->GetVertexAt(0)==pFocusVertex)
            pAlpha = 0.8f;
         else
         {
            pAlpha = 0.0f;
            fStep = -fStep;
         }

         for (UINT i = 0; i < nNumVerts-1; i++)
         {
            Vertex *pVertex = GetAlignedTessEdgeVertices()->GetVertexAt(i);
            ASSERT(pVertex);
            glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), pAlpha );
            pAlpha-=fStep;
            glVertex3fv( &pVertex->GetPos().x );

            pVertex = GetAlignedTessEdgeVertices()->GetVertexAt(i+1);
            ASSERT(pVertex);
            glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), pAlpha );
            glVertex3fv( &pVertex->GetPos().x );
         }
      }
      else if ( pFocusObject->IsKindOf( SOT_Poly ))
      {
      }
      else
      {
         ASSERT(FALSE);
      }
   }

   glEnd();


   if (GetPatch())
      GetPatch()->DrawInternalEdges();
   if (GetConnectedPatch(GetPatch()))
      GetConnectedPatch(GetPatch())->DrawInternalEdges();
}

void PatchEdge::Draw( CGLBaseView *pView )
{
   CGLModelView *pModelView = (CGLModelView *)pView;

   if (pModelView->IsDrawAsPolyModel())
   {
      Edge::Draw( pView );
      return;
   }

   UINT nNumVerts = GetAlignedTessEdgeVertices()->GetNumVertices();
   if (nNumVerts==0)
      return;

   glLineWidth( IsBorderEdge() ? 3.0f : 1.4f );
   if (IsManifold())
      glColor3f(1.0f,0.0f,0.0f);

   glBegin( GL_LINE_STRIP );
      for (UINT i=0; i<nNumVerts; i++)
      {
         Vertex *pVertex = GetAlignedTessEdgeVertices()->GetVertexAt(i);
         ASSERT(pVertex);
         glVertex3fv( &pVertex->GetRTVertexHead()->GetPos().x );
      }
   glEnd();

//   if (IsSelected())
//      DrawEdgeContinuity(pView);
}

void PatchEdge::DrawNearestFocused( CGLBaseView *pView, SnipeObject *pFocusObject )
{
   CGLModelView *pModelView = (CGLModelView *)pView;

   if (pModelView->IsDrawAsPolyModel())
   {
      Edge::DrawNearestFocused( pView, pFocusObject );
      return;
   }

   if (pFocusObject->IsKindOf( SOT_Vertex ))
   {
      BOOL bReverse = GetFirstVertex() == pFocusObject;

      UINT nNumVerts = GetAlignedTessEdgeVertices()->GetNumVertices();

      glDisable( GL_DEPTH_TEST );
      glBegin( GL_LINE_STRIP );
         for (UINT i=0; i<nNumVerts; i++)
         {
            Vertex *pVertex = GetAlignedTessEdgeVertices()->GetVertexAt(i);
            float fPercent = (float)i / (float)(nNumVerts-1);
            if (bReverse)
               fPercent = 1.0f - fPercent;
            glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), g_focuscolor.GetAlpha()*0.2f*fPercent );
            glVertex3fv( &pVertex->GetRTVertexHead()->GetPos().x );
         }
      glEnd();

      glEnable( GL_DEPTH_TEST );
      glBegin( GL_LINE_STRIP );
         for (UINT i=0; i<nNumVerts; i++)
         {
            Vertex *pVertex = GetAlignedTessEdgeVertices()->GetVertexAt(i);
            float fPercent = (float)i / (float)(nNumVerts-1);
            if (bReverse)
               fPercent = 1.0f - fPercent;
            glColor4f( g_focuscolor.GetRed(), g_focuscolor.GetGreen(), g_focuscolor.GetBlue(), g_focuscolor.GetAlpha()*0.6f*fPercent );
            glVertex3fv( &pVertex->GetRTVertexHead()->GetPos().x );
         }
      glEnd();
   }
}

void Patch::Draw( CGLBaseView *pView )
{
   CGLModelView *pModelView = (CGLModelView *)pView;

   if (pModelView->IsDrawAsPolyModel())
   {
      Poly::Draw( pView );
      return;
   }

   for (UINT i=0; i<GetNumTessPolys(); i++)
   {
      TessPoly *pTessPoly = GetTessPolyAt(i);
      UINT nNumVerts = pTessPoly->GetNumSides();
      glBegin( nNumVerts==4 ? GL_QUADS : GL_POLYGON );
      for (UINT i = 0; i < nNumVerts; i++)
         glVertex3fv( &pTessPoly->GetVertexAt(i)->GetRTVertexHead()->GetPos().x );
      glEnd();
   }
}

//#define SHOWHIDDENFACESIDS

void PatchModel::DrawHiddenEdges( CGLModelView *pModelView )
{
#ifdef SHOWHIDDENFACESIDS
   String temp;
#endif
   glDisable( GL_LIGHTING );
   glDepthMask( FALSE );

   glLineWidth(1.0f);
   for (POSITION pos=m_polylist.GetHeadPosition(); pos; )
   {
      Patch *pPatch = (Patch *)m_polylist.GetNext(pos);
      if (!pPatch->IsVisible())
         continue;
      for (UINT nTessPoly=0; nTessPoly<pPatch->GetNumTessPolys(); nTessPoly++)
      {
         TessPoly *pPoly = pPatch->GetTessPolyAt(nTessPoly);

#ifdef SHOWHIDDENFACESIDS
         Vector center;
         pPoly->CalcPolyCenter(center);
         temp = String((ULONG)pPoly->GetID());
         glColor4f( 1.0f, 1.0f, 1.0f, 0.4f );
         pModelView->DrawText(center.x,center.y,center.z,temp);
#endif
         UINT nNumVerts = pPoly->GetNumSides();
         ASSERT(nNumVerts);
         for (UINT i = 0; i < nNumVerts; i++)
         {
            UINT nNext = i + 1;
            if (nNext==nNumVerts)
               nNext = 0;
            Vertex *pVertex = pPoly->GetVertexAt(i);
            Vertex *pNextVertex = pPoly->GetVertexAt(nNext);
            //if ((pVertex->IsBorder())&&(pNextVertex->IsBorder()))
            //   continue;
            glBegin(GL_LINES);
            glColor4f( 1.0f, 1.0f, 1.0f, 0.1f );
            glVertex3fv(&pVertex->GetPos().x);
            glVertex3fv(&pNextVertex->GetPos().x);
            glEnd();
         }
      }
   }
   glDepthMask( TRUE );
}

void PatchModel::DrawControlVertices()
{
   glColor3fv(colorGreen);
   glPointSize(6.0f);
 
#if 1
   glBegin(GL_POINTS);
   for (POSITION pos=m_edgelist.GetHeadPosition(); pos; )
   {
      PatchEdge *pEdge = (PatchEdge*)m_edgelist.GetNext(pos);
      if (!pEdge->IsVisible())
         continue;
      Vector v0 = pEdge->GetFirstTangentPos();
      glVertex3f(v0.x,v0.y,v0.z);

      Vector v1 = pEdge->GetSecondTangentPos();
      glVertex3f(v1.x,v1.y,v1.z);
   }
   glEnd();

   Vector v;
   glLineWidth(4.0f);
   for (POSITION pos=m_edgelist.GetHeadPosition(); pos; )
   {
      PatchEdge *pEdge = (PatchEdge*)m_edgelist.GetNext(pos);
      if (!pEdge->IsVisible())
         continue;
      glBegin(GL_LINES);
      glColor4f(0.0f,1.0f,0.0f,1.0f);
      v = pEdge->GetFirstVertex()->GetPos();
      glVertex3f(v.x,v.y,v.z);
      glColor4f(0.0f,1.0f,0.0f,0.0f);
      v = pEdge->GetFirstTangentPos();
      glVertex3f(v.x,v.y,v.z);
      glEnd();

      glBegin(GL_LINES);
      glColor4f(0.0f,1.0f,0.0f,1.0f);
      v = pEdge->GetSecondVertex()->GetPos();
      glVertex3f(v.x,v.y,v.z);
      glColor4f(0.0f,1.0f,0.0f,0.0f);
      v = pEdge->GetSecondTangentPos();
      glVertex3f(v.x,v.y,v.z);
      glEnd();

   }

#else// use actual cvs stored in quadpatch
  glBegin(GL_POINTS);
   UINT order[] = { 1,2,4,7,8,11,13,14 };
   for (POSITION pos=m_polylist.GetHeadPosition(); pos; )
   {
      Patch *pPatch = GetNextFace(pos);
      if (!pPatch->IsVisible())
         continue;
      for (UINT nSubPatchIndex = 0; nSubPatchIndex < pPatch->GetNumSubPatches(); nSubPatchIndex++)
      {
         QuadPatch *pQuadPatch = pPatch->GetQuadPatch(nSubPatchIndex);
         Vector *pCVs = pQuadPatch->GetCVs();
         for (UINT i = 0; i < 8; i++)
            glVertex3fv(&pCVs[order[i]].x);
      }
   }
   glEnd();
#endif
}

#ifdef _DEBUG
#define SHOWVREFCOUNT
//#define SHOWVDETACHCOUNT
//#define SHOWPOLYNEIGHBORS
//#define SHOWUVREFCOUNT
//#define SHOWUVDETACHCOUNT
//#define SHOWEDGENEIGHBORS
#endif // _DEBUG

void PatchModel::DrawVertexIndices( CGLModelView *pModelView )
{
   PolyModel::DrawVertexIndices( pModelView );

#if 0
   String temp;
   for (POSITION pos=m_polylist.GetHeadPosition(); pos; )
   {
      Patch *pPatch = (Patch *)m_polylist.GetNext(pos);
      if (!pPatch->m_pCenterVertex || !pPatch->IsVisible())
         continue;
      Vector position = pPatch->m_pCenterVertex->GetPos();
//      temp = String((ULONG)pPatch->m_pCenterVertex->GetID());
      temp.Empty;
#ifdef SHOWVREFCOUNT
      temp += "(";
      temp += "VREF:";
      temp += String((ULONG)pPatch->m_pCenterVertex->RefCount());
      temp += ")";
#endif
#ifdef SHOWVDETACHCOUNT
      temp += "(";
      temp += "Detach:";
      temp += String((ULONG)pPatch->m_pCenterVertex->DetachCount());
      temp += ")";
#endif
#ifdef SHOWUVDETACHCOUNT
      if (pPatch->m_pCenterVertex)
      {
         CArray< CArray<UV*> > uvs;
         pPatch->m_pCenterVertex->GatherSecondaryUVs(uvs);
         temp += "(";
         temp += "UVDetach:";
         for (int i = 0; i < uvs.GetSize(); i++)
         {
            for (int j = 0; j < uvs[i].GetSize(); j++)
            {
               if (i>0)
                  temp += ",";
               temp += String((ULONG)uvs[i][j]->DetachCount());
            }
         }
         temp += ")";
      }
#endif

      pModelView->DrawText(position.x,position.y,position.z,temp,1.5f);
   }
#endif
}

void PatchModel::DrawVertexNormals( CGLModelView *pModelView )
{
   float factor = pModelView->GetCamera()->GetCameraDistanceFactor();
   glColor3fv(colorYellow);
   glLineWidth(2.0f);

   glBegin(GL_LINES);
   for (POSITION pos=m_polylist.GetHeadPosition(); pos; )
   {
      Patch *pPatch = (Patch *)m_polylist.GetNext(pos);
      for (UINT nTessPoly=0; nTessPoly<pPatch->GetNumTessPolys(); nTessPoly++)
      {
         TessPoly *pPoly = pPatch->GetTessPolyAt(nTessPoly);
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
   }
   glEnd();
}

/*
void PatchModel::DrawTessEdge(const TessEdge &tessedge,
                              CMap <Vector, const Vector &, UINT, UINT> &vertexindexmap,
                              CMap<RTEdge, const RTEdge &, UINT, UINT > &edgemap)
{
   RTEdge rtedge;
   UINT numVerts = tessedge.GetNumVertices();
   if (numVerts==0)
      return;
   for (UINT i = 0; i < numVerts-1; i++)
   {
      UINT nIndex0,nIndex1;
      Vertex *pVertex = (Vertex*)tessedge.GetVertexAt(i);
      if (!pVertex)
         continue;
      Vector pos = pVertex->GetPos();
      if (!vertexindexmap.Lookup(pos,nIndex0))
      {
 //        ASSERT(FALSE);
         continue;
      }
      pVertex = (Vertex*)tessedge.GetVertexAt(i+1);
      if (!pVertex)
         continue;
      pos = pVertex->GetPos();
      if (!vertexindexmap.Lookup(pos,nIndex1))
      {
//         ASSERT(FALSE);
         continue;
      }
      rtedge.SetVertexIndices(nIndex0,nIndex1);
      UINT nIndex;
      if (!edgemap.Lookup(rtedge,nIndex)){
         nIndex = edgemap.GetSize();
         edgemap.SetAt(rtedge,nIndex);
      }
   }
}
*/
void PatchModel::CountPickObjectType( CGLBaseView *pBaseView, UINT &nCount )
{
   PolyModel::CountPickObjectType(pBaseView, nCount);

   switch (pBaseView->m_nPickObjectType)
   {
   case SOT_Patch :
      nCount += m_polylist.GetSize();
   }
}

void PatchModel::DrawBezierSplineSegment(UINT nMaxLevel,
                                         UINT nLevel,
                                         const Vector &p0,const Vector &p1,const Vector &p2,const Vector &p3)
{
   if (nLevel == nMaxLevel)
   {
      glBegin( GL_LINES );
      glVertex3fv(&p0.x);
      glVertex3fv(&p3.x);
      glEnd();
      return;
   }

   Vector a0,a1,a2,a3;
   Vector b0,b1,b2,b3;

   MathTools::SplitBezierCurve(0.5,p0,p1,p2,p3,a0,a1,a2,a3,b0,b1,b2,b3);

   DrawBezierSplineSegment(nMaxLevel,nLevel+1,a0,a1,a2,a3);
   DrawBezierSplineSegment(nMaxLevel,nLevel+1,b0,b1,b2,b3);
}

void PatchModel::DrawContinuity( CGLModelView *pModelView )
{
   float viewfactor = pModelView->GetCamera()->GetCameraDistanceFactor();
   glDisable(GL_CULL_FACE);
   for (POSITION pos=m_edgelist.GetHeadPosition(); pos; )
   {
      PatchEdge *pPatchEdge = (PatchEdge *)m_edgelist.GetNext(pos);
      if (pPatchEdge->IsVisible())
         pPatchEdge->DrawEdgeContinuity((CGLBaseView*)pModelView);
   }
}