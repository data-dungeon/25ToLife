#ifndef __DX_VSHADER_HASH_ENTRY_H
#define __DX_VSHADER_HASH_ENTRY_H

#include "CHashTable.h"
#include "DXVShader.h"

class DXShaderLibrary;

class DXVShaderHashEntry
{
	friend DXShaderLibrary;

	unsigned int			d_Key;

	bool						b_Permanent;

	const DWORD *			p_ShaderTokens;

	const void *			p_Descriptor;
	unsigned int			d_DescriptorTokens;

	DWORD *					p_LocalDescriptor;

	DXVShader *				p_Shader;

public:
	DXVShaderHashEntry(
		const DWORD *				_ShaderTokens,
		const void *				_StreamDescriptor,
		unsigned int				_DescriptorTokens,
		bool							_Permanent = false
		) :
	p_ShaderTokens( _ShaderTokens ),
	p_Descriptor( _StreamDescriptor ),
	d_DescriptorTokens( _DescriptorTokens ),
	d_Key( CHashTable<DXVShaderHashEntry>::hashPtr( _ShaderTokens ) ),
	b_Permanent( _Permanent )
	{
		p_Shader = 0;
		p_LocalDescriptor = 0;
	}

	DXVShaderHashEntry( 
		DXVShader *					_Shader,
		unsigned int				_Key
		) :
	p_Shader( _Shader ),
	d_Key( _Key )
	{
		p_ShaderTokens = 0;
		p_LocalDescriptor = 0;
		p_Descriptor = 0;
		d_DescriptorTokens = 0;
		b_Permanent = false;
	}

	bool						operator==( const DXVShaderHashEntry & Other ) const
	{
		if( p_Shader  &&  (Other.p_Shader == p_Shader) )
				return true;

		if( p_ShaderTokens != p_ShaderTokens )
			return false;

		if( d_DescriptorTokens != Other.d_DescriptorTokens )
			return false;

		return memcmp( p_Descriptor, Other.p_Descriptor, d_DescriptorTokens * sizeof( DWORD ) ) == 0 ? true : false;
	}

} ;

#endif
