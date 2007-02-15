#pragma once

class GLImageList 
{
private:
   GLuint   m_glDisplayListBase;
   GLuint   m_glTexture;
   UINT     m_nWidthEach, m_nHeight;
   UINT     m_nTextureWidth, m_nTextureHeight;
   UINT     m_nCount;
   CRect    m_rcDice;
   GLenum   m_nTextureType;
   RGBFloat m_cTint;

public:
    GLImageList();
   ~GLImageList();

// Operations
   BOOL Create( UINT nBitmapID, int cx=0, HINSTANCE hInstance=NULL );
   BOOL Create( UINT nBitmapID, const CRect &rcDice, int cx=0, HINSTANCE hInstance=NULL );
   void Clear(); // clears the members so that they won't be destroyed
   void Destroy();
   BOOL Draw( float x=0.0f, float y=0.0f, UINT nImage=0, float fAlpha=1.0f );
   BOOL Draw( const CRect &rc, BOOL bTileHorz=FALSE, BOOL bTileVert=FALSE, UINT nImage=0 );
   BOOL DrawDiced( const CRect &rc, UINT nImage=0 );
   RGBFloat GetTint() { return m_cTint; }
   RGBFloat SetTint( const RGBFloat &cTint ) { RGBFloat cOldTint=m_cTint; m_cTint=cTint; return cOldTint; }

   BOOL IsEmpty() const { return m_nCount == 0; }
   UINT GetImageCount() const { return m_nCount; }
   UINT GetWidthEach() const { return m_nWidthEach; }
   UINT GetWidth() const { return m_nWidthEach * m_nCount; }
   UINT GetHeight() const { return m_nHeight; }
   const CRect &GetDiceRect() const { return m_rcDice; }
   void SetDiceRect( const CRect &rcDice ) { m_rcDice = rcDice; }

   BOOL Bind();
   BOOL UnBind();
   void DrawQuad( const Vector &p1, const Vector &p2, UINT nImage=0 );

private:
   void DrawChunk( const CRect &rcSrc, const CRect &rcDest );
};

