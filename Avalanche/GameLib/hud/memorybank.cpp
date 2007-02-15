#include "hud/HudPCH.h"
#include "hud/memorybank.h"

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

bool MemoryBlock::Create( void *pBuffer, int BufferSize )
{
	Destroy();

	m_pBuffer = new char[BufferSize];

	if( !m_pBuffer )
		return false;

	m_BufferSize = BufferSize;

	if( pBuffer )
		memcpy( m_pBuffer, pBuffer, BufferSize );

	return true;
}

bool MemoryBlock::Copy( void *pBuffer, int BufferSize, int Offset )
{
	if( !m_pBuffer )
		return false;

	if( m_BufferSize - Offset < BufferSize )
		return false;

	char *p = &((char *) m_pBuffer)[Offset];

	memcpy( p, pBuffer, BufferSize );

	return true;
}

MemoryBank::MemoryBank( void )
{
	m_ReadPosition = 0;
}

MemoryBank::~MemoryBank( void )
{
	Destroy();
}

void MemoryBank::BeginRead( void )
{
	m_ReadPosition = 0;

	Combine();
}

bool MemoryBank::Read( void *pBuffer, int BufferSize )
{
	if( pBuffer == 0 || BufferSize < 1 )
		return false;

	if( !Combine() )
		return false;

	MemoryBlock *p = (MemoryBlock *) m_MemoryBlocks.Child();

	if( p->m_BufferSize - m_ReadPosition < BufferSize )
		return false;

	char *b = (char *) p->m_pBuffer;

	memcpy( pBuffer, &b[m_ReadPosition], BufferSize );

	m_ReadPosition = m_ReadPosition + BufferSize;

	return true;
}

bool MemoryBank::ReadString( void *pBuffer, int BufferSize )
{
	char *p = (char *) GetReadBuffer();

	if( !p )
		return false;

	int Length = strlen(p)+1;

	if( Length > BufferSize )
		return false;

	return Read( pBuffer, Length );
}

bool MemoryBank::ReadString( xstring &str )
{
	str.set( GetString() );

	if( !str.get() )
		return false;

	return true;
}

char * MemoryBank::GetString( void )
{
	char *p = (char *) GetReadBuffer();

	if( p )
		m_ReadPosition += strlen(p)+1;

	return p;
}

void * MemoryBank::GetReadBuffer( void )
{
	void *pBuffer = 0;

	if( Combine() )
	{
		MemoryBlock *p = (MemoryBlock *) m_MemoryBlocks.Child();

		if( p )
		{
			char *b = (char *) p->m_pBuffer;

			if( b ) 
				pBuffer = &b[m_ReadPosition];
		}
	}

	return pBuffer;
}

void MemoryBank::BeginWrite( void )
{
	Destroy();
}

bool MemoryBank::Write( void *pBuffer, int BufferSize )
{
	MemoryBlock *p = new MemoryBlock;

	if( !p )
		return false;

	if( !p->Create( pBuffer, BufferSize ) )
	{
		delete p;
		return false;
	}

	m_MemoryBlocks.AddChild( p );

	return true;
}

bool MemoryBank::WriteString( void *pBuffer )
{
	char *s = (char *) pBuffer;

	if( !s )
		s = "";

	return Write( s, strlen(s) + 1);
}

bool MemoryBank::Combine( void )
{
	if( !m_MemoryBlocks.Child() )
		return false;

	if( !m_MemoryBlocks.Child()->Next() )
		return true;

	int BufferSize = Size();

	if( BufferSize > 0 )
	{
		MemoryBlock *p = new MemoryBlock;

		if( !p )
			return false;

		if( !p->Create( 0, BufferSize ) )
		{
			delete p;
			return false;
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

	return true;
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

bool MemoryBank::EndOfFile( void )
{
	if( m_ReadPosition == Size() )
		return true;

	return false;
}