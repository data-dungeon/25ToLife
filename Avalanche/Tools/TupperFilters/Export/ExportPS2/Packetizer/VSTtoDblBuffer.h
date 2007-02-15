#ifndef __VST_TO_DBL_BUFFER_H
#define __VST_TO_DBL_BUFFER_H

#include <ostream>
using namespace 				std;

#include "VSTtoDMABlock.h"
#include "VUBufferWrite.h"
#include "MeshInterface.h"
#include "VIFControl.h"
#include "MemManager.h"
#include "Display/PS2/Renderer/Common/VUDataHeaders.h"
#include "WriteModeTypes.h"
#include "StatesWriteInfo.h"

// TJC - Joe's derived class to specialize for converters that convert from VST to 
// a dma block for a double buffered microcode.

class VSTtoDblBuffer : public VSTtoDMABlock
{
public:
	enum VUHeaderFormat
	{
		VUHEADER_V1 = 0,
		VUHEADER_V1_UINT16,
		VUHEADER_V2,
		VUHEADER_V2_UINT16
	} ;

	typedef void				(VSTtoDblBuffer:: *iterateCallback)(VUStateProtocol &CState, unsigned int stateQWs, bool skipFlag, unsigned int buffersThisState, unsigned int& baseOffset, unsigned int baseVertice, unsigned int readVerts, unsigned int writeVerts);

	// TJC - must be protected, not private (as Joe had it), because if you
	// override calcVIFDataSize, these values get changed.  besides that, 
	// I have an inherent dislike for private data..
protected: 
	bool							b_Initted;

	unsigned int				d_TotalVertices;
	unsigned int				d_TotalVUBuffers;

	unsigned int				d_TexFixupPos;

	void							setupWriteInfo( StateWriteInfo &WriteInfo);
	void							setupWriters( RGBADataFormat RGBAFormat, NormalDataFormat NormalFormat );

	void							addBufferVertCount(unsigned int readCount);

	VUHeaderFormat				d_HeaderFormat;
	unsigned int				d_HeaderVUMemQWords;
	unsigned int				d_HeaderDMAWords;
	void 							(VSTtoDblBuffer::*p_HeaderWriteMethod)( void );

	unsigned int				currentHeader_VUMemOffset;
	unsigned int				currentHeader_StreamPos;

	// V1Header info
	unsigned int				currentBuffer_TotalVerts;
	unsigned int				currentBuffer_StateQWs;
	unsigned int				currentBuffer_RGBAStreamOffset;

	// additional V2Header info
	unsigned int				currentBuffer_StateFlags;
	unsigned int				currentBuffer_NormalStreamOffset;
	unsigned int				currentBuffer_OutputOffset;

	DMATag						d_DMATag;

	unsigned int				d_VUMemBase;
	unsigned int				d_QWsPerBuffer;

	unsigned int				d_QWsPerSetIn;
	unsigned int				d_QWsPerSetOut;

	unsigned int				d_VertsPerSet;
	unsigned int				d_MinSets;

	unsigned int				d_ByteAlignmentAfterBuffer;
	unsigned int				d_StatePacketAlignmentSize;

	unsigned int				d_StartofDMABlock;
	unsigned int				d_DuplicateVerts;

	unsigned int				d_ADCValue;

	unsigned int				d_BlockWriters;
	VUBufferWrite **			p_BlockWriters;

	VUBufferWrite *			p_VertexWriter;

	StateWriteInfo *			p_CurrentWriteInfo;

	ostream *					p_DMAStream;

	MeshInterface *			p_Protocol;
	VIFControl *				p_VIFControl;
	DMAControl *				p_DMAControl;
	
	unsigned int				d_CurrentDMAStart; // can be used to calculate sizeof current DMA block
	unsigned int				d_TotalDataSize; 	// size returned by calcVIFDataSize
	
	// this info is for dynamic texturing fixup
	bool							b_DTEnabled;
	DTInfo *						p_DTTable;
	DTInfo *						p_DTNextEntry;

	// this holds the # of verts uploaded with each buffer full... its only valid AFTER calcVIFDataSize
	unsigned int *				d_BufferVerts; // # of verts in each buffer full
	unsigned int				d_BufferCount; // # of buffers full uploaded
	unsigned int				d_BufferVertsAlloced;

	// member functions
	void						setVUMemUsage( unsigned int _VUMemBase, unsigned int _QWsPerBuffer) { d_VUMemBase = _VUMemBase; d_QWsPerBuffer = _QWsPerBuffer; b_Initted = true; }

	virtual void	 		iterateThroughStates(unsigned int numStates, StateWriteInfo *pStateWriteInfoArray, iterateCallback callback);


	// internal allocation of block writers
	void						allocateBlockWriteData( unsigned int numWriters );
	void						deallocateBlockWriteData( void );
	void						releaseBlockWriters( void );

	// calculate valid set and vertex counts
	unsigned int			cvtVertsToSets( unsigned int numVerts );
	unsigned int			calcValidSets( unsigned int numVerts );
	void						calcValidCounts( unsigned int totalVertices, unsigned int baseVertice,
									unsigned int bufferQWs, unsigned int &readVerts, unsigned int &writeVerts);

	unsigned int			calcEndTriangle( unsigned int baseVertice, unsigned int checkVertices);

	// handle writing of the VUHeader
	void						writeVUHeader( bool skipFlag, unsigned int &baseOffset);
	void 						rewriteVUHeader( bool skipFlag);

	void						writeVUHeaderV1( void );
	void						writeVUHeaderV1_UInt16( void );

	void						writeVUHeaderV2( void );
	void						writeVUHeaderV2_UInt16( void );

	// change the stream idxs for new state
	void						changeWriterStreamIdxs( StateWriteInfo &WriteInfo);

	void						startDMA( unsigned int tagID);
	void						endDMA( void );

	// virtuals
	// can be overridden
	virtual void 			writeVIFBlock( unsigned int numStates, StateWriteInfo *pStateWriteInfoArray);
	virtual void			calcVIFBlockSize( unsigned int numStates, StateWriteInfo *pStateWriteInfoArray);

	virtual void			setupNewState( VUStateProtocol &CState);
	virtual void			writeState( VUStateProtocol &CState, unsigned int &baseOffset);

	virtual void 			writeGeometry( unsigned int &baseOffset, unsigned int baseVertice, unsigned int readVerts, unsigned int writeVerts);
	virtual unsigned int	sizeGeometry( unsigned int readVerts, unsigned int writeVerts);

	virtual void			endGeometry( unsigned int &baseOffset, unsigned int baseVertice, unsigned int readVerts, unsigned int writeVerts);

	// probably be overridden
	virtual void			writeBlockStart( void );
	virtual unsigned int	sizeBlockStart( void );

	virtual	void			writeBufferStart( VUStateProtocol &CState, unsigned int stateQWs, bool skipFlag, unsigned int &baseOffset, unsigned int readVerts, unsigned int writeVerts);
	virtual unsigned int	sizeBufferStart( VUStateProtocol &CState, unsigned int stateQWs );

	virtual void			writeBufferEnd( VUStateProtocol &CState, unsigned int stateQWs, bool skipFlag, unsigned int &baseOffset, unsigned int readVerts, unsigned int writeVerts);
	virtual unsigned int	sizeBufferEnd( VUStateProtocol &CState, unsigned int stateQWs );

	virtual void			writeBlockEnd( void );
	virtual unsigned int	sizeBlockEnd( void );

	// object data block support
	virtual unsigned int getObjectDataSize( unsigned int /*numStates*/, StateWriteInfo * /*pStateWriteInfoArray*/) { return 0; }
	virtual void			writeObjectData( unsigned int /*numStates*/, StateWriteInfo * /*pStateWriteInfoArray*/) { }
	
public:
	VSTtoDblBuffer(
		unsigned int 			_QWsPerSet,
		unsigned int 			_VertsPerSet,
		unsigned int 			_MinSets,
		unsigned int			_ByteAlignmentAfterBuffer = 1,	// byte alignment enforced for buffer end
		unsigned int			_StatePacketAlignmentSize = 1		// byte alignment enforced for state packet
		);

	~VSTtoDblBuffer( );

	virtual void				adjustVUMemUsage( unsigned int _VUMemBase, unsigned int _VUMemAvailable) = 0;

	void							setVUHeaderFormat( VUHeaderFormat);
	VUHeaderFormat				getVUHeaderFormat( void ) 		{ return d_HeaderFormat; }

	// informational functions
	virtual unsigned int		lastVertices( void ) 			{ return d_TotalVertices; }
	virtual unsigned int		lastVUBuffers( void ) 			{ return d_TotalVUBuffers; }

	// work functions
	virtual unsigned int		calcDMAModelDataSize( MeshInterface &Protocol, StatesWriteInfo &WriteInfo);
	virtual unsigned int		writeDMAModelData( MeshInterface &Protocol, ostream &stream, bool b_DMATTE, StatesWriteInfo &WriteInfo);

	// iterateCallbacks used by default implimentations
	virtual void				calcVIFDataSizeCallback( VUStateProtocol &CState, unsigned int stateQWs, bool skipFlag, unsigned int buffersThisState, unsigned int& baseOffset, unsigned int baseVertice, unsigned int readVerts, unsigned int writeVerts);
	virtual void				runStatesCallback( VUStateProtocol &CState, unsigned int stateQWs, bool skipFlag, unsigned int buffersThisState, unsigned int& baseOffset, unsigned int baseVertice, unsigned int readVerts, unsigned int writeVerts);

	// c++ will only allow you to call a pointer to a member if that member is a member of the class that you
	// declare the pointer to the member to be of... in otherwords, iterateThroughStates can only be called
	// for VSTtoDblBuffer::* type functions.  So here are a few that derived classes can use...
	virtual void				derivedCallback1(VUStateProtocol &/*CState*/, unsigned int /*stateQWs*/, bool /*skipFlag*/, unsigned int /*buffersThisState*/, unsigned int& /*baseOffset*/, unsigned int /*baseVertice*/, unsigned int /*readVerts*/, unsigned int /*writeVerts*/) {}
	virtual void				derivedCallback2(VUStateProtocol &/*CState*/, unsigned int /*stateQWs*/, bool /*skipFlag*/, unsigned int /*buffersThisState*/, unsigned int& /*baseOffset*/, unsigned int /*baseVertice*/, unsigned int /*readVerts*/, unsigned int /*writeVerts*/) {}
	virtual void				derivedCallback3(VUStateProtocol &/*CState*/, unsigned int /*stateQWs*/, bool /*skipFlag*/, unsigned int /*buffersThisState*/, unsigned int& /*baseOffset*/, unsigned int /*baseVertice*/, unsigned int /*readVerts*/, unsigned int /*writeVerts*/) {}
	virtual void				derivedCallback4(VUStateProtocol &/*CState*/, unsigned int /*stateQWs*/, bool /*skipFlag*/, unsigned int /*buffersThisState*/, unsigned int& /*baseOffset*/, unsigned int /*baseVertice*/, unsigned int /*readVerts*/, unsigned int /*writeVerts*/) {}
	virtual void				derivedCallback5(VUStateProtocol &/*CState*/, unsigned int /*stateQWs*/, bool /*skipFlag*/, unsigned int /*buffersThisState*/, unsigned int& /*baseOffset*/, unsigned int /*baseVertice*/, unsigned int /*readVerts*/, unsigned int /*writeVerts*/) {}
} ;

#endif
