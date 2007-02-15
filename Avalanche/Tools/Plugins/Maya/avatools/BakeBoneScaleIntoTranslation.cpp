#include "stdafx.h"
#include "bakeBoneScaleIntoTranslation.h"

bakeBoneScaleIntoTranslation::bakeBoneScaleIntoTranslation() {}

bakeBoneScaleIntoTranslation::~bakeBoneScaleIntoTranslation() {}

void* bakeBoneScaleIntoTranslation::creator()
{
	return new bakeBoneScaleIntoTranslation();
}

bool bakeBoneScaleIntoTranslation::isUndoable() const
{
    return false;
}

MStatus bakeBoneScaleIntoTranslation::undoIt()
{
    return MS::kSuccess;
}

MStatus bakeBoneScaleIntoTranslation::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus bakeBoneScaleIntoTranslation::redoIt()
{
	MStatus status;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);

	if (selection.length()==0)
	{
		MGlobal::displayError("Nothing selected.");
		return MS::kFailure;
	}

	MDagPath dagpath;

	selection.getDagPath(0,dagpath);

	MTools::BakeBoneScaleIntoTranslation(dagpath);

	return MS::kSuccess;
}
