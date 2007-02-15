#ifndef __VU_DATA_CLASS_H
#define __VU_DATA_CLASS_H

#include <libvu0.h>

#include "PS2Renderer.h"
using namespace PS2Renderer;

#include "DDMAObjectClassIDs.h"
#include "VUDataHeaders.h"
#include "VURenderInfo.h"
#include "VUCodeSet.h"
#include "GIFTag.h"

class VUCodeSetupHelper;
class DMAObject;
class DDMAObject;
class VUCodeManager;

class VUDataClass
{

	DDMAObject_DCID			d_ClassID;

protected:
	static VUCodeSetup				CachedVUSetup;
	static TextureDataCallback * 	p_TextureDataCallback;
	static PS2MaterialInfo			d_DefaultMaterial;
	static VUCodeManager *			p_CodeManager;
	
	unsigned int						d_VUMemOffset;
	GIFTag								d_GIFTag;
	GIFTag								d_ShadowerGIFTag;

	struct LightingInfo
	{
		sceVu0FMATRIX 			DirLightMtx;
		sceVu0FMATRIX			PointLightMtx;
		sceVu0FMATRIX			DirLightColors;
		sceVu0FMATRIX			PointLightColors;
		u_int						numPointLights;
		u_int						numDirectionalLights;
	};
	static LightingInfo		d_LightingInfo;

	VUCodeEntry *			selectBaseCode( VUCodeSet &BaseCodeSet, unsigned int &renderFlags);

	void		 				selectVUCode_Static( VUCodeSet ** pCodeSetArray, VUCodeEntry ** pMiscArray, VURenderInfo & rndInfo, VUCodeSetup & ActiveCode) NAMED_SECTION("text.rend1");
	void						setStandardReqFlags_Static( VURenderInfo & rndInfo) NAMED_SECTION("text.rend1");
		
public:
	static unsigned int		d_VUMemBase;

	VUDataClass(
		DDMAObject_DCID			_ClassID
		)
		: 
		d_GIFTag( 0, 1, 0, 0, 0, 3, 0x00000512),
		d_ShadowerGIFTag( 0, 1, 0, 0, 0, 1, 0x00000002)
	{ 
		d_ClassID = _ClassID; 
	}

	static void				setTextureDataCallback(TextureDataCallback* callback) { p_TextureDataCallback = callback; }
	 
	static void				setCodeManager( VUCodeManager & _CodeManager)	{ p_CodeManager = & _CodeManager; }

	static void				prepLightingInfo( const VURenderInfo & rndInfo );
	static void				prepEnvMappingMatrix( const VURenderInfo & rndInfo, const PS2Matrix4x4 & Mtx1 );
	
	void 						basicPrepDMA( 
		DDMAObject &Model, const PS2Matrix4x4 & Mtx1, const PS2Matrix4x4 & Mtx2, 
		const VURenderInfo & rndInfo, const VUCodeSetup & Setup) NAMED_SECTION("text.rend1");


	void 						appendGSSettings(u_int bind, PS2MaterialInfo* matInfo) NAMED_SECTION("text.rend1");
	void			 			addToMasterList_DTTable( void * pDTTableData, void * pDMAData) NAMED_SECTION("text.rend1");
	
	DDMAObject_DCID		classID( void ) const 									{ return d_ClassID; }

	virtual void			init(VUCodeSetupHelper& helper) = 0;
	virtual void			prepDMA( DDMAObject &Model, const PS2Matrix4x4& Mtx1, const PS2Matrix4x4& Mtx2, const VURenderInfo &rndInfo) = 0;

	// TJC - this is called by the renderer to get information about what the handler needs
	// for a given model and set of render flags.
	// the function should fill in the rndInfo.requirementsFlags & rndInfo.validMatricies flags
	// values before returning from this function.  the rest of the render info should be
	// considered read-only.
	virtual void			getRequirementsFlags( DDMAObject &DMAObject, VURenderInfo &rndInfo) = 0;
};


#endif
