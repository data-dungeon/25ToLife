#include <Assert.h>

#include "DXAnimShadowCastShader.h"
#include "DXShaderLibrary.h"

DXAnimShadowCastShader::DXAnimShadowCastShader( DXVShader & Shader ) :
	d_Shader( Shader )
{
	p_LocalDesc = 0;
	d_Shader.incRef( );
}

DXAnimShadowCastShader::~DXAnimShadowCastShader( void )
{
	d_Shader.decRef( );

	DXShaderLib.releaseShader( & d_Shader );
}
	
void DXAnimShadowCastShader::sendShaderData( void )
{
	assert( p_DXDevice );

	HRESULT result = D3D_OK;

	D3DXMATRIX * pWPTransform = getWorldProjectionT( );
	assert( pWPTransform );

	result = p_DXDevice->SetVertexShaderConstant( 4, pWPTransform, 4 );
	assert( result == D3D_OK );	

	D3DCOLORVALUE	shadowColor = { .55f, .55f, .55f, 1.0f } ;

	result = p_DXDevice->SetVertexShaderConstant( 8, & shadowColor, 1 );
	assert( result == D3D_OK );

}

void DXAnimShadowCastShader::render( void )
{
	assert( p_DXDevice );

	sendShaderData( );

	d_Shader.set( );
}
