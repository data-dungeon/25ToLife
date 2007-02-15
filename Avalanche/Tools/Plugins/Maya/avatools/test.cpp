#include "stdafx.h"
#include "test.h"

test::test() {}

test::~test() {}

void* test::creator()
{
	return new test();
}

bool test::isUndoable() const
{
    return false;
}

MStatus test::undoIt()
{
    return MS::kSuccess;
}

MStatus test::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus test::redoIt()
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
	
	return MS::kSuccess;
}
