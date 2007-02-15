#include "Display/DisplayPCH.h"
#include "DynamicVB.h"


DynamicVB::DynamicVB( IDirect3DDevice8 *pDXDevice, unsigned int _totalBufferBytes, unsigned int FVF, bool _AutoFlush )
{
	b_AutoFlush = _AutoFlush;

	p_VB = 0;

	p_DXDevice = pDXDevice;

	HRESULT result = pDXDevice->CreateVertexBuffer( _totalBufferBytes, 
								D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, FVF, D3DPOOL_DEFAULT, &p_VB );

	ASSERT( result == D3D_OK );
	ASSERT( p_VB );

	b_FlushNextUse = true;
	b_Locked = false;

	m_NumBytes = _totalBufferBytes;
	m_MyFVF = FVF;

	m_BytesUsed = 0;

	p_LockPtr = 0;
	m_LockBytes = 0;

	memset( &m_Desc, 0, sizeof( m_Desc ) );

	p_VB->GetDesc( & m_Desc );
}

DynamicVB::~DynamicVB( void )
{
	ASSERT( p_VB );

	unlock( );

	ULONG useCount = p_VB->Release( );
	ASSERT( useCount == 0 );
}

void DynamicVB::releaseDeviceObjects( void )
{
	if( p_VB )
	{
		unlock( );

		ULONG useCount = p_VB->Release( );
		ASSERT( useCount == 0 );

		p_VB = 0;
	}
}	

void DynamicVB::restoreDeviceObjects( void )
{
	ASSERT( p_DXDevice );

	if( p_VB == 0 )
	{
		HRESULT result = p_DXDevice->CreateVertexBuffer( m_NumBytes,	D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
									m_MyFVF, D3DPOOL_DEFAULT, &p_VB );

		ASSERT( result == D3D_OK );
		ASSERT( p_VB );

		b_Locked = false;
		b_FlushNextUse = true;
		m_BytesUsed = 0;
		p_LockPtr = 0;
		m_LockBytes = 0;

		memset( &m_Desc, 0, sizeof( m_Desc ) );

		p_VB->GetDesc( & m_Desc );
	}
}

void * DynamicVB::lock( unsigned int & lockBytes, unsigned int &baseOffset )
{
	ASSERT( p_VB );
	ASSERT( lockBytes < m_NumBytes );

	DWORD					dwFlags = LOCKFLAGS_APPEND;

	p_LockPtr = 0;
	lockBytes = lockBytes ? lockBytes : (m_NumBytes - m_BytesUsed);

	// If either Dynamic forced us to flush,
	//  or there is not enough space for the vertex data,
	//  then flush the buffer contents

	if( b_FlushNextUse  ||  (lockBytes + m_BytesUsed) > m_NumBytes )
	{
		if( (b_FlushNextUse == false)  &&  (b_AutoFlush == false) )
		{
			lockBytes = 0;
			return 0;
		}

		m_BytesUsed = 0;
				
		b_FlushNextUse = false;

		dwFlags = LOCKFLAGS_FLUSH;
	}

	HRESULT result = p_VB->Lock( m_BytesUsed,	lockBytes, & p_LockPtr, dwFlags );

	ASSERT( result == D3D_OK );
	ASSERT( p_LockPtr );

	b_Locked = true;

	baseOffset = m_BytesUsed;
	m_BytesUsed += lockBytes;

	return p_LockPtr;
}

void * DynamicVB::lock( unsigned int vertexBytes, unsigned int lockCount, unsigned int &baseVertex )
{
	ASSERT( p_VB );
	ASSERT( (vertexBytes * lockCount) < m_NumBytes );

	DWORD					dwFlags = LOCKFLAGS_APPEND;

	baseVertex = (m_BytesUsed + (vertexBytes - 1)) / vertexBytes;
	
	unsigned int totalVertices = (m_NumBytes / vertexBytes);
	unsigned int fitVertices = (totalVertices > baseVertex) ? (totalVertices - baseVertex) : 0;

	p_LockPtr = 0;
	m_LockBytes = lockCount * vertexBytes;

	// If either Dynamic forced us to flush,
	//  or there is not enough space for the vertex data,
	//  then flush the buffer contents

	if( b_FlushNextUse  ||  (lockCount > fitVertices) )
	{
		baseVertex = 0;

		b_FlushNextUse = false;

		dwFlags = LOCKFLAGS_FLUSH;
	}

	HRESULT result = p_VB->Lock( baseVertex * vertexBytes, m_LockBytes, & p_LockPtr, dwFlags );

	ASSERT( result == D3D_OK );
	ASSERT( p_LockPtr );

	b_Locked = true;

	m_BytesUsed = (baseVertex + lockCount) * vertexBytes;

	return p_LockPtr;
}

void DynamicVB::unlock( unsigned int unusedBytes )
{
	ASSERT( p_VB );

	if( b_Locked )
	{
		HRESULT result = p_VB->Unlock( );
		ASSERT( result == D3D_OK );

		ASSERT( p_LockPtr );
		ASSERT( m_LockBytes );

		ASSERT( unusedBytes <= m_BytesUsed );

		m_BytesUsed -= unusedBytes;

		p_LockPtr = 0;
		m_LockBytes = 0;

		b_Locked = false;
	}
}
