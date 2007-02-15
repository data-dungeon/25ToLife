#include "stdafx.h"
#include "updateReference.h"

updateReference::updateReference() {}

updateReference::~updateReference() {}

void* updateReference::creator()
{
	return new updateReference();
}

bool updateReference::isUndoable() const
{
    return false;
}

MStatus updateReference::undoIt()
{
    return MS::kSuccess;
}

MStatus updateReference::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus updateReference::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"[updateInstances] Couldn't get active selection list.");

	if (selection.length()<=0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MDagPath dagpath;
	selection.getDagPath(0,dagpath);

	MTools::ReloadReference(dagpath);

	return MS::kSuccess;
}

