#include "renderassert.h"

#include "VUColorSetData.h"

#include "VUCodeSet.h"
#include "VUManagedCode.h"
#include "VUManagedFunction.h"
#include "VUCodeSetupHelper.h"
#include "MasterCodeFactory.h"
#include "VUCode.h"
#include "MasterList.h"
#include "PS2Renderer.h"
#include "DDMAObjectClassIDs.h"

#include "VUCodeDefs.h"

#include "VIFInline.h"
using namespace			VIFInline;

#include <eestruct.h>

extern VUCodeSet			VStd, VAll, VVTP, VShM, VPLight;

struct FixupHeader
{
	PRIMReg prim;
	u_int vertexCount;
	u_int flags;
	u_int offsetToFirstTextureSetting;
	u_int offsetToDMAChain;
	u_int wordLengthOfChain;
	u_int numberOfChunks;
	u_int numberOfColorSets;
	u_int offsetsToColorSets[1]; // a set of offsets to beginnings of color sets [numberOfColorSets]
	//u_int AddrOffsets[]; // a set of offsets to patch locations. [numberOfChunks*2]
};

VUColorSetData::VUColorSetData( void ) :
	VUDataClass( DATA_STATIC_UC )
{

	d_VUMemOffset = (1024 - d_VUMemBase) / 2;

}

void VUColorSetData::init(VUCodeSetupHelper& helper)
{
	// add to the helper
	helper.addCode( StaticUC);
	helper.addCode( StaticUC_Cull);
	helper.addCode( StaticUC_Clip);

	helper.addCode( StaticUCAll);
	helper.addCode( StaticUCAllPLight);
	helper.addCode( StaticUCAll_Cull);
	helper.addCode( StaticUCAll_Clip);
	helper.addCode( StaticUCAllPL_Clip);

	helper.addCode( StaticUCVTP);
	helper.addCode( StaticUCVTP_Cull);
	helper.addCode( StaticUCVTP_Clip);

	helper.addCode( StaticUCSMC);
	helper.addCode( StaticUCSMC_Cull);
	helper.addCode( StaticUCSMC_Clip);
	
	helper.addCode( GenericClip);
}

extern VUCodeSet *		StaticUCArray[];
extern VUCodeEntry *		StaticUCMArray[];

void VUColorSetData::prepDMA( DDMAObject & Model, const PS2Matrix4x4 & Mtx1, const PS2Matrix4x4 & Mtx2, const VURenderInfo & rndInfo)
{
	RENDER_ASSERT( CachedVUSetup.pCachedModelPtr == &Model);
	
	basicPrepDMA( Model, Mtx1, Mtx2, rndInfo, CachedVUSetup );
	
}

// TJC - this is called by the renderer to get information about what the handler needs
// for a given model and set of render flags.
// the funtion should fill in the rndInfo.requirementsFlags & rndInfo.validMatricies flags
// values before returning from this function.  the rest of the render info should be
// considered read-only.
void VUColorSetData::getRequirementsFlags( DDMAObject &Model, VURenderInfo &rndInfo)
{
	
	selectVUCode_Static( StaticUCArray, StaticUCMArray, rndInfo, CachedVUSetup);
	CachedVUSetup.pCachedModelPtr = &Model;
	
	setStandardReqFlags_Static( rndInfo);

}

