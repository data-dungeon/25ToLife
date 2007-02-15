#include "ExportPch.h"
#include <assert.h>

#include <strstream>

#include "VSTtoANMDblBuffer.h"
#include "DMAUtilities.h"
#include "Display/PS2/Renderer/Common/DMAObject.h"
#include "VUStateProtocol.h"
#include "Display/PS2/Renderer/Common/DMATag.h"
#include "Display/PS2/Renderer/Common/GIFTag.h"
#include "Display/PS2/Renderer/Common/VifCode.h"
#include "Display/PS2/Renderer/Common/VUMicroID.h"
#include "Display/PS2/Renderer/Common/DDMAObjectClassIDs.h"

	// a quick rundown of our boned dma format:
	//
	// There is no global header for now, but we need to set the base and offset
	//		VifOffset		/		VifBase		/		DMATag		/		DMATag
	//		VifNOP			/		VifNOP		/		VifNOP		/		VifFlushA

	// For each Buffer:
	// (all matrix chains appear first, followed by geometry chains)

	// Matrix DMA chain:
	// (word layouts appear before the text explaining what they are)
	// 	VifUnpack		/	SetCycl 4,4		/		DMATag		/		DMATag
	//		(a ref that points to matrix1, and unpacks the matrix to its)
	//		(location in the matrix cache)
	// 	VifUnpack		/		VifNop		/		DMATag		/		DMATag
	//		(a ref that points to matrix2, and unpacks the matrix to its)
	//		(location in the matrix cache)
	//	(etc...one REF dmatag for each matrix used by the buffer)
	// The last matrix dma is a CALL into the geometry for that buffer.
	// If there are no matrices being dmaed for the buffer, we need to create an empty
	// dma CALL

	// Geometry chain:
	//									VifUnpack	/		DMATag		/		DMATag
	//	The geometry dmatag is a RET
	//	(unpack following data to beginning of buffer using vtops)
	//		Output Offset	/	Giftag Offset	/	Input Offset	/	Vertcount
	//	if there is a state change associated with this buffer:
	//											State Giftag
	//											State Data
	//											State Data
	// Then:
	//											Tristrip Giftag
	//										Instruction Stream
	//										Instruction Stream
	//	(etc...Instruction stream can be a variable size if appropriate)
	//	if there were a matrix stream, there would be an unpack for it here
	//									Matrix Stream (currently empty)
	//									Matrix Stream (currently empty)
	//	(etc...Matrix stream can be a variable size, and in fact for now will be zero)
	//			VifUnpack
	//    v0 Next Instr	/		v0 Z			/		v0 Y			/		v0 X
	//		v0 ADC | A		/		v0 B			/		v0 T | G		/		v0 S | R
	//    v1 Next Instr	/		v1 Z			/		v1 Y			/		v1 X
	//		v1 ADC | A		/		v1 B			/		v1 T | G		/		v1 S | R
	//    v2 Next Instr	/		v2 Z			/		v2 Y			/		v2 X
	//		v2 ADC | A		/		v2 B			/		v2 T | G		/		v2 S | R
	//		(etc... one for each vert, the last has a jump instruction to exit the transform loop)
	//		( other instructions are jumps to transform by the appropriate matrix in registers )

	//		For lit models, the verts look like:
	//		v0 T				/		v0 Z			/		v0 Y			/		v0 X
	//		v0 S				/		v0 NrmZ		/		v0 NrmY		/		v0 NrmX | Instr Delta
	//		v1 T				/		v1 Z			/		v1 Y			/		v1 X
	//		v1 S				/		v1 NrmZ		/		v1 NrmY		/		v1 NrmX | Instr Delta

	// Finally:
	//			MSCAL/CNT

	// Header Meanings:
	//
	//	Vertex Count - duh
	//
	//	Input Offset - Offset to the beginning of the input geometry buffer
	//
	// Giftag Offset - If there is a state giftag, it is linked to the tristrip
	// 	giftag by setting EOP = 0, and the giftag offset points to the state
	//		giftag.  If there is no state giftag, the giftag offset should point
	//		to the tristrip giftag
	//
	//	Ouput Offset - Offset to the beginning of the output geometry buffer, which should
	//		immediately follow the tristrip giftag


// AnmBuffer is simply a helper class to determine how to build animated model
// DMA chains.  Nobody else should ever have a use for this class or any of
// its members or defines, so it's all here in the CPP file instead of the
// header.  Note that this means AnmBuffer.cpp should not be compiled separately

#include "AnmBuffer.cpp"

bool VSTtoANMDblBuffer::b_Initted = false;

int VSTtoANMDblBuffer::BufferStart;
int VSTtoANMDblBuffer::BufferSize;
int VSTtoANMDblBuffer::BufferInputOffset;
int VSTtoANMDblBuffer::BufferInOutSize;
int VSTtoANMDblBuffer::BufferInputSize;
int VSTtoANMDblBuffer::BufferGiftagOffset;
int VSTtoANMDblBuffer::VUMemBase;

VSTtoANMDblBuffer::VSTtoANMDblBuffer( bool _lit )
{
	b_Initted = false;
	lit = _lit;

	AnmBuffer::Cleanup( );
}

VSTtoANMDblBuffer::~VSTtoANMDblBuffer( )
{
	AnmBuffer::Cleanup( );
}

void VSTtoANMDblBuffer::adjustVUMemUsage(unsigned int _VUMemBase, unsigned int _VUMemAvailable)
{
	VUMemBase = _VUMemBase;

	BufferStart = _VUMemBase + BONED_GLOBAL_HEADER_SIZE + BONED_MATRIX_CACHE_SIZE;
	BufferSize = (_VUMemAvailable - BufferStart) / 2;
	BufferInputOffset = BONED_BUFFER_HEADER_SIZE + BONED_INSTRUCTION_STREAM_SIZE;
	BufferInOutSize = BufferSize - BufferInputOffset;
	BufferInputSize = (BufferInOutSize * 2) / 5;
	BufferGiftagOffset = BufferInputOffset + BufferInputSize;

	b_Initted = true;
}

unsigned int VSTtoANMDblBuffer::writeDMAModelData( MeshInterface &Protocol, ostream &stream,
		bool /*bTTEEnabled*/, unsigned int startState, unsigned int runStates, unsigned int uiFlags )
{
	VUStateProtocol::clearFixupChain( );

	AnmBuffer::SetConversionFlags( uiFlags);
	return AnmBuffer::Write( Protocol, stream, startState, runStates );
}

unsigned int VSTtoANMDblBuffer::calcDMAModelDataSize( MeshInterface &Protocol,
		unsigned int startState, unsigned int runStates, unsigned int uiFlags )
{
	unsigned int s, vert;
	AnmBuffer *pAB;

	assert( startState + runStates <= Protocol.numStates() );

	AnmBuffer::SetConversionFlags( uiFlags);

	for( s = startState; s < startState + runStates; s++ )
	{
		int passes = Protocol.numPasses( s );
		for ( int pass = 0 ; pass < passes ; pass++ )
		{
			VUStateProtocol &state = Protocol.activateState( s, pass );

			if( Protocol.numVertices() == 0 )
				continue; // nothing to do for this state, bail

			vert = 0;
			pAB = new AnmBuffer( vert, lit );
			pAB->AddState( s, state, pass );

			while( vert < Protocol.numVertices() )
			{
				int testv;

				testv = pAB->AddVerts( Protocol, vert );
				vert += testv;
				// is this state going to need another buffer?
				if( vert < Protocol.numVertices() )
				{
					// if the last vert in the last buffer didn't end a strip, we need
					// to duplicate the last 2 verts
					if( !(pAB->StripEnd( Protocol )) )
						vert -= 2;
					pAB = new AnmBuffer( vert, lit );
					pAB->SetState( s, pass );
				}
			}
		}

	}

	return AnmBuffer::TotalSize();
}


/*
	AnmBuffer statics:
 *
	static int TotalWordCount;
	static int Matrices[BONED_MATRIX_CACHE];
	static int MatrixInUse[BONED_MATRIX_CACHE];
	static int Regs[BONED_MATRIX_REGS];
	static int MaxRegs;
	static int MaxCached;
	static int MinVerts;
*/