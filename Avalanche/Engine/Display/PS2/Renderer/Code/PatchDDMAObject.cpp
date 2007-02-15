#include "renderassert.h"
#include "string.h"
#include "PatchDDMAObject.h"
#include "PS2Renderer.h"

#include "VUDataHeaders.h"

PatchDDMAObject::PatchDDMAObject( 
	const PatchDMAModelHdr & Hdr, 
	unsigned int		patchNumber,

	void *				_Handler,
	DDMAObject *		_Next
) :							  
	DDMAObject(
		(DDMAObject_DCID) Hdr.mDataClassID,
		Hdr.mFlags | OBJ_FLAG_OWNS_DMA_DATA,
		Hdr.mPRIM,
		createDMAChain( Hdr, patchNumber),
		Hdr.mDMADataSize + Hdr.mDMAChainSize,
		Hdr.getDTTableData( ),
		Hdr.mVertCount,
		_Handler,
		_Next
	),

	p_PatchData( Hdr.getPatchData( patchNumber)),
	d_PatchDataSize( Hdr.getPatchHdr( patchNumber)->mPatchDataSize),
	d_PatchDataFlags( Hdr.getPatchHdr( patchNumber)->mPatchFlags)
{

}
	
void * PatchDDMAObject::createDMAChain( const PatchDMAModelHdr & ModelHdr, unsigned int patchNumber)
{
	RENDER_ASSERT( patchNumber < ModelHdr.mPatchSetCount);

	// create a unique master DMAChain for this instance	
	unsigned int *		pNewDMAChain = (unsigned int *) PS2RendererMemAlign( 128, ModelHdr.mDMAChainSize);
	
	RENDER_ASSERT( pNewDMAChain);

	memcpy( pNewDMAChain, ModelHdr.getDMAChain( ), ModelHdr.mDMAChainSize);

	// do address fixup on DMATags in our DMAChain
	const u_long128 *	pColorBuffer = (u_long128 *) ModelHdr.getPatchData( patchNumber);
	
	unsigned int *		pFixupData = (unsigned int *) ModelHdr.getFixupData( );

	unsigned int		numFixupChunks = *pFixupData++;

	while( numFixupChunks--)
	{
		unsigned int		colorSetOffset = pNewDMAChain[*pFixupData];

		pNewDMAChain[*pFixupData] = (u_int) (pColorBuffer + colorSetOffset);
		pFixupData++;

	
		unsigned int		geometryDMAOffset = pNewDMAChain[*pFixupData];

		pNewDMAChain[*pFixupData] = (unsigned int) &ModelHdr + geometryDMAOffset; 
		pFixupData++;
	}

	return pNewDMAChain;
}


