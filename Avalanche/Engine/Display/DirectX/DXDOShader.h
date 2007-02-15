#ifndef __DX_DO_SHADER_H
#define __DX_DO_SHADER_H

#include "DXBase.h"

class DXDOShader : public DXDeviceObjectClass
{

	const DWORD *			p_Tokens;
	const DWORD *			p_Decl;

	DWORD						p_Shader;

public:
	DXDOShader( 
		const DWORD *		pShaderTokens,
		const DWORD *		pShaderDecl
		) :
		p_Tokens( pShaderTokens ),
		p_Decl( pShaderDecl )
	{
		assert( DXBase.p_DXDevice );

		HRESULT result = DXBase.p_DXDevice->CreateVertexShader( pShaderDecl, pShaderTokens, & p_Shader, 0 );
		assert( result == D3D_OK );

		DXBase.addDOCallback( this );
	}

	~DXDOShader( void )
	{
		releaseDeviceObjects( );

		DXBase.removeDOCallback( this );
	}

	void				releaseDeviceObjects( void )
	{
		if( p_Shader )
		{
			assert( DXBase.p_DXDevice );

			HRESULT result = DXBase.p_DXDevice->DeleteVertexShader( p_Shader );
			assert( result == D3D_OK );

			p_Shader = 0;
		}
	}

	void				restoreDeviceObjects( void )
	{
		assert( DXBase.p_DXDevice );
		assert( p_Shader == 0 );

		HRESULT result = DXBase.p_DXDevice->CreateVertexShader( p_Decl, p_Tokens, & p_Shader, 0 );
		assert( result == D3D_OK );
	}

	void				set( void )
	{
		assert( DXBase.p_DXDevice );
		assert( p_Shader );

		HRESULT result = DXBase.p_DXDevice->SetVertexShader( p_Shader );
		assert( result == D3D_OK );
	}

} ;

#endif


