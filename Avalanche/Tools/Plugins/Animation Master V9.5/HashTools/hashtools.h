#ifndef __HashTools_H
#define __HashTools_H

#ifndef __HCP_H
#include "SDK\HCP.h"
#endif

#ifndef __HPATCH_H
#include "SDK\HPatch.h"
#endif

#ifndef __HDECAL_H
#include "SDK\HDecal.h"
#endif

#ifndef __HPROPERT_H
#include "SDK\HPropert.h"
#endif

#ifndef __HPROPDRI_H
#include "SDK\HPropdri.h"
#endif

#ifndef __STRCLASS_H
#include "strclass.h"
#endif

#include <vector>

enum surface_properties { DIFFUSECOLOR,DIFFUSEFALLOFF,AMBIANCE,SPECULARCOLOR,SPECULARSIZE,
							SPECULARINTENSITY,ROUGHNESS,ROUGHNESSSCALE,TRANSPARENCY,DENSITY,
							INDEXOFREFRACTION,TRANSLUCENCY,REFLECTIVITY,REFLECTIVITYFALLOFF,
							RADIANCE,GLOW,TOONLINES,TOONSHADING,DIFFUSERENDERSHADER,
							SPECULARRENDERSHADER };

typedef std::vector<HCP*> HashCPs;
typedef std::vector<HGroup*> HashGroups;
typedef std::vector<HPatch*> HashPatches;
typedef std::vector<HDecal*> HashDecals;
typedef std::vector<HHashObject*> HashAttributes;
typedef std::vector<HDecalStamp*> HashStamps;
typedef std::vector<HBasePatchUV*> HashBasePatchUVs;

class HashTools {
public:
	static void			GetAllGroups(HModelCache *hmc,HashGroups &groups);
	static void			GetAllDecals(HModelCache *hmc,HashDecals &decals);
	static void			GetAllPatches(HModelCache *hmc,HashPatches &patches);

	static void			GetPatchesInGroup(HashPatches &patches,HGroup *group,HModelCache *hmc);
	static void			GetPatchesInStamp(HDecalStamp *stamp,HashPatches &patches);
	static void			GetGroupsFromPatch(HashGroups &groups,HPatch *patch,HashGroups &allGroups);
	static void			GetStampsFromDecal(HDecal *decal,HashStamps &stamps);
	static void			GetDecalsFromPatch(HashDecals &decals,HPatch *patch,HashDecals &allDecals);
	static void			GetStampsFromDecalThatBelongToPatch(HDecal *decal,HashStamps &stamps,HPatch *patch);

	static void			GetBasePatchUVsFromStamp(HDecalStamp *stamp,HashBasePatchUVs &patchuvs);

	static bool			IsPatchInStamp(HDecalStamp *stamp,HPatch *patch);
	static bool			IsPatchInDecal(HDecal *decal,HPatch *patch);
	
	static void			MakeUniqueDecalName(std::string &name,HashDecals &alldecals);

	static HProperty	*GetSurface(HHashObject *attribute);
	static void			GetMaterialAttributesFromGroup(HGroup *group,HashAttributes &attributes);

	static HHashObject	*GetClipContainer(HModelCache *hmc);

	static void			GetPatchCPs(HPatch *patch,HashCPs &cps);
	static void			GetPatch5CPs(HPatch5 *patch,HashCPs &cps);

	static void			GetGroupCPs(HGroup *group,HashCPs &cps);

	static HProperty	*FindProperty(const char *matchname,HProperty *pluginproperties);
	static HProperty	*FindPropertyInPluginProperties(const char *matchname,HProperty *pluginproperties);

	
	static HCP			*MyGetHead(HCP *cp);
	static HCP			*GetHookFromStack(HCP *cp);	
	static HCP			*GetHookBaseFromStack(HCP *cp);
	static HCP			*GetHookBaseNeighborFromStack(HCP *cp);
	
	static bool			IsCPInStack(HCP *cp,HCP *stackcp);
	static bool			IsHookSegment(HCP *hook, HCP *nextcp);
	
};

#include "hashtools.hpp"

#endif