#ifndef __DX_INIT_H
#define __DX_INIT_H

#include "DirectX.h"
#include "D3DEnumerator.h"


struct DXInitSettings
{
	D3DDEVTYPE			RequiredD3DDevType;

	bool					Fullscreen;
	bool					Windowed;
	bool					ShaderDebugging;
	bool					SoftwareVP;

	int					Adapter;

	bool					MatchAdapterResolution;

	unsigned int		Width;
	unsigned int		Height;

	unsigned int		FullscreenRefresh;
#ifdef DIRECTX_PC	
	unsigned int		AntialiasingLevel;
	unsigned int		AnisotropicLevel;
#endif


	DXInitSettings( )
	{
		RequiredD3DDevType = (D3DDEVTYPE) 0;

		Fullscreen = false;
		Windowed = true;

		ShaderDebugging = false;
		SoftwareVP = false;

		Adapter = -1;

		MatchAdapterResolution = false;

		Width = 800;
		Height = 600;

		FullscreenRefresh = 60;
#ifdef DIRECTX_PC
		AntialiasingLevel = AnisotropicLevel = 0;
#endif
	} ;

} ;


class DXInit
{
	IDirect3D8 *			m_pD3D;
	DXInitSettings	&		m_UserSettings;


	int				findPerfHUD( void );

	void						setupEnumerator( void );

	IDirect3DDevice8 *	createWindowed( HWND hWnd );
	IDirect3DDevice8 *	createFullscreen( HWND hWnd );

	int					findAdapter( HMONITOR hMonitor );
	int					findAdapter( HWND hWnd );

	bool					findFullscreenMode( D3DAdapterInfo & AdapterInfo, D3DDISPLAYMODE mode );

public:
	D3DEnumerator			Enumerator;
	D3DBestMatch			dxBestDeviceMatch;

	D3DPRESENT_PARAMETERS	dxCreateParams;

	DXInit( DXInitSettings & _Settings )
	:
		m_UserSettings( _Settings )
	{
		m_pD3D = 0;

	} ;

	void						updateSettings( const DXInitSettings & newSettings )		{ m_UserSettings = newSettings; }

	IDirect3DDevice8 *	createDevice( HWND hWnd, IDirect3D8 & D3D );

} ;

#endif
