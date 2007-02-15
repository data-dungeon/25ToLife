#include "stdafx.h"
#include "ImagePointerProperty.h"
#include "Project.h"

////////////////////////////////////////
// ImagePointerProperty

Image *BasicProperty<Image *>::m_prevvalue;

IMPLEMENT_SNIPEOBJECT_CREATE( ImagePointerProperty );

Transform2Property::Info ImagePointerProperty::m_transformInfo( "Transform", "Transform" );
   Vector2Property::Info    ImagePointerProperty::m_v2TranslateInfo( "Translate", "Translate", 0.0f, -MAX_FLOAT, MAX_FLOAT, 100.0f, Vector2Property::Info::FPT_PERCENT );
   Vector2Property::Info    ImagePointerProperty::m_v2PivotInfo( "Pivot", "Pivot", 0.5f, -MAX_FLOAT, MAX_FLOAT, 100.0f, Vector2Property::Info::FPT_PERCENT );

ImagePointerProperty::ImagePointerProperty()
{
   m_transform.Init( &m_transformInfo, this );
      m_transform.m_v2Translate.m_pPropertyInfo= &m_v2TranslateInfo;
      m_v2Pivot.Init( &m_v2PivotInfo, &m_transform );
}

BOOL ImagePointerProperty::Bind()
{
   Image *pImage = GetValue();
   if (!pImage)
      return FALSE;

   int gltextureid = pImage->GetGLTexture(0);
   if (gltextureid<0)
      return FALSE;

   glBindTexture(GLTEXTURETYPE, gltextureid);
   glEnable(GLTEXTURETYPE);

   glMatrixMode( GL_TEXTURE );
      glPushMatrix();
      glLoadIdentity();
      glTranslatef( m_v2Pivot.GetValue().x, m_v2Pivot.GetValue().y, 0.0f );
      const Matrix44 &imagematrix = m_transform.GetInverseMatrix();
      GLdouble glmatrix[16];
      imagematrix.GetDoubleMatrix( glmatrix, TRUE );
      glMultMatrixd( glmatrix );
      glTranslatef( -m_v2Pivot.GetValue().x, -m_v2Pivot.GetValue().y, 0.0f );
   glMatrixMode( GL_MODELVIEW );

   return TRUE;
}

BOOL ImagePointerProperty::UnBind()
{
   Image *pImage = GetValue();
   if (!pImage)
      return FALSE;

   glMatrixMode( GL_TEXTURE );
      glPopMatrix();
   glMatrixMode( GL_MODELVIEW );

   glDisable( GLTEXTURETYPE );
   glBindTexture( GLTEXTURETYPE, 0 );

   return TRUE;
}

BaseStream &operator << ( BaseStream &bs, const Image *&value )
{
   return operator << ( bs, (const SnipeObject * &) value);
}

BaseStream &operator >> ( BaseStream &bs, Image *&value )
{
   return operator >> (bs, (SnipeObject *&) value );
}

BOOL ImagePointerProperty::GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList )
{
   Project *pProject = GetMainInterface()->GetProject();

   if (!pProject)
   {
      ASSERT( FALSE );
      return TRUE;
   }

   ImageContainer *pImageContainer = &pProject->m_imagecontainer;
   for (UINT i=0; i<pImageContainer->GetNumImages(); i++)
   {
      Image *pImage = pImageContainer->GetImageAt( i );
      pSnipeObjectList->AddTail( pImage );
   }

   return TRUE;
}
