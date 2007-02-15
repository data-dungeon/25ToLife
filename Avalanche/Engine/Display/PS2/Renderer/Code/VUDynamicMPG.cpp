#include "PS2Renderer.h"
#include "MasterList.h"
#include "VUDynamicMPG.h"

#include "VIFInline.h"
#include "DMAInline.h"

using namespace			VIFInline;
using namespace			DMAInline;

const unsigned int VUDynamicMPG::bytesPerVUInst = 8;
const unsigned int VUDynamicMPG::maxMPGVUInst = 256;

VUDynamicMPG::VUDynamicMPG( void * _Code, unsigned int _VUInstructions) :
	p_Code( _Code),
	d_VUInstructions( _VUInstructions)
{


}

VUDynamicMPG * VUDynamicMPG::create( void * pCodeStart, void *pCodeEnd)
{
	return create( pCodeStart, instructionsInBlock( pCodeStart, pCodeEnd));
}

VUDynamicMPG * VUDynamicMPG::create( void * pCodeStart, unsigned int vuInstructions)
{
	if( vuInstructions > maxMPGVUInst)
		return new VUDynamicMPGMult( pCodeStart, vuInstructions);
	else
		return new VUDynamicMPGSngl( pCodeStart, vuInstructions);
}


void VUDynamicMPGMult::addToDMA_MPG( unsigned int vuAddr, bool bFlush)
{
//	RENDER_ASSERT( pDMAMem);
//	RENDER_ASSERT( dmaiIsWordAlignedM( pDMAMem) );					  

//	unsigned long *		pDMATag = (unsigned long *) vifiQWordAlignNOP( (unsigned int *) pDMAMem);

	unsigned int			totalVUInst = d_VUInstructions;
	unsigned int			thisVUAddr = vuAddr;
	unsigned long *		thisCode = (unsigned long *) p_Code;
		
	do
	{
		unsigned int 		thisInstCount = (totalVUInst > 256) ? 256 : totalVUInst;
		unsigned int 		thisQWC = (thisInstCount+1) / 2;

#ifndef SHIP_IT
		// if there are an odd number of instructions, we transfer 2 extra vif codes
		// off the end of the instruction stream... we need to make sure they are
		// vif nops..!!
		if(thisInstCount & 0x1)
		{
//			RENDER_ASSERT(thisCode[thisInstCount] & 0xFF000000FF000000 == 0x0000000000000000);
		}
#endif // SHIP_IT

//		*pDMATag++ = createDMATag( thisQWC, DTAG_ID_REF, DTAG_PCE_NOTHING, (unsigned int) thisCode);
		MasterList::AppendDMATAG( thisQWC, TAGID_REF, thisCode );

//		unsigned int *		pVIFPtr = (unsigned int *) pDMATag;

		if( bFlush)
		{
//			*pVIFPtr++ = createFLUSH( 0);
			MasterList::Append(createFLUSH());
			bFlush = false;
		}
		else
//			*pVIFPtr++ = createNOP( 0);
			MasterList::Append(createNOP());
			
//		*pVIFPtr++ = createMPG( (thisInstCount == 256) ? 0 : thisInstCount, thisVUAddr);
		MasterList::Append( createMPG((thisInstCount == 256) ? 0 : thisInstCount, thisVUAddr) );

//		pDMATag = (unsigned long *) pVIFPtr;

		totalVUInst -= thisInstCount;
		thisVUAddr += thisInstCount;
		thisCode += thisInstCount;

	} while( totalVUInst > 0);


//	return pDMATag;
}

void VUDynamicMPGSngl::addToDMA_MPG( unsigned int vuAddr, bool bFlush)
{
//	RENDER_ASSERT( pDMAMem);
//	RENDER_ASSERT( dmaiIsWordAlignedM( pDMAMem) );

//	unsigned long *		pDMATag = (unsigned long *) vifiQWordAlignNOP( (unsigned int *) pDMAMem);

//	*pDMATag++ = createDMATag( d_dmaQWC, DTAG_ID_REF, DTAG_PCE_NOTHING, (unsigned int) p_Code);
	MasterList::AppendDMATAG( d_dmaQWC, TAGID_REF, p_Code );

//	unsigned int *			pVIFPtr = (unsigned int *) pDMATag;
	
#ifndef SHIP_IT
		// if there are an odd number of instructions, we transfer 2 extra vif codes
		// off the end of the instruction stream... we need to make sure they are
		// vif nops..!!
		if(d_VUInstructions & 0x1)
		{
//			printf("code = %x %x\n", (int)(((unsigned long*)p_Code)[d_VUInstructions] >> 32),
//				(int)(((unsigned long*)p_Code)[d_VUInstructions] & 0xFFFFFFFF));
//			RENDER_ASSERT(((unsigned long*)p_Code)[d_VUInstructions] & 0xFF000000FF000000 == 0x0000000000000000);
		}
#endif // SHIP_IT

	if( bFlush)
//		*pVIFPtr++ = createFLUSH( 0);
		MasterList::Append(createFLUSH());
	else
//		*pVIFPtr++ = createNOP( 0);
		MasterList::Append(createNOP());
		
//	*pVIFPtr++ = createMPG( (d_VUInstructions == 256) ? 0 : d_VUInstructions, vuAddr);
	MasterList::Append(createMPG( (d_VUInstructions == 256) ? 0 : d_VUInstructions, vuAddr));
	
//	return pVIFPtr;
}
 	
