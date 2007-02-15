#include "renderassert.h"

#include "AnimDDMAObject.h"
#include "MasterList.h"
#include "PS2Renderer.h"

#include "DMATag.h"
#include "VIFCreator.h"
#include "VUDataHeaders.h"
#include "VUMicroTables.h"
#include "VUManagedCode.h"
#include "VUManagedSlot.h"

extern VUManagedCode &AnimLFCMP;

#define ROUND_UP_TO(x, val) ((((u_int)x) % (val)) ? (((u_int)x) + ((val)-(((u_int)x)) % (val))) : (x))

#define PERSISTANT_TABLE_SIZE	24

unsigned int 	AnimDDMAObject::d_PersistantLit[PERSISTANT_TABLE_SIZE];
bool				AnimDDMAObject::b_PersistantInited = false;

AnimDDMAObject* AnimDDMAObject::s_sharedMatrixDMAObject = NULL;

void buildPersistantTable( unsigned int * pNewTable, const VUManagedCode &Code, unsigned int *pOffsetTable, unsigned int szeTable);

AnimDDMAObject::AnimDDMAObject( 
	AnimDMAModelHdr & Hdr,

	void *						_Handler,
	DDMAObject *				_Next
)
 :
 DDMAObject(
	(DDMAObject_DCID) Hdr.mDataClassID,
	Hdr.mFlags | OBJ_FLAG_VERTEX_NORMALS | OBJ_FLAG_TEX_COORDS | ((Hdr.mMatrixDMASize == NUM_LIGHT_VECTORS) ? FLAG_LIGHT : 0),
	Hdr.mPRIM,
	NULL,
	Hdr.mDMADataSize + Hdr.mDMAChainSize,
	Hdr.getDTTableData( ),
	Hdr.mVertCount,
	_Handler,
	_Next
	)

{

 	b_MatrixBufferLightsSet = false;
	d_MatrixCount = Hdr.mMatrixCount;

	buildDMAObject( Hdr);

	p_MatrixBuffer = p_MatrixBuffers[0];
	p_DMAPtr = p_MatrixDMAChains[0];

	d_MatrixBufferFrameID = MasterList::GetFrameBeingGenerated( ) - 1;
				
		
}

void AnimDDMAObject::buildDMAObject( AnimDMAModelHdr & Hdr)
{
	// TJC - modified the way this section works to avoid a permenant memory allocation after init time.
	if( !b_PersistantInited)
	{
		RENDER_ASSERT( VUMicro::vuAnimTableLitFogCullMPEntries == PERSISTANT_TABLE_SIZE);
		buildPersistantTable( d_PersistantLit, AnimLFCMP, VUMicro::vuAnimTableLitFogCullMP, VUMicro::vuAnimTableLitFogCullMPEntries);
		
		b_PersistantInited = true;
	}

	if ( s_sharedMatrixDMAObject )
	{
		Hdr.mMatrixCount = 0;
		d_MatrixCount = 0;
	}
	
	RENDER_ASSERT( (Hdr.mMatrixDMASize == 4)  ||  (Hdr.mMatrixDMASize == NUM_LIGHT_VECTORS) );
	
	// lit models need 7 qwords for matrices.  technically I ought to add this as a flag
	// and pass it in the header, because lit models might not always need 7 matrices, but
	// at this point I'm not sure I give a shit.
	bool bLit = (Hdr.mMatrixDMASize == NUM_LIGHT_VECTORS) ? true : false;
	
	// allocate space for DMAChains + 2 * MatrixBuffers
	unsigned int mtxBufferSize = Hdr.mMatrixDMASize * Hdr.mMatrixCount * 16;
	
	// fix up matrix dma chain.  They begin directly after the object header
	unsigned int * pOriginalMatrixDMAChain;
	unsigned int * pLocalMatrixDMAChain1;
	unsigned int * pLocalMatrixDMAChain2;
	
	// TJC - allocate all the memory we need in one hunk...
	void * pNewObjectData = PS2RendererMemAlign( 128, 2 * ROUND_UP_TO( Hdr.mDMAChainSize * 4, 128) + (2 * mtxBufferSize) );
	RENDER_ASSERT( pNewObjectData);

	// TJC - to support instances, the first thing we need to do is make a duplicate of the
	// DMA chain so tat we can modify it to point at our matrix buffer.
	pOriginalMatrixDMAChain = (unsigned int *) Hdr.getObjectData( );
	pLocalMatrixDMAChain1 = (unsigned int *) pNewObjectData;
	pLocalMatrixDMAChain2 = (unsigned int *) ((unsigned int) pNewObjectData + ROUND_UP_TO( Hdr.mDMAChainSize * 4, 128));

	// skip the Matrix DMAChains
	pNewObjectData = ((char *) pNewObjectData) + 2 * ROUND_UP_TO( Hdr.mDMAChainSize * 4, 128);

	memcpy( pLocalMatrixDMAChain1, pOriginalMatrixDMAChain, Hdr.mDMAChainSize * 4);
	memcpy( pLocalMatrixDMAChain2, pOriginalMatrixDMAChain, Hdr.mDMAChainSize * 4);

	//pMatrixDMAData1 = pHeader + 4;
	// the data looks like this:
	// 2 copies for double buffered matrices
	// use the following format:
	//		word 0: offset to geometry block (only on CALL tags
	//		word 1: matrix index (-1 for CALL tag, -2 for finishing tag
	//		word 2: setcycl(4,4)
	//		word 3: unpack matrix into cache
	// a matrix index of -2 indicates that we're done with the matrix dma chain, so we
	// should build the final dmatag there.
	// NOTE: we only need to build dmatags.  vifcodes are already correct in the stream

	u_int * pMatrixBuffer1 = (u_int *) pNewObjectData;
	pNewObjectData = ((char *) pNewObjectData) + mtxBufferSize;

	if ( s_sharedMatrixDMAObject )
		pMatrixBuffer1 = (u_int*) s_sharedMatrixDMAObject->MatrixBuffer( 0 );

	fixupMatrixDMAChain( pLocalMatrixDMAChain1, pOriginalMatrixDMAChain, pMatrixBuffer1, Hdr.mMatrixDMASize);

	u_int * pMatrixBuffer2 = (u_int *) pNewObjectData;
	pNewObjectData = ((char *) pNewObjectData) + mtxBufferSize;

	if ( s_sharedMatrixDMAObject )
		pMatrixBuffer2 = (u_int*) s_sharedMatrixDMAObject->MatrixBuffer( 1 );

	fixupMatrixDMAChain( pLocalMatrixDMAChain2, pOriginalMatrixDMAChain, pMatrixBuffer2, Hdr.mMatrixDMASize);

	if( Hdr.mOffsetToInstructionFixup != 0) 
	{
		fixupGeometryChain( &Hdr, Hdr.mOffsetToInstructionFixup, bLit);
		Hdr.mOffsetToInstructionFixup = 0;
	}

	p_MatrixBuffers[0] = pMatrixBuffer1;
	p_MatrixBuffers[1] = pMatrixBuffer2;

	p_MatrixDMAChains[0] = pLocalMatrixDMAChain1;
	p_MatrixDMAChains[1] = pLocalMatrixDMAChain2;
	
}

void AnimDDMAObject::fixupMatrixDMAChain( unsigned int *pMatrixChain, unsigned int *pOriginalChain, void *pMatrixBuffer, unsigned int mtxEntrySize )
{
DMATag *						pDMA;
	
	while( 1 )
	{
		pDMA = (DMATag *) pMatrixChain;
		
		if( -1 == (int) pMatrixChain[1] ) // CALL tag to geometry
		{
			unsigned int geomAddrBase = (unsigned int) pOriginalChain;
			unsigned int geomAddrOffset = pMatrixChain[0];

			pDMA->setQWC( 0 );
			pDMA->setID( ID_CALL );
			
			pDMA->setADDR( (u_int) geomAddrBase + geomAddrOffset );
		}
		else if( -2 == (int) pMatrixChain[1] ) // finishing tag
		{
			pMatrixChain[0] = 0;
			pMatrixChain[1] = 0;
			pDMA->setQWC( 0 );
			pDMA->setID( ID_RET );
			pDMA->setADDR( 0);
		}
		else if( -3 == (int) pMatrixChain[1] ) // finishing tag
		{
			pMatrixChain[0] = 0;
			pMatrixChain[1] = 0;
			pDMA->setQWC( 0 );
			pDMA->setID( ID_RET );
			pDMA->setADDR( 0);
			break;
		}
		else // REF tag for matrix into cache
		{
			unsigned int mtxIndex = pMatrixChain[1];
			unsigned int mtxAddress = mtxIndex * mtxEntrySize * 16;

			pDMA->setQWC( mtxEntrySize );
			pDMA->setID( ID_REF );

			pDMA->setADDR( ((unsigned int) pMatrixBuffer) + mtxAddress );
		}
		
		pMatrixChain += 4;
		pOriginalChain += 4;
	}
	
}

void AnimDDMAObject::fixupGeometryChain( AnimDMAModelHdr * pDataBuffer, unsigned int instFixupOffset, bool bLit)
{
u_int					i;
u_int					u32Offset;

	unsigned int * pInstructionAddrs = d_PersistantLit;

	// instruction stream fixup
	u32Offset = instFixupOffset;
	
	// this is the beginning of the instruction stream for each buffer
	unsigned int *		pInst;
	unsigned int *		pIStream = (unsigned int *) pDataBuffer;

	while( u32Offset )
	{
		// the fourth word of the first instruction in the stream (and there will
		// always be at least one, since we need one for transofrm) holds the
		// fixup offset in the lower 16 bits, the number of instructions in the next
		// 8 bits, and the number of verts in the high 8 bits
		u_int 					nInst, nVert;

		pIStream += (u32Offset / 4);
		u32Offset = pIStream[3] & 0xffff;
		nInst = (pIStream[3] & 0xff0000) >> 16;
		nVert = pIStream[3] >> 24;

		pInst = (u_int *) pIStream;
		for( i = 0; i < nInst; i++ )
		{
			//pInst[0] is the instruction ID.  look it up in vu code addresses
			pInst[0] = pInstructionAddrs[ (pInst[0] & 0xff) ];
			pInst += 4;
		}
		
		// now we're pointing at the vertex stream

		// skip the UNPACK for the geometry
		pInst++;
		
		if( bLit )
		{
			for( i = 0; i < nVert; i++ )
			{
				// for lit models, the instruction is in the lower byte of
				// pInst[3] along with the normal X.  look up the vu code address and delta
				// it from the last instruction
				unsigned int inst = pInst[3] & 0xff;
				if( i == 0)
					inst = fixupAnimLitAddress( pInstructionAddrs, inst, AL_INST_FIRST);
				else if( i == (nVert - 1) )
					inst = fixupAnimLitAddress( pInstructionAddrs, inst, AL_INST_LAST);
				else
					inst = fixupAnimLitAddress( pInstructionAddrs, inst, AL_INST_OTHER);
				pInst[3] &= (u_int)~0xff;
				pInst[3] |= inst;
				
				pInst += 8; // 2 qwords per vertex
			}
		}
		else
		{
			for( i = 0; i < nVert; i++ )
			{
				// for non-lit models, the instruction is in pInst[3], and should be looked up
				// as a full vu code address, rather than a delta.
				pInst[3] = pInstructionAddrs[ (pInst[3] & 0xff) ];


				pInst += 4; // 1 qwords per vertex
			}
			
			// skip ST & RGBA
			pInst += 4 * nVert;

		}
		
		*pInst = VIFCreator::createMSCNT( );
	}

}


unsigned int AnimDDMAObject::fixupAnimLitAddress( unsigned int *pVUInstructionTable, unsigned int inst, ALInstType instType)
{
unsigned int	vuAddr = pVUInstructionTable[inst];

	switch( instType)
	{
		case AL_INST_FIRST:
		{ // 1st vertex will always jump to a NoWrite routine
			vuAddr -= pVUInstructionTable[VUMicro::ANIML_TRANSFORM_MTX_1_CONT_NW];
			break;
		}
		case AL_INST_LAST:
		{ // last vertex will always jump to a Finish routine
			vuAddr -= pVUInstructionTable[VUMicro::ANIML_TRANSFORM_FINISH];
			break;
		}
		default:
		{ // all others with jump into core, based on Transform Load
			vuAddr -= pVUInstructionTable[VUMicro::ANIML_TRANSFORM_PROCESS_INST];
		}
	}

	return vuAddr;
}

void buildPersistantTable( unsigned int * pNewTable, const VUManagedCode &Code, unsigned int *pOffsetTable, unsigned int szeTable)
{

	VUManagedSlot * pLockedSlot = Code.lockSlot( );

	RENDER_ASSERT( pLockedSlot);

	for( unsigned int i = 0; i < szeTable; i++)
	{
		unsigned int offsetInsts = Code.offsetInstructions( (void *) pOffsetTable[i] );

		pNewTable[i] = pLockedSlot->vuAddr( ) + offsetInsts;
	}

}

