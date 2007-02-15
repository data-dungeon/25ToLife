#include "Display/DisplayPCH.h"
#include "DXMeshBase.h"

void DXMeshBase::createStreamDescriptor( void * pDescriptorData, void * & pDescriptor )
{
	pDescriptor = pDescriptorData;
}

void DXMeshBase::releaseStreamDescriptor( void * pDescriptor )
{
}

void DXMeshBase::setStreamSource( unsigned int stream, DynamicVB & VB, unsigned int offset, unsigned int stride )
{
	ASSERT( pDXDevice );

	HRESULT result = pDXDevice->SetStreamSource( stream, VB, stride );	
	ASSERT( result == D3D_OK );
}

void DXMeshBase::setIndices( DynamicIB & IB )
{
	ASSERT( pDXDevice );

	HRESULT result = pDXDevice->SetIndices( IB, m_DIPBaseVertex );
	ASSERT( result == D3D_OK );
}

void DXMeshBase::setVertexDeclaration( void * pDeclaration )
{
}

void DXMeshBase::callDIP( DXDIPData & DIPData )
{
	ASSERT( pDXDevice );
	ASSERT( *( (int *) & m_DIPBaseVertex) != -1 );
	ASSERT( *( (int *) & m_DIPBaseIndex) != -1 );

	HRESULT result = pDXDevice->DrawIndexedPrimitive( 
		D3DPT_TRIANGLESTRIP,
		DIPData.d_DIPMinIndex,
		DIPData.d_DIPNumVertices,
		m_DIPBaseIndex + DIPData.d_IBBase,
		DIPData.d_DIPPrimitiveCount );

	ASSERT( result == D3D_OK );
}
