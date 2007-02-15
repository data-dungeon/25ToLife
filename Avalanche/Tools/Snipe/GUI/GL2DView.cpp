//
#include "stdafx.h"
#include "GL2DView.h"
#include "Snipe.h"
#include "GLRenderWnd.h"
#include "Interface/SnipeNavigator.h"
#include "resource.h"
#include ".\gl2dview.h"

IMPLEMENT_DYNAMIC(CGL2DView, CGLHeaderView)

BEGIN_MESSAGE_MAP(CGL2DView, CGLHeaderView)
   ON_COMMAND(ID_DISPLAY_GRID, OnDisplayGrid)
END_MESSAGE_MAP()

BoolProperty::Info    CGL2DView::m_bDisplayGridInfo( "Grid", "Grid", TRUE );
Vector2Property::Info    CGL2DView::m_nSquareSizeInfo( "Square Size", "SquareSize", 30.0f, 1.0f, MAX_FLOAT, 1.0f, Vector2Property::Info::FPT_UNITS );
Vector2Property::Info    CGL2DView::m_nNumSquaresInfo( "Resolution", "Resolution", 10.0f, 1.0f, MAX_FLOAT );

CGL2DView::CGL2DView()
{
   m_bDisplayGrid.Init( &m_bDisplayGridInfo, &m_categoryDisplay );
      m_nSquareSize.Init( &m_nSquareSizeInfo, &m_bDisplayGrid );
      m_nNumSquares.Init( &m_nNumSquaresInfo, &m_bDisplayGrid );

   m_defaultcamera.m_strName = "View Camera";
   m_defaultcamera.Reference();
   m_pCurCamera = &m_defaultcamera;
   m_objecttoviewmatrix.Identity();
   m_viewtoobjectmatrix.Identity();
   m_glGridCallList = 0;
}

CGL2DView::~CGL2DView()
{
   SetCamera( NULL );
}

void CGL2DView::PostInit()
{
   SetCamera( &m_defaultcamera );

   CGLHeaderView::PostInit();

   //ZoomFitAll();
}

void CGL2DView::SetCamera( Camera *pCamera )
{
   if (m_pCurCamera)
      m_pCurCamera->RemoveNotifyNode( GetNavigatorInstance());

   m_pCurCamera = pCamera;

   if (m_pCurCamera && GetNavigatorInstance())
      m_pCurCamera->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, GetNavigatorInstance() );
}

SnipeNavigatorInstance *CGL2DView::SetNavigatorInstance( SnipeNavigator *pNavigator )
{
   if (GetNavigatorInstance() && m_pCurCamera)
      m_pCurCamera->RemoveNotifyNode( GetNavigatorInstance() );

   SnipeNavigatorInstance *pNavigatorInstance = CGLBaseView::SetNavigatorInstance( pNavigator );

   if (GetNavigatorInstance() && m_pCurCamera)
      m_pCurCamera->AddNotifyNode( NOTIFYOBJ_VALUECHANGED, GetNavigatorInstance() );

   return pNavigatorInstance;
}

void CGL2DView::GetNearAndFarClip( float &fNearClip, float &fFarClip )
{
   fNearClip = m_pCurCamera->m_fFocusDistance+10.0f;
   fFarClip = m_pCurCamera->m_fFocusDistance-10.0f;
}

void CGL2DView::SetProjectionMatrix()
{
   // clear projection matrix
   glMatrixMode(GL_PROJECTION);
   if (!m_bDoPick)
      glLoadIdentity();

   // setup projection matrix
   float fHeight = tan( m_pCurCamera->m_fFOV / 2.0f * DEGTORAD ) * m_pCurCamera->m_fFocusDistance;
   float fWidth = m_pCurCamera->m_fAspectRatio * fHeight * ((float)m_szControl.cx / (float)m_szControl.cy);
   float fNearClip, fFarClip;
   GetNearAndFarClip( fNearClip, fFarClip );
   glOrtho( -fWidth, fWidth, -fHeight, fHeight, fNearClip, fFarClip );

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

void CGL2DView::ViewToWorld( Vector &p, BOOL bRotateOnly/*=FALSE*/ ) const
{
   if (bRotateOnly)
      TransformVectorNoTranslate( m_viewtoobjectmatrix * m_pCurCamera->GetObjectToWorldMatrix(), p );
   else
      p *= m_viewtoobjectmatrix * m_pCurCamera->GetObjectToWorldMatrix();
}

void CGL2DView::WorldToView( Vector &p ) const
{
   p *= m_pCurCamera->GetWorldToObjectMatrix() * m_objecttoviewmatrix;
}

void CGL2DView::ScreenToWorld( Vector &p, BOOL bRotateOnly/*=FALSE*/ ) const
{
   UnProject( p );
   ViewToWorld( p, bRotateOnly );
}

void CGL2DView::WorldToScreen( Vector &p ) const
{
   WorldToView( p );
   Project( p );
}

const Matrix44 &CGL2DView::GetWorldToViewMatrix() const
{
   static Matrix44 matrix;
   matrix = m_pCurCamera->GetWorldToObjectMatrix() * m_objecttoviewmatrix;
   return matrix;
   return m_pCurCamera->GetWorldToObjectMatrix();
}

const Matrix44 &CGL2DView::GetViewToWorldMatrix() const
{
   static Matrix44 matrix;
   matrix = m_viewtoobjectmatrix * m_pCurCamera->GetObjectToWorldMatrix();
   return matrix;
   return m_pCurCamera->GetObjectToWorldMatrix();
}

float CGL2DView::GetScreenPixelSize( float fDistance ) const
{
   float fHeight = tan( m_pCurCamera->m_fFOV / 2.0f * DEGTORAD ) * m_pCurCamera->m_fFocusDistance * 2.0;
   return fHeight / (float)m_szControl.cy;
}

BOOL CGL2DView::Project( Vector &p ) const
{
#if 0
   GLdouble in[4], out[4];

   /* initilise la matrice et le vecteur a transformer */
   in[0] = p.x;
   in[1] = p.y;
   in[2] = p.z;
   in[3] = 1.0;
   transform_point(out, m_GLViewTransformMatrix, in);
   transform_point(in, m_GLProjectionMatrix, out);

   /* d'ou le resultat normalise entre -1 et 1 */
   if (in[3] == 0.0)
      return FALSE;

   in[0] /= in[3];
   in[1] /= in[3];
   in[2] /= in[3];

   /* en coordonnees ecran */
   p.x = (1 + in[0]) * m_szControl.cx / 2;
   p.y = (1 + in[1]) * m_szControl.cy / 2;
   /* entre 0 et 1 suivant z */
   p.z = (1 + in[2]) / 2;

   return TRUE;
#else
   GLint GLViewport[4];
   GetGLViewportInfo(GLViewport);

   Matrix44 identity;
   identity.Identity();
   GLdouble glIdentityMatrix[16];
   identity.GetDoubleMatrix( glIdentityMatrix, true );

   GLdouble winx,winy,winz;
   BOOL successful = gluProject( p.x, p.y, p.z, glIdentityMatrix/*m_GLViewTransformMatrix*/, m_GLProjectionMatrix, GLViewport, &winx, &winy, &winz );
   ASSERT(successful);

   CRect rcClient;
   m_pGLRenderWnd->GetClientRect( &rcClient );
   CRect rcViewport = GetViewportRect();
   winx -= rcViewport.left;
   winy = rcClient.bottom - winy;
   winy -= rcViewport.top;

   p.Set( winx, winy, winz );

   return successful;
#endif
}

BOOL CGL2DView::UnProject( Vector &p ) const
{
#if 0
   Vector4 p4( p.x * 2.0 / m_szControl.cx - 1.0,
               p.y * 2.0 / m_szControl.cy - 1.0,
               2.0 * p.z - 1.0,
               1.0 );

   Matrix44 matrix = m_GLProjectionMatrix;
   matrix *= m_GLViewTransformMatrix;
   matrix.Invert( matrix );

   p4 *= matrix;
   if (p4.w == 0.0)
      return GL_FALSE;

   p.x = p4.x / p4.w;
   p.y = p4.y / p4.w;
   p.z = p4.z / p4.w;

   return GL_TRUE;
#else
   CRect rcClient;
   m_pGLRenderWnd->GetClientRect(&rcClient);
   CRect rcViewport = GetViewportRect();

   Vector projection_point_window = p;
   projection_point_window += Vector(rcViewport.left, rcViewport.top, 0.0);
   projection_point_window.y = rcClient.bottom - projection_point_window.y;

   //projection_point_window.z += m_pCurCamera->m_fNearClip;
   //projection_point_window.z /= (m_pCurCamera->m_fFarClip - m_pCurCamera->m_fNearClip);
   //projection_point_window.z = -projection_point_window.z;
   //projection_point_window.z += 0.5f;

   GLint GLViewport[4];
   GetGLViewportInfo(GLViewport);

   static GLdouble glIdentityMatrix[16] =
   { 
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
   };

   GLdouble objx, objy, objz;
   BOOL successful = gluUnProject( projection_point_window.x, projection_point_window.y, projection_point_window.z,
                                   glIdentityMatrix, m_GLProjectionMatrix, GLViewport, &objx, &objy, &objz);
   ASSERT(successful);

   p.Set( objx, objy, objz );

   return successful;
#endif
}

void CGL2DView::Draw3DText( const String &strText, const Vector &pos, const Vector &dir, float fHeightAtCameraPlane/*=20.0f*/, const Vector &up/*=Vector(0,1,0)*/, Justify justify/*=JUST_CENTER*/ )
{
   static GLImageList imagelist;
   if (imagelist.IsEmpty())
      imagelist.Create( IDB_FONT, 32 );

   Vector viewpos( pos );
   WorldToView( viewpos );
   float fFactor = GetScreenPixelSize( -viewpos.z ) * fHeightAtCameraPlane;
   float fCharWidth = fFactor * 0.6f;
   float fHalfCharHeight = fCharWidth * 0.5f;
   float z = fFactor * 0.5f;
   int nLength = strText.Length();

   float fLeft;
   if (justify == JUST_LEFT)
      fLeft = 0.0f;
   else if (justify == JUST_RIGHT)
      fLeft = -((fCharWidth*0.85f) * nLength);
   else // Center
      fLeft = -((fCharWidth*0.85f) * nLength * 0.5f);

   GLboolean bOldCulling = glSet( GL_CULL_FACE, FALSE );
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
      glTranslatef( pos.x, pos.y, pos.z );
      Quaternion quat = VectorsToStandardQ( dir, up ).Inverse();
      RotateEuler re( quat );
      glRotatef( re.y, 0.0f, 1.0f, 0.0f );
      glRotatef( re.x, 1.0f, 0.0f, 0.0f );
      glRotatef( re.z, 0.0f, 0.0f, 1.0f );
      int nSize = imagelist.GetWidthEach();
      imagelist.Bind();
         glBegin( GL_QUADS );
            for (int i=0; i<nLength; i++, fLeft+=fCharWidth*0.85f)
            {
               int ascii = strText[i];
               if (ascii<' ' || ascii>'~')
                  continue;
               int nImage = ascii - ' ';
               int x = nImage % 16 * nSize;
               int y = nImage / 16 * nSize;
               Vector p1(fLeft, fHalfCharHeight, z);
               Vector p2(fLeft+fCharWidth, -fHalfCharHeight, z);
               glBegin( GL_QUADS );
                  glTexCoord2d( x,         y );         glVertex3f( p1.x, p1.y, p1.z );
                  glTexCoord2d( x + nSize, y );         glVertex3f( p2.x, p1.y, p1.z );
                  glTexCoord2d( x + nSize, y + nSize ); glVertex3f( p2.x, p2.y, p1.z );
                  glTexCoord2d( x,         y + nSize ); glVertex3f( p1.x, p2.y, p1.z );
               glEnd();
//               if (ascii>='0' && ascii<='9')
//                  imagelist.DrawQuad( Vector(fLeft, fHalfCharHeight, z), Vector(fLeft+fCharWidth, -fHalfCharHeight, z), ascii-'0' );
            }
         glEnd();
      imagelist.UnBind();
   glPopMatrix();
   glSet( GL_CULL_FACE, bOldCulling );
}

BOOL CGL2DView::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty==&m_nSquareSize || pProperty==&m_nNumSquares)
         {
            if (m_glGridCallList)
            {
               glDeleteLists( m_glGridCallList, 1 );
               m_glGridCallList = 0;
            }
         }
         break;
      }
   }

   return CGLHeaderView::OnObjectNotify( msg, wParam, lParam );
}

void CGL2DView::DrawGrid()
{
   if (!m_bDisplayGrid)
      return;

   if (m_glGridCallList)
   {
      glCallList( m_glGridCallList );
      return;
   }

   m_glGridCallList = glGenLists( 1 );
   glNewList( m_glGridCallList, GL_COMPILE_AND_EXECUTE );

   glDisable(GL_LIGHTING);
   glDisable( GL_CULL_FACE );

   // checker board
   BOOL bOdd = TRUE;
   glColor4f( 1.0f, 1.0f, 1.0f, 0.03f );
   glBegin( GL_QUADS );
   for ( int y = -m_nNumSquares.GetValue().y ; y < m_nNumSquares.GetValue().y ; y++, bOdd = !bOdd )
   {
      for ( int x = -m_nNumSquares.GetValue().x + bOdd; x < m_nNumSquares.GetValue().x ; x+=2 )
      {
         Vector corner( x*m_nSquareSize.GetValue().x, y*m_nSquareSize.GetValue().y, 0.0f );
         glVertex3fv( &corner.x );
         corner.x += m_nSquareSize.GetValue().x;
         glVertex3fv( &corner.x );
         corner.y += m_nSquareSize.GetValue().y;
         glVertex3fv( &corner.x );
         corner.x -= m_nSquareSize.GetValue().x;
         glVertex3fv( &corner.x );
      }
   }
   glEnd();

   GLfloat v0[3], v1[3], v2[3], v3[3];

   v0[2] = v1[2] = v2[2] = v3[2] = 0.0f;

   glLineWidth(1.0);

   // grid color
   glColor3f(0.5f,0.5f,0.5f);

   // inner lines of grid
   glBegin( GL_LINES );
   for ( int x = -m_nNumSquares.GetValue().x ; x <= m_nNumSquares.GetValue().x ; x++ )
   {
      v0[0] = x*m_nSquareSize.GetValue().x; v0[1] = -m_nNumSquares.GetValue().y*m_nSquareSize.GetValue().y; v0[2] = 0.0;
      v1[0] = x*m_nSquareSize.GetValue().x; v1[1] =  m_nNumSquares.GetValue().y*m_nSquareSize.GetValue().y; v1[2] = 0.0; 
      glVertex3fv( v0 );
      glVertex3fv( v1 );
   }
   glEnd();

   glBegin( GL_LINES );
   for ( int y = -m_nNumSquares.GetValue().y ; y <= m_nNumSquares.GetValue().y ; y++ )
   {
      v0[0] = -m_nNumSquares.GetValue().x*m_nSquareSize.GetValue().x; v0[1] = y*m_nSquareSize.GetValue().y;
      v1[0] =  m_nNumSquares.GetValue().x*m_nSquareSize.GetValue().x; v1[1] = y*m_nSquareSize.GetValue().y;
      glVertex3fv( v0 );
      glVertex3fv( v1 );	
   }
   glEnd();

   //	glDisable(GL_DEPTH_TEST);

   // grid axis color
   glColor3f(0.1f,0.1f,0.1f);

   glBegin( GL_LINES );

   // draw dark x and z grid lines

   v0[0] = -m_nNumSquares.GetValue().x*m_nSquareSize.GetValue().x; v0[1] = 0.0; v0[2] = 0.0f;
   v1[0] = m_nNumSquares.GetValue().x*m_nSquareSize.GetValue().x; v1[1] = 0.0; v1[2] = 0.0f;

   v2[0] = 0.0; v2[1] = -m_nNumSquares.GetValue().y*m_nSquareSize.GetValue().y; v2[2] = 0.0;
   v3[0] = 0.0; v3[1] = m_nNumSquares.GetValue().y*m_nSquareSize.GetValue().y; v3[2] = 0.0;

   glVertex3fv( v0 );
   glVertex3fv( v1 );

   glVertex3fv( v2 );
   glVertex3fv( v3 );

   glEnd();

   glEnable(GL_LIGHTING);
   //	glEnable(GL_DEPTH_TEST);

   glEndList();
}

BOOL CGL2DView::IsSphereInFrustum( const BoundingSphere &boundingsphere )
{
/////////////////////////
// Compute clipping planes

   Vector4 v4Plane[6] =
   {
      Vector4(  1,  0,  0, 0 ), // left
      Vector4( -1,  0,  0, 0 ), // right
      Vector4(  0, -1,  0, 0 ), // top
      Vector4(  0,  1,  0, 0 ), // bottom
      Vector4(  0,  0,  1, 0 ), // front
      Vector4(  0,  0, -1, 0 ), // back
   };

   Camera *pCamera = GetCamera();
   float fViewAspect = (float)m_szControl.cx / m_szControl.cy;
   float fHalfVFOV = pCamera->m_fFOV / 2.0f;
   float fHalfHFOV = fHalfVFOV * pCamera->m_fAspectRatio * fViewAspect;

   RotateEuler rotate( 0.0f, fHalfHFOV, 0.0f );
   Matrix44 matrix( rotate );
   v4Plane[0] *= matrix;

   rotate.y = -fHalfHFOV;
   matrix = rotate;
   v4Plane[1] *= matrix;

   rotate.y = 0.0f;

   rotate.x = fHalfVFOV;
   matrix = rotate;
   v4Plane[2] *= matrix;

   rotate.x = -fHalfVFOV;
   matrix = rotate;
   v4Plane[3] *= matrix;

/////////////////////////////////

   Vector p = boundingsphere.m_vCenter;
   WorldToView( p );

   for (int i=0; i<4; i++)
   {
      Vector vPlane( v4Plane[i].x, v4Plane[i].y, v4Plane[i].z );
      float fDist = p.Dot( vPlane ) + v4Plane[i].w;
      if (fDist + boundingsphere.m_fRadius < 0.0f)
         return FALSE;
   }

   return TRUE;
}

void CGL2DView::OnDisplayGrid()
{
   m_bDisplayGrid.StoreValue( !m_bDisplayGrid );
   GetApp().DrawInvalidControls();
}
