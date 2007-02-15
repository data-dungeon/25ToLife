#include "renderassert.h"

#include <libvu0.h>
#include <math.h>
#include "GIFTag.h"
#include "VUStaticData31.h"

#include "VUCodeSet.h"
#include "VUCodeDefs.h"

#include "VUManagedCode.h"
#include "VUManagedFunction.h"
#include "VUCodeSetupHelper.h"
#include "MasterCodeFactory.h"
#include "VUCode.h"
#include "MasterList.h"
#include "PS2Renderer.h"
#include "DDMAObject.h"

#include "VIFInline.h"
using namespace			VIFInline;

extern VUCodeSet					Std, All, PLight, VTP, ShM;
extern VUCodeSet					AllMP;


VUStaticData31::VUStaticData31( void ) :
	VUDataClass( DATA_STATIC_CC)
{

	d_VUMemOffset = (1024 - d_VUMemBase) / 2;
	
}

void VUStaticData31::init(VUCodeSetupHelper& helper)
{
	// add to the helper
	helper.addCode( StaticCC );
	helper.addCode( StaticCC_Cull );
	helper.addCode( StaticCC_Clip );
//	helper.addCode( StaticCC_Clip2 );
	
	helper.addCode( StaticCCAll );
	helper.addCode( StaticCCAll_Cull );
	helper.addCode( StaticCCAll_Clip );
	
	helper.addCode( StaticCCSMC );
	helper.addCode( StaticCCSMC_Cull );
	helper.addCode( StaticCCSMC_Clip );

	helper.addCode( StaticCCVTP );
	helper.addCode( StaticCCVTP_Cull );
	helper.addCode( StaticCCVTP_Clip );
	
	helper.addCode( StaticCCAC_MP );
	
	helper.addCode( Static_EVMap );
	
	helper.addCode( GenericClip );
	helper.addCode( StaticCCAllPL_Clip );
}

//#define OBJ_DEBUG

#ifdef OBJ_DEBUG
volatile unsigned int objDrawID = 0;
volatile unsigned int objNoDrawID = 0;

volatile unsigned int objDrawAddr = 0;
volatile unsigned int objNoDrawAddr = 0;

unsigned int objCount = 0;
#endif

extern VUCodeSet * 		StaticCCArray[];
extern VUCodeEntry * 	StaticCCMArray[];

void VUStaticData31::prepDMA( DDMAObject &Model, const PS2Matrix4x4& Mtx1, const PS2Matrix4x4& Mtx2, const VURenderInfo &rndInfo)
{
	RENDER_ASSERT( CachedVUSetup.pCachedModelPtr == &Model);
	
	basicPrepDMA( Model, Mtx1, Mtx2, rndInfo, CachedVUSetup );
}

// TJC - this is called by the renderer to get information about what the handler needs
// for a given model and set of render flags.
// the function should fill in the rndInfo.requirementsFlags & rndInfo.validMatricies flags
// values before returning from this function.  the rest of the render info should be
// considered read-only.
void VUStaticData31::getRequirementsFlags( DDMAObject &Model, VURenderInfo &rndInfo)
{

	selectVUCode_Static( StaticCCArray, StaticCCMArray, rndInfo, CachedVUSetup);
	CachedVUSetup.pCachedModelPtr = &Model;
	
	setStandardReqFlags_Static( rndInfo);
	
}




