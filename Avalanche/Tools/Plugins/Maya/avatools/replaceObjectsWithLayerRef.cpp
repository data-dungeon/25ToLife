#include "stdafx.h"
#include "replaceObjectsWithLayerRef.h"

replaceObjectsWithLayerRef::replaceObjectsWithLayerRef() {}

replaceObjectsWithLayerRef::~replaceObjectsWithLayerRef() {}

void* replaceObjectsWithLayerRef::creator()
{
	return new replaceObjectsWithLayerRef();
}

bool replaceObjectsWithLayerRef::isUndoable() const
{
    return false;
}

MStatus replaceObjectsWithLayerRef::undoIt()
{
    return MS::kSuccess;
}

MStatus replaceObjectsWithLayerRef::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus replaceObjectsWithLayerRef::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;

	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	if (selection.length()==0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MString result;
	MTools::GetCurrentDisplayLayer(result);
	if (result == "")
		return MS::kFailure;

	MObject layerobject = MTools::GetMObject(result);

	if (!MTools::IsRefLayer(layerobject))
	{
		DISPLAY_ERROR("No Instance Layer selected.");
		return MS::kFailure;
	}

	MString refname;
	MTools::GetShapeReferenceFromLayer(layerobject,refname);

	const char *debug = refname.asChar();
	MObject refobject = MTools::GetMObject(refname);

	MDagPathArray dagpatharray;
	dagpatharray.clear();

	for (unsigned i = 0; i < selection.length(); i++)
	{
		MDagPath dagpath;
		selection.getDagPath(i,dagpath);
		dagpatharray.append(dagpath);
	}

	MDagPath dagpath;
	MFnDagNode dagnode(refobject);
	dagnode.getPath(dagpath);

	MDagPathArray results;

	if (!MTools::ReplaceInstances(dagpath,dagpatharray,results))
		return MS::kFailure;

	MStringArray instances;

	MTools::ConvertDagPathsToNames(results,instances);

	MTools::AddNodesToDisplayLayer(instances,result);

	return MS::kSuccess;
}

