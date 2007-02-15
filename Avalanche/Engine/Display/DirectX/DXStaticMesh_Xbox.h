#ifndef __DX_STATIC_MESH_H
#define __DX_STATIC_MESH_H

#include "FileRecords.h"
#include "DXMeshBase_Xbox.h"

class DXMaterialBase;

class DXStaticMesh : public DXMeshBase
{
	unsigned int					m_NumIndexStreams;
	DXIndexStream *				a_IndexStreams;

	unsigned int					m_NumVertexStreams;
	DXVertexStream *				a_VertexStreams;

	unsigned int					m_NumSingleStreams;
	DXSingleStream *				a_SingleStreams;

	unsigned int					m_NumMultiStreams;
	DXMultiStream *				a_MultiStreams;

	unsigned int					m_MatrixIndexMax;

	unsigned int					m_MatrixCacheSize;
	int *								p_CachedMatrices;

	D3DXMATRIX *					p_ActiveMatrices;

	unsigned int					m_NumMatrixLists;
	MatrixList		*				a_MatrixLists;

	unsigned int					m_NumShaderRecords;
	ShaderRecord *					p_ShaderRecords;

	unsigned int					m_NumSubMeshes;
	DXSubMesh *						a_SubMeshes;

	int								m_ActiveIndexStream;
	int								m_ActiveVertexStream;

	int								m_MatrixBase;

	DynamicIB *						p_ActiveIB;
	DynamicVB *						p_ActiveVB;

	friend class					DXMeshInterface;

	void							clearCachedMatrices( void );
	void							testDIP( const DXDIPData & Data );

public:
	DXStaticMesh(
		const MeshRecord & 		meshRec
	);

	~DXStaticMesh( );

	unsigned int				numSingleStreams( void ) const						{ return m_NumSingleStreams; }

	void							activateIndexStream( unsigned int stream );
   void							activateSingleVertexStream( unsigned int vertexStream, DXStreamRenderInfo & info );
	void							activateMatrixList( unsigned int list, const D3DXMATRIX * pOptTransform );

	void							render( DXMaterialBase * Materials[] );

	void							endRendering( void )
	{
		assert( pDXDevice );
		
		HRESULT result = D3D_OK;
		
		pDXDevice->SetDebugMarker( 0 );

		result = pDXDevice->SetIndices( 0, 0 );
		assert( result == D3D_OK );

		result = pDXDevice->SetStreamSource( 0, 0, 0 );
		assert( result == D3D_OK );
		
		m_ActiveVertexStream = -1;
		m_ActiveIndexStream = -1;

		DXMeshBase::endRendering( );
	}

	unsigned int				getMatrixArraySize( void )	const						{ return m_MatrixIndexMax; }

	void							setMatrixArrayVertexRegisterBase( unsigned int baseSlot )
	{
		assert( m_ActiveVertexStream == -1 );

		m_MatrixBase = baseSlot;
	}

	void							setMatrixArray( D3DXMATRIX * pMatrices, unsigned int numMatrices )
	{ 
		assert( numMatrices > m_MatrixIndexMax );
		
		p_ActiveMatrices = pMatrices; 
	}

	void							ReleaseDeviceData( void );
} ;

#endif __DX_STATIC_MESH_H