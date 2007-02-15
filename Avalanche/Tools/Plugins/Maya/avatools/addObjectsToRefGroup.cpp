#include "stdafx.h"
#include "addObjectsToRefGroup.h"

addObjectsToRefGroup::addObjectsToRefGroup() {}

addObjectsToRefGroup::~addObjectsToRefGroup() {}

void* addObjectsToRefGroup::creator()
{
	return new addObjectsToRefGroup();
}

bool addObjectsToRefGroup::isUndoable() const
{
    return false;
}

MStatus addObjectsToRefGroup::undoIt()
{
    return MS::kSuccess;
}

MStatus addObjectsToRefGroup::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus addObjectsToRefGroup::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;

	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	if (selection.length()==0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MObject group;
	selection.getDependNode(selection.length()-1,group);

	if (!MTools::IsRefGroup(group))
	{
		DISPLAY_ERROR("Last selected object needs to be an Instance Group.");
		return MS::kFailure;
	}

	MString refname;
	MTools::GetShapeReferenceFromReferenceGroup(group,refname);
	MObject refobject = MTools::GetMObject(refname);

	MObjectArray objects;

	MDagPathArray dagpatharray;
	dagpatharray.clear();

	for (unsigned i = 0; i < selection.length()-1; i++)
	{
		MDagPath dagpath;
		selection.getDagPath(i,dagpath);
		dagpatharray.append(dagpath);
	}

	MDagPath dagpath;
	MFnDagNode dagnode(refobject);
	dagnode.getPath(dagpath);

	MDagPathArray results;

	MTools::ReplaceInstances(dagpath,dagpatharray,results);

	MTools::AddNodesToInstanceGroup(results,group);

	MFnDependencyNode groupdepnode(group);

	MGlobal::executeCommand(MString("select -r ") + groupdepnode.name());
	dgModifier.doIt();

	return MS::kSuccess;
}

