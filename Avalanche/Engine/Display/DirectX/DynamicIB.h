#ifndef __DYNAMIC_IB_H_
#define __DYNAMIC_IB_H_

#include <Assert.h>
#include <d3d8.h>

#include "DXBase.h"

class DynamicIB : public DXDeviceObjectClass
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

	IDirect3DDevice8 *					p_DXDevice;
	IDirect3DIndexBuffer8 *				p_IB;

	D3DFORMAT								m_MyFormat;
	unsigned int							m_NumIndices;
	unsigned int							m_CurrentBaseIndex;

	bool										b_Locked;
	bool										b_FlushNextUse;
	bool										b_AutoFlush; 

public:
	D3DINDEXBUFFER_DESC					m_Desc;

	DynamicIB( IDirect3DDevice8 * pDXDevice, unsigned int _numIndices, D3DFORMAT fmt, bool b_AutoFlush = false );
	~DynamicIB( );

	operator IDirect3DIndexBuffer8 * ( void ) const			{ return p_IB; }

	void							releaseDeviceObjects( void );
	void							restoreDeviceObjects( void );

	void							setFlush( void )					{ b_FlushNextUse = true; }
	void							setAutoFlush( bool state )		{ b_AutoFlush = state; }

	unsigned int				indexSize( void ) const			{ return (m_Desc.Format == D3DFMT_INDEX16) ? 2 : 4; }

	unsigned int				lockableIndices( void ) const	{ return (b_FlushNextUse == true) ? m_NumIndices : (m_NumIndices - m_CurrentBaseIndex); }

	void *						lock( unsigned int &lockCount, unsigned int &baseIndex );
	void							unlock( unsigned int unusedIndices = 0 );
} ;


#endif
