#ifndef __MANAGED_BLOCK_H
#define __MANAGED_BLOCK_H

#include "MemBlock.h"

class MemManager;

class ManagedBlock : public MemBlock
{

	MemManager &			d_Manager;

public:
	ManagedBlock( MemManager &defManager, unsigned int defSize);
	ManagedBlock( const ManagedBlock &defObj);

	ManagedBlock & operator=( const ManagedBlock &defObj);

	~ManagedBlock( ) { free( ); }

	void *					realloc( unsigned int nBytes);
	void						free( void);
		
} ;

#endif

	