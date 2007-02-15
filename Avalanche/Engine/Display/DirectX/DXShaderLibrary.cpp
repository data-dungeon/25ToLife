#include "DXShaderLibrary.h"
#include "DXVShaderHashEntry.h"

#include "DXVertexElement9.h"

DXShaderLibrary			DXShaderLib;

DXShaderLibrary::DXShaderLibrary( ) :
	m_ShaderTable( 256 )
{
	b_PermanentFlag = false;
}

unsigned int DXShaderLibrary::descriptorEntries( void * pStreamDescriptor )
{
	D3DVERTEXELEMENT9 * pElements = (D3DVERTEXELEMENT9 *) pStreamDescriptor;

	unsigned int streamElements = 0;

	while( pElements[streamElements].Stream != 0xff )
		streamElements++;

	return streamElements;
}

DXVShader * DXShaderLibrary::createVertexShader( void * pStreamDescriptor, unsigned int numStreamComponents, const extShaderDesc & shaderDesc )
{
	DXVShaderHashEntry HashEntry( shaderDesc.Code.pData, pStreamDescriptor, descriptorEntries( pStreamDescriptor ), b_PermanentFlag );

	const DXVShaderHashEntry * pTableEntry = m_ShaderTable.find( HashEntry, HashEntry.d_Key );

	if( pTableEntry )
	{
		assert( pTableEntry->p_Shader );

		return pTableEntry->p_Shader;
	}
	else
	{
		unsigned int		numDescriptorTokens = 0;

		HashEntry.p_LocalDescriptor = createStreamDescriptor( shaderDesc.Defn, shaderDesc.Decl, pStreamDescriptor, numStreamComponents, numDescriptorTokens );

		DXVShader * pVShader = new DXVShader( shaderDesc.Code.pData, HashEntry.p_LocalDescriptor, numDescriptorTokens );

		HashEntry.p_Shader = pVShader;
		m_ShaderTable.insert( HashEntry, HashEntry.d_Key );

		return pVShader;
	}

}

DXVShader * DXShaderLibrary::createVertexShader( const DWORD * pStreamDescriptor, unsigned int numDescriptorTokens, const DWORD * pShaderTokens )
{
	DXVShaderHashEntry HashEntry( pShaderTokens, pStreamDescriptor, numDescriptorTokens, b_PermanentFlag );

	const DXVShaderHashEntry * pTableEntry = m_ShaderTable.find( HashEntry, HashEntry.d_Key );

	if( pTableEntry )
	{
		assert( pTableEntry->p_Shader );

		return pTableEntry->p_Shader;
	}
	else
	{
		unsigned int		numDescriptorTokens = 0;

		DXVShader * pVShader = new DXVShader( pShaderTokens, pStreamDescriptor, numDescriptorTokens );

		HashEntry.p_Shader = pVShader;
		m_ShaderTable.insert( HashEntry, HashEntry.d_Key );

		return pVShader;
	}

}

void DXShaderLibrary::releaseShader( DXVShader * pShader )
{
	assert( pShader );

	if( pShader->getRefCount( ) > 0 )
		return;

	DXVShaderHashEntry HashEntry( pShader, m_ShaderTable.hashPtr( pShader->getShaderTokens( ) ) );

	const DXVShaderHashEntry * pEntry = m_ShaderTable.find( HashEntry, HashEntry.d_Key );

	if( pEntry )
	{
		if( pEntry->b_Permanent )
			return;

		if( pEntry->p_LocalDescriptor )
			delete [] pEntry->p_LocalDescriptor;

		m_ShaderTable.remove( *pEntry, pEntry->d_Key );
	}

	delete pShader;
}


#ifdef _XBOX

int getRegNumber( D3DDECLUSAGE findUsage, unsigned int findIndex )
{
	switch( findUsage )
	{
	case D3DDECLUSAGE_POSITION:
		switch( findIndex )
		{
		case 0:
			return D3DVSDE_POSITION;
		case 1:
			return 13;
		default:
			assert( 0 );
			return -1;
		}
	case D3DDECLUSAGE_BLENDWEIGHT:
		assert( findIndex == 0 );
		return D3DVSDE_BLENDWEIGHT;

	case D3DDECLUSAGE_BLENDINDICES:
		assert( findIndex == 0 );
		return 13;

	case D3DDECLUSAGE_NORMAL:
		switch( findIndex )
		{
		case 0:
			return D3DVSDE_NORMAL;
		case 1:
			return 14;
		case 2:
			return 15;
		default:
			assert( 0 );
			return -1;
		}

	case D3DDECLUSAGE_COLOR:
		switch( findIndex )
		{
		case 0:
			return D3DVSDE_DIFFUSE;
		case 1:
			return D3DVSDE_SPECULAR;
		case 2:
			return D3DVSDE_BACKDIFFUSE;
		case 3:
			return D3DVSDE_BACKSPECULAR;
		default:
			assert( 0 );
			return -1;
		}

	case D3DDECLUSAGE_FOG:
		assert( findIndex == 0 );
		return D3DVSDE_FOG;
	case D3DDECLUSAGE_PSIZE:
		assert( findIndex == 0 );
		return D3DVSDE_PSIZE;

	case D3DDECLUSAGE_TEXCOORD:
		assert( findIndex < 4 );
		return 9 + findIndex;
	case D3DDECLUSAGE_BINORMAL:
		assert( findIndex == 0 );
		return 14;
	case D3DDECLUSAGE_TANGENT:
		assert( findIndex == 0 );
		return 15;
	default:
		assert( 0 );
		return -1;
	}
}

DWORD * DXShaderLibrary::createStreamDescriptor( const shaderDescEntry & constDesc, const shaderDescEntry & streamDesc, void *pStreamDescriptor, unsigned int numStreamComponents, unsigned int & numDescriptorTokens )
{
	int	convertedType[] =
	{
		D3DVSDT_FLOAT1,
		D3DVSDT_FLOAT2,
		D3DVSDT_FLOAT3,
		D3DVSDT_FLOAT4,
		D3DVSDT_D3DCOLOR,
		-1,
		D3DVSDT_SHORT2,
		D3DVSDT_SHORT4
	} ;

	D3DVERTEXELEMENT9 * pElements = (D3DVERTEXELEMENT9 *) pStreamDescriptor;

	unsigned int streamElements = 0;

	while( pElements[streamElements].Stream != 0xff )
		streamElements++;

	numDescriptorTokens = constDesc.lenData + numStreamComponents + streamElements + 1;

	DWORD * pDecl = new DWORD[numDescriptorTokens];

	if( constDesc.lenData )
		memcpy( pDecl, constDesc.pData, constDesc.lenData * sizeof( DWORD ) );

	DWORD * pStreamDesc = pDecl + constDesc.lenData;

	int currentStreamIndex = pElements->Stream;

	*pStreamDesc++ = D3DVSD_STREAM( currentStreamIndex );

	while( streamElements-- )
	{
		if( pElements->Stream != currentStreamIndex )
		{
			assert( pElements->Stream > currentStreamIndex );
	
			currentStreamIndex = pElements->Stream;

			*pStreamDesc++ = D3DVSD_STREAM( currentStreamIndex );
		}

		assert( pElements->Type < (sizeof( convertedType ) / sizeof( convertedType[0] )) );
		assert( convertedType[pElements->Type] != -1 );

		unsigned int	regNo = getRegNumber( (D3DDECLUSAGE) pElements->Usage, pElements->UsageIndex );

		*pStreamDesc++ = D3DVSD_REG( regNo, convertedType[pElements->Type] );

		pElements++;
	}

	*pStreamDesc++ = D3DVSD_END( );

	assert( pStreamDesc == (pDecl + numDescriptorTokens) );

	return pDecl;
}

#else

int getRegNumber( const shaderDescEntry & streamDecl, D3DDECLUSAGE findUsage, unsigned int findIndex )
{
	assert( (streamDecl.lenData % 3) == 0 );

	unsigned int numRegisters = streamDecl.lenData / 3;

	DWORD * pTokens = streamDecl.pData;

	while( numRegisters-- )
	{
		DWORD	Instruction = *pTokens++;
		assert( Instruction = 0x0000001f );

		D3DDECLUSAGE Usage = (D3DDECLUSAGE) (*pTokens & 0x0f);
		unsigned int Index = (*pTokens++ >> 16) & 0x0f;

		unsigned int reg = *pTokens++ & 0x000003ff;

		if( (findUsage == Usage)  &&  (findIndex == Index) )
			return reg;
	}

	return -1;
}

DWORD * DXShaderLibrary::createStreamDescriptor( const shaderDescEntry & constDesc, const shaderDescEntry & streamDesc, void *pStreamDescriptor, unsigned int numStreamComponents, unsigned int & numDescriptorTokens )
{

	D3DVERTEXELEMENT9 * pElements = (D3DVERTEXELEMENT9 *) pStreamDescriptor;

	unsigned int streamElements = 0;

	while( pElements[streamElements].Stream != 0xff )
		streamElements++;

	numDescriptorTokens = constDesc.lenData + numStreamComponents + streamElements + 1;

	DWORD * pDecl = new DWORD[numDescriptorTokens];

	if( constDesc.lenData )
		memcpy( pDecl, constDesc.pData, constDesc.lenData * sizeof( DWORD ) );

	DWORD * pStreamDesc = pDecl + constDesc.lenData;

	if( streamDesc.lenData )
	{
		int currentStreamIndex = pElements->Stream;

		*pStreamDesc++ = D3DVSD_STREAM( currentStreamIndex );

		while( streamElements-- )
		{
			if( pElements->Stream != currentStreamIndex )
			{
				assert( pElements->Stream > currentStreamIndex );
	
				currentStreamIndex = pElements->Stream;

				*pStreamDesc++ = D3DVSD_STREAM( currentStreamIndex );
			}

			unsigned int regNo = getRegNumber( streamDesc, (D3DDECLUSAGE) pElements->Usage, pElements->UsageIndex );

			if( regNo == -1 )
				*pStreamDesc++ = D3DVSD_SKIP( numDWORDs( (D3DDECLTYPE) pElements->Type ) );
			else
				*pStreamDesc++ = D3DVSD_REG( regNo, pElements->Type );

			pElements++;
		}

	}

	*pStreamDesc++ = D3DVSD_END( );

	assert( pStreamDesc == (pDecl + numDescriptorTokens) );

	return pDecl;
}

#endif
