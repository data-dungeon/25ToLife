#include "stdafx.h"
#include "bakeTransform.h"

bakeTransform::bakeTransform() {}

bakeTransform::~bakeTransform() {}

void* bakeTransform::creator()
{
	return new bakeTransform();
}

bool bakeTransform::isUndoable() const
{
    return false;
}

MStatus bakeTransform::undoIt()
{
    return MS::kSuccess;
}

MStatus bakeTransform::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus bakeTransform::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;

	status = MGlobal::getActiveSelectionList(selection);
	if (selection.length()<=0)
	{
		MGlobal::displayError("No objects selected.");
		return MS::kFailure;
	}
	
	MDagPath dagpath;

	selection.getDagPath(0,dagpath);

	MTools::BakeTransformAndCenterPivot(dagpath);

	return MS::kSuccess;
}

