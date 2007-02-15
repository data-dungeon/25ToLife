#include "Display/DisplayPCH.h"
#include "DXMeshInterface.h"

#include "DXStaticMesh.h"

IDirect3DDevice8 * DXMaterialBase::p_DXDevice = 0;

void DXMaterialBase::initDXDevice( IDirect3DDevice8 * _DXDevice )
{
	ASSERT( _DXDevice );

	p_DXDevice = _DXDevice;
}

void DXMaterialBase::releaseDXDevice( void )
{
	p_DXDevice = 0;
}

DXMeshInterface::~DXMeshInterface( void )
{
	delete &m_Mesh;
}

const ShaderRecord * DXMeshInterface::getShaderRecords( unsigned int & count ) const
{
	count = m_Mesh.m_NumShaderRecords;

	return m_Mesh.p_ShaderRecords;
}

const DXStreamDescriptor * DXMeshInterface::getStreamDescriptor( unsigned int streamIdx ) const
{
	ASSERT( streamIdx < m_Mesh.m_NumSingleStreams );

	return & m_Mesh.a_SingleStreams[streamIdx].m_Desc;
}

void DXMeshInterface::setMatrixArray( D3DXMATRIX * pMatrices, unsigned int numMatrices )
{
	m_Mesh.setMatrixArray( pMatrices, numMatrices );
}

void DXMeshInterface::setDefaultMaterials( DXMaterialBase ** _Materials, unsigned int numMaterials )
{
	ASSERT( numMaterials == m_Mesh.m_NumShaderRecords );

	p_DefaultMaterials = _Materials;
}

DXMaterialBase ** DXMeshInterface::getDefaultMaterials( unsigned int & numMaterials )
{
	numMaterials = p_DefaultMaterials ? m_Mesh.m_NumShaderRecords : 0;

	return p_DefaultMaterials; 
}

void DXMeshInterface::renderDefault( void )
{
	ASSERT( p_DefaultMaterials );

	p_DefaultMaterials[0]->meshStart( );
	m_Mesh.render( p_DefaultMaterials );
}

