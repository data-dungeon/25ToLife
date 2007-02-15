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

#ifndef __HPROJECT_H
#include "SDK\hproject.h"
#endif

class HProject;

#define MAXFRAMEERROR      0.003f

class CubicCoefficients1D {
  public:
   float a, b, c, d;             // CUBIC COEFFICIENTS f(t) = a*t^3 + b*t^2 + c*t + d
   float fda, fdb, fdc, fdd;     // BUMP FD COEFFICIENTS
   
   CubicCoefficients1D( float p1, float p4, float r1, float r4 ) {
      a =  2*p1 + -2*p4 +    r1 +  r4;
      b = -3*p1 +  3*p4 + -2*r1 + -r4;
      c =                    r1      ;
      d =    p1                      ;
   }
   float Evaluate( float t ) { return t*(t*(a*t + b) + c) + d; }
   float Derivative( float t ) { return t*(3*a*t + 2*b) + c; }
   float InitFD( int steps ) {
      float fd12 = 1.0f/steps;
      float fd11 = fd12 * fd12;
      float fd10 = fd12 * fd11;
      float fd20 = 6.0f * fd10;
      float fd21 = 2.0f * fd11;
      fda = d;
      fdb = a*fd10 + b*fd11 + c*fd12;
      fdc = a*fd20 + b*fd21;
      fdd = a*fd20;
      return fda;
   }   
   float BumpFD() { fda+=fdb; fdb+=fdc; fdc+=fdd; return fda; }
   float GetFDValue() { return fda; }
};

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
typedef std::vector<HTreeObject*> HashTreeObjects;
typedef std::vector<HDecalClip*> HashDecalClips;
typedef std::vector<HClip*> HashClips;
typedef std::vector<HBoneCache*> HashBones;

class HashTools
{
public:
	static void			GetAllGroups(HModelCache *hmc,HashGroups &groups);
	static void			GetAllDecals(HModelCache *hmc,HashDecals &decals);
	static void			GetAllPatches(HModelCache *hmc,HashPatches &patches);
	static void			GetAllCPs(HModelCache *hmc,HashCPs &cps);
	static void			GetAllClips(HModelCache *hmc,HashClips &clips);
	static void			GetAllBones(HModelCache *hmc,HashBones &parents,HashBones &bones);

	static void			GetPatchesInGroup(HashPatches &patches,HGroup *group,HModelCache *hmc);
	static void			GetPatchesInStamp(HDecalStamp *stamp,HashPatches &patches);
	static void			GetGroupsFromPatch(HashGroups &groups,HPatch *patch,HashGroups &allGroups);
	static void			GetDecalsFromPatch(HashDecals &decals,HPatch *patch,HashDecals &allDecals);
	static void			GetStampsFromDecalThatBelongToPatch(HDecal *decal,HashStamps &stamps,HPatch *patch);

	static void			GetStampsFromDecal(HDecal *decal,HashStamps &stamps);
	static void			GetClipsFromDecal(HDecal *decal,HashDecalClips &clips,bool clear = true);
	static void			GetClipsFromGroup(HGroup *group,HashDecalClips &clips,bool clear = true);

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

	static HTreeObject	*FindTreeObject(const char *matchname,HTreeObject *htreeobject);
	static HTreeObject	*FindTreeObject(ObjectType type,HTreeObject *htreeobject);

	static void			FindTreeObject(const char *matchname,HTreeObject *htreeobject,HashTreeObjects &treeobjects);
	static void			FindTreeObject(ObjectType type,HTreeObject *htreeobject,HashTreeObjects &treeobjects);
	
	static HTreeObject	*FindParentTreeObject(ObjectType type,HTreeObject *htreeobject);

	static HProperty	*FindProperty(const char *matchname,HProperty *property);
	static HProperty	*FindProperty(const char *matchname,HTreeObject *htreeobject);
	
	static HProperty	*FindPropertyInPluginProperties(const char *matchname,HHashObject *hashobject);
//	static HProperty	*FindPropertyInUserProperties(const char *matchname,HHashObject *hashobject);

	static void			CopyProperties(HProperty *dstproperty,HProperty *srcproperty);
	static void			CopyProperties(HTreeObject *dsttreeobject,HTreeObject *srctreeobject);

	static HCP			*MyGetHead(HCP *cp);
	static HCP			*GetHookFromStack(HCP *cp);	
	static HCP			*GetHookBaseFromStack(HCP *cp);
	static HCP			*GetHookBaseNeighborFromStack(HCP *cp);
	
	static bool			IsCPInStack(HCP *cp,HCP *stackcp);
	static bool			IsHookSegment(HCP *hook, HCP *nextcp);

//	static bool			Interpolate(HChannelDriver *hchanneldriver,const Time &time,float &value);

private:
	static void			GetBone(HTreeObject *htreeobject,HashBones &parents,HashBones &bones);
//	static float		InterpolateValue(HChannelDriver *hchanneldriver,HChannelCP *hchannelcp,const Time &time);

};

#include "hashtools.hpp"

#endif