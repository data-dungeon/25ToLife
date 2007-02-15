#include "Assert.h"
#include "DXInit.h"

#ifdef DIRECTX_PC
#include "Display/DisplayPCH.h"
#endif

static unsigned int		enumMatchAdapterOrdinal = 0;

static bool					enumConfirmAdapterID( unsigned int adapterOrdinal, D3DADAPTER_IDENTIFIER8 & Identifier )
{
	if( adapterOrdinal == enumMatchAdapterOrdinal )
		return true;

	return false;
}

IDirect3DDevice8 * DXInit::createDevice( HWND hWnd, IDirect3D8 & D3D )
{
	assert( (m_UserSettings.Fullscreen == false)  ||  (m_UserSettings.Windowed == false) );
	assert( (m_UserSettings.Fullscreen == true)  ||  (m_UserSettings.Windowed == true) );

	D3DBestMatch	BestFullscreenMatch = { 0 };
	D3DBestMatch	BestWindowedMatch = { 0 };

	m_pD3D = & D3D;

	setupEnumerator( );

	if( Enumerator.Enumerate( D3D ) != D3D_OK )
		return 0;

	if( m_UserSettings.Windowed )
		return createWindowed( hWnd );
	else
		return createFullscreen( hWnd );
}

IDirect3DDevice8 * DXInit::createWindowed( HWND hWnd )
{
	MONITORINFO			MonitorInfo = { 0 };

	bool bRequiresHAL = false;
	bool bRequiresREF = false;

	switch( m_UserSettings.RequiredD3DDevType )
	{
	case D3DDEVTYPE_HAL:
		bRequiresHAL = true;
		break;
	case D3DDEVTYPE_REF:
		bRequiresREF = true;
		break;
	}

	if( Enumerator.findBestWindowedMode( m_UserSettings.Adapter, bRequiresHAL, bRequiresREF, dxBestDeviceMatch ) == false )
		return 0;		

	m_UserSettings.Adapter = dxBestDeviceMatch.pBestAdapterInfo->AdapterOrdinal;

	HMONITOR hMonitor = m_pD3D->GetAdapterMonitor( m_UserSettings.Adapter );

	MonitorInfo.cbSize = sizeof( MONITORINFO );

	if( GetMonitorInfo( hMonitor, & MonitorInfo ) == false )
		return 0;

	LONG monitorWidth = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left;
	LONG monitorHeight = MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top;

	if( m_UserSettings.MatchAdapterResolution )
	{
		m_UserSettings.Width = monitorWidth;
		m_UserSettings.Height = monitorHeight;
	}

	WINDOWINFO		winInfo;
	winInfo.cbSize = sizeof( WINDOWINFO );

	BOOL success = GetWindowInfo( hWnd, & winInfo );
	assert( success == TRUE );

	LONG windowWidthBuffer = (winInfo.rcWindow.right - winInfo.rcWindow.left) - (winInfo.rcClient.right - winInfo.rcClient.left);
	LONG windowHeightBuffer = (winInfo.rcWindow.bottom - winInfo.rcWindow.top) - (winInfo.rcClient.bottom - winInfo.rcClient.top);

	if( (LONG) m_UserSettings.Width > (monitorWidth - windowWidthBuffer) )
		m_UserSettings.Width = monitorWidth - windowWidthBuffer;

	if( (LONG) m_UserSettings.Height > (monitorHeight - windowHeightBuffer) )
		m_UserSettings.Height = monitorHeight - windowHeightBuffer;

	memset( & dxCreateParams, 0, sizeof( dxCreateParams ) );

	dxCreateParams.BackBufferCount = 1;
	dxCreateParams.BackBufferFormat = dxBestDeviceMatch.pBestDeviceCombo->BackBufferFormat;
	dxCreateParams.BackBufferHeight = m_UserSettings.Height;
	dxCreateParams.BackBufferWidth = m_UserSettings.Width;
	dxCreateParams.EnableAutoDepthStencil = true;
	dxCreateParams.AutoDepthStencilFormat = * (D3DFORMAT *) dxBestDeviceMatch.pBestDeviceCombo->DepthStencilFormatList.getPtr( 0 );
	dxCreateParams.hDeviceWindow = hWnd;
#ifdef DIRECTX_PC
	if ( m_UserSettings.AntialiasingLevel > 0 )
	{
		int validAATypesCount = dxBestDeviceMatch.pBestDeviceCombo->MultiSampleTypeList.count();
		for( int c=0; c < validAATypesCount; ++c )
		{
			if ( m_UserSettings.AntialiasingLevel == *(D3DMULTISAMPLE_TYPE*)dxBestDeviceMatch.pBestDeviceCombo->MultiSampleTypeList.getPtr( c ) )
			{
				dxCreateParams.MultiSampleType = (D3DMULTISAMPLE_TYPE)m_UserSettings.AntialiasingLevel;
				continue;
			}
			else dxCreateParams.MultiSampleType = D3DMULTISAMPLE_NONE;
		}
	}
	else
		dxCreateParams.MultiSampleType = D3DMULTISAMPLE_NONE;
#else
	dxCreateParams.MultiSampleType = * (D3DMULTISAMPLE_TYPE *) dxBestDeviceMatch.pBestDeviceCombo->MultiSampleTypeList.getPtr( 0 );
#endif
	dxCreateParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	dxCreateParams.Windowed = true;
#ifdef DIRECTX_PC
	dxCreateParams.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	DisplayState.backBufferFormat = dxCreateParams.BackBufferFormat;
#endif

	D3DEnumerator::VertexProcessingType VPT = * (D3DEnumerator::VertexProcessingType *) dxBestDeviceMatch.pBestDeviceCombo->VertexProcessingTypeList.getPtr( 0 );
	
	DWORD behaviorFlags = 0;

	switch( VPT )
	{
	case D3DEnumerator::SOFTWARE_VP:
		behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		break;
	case D3DEnumerator::MIXED_VP:
		behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
		break;
	case D3DEnumerator::HARDWARE_VP:
		behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
#ifndef _XBOX
		behaviorFlags |= D3DCREATE_MULTITHREADED;
#endif
		break;
	case D3DEnumerator::PURE_HARDWARE_VP:
		behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
		break;
	default:
		assert( 0 );
		break;
	}

	IDirect3DDevice8 * pNewDXDevice = 0;

	HRESULT result = m_pD3D->CreateDevice( m_UserSettings.Adapter, dxBestDeviceMatch.pBestDeviceCombo->DevType, hWnd, behaviorFlags, & dxCreateParams, & pNewDXDevice );

#ifdef DIRECTX_PC
	// once we've created a device, initialize anisotropic filtering
	// if we've requested it
	if ( pNewDXDevice && m_UserSettings.AnisotropicLevel )
	{
		D3DCAPS8 deviceCaps;
		m_pD3D->GetDeviceCaps( m_UserSettings.Adapter, dxBestDeviceMatch.pBestDeviceCombo->DevType, & deviceCaps );

		int anisoLevel = 0;
		if ( m_UserSettings.AnisotropicLevel < deviceCaps.MaxAnisotropy )
			anisoLevel = m_UserSettings.AnisotropicLevel;
		else
			anisoLevel = deviceCaps.MaxAnisotropy;
		_TextureState_AnisotropyLevel = anisoLevel;
	}
	else
		_TextureState_AnisotropyLevel = 0;
#endif
	RECT windowRect = { 0, 0, m_UserSettings.Width, m_UserSettings.Height };

	AdjustWindowRect( &windowRect, winInfo.dwStyle, FALSE );

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	int windowX = MonitorInfo.rcMonitor.left + (int) (.2f * (monitorWidth - windowWidth) );
	int windowY = MonitorInfo.rcMonitor.top + (int) (.2f * (monitorHeight - windowHeight) );

	BOOL bWindowPosChanged = SetWindowPos( hWnd, HWND_NOTOPMOST, windowX, windowY, windowWidth, windowHeight, 0);
	assert( bWindowPosChanged == TRUE );

	return (result == D3D_OK) ? pNewDXDevice : 0;
}

IDirect3DDevice8 * DXInit::createFullscreen( HWND hWnd )
{
	bool bRequiresHAL = false;
	bool bRequiresREF = false;

	switch( m_UserSettings.RequiredD3DDevType )
	{
	case D3DDEVTYPE_HAL:
		bRequiresHAL = true;
		break;
	case D3DDEVTYPE_REF:
		bRequiresREF = true;
		break;
	}

	if( Enumerator.findBestFullscreenMode( m_UserSettings.Adapter, bRequiresHAL, bRequiresREF, m_UserSettings.MatchAdapterResolution, dxBestDeviceMatch ) == false )
		return 0;		

	m_UserSettings.Adapter = dxBestDeviceMatch.pBestAdapterInfo->AdapterOrdinal;

	D3DDISPLAYMODE	searchMode = { m_UserSettings.Width, m_UserSettings.Height, m_UserSettings.FullscreenRefresh, dxBestDeviceMatch.pBestDeviceCombo->AdapterFormat };

	if( m_UserSettings.MatchAdapterResolution )
	{
		D3DDISPLAYMODE adapterMode;
		HRESULT result = m_pD3D->GetAdapterDisplayMode( dxBestDeviceMatch.pBestAdapterInfo->AdapterOrdinal, & adapterMode );

		if( SUCCEEDED( result ) )
			searchMode = adapterMode;
	}

	bool bModeFound = findFullscreenMode( * dxBestDeviceMatch.pBestAdapterInfo, searchMode );

	if( bModeFound == false )
	{
		searchMode.RefreshRate = 0;

		bModeFound = findFullscreenMode( * dxBestDeviceMatch.pBestAdapterInfo, searchMode );
		if( bModeFound == false )
		{
			searchMode.Height = 0;

			bModeFound = findFullscreenMode( * dxBestDeviceMatch.pBestAdapterInfo, searchMode );
			assert( bModeFound == true );
		}
	}

	dxCreateParams.BackBufferCount = 1;
	dxCreateParams.BackBufferFormat = D3DFMT_X8R8G8B8; //dxBestDeviceMatch.pBestDeviceCombo->BackBufferFormat;
	dxCreateParams.BackBufferHeight = searchMode.Height;
	dxCreateParams.BackBufferWidth = searchMode.Width;
	dxCreateParams.EnableAutoDepthStencil = true;
	dxCreateParams.AutoDepthStencilFormat = * (D3DFORMAT *) dxBestDeviceMatch.pBestDeviceCombo->DepthStencilFormatList.getPtr( 0 );
	dxCreateParams.hDeviceWindow = hWnd;
#ifdef DIRECTX_PC
	if ( m_UserSettings.AntialiasingLevel > 0 )
	{
		int validAATypesCount = dxBestDeviceMatch.pBestDeviceCombo->MultiSampleTypeList.count();
		for( int c=0; c < validAATypesCount; ++c )
		{
			if ( m_UserSettings.AntialiasingLevel == *(D3DMULTISAMPLE_TYPE*)dxBestDeviceMatch.pBestDeviceCombo->MultiSampleTypeList.getPtr( c ) )
			{
				dxCreateParams.MultiSampleType = (D3DMULTISAMPLE_TYPE)m_UserSettings.AntialiasingLevel;
				continue;
			}
			else dxCreateParams.MultiSampleType = D3DMULTISAMPLE_NONE;
		}
	}
	else
		dxCreateParams.MultiSampleType = D3DMULTISAMPLE_NONE;
#else
	dxCreateParams.MultiSampleType = * (D3DMULTISAMPLE_TYPE *) dxBestDeviceMatch.pBestDeviceCombo->MultiSampleTypeList.getPtr( 0 );
#endif
	dxCreateParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	dxCreateParams.Windowed = false;
	dxCreateParams.FullScreen_RefreshRateInHz = searchMode.RefreshRate;
	dxCreateParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	dxCreateParams.Flags = 0;
#ifdef DIRECTX_PC
	dxCreateParams.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	DisplayState.backBufferFormat = dxCreateParams.BackBufferFormat;
#endif


	D3DEnumerator::VertexProcessingType VPT = * (D3DEnumerator::VertexProcessingType *) dxBestDeviceMatch.pBestDeviceCombo->VertexProcessingTypeList.getPtr( 0 );
	
	DWORD behaviorFlags = 0;

	switch( VPT )
	{
	case D3DEnumerator::SOFTWARE_VP:
		behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		break;
	case D3DEnumerator::MIXED_VP:
		behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
		break;
	case D3DEnumerator::HARDWARE_VP:
		behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
#ifndef _XBOX
		behaviorFlags |= D3DCREATE_MULTITHREADED;
#endif
		break;
	case D3DEnumerator::PURE_HARDWARE_VP:
		behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
		break;
	default:
		assert( 0 );
		break;
	}

	IDirect3DDevice8 * pNewDXDevice = 0;

	HRESULT result = m_pD3D->CreateDevice( m_UserSettings.Adapter, dxBestDeviceMatch.pBestDeviceCombo->DevType, hWnd, behaviorFlags, & dxCreateParams, & pNewDXDevice );

#ifdef DIRECTX_PC
	// once we've created a device, initialize anisotropic filtering
	// if we've requested it
	if ( pNewDXDevice && m_UserSettings.AnisotropicLevel )
	{
		D3DCAPS8 deviceCaps;
		m_pD3D->GetDeviceCaps( m_UserSettings.Adapter, dxBestDeviceMatch.pBestDeviceCombo->DevType, & deviceCaps );

		int anisoLevel = 0;
		if ( m_UserSettings.AnisotropicLevel < deviceCaps.MaxAnisotropy )
			anisoLevel = m_UserSettings.AnisotropicLevel;
		else
			anisoLevel = deviceCaps.MaxAnisotropy;
		_TextureState_AnisotropyLevel = anisoLevel;
	}
	else
		_TextureState_AnisotropyLevel = 0;
#endif

	return (result == D3D_OK) ? pNewDXDevice : 0;
}

bool DXInit::findFullscreenMode( D3DAdapterInfo & AdapterInfo, D3DDISPLAYMODE mode )
{
	for( unsigned int i = 0; i < AdapterInfo.DisplayModeList.count( ); i++ )
	{
		D3DDISPLAYMODE * pListMode = (D3DDISPLAYMODE *) AdapterInfo.DisplayModeList.getPtr( i );

		if( ( (pListMode->Format == mode.Format)  ||  (mode.Format == 0) )  &&
			 (	(pListMode->Height == mode.Height)  ||  (mode.Height == 0) )  &&
			 (	(pListMode->RefreshRate == mode.RefreshRate)  ||  (mode.RefreshRate == 0) )  &&
			 ( (pListMode->Width == mode.Width)  ||  (mode.Width == 0) ) )
		{
			mode = *pListMode;
			return true;
		}
	}

	return false;
}

void DXInit::setupEnumerator( void )
{
	int perfHUDAdapter = findPerfHUD( );

	if( m_UserSettings.ShaderDebugging )
		m_UserSettings.SoftwareVP = true;

	if( perfHUDAdapter >= 0 )
	{
		m_UserSettings.Adapter = perfHUDAdapter;
		m_UserSettings.SoftwareVP = true;
		m_UserSettings.RequiredD3DDevType = D3DDEVTYPE_REF;
	}
	else if( m_UserSettings.Adapter >= 0 )
	{
		if( (unsigned int) m_UserSettings.Adapter >= m_pD3D->GetAdapterCount( ) )
			m_UserSettings.Adapter = -1;
	}

	if( m_UserSettings.Adapter >= 0 )
	{
		enumMatchAdapterOrdinal = m_UserSettings.Adapter;
		Enumerator.appConfirmAdapterCallback = enumConfirmAdapterID;
	}
	else
		Enumerator.appConfirmAdapterCallback = 0;

	Enumerator.appConfirmDeviceCallback = 0;

	Enumerator.appMinDepthBits = 24;
	Enumerator.appUsesPureDevice = false;
	Enumerator.appRequiresHardwareVP = false;
	Enumerator.appRequiresSoftwareVP = m_UserSettings.SoftwareVP;

	Enumerator.appUsesDepthBuffer = true;

	if( m_UserSettings.Fullscreen )
	{
		Enumerator.appRequiresFullscreen = true;
		Enumerator.appRequiresWindowed = false;

		Enumerator.appMinFullscreenWidth = 640;
		Enumerator.appMinFullscreenWidth = 400;
	}
}

int DXInit::findPerfHUD( void )
{
	assert( m_pD3D );

	unsigned int numAdapters = m_pD3D->GetAdapterCount( );

	for( unsigned int adapterOrdinal = 0; adapterOrdinal < numAdapters; adapterOrdinal++ )
	{
		D3DADAPTER_IDENTIFIER8	Identifier;

		HRESULT result = m_pD3D->GetAdapterIdentifier( adapterOrdinal, 0, &Identifier );

		if( result == D3D_OK )
		{
			if( strcmp( Identifier.Description, "NVIDIA NVPerfHUD" ) == 0 )
				return adapterOrdinal;
		}
	}

	return -1;
}

int DXInit::findAdapter( HMONITOR hMonitor )
{
	assert( m_pD3D );

	unsigned int numAdapters = m_pD3D->GetAdapterCount( );

	for( unsigned int i = 0; i < numAdapters; i++ )
		if( m_pD3D->GetAdapterMonitor( i ) == hMonitor )
			return i;

	return -1;
}
	
int DXInit::findAdapter( HWND hWnd )
{
	HMONITOR hMonitor = MonitorFromWindow( hWnd, MONITOR_DEFAULTTONEAREST );

	return findAdapter( hMonitor );
}





