#include <xtl.h>
#include <xgraphics.h>

#include "DXBase.h"
#include "DXMeshBase_Xbox.h"

#include "platform/xbox/xbresource.h"

/* statics used by the device init.
	they can be set before calling initDXDevice
	*/

unsigned int			DXMeshBase::numUserIB16Indices = 0; //32 * 1024;
unsigned int			DXMeshBase::numUserVBVertexBytes = 0; //64 * 1024;

/* statics used by derived classes */
IDirect3DDevice8 *	DXMeshBase::pDXDevice = 0;

DynamicIB *				DXMeshBase::p_UserIB	= 0;
DynamicVB *				DXMeshBase::p_UserVB	= 0;

CXBPackedResource *	DXMeshBase::m_Resources = 0;

void DXMeshBase::initDXDevice( IDirect3DDevice8 * pNewDXDevice )
{

	assert( pDXDevice == 0 );
	assert( pNewDXDevice );

	pDXDevice = pNewDXDevice;

	createDeviceObjects( );
}

void DXMeshBase::createDeviceObjects( void )
{

	if( numUserIB16Indices )
	{
		p_UserIB = new DynamicIB( pDXDevice, numUserIB16Indices, D3DFMT_INDEX16, true );
		DXBase.addDOCallback( p_UserIB );
	}

	if( numUserVBVertexBytes )
	{
		p_UserVB = new DynamicVB( pDXDevice, numUserVBVertexBytes, 0, true );
		DXBase.addDOCallback( p_UserVB );
	}

}

void DXMeshBase::deleteDeviceObjects( void )
{
	if( p_UserIB )
	{
		DXBase.removeDOCallback( p_UserIB );
		delete p_UserIB;

		p_UserIB = 0;
	}

	if( p_UserVB )
	{
		DXBase.removeDOCallback( p_UserVB );
		delete p_UserVB;

		p_UserVB = 0;
	}
}

void DXMeshBase::setActiveResources( CXBPackedResource * pResources )
{
	m_Resources = pResources;
}


void DXMeshBase::releaseDeviceObjects( void )
{

}

void DXMeshBase::restoreDeviceObjects( void )
{

}

void DXMeshBase::releaseDXDevice( void )
{
	deleteDeviceObjects( );

	pDXDevice = 0;
}


void DXMeshBase::initDXVertexStream( VertexDataRecord & VBData, char * pBase, DXVertexStream & DXData )
{
	if( m_Resources )
		DXData.p_VB = m_Resources->GetVertexBuffer( VBData.d_OffsetVertexData );
	else
		DXData.p_VB = 0;

	DXData.m_NumVertices = VBData.d_NumVertices;
	DXData.m_VertexStride = VBData.d_VertexBytes;
	DXData.m_StreamBytes = DXData.m_VertexStride * DXData.m_NumVertices;
	DXData.p_StreamData = 0;

}

void DXMeshBase::initDXIndexStream( IndexDataRecord & IBData, char * pBase, DXIndexStream & DXData )
{
	assert( IBData.d_CIBFormat == D3DFMT_INDEX16 );

	if( m_Resources )
		DXData.p_IB = (IDirect3DIndexBuffer8 *) m_Resources->GetIndexBuffer( IBData.d_OffsetIndexData );
	else
		DXData.p_IB = 0;

	DXData.m_IndexFormat = (D3DFORMAT) IBData.d_CIBFormat;
	DXData.m_NumIndices = IBData.d_NumIndices;
	DXData.p_IndexData = 0;
}

IDirect3DVertexBuffer8 * DXMeshBase::loadDXVertexStream( DXVertexStream & vertexStream, bool bNeedZeroBase )
{
	assert( vertexStream.p_VB );

	return vertexStream.p_VB;
}

IDirect3DIndexBuffer8 * DXMeshBase::loadDXIndexStream( DXIndexStream & indexStream )
{
	assert( indexStream.p_IB );

	return indexStream.p_IB;
}

void DXMeshBase::loadMatrixList( const D3DXMATRIX * pMatrices, D3DXMATRIX *pOptTransform, MatrixList & List, unsigned int baseSlot )
{

	for( unsigned int i = 0; i < List.m_NumTransfers; i++ )
	{
		D3DXMATRIX mFinalT;		

		if( pOptTransform )
		{
			D3DXMATRIX mFinal = pMatrices[List.a_MatrixList[i].m_Index] * ( *pOptTransform );

			D3DXMatrixTranspose( & mFinalT, & mFinal );
		}
		else
		{
			D3DXMatrixTranspose( & mFinalT, & pMatrices[List.a_MatrixList[i].m_Index] );
		}


		HRESULT result = pDXDevice->SetVertexShaderConstant( baseSlot + ( List.a_MatrixList[i].m_Slot * 4 ), mFinalT, 4 );
		assert( result == D3D_OK );
	}
}

void DXMeshBase::createStreamDescriptor( void * pDescriptorData, void * & pDescriptor )
{
	pDescriptor = pDescriptorData;
}

void DXMeshBase::releaseStreamDescriptor( void * pDescriptor )
{

}

void DXMeshBase::setIndices( DynamicIB & IB )
{
	assert( pDXDevice );

	HRESULT result = pDXDevice->SetIndices( IB, m_DIPBaseVertex );
	assert( result == D3D_OK );
}

void DXMeshBase::setVertexDeclaration( void * pDeclaration )
{
}

void DXMeshBase::callDIP( DXDIPData & DIPData )
{
	assert( pDXDevice );
	assert( *( (int *) & m_DIPBaseVertex) != -1 );
	assert( *( (int *) & m_DIPBaseIndex) != -1 );

	HRESULT result = pDXDevice->DrawIndexedPrimitive( 
		D3DPT_TRIANGLESTRIP,
		DIPData.d_DIPMinIndex,
		DIPData.d_DIPNumVertices,
		m_DIPBaseIndex + DIPData.d_IBBase,
		DIPData.d_DIPPrimitiveCount );

	assert( result == D3D_OK );
}

		
