#include "stdafx.h"
#include "copyClips.h"

copyClips::copyClips() {}

copyClips::~copyClips() {}

void* copyClips::creator()
{
	return new copyClips();
}

bool copyClips::isUndoable() const
{
    return false;
}

MStatus copyClips::undoIt()
{
    return MS::kSuccess;
}

MStatus copyClips::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus copyClips::redoIt()
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

	MObject srcobject,dstobject;

	if (selection.length() < 2)
		return MS::kFailure;

	selection.getDependNode(0,srcobject);
	selection.getDependNode(1,dstobject);

	MTools::CopyClips(srcobject,dstobject);
	
	return MS::kSuccess;
}
