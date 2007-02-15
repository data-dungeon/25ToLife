#ifndef __VSTTOANMDBLBUFFER_H_INCLUDED__
#define __VSTTOANMDBLBUFFER_H_INCLUDED__

#include <ostream>

#include "VSTtoDMABlock.h"
#include "VUBufferWrite.h"
#include "MeshInterface.h"
#include "VIFControl.h"
#include "MemManager.h"


class VSTtoANMDblBuffer : public VSTtoDMABlock
{
private:
	int totalVerts;
	int totalBuffers;

	bool lit;

public:
	VSTtoANMDblBuffer( bool _lit );
	~VSTtoANMDblBuffer();

	static bool b_Initted;

	static int BufferStart;
	static int BufferSize;
	static int BufferInputOffset;
	static int BufferInOutSize;
	static int BufferInputSize;
	static int BufferGiftagOffset;
	static int VUMemBase;

	virtual void			adjustVUMemUsage( unsigned int _VUMemBase, unsigned int _VUMemAvailable );

	virtual unsigned int	lastVertices( void ) { return totalVerts; }
	virtual unsigned int	lastVUBuffers( void ) { return totalBuffers; }

	virtual unsigned int	writeDMAModelData( MeshInterface &Protocol, ostream &stream, bool bTTEEnabled,
									unsigned int startState, unsigned int runStates, unsigned int uiFlags );

	virtual unsigned int	calcDMAModelDataSize( MeshInterface &Protocol, unsigned int startState, unsigned int runState, unsigned int uiFlags);

};


#endif