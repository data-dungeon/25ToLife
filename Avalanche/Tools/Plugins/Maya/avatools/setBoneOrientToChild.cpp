#include "stdafx.h"
#include "setBoneOrientToChild.h"

setBoneOrientToChild::setBoneOrientToChild() {}

setBoneOrientToChild::~setBoneOrientToChild() {}

void* setBoneOrientToChild::creator()
{
	return new setBoneOrientToChild();
}

bool setBoneOrientToChild::isUndoable() const
{
    return false;
}

MStatus setBoneOrientToChild::undoIt()
{
    return MS::kSuccess;
}

MStatus setBoneOrientToChild::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus setBoneOrientToChild::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;

	status = MGlobal::getActiveSelectionList(selection);

	if (selection.length()==0)
	{
		MGlobal::displayError("Nothing selected.");
		return MS::kFailure;
	}

	MDagPath dagpath;

	selection.getDagPath(0,dagpath);

	MTools::SetParentJointOrientationToChildBone(dagpath);

	return MS::kSuccess;
}

