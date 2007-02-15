#ifndef __DX_VSHADER_BASE_H
#define __DX_VSHADER_BASE_H

#include "DirectX.h"

#include "DXExtShaderDesc.h"
#include "DXStreamDescriptor.h"

class DXVShaderBase
{

protected:
	static IDirect3DDevice8 * p_DXDevice;


public:
	static void				initDXDevice( IDirect3DDevice8 * pDXDevice );
	static void				releaseDXDevice( void );

	virtual void			render( void ) = 0;

	static D3DXMATRIX *	getWorldView( void );
	static D3DXMATRIX *	getWorldViewT( void );

	static D3DXMATRIX *	getWorldProjection( void );
	static D3DXMATRIX *	getWorldProjectionT( void );

	static D3DXMATRIX *	getViewProjectionT( void );

} ;

#endif
