#ifndef __DX_V_SHADER_H
#define __DX_V_SHADER_H

#include <Assert.h>

#include "DirectX.h"
#include "DXBase.h"

class DXVShader : public DXDeviceObjectClass
{
	const DWORD *			p_Tokens;
	const DWORD *			p_Decl;

	unsigned int			m_DeclTokens;

	unsigned int			m_RefCount;

	DWORD						p_Shader;

public:
	DXVShader( 
		const DWORD *		pShaderTokens,
		const DWORD *		pShaderDecl,
		unsigned int		_DeclTokens
		) :
		m_RefCount( 0 ),
		p_Tokens( pShaderTokens ),
		p_Decl( pShaderDecl ),
		m_DeclTokens( _DeclTokens )
	{
		assert( DXBase.p_DXDevice );

		HRESULT result = DXBase.p_DXDevice->CreateVertexShader( pShaderDecl, pShaderTokens, & p_Shader, 0 );
		assert( result == D3D_OK );

		DXBase.addDOCallback( this );
	}

	~DXVShader( void )
	{
		assert( m_RefCount == 0 );

		releaseDeviceObjects( );

		DXBase.removeDOCallback( this );
	}

	unsigned int	incRef( void )				{ return ++m_RefCount; }
	unsigned int	decRef( void )				{ return --m_RefCount; }
	unsigned int	getRefCount( void )		{ return m_RefCount; }

	const DWORD *	getShaderTokens( void )	{ return p_Tokens; }

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