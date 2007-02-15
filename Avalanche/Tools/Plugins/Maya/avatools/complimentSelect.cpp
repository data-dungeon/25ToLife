#include "stdafx.h"
#include "complimentSelect.h"

complimentSelect::complimentSelect() {}

complimentSelect::~complimentSelect() {}

void* complimentSelect::creator()
{
	return new complimentSelect();
}

bool complimentSelect::isUndoable() const
{
    return false;
}

MStatus complimentSelect::undoIt()
{
    return MS::kSuccess;
}

MStatus complimentSelect::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus complimentSelect::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;
	MStringArray resultarray;

	MGlobal::executeCommand("ls -sl",resultarray);
	dgModifier.doIt();

	if (resultarray.length() == 0) {
		FATAL_ERROR(status,"complimentSelect: No polygons selected.");
	}

	MStringArray splitstring;

	resultarray[0].split('.',splitstring);

	if (splitstring.length() > 1) {
		MGlobal::executeCommand("polySelectConstraint -m 2 -sh true");
		dgModifier.doIt();
		MGlobal::executeCommand("polySelectConstraint -pp 1");
		dgModifier.doIt();
		MGlobal::executeCommand("resetPolySelectConstraint");
		dgModifier.doIt();
	}
	else {
		FATAL_ERROR(status,"complimentSelect: No polygons selected.");
	}

    return MS::kSuccess;
}