#include "stdafx.h"
#include "pasteKeys.h"

pasteKeys::pasteKeys() {}

pasteKeys::~pasteKeys()
{
}

void* pasteKeys::creator()
{
	return new pasteKeys();
}

bool pasteKeys::isUndoable() const
{
    return false;
}

MStatus pasteKeys::undoIt()
{
    return MS::kSuccess;
}

MStatus pasteKeys::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus pasteKeys::redoIt()
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

	MTools::PasteKeysFromClipboard(dagpath);

	return MS::kSuccess;
}

