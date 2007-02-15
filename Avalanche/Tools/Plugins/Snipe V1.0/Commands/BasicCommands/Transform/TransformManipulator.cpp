//

#include "stdafx.h"
#include "TransformManipulator.h"
#include "CommandUIAccel.h"
#include "HCommandPluginManager.h"
#include "BoundingBox.h"
#include "HSelectionList.h"
#include "HHitArray.h"
#include "HMainInterface.h"
#include "HCGLBaseView.h"
#include "HCBaseDoc.h"
#include "../Resource.h"

#define SCREENTRANSLATESIZE 0.06f

// Plugin //////////////////////////////////////////

ManipulatorInstance *TransformManipulatorPlugin::CreateInstance()
{
   return SNEW TransformManipulator( this );
}

const char *TransformManipulatorPlugin::GetTitleName() const { return "Transform"; }

UINT TransformManipulatorPlugin::GetBitmapID() const { return IDB_SCALE; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'T')
};

void TransformManipulatorPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void TransformManipulatorPlugin::Init( HCommandPluginManager *pCommandManager )
{
   // Collect dependencies here
   m_pTranslateCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Translate");
   m_pRotateCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Rotate");
   m_pScaleCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Scale");
   if (!m_pTranslateCommandPlugin || !m_pRotateCommandPlugin || !m_pScaleCommandPlugin)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Transform ).");

   HCMainFrame *pMainFrame = GetHCMainFrame();
   m_nCallListTranslsateZArrow = pMainFrame->CreateCallListFromGeoResource(IDR_TRANSLATEMANIPULATORZ_GEO);
   m_nCallListRotateAll = pMainFrame->CreateCallListFromGeoResource(IDR_ROTATEMANIPULATORALL_GEO);
   m_nCallListRotateZ = pMainFrame->CreateCallListFromGeoResource(IDR_ROTATEMANIPULATORZ_GEO);
   m_nCallListRotateScreen = pMainFrame->CreateCallListFromGeoResource(IDR_ROTATEMANIPULATORSCREEN_GEO);
   m_nCallListScaleKnob = pMainFrame->CreateCallListFromGeoResource(IDR_SCALEMANIPULATORKNOB_GEO);
   m_nCallListPivotRotate = pMainFrame->CreateCallListFromGeoResource(IDR_PIVOTMANIPULATORROTATE_GEO);
   if (!m_nCallListTranslsateZArrow || !m_nCallListRotateAll || !m_nCallListRotateZ || !m_nCallListRotateScreen || !m_nCallListScaleKnob || !m_nCallListPivotRotate)
      THROW_FATAL_ERROR("Failed loading manipulator geometry ( SnipeBasicCommand : Transform ).");

   SelectManipulatorPlugin::Init( pCommandManager );
}

void TransformManipulatorPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
   if (GetClassName() == GetThisClassName())
      pToolBar->AddButton(NULL);//Separator
}

// Instance

TransformManipulator::TransformManipulator( TransformManipulatorPlugin *pPluginHelper ) :
 SelectManipulator( pPluginHelper )
{
   m_nSelectedElement = 0;
   m_nMouseOverElement = 0;
   m_bManipulating = FALSE;
   m_bDrawPick = FALSE;
   m_bShowManipulator = TRUE;

   m_pScreenSizeInfo = HFloatProperty::HInfo::New( "Screen Size", "ScreenSize", 80.0f, 30.0f, 300.0f );
   m_pScreenSize = HFloatProperty::New( m_pScreenSizeInfo, m_pOptions );

   m_pSpaceNodes = HTypeProperty::HInfo::AllocNodes( SPACE_NUMSPACES );
   HTypeProperty::HInfo::GetNodeAt( m_pSpaceNodes, SPACE_LOCAL )->SetName( "Pivot" );
   HTypeProperty::HInfo::GetNodeAt( m_pSpaceNodes, SPACE_LOCAL )->SetMatchName( "Pivot" );
   HTypeProperty::HInfo::GetNodeAt( m_pSpaceNodes, SPACE_WORLD )->SetName( "World" );
   HTypeProperty::HInfo::GetNodeAt( m_pSpaceNodes, SPACE_WORLD )->SetMatchName( "World" );
   m_pSpaceInfo = HTypeProperty::HInfo::New( "Space", "Space", m_pSpaceNodes, SPACE_NUMSPACES, SPACE_LOCAL );
   m_pSpaceInfo->SetDisplayAsRadios( TRUE );
   m_pSpace = HTypeProperty::New( m_pSpaceInfo, NULL );
   m_pPropertyArray->Add( m_pSpace );

   m_pPivotModeInfo = HBoolProperty::HInfo::New( "Pivot", "Pivot", FALSE );
   m_pPivotModeInfo->SetAutoHideChildren( FALSE );
   m_pPivotMode = HBoolProperty::New( m_pPivotModeInfo, NULL ); 
   m_pPropertyArray->Add( m_pPivotMode );

   m_pPivotPosInfo = HVectorProperty::HInfo::New("Position", "Position", Vector(0), Vector(-MAX_FLOAT), Vector(MAX_FLOAT));
   m_pPivotPos = HVectorProperty::New( m_pPivotPosInfo, m_pPivotMode );
   m_pPivotRotateInfo = HRotateProperty::HInfo::New("Orientation", "Orientation", Vector(0), Vector(-MAX_FLOAT), Vector(MAX_FLOAT));
   m_pPivotRotate = HRotateProperty::New( m_pPivotRotateInfo, m_pPivotMode );

   m_pTranslateInfo = HVectorProperty::HInfo::New("Offset", "Offset", Vector(0), Vector(-MAX_FLOAT), Vector(MAX_FLOAT));
   m_pTranslate = HVectorProperty::New( m_pTranslateInfo, NULL );
   m_pPropertyArray->Add( m_pTranslate );

   m_pRotateInfo = HRotateProperty::HInfo::New("Rotate", "Rotate", Vector(0), Vector(-MAX_FLOAT), Vector(MAX_FLOAT));
   m_pRotate = HRotateProperty::New( m_pRotateInfo, NULL );
   m_pPropertyArray->Add( m_pRotate );
   m_fAngle = 0.0f;

   m_pScaleInfo = HVectorProperty::HInfo::New("Scale", "Scale", Vector(1.0f), Vector(-MAX_FLOAT), Vector(MAX_FLOAT));
   m_pScale = HVectorProperty::New( m_pScaleInfo, NULL );
   m_pPropertyArray->Add( m_pScale );

   ComputePivot();
   ComputeMatrix();

   AddCallback( NOTIFYOBJ_SELECTIONLISTCHANGED );

   SetMode( MODE_TRANSLATE );
}

TransformManipulator::~TransformManipulator()
{
   RemoveCallback();

   delete m_pScreenSize;
   delete m_pScreenSizeInfo;

   delete m_pSpace;
   delete m_pSpaceInfo;
   delete [] m_pSpaceNodes;

   delete m_pPivotMode;
   delete m_pPivotModeInfo;

   delete m_pPivotPos;
   delete m_pPivotPosInfo;

   delete m_pPivotRotate;
   delete m_pPivotRotateInfo;

   delete m_pTranslate;
   delete m_pTranslateInfo;

   delete m_pRotate;
   delete m_pRotateInfo;

   delete m_pScale;
   delete m_pScaleInfo;
}

void TransformManipulator::ComputePivot()
{
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   //if (pSelectionList->GetSize() == 1)
   //{
   //   HSnipeObject *pObject = pSelectionList->GetHead();
   //   m_startmatrix = pObject->GetObjectToWorldMatrix();
   //}
   //else
   //{
      BoundingBox bounds = pSelectionList->GetBoundingBox();
      m_pPivotPos->StoreValue( bounds.CalculateCenterPoint());
   //}
}

void TransformManipulator::ComputeMatrix()
{
   m_curmatrix.Identity();
   m_curmatrix.SetTranslation( m_pPivotPos->GetValue() );
   //RotateEuler euler( m_pPivotRotate->GetValue().x, m_pPivotRotate->GetValue().y, m_pPivotRotate->GetValue().z );
   //m_curmatrix *= euler;
}

void TransformManipulator::SetMode( Mode nMode )
{
   m_nMode = nMode;
   m_nVisibleElements = AXIS_NONE;

   if (m_pPivotMode->GetValue())
   {
      m_nVisibleElements |= AXIS_PIVOT_X | AXIS_PIVOT_Y | AXIS_PIVOT_Z | AXIS_PIVOT_SCREEN | AXIS_PIVOT_ROTATEX | AXIS_PIVOT_ROTATEY | AXIS_PIVOT_ROTATEZ | AXIS_PIVOT_ROTATEFREE;
      return;
   }

   switch (m_nMode)
   {
      case MODE_TRANSLATE:
         m_nVisibleElements |= AXIS_ROTATE_SCREEN;
         m_nVisibleElements |= AXIS_SCALE_X | AXIS_SCALE_Y | AXIS_SCALE_Z;
         // no break;
      case MODE_TRANSLATE_ONLY:
         m_nVisibleElements |= AXIS_TRANSLATE_X | AXIS_TRANSLATE_Y | AXIS_TRANSLATE_Z | AXIS_TRANSLATE_SCREEN;
         //if (m_nMode == MODE_TRANSLATE_ONLY)
            m_nVisibleElements |= AXIS_TRANSLATE_XY | AXIS_TRANSLATE_YZ | AXIS_TRANSLATE_XZ;
         break;

      case MODE_ROTATE:
         m_nVisibleElements |= AXIS_TRANSLATE_X | AXIS_TRANSLATE_Y | AXIS_TRANSLATE_Z;
         m_nVisibleElements |= AXIS_SCALE_X | AXIS_SCALE_Y | AXIS_SCALE_Z;
         // no break;
      case MODE_ROTATE_ONLY:
         m_nVisibleElements |= AXIS_ROTATE_X | AXIS_ROTATE_Y | AXIS_ROTATE_Z | AXIS_ROTATE_SCREEN | AXIS_ROTATE_FREE;
         break;

      case MODE_SCALE:
         m_nVisibleElements |= AXIS_TRANSLATE_X | AXIS_TRANSLATE_Y | AXIS_TRANSLATE_Z;
         m_nVisibleElements |= AXIS_ROTATE_SCREEN;
         // no break;
      case MODE_SCALE_ONLY:
         m_nVisibleElements |= AXIS_SCALE_X | AXIS_SCALE_Y | AXIS_SCALE_Z | AXIS_SCALE_ALL;
         //if (m_nMode == MODE_SCALE_ONLY)
            m_nVisibleElements |= AXIS_SCALE_XY | AXIS_SCALE_YZ | AXIS_SCALE_XZ;
         break;
   }
}

void TransformManipulator::SetModeToSelectedElement()
{
   if (m_nSelectedElement <= AXIS_TRANSLATE_SCREEN)
      SetMode( MODE_TRANSLATE );
   else if (m_nSelectedElement <= AXIS_ROTATE_SCREEN)
      SetMode( MODE_ROTATE );
   else
      SetMode( MODE_SCALE );
}

void TransformManipulator::DrawPick( HCGLBaseView *pView )
{
//   ComputePivot( pView );

   // Load Manipulator Matrix
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   GLfloat glmatrix[16];
   m_curmatrix.GetFloatMatrix( glmatrix, true );
   glMultMatrixf( glmatrix );

//   glEnable( GL_CULL_FACE );

   m_bDrawPick = TRUE;
      Draw( pView );
   m_bDrawPick = FALSE;

//   glDisable( GL_CULL_FACE );
}

void TransformManipulator::Draw( HCGLBaseView *pView, float fAlpha/*=1.0f*/, BOOL bOpaqueItemsOnly/*=FALSE*/ )
{
   glPushAttrib( GL_LIGHTING_BIT | GL_ENABLE_BIT );

   GLfloat param[4];
   param[3] = fAlpha;

   Vector center;
   m_curmatrix.GetTranslation( center );
   Vector startcenter;
   m_startmatrix.GetTranslation( startcenter );
   BOOL b3D = pView->IsKindOf( SVT_CGL3DView ) != NULL;
   BOOL bTimeline = pView->IsKindOf( SVT_CGLTimeLineView ) != NULL;
   BOOL bTransformManip = GetClassName() == GetThisClassName();

   // line back to original location while translating
   if (m_bManipulating && !bOpaqueItemsOnly && m_nSelectedElement>=AXIS_TRANSLATE_X && m_nSelectedElement<=AXIS_TRANSLATE_SCREEN)
   {
      glDisable( GL_LIGHTING );
      glColor4f( 1.0f, 1.0f, 0.0f, fAlpha*0.25f );
      Vector vDelta = startcenter - center;
      glBegin( GL_LINES );
      glVertex3f( 0.0f, 0.0f, 0.0f );
      glVertex3fv( &vDelta.x );
      glEnd();

      glBegin( GL_POINTS );
      glVertex3fv( &vDelta.x );
      glEnd();
   }

   // Scale it to n screen pixels big
   glPushMatrix();
   Vector viewcenter = center;
   pView->WorldToView( viewcenter );
   float fPixelSize = pView->GetScreenPixelSize( fabs(viewcenter.z) );
   float fManipulatorSize = fPixelSize * m_pScreenSize->GetValue();
   glScalef( fManipulatorSize, fManipulatorSize, fManipulatorSize );

   //////////////////////////////////////////////////////////
   // PIVOT
   //////////////////////////////////////////////////////////
   if (m_pPivotMode->GetValue())
   {
      if ((!m_bManipulating && (m_nVisibleElements&AXIS_PIVOT_SCREEN)) || m_nSelectedElement==AXIS_PIVOT_SCREEN)
      {
         glDisable( GL_LIGHTING );
         glLoadName( AXIS_PIVOT_SCREEN );
         SetColor( AXIS_PIVOT_SCREEN, RGBAFloat(0,1,1,fAlpha));
         glLineWidth( 1.4f );

         // TODO: Draw a circle if any snapping is on
         if (m_bDrawPick)
            glBegin( GL_QUADS );
         else
            glBegin( GL_LINE_LOOP );

         Vector corner;
         corner.Set( 0.0f, SCREENTRANSLATESIZE, 0.0f );
         TransformVectorNoTranslate( pView->GetViewToWorldMatrix(), corner );
         glVertex3fv( &corner.x );

         corner.Set( SCREENTRANSLATESIZE, 0.0f, 0.0f );
         TransformVectorNoTranslate( pView->GetViewToWorldMatrix(), corner );
         glVertex3fv( &corner.x );

         corner.Set( 0.0f, -SCREENTRANSLATESIZE, 0.0f );
         TransformVectorNoTranslate( pView->GetViewToWorldMatrix(), corner );
         glVertex3fv( &corner.x );

         corner.Set( -SCREENTRANSLATESIZE, 0.0f, 0.0f );
         TransformVectorNoTranslate( pView->GetViewToWorldMatrix(), corner );
         glVertex3fv( &corner.x );

         glEnd();
      }

      if (!bOpaqueItemsOnly)
      {
         float fScale = m_bDrawPick ? 0.4f : 0.5f;
         glScalef( fScale, fScale, fScale );
         DrawElement( AXIS_PIVOT_ROTATEFREE, RGBAFloat(1,1,1,fAlpha*0.2f), GetTransformManipulatorPlugin()->m_nCallListRotateAll );
         glScalef( 1.0f/fScale, 1.0f/fScale, 1.0f/fScale );
      }

      //DrawPlane( AXIS_PIVOT_XY, RGBAFloat(1,0,0,fAlpha*0.4f), RGBAFloat(0,1,0,fAlpha*0.4f) );
      //glTranslatef( 0, 0, 0.5 );
         if (b3D)
            DrawElement( AXIS_PIVOT_ROTATEZ, RGBAFloat(0,0,1,fAlpha), GetTransformManipulatorPlugin()->m_nCallListPivotRotate );
         DrawPivotAxis( AXIS_PIVOT_Z, RGBAFloat(0,0,1,fAlpha) );
      //glTranslatef( 0, 0, -0.5 );

      glRotatef( -90, 0, 1, 0 );
         glRotatef( -90, 1, 0, 0 );
            //if (b3D)
            //   DrawPlane( AXIS_TRANSLATE_XZ, RGBAFloat(0,0,1,fAlpha*0.4f), RGBAFloat(1,0,0,fAlpha*0.4f) );
            //glTranslatef( 0, 0, 0.5 );
               DrawElement( AXIS_PIVOT_ROTATEY, RGBAFloat(0,1,0,fAlpha), GetTransformManipulatorPlugin()->m_nCallListPivotRotate );
               DrawPivotAxis( AXIS_PIVOT_Y, RGBAFloat(0,1,0,fAlpha) );
            //glTranslatef( 0, 0, -0.5 );
         glRotatef( 90, 1, 0, 0 );
      glRotatef( 90, 0, 1, 0 );

      glRotatef( 90, 1, 0, 0 );
         glRotatef( 90, 0, 1, 0 );
            //if (b3D)
            //   DrawPlane( AXIS_TRANSLATE_YZ, RGBAFloat(0,1,0,fAlpha*0.4f), RGBAFloat(0,0,1,fAlpha*0.4f) );
            //glTranslatef( 0, 0, 0.5 );
               DrawElement( AXIS_PIVOT_ROTATEX, RGBAFloat(1,0,0,fAlpha), GetTransformManipulatorPlugin()->m_nCallListPivotRotate );
               DrawPivotAxis( AXIS_PIVOT_X, RGBAFloat(1,0,0,fAlpha) );
            //glTranslatef( 0, 0, -0.5 );
         glRotatef( -90, 0, 1, 0 );
      glRotatef( -90, 1, 0, 0 );

      glPopMatrix();
      glPopAttrib();

      return;
   }

//////////////////////////////////////////////////////////
// TRANSLATE
//////////////////////////////////////////////////////////
   if (IsShowManipulator() || bTransformManip)
   {
      glDisable( GL_LIGHTING );

      if (!m_bDrawPick && ((!m_bManipulating && (m_nVisibleElements&AXIS_TRANSLATE_SCREEN)) || m_nSelectedElement==AXIS_TRANSLATE_SCREEN))
      {
         BOOL bDepthTestEnabled = glIsEnabled( GL_DEPTH_TEST );
         if (bDepthTestEnabled)
            glDisable( GL_DEPTH_TEST );
         glLoadName( AXIS_TRANSLATE_SCREEN );
         SetColor( AXIS_TRANSLATE_SCREEN, RGBAFloat(0.0f, 1.0f, 1.0f, fAlpha) );
         glLineWidth( 1.4f );

         // TODO: Draw a circle if any snapping is on
         glBegin( GL_LINE_LOOP );
            DrawTranslateScreen( pView );
         glEnd();

         if (m_bDrawPick || m_nMouseOverElement==AXIS_TRANSLATE_SCREEN)
         {
            SetColor( AXIS_TRANSLATE_SCREEN, RGBAFloat(0.0f, 1.0f, 1.0f, fAlpha*0.2f) );
            glBegin( GL_QUADS );
               DrawTranslateScreen( pView );
            glEnd();
         }

         if (bDepthTestEnabled)
            glEnable( GL_DEPTH_TEST );
      }

      DrawPlane( AXIS_TRANSLATE_XY, RGBAFloat(1,0,0,fAlpha*0.4f), RGBAFloat(0,1,0,fAlpha*0.4f) );
      if (b3D && DrawElement( AXIS_TRANSLATE_Z, RGBAFloat(0,0,1,fAlpha), GetTransformManipulatorPlugin()->m_nCallListTranslsateZArrow, m_nSelectedElement==AXIS_TRANSLATE_XZ||m_nSelectedElement==AXIS_TRANSLATE_YZ ))
      {
         glBegin( GL_LINES );
         glVertex3f( 0.0f, 0.0f, 0.0f );
         glVertex3f( 0.0f, 0.0f, 0.8f );
         glEnd();
      }

      glRotatef( -90, 0, 1, 0 );
      glRotatef( -90, 1, 0, 0 );
      if (b3D)
         DrawPlane( AXIS_TRANSLATE_XZ, RGBAFloat(0,0,1,fAlpha*0.4f), RGBAFloat(1,0,0,fAlpha*0.4f) );
      if (DrawElement( AXIS_TRANSLATE_Y, RGBAFloat(0,1,0,fAlpha), GetTransformManipulatorPlugin()->m_nCallListTranslsateZArrow, m_nSelectedElement==AXIS_TRANSLATE_XY||m_nSelectedElement==AXIS_TRANSLATE_YZ ))
      {
         glBegin( GL_LINES );
         glVertex3f( 0.0f, 0.0f, 0.0f );
         glVertex3f( 0.0f, 0.0f, 0.8f );
         glEnd();
      }
      glRotatef( 90, 1, 0, 0 );
      glRotatef( 90, 0, 1, 0 );

      glRotatef( 90, 1, 0, 0 );
      glRotatef( 90, 0, 1, 0 );
      if (b3D)
         DrawPlane( AXIS_TRANSLATE_YZ, RGBAFloat(0,1,0,fAlpha*0.4f), RGBAFloat(0,0,1,fAlpha*0.4f) );
      if (DrawElement( AXIS_TRANSLATE_X, RGBAFloat(1,0,0,fAlpha), GetTransformManipulatorPlugin()->m_nCallListTranslsateZArrow, m_nSelectedElement==AXIS_TRANSLATE_XY||m_nSelectedElement==AXIS_TRANSLATE_XZ ))
      {
         glBegin( GL_LINES );
         glVertex3f( 0.0f, 0.0f, 0.0f );
         glVertex3f( 0.0f, 0.0f, 0.8f );
         glEnd();
      }
      glRotatef( -90, 0, 1, 0 );
      glRotatef( -90, 1, 0, 0 );
   }

//////////////////////////////////////////////////////////
// ROTATE
//////////////////////////////////////////////////////////
   if (b3D)
   {
      glEnable( GL_LIGHTING );

      if (!bOpaqueItemsOnly && !(m_bDrawPick && bTransformManip))
         DrawElement( AXIS_ROTATE_FREE, RGBAFloat(1,1,1,fAlpha*0.2f), GetTransformManipulatorPlugin()->m_nCallListRotateAll );

      if (m_bManipulating && m_nSelectedElement != AXIS_ROTATE_FREE)
         glDisable( GL_CULL_FACE );

      DrawElement( AXIS_ROTATE_Z, RGBAFloat(0,0,1,fAlpha), GetTransformManipulatorPlugin()->m_nCallListRotateZ, m_nSelectedElement==AXIS_ROTATE_FREE );

      glRotatef( -90, 1, 0, 0 );
      DrawElement( AXIS_ROTATE_Y, RGBAFloat(0,1,0,fAlpha), GetTransformManipulatorPlugin()->m_nCallListRotateZ, m_nSelectedElement==AXIS_ROTATE_FREE );
      glRotatef( 90, 1, 0, 0 );

      glRotatef( 90, 0, 1, 0 );
      DrawElement( AXIS_ROTATE_X, RGBAFloat(1,0,0,fAlpha), GetTransformManipulatorPlugin()->m_nCallListRotateZ, m_nSelectedElement==AXIS_ROTATE_FREE );
      glRotatef( -90, 0, 1, 0 );

      if (m_bManipulating && m_nSelectedElement != AXIS_ROTATE_FREE)
         glEnable( GL_CULL_FACE );
   }

//////////////////////////////////////////////////////////
// SCALE
//////////////////////////////////////////////////////////
   DrawElement( AXIS_SCALE_ALL, RGBAFloat(0,1,1,fAlpha), GetTransformManipulatorPlugin()->m_nCallListScaleKnob );

   Vector vScale = m_pScale->GetValue();
   DrawPlane( AXIS_SCALE_XY, RGBAFloat(1,0,0,fAlpha*0.4f), RGBAFloat(0,1,0,fAlpha*0.4f) );
   if (b3D)
   {
      glTranslatef( 0, 0, vScale.z );
      if (DrawElement( AXIS_SCALE_Z, RGBAFloat(0,0,1,fAlpha), GetTransformManipulatorPlugin()->m_nCallListScaleKnob, m_nSelectedElement==AXIS_SCALE_ALL||m_nSelectedElement==AXIS_SCALE_XZ||m_nSelectedElement==AXIS_SCALE_YZ ) && !m_bDrawPick)
         DrawScaleAxis( vScale.z, fAlpha );
      glTranslatef( 0, 0, -vScale.z );
   }

   glRotatef( -90, 0, 1, 0 );
   glRotatef( -90, 1, 0, 0 );
   if (b3D)
      DrawPlane( AXIS_SCALE_XZ, RGBAFloat(0,0,1,fAlpha*0.4f), RGBAFloat(1,0,0,fAlpha*0.4f) );
   glTranslatef( 0, 0, vScale.y );
   if (DrawElement( AXIS_SCALE_Y, RGBAFloat(0,1,0,fAlpha), GetTransformManipulatorPlugin()->m_nCallListScaleKnob, m_nSelectedElement==AXIS_SCALE_ALL||m_nSelectedElement==AXIS_SCALE_XY||m_nSelectedElement==AXIS_SCALE_YZ ) && !m_bDrawPick)
      DrawScaleAxis( vScale.y, fAlpha );
   glTranslatef( 0, 0, -vScale.y );
   glRotatef( 90, 1, 0, 0 );
   glRotatef( 90, 0, 1, 0 );

   glRotatef( 90, 1, 0, 0 );
   glRotatef( 90, 0, 1, 0 );
   if (b3D)
      DrawPlane( AXIS_SCALE_YZ, RGBAFloat(0,1,0,fAlpha*0.4f), RGBAFloat(0,0,1,fAlpha*0.4f) );
   glTranslatef( 0, 0, vScale.x );
   if (DrawElement( AXIS_SCALE_X, RGBAFloat(1,0,0,fAlpha), GetTransformManipulatorPlugin()->m_nCallListScaleKnob, m_nSelectedElement==AXIS_SCALE_ALL||m_nSelectedElement==AXIS_SCALE_XY||m_nSelectedElement==AXIS_SCALE_XZ ) && !m_bDrawPick)
      DrawScaleAxis( vScale.x, fAlpha );
   glTranslatef( 0, 0, -vScale.x );
   glRotatef( -90, 0, 1, 0 );
   glRotatef( -90, 1, 0, 0 );

   // ROTATE SCREEN
   if ((!m_bManipulating && (m_nVisibleElements & AXIS_ROTATE_SCREEN)) || m_nSelectedElement == AXIS_ROTATE_SCREEN)
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

      DrawElement( AXIS_ROTATE_SCREEN, RGBAFloat(0,1,1,fAlpha), GetTransformManipulatorPlugin()->m_nCallListRotateScreen );

      if (!b3D)
         glDisable( GL_CULL_FACE );
   }

   glPopMatrix();

//////////////////////////////////////////////////////////
// HUD
//////////////////////////////////////////////////////////
   if (m_bManipulating)
   {
      if (m_nSelectedElement>=AXIS_ROTATE_X && m_nSelectedElement<=AXIS_ROTATE_SCREEN)
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
      }

      if (bOpaqueItemsOnly && fAlpha==1.0f)
      {
         if (m_nSelectedElement<=AXIS_TRANSLATE_SCREEN)
         {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

            Vector vOffset = center - startcenter;
            float fDistance = vOffset.Length();
            if (fDistance)
            {
               String strValue = (LPCTSTR)UnitToString( fDistance );

               Vector screenstartcenter = startcenter;
               pView->WorldToScreen( screenstartcenter );
               Vector screencenter = center;
               pView->WorldToScreen( screencenter );
               Vector2 viewoffset( screencenter.x - screenstartcenter.x, screencenter.y - screenstartcenter.y );
               viewoffset.Normalize();
               screenstartcenter.x -= pView->GetTextExtent( strValue ).cx / 2;
               if (viewoffset.y > 0)
                  screenstartcenter.y -= 14.0f;
               else
                  screenstartcenter.y += 14.0f;

               glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
               pView->DrawText( screenstartcenter.x + 1, screenstartcenter.y + 5, strValue );
               glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
               pView->DrawText( screenstartcenter.x, screenstartcenter.y + 4, strValue );
            }
         }
         else if (m_nSelectedElement>=AXIS_ROTATE_X && m_nSelectedElement<=AXIS_ROTATE_SCREEN)
         {
            Vector vScreenPos = center + m_vCurrent;
            pView->WorldToScreen( vScreenPos );
            String strValue = (LPCTSTR)UnitToString( m_fAngle, UNIT_ANGLE );
            glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
            pView->DrawText( vScreenPos.x + 13, vScreenPos.y + 5, strValue );
            glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
            pView->DrawText( vScreenPos.x + 12, vScreenPos.y + 4, strValue );
         }
         else if (m_nSelectedElement>=AXIS_SCALE_X && m_nSelectedElement<=AXIS_SCALE_ALL)
         {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

            Vector viewcenter = startcenter;
            pView->WorldToView( viewcenter );

            String strValue;
            int lines = -1;
            if ((m_nSelectedElement & AXIS_SCALE_ALL) == AXIS_SCALE_ALL)
               strValue = String( vScale.x * 100.0f ) + "%\n";
            else
            {
               if (m_nSelectedElement & AXIS_SCALE_X)
               {
                  strValue += "X: " + String( vScale.x * 100.0f ) + "%\n";
                  lines++;
               }
               if (m_nSelectedElement & AXIS_SCALE_Y)
               {
                  strValue += "Y: " + String( vScale.y * 100.0f ) + "%\n";
                  lines++;
               }
               if (m_nSelectedElement & AXIS_SCALE_Z)
               {
                  strValue += "Z: " + String( vScale.z * 100.0f ) + "%\n";
                  lines++;
               }
            }
            pView->Project( viewcenter );
            glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
            pView->DrawText( viewcenter.x + 13, viewcenter.y + 5 - lines * 7, strValue );
            glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
            pView->DrawText( viewcenter.x + 12, viewcenter.y + 4 - lines * 7, strValue );
         }
      }
  }

   glPopAttrib();
}

void TransformManipulator::DrawTranslateScreen( HCGLBaseView *pView )
{
   Vector corner;

   corner.Set( -SCREENTRANSLATESIZE, SCREENTRANSLATESIZE, 0.0f );
   TransformVectorNoTranslate( pView->GetViewToWorldMatrix(), corner );
   glVertex3fv( &corner.x );

   corner.Set( -SCREENTRANSLATESIZE, -SCREENTRANSLATESIZE, 0.0f );
   TransformVectorNoTranslate( pView->GetViewToWorldMatrix(), corner );
   glVertex3fv( &corner.x );

   corner.Set( SCREENTRANSLATESIZE, -SCREENTRANSLATESIZE, 0.0f );
   TransformVectorNoTranslate( pView->GetViewToWorldMatrix(), corner );
   glVertex3fv( &corner.x );

   corner.Set( SCREENTRANSLATESIZE, SCREENTRANSLATESIZE, 0.0f );
   TransformVectorNoTranslate( pView->GetViewToWorldMatrix(), corner );
   glVertex3fv( &corner.x );
}

void TransformManipulator::SetColor( GLuint nElement, const RGBAFloat &color )
{
   GLfloat param[4];
   if (m_nMouseOverElement == nElement)
      param[0] = 1.0f, param[1] = 0.75f, param[2] = 0.0f, param[3] = color.GetAlpha();
   else if (m_nSelectedElement == nElement)
      param[0] = 1.0f, param[1] = 1.0f, param[2] = 0.0f, param[3] = color.GetAlpha();
   else
      param[0] = color.GetRed(), param[1] = color.GetGreen(), param[2] = color.GetBlue(), param[3] = color.GetAlpha();

   glColor4fv( param );
   glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, param );
}

BOOL TransformManipulator::DrawElement( GLuint nElement, const RGBAFloat &color, GLuint nCallList, BOOL bForceDraw/*=FALSE*/ )
{
   if ((!m_bManipulating && (nElement & m_nVisibleElements)) || bForceDraw || m_nSelectedElement==nElement)
   {
      glLoadName( nElement );

      glEnable( GL_LIGHTING );
   
      SetColor( nElement, color );
      glCallList( nCallList );

      glDisable( GL_LIGHTING );

      return TRUE;
   }
   return FALSE;
}

void TransformManipulator::DrawPlane( GLuint nElement, const RGBAFloat &colorx, const RGBAFloat &colory )
{
   if ((!m_bManipulating && (nElement & m_nVisibleElements)) || ((m_nSelectedElement & nElement) == nElement))
   {
      glLoadName( nElement );

#if 1
      glPushAttrib( GL_ENABLE_BIT );
      glDisable( GL_CULL_FACE );
      glBegin( GL_TRIANGLES );
         RGBAFloat average = (colorx + colory) * 0.5f;
         SetColor( nElement, average );
         glVertex3f( 0.0f, 0.0f, 0.0f );
         SetColor( nElement, colory );
         glVertex3f( 0.0f, 0.5f, 0.0f );

         SetColor( nElement, colorx );
         glVertex3f( 0.5f, 0.0f, 0.0f );
      glEnd();
      glPopAttrib();
#else
      glBegin( GL_LINES );
         SetColor( nElement, colory );
         glVertex3f( 0.0f, 0.5f, 0.0f );

         SetColor( nElement, colorx );
         glVertex3f( 0.5f, 0.0f, 0.0f );
      glEnd();
#endif
   }
}

void TransformManipulator::DrawScaleAxis( float fScale, float fAlpha )
{
   if (m_nMode != MODE_SCALE_ONLY && !m_bManipulating)
      return;

   glColor4f( 0.5f, 0.5f, 0.5f, fAlpha );
   glBegin( GL_LINES );
      glVertex3f( 0.0f, 0.0f, -0.05f );
      glVertex3f( 0.0f, 0.0f, -(fScale - 0.05f) );
   glEnd();

   if (m_bManipulating)
   {
      glTranslatef( 0, 0, -(fScale - 1.0f) );
      glBegin( GL_POINTS );
         glVertex3f( 0, 0, 0 );
      glEnd();
      glTranslatef( 0, 0, fScale - 1.0f );
   }
}

void TransformManipulator::DrawPivotAxis( GLuint nElement, const RGBAFloat &color )
{
   if ((!m_bManipulating && (nElement & m_nVisibleElements)) || ((m_nSelectedElement & nElement) == nElement))
   {
      glLoadName( nElement );
      SetColor( nElement, color );
      glBegin( GL_LINES );
         glVertex3f( 0.0f, 0.0f,  0.85f );
         glVertex3f( 0.0f, 0.0f, -0.85f );
      glEnd();

      DrawElement( nElement, color, GetTransformManipulatorPlugin()->m_nCallListTranslsateZArrow );
      glRotatef( 180, 1, 0, 0 );
      DrawElement( nElement, color, GetTransformManipulatorPlugin()->m_nCallListTranslsateZArrow );
      glRotatef( -180, 1, 0, 0 );
   }
}

void TransformManipulator::PostDraw(HCGLBaseView *pView)
{
   SelectManipulator::PostDraw( pView );
   if (m_pMarqueeView || pView->GetDocument()->GetSelectionList()->IsEmpty())
      return;

   //if (!m_bManipulating)
   //   ComputePivot( pView );

   glPushAttrib( GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT );

// Lighting
   GLfloat glmatrix[16];
   pView->GetViewToWorldMatrix().GetFloatMatrix( glmatrix );
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glLoadMatrixf( glmatrix );

   glEnable( GL_LIGHT0 );
   GLfloat params[4];
   params[0] = 0.0f;
   params[1] = 0.0f;
   params[2] = 1.0f;
   params[3] = 0.0f;
   glLightfv( GL_LIGHT0, GL_POSITION, params );

   glPopMatrix();

   glEnable( GL_CULL_FACE );
   glCullFace( GL_BACK );

// Load Manipulator Matrix
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   m_curmatrix.GetFloatMatrix( glmatrix, true );
   glMultMatrixf( glmatrix );

// Draw
   glDisable( GL_DEPTH_TEST );
   Draw( pView, 0.5f );

   glEnable( GL_DEPTH_TEST );
   Draw( pView );

   glEnable( GL_POLYGON_SMOOTH );
   glDisable( GL_DEPTH_TEST );
   Draw( pView, 0.5f, TRUE );

   glEnable( GL_DEPTH_TEST );
   Draw( pView, 1.0f, TRUE );

   glPopAttrib();

   glPopMatrix();
}

void TransformManipulator::OnSelectionListChanged( HSelectionList *pSelectionList, BOOL bTemporary )
{
   ComputePivot();
}

UINT TransformManipulator::HitTest( const CPoint &pt, HCGLBaseView *pView )
{
   Vector screencenter = m_pPivotPos->GetValue();
   pView->WorldToScreen( screencenter );
   Vector2 delta( screencenter.x - pt.x, screencenter.y - pt.y );
   float fSize = SCREENTRANSLATESIZE * m_pScreenSize->GetValue();

   if ((m_nVisibleElements & AXIS_TRANSLATE_SCREEN) && abs(delta.x)<=fSize && abs(delta.y)<=fSize)
      return AXIS_TRANSLATE_SCREEN;
   
   if (!IsShowManipulator())
      return AXIS_NONE;
   
   if ((m_nVisibleElements & AXIS_SCALE_ALL) && abs(delta.x)<=fSize && abs(delta.y)<=fSize)
      return AXIS_SCALE_ALL;
   else if ((m_nVisibleElements & AXIS_PIVOT_SCREEN) && abs(delta.x)<=fSize && abs(delta.y)<=fSize)
      return AXIS_PIVOT_SCREEN;

   HPickHit *pPickHit = HPickHit::New();
   pView->HitTestManipulator( pt, pPickHit );
   UINT nElement = AXIS_NONE;
   if (pPickHit->GetNumNames())
   {
      nElement = pPickHit->GetName(0);
   }
   delete pPickHit;
   if (nElement != AXIS_NONE)
      return nElement;

   //if ((m_nVisibleElements & AXIS_PIVOT_ROTATEFREE) && delta.Length()<=m_pScreenSize->GetValue()*0.5f)
   //   return AXIS_PIVOT_ROTATEFREE;
   //else
      if ((m_nVisibleElements & AXIS_ROTATE_FREE) && delta.Length()<=m_pScreenSize->GetValue())
         return AXIS_ROTATE_FREE;

   return AXIS_NONE;
}

BOOL TransformManipulator::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   UINT nOldSelectedElement = m_nSelectedElement;
   m_nSelectedElement = HitTest( point, pView );
   if (m_nSelectedElement != nOldSelectedElement && GetClassName() == GetThisClassName())
      SetModeToSelectedElement();

   if (m_nSelectedElement) 
   {
      OnStartManipulating( point, pView );
      return TRUE;
   }
   else
      return SelectManipulator::OnLButtonDown( keymodifier, mousestate, point, pView );
}

BOOL TransformManipulator::OnLButtonDblClk( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   SelectManipulator::OnLButtonDblClk( keymodifier, mousestate, point, pView );
   OnLButtonDown( keymodifier, mousestate, point, pView );

   SetShowManipulator(!IsShowManipulator());
   pView->Invalidate();
   return TRUE;
}

BOOL TransformManipulator::OnMButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   if (m_nSelectedElement && keymodifier.m_bCtrlKeyModifier) 
   {
      m_bManipulating = TRUE;
      OnStartManipulating( point, pView );
      return TRUE;
   }
   else
      return SelectManipulator::OnMButtonDown( keymodifier, mousestate, point, pView );
}

BOOL TransformManipulator::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   if (m_bManipulating)
   {
      OnManipulating( point, pView );
      return TRUE;
   }
   else
   {
      UINT nAxis = HitTest( point, pView );
      if (nAxis != m_nMouseOverElement)
      {
         m_nMouseOverElement = nAxis;
         pView->Invalidate();
      }
      return SelectManipulator::OnMouseMove( keymodifier, mousestate, point, pView );
   }
}

BOOL TransformManipulator::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   if (m_bManipulating)
   {
      GetHMainInterface()->CommitTempCommand();
      OnEndManipulating( point, pView );
      m_bManipulating = FALSE;
      pView->Invalidate();
      return TRUE;
   }
   else
      return SelectManipulator::OnLButtonUp( keymodifier, mousestate, point, pView );
}

BOOL TransformManipulator::OnMButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{ 
   if (m_bManipulating)
      return OnLButtonUp( keymodifier, mousestate, point, pView );
   else
      return SelectManipulator::OnMButtonUp( keymodifier, mousestate, point, pView );
}

UINT TransformManipulator::GetToolTipText( CPoint point, String &strText, const HCGLBaseView *pView )
{
   if (m_nMouseOverElement == AXIS_NONE)
      return -1;

   switch (m_nMouseOverElement)
   {
   case AXIS_TRANSLATE_X:
      strText = "X Translate";
      break;
   case AXIS_TRANSLATE_Y:
      strText = "Y Translate";
      break;
   case AXIS_TRANSLATE_Z:
      strText = "Z Translate";
      break;
   case AXIS_TRANSLATE_XY:
      strText = "X-Y Plane Translate";
      break;
   case AXIS_TRANSLATE_XZ:
      strText = "X-Z Plane Translate";
      break;
   case AXIS_TRANSLATE_YZ:
      strText = "Y-Z Plane Translate";
      break;
   case AXIS_TRANSLATE_SCREEN:
      strText = "Screen Translate";
      break;

   case AXIS_ROTATE_X:
      strText = "X Rotate";
      break;
   case AXIS_ROTATE_Y:
      strText = "Y Rotate";
      break;
   case AXIS_ROTATE_Z:
      strText = "Z Rotate";
      break;
   case AXIS_ROTATE_SCREEN:
      strText = "Screen Rotate";
      break;
   case AXIS_ROTATE_FREE:
      strText = "Free Rotate";
      break;

   case AXIS_SCALE_X:
      strText = "X Scale";
      break;
   case AXIS_SCALE_Y:
      strText = "Y Scale";
      break;
   case AXIS_SCALE_Z:
      strText = "Z Scale";
      break;
   case AXIS_SCALE_XY:
      strText = "X-Y Scale";
      break;
   case AXIS_SCALE_XZ:
      strText = "X-Z Scale";
      break;
   case AXIS_SCALE_YZ:
      strText = "Y-Z Scale";
      break;
   case AXIS_SCALE_ALL:
      strText = "Uniform Scale";
      break;
   }

   return m_nMouseOverElement;
}

KeyModifiers TransformManipulator::GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const 
{
   KeyModifiers keymodifiers = SelectManipulator::GetKeyModifiers( pView, mousestate );

   switch ( mousestate.GetValue() )
   {
   case MOUSESTATE_MIDDLE:
      if (m_nSelectedElement)
         return KeyModifiers( KEYMOD_CTRL ) | keymodifiers;
   }

   return keymodifiers;
}

const char *TransformManipulator::GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const 
{
   switch (mousestate.GetValue())
   {
      case MOUSESTATE_LEFT:
      case MOUSESTATE_MIDDLE:
      {
         if (mousestate.m_bMiddleButton && !keymodifier.m_bCtrlKeyModifier)
            break;

         UINT nElement = mousestate.m_bLeftButton ? m_nMouseOverElement : m_nSelectedElement;
         switch (nElement)
         {
         case AXIS_TRANSLATE_X:
            return "Translate along the X axis only.";
         case AXIS_TRANSLATE_Y:
            return "Translate along the Y axis only.";
         case AXIS_TRANSLATE_Z:
            return "Translate along the Z axis only.";
         case AXIS_TRANSLATE_XY:
            return "Translate on the X-Y plane only.";
         case AXIS_TRANSLATE_XZ:
            return "Translate on the X-Z plane only.";
         case AXIS_TRANSLATE_YZ:
            return "Translate on the Y-Z plane only.";
         case AXIS_TRANSLATE_SCREEN:
            return "Translate on the screen plane.";

         case AXIS_ROTATE_X:
            return "Rotate around the X axis.";
         case AXIS_ROTATE_Y:
            return "Rotate around the Y axis.";
         case AXIS_ROTATE_Z:
            return "Rotate around the Z axis.";
         case AXIS_ROTATE_SCREEN:
            return "Rotate on the screen plane.";
         case AXIS_ROTATE_FREE:
            return "Rotate freely.";

         case AXIS_SCALE_X:
            return "Scale along the X axis only.";
         case AXIS_SCALE_Y:
            return "Scale along the Y axis only.";
         case AXIS_SCALE_Z:
            return "Scale along the Z axis only.";
         case AXIS_SCALE_XY:
            return "Scale on the X-Y plane only.";
         case AXIS_SCALE_XZ:
            return "Scale on the X-Z plane only.";
         case AXIS_SCALE_YZ:
            return "Scale on the Y-Z plane only.";
         case AXIS_SCALE_ALL:
            return "Scale all axis uniformly.";
         }
         break;
      }
   }

   return SelectManipulator::GetStatusbarInstructions( keymodifier, mousestate, pView );
}

BOOL TransformManipulator::OnObjectNotify(ObjectNotificationMsg nMsg, WPARAM wParam, LPARAM lParam)
{
   switch ( nMsg ) 
   {
      case NOTIFYOBJ_VALUECHANGED: 
      {
         HProperty *pProperty = (HProperty *)lParam;
         BOOL bStoreUndo = (BOOL)wParam;
         if (pProperty == m_pTranslate)
            OnTranslateChanged( !bStoreUndo );
         else if (pProperty == m_pRotate)
            OnRotateChanged( !bStoreUndo );
         else if (pProperty == m_pScale)
            OnScaleChanged( !bStoreUndo );
         else if (pProperty == m_pPivotMode)
         {
            m_nSelectedElement = AXIS_NONE;
            SetMode( m_nMode );
         }
         else if (pProperty == m_pPivotPos || pProperty == m_pPivotRotate)
            ComputeMatrix();
         break;
      }
   }

   return SelectManipulator::OnObjectNotify( nMsg, wParam, lParam );
}

void TransformManipulator::OnTranslateChanged( BOOL bTemp )
{
   String strParameters;
   Vector vOffset = m_pTranslate->GetValue();

   if (vOffset == Vector(0))
      return;

   if (vOffset.x != 0)
      strParameters += (String)"-x " + vOffset.x + " ";
   if (vOffset.y != 0)
      strParameters += (String)"-y " + vOffset.y + " ";
   if (vOffset.z != 0)
      strParameters += (String)"-z " + vOffset.z + " ";

   Vector startcenter;
   m_startmatrix.GetTranslation( startcenter );
   m_pPivotPos->StoreValue( startcenter + vOffset );

   StringList strResults;
   GetHMainInterface()->DoCommand( GetTransformManipulatorPlugin()->m_pTranslateCommandPlugin, strParameters, strResults, bTemp );

   if (!bTemp)
      m_pTranslate->StoreValue( 0.0f, TRUE );
}

void TransformManipulator::OnRotateChanged( BOOL bTemp )
{
   String strParameters;
   Vector vRotate = m_pRotate->GetValue();

   if (vRotate == Vector(0))
      return;

   Vector center;
   m_curmatrix.GetTranslation( center );
   strParameters = (String)"-about " + center;

   strParameters += (String)" -euler " + vRotate.x + " " + vRotate.y + " " + vRotate.z;
   StringList strResults;

   GetHMainInterface()->DoCommand( GetTransformManipulatorPlugin()->m_pRotateCommandPlugin, strParameters, strResults, bTemp );

   if (!bTemp)
      m_pRotate->StoreValue( 0.0f, TRUE );
}

void TransformManipulator::OnScaleChanged( BOOL bTemp )
{
   String strParameters;
   Vector vScale = m_pScale->GetValue();

   if (vScale == Vector(1))
      return;

   Vector center;
   m_curmatrix.GetTranslation( center );

   strParameters = (String)"-about " + center;

   if (vScale.x != 1)
      strParameters += (String)" -x " + vScale.x;
   if (vScale.y != 1)
      strParameters += (String)" -y " + vScale.y;
   if (vScale.z != 1)
      strParameters += (String)" -z " + vScale.z;

   StringList strResults;
   GetHMainInterface()->DoCommand( GetTransformManipulatorPlugin()->m_pScaleCommandPlugin, strParameters, strResults, bTemp );

   if (!bTemp)
      m_pScale->StoreValue( 1.0f, TRUE );
}

static Vector f_vStart;
static float  f_fDistance;
static float  f_fStartAngle;
static float  f_fStartPivotAngle;

void TransformManipulator::OnStartManipulating( CPoint point, HCGLBaseView *pView )
{
   //ComputePivot( pView );

   m_bManipulating = TRUE;
   m_startmatrix = m_curmatrix;

   Vector center;
   m_curmatrix.GetTranslation( center );
   Matrix44 matrix33;
   m_curmatrix.GetMatrix33( matrix33 );
   Vector screencenter = center;
   pView->WorldToScreen( screencenter );
   f_fDistance = screencenter.z;
   Vector viewcenter = center;
   pView->WorldToView( viewcenter );
   float fPixelSize = pView->GetScreenPixelSize( fabs(viewcenter.z) );
   float fManipulatorSize = fPixelSize * m_pScreenSize->GetValue();

   if (m_nSelectedElement <= AXIS_PIVOT_ROTATEZ)
   {
      switch (m_nSelectedElement)
      {
      case AXIS_PIVOT_X:
         m_mouseonline.Init( pView, center, matrix33 * Vector(1, 0, 0));
         f_vStart = m_mouseonline.GetWorldPos( point );
         break;
      case AXIS_PIVOT_Y:
         m_mouseonline.Init( pView, center, matrix33 * Vector(0, 1, 0));
         f_vStart = m_mouseonline.GetWorldPos( point );
         break;
      case AXIS_PIVOT_Z:
         m_mouseonline.Init( pView, center, matrix33 * Vector(0, 0, 1));
         f_vStart = m_mouseonline.GetWorldPos( point );
         break;
      case AXIS_PIVOT_SCREEN:
         m_mouseonscreen.Init( pView, center );
         f_vStart = m_mouseonscreen.GetWorldPos( point );
         break;
      case AXIS_PIVOT_ROTATEX:
         f_fStartPivotAngle = m_pPivotRotate->GetValue().x;
         m_vAxis.Set( 1.0f, 0.0f, 0.0f );
         m_mouseoncircle.Init( pView, center, matrix33*m_vAxis, fManipulatorSize );
         m_vStart = m_mouseoncircle.GetWorldPos( point );
         m_vStart -= center;
         f_fStartAngle = atan2( m_vStart.z, m_vStart.y ) * RADIANS;
         break;
      case AXIS_PIVOT_ROTATEY:
         f_fStartPivotAngle = m_pPivotRotate->GetValue().y;
         m_vAxis.Set( 0.0f, 1.0f, 0.0f );
         m_mouseoncircle.Init( pView, center, matrix33*m_vAxis, fManipulatorSize );
         m_vStart = m_mouseoncircle.GetWorldPos( point );
         m_vStart -= center;
         f_fStartAngle = atan2( m_vStart.x, m_vStart.z ) * RADIANS;
         break;
      case AXIS_PIVOT_ROTATEZ:
         f_fStartPivotAngle = m_pPivotRotate->GetValue().z;
         m_vAxis.Set( 0.0f, 0.0f, 1.0f );
         m_mouseoncircle.Init( pView, center, matrix33*m_vAxis, fManipulatorSize );
         m_vStart = m_mouseoncircle.GetWorldPos( point );
         m_vStart -= center;
         f_fStartAngle = atan2( m_vStart.y, m_vStart.x ) * RADIANS;
         break;
      }
   }
   else if (m_nSelectedElement <= AXIS_TRANSLATE_SCREEN)
   {
      switch (m_nSelectedElement)
      {
      case AXIS_TRANSLATE_X:
         m_mouseonline.Init( pView, center, matrix33 * Vector(1, 0, 0));
         f_vStart = m_mouseonline.GetWorldPos( point );
         break;
      case AXIS_TRANSLATE_Y:
         m_mouseonline.Init( pView, center, matrix33 * Vector(0, 1, 0));
         f_vStart = m_mouseonline.GetWorldPos( point );
         break;
      case AXIS_TRANSLATE_Z:
         m_mouseonline.Init( pView, center, matrix33 * Vector(0, 0, 1));
         f_vStart = m_mouseonline.GetWorldPos( point );
         break;
      case AXIS_TRANSLATE_XY:
         m_mouseonplane.Init( pView, center, matrix33 * Vector(0, 0, 1));
         f_vStart = m_mouseonplane.GetWorldPos( point );
         break;
      case AXIS_TRANSLATE_XZ:
         m_mouseonplane.Init( pView, center, matrix33 * Vector(0, 1, 0));
         f_vStart = m_mouseonplane.GetWorldPos( point );
         break;
      case AXIS_TRANSLATE_YZ:
         m_mouseonplane.Init( pView, center, matrix33 * Vector(1, 0, 0));
         f_vStart = m_mouseonplane.GetWorldPos( point );
         break;
      case AXIS_TRANSLATE_SCREEN:
         m_mouseonscreen.Init( pView, center );
         f_vStart = m_mouseonscreen.GetWorldPos( point );
         break;
      }

      m_pTranslate->StoreValue( 0.0f, FALSE );
   }
   else if (m_nSelectedElement <= AXIS_ROTATE_FREE)
   {
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
         TransformVectorNoTranslate( pView->GetWorldToViewMatrix(), m_vStart );
         f_fStartAngle = atan2( m_vStart.y, m_vStart.x ) * RADIANS;
         break;
      case AXIS_ROTATE_FREE:
         m_mouseonsphere.Init( pView, center, fManipulatorSize );
         m_vStart = m_mouseonsphere.GetWorldPos( point );
         m_vStart -= center;
         break;
      }
      m_pRotate->StoreValue( 0.0f, FALSE );
   }
   else if (m_nSelectedElement <= AXIS_SCALE_ALL)
   {
      f_vStart.Set( (float)point.x, (float)point.y, f_fDistance );
      pView->ScreenToWorld( f_vStart );

      m_pScale->StoreValue( 1.0f, FALSE );
   }
}

void TransformManipulator::OnManipulating( CPoint point, HCGLBaseView *pView )
{
   Vector vMouse(0.0f);

   if (m_nSelectedElement <= AXIS_PIVOT_ROTATEZ)
   {
      switch (m_nSelectedElement)
      {
      case AXIS_PIVOT_X:
      case AXIS_PIVOT_Y:
      case AXIS_PIVOT_Z:
         vMouse = m_mouseonline.GetWorldPos( point );
         m_pPivotPos->StoreValue( vMouse, FALSE );
         break;
      case AXIS_PIVOT_SCREEN:
         vMouse = m_mouseonscreen.GetWorldPos( point );
         m_pPivotPos->StoreValue( vMouse, FALSE );
         break;
      case AXIS_PIVOT_ROTATEX:
         m_vCurrent = m_mouseoncircle.GetWorldPos( point );
         m_vCurrent -= m_pPivotPos->GetValue();
         m_fAngle = atan2( m_vCurrent.z, m_vCurrent.y ) * RADIANS - f_fStartAngle;
         m_pPivotRotate->StoreValue( Vector( f_fStartPivotAngle + m_fAngle, 0, 0 ), FALSE );
         break;
      case AXIS_PIVOT_ROTATEY:
         m_vCurrent = m_mouseoncircle.GetWorldPos( point );
         m_vCurrent -= m_pPivotPos->GetValue();
         m_fAngle = atan2( m_vCurrent.x, m_vCurrent.z ) * RADIANS - f_fStartAngle;
         m_pPivotRotate->StoreValue( Vector( 0, f_fStartPivotAngle + m_fAngle, 0 ), FALSE );
         break;
      case AXIS_PIVOT_ROTATEZ:
         m_vCurrent = m_mouseoncircle.GetWorldPos( point );
         m_vCurrent -= m_pPivotPos->GetValue();
         m_fAngle = atan2( m_vCurrent.y, m_vCurrent.x ) * RADIANS - f_fStartAngle;
         m_pPivotRotate->StoreValue( Vector( 0, 0, f_fStartPivotAngle + m_fAngle ), FALSE );
         break;
      }
   }
   else if (m_nSelectedElement <= AXIS_TRANSLATE_SCREEN)
   {
      switch (m_nSelectedElement)
      {
      case AXIS_TRANSLATE_X:
      case AXIS_TRANSLATE_Y:
      case AXIS_TRANSLATE_Z:
         vMouse = m_mouseonline.GetWorldPos( point );
         break;
      case AXIS_TRANSLATE_XY:
      case AXIS_TRANSLATE_XZ:
      case AXIS_TRANSLATE_YZ:
         vMouse = m_mouseonplane.GetWorldPos( point );
         break;
      case AXIS_TRANSLATE_SCREEN:
         vMouse = m_mouseonscreen.GetWorldPos( point );
         break;
      }

      m_pTranslate->StoreValue( vMouse - f_vStart, FALSE );
   }
   else if (m_nSelectedElement <= AXIS_ROTATE_FREE)
   {
      Vector center;
      m_curmatrix.GetTranslation( center );

      switch (m_nSelectedElement) {
      case AXIS_ROTATE_X:
         m_vCurrent = m_mouseoncircle.GetWorldPos( point );
         m_vCurrent -= center;
         m_fAngle = atan2( m_vCurrent.z, m_vCurrent.y ) * RADIANS - f_fStartAngle;
         m_pRotate->StoreValue( Vector( m_fAngle, 0, 0 ), FALSE );
         m_curmatrix = m_startmatrix * RotateV( m_fAngle, m_vAxis );
         break;
      case AXIS_ROTATE_Y:
         m_vCurrent = m_mouseoncircle.GetWorldPos( point );
         m_vCurrent -= center;
         m_fAngle = atan2( m_vCurrent.x, m_vCurrent.z ) * RADIANS - f_fStartAngle;
         m_pRotate->StoreValue( Vector( 0, m_fAngle, 0 ), FALSE );
         m_curmatrix = m_startmatrix * RotateV( m_fAngle, m_vAxis );
         break;
      case AXIS_ROTATE_Z:
         m_vCurrent = m_mouseoncircle.GetWorldPos( point );
         m_vCurrent -= center;
         m_fAngle = atan2( m_vCurrent.y, m_vCurrent.x ) * RADIANS - f_fStartAngle;
         m_pRotate->StoreValue( Vector( 0, 0, m_fAngle ), FALSE );
         m_curmatrix = m_startmatrix * RotateV( m_fAngle, m_vAxis );
         break;
      case AXIS_ROTATE_SCREEN:
         {
            m_vCurrent = m_mouseoncircle.GetWorldPos( point );
            m_vCurrent -= center;
            TransformVectorNoTranslate( pView->GetWorldToViewMatrix(), m_vCurrent );
            m_fAngle = f_fStartAngle - (atan2( m_vCurrent.y, m_vCurrent.x ) * RADIANS);
            Matrix44 rotatematrix = RotateV( m_fAngle, m_vAxis );
            RotateEuler euler = RotateEuler( rotatematrix );
            m_pRotate->StoreValue( Vector( euler.x, euler.y, euler.z ), FALSE );
            m_curmatrix = m_startmatrix * rotatematrix;
            break;
         }
      case AXIS_ROTATE_FREE:
         m_vCurrent = m_mouseonsphere.GetWorldPos( point );
         m_vCurrent -= center;
         Matrix44 rotatematrix = Rotate2V( m_vStart, m_vCurrent );
         RotateEuler euler = RotateEuler( rotatematrix );
         m_pRotate->StoreValue( Vector( euler.x, euler.y, euler.z ), FALSE );
         m_curmatrix = m_startmatrix * rotatematrix;
         break;
      }
   }
   else if (m_nSelectedElement <= AXIS_SCALE_ALL)
   {
      vMouse.Set( (float)point.x, (float)point.y, f_fDistance );
      pView->ScreenToWorld( vMouse );

      Vector vScale;
      if (IsDownControl())
         vScale = Vector(1.0f) + (vMouse - f_vStart).Length();
      else
         vScale = Vector(1.0f) + vMouse - f_vStart;

      if (m_nSelectedElement == AXIS_SCALE_ALL)
         vScale = (vScale.x + vScale.y + vScale.z) / 3.0f;
      else if (m_nSelectedElement == AXIS_SCALE_X)
         vScale.y = vScale.z = 1.0f;
      else if (m_nSelectedElement == AXIS_SCALE_Y)
         vScale.x = vScale.z = 1.0f;
      else if (m_nSelectedElement == AXIS_SCALE_Z)
         vScale.x = vScale.y = 1.0f;
      else if (m_nSelectedElement == AXIS_SCALE_XY)
         vScale.z = 1.0f;
      else if (m_nSelectedElement == AXIS_SCALE_XZ)
         vScale.y = 1.0f;
      else if (m_nSelectedElement == AXIS_SCALE_YZ)
         vScale.x = 1.0f;

      m_pScale->StoreValue( vScale, FALSE );
   }
}

void TransformManipulator::OnEndManipulating( CPoint point, HCGLBaseView *pView )
{
   if (m_nSelectedElement <= AXIS_PIVOT_ROTATEZ)
      return;
   else if (m_nSelectedElement <= AXIS_TRANSLATE_SCREEN)
      m_pTranslate->StoreValue( 0.0f, FALSE );
   else if (m_nSelectedElement <= AXIS_ROTATE_FREE)
   {
      m_pRotate->StoreValue( 0.0f, FALSE );
      m_curmatrix = m_startmatrix;
   }
   else if (m_nSelectedElement <= AXIS_SCALE_ALL)
      m_pScale->StoreValue( 1.0f, FALSE );

   if (!IsShowManipulator())
      m_nSelectedElement = AXIS_NONE;
}
