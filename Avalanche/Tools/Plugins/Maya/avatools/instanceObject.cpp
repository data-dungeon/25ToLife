#include "stdafx.h"
#include "instanceObject.h"

instanceObject::instanceObject() {}

instanceObject::~instanceObject() {}

void* instanceObject::creator()
{
	return new instanceObject();
}

bool instanceObject::isUndoable() const
{
    return false;
}

MStatus instanceObject::undoIt()
{
    return MS::kSuccess;
}

MStatus instanceObject::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() > 0)
		args.get(0,m_uniquetransforms);
	else
		m_uniquetransforms = 0;

	status = redoIt();	

	return status;
}

MStatus instanceObject::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"[updateInstances] Couldn't get active selection list.");

	if (selection.length()<=0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MDagPath dagpath;

	MStringArray selection0;

	for (unsigned i = 0; i < selection.length(); i++)
	{
		selection.getDagPath(i,dagpath);
		
		MDagPath instancedagpath;

		MObject parent = MObject::kNullObj;

		MFnDagNode dagnode(dagpath.node());

		if (!MTools::DoesObjectBelongToReferences(dagpath))
		{
			if (dagnode.parentCount(&status)>0)
			{
				MObject temp = dagnode.parent(0,&status);
				if (temp.hasFn(MFn::kTransform))
				{
					parent = temp;
				}
			}
		}	
		if (m_uniquetransforms)
		{

			if (!MTools::InstanceObject(dagpath,instancedagpath,true,true,parent))
				return MS::kFailure;
		}
		else
		{
			if (!MTools::InstanceObject(dagpath,instancedagpath,false,true,parent))
				return MS::kFailure;
		}
		selection0.append(instancedagpath.fullPathName());
	}
	
	MString command = MString("select -r ") + selection0[0];
	for (i = 0; i < selection0.length(); i++)
		command += MString(" ") + selection0[i];
	MGlobal::executeCommand(command);
	dgModifier.doIt();

	return MS::kSuccess;
}

