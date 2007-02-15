#ifndef _DIRECT3D_H_
#define _DIRECT3D_H_

#include <windows.h>
#include <d3d9.h>

PIXELFORMAT PixelFormatFromDirectX( D3DFORMAT Format );
D3DFORMAT PixelFormatToDirectX( PIXELFORMAT Format );

class CDirect3D
{
	public:
		CDirect3D( void );
		virtual ~CDirect3D();

	public:
		virtual BOOL Initialize( HWND hWnd, int Width, int Height, D3DFORMAT Format, BOOL Windowed );
		virtual void Terminate( void );

		//////////////////////////////////////////////////////////////////////////
		
		HRESULT BeginScene( void );
		HRESULT Clear( DWORD Count,  D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil );
		HRESULT CreateAdditionalSwapChain( D3DPRESENT_PARAMETERS *pPresentParameters, LPDIRECT3DSWAPCHAIN9 *ppSwapChain );
		HRESULT CreateDepthStencilSurface( UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, LPDIRECT3DSURFACE9 *ppSurface, HANDLE *pSharedHandle );
		HRESULT CreateIndexBuffer( UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DINDEXBUFFER9 *ppIndexBuffer, HANDLE *pSharedHandle );
		HRESULT CreateRenderTarget( UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, LPDIRECT3DSURFACE9 *ppSurface, HANDLE* pSharedHandle );
		HRESULT CreateTexture( UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 *ppTexture, HANDLE *pSharedHandle );
		HRESULT CreateVertexBuffer( UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, LPDIRECT3DVERTEXBUFFER9 *ppVertexBuffer, HANDLE *pSharedHandle );
		HRESULT DrawIndexedPrimitive( D3DPRIMITIVETYPE PrimitiveType, int BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount );
		HRESULT DrawPrimitive( D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount );
		HRESULT EndScene( void );
		HRESULT GetAdapterDisplayMode( UINT Adapter, D3DDISPLAYMODE *pDisplayMode );
		HRESULT GetDepthStencilSurface( LPDIRECT3DSURFACE9 *ppSurface );
		HRESULT GetDisplayMode( UINT SwapChain, D3DDISPLAYMODE *pDisplayMode );
		HRESULT GetRenderState( D3DRENDERSTATETYPE State, DWORD *pValue );
		HRESULT GetRenderTarget( DWORD RenderTargetIndex, LPDIRECT3DSURFACE9 *ppRenderTarget );
		HRESULT GetStreamSource( UINT StreamNumber, LPDIRECT3DVERTEXBUFFER9 *ppStreamData, UINT *pOffsetInBytes, UINT *pStride );
		HRESULT GetTexture( DWORD Stage, LPDIRECT3DBASETEXTURE9 *ppTexture );
		HRESULT GetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue );
		HRESULT GetVertexShader( LPDIRECT3DVERTEXSHADER9* ppShader );
		HRESULT MultiplyTransform( D3DTRANSFORMSTATETYPE State, D3DMATRIX *pTransform );
		HRESULT Present( RECT *pSrc, RECT *pDst, HWND hWndOverride, RGNDATA *pDirtyRegion );
		HRESULT SetFVF( DWORD FVF );
		HRESULT SetRenderState( D3DRENDERSTATETYPE State, DWORD Value );		
		HRESULT SetRenderTarget( UINT RenderTargetIndex, LPDIRECT3DSURFACE9 pRenderTarget );
		HRESULT SetSamplerState( UINT Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value );
		HRESULT SetStreamSource( UINT Stream, LPDIRECT3DVERTEXBUFFER9 pStreamData, UINT OffsetInBytes, UINT Stride );
		HRESULT SetTexture( DWORD Stage, LPDIRECT3DTEXTURE9 pTexture );
		HRESULT SetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value );
		HRESULT SetTransform( D3DTRANSFORMSTATETYPE State,  D3DMATRIX *pMatrix );
		HRESULT SetVertexShader( LPDIRECT3DVERTEXSHADER9 pShader );
		HRESULT SetViewport( CONST D3DVIEWPORT9 *pViewport );

		//////////////////////////////////////////////////////////////////////////

		virtual LPDIRECT3DTEXTURE9 CreateTextureFromBitmap( BITMAP2 *pBitmap, int Width, int Height, D3DFORMAT Format, D3DPOOL Pool );
		virtual BOOL BitmapToTexture( LPDIRECT3DTEXTURE9 pTexture, int Dx, int Dy, int Dw, int Dh, BITMAP2 *pBitmap, int Sx, int Sy, int Sw, int Sh );
		virtual BOOL BitmapToSurface( LPDIRECT3DSURFACE9 pSurface, int Dx, int Dy, int Dw, int Dh, BITMAP2 *pBitmap, int Sx, int Sy, int Sw, int Sh );
		virtual BOOL SurfaceToBitmap( BITMAP2 *pBitmap, int Dx, int Dy, int Dw, int Dh, LPDIRECT3DSURFACE9 pSurface, int Sx, int Sy, int Sw, int Sh );

	public:
		LPDIRECT3D9 m_pDirect3D;
		LPDIRECT3DDEVICE9 m_pDirect3DDevice;
		D3DPRESENT_PARAMETERS m_PresentParameters;
};

HRESULT Direct3DResult( HRESULT hresult, int Line, char *pFile );

#ifdef _DEBUG
#define D3DRESULT(stuff) Direct3DResult(stuff, __LINE__, __FILE__ )
#else
#define D3DRESULT
#endif

extern CDirect3D d3d;

#endif // _DIRECT3D_H_