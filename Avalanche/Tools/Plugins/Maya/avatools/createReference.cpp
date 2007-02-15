#include "stdafx.h"
#include "createReference.h"

createReference::createReference() {}

createReference::~createReference() {}

void* createReference::creator()
{
	return new createReference();
}

bool createReference::isUndoable() const
{
    return false;
}

MStatus createReference::undoIt()
{
    return MS::kSuccess;
}

MStatus createReference::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() == 2)
	{
		args.get(0,m_uniquetransforms);
		args.get(1,m_createinstance);
	}
	else
	{
		m_uniquetransforms = 1;
		m_createinstance = 1;
	}

	status = redoIt();	

	return status;
}

MStatus createReference::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;

	MString result;
	MGlobal::executeCommand(MString("fileDialog -dm \"*.mb\""),result);
	dgModifier.doIt();
	if (result == "")
		return MS::kSuccess;

	MDagPath instancepath;
	if(!MTools::CreateReference(result,instancepath,m_uniquetransforms ? true : false ,m_createinstance ? true : false))
		return MS::kFailure;

	MFnDependencyNode fnNode(instancepath.node());

	MGlobal::executeCommand(MString("select -r ") + fnNode.name());
	dgModifier.doIt();

	return MS::kSuccess;
}

