#include "ExportPch.h"
#include <assert.h>

#include <strstream>

#include "VSTtoDblBuffer.h"
#include "DMAUtilities.h"
#include "Display/PS2/Renderer/Common/DDMAObjectClassIDs.h"

using namespace 					std;
using namespace 					DMAUtilities;

VSTtoDblBuffer::VSTtoDblBuffer( 
										 unsigned int _QWsPerSetOut, 
										 unsigned int _VertsPerSet, 
										 unsigned int _MinSets, 
										 unsigned int _ByteAlignmentAfterBuffer, 
										 unsigned int _StatePacketAlignmentSize) :
d_DuplicateVerts( 2),
d_BufferVerts( NULL),
d_BufferVertsAlloced( 0),
d_ByteAlignmentAfterBuffer( _ByteAlignmentAfterBuffer),
d_StatePacketAlignmentSize( _StatePacketAlignmentSize)
{
	d_QWsPerSetOut = _QWsPerSetOut;
	d_VertsPerSet = _VertsPerSet;
	d_MinSets = _MinSets;
	
	float ADCTemp = 2056.0f;
	d_ADCValue = * ((unsigned int *) &ADCTemp);
	
	b_Initted = false;

	setVUHeaderFormat( VUHEADER_V2);
}


VSTtoDblBuffer::~VSTtoDblBuffer( void )
{

	if( d_BufferVerts)
		free( d_BufferVerts);

	releaseBlockWriters( );
	deallocateBlockWriteData( );
}

// Setup routines for new Processing

void VSTtoDblBuffer::setupWriters( RGBADataFormat RGBAFormat, NormalDataFormat NormalFormat )
{
	d_QWsPerSetIn = 0;
	
	for( unsigned int i = 0; i < d_BlockWriters; i++)
	{
		p_BlockWriters[i]->setADCValue( d_ADCValue);
		p_BlockWriters[i]->setSpan( 1);
		
		if( p_Protocol)
			p_BlockWriters[i]->setProtocol( *p_Protocol );
		if( p_VIFControl)
			p_BlockWriters[i]->setVIFControl( *p_VIFControl );

		p_BlockWriters[i]->setRGBAStorageFormat( RGBAFormat);
		p_BlockWriters[i]->setNormalStorageFormat( NormalFormat);

		d_QWsPerSetIn += p_BlockWriters[i]->getVUQWordsPerSet( );
	}
	
}

void VSTtoDblBuffer::setupWriteInfo( StateWriteInfo &WriteInfo)
{
	p_CurrentWriteInfo = &WriteInfo;

	changeWriterStreamIdxs( WriteInfo);
}

void VSTtoDblBuffer::changeWriterStreamIdxs( StateWriteInfo &WriteInfo)
{
	for( unsigned int i = 0; i < d_BlockWriters; i++)
		p_BlockWriters[i]->setStageIdxs( WriteInfo.idxRGBAStream, WriteInfo.idxTexCoordStream, WriteInfo.idxNormalStream );
}

void VSTtoDblBuffer::setupNewState( VUStateProtocol &/*CState*/)
{
	if( p_VertexWriter)
		p_VertexWriter->setADCCount( 2);
}

void VSTtoDblBuffer::addBufferVertCount(unsigned int readCount)
{
	if( d_BufferVertsAlloced - d_TotalVUBuffers == 0 )
	{
		d_BufferVertsAlloced += 100;
		if( d_BufferVerts)
		{
			d_BufferVerts = (unsigned int *) realloc( d_BufferVerts, d_BufferVertsAlloced * sizeof( unsigned int));
		}
		else
		{
			d_BufferVerts = (unsigned int *) malloc( d_BufferVertsAlloced * sizeof( unsigned int));
		}
	}
	
 	d_BufferVerts[d_TotalVUBuffers++] = readCount;
}

void VSTtoDblBuffer::iterateThroughStates( unsigned int numStates, StateWriteInfo *pStateWriteInfoArray, VSTtoDblBuffer::iterateCallback callback)
{
	assert( b_Initted == true);
	assert( p_VertexWriter != 0);

	// calculate sizes for InBuffer and OutBuffer
	unsigned int availableMem = d_QWsPerBuffer - d_HeaderVUMemQWords - 1; 	//  - HeaderQWs - 1 (for GIFTag)
	unsigned int d_QWsPerSet = d_QWsPerSetIn + d_QWsPerSetOut;

	unsigned int maxSets = availableMem / d_QWsPerSet;								// max sets that will fit
	unsigned int szeOutBuffer = (maxSets * d_QWsPerSetOut) + 1; 	 			// GIFTag takes 1 QW
	unsigned int szeInBuffer = d_QWsPerBuffer - szeOutBuffer; 		
		
	unsigned int szeOutGeom = szeOutBuffer - 1; 										// GIFTag takes 1 QW
	unsigned int szeInGeom = szeInBuffer - d_HeaderVUMemQWords; 				// goemHdr

	currentBuffer_OutputOffset = szeInBuffer;

	d_TotalVertices = 0;

	for( unsigned int currentStateIdx = 0; currentStateIdx < numStates; currentStateIdx++)
	{
		bool							skipFlag;
		unsigned int				baseOffset;
		unsigned int				baseVertice;

		unsigned int				readVerts, writeVerts;
		unsigned int				totalVertices;

		unsigned int				buffersThisState;

		unsigned int				stateQWordsIn, stateQWordsOut;

		StateWriteInfo &			currentWriteInfo = pStateWriteInfoArray[currentStateIdx];

		// here we go
		setupWriteInfo( currentWriteInfo);		

		// setup VUState
		VUStateProtocol &	CState = p_Protocol->activateState( currentWriteInfo.idxState, currentWriteInfo.idxPass );

		setupNewState( CState);
		stateQWordsIn = stateQWordsOut = CState.sizeVUQWords( );

		// setup static header info
		currentBuffer_StateFlags = currentWriteInfo.u16VUStateFlags;
	
		totalVertices = p_Protocol->numVertices( );

		baseVertice = 0;

		baseOffset = 0;
		buffersThisState = 0;

		while( baseVertice < totalVertices)
		{
			unsigned int maxInSets = (szeInGeom - stateQWordsIn) / d_QWsPerSetIn;
			unsigned int maxOutSets = (szeOutGeom - stateQWordsOut) / d_QWsPerSetOut;
			
			if( maxInSets > maxOutSets)
				maxInSets = maxOutSets;

			calcValidCounts( totalVertices, baseVertice, maxInSets, readVerts, writeVerts);

			assert( ((cvtVertsToSets( writeVerts) * d_QWsPerSetIn) + d_HeaderVUMemQWords + stateQWordsIn) <= szeInBuffer);
			assert( ((cvtVertsToSets( writeVerts) * d_QWsPerSetOut) + 1 ) <= szeOutBuffer);
			
			if( (d_DuplicateVerts != 0)  &&  
				((p_Protocol->idx( baseVertice + readVerts - 1) & MeshInterface::IDX_END_OF_STRIP) == 0))
			{
				skipFlag = true;
			} else
			{
				skipFlag = false;
			}

			(this->*callback)(CState, stateQWordsIn, skipFlag, buffersThisState, baseOffset, baseVertice, readVerts, writeVerts);

			baseVertice += readVerts;
			
			if( skipFlag  &&  (baseVertice != totalVertices))
			{
				baseVertice -= d_DuplicateVerts;
				p_VertexWriter->setADCCount( d_DuplicateVerts);
			}
			
			baseOffset = 0;
			buffersThisState++;
			
			stateQWordsIn = stateQWordsOut = 0;
		}

		d_TotalVertices += totalVertices;
	}
}

void VSTtoDblBuffer::calcVIFDataSizeCallback( VUStateProtocol &CState, unsigned int stateQWs, bool /*skipFlag*/, unsigned int buffersThisState, unsigned int& /*baseOffset*/, unsigned int /*baseVertice*/, unsigned int readVerts, unsigned int writeVerts)
{
	addBufferVertCount( readVerts);
			
	d_TotalDataSize += sizeBufferStart( CState, stateQWs );

	d_TotalDataSize += d_HeaderDMAWords;
			
	if( buffersThisState == 0)
	{
		if( b_DTEnabled == false)
		{
			unsigned int sizeState = CState.sizeDMAWords( );
			if( sizeState  &&  ( sizeState % d_StatePacketAlignmentSize != 0))
					sizeState += d_StatePacketAlignmentSize - (sizeState % d_StatePacketAlignmentSize);

			d_TotalDataSize += sizeState;
		}
	}
			
	d_TotalDataSize += sizeGeometry( readVerts, writeVerts);
	d_TotalDataSize += sizeBufferEnd( CState, stateQWs);
			
	// TJC - added this if for alignment...
	if(d_TotalDataSize % d_ByteAlignmentAfterBuffer != 0)
	{
		d_TotalDataSize += d_ByteAlignmentAfterBuffer - (d_TotalDataSize % d_ByteAlignmentAfterBuffer);
	}
			
}

void VSTtoDblBuffer::runStatesCallback( VUStateProtocol &CState, unsigned int stateQWs, bool skipFlag, unsigned int buffersThisState, unsigned int& baseOffset, unsigned int baseVertice, unsigned int readVerts, unsigned int writeVerts)
{
unsigned int			headerVUMemOffset;

	currentBuffer_TotalVerts = readVerts;
	currentBuffer_StateQWs = stateQWs;

	unsigned int startStreamPos = p_DMAStream->tellp( );
	
	writeBufferStart( CState, stateQWs, skipFlag, baseOffset, readVerts, writeVerts);
			
	headerVUMemOffset = baseOffset;
	baseOffset += d_HeaderVUMemQWords;

	if( buffersThisState == 0)
		baseOffset += CState.sizeVUQWords( );

	unsigned int dataStreamPos = p_DMAStream->tellp( );
	
	writeGeometry( baseOffset, baseVertice, readVerts, writeVerts);
	endGeometry( baseOffset, baseVertice, readVerts, writeVerts);

	unsigned int hrdStreamPos = p_DMAStream->tellp( );

	writeVUHeader( skipFlag, headerVUMemOffset);
				
	unsigned int stateStreamPos = p_DMAStream->tellp( );

	if( !b_DTEnabled  &&  buffersThisState == 0)
	{
		writeState( CState, headerVUMemOffset );
	}
			
	unsigned int endStreamPos = p_DMAStream->tellp( );

	writeBufferEnd( CState, stateQWs, skipFlag, baseOffset, readVerts, writeVerts);

	unsigned int finalStreamPos = p_DMAStream->tellp( );

}

void VSTtoDblBuffer::calcVIFBlockSize( unsigned int numStates, StateWriteInfo *pStateWriteInfoArray)
{
	d_TotalDataSize += sizeBlockStart( );

	iterateThroughStates( numStates, pStateWriteInfoArray, &VSTtoDblBuffer::calcVIFDataSizeCallback);

	d_TotalDataSize += sizeBlockEnd( );
}

void VSTtoDblBuffer::writeVIFBlock( unsigned int numStates, StateWriteInfo *pStateWriteInfoArray)
{
	writeBlockStart( );

	iterateThroughStates( numStates, pStateWriteInfoArray, &VSTtoDblBuffer::runStatesCallback);
	
	writeBlockEnd( );
}

void VSTtoDblBuffer::setVUHeaderFormat( VUHeaderFormat _VUHeaderFormat)
{
	d_HeaderFormat = _VUHeaderFormat;

	switch( _VUHeaderFormat)
	{
		case VUHEADER_V2:
			d_HeaderVUMemQWords = 2;
			d_HeaderDMAWords = 9; // UNPACK (1) + 8 words (8)
			p_HeaderWriteMethod = writeVUHeaderV2;
			break;

		case VUHEADER_V2_UINT16:
			d_HeaderVUMemQWords = 2;
			d_HeaderDMAWords = 5; // UNPACK (1) + 8 halfwords (8 / 2)
			p_HeaderWriteMethod = writeVUHeaderV2_UInt16;
			break;
		default:
		case VUHEADER_V1:
			d_HeaderVUMemQWords = 1;
			d_HeaderDMAWords = 5; // UNPACK (1) + 4 words (4)
			p_HeaderWriteMethod = writeVUHeaderV1;
			break;
		case VUHEADER_V1_UINT16:
			d_HeaderVUMemQWords = 1;
			d_HeaderDMAWords = 3; // UNPACK (1) + 4 halfwords (4 / 2)
			p_HeaderWriteMethod = writeVUHeaderV1_UInt16;
			break;
	}
}

void VSTtoDblBuffer::writeVUHeader( bool /*skipFlag*/, unsigned int &baseOffset)
{
	assert( p_HeaderWriteMethod);

	currentHeader_VUMemOffset = baseOffset;
	
	currentHeader_StreamPos = p_DMAStream->tellp( );

	(this->*p_HeaderWriteMethod)( );

	baseOffset += d_HeaderVUMemQWords;
}


void VSTtoDblBuffer::rewriteVUHeader( bool /*skipFlag*/)
{
	assert( p_HeaderWriteMethod);

	unsigned int currentStreamPos = p_DMAStream->tellp( );

	p_DMAStream->seekp( currentHeader_StreamPos);

	(this->*p_HeaderWriteMethod)( );

	p_DMAStream->seekp( currentStreamPos);

}

void VSTtoDblBuffer::writeVUHeaderV1( void )
{
unsigned int				temp;

	*p_DMAStream << createUNPACK( currentHeader_VUMemOffset, V4_32, 1, 1);

	temp = currentBuffer_TotalVerts;
	p_DMAStream->write( (char *) &temp, 4);

	temp = currentBuffer_StateQWs;
	p_DMAStream->write( (char *) &temp, 4);

	temp = calcValidSets( currentBuffer_TotalVerts);	
	p_DMAStream->write( (char *) &temp, 4);

	temp = currentBuffer_RGBAStreamOffset;
	p_DMAStream->write( (char *) &temp, 4);

}

void VSTtoDblBuffer::writeVUHeaderV1_UInt16( void )
{
unsigned short				temp;

	*p_DMAStream << createUNPACK( currentHeader_VUMemOffset, V4_16, 1, 1);

	temp = currentBuffer_TotalVerts;
	p_DMAStream->write( (char *) &temp, 2);

	temp = currentBuffer_StateQWs;
	p_DMAStream->write( (char *) &temp, 2);

	temp = calcValidSets( currentBuffer_TotalVerts);	
	p_DMAStream->write( (char *) &temp, 2);

	temp = currentBuffer_RGBAStreamOffset;
	p_DMAStream->write( (char *) &temp, 2);

}

void VSTtoDblBuffer::writeVUHeaderV2( void )
{
	unsigned int				temp;

	*p_DMAStream << createUNPACK( currentHeader_VUMemOffset, V4_32, 2, 1);

	// qword 1 - TotalVerts | TransformSets | StateSize | StateFlags
	temp = currentBuffer_TotalVerts;
	p_DMAStream->write( (char *) &temp, 4);

	temp = calcValidSets( currentBuffer_TotalVerts);	
	p_DMAStream->write( (char *) &temp, 4);

	temp = currentBuffer_StateQWs;
	p_DMAStream->write( (char *) &temp, 4);

	temp = currentBuffer_StateFlags;
	p_DMAStream->write( (char *) &temp, 4);

	// qword 2 - RGBA Offset | Normal Offset | ?? | Output offset

	temp = currentBuffer_RGBAStreamOffset;		// rgba offset
	p_DMAStream->write( (char *) &temp, 4);

	temp = currentBuffer_NormalStreamOffset; 	// rgba offset
	p_DMAStream->write( (char *) &temp, 4);

	temp = 0;
	p_DMAStream->write( (char *) &temp, 4);

	temp = currentBuffer_OutputOffset;			// Data Output offset
	p_DMAStream->write( (char *) &temp, 4);
}

void VSTtoDblBuffer::writeVUHeaderV2_UInt16( void )
{
	unsigned short				temp;

	*p_DMAStream << createUNPACK( currentHeader_VUMemOffset, V4_16, 2, 1);

	// qword 1 - TotalVerts | TransformSets | StateSize | StateFlags
	temp = currentBuffer_TotalVerts;
	p_DMAStream->write( (char *) &temp, 2);

	temp = calcValidSets( currentBuffer_TotalVerts);	
	p_DMAStream->write( (char *) &temp, 2);

	temp = currentBuffer_StateQWs;
	p_DMAStream->write( (char *) &temp, 2);

	temp = currentBuffer_StateFlags;
	p_DMAStream->write( (char *) &temp, 2);

	// qword 2 - RGBA Offset | Normal Offset | ?? | Output Offset

	temp = currentBuffer_RGBAStreamOffset; 	// rgba offset
	p_DMAStream->write( (char *) &temp, 2);

	temp = currentBuffer_NormalStreamOffset; 	// Normal offset
	p_DMAStream->write( (char *) &temp, 2);

	temp = 0;
	p_DMAStream->write( (char *) &temp, 2);

	temp = currentBuffer_OutputOffset;			// Output offset
	p_DMAStream->write( (char *) &temp, 2);
}

void VSTtoDblBuffer::writeBufferStart( VUStateProtocol &CState, unsigned int stateQWs, bool skipFlag, unsigned int &baseOffset, unsigned int readVerts, unsigned int writeVerts)
{
	skipFlag = skipFlag;	baseOffset = baseOffset; readVerts = readVerts;	writeVerts = writeVerts;

	if( b_DTEnabled  &&  (stateQWs != 0))
	{
		if( d_TotalVertices)
			endDMA( );

		p_DTNextEntry->uiBlockOffset = (unsigned int) p_DMAStream->tellp( ) - p_DTNextEntry->uiBlockOffset;
		p_DTNextEntry->uiTexID = CState.getDynamicTextureID( );
		p_DTNextEntry++;
		
		startDMA( ID_RET);
	}
		

}

unsigned int VSTtoDblBuffer::sizeBufferStart( VUStateProtocol & /*CState*/, unsigned int stateQWs )
{
	
	if( b_DTEnabled  &&  (stateQWs != 0))
	{
		unsigned int szeEndLastDMA = 0;
		
		if( d_TotalVertices)
		{
			unsigned int szeDMABlock = d_TotalDataSize - d_CurrentDMAStart - 2;
			szeEndLastDMA = (szeDMABlock % 4) ? (4 - (szeDMABlock % 4)) : 0;
		}
		
		d_CurrentDMAStart = d_TotalDataSize + szeEndLastDMA + 2;
		
		return 2 + szeEndLastDMA;
	}
			
	return 0;
}

void VSTtoDblBuffer::writeBufferEnd( VUStateProtocol &CState, unsigned int stateQWs, bool skipFlag, unsigned int &baseOffset, unsigned int readVerts, unsigned int writeVerts)
{
	stateQWs = stateQWs; skipFlag = skipFlag;	baseOffset = baseOffset; readVerts = readVerts;	writeVerts = writeVerts;

	*p_DMAStream << createMSCNT( );
}

unsigned int VSTtoDblBuffer::sizeBufferEnd( VUStateProtocol & /*CState*/, unsigned int /*stateQWs*/ )
{ 
	return 1; 
}

// These methods will hopefully be part of the base class

unsigned int VSTtoDblBuffer::cvtVertsToSets( unsigned int numVerts )
{
	/* calculates the number of sets for a given number of vertices. */

	return (numVerts + (d_VertsPerSet - 1)) / d_VertsPerSet;
}

unsigned int VSTtoDblBuffer::calcValidSets( unsigned int numVerts )
{
	unsigned int numSets = cvtVertsToSets( numVerts);

	return (numSets < d_MinSets) ? d_MinSets : numSets;
}

unsigned int VSTtoDblBuffer::calcEndTriangle( unsigned int baseVertice, unsigned int checkVertices)
{

	unsigned int lastVert = checkVertices - 1;

	for( unsigned int i = 0; i < checkVertices; i++)
	{
		unsigned int	vtxIdx = p_Protocol->idx( baseVertice + i);

		if( (vtxIdx & MeshInterface::IDX_END_OF_STRIP)  &&  ((lastVert - i) < 3))
			return i + 1;
	}

	return checkVertices;
}


void VSTtoDblBuffer::calcValidCounts( unsigned int totalVertices, unsigned int baseVertice,
	unsigned int bufferSets, unsigned int &readVerts, unsigned int &writeVerts)
{

	unsigned int availableVerts = totalVertices - baseVertice;
	unsigned int bufferVerts = bufferSets * d_VertsPerSet;

	if( availableVerts > bufferVerts)
	{ // full block
		if( (totalVertices - (baseVertice + bufferVerts)) < (d_MinSets * d_VertsPerSet) )
			readVerts = calcEndTriangle( baseVertice, totalVertices - baseVertice - (d_MinSets * d_VertsPerSet));
		else
			readVerts = calcEndTriangle( baseVertice, bufferVerts);
	}
	else
	{ // partial block
		readVerts = availableVerts;
	}

	writeVerts = readVerts;

}

// misc Writers

void VSTtoDblBuffer::writeState( VUStateProtocol &CState, unsigned int &baseOffset )
{
	int words = CState.sizeDMAWords( );
	if( words )
	{
		CState.writeVIF( *p_DMAStream, baseOffset);

		while( words % d_StatePacketAlignmentSize != 0)
		{
			*p_DMAStream << createNOP( );
			words++;
		}

		if( d_TexFixupPos == 0)
			d_TexFixupPos = CState.getLastPacketStartPos( );
	}
	
}


void VSTtoDblBuffer::writeGeometry( unsigned int &baseOffset, unsigned int baseVertice, unsigned int readVerts, unsigned int writeVerts)
{

	for( unsigned int i = 0; i < d_BlockWriters; i++)
		p_BlockWriters[i]->writeBlock( baseOffset, baseVertice, readVerts, writeVerts);

}

void VSTtoDblBuffer::endGeometry( unsigned int & /*baseOffset*/, unsigned int /*baseVertice*/, unsigned int /*readVerts*/, unsigned int /*writeVerts*/)
{
	currentBuffer_RGBAStreamOffset = p_VertexWriter->lastRGBAStreamVUMemOffset;
	currentBuffer_NormalStreamOffset = p_VertexWriter->lastNormalStreamVUMemOffset;
}

unsigned int VSTtoDblBuffer::sizeGeometry( unsigned int readVerts, unsigned int writeVerts)
{
	unsigned int sizeBlocks = 0;

	for( unsigned int i = 0; i < d_BlockWriters; i++)
		sizeBlocks += p_BlockWriters[i]->calcBlockSize( readVerts, writeVerts);

	return sizeBlocks;
}

// utility functions. may not have a lot to do with writing

void VSTtoDblBuffer::allocateBlockWriteData( unsigned int numWriters )
{
	d_BlockWriters = numWriters;

	p_BlockWriters = new VUBufferWrite *[numWriters];
}

void VSTtoDblBuffer::deallocateBlockWriteData( void )
{

	if( p_BlockWriters)
		delete [] p_BlockWriters;

	d_BlockWriters = 0;

}

void VSTtoDblBuffer::releaseBlockWriters( void )
{
	for( unsigned int i = 0; i < d_BlockWriters; i++)
	{
		if( p_BlockWriters[i])
		{
			delete p_BlockWriters[i];

			p_BlockWriters[i] = 0;
		}
	}

}

void VSTtoDblBuffer::startDMA( unsigned int tagID)
{
	d_DMATag.setQWC( 0);
	d_DMATag.setID( tagID);
	d_DMATag.setADDR( 0);
	
	p_DMAControl->writeDMATag( d_DMATag, true);

	p_VIFControl->startVIFChunk( );
	p_DMAControl->startDMAChunk( );
	
	p_DMAControl->seekPadPosition( );	
}

void VSTtoDblBuffer::endDMA( void )
{
	unsigned int		finalQWC;

	p_VIFControl->endVIFChunk( );

	p_DMAControl->endDMAChunk( );
	finalQWC = p_DMAControl->endDMAChunk( );
	
	d_DMATag.setQWC( finalQWC);
	
	p_DMAControl->updateDMATag( &d_DMATag );

}
	
void VSTtoDblBuffer::writeBlockStart( void )
{
	if( b_DTEnabled == false)
		startDMA( ID_RET);
}

unsigned int VSTtoDblBuffer::sizeBlockStart( void )
{
	unsigned int szeStart;
	
	if( b_DTEnabled == false)
	{
		szeStart = 2;
		d_CurrentDMAStart = d_TotalDataSize + szeStart;
	}
	else
		szeStart = 0;
		
	return szeStart;
}

void VSTtoDblBuffer::writeBlockEnd( void )
{
	endDMA( );
}

unsigned int VSTtoDblBuffer::sizeBlockEnd( void )
{
	unsigned int szeDMABlock = d_TotalDataSize - d_CurrentDMAStart - 2;
	
	return (szeDMABlock % 4) ? (4 - (szeDMABlock % 4)) : 0;
}

// calculate size of buffer needed
unsigned int VSTtoDblBuffer::calcDMAModelDataSize( MeshInterface &Protocol, StatesWriteInfo &WriteInfo)
{
	unsigned int szePreDMAData = 0;
	
	d_TotalVUBuffers = 0;
	d_TotalDataSize = 0;

	p_Protocol = &Protocol;

	assert( p_VIFControl == 0);
	assert( p_DMAControl == 0);

	if( WriteInfo.Flags & STATES_DYNAMIC_TEXTURES)
		b_DTEnabled = true;
	else
		b_DTEnabled = false;

	setupWriters( 	createRGBADataFormat( WriteInfo.RGBAStorage, WriteInfo.RGBASize),
						createNormalDataFormat( WriteInfo.NormalStorage, WriteInfo.NormalSize) );

	// fills d_LengthofDMAChain;
	calcVIFBlockSize( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);

	// pad out to qword boundry..
	if( (d_TotalDataSize % 4) != 0)
		d_TotalDataSize += 4 - (d_TotalDataSize % 4);

	if( b_DTEnabled)
	{
		szePreDMAData += 1;		// for count
		szePreDMAData += (WriteInfo.numStates * sizeof( DTInfo)) / 4;
	}

	assert( (sizeof( DMAModelHdr) % 4) == 0);
	unsigned int szeHeader = ( sizeof( DMAModelHdr) / 4);
	unsigned int szeObjectData = getObjectDataSize( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);

	szePreDMAData += szeHeader + szeObjectData;
	
	if( szePreDMAData % 4)
		szePreDMAData += 4 - (szePreDMAData % 4);
		
	d_StartofDMABlock = szePreDMAData;
	
	d_TotalDataSize += d_StartofDMABlock;

	return d_TotalDataSize;
}

unsigned int VSTtoDblBuffer::writeDMAModelData( MeshInterface &Protocol, ostream &stream, bool b_DMATTE, StatesWriteInfo &WriteInfo)
{
	unsigned int					currPos;
	unsigned int					szeObjectData;

	DMAModelHdr						Header;

	p_Protocol = &Protocol;
	p_DMAStream = &stream;

	d_TexFixupPos = 0;

	// setup the data stuff
	p_DMAControl = new DMAControl( stream, b_DMATTE, true);
	p_VIFControl = new VIFControl( *p_DMAControl);

	szeObjectData = getObjectDataSize( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);

	// master data info...
	PRIMReg				PRIM = Protocol.peekVUState( WriteInfo.pStateWriteInfoArray[0].idxState, WriteInfo.pStateWriteInfoArray[0].idxPass).getPRIM( );

	Header.mFixupClassID = FIXUP_STATIC;
	if( WriteInfo.RGBAStorage == RGBA_STORE_UNCOMPRESSED)
		Header.mDataClassID = DATA_STATIC_UC;
	else
		Header.mDataClassID = DATA_STATIC_CC;

	Header.mPRIM = PRIM;

	Header.mFlags = OBJ_FLAG_TEXTURE_COORDS;

	if( WriteInfo.Flags & STATE_FLAG_ENVIRONMENT_MAPPED )
		Header.mFlags |= OBJ_FLAG_ENVIRONMENT_MAPPED;

	if( WriteInfo.RGBAStorage != RGBA_STORE_NONE)
		Header.mFlags |= OBJ_FLAG_VERTEX_COLORS;
	if( WriteInfo.NormalStorage != NORMAL_STORE_NONE)
		Header.mFlags |= OBJ_FLAG_VERTEX_NORMALS;

	// write out the data header....
	//Write init info to buffer.  mOffsetToFirstTexture will need to be filled in when we get the first state offset
	int headerPos = (int) p_DMAStream->tellp( );
	p_DMAStream->write( (char *) &Header, sizeof( DMAModelHdr));

	assert( (int) p_DMAStream->tellp( ) % 4 == 0);

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
			p_DTTable[i].uiBlockOffset = headerPos;
		
		stream.seekp( numDTStructs * sizeof( DTInfo), ios::cur );
	}
	else
	{
		b_DTEnabled = false;
		p_DTNextEntry = p_DTTable = 0;
	}
	
	currPos = p_DMAStream->tellp( );
	writeObjectData( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);
	assert( szeObjectData * 4 == (int) p_DMAStream->tellp( ) - currPos);

	currPos = p_DMAStream->tellp( );
	if( ( currPos - headerPos) % 16)
	{
		char				streamPad[17] = { 0 };
		unsigned int	padBytes = 16 - ((currPos - headerPos) % 16);

		stream.write( streamPad, padBytes);		
	}

	Header.mOffsetToDMAData = (int) p_DMAStream->tellp( ) - headerPos;
	assert( Header.mOffsetToDMAData % 16 == 0);
		
	setupWriters( 	createRGBADataFormat( WriteInfo.RGBAStorage, WriteInfo.RGBASize),
						createNormalDataFormat( WriteInfo.NormalStorage, WriteInfo.NormalSize) );

	Protocol.enableColorSet( 0);

	VUStateProtocol::clearFixupChain( );

	writeVIFBlock( WriteInfo.numStates, WriteInfo.pStateWriteInfoArray);

	unsigned int endDMAData = (unsigned int) p_DMAStream->tellp( ) - headerPos;
	
	assert( (int) p_DMAStream->tellp( ) % 4 == 0);

	// pad out to qword boundry..
	if( endDMAData % 16)
	{
		while( (int) p_DMAStream->tellp( ) % 16 != 0)
			*p_DMAStream << createNOP( );
	}

	Header.mVertCount = d_TotalVertices;

	if( b_DTEnabled == false)	
		Header.mOffsetToFirstTexture = d_TexFixupPos;
	else
	{
		for( unsigned int i = 0; i < WriteInfo.numStates; i++)
		{
			assert( p_DTTable[i].uiBlockOffset >= Header.mOffsetToDMAData);
			
			p_DTTable[i].uiBlockOffset -= Header.mOffsetToDMAData;
		}
	}

	// seek back and fix header
	currPos = (unsigned int) p_DMAStream->tellp( );

	p_DMAStream->seekp( headerPos);
	p_DMAStream->write( (char *) &Header, sizeof( DMAModelHdr));
	p_DMAStream->seekp( currPos);	
	
	delete p_VIFControl; p_VIFControl = 0;
	delete p_DMAControl; p_DMAControl = 0;

	return (unsigned int) p_DMAStream->tellp( );
}









