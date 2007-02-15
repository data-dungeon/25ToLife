#ifndef __MAYATOOLS_H
#define __MAYATOOLS_H

#define _BOOL

#include <maya/MObjectArray.h>
#include <maya/MFloatArray.h>
#include <maya/MIntArray.h>
#include <maya/MVectorArray.h>
#include <maya/MPointArray.h>
#include <maya/MDagPath.h>

#include "hspline.h"
#include "hpatch.h"
#include "hstatus.h"
#include "hvector.h"
#include "hintarray.h"
#include "hpoly.h"
#include "hgroup.h"
#include "hshader.h"
#include "uiflags.h"

class mdlObject;

class mayaTools
{
public:
	mayaTools();
	~mayaTools();

	mdlObject			*GetModel();

	static HStatus		BezierToCubicNurbCurve(HSpline *spline,unsigned nSpline);
	static HStatus		BezierSegmentToCubicNurbCurve(HSplineSegment &seg,unsigned nSpline);

	HStatus				createSplineSegments();
	HStatus				createNurbPatches();
	HStatus				createPolygons();
	HStatus				createSkeleton();

	static HStatus		BezierPatchToMayaCubicNurbPatches( HPatch *patch, unsigned nPatch);
	static HStatus		BezierPatchToMayaCubicNurbPatch( HVector *patchVectors, unsigned nPatch );
	static MPoint		HVectorToMPoint(const HVector &other);

	HStatus				createPolygonTables(MPointArray &pa,
											MIntArray &polyCounts,
											MIntArray &polyConnects,
											MVectorArray &normals,
											MIntArray &normalconnects,
											MFloatArray &uArray,
											MFloatArray &vArray,
											MIntArray &uvIds,
											MIntArray &uvCounts,
											HIntArray &sgIds);

	static void			convertHVectorArrayToMPointArray(HVectorArray &hv, MPointArray &mv);
	static void			convertHVectorArrayToMVectorArray(HVectorArray &hv, MVectorArray &mv);
	static void			convertHIntArrayToMIntArray(HIntArray &hi,MIntArray &mi);

	HStatus				connectShadingGroups(MObjectArray &SGList,MObject &polyObject,HIntArray &sgIds);
	HStatus				createShadingGroup(HShader *shader,unsigned sgindex,MObjectArray &SGList,MObjectArray &shaderlist);
	HStatus				createShadingGroups(MObjectArray &SGList);

	HStatus				linkUpMapNode(MString shaderNode,MString attrName,MString mapName,HVector2D repeat);
	HStatus				createPlacementNode(MString filenode, float x_repeat, float y_repeat);

	MString				HashBoneNameToMayaBoneName(const char *bonename);

public:
	mdlObject *model;
	MDagPath m_dagpath;
};

#endif

