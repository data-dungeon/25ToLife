#ifndef __VIF_DMA_BLOCK_H
#define __VIF_DMA_BLOCK_H

#include "DMABlock.h"

class VIFDMABlock : public DMABlock
{
	bool						b_Flushed;

	int						d_CurrentBase, d_CurrentOffset;
public:
	VIFDMABlock( 
		void * 					_BasePtr,
		unsigned int			_size
		) :
	DMABlock( _BasePtr, _size)
	{
		b_Flushed = true;

		d_CurrentBase = d_CurrentOffset = -1;
	}
	
	void 						start( void )
	{
		reset( );
	
		b_Flushed = true;
	
		d_CurrentBase = d_CurrentOffset = -1;
	}

	
	inline bool				flushed( void ) { return b_Flushed; }
	inline void				setFlushed( bool _Flushed ) { b_Flushed = _Flushed; }
	
	inline bool				compareBaseOffset( unsigned int base, unsigned int offset)
	{
		if( (base != d_CurrentBase)  ||  (offset != d_CurrentOffset) )
			return false;
			
		return true;
	}
	
	inline void				setBaseOffset( unsigned int base, unsigned int offset)
	{
		d_CurrentBase = base;
		d_CurrentOffset = offset;
	}
	
} ;

#endif