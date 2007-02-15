#ifndef __LOCAL_MEM_MANAGER_H
#define __LOCAL_MEM_MANAGER_H

#include <stdlib.h>

#include "Packetizer/MemManager.h"

class LocalMemManager : public MemManager
{

public:
	LocalMemManager( ) { }
	virtual void *		alloc( unsigned int nBytes) { return malloc( nBytes); }
	virtual void *		_realloc( void *pMemory, unsigned int nNewBytes) { return realloc( pMemory, nNewBytes); }
	virtual void 		_free( void *pMemory) { free( pMemory); }
	
	ManagedBlock *		allocBlock( unsigned int nBytes) { return new ManagedBlock( *this, nBytes); }

} ;

#endif