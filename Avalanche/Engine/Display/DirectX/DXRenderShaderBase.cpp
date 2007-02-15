#include <Assert.h>

#include "DXRenderShaderBase.h"
#include "DXVertexElement9.h"

IDirect3DDevice8 * DXRenderShaderBase::p_DXDevice = 0;

void DXRenderShaderBase::initDXDevice( IDirect3DDevice8 * pDXDevice )
{
	assert( pDXDevice );
	assert( p_DXDevice == 0 );

	p_DXDevice = pDXDevice;
}

void DXRenderShaderBase::releaseDXDevice( void )
{
	p_DXDevice = 0;
}

D3DXMATRIX * DXRenderShaderBase::getWorldView( void )
{
	static D3DXMATRIX		mFinal;
	D3DXMATRIX				mWorld, mView;

	HRESULT result = D3D_OK;

	result = p_DXDevice->GetTransform( D3DTS_WORLD, & mWorld );
	assert( result == D3D_OK );

	result = p_DXDevice->GetTransform( D3DTS_VIEW, & mView );
	assert( result == D3D_OK );

	D3DXMatrixMultiply( & mFinal, & mWorld, & mView );
	
	return & mFinal;
}

D3DXMATRIX * DXRenderShaderBase::getWorldViewT( void )
{
	D3DXMATRIX * pTransform = getWorldView( );
	assert( pTransform );

	D3DXMatrixTranspose( pTransform, pTransform );

	return pTransform;
}

D3DXMATRIX * DXRenderShaderBase::getWorldProjection( void )
{
	static D3DXMATRIX		mFinal;
	D3DXMATRIX				mWorld, mView, mProjection;

	HRESULT result = D3D_OK;

	result = p_DXDevice->GetTransform( D3DTS_WORLD, & mWorld );
	assert( result == D3D_OK );

	result = p_DXDevice->GetTransform( D3DTS_VIEW, & mView );
	assert( result == D3D_OK );

	result = p_DXDevice->GetTransform( D3DTS_PROJECTION, & mProjection );
	assert( result == D3D_OK );

	D3DXMatrixMultiply( & mFinal, & mWorld, & mView );
	D3DXMatrixMultiply( & mFinal, & mFinal, & mProjection );
	
	return & mFinal;
}

D3DXMATRIX * DXRenderShaderBase::getWorldProjectionT( void )
{
	D3DXMATRIX * pTransform = getWorldProjection( );
	assert( pTransform );

	D3DXMatrixTranspose( pTransform, pTransform );

	return pTransform;
}

D3DXMATRIX * DXRenderShaderBase::getViewProjectionT( void )
{
	static D3DXMATRIX		mFinal;
	D3DXMATRIX				mView, mProjection;

	HRESULT result = D3D_OK;

	result = p_DXDevice->GetTransform( D3DTS_VIEW, & mView );
	assert( result == D3D_OK );

	result = p_DXDevice->GetTransform( D3DTS_PROJECTION, & mProjection );
	assert( result == D3D_OK );

	D3DXMatrixMultiply( & mFinal, & mView, & mProjection );
	
	D3DXMatrixTranspose( & mFinal, & mFinal );

	return & mFinal;
}
