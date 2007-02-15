//

#include "stdafx.h"
#include "Snipe.h"
#include "3DDoc.h"
#include "GLHeaderFrame.h"
#include "GL3DView.h"
#include "Resource.h"
#include "Interface/SnipeNavigator.h"
#include "../utility/rayseg.h"
#include "../utility/mathtools.h"
#include "GLRenderWnd.h"
#include "../Utility/Vector4.h"

IMPLEMENT_DYNAMIC(CGL3DView, CGL2DView)

BEGIN_MESSAGE_MAP(CGL3DView, CGL2DView)
   ON_COMMAND_RANGE(ID_VIEW_FRONT, ID_VIEW_BIRDSEYE, OnViewAnglePreset)
   ON_COMMAND(ID_TOGGLE_PERSPECTIVE, OnTogglePerspective)
END_MESSAGE_MAP()

static TypeProperty::Info::Node f_viewangleinfos[] = {
   TypeProperty::Info::Node( "Front", "Front" ),
   TypeProperty::Info::Node( "Back", "Shaded" ),
   TypeProperty::Info::Node( "Left", "Left" ),
   TypeProperty::Info::Node( "Right", "Right" ),
   TypeProperty::Info::Node( "Top", "Top" ),
   TypeProperty::Info::Node( "Bottom", "Bottom" ),
   TypeProperty::Info::Node( "Bird's Eye", "BirdsEye" )
};

BoolProperty::Info  CGL3DView::m_bDisplayAxisInfo( "Axis", "Axis", TRUE );
TypeProperty::Info  CGL3DView::m_nViewAngleInfo( "View Angle", "View Angle", f_viewangleinfos, sizeof(f_viewangleinfos)/sizeof(TypeProperty::Info::Node), VIEW_FRONT );

CGL3DView::CGL3DView()
{
   m_bDisplayAxis.Init( &m_bDisplayAxisInfo, &m_categoryDisplay );
   m_nViewAngle.Init( &m_nViewAngleInfo, this );

   m_viewanglepreset.SetAtGrow( VIEW_FRONT,    ViewAnglePreset( "Front",     Vector(   0,   0, 0 )) );
   m_viewanglepreset.SetAtGrow( VIEW_BACK,     ViewAnglePreset( "Back",      Vector(   0, 180, 0 )) );
   m_viewanglepreset.SetAtGrow( VIEW_LEFT,     ViewAnglePreset( "Left",      Vector(   0, -90, 0 )) );
   m_viewanglepreset.SetAtGrow( VIEW_RIGHT,    ViewAnglePreset( "Right",     Vector(   0,  90, 0 )) );
   m_viewanglepreset.SetAtGrow( VIEW_TOP,      ViewAnglePreset( "Top",       Vector( -90,   0, 0 )) );
   m_viewanglepreset.SetAtGrow( VIEW_BOTTOM,   ViewAnglePreset( "Bottom",    Vector(  90,   0, 0 )) );
   m_viewanglepreset.SetAtGrow( VIEW_BIRDSEYE, ViewAnglePreset( "Birds Eye", Vector( -30, -30, 0 )) );

   m_defaultlight.m_strName = "View Light";
   m_pCurLight = &m_defaultlight;
   m_defaultlight.Reference();

   m_nPrevViewAngle = VIEW_FRONT;

   m_defaultcamera.AddChildAtTail( &m_defaultlight );
}

CGL3DView::~CGL3DView()
{
}

void CGL3DView::UpdateHeader()
{
   m_glViewAngleComboControl.SetCurSel( m_nViewAngle, FALSE );

   CGL2DView::UpdateHeader();
}

float CGL3DView::GetScreenPixelSize( float fDistance ) const
{
   if (!m_pCurCamera->m_bPerspective)
      return CGL2DView::GetScreenPixelSize( fDistance );

   float fHeight = tan( m_pCurCamera->m_fFOV / 2.0f * DEGTORAD ) * fDistance * 2.0;
   return fHeight / (float)m_szControl.cy;
}

void CGL3DView::GetNearAndFarClip( float &fNearClip, float &fFarClip )
{
   if (m_pCurCamera->m_bManualClip)
   {
      fNearClip = m_pCurCamera->m_fNearClip;
      fFarClip = m_pCurCamera->m_fFarClip;
   }
   else
   {
      BoundingSphere boundingsphere = GetDocument()->GetBoundingSphere();
      WorldToView( boundingsphere.m_vCenter );
      float fDistance = boundingsphere.m_vCenter.Dot( Vector(0,0,-1) );

      float fRadius;
      
      if (boundingsphere.IsEmpty())
         fRadius = 1.0f;
      else
         fRadius = boundingsphere.m_fRadius;

      fNearClip = fDistance - fRadius;
      fFarClip = fDistance + fRadius;

      if (m_pCurCamera->m_bPerspective)
      {
         if (fNearClip < 0.01f)
            fNearClip = 0.01f;
         if (fFarClip < 0.02f)
            fFarClip = 0.02f;
      }

      if (m_bDisplayGrid && (m_pCurCamera->m_bPerspective || GetViewAngle()==VIEW_BIRDSEYE))
      {
         Vector2 v2GridSize( m_nSquareSize * m_nNumSquares );
         Vector vGrid( v2GridSize.x, 0.0f, v2GridSize.y );
         WorldToView( vGrid );
         fDistance = vGrid.Dot( Vector(0,0,-1) );
         if (fDistance < 1.0f && m_pCurCamera->m_bPerspective)
            fDistance = 1.0f;
         if (fDistance > fFarClip)
            fFarClip = fDistance;
         if (fDistance < fNearClip)
            fNearClip = fDistance;

         vGrid.Set( -v2GridSize.x, 0.0f, v2GridSize.y );
         WorldToView( vGrid );
         fDistance = vGrid.Dot( Vector(0,0,-1) );
         if (fDistance < 1.0f && m_pCurCamera->m_bPerspective)
            fDistance = 1.0f;
         if (fDistance > fFarClip)
            fFarClip = fDistance;
         if (fDistance < fNearClip)
            fNearClip = fDistance;

         vGrid.Set( v2GridSize.x, 0.0f, -v2GridSize.y );
         WorldToView( vGrid );
         fDistance = vGrid.Dot( Vector(0,0,-1) );
         if (fDistance < 1.0f && m_pCurCamera->m_bPerspective)
            fDistance = 1.0f;
         if (fDistance > fFarClip)
            fFarClip = fDistance;
         if (fDistance < fNearClip)
            fNearClip = fDistance;

         vGrid.Set( -v2GridSize.x, 0.0f, -v2GridSize.y );
         WorldToView( vGrid );
         fDistance = vGrid.Dot( Vector(0,0,-1) );
         if (fDistance < 1.0f && m_pCurCamera->m_bPerspective)
            fDistance = 1.0f;
         if (fDistance > fFarClip)
            fFarClip = fDistance;
         if (fDistance < fNearClip)
            fNearClip = fDistance;
      }
   }
}

void CGL3DView::SetProjectionMatrix()
{
   if (!m_pCurCamera->m_bPerspective)
   {
      CGL2DView::SetProjectionMatrix();
      return;
   }

   // clear projection matrix
   glMatrixMode(GL_PROJECTION);
   if (!m_bDoPick)
      glLoadIdentity();

   // setup projection matrix
   float fNearClip, fFarClip;
   GetNearAndFarClip( fNearClip, fFarClip );
   gluPerspective( m_pCurCamera->m_fFOV, (GLdouble)m_szControl.cx/(double)m_szControl.cy, fNearClip, fFarClip );

   // cache projection matrix
   glGetDoublev(GL_PROJECTION_MATRIX,m_GLProjectionMatrix);

   // setup view transform matrix
   glMatrixMode(GL_MODELVIEW);
   GLfloat glmatrix[16];

   GetCamera()->GetWorldToObjectMatrix().GetFloatMatrix( glmatrix, true );
   glLoadMatrixf( glmatrix );

   // cache viewing transform
   glGetDoublev(GL_MODELVIEW_MATRIX,m_GLViewTransformMatrix);
}

void CGL3DView::SetViewAngle( ViewAngleIndex index, BOOL bSnapToLastBirdsEye/*=TRUE*/ )
{
   if (index == m_nPrevViewAngle)
      return;

   if (GetNavigatorInstance() && !GetNavigatorInstance()->SetViewAngle( this, index, bSnapToLastBirdsEye ))
      DefaultSetViewAngle( index, bSnapToLastBirdsEye );
}

void CGL3DView::FillViewAngleCombo()
{
   int nCurSel = m_glViewAngleComboControl.GetCurSel();
   m_glViewAngleComboControl.ResetContent( FALSE );

   int nCount = m_viewanglepreset.GetCount();
   for (int i=0; i<nCount; i++)
      m_glViewAngleComboControl.AddString( m_viewanglepreset[i].m_name, i );

   for (int i=0; i<GetDocument()->m_cameraarray.GetCount(); i++)
   {
      Camera *pCamera=GetDocument()->m_cameraarray.GetAt(i);
      m_glViewAngleComboControl.AddString( pCamera->GetTitle(), VIEW_FIRSTCAMERA+i );
   }

   if (nCurSel < m_glViewAngleComboControl.GetCount())
      m_glViewAngleComboControl.SetCurSel( nCurSel, FALSE );
   else
      m_glViewAngleComboControl.SetCurSel( 0 );
}

void CGL3DView::DefaultSetViewAngle( ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye/*=TRUE*/ )
{
   if (nIndex == m_nPrevViewAngle)
      return;

   if (m_nPrevViewAngle < NUM3DVIEWANGLEPRESETS)
   {
      m_viewanglepreset[m_nPrevViewAngle].m_vTranslate = m_pCurCamera->m_transform.m_vTranslate;
      m_viewanglepreset[m_nPrevViewAngle].m_vRotate    = m_pCurCamera->m_transform.m_vRotate;
      m_viewanglepreset[m_nPrevViewAngle].m_fFOV       = m_pCurCamera->m_fFOV;
   }

   if (!bSnapToLastBirdsEye && nIndex == VIEW_BIRDSEYE) // start birdseye from current viewangle
   {
      m_viewanglepreset[VIEW_BIRDSEYE].m_vTranslate = m_pCurCamera->m_transform.m_vTranslate;
      m_viewanglepreset[VIEW_BIRDSEYE].m_vRotate = m_pCurCamera->m_transform.m_vRotate;
      m_viewanglepreset[VIEW_BIRDSEYE].m_fFOV = m_pCurCamera->m_fFOV;
   }

   m_nPrevViewAngle = nIndex;
   m_nViewAngle.StoreValue( nIndex );

   if (nIndex < NUM3DVIEWANGLEPRESETS)
   {
      SetCamera( &m_defaultcamera );
      m_pCurCamera->m_transform.m_vTranslate.StoreValue( m_viewanglepreset[nIndex].m_vTranslate );
      m_pCurCamera->m_transform.m_vRotate.StoreValue( m_viewanglepreset[nIndex].m_vRotate );
      m_pCurCamera->m_fFOV.StoreValue( m_viewanglepreset[nIndex].m_fFOV );
      if (bSnapToLastBirdsEye && !m_viewanglepreset[nIndex].m_bZoomFit) {
         ZoomFitAll();
         m_viewanglepreset[nIndex].m_bZoomFit = TRUE;
      }
   }
   else if (nIndex >= VIEW_FIRSTCAMERA && nIndex <= VIEW_LASTCAMERA)
   {
      SetCamera( GetDocument()->m_cameraarray[ nIndex - VIEW_FIRSTCAMERA ] );
   }
   else if (nIndex >= VIEW_FIRSTLIGHT && nIndex <= VIEW_LASTLIGHT)
   {
      // TODO
   }
   else
   {
      SetCamera( &m_defaultcamera );
      m_pCurCamera->m_transform.m_vTranslate.StoreValue( m_viewanglepreset[VIEW_FRONT].m_vTranslate );
      m_pCurCamera->m_transform.m_vRotate.StoreValue( m_viewanglepreset[VIEW_FRONT].m_vRotate );
      m_pCurCamera->m_fFOV.StoreValue( m_viewanglepreset[VIEW_FRONT].m_fFOV );
   }

   UpdateHeader();
   Invalidate();
}

void CGL3DView::DrawAxis()
{
   if (!m_bDisplayAxis)
      return;

   CRect rcViewport = GetViewportRect();

   GLboolean bOldLighting = glSet( GL_LIGHTING, FALSE );
   GLboolean bOldDepthtest = glSet( GL_DEPTH_TEST, FALSE );

   Vector screenpos( 40.0f, rcViewport.Height()-40.0f, 0.3f );
   Vector worldpos = screenpos;
   ScreenToWorld( worldpos );

   Vector viewpos = worldpos;
   WorldToView( viewpos );

   float fPixel = GetScreenPixelSize( -viewpos.z );
   float length = 20.0f * fPixel;

   glLineWidth( 3.0f );
   glBegin( GL_LINES );
      glColor3fv(colorBlack);
      glVertex3d( worldpos.x, worldpos.y, worldpos.z);
      glVertex3d( worldpos.x+length+fPixel, worldpos.y, worldpos.z);

      glVertex3d( worldpos.x, worldpos.y, worldpos.z);
      glVertex3d( worldpos.x, worldpos.y+length+fPixel, worldpos.z);

      glVertex3d( worldpos.x, worldpos.y, worldpos.z);
      glVertex3d( worldpos.x, worldpos.y, worldpos.z+length+fPixel);
   glEnd();

   glLineWidth( 1.5f );
   glBegin( GL_LINES );
      glColor3fv(colorRed);
      glVertex3d( worldpos.x, worldpos.y, worldpos.z);
      glVertex3d( worldpos.x+length, worldpos.y, worldpos.z);

      glColor3fv(colorGreen);
      glVertex3d( worldpos.x, worldpos.y, worldpos.z);
      glVertex3d( worldpos.x, worldpos.y+length, worldpos.z);

      glColor3fv(colorBlue);
      glVertex3d( worldpos.x, worldpos.y, worldpos.z);
      glVertex3d( worldpos.x, worldpos.y, worldpos.z+length);
   glEnd();
 
   Vector vDir(0,0,1);
   ViewToWorld( vDir, TRUE );
   ::glColor3fv(colorRed);
   Draw3DText( "X", Vector(worldpos.x+length*1.5,worldpos.y, worldpos.z), vDir );
   ::glColor3fv(colorGreen);
   Draw3DText( "Y", Vector(worldpos.x,worldpos.y+length*1.5f,worldpos.z), vDir );
   ::glColor3fv(colorBlue);
   Draw3DText( "Z", Vector(worldpos.x,worldpos.y,worldpos.z+length*1.5f), vDir );

   glLineWidth( 1.0f );
   glSet( GL_DEPTH_TEST, bOldDepthtest );
   glSet( GL_LIGHTING, bOldLighting );
}

//#if 0
//static void transform_point( GLdouble out[4], const GLdouble m[16], const GLdouble in[4] )
//{
//#define M(row,col)  m[col*4+row]
//   out[0] = M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
//   out[1] = M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
//   out[2] = M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
//   out[3] = M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
//#undef M
//}
//#endif

void CGL3DView::GetProjectionFromWorld(const Vector &from, Vector2 &to)
{
   // get viewport info
   GLint GLViewport[4];
   GetGLViewportInfo(GLViewport);

   GLdouble winx,winy,winz;
   BOOL successful = gluProject(from.x,from.y,from.z,
                                m_GLViewTransformMatrix,m_GLProjectionMatrix,GLViewport,
                                &winx,&winy,&winz);
   ASSERT(successful);

   CRect rcClient;
   m_pGLRenderWnd->GetClientRect( &rcClient );
   CRect rcViewport = GetViewportRect();
   winx -= rcViewport.left;
   winy = rcClient.bottom - winy;
   winy -= rcViewport.top;

   to.x = (float)winx;
   to.y = (float)winy;;
}

// takes screen coordinates
void CGL3DView::GetViewFromProjection(const Vector &from, Vector &to)
{
#if 0
   Vector4 p4( from.x * 2.0 / m_szControl.cx - 1.0,
               from.y * 2.0 / m_szControl.cy - 1.0,
               2.0 * from.z - 1.0,
               1.0 );

   Matrix44 matrix = m_GLProjectionMatrix;
   matrix *= m_GLViewTransformMatrix;
   matrix.Invert( matrix );

   p4 *= matrix;
   if (p4.w == 0.0)
      return;

   to.x = p4.x / p4.w;
   to.y = p4.y / p4.w;
   to.z = p4.z / p4.w;
#else
   // get viewport info
   CRect rcClient;
   m_pGLRenderWnd->GetClientRect(&rcClient);
   CRect rcViewport = GetViewportRect();

   Vector2 projection_point_window = Vector2(from.x,from.y);
   projection_point_window += Vector2(rcViewport.left, rcViewport.top);
   projection_point_window.y = rcClient.bottom - projection_point_window.y;

   GLint GLViewport[4];
   GetGLViewportInfo(GLViewport);

   GLdouble winx,winy,winz;
   BOOL successful = gluUnProject( projection_point_window.x, projection_point_window.y, from.z,
      m_GLViewTransformMatrix, m_GLProjectionMatrix, GLViewport,
      &winx, &winy, &winz);
   ASSERT(successful);
   to.x = winx;
   to.y = winy;
   to.z = winz;
#endif
}

BOOL CGL3DView::GetIntersectionOfProjectionPointAndPlane(Vector2 projection_point_control,
                                                      Vector plane_position,
                                                      Vector plane_normal,
                                                      Vector &intersection)
{
   // get viewport info
   CRect rcClient;
   m_pGLRenderWnd->GetClientRect(&rcClient);
   CRect rcViewport = GetViewportRect();

   Vector2 projection_point_window = projection_point_control;
   projection_point_window += Vector2(rcViewport.left, rcViewport.top);
   projection_point_window.y = rcClient.bottom - projection_point_window.y;

   GLint GLViewport[4];
   GetGLViewportInfo(GLViewport);

   GLdouble winx,winy,winz;
   BOOL successful = gluUnProject( projection_point_window.x, projection_point_window.y, 0.0,
                                   m_GLViewTransformMatrix, m_GLProjectionMatrix, GLViewport,
                                   &winx, &winy, &winz);
   ASSERT(successful);

   Vector start(winx,winy,winz);

   successful = gluUnProject( projection_point_window.x, projection_point_window.y, 1.0,
      m_GLViewTransformMatrix, m_GLProjectionMatrix, GLViewport,
      &winx, &winy, &winz);
   ASSERT(successful);

   Vector end(winx,winy,winz);

   RaySegment ray( start, end );
   successful = ray.IntersectWithPlane(plane_position,plane_normal,intersection);
   ASSERT(successful);

   return successful;
}

BOOL CGL3DView::GetIntersectionOfLineAndPlane(Vector point0,Vector point1,Vector plane_position,Vector plane_normal,Vector &intersection)
{
   RaySegment ray(point0,point1);
   BOOL successful = ray.IntersectWithPlane(plane_position,plane_normal,intersection);
   return successful;
}

void CGL3DView::OnViewAnglePreset( UINT nID )
{
   SetViewAngle( (ViewAngleIndex)(nID - ID_VIEW_FRONT));
   GetApp().DrawInvalidControls();
}

void CGL3DView::OnTogglePerspective()
{
   GetCamera()->m_bPerspective.StoreValue( !GetCamera()->m_bPerspective );
   GetApp().DrawInvalidControls();
}

void CGL3DView::PostInit()
{
   CGL2DView::PostInit();

   GetHeaderFrame()->AddHeaderControl( &m_glViewAngleComboControl, CPoint(5,0), CSize(94,CGLHeaderFrame::HEADERHEIGHT), ID_VIEW_ANGLEPRESET_COMBO );
}

BOOL CGL3DView::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_CB_CHANGED:
      {
         UINT nID = LOWORD(wParam);
         int nIndex = HIWORD(wParam);
         switch (nID)
         {
            case ID_VIEW_ANGLEPRESET_COMBO:
            {
               CGLComboControl *pCombo = (CGLComboControl *)lParam;
               ViewAngleIndex nViewAngleIndex = (ViewAngleIndex)pCombo->GetItemData( nIndex );
               m_nViewAngle.StoreValue( nViewAngleIndex );
               GetApp().DrawInvalidControls();
               return TRUE;
            }
         }
         break;
      }
   }

   return CGL2DView::OnControlNotify( msg, wParam, lParam );
}

BOOL CGL3DView::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_nViewAngle)
            SetViewAngle( (ViewAngleIndex)m_nViewAngle.GetValue() );
         break;
      }
   }

   return CGL2DView::OnObjectNotify( msg, wParam, lParam );
}

String CGL3DView::GetTitle()
{
   String strTitle = CGL2DView::GetTitle();

   strTitle += " (";
   strTitle += m_viewanglepreset[ m_nViewAngle ].m_name;
   strTitle += ")";

   return strTitle;
}

void CGL3DView::DrawGrid()
{
   if (!m_bDisplayGrid)
      return;

   glPushMatrix();

   Camera *pCamera = GetCamera();

   if (pCamera->m_bPerspective || GetViewAngle()==VIEW_BIRDSEYE)
      glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
   else
   {
      Vector vRotate = GetCamera()->m_transform.m_vRotate.GetValue();
      glRotatef( vRotate.z, 0.0f, 0.0f, 1.0f );
      glRotatef( vRotate.x, 1.0f, 0.0f, 0.0f );
      glRotatef( vRotate.y, 0.0f, 1.0f, 0.0f );
      float fNearClip, fFarClip;
      GetNearAndFarClip( fNearClip, fFarClip );
      Vector pos( 0, 0, -fFarClip + (fFarClip - fNearClip)*0.20f );
      pos *= pCamera->GetObjectToWorldMatrix();
      glTranslatef( 0, 0, pos.z );
   }

   CGL2DView::DrawGrid();

   glPopMatrix();
}

void CGL3DView::Draw3DCircleFacingScreen( Vector translate, float fRadius, UINT nSegments, bool bFilled )
{
   const double PI = 3.141592653589793238462643383279502884197169399375105820974944592308;

   glPushMatrix();

   Matrix44 matrix = GetWorldToViewMatrix();
   GLfloat glmatrix[16];
   matrix.GetFloatMatrix( glmatrix, true );
   glLoadMatrixf( glmatrix );

   WorldToScreen(translate);

   if (bFilled)
   {
      glDisable(GL_CULL_FACE);
      glPolygonMode( GL_FRONT, GL_FILL );
      glBegin(GL_POLYGON);
   }
   else
      glBegin(GL_LINE_LOOP);

   Vector v;
   for(UINT i=0;i<nSegments;i++)
   {
      v.Set(fRadius * cos(i*2.0f*PI/nSegments)+translate.x,
         fRadius * sin(i*2.0f*PI/nSegments)+translate.y,translate.z);
      ScreenToWorld(v);
      glVertex3fv(&v.x);
   }
   glEnd();

   glPopMatrix();
}