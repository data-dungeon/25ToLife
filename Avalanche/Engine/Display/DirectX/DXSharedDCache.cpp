#include <Assert.h>

#include "DirectX.h"
#include "DXSharedDCache.h"

DXSharedDCache	DXShaderDCache;

DXSharedDCache::DXSharedDCache( )
{
	p_DXDevice = 0;

	m_Lights = 0;
	p_Lights = 0;

	m_FogNear = 0.0f;
	m_FogFar = 0.0f;
}

void DXSharedDCache::initDXDevice( IDirect3DDevice8 * _DXDevice )
{
	assert( _DXDevice );
	assert( p_DXDevice == 0 );

	p_DXDevice = _DXDevice;

	createDeviceData( );
}

void DXSharedDCache::createDeviceData( void )
{
	D3DCAPS8					DeviceCaps;

	HRESULT result = p_DXDevice->GetDeviceCaps( & DeviceCaps );
	assert( result == D3D_OK );

	m_Lights = DeviceCaps.MaxActiveLights;

	if( m_Lights )
	{
		p_Lights = new D3DLIGHT8[DeviceCaps.MaxActiveLights];

		for( unsigned int i = 0; i < m_Lights; i++ )
			p_Lights[i].Type = (D3DLIGHTTYPE) 0;
	}
	else
		p_Lights = 0;
}

void DXSharedDCache::releaseDXDevice( void )
{
	releaseDeviceData( );

	p_DXDevice = 0;
}

void DXSharedDCache::releaseDeviceData( void )
{
	if( p_Lights )
	{
		delete [] p_Lights;

		p_Lights = 0;
	}
}

void DXSharedDCache::sendFFLights( void )
{
	assert( p_DXDevice );

	for( unsigned int i = 0; i < m_Lights; i++ )
	{
		HRESULT		result = D3D_OK;

		if( p_Lights[i].Type != (D3DLIGHTTYPE) 0 )
		{
			result = p_DXDevice->SetLight( i, p_Lights + i );
			assert( result == D3D_OK );

			result = p_DXDevice->LightEnable( i, true );
			assert( result == D3D_OK );
		}
		else
		{
			result = p_DXDevice->LightEnable( i, false );
			assert( result == D3D_OK );
		}
	}
}



DXSharedDCache::~DXSharedDCache( )
{
	releaseDeviceData( );
}