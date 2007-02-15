#include "Display/DisplayPCH.h"
#include <d3d8.h>
#include <d3dx8.h>

#include "DXBase.h"
#include "DXMeshBase.h"

/* statics used by the device init.
	they can be set before calling initDXDevice
	*/

unsigned int	DXMeshBase::numUserIB16Indices = 64 * 1024;
unsigned int	DXMeshBase::numUserIB32Indices = 0;

unsigned int	DXMeshBase::numUserVBVertexBytes = 128 * 1024;

/* statics used by derived classes */
IDirect3DDevice8 * DXMeshBase::pDXDevice = 0;

DynamicIB *	DXMeshBase::p_UserIB16	= 0;
DynamicIB *	DXMeshBase::p_UserIB32	= 0;

DynamicVB *	DXMeshBase::p_UserVB		= 0;


void DXMeshBase::initDXDevice( IDirect3DDevice8 * pNewDXDevice )
{

	ASSERT( pDXDevice == 0 );
	ASSERT( pNewDXDevice );

	pDXDevice = pNewDXDevice;

	createDeviceObjects( );
}

void DXMeshBase::createDeviceObjects( void )
{

	if( numUserIB16Indices )
	{
		p_UserIB16 = new DynamicIB( pDXDevice, numUserIB16Indices, D3DFMT_INDEX16, true );
		DXBase.addDOCallback( p_UserIB16 );
	}
	#if !defined (_XBOX)		// INDEX32 doesnt exist on XBOX
	if( numUserIB32Indices )
	{
		p_UserIB32 = new DynamicIB( pDXDevice, numUserIB32Indices, D3DFMT_INDEX32, true );
		DXBase.addDOCallback( p_UserIB32 );
	}
	#endif

	if( numUserVBVertexBytes )
	{
		p_UserVB = new DynamicVB( pDXDevice, numUserVBVertexBytes, 0, true );
		DXBase.addDOCallback( p_UserVB );
	}

}

void DXMeshBase::deleteDeviceObjects( void )
{
	if( p_UserIB16 )
	{
		DXBase.removeDOCallback( p_UserIB16 );
		delete p_UserIB16;

		p_UserIB16 = 0;
	}

	if( p_UserIB32 )
	{
		DXBase.removeDOCallback( p_UserIB32 );
		delete p_UserIB32;

		p_UserIB32 = 0;
	}

	if( p_UserVB )
	{
		DXBase.removeDOCallback( p_UserVB );
		delete p_UserVB;

		p_UserVB = 0;
	}
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
	DXData.p_VB = 0;

	DXData.m_NumVertices = VBData.d_NumVertices;
	DXData.m_VertexStride = VBData.d_VertexBytes;
	DXData.m_StreamBytes = DXData.m_VertexStride * DXData.m_NumVertices;
	DXData.p_StreamData = pBase + VBData.d_OffsetVertexData;

/* debug code for fucking with RGBs
	unsigned char * pRGBs;
	if( DXData.m_VertexStride == 24 )
		pRGBs = (unsigned char *) ((unsigned int) DXData.p_StreamData + (sizeof(float) * 3));
	else
		pRGBs = (unsigned char *) ((unsigned int) DXData.p_StreamData + (sizeof(float) * 6));

	for( unsigned int i = 0; i < DXData.m_NumVertices; i++ )
	{
		pRGBs[0] = 255;
		pRGBs[1] = 255;
		pRGBs[2] = 255;
		pRGBs[3] = 255;

		pRGBs += DXData.m_VertexStride;
	}
 */

}

void DXMeshBase::initDXIndexStream( IndexDataRecord & IBData, char * pBase, DXIndexStream & DXData )
{
	#if !defined (_XBOX)		// INDEX32 doesnt exist on XBOX
	ASSERT( ( IBData.d_CIBFormat == D3DFMT_INDEX16 )  ||  ( IBData.d_CIBFormat == D3DFMT_INDEX32 ) );
	#else
	ASSERT( IBData.d_CIBFormat == D3DFMT_INDEX16 );
	#endif

	DXData.p_IB = 0;

	DXData.m_IndexFormat = (D3DFORMAT) IBData.d_CIBFormat;
	DXData.m_NumIndices = IBData.d_NumIndices;
	DXData.p_IndexData = pBase + IBData.d_OffsetIndexData;
}

DynamicVB * DXMeshBase::loadDXVertexStream( DXVertexStream & vertexStream, bool bNeedZeroBase )
{
	HRESULT						result = D3D_OK;

	ASSERT( vertexStream.p_StreamData );

	DynamicVB * pVB = ( vertexStream.p_VB != 0 ) ? vertexStream.p_VB : getUserVB( 0 );
	ASSERT( pVB );

	if( bNeedZeroBase  &&  pVB->currentOffset( ) )
		pVB->setFlush( );

	void * pVertexMem = pVB->lock( vertexStream.m_VertexStride, vertexStream.m_NumVertices, m_DIPBaseVertex );
	ASSERT( pVertexMem );

	memcpy( pVertexMem, vertexStream.p_StreamData, vertexStream.m_VertexStride * vertexStream.m_NumVertices );

	pVB->unlock( );

	return pVB;
}

 DynamicIB * DXMeshBase::loadDXIndexStream( DXIndexStream & indexStream )
{
	unsigned int				indexSize = indexStream.m_IndexFormat == D3DFMT_INDEX16 ? 2 : 4;

	ASSERT( indexStream.p_IndexData );

	DynamicIB * pIB = ( indexStream.p_IB != 0 ) ? indexStream.p_IB : getUserIB( indexStream.m_IndexFormat );
	ASSERT( pIB );

	void * pIndexMem = pIB->lock( indexStream.m_NumIndices, m_DIPBaseIndex );
	ASSERT( pIndexMem );

	memcpy( pIndexMem, indexStream.p_IndexData, indexStream.m_NumIndices * indexSize );

	pIB->unlock( );

	return pIB;
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
		ASSERT( result == D3D_OK );
	}
}


		
