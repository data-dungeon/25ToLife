//
// Much of this code is basically copy and paste from Joe's code
// so I dont nessesarily understand it all.  But hopefully, by
// the time Im done, it will work.  Please forgive the sparsity
// of my comments.. not really sure what to write in lots of
// instances.
//

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
// 			NOP, MSCNT
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

#ifndef __VST_TO_DBL_STP_COLOR_SETS_H__
#define __VST_TO_DBL_STP_COLOR_SETS_H__

#include "VSTtoDblBuffer.h"
#include <strstream>


class VSTtoDblSTPColorSets : public VSTtoDblBuffer
{
protected:
	unsigned int			mStartOfDataBlock; 			// offset into the buffer where the data block begins
	unsigned int			mStartOfDMABlock;				// offset into the buffer where the dma block begins
	unsigned int			mLengthOfDMAChain; 			// length of data put into p_DMAStream

	// hold the stuff used to make it all work
	unsigned int			mBufferStart;					// offset to beginning of current buffer data
	unsigned int			mStartVertex;					// index of first vertex in current buffer data
	unsigned int			mLengthOfDMABlock; 			// length of the dma block
	unsigned int			mStartOfPatchData;			// offset to first PatchHeader
	unsigned int *			mFixupBuffer;					// pointer to list of fixup data
	unsigned int			mFixup;							// current pos in mFixupBuffer
	unsigned int			mRGBAOffset;					// offset in this buffer for RGBA write

	// variables used as globals for callback data passing
	unsigned int			mTotalColorSize; 				// used to calculate the total size of one color block...
	unsigned int			mColorUnpackBaseOffsetAt; 	// used to go back and refill in the offset to the colors...
	
	void 						setWriters( void);
	
	ostream *				p_DMAChainStream;
	VIFControl *			p_VIFChainControl;
	DMAControl *			p_DMAChainControl;

	/////////////////////////////////
	// overridable internal stuff
	/////////////////////////////////
	
	// this stuff handles writting out dma tags & MSCNT calls.
	// at least, for VSTtoDblSTPColorSets they do...
	virtual void				writeBlockStart( void);
	virtual unsigned int		sizeBlockStart( void);

	virtual void				writeBlockEnd( void);
	virtual unsigned int		sizeBlockEnd( void);

	virtual void				writeBufferStart( VUStateProtocol &CState, unsigned int stateQWs, bool skipFlag, unsigned int &baseOffset, unsigned int readVerts, unsigned int writeVerts);
	virtual unsigned int		sizeBufferStart( VUStateProtocol &CState, unsigned int stateQWs );

	virtual void				endGeometry( unsigned int &baseOffset, unsigned int baseVertice, unsigned int readVerts, unsigned int writeVerts);

	virtual void				writeBufferEnd( VUStateProtocol &CState, unsigned int stateQWs, bool skipFlag, unsigned int &baseOffset, unsigned int readVerts, unsigned int writeVerts);
	virtual unsigned int		sizeBufferEnd( VUStateProtocol &CState, unsigned int stateQWs );
	
	// patch set support... sometimes needed in derived classes...
	virtual unsigned int		getPatchSetCount( unsigned int numStates, StateWriteInfo *pStateWriteInfoArray);
	virtual unsigned int 	getPatchHeaderFlags( unsigned int patchNumber);
	virtual unsigned int 	getPatchDataSize( unsigned int patchNumber, unsigned int numStates, StateWriteInfo *pStateWriteInfoArray);
	virtual void				writePatchData( unsigned int patchNumber, unsigned int numStates, StateWriteInfo *pStateWriteInfoArray);
	
	virtual unsigned int 	getObjectDataSize( unsigned int numStates, StateWriteInfo *pStateWriteInfoArray);
	virtual unsigned int 	getDMAChainSize( unsigned int numStates, StateWriteInfo *pStateWriteInfoArray);
	virtual void				writeObjectData( unsigned int numStates, StateWriteInfo *pStateWriteInfoArray);

	// a couple of iterators we need...
	virtual void				derivedCallback1(VUStateProtocol &/*CState*/, unsigned int /*stateQWs*/, bool /*skipFlag*/, unsigned int /*buffersThisState*/, unsigned int& /*baseOffset*/, unsigned int /*baseVertice*/, unsigned int /*readVerts*/, unsigned int /*writeVerts*/);
	virtual void				derivedCallback2(VUStateProtocol &/*CState*/, unsigned int /*stateQWs*/, bool /*skipFlag*/, unsigned int /*buffersThisState*/, unsigned int& /*baseOffset*/, unsigned int /*baseVertice*/, unsigned int /*readVerts*/, unsigned int /*writeVerts*/);
		
public:

	/////////////////////////////////
	// contstruction / setup
	/////////////////////////////////

	VSTtoDblSTPColorSets( unsigned int _buildOpts = 0);
	~VSTtoDblSTPColorSets( );
	
	virtual void 				adjustVUMemUsage( unsigned int _VUMemBase, unsigned int _VUMemAvailable);

	/////////////////////////////////
	// work functions
	/////////////////////////////////
	virtual unsigned int		calcDMAModelDataSize( MeshInterface &Protocol, StatesWriteInfo &WriteInfo);
	virtual unsigned int		writeDMAModelData( MeshInterface &Protocol, ostream &stream, bool b_DMATTE, StatesWriteInfo &WriteInfo);

};

#endif // __VST_TO_DBL_STP_COLOR_SETS_H__
