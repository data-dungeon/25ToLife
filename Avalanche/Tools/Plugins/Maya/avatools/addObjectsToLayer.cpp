#include "stdafx.h"
#include "addObjectsToLayer.h"

addObjectsToLayer::addObjectsToLayer() {}

addObjectsToLayer::~addObjectsToLayer() {}

void* addObjectsToLayer::creator()
{
	return new addObjectsToLayer();
}

bool addObjectsToLayer::isUndoable() const
{
    return false;
}

MStatus addObjectsToLayer::undoIt()
{
    return MS::kSuccess;
}

MStatus addObjectsToLayer::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() > 0)
		args.get(0,m_noRecurse);
	else
		m_noRecurse = 0;

	status = redoIt();	

	return status;
}

MStatus addObjectsToLayer::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;

	MStringArray selected;

	MGlobal::executeCommand("ls -sl",selected);
	dgModifier.doIt();

	if (selected.length()<=0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MString result;
	MTools::GetCurrentDisplayLayer(result);
	if (result == "")
		return MS::kFailure;

	MTools::AddNodesToDisplayLayer(selected,result);

	return MS::kSuccess;
}

