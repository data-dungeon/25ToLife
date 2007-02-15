#ifndef __PATCH_DISPLAY_DMA_OBJECT_H
#define __PATCH_DISPLAY_DMA_OBJECT_H

#include "DDMAObject.h"

class PatchDMAModelHdr;

class PatchDDMAObject : public DDMAObject
{

	static void *			createDMAChain( const PatchDMAModelHdr & Hdr, unsigned int patchNumber);

public:
	void *					p_PatchData;
	unsigned int			d_PatchDataSize;
	unsigned int			d_PatchDataFlags;

	PatchDDMAObject(
		const PatchDMAModelHdr & Hdr,
		unsigned int		patchNumber,

		void *				_Handler = 0,
		DDMAObject * 		_Next = 0
	) ;

} ;

#endif
