#ifndef __DMA_CONTROL_H
#define __DMA_CONTROL_H

#include <ostream>
using namespace 		std;

#include "Display/PS2/Renderer/Common/DMATag.h"
#include "ByteAligner.h"

class DMAControl
{
	static ByteAligner	stdAlign;

	bool					b_TagWritten;
	bool					b_TTEEnabled;
	bool					b_AlignChunk;

	streampos			d_DMAStart;
	streampos			d_DMAEnd;
	streampos			d_DMATagPosition;
	streampos			d_DMATagPadPosition;

	ostream &			d_str;

	OStreamAligner *	p_Align;

	DMATag *				p_DMATag;

public:
	DMAControl( ostream &defstream, bool TTEEnabled, bool defAlign);

	ostream &			stream( void) { return d_str; }

	void					setStreamAligner( OStreamAligner *p_Align);
	void					seekPadPosition( void );

	bool					TTEFlag( void) const { return b_TTEEnabled; }
	bool					ChunkAlignFlag( void) const { return b_AlignChunk; }

	void					enableTTE( void ) { b_TTEEnabled = true; }
	void					disableTTE( void ) { b_TTEEnabled = false; }

	void					enableChunkAlign( void) { b_AlignChunk = true; }
	void					disableChunkAlign( void) { b_AlignChunk = false; }

	void 					writeDMATag( DMATag &Tag, bool bWritePad);
	void					updateDMATag( DMATag *pNewTag );

	void 					startDMAChunk( void );
	unsigned int		endDMAChunk( void );

	void					QWAlign( void );
	unsigned int		QWBytesRemaining( void );
} ;

#endif
