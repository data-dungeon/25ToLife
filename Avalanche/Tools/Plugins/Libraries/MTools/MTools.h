//////////////////////////////////////////////////////////////////////////////////////
//
//	LIBRARY
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _MTOOLS_H
#define _MTOOLS_H

#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MAnimUtil.h>
#include <maya/MVector.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFn.h>
#include <maya/MAnimControl.h>

#include <vector>

class MObject;
class MString;
class MDagPath;
class MDagPathArray;
class MObjectArray;
class MStringArray;
class MStatus;
class MSelectionList;
class IntHash;
class MPlug;
class MPlugArray;
class MIntArray;
class MVector;
class MQuaternion;
class MColor;
class MFloatPointArray;
class MFloatVectorArray;
class MTransformationMatrix;

class MTools
{
public:

//	Reference
public:
	static int		CreateReference(MString filename,MDagPath &result,bool uniquetransforms = false,bool createinstance = true);
	static int		IsReference(MDagPath dagpath,bool recursive = true);
	static MString	GetReferenceFileName(MDagPath dagpath);
	static int		ReloadReference(MDagPath dagpath);
	static int		MakeReferenceLayer(MDagPath dagpath,const MObjectArray &objects,MDagPathArray &results);
	static int		GetReferenceObject(MDagPath dagpath,MDagPath &result,bool usename = false);
	static int		GetTransformBeforeReference(MDagPath dagpath,MDagPath &result);
	static int		GetTransformAfterReference(MDagPath dagpath,MDagPath &result);
	static int		DoesObjectBelongToReferences(MDagPath dagpath);
	static int		RemoveReference(MDagPath dagpath);

//	Instance
public:
	static int		IsInstance(MDagPath dagpath);//RECURSIVE
	static int		GetNonReferenceInstance(MDagPath dagpath,MDagPath &result);
	static int		GetTransformBeforeInstance(MDagPath dagpath,MDagPath &result);
	static int		UpdateInstances(MDagPath dagpath,bool usename = false);
	static int		ReplaceInstances(MDagPath sourcedagpath,MDagPathArray &dests,MDagPathArray &results,bool copy = false,bool removeold = true,bool uniquenames = false);
	static int		InstanceObject(MDagPath dagpath,MDagPath &result,bool uniquetransforms = false,bool copyattributes = true,MObject parentobject = MObject::kNullObj);

//	Duplicate
public:
	static void		CopyHierarchicalTransforms(MDagPath sourcedagpath, MDagPath destdagpath, bool translation = true, bool rotation = true, bool scale = true);
	static void		CopyTransform(MDagPath sourcedagpath, MDagPath destdagpath, bool translation, bool rotation, bool scale);

	static void		CopyShadersBranch(MDagPath sourcedagpath, MDagPath destdagpath);
	static void		CopyShaders(MDagPath sourcedagpath, MDagPath destdagpath);

	static void		CopyHierarchicalAttributes(MDagPath sourcedagpath, MDagPath destdagpath);
	static void		CopyAttributes(MDagPath sourcedagpath, MDagPath destdagpath);

	static int		DuplicateObject(MDagPath dagpath,MDagPath &result,bool copyattributes = true,MObject parentobject = MObject::kNullObj,bool uniquenames = false);

	static int		DuplicateBoundGeometry(MDagPath dagpath,MDagPath &result);
	static int		DuplicateBoundGeometryAndCharacter(MDagPath dagpath,MDagPath &result);

//	Grouping
public:
	static int		MakeReferenceGroup(MDagPath dagpath,const MObjectArray &objects,MDagPath &result);
	static int		IsRefGroup(MObject group);
	static void		AddNodesToInstanceGroup(const MDagPathArray &nodes,MObject igroup);
	static bool		GetShapeReferenceFromReferenceGroup(MObject igroup,MString &reference);

//  Display Layer
public:
	static void		CreateDisplayLayer(MString layername);
	static void		GetCurrentDisplayLayer(MString &name);
	static void		GetDisplayLayers(MStringArray &layers);
	static void		GetNodesInLayer(MString layer,MStringArray &nodes);	
	static int		IsRefLayer(MObject layer);
	static bool		GetShapeReferenceFromLayer(MObject layer,MString &reference);
	static void		AddNodeToDisplayLayer(MString node,MString layername);
	static void		AddNodesToDisplayLayer(MStringArray nodes,MString layername);
	static void		GetDisplayLayerProperties(MString layername,int &visibility,MString &state);
	static void		SetDisplayLayerProperties(MString layername,int visibility,MString state);
	static int		GetDisplayLayer(MString objectname,MString &layername);

//	General
public:
//	find node by name
	static MObject	GetMObject(MString name);
	static int		GetMDagPath(MString name,MDagPath &dagpath);

//	transforms
	static int		GetRootDagPath(MDagPath dagpath,MDagPath &result);
	static int		GetTransformRoot(MDagPath dagpath,MDagPath &result);
	static int		GetFirstShape(MDagPath dagpath,MDagPath &result,bool lookforinstance = false);
	static void		GetShapes(MDagPath dagpath,MDagPathArray &shapes);

	static void		GetNodesOfType(MString type,MObjectArray &objects);
	static MStatus	GetDagNodesOfType(MString type,MDagPathArray &dagpaths);

	static int		NumParents(MDagPath dagpath);
protected:
	static int		NumParentsRecurse(MDagPath dagpath,int count);
public:

//	conversion
	static void		ConvertDagPathsToNames(MDagPathArray dagpaths,MStringArray &mstrings);
	static void		ConvertMObjectsToNames(MObjectArray objects,MStringArray &mstrings);
	static void		ConvertNamesToMObjects(const MStringArray &mstrings,MSelectionList &objects);
	static void		ConvertNamesToMObjects(const MStringArray &mstrings,MObjectArray &objects);
	static void		ConvertSelectionToMObjects(const MSelectionList &selection,MObjectArray &objects);	
	static int		FindMObject(MObject object,MObjectArray &mobjects);//	find mobject in an array of mobjects

//	string
	static void		FindAndReplace(MString &mstr,const char*find,const char *replace);

//	similar
	static MString	GetShortName(MObject object,MString longname);
	static MString	ExtractName(MString name);// this is used to get name from string for similar name comparisions
	static MString	ExtractName2(MString name);
	static int		IsNodeNameSimilar(MString name0,MString name1);// compares two manes for similarity
	static int		FindSimilarName(MDagPath dagpath,MString name,MDagPath &result);//looks through parents and children for similarity
	static MStatus	GetDagPathsOfSimilarName(MString name,MDagPathArray &dagpaths);//iterates through dag for similar names

//	ui
	static int		StartProgressBar(int steps,MString displayline);
	static int		StepProgressBar(int step);
	static int		StopProgressBar();

//	plugs
	static int		IsPlugConnectedToPlug(MPlug plugA,MPlug plugB);

//	misc
	static int		IsMeshConnectedToUVChooser(MDagPath dagpath,MObject uvchooser);
	static int		BakeTransformAndCenterPivot(MDagPath dagpath);//needs to be reevaluated
	static int		ArePolygonObjectsIdentical(MDagPath dagpathA,MDagPath dagpathB);
	static int		ExploreParents(MFn::Type mfntype,MDagPath dagpath,MDagPath child,MDagPath &result,bool inverse = false);
	static int		ExploreChildren(MFn::Type mfntype,MDagPath dagpath,MDagPath &result);

	static int		RedirectConnections(MPlug source,MPlug dest,bool breaksourceconnection = false,bool duplicatedest = true);

	static void		CopyNames(MDagPath sourcedagpath, MDagPath destdagpath);

	static int		MelConnect(MPlug sourceplug,MPlug destplug);

//	object naming
	static int		MakeObjectNamesSimilar(MDagPath srcpath,MDagPath dstpath,int index);
	static int		MakeObjectsNamesSimilar(MDagPath srcpath,MSelectionList selection);
	static int		MakeObjectNamesDifferent(MDagPath srcpath,MDagPath dstpath,int index);
	static int		MakeObjectsNamesDifferent(MDagPath srcpath,MSelectionList selection);

//	Shading
public:
	static MStatus	ConnectShadingGroups(MDagPath dagpath,const MObjectArray &SGList,const MIntArray &shaderindicies);
	static MStatus	ConnectShadingGroups(MDagPath dagpath,const MObjectArray &SGList,const std::vector<int> &shaderindicies);

//	Lighting
public:
	static MObject	CreateLight(MString type,MString name);
	static void		CreateLightAttenuationNetwork(MObject light);
	static void		CreateLightAttenuationNetwork(MObject light,MString animcurvename);

//  Notes Field
public:
	static int		CreateOrReplaceNote(MObject object,MString note);
	static int		InsertKeyValueToNotes(MObject object,MString key,MString value);
	static int		DeleteKeyValueFromNotes(MObject object,MString key);
	static int		GetNote(MObject object, MString &note);
	static int		GetKeyValueFromNotes(MObject object,MString key,MString &value);
	static void		ListKeysFromNotes(MObject object,MStringArray &results);
protected:
	static int		FindKey(MString key,MStringArray &notelines,MString &value);
	static int		FindLine(MString line,MStringArray &notelines);

//	Bones
public:
	static MObject	GetJointCluster(MObject bone);
	static MObject	GetBoneSet(MObject bone);
	static MStatus	GetBoneLookup(MDagPath dagpath,std::vector<int> &keys,MObjectArray &bonesets);
	static void		GetAssociatedSets(MDagPath dagpath,MObject component,MObjectArray &sets);
	static bool		FindBoneByName(const char *name,const MDagPath &dagPath,MDagPath &result);

	static int		GetChildBoneThatMatchesOrientationVector(MDagPath bonepath,MDagPath &endbonepath);

	static MObject	CreateJointCluster(MObject bone);
	static MObject	CreateBoneSet(MObject bone,MSelectionList selection);
	static int		CreateJointClusterChain(MDagPath bonedagpath,MSelectionList vertices,MObject prevjointcluster,MObject &jointcluster,MObject &boneset);
	static MObject	CreateBindPose();
	static int		CreateBone(MDagPath &dagpath,MObject parent,MVector pivottranslation,MQuaternion *orientation);	
	static int		SetParentJointOrientationToChildBone(MDagPath childpath);
	static int		BindSkin(MDagPath meshtransform,std::vector<MDagPath> &bones,std::vector<MSelectionList> &vertexgroups);
	static int		HookUpBoneToBindPose(MDagPath jointdagpath,MObject bindpose,MObject prevjointcluster,MSelectionList vertices,MObject &jointcluster,MObject &boneset);
	static MObject	CreateSkinPartition();
	static int		GetBindPoseMatrix(MDagPath bonedagpath,MTransformationMatrix &matrix);

	static int		GetInclusiveOrientation(MDagPath bonepath,MQuaternion &orientation);
	static int		GetInclusiveBindPoseMatrix(MDagPath bonepath,MMatrix &resultmatrix);
	static int		GetExclusiveBindPoseMatrix(MDagPath bonepath,MMatrix &resultmatrix);
	
	static int		BakeBoneScaleIntoTranslation(MDagPath dagpath);
	static int		ParentMObject(MObject parent,MObject child);
	static int		FindCharacters(MDagPath dagpath,MObjectArray &characterobjects);

	static int		GetRootBone(MDagPath dagpath,MDagPath &rootjointdagpath);
	static int		GetRootBoneFromClip(MObject clipobject,MDagPath &rootbone);
	static int		GetRootBoneFromCharacter(MObject character,MDagPath &rootjointdagpath);
	static int		GetCharacterFromClip(MObject clipobject,MObject &character);
	
	static int		LookThroughTransformPlugsToCharacter(MDagPath dagpath,MObject &characterobject);
	
	static int		CopyCharacter(MObject srcobject,MDagPath dagpath);
	static int		CopyClipAttributes(MObject srcclipobject,MObject dstclipobject);
	static int		CopyClips(MObject srcobject,MObject dstobject);
	static int		GetInstanceClipsPerCharacter(MObject characterobject,MObject sourceclipobject,MObjectArray &instanceclips);
	static int		GetInstanceClipsPerScene(MObject sourceclipobject,MObjectArray &characterobjects,MObjectArray &instanceclips);
	static int		FindMemberNameInSkeleton(MObject member,MDagPath dagpath,MDagPath &result);
	static int		GetClipIndicies(MObject clipscheduler,MStringArray &clipnames,MIntArray &indicies);
	static int		GetClipIndex(MObject clip,MObject clipscheduler,int &index)	;
	
protected:
	static int		FindSimilarNameLookParents(MDagPath dagpath,MString name,MDagPath &result);
	static int		FindSimilarNameLookChildren(MDagPath dagpath,MString name,MDagPath &result);

	static int		FollowConnectionToCharacter(MPlug plug,MObject &characterobject);

	static void		CollectBoneAndVertexAssignments(MDagPath dagpath,std::vector<MDagPath> &bones,std::vector<MSelectionList> &vertexgroups);
	static void		ConvertBoneAndVertexAssignments(MDagPath newrootbone,MDagPath newmesh,std::vector<MDagPath> &bones,std::vector<MSelectionList> &vertexgroups,std::vector<MDagPath> &newbones,std::vector<MSelectionList> &newvertexgroups);


//	Vertex Lighting
public:
	static int		BakeVertexColors(MSelectionList selection,bool shadowmaps,bool reuseshadowmaps);
	static int		BakeVertexColorsPerShadingGroup(MString shadername,MDagPath &dagpath,
											MIntArray &vlist,MIntArray &flist,
											MFloatPointArray &points,MFloatVectorArray &normals,
											bool shadowmaps,bool reuseshadowmaps);
	static void		CopyVertexColors(MDagPath srcPath,MDagPath dstPath);

//	Animation
public:

	static int		CopyKeysToClipboard(MDagPath rootdagpath,int startframe = -1,int endframe = -1);
	static int		PasteKeysFromClipboard(MDagPath rootdagpath,bool includechildren = false);
	static int		GetAnimCurveFromPlug(MPlug plug,MObject &animcurve);

	static int		AddDagPathToClipboard(const MDagPath &path,int startframe,int endframe,MString &clipboardstring);
	static int		AddMObjectToClipboard(const MObject &node,int startframe,int endframe,MString &clipboardstring);
	static int		AddAnimatedPlugs(const MPlugArray &animatedPlugs,const MString &nodeName,int startframe,int endframe,unsigned int depth,unsigned int childCount,MDagPath *dagpath,MString &clipboardstring);
	
	static int		CopyAnimCurve(MObject source, MObject dest);

	static int		ReadStringFromClipboard(MString &clipboard);
	static int		WriteStringToClipboard(const MString &clipboard);

	static int		GetBindPoseFromJoint(MDagPath bonedagpath,MObject &bindposeobject);

	static int		GetParentOrderIndex(const MDagPath &dagPath);

// MPtrBase
	static const MPtrBase *GetMPtrBase(const MFnBase &mfnBase);
	static MPtrBase *GetMPtrBase(MFnBase &mfnBase);
	static const MPtrBase *GetMPtrBase(const MObject &mObject);
	static MPtrBase *GetMPtrBase(MObject &mObject);

};



#endif
