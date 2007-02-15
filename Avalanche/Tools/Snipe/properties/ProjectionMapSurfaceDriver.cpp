//

#include "stdafx.h"
#include "ProjectionMapSurfaceDriver.h"
#include "Poly.h"
#include "TessPoly.h"
#include "GUI/GL2DView.h"
#include "GUI/GLViewControl.h"
#include "GUI/BaseDoc.h"
#include "GUI/Snipe.h"
#include "resource.h"

IMPLEMENT_SNIPEOBJECT_CREATE( ProjectionMapSurfaceDriver )

static TypeProperty::Info::Node f_projectioninfos[] =
{
   TypeProperty::Info::Node( "Planar", "Planar" ),
   TypeProperty::Info::Node( "Cylindrical", "Cylindrical" ),
   TypeProperty::Info::Node( "Spherical", "Spherical" ),
};

TransformProperty::Info ProjectionMapSurfaceDriver::m_transformInfo( "Transform", "Transform" );
TypeProperty::Info      ProjectionMapSurfaceDriver::m_nProjectionMethodInfo( "Projection", "Projection", f_projectioninfos, sizeof(f_projectioninfos)/sizeof(TypeProperty::Info::Node), ProjectionMapSurfaceDriver::PM_PLANAR );

ProjectionMapSurfaceDriver::ProjectionMapSurfaceDriver()
{
   m_nProjectionMethod.Init( &m_nProjectionMethodInfo, this );
   m_transform.Init( &m_transformInfo, &m_nProjectionMethod );
}

void ProjectionMapSurfaceDriver::Bind()
{
   MapSurfaceDriver::Bind();

   static Matrix44 offset = Translatef( -0.5f, -0.5f, 0.0f );
   Matrix44 matrix = offset * m_transform.GetInverseMatrix();

   glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
   glTexGenfv( GL_S, GL_OBJECT_PLANE, &matrix.GetData()[0] );
   glEnable( GL_TEXTURE_GEN_S );

   glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
   glTexGenfv( GL_T, GL_OBJECT_PLANE, &matrix.GetData()[4] );
   glEnable( GL_TEXTURE_GEN_T );

   glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
   glTexGenfv( GL_R, GL_OBJECT_PLANE, &matrix.GetData()[8] );
   glEnable( GL_TEXTURE_GEN_R );

   glTexGeni( GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
   glTexGenfv( GL_Q, GL_OBJECT_PLANE, &matrix.GetData()[12] );
   glEnable( GL_TEXTURE_GEN_Q );
}

void ProjectionMapSurfaceDriver::UnBind()
{
   glDisable( GL_TEXTURE_GEN_S );
   glDisable( GL_TEXTURE_GEN_T );
   glDisable( GL_TEXTURE_GEN_R );
   glDisable( GL_TEXTURE_GEN_Q );

   MapSurfaceDriver::UnBind();
}

Vector2 ProjectionMapSurfaceDriver::GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, Poly *pPoly, LPARAM lParam )
{
   return 0;
   Vertex *pVertex = pPoly->GetVertexAt( nIndex );
   Vector vPos = pVertex->GetPos();
   vPos *= m_transform.GetInverseMatrix();

   Vector2 uv;
   switch (m_nProjectionMethod)
   {
      case PM_PLANAR:
         uv.Set( vPos.x, vPos.y );
         break;
      case PM_CYLINDRICAL:
         uv.x = ((float)atan2(-vPos.x, vPos.z) + (float)A_PI) / (2.0f * (float)A_PI);
         uv.y = vPos.y;
         break;
      case PM_SPHERICAL:
         uv.x = ((float)atan2( -vPos.x, vPos.z ) + (float)A_PI) / (2.0f * (float)A_PI);
         uv.y = (float)acos( vPos.y ) / (float)A_PI;
         break;
   }

   //TransformUV( uv );

   return uv;
}

Vector2 ProjectionMapSurfaceDriver::GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, TessPoly *pPoly, LPARAM lParam )
{
   return 0;
   Vertex *pVertex = pPoly->GetVertexAt( nIndex );
   Vector vPos = pVertex->GetPos();
   vPos *= m_transform.GetInverseMatrix();

   Vector2 uv;
   switch (m_nProjectionMethod)
   {
      case PM_PLANAR:
         uv.Set( vPos.x, vPos.y );
         break;
      case PM_CYLINDRICAL:
         uv.x = ((float)atan2(-vPos.x, vPos.z) + (float)A_PI) / (2.0f * (float)A_PI);
         uv.y = vPos.y;
         break;
      case PM_SPHERICAL:
         uv.x = ((float)atan2( -vPos.x, vPos.z ) + (float)A_PI) / (2.0f * (float)A_PI);
         uv.y = (float)acos( vPos.y ) / (float)A_PI;
         break;
   }

   //TransformUV( uv );

   return uv;
}

void ProjectionMapViewControlChangedCallback( CGLViewControl *pViewControl, void *pData )
{
   ProjectionMapSurfaceDriver *pProjectionMapSurfaceDriver = (ProjectionMapSurfaceDriver *)pData;
   //pProjectionMapSurfaceDriver->OnViewControlChanged( pViewControl );
   CGL2DScaleViewControl *pScaleViewControl = (CGL2DScaleViewControl *)pViewControl;
   pProjectionMapSurfaceDriver->m_transform.m_vScale.StoreValue( Vector(pScaleViewControl->m_v2Size.x, pScaleViewControl->m_v2Size.y, pProjectionMapSurfaceDriver->m_transform.m_vScale.GetValue().z) );
}

void ProjectionMapSurfaceDriver::OnFocusChanged( BOOL bFocus )
{
   MapSurfaceDriver::OnFocusChanged( bFocus );

   for (POSITION docpos=GetApp().GetDocList().GetHeadPosition(); docpos; )
   {
      CBaseDoc *pDoc = GetApp().GetDocList().GetNext( docpos );

      if (pDoc->UsesObject( this ))
      {
         for (POSITION viewpos=pDoc->m_viewlist.GetHeadPosition(); viewpos; )
         {
            CGLBaseView *pView = pDoc->m_viewlist.GetNext( viewpos );
            if (bFocus)
               SNEW CGLTransformPropertyViewControl( pView, &m_transform );
            else
            {
               POSITION controlpos=pView->GetViewControlList().GetHeadPosition();
               while (controlpos)
               {
                  for (controlpos=pView->GetViewControlList().GetHeadPosition(); controlpos; )
                  {
                     CGLViewControl *pControl = pView->GetViewControlList().GetNext( controlpos );
                     if (pControl->IsFor( &m_transform ))
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

void ProjectionMapSurfaceDriver::DrawFocused( CGLBaseView *pView )
{
   glDepthMask( FALSE );
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   GLfloat glMatrix[16];
   Matrix44 matrix = m_transform.GetMatrix();
   float fLength = 20.0f;
   matrix *= Translatef( 0, 0, fLength );
   matrix.GetFloatMatrix( glMatrix, TRUE );
   glMultMatrixf( glMatrix );

   glLineWidth( 1.4f );
   glDisable( GL_LIGHTING );
   glColor4f( 1.0f, 1.0f, 0.0f, 0.7f );
   fLength *= 2.0f;
   glBegin( GL_LINES );
      glVertex3f( 0.5f, 0.5f, -fLength );
      glVertex3f( 0.5f, 0.5f,  0 );
      glVertex3f( 0.5f, -0.5f, -fLength );
      glVertex3f( 0.5f, -0.5f,  0 );
      glVertex3f( -0.5f, -0.5f, -fLength );
      glVertex3f( -0.5f, -0.5f,  0 );
      glVertex3f( -0.5f, 0.5f, -fLength );
      glVertex3f( -0.5f, 0.5f,  0 );
   glEnd();
   glBegin( GL_LINE_LOOP );
      glVertex3f( 0.5f, 0.5f, -fLength );
      glVertex3f( 0.5f, -0.5f, -fLength );
      glVertex3f( -0.5f, -0.5f, -fLength );
      glVertex3f( -0.5f, 0.5f, -fLength );
   glEnd();

   Image *pImage = GetImage();
   if (pImage)
   {
      m_pImage.Bind();
         pImage->Draw( pView, Vector(-0.5f, -0.5f, 0.0f), 1.0f, 0.4f );
      m_pImage.UnBind();
   }

   glColor4f( 1.0f, 1.0f, 0.0f, 0.7f );
   glBegin( GL_LINE_LOOP );
      glVertex3f( 0.5f, 0.5f, 0 );
      glVertex3f( 0.5f, -0.5f, 0 );
      glVertex3f( -0.5f, -0.5f, 0 );
      glVertex3f( -0.5f, 0.5f, 0 );
   glEnd();

   {
      if (m_glImageListPivot.IsEmpty())
         m_glImageListPivot.Create( IDB_TARGET );
      GLDrawPrepper prepper( pView ); // push 2d projection
      int nWidth = m_glImageListPivot.GetWidthEach();
      int nHeight = m_glImageListPivot.GetHeight();
      Vector vPivot( 0.0f, 0.0f, -fLength/2.0f );
      vPivot *= matrix;
      pView->WorldToScreen( vPivot );
      m_glImageListPivot.Draw( vPivot.x-(nWidth/2), vPivot.y-(nHeight/2), 0, .5f );
   }

//   static CGLTransformPropertyViewControl *pScaleViewControl;
//   if (!pView->GetNumViewControls())
//   {
//      //CGLTranslateViewControl *pViewControl = SNEW CGLTranslateViewControl( pView );
//      //pView->AddViewControl( pViewControl );
//
//      pScaleViewControl = SNEW CGLTransformPropertyViewControl( pView, &m_transform );
//      //pScaleViewControl->SetCallback( ProjectionMapViewControlChangedCallback, this );
//
//      //CGLSliderViewControl *pSliderViewControl = SNEW CGLSliderViewControl( pView, Vector(-1,0,0), Vector(1,1,1), 0.0f, 5.0f, 2.0f, TRUE );
//      //pView->AddViewControl( pSliderViewControl );
//   }
//   //else
//   //   pScaleViewControl->SetMatrix( matrix );

   //m_transform.m_vScale.DrawManipulator( pView, matrix, ScaleProperty::AXIS_X|ScaleProperty::AXIS_Y );
   //m_transform.m_vRotate.DrawManipulator( pView, matrix, ScaleProperty::AXIS_X|ScaleProperty::AXIS_Y );

   glPopMatrix();
   glDepthMask( TRUE );
}
