#include "stdafx.h"
#include "goToBindPose.h"
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>

goToBindPose::goToBindPose() {}

goToBindPose::~goToBindPose() {}

void* goToBindPose::creator()
{
	return new goToBindPose();
}

bool goToBindPose::isUndoable() const
{
    return false;
}

MStatus goToBindPose::undoIt()
{
    return MS::kSuccess;
}

MStatus goToBindPose::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}


MStatus goToBindPose::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgmodifier;

	status = MGlobal::getActiveSelectionList(selection);

	if (selection.length()==0)
	{
		MGlobal::displayError("Nothing selected.");
		return MS::kFailure;
	}
	
	MDagPath dagpath,rootjointdagpath;
	MObject object;
	selection.getDagPath(0,dagpath);
	selection.getDependNode(0,object);

	if (object.apiType() == MFn::kCharacter)
	{
		MTools::GetRootBoneFromCharacter(object,rootjointdagpath);
	}
	else
	{
		MTools::GetRootBone(dagpath,rootjointdagpath);
	}

	// set character to bindpose
	MObject bindposeobject;
	MTools::GetBindPoseFromJoint(rootjointdagpath,bindposeobject);
	MFnDependencyNode bindposedepnode(bindposeobject);
	const char *bindposedebug = bindposedepnode.name().asChar();

	MString bindPoseName = bindposedepnode.name();
	MGlobal::executeCommand(MString("dagPose -restore ") + bindPoseName);
	dgmodifier.doIt();


	return MS::kSuccess;
}
