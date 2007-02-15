//-----------------------------------------------------------------------------
// File: TextureRenderer.hpp
//
// Desc: DirectShow sample code - adds support for DirectShow videos playing 
//       on a DirectX 8.0 texture surface. Turns the D3D texture tutorial into 
//       a recreation of the VideoTex sample from previous versions of DirectX.
//
// Copyright (c) 2000 Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include <streams.h>
#include <d3d8.h>

//-----------------------------------------------------------------------------
// Define GUID for Texture Renderer
// {71771540-2017-11cf-AE26-0020AFD79767}
//-----------------------------------------------------------------------------
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

//-----------------------------------------------------------------------------
// CTextureRenderer Class Declarations
//-----------------------------------------------------------------------------
class CTextureRenderer : public CBaseVideoRenderer
{
	public:
		CTextureRenderer(LPUNKNOWN pUnk,HRESULT *phr);
		~CTextureRenderer();

		HRESULT CheckMediaType(const CMediaType *pmt );     // Format acceptable?
		HRESULT SetMediaType(const CMediaType *pmt );       // Video format notification
		HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

	private:
		LONG      m_lVidWidth;         // Video width
		LONG      m_lVidHeight;        // Video Height
		LONG      m_lVidPitch;         // Video Pitch
		D3DSURFACE_DESC m_TextureDesc; // Texture description
		LPDIRECT3DTEXTURE8 m_pTexture; // Texture we're using
};


