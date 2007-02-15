#include "stdafx.h"
#include "copyKeys.h"

copyKeys::copyKeys() {}

copyKeys::~copyKeys()
{
}

void* copyKeys::creator()
{
	return new copyKeys();
}

bool copyKeys::isUndoable() const
{
    return false;
}

MStatus copyKeys::undoIt()
{
    return MS::kSuccess;
}

MStatus copyKeys::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus copyKeys::redoIt()
{
	MStatus status;
	MDGModifier dgmodifier;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	if ((selection.length()==0)&&(!status))
	{
		MGlobal::displayError("No objects selected.");
		return MS::kFailure;
	}

	int numselections = selection.length();
	
	if (selection.length() == 0)
		return MS::kFailure;

	MDagPath dagpath;

	selection.getDagPath(0,dagpath);

	MObject object;
	selection.getDependNode(0,object);

	MTools::CopyKeysToClipboard(dagpath);

	return MS::kSuccess;
}

