#include "stdafx.h"
#include "TexCoords.h"
#include "../gui/Snipe.h"
#include "../gui/GLRenderWnd.h"
#include "Poly.h"
#include "../GUI/GLTexCoordView.h"
#include "PatchModel.h"
#include "../interface/Implement.h"
#include "resource.h"

#include "UV.h"
#include "UVEdge.h"
#include "UVPoly.h"
#include "PatchUV.h"
#include "UVPatchEdge.h"
#include "UVPatch.h"

/////////////////////////////////////////////////////
// TexCoordsContainer

IMPLEMENT_SNIPEOBJECT_CREATE( TexCoordsContainer )

TexCoordsContainer::TexCoordsContainer()
{
   m_strName = "Texture Coordinate Sets";
}

TexCoordsContainer::~TexCoordsContainer()
{
}

/////////////////////////////////////////////////////
// TexCoords

IMPLEMENT_SNIPEOBJECT_CREATE( TexCoords );

TexCoords::TexCoords()
{
   m_pCurInstance = NULL;
}

TexCoords::~TexCoords()
{
}

CSnipeDocTemplate *TexCoords::GetDocTemplate()
{
   return GetApp().GetModelDocTemplate();
}

int TexCoords::GetItemHeight()
{
   static int nItemHeight = min(128, max(16, GetApp().GetProfileInt( "General", "TexCoords Icon Size", 48 )));
   return nItemHeight;
}

void TexCoords::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   float fIconSize = min(GetItemHeight(), pItem->m_szControl.cy) - 2;
   float y = 1.0f;

   GLfloat fScissor[4]; // x, y, w, h
   glGetFloatv( GL_SCISSOR_BOX, fScissor );
   glScissor( fScissor[0] + x, fScissor[1] + y, fIconSize, fIconSize );

   //glColor4f( 0.0f, 0.0f, 0.0f, 0.7f );
   //glBegin( GL_QUADS );
   //   glVertex2f( x, y );
   //   glVertex2f( x + fIconSize, y );
   //   glVertex2f( x + fIconSize, y + fIconSize );
   //   glVertex2f( x, y + fIconSize );
   //glEnd();

   glColor4f( 0.0f, 0.0f, 0.0f, 0.5f );
   glLineWidth( 0.5f );
   glBegin( GL_LINES );
   Vector2 v2Pos, v2Offset( x, y );
   for (POSITION pos=GetHeadPropertyEdgePosition(); pos; )
   {
      PropertyEdge *pPropertyEdge = GetNextPropertyEdge(pos);
      if (pPropertyEdge->GetPropertySet()->GetSOT() != SOT_TexCoords)
         continue;

      UVEdge *pUVEdge = (UVEdge*)pPropertyEdge;

      v2Pos = pUVEdge->GetFirstUV()->GetPos();
      v2Pos *= fIconSize;
      v2Pos += v2Offset;
      glVertex2fv( &v2Pos.x );

      v2Pos = pUVEdge->GetSecondUV()->GetPos();
      v2Pos *= fIconSize;
      v2Pos += v2Offset;
      glVertex2fv( &v2Pos.x );
   }
   glEnd();

   glScissor( fScissor[0], fScissor[1], fScissor[2], fScissor[3] );

   glColor3f( 0.1f, 0.1f, 0.1f );
   pItem->DrawText( x + fIconSize + 4, 12, GetTitle() );
}

void TexCoords::DrawUVs( CGLTexCoordView *pView )
{
   if (pView->m_bDoPick)
   {
      glPointSize( VERTEXSIZE );
      for (POSITION pos=GetHeadPropertyVertexPosition(); pos; )
      {
         PropertyVertex *pPropertyVertex = m_propertyvertexlist.GetNext(pos);
         if (pPropertyVertex->GetPropertySet()->GetSOT() != SOT_TexCoords)
            continue;

         UV *pUV = (UV*)pPropertyVertex;

         if (!pUV->IsVisible())
            continue;

         glLoadName( (GLuint)pUV );
         glBegin( GL_POINTS );
         glVertex2fv( &pUV->GetPos().x );
         glEnd();
      }
      glLoadName( NULL );
      return;
   }

   BOOL bDrawTess = TRUE;//GetModel()->IsDrawTess( pView );
   float fAlpha;
   for (POSITION pos=GetHeadPropertyEdgePosition(); pos; )
   {
      PropertyEdge *pUVEdge = GetNextPropertyEdge(pos);
      if (pUVEdge->GetPropertySet()->GetSOT() != SOT_TexCoords)
         continue;
//      UVEdge *pUVEdge = (UVEdge*)pPropertyEdge;

      if (!pUVEdge->IsDraw())
         continue;

      if (pUVEdge->IsSoftEdge())
      {
         fAlpha = pUVEdge->IsSelected() ? 0.8f : 0.4f;
         glLineWidth(1.0f);
      }
      else if (pUVEdge->IsBorderEdge())
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
      pUVEdge->GetDrawColor( color );
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

      pUVEdge->Draw( bDrawTess );
   }

   // draw patch tangents
#if 0
   glColor3f(0.0f,1.0f,0.0f);
   glBegin(GL_POINTS);
   for (pos = GetHeadPropertyPolyPosition(); pos; )
   {
      PropertyPoly *pPropertyPoly = GetNextPropertyPoly(pos);
      if (!pPropertyPoly->IsKindOf(SOT_UVPatch))
         continue;

      if (pPropertyPoly->GetPropertySet()->GetSOT() != SOT_TexCoords)
         continue;

      UVPatch *pUVPatch = (UVPatch*)pPropertyPoly;

//      int order[] = { 5,6,9,10 };
      int order[] = { 1,2,4,7,8,11,13,14 };

      if (pUVPatch->GetID() == 58)
      {

         Vector2 *pCVs;
         for (UINT i = 0; i < 1; i++)
         {
            pCVs = pUVPatch->GetVector2CVs(i);
            for (UINT j = 0; j < 8; j++)
            {
               glVertex2fv(&pCVs[order[j]].x);
            }
         } 
      }
   }
   glEnd();
#endif

#if 0
   // draw uv handles
   glColor3f(0.0f,1.0f,0.0f);
   glBegin(GL_POINTS);
   for (pos = GetHeadPropertyEdgePosition(); pos; )
   {
      PropertyEdge *pPropertyEdge = (PropertyEdge *)GetNextPropertyEdge(pos);
      if (!pPropertyEdge->IsKindOf(SOT_UVPatchEdge))
         continue;

      if (pPropertyEdge->GetPropertySet()->GetSOT() != SOT_TexCoords)
         continue;

      UVPatchEdge *pUVPatchEdge = (UVPatchEdge*)pPropertyEdge;

      Vector uv;
      if (pUVPatchEdge->GetFirstUV()->IsSelected())
      {
         uv = pUVPatchEdge->GetFirstUVTangentPos();
         glVertex2fv(&uv.x);
      }
      if (pUVPatchEdge->GetSecondUV()->IsSelected())
      {
         uv = pUVPatchEdge->GetSecondUVTangentPos();
         glVertex2fv(&uv.x);
      }
 
   }
   glEnd();
#endif

   glDepthFunc( GL_GREATER ); // first one drawn wins
   glPointSize( VERTEXSIZE );
   glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );
   glBegin( GL_POINTS );
   for (POSITION pos=GetHeadPropertyVertexPosition(); pos; )
   {
      PropertyVertex *pPropertyVertex = m_propertyvertexlist.GetNext(pos);
      if (pPropertyVertex->GetPropertySet()->GetSOT() != SOT_TexCoords)
         continue;

      UV *pUV = (UV*)pPropertyVertex;
      if (!pUV->IsVisible())
         continue;

      Vector2 p = pUV->GetPos();
      glVertex3f( p.x, p.y, 0.001f );
   }
   glEnd();

   glDepthFunc( GL_LEQUAL );

}

// HierObject overrides
void TexCoords::CountPickObjectType( CGLBaseView *pBaseView, UINT &nCount )
{
   switch (pBaseView->m_nPickObjectType)
   {
   case SOT_UV :
      nCount += GetNumUVs();
   }
}

SimpleComponentList *TexCoords::FindComponentList( const String &strComponentType )
{
   if (strComponentType == "uv")
      return (SimpleComponentList *)&m_propertyvertexlist;
   return SnipeObject::FindComponentList(strComponentType);
}

Component *TexCoords::CreateComponent( SnipeObjectType nSOT )
{
   PolyModel *pModel = GetModel();
   switch (nSOT)
   {
      case SOT_PropertyVertex:
      {
       if (pModel->IsKindOf(SOT_PatchModel))
         return PatchUV::Create();
       else
          return UV::Create();
      }
      case SOT_PropertyPoly:
      {
         if (pModel->IsKindOf(SOT_PatchModel))
            return UVPatch::Create();
         else
            return UVPoly::Create();
      }
   }
   return PropertySet::CreateComponent( nSOT );
}

void TexCoords::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   m_propertyvertexlist.Save( bs, "UVs" );
   m_propertypolys.Save( bs, "Faces" );

   PropertySet::Save( bs );
}

BOOL TexCoords::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "UVs")
      return m_propertyvertexlist.Load( bs );
   else if (dataheader.m_strKey == "Faces")
      return m_propertypolys.Load( bs );
   else
      return PropertySet::ParseData( bs, dataheader );
}

BOOL TexCoords::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   return PropertySet::OnObjectNotify( msg, wParam, lParam );
}

PropertyVertex *TexCoords::DuplicateComponent(PropertyVertex *pPropertyVertex)
{
   PropertyVertex *pNewPropertyVertex = AddPropertyVertex();
   ((UV*)pNewPropertyVertex)->SetPos(((UV*)pPropertyVertex)->GetPos());
   return pNewPropertyVertex;
}

PropertyVertex *TexCoords::CreatePropertyVertex(UINT nID/*=-1*/)
{
   return (PropertyVertex*)UV::CreateWithID(nID);
}

PropertyEdge *TexCoords::CreatePropertyEdge(UINT nID/*=-1*/)
{
   return (PropertyEdge*)UVEdge::CreateWithID(nID);
}

PropertyPoly *TexCoords::CreatePropertyPoly(UINT nID/*=-1*/)
{
   return (PropertyPoly*)UVPoly::CreateWithID(nID);
}

PropertyVertex *TexCoords::CreatePropertyPatchComponent(UINT nID/*=-1*/)
{
   return (PropertyVertex*)PatchUV::CreateWithID(nID);
}

PropertyPatchEdge *TexCoords::CreatePropertyPatchEdge(UINT nID/*=-1*/)
{
   return (PropertyPatchEdge*)UVPatchEdge::CreateWithID(nID);
}

PropertyPatch *TexCoords::CreatePropertyPatch(UINT nID/*=-1*/)
{
   return (PropertyPatch*)UVPatch::CreateWithID(nID);
}
