#include "ExportPch.h"
#include <string.h>

#include "MemManager.h"
#include "ManagedBlock.h"

ManagedBlock::ManagedBlock( MemManager &defManager, unsigned int nBytes)
:
	d_Manager( defManager),
	MemBlock( defManager.alloc( nBytes), nBytes)
{

}

ManagedBlock::ManagedBlock( const ManagedBlock &defBlock)
:
	d_Manager( defBlock.d_Manager),
	MemBlock( d_Manager.alloc( defBlock.d_Size), d_Size)
{
	memcpy( p_Buffer, defBlock.p_Buffer, defBlock.d_Size);
}


ManagedBlock & ManagedBlock::operator=( const ManagedBlock &defBlock)
{
	free( );

	d_Size = defBlock.d_Size;
	p_Buffer = d_Manager.alloc( d_Size);

	memcpy( p_Buffer, defBlock.p_Buffer, d_Size);

	return *this;
}
	
void * ManagedBlock::realloc( unsigned int nBytes)
{
	d_Size = nBytes;
	p_Buffer = d_Manager._realloc( p_Buffer, nBytes);

	return p_Buffer;

}

void ManagedBlock::free( void)
{

	if( p_Buffer)
	{
		d_Manager._free( p_Buffer);
		p_Buffer = 0;
		d_Size = 0;
	}

}

