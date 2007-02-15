#include "makeGeomConsolidateGroup.h"

#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>
#include <maya/MString.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFileIO.h>
#include <maya/MItDag.h>
#include <maya/MDagModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>
#include <maya/MFnTransform.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MDagPathArray.h>
#include <maya/MObjectArray.h>
#include <maya/MArgList.h>

#include "../../libraries/MTools/MTools.h"

#include <iostream.h>
#include "macros.h"

makeGeomConsolidateGroup::makeGeomConsolidateGroup() {}

makeGeomConsolidateGroup::~makeGeomConsolidateGroup() {}

void* makeGeomConsolidateGroup::creator()
{
	return new makeGeomConsolidateGroup();
}

bool makeGeomConsolidateGroup::isUndoable() const
{
    return false;
}

MStatus makeGeomConsolidateGroup::undoIt()
{
    return MS::kSuccess;
}

MStatus makeGeomConsolidateGroup::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus makeGeomConsolidateGroup::redoIt()
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


	MObjectArray objects;

	objects.clear();

	for (unsigned i = 0; i < selection.length(); i++)
	{
		MObject temp;
		selection.getDependNode(i,temp);
		objects.append(temp);
	}

	MTools::MakeGeomConsolidationGroup(objects);

	return MS::kSuccess;
}

