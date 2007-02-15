#ifndef __DMA_BLOCK_H
#define __DMA_BLOCK_H

#include "renderassert.h"

#include "VIFInline.h"
#include "DMAInline.h"

using namespace				VIFInline;
using namespace				DMAInline;

class DMABlock
{
	bool							b_TTEEnabled;

	unsigned int *				p_Base;
	unsigned int *				p_End;

	unsigned int *				p_Write;

	unsigned long *			p_DMATag;

	unsigned int				d_Size;

public:
	DMABlock(
		void *					_BasePtr,
		unsigned int			_Size
		) ;

	void						reset( void );
	void						setTTE( bool _TTEEnabled) 	{ b_TTEEnabled = _TTEEnabled; }

	inline bool				isEmpty( void ) 		{ return p_Write == p_Base; }

	inline void *	  		getBasePtr( void )  	{ return p_Base; }
	inline void *	  		getEndPtr( void )	  	{ return p_End; }	

	inline void * 			getWritePtr( void ) 	{ return p_Write; }

	inline void *			getTagAlignedWritePtr( void )
	{
		p_Write = vifiQWordAlignNOP( p_Write);

		return p_Write;
	}


	inline void				setWritePtr( void * _Write)
	{
		RENDER_ASSERT( dmaiIsWordAlignedM( _Write));
		RENDER_ASSERT( (_Write >= p_Base)  &&  ( _Write < p_End));

		p_Write = (unsigned int *) _Write;
	}


	void *					startDMAPacket( void);
	void *					startDMAPacket( unsigned long _DMATag);
	
	void						endDMAPacket( void );
	void						endDMAPacket( unsigned long _DMATag);

	void						endDMAPacket( void *_Write);
	void						endDMAPacket( void *_Write, unsigned long _DMATag);

	void						closeDMA( void );
} ;




#endif