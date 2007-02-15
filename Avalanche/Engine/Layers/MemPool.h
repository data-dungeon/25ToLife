//============================================================================
//=
//= MemPool.h - Pool class for managing lots of small pieces of memory
//=
//============================================================================

#ifndef _MEMPOOL_H
#define _MEMPOOL_H

class FileStream;

//============================================================================

struct CMemPool
{
	void	Init( void* buffer );
	bool	Empty();
	int	Size() const;
	void*	Alloc( int size );
	void	Free( void* block );
	bool	Owns( void* block ) const;
	int	BlockSize() const;
	bool	Valid() const;
	bool	MarkAllocated( void* block );
	bool	ClearAllocated( void* block );
	void	Dump( FileStream* fp );

	int	d_size;
	int	d_count;
	int	d_free;
	char*	d_buffer;
	char*	d_allocated;
	char*	d_freeHead;
};

#define POOLDEF( size, count ) { (size), (count), 0, NULL, NULL, NULL }

//============================================================================

struct CMemPoolSet
{
public:
	CMemPoolSet();
	void	Init( CMemPool* pools, void* buffer );
	bool	Empty() const;
	void*	Alloc( int size );
	void	Free( void* block );
	bool	Owns( void* block ) const;
	static int Size( CMemPool* pools );
	void	Dump( FileStream* fp );

private:
	CMemPool*	d_pools;
	char*			d_buffer;
	int			d_size;
	int			d_maxBlock;
};

//============================================================================

#endif //_MEMPOOL_H
