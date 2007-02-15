#ifndef __DX_ANIM_SHADOW_CAST_SHADER_H
#define __DX_ANIM_SHADOW_CAST_SHADER_H

#include "DXRenderShaderBase.h"
#include "DXStreamDescriptor.h"
#include "DXBase.h"
#include "DXPublished.h"
#include "DXVShader.h"

class DXAnimShadowCastShader : public DXRenderShaderBase
{
	DWORD *					p_LocalDesc;
	DXVShader &				d_Shader;

	void					sendShaderData( void );

public:
	DXAnimShadowCastShader( 
		DXVShader &				_Shader
		) ;

	~DXAnimShadowCastShader( void );

	void					render( void );
} ;

#endif

