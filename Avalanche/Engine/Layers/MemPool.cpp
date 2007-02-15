//============================================================================
//=
//= MemPool.cpp - Pool class for managing lots of small pieces of memory
//=
//============================================================================

#include "Layers/LayersPCH.h"
#include "Layers/MemPool.h"
#include "EngineHelper/filestream.h"

//============================================================================

bool CMemPool::Valid() const
{
	return d_count > 0;
}

//============================================================================

bool CMemPool::Empty()
{
	return d_count == d_free;
}

//============================================================================

int CMemPool::Size() const
{
	int size = d_count * d_size;	// blocks
	size += (d_count + 7) / 8;		// tracking bits
	size = ( size + 15 ) & ~15;	// round to 16-bytes
	return size;
}

//============================================================================

int CMemPool::BlockSize() const
{
	return d_size;
}

//============================================================================

void CMemPool::Init( void* buffer )
{
	d_buffer = (char*) buffer;
	memset( d_buffer, 0xee, Size() );
	char* last = NULL;
	for ( int i = 0 ; i < d_count ; i++ )
	{
		*(char**)( d_buffer + i * d_size ) = last;
		last = d_buffer + i * d_size;
	}
	d_freeHead = last;

	d_allocated = d_buffer + d_count * d_size;
	memset( d_allocated, 0, (d_count + 7) / 8 );

	d_free = d_count;
}

//============================================================================

bool CMemPool::Owns( void* block ) const
{
	return ( block >= d_buffer ) && ( block < d_buffer + d_count * d_size );
}

//============================================================================

void* CMemPool::Alloc( int size )
{
	if ( size <= d_size && d_freeHead )
	{
		if ( MarkAllocated( d_freeHead ) )
		{
			char* block = d_freeHead;
			d_freeHead = *(char**) block;
			d_free--;
			return block;
		}
	}
	return NULL;
}

//============================================================================

void CMemPool::Free( void* block )
{
	if ( ClearAllocated( block ) )
	{
		*(char**)block = d_freeHead;
		d_freeHead = (char*)block;
		d_free++;
	}
}

//============================================================================

bool CMemPool::MarkAllocated( void* block )
{
	int index = ( (char*) block - d_buffer ) / d_size;
	if ( index >= 0 && index < d_count )
	{
		ASSERT( d_buffer + index * d_size == block );
		int bits = d_allocated[ index >> 3 ];
		int b = 1 << ( index & 7 );
		if ( ( bits & b ) == 0 )
		{
			d_allocated[ index >> 3 ] = bits | b;
			return true;
		}
		else
			ASSERTS( false, "Trying to double alloc a block" );
	}
	return false;
}

//============================================================================

bool CMemPool::ClearAllocated( void* block )
{
	int index = ( (char*) block - d_buffer ) / d_size;
	if ( index >= 0 && index < d_count )
	{
		ASSERT( d_buffer + index * d_size == block );
		int bits = d_allocated[ index >> 3 ];
		int b = 1 << ( index & 7 );
		if ( bits & b )
		{
			d_allocated[ index >> 3 ] = bits & ~b;
			return true;
		}
		else
			ASSERTS( false, "Trying to double free a block" );
	}
	return false;
}

//============================================================================

void CMemPool::Dump( FileStream* fp )
{
	fp->Print( "POOL: size=%3d blocks=%4d free=%4d (%d%%)\n",
				  d_size, d_count, d_free, d_free * 100 / d_count );
}

//============================================================================
//============================================================================
//============================================================================

CMemPoolSet::CMemPoolSet()
{
	d_pools    = NULL;
	d_buffer   = NULL;
	d_size     = 0;
	d_maxBlock = 0;
}

//============================================================================

void CMemPoolSet::Init( CMemPool* pools, void* buffer )
{
	ASSERT_PTR( pools );
	ASSERT_PTR( buffer );

	d_pools    = pools;
	d_buffer   = (char*)buffer;
	d_size     = 0;
	d_maxBlock = 0;

	for ( CMemPool* p = d_pools ; p->Valid() ; p++ )
	{
		p->Init( d_buffer + d_size );
		d_size += p->Size();
		ASSERT( p->BlockSize() >= d_maxBlock );
		d_maxBlock = p->BlockSize();
	}
}

//============================================================================

bool CMemPoolSet::Empty() const
{
	for ( CMemPool* p = d_pools ; p->Valid() ; p++ )
		if ( !p->Empty() )
			return false;
	return true;
}

//============================================================================

int CMemPoolSet::Size( CMemPool* pools )
{
	int size = 0;
	for ( CMemPool* p = pools ; p->Valid() ; p++ )
		size += p->Size();
	return size;
}

//============================================================================

void* CMemPoolSet::Alloc( int size )
{
	void* block = NULL;
	if ( size <= d_maxBlock )
	{
		for ( CMemPool* p = d_pools ; p->Valid() ; p++ )
			if ( ( block = p->Alloc( size ) ) != NULL )
				break;
	}
	return block;
}

//============================================================================

void CMemPoolSet::Free( void* block )
{
	if ( Owns( block ) )
	{
		for ( CMemPool* p = d_pools ; p->Valid() ; p++ )
		{
			if ( p->Owns( block ) )
			{
				p->Free( block );
				return;
			}
		}
	}
}

//============================================================================

void CMemPoolSet::Dump( FileStream* fp )
{
	for ( CMemPool* p = d_pools ; p->Valid() ; p++ )
		p->Dump( fp );
}

//============================================================================

bool CMemPoolSet::Owns( void* block ) const
{
	return ( block >= d_buffer ) && ( block < d_buffer + d_size );
}

//============================================================================
