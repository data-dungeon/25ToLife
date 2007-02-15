#include "stdafx.h"
#include "Camera.h"
#include "Project.h"
#include "PolyModel.h"
#include "GUI/Snipe.h"
#include "GUI/MainFrm.h"
#include "GUI/GLModelView.h"
#include "GUI/ModelDoc.h"
#include "Interface/SnipeNavigator.h"
#include "resource.h"

/////////////////////////////////////////////////////
// Camera

BoolProperty::Info       Camera::m_bPerspectiveInfo( "Perspective", "Perspective", FALSE );
FloatProperty::Info      Camera::m_fFOVInfo( "Field Of View", "FOV", DEFAULTFOV, 1.0f, 175.0f, 1.0f, FloatProperty::Info::FPT_ANGLE );
FloatProperty::Info      Camera::m_fFocusDistanceInfo( "Focus Distance", "Focus Distance", 200.0f, 0.00001f, MAX_FLOAT, 1.0f, FloatProperty::Info::FPT_UNITS );
BoolProperty::Info       Camera::m_bManualClipInfo( "Manual Clip", "ManualClip", FALSE );
   FloatProperty::Info      Camera::m_fNearClipInfo( "Nearest Clip", "Nearest Clip", 5.0f, .01f, MAX_FLOAT, 1.0f, FloatProperty::Info::FPT_UNITS );
   FloatProperty::Info      Camera::m_fFarClipInfo( "Far Clip", "Far Clip", 1000.0f, .02f, MAX_FLOAT, 1.0f, FloatProperty::Info::FPT_UNITS );
FloatProperty::Info      Camera::m_fAspectRatioInfo( "Aspect Ratio", "Aspect Ratio", 1.0f, 0.000001f, MAX_FLOAT, 100.0f, FloatProperty::Info::FPT_PERCENT );

GLuint Camera::m_glCameraCallList;
GLuint Camera::m_glCameraLogoCallList;

IMPLEMENT_SNIPEOBJECT_CREATE( Camera )

Camera::Camera()
{
   m_strName = "Camera1";

   m_bPerspective.Init( &m_bPerspectiveInfo, this );
   m_fFOV.Init( &m_fFOVInfo, this );
	m_fFocusDistance.Init( &m_fFocusDistanceInfo, this );
   m_bManualClip.Init( &m_bManualClipInfo, this );
      m_fNearClip.Init( &m_fNearClipInfo, &m_bManualClip );
      m_fFarClip.Init( &m_fFarClipInfo, &m_bManualClip );
   m_fAspectRatio.Init( &m_fAspectRatioInfo, this );

   if (!m_glCameraCallList)
   {
      m_glCameraCallList = GetMainFrame()->CreateCallListFromGeoResource( IDR_CAMERA_GEO );
      m_glCameraLogoCallList = GetMainFrame()->CreateCallListFromGeoResource( IDR_CAMERALOGO_GEO );
   }
}

Camera::~Camera()
{
}

float Camera::GetCameraDistanceFactor()
{
#if 0
   return tan( m_fFOV / 2.0 * DEGTORAD );
#else
	// calculate distance to camera
   float distance = m_fFocusDistance.GetValue();
	float cosine = distance - (cos(1.0*DEGTORAD)*distance);
	float sine = sin(1.0*DEGTORAD)*distance;

	// length of line in object coordinates
	return sqrt(cosine*cosine+sine*sine);
#endif
}

void Camera::Draw( CGLBaseView *pBaseView )
{
   if (((CGL2DView *)pBaseView)->m_pCurCamera == this)
      return;

   TransformableObjectAttribsPusher glattribs( this, pBaseView );
   glPushAttrib( GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT );

   GLfloat param[4];
   param[0] = 0.1f, param[1] = 0.1f, param[2] = 0.1f, param[3] = 1.0f;
   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, param );
   param[0] = 0.5f, param[1] = 0.5f, param[2] = 0.5f, param[3] = 1.0f;
   glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, param );
   glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 80.0f );
   if (IsSelected())
      param[0] = 0.3f, param[1] = 0.3f, param[2] = 0.05f, param[3] = 1.0f;
   else
      param[0] = 0.0f, param[1] = 0.0f, param[2] = 0.0f, param[3] = 1.0f;
   glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, param );

   glEnable( GL_CULL_FACE );
   glCullFace( GL_BACK );
   // body
glScalef( .05, .05, .05 );
   glCallList( m_glCameraCallList );

   // logo
   glCallList( m_glCameraLogoCallList );
glScalef( 1.0/.05, 1.0/.05, 1.0/.05 );

   if (pBaseView->m_bDoPick)
   {
      glPopAttrib();
      return;
   }

   // Cone from camera to near clip
   glLineWidth( 1.5f );
   glDisable( GL_LIGHTING );
   float fAspect = 16.0f / 9.0f;
   float fHalfHeightAtNear = tan( m_fFOV / 2.0f * DEGTORAD ) * m_fNearClip;
   float fHalfWidthAtNear = fHalfHeightAtNear * fAspect;
   glColor4f( 0.0f, 0.0f, 0.4f, 0.2f );
   glBegin( GL_LINES );
      glVertex3f( 0,0,0 );
      glVertex3f(  fHalfWidthAtNear,  fHalfHeightAtNear, -m_fNearClip );
      glVertex3f( 0,0,0 );
      glVertex3f(  fHalfWidthAtNear, -fHalfHeightAtNear, -m_fNearClip );
      glVertex3f( 0,0,0 );
      glVertex3f( -fHalfWidthAtNear, -fHalfHeightAtNear, -m_fNearClip );
      glVertex3f( 0,0,0 );
      glVertex3f( -fHalfWidthAtNear,  fHalfHeightAtNear, -m_fNearClip );
   glEnd();

   // frame at focus distance
   float fHalfHeightAtFocus = tan( m_fFOV / 2.0f * DEGTORAD ) * m_fFocusDistance;
   float fHalfWidthAtFocus = fHalfHeightAtFocus * fAspect;
   glBegin( GL_LINE_LOOP );
      glVertex3f(  fHalfWidthAtFocus,  fHalfHeightAtFocus, -m_fFocusDistance );
      glVertex3f(  fHalfWidthAtFocus, -fHalfHeightAtFocus, -m_fFocusDistance );
      glVertex3f( -fHalfWidthAtFocus, -fHalfHeightAtFocus, -m_fFocusDistance );
      glVertex3f( -fHalfWidthAtFocus,  fHalfHeightAtFocus, -m_fFocusDistance );
   glEnd();

   // lines from far clip to focus if need be
   float fHalfHeightAtFar = tan( m_fFOV / 2.0f * DEGTORAD ) * m_fFarClip;
   float fHalfWidthAtFar = fHalfHeightAtFar * fAspect;
   if (m_fFocusDistance > m_fFarClip)
   {
      glBegin( GL_LINES );
         glVertex3f(  fHalfWidthAtFocus,  fHalfHeightAtFocus, -m_fFocusDistance );
         glVertex3f(  fHalfWidthAtFar,  fHalfHeightAtFar, -m_fFarClip );
         glVertex3f(  fHalfWidthAtFocus, -fHalfHeightAtFocus, -m_fFocusDistance );
         glVertex3f(  fHalfWidthAtFar, -fHalfHeightAtFar, -m_fFarClip );
         glVertex3f( -fHalfWidthAtFocus, -fHalfHeightAtFocus, -m_fFocusDistance );
         glVertex3f( -fHalfWidthAtFar, -fHalfHeightAtFar, -m_fFarClip );
         glVertex3f( -fHalfWidthAtFocus,  fHalfHeightAtFocus, -m_fFocusDistance );
         glVertex3f( -fHalfWidthAtFar,  fHalfHeightAtFar, -m_fFarClip );
      glEnd();
   }

   // Frustum
   glColor4f( 0.0f, 0.0f, 0.4f, 0.7f );
   glBegin( GL_LINES );
      glVertex3f(  fHalfWidthAtNear,  fHalfHeightAtNear, -m_fNearClip );
      glVertex3f(  fHalfWidthAtFar,  fHalfHeightAtFar, -m_fFarClip );
      glVertex3f(  fHalfWidthAtNear, -fHalfHeightAtNear, -m_fNearClip );
      glVertex3f(  fHalfWidthAtFar, -fHalfHeightAtFar, -m_fFarClip );
      glVertex3f( -fHalfWidthAtNear, -fHalfHeightAtNear, -m_fNearClip );
      glVertex3f( -fHalfWidthAtFar, -fHalfHeightAtFar, -m_fFarClip );
      glVertex3f( -fHalfWidthAtNear,  fHalfHeightAtNear, -m_fNearClip );
      glVertex3f( -fHalfWidthAtFar,  fHalfHeightAtFar, -m_fFarClip );
   glEnd();
   glBegin( GL_LINE_LOOP );
      glVertex3f(  fHalfWidthAtNear,  fHalfHeightAtNear, -m_fNearClip );
      glVertex3f(  fHalfWidthAtNear, -fHalfHeightAtNear, -m_fNearClip );
      glVertex3f( -fHalfWidthAtNear, -fHalfHeightAtNear, -m_fNearClip );
      glVertex3f( -fHalfWidthAtNear,  fHalfHeightAtNear, -m_fNearClip );
   glEnd();
   glBegin( GL_LINE_LOOP );
      glVertex3f(  fHalfWidthAtFar,  fHalfHeightAtFar, -m_fFarClip );
      glVertex3f(  fHalfWidthAtFar, -fHalfHeightAtFar, -m_fFarClip );
      glVertex3f( -fHalfWidthAtFar, -fHalfHeightAtFar, -m_fFarClip );
      glVertex3f( -fHalfWidthAtFar,  fHalfHeightAtFar, -m_fFarClip );
   glEnd();

   glPopAttrib();
}

// SnipeObject overrides
BOOL Camera::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_transform.m_vRotate)
         {
            CGLBaseView *pCurrentView = GetMainInterface()->GetCurrentView();
            if (!pCurrentView)
               break;

            CGLModelView *pModelView = (CGLModelView *)pCurrentView->IsKindOf( SVT_CGLModelView );
            if (!pModelView || pModelView->GetCamera()!=this)
               break;

            PolyModel *pModel = pModelView->GetDocument()->m_pCurModel;
            if (!pModel || !pModel->GetSurfaceContainer())
               break;

            SurfaceContainer *pSurfaceContainer = pModel->GetSurfaceContainer();
#if 0 // faster method of causing all surfaces to redraw in tree, but doesn't catch the property panel
            pSurfaceContainer->NotifyNodes( NOTIFYOBJ_ICONCHANGED, 0, (LPARAM)this );
#else
            for (UINT i=0; i<pSurfaceContainer->GetNumSurfaces(); i++)
            {
               Surface *pSurface = pSurfaceContainer->GetSurfaceAt( i );
               pSurface->NotifyNodes( NOTIFYOBJ_ICONCHANGED, 0, (LPARAM)pSurface );
            }
#endif
         }
         break;
      }
   }

   return SceneObject::OnObjectNotify( msg, wParam, lParam );
}

BOOL Camera::GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList )
{
   if (pPointerProperty != &m_pSceneObjectCache)
      return TRUE;

   Project *pProject = GetMainInterface()->GetProject();
   if (!pProject)
   {
      ASSERT( FALSE );
      return TRUE;
   }

   CameraContainer *pCameraContainer = &pProject->m_cameracontainer;
   for (UINT i=0; i<pCameraContainer->GetNumCameras(); i++)
   {
      Camera *pCamera = pCameraContainer->GetCameraAt( i );
      pSnipeObjectList->AddTail( pCamera );
   }

   return TRUE;
}

/////////////////////////////////////////////////////
// CameraContainer

IMPLEMENT_SNIPEOBJECT_CREATE( CameraContainer )

CameraContainer::CameraContainer()
{
   m_strName = "Cameras";
}

CameraContainer::~CameraContainer()
{
}
