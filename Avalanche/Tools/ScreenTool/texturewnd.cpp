#include "pch.h"
#include "texturewnd.h"
#include "direct3d.h"

BEGIN_MESSAGE_MAP( TextureWnd, CWnd )
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

TextureWnd::TextureWnd( void )
{
	m_pTexturemap = 0;

	SetTextureCoordinates( 0, 0.0f, 0.0f );
	SetTextureCoordinates( 1, 0.0f, 0.0f );
	SetTextureCoordinates( 2, 0.0f, 0.0f );
	SetTextureCoordinates( 3, 0.0f, 0.0f );

	m_SelectedVertex = -1;
	m_MouseMode = MOUSEMODE_HOVER;
}

TextureWnd::~TextureWnd( void )
{
	m_pTexturemap = 0;
}

BOOL TextureWnd::CreateTexture( BITMAP2 *pBitmap )
{
	DestroyTexture();

	if( !pBitmap )
		return FALSE;

	int Width = pBitmap->Width;
	int Height = pBitmap->Height;

	m_pTexturemap = CreateBitmap2( Width, Height, pBitmap->Format, 0, 0 );

	if( !m_pTexturemap )
		return FALSE;

	BitmapToBitmap( m_pTexturemap, 0, 0, Width, Height, pBitmap, 0, 0, Width, Height ); 

	return TRUE;
}

BOOL TextureWnd::CreateTexture( LPDIRECT3DSURFACE9 pSurface )
{
	DestroyTexture();

	if( !pSurface )
		return FALSE;

	D3DSURFACE_DESC SurfaceDesc;
	HRESULT hresult = D3DRESULT( pSurface->GetDesc( &SurfaceDesc ) );

	if( FAILED(hresult) )
		return FALSE;

	int Width = SurfaceDesc.Width;
	int Height = SurfaceDesc.Height;
	PIXELFORMAT Format = PixelFormatFromDirectX( SurfaceDesc.Format );

	m_pTexturemap = CreateBitmap2( Width, Height, Format, 0, 0 );

	if( !m_pTexturemap )
		return FALSE;

	return d3d.SurfaceToBitmap( m_pTexturemap, 0, 0, Width, Height, pSurface, 0, 0, Width, Height );
}

BOOL TextureWnd::CreateTexture( LPDIRECT3DTEXTURE9 pTexture )
{
	DestroyTexture();

	if( !pTexture )
		return FALSE;

	LPDIRECT3DSURFACE9 pSurface = 0;
	
	HRESULT hresult = D3DRESULT( pTexture->GetSurfaceLevel( 0, &pSurface ) );

	if( FAILED(hresult) )
		return FALSE;

	BOOL bResult = CreateTexture( pSurface );

	pSurface->Release();

	return bResult;
}

void TextureWnd::DestroyTexture( void )
{
	if( m_pTexturemap )
		m_pTexturemap = DeleteBitmap2( m_pTexturemap );

	return;
}

void TextureWnd::SetTextureCoordinates( int Vertex, float x, float y )
{
	m_VertexX[Vertex] = x;
	m_VertexY[Vertex] = y;
}

void TextureWnd::GetTextureCoordinates( int Vertex, float &x, float &y )
{
	x = m_VertexX[Vertex];
	y = m_VertexY[Vertex];
}

void TextureWnd::OnLButtonDown( UINT Flags, CPoint p )
{
	SetCapture();

	if( m_SelectedVertex != -1 )
		m_MouseMode = MOUSEMODE_DRAG;

	return;
}

void TextureWnd::OnLButtonUp( UINT Flags, CPoint p )
{
	ReleaseCapture();

	m_MouseMode = MOUSEMODE_HOVER;
}

void TextureWnd::OnMouseMove( UINT Flags, CPoint p )
{
	RECT rc;
	GetClientRect( &rc );

	if( m_MouseMode == MOUSEMODE_HOVER )
	{
		int DistanceSquared;
		int MinDistanceSquared;
		int SelectedVertex;

		int Dx, Dy;
		int x, y;
		float cw = (float) (rc.right - rc.left);
		float ch = (float) (rc.bottom - rc.top);

		SelectedVertex = -1;
		MinDistanceSquared = 0x7fffffff;

		for( int i = 0; i < 4; i++ )
		{
			x = (int) (m_VertexX[i] * cw);
			y = (int) (m_VertexY[i] * ch);

			Dx = p.x - x;
			Dy = p.y - y;

			DistanceSquared = Dx*Dx + Dy*Dy;

			if( MinDistanceSquared > DistanceSquared )
			{
				MinDistanceSquared = DistanceSquared;

				if( MinDistanceSquared < 8*8 )
					SelectedVertex = i;
			}
		}

		if( m_SelectedVertex != SelectedVertex )
		{
			m_SelectedVertex = SelectedVertex;
			Invalidate( FALSE );
		}
	}
	else
	if( m_MouseMode == MOUSEMODE_DRAG )
	{
		if( m_SelectedVertex != -1 )
		{
			float x = p.x / (float) (rc.right - rc.left);
			float y = p.y / (float) (rc.bottom - rc.top);

			x = Math::Clamp( x );
			y = Math::Clamp( y );

			SetTextureCoordinates( m_SelectedVertex, x, y );

			CWnd *pParent = GetParent();

			if( pParent )
			{
				NMHDR nmhdr;

				nmhdr.hwndFrom = m_hWnd;
				nmhdr.idFrom = GetDlgCtrlID();
				nmhdr.code = TEXNOTIFY_UPDATE;
				
				pParent->SendMessage( WM_NOTIFY, (WPARAM) nmhdr.idFrom, (LPARAM) &nmhdr );

				Invalidate( FALSE );
			}
		}
	}

	return;
}

void TextureWnd::OnPaint( void )
{
	PAINTSTRUCT ps;
	CDC *pdc = BeginPaint( &ps );

	RECT rc;
	GetClientRect( &rc );

	if( m_pTexturemap )
	{
		BitmapToDC( pdc->m_hDC, 0, 0, rc.right, rc.bottom, m_pTexturemap, 0, 0, m_pTexturemap->Width, m_pTexturemap->Height );

		int x1, y1;
		int x2, y2;
		float cw = (float) (rc.right - rc.left);
		float ch = (float) (rc.bottom - rc.top);
		COLORREF c;

		for( int i = 0; i < 4; i++ )
		{
			x1 = (int) (m_VertexX[i] * cw);
			y1 = (int) (m_VertexY[i] * ch);
			x2 = (int) (m_VertexX[i+1&3] * cw);
			y2 = (int) (m_VertexY[i+1&3] * ch);

			c = 0xffffff;

			if( i == m_SelectedVertex )
				c = 0xff0000;

			GdiLine( pdc->m_hDC, x1, y1, x2, y2, 0xffffff );
			GdiFrame( pdc->m_hDC, x1-4, y1-4, 8, 8, c );
		}
	}
	else
	{
		GdiFillRect( pdc->m_hDC, rc, 0x000000 );

		pdc->SetTextColor( 0xffffff );
		pdc->SetBkMode( TRANSPARENT );
		GdiCenterText( pdc->m_hDC, rc.left, rc.top, rc.left+rc.right, rc.top+rc.bottom, "NO TEXTURE" );
	}

	EndPaint( &ps );
}