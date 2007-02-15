#ifndef __VU_STATIC_DATA_31_H
#define __VU_STATIC_DATA_31_H

#include "VUDataClass.h"
#include "GIFTag.h"
#include "DMAObject.h"
#include "CompilerControl.h"

class VUStaticData31 : public VUDataClass
{
	GIFTag						d_GIFTag;

public:
	VUStaticData31( );
	void						init(VUCodeSetupHelper& helper);

	void 						prepDMA( DDMAObject &Model, const PS2Matrix4x4& Mtx1, const PS2Matrix4x4& Mtx2, const VURenderInfo& rndInfo) NAMED_SECTION("text.rend1");

	// TJC - this is called by the renderer to get information about what the handler needs
	// for a given model and set of render flags.
	// the function should fill in the rndInfo.requirementsFlags & rndInfo.validMatricies flags
	// values before returning from this function.  the rest of the render info should be
	// considered read-only.
	virtual void			getRequirementsFlags( DDMAObject &DMAObject, VURenderInfo &rndInfo) NAMED_SECTION("text.rend1");
} ;

#endif
	
