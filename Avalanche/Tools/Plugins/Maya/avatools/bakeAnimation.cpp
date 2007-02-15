#include "stdafx.h"
#include "bakeAnimation.h"
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>

bakeAnimation::bakeAnimation() {}

bakeAnimation::~bakeAnimation() {}

void* bakeAnimation::creator()
{
	return new bakeAnimation();
}

bool bakeAnimation::isUndoable() const
{
    return false;
}

MStatus bakeAnimation::undoIt()
{
    return MS::kSuccess;
}

MStatus bakeAnimation::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}


MStatus bakeAnimation::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgmodifier;

	status = MGlobal::getActiveSelectionList(selection);

	if (selection.length()==0)
	{
		MGlobal::displayError("Nothing selected.");
		return MS::kFailure;
	}
	
	MDagPath dagpath;

	selection.getDagPath(0,dagpath);

	MStringArray duplicate,timerange;

	MGlobal::executeCommand(MString("duplicate ") + dagpath.fullPathName(),duplicate);

	MStringArray pointconstraint,orientconstraint,scaleconstraint;

	MGlobal::executeCommand(MString("pointConstraint -weight 1 ") + dagpath.fullPathName() + MString(" ") + duplicate[0],pointconstraint);
	dgmodifier.doIt();
	MGlobal::executeCommand(MString("orientConstraint -weight 1 ") + dagpath.fullPathName() + MString(" ") + duplicate[0],orientconstraint);
	dgmodifier.doIt();
	MGlobal::executeCommand(MString("scaleConstraint -weight 1 ") + dagpath.fullPathName() + MString(" ") + duplicate[0],scaleconstraint);
	dgmodifier.doIt();

	MGlobal::executeCommand("timeControl -q -range $gPlayBackSlider",timerange);
	dgmodifier.doIt();

	MGlobal::executeCommand(MString("bakeResults -simulation true -t ") + timerange[0] + MString(" ") + dagpath.fullPathName() + MString(" {\"") + duplicate[0] + MString("\"}"));
	dgmodifier.doIt();

	return MS::kSuccess;
}
