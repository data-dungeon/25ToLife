//////////////////////////////////////////////////////////////////////////////////////////
//
// HPATCH ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HPATCHARRAY_H
#define __HPATCHARRAY_H

#ifndef __HPATCH_H
#include "hpatch.h"
#endif
#ifndef __HDECAL_H
#include "hdecal.h"
#endif
#ifndef __HIMAGE_H
#include "himage.h"
#endif
#ifndef __HATTR_H
#include "hattr.h"
#endif
#ifndef __HPOLYARRAY_H
#include "hpolyarray.h"
#endif
#ifndef __UIFLAGS_H
#include "uiflags.h"
#endif
#ifndef __HMAP_H
#include "hmap.h"
#endif
#ifndef __HBONEARRAY_H
#include "hbonearray.h"
#endif
#ifndef __HSTAMP_H
#include "hstamp.h"
#endif
#ifndef __HGROUP_H
#include "hgroup.h"
#endif
#ifndef __HTESSCORNER_H
#include "htesscorner.h"
#endif
#ifndef __HBONECHAINARRAY_H
#include "hbonechainarray.h"
#endif


#include <map>

class mdlObject;

class HPatchArray
{
public:
	HPatchArray ();
	HPatchArray(const HPatchArray &other);
	~HPatchArray ();

	void				clear(void);
	unsigned			length(void) const;
	void				append(HPatch &patch);

	void				calculateTangents();
	void				derivePatchArraySegments();

	bool				derivePatchIndicies(std::vector<int> &cparray,bool deriveAsGroup,HIntMap &patchIndicies,unsigned uvsidx);

	void				setPatchGroupIndicies(HGroupArray &groups,unsigned groupidx);
	void				setPatchDecalIndicies(HDecalArray &decals,unsigned decalidx);

	void				addShaderFromGroups(HPatch *patch,HPolyArray &polygons,std::vector<int> &shaderIndicies);
	void				addShaderFromDecals(HPatch *patch,HPolyArray &polygons,std::vector<int> &shaderIndicies);
	void				getMap(HDecal *decal,HMap &map);

	void				tessellate(HPolyArray &polygons,unsigned numsubdivisions);

	void				deriveBoneVids();
	int				getVIDFromCPID(unsigned cpid,int &hookvindex);

	void				fillInTheHoles();
	int				getVertexClosest(int vid,const std::vector<int> &flags);

	HPatch			*get(unsigned index);
	HPatch			*begin();
	HPatch			*end();
	void				updateBackPointers();

	void				readHPatchArray(std::ifstream &fin);

	HPatch				operator[](unsigned index) const;
	HPatch				&operator[](unsigned index);
	HPatchArray			&operator = (const HPatchArray &other);
	friend std::ostream		&operator << (std::ostream &os,const HPatchArray &patches);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	HImageArray				imageArray;
	HDecalArray				decalArray;
	HAttrArray				attrArray;
	HStampArray				stampArray;

	HBoneArray				boneArray;
	HBoneChainArray			boneChainArray;
	
	HGroupArray				groupArray;

	HTessCornerArray		*tesscorners;	//tessellated corners
	HTessEdgeArray			tessedges;

	mdlObject				*model;

	HIntMap						fivepointintmap;

	std::vector<HPatch> classList;
};

typedef std::pair <int, int> Int_Pair;

#endif