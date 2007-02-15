#include "stdafx.h"
#include "replaceInstances.h"

replaceInstances::replaceInstances() {}

replaceInstances::~replaceInstances() {}

void* replaceInstances::creator()
{
	return new replaceInstances();
}

bool replaceInstances::isUndoable() const
{
    return false;
}

MStatus replaceInstances::undoIt()
{
    return MS::kSuccess;
}

MStatus replaceInstances::doIt( const MArgList& args )
{
	MStatus status;

	int removeold = 1;
	int copy = 1;
	int uniquenames = 1;

	if (args.length() > 0)
		args.get(0,copy);
	if (args.length() > 1)
		args.get(1,uniquenames);
	if (args.length() > 2)
		args.get(2,removeold);

	m_removeold = false;
	if (removeold)
		m_removeold = true;

	m_copy = false;
	if (copy)
		m_copy = true;

	m_uniquenames = false;
	if (uniquenames)
		m_uniquenames = true;

	status = redoIt();	

	return status;
}

// First object selected is master.
MStatus replaceInstances::redoIt()
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

	MDagPathArray dagpatharray;
	dagpatharray.clear();
	MDagPath source;

	for (unsigned i = 0; i < selection.length(); i++)
	{
		MDagPath dagpath;
		selection.getDagPath(i,dagpath);

		if (i == (selection.length()-1))
		{
			source = dagpath;	
		}
		else
		{
			dagpatharray.append(dagpath);
		}
	}

	MFnDependencyNode fnNode(source.node());

	const char *debug = fnNode.name().asChar();

	MDagPathArray results;

	if (!m_copy)
	{
		if (!MTools::ReplaceInstances(source,dagpatharray,results))
			return MS::kFailure;
	}
	else
	{
		if (!MTools::ReplaceInstances(source,dagpatharray,results,true,m_removeold,m_uniquenames))
			return MS::kFailure;
	}
	
	return MS::kSuccess;
}

