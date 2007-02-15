#include "renderassert.h"

#include "DMABlock.h"

#include "VIFInline.h"
#include "DMAInline.h"

using namespace 					VIFInline;
using namespace					DMAInline;

DMABlock::DMABlock( void * _BasePtr, unsigned int _Size) :
	b_TTEEnabled( false),
	p_DMATag( 0)
{

	p_Base = dmaiNextQWordAlignedAddr( _BasePtr);
	p_End = (unsigned int *) (( (unsigned int) _BasePtr) + _Size);

	d_Size = ((unsigned int) p_End) - ((unsigned int) p_Base);
	
	reset( );

}



void DMABlock::reset( void )
{
	p_Write = p_Base;
	p_DMATag = 0;
}

void DMABlock::closeDMA( void )
{
	RENDER_ASSERT( p_DMATag == 0);

	if( p_Write != p_Base)
	{
		p_Write = vifiQWordAlignNOP( p_Write);

		*((unsigned long *) p_Write) = createDMATag( 0, DTAG_ID_END, DTAG_PCE_NOTHING, 0);
		p_Write+= 2;
		*p_Write++ = createFLUSHA( );
		*p_Write++ = createNOP( );
		
		asm( "sync.l" );
		
	}

}

		
void * DMABlock::startDMAPacket( unsigned long _DMATag)
{
	RENDER_ASSERT( p_DMATag == 0);

	p_DMATag = (unsigned long *) vifiQWordAlignNOP( p_Write);

	*p_DMATag = _DMATag;

	p_Write = (unsigned int *) (b_TTEEnabled ? (p_DMATag + 1) : (p_DMATag + 2));

	return p_Write;
}

void * DMABlock::startDMAPacket( void )
{
	RENDER_ASSERT( p_DMATag == 0);

	p_DMATag = (unsigned long *) vifiQWordAlignNOP( p_Write);

	p_Write = (unsigned int *) (b_TTEEnabled ? (p_DMATag + 1) : (p_DMATag + 2));

	return p_Write;
}

void DMABlock::endDMAPacket( void )
{
	RENDER_ASSERT( p_DMATag);

	p_Write = vifiQWordAlignNOP( p_Write);

	*p_DMATag = *p_DMATag | dmaiCalculateQWCM( p_Write, p_DMATag + 2);
		
	p_DMATag = 0;
}
	
void DMABlock::endDMAPacket( unsigned long _DMATag )
{
	RENDER_ASSERT( p_DMATag);

	p_Write = vifiQWordAlignNOP( p_Write);

	*p_DMATag = _DMATag | dmaiCalculateQWCM( p_Write, p_DMATag + 2);
		
	p_DMATag = 0;
}
	
void DMABlock::endDMAPacket( void * _Write)
{
	RENDER_ASSERT( p_DMATag);
		
	p_Write = vifiQWordAlignNOP( (unsigned int *) _Write);
		
	*p_DMATag = *p_DMATag | dmaiCalculateQWCM( p_Write, p_DMATag + 2);
		
	p_DMATag = 0;
} 
		
	
void DMABlock::endDMAPacket( void * _Write, unsigned long _DMATag)
{
	RENDER_ASSERT( p_DMATag);
		
	p_Write = vifiQWordAlignNOP( (unsigned int *) _Write);
		
	*p_DMATag = _DMATag | dmaiCalculateQWCM( p_Write, p_DMATag + 2);
		
	p_DMATag = 0;
} 
