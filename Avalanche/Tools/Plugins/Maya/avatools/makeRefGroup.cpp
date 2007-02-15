#include "stdafx.h"
#include "makeRefGroup.h"

makeRefGroup::makeRefGroup() {}

makeRefGroup::~makeRefGroup() {}

void* makeRefGroup::creator()
{
	return new makeRefGroup();
}

bool makeRefGroup::isUndoable() const
{
    return false;
}

MStatus makeRefGroup::undoIt()
{
    return MS::kSuccess;
}

MStatus makeRefGroup::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus makeRefGroup::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"Couldn't get active selection list.");

	if (selection.length()<=0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MDagPath dagpath;
	selection.getDagPath(selection.length()-1,dagpath);

	MObjectArray objects;

	objects.clear();

	for (unsigned i = 0; i < selection.length()-1; i++)
	{
		MObject temp;
		selection.getDependNode(i,temp);
		objects.append(temp);
	}

	MDagPath groupdagpath;
	if (!MTools::MakeReferenceGroup(dagpath,objects,groupdagpath))
		return MS::kFailure;

	MGlobal::executeCommand(MString("select -r ") + groupdagpath.fullPathName());
	dgModifier.doIt();

	return MS::kSuccess;
}

