#ifndef __VU_COLOR_SET_DATA_H
#define __VU_COLOR_SET_DATA_H

#include "VUDataClass.h"
#include "GIFTag.h"
#include "DMAObject.h"
#include "DDMAObject.h"
#include "CompilerControl.h"

class VUColorSetData : public VUDataClass
{
public:
	VUColorSetData( );
	void						init(VUCodeSetupHelper& helper);

	// add an object to the master dma chain
	void 						prepDMA( DDMAObject &Model, const PS2Matrix4x4& Mtx1, const PS2Matrix4x4& Mtx2, const VURenderInfo& vuRenderInfo);

	// TJC - this is called by the renderer to get information about what the handler needs
	// for a given model and set of render flags.
	// the function should fill in the rndInfo.requirementsFlags & rndInfo.validMatricies flags
	// values before returning from this function.  the rest of the render info should be
	// considered read-only.
	virtual void			getRequirementsFlags( DDMAObject &DMAObject, VURenderInfo &rndInfo);

} ;

#endif
