#ifndef __VU_STATIC_DATA_3_H
#define __VU_STATIC_DATA_3_H

#include "VUDataClass.h"
#include "GIFTag.h"
#include "DMAObject.h"

class VUStaticData3 : public VUDataClass
{
	GIFTag						d_GIFTag;

	unsigned int				d_VUMemOffset;
	
public:
	VUStaticData3( );
	void						init(VUCodeSetupHelper& helper);

	void 						prepDMA( DDMAObject &Model, const PS2Matrix4x4& Mtx1, const PS2Matrix4x4& Mtx2, const VURenderInfo &rndInfo);
} ;

#endif
	