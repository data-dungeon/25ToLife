//

#include "stdafx.h"
#include "TransformableObject.h"
#include "GLBaseView.h"
#include "GL3DView.h"

TransformableObjectAttribsPusher::TransformableObjectAttribsPusher( TransformableObject *pObj, CGLBaseView *pView )
{
   m_pView = pView;
   m_pObject = pObj;

   glGetDoublev( GL_MODELVIEW_MATRIX, m_GLModelViewMatrix );

   CGL2DView *p2DView = (CGL2DView *)pView;
   Matrix44 matrix = p2DView->GetWorldToViewMatrix();
   matrix *= pObj->GetObjectToWorldMatrix();
   matrix *= pObj->GetBoneToObjectMatrix();

   GLfloat glmatrix[16];
   matrix.GetFloatMatrix( glmatrix, true );
   ::glMatrixMode( GL_MODELVIEW );
   glLoadMatrixf( glmatrix );

   //pObj->PushModelViewMatrix();

   if (m_pView->m_bDoPick && m_pObject->IsKindOf( m_pView->m_nPickObjectType ))
      glLoadName( (GLuint)m_pObject );
}

TransformableObjectAttribsPusher::~TransformableObjectAttribsPusher()
{
   glMatrixMode( GL_MODELVIEW );
   glLoadMatrixd( m_GLModelViewMatrix );
   glLoadName( NULL );
}

