#include "showRefOnly.h"
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>
#include <maya/MString.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
//#include <maya/MFileIO.h>
//#include <maya/MItDag.h>
//#include <maya/MDagModifier.h>
//#include <maya/MPlugArray.h>
//#include <maya/MPlug.h>
//#include <maya/MFnTransform.h>
//#include <maya/MTransformationMatrix.h>

#include "reflib.h"

#include <iostream.h>
#include "macros.h"

showRefOnly::showRefOnly() {}

showRefOnly::~showRefOnly() {}

void* showRefOnly::creator()
{
	return new showRefOnly();
}

bool showRefOnly::isUndoable() const
{
    return false;
}

MStatus showRefOnly::undoIt()
{
    return MS::kSuccess;
}

MStatus showRefOnly::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus showRefOnly::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"[showRefOnly] Couldn't get active selection list.");

	if (selection.length()<=0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MDagPath dagpath;
	selection.getDagPath(0,dagpath);
	MObject object = dagpath.node();

	MFnDependencyNode fnNode(object);

	MString selectednodename = fnNode.name();

	refLib::ShowRefOnly(dagpath);

	const char *debug = selectednodename.asChar();

	MGlobal::executeCommand(MString("select -r ")+selectednodename);
	dgModifier.doIt();

	return MS::kSuccess;
}

