//

#include "stdafx.h"
#include "ScaleManipulator.h"
#include "CommandUIAccel.h"
#include "HCommandPluginManager.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCBaseDoc.h"
#include "../Resource.h"

// Plugin //////////////////////////////////////////
ManipulatorInstance *ScaleManipulatorPlugin::CreateInstance()
{
   return SNEW ScaleManipulator( this );
}

const char *ScaleManipulatorPlugin::GetTitleName() const { return "Scale"; }

UINT ScaleManipulatorPlugin::GetBitmapID() const { return IDB_SCALE; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'R')
};

void ScaleManipulatorPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

// Instance //////////////////////////////////////////////////////////
ScaleManipulator::ScaleManipulator( ScaleManipulatorPlugin *pPluginHelper) :
 TransformManipulator( pPluginHelper )
{
   SetMode( MODE_SCALE_ONLY );

   m_pTranslate->SetVisible( FALSE );
   m_pRotate->SetVisible( FALSE );
}

ScaleManipulator::~ScaleManipulator()
{
}

#if 0
void ScaleManipulator::Draw( HCGLBaseView *pView, float fAlpha/*=1.0f*/, BOOL bOpaqueItemsOnly/*=FALSE*/ )
{
   glPushAttrib( GL_LIGHTING_BIT | GL_ENABLE_BIT );
   GLfloat param[4];
   param[3] = fAlpha;

   Vector center;
   m_curmatrix.GetTranslation( center );
   Vector startcenter;
   m_startmatrix.GetTranslation( startcenter );

   if (m_bManipulating && !bOpaqueItemsOnly)
   {
      glDisable( GL_LIGHTING );
      glColor4f( 1.0f, 1.0f, 0.0f, fAlpha );
      Vector vDelta = startcenter - center;
      glBegin( GL_LINES );
         glVertex3f( 0.0f, 0.0f, 0.0f );
         glVertex3fv( &vDelta.x );
      glEnd();

      glEnable( GL_LIGHTING );

      glPushMatrix();
      glTranslatef( vDelta.x, vDelta.y, vDelta.z );

      // Scale start knob to n screen pixels big
      Vector viewcenter = startcenter;
      pView->WorldToView( viewcenter );
      float fPixelSize = pView->GetScreenPixelSize( fabs(viewcenter.z) );
      float fManipulatorSize = fPixelSize * m_pScreenSize->GetValue();
      fManipulatorSize *= 0.5f;

      glScalef( fManipulatorSize, fManipulatorSize, fManipulatorSize );

      param[0] = 1.0f, param[1] = 1.0f, param[2] = 0.0f;
      glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, param );
      glCallList( GetScaleManipulatorPlugin()->m_nCallListScaleKnob );
   
      glPopMatrix();
   }

// Scale it to n screen pixels big
   glPushMatrix();
   Vector viewcenter = center;
   pView->WorldToView( viewcenter );
   float fPixelSize = pView->GetScreenPixelSize( fabs(viewcenter.z) );
   float fManipulatorSize = fPixelSize * m_pScreenSize->GetValue();
   glScalef( fManipulatorSize, fManipulatorSize, fManipulatorSize );
/////////

   BOOL b3D = pView->IsKindOf( SVT_CGL3DView ) != NULL;
   if (b3D)
      glEnable( GL_LIGHTING );

   DrawElement( AXIS_SCALE_ALL, RGBAFloat(0,1,1,fAlpha), GetScaleManipulatorPlugin()->m_nCallListScaleKnob );

   if (b3D)
   {
      DrawPlane( AXIS_SCALE_XY, RGBAFloat(1,0,0,fAlpha*0.4f), RGBAFloat(0,1,0,fAlpha*0.4f) );
      glTranslatef( 0, 0, m_vScale.z );
      if (DrawElement( AXIS_SCALE_Z, RGBAFloat(0,0,1,fAlpha), GetScaleManipulatorPlugin()->m_nCallListScaleKnob ) && !m_bDrawPick)
         DrawScaleAxis( m_vScale.z );
      glTranslatef( 0, 0, -m_vScale.z );
   }

   glRotatef( -90, 0, 1, 0 );
   glRotatef( -90, 1, 0, 0 );
   if (b3D)
      DrawPlane( AXIS_SCALE_XZ, RGBAFloat(0,0,1,fAlpha*0.4f), RGBAFloat(1,0,0,fAlpha*0.4f) );
   glTranslatef( 0, 0, m_vScale.y );
   if (DrawElement( AXIS_SCALE_Y, RGBAFloat(0,1,0,fAlpha), GetScaleManipulatorPlugin()->m_nCallListScaleKnob ) && !m_bDrawPick)
      DrawScaleAxis( m_vScale.y );
   glTranslatef( 0, 0, -m_vScale.y );
   glRotatef( 90, 1, 0, 0 );
   glRotatef( 90, 0, 1, 0 );

   glRotatef( 90, 1, 0, 0 );
   glRotatef( 90, 0, 1, 0 );
   if (b3D)
      DrawPlane( AXIS_SCALE_YZ, RGBAFloat(0,1,0,fAlpha*0.4f), RGBAFloat(0,0,1,fAlpha*0.4f) );
   glTranslatef( 0, 0, m_vScale.x );
   if (DrawElement( AXIS_SCALE_X, RGBAFloat(1,0,0,fAlpha), GetScaleManipulatorPlugin()->m_nCallListScaleKnob ) && !m_bDrawPick)
      DrawScaleAxis( m_vScale.x );
   glTranslatef( 0, 0, -m_vScale.x );
   glRotatef( -90, 0, 1, 0 );
   glRotatef( -90, 1, 0, 0 );

   glPopMatrix();

   if (m_bManipulating && bOpaqueItemsOnly)
   {
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

      Vector viewcenter = startcenter;
      pView->WorldToView( viewcenter );

      String strValue;
      int lines = -1;
      if ((m_nSelectedElement & AXIS_SCALE_ALL) == AXIS_SCALE_ALL)
         strValue = String( m_vScale.x * 100.0f ) + "%\n";
      else
      {
         if (m_nSelectedElement & AXIS_SCALE_X)
         {
            strValue += "X: " + String( m_vScale.x * 100.0f ) + "%\n";
            lines++;
         }
         if (m_nSelectedElement & AXIS_SCALE_Y)
         {
            strValue += "Y: " + String( m_vScale.y * 100.0f ) + "%\n";
            lines++;
         }
         if (m_nSelectedElement & AXIS_SCALE_Z)
         {
            strValue += "Z: " + String( m_vScale.z * 100.0f ) + "%\n";
            lines++;
         }
      }
      pView->Project( viewcenter );
      glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
      pView->DrawText( viewcenter.x + 13, viewcenter.y + 5 - lines * 7, strValue );
      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
      pView->DrawText( viewcenter.x + 12, viewcenter.y + 4 - lines * 7, strValue );
   }

   glPopAttrib();
}

void ScaleManipulator::OnStartManipulating( CPoint point, HCGLBaseView *pView ) 
{ 
   m_bManipulating = TRUE;
   m_startmatrix = m_curmatrix;
//      Vector zero(0);
//      pView->WorldToScreen( zero );
//      pView->ScreenToWorld( zero );

   Vector center;
   m_curmatrix.GetTranslation( center );
   Vector screencenter = center;
   pView->WorldToScreen( screencenter );
   f_fDistance = screencenter.z;

   f_vStart.Set( (float)point.x, (float)point.y, f_fDistance );
   pView->ScreenToWorld( f_vStart );

   m_vScale.Set( 1.0f );
   Scale( pView, center, m_vScale, TRUE );
}

void ScaleManipulator::OnManipulating( CPoint point, HCGLBaseView *pView ) 
{ 
   Vector vMouse( (float)point.x, (float)point.y, f_fDistance );
   pView->ScreenToWorld( vMouse );

   Vector center;
   m_curmatrix.GetTranslation( center );
   m_vScale = Vector(1.0f) + vMouse - f_vStart;
   Scale( pView, center, m_vScale, TRUE );
}

void ScaleManipulator::OnEndManipulating( CPoint point, HCGLBaseView *pView ) 
{ 
   Vector vMouse( (float)point.x, (float)point.y, f_fDistance );
   pView->ScreenToWorld( vMouse );

   Vector center;
   m_curmatrix.GetTranslation( center );
   m_vScale = Vector(1.0f) + vMouse - f_vStart;
   Scale( pView, center, m_vScale, FALSE );
   m_vScale.Set(1.0f);
}
#endif
