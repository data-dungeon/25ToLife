#ifndef __DX_SHARED_DCACHE_H
#define __DX_SHARED_DCACHE_H

#include "DirectX.h"

class DXSharedDCache
{
	IDirect3DDevice8 *						p_DXDevice;

	void						createDeviceData( void );
	void						releaseDeviceData( void );

public:
	unsigned int								m_Lights;
	D3DLIGHT8 *									p_Lights;

	D3DCOLORVALUE								m_SkyColor;
	D3DCOLORVALUE								m_GroundColor;

	float											m_FogNear;
	float											m_FogFar;

	DXSharedDCache( );
	~DXSharedDCache( );

	void						initDXDevice( IDirect3DDevice8 * pDXDevice );
	void						releaseDXDevice( void ); 

	void						sendFFLights( void );
} ;

extern DXSharedDCache	DXShaderDCache;

#endif
