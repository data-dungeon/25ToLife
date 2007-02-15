#ifndef __DX_PUBLISHED_H
#define __DX_PUBLISHED_H

#include "DirectX.h"

#include "DXDeviceObjectClass.h"
#include "DXMeshInterface.h"


namespace DXPublished
{
	void							initDXDevice( IDirect3DDevice8 * _DXDevice );
	void							releaseDXDevice( void );

	void							addLostDeviceCallback( DXDeviceObjectClass * pObject );
	void							removeLostDeviceCallback( DXDeviceObjectClass * pObject );

	void							releaseDeviceObjects( void );
	void							restoreDeviceObjects( void );

	DXMeshInterface *			createMeshInterface( void * pData );

	void							setWorldMatrix( const D3DXMATRIX & matWorld );
	void							setViewMatrix( const D3DXMATRIX & matView );
	void							setProjectionMatrix( const D3DXMATRIX & matProj );

} ;

#endif
