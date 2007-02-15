#include "stdafx.h"
#include "updateInstances.h"

updateInstances::updateInstances() {}

updateInstances::~updateInstances() {}

void* updateInstances::creator()
{
	return new updateInstances();
}

bool updateInstances::isUndoable() const
{
    return false;
}

MStatus updateInstances::undoIt()
{
    return MS::kSuccess;
}

MStatus updateInstances::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() > 0)
	{
		args.get(0,m_usename);
	}
	else
	{
		m_usename = 0;
	}

	status = redoIt();	

	return status;
}

MStatus updateInstances::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"[updateInstances] Couldn't get active selection list.");

	if (selection.length()<=0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MDagPath dagpath;
	selection.getDagPath(0,dagpath);
	MObject object = dagpath.node();

	MFnDependencyNode fnNode(object);

	MString selectednodename = fnNode.name();

	MTools::UpdateInstances(dagpath,m_usename);

	const char *debug = selectednodename.asChar();

	MGlobal::executeCommand(MString("select -r ")+selectednodename);
	dgModifier.doIt();

	return MS::kSuccess;
}

