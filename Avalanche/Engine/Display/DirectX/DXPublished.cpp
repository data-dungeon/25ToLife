#include "Display/DisplayPCH.h"


#include "DXBase.h"
#include "DXPublished.h"
#if defined(_XBOX)
#include "DXMeshBase_Xbox.h"
#else
#include "DXMeshBase.h"
#endif
#include "DXMeshInterface.h"
#include "DXStaticMesh.h"
#include "DXRenderShaderBase.h"
#include "DXSharedDCache.h"

void DXPublished::initDXDevice( IDirect3DDevice8 * _DXDevice )
{
	DXBase.initDXDevice( _DXDevice );
	DXShaderDCache.initDXDevice( _DXDevice );
	DXMeshBase::initDXDevice( _DXDevice );
	DXMaterialBase::initDXDevice( _DXDevice );
	DXRenderShaderBase::initDXDevice( _DXDevice );
}

void DXPublished::releaseDXDevice( void )
{
	DXBase.releaseDXDevice( );
	DXShaderDCache.releaseDXDevice( );
	DXMeshBase::releaseDXDevice( );
	DXMaterialBase::releaseDXDevice( );
	DXRenderShaderBase::releaseDXDevice( );
}

void DXPublished::addLostDeviceCallback( DXDeviceObjectClass * pObject )
{
	ASSERT( pObject );

	DXBase.addDOCallback( pObject );
}

void DXPublished::removeLostDeviceCallback( DXDeviceObjectClass * pObject )
{
	ASSERT( pObject );

	DXBase.removeDOCallback( pObject );
}

void DXPublished::releaseDeviceObjects( void )
{
	DXBase.releaseDeviceObjects( );
}

void DXPublished::restoreDeviceObjects( void )
{
	DXBase.restoreDeviceObjects( );
}

DXMeshInterface * DXPublished::createMeshInterface( void * pMeshData )
{
	ASSERT( pMeshData );

	MeshRecord * pMeshRec = (MeshRecord *) pMeshData;
	ASSERT( pMeshRec->d_MeshFlags == 0xdeadbeef );

	DXStaticMesh * pNewMesh = new DXStaticMesh( * pMeshRec );

	pNewMesh->setMatrixArrayVertexRegisterBase( 32 );

	return new DXMeshInterface( *pNewMesh );
}

