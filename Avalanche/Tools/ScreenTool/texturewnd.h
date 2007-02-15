#ifndef _TEXTUREWND_H_
#define _TEXTUREWND_H_

#define TEXNOTIFY_UPDATE	(0x100)

class TextureWnd : public CWnd
{
	public:
		TextureWnd( void );
		virtual ~TextureWnd( void );
		virtual BOOL CreateTexture( BITMAP2 *pBitmap );
		virtual BOOL CreateTexture( LPDIRECT3DSURFACE9 pSurface );
		virtual BOOL CreateTexture( LPDIRECT3DTEXTURE9 pTexture );
		virtual void DestroyTexture( void );

		virtual void SetTextureCoordinates( int Vertex, float x, float y );
		virtual void GetTextureCoordinates( int Vertex, float &x, float &y );

		float m_VertexX[4];
		float m_VertexY[4];

		int m_SelectedVertex;

	public:
		typedef enum
		{
			MOUSEMODE_HOVER,
			MOUSEMODE_DRAG,
		}
		MOUSEMODE;

		MOUSEMODE m_MouseMode;
		
	protected:
		afx_msg void OnLButtonDown( UINT Flags, CPoint p );
		afx_msg void OnLButtonUp( UINT Flags, CPoint p );
		afx_msg void OnMouseMove( UINT Flags, CPoint p );
		afx_msg void OnPaint( void );

	protected:
		BITMAP2 *m_pTexturemap;

	DECLARE_MESSAGE_MAP();
};

#endif // _TEXTUREWND_H_