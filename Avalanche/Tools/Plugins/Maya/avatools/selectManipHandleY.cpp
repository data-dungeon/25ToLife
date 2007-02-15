#include "stdafx.h"
#include "selectManipHandleY.h"

selectManipHandleY::selectManipHandleY() {}

selectManipHandleY::~selectManipHandleY() {}

void* selectManipHandleY::creator()
{
	return new selectManipHandleY();
}

bool selectManipHandleY::isUndoable() const
{
    return false;
}

MStatus selectManipHandleY::undoIt()
{
    return MS::kSuccess;
}

MStatus selectManipHandleY::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus selectManipHandleY::redoIt()
{
	MDGModifier dgModifier;
	MString currctx,manipctx;

	MGlobal::executeCommand("currentCtx",currctx);
	dgModifier.doIt();

	MGlobal::executeCommand(MString("superCtx -q ")+ currctx,manipctx);
	dgModifier.doIt();

	if (manipctx == "Move") {
		MGlobal::executeCommand(MString("manipMoveContext -e -ah 1 ") + manipctx);
		dgModifier.doIt();
	}
	else if (manipctx == "Rotate") {
		MGlobal::executeCommand(MString("manipRotateContext -e -ah 1 ") + manipctx);
		dgModifier.doIt();
	}
	else if (manipctx == "Scale") {
		MGlobal::executeCommand(MString("manipScaleContext -e -ah 1 ") + manipctx);
		dgModifier.doIt();
	}

    return MS::kSuccess;
}