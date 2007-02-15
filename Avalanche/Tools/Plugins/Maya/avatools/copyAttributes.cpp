#include "stdafx.h"
#include "copyAttributes.h"

copyAttributes::copyAttributes() {}

copyAttributes::~copyAttributes() {}

void* copyAttributes::creator()
{
	return new copyAttributes();
}

bool copyAttributes::isUndoable() const
{
    return false;
}

MStatus copyAttributes::undoIt()
{
    return MS::kSuccess;
}

MStatus copyAttributes::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus copyAttributes::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"Couldn't get active selection list.");

	if (selection.length()<=0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MDagPath sourcedagpath,destdagpath;

	selection.getDagPath(selection.length()-1,sourcedagpath);

	for (unsigned i = 0; i < selection.length()-1; i++)
	{
		selection.getDagPath(i,destdagpath);
		
		MTools::CopyHierarchicalAttributes(sourcedagpath,destdagpath);
	}

	return MS::kSuccess;
}

