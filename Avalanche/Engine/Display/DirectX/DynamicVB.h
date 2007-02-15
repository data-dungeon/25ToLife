#ifndef __DYNAMIC_VB_H_
#define __DYNAMIC_VB_H_

#include <Assert.h>
#include <d3d8.h>

#include "DXBase.h"

class DynamicVB : public DXDeviceObjectClass
{
	enum LOCK_FLAGS
	{
		#if !defined (_XBOX)	
		LOCKFLAGS_FLUSH  = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD,
		LOCKFLAGS_APPEND = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE
		#else
		LOCKFLAGS_FLUSH  = 0,		// PAB TODO
		LOCKFLAGS_APPEND = 0
		#endif
	} ;
	IDirect3DDevice8 *				p_DXDevice;
	IDirect3DVertexBuffer8 *		p_VB;

	D3DVERTEXBUFFER_DESC				m_Desc;

	unsigned int						m_MyFVF;
	unsigned int						m_NumBytes;
	unsigned int						m_BytesUsed;

	bool									b_Locked;
	bool									b_FlushNextUse;
	bool									b_AutoFlush;

	BYTE *								p_LockPtr;
	unsigned int						m_LockBytes;

public:
	DynamicVB( 
		IDirect3DDevice8 *			pDXDevice, 
		unsigned int					_totalBufferBytes,
		unsigned int					_FVF = 0,
		bool								_AutoFlush = false
	) ;

	~DynamicVB( );
	
	operator IDirect3DVertexBuffer8 * (void ) const				{ return p_VB; }

	bool								flushQueued( void ) const		{ return b_FlushNextUse; }

	void								releaseDeviceObjects( void );
	void								restoreDeviceObjects( void );

	unsigned int					totalBytes( void ) const		{ return m_NumBytes; }
	unsigned int					lockableBytes( void ) const	{ return b_FlushNextUse ? m_NumBytes : ( m_NumBytes - m_BytesUsed ); }
	unsigned int					lockableVertices( unsigned int vertexBytes ) const
	{
		unsigned int baseOffset = b_FlushNextUse ? 0 : m_NumBytes;

		unsigned int startVertex = (baseOffset + (vertexBytes - 1)) / vertexBytes;
		unsigned int totalVertices = m_NumBytes / vertexBytes;

		return totalVertices - startVertex;
	}

	unsigned int					currentOffset( void ) const	{ return m_BytesUsed; }

	void								setFlush( void )					{ b_FlushNextUse = true; }
	void								setAutoFlush( bool state )		{ b_AutoFlush = state; }

	void *							lock( unsigned int &numLockBytes, unsigned int &baseOffset );
	void *							lock( unsigned int vertexBytes, unsigned int lockCount, unsigned int &baseVertex );

	void								unlock( unsigned int unusedBytes = 0 );
		
} ;


	
#endif