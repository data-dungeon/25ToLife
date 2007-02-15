#ifndef __XB_RESOURCE_MANAGER_H
#define __XB_RESOURCE_MANAGER_H

#include "CArray.h"
//#include "platform/Xbox/dxdmanager.h"

class CDXD_Data;
class CXBPackedResource;

class XBResourceManager
{
	CArray< CXBPackedResource * > m_ResourceList;
public:
	XBResourceManager( );
	~XBResourceManager( );
	
	CXBPackedResource *	addResourceFile( const char * pResourceFilename );
	CXBPackedResource*	addResourceFileDXD( CDXD_Data* Dx );
	CXBPackedResource*	getResourceFile( const char * pResourceFilename );
	
	void						destroyResourceFile( const char * pResourceFilename );
	void						destroyResourceFileDXD( CDXD_Data* Dx );

	void						destroyResourceFiles( void );
} ;

#endif
