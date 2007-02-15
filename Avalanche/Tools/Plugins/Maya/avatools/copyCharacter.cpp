#include "stdafx.h"
#include "copyCharacter.h"

copyCharacter::copyCharacter() {}

copyCharacter::~copyCharacter() {}

void* copyCharacter::creator()
{
	return new copyCharacter();
}

bool copyCharacter::isUndoable() const
{
    return false;
}

MStatus copyCharacter::undoIt()
{
    return MS::kSuccess;
}

MStatus copyCharacter::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus copyCharacter::redoIt()
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

	if (selection.length() == 0)
		return MS::kFailure;

	MDagPath dagpath;

	selection.getDependNode(0,srcobject);
	selection.getDagPath(1,dagpath);

	MTools::CopyCharacter(srcobject,dagpath);
	
	return MS::kSuccess;
}
