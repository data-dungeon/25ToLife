#ifndef __MEM_MANAGER_H
#define __MEM_MANAGER_H


#include "ManagedBlock.h"

class MemManager
{


public:
	MemManager( ) { }
	virtual void *		alloc( unsigned int nBytes) = 0;
	virtual void *		_realloc( void *pMemory, unsigned int nNewBytes) = 0;
	virtual void 		_free( void *pMemory) = 0;
	
	ManagedBlock *		allocBlock( unsigned int nBytes) { return new ManagedBlock( *this, nBytes); }
} ;


#endif

