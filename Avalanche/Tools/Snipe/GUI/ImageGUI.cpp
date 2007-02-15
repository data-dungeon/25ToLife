
#include "stdafx.h"
#include "GLBaseFrame.h"
#include "SnipeDocTemplate.h"
#include "Image.h"
#include "ImageContainer.h"
#include "Snipe.h"
#include "ImageDoc.h"
#include "MainFrm.h"

//////////////////////////////////////////////////////////////////////////////////
// Image

void Image::Draw( CGLBaseView *pView )
{
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glScalef( 1.0f, -1.0f, 1.0f );

   Draw( pView, Vector2(-0.5f,-0.5f), Vector2(1.0f, 1.0f));

   glPopMatrix();
}

void Image::Draw( CGLControl *pControl, const Vector &vPos, const Vector2 &v2Size, float fOpacity/*=1.0f*/ )
{
   pControl->MakeRenderContextCurrent();

#if GLTEXTURETYPE == GL_TEXTURE_RECTANGLE_ARB
   float fWidth = (float)GetWidth();
   float fHeight = (float)GetHeight();
#else
   float fWidth = 1.0f;
   float fHeight = 1.0f;
#endif

   GLuint nIndex = GetGLTexture(BitmapObject::IMAGE_BUFFER);

   glBindTexture( GLTEXTURETYPE, nIndex );
   glEnable( GLTEXTURETYPE );
   glColor4f( 1, 1, 1, fOpacity );

   glBegin( GL_QUADS );
      glTexCoord2f( 0, 0 );
      glVertex3fv( &vPos.x );

      glTexCoord2f( fWidth, 0 );
      glVertex3f( vPos.x + v2Size.x, vPos.y, vPos.z );

      glTexCoord2f( fWidth, fHeight );
      glVertex3f( vPos.x + v2Size.x, vPos.y + v2Size.y, vPos.z );

      glTexCoord2f( 0, fHeight );
      glVertex3f( vPos.x, vPos.y + v2Size.y, vPos.z );
   glEnd();

   glDisable( GLTEXTURETYPE );
   glBindTexture( GLTEXTURETYPE, 0 );
}

int Image::GetItemHeight()
{
   static int nItemHeight = min(128, max(16, GetApp().GetProfileInt( "General", "Image Icon Size", 48 )));
   return nItemHeight;
}

void Image::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   Draw( pItem, Vector2( x, 1 ), Vector2( pItem->m_szControl.cy-2, pItem->m_szControl.cy-2 ));

   glColor3f( 0.1f, 0.1f, 0.1f );
   pItem->DrawText( x + pItem->m_szControl.cy-2 + 4, 12, GetTitle());

   glColor3f( 0.4f, 0.4f, 0.4f );
   pItem->DrawText( x + pItem->m_szControl.cy-2 + 4, 12 + 14, (String)GetWidth() + "x" + GetHeight() /*+ "x" + m_bitmapobject.GetBuffer(0)->m_nBufferChannels*8*/ );
}
