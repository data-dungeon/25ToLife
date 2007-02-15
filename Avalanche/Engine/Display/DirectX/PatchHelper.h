#ifndef __PATCH_HELPER_H
#define __PATCH_HELPER_H

#include "Display/DirectX/DXPublished.h"
#include "Game/Database/Som.h"

#include "DXAnimShader.h"
#include "DXAnimShadowCastShader.h"

#include "DXPublished.h"

class StandardTCMaterial : public DXMaterialBase
{
public:
	enum RenderMode
	{
		MODE_DEFAULT = 0,
		MODE_SHADOW_CAST,
		MODE_SHADOW_APPLY
	} ;

private:
	static DXRenderShaderBase *	m_CurrentShader;
	static bool							m_BlendAlpha;
	static unsigned int				m_CurrentFVF;
	static unsigned int				m_TextureUnitsClearStart;
	static unsigned int				m_DisableMaps;
	static RenderMode					m_RenderMode;

	unsigned int				m_ShaderFlags;

	DXAnimShader *				p_Shader;	
	DXAnimShadowCastShader * p_ShadowCastShader;

	ts_TextureContext *		p_TextureContext;

	const DXStreamDescriptor * p_Descriptor;

	DWORD							pPShader;

	void							setMaterial( void );

	void							setupShadowCast( void );
	void							setupBaseMaterial( void );

	void							applyVSPShader( unsigned int pass );
	void							applyVSMaterial( unsigned int pass );
	void							applyFFPMaterial( unsigned int pass );
	void							createPixelShader( void );

public:
	StandardTCMaterial( const DXMeshInterface & MeshInterface, const ShaderRecord & _Record, ts_TextureContext * p_TC );
	~StandardTCMaterial( );

	ts_TextureContext *		getTextureContext( void )				{ return p_TextureContext; }

	void							meshStart( void );
	static void					setRenderMode( RenderMode mode )		{ m_RenderMode = mode; }

	unsigned int				numPasses( ) const						{ return 1; }
	void							applyPass( unsigned int pass );
				
};

#endif