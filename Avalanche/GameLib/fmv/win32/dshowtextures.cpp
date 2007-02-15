//-----------------------------------------------------------------------------
// File: DShowTextures.cpp
//
// Desc: DirectShow sample code - adds support for DirectShow videos playing 
//       on a DirectX 8.0 texture surface. Turns the D3D texture tutorial into 
//       a recreation of the VideoTex sample from previous versions of DirectX.
//
// Copyright (c) 2000 Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

#include "DShowTextures.h"
#include "DXUtil.h"
#include "TextureRenderer.hpp"
#include <new>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Clean up DirectShow on exit
///////////////////////////////////////////////////////////////////////////////////////////////////
MoviePlayer::~MoviePlayer()
{
	// Shut down the graph
    if (!(!m_pMC)) m_pMC->Stop();
    if (!(!m_pGB)) m_pGB.Release ();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Create DirectShow filter graph and run the graph
///
/// \\param pFilename The movie file to load
///
/// \\param pTexture The texture to place the movie into--must be initialized and large enough to
///  hold the movie
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MoviePlayer::Initialize()
{
	HRESULT hr = S_OK;
    CComPtr<IBaseFilter> pFTR;           // Texture Renderer Filter
    CTextureRenderer*    pCTR;           // DShow Texture renderer

    // Create the filter graph
    if (FAILED(m_pGB.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)))
        return false;

    // Create the Texture Renderer object
	pCTR = new CTextureRenderer(NULL, &hr);
    if (FAILED(hr))                                      
    {
        ASSERT(0);
        //Msg(TEXT("Could not create texture renderer object!  hr=0x%x"), hr);
        return false;
    }

    // Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
    pFTR = pCTR;
    if (FAILED(hr = m_pGB->AddFilter(pFTR, L"TEXTURERENDERER")))
    {
        ASSERT(0);
        //Msg(TEXT("Could not add renderer filter to graph!  hr=0x%x"), hr);
        return false;
    }
    
    return true;
}

void MoviePlayer::Play(const char *pFilename)
{
	// Determine the file to load based on DirectX Media path (from SDK)
    // Use a helper function included in DXUtils.cpp
    WCHAR wFileName[MAX_PATH];

    #ifndef UNICODE
        MultiByteToWideChar(CP_ACP, 0, pFilename, -1, wFileName, MAX_PATH);
    #else
        lstrcpy(wFileName, pFilename);
    #endif

	m_pGB->RenderFile(wFileName, 0);
    
    // Get the graph's media control and event interfaces
    m_pGB.QueryInterface(&m_pMC);
    m_pGB.QueryInterface(&m_pME);

	// Start the graph running;
    if (FAILED(m_pMC->Run()))
    {
        ASSERT(0);
		//Msg(TEXT("Could not run the DirectShow graph!  hr=0x%x"), hr);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// See if the movie is done playing
///
/// \\return true if done playing, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MoviePlayer::IsMovieDone()
{
	long lEventCode;
    long lParam1;
    long lParam2;

    // Check for completion events
    m_pME->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
    if (EC_COMPLETE == lEventCode)
	{
        return true;
    }

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Stop playing the movie
///////////////////////////////////////////////////////////////////////////////////////////////////
void MoviePlayer::Stop()
{
	m_pMC->Stop();
}
