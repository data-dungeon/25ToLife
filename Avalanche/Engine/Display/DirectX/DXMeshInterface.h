#ifndef __DX_MESH_INTERFACE_H
#define __DX_MESH_INTERFACE_H

#include <d3d8.h>
#include <d3dx8.h>

#include "ShaderRecords.h"
#include "DXStreamDescriptor.h"


#ifdef MASSIVE_AD
#include "Massive\MassiveAdObject.h"
#endif


class DXStaticMesh;
class DXShaderBase;

class DXMaterialBase
{

protected:
	static IDirect3DDevice8 *		p_DXDevice;

public:
	virtual void				meshStart( void ) { };
	virtual unsigned int		numPasses( void ) const = 0;
	virtual void				applyPass( unsigned int ) = 0;

	static void					initDXDevice( IDirect3DDevice8 * _DXDevice);
	static void					releaseDXDevice( void );
} ;


class DXMeshInterface
{
	DXStaticMesh &				m_Mesh;

	unsigned int				m_numDefaultMaterials;
	DXMaterialBase **			p_DefaultMaterials;

public:
	DXMeshInterface(	
		DXStaticMesh &			_Mesh
		) :
	m_Mesh( _Mesh )
	{
		p_DefaultMaterials = 0;


#ifdef MASSIVE_AD
		m_MassiveIE = NULL;
#endif

	}
	
	~DXMeshInterface( void );

	const ShaderRecord *		getShaderRecords( unsigned int & count ) const;
	const DXStreamDescriptor * getStreamDescriptor( unsigned int streamIndex ) const;

	void							setMatrixArray( D3DXMATRIX * pMatrices, unsigned int numMatrices );

	void							setDefaultMaterials( DXMaterialBase ** Materials, unsigned int numMaterials );
	DXMaterialBase **			getDefaultMaterials( unsigned int & numMaterials );

	void							renderDefault(  );


#ifdef MASSIVE_AD
	MassiveIE					*m_MassiveIE;
#endif 

	
} ;

#endif