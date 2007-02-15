#ifndef __DX_SHADER_LIBRARY_H
#define __DX_SHADER_LIBRARY_H

#include "DirectX.h"

#include "CHashTable.h"

#include "DXExtShaderDesc.h"
#include "DXStreamDescriptor.h"
#include "DXVShader.h"
#include "DXVShaderHashEntry.h"

class DXShaderLibrary
{
	bool						b_PermanentFlag;

	CHashTable<DXVShaderHashEntry>	m_ShaderTable;

public:
	DXShaderLibrary( );

	static unsigned int	descriptorEntries( void * pStreamDescriptor );
	static DWORD *			createStreamDescriptor( const shaderDescEntry & constDesc, const shaderDescEntry & streamDesc, void *pStreamDescriptor, unsigned int numStreamComponents, unsigned int & numDescriptorTokens );

	void						setPermanentFlag( bool bPermanent )	{ b_PermanentFlag = bPermanent; }

	DXVShader *				createVertexShader( const DWORD * pStreamDescTokens, unsigned int numDescriptorTokens, const DWORD * pShaderTokens );
	DXVShader *				createVertexShader( void * pStreamDescriptor, unsigned int numStreamComponents, const extShaderDesc & shaderDesc );

	void						releaseShader( DXVShader * pShader );
	void						releaseNonPermanent( void );
} ;


extern DXShaderLibrary DXShaderLib;
#endif
