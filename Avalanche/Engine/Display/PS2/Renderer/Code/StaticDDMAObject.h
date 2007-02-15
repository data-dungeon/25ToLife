#ifndef __STATIC_DISPLAY_DMA_OBJECT_H
#define __STATIC_DISPLAY_DMA_OBJECT_H

#include "DDMAObject.h"

class StaticDDMAObject : public DDMAObject
{
	static void * 			createDMAModel( const DMAModelHdr & Hdr);
	
public:
	StaticDDMAObject(
		const DMAModelHdr &		Hdr,

		void *				_Handler = 0,
		DDMAObject *		_Next = 0
	) :
	DDMAObject(
		Hdr,
		0,
		Hdr.getDMAData( ),
		0,
		Hdr.getDTTableData( ),
		_Handler,
		_Next
	) 
	
	{

	}


} ;


#endif