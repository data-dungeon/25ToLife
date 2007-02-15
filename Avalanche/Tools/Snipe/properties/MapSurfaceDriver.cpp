#include "stdafx.h"
#include "MapSurfaceDriver.h"
#include "Surface.h"
#include "resource.h."
#include "gui/GLControls.h"

/////////////////////////////////////////////////////
// MapSurfaceDriver

IMPLEMENT_SNIPEOBJECT_PURE( MapSurfaceDriver )

GLImageList MapSurfaceDriver::m_glImageListPivot;

static TypeProperty::Info::Node f_blendstyleinfos[] = {
   TypeProperty::Info::Node( "Replace", "Replace" ),
   TypeProperty::Info::Node( "Multiply", "Multiply" ),
   TypeProperty::Info::Node( "Add", "Add" ),
   TypeProperty::Info::Node( "Decal", "Decal" ),
};

ImagePointerProperty::Info MapSurfaceDriver::m_pImageInfo( "Image", "Image" );
TypeProperty::Info         MapSurfaceDriver::m_nBlendStyleInfo( "Blend Style", "BlendStyle", f_blendstyleinfos, sizeof(f_blendstyleinfos)/sizeof(TypeProperty::Info::Node), MapSurfaceDriver::DECAL );
FloatProperty::Info        MapSurfaceDriver::m_fOpacityInfo( "Opacity", "Opacity", 1.0f, 0.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT );

MapSurfaceDriver::MapSurfaceDriver()
{
   m_pImage.Init( &m_pImageInfo, this );
   m_nBlendStyle.Init( &m_nBlendStyleInfo, this );
   m_fOpacity.Init( &m_fOpacityInfo, this );
}

void MapSurfaceDriver::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   if (GetImage())
   {
      GetImage()->Draw( pItem, Vector2( x, 1 ), Vector2( pItem->m_szControl.cy-2, pItem->m_szControl.cy-2 ));

      glColor3f( 0.1f, 0.1f, 0.1f );
      pItem->DrawText( x + pItem->m_szControl.cy-2 + 4, 12, GetTitle());
   }
   else
      SurfaceDriver::DrawTreeItem( pItem, x );
}

void MapSurfaceDriver::Bind()
{
   if (m_pImage.Bind())
   {
      GLfloat params[4];

      glGetMaterialfv( GL_FRONT, GL_DIFFUSE, params );
      params[0] = params[1] = params[2] = 1.0f;
      params[3] *= m_fOpacity;
      glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, params );

      BlendStyle blendstyle = (BlendStyle)m_nBlendStyle.GetValue();
      //if ((nPass>1)&&(m_nBlendStyle==REPLACE))
      //   blendstyle = DECAL;
      DoStateChange( blendstyle, m_fOpacity );
   }
}

void MapSurfaceDriver::UnBind()
{
   m_pImage.UnBind();
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void MapSurfaceDriver::DoStateChange(const BlendStyle style,float opacity)
{
   switch(style)
   {
   case MULTIPLY :
      {
         const GLfloat envcolor[4]={1.0f,1.0f,1.0f,1.0f};
         glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,envcolor);
         glDisable(GL_LIGHTING);
         opacity = 1.0f-opacity;//invert opacity
         glColor4f(opacity,opacity,opacity,opacity);
         glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_BLEND);
         glBlendFunc(GL_DST_COLOR,GL_ZERO);
      }
      break;
   case ADDITIVE :
      {
         const GLfloat envcolor[4]={0.0f,0.0f,0.0f,1.0f};
         glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,envcolor);
         glDisable(GL_LIGHTING);
         glColor4f(opacity,opacity,opacity,opacity);
         glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_BLEND);
         glBlendFunc(GL_ONE,GL_ONE);
      }
      break;
   case DECAL :
      {
         const GLfloat envcolor[4]={1.0f,1.0f,1.0f,1.0f};
         glEnable(GL_LIGHTING);
         glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,envcolor);
//         glDisable(GL_LIGHTING);
//         glColor4d(0.0,0.0,0.0,opacity);
         glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
      }
      break;
   case REPLACE :
      {
         glEnable(GL_LIGHTING);
         //glBlendFunc(GL_ONE,GL_ZERO);
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);		
      }
   }
}

void MapSurfaceDriver::TransformUV( Vector2 &uv )
{
   uv -= m_pImage.m_v2Pivot.GetValue();
   uv *= m_pImage.m_transform.GetInverseMatrix();
   uv += m_pImage.m_v2Pivot.GetValue();

#if GLTEXTURETYPE == GL_TEXTURE_RECTANGLE_ARB
   Image *pImage = GetImage();
   if (pImage)
   {
      uv.x *= pImage->GetWidth();
      uv.y *= pImage->GetHeight();
   }
#endif
}

