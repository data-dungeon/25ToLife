#include <Assert.h>

#include "DXAnimShader.h"
#include "DXShaderLibrary.h"

#include "DXSharedDCache.h"

DXAnimShader::RegsLight		DXAnimShader::m_RegDataLights;

DXAnimShader::DXAnimShader( DXVShader & Shader, bool _SendLights, float _SpecularPower ) :
	b_SendLights( _SendLights ),
	d_SpecularPower( _SpecularPower ),
	p_LocalDesc( 0 ),
	d_Shader( Shader )
{
	d_Shader.incRef( );
}

DXAnimShader::~DXAnimShader( void )
{
	d_Shader.decRef( );

	DXShaderLib.releaseShader( & d_Shader );
}
	
void DXAnimShader::convertHWLights( void )
{
	unsigned int				i;
	D3DXMATRIX					mWorld, mWorldLocal;

	HRESULT result = D3D_OK;

	result = p_DXDevice->GetTransform( D3DTS_WORLD, & mWorld );
	assert( result == D3D_OK );

	D3DXMATRIX * pM = D3DXMatrixInverse( & mWorldLocal, NULL, & mWorld );
	if( pM == NULL )
		D3DXMatrixIdentity( &mWorldLocal );

	unsigned int plight = 0;
	unsigned int dlight = 0;

	for( i = 0; i < DXShaderDCache.m_Lights; i++ )
	{
		D3DLIGHT8 * pLight = DXShaderDCache.p_Lights + i;

		switch( pLight->Type )
		{
		case (D3DLIGHTTYPE) 0:
			break;
		case D3DLIGHT_POINT:
			if( plight < MAX_PLIGHTS )
			{
				memcpy( & m_RegDataLights.PLights[plight].Clr, & pLight->Diffuse, sizeof( D3DCOLORVALUE ) );

				D3DXVec3TransformCoord( (D3DXVECTOR3 *) & m_RegDataLights.PLights[plight].Pos, (D3DXVECTOR3 *) & pLight->Position, & mWorldLocal );

				m_RegDataLights.PLights[plight].Attenuations[0] = pLight->Attenuation0;
				m_RegDataLights.PLights[plight].Attenuations[1] = pLight->Attenuation1;
				m_RegDataLights.PLights[plight].Attenuations[2] = pLight->Attenuation2;

				plight++;
			}
			break;
		case D3DLIGHT_DIRECTIONAL:
			if( dlight < MAX_DLIGHTS )
			{
				memcpy( & m_RegDataLights.DLights[dlight].Clr, & pLight->Diffuse, sizeof( D3DCOLORVALUE ) );

				D3DXVec3TransformNormal( (D3DXVECTOR3 *) & m_RegDataLights.DLights[dlight].Dir, (D3DXVECTOR3 *) & pLight->Direction, & mWorldLocal );

				dlight++;
			}
			break;
		}
	}

	if( d_SpecularPower )
	{
		m_RegDataLights.SLight.Clr.r = 1.0f;
		m_RegDataLights.SLight.Clr.g = 1.0f;
		m_RegDataLights.SLight.Clr.b = 1.0f;

		m_RegDataLights.SLight.Pos_Power[0] = 1000.0f;
		m_RegDataLights.SLight.Pos_Power[1] = 1000.0f;
		m_RegDataLights.SLight.Pos_Power[2] = 0.0f;
		m_RegDataLights.SLight.Pos_Power[3] = d_SpecularPower;
	}
	else
	{
		m_RegDataLights.SLight.Clr.r = 0.0f;
		m_RegDataLights.SLight.Clr.g = 0.0f;
		m_RegDataLights.SLight.Clr.b = 0.0f;

		m_RegDataLights.SLight.Pos_Power[3] = 0.0f;
	}
	

	D3DXVECTOR3	skyVec( 0.0f, 1.0f, 0.0f );

	D3DXVec3TransformNormal( (D3DXVECTOR3 *) & m_RegDataLights.SkyVec, & skyVec, & mWorldLocal );

	memcpy( & m_RegDataLights.ASkyColor, & DXShaderDCache.m_SkyColor, sizeof( D3DCOLORVALUE ) );
	memcpy( & m_RegDataLights.AGroundColor, & DXShaderDCache.m_GroundColor, sizeof( D3DCOLORVALUE ) );

	D3DCOLORVALUE	zeroColor = { 0 };

	for( i = plight; i < MAX_PLIGHTS; i++)
		memcpy( & m_RegDataLights.PLights[i].Clr, & zeroColor, sizeof( D3DCOLORVALUE ) );

	for( i = dlight; i < MAX_DLIGHTS; i++)
		memcpy( & m_RegDataLights.DLights[i].Clr, & zeroColor, sizeof( D3DCOLORVALUE ) );

}

void DXAnimShader::sendShaderData( void )
{
	assert( DirectX_Device );
	assert( DXBase.p_DXDevice );

	HRESULT result = D3D_OK;

// get world eye pos
	D3DXMATRIX * pWV = getWorldView( );
	assert( pWV );

	D3DXMATRIX	mViewWorld;
	D3DXVECTOR3	eyePosView( 0.0f, 0.0f, 0.0f );

	if( D3DXMatrixInverse( & mViewWorld, NULL, pWV ) != NULL )
		D3DXVec3TransformCoord( (D3DXVECTOR3 *) & m_RegDataLights.EyePosWorld, & eyePosView, & mViewWorld );

	// get / set world-view matris
	D3DXMATRIX * pWVT = getWorldViewT( );
	assert( pWVT );

	result = p_DXDevice->SetVertexShaderConstant( 0, pWVT, 3 );
	assert( result == D3D_OK );

	// get / set world-projection matrix
	D3DXMATRIX * pWPT = getWorldProjectionT( );
	assert( pWPT );

	result = p_DXDevice->SetVertexShaderConstant( 4, pWPT, 4 );
	assert( result == D3D_OK );	

	float		fogData[4];

	if( DXShaderDCache.m_FogFar == 0.0f )
	{
		fogData[0] = 0.0f;
		fogData[1] = 0.0f;
	}
	else
	{
		assert( DXShaderDCache.m_FogFar > DXShaderDCache.m_FogNear );

		fogData[0] = DXShaderDCache.m_FogFar;
		fogData[1] = 1.0f / (DXShaderDCache.m_FogFar - DXShaderDCache.m_FogNear );
	}

	result = p_DXDevice->SetVertexShaderConstant( 8, & fogData, 1 );
	assert( result == D3D_OK );

	if( b_SendLights )
	{
		convertHWLights( );

		unsigned int numLightRegs = sizeof( m_RegDataLights ) / 16;
		result = p_DXDevice->SetVertexShaderConstant( 9, & m_RegDataLights, numLightRegs );
		assert( result == D3D_OK );
	}

}

void DXAnimShader::render( void )
{
	assert( p_DXDevice );

	sendShaderData( );

	d_Shader.set( );
}
