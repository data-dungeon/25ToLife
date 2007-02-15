#ifndef __VST_TO_DMA_BLOCK_H
#define __VST_TO_DMA_BLOCK_H

#include <ostream>
using namespace 				std;

#include "MeshInterface.h"
#include "VIFControl.h"
#include "StatesWriteInfo.h"

// TJC - Joe's base class for converters to convert from VST format to a dma block

class VSTtoDMABlock
{
	static const unsigned int	d_VUMemAvailable;


public:
	virtual void			adjustVUMemUsage( unsigned int _VUMemBase, unsigned int _VUMemAvailable ) = 0;

	virtual unsigned int	lastVertices( void ) = 0;
	virtual unsigned int	lastVUBuffers( void ) = 0;

	virtual unsigned int	calcDMAModelDataSize( MeshInterface &Protocol, StatesWriteInfo &WriteInfo)
	{
		assert( 0);
		return 0;
	} 

	virtual unsigned int	writeDMAModelData( MeshInterface &Protocol, ostream &stream, bool bTTEEnabled, StatesWriteInfo &WriteInfo)
	{
		assert( 0);
		return 0;
	}

	virtual unsigned int	calcDMAModelDataSize( MeshInterface &Protocol, unsigned int startState, unsigned int runStates, unsigned int uiFlags )
	{
		assert( 0);
		return 0;
	} 

	virtual unsigned int	writeDMAModelData( MeshInterface &Protocol, ostream &stream, bool bTTEEnabled, unsigned int startState, unsigned int runStates, unsigned int uiFlags)
	{
		assert( 0);
		return 0;
	}

} ;

#endif
