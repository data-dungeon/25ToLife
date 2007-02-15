#include "setFarClipToBBox.h"
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>
#include <maya/MString.h>

#include <iostream.h>
#include "macros.h"

setFarClipToBBox::setFarClipToBBox() {}

setFarClipToBBox::~setFarClipToBBox() {}

void* setFarClipToBBox::creator()
{
	return new setFarClipToBBox();
}

bool setFarClipToBBox::isUndoable() const
{
    return false;
}

MStatus setFarClipToBBox::undoIt()
{
    return MS::kSuccess;
}

MStatus setFarClipToBBox::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus setFarClipToBBox::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;
	MStringArray selected;

	MGlobal::executeCommand("ls -sl",selected);
	dgModifier.doIt();

	if (selected.length() == 0) {
		FATAL_ERROR(status,"setFarClipToBBox: Nothing selected.");
	}

	double max=0.0f;
	double sizeX=0.0f,sizeY=0.0f,sizeZ=0.0f,size=0.0f;

	for (unsigned i = 0; i < selected.length(); i++) {

		command = MString("getAttr ") + selected[i] + MString(".boundingBox.boundingBoxSize.boundingBoxSizeX");
		MGlobal::executeCommand(command,sizeX);
		dgModifier.doIt();
		command = MString("getAttr ") + selected[i] + MString(".boundingBox.boundingBoxSize.boundingBoxSizeY");
		MGlobal::executeCommand(command,sizeY);
		dgModifier.doIt();
		command = MString("getAttr ") + selected[i] + MString(".boundingBox.boundingBoxSize.boundingBoxSizeZ");
		MGlobal::executeCommand(command,sizeZ);
		dgModifier.doIt();
		
		max = 0.0f;
		if (sizeX > max)
			max = sizeX;
		if (sizeY > max)
			max = sizeY;
		if (sizeZ > max)
			max = sizeZ;
		size += max;
	}
	size += size * 2.0f + 100.0f;

	command = MString("setAttr perspShape.farClipPlane ") + size;
	MGlobal::executeCommand(command);
	dgModifier.doIt();		

	command = MString("setAttr topShape.farClipPlane ") + size;
	MGlobal::executeCommand(command);
	dgModifier.doIt();		

	command = MString("setAttr frontShape.farClipPlane ") + size;
	MGlobal::executeCommand(command);
	dgModifier.doIt();	

	command = MString("setAttr sideShape.farClipPlane ") + size;
	MGlobal::executeCommand(command);
	dgModifier.doIt();	

    return MS::kSuccess;
}
