#ifndef __VST_TO_DBL_VST_AND_Z32_H
#define __VST_TO_DBL_VST_AND_Z32_H

#include "VSTtoDblBuffer.h"
#include "writeVSTandZ32.h"

// TJC - Joe's specialized version of VSTtoDblBuffer for conversion to the
// VCRandSTP format (which means that it has compressed vertex
// and color data as well as ST pairs).

class VSTtoDblVSTandZ32 : public VSTtoDblBuffer
{


protected:
	void									setWriters( void )
	{
		allocateBlockWriteData( 1);

		p_BlockWriters[0] = p_VertexWriter = new writeVSTandZ32( );
	}

public:
	VSTtoDblVSTandZ32( unsigned int _buildOpts = 0);
	void adjustVUMemUsage( unsigned int _VUMemBase, unsigned int _VUMemAvailable);

} ; 

#endif
