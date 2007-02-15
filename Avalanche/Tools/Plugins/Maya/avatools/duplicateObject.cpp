#include "stdafx.h"
#include "duplicateObject.h"

duplicateObject::duplicateObject() {}

duplicateObject::~duplicateObject() {}

void* duplicateObject::creator()
{
	return new duplicateObject();
}

bool duplicateObject::isUndoable() const
{
    return false;
}

MStatus duplicateObject::undoIt()
{
    return MS::kSuccess;
}

MStatus duplicateObject::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() > 0)
		args.get(0,m_uniquetransforms);
	else
		m_uniquetransforms = 0;

	status = redoIt();	

	return status;
}

MStatus duplicateObject::redoIt()
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
		MDagPath copydagpath;
		if(!MTools::DuplicateObject(dagpath,copydagpath,true,parent))
			return MS::kFailure;
		
		selection0.append(copydagpath.fullPathName());
	}

	MString command = MString("select -r ") + selection0[0];
	for (i = 0; i < selection0.length(); i++)
		command += MString(" ") + selection0[i];
	MGlobal::executeCommand(command);
	dgModifier.doIt();

	return MS::kSuccess;
}

