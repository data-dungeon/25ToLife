#ifndef __DMA_UTILITIES_H
#define __DMA_UTILITIES_H

namespace DMAUtilities
{

#define dmaAddr(adr)	((((adr) & 0x70000000) == 0x70000000) ? (((adr) & 0x0fffffff) | 0x80000000) : (adr))

enum _dmaAlignAddr
{
	DMA_ALIGN_2 = 0,
	DMA_ALIGN_4,
	DMA_ALIGN_8,
	DMA_ALIGN_16,
	DMA_ALIGN_32,
	DMA_ALIGN_64,
	DMA_ALIGN_128
} ;

unsigned int 	dmaCalcBufferBytes( unsigned int uiBytes, bool bDMAStartTag, bool bDMATTE, unsigned int &QWC);
void *			dmaAlignAddr( void *pAddr, unsigned int Bound);



}

#endif
