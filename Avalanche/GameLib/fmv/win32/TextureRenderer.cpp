#include "Platform/CompilerControl.h"	// for NAMED_SECTION
#include "Platform/BaseType.h"
#include "layers/media.h"
#include "Display/Common/DisplayState.h"
#include "Display/Common/TextureState.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"
#include "Game/Managers/TextureMgr/Texture.h"
#include "Game/Managers/TextureMgr/PlatformImage.h"
#include "Game/Managers/SpriteMgr.h"
#include "Display/Common/DisplayPlatform.h"
#include "Display/Common/RenderState.h"
#include "Display/Common/MatrixStack.h"
#include "Display/Common/Draw.h"
#include "DShowTextures.h"
#include "DXUtil.h"
#include "TextureRenderer.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// CTextureRenderer constructor
///
/// \param pUnk Pointer to the aggregated owner object
///
/// \param phr Point to an HRESULT value returning the error code of the operation
///
/// \param pTexture A pointer to the texture that should be renderered to
///
///////////////////////////////////////////////////////////////////////////////////////////////////
CTextureRenderer::CTextureRenderer(LPUNKNOWN pUnk, HRESULT *phr)
	: CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer), NAME("Texture Renderer"), pUnk, phr),
	  m_pTexture(0)
{
    // Store and AddRef the texture for our use.
    *phr = S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// CTextureRenderer destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
CTextureRenderer::~CTextureRenderer()
{
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// CheckMediaType: This method forces the graph to give us an R8G8B8 video
/// type, making our copy to texture memory trivial.
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CTextureRenderer::CheckMediaType(const CMediaType *pmt)
{
    HRESULT   hr = E_FAIL;
    VIDEOINFO *pvi;
    
    // Reject the connection if this is not a video type
    if (*pmt->FormatType() != FORMAT_VideoInfo)
	{
        return E_INVALIDARG;
    }
    
    // Only accept RGB24
    pvi = (VIDEOINFO *)pmt->Format();
    if (IsEqualGUID(*pmt->Type(), MEDIATYPE_Video) && IsEqualGUID(*pmt->Subtype(), MEDIASUBTYPE_RGB24))
    {
        hr = S_OK;
    }
    
    return hr;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetMediaType: Graph connection has been made.
///
/// \param pmt Pointer to an AM_MEDIA_TYPE structure that specifies the media type.
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CTextureRenderer::SetMediaType(const CMediaType *pmt)
{
    HRESULT hr;

    // Retrive the size of this media type
    VIDEOINFO *pviBmp;                      // Bitmap info header
    pviBmp = (VIDEOINFO *)pmt->Format();
    m_lVidWidth  = pviBmp->bmiHeader.biWidth;
    m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
    m_lVidPitch = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24

	// Create the texture to render the movie to
	if(FAILED(D3DXCreateTexture(DirectX_Device, m_lVidWidth, m_lVidHeight,
								1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture)))
	{
		return false;
	}

    // D3DXCreateTexture can silently change the parameters on us
    if (FAILED(hr = m_pTexture->GetLevelDesc(0, &m_TextureDesc)))
	{
		ASSERT(0);
        //Msg(TEXT("Could not get level Description of D3DX texture! hr = 0x%x"), hr);
        return hr;
    }
    if (m_TextureDesc.Format != D3DFMT_A8R8G8B8 && m_TextureDesc.Format != D3DFMT_A1R5G5B5)
	{
        ASSERT(0);
        //Msg(TEXT("Texture is format we can't handle! Format = 0x%x"), m_TextureDesc.Format);
        return VFW_E_TYPE_NOT_ACCEPTED;
    }

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// DoRenderSample: A sample has been delivered. Copy it to the texture.
///
/// \param pSample Pointer ot the sample to copy.
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CTextureRenderer::DoRenderSample(IMediaSample* pSample)
{
    BYTE  *pBmpBuffer, *pTxtBuffer;     // Bitmap buffer, texture buffer
    LONG  lTxtPitch;                // Pitch of bitmap, texture

	//begin the Direct3D scene
	HRESULT result;
	result = DirectX_Device->BeginScene( );
	ASSERT( result == D3D_OK );

    // Get the video bitmap buffer
    pSample->GetPointer(&pBmpBuffer);
	pBmpBuffer += m_lVidHeight * m_lVidPitch;  // Go to the end of the bitmap buffer
	                                           // and blit from there, since bitmaps
	                                           // are upside down in Windows

    // Lock the Texture
	D3DLOCKED_RECT d3dlr;
    if (FAILED(m_pTexture->LockRect(0, &d3dlr, 0, 0)))
	{
        return E_FAIL;
	}
    
    // Get the texture buffer & pitch
    pTxtBuffer = static_cast<byte *>(d3dlr.pBits);
    lTxtPitch = d3dlr.Pitch;
    
    // Copy the bits    
    // OPTIMIZATION OPPORTUNITY: Use a video and texture
    // format that allows a simpler copy than this one.
    if (m_TextureDesc.Format == D3DFMT_A8R8G8B8)
	{
        for (int y = m_lVidHeight - 1; y >=0; y--)
		{
            BYTE *pBmpBufferOld = pBmpBuffer;
            BYTE *pTxtBufferOld = pTxtBuffer;   
			//pTxtBuffer += lTxtPitch; // Blit from right to left since bitmaps are flipped on Windows
            for (int x = 0; x < m_lVidWidth; x++)
			{
				pBmpBuffer -= 3;
                pTxtBuffer[0] = pBmpBuffer[0];
                pTxtBuffer[1] = pBmpBuffer[1];
                pTxtBuffer[2] = pBmpBuffer[2];
                pTxtBuffer[3] = 0xff;
                pTxtBuffer += 4;
            }
            pBmpBuffer = pBmpBufferOld - m_lVidPitch;
            pTxtBuffer = pTxtBufferOld + lTxtPitch;
        }
    }

    if (m_TextureDesc.Format == D3DFMT_A1R5G5B5)
	{
        for (int y = m_lVidHeight - 1; y >=0; y--)
		{
            BYTE *pBmpBufferOld = pBmpBuffer;
            BYTE *pTxtBufferOld = pTxtBuffer;
			//pTxtBuffer += lTxtPitch; // Blit form right to left since bitmaps are flipped on Windows
            for (int x = 0; x < m_lVidWidth; x++)
			{
				pBmpBuffer -= 3;
                *(WORD *)pTxtBuffer =
                    0x8000 +
                    ((pBmpBuffer[2] & 0xF8) << 7) +
                    ((pBmpBuffer[1] & 0xF8) << 2) +
                    (pBmpBuffer[0] >> 3);
				pTxtBuffer += 2;
            }
            pBmpBuffer = pBmpBufferOld - m_lVidPitch;
            pTxtBuffer = pTxtBufferOld + lTxtPitch;
        }
    }

        
    // Unlock the Texture
    if (FAILED(m_pTexture->UnlockRect(0)))
	{
        return E_FAIL;
	}
    
	// Blit the texture to the screen
	gfxBlitTextureToScreen(m_pTexture, float(m_lVidWidth) / float(m_TextureDesc.Width), 0.0f, 0.0f, float(m_lVidHeight) / float(m_TextureDesc.Height));

	result = DirectX_Device->EndScene( );
	ASSERT( result == D3D_OK );

	// Present the backbuffer contents to the display
	result = DirectX_Device->Present( NULL, NULL, NULL, NULL );
	ASSERT( result == D3D_OK );

    return S_OK;
}
