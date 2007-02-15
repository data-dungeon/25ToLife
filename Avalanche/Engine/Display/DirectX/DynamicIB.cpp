#include "Display/DisplayPCH.h"
#include <Assert.h>

#include "DynamicIB.h"


DynamicIB::DynamicIB( IDirect3DDevice8 * pDXDevice, unsigned int _numIndices, D3DFORMAT fmt, bool _AutoFlush )
{
	ASSERT( pDXDevice );	
	#if !defined (_XBOX)			// XBOX doesnt support INDEX32
	ASSERT( (fmt == D3DFMT_INDEX16)  ||  (fmt == D3DFMT_INDEX32) );
	#else
	ASSERT(fmt == D3DFMT_INDEX16);
	#endif

	b_AutoFlush = _AutoFlush;

	p_IB = 0;

	unsigned int szeIndex = (fmt == D3DFMT_INDEX16) ? 2 : 4;

	p_DXDevice = pDXDevice;

	HRESULT result = pDXDevice->CreateIndexBuffer( _numIndices * szeIndex, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
							fmt, D3DPOOL_DEFAULT, &p_IB);

	ASSERT( result == D3D_OK );
	ASSERT( p_IB );

	m_CurrentBaseIndex = 0;

	b_FlushNextUse = true;
	b_Locked = false;

	m_NumIndices = _numIndices;
	m_MyFormat = fmt;
	
	memset( &m_Desc, 0, sizeof( m_Desc ) );
	
	result = p_IB->GetDesc( &m_Desc );

	ASSERT( result == D3D_OK );
}

DynamicIB::~DynamicIB( )
{
	ASSERT( p_IB );

	unlock( );

	ULONG useCount = p_IB->Release( );
	ASSERT( useCount == 0 );
}

void DynamicIB::releaseDeviceObjects( void )
{
	if( p_IB )
	{
		unlock( );

		ULONG useCount = p_IB->Release( );
		ASSERT( useCount == 0 );

		p_IB = 0;
	}
}

void DynamicIB::restoreDeviceObjects( void )
{
	ASSERT( p_DXDevice );

	if( p_IB == 0 )
	{
		unsigned int szeIndex = (m_MyFormat == D3DFMT_INDEX16) ? 2 : 4;

		HRESULT result = p_DXDevice->CreateIndexBuffer( m_NumIndices * szeIndex, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
								m_MyFormat, D3DPOOL_DEFAULT, &p_IB);

		ASSERT( result == D3D_OK );
		ASSERT( p_IB );

		m_CurrentBaseIndex = 0;

		b_FlushNextUse = true;
		b_Locked = false;

	
		memset( &m_Desc, 0, sizeof( m_Desc ) );
	
		result = p_IB->GetDesc( &m_Desc );
	
		ASSERT( result == D3D_OK );
	}
}


void * DynamicIB::lock( unsigned int &lockCount, unsigned int &baseIndex )
{
	ASSERT( lockCount < m_NumIndices );
	ASSERT( p_IB );

	unsigned int			szeIndex = (m_Desc.Format == D3DFMT_INDEX16) ? 2 : 4;
	DWORD						dwFlags = LOCKFLAGS_APPEND;
	BYTE *					pReturnPtr = 0;

	baseIndex = 0;
	
	if( lockCount == 0 )
		lockCount = (b_FlushNextUse == true) ? m_NumIndices : (m_NumIndices - m_CurrentBaseIndex);

	// If either user forced us to flush,
	//  or there is not enough space for the Index data,
	//  then flush the buffer contents
	//
	if ( b_FlushNextUse  || ( (lockCount + m_CurrentBaseIndex) > m_NumIndices) )
	{
		if( (b_FlushNextUse == false)  &&  (b_AutoFlush == false) )
		{
			lockCount = 0;
			return 0;
		}

		b_FlushNextUse = false;
		m_CurrentBaseIndex = 0;
		
		dwFlags = LOCKFLAGS_FLUSH;
	}

	DWORD dwSize = 0;
	
	HRESULT result = p_IB->Lock( m_CurrentBaseIndex * szeIndex, lockCount * szeIndex, 
							& pReturnPtr, dwFlags );


	ASSERT( result == D3D_OK );
	ASSERT( pReturnPtr );

	b_Locked = true;

	baseIndex = m_CurrentBaseIndex;
	m_CurrentBaseIndex += lockCount;

	return pReturnPtr;
}

void DynamicIB::unlock( unsigned int unusedIndices )
{
	ASSERT( p_IB );

	if( b_Locked )
	{
		ASSERT( unusedIndices <= m_CurrentBaseIndex );

		m_CurrentBaseIndex -= unusedIndices;

		HRESULT result = p_IB->Unlock( );
		ASSERT( result == D3D_OK );

		b_Locked = false;
	}
}