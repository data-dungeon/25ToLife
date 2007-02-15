#include "stdafx.h"
#include "selectManipHandleView.h"

selectManipHandleView::selectManipHandleView() {}

selectManipHandleView::~selectManipHandleView() {}

void* selectManipHandleView::creator()
{
	return new selectManipHandleView();
}

bool selectManipHandleView::isUndoable() const
{
    return false;
}

MStatus selectManipHandleView::undoIt()
{
    return MS::kSuccess;
}

MStatus selectManipHandleView::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus selectManipHandleView::redoIt()
{
	MDGModifier dgModifier;
	MString currctx,manipctx;

	MGlobal::executeCommand("currentCtx",currctx);
	dgModifier.doIt();

	MGlobal::executeCommand(MString("superCtx -q ")+ currctx,manipctx);
	dgModifier.doIt();

	if (manipctx == "Move") {
		MGlobal::executeCommand(MString("manipMoveContext -e -ah 3 ") + manipctx);
		dgModifier.doIt();
	}
	else if (manipctx == "Rotate") {
		MGlobal::executeCommand(MString("manipRotateContext -e -ah 3 ") + manipctx);
		dgModifier.doIt();
	}
	else if (manipctx == "Scale") {
		MGlobal::executeCommand(MString("manipScaleContext -e -ah 3 ") + manipctx);
		dgModifier.doIt();
	}

    return MS::kSuccess;
}