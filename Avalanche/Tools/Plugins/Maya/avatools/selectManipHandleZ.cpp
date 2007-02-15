#include "stdafx.h"
#include "selectManipHandleZ.h"

selectManipHandleZ::selectManipHandleZ() {}

selectManipHandleZ::~selectManipHandleZ() {}

void* selectManipHandleZ::creator()
{
	return new selectManipHandleZ();
}

bool selectManipHandleZ::isUndoable() const
{
    return false;
}

MStatus selectManipHandleZ::undoIt()
{
    return MS::kSuccess;
}

MStatus selectManipHandleZ::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus selectManipHandleZ::redoIt()
{
	MDGModifier dgModifier;
	MString currctx,manipctx;

	MGlobal::executeCommand("currentCtx",currctx);
	dgModifier.doIt();

	MGlobal::executeCommand(MString("superCtx -q ")+ currctx,manipctx);
	dgModifier.doIt();

	if (manipctx == "Move") {
		MGlobal::executeCommand(MString("manipMoveContext -e -ah 2 ") + manipctx);
		dgModifier.doIt();
	}
	else if (manipctx == "Rotate") {
		MGlobal::executeCommand(MString("manipRotateContext -e -ah 2 ") + manipctx);
		dgModifier.doIt();
	}
	else if (manipctx == "Scale") {
		MGlobal::executeCommand(MString("manipScaleContext -e -ah 2 ") + manipctx);
		dgModifier.doIt();
	}

    return MS::kSuccess;
}