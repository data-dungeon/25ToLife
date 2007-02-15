#include "XBResourceManager.h"
#include "xtl.h"
#include "platform/Xbox/dxdmanager.h"

#define RESOURCE_ARRAY_SIZE	192

XBResourceManager::XBResourceManager( void ) 
:
	m_ResourceList( RESOURCE_ARRAY_SIZE )
{

}

XBResourceManager::~XBResourceManager( )
{
	destroyResourceFiles( );
}


CXBPackedResource * XBResourceManager::addResourceFileDXD( CDXD_Data* Dx )
{
	if (Dx == NULL)
		return 0;

	CXBPackedResource * pResource = getResourceFile( Dx->GetDBLName() );

	if( pResource )
		return pResource;
	else
	{
		assert( m_ResourceList.count( ) < RESOURCE_ARRAY_SIZE );

		pResource = &Dx->m_xvdResources;
		m_ResourceList.add( pResource );
		return pResource;
	}
}

void XBResourceManager::destroyResourceFileDXD( CDXD_Data* Dx )
{
	if (Dx == NULL)
		return;

	CXBPackedResource * pResource = getResourceFile( Dx->GetDBLName());

	if( pResource )
		m_ResourceList.remove( pResource );
}


void XBResourceManager::destroyResourceFiles( void )
{
	for( unsigned int i = 0; i < m_ResourceList.count( ); i++ )
	{
		delete m_ResourceList[i];
		m_ResourceList.remove( i );
	}
}

CXBPackedResource * XBResourceManager::getResourceFile( const char * pResourceFilename )
{
	for( unsigned int i = 0; i < m_ResourceList.count( ); i++ )
	{
		if( stricmp( pResourceFilename, m_ResourceList[i]->GetResourceFilename( ) ) == 0 )
			return m_ResourceList[i];
	}

	return 0;
}
