#include <Assert.h>

#include "DXVShaderBase.h"
#include "DXVertexElement9.h"

IDirect3DDevice8 * DXVShaderBase::p_DXDevice = 0;

void DXVShaderBase::initDXDevice( IDirect3DDevice8 * pDXDevice )
{
	assert( pDXDevice );
	assert( p_DXDevice == 0 );

	p_DXDevice = pDXDevice;
}

void DXVShaderBase::releaseDXDevice( void )
{
	p_DXDevice = 0;
}

D3DXMATRIX * DXVShaderBase::getWorldView( void )
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

D3DXMATRIX * DXVShaderBase::getWorldViewT( void )
{
	D3DXMATRIX * pTransform = getWorldView( );
	assert( pTransform );
	
	D3DXMatrixTranspose( pTransform, pTransform );

	return pTransform;
}

D3DXMATRIX * DXVShaderBase::getWorldProjection( void )
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

D3DXMATRIX * DXVShaderBase::getWorldProjectionT( void )
{
	D3DXMATRIX * pTransform = getWorldProjection( );
	assert( pTransform );

	D3DXMatrixTranspose( pTransform, pTransform );

	return pTransform;
}

D3DXMATRIX * DXVShaderBase::getViewProjectionT( void )
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
