#include "stdafx.h"
#include "ColorVertexSet.h"
#include "../gui/Snipe.h"
#include "../gui/GLRenderWnd.h"
#include "Poly.h"
#include "../GUI/GLTexCoordView.h"
#include "PatchModel.h"
#include "../interface/Implement.h"
#include "resource.h"

#include "ColorVertex.h"
#include "ColorEdge.h"
#include "ColorPoly.h"
#include "ColorPatchVertex.h"
#include "ColorPatchEdge.h"
#include "ColorPatch.h"

/////////////////////////////////////////////////////
// ColorVertexSetContainer

IMPLEMENT_SNIPEOBJECT_CREATE( ColorVertexSetContainer )

ColorVertexSetContainer::ColorVertexSetContainer()
{
   m_strName = "Color Vertex Sets";
}

ColorVertexSetContainer::~ColorVertexSetContainer()
{
}

/////////////////////////////////////////////////////
// ColorVertexSet

IMPLEMENT_SNIPEOBJECT_CREATE( ColorVertexSet );
IMPLEMENT_COMMANDOBJECT( ColorVertexSet );

ColorVertexSet::ColorVertexSet()
{
   m_strName = "Color Vertex Set";
}

ColorVertexSet::~ColorVertexSet()
{
}

void ColorVertexSet::PreDestruct()
{
   HierObject::PreDestruct();
}

void ColorVertexSet::DrawColorVertices( CGLTexCoordView *pView )
{
/*
   if (pView->m_bDoPick)
   {
      glPointSize( VERTEXSIZE );
      for (POSITION pos=GetHeadPropertyVertexPosition(); pos; )
      {
         PropertyVertex *pPropertyVertex = m_propertyvertexlist.GetNext(pos);
         if (pPropertyVertex->GetPropertySet()->GetSOT() != SOT_ColorVertexSet)
            continue;

         ColorVertex *pColorVertex = (ColorVertex*)pPropertyVertex;

         if (!pColorVertex->IsVisible())
            continue;

         glLoadName( (GLuint)pColorVertex );
         glBegin( GL_POINTS );
         glVertex2fv( &pColorVertex->GetPos().x );
         glEnd();
      }
      glLoadName( NULL );
      return;
   }

   BOOL bDrawTess = TRUE;//GetModel()->IsDrawTess( pView );
   float fAlpha;
   for (POSITION pos=GetHeadPropertyEdgePosition(); pos; )
   {
      PropertyEdge *pPropertyEdge = GetNextPropertyEdge(pos);
      if (pPropertyEdge->GetPropertySet()->GetSOT() != SOT_ColorVertexSet)
         continue;
      ColorEdge *pColorEdge = (ColorEdge*)pPropertyEdge;

      if (!pColorEdge->IsDraw())
         continue;

      if (pColorEdge->IsSoftEdge())
      {
         fAlpha = pColorEdge->IsSelected() ? 0.8f : 0.4f;
         glLineWidth(1.0f);
      }
      else if (pColorEdge->IsBorderEdge())
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
      pColorEdge->GetDrawColor( color );
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

      pColorEdge->Draw( bDrawTess );
   }

   // THIS NEEDS TO BE OOP!
   // Draw the tessellated edges
   PolyModel *pPolyModel = GetModel();
   PatchModel *pPatchModel = (PatchModel*)pPolyModel->IsKindOf(SOT_PatchModel);
   if (bDrawTess && pView->m_bDisplayHiddenEdges)
   {
      glLineWidth( 1.0f );
      glColor4f( 1.0f, 1.0f, 1.0f, 0.4f );

      for (POSITION pos=GetHeadPropertyPolyPosition(); pos; )
      {
         PropertyPoly *pPropertyPoly = (PropertyPoly *)GetNextPropertyPoly(pos);
         Patch *pPatch = (Patch*)pPropertyPoly->GetPoly();

//         if (pPatch->GetID() != 11)
//            continue;

         for (UINT nTessPoly=0; nTessPoly<pPatch->GetNumTessPolys(); nTessPoly++)
         {
            TessPoly *pTessPoly = pPatch->GetTessPolyAt(nTessPoly);

            for (TessPropertyPoly *pTessPropertyPoly = pTessPoly->GetHeadTessPropertyPoly(); pTessPropertyPoly; pTessPropertyPoly=pTessPropertyPoly->GetNextTessPropertyPoly())
            {
               TessColorPoly *pTessColorPoly = (TessColorPoly*)pTessPropertyPoly;
               glBegin(GL_LINES);
               for (UINT i = 0; i < pTessPoly->GetNumSides(); i++)
               {
                  ColorVertex *pColorVertex = pTessColorPoly->GetColorVertexAt(i);
                  ColorVertex *pNextColorVertex = pTessColorPoly->GetColorVertexAt(pTessPoly->GetNextIndex(i));
                  if (!pColorVertex->IsVisible() || !pNextColorVertex->IsVisible())
                     continue;
                  glVertex2fv(&pColorVertex->GetPos().x);
                  glVertex2fv(&pNextColorVertex->GetPos().x);
               }
               glEnd();
               break;
            }
            break;
         }
      }
   }

   // draw patch tangents
#if 0
   glColor3f(0.0f,1.0f,0.0f);
   glBegin(GL_POINTS);
   for (pos = GetHeadPropertyPolyPosition(); pos; )
   {
      PropertyPoly *pPropertyPoly = GetNextPropertyPoly(pos);
      if (pPropertyPoly->GetPropertySet()->GetSOT() != SOT_ColorVertexSet)
         continue;

      ColorPatch *pColorPatch = (ColorPatch*)pPropertyPoly;

      int order[] = { 5,6,9,10 };
//      if (pColorPatch->GetID() == 1)
      {

         Vector2 *pCVs;
         for (UINT i = 0; i < 1; i++)
         {
            pCVs = pColorPatch->GetVector2CVs(i);
            for (UINT j = 0; j < 4; j++)
            {
               glVertex2fv(&pCVs[order[j]].x);
            }
         } 
      }
   }
   glEnd();
#endif

   glDepthFunc( GL_GREATER ); // first one drawn wins
   glPointSize( VERTEXSIZE );
   glBegin( GL_POINTS );
   for (POSITION pos=GetHeadPropertyVertexPosition(); pos; )
   {
      PropertyVertex *pPropertyVertex = m_propertyvertexlist.GetNext(pos);
      if (pPropertyVertex->GetPropertySet()->GetSOT() != SOT_ColorVertexSet)
         continue;

      ColorVertex *pColorVertex = (ColorVertex*)pPropertyVertex;
      if (!pColorVertex->IsVisible())
         continue;

      RGBAFloat color( 1.0f, 0.0f, 0.0f, 1.0f );
      pColorVertex->GetDrawColor( color );
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

      Vector2 p = pColorVertex->GetPos();
      glVertex3f( p.x, p.y, 0.001f );
   }
   glEnd();

   glDepthFunc( GL_LEQUAL );
*/
}

// HierObject overrides
void ColorVertexSet::CountPickObjectType( CGLBaseView *pBaseView, UINT &nCount )
{
   switch (pBaseView->m_nPickObjectType)
   {
   case SOT_ColorVertex :
      nCount += GetNumColorVertexs();
   }
}

SimpleComponentList *ColorVertexSet::FindComponentList( const String &strComponentType )
{
   if (strComponentType == "ColorVertex")
      return (SimpleComponentList *)&m_propertyvertexlist;
   return SnipeObject::FindComponentList(strComponentType);
}

Component *ColorVertexSet::CreateComponent( SnipeObjectType nSOT )
{
   PolyModel *pModel = GetModel();
   switch (nSOT)
   {
      case SOT_PropertyVertex:
      {
       if (pModel->IsKindOf(SOT_PatchModel))
         return ColorPatchVertex::Create();
       else
          return ColorVertex::Create();
      }
      case SOT_PropertyPoly:
      {
         if (pModel->IsKindOf(SOT_PatchModel))
            return ColorPatch::Create();
         else
            return ColorPoly::Create();
      }
   }
   return PropertySet::CreateComponent( nSOT );
}

void ColorVertexSet::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   m_propertyvertexlist.Save( bs, "ColorVertices" );
   m_propertypolys.Save( bs, "Faces" );

   PropertySet::Save( bs );
}

BOOL ColorVertexSet::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "ColorVertices")
      return m_propertyvertexlist.Load( bs );
   else if (dataheader.m_strKey == "Faces")
      return m_propertypolys.Load( bs );
   else
      return PropertySet::ParseData( bs, dataheader );
}

BOOL ColorVertexSet::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_REMOVEDFROMPROJECT:
      {
         break;
      }
      case NOTIFYOBJ_ADDEDTOPROJECT:
      {
         break;
      }
   }

   return PropertySet::OnObjectNotify( msg, wParam, lParam );
}

PropertyVertex *ColorVertexSet::DuplicateComponent(PropertyVertex *pPropertyVertex)
{
   PropertyVertex *pNewPropertyVertex = AddPropertyVertex();
   ((ColorVertex*)pNewPropertyVertex)->SetPos(((ColorVertex*)pPropertyVertex)->GetPos());
   return pNewPropertyVertex;
}

PropertyVertex *ColorVertexSet::CreatePropertyVertex(UINT nID/*=-1*/)
{
   return (PropertyVertex*)ColorVertex::CreateWithID(nID);
}

PropertyEdge *ColorVertexSet::CreatePropertyEdge(UINT nID/*=-1*/)
{
   return (PropertyEdge*)ColorEdge::CreateWithID(nID);
}

PropertyPoly *ColorVertexSet::CreatePropertyPoly(UINT nID/*=-1*/)
{
   return (PropertyPoly*)ColorPoly::CreateWithID(nID);
}

PropertyVertex *ColorVertexSet::CreatePropertyPatchComponent(UINT nID/*=-1*/)
{
   return (PropertyVertex*)ColorPatchVertex::CreateWithID(nID);
}

PropertyPatchEdge *ColorVertexSet::CreatePropertyPatchEdge(UINT nID/*=-1*/)
{
   return (PropertyPatchEdge*)ColorPatchEdge::CreateWithID(nID);
}

PropertyPatch *ColorVertexSet::CreatePropertyPatch(UINT nID/*=-1*/)
{
   return (PropertyPatch*)ColorPatch::CreateWithID(nID);
}
