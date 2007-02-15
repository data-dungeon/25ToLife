#include "stdafx.h"
#include "Rotoscope.h"
#include "GUI/Snipe.h"
#include "GUI/GL3DView.h"

#define DPCM 28.346456692913385826771653543307 // = 72 DPI

/////////////////////////////////////////////////////
// RotoscopeContainer

IMPLEMENT_SNIPEOBJECT_CREATE( RotoscopeContainer )
RotoscopeContainer::RotoscopeContainer()
{
   m_strName = "Rotoscopes";
}

RotoscopeContainer::~RotoscopeContainer()
{
}

/////////////////////////////////////////////////////
// Rotoscope

IMPLEMENT_SNIPEOBJECT_CREATE( Rotoscope );

static TypeProperty::Info::Node f_viewangleinfos[] =
{
   TypeProperty::Info::Node( "Front", "Front" ),
   TypeProperty::Info::Node( "Back", "Shaded" ),
   TypeProperty::Info::Node( "Left", "Left" ),
   TypeProperty::Info::Node( "Right", "Right" ),
   TypeProperty::Info::Node( "Top", "Top" ),
   TypeProperty::Info::Node( "Bottom", "Bottom" )
};

TypeProperty::Info         Rotoscope::m_nViewAngleInfo( "View Angle", "View Angle", f_viewangleinfos, sizeof(f_viewangleinfos)/sizeof(TypeProperty::Info::Node), VIEW_FRONT );
ImagePointerProperty::Info Rotoscope::m_pImageInfo( "Image", "Image" );
FloatProperty::Info        Rotoscope::m_fOpacityInfo( "Opacity", "Opacity", 1.0f, 0.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT );
Transform2Property::Info   Rotoscope::m_transformInfo( "Transform", "Transform" );

Rotoscope::Rotoscope()
{
   m_nViewAngle.Init( &m_nViewAngleInfo, this );
   m_pImage.Init( &m_pImageInfo, this );
   m_fOpacity.Init( &m_fOpacityInfo, this );
   m_transform.Init( &m_transformInfo, this );
}

Rotoscope::~Rotoscope()
{
}

int Rotoscope::GetItemHeight()
{
   static int nItemHeight = min(128, max(16, GetApp().GetProfileInt( "General", "Image Icon Size", 48 )));
   return nItemHeight;
}

void Rotoscope::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   if (m_pImage)
      m_pImage.GetValue()->Draw( pItem, Vector2( x, 1 ), Vector2( pItem->m_szControl.cy-2, pItem->m_szControl.cy-2 ));

   glColor3f( 0.1f, 0.1f, 0.1f );
   pItem->DrawText( x + pItem->m_szControl.cy-2 + 4, 12, GetTitle());

   glColor3f( 0.4f, 0.4f, 0.4f );
   String strViewAngle;
   m_nViewAngle.GetValueString( strViewAngle );
   pItem->DrawText( x + pItem->m_szControl.cy-2 + 4, 12 + 14, strViewAngle );
}

void Rotoscope::PushMatrix( CGL3DView *p3DView )
{
   Vector vRotate = p3DView->GetCamera()->m_transform.m_vRotate.GetValue();
   glRotatef( vRotate.z, 0.0f, 0.0f, 1.0f );
   glRotatef( vRotate.x, 1.0f, 0.0f, 0.0f );
   glRotatef( vRotate.y, 0.0f, 1.0f, 0.0f );
   const Matrix44 &matrix = m_transform.GetMatrix();
   GLdouble glmatrix[16];
   matrix.GetDoubleMatrix( glmatrix, TRUE );
   glMultMatrixd( glmatrix );
   Image *pImage = m_pImage;
   glScalef( (float)pImage->GetWidth()/DPCM, (float)pImage->GetHeight()/DPCM, 1 );
}

void Rotoscope::Draw( CGLBaseView *pView )
{
   static Vector vPos(-0.5f, 0.5f, 0.0f);
   static Vector2 v2Size(1.0f, -1.0f);

   Image *pImage = m_pImage;
   if (!pImage)
      return;

   ASSERT( pView->IsKindOf( SVT_CGL3DView ) );
   CGL3DView *p3DView = (CGL3DView *)pView;
   if (p3DView->GetViewAngle() != m_nViewAngle || p3DView->GetCamera()->m_bPerspective)
      return;

   vPos.z = -(p3DView->GetCamera()->m_fFarClip - p3DView->GetCamera()->m_fNearClip) * 0.90f;

   BOOL bWasCullFace = glSet( GL_CULL_FACE, FALSE );
      glMatrixMode( GL_MODELVIEW );
      glPushMatrix();
         PushMatrix( p3DView );

         m_pImage.Bind();
            pImage->Draw( pView, vPos, v2Size, m_fOpacity );
         m_pImage.UnBind();

      glPopMatrix();
   glSet( GL_CULL_FACE, bWasCullFace );
}

void Rotoscope::DrawFocused( CGLBaseView *pView )
{
   static Vector vPos(-0.5f, 0.5f, 0.0f);
   static Vector2 v2Size(1.0f, -1.0f);

   Image *pImage = m_pImage;
   if (!pImage)
      return;

   CGL3DView *p3DView = (CGL3DView *)pView;
   if (p3DView->GetViewAngle() != m_nViewAngle || p3DView->GetCamera()->m_bPerspective)
      return;

   vPos.z = -(p3DView->GetCamera()->m_fFarClip - p3DView->GetCamera()->m_fNearClip) * 0.90f;

   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
      PushMatrix( p3DView );

      glColor3f( 1.0f, 1.0f, 0.0f );
      glBegin( GL_LINE_LOOP );
         glVertex3fv( &vPos.x );
         glVertex3f( vPos.x + v2Size.x, vPos.y, vPos.z );
         glVertex3f( vPos.x + v2Size.x, vPos.y + v2Size.y, vPos.z );
         glVertex3f( vPos.x, vPos.y + v2Size.y, vPos.z );
      glEnd();
   glPopMatrix();
}
