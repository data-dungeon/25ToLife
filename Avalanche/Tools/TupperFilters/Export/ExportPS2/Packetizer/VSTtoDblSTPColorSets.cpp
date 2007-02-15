//
// TJC - My specialized version of VSTtoDblBuffer for conversion
// to the VSTtoDblSTPColorSets format (which means that it has
// separate vertex and color data as well as ST pairs). This makes
// color uploading a separate step from the XYZ/ST information.

// TJC - This generates a very different data chunk than
// VSTtoDblVCRandSTP.
//
// The data chunk looks like this:
//		FixupDataStructure
//			u32 Offset to start of DMA Chain from FixupDataStructure start (ie, sizeof FixupDataStructure)
//			u32 word length of dma chain
//			u32 number of chunks of data
//			u32[] word offsets in dma chain of ADDR field of DMARef colorData
//				note that the addr field is initialized with starting vtx #
//				of this chunk.
//			zero padding to QWord boundry
// 	DMA Chain:
//			DMARef geomData/stateData  chunk 1
//				NOP, NOP
//			DMARef colorData chunk 1  ADDR=first_vertex_#
//				NOP, NOP
// 		DMARef geomData/stateData  chunk 2
// 			NOP, 

//			DMARef colorData chunk 2  ADDR=first_vertex_#
//				NOP, NOP
// 		DMARef geomData/stateData  chunk 3
//				NOP, MSCNT
//			DMARef colorData chunk 3  ADDR=first_vertex_#
//				NOP, NOP
// 		.
//			.
//			.
// 		DMARet
//				NOP, MSCNT
// 	DMA Data:
// 	   geomData/stateData chunk 1
// 		zero padding to align to a multiple of 1 QW in length
//			geomData/stateData chunk 2
// 		zero padding to align to a multiple of 1 QW in length
// 		.
//			.
//			.
//
//

#include "ExportPch.h"
#include "VSTtoDblSTPColorSets.h"
#include "writeVSTandZ32.h"
#include "DMAUtilities.h"
#include "Display/PS2/Renderer/Common/DDMAObjectClassIDs.h"

// for now this is hard coded... but it should be switched to grow on demand..
#define MAXBUFFERS 200


VSTtoDblSTPColorSets::VSTtoDblSTPColorSets(unsigned int _buildOpts)
: VSTtoDblBuffer( 12, 4, 2, 4, 4), mFixup( 0)
{
	_buildOpts = 0;

	setWriters( );
	
	if( !mFixupBuffer)
		mFixupBuffer = (unsigned int *) malloc( MAXBUFFERS * 2 * sizeof( unsigned int));
	
}

VSTtoDblSTPColorSets::~VSTtoDblSTPColorSets( )
{
	if( mFixupBuffer)
		free( mFixupBuffer);
}

void VSTtoDblSTPColorSets::adjustVUMemUsage(unsigned int _VUMemBase, unsigned int _VUMemAvailable)
{
	setVUMemUsage(_VUMemBase, (_VUMemAvailable / 2) - 1);
}


// this function initializes the writers for this converter.
void VSTtoDblSTPColorSets::setWriters( void)
{
	allocateBlockWriteData( 1);
	
	// we have only one writer...
	p_BlockWriters[0] = p_VertexWriter = new writeVSTandZ32( );
}

void VSTtoDblSTPColorSets::writeBufferStart( VUStateProtocol & /*CState*/, unsigned int stateQWs, bool /*skipFlag*/, unsigned int &baseOffset, unsigned int readVerts, unsigned int /*writeVerts*/)
{
	// start new chunk...
	p_VIFControl->startVIFChunk( );
	p_DMAControl->startDMAChunk( );
	p_VIFChainControl->startVIFChunk( );
	p_DMAChainControl->startDMAChunk( );

	mBufferStart = (int) p_DMAStream->tellp( );
	assert( (mBufferStart % 16) == 0);

}

unsigned int VSTtoDblSTPColorSets::sizeBufferStart( VUStateProtocol &/*CState*/, unsigned int /*stateQWs*/)
{
	return 0;
}

void VSTtoDblSTPColorSets::endGeometry( unsigned int &baseOffset, unsigned int baseVertice, unsigned int readVerts, unsigned int writeVerts)
{
	currentBuffer_RGBAStreamOffset = mRGBAOffset = baseOffset;
	currentBuffer_NormalStreamOffset = p_VertexWriter->lastNormalStreamVUMemOffset;

	baseOffset += readVerts; // cause were going to write out this many RGBAs
}

void VSTtoDblSTPColorSets::writeBufferEnd( VUStateProtocol & CState, unsigned int stateQWs, bool /*skipFlag*/, unsigned int &baseOffset, unsigned int readVerts, unsigned int /*writeVerts*/)
{
	// make sure the VIF data for the geometry block ended smoothly
	// write the MSCNT to the end of geometry block
	// and end the DMA
	p_VIFControl->endVIFChunk( );
	*p_DMAStream << createMSCNT( );

	unsigned int dataQWC = p_DMAControl->endDMAChunk( );

	// RGBA DMATag
	// ref tag to transfer the vertex colors...
	// the addr field refers to the # of qwords
	// into the data to go to get to the color...
	if( b_DTEnabled  &&  (stateQWs != 0))
	{
		if( d_TotalVertices)
		{
			d_DMATag.setID( ID_RET);
			d_DMATag.setADDR( 0);
			d_DMATag.setQWC( 0);

			p_DMAChainControl->writeDMATag( d_DMATag, true);
		}

		unsigned int currPos = p_DMAChainStream->tellp( );

		p_DTNextEntry->uiBlockOffset += currPos;
		p_DTNextEntry->uiTexID = CState.getDynamicTextureID( );
		p_DTNextEntry++;
	}

		
	d_DMATag.setID( ID_REF);
	if(p_Protocol->numRequiredAlphaBits( ) < 2)
	{
		assert(mStartVertex % 8 == 0);
		d_DMATag.setADDR( mStartVertex / 8);
		d_DMATag.setQWC( (readVerts + 7) / 8);
	}
	else
	{
		d_DMATag.setADDR( mStartVertex / 4);
		d_DMATag.setQWC( (readVerts + 3) / 4);
	}
	
	unsigned int mFixupOffset = p_DMAChainStream->tellp( );
	assert( mFixupBuffer[mFixup++] * 4 == (int) mFixupOffset);

	p_DMAChainControl->writeDMATag( d_DMATag, false);

	// write VIFCodes for RGBA UNPACKing
	*p_DMAChainStream << createSTCYCL( 4, 4);
	if(p_Protocol->numRequiredAlphaBits( ) < 2)
	{
		*p_DMAChainStream << createUNPACK( mRGBAOffset, V4_5, readVerts, 1 /* useTOPS */, 0 /* mask */, 1 /* isUnsigned */);
	}
	else
	{
		*p_DMAChainStream << createUNPACK( mRGBAOffset, V4_8, readVerts, 1 /* useTOPS */, 0 /* mask */, 1 /* isUnsigned */);
	}

	mStartVertex += readVerts;

	// cant have start vertex numbers that
	// arent qword aligned.  This accounts
	// for the padding added in writeGeometry( )
	if(p_Protocol->numRequiredAlphaBits( ) < 2)
	{
		if(mStartVertex % 8)
			mStartVertex += 8 - (mStartVertex % 8);
	}
	else
	{
		if(mStartVertex % 4)
			mStartVertex += 4 - (mStartVertex % 4);
	}

	d_DMATag.setID( ID_REF);
	d_DMATag.setADDR( mBufferStart);
	d_DMATag.setQWC( dataQWC);

	mFixupOffset = p_DMAChainStream->tellp( );	
	assert( mFixupBuffer[mFixup++] * 4 == (int) mFixupOffset);

	p_DMAChainControl->writeDMATag( d_DMATag, true);
	
}

unsigned int VSTtoDblSTPColorSets::sizeBufferEnd( VUStateProtocol & /*CState*/, unsigned int stateQWs)
{
	if( b_DTEnabled)
		if( stateQWs  &&  mFixup )
			mLengthOfDMAChain += 4;

	mFixupBuffer[mFixup++] = mLengthOfDMAChain;
	mFixupBuffer[mFixup++] = mLengthOfDMAChain + 4;

	// size of 2 dma tags (w/ 4 imbedded vif codes)
	mLengthOfDMAChain += 8;

	// MSCNT
	return 1;
}

void VSTtoDblSTPColorSets::writeBlockStart( void)
{
	mStartVertex = 0;
	mFixup = 0;
}

unsigned int VSTtoDblSTPColorSets::sizeBlockStart( void)
{
	mFixup = 0;
	return 0;
}

void VSTtoDblSTPColorSets::writeBlockEnd( void)
{
	// return tag
	d_DMATag.setQWC( 0);
	d_DMATag.setID( ID_RET);

	// just for kicks, we fill the ADDR field of the
	// RET tag with the end of the data buffer
	mBufferStart = (int) p_DMAStream->tellp( );

	d_DMATag.setADDR( mBufferStart);

	*p_DMAChainStream << d_DMATag;

	*p_DMAChainStream << createNOP( );
	*p_DMAChainStream << createNOP( );
}

unsigned int VSTtoDblSTPColorSets::sizeBlockEnd( void)
{
	mLengthOfDMAChain += 4;
	return 0;
}

// patch set support... sometimes needed in derived classes...
unsigned int VSTtoDblSTPColorSets::getPatchSetCount( unsigned int /*numStates*/, StateWriteInfo * /*pStateWriteInfoArray*/)
{
	return p_Protocol->numColorSets( );
}

unsigned int VSTtoDblSTPColorSets::getPatchHeaderFlags( unsigned int /*patchNumber*/)
{
	return 0;
}

void VSTtoDblSTPColorSets::derivedCallback1( VUStateProtocol &/*CState*/, unsigned int /*stateQWs*/, bool /*skipFlag*/, unsigned int /*buffersThisState*/, unsigned int& /*baseOffset*/, unsigned int /*baseVertice*/, unsigned int readVerts, unsigned int /*writeVerts*/)
{
	unsigned int colors = readVerts;
	if( p_Protocol->numRequiredAlphaBits( ) < 2)
	{
		if(colors % 8)
			colors += 8 - (colors % 8);

		mTotalColorSize += colors * 2;
	}
	else
	{
		if(colors % 4)
			colors += 4 - (colors % 4);

		mTotalColorSize += colors * 4;
	}
}

unsigned int VSTtoDblSTPColorSets::getPatchDataSize( unsigned int patchNumber, unsigned int numStates, StateWriteInfo *pStateWriteInfoArray)
{
	mTotalColorSize = 0;
	
	p_Protocol->enableColorSet( patchNumber);
	
	iterateThroughStates( numStates, pStateWriteInfoArray, &VSTtoDblBuffer::derivedCallback1);
	
	assert( (mTotalColorSize % 4) == 0);

	return mTotalColorSize / 4;
}

void VSTtoDblSTPColorSets::derivedCallback2( VUStateProtocol &/*CState*/, unsigned int /*stateQWs*/, bool /*skipFlag*/, unsigned int /*buffersThisState*/, unsigned int& /*baseOffset*/, unsigned int baseVertice, unsigned int readVerts, unsigned int /*writeVerts*/)
{
	unsigned int currVert;
	unsigned int j;
	unsigned int vtxIdx;
	
	assert( p_CurrentWriteInfo);

	for( currVert = baseVertice, j = 0; j < readVerts; j++, currVert++)
	{
		vtxIdx = p_Protocol->idx( currVert);
		if(p_Protocol->numRequiredAlphaBits( ) < 2)
		{
			Color1555 color = p_Protocol->clr( p_CurrentWriteInfo->idxRGBAStream, vtxIdx & MeshInterface::IDX_MASK).ColorRGBA5551( );
			p_DMAStream->write( (const char *) &color, 2);
		}
		else
		{
			Color8888 color = p_Protocol->clr( p_CurrentWriteInfo->idxRGBAStream, vtxIdx & MeshInterface::IDX_MASK).ColorRGBA8888( );
			p_DMAStream->write( (const char * ) &color, 4);
		}
	}
	
	if( p_Protocol->numRequiredAlphaBits( ) < 2)
	{
		if( readVerts % 2 != 0)
		{
			Color1555 color = 0xFFFF;
			p_DMAStream->write( (const char *) &color, 2);
			readVerts++;
		}
		if( readVerts % 8 != 0)
		{
			for( j = readVerts % 8; j < 8; j += 2)
			{
				*p_DMAStream << createNOP( );
			}
		}
	}
	else
	{
		if( readVerts % 4 != 0)
		{
			for( j = readVerts % 4; j < 4; j += 1)
			{
				*p_DMAStream << createNOP( );
			}
		}
	}
}

void VSTtoDblSTPColorSets::writePatchData( unsigned int patchNumber, unsigned int numStates, StateWriteInfo *pStateWriteInfoArray)
{
	p_Protocol->enableColorSet( patchNumber);

	iterateThroughStates( numStates, pStateWriteInfoArray, &VSTtoDblBuffer::derivedCallback2);
}

unsigned int VSTtoDblSTPColorSets::getDMAChainSize( unsigned int /*numStates*/, StateWriteInfo * /*pStateWriteInfoArray*/)
{
	return mLengthOfDMAChain;
}

unsigned int VSTtoDblSTPColorSets::getObjectDataSize( unsigned int /*numStates*/, StateWriteInfo * /*pStateWriteInfoArray*/)
{
	unsigned int len = 0;

	// space for # of buffers of data
	len += 1;

	// space for 2 words of patch offsets per buffer
	len += d_TotalVUBuffers * 2;

	// pad to qword boundry
	if(len % 4)
	{
		len += 4 - (len % 4);
	}
	
	return len;
}

void VSTtoDblSTPColorSets::writeObjectData( unsigned int /*numStates*/, StateWriteInfo * /*pStateWriteInfoArray*/)
{
	unsigned int cnt = d_TotalVUBuffers;
	unsigned int val;
	
	unsigned int currPos = p_DMAStream->tellp( );
	// # of patch sets to make
	p_DMAStream->write( (char *) &cnt, sizeof( cnt));
	
	// write out the offsets themselves
	for(unsigned int i = 0; i < cnt; i++)
	{
		val = mFixupBuffer[i * 2] + 1;
		p_DMAStream->write( (char *) &val, sizeof(val));
		val = mFixupBuffer[(i * 2) + 1] + 1;
		p_DMAStream->write( (char *) &val, sizeof(val));
	}

	currPos = p_DMAStream->tellp( );
	// align to qword boundry...
	val = 0;
	
	// total words
	cnt = ((d_TotalVUBuffers * 2) + 1);

	// amount of alignment nessisary
	cnt = 4 - (cnt % 4);

	if(cnt != 4)
	{
		while(cnt--)
		{
			p_DMAStream->write( (char *) &val, sizeof(val));
		}
	}
	
	currPos = p_DMAStream->tellp( );
}

// calculate size of buffer needed
unsigned int VSTtoDblSTPColorSets::calcDMAModelDataSize( MeshInterface &Protocol, StatesWriteInfo &WriteInfo)
{
	d_TotalVUBuffers = 0;
	mLengthOfDMAChain = 0;
	d_TotalDataSize = 0;

	p_Protocol = &Protocol;

	if( WriteInfo.Flags & STATES_DYNAMIC_TEXTURES)
		b_DTEnabled = true;
	else
		b_DTEnabled = false;

	setupWriters( 	createRGBADataFormat( WriteInfo.RGBAStorage, WriteInfo.RGBASize),
					createNormalDataFormat( WriteInfo.NormalStorage, WriteInfo.NormalSize) );
	d_QWsPerSetIn += 4;

	unsigned int szePreDMAData = (sizeof( PatchDMAModelHdr) / 4);
	
	calcVIFBlockSize( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);

	// pad out to qword boundry..
	if( (d_TotalDataSize % 4) != 0)
		d_TotalDataSize += 4 - (d_TotalDataSize % 4);

	if( (mLengthOfDMAChain % 4) != 0)
		mLengthOfDMAChain += 4 - (mLengthOfDMAChain % 4);

	if( b_DTEnabled)
	{
		szePreDMAData += 1;		// for count
		szePreDMAData += (WriteInfo.numStates * sizeof( DTInfo)) / 4;
	}

	// include the dma chain
	d_TotalDataSize += mLengthOfDMAChain;
	
	// deal with data header size
	unsigned int szeObjectData = getObjectDataSize( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);
	unsigned int patchSets = getPatchSetCount( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);

	assert( szeObjectData % 4 == 0);
	assert( sizeof(PatchDMAModelHdr) % 4 == 0);
	assert( sizeof(PatchHdr) % 4 == 0);
	
	szePreDMAData += szeObjectData;
	szePreDMAData += (patchSets * sizeof(PatchHdr) / 4);

	if( szePreDMAData % 4)
		szePreDMAData += 4 - (szePreDMAData % 4);		// pad to happy DMA boundary

	d_StartofDMABlock = szePreDMAData;

	d_TotalDataSize += szePreDMAData;

	// record where patch info starts...
	mStartOfPatchData = d_TotalDataSize * 4;

	for(unsigned int currPatch = 0; currPatch < patchSets; currPatch++)
		d_TotalDataSize += getPatchDataSize( currPatch, WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);

	return d_TotalDataSize;
}

unsigned int VSTtoDblSTPColorSets::writeDMAModelData( MeshInterface &Protocol, ostream &stream, bool b_DMATTE, StatesWriteInfo &WriteInfo)
{
	unsigned int					currPos;
	unsigned int 					currPatch;
	unsigned int					szeObjectData;
	unsigned int					szeDMAChain;
	unsigned int					numPatches;

	PatchDMAModelHdr				Header;

	p_Protocol = &Protocol;
	p_DMAStream = &stream;
	d_TexFixupPos = 0;

	szeObjectData = getObjectDataSize( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);
	szeDMAChain = getDMAChainSize( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);
	numPatches = getPatchSetCount( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);

	// master data info...
	PRIMReg				PRIM = Protocol.peekVUState( WriteInfo.pStateWriteInfoArray[0].idxState, WriteInfo.pStateWriteInfoArray[0].idxPass).getPRIM( );

	Header.mPRIM = PRIM;

	Header.mDataClassID = DATA_STATIC_UC;
	Header.mFixupClassID = FIXUP_PATCH_SETS;
	Header.mPatchSetCount = numPatches;
	Header.mDMAChainSize = szeDMAChain * 4;

	Header.mFlags = OBJ_FLAG_TEXTURE_COORDS;

	if( WriteInfo.Flags & STATE_FLAG_ENVIRONMENT_MAPPED )
		Header.mFlags |= OBJ_FLAG_ENVIRONMENT_MAPPED;

	if( WriteInfo.RGBAStorage != RGBA_STORE_NONE)
		Header.mFlags |= OBJ_FLAG_VERTEX_COLORS;

	if( WriteInfo.NormalStorage != NORMAL_STORE_NONE)
		Header.mFlags |= OBJ_FLAG_VERTEX_NORMALS;

	Header.mOffsetToFirstTexture = 0;

	// write out the data header....
	//Write init info to buffer.  mOffsetToFirstTexture will need to be filled in when we get the first state offset
	int headerPos = (int) stream.tellp( );
	stream.write( (char *) &Header, sizeof( PatchDMAModelHdr));
	assert( (int) stream.tellp( ) % 4 == 0);

	if( WriteInfo.Flags & STATES_DYNAMIC_TEXTURES )
	{
		unsigned int numDTStructs = WriteInfo.numStates;

		b_DTEnabled = true;

		Header.mFlags |= OBJ_FLAG_DYNAMIC_TEXTURE;
		Header.mOffsetToFirstTexture = (unsigned int) stream.tellp( ) - headerPos;

		// write out # of Blocks
		stream.write( (char *) & numDTStructs, 4);

		char *baseMem = ((ostrstream *) &stream)->str( ); ((ostrstream *) &stream)->freeze( 0);

		p_DTNextEntry = p_DTTable = (DTInfo *) (baseMem + stream.tellp( ));

		for( unsigned int i = 0; i < numDTStructs; i++)
			p_DTTable[i].uiBlockOffset = 0;
		
		stream.seekp( numDTStructs * sizeof( DTInfo), ios::cur );
	}
	else
	{
		b_DTEnabled = false;
		p_DTNextEntry = p_DTTable = 0;
	}
	
	unsigned int patchPos = stream.tellp( );
					
	// write the patch headers 
	currPos = mStartOfPatchData;
	for( currPatch = 0; currPatch < numPatches; currPatch++)
	{
		PatchHdr		 			patchHdr;

		patchHdr.mOffsetToPatchData = currPos;
		patchHdr.mPatchDataSize = getPatchDataSize( currPatch, WriteInfo.numStates, WriteInfo.pStateWriteInfoArray) * 4;
		patchHdr.mPatchFlags = getPatchHeaderFlags( currPatch);
		patchHdr.mUnused = 0;

		currPos += patchHdr.mPatchDataSize;
		stream.write( (char*) &patchHdr, sizeof(PatchHdr));
	}
	assert( (int) stream.tellp( ) % 4 == 0);
	
	currPos = p_DMAStream->tellp( );
	writeObjectData( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);
	assert( szeObjectData * 4 == (int) p_DMAStream->tellp() - currPos);

	if( ((int) stream.tellp( ) - headerPos) % 16)
	{
		char				streamPad[16] = { 0 };
		stream.write( streamPad, 16 - (((int) stream.tellp( ) - headerPos) % 16));
	}

	assert( (int) p_DMAStream->tellp( ) % 16 == 0);

	// setup the dma chain stuff
	assert( szeDMAChain % 4 == 0);

	char * 					dmaBlock = (char *) ((ostrstream *) &stream)->str( );

	((ostrstream *) &stream)->freeze( 0);

	dmaBlock += (unsigned int) p_DMAStream->tellp( );
	p_DMAChainStream = new ostrstream( dmaBlock, szeDMAChain * 4, ios::out | ios::binary);
	p_DMAChainControl = new DMAControl( *p_DMAChainStream, b_DMATTE, true);
	p_VIFChainControl = new VIFControl( *p_DMAChainControl);

	// make state data offsets be shifted based on where the dma chain starts
	unsigned int d_TexFixupOffset = p_DMAChainStream->tellp( );

	Header.mOffsetToDMAChain = (int) p_DMAStream->tellp( ) - headerPos;
	assert( Header.mOffsetToDMAChain % 16 == 0);

	// setup the data stuff
	p_DMAStream->seekp( szeDMAChain * 4, ios::cur);
	p_DMAControl = new DMAControl( *p_DMAStream, false, true);
	p_VIFControl = new VIFControl( *p_DMAControl);

	Header.mOffsetToDMAData = (unsigned int) p_DMAStream->tellp( ) - headerPos;
	assert( Header.mOffsetToDMAData % 16 == 0);

	setupWriters( 	createRGBADataFormat( WriteInfo.RGBAStorage, WriteInfo.RGBASize),
					createNormalDataFormat( WriteInfo.NormalStorage, WriteInfo.NormalSize) );

	d_QWsPerSetIn += 4;

	VUStateProtocol::clearFixupChain( );

	writeVIFBlock( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);

	assert( (int) p_DMAStream->tellp( ) % 4 == 0);

	// pad out to qword boundry..
	while( (int) p_DMAStream->tellp( ) % 16 != 0)
		*p_DMAStream << createNOP( );

	Header.mDMADataSize = (unsigned int) p_DMAStream->tellp( ) - Header.mOffsetToDMAData;

	for( currPatch = 0; currPatch < numPatches; currPatch++)
	{
		currPos = (unsigned int) p_DMAStream->tellp( );

		writePatchData( currPatch, WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);
		
		unsigned int endPos = p_DMAStream->tellp( );
		unsigned int writeSize = getPatchDataSize( currPatch, WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);
		
		assert( getPatchDataSize( currPatch, WriteInfo.numStates, WriteInfo.pStateWriteInfoArray) * 4 == (int) p_DMAStream->tellp( ) - currPos);
	}
	
	Header.mVertCount = d_TotalVertices;

	if( b_DTEnabled == false)
		Header.mOffsetToFirstTexture = d_TexFixupOffset + d_TexFixupPos;

	// seek back and fix header
	currPos = (unsigned int) stream.tellp( );

	stream.seekp( headerPos);
	stream.write( (char *) &Header, sizeof( PatchDMAModelHdr));
	stream.seekp( currPos);	

	delete p_DMAChainStream;
	delete p_DMAChainControl;
	delete p_VIFChainControl;

	delete p_VIFControl; p_VIFControl = 0;
	delete p_DMAControl; p_VIFControl = 0;

	
	return (unsigned int) stream.tellp( );
}
