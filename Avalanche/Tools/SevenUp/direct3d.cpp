#include "pch.h"
#include "direct3d.h"

CDirect3D d3d;

//=============================================================================
// PixelFormatFromDirectX()
//=============================================================================

PIXELFORMAT PixelFormatFromDirectX( D3DFORMAT Format )
{
	switch( Format )
	{
	case D3DFMT_A8R8G8B8:
		return PIXELFMT_A8R8G8B8;
	case D3DFMT_X8R8G8B8:
		return PIXELFMT_X8R8G8B8;
	case D3DFMT_R8G8B8:
		return PIXELFMT_R8G8B8;
	case D3DFMT_A4R4G4B4:
		return PIXELFMT_A4R4G4B4;
	case D3DFMT_X4R4G4B4:
		return PIXELFMT_X4R4G4B4;
	case D3DFMT_A1R5G5B5:
		return PIXELFMT_A1R5G5B5;
	case D3DFMT_X1R5G5B5:
		return PIXELFMT_X1R5G5B5;
	case D3DFMT_R5G6B5:
		return PIXELFMT_R5G6B5;
	case D3DFMT_P8:
		return PIXELFMT_PALETTE8;
	}

	return PIXELFMT_UNKNOWN;
}

//=============================================================================
// PixelFormatToDirectX()
//=============================================================================

D3DFORMAT PixelFormatToDirectX( PIXELFORMAT Format )
{
	switch( Format )
	{
	case PIXELFMT_A8R8G8B8:
		return D3DFMT_A8R8G8B8;
	case PIXELFMT_X8R8G8B8:
		return D3DFMT_X8R8G8B8;
	case PIXELFMT_R8G8B8:
		return D3DFMT_R8G8B8;
	case PIXELFMT_A4R4G4B4:
		return D3DFMT_A4R4G4B4;
	case PIXELFMT_X4R4G4B4:
		return D3DFMT_X4R4G4B4;
	case PIXELFMT_A1R5G5B5:
		return D3DFMT_A1R5G5B5;
	case PIXELFMT_X1R5G5B5:
		return D3DFMT_X1R5G5B5;
	case PIXELFMT_R5G6B5:
		return D3DFMT_R5G6B5;
	case PIXELFMT_PALETTE8:
		return D3DFMT_P8;
	}

	return D3DFMT_UNKNOWN;
}

//=============================================================================
// CDirect3D::CDirect3D()
//=============================================================================

CDirect3D::CDirect3D( void )
{
	m_pDirect3D = 0;
	m_pDirect3DDevice = 0;
}

//=============================================================================
// CDirect3D::~CDirect3D()
//=============================================================================

CDirect3D::~CDirect3D()
{
	Terminate();
}

//=============================================================================
// CDirect3D::Initialize()
//=============================================================================

BOOL CDirect3D::Initialize( HWND hWnd, int Width, int Height, D3DFORMAT Format, BOOL Windowed )
{
	if( !m_pDirect3D )
	{
		m_pDirect3D = Direct3DCreate9( D3D_SDK_VERSION );

		if( !m_pDirect3D )
			return FALSE;
	}

	if( !m_pDirect3DDevice )
	{
		D3DDISPLAYMODE DisplayMode;

		HRESULT hresult = D3DRESULT( m_pDirect3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &DisplayMode ) );

		if( FAILED(hresult) )
			return FALSE;

		if( Windowed )
		{
			RECT rc = Win32_GetClientRect( hWnd );

			if( Width == 0 )
				Width = rc.right - rc.left;

			if( Height == 0 )
				Height = rc.bottom - rc.top;
		}

		if( Format == D3DFMT_UNKNOWN )
			Format = DisplayMode.Format;

		if( Width == 0 )
			Width = DisplayMode.Width;

		if( Height == 0 )
			Height = DisplayMode.Height;

		m_PresentParameters.BackBufferWidth				= Width;
		m_PresentParameters.BackBufferHeight			= Height;
		m_PresentParameters.BackBufferFormat			= Format;
		m_PresentParameters.BackBufferCount				= 1;
		m_PresentParameters.MultiSampleType				= D3DMULTISAMPLE_NONE;
		m_PresentParameters.SwapEffect					= D3DSWAPEFFECT_DISCARD;
		m_PresentParameters.hDeviceWindow				= hWnd;
		m_PresentParameters.Windowed						= Windowed;
		m_PresentParameters.EnableAutoDepthStencil	= TRUE;
		m_PresentParameters.AutoDepthStencilFormat	= D3DFMT_D24X8;
		m_PresentParameters.Flags							= 0;
		m_PresentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

		hresult = D3DRESULT( m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_PresentParameters, &m_pDirect3DDevice ) );

		if( FAILED(hresult) )
		{
			hresult = D3DRESULT( m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_PresentParameters, &m_pDirect3DDevice ) );

			if( FAILED(hresult) )
			{
				hresult = D3DRESULT( m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_PresentParameters, &m_pDirect3DDevice ) );

				if( FAILED(hresult) )
					return FALSE;
			}
		}

		D3DRESULT( Clear( 0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x000000, 0.0f, 0 ) );
	}

	return TRUE;
}

//=============================================================================
// CDirect3D::Terminate()
//=============================================================================

void CDirect3D::Terminate( void )
{
	if( m_pDirect3DDevice )
	{
		m_pDirect3DDevice->Release();
		m_pDirect3DDevice = 0;
	}

	if( m_pDirect3D )
	{
		m_pDirect3D->Release();
		m_pDirect3D = 0;
	}

	return;
}

//=============================================================================
// CDirect3D::BeginScene()
//=============================================================================

HRESULT CDirect3D::BeginScene( void )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->BeginScene() );
}

//=============================================================================
// CDirect3D::Clear()
//=============================================================================

HRESULT CDirect3D::Clear( DWORD Count,  D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->Clear( Count, pRects, Flags, Color, Z, Stencil ) );
}

//=============================================================================
// CDirect3D::CreateAdditionalSwapChain()
//=============================================================================

HRESULT CDirect3D::CreateAdditionalSwapChain( D3DPRESENT_PARAMETERS *pPresentParameters, LPDIRECT3DSWAPCHAIN9 *ppSwapChain )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->CreateAdditionalSwapChain( pPresentParameters, ppSwapChain ) );
}

//=============================================================================
// CDirect3D::CreateDepthStencilSurface()
//=============================================================================

HRESULT CDirect3D::CreateDepthStencilSurface( UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, LPDIRECT3DSURFACE9 *ppSurface, HANDLE *pSharedHandle )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->CreateDepthStencilSurface( Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle ) );
}

//=============================================================================
// CDirect3D::CreateIndexBuffer()
//=============================================================================

HRESULT CDirect3D::CreateIndexBuffer( UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DINDEXBUFFER9 *ppIndexBuffer, HANDLE *pSharedHandle )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->CreateIndexBuffer( Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle ) );
}

//=============================================================================
// CDirect3D::CreateRenderTarget()
//=============================================================================

HRESULT CDirect3D::CreateRenderTarget(	UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, LPDIRECT3DSURFACE9 *ppSurface, HANDLE* pSharedHandle )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->CreateRenderTarget( Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle ) );
}

//=============================================================================
// CDirect3D::CreateTexture()
//=============================================================================

HRESULT CDirect3D::CreateTexture( UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 *ppTexture, HANDLE *pSharedHandle )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->CreateTexture( Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle ) );
}

//=============================================================================
// CDirect3D::CreateVertexBuffer()
//=============================================================================

HRESULT CDirect3D::CreateVertexBuffer( UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, LPDIRECT3DVERTEXBUFFER9 *ppVertexBuffer, HANDLE *pSharedHandle )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->CreateVertexBuffer( Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle ) );
}

//=============================================================================
// CDirect3D::CreateIndexedPrimitive()
//=============================================================================

HRESULT CDirect3D::DrawIndexedPrimitive( D3DPRIMITIVETYPE PrimitiveType, int BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->DrawIndexedPrimitive( PrimitiveType, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount ) );
}

//=============================================================================
// CDirect3D::DrawPrimtive()
//=============================================================================

HRESULT CDirect3D::DrawPrimitive( D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->DrawPrimitive( PrimitiveType, StartVertex, PrimitiveCount ) );
}

//=============================================================================
// HRESULT CDirect3D::EndScene()
//=============================================================================

HRESULT CDirect3D::EndScene( void )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->EndScene() );
}

//=============================================================================
// CDirect3D::GetAdapterDisplayMode()
//=============================================================================

HRESULT CDirect3D::GetAdapterDisplayMode( UINT Adapter, D3DDISPLAYMODE *pDisplayMode )
{
	if( !m_pDirect3D )
		return E_FAIL;

	return D3DRESULT( m_pDirect3D->GetAdapterDisplayMode( Adapter, pDisplayMode ) );
}

//=============================================================================
// CDirect3D::GetDepthStencilSurface()
//=============================================================================

HRESULT CDirect3D::GetDepthStencilSurface( LPDIRECT3DSURFACE9 *ppSurface )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->GetDepthStencilSurface( ppSurface ) );
}

//=============================================================================
// CDirect3D::GetDisplayMode()
//=============================================================================

HRESULT CDirect3D::GetDisplayMode( UINT SwapChain, D3DDISPLAYMODE *pDisplayMode )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->GetDisplayMode( SwapChain, pDisplayMode ) );
}

//=============================================================================
// CDirect3D::GetRenderState()
//=============================================================================

HRESULT CDirect3D::GetRenderState( D3DRENDERSTATETYPE State, DWORD *pValue )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->GetRenderState( State, pValue ) );
}

//=============================================================================
// CDirect3D::GetRenderTarget()
//=============================================================================

HRESULT CDirect3D::GetRenderTarget(	 DWORD RenderTargetIndex, LPDIRECT3DSURFACE9 *ppRenderTarget )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->GetRenderTarget( RenderTargetIndex, ppRenderTarget ) );
}

//=============================================================================
// CDirect3D::GetStreamSource()
//=============================================================================

HRESULT CDirect3D::GetStreamSource( UINT StreamNumber, LPDIRECT3DVERTEXBUFFER9 *ppStreamData, UINT *pOffsetInBytes, UINT *pStride )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->GetStreamSource( StreamNumber, ppStreamData, pOffsetInBytes, pStride ) );
}

//=============================================================================
// CDirect3D::GetTexture()
//=============================================================================

HRESULT CDirect3D::GetTexture( DWORD Stage, LPDIRECT3DBASETEXTURE9 *ppTexture )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->GetTexture( Stage, ppTexture ) );
}

//=============================================================================
// CDirect3D::GetTextureStageState()
//=============================================================================

HRESULT CDirect3D::GetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->GetTextureStageState( Stage, Type, pValue ) );
}

//=============================================================================
// CDirect3D::GetVertexShader()
//=============================================================================

HRESULT CDirect3D::GetVertexShader(	LPDIRECT3DVERTEXSHADER9* ppShader)
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->GetVertexShader( ppShader ) );
}

//=============================================================================
// CDirect3D::MultiplyTransform
//=============================================================================

HRESULT CDirect3D::MultiplyTransform( D3DTRANSFORMSTATETYPE State, D3DMATRIX *pTransform )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->MultiplyTransform( State, pTransform ) );
}

//=============================================================================
// CDirect3D::Present()
//=============================================================================

HRESULT CDirect3D::Present( RECT *pSrc, RECT *pDst, HWND hWndOverride, RGNDATA *pDirtyRegion )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->Present( pSrc, pDst, hWndOverride, pDirtyRegion ) );
}

//=============================================================================
// CDirect3D::SetFVF
//=============================================================================

HRESULT CDirect3D::SetFVF(	DWORD FVF )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->SetFVF( FVF ) );
}

//=============================================================================
// CDirect3D::SetRenderState()
//=============================================================================

HRESULT CDirect3D::SetRenderState( D3DRENDERSTATETYPE State, DWORD Value )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->SetRenderState( State, Value ) );
}

//=============================================================================
// CDirect3D::SetRenderState()
//=============================================================================

HRESULT CDirect3D::SetRenderTarget( UINT RenderTargetIndex, LPDIRECT3DSURFACE9 pRenderTarget )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->SetRenderTarget( RenderTargetIndex, pRenderTarget ) );
}

//=============================================================================
// CDirect3D::SetSamplerState
//=============================================================================

HRESULT CDirect3D::SetSamplerState(	UINT Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->SetSamplerState( Sampler, Type, Value ) );
}

//=============================================================================
// CDirect3D::SetStreamSource()
//=============================================================================

HRESULT CDirect3D::SetStreamSource( UINT Stream, LPDIRECT3DVERTEXBUFFER9 pStreamData, UINT OffsetInBytes, UINT Stride )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->SetStreamSource( Stream, pStreamData, OffsetInBytes, Stride ) );
}

//=============================================================================
// CDirect3D:SetTransform()
//=============================================================================

HRESULT CDirect3D::SetTransform( D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->SetTransform( State, pMatrix ) );
}

//=============================================================================
// CDirect3D::SetTexture()
//=============================================================================

HRESULT CDirect3D::SetTexture( DWORD Stage, LPDIRECT3DTEXTURE9 pTexture )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->SetTexture( Stage, pTexture ) );
}

//=============================================================================
// CDirect3D::SetTextureStageState()
//=============================================================================

HRESULT CDirect3D::SetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->SetTextureStageState( Stage, Type, Value ) );
}

//=============================================================================
// CDirect3D::SetVertexShader()
//=============================================================================

HRESULT CDirect3D::SetVertexShader( LPDIRECT3DVERTEXSHADER9 pShader )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->SetVertexShader( pShader ) );
}

//=============================================================================
// CDirect3D::SetViewport
//=============================================================================

HRESULT CDirect3D::SetViewport( CONST D3DVIEWPORT9 *pViewport )
{
	if( !m_pDirect3DDevice )
		return E_FAIL;

	return D3DRESULT( m_pDirect3DDevice->SetViewport( pViewport ) );
}

//=============================================================================
// CDirect3D::CreateTextureFromBitmap()
//=============================================================================

LPDIRECT3DTEXTURE9 CDirect3D::CreateTextureFromBitmap( BITMAP2 *pBitmap, int Width, int Height, D3DFORMAT Format, D3DPOOL Pool )
{
	LPDIRECT3DTEXTURE9 pTexture = 0;

	if( pBitmap )
	{
		if( Width == 0 )
			Width = pBitmap->Width;

		if( Height == 0 )
			Height = pBitmap->Height;

		HRESULT hresult = D3DRESULT( CreateTexture( Width, Height, 0, 0, Format, Pool, &pTexture, 0 ) );

		if( !FAILED(hresult) )
			BitmapToTexture( pTexture, 0, 0, Width, Height, pBitmap, 0, 0, pBitmap->Width, pBitmap->Height );
	}

	return pTexture;
}

//=============================================================================
// CDirect3D::BitmapToTexture()
//=============================================================================

BOOL CDirect3D::BitmapToTexture( LPDIRECT3DTEXTURE9 pTexture, int Dx, int Dy, int Dw, int Dh, BITMAP2 *pBitmap, int Sx, int Sy, int Sw, int Sh )
{
	if( !pTexture || !pBitmap )
		return FALSE;

	LPDIRECT3DSURFACE9 pSurface;
	HRESULT hresult = D3DRESULT( pTexture->GetSurfaceLevel( 0, &pSurface ) );

	if( FAILED(hresult) )
		return FALSE;

	BOOL bResult = BitmapToSurface( pSurface, Dx, Dy, Dw, Dh, pBitmap, Sx, Sy, Sw, Sh );

	pSurface->Release();

	return bResult;
}

//=============================================================================
// CDirect3D::BitmapToSurface()
//=============================================================================

BOOL CDirect3D::BitmapToSurface( LPDIRECT3DSURFACE9 pSurface, int Dx, int Dy, int Dw, int Dh, BITMAP2 *pBitmap, int Sx, int Sy, int Sw, int Sh )
{
	if( !pSurface || !pBitmap )
		return FALSE;

	D3DLOCKED_RECT Lock;
	D3DSURFACE_DESC SurfaceDesc;
	HRESULT hresult;
	RECT rc;
	int x, y;
		
	hresult = D3DRESULT( pSurface->GetDesc( &SurfaceDesc ) );

	if( FAILED(hresult) )
		return FALSE;

	rc = GdiMakeRect( Dx, Dy, Dw, Dh );

	hresult = D3DRESULT( pSurface->LockRect( &Lock, &rc, 0 ) );

	if( FAILED(hresult) )
		return FALSE;

	PIXELFORMAT Format = PixelFormatFromDirectX( SurfaceDesc.Format );

	if( Format == PIXELFMT_UNKNOWN )
		return FALSE;

	BeginBitmap( 0, Dw, Dh, Format, Lock.Pitch, Lock.pBits );
	BeginBitmap( 1, pBitmap );

	if( Dw == Sw && Dh == Sh )
	{
		for( y = 0; y < Dh; y++ )
			for( x = 0; x < Dw; x++ )
				SetPixelF( 0, x, y, GetPixelF( 1, Sx+x, Sy+y ) );
	}
	else
	{
		int Tx = (Sw << 16) / Dw;
		int Ty = (Sh << 16) / Dh;
		int Px = Sx << 16;
		int Py = Sy << 16;

		for( y = 0; y < Dh; y++ )
		{
			Px = Sx << 16;

			for( x = 0; x < Dw; x++ )
			{
				SetPixelF( 0, x, y, GetPixelF( 1, Px>>16, Py>>16 ) );

				Px = Px + Tx;
			}

			Py = Py + Ty;
		}
	}

	FinishBitmap( 0 );
	FinishBitmap( 1 );

	D3DRESULT( pSurface->UnlockRect() );

	return TRUE;
}

//=============================================================================
// CDirect3D::SurfaceToBitmap()
//=============================================================================

BOOL CDirect3D::SurfaceToBitmap( BITMAP2 *pBitmap, int Dx, int Dy, int Dw, int Dh, LPDIRECT3DSURFACE9 pSurface, int Sx, int Sy, int Sw, int Sh )
{
	if( !pSurface || !pBitmap )
		return FALSE;

	D3DLOCKED_RECT Lock;
	D3DSURFACE_DESC SurfaceDesc;
	HRESULT hresult;
	RECT rc;
	int x, y;
	int x2, y2;

	hresult = D3DRESULT( pSurface->GetDesc( &SurfaceDesc ) );

	if( FAILED(hresult) )
		return FALSE;

	rc = GdiMakeRect( Sx, Sy, Sw, Sh );

	hresult = D3DRESULT( pSurface->LockRect( &Lock, &rc, D3DLOCK_READONLY ) );

	if( FAILED(hresult) )
		return FALSE;

	PIXELFORMAT Format = PixelFormatFromDirectX( SurfaceDesc.Format );

	if( Format == PIXELFMT_UNKNOWN )
		return FALSE;

	BeginBitmap( 0, pBitmap );
	BeginBitmap( 1, Sw, Sh, Format, Lock.Pitch, Lock.pBits );

	x2 = Dx + Dw;
	y2 = Dy + Dh;

	if( Dw == Sw && Dh == Sh )
	{
		int Px = Sx - Dx;
		int Py = Sy - Dy;

		for( y = 0; y < Dh; y++ )
			for( x = 0; x < Dw; x++ )
				SetPixelF( 0, Dx+x, Dy+y, GetPixelF( 1, x, y ) );
	}
	else
	{
		int Tx = (Sw << 16) / Dw;
		int Ty = (Sh << 16) / Dh;
		int Px = Sx << 16;
		int Py = Sy << 16;

		for( y = Dy; y < y2; y++ )
		{
			Px = Sx << 16;

			for( x = Dx; x < x2; x++ )
			{
				SetPixelF( 0, x, y, GetPixelF( 1, Px>>16, Py>>16 ) );

				Px = Px + Tx;
			}

			Py = Py + Ty;
		}
	}

	FinishBitmap( 0 );
	FinishBitmap( 1 );

	D3DRESULT( pSurface->UnlockRect() );

	return TRUE;
}

//=============================================================================
// Direct3DResult()
//=============================================================================

#define D3DRESULTCASE(hr, text) case hr: result = #hr; description = text;\
	break;

HRESULT Direct3DResult( HRESULT hresult, int Line, char *pFile )
{
	if( hresult != D3D_OK )
	{
		char *result = "";
		char *description = "";

		switch( hresult )
		{
			D3DRESULTCASE( D3DERR_CONFLICTINGRENDERSTATE, "The currently set render states cannot be used together." );
			D3DRESULTCASE( D3DERR_CONFLICTINGTEXTUREFILTER, "The current texture filters cannot be used together." );
			D3DRESULTCASE( D3DERR_CONFLICTINGTEXTUREPALETTE, "The current textures cannot be used simultaneously. This generally occurs when a multitexture device requires that all palletized textures simultaneously enabled also share the same palette." );
			D3DRESULTCASE( D3DERR_DEVICELOST, "The device is lost and cannot be restored at the current time, so rendering is not possible." );
			D3DRESULTCASE( D3DERR_DEVICENOTRESET, "The device cannot be reset." );
			D3DRESULTCASE( D3DERR_DRIVERINTERNALERROR, "Internal driver error." );
			D3DRESULTCASE( D3DERR_INVALIDCALL, "The method call is invalid. For example, a method's parameter may have an invalid value." );
			D3DRESULTCASE( D3DERR_INVALIDDEVICE, "The requested device type is not valid." );
			D3DRESULTCASE( D3DERR_MOREDATA, "There is more data available than the specified buffer size can hold." );
			D3DRESULTCASE( D3DERR_NOTAVAILABLE, "This device does not support the queried technique." );
			D3DRESULTCASE( D3DERR_NOTFOUND, "The requested item was not found." );
			D3DRESULTCASE( D3DERR_OUTOFVIDEOMEMORY, "Direct3D does not have enough display memory to perform the operation." );
			D3DRESULTCASE( D3DERR_TOOMANYOPERATIONS, "The application is requesting more texture-filtering operations than the device supports." );
			D3DRESULTCASE( D3DERR_UNSUPPORTEDALPHAARG, "The device does not support a specified texture-blending argument for the apha channel." );
			D3DRESULTCASE( D3DERR_UNSUPPORTEDALPHAOPERATION, "The device does not support a specified texture-blending operation for the apha channel." );
			D3DRESULTCASE( D3DERR_UNSUPPORTEDCOLORARG, "The device does not support a specified texture-blending argument for color values." );
			D3DRESULTCASE( D3DERR_UNSUPPORTEDCOLOROPERATION, "The device does not support a specified texture-blending operation for color values." );
			D3DRESULTCASE( D3DERR_UNSUPPORTEDFACTORVALUE, "The device does not support the specified texture factor value." );
			D3DRESULTCASE( D3DERR_UNSUPPORTEDTEXTUREFILTER, "The device does not support the specified texture filter." );
			D3DRESULTCASE( D3DERR_WRONGTEXTUREFORMAT, "The pixel format of the texture surface is not valid." );
			D3DRESULTCASE( E_FAIL, "An undetermined error occurred inside the Direct3D subsystem." );
			D3DRESULTCASE( E_INVALIDARG, "An invalid parameter was passed to the returning function" );
			D3DRESULTCASE( E_OUTOFMEMORY, "Direct3D could not allocate sufficient memory to complete the call." );
		}

		Debugf( "%s(%d) : %s - %s\n", pFile, Line, result, description );
	}

	return hresult;
}

