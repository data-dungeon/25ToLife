#include "stdafx.h"
#include "makeRefLayer.h"

makeRefLayer::makeRefLayer() {}

makeRefLayer::~makeRefLayer() {}

void* makeRefLayer::creator()
{
	return new makeRefLayer();
}

bool makeRefLayer::isUndoable() const
{
    return false;
}

MStatus makeRefLayer::undoIt()
{
    return MS::kSuccess;
}

MStatus makeRefLayer::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus makeRefLayer::redoIt()
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

	for (unsigned i = 0; i < selection.length()-1; i++)
	{
		MObject temp;
		selection.getDependNode(i,temp);
		objects.append(temp);
	}

	MDagPathArray results;
	MTools::MakeReferenceLayer(dagpath,objects,results);

	MString command = "select -r";

	for (i = 0; i < results.length(); i++)
	{
		MFnDependencyNode fnNode(results[i].node());
		command+= MString(" ") + fnNode.name();
	}
	
	MGlobal::executeCommand(command);
	dgModifier.doIt();

	return MS::kSuccess;
}

