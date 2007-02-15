#include "stdafx.h"
#include "selectManipHandleX.h"

selectManipHandleX::selectManipHandleX() {}

selectManipHandleX::~selectManipHandleX() {}

void* selectManipHandleX::creator()
{
	return new selectManipHandleX();
}

bool selectManipHandleX::isUndoable() const
{
    return false;
}

MStatus selectManipHandleX::undoIt()
{
    return MS::kSuccess;
}

MStatus selectManipHandleX::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus selectManipHandleX::redoIt()
{
	MDGModifier dgModifier;
	MString currctx,manipctx;

	MGlobal::executeCommand("currentCtx",currctx);
	dgModifier.doIt();

	MGlobal::executeCommand(MString("superCtx -q ")+ currctx,manipctx);
	dgModifier.doIt();

	if (manipctx == "Move") {
		MGlobal::executeCommand(MString("manipMoveContext -e -ah 0 ") + manipctx);
		dgModifier.doIt();
	}
	else if (manipctx == "Rotate") {
		MGlobal::executeCommand(MString("manipRotateContext -e -ah 0 ") + manipctx);
		dgModifier.doIt();
	}
	else if (manipctx == "Scale") {
		MGlobal::executeCommand(MString("manipScaleContext -e -ah 0 ") + manipctx);
		dgModifier.doIt();
	}

    return MS::kSuccess;
}