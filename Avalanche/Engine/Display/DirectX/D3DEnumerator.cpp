#include "Assert.h"

#include "DirectX.h"
#include "D3DEnumerator.h"

//-----------------------------------------------------------------------------
// Name: D3DAdapterInfo constructor / destructor
// Desc: 
//-----------------------------------------------------------------------------

D3DAdapterInfo::D3DAdapterInfo( ) :
	DisplayModeList( CArrayList::AL_VALUE, sizeof( D3DDISPLAYMODE ) ),
	DeviceInfoList( CArrayList::AL_REFERENCE )
{

}

D3DAdapterInfo::~D3DAdapterInfo( void )
{
	for( unsigned int ili = 0; ili < DeviceInfoList.count( ); ili++ )
		delete (D3DDeviceInfo *) DeviceInfoList.getPtr( ili );
}

//-----------------------------------------------------------------------------
// Name: D3DDeviceInfo constructor / destructor
// Desc: 
//-----------------------------------------------------------------------------
D3DDeviceInfo::D3DDeviceInfo( ) :
	DeviceComboList( CArrayList::AL_REFERENCE )
{
}

D3DDeviceInfo::~D3DDeviceInfo( void )
{
	for( unsigned int cli = 0; cli < DeviceComboList.count( ); cli++ )
		delete (D3DDeviceCombo *) DeviceComboList.getPtr( cli);

}

//-----------------------------------------------------------------------------
// Name: D3DDeviceCombo comstructior / destructor
// Desc: 
//-----------------------------------------------------------------------------
D3DDeviceCombo::D3DDeviceCombo( ) :
	DepthStencilFormatList( CArrayList::AL_VALUE, sizeof( D3DFORMAT ) ),
	MultiSampleTypeList( CArrayList::AL_VALUE, sizeof( D3DMULTISAMPLE_TYPE ) ),
	DSMSConflictList( CArrayList::AL_VALUE, sizeof( D3DDSMSConflict ) ),
	VertexProcessingTypeList( CArrayList::AL_VALUE, sizeof( D3DEnumerator::VertexProcessingType ) ),
	PresentIntervalList( CArrayList::AL_VALUE, sizeof( unsigned int ) )
{
}

D3DDeviceCombo::~D3DDeviceCombo( void )
{

}

//-----------------------------------------------------------------------------
// Name: ColorChannelBits
// Desc: Returns the number of color channel bits in the specified D3DFORMAT
//-----------------------------------------------------------------------------
unsigned int D3DEnumerator::numColorChannelBits( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_R8G8B8:
		return 8;
	case D3DFMT_A8R8G8B8:
		return 8;
	case D3DFMT_X8R8G8B8:
		return 8;
	case D3DFMT_R5G6B5:
		return 5;
	case D3DFMT_X1R5G5B5:
		return 5;
	case D3DFMT_A1R5G5B5:
		return 5;
	case D3DFMT_A4R4G4B4:
		return 4;
	case D3DFMT_R3G3B2:
		return 2;
	case D3DFMT_A8R3G3B2:
		return 2;
	case D3DFMT_X4R4G4B4:
		return 4;
	case D3DFMT_A2B10G10R10:
		return 10;
	default:
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: AlphaChannelBits
// Desc: Returns the number of alpha channel bits in the specified D3DFORMAT
//-----------------------------------------------------------------------------
unsigned int D3DEnumerator::numAlphaChannelBits( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_R8G8B8:
		return 0;
	case D3DFMT_A8R8G8B8:
		return 8;
	case D3DFMT_X8R8G8B8:
		return 0;
	case D3DFMT_R5G6B5:
		return 0;
	case D3DFMT_X1R5G5B5:
		return 0;
	case D3DFMT_A1R5G5B5:
		return 1;
	case D3DFMT_A4R4G4B4:
		return 4;
	case D3DFMT_R3G3B2:
		return 0;
	case D3DFMT_A8R3G3B2:
		return 8;
	case D3DFMT_X4R4G4B4:
		return 0;
	case D3DFMT_A2B10G10R10:
		return 2;
	default:
		return 0;
   }
}

//-----------------------------------------------------------------------------
// Name: DepthBits
// Desc: Returns the number of depth bits in the specified D3DFORMAT
//-----------------------------------------------------------------------------
unsigned int D3DEnumerator::numDepthBits( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_D16:
		return 16;
	case D3DFMT_D15S1:
		return 15;
	case D3DFMT_D24X8:
		return 24;
	case D3DFMT_D24S8:
		return 24;
	case D3DFMT_D24X4S4:
		return 24;
	case D3DFMT_D32:
		return 32;
	default:
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: StencilBits
// Desc: Returns the number of stencil bits in the specified D3DFORMAT
//-----------------------------------------------------------------------------
unsigned int D3DEnumerator::numStencilBits( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_D16:
		return 0;
	case D3DFMT_D15S1:
		return 1;
	case D3DFMT_D24X8:
		return 0;
	case D3DFMT_D24S8:
		return 8;
	case D3DFMT_D24X4S4:
		return 4;
	case D3DFMT_D32:
		return 0;
	default:
		return 0;
	}
}


//-----------------------------------------------------------------------------
// Name: D3DEnumerator constructor
// Desc: 
//-----------------------------------------------------------------------------
D3DEnumerator::D3DEnumerator( bool bFillAdapterFormatList, bool bFillBackBufferFormatList ) :
	m_AdapterInfoList( CArrayList::AL_REFERENCE ),
	m_AllowedAdapterFormatList( CArrayList::AL_VALUE, sizeof( D3DFORMAT ) ),
	m_AllowedBackBufferFormatList( CArrayList::AL_VALUE, sizeof( D3DFORMAT ) )
{
	if( bFillAdapterFormatList )
		fillAdapterFormatList( );

	if( bFillBackBufferFormatList )
		fillBackBufferFormatList( );

	appConfirmDeviceCallback = 0;
	appConfirmAdapterCallback = 0;

	appMinFullscreenWidth = 640;
	appMinFullscreenHeight = 480;
	appMinColorChannelBits = 5;
	appMinAlphaChannelBits = 0;
	appMinDepthBits = 0;
	appMinStencilBits = 0;
	appUsesDepthBuffer = false;
	appUsesPureDevice = true;
	appUsesMixedVP = false;
	appRequiresHardwareVP = false;
	appRequiresSoftwareVP = false;
	appRequiresWindowed = false;
	appRequiresFullscreen = false;
}

//-----------------------------------------------------------------------------
// Name: D3DEnumerator destructor
// Desc: 
//-----------------------------------------------------------------------------
D3DEnumerator::~D3DEnumerator( )
{
	for( unsigned int aii = 0; aii < m_AdapterInfoList.count( ); aii++ )
	{
		D3DAdapterInfo * pAdapterInfo = (D3DAdapterInfo *) m_AdapterInfoList.getPtr( aii );
		delete pAdapterInfo;
	}
}

void D3DEnumerator::fillAdapterFormatList( void )
{
	D3DFORMAT				fmt;
	HRESULT					result;

	result = m_AllowedAdapterFormatList.add( & (fmt = D3DFMT_X8R8G8B8) );
	assert( result == D3D_OK );

	result = m_AllowedAdapterFormatList.add( & (fmt = D3DFMT_X1R5G5B5) );
	assert( result == D3D_OK );

	result = m_AllowedAdapterFormatList.add( & (fmt = D3DFMT_R5G6B5) );
	assert( result == D3D_OK );
}

void D3DEnumerator::clearAdapterFormatList( void )
{
	m_AllowedAdapterFormatList.clear( );
}

void D3DEnumerator::addAdapterFormat( D3DFORMAT fmt )
{
	HRESULT					result;

	switch( fmt )
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		fmt = D3DFMT_X8R8G8B8;
		break;
	case D3DFMT_R5G6B5:
		break;
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		fmt = D3DFMT_X1R5G5B5;
		break;
	case D3DFMT_A2B10G10R10:
		break;
	default:
		assert( 0 );
		break;
	}

	if( ! m_AllowedAdapterFormatList.contains( &fmt ) )
	{
		result = m_AllowedAdapterFormatList.add( &fmt );
		assert( result == D3D_OK );
	}
}

void D3DEnumerator::fillBackBufferFormatList( void )
{
	D3DFORMAT				fmt;
	HRESULT					result;

	result = m_AllowedBackBufferFormatList.add( & (fmt = D3DFMT_X8R8G8B8) );
	assert( result == D3D_OK );

	result = m_AllowedBackBufferFormatList.add( & (fmt = D3DFMT_A8R8G8B8) );
	assert( result == D3D_OK );

	result = m_AllowedBackBufferFormatList.add( & (fmt = D3DFMT_X1R5G5B5) );
	assert( result == D3D_OK );

	result = m_AllowedBackBufferFormatList.add( & (fmt = D3DFMT_A1R5G5B5) );
	assert( result == D3D_OK );

	result = m_AllowedBackBufferFormatList.add( & (fmt = D3DFMT_R5G6B5) );
	assert( result == D3D_OK );

}

void D3DEnumerator::clearBackBufferFormatList( void )
{
	m_AllowedBackBufferFormatList.clear( );
}

void D3DEnumerator::addBackBufferFormat( D3DFORMAT fmt )
{
	HRESULT					result;

	switch( fmt )
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A2B10G10R10:
		break;
	default:
		assert( 0 );
		break;
	}

	if( ! m_AllowedAdapterFormatList.contains( &fmt ) )
	{
		result = m_AllowedAdapterFormatList.add( &fmt );
		assert( result == D3D_OK );
	}
}

//-----------------------------------------------------------------------------
// Name: SortModesCallback
// Desc: Used to sort D3DDISPLAYMODEs
//-----------------------------------------------------------------------------
static int __cdecl sortModesCallback( const void * arg1, const void * arg2 )
{
	D3DDISPLAYMODE * pDM1 = (D3DDISPLAYMODE *) arg1;
	D3DDISPLAYMODE * pDM2 = (D3DDISPLAYMODE *) arg2;

	if( pDM1->Width > pDM2->Width )
        return 1;
	if( pDM1->Width < pDM2->Width )
        return -1;
	if( pDM1->Height > pDM2->Height )
        return 1;
	if( pDM1->Height < pDM2->Height )
        return -1;
	if( pDM1->Format > pDM2->Format )
        return 1;
	if( pDM1->Format < pDM2->Format )
        return -1;
	if( pDM1->RefreshRate > pDM2->RefreshRate )
        return 1;
	if( pDM1->RefreshRate < pDM2->RefreshRate )
        return -1;

	return 0;
}

//-----------------------------------------------------------------------------
// Name: Enumerate
// Desc: Enumerates available D3D adapters, devices, modes, etc.
//-----------------------------------------------------------------------------
HRESULT D3DEnumerator::Enumerate( IDirect3D8 & D3D )
{
	HRESULT					result = D3D_OK;
	CArrayList				adapterFormatList( CArrayList::AL_VALUE, sizeof(D3DFORMAT) );

	assert( (appRequiresHardwareVP == false)  ||  (appRequiresSoftwareVP == false) );

	m_pD3D = & D3D;

	unsigned int numAdapters = m_pD3D->GetAdapterCount( );

	for( unsigned int adapterOrdinal = 0; adapterOrdinal < numAdapters; adapterOrdinal++ )
	{
		D3DADAPTER_IDENTIFIER8	Identifier;

		m_pD3D->GetAdapterIdentifier( adapterOrdinal, 0, &Identifier );

		if( appConfirmAdapterCallback  &&  (appConfirmAdapterCallback( adapterOrdinal, Identifier) == false) )
			continue;
		 
		D3DAdapterInfo * pAdapterInfo = new D3DAdapterInfo;
		if( pAdapterInfo == NULL )
			return E_OUTOFMEMORY;

		pAdapterInfo->AdapterOrdinal = adapterOrdinal;
		pAdapterInfo->AdapterIdentifier = Identifier;

		// Get list of all display modes on this adapter.  
		// Also build a temporary list of all display adapter formats.
		adapterFormatList.clear( );

		unsigned int numAdapterModes = m_pD3D->GetAdapterModeCount( adapterOrdinal );

		for( unsigned int mode = 0; mode < numAdapterModes; mode++ )
		{
			D3DDISPLAYMODE displayMode;

			m_pD3D->EnumAdapterModes( adapterOrdinal, mode, &displayMode );

			if( ! m_AllowedAdapterFormatList.contains( &displayMode.Format ) )
				continue;

			if( (displayMode.Width < appMinFullscreenWidth)  ||  (displayMode.Height < appMinFullscreenHeight) ||
				 (numColorChannelBits( displayMode.Format ) < appMinColorChannelBits ) )
				continue;

			pAdapterInfo->DisplayModeList.add( &displayMode );

			if( ! adapterFormatList.contains( &displayMode.Format ) )
				adapterFormatList.add( &displayMode.Format );

		}

		// Sort displaymode list
		qsort( pAdapterInfo->DisplayModeList.getPtr( 0 ), pAdapterInfo->DisplayModeList.count( ), sizeof( D3DDISPLAYMODE ),
					sortModesCallback );

		// Get info for each device on this adapter
		result = enumerateDevices( *pAdapterInfo, adapterFormatList );
		if( FAILED( result ) )
		{
			delete pAdapterInfo;
			return result;
		}

		// If at least one device on this adapter is available and compatible
		// with the app, add the adapterInfo to the list
		if( pAdapterInfo->DeviceInfoList.count( ) == 0 )
			delete pAdapterInfo;
		else
			m_AdapterInfoList.add( pAdapterInfo );
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: EnumerateDevices
// Desc: Enumerates D3D devices for a particular adapter.
//-----------------------------------------------------------------------------
HRESULT D3DEnumerator::enumerateDevices( D3DAdapterInfo & AdapterInfo, CArrayList & AdapterFormatList )
{
	const D3DDEVTYPE		devTypeArray[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_SW, D3DDEVTYPE_REF };
	const unsigned int	devTypeArrayCount = sizeof( devTypeArray ) / sizeof( devTypeArray[0] );
   
	HRESULT					result = D3D_OK;

	D3DDeviceInfo *		pDeviceInfo = NULL;

	for( UINT idt = 0; idt < devTypeArrayCount; idt++ )
	{
		D3DDeviceInfo * pDeviceInfo = new D3DDeviceInfo;

		if( pDeviceInfo == NULL )
			return E_OUTOFMEMORY;

		pDeviceInfo->AdapterOrdinal = AdapterInfo.AdapterOrdinal;
		pDeviceInfo->DevType = devTypeArray[idt];

		result = m_pD3D->GetDeviceCaps( AdapterInfo.AdapterOrdinal, pDeviceInfo->DevType, &pDeviceInfo->Caps );

		if( FAILED( result ) )
		{
			delete pDeviceInfo;
			continue;
		}

		// Get info for each devicecombo on this device
		result = enumerateDeviceCombos( *pDeviceInfo, AdapterFormatList );
		if( FAILED( result ) )
		{
			delete pDeviceInfo;
			return result;
		}

		// If at least one devicecombo for this device is found, 
		// add the deviceInfo to the list
		if( pDeviceInfo->DeviceComboList.count( ) == 0 )
		{
			delete pDeviceInfo;
			continue;
		}

		AdapterInfo.DeviceInfoList.add( pDeviceInfo );
	}
	
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: EnumerateDeviceCombos
// Desc: Enumerates DeviceCombos for a particular device.
//-----------------------------------------------------------------------------
HRESULT D3DEnumerator::enumerateDeviceCombos( D3DDeviceInfo & DeviceInfo, CArrayList & AdapterFormatList )
{
	bool isWindowedArray[] = { false, true };

	// See which adapter formats are supported by this device
	for( unsigned int iaf = 0; iaf < AdapterFormatList.count( ); iaf++ )
	{
		D3DFORMAT adapterFormat = * (D3DFORMAT *) AdapterFormatList.getPtr( iaf );
        
		for( unsigned int ibbf = 0; ibbf < m_AllowedBackBufferFormatList.count( ); ibbf++ )
		{
			D3DFORMAT backBufferFormat = * (D3DFORMAT *) m_AllowedBackBufferFormatList.getPtr( ibbf );
			
			if( numAlphaChannelBits( backBufferFormat ) < appMinAlphaChannelBits )
				continue;
            
			for( unsigned int iiw = 0; iiw < 2; iiw++ )
			{
				bool isWindowed = isWindowedArray[iiw];
                
				if( !isWindowed  &&  appRequiresWindowed )
					continue;

				if( isWindowed  &&  appRequiresFullscreen )
					continue;

				HRESULT result = m_pD3D->CheckDeviceType( DeviceInfo.AdapterOrdinal, DeviceInfo.DevType, 
											adapterFormat, backBufferFormat, isWindowed );
				if( FAILED( result ) )
					continue;

				// At this point, we have an adapter/device/adapterformat/backbufferformat/iswindowed
				// DeviceCombo that is supported by the system.  We still need to confirm that it's 
				// compatible with the app, and find one or more suitable depth/stencil buffer format,
				// multisample type, vertex processing type, and present interval.
				D3DDeviceCombo * pDeviceCombo = new D3DDeviceCombo;
				if( pDeviceCombo == NULL )
					return E_OUTOFMEMORY;

				pDeviceCombo->AdapterOrdinal = DeviceInfo.AdapterOrdinal;
				pDeviceCombo->DevType = DeviceInfo.DevType;
				pDeviceCombo->AdapterFormat = adapterFormat;
				pDeviceCombo->BackBufferFormat = backBufferFormat;
				pDeviceCombo->IsWindowed = isWindowed;

				if( appUsesDepthBuffer )
				{
					buildDepthStencilFormatList( *pDeviceCombo );
					if( pDeviceCombo->DepthStencilFormatList.count( ) == 0 )
					{
						delete pDeviceCombo;
						continue;
					}
				}

				buildMultiSampleTypeList( *pDeviceCombo );
				if( pDeviceCombo->MultiSampleTypeList.count( ) == 0 )
				{
					delete pDeviceCombo;
					continue;
				}

				buildDSMSConflictList( *pDeviceCombo );
				buildVertexProcessingTypeList( DeviceInfo, *pDeviceCombo );
				if( pDeviceCombo->VertexProcessingTypeList.count( ) == 0 )
				{
					delete pDeviceCombo;
					continue;
				}
                
				buildPresentIntervalList( DeviceInfo, *pDeviceCombo );

				DeviceInfo.DeviceComboList.add( pDeviceCombo );
			}
		}
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: BuildDepthStencilFormatList
// Desc: Adds all depth/stencil formats that are compatible with the device 
//       and app to the given D3DDeviceCombo.
//-----------------------------------------------------------------------------
void D3DEnumerator::buildDepthStencilFormatList( D3DDeviceCombo & DeviceCombo )
{
	const D3DFORMAT depthStencilFormatArray[] = 
	{
		D3DFMT_D16,
		D3DFMT_D15S1,
		D3DFMT_D24X8,
		D3DFMT_D24S8,
		D3DFMT_D24X4S4,
		D3DFMT_D32,
	} ;
	const unsigned int depthStencilFormatArrayCount = sizeof( depthStencilFormatArray ) / sizeof( depthStencilFormatArray[0] );

	assert( m_pD3D );

	for( unsigned int idsf = 0; idsf < depthStencilFormatArrayCount; idsf++ )
    {
		D3DFORMAT depthStencilFmt = depthStencilFormatArray[idsf];
		if( numDepthBits( depthStencilFmt ) < appMinDepthBits )
			continue;
		if( numStencilBits( depthStencilFmt ) < appMinStencilBits )
			continue;

		HRESULT result = D3D_OK;
		
		result = m_pD3D->CheckDeviceFormat( DeviceCombo.AdapterOrdinal, DeviceCombo.DevType, DeviceCombo.AdapterFormat, 
						      D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, depthStencilFmt );
		if( SUCCEEDED( result ) )
		{
			result = m_pD3D->CheckDepthStencilMatch( DeviceCombo.AdapterOrdinal, DeviceCombo.DevType, DeviceCombo.AdapterFormat, 
							DeviceCombo.BackBufferFormat, depthStencilFmt);

			if( SUCCEEDED( result ) )
				DeviceCombo.DepthStencilFormatList.add( &depthStencilFmt );
		}
	}
}

//-----------------------------------------------------------------------------
// Name: BuildMultiSampleTypeList
// Desc: Adds all multisample types that are compatible with the device and app to
//       the given D3DDeviceCombo.
//-----------------------------------------------------------------------------
void D3DEnumerator::buildMultiSampleTypeList( D3DDeviceCombo & DeviceCombo )
{
	const D3DMULTISAMPLE_TYPE msTypeArray[] = 
	{ 
		D3DMULTISAMPLE_NONE,
		D3DMULTISAMPLE_2_SAMPLES,
		D3DMULTISAMPLE_3_SAMPLES,
		D3DMULTISAMPLE_4_SAMPLES,
		D3DMULTISAMPLE_5_SAMPLES,
		D3DMULTISAMPLE_6_SAMPLES,
		D3DMULTISAMPLE_7_SAMPLES,
		D3DMULTISAMPLE_8_SAMPLES,
		D3DMULTISAMPLE_9_SAMPLES,
		D3DMULTISAMPLE_10_SAMPLES,
		D3DMULTISAMPLE_11_SAMPLES,
		D3DMULTISAMPLE_12_SAMPLES,
		D3DMULTISAMPLE_13_SAMPLES,
		D3DMULTISAMPLE_14_SAMPLES,
		D3DMULTISAMPLE_15_SAMPLES,
		D3DMULTISAMPLE_16_SAMPLES,
	} ;
	const unsigned int msTypeArrayCount = sizeof( msTypeArray ) / sizeof( msTypeArray[0] );

	for( unsigned int imst = 0; imst < msTypeArrayCount; imst++ )
	{
		D3DMULTISAMPLE_TYPE msType = msTypeArray[imst];

		HRESULT result = m_pD3D->CheckDeviceMultiSampleType( DeviceCombo.AdapterOrdinal, DeviceCombo.DevType, 
								DeviceCombo.BackBufferFormat, DeviceCombo.IsWindowed, msType );
      if( SUCCEEDED( result ) )
		{
			DeviceCombo.MultiSampleTypeList.add( &msType );
		}
	}
}

//-----------------------------------------------------------------------------
// Name: BuildDSMSConflictList
// Desc: Find any conflicts between the available depth/stencil formats and
//       multisample types.
//-----------------------------------------------------------------------------
void D3DEnumerator::buildDSMSConflictList( D3DDeviceCombo & DeviceCombo )
{
	for( unsigned int ids = 0; ids < DeviceCombo.DepthStencilFormatList.count( ); ids++ )
	{
		D3DFORMAT format = * (D3DFORMAT *) DeviceCombo.DepthStencilFormatList.getPtr(ids);
		
		for( unsigned int ims = 0; ims < DeviceCombo.MultiSampleTypeList.count( ); ims++ )
		{
			D3DDSMSConflict DSMSConflict;

			D3DMULTISAMPLE_TYPE msType = * (D3DMULTISAMPLE_TYPE *) DeviceCombo.MultiSampleTypeList.getPtr( ims );

			HRESULT result = m_pD3D->CheckDeviceMultiSampleType( DeviceCombo.AdapterOrdinal, DeviceCombo.DevType,
									format, DeviceCombo.IsWindowed, msType );

			if( FAILED( result ) )
			{
				DSMSConflict.DSFormat = format;
				DSMSConflict.MSType = msType;
				
				DeviceCombo.DSMSConflictList.add( &DSMSConflict );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: BuildVertexProcessingTypeList
// Desc: Adds all vertex processing types that are compatible with the device 
//       and app to the given D3DDeviceCombo.
//-----------------------------------------------------------------------------
void D3DEnumerator::buildVertexProcessingTypeList( D3DDeviceInfo & DeviceInfo, D3DDeviceCombo & DeviceCombo )
{
	VertexProcessingType vpt;

	if( (appRequiresSoftwareVP == false)  &&(DeviceInfo.Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) )
	{
		if( (DeviceInfo.Caps.DevCaps & D3DDEVCAPS_PUREDEVICE)  &&  (appUsesPureDevice == true) )
		{
			if( appConfirmDeviceCallback == NULL  || 
				 appConfirmDeviceCallback( DeviceInfo.Caps, PURE_HARDWARE_VP, DeviceCombo.AdapterFormat, DeviceCombo.BackBufferFormat) )
			{
				vpt = PURE_HARDWARE_VP;
				DeviceCombo.VertexProcessingTypeList.add( &vpt );
			}
		}
        
		if( appConfirmDeviceCallback == NULL  ||
			 appConfirmDeviceCallback( DeviceInfo.Caps, HARDWARE_VP, DeviceCombo.AdapterFormat, DeviceCombo.BackBufferFormat) )
		{
			vpt = HARDWARE_VP;
			DeviceCombo.VertexProcessingTypeList.add( &vpt );
		}

		if( appUsesMixedVP  &&  ( appConfirmDeviceCallback == NULL ||
			 appConfirmDeviceCallback( DeviceInfo.Caps, MIXED_VP, DeviceCombo.AdapterFormat, DeviceCombo.BackBufferFormat) ) )
		{
			vpt = MIXED_VP;
			DeviceCombo.VertexProcessingTypeList.add( &vpt );
		}
	}
    
	if( (appRequiresHardwareVP == false)  && ( appConfirmDeviceCallback == NULL  ||
		 appConfirmDeviceCallback( DeviceInfo.Caps, SOFTWARE_VP, DeviceCombo.AdapterFormat, DeviceCombo.BackBufferFormat) ) )
	{
		vpt = SOFTWARE_VP;
		DeviceCombo.VertexProcessingTypeList.add( &vpt );
	}
}

//-----------------------------------------------------------------------------
// Name: BuildPresentIntervalList
// Desc: Adds all present intervals that are compatible with the device and app 
//       to the given D3DDeviceCombo.
//-----------------------------------------------------------------------------
void D3DEnumerator::buildPresentIntervalList( D3DDeviceInfo & DeviceInfo, D3DDeviceCombo & DeviceCombo )
{
	const unsigned int  piArray[] = 
	{ 
		D3DPRESENT_INTERVAL_IMMEDIATE,
		D3DPRESENT_INTERVAL_DEFAULT,
		D3DPRESENT_INTERVAL_ONE,
		D3DPRESENT_INTERVAL_TWO,
		D3DPRESENT_INTERVAL_THREE,
		D3DPRESENT_INTERVAL_FOUR,
	} ;
	const unsigned int piArrayCount = sizeof( piArray ) / sizeof( piArray[0] );

	for( unsigned int ipi = 0; ipi < piArrayCount; ipi++ )
	{
		unsigned int pi = piArray[ipi];
		
		if( DeviceCombo.IsWindowed )
		{
			if( pi == D3DPRESENT_INTERVAL_TWO  ||  pi == D3DPRESENT_INTERVAL_THREE  ||  pi == D3DPRESENT_INTERVAL_FOUR  )
				continue;
		}
        
		// Note that D3DPRESENT_INTERVAL_DEFAULT is zero, so you
		// can't do a caps check for it -- it is always available.
      if( pi == D3DPRESENT_INTERVAL_DEFAULT || (DeviceInfo.Caps.PresentationIntervals & pi ) )
			DeviceCombo.PresentIntervalList.add( &pi );
    }
}

bool D3DEnumerator::findBestWindowedMode( int iAdapterOrdinal, bool bRequireHAL, bool bRequireREF, D3DBestMatch & BestDevice )
{
	assert( m_pD3D );
    
	BestDevice.pBestAdapterInfo = NULL;
	BestDevice.pBestDeviceInfo = NULL;
	BestDevice.pBestDeviceCombo = NULL;

	bool					bFoundBestCombo = false;
	unsigned int		iAdapterStart = 0;
	unsigned int		iAdapterEnd = m_AdapterInfoList.count( );

	if( iAdapterOrdinal >= 0 )
	{
		for( unsigned int i = 0; i < m_AdapterInfoList.count( ); i++ )
		{
			D3DAdapterInfo * pAdapterInfo = (D3DAdapterInfo *) m_AdapterInfoList.getPtr( i );

			if( pAdapterInfo->AdapterOrdinal == iAdapterOrdinal )
			{
				iAdapterStart = iAdapterEnd = i;
				break;
			}
		}

		if( iAdapterStart != iAdapterEnd )
			return false;
	}

	do
	{
		D3DDISPLAYMODE adapterDesktopDisplayMode;
    
		D3DAdapterInfo * pAdapterInfo = (D3DAdapterInfo *) m_AdapterInfoList.getPtr( iAdapterStart );

		HRESULT result = m_pD3D->GetAdapterDisplayMode( pAdapterInfo->AdapterOrdinal, &adapterDesktopDisplayMode );

		if( FAILED( result ) )
			continue;

		for( unsigned int idi = 0; idi < pAdapterInfo->DeviceInfoList.count( ); idi++ )
		{
			D3DDeviceInfo * pDeviceInfo = (D3DDeviceInfo *) pAdapterInfo->DeviceInfoList.getPtr( idi );

			if( bRequireHAL  &&  pDeviceInfo->DevType != D3DDEVTYPE_HAL )
				continue;
			if( bRequireREF  &&  pDeviceInfo->DevType != D3DDEVTYPE_REF )
				continue;

			for( unsigned int idc = 0; idc < pDeviceInfo->DeviceComboList.count( ); idc++ )
			{
				D3DDeviceCombo * pDeviceCombo = (D3DDeviceCombo *) pDeviceInfo->DeviceComboList.getPtr( idc );

				bool bAdapterMatchesBackBuffer = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);

				if( !pDeviceCombo->IsWindowed )
					continue;
				if( pDeviceCombo->AdapterFormat != adapterDesktopDisplayMode.Format )
					continue;

				// If we haven't found a compatible DeviceCombo yet, or if this set
				// is better (because it's a HAL, and/or because formats match better),
				// save it
				if( (BestDevice.pBestDeviceCombo == NULL)  || 
					(BestDevice.pBestDeviceCombo->DevType != D3DDEVTYPE_HAL)  &&  (pDeviceCombo->DevType == D3DDEVTYPE_HAL)  ||
					(pDeviceCombo->DevType == D3DDEVTYPE_HAL)  &&  bAdapterMatchesBackBuffer )
				{
					BestDevice.pBestAdapterInfo = pAdapterInfo;
					BestDevice.pBestDeviceInfo = pDeviceInfo;
					BestDevice.pBestDeviceCombo = pDeviceCombo;
		
					if( (pDeviceCombo->DevType == D3DDEVTYPE_HAL)  &&  bAdapterMatchesBackBuffer )
						bFoundBestCombo = true;
            }
			}
		}
    } while( (++iAdapterStart < iAdapterEnd)  &&  !bFoundBestCombo );

	return (BestDevice.pBestAdapterInfo != NULL) ? true : false;
}

bool D3DEnumerator::findBestFullscreenMode( int iAdapterOrdinal, bool bRequireHAL, bool bRequireREF, bool bMatchAdapter, D3DBestMatch & BestDevice )
{
	assert( m_pD3D );

	BestDevice.pBestAdapterInfo = NULL;
	BestDevice.pBestDeviceInfo = NULL;
	BestDevice.pBestDeviceCombo = NULL;

	if( m_AdapterInfoList.count( ) == 0 )
		return false;

	bool					bFoundBestCombo = false;
	unsigned int		iAdapterStart = 0;
	unsigned int		iAdapterEnd = m_AdapterInfoList.count( );

	if( iAdapterOrdinal >= 0 )
	{
		for( unsigned int i = 0; i < m_AdapterInfoList.count( ); i++ )
		{
			D3DAdapterInfo * pAdapterInfo = (D3DAdapterInfo *) m_AdapterInfoList.getPtr( i );

			if( pAdapterInfo->AdapterOrdinal == iAdapterOrdinal )
			{
				iAdapterStart = iAdapterEnd = i;
				break;
			}
		}

		if( iAdapterStart != iAdapterEnd )
			return false;
	}

	do
	{
		D3DDISPLAYMODE adapterDesktopDisplayMode;

		D3DAdapterInfo * pAdapterInfo = (D3DAdapterInfo *) m_AdapterInfoList.getPtr( iAdapterStart );

		HRESULT result = m_pD3D->GetAdapterDisplayMode( pAdapterInfo->AdapterOrdinal, &adapterDesktopDisplayMode );

		if( FAILED( result ) )
			continue;

		for( unsigned int idi = 0; idi < pAdapterInfo->DeviceInfoList.count( ); idi++ )
		{
			D3DDeviceInfo * pDeviceInfo = (D3DDeviceInfo *) pAdapterInfo->DeviceInfoList.getPtr( idi );
            
			if( bRequireHAL  &&  (pDeviceInfo->DevType != D3DDEVTYPE_HAL) )
				continue;

			if( bRequireREF  &&  (pDeviceInfo->DevType != D3DDEVTYPE_REF) )
				continue;

			for( unsigned int idc = 0; idc < pDeviceInfo->DeviceComboList.count( ); idc++ )
			{
				D3DDeviceCombo * pDeviceCombo = (D3DDeviceCombo *) pDeviceInfo->DeviceComboList.getPtr( idc );

				bool bGoodBackBufferFormat = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat)  ||  (bMatchAdapter == false);
				bool bGoodAdapterFormat = (pDeviceCombo->AdapterFormat == adapterDesktopDisplayMode.Format)  ||  (bMatchAdapter == false);

				if( pDeviceCombo->IsWindowed )
					continue;

				// If we haven't found a compatible set yet, or if this set
				// is better (because it's a HAL, and/or because formats match better),
				// save it

				if( BestDevice.pBestDeviceCombo == NULL  ||
					(BestDevice.pBestDeviceCombo->DevType != D3DDEVTYPE_HAL)  &&  (pDeviceInfo->DevType == D3DDEVTYPE_HAL)  ||
					(pDeviceCombo->DevType == D3DDEVTYPE_HAL)  &&  (BestDevice.pBestDeviceCombo->AdapterFormat != adapterDesktopDisplayMode.Format && bGoodAdapterFormat)  ||
               (pDeviceCombo->DevType == D3DDEVTYPE_HAL)  &&  bGoodAdapterFormat  &&  bGoodBackBufferFormat )
				{
					BestDevice.pBestAdapterInfo = pAdapterInfo;
					BestDevice.pBestDeviceInfo = pDeviceInfo;
					BestDevice.pBestDeviceCombo = pDeviceCombo;
                    
					if( (pDeviceInfo->DevType == D3DDEVTYPE_HAL)  &&  bGoodAdapterFormat  &&  bGoodBackBufferFormat )
						bFoundBestCombo = true;
				}
			}
		}
	} while( (++iAdapterStart < iAdapterEnd)  &&  !bFoundBestCombo );

	return (BestDevice.pBestAdapterInfo != NULL) ? true : false;
}


//-----------------------------------------------------------------------------
// Name: CArrayList constructor
// Desc: 
//-----------------------------------------------------------------------------
CArrayList::CArrayList( ArrayListType Type, UINT BytesPerEntry )
{
    if( Type == AL_REFERENCE )
        BytesPerEntry = sizeof(void*);
    m_ArrayListType = Type;
    m_pData = NULL;
    m_BytesPerEntry = BytesPerEntry;
    m_NumEntries = 0;
    m_NumEntriesAllocated = 0;
}



//-----------------------------------------------------------------------------
// Name: CArrayList destructor
// Desc: 
//-----------------------------------------------------------------------------
CArrayList::~CArrayList( void )
{
    if( m_pData != NULL )
        delete[] m_pData;
}




//-----------------------------------------------------------------------------
// Name: CArrayList::Add
// Desc: Adds pEntry to the list.
//-----------------------------------------------------------------------------
HRESULT CArrayList::add( void* pEntry )
{
    if( m_BytesPerEntry == 0 )
        return E_FAIL;
    if( m_pData == NULL || m_NumEntries + 1 > m_NumEntriesAllocated )
    {
        void* pDataNew;
        UINT NumEntriesAllocatedNew;
        if( m_NumEntriesAllocated == 0 )
            NumEntriesAllocatedNew = 16;
        else
            NumEntriesAllocatedNew = m_NumEntriesAllocated * 2;
        pDataNew = new BYTE[NumEntriesAllocatedNew * m_BytesPerEntry];
        if( pDataNew == NULL )
            return E_OUTOFMEMORY;
        if( m_pData != NULL )
        {
            CopyMemory( pDataNew, m_pData, m_NumEntries * m_BytesPerEntry );
            delete[] m_pData;
        }
        m_pData = pDataNew;
        m_NumEntriesAllocated = NumEntriesAllocatedNew;
    }

    if( m_ArrayListType == AL_VALUE )
        CopyMemory( (BYTE*)m_pData + (m_NumEntries * m_BytesPerEntry), pEntry, m_BytesPerEntry );
    else
        *(((void**)m_pData) + m_NumEntries) = pEntry;
    m_NumEntries++;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CArrayList::Remove
// Desc: Remove the item at Entry in the list, and collapse the array. 
//-----------------------------------------------------------------------------
void CArrayList::remove( UINT Entry )
{
    // Decrement count
    m_NumEntries--;

    // Find the entry address
    BYTE* pData = (BYTE*)m_pData + (Entry * m_BytesPerEntry);

    // Collapse the array
    MoveMemory( pData, pData + m_BytesPerEntry, ( m_NumEntries - Entry ) * m_BytesPerEntry );
}




//-----------------------------------------------------------------------------
// Name: CArrayList::GetPtr
// Desc: Returns a pointer to the Entry'th entry in the list.
//-----------------------------------------------------------------------------
void* CArrayList::getPtr( UINT Entry )
{
    if( m_ArrayListType == AL_VALUE )
        return (BYTE*)m_pData + (Entry * m_BytesPerEntry);
    else
        return *(((void**)m_pData) + Entry);
}




//-----------------------------------------------------------------------------
// Name: CArrayList::Contains
// Desc: Returns whether the list contains an entry identical to the 
//       specified entry data.
//-----------------------------------------------------------------------------
bool CArrayList::contains( void* pEntryData )
{
    for( UINT iEntry = 0; iEntry < m_NumEntries; iEntry++ )
    {
        if( m_ArrayListType == AL_VALUE )
        {
            if( memcmp( getPtr(iEntry), pEntryData, m_BytesPerEntry ) == 0 )
                return true;
        }
        else
        {
            if( getPtr(iEntry) == pEntryData )
                return true;
        }
    }
    return false;
}

