#include <xtl.h>
#include <xgraphics.h>

#include "DXStaticMesh_Xbox.h"
#include "DXMeshInterface.h"

DXStaticMesh::DXStaticMesh( const MeshRecord &meshRec )
{
	assert( meshRec.d_NumIDRecords );
	assert( meshRec.d_NumVDRecords );
	assert( meshRec.d_OffsetIDRecords );
	assert( meshRec.d_OffsetVDRecords );
	assert( meshRec.d_OffsetSubMeshRecords );

	unsigned int			i;
	char *					pBase = (char *) & meshRec;

	// allocate the Index Streams
	m_NumIndexStreams = meshRec.d_NumIDRecords;
	a_IndexStreams = new DXIndexStream[m_NumIndexStreams];
	assert( a_IndexStreams );

 	// init the Index Streams
	IndexDataRecord *		pIDRecords = (IndexDataRecord *) ( pBase + meshRec.d_OffsetIDRecords );
	for( i = 0; i < m_NumIndexStreams; i++)
		initDXIndexStream( pIDRecords[i], pBase, a_IndexStreams[i] );

	// allocate the Vertex Streams
	m_NumVertexStreams = meshRec.d_NumVDRecords;
	a_VertexStreams = new DXVertexStream[m_NumVertexStreams];
	assert( a_VertexStreams );

	// init the Index Streams
	VertexDataRecord *	pVDRecords = (VertexDataRecord *) ( pBase + meshRec.d_OffsetVDRecords );
	for( i = 0; i < m_NumVertexStreams; i++)
		initDXVertexStream( pVDRecords[i], pBase, a_VertexStreams[i] ); 

	// init the Matrix Lists
	m_NumMatrixLists = meshRec.d_NumMLRecords;
	
	if( m_NumMatrixLists )
	{
		a_MatrixLists = new MatrixList [m_NumMatrixLists];
		assert( a_MatrixLists );
		
		MLDataRecord *		pMLRecords = (MLDataRecord *) ( pBase + meshRec.d_OffsetMLRecords );

		for( i = 0; i < m_NumMatrixLists; i++)
		{
			a_MatrixLists[i].m_NumTransfers = pMLRecords[i].d_NumMatrices;
			a_MatrixLists[i].a_MatrixList = (MatrixTransfer *) ( pBase + pMLRecords[i].d_OffsetML );
		}

		m_MatrixIndexMax = meshRec.d_MatrixIndexMax;

		m_MatrixCacheSize = meshRec.d_MatrixCacheSize;
		p_CachedMatrices = new int [m_MatrixCacheSize];
	}
	else
	{
		a_MatrixLists = 0;

		m_MatrixIndexMax = 0;

		m_MatrixCacheSize = 0;
		p_CachedMatrices = 0;
	}


	// allocate the SubMeshes
 	m_NumSubMeshes = meshRec.d_NumSubMeshRecords;
	a_SubMeshes = new DXSubMesh[m_NumSubMeshes];
	assert( a_SubMeshes );

	// init the material indices
	m_NumShaderRecords = meshRec.d_NumShaderRecords;
	p_ShaderRecords = (ShaderRecord *) ( pBase + meshRec.d_OffsetShaderRecords);

	// init the SubMehes
	SubMeshRecord *		p_SMRecords = (SubMeshRecord *) ( pBase + meshRec.d_OffsetSubMeshRecords );
	for( i = 0; i < m_NumSubMeshes; i++)
		a_SubMeshes[i].p_SubMeshRecord = p_SMRecords + i;

	m_NumSingleStreams = meshRec.d_NumSSRecords;

	if( m_NumSingleStreams )
	{
		a_SingleStreams = new DXSingleStream[m_NumSingleStreams];

		SSDataRecord *		pSSRecords = (SSDataRecord *) ( pBase + meshRec.d_OffsetSSRecords );

		for( i = 0; i < m_NumSingleStreams; i++ )
		{
			assert( pSSRecords[i].d_NumSubMeshes );

			a_SingleStreams[i].m_Desc.m_StreamFVF = pSSRecords[i].d_FVF;
			a_SingleStreams[i].m_Desc.m_StreamFlags = pSSRecords[i].d_StreamFlags;
			a_SingleStreams[i].m_Desc.m_StreamComponents = 1;
			createStreamDescriptor( (pBase + pSSRecords[i].d_OffsetDescriptor), a_SingleStreams[i].m_Desc.p_Descriptor );

			a_SingleStreams[i].m_NumSubMeshes = pSSRecords[i].d_NumSubMeshes;

			a_SingleStreams[i].ap_SubMeshes = new DXSubMesh * [a_SingleStreams[i].m_NumSubMeshes];

			a_SingleStreams[i].p_Stream = & a_VertexStreams[pSSRecords[i].d_VBDataIndex];

			unsigned int idx = 0;

			for( unsigned int mesh = 0; mesh < m_NumSubMeshes; mesh++ )
				if( a_SubMeshes[mesh].p_SubMeshRecord->d_VertexStreamIndex == i )
					a_SingleStreams[i].ap_SubMeshes[idx++] = a_SubMeshes + mesh;

			assert( idx == a_SingleStreams[i].m_NumSubMeshes );
		}
	}

	m_NumMultiStreams = 0;
	a_MultiStreams = 0;

	p_ActiveMatrices = 0;

	m_ActiveIndexStream = -1;
	m_ActiveVertexStream = -1;

	m_MatrixBase = -1;

	p_ActiveIB = 0;
	p_ActiveVB = 0;
}


void DXStaticMesh::ReleaseDeviceData( void )
{
	for( unsigned int i = 0; i < m_NumSingleStreams; i++ )
		if( a_SingleStreams[i].m_Desc.p_Descriptor )
			releaseStreamDescriptor( a_SingleStreams[i].m_Desc.p_Descriptor );

}
		
DXStaticMesh::~DXStaticMesh( void )
{
	ReleaseDeviceData( );

	if( a_SingleStreams )
	{
		for( unsigned int i = 0; i < m_NumSingleStreams; i++ )
			delete [] a_SingleStreams[i].ap_SubMeshes;

		delete [] a_SingleStreams;
	}

	if( p_CachedMatrices )
		delete [] p_CachedMatrices;

	if( a_VertexStreams )
	{
		for( unsigned int i = 0; i < m_NumVertexStreams; i++ )
			if( a_VertexStreams[i].p_VB )
				a_VertexStreams[i].p_VB->Release( );

		delete [] a_VertexStreams;
	}

	if( a_IndexStreams )
	{
		for( unsigned int i = 0; i < m_NumIndexStreams; i++ )
			if( a_IndexStreams[i].p_IB )
				a_IndexStreams[i].p_IB->Release( );

		delete [] a_IndexStreams;
	}

	if( a_MatrixLists )
		delete [] a_MatrixLists;

	if( a_SubMeshes )
		delete [] a_SubMeshes;

}

void DXStaticMesh::activateIndexStream( unsigned int stream )
{
	assert( stream < m_NumIndexStreams );

	if( stream != m_ActiveIndexStream )
	{
//		p_ActiveIB = loadDXIndexStream( a_IndexStreams[stream] );
//		assert( p_ActiveIB );

		m_ActiveIndexStream = stream;
	}
}

void DXStaticMesh::activateMatrixList( unsigned int list, const D3DXMATRIX * pOptTransform )
{
	assert( m_MatrixBase > -1 );
	assert( p_ActiveMatrices );
	assert( list < m_NumMatrixLists );

	MatrixList & List = a_MatrixLists[list];

	for( unsigned int i = 0; i < List.m_NumTransfers; i++ )
	{
		assert( List.a_MatrixList[i].m_Slot < m_MatrixCacheSize );

		if( p_CachedMatrices[List.a_MatrixList[i].m_Slot] != List.a_MatrixList[i].m_Index )
		{
			p_CachedMatrices[List.a_MatrixList[i].m_Slot] = List.a_MatrixList[i].m_Index;

			D3DXMATRIX mFinalT;		

			if( pOptTransform )
			{
				D3DXMATRIX mFinal = p_ActiveMatrices[List.a_MatrixList[i].m_Index] * ( *pOptTransform );

				D3DXMatrixTranspose( & mFinalT, & mFinal );
			}
			else
			{
				D3DXMatrixTranspose( & mFinalT, & p_ActiveMatrices[List.a_MatrixList[i].m_Index] );
			}


			HRESULT result = pDXDevice->SetVertexShaderConstant( m_MatrixBase + ( List.a_MatrixList[i].m_Slot * 4 ), mFinalT, 4 );
			assert( result == D3D_OK );
		}
	}

}

void DXStaticMesh::clearCachedMatrices( void )
{
	for( unsigned int i = 0; i < m_MatrixCacheSize; i++ )
		p_CachedMatrices[i] = -1;
}
			
void DXStaticMesh::activateSingleVertexStream( unsigned int stream, DXStreamRenderInfo & info )
{
	assert( stream < m_NumSingleStreams );
	assert( a_SingleStreams[stream].p_Stream );

	m_ActiveVertexStream = stream;
	m_DIPBaseVertex = 0;

	if( a_SingleStreams[stream].p_Stream->p_VB == 0 )
	{
		info.ap_SubMeshes = 0;
		info.m_NumSubMeshes = 0;
		return;
	}

	HRESULT result = pDXDevice->SetStreamSource( 0, a_SingleStreams[stream].p_Stream->p_VB, a_SingleStreams[stream].p_Stream->m_VertexStride );
	assert( result == D3D_OK );

	info.p_Desc = & a_SingleStreams[stream].m_Desc;

	info.m_NumSubMeshes = a_SingleStreams[stream].m_NumSubMeshes;
	info.ap_SubMeshes = a_SingleStreams[stream].ap_SubMeshes;
}

void DXStaticMesh::render( DXMaterialBase * Materials[] )
{
	int						currentMatrixList = (a_MatrixLists == 0) ? 0 : -1;

	if( a_MatrixLists )
		clearCachedMatrices( );

	HRESULT result = pDXDevice->SetIndices( a_IndexStreams[0].p_IB, 0 );
	assert( result == D3D_OK );

	for( unsigned int i = 0; i < numSingleStreams( ); i++ )
	{
		DXStreamRenderInfo			info;

		activateSingleVertexStream( i, info );

		for( unsigned int mesh = 0; mesh < info.m_NumSubMeshes; mesh++ )
		{
			SubMeshRecord * subMesh = info.ap_SubMeshes[mesh]->p_SubMeshRecord;
			assert( subMesh );

			if( currentMatrixList != subMesh->d_MatrixListIndex )
			{
				activateMatrixList( subMesh->d_MatrixListIndex, 0 );
				currentMatrixList = subMesh->d_MatrixListIndex;
			}

			if( Materials[subMesh->d_ShaderListIndex] )
			{
				DXMaterialBase & currentMaterial = * Materials[subMesh->d_ShaderListIndex];

				const unsigned int numPasses = currentMaterial.numPasses( );

				for( unsigned int pass = 0; pass < numPasses; pass++ )
				{
					unsigned int numIndices = subMesh->d_DIPData.d_DIPPrimitiveCount + 2;
					WORD * pIndices = D3D__IndexData + subMesh->d_DIPData.d_IBBase;

					currentMaterial.applyPass( mesh );

					HRESULT result = pDXDevice->DrawIndexedVertices( D3DPT_TRIANGLESTRIP, numIndices, pIndices );
					assert( result == D3D_OK );
				}
			}
		}
	}

	endRendering( );
}








