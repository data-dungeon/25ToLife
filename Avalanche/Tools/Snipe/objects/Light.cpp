#include "stdafx.h"
#include "Light.h"
#include "Project.h"
#include "GUI/Snipe.h"
#include "GUI/GLBaseView.h"
#include "GUI/MainFrm.h"
#include "resource.h"

/////////////////////////////////////////////////////
// Light

static TypeProperty::Info::Node f_typeinfos[] = {
   TypeProperty::Info::Node( "Directional", "Directional" ),
   TypeProperty::Info::Node( "Point", "Point" ),
   TypeProperty::Info::Node( "Spot", "Spot" ),
//   TypeProperty::Info::Node( "Linear" ),
//   TypeProperty::Info::Node( "Area" ),
};

ColorProperty::Info  Light::m_cAmbientColorInfo( "Ambient", "Ambient", RGBFloat(1.0f), RGBFloat(-MAX_FLOAT), RGBFloat(MAX_FLOAT) );
ColorProperty::Info  Light::m_cDiffuseColorInfo( "Diffuse", "Diffuse", RGBFloat(1.0f), RGBFloat(-MAX_FLOAT), RGBFloat(MAX_FLOAT) );
ColorProperty::Info  Light::m_cSpecularColorInfo( "Specular", "Specular", RGBFloat(1.0f), RGBFloat(-MAX_FLOAT), RGBFloat(MAX_FLOAT) );
TypeProperty::Info   Light::m_nTypeInfo( "Type", "Type", f_typeinfos, sizeof(f_typeinfos)/sizeof(TypeProperty::Info::Node), Light::TYPE_DIRECTIONAL );
FloatProperty::Info  Light::m_fSpotExponentInfo( "Spot Exponent", "Spot Exponent", 100.0f, 0.0f, 128.0f );
FloatProperty::Info  Light::m_fSpotCutoffInfo( "Spot Cutoff", "Spot Cutoff", 30.0f, 0.0f, 90.0f, 1.0f, FloatProperty::Info::FPT_ANGLE );
VectorProperty::Info Light::m_vAttenuationInfo( "Attenuation", "Attenuation", Vector(1,0,0), Vector(0,0,0), Vector(1,1,1), 100.0f, VectorProperty::Info::FPT_PERCENT );
GLuint               Light::m_glDirectionalIconCallList;
GLuint               Light::m_glPointIconCallList;
GLuint               Light::m_glSpotIconCallList;

IMPLEMENT_SNIPEOBJECT_CREATE( Light )

Light::Light()
{
   m_strName = "Light1";

   m_cAmbientColor.Init( &m_cAmbientColorInfo, this );
   m_cDiffuseColor.Init( &m_cDiffuseColorInfo, this );
   m_cSpecularColor.Init( &m_cSpecularColorInfo, this );
   m_nType.Init( &m_nTypeInfo, this );
   m_fSpotExponent.Init( &m_fSpotExponentInfo, this );
   m_fSpotCutoff.Init( &m_fSpotCutoffInfo, this );
   m_vAttenuation.Init( &m_vAttenuationInfo, this );

   if (!m_glSpotIconCallList)
   {
      m_glSpotIconCallList = GetMainFrame()->CreateCallListFromGeoResource( IDR_LIGHT_GEO );

      m_glPointIconCallList = glGenLists( 1 );
      glNewList( m_glPointIconCallList, GL_COMPILE_AND_EXECUTE );
      glBegin( GL_LINES );
      for (int i=0; i<30; i++)
      {
         Vector p( Rnd()-0.5f, Rnd()-0.5f, Rnd()-0.5f );
         p.Normalize();
         p += p * Rnd();
         glVertex3f( 0,0,0 );
         glVertex3fv( &p.x );
      }
      glEnd();
      glEndList();
   }
}

Light::~Light()
{
}

void Light::Bind( UINT nIndex )
{
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   GLfloat glmatrix[16];
   GetObjectToWorldMatrix().GetFloatMatrix( glmatrix, true );
   ::glMultMatrixf( glmatrix );

   glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 + nIndex );

   switch (m_nType) {
      case TYPE_DIRECTIONAL:
      {
         Vector4 pos( 0.0f, 0.0f, 1.0f, 0.0f );
         glLightfv( GL_LIGHT0 + nIndex, GL_POSITION, &pos.x );
         break;
      }
      case TYPE_POINT:
      {
         Vector4 pos( 0.0f, 0.0f, 0.0f, 1.0f );
         glLightfv( GL_LIGHT0 + nIndex, GL_POSITION, &pos.x );

         Vector4 dir( 0.0f, 0.0f, -1.0f, 0.0f );
         glLightfv( GL_LIGHT0 + nIndex, GL_SPOT_DIRECTION, &dir.x );

         glLightf( GL_LIGHT0 + nIndex, GL_SPOT_CUTOFF, 180.0f );
         break;
      }
      case TYPE_SPOT:
      {
         Vector4 pos( 0.0f, 0.0f, 0.0f, 1.0f );
         glLightfv( GL_LIGHT0 + nIndex, GL_POSITION, &pos.x );

         Vector4 dir( 0.0f, 0.0f, -1.0f, 0.0f );
         glLightfv( GL_LIGHT0 + nIndex, GL_SPOT_DIRECTION, &dir.x );

         glLightf( GL_LIGHT0 + nIndex, GL_SPOT_CUTOFF, m_fSpotCutoff );
         break;
      }
   }

   GLfloat params[4];
   params[0] = m_cAmbientColor.GetValue().GetRed(); params[1] = m_cAmbientColor.GetValue().GetGreen(); params[2] = m_cAmbientColor.GetValue().GetBlue();
   params[3] = 1.0f;
   glLightfv( GL_LIGHT0 + nIndex, GL_AMBIENT, params );

   params[0] = m_cDiffuseColor.GetValue().GetRed(); params[1] = m_cDiffuseColor.GetValue().GetGreen(); params[2] = m_cDiffuseColor.GetValue().GetBlue();
   glLightfv( GL_LIGHT0 + nIndex, GL_DIFFUSE, params );

   params[0] = m_cSpecularColor.GetValue().GetRed(); params[1] = m_cSpecularColor.GetValue().GetGreen(); params[2] = m_cSpecularColor.GetValue().GetBlue();
   glLightfv( GL_LIGHT0 + nIndex, GL_SPECULAR, params );

   glLightf( GL_LIGHT0 + nIndex, GL_SPOT_EXPONENT, m_fSpotExponent );

   glLightf( GL_LIGHT0 + nIndex, GL_CONSTANT_ATTENUATION, m_vAttenuation.GetValue().x );
   glLightf( GL_LIGHT0 + nIndex, GL_LINEAR_ATTENUATION, m_vAttenuation.GetValue().y );
   glLightf( GL_LIGHT0 + nIndex, GL_QUADRATIC_ATTENUATION, m_vAttenuation.GetValue().z );

   glPopMatrix();
}

void Light::UnBind()
{
}

void Light::Draw( CGLBaseView *pBaseView )
{
   TransformableObjectAttribsPusher glattribs( this, pBaseView );
   //glPushAttrib( GL_LIGHTING_BIT | GL_ENABLE_BIT );

   glEnable( GL_CULL_FACE );

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

   switch (m_nType)
   {
      case TYPE_DIRECTIONAL:
         glScalef( 0.2, 0.2, 0.2 );
         glCallList( m_glSpotIconCallList );
         break;
      case TYPE_POINT:
//         glScalef( 5, 5, 5 );
         glScalef( 0.2, 0.2, 0.2 );
         glCallList( m_glSpotIconCallList );
         break;
      case TYPE_SPOT:
         glScalef( 0.2, 0.2, 0.2 );
         glCallList( m_glSpotIconCallList );
         glScalef( 1.0/0.2, 1.0/0.2, 1.0/0.2 );

         // cone
         float fDistance = 100.0f;
         glLineWidth( 1.0f );
         glDisable( GL_LIGHTING );
         glColor3f( m_cDiffuseColor.GetValue().GetRed(), m_cDiffuseColor.GetValue().GetGreen(), m_cDiffuseColor.GetValue().GetBlue() );
         glBegin( GL_LINES );
            glVertex3f( 0,0,0 );
            glVertex3f( 16,9,-fDistance );
            glVertex3f( 0,0,0 );
            glVertex3f( 16,-9,-fDistance );
            glVertex3f( 0,0,0 );
            glVertex3f( -16,-9,-fDistance );
            glVertex3f( 0,0,0 );
            glVertex3f( -16,9,-fDistance );
         glEnd();
         break;
   }

   //glPopAttrib();

   SceneObject::Draw( pBaseView );
}

BOOL Light::GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList )
{
   if (pPointerProperty != &m_pSceneObjectCache)
      return TRUE;

   Project *pProject = GetMainInterface()->GetProject();
   if (!pProject)
   {
      ASSERT( FALSE );
      return TRUE;
   }

   LightContainer *pLightContainer = &pProject->m_lightcontainer;
   for (UINT i=0; i<pLightContainer->GetNumLights(); i++)
   {
      Light *pLight = pLightContainer->GetLightAt( i );
      pSnipeObjectList->AddTail( pLight );
   }

   return TRUE;
}

/////////////////////////////////////////////////////
// LightContainer

IMPLEMENT_SNIPEOBJECT_CREATE( LightContainer )

LightContainer::LightContainer()
{
   m_strName = "Lights";
}

LightContainer::~LightContainer()
{
}
