//

#include "stdafx.h"
#include "RotateManipulator.h"
#include "CommandUIAccel.h"
#include "HCommandPluginManager.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCBaseDoc.h"
#include "Rotate.h"
#include "../Resource.h"

// Plugin //////////////////////////////////////////
ManipulatorInstance *RotateManipulatorPlugin::CreateInstance()
{
   return SNEW RotateManipulator( this );
}

const char *RotateManipulatorPlugin::GetTitleName() const { return "Rotate"; }

UINT RotateManipulatorPlugin::GetBitmapID() const { return IDB_ROTATE; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'E')
};

void RotateManipulatorPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

// Instance //////////////////////////////////////////////////////////

RotateManipulator::RotateManipulator( RotateManipulatorPlugin *pPluginHelper) :
 TransformManipulator( pPluginHelper )
{
   SetMode( MODE_ROTATE_ONLY );

   m_pTranslate->SetVisible( FALSE );
   m_pScale->SetVisible( FALSE );
}

RotateManipulator::~RotateManipulator()
{
}
#if 0
void RotateManipulator::Draw( HCGLBaseView *pView, float fAlpha/*=1.0f*/, BOOL bOpaqueItemsOnly/*=FALSE*/ )
{
   glPushAttrib( GL_LIGHTING_BIT | GL_ENABLE_BIT );

// Scale it to n screen pixels big
   glPushMatrix();
   Vector center;
   m_curmatrix.GetTranslation( center );
   Vector viewcenter = center;
   pView->WorldToView( viewcenter );
   float fPixelSize = pView->GetScreenPixelSize( fabs(viewcenter.z) );
   float fManipulatorSize = fPixelSize * m_pScreenSize->GetValue();
   glScalef( fManipulatorSize, fManipulatorSize, fManipulatorSize );
/////////

   BOOL b3D = pView->IsKindOf( SVT_CGL3DView ) != NULL;
   if (b3D)
   {
      glEnable( GL_LIGHTING );

      if (!bOpaqueItemsOnly)
         DrawElement( AXIS_ROTATE_FREE, RGBAFloat(1,1,1,fAlpha*0.2f), GetRotateManipulatorPlugin()->m_nCallListRotateAll );

      if (m_bManipulating && m_nSelectedElement != AXIS_ROTATE_FREE)
         glDisable( GL_CULL_FACE );

      DrawElement( AXIS_ROTATE_Z, RGBAFloat(0,0,1,fAlpha), GetRotateManipulatorPlugin()->m_nCallListRotateZ );

      glRotatef( -90, 1, 0, 0 );
         DrawElement( AXIS_ROTATE_Y, RGBAFloat(0,1,0,fAlpha), GetRotateManipulatorPlugin()->m_nCallListRotateZ );
      glRotatef( 90, 1, 0, 0 );

      glRotatef( 90, 0, 1, 0 );
         DrawElement( AXIS_ROTATE_X, RGBAFloat(1,0,0,fAlpha), GetRotateManipulatorPlugin()->m_nCallListRotateZ );
      glRotatef( -90, 0, 1, 0 );
   }

   if (!m_bManipulating || m_nSelectedElement == AXIS_ROTATE_SCREEN)
   {
      if (b3D)
      {
         GLdouble glViewMatrix[16];
         glGetDoublev( GL_MODELVIEW_MATRIX, glViewMatrix );
         Matrix44 matrix( glViewMatrix, true );
         Vector translate, scale, shear, rotate;
         matrix.Decompose( translate, scale, shear, rotate );
         glRotatef( -rotate.x, 1, 0, 0 );
         glRotatef( -rotate.y, 0, 1, 0 );
         glRotatef( -rotate.z, 0, 0, 1 );
      }
      else
         glDisable( GL_CULL_FACE );

      DrawElement( AXIS_ROTATE_SCREEN, RGBAFloat(0,1,1,fAlpha), GetRotateManipulatorPlugin()->m_nCallListRotateScreen );
   }

   glPopMatrix();

   glDisable( GL_LIGHTING );

   glColor4f( 1.0f, 1.0f, 0.0f, fAlpha );
   glBegin(GL_POINTS);
      glVertex3f(0,0,0);
   glEnd();

   if (m_bManipulating && m_nSelectedElement!=AXIS_ROTATE_FREE)
   {
      glLineWidth( 2.0f );
      glBegin( GL_LINES );
         glVertex3f( 0,0,0 );
         glVertex3fv( &m_vStart.x );
      glEnd();

      glRotatef( -m_fAngle, m_vAxis.x, m_vAxis.y, m_vAxis.z );
         glBegin( GL_LINES );
            glVertex3f( 0,0,0 );
            glVertex3fv( &m_vStart.x );
         glEnd();
      glRotatef( m_fAngle, m_vAxis.x, m_vAxis.y, m_vAxis.z );

      if (bOpaqueItemsOnly && fAlpha==1.0f)
      {
         Vector vScreenPos = center + m_vCurrent;
         pView->WorldToScreen( vScreenPos );
         String strValue;
         strValue = m_fAngle;
         glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
         pView->DrawText( vScreenPos.x + 13, vScreenPos.y + 5, strValue );
         glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
         pView->DrawText( vScreenPos.x + 12, vScreenPos.y + 4, strValue );
      }
   }

   glPopAttrib();
}
void RotateManipulator::Rotate( HCGLBaseView *pView, const Vector &vAbout, const Vector &vAxis, float fAngle, BOOL bTemp )
{
   String strParameters;

   strParameters += (String)"-angle " + fAngle;
   strParameters += (String)" -about " + vAbout;
   strParameters += (String)" -axis " + vAxis;

   DoCommand( GetRotateManipulatorPlugin()->m_pRotateCommandPlugin, strParameters, bTemp );

   m_curmatrix = m_startmatrix;
   if (bTemp)
      m_curmatrix *= RotateV( fAngle, vAxis );
}

void RotateManipulator::Rotate( HCGLBaseView *pView, const Vector &vAbout, const Matrix44 &matrix, BOOL bTemp )
{
   RotateEuler euler = RotateEuler( matrix );

   String strParameters;
   strParameters += (String)"-about " + vAbout;
   strParameters += (String)" -euler " + euler.x + " " + euler.y + " " + euler.z;

   DoCommand( GetRotateManipulatorPlugin()->m_pRotateCommandPlugin, strParameters, bTemp );

   m_curmatrix = m_startmatrix;
   if (bTemp)
      m_curmatrix *= matrix;
}

static float  f_fStartAngle;

void RotateManipulator::OnStartManipulating( CPoint point, HCGLBaseView *pView )
{
   ComputeMatrix(pView);

   m_fAngle = 0.0f;

   Matrix44 matrix33;
   m_curmatrix.GetMatrix33( matrix33 );

   Vector center;
   m_curmatrix.GetTranslation( center );
   Vector viewcenter = center;
   pView->WorldToView( viewcenter );
   float fPixelSize = pView->GetScreenPixelSize( fabs(viewcenter.z) );
   float fManipulatorSize = fPixelSize * m_pScreenSize->GetValue();

   m_bManipulating = TRUE;

   switch (m_nSelectedElement) {
      case AXIS_ROTATE_X:
         m_vAxis.Set( 1.0f, 0.0f, 0.0f );
         m_mouseoncircle.Init( pView, center, matrix33*m_vAxis, fManipulatorSize );
         m_vStart = m_mouseoncircle.GetWorldPos( point );
         m_vStart -= center;
         f_fStartAngle = atan2( m_vStart.z, m_vStart.y ) * RADIANS;
         break;
      case AXIS_ROTATE_Y:
         m_vAxis.Set( 0.0f, 1.0f, 0.0f );
         m_mouseoncircle.Init( pView, center, matrix33*m_vAxis, fManipulatorSize );
         m_vStart = m_mouseoncircle.GetWorldPos( point );
         m_vStart -= center;
         f_fStartAngle = atan2( m_vStart.x, m_vStart.z ) * RADIANS;
         break;
      case AXIS_ROTATE_Z:
         m_vAxis.Set( 0.0f, 0.0f, 1.0f );
         m_mouseoncircle.Init( pView, center, matrix33*m_vAxis, fManipulatorSize );
         m_vStart = m_mouseoncircle.GetWorldPos( point );
         m_vStart -= center;
         f_fStartAngle = atan2( m_vStart.y, m_vStart.x ) * RADIANS;
         break;
      case AXIS_ROTATE_SCREEN:
         m_vAxis.Set( 0.0f, 0.0f, -1.0f );
         pView->ViewToWorld( m_vAxis, TRUE );
         m_mouseoncircle.Init( pView, center, m_vAxis, fManipulatorSize );
         m_vStart = m_mouseoncircle.GetWorldPos( point );
         m_vStart -= center;
         TransformVectorNoTranslate( *pView->GetWorldToViewMatrix(), m_vStart );
         f_fStartAngle = atan2( m_vStart.y, m_vStart.x ) * RADIANS;
         break;
      case AXIS_ROTATE_FREE:
         m_mouseonsphere.Init( pView, center, fManipulatorSize );
         m_vStart = m_mouseonsphere.GetWorldPos( point );
         m_vStart -= center;
         break;
   }

   m_vCurrent = m_vStart;
   Rotate( pView, center, m_vAxis, 0.0f, TRUE );
}

void RotateManipulator::OnManipulating( CPoint point, HCGLBaseView *pView )
{ 
   Vector center;
   m_curmatrix.GetTranslation( center );

   Vector vAngle;
   switch (m_nSelectedElement) {
      case AXIS_ROTATE_X: {
         m_vCurrent = m_mouseoncircle.GetWorldPos( point );
         m_vCurrent -= center;
         m_fAngle = vAngle.x = atan2( m_vCurrent.z, m_vCurrent.y ) * RADIANS - f_fStartAngle;
         Rotate( pView, center, m_vAxis, m_fAngle, TRUE );
         break;
      }
      case AXIS_ROTATE_Y: {
         m_vCurrent = m_mouseoncircle.GetWorldPos( point );
         m_vCurrent -= center;
         m_fAngle = vAngle.y = atan2( m_vCurrent.x, m_vCurrent.z ) * RADIANS - f_fStartAngle;
         Rotate( pView, center, m_vAxis, m_fAngle, TRUE );
         break;
      }
      case AXIS_ROTATE_Z: {
         m_vCurrent = m_mouseoncircle.GetWorldPos( point );
         m_vCurrent -= center;
         m_fAngle = vAngle.z = atan2( m_vCurrent.y, m_vCurrent.x ) * RADIANS - f_fStartAngle;
         Rotate( pView, center, m_vAxis, m_fAngle, TRUE );
         break;
      }
      case AXIS_ROTATE_SCREEN: {
         m_vCurrent = m_mouseoncircle.GetWorldPos( point );
         m_vCurrent -= center;
         TransformVectorNoTranslate( *pView->GetWorldToViewMatrix(), m_vCurrent );
         float fCurAngle = atan2( m_vCurrent.y, m_vCurrent.x ) * RADIANS;
         m_fAngle = f_fStartAngle - fCurAngle;
         Rotate( pView, center, m_vAxis, m_fAngle, TRUE );
         break;
      }
      case AXIS_ROTATE_FREE: {
         m_vCurrent = m_mouseonsphere.GetWorldPos( point );
         m_vCurrent -= center;
         Matrix44 rotatematrix = Rotate2V( m_vStart, m_vCurrent );
         Rotate( pView, center, rotatematrix, TRUE );
         break;
      }
   }
}

void RotateManipulator::OnEndManipulating( CPoint point, HCGLBaseView *pView )
{ 
   m_fAngle = 0.0f;
   m_curmatrix = m_startmatrix;
}
#endif
