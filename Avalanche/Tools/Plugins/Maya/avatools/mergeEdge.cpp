#include "stdafx.h"
#include "mergeEdge.h"

mergeEdge::mergeEdge() {}

mergeEdge::~mergeEdge() {}

void* mergeEdge::creator()
{
	return new mergeEdge();
}

bool mergeEdge::isUndoable() const
{
    return false;
}

MStatus mergeEdge::undoIt()
{
    return MS::kSuccess;
}

MStatus mergeEdge::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus mergeEdge::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;

	MStringArray verts, edges;

	MGlobal::executeCommand("ls -sl",edges);
	dgModifier.doIt();

	if (edges.length() == 0) {
		FATAL_ERROR(status,"edgeMerge: Nothing selected.");
	}

	MGlobal::executeCommand("PolySelectConvert 3");
	dgModifier.doIt();

	MGlobal::executeCommand("ls -sl",verts);
	dgModifier.doIt();

	MString command;

	command = "polyMergeVertex -ch 1 -tx 1 -d 0.01 ";

	for (int i = 0; i < (int)verts.length(); i++)
	{
		command += verts[i] + " ";
	}

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	command = "select -r ";

	for (i = 0; i < (int)edges.length(); i++)
	{
		command += edges[i] + " ";
	}

	MGlobal::executeCommand(command);
	dgModifier.doIt();

    return MS::kSuccess;
}
