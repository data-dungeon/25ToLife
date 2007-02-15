#include "stdafx.h"
#include "tileUVs.h"
#include "neighbortree.h"

tileUVs::tileUVs() {}

tileUVs::~tileUVs() {}

void* tileUVs::creator()
{
	return new tileUVs();
}

bool tileUVs::isUndoable() const
{
    return false;
}

MStatus tileUVs::undoIt()
{
    return MS::kSuccess;
}

MStatus tileUVs::doIt(const MArgList& args)
{
	MStatus status;

	if (args.length() == 3)
	{
		args.get(0,ui_nostretch);
		args.get(1,ui_welduvs);
		args.get(2,ui_scaletofit);
	}
	else
	{
		DISPLAY_INFO("Missing arguments. [nostretch, welduvs, scaletofit]");
		return MS::kFailure;
	}

	status = redoIt();

	return status;
}

MStatus tileUVs::redoIt()
{
	MDGModifier dgModifier;
	MStatus status;
	MSelectionList selection;
	NeighborTree ntree;

	status = MGlobal::getActiveSelectionList(selection);
	int numSelections = selection.length();

	if (selection.length()==0)
	{
		DISPLAY_ERROR("Nothing selected.");
		return MS::kFailure;
	}
	
	MDagPath dagpath;
	MObject component;
	selection.getDagPath(0,dagpath,component);

	ntree.GenerateNeighborInfo(0);

	if(!ntree.GenerateTopologyInfo())
		return MS::kFailure;

	ntree.GenerateDistanceInfo();

//	cerr << ntree << endl;

	MObject shape = dagpath.node();

	if (dagpath.hasFn(MFn::kTransform))
		shape = dagpath.child(0);

//	MFnMesh fnMesh(shape);

	MFnMesh fnMesh(dagpath);

	MFnDependencyNode shapedepnode(shape);

	MString shapename = shapedepnode.name();

	MString selUVSet;

	fnMesh.getCurrentUVSetName( selUVSet );

	MFloatArray uarray,varray;
	MIntArray uvconnects;
	MIntArray polycounts;

	ntree.GetPolyCounts(polycounts,true);

	ntree.BuildTextureClampData(uarray,varray,uvconnects,ui_nostretch,ui_scaletofit);

	fnMesh.clearUVs(&selUVSet);

	status = fnMesh.setUVs(uarray,varray,&selUVSet);
	if (!status)
		MGlobal::displayError("Couldn't apply uv.(1)");
	
	fnMesh.updateSurface();

	status = fnMesh.assignUVs(polycounts,uvconnects,&selUVSet);
	if (!status)
		MGlobal::displayError("Couldn't apply uv.(2)");
	
	fnMesh.updateSurface();

	fnMesh.syncObject();

//	MGlobal::executeCommand(MString("dgdirty ") + shapename + MString(".outMesh"));
//	dgModifier.doIt();

	MGlobal::executeCommand(MString("delete -ch ") + shapename);
	dgModifier.doIt();

//	MGlobal::executeCommand(MString("dgdirty ") + shapename);
//	dgModifier.doIt();

//	MGlobal::executeCommand(MString("dgdirty -a ") + shapename);
//	dgModifier.doIt();

	status = MGlobal::setActiveSelectionList(selection);
	
    return MS::kSuccess;
}
