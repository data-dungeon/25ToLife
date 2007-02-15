#include "stdafx.h"
#include "removeReference.h"

removeReference::removeReference() {}

removeReference::~removeReference() {}

void* removeReference::creator()
{
	return new removeReference();
}

bool removeReference::isUndoable() const
{
    return false;
}

MStatus removeReference::undoIt()
{
    return MS::kSuccess;
}

MStatus removeReference::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus removeReference::redoIt()
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

	MTools::RemoveReference(dagpath);

	return MS::kSuccess;
}

