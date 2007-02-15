#include "pch.h"
#include "memorybank.h"

MemoryBlock::MemoryBlock( void )
{
	m_pBuffer = 0;
	m_BufferSize = 0;
}

MemoryBlock::~MemoryBlock( void )
{
	Destroy();
}

void MemoryBlock::Destroy( void )
{
	if( m_pBuffer )
	{
		delete [] m_pBuffer;
		m_pBuffer = 0;
		m_BufferSize = 0;
	}

	return;
}

BOOL MemoryBlock::Create( void *pBuffer, int BufferSize )
{
	Destroy();

	m_pBuffer = new BYTE[BufferSize];

	if( !m_pBuffer )
		return FALSE;

	m_BufferSize = BufferSize;

	if( pBuffer )
		memcpy( m_pBuffer, pBuffer, BufferSize );

	return TRUE;
}

BOOL MemoryBlock::Copy( void *pBuffer, int BufferSize, int Offset )
{
	if( !m_pBuffer )
		return FALSE;

	if( m_BufferSize - Offset < BufferSize )
		return FALSE;

	BYTE *p = &((BYTE *) m_pBuffer)[Offset];

	memcpy( p, pBuffer, BufferSize );

	return TRUE;
}

MemoryBank::MemoryBank( void )
{
}

MemoryBank::~MemoryBank( void )
{
	Destroy();
}

BOOL MemoryBank::AddData( void *pBuffer, int BufferSize )
{
	MemoryBlock *p = new MemoryBlock;

	if( !p )
		return FALSE;

	if( !p->Create( pBuffer, BufferSize ) )
	{
		delete p;
		return FALSE;
	}

	m_MemoryBlocks.AddChild( p );

	return TRUE;
}

BOOL MemoryBank::Combine( void )
{
	int BufferSize = Size();

	if( BufferSize > 0 )
	{
		MemoryBlock *p = new MemoryBlock;

		if( !p )
			return FALSE;

		if( !p->Create( 0, BufferSize ) )
		{
			delete p;
			return FALSE;
		}

		MemoryBlock *pMemoryBlock = (MemoryBlock *) m_MemoryBlocks.Child();
		void *pBuffer = 0;
		int BufferSize = 0;
		int Offset = 0;

		while( pMemoryBlock )
		{
			pBuffer = pMemoryBlock->m_pBuffer;
			BufferSize = pMemoryBlock->m_BufferSize;
			p->Copy( pBuffer, BufferSize, Offset );
			Offset = Offset + BufferSize;
			pMemoryBlock = (MemoryBlock *) pMemoryBlock->Next();
		}

		Destroy();

		m_MemoryBlocks.AddChild( p );
	}

	return TRUE;
}

void MemoryBank::Destroy( void )
{
	while( m_MemoryBlocks.Child() )
		delete m_MemoryBlocks.Child();

	return;
}

int MemoryBank::Size( void )
{
	MemoryBlock *p = (MemoryBlock *) m_MemoryBlocks.Child();
	int c = 0;

	while( p )
	{
		c = c + p->m_BufferSize;
		p = (MemoryBlock *) p->Next();
	}

	return c;
}