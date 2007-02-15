#include "ExportPch.h"
#include "DMAUtilities.h"

unsigned int DMAUtilities::dmaCalcBufferBytes( unsigned int dmaBytes, bool dmaStartTag, bool TTE, unsigned int &QWC)
{
unsigned int		dmaPacketSize; // size of actual packet


	dmaPacketSize = dmaBytes;

	// 8 Bytes in DMATag QWord
	if( dmaStartTag  &&  TTE)
		dmaPacketSize -= 8;
		
	// round up to QWords		
	while( dmaPacketSize & 0x0f)
		dmaPacketSize++;
	
	QWC = dmaPacketSize / 16;
		
	return dmaStartTag ? (dmaPacketSize + 16) : dmaPacketSize;

}
		
void *DMAUtilities::dmaAlignAddr( void *pAddr, unsigned int Bound)
{
unsigned int		dmaMaskArray[] = 
{
	0x01,	0x03,	0x07,	0x0f,	0x1f,	0x3f,	0x7f,	0xff
} ;

unsigned int		dmaMask;
unsigned int		dmaAddr;

	dmaMask = dmaMaskArray[Bound];
	
	dmaAddr = (unsigned int) pAddr;
	
	while( dmaAddr & dmaMask)
		dmaAddr++;
		
	return (void *) dmaAddr;
}
			
