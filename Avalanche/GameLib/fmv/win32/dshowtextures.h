//-----------------------------------------------------------------------------
// File: DShowTextures.h
//
// Desc: DirectShow sample code - adds support for DirectShow videos playing 
//       on a DirectX 8.0 texture surface. Turns the D3D texture tutorial into 
//       a recreation of the VideoTex sample from previous versions of DirectX.
//
// Copyright (c) 2000 Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include "TextureRenderer.hpp"
#include <dshow.h>
#include <atlcomcli.h>

class MoviePlayer
{
	public:
		MoviePlayer() { }
		~MoviePlayer();

		bool Initialize();
		void Play(const char *pFilename);
		void Stop();
		bool IsMovieDone();

	private:
		CComPtr<IGraphBuilder>  m_pGB;          // GraphBuilder
		CComPtr<IMediaControl>  m_pMC;          // Media Control
		CComPtr<IMediaEvent>    m_pME;          // Media Event
};
