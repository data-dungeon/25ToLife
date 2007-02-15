#ifndef __VIF_DMA_ALIGNER_H
#define __VIF_DMA_ALIGNER_H

#include "ByteAligner.h"
#include "Display/PS2/Renderer/Common/VIFCreator.h"

using namespace		VIFCreator;

class VIFDMAAligner : public ByteAligner
{

public:
	VIFDMAAligner( ) : ByteAligner( 16) { }

	virtual void				align( ostream &str);

} ;

#endif
