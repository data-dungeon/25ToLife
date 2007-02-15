#include "stdafx.h"
#include "makeObjectsNamesDifferent.h"

makeObjectsNamesDifferent::makeObjectsNamesDifferent() {}

makeObjectsNamesDifferent::~makeObjectsNamesDifferent() {}

void* makeObjectsNamesDifferent::creator()
{
	return new makeObjectsNamesDifferent();
}

bool makeObjectsNamesDifferent::isUndoable() const
{
    return false;
}

MStatus makeObjectsNamesDifferent::undoIt()
{
    return MS::kSuccess;
}

MStatus makeObjectsNamesDifferent::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus makeObjectsNamesDifferent::redoIt()
{
	MStatus status;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);

	if (selection.length()==0)
	{
		MGlobal::displayError("Nothing selected.");
		return MS::kFailure;
	}

	MDagPath source;

	selection.getDagPath(0,source);

	MSelectionList selection0;

	for (int i = 1; i < (int)selection.length(); i++)
	{
		MDagPath dagpath;

		selection.getDagPath(i,dagpath);
		selection0.add(dagpath);
	}

	MTools::MakeObjectsNamesDifferent(source,selection0);

	return MS::kSuccess;
}
