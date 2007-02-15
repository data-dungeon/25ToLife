//

#include "stdafx.h"
#include "GLImageList.h"
#include "MainFrm.h"

//#define MIPMAPS

/////////////////////////////////////////////////////
// GLImageList

GLImageList::GLImageList()
{
   m_glDisplayListBase = 0;
   m_glTexture = 0;
   m_nWidthEach = 0;
   m_nCount = 0;
   m_cTint.Set( 1, 1, 1 );
}

GLImageList::~GLImageList()
{
   Destroy();
}

void GLImageList::Clear()
{
   m_glDisplayListBase = 0;
   m_glTexture = 0;
}

void GLImageList::Destroy()
{
   if (m_glDisplayListBase) {
      glDeleteLists( m_glDisplayListBase, m_nCount );
      m_glDisplayListBase = 0;
   }
   if (m_glTexture) {
      glDeleteTextures( 1, &m_glTexture );
      m_glTexture = 0;
   }
}

BOOL GLImageList::Create( UINT nBitmapID, int cx/*=0*/, HINSTANCE hInstance/*=NULL*/ )
{
   Destroy();

   CBitmap cbitmap;

   if (hInstance)
   {
      HBITMAP hBitmap = ::LoadBitmap( hInstance, MAKEINTRESOURCE(nBitmapID));
      if (!hBitmap)
         return FALSE;

      if (!cbitmap.Attach( hBitmap ))
         return FALSE;
   }
   else if (!cbitmap.LoadBitmap( nBitmapID ))
      return FALSE;

   BITMAP bitmap;
   memset( &bitmap, 0, sizeof(BITMAP));
   if (!cbitmap.GetBitmap( &bitmap ))
      return FALSE;

   int nBytes = bitmap.bmWidthBytes * bitmap.bmHeight;
   char *lpBits = SNEW char[ nBytes ];
   if (cbitmap.GetBitmapBits( nBytes, lpBits ) == 0) {
      delete [] lpBits;
      return FALSE;
   }

   GLContextPusher contextpusher(&GetMainFrame()->m_glSharedRenderWnd);

   m_nTextureWidth = bitmap.bmWidth;
   m_nTextureHeight = bitmap.bmHeight;

   GLubyte *pTextureData = SNEW GLubyte[ 4 * m_nTextureWidth * m_nTextureHeight ];

   for (int y=0; y<bitmap.bmHeight; y++) {
      ULONG *src = (ULONG *)(lpBits + y * bitmap.bmWidthBytes);
      ULONG *dst = (ULONG *)(pTextureData + y * (4*m_nTextureWidth));
      for (int x=0; x<bitmap.bmWidth; x++, src++, dst++)
         *dst = *src;
   }
   delete [] lpBits;

   m_nTextureType = GL_TEXTURE_RECTANGLE_ARB;

   // AARRGGBB
   glEnable( m_nTextureType );
	glGenTextures( 1, &m_glTexture );
   glBindTexture( m_nTextureType, m_glTexture );
	glTexParameteri( m_nTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
#ifdef MIPMAPS
   glTexParameteri( m_nTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

   gluBuild2DMipmaps( m_nTextureType, GL_RGBA, m_nTextureWidth, m_nTextureHeight,
      GL_BGRA_EXT, GL_UNSIGNED_BYTE, pTextureData );
#else
   glTexParameteri( m_nTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

   glTexImage2D( m_nTextureType, 0, GL_RGBA, m_nTextureWidth, m_nTextureHeight,
      0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pTextureData );
#endif

   delete [] pTextureData;

   if (cx == 0)
      cx = bitmap.bmWidth;

   m_nWidthEach = cx;
   m_nHeight = bitmap.bmHeight;
   m_nCount = bitmap.bmWidth / m_nWidthEach;

   m_glDisplayListBase = glGenLists( m_nCount );
   for (UINT i=0; i<m_nCount; i++) {
	   glNewList( m_glDisplayListBase + i, GL_COMPILE );
	      glBindTexture( m_nTextureType, m_glTexture );
	      glBegin( GL_QUADS );
	         glTexCoord2d( i * m_nWidthEach, 0 ); glVertex2f( 0, 0 );
	         glTexCoord2d( (i+1) * m_nWidthEach, 0 ); glVertex2f( m_nWidthEach, 0 );
	         glTexCoord2d( (i+1) * m_nWidthEach, m_nHeight ); glVertex2f( m_nWidthEach, m_nHeight );
	         glTexCoord2d( i * m_nWidthEach, m_nHeight ); glVertex2f( 0, m_nHeight );
	      glEnd();
	   glEndList();
   }

   return TRUE;
}

BOOL GLImageList::Create( UINT nBitmapID, const CRect &rcDice, int cx/*=0*/, HINSTANCE hInstance/*=NULL*/ )
{
   m_rcDice = rcDice;
   return Create( nBitmapID, cx, hInstance );
}

BOOL GLImageList::Bind()
{
   if (m_glTexture)
   {
      glEnable( m_nTextureType );
      glBindTexture( m_nTextureType, m_glTexture );
      return TRUE;
   }
   return FALSE;
}

BOOL GLImageList::UnBind()
{
   if (m_glTexture)
   {
      glBindTexture( m_nTextureType, 0 );
      glDisable( m_nTextureType );
      return TRUE;
   }
   return FALSE;
}

void GLImageList::DrawQuad( const Vector &p1, const Vector &p2, UINT nImage/*=0*/ )
{
   glBegin( GL_QUADS );
      glTexCoord2d( nImage    * m_nWidthEach, 0 );         glVertex3f( p1.x, p1.y, p1.z );
      glTexCoord2d((nImage+1) * m_nWidthEach, 0 );         glVertex3f( p2.x, p1.y, p1.z );
      glTexCoord2d((nImage+1) * m_nWidthEach, m_nHeight ); glVertex3f( p2.x, p2.y, p1.z );
      glTexCoord2d( nImage    * m_nWidthEach, m_nHeight ); glVertex3f( p1.x, p2.y, p1.z );
   glEnd();
}

BOOL GLImageList::Draw( float x, float y, UINT nImage/*=0*/, float fAlpha/*=1.0f*/ )
{
   ASSERT( m_glDisplayListBase );
   ASSERT( nImage >= 0 && nImage < m_nCount );

	glEnable( m_nTextureType );

   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glTranslatef( x, y, 0 );
   RGBAFloat oldcolor;
   glGetFloatv( GL_CURRENT_COLOR, (GLfloat *)&oldcolor );
   glColor3f( m_cTint.GetRed(), m_cTint.GetGreen(), m_cTint.GetBlue() );
   glCallList( m_glDisplayListBase + nImage );
   glPopMatrix();

   glColor4fv( (GLfloat *)&oldcolor );
   glDisable( m_nTextureType );

   return TRUE;
}

BOOL GLImageList::Draw( const CRect &rc, BOOL bTileHorz/*=FALSE*/, BOOL bTileVert/*=FALSE*/, UINT nImage/*=0*/ )
{
   ASSERT( m_glDisplayListBase );
   ASSERT( nImage >= 0 && nImage < m_nCount );
   if (rc.top > rc.bottom)
      return FALSE;

	glEnable( m_nTextureType );

	glBindTexture( m_nTextureType, m_glTexture );
   RGBAFloat oldcolor;
   glGetFloatv( GL_CURRENT_COLOR, (GLfloat *)&oldcolor );
   glColor3fv( (GLfloat *)&m_cTint );

   UINT nStampWidthEach = bTileHorz ? m_nWidthEach : rc.Width();
   UINT nStampHeightEach = bTileVert ? m_nHeight : rc.Height();
   UINT nWidth, nHeight, nStampWidth, nStampHeight;

   glBegin( GL_QUADS );
   for (UINT y=rc.top; y<(UINT)rc.bottom; y+=nStampHeightEach) {
		if (bTileVert && m_nHeight > rc.bottom-y) {
         nStampHeight = nHeight = rc.bottom-y;
		}
		else {
         nHeight = m_nHeight;
			nStampHeight = nStampHeightEach;
		}
      for (UINT x=rc.left; x<(UINT)rc.right; x+=nStampWidthEach) {
			if (bTileHorz && m_nWidthEach > rc.right-x) {
            nStampWidth = nWidth = rc.right-x;
			}
			else {
				nStampWidth = nStampWidthEach;
            nWidth = m_nWidthEach;
			}
	      glTexCoord2d( nImage * m_nWidthEach,          0       ); glVertex2i( x,             y );
	      glTexCoord2d( nImage * m_nWidthEach + nWidth, 0       ); glVertex2i( x+nStampWidth, y );
	      glTexCoord2d( nImage * m_nWidthEach + nWidth, nHeight ); glVertex2i( x+nStampWidth, y+nStampHeight );
	      glTexCoord2d( nImage * m_nWidthEach,          nHeight ); glVertex2i( x,             y+nStampHeight );
      }
   }
   glEnd();

   glColor4fv( (GLfloat *)&oldcolor );
   glBindTexture( m_nTextureType, 0 );
   glDisable( m_nTextureType );

   return TRUE;
}

void GLImageList::DrawChunk( const CRect &rcSrc, const CRect &rcDest )
{
   if (!rcDest.Width() || !rcDest.Height())
      return;
	glTexCoord2d( rcSrc.left,  rcSrc.top );    glVertex2i( rcDest.left,  rcDest.top );
	glTexCoord2d( rcSrc.right, rcSrc.top );    glVertex2i( rcDest.right, rcDest.top );
	glTexCoord2d( rcSrc.right, rcSrc.bottom ); glVertex2i( rcDest.right, rcDest.bottom);
	glTexCoord2d( rcSrc.left,  rcSrc.bottom ); glVertex2i( rcDest.left,  rcDest.bottom );
}

BOOL GLImageList::DrawDiced( const CRect &rc, UINT nImage/*=0*/ )
{
   ASSERT( m_glDisplayListBase );
   ASSERT( nImage >= 0 && nImage < m_nCount );
   if (rc.top > rc.bottom)
      return FALSE;

	glEnable( m_nTextureType );

	glBindTexture( m_nTextureType, m_glTexture );
   RGBAFloat oldcolor;
   glGetFloatv( GL_CURRENT_COLOR, (GLfloat *)&oldcolor );
   glColor3fv( (GLfloat *)&m_cTint );
   int nImageLeft = nImage * m_nWidthEach;
   glBegin( GL_QUADS );
      // top (left to right)
      DrawChunk( CRect(nImageLeft,                  0, nImageLeft + m_rcDice.left,  m_rcDice.top),               CRect(rc.left,                                    rc.top, rc.left + m_rcDice.left,                    rc.top + m_rcDice.top) );
      DrawChunk( CRect(nImageLeft + m_rcDice.left,  0, nImageLeft + m_rcDice.right, m_rcDice.top),               CRect(rc.left + m_rcDice.left,                    rc.top, rc.right - (m_nWidthEach - m_rcDice.right), rc.top + m_rcDice.top) );
      DrawChunk( CRect(nImageLeft + m_rcDice.right, 0, nImageLeft + m_nWidthEach, m_rcDice.top),                 CRect(rc.right - (m_nWidthEach - m_rcDice.right), rc.top, rc.right,                                   rc.top + m_rcDice.top) );
      // middle (left to right)
      DrawChunk( CRect(nImageLeft,                  m_rcDice.top, nImageLeft + m_rcDice.left,  m_rcDice.bottom), CRect(rc.left,                                    rc.top + m_rcDice.top, rc.left + m_rcDice.left,                    rc.bottom - (m_nHeight - m_rcDice.bottom)) );
      DrawChunk( CRect(nImageLeft + m_rcDice.left,  m_rcDice.top, nImageLeft + m_rcDice.right, m_rcDice.bottom), CRect(rc.left + m_rcDice.left,                    rc.top + m_rcDice.top, rc.right - (m_nWidthEach - m_rcDice.right), rc.bottom - (m_nHeight - m_rcDice.bottom)) );
      DrawChunk( CRect(nImageLeft + m_rcDice.right, m_rcDice.top, nImageLeft + m_nWidthEach,   m_rcDice.bottom), CRect(rc.right - (m_nWidthEach - m_rcDice.right), rc.top + m_rcDice.top, rc.right,                                 rc.bottom - (m_nHeight - m_rcDice.bottom)) );
      // bottom (left to right)
      DrawChunk( CRect(nImageLeft,                  m_rcDice.bottom, nImageLeft + m_rcDice.left,  m_nHeight),    CRect(rc.left,                                    rc.bottom - (m_nHeight - m_rcDice.bottom), rc.left + m_rcDice.left,                    rc.bottom) );
      DrawChunk( CRect(nImageLeft + m_rcDice.left,  m_rcDice.bottom, nImageLeft + m_rcDice.right, m_nHeight),    CRect(rc.left + m_rcDice.left,                    rc.bottom - (m_nHeight - m_rcDice.bottom), rc.right - (m_nWidthEach - m_rcDice.right), rc.bottom) );
      DrawChunk( CRect(nImageLeft + m_rcDice.right, m_rcDice.bottom, nImageLeft + m_nWidthEach,   m_nHeight),    CRect(rc.right - (m_nWidthEach - m_rcDice.right), rc.bottom - (m_nHeight - m_rcDice.bottom), rc.right,                                 rc.bottom) );
   glEnd();

   glColor4fv( (GLfloat *)&oldcolor );
   glBindTexture( m_nTextureType, 0 );
   glDisable( m_nTextureType );

   return TRUE;
}
