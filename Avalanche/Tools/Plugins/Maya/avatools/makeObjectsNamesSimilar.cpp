#include "stdafx.h"
#include "makeObjectsNamesSimilar.h"

makeObjectsNamesSimilar::makeObjectsNamesSimilar() {}

makeObjectsNamesSimilar::~makeObjectsNamesSimilar() {}

void* makeObjectsNamesSimilar::creator()
{
	return new makeObjectsNamesSimilar();
}

bool makeObjectsNamesSimilar::isUndoable() const
{
    return false;
}

MStatus makeObjectsNamesSimilar::undoIt()
{
    return MS::kSuccess;
}

MStatus makeObjectsNamesSimilar::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus makeObjectsNamesSimilar::redoIt()
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

	MTools::MakeObjectsNamesSimilar(source,selection0);

	return MS::kSuccess;
}
