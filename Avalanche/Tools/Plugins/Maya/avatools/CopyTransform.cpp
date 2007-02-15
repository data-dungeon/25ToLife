#include "stdafx.h"
#include "copyTransform.h"

copyTransform::copyTransform() {}

copyTransform::~copyTransform() {}

void* copyTransform::creator()
{
	return new copyTransform();
}

bool copyTransform::isUndoable() const
{
    return false;
}

MStatus copyTransform::undoIt()
{
    return MS::kSuccess;
}

MStatus copyTransform::doIt( const MArgList& args )
{
	MStatus status;

	int immediateonly = 0;

	if (args.length() > 0)
	{
		args.get(0,immediateonly);
	}

	m_immediateonly = false;

	if (immediateonly)
		m_immediateonly = true;

	status = redoIt();	

	status = redoIt();	

	return status;
}

MStatus copyTransform::redoIt()
{
	MStatus status;
	MDGModifier dgmodifier;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	if ((selection.length()==0)&&(!status))
	{
		MGlobal::displayError("No objects selected.");
		return MS::kFailure;
	}

	MObject srcobject,dstobject;

	if (selection.length() == 0)
		return MS::kFailure;

	MDagPath dagpath1,dagpath2;

	selection.getDagPath(0,dagpath1);
	selection.getDagPath(1,dagpath2);

	if (m_immediateonly)
		MTools::CopyTransform(dagpath1,dagpath2,true,true,true);
	else
		MTools::CopyHierarchicalTransforms(dagpath1,dagpath2);

	return MS::kSuccess;
}
