#include "stdafx.h"
#include "rotateUVs.h"

rotateUVs::rotateUVs() {}

rotateUVs::~rotateUVs() {}

void* rotateUVs::creator()
{
	return new rotateUVs();
}

bool rotateUVs::isUndoable() const
{
    return true;
}

MStatus rotateUVs::undoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;

	MStringArray selUVsUndo;

	selUVsUndo = selUVs;

	command = (MString("select -r "));
	for (unsigned i = 0; i < selUVsUndo.length(); i++)
		command += selUVsUndo[i] + " ";

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	if (ui_rotclockwise)
		command = (MString("polyEditUV -pu 0.5 -pv 0.5 -a -90 "));
	else
		command  = (MString("polyEditUV -pu 0.5 -pv 0.5 -a 90 "));

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	// RESELECT THE POLYS
	command = (MString("select -r "));
	for (i = 0; i < selFaces.length(); i++)
		command += selFaces[i] + " ";

	MGlobal::executeCommand(command);
	dgModifier.doIt();

    return MS::kSuccess;
}

MStatus rotateUVs::doIt( const MArgList& args )
{
	MStatus status;

	bool rotclockwise = true;

	if (args.length() > 0)
		args.get(0,rotclockwise);

	ui_rotclockwise = rotclockwise;

	status = redoIt();	

	return status;
}

MStatus rotateUVs::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"tileUVs: Couldn't get active selection list.");

	MString result;
	MStringArray selFacesRedo,selUVsRedo;

	command = MString("filterExpand -sm 34 -ex true");
	MGlobal::executeCommand(command,selFacesRedo);
	dgModifier.doIt();
	if (selFacesRedo.length() == 0) {
		FATAL_ERROR(status,"rotateUVs: No polygons selected.");
	}

	selFaces = selFacesRedo;

	command = (MString("polyListComponentConversion -ff -tuv "));

	for (unsigned i = 0; i < selFacesRedo.length(); i++)
		command += selFacesRedo[i] + " ";

	MGlobal::executeCommand(command,selUVsRedo);
	dgModifier.doIt();

	selUVs = selUVsRedo;

	command = (MString("select -r "));
	for (i = 0; i < selUVsRedo.length(); i++)
		command += selUVsRedo[i] + " ";

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	if (ui_rotclockwise)
		command = (MString("polyEditUV -pu 0.5 -pv 0.5 -a 90 "));
	else
		command  = (MString("polyEditUV -pu 0.5 -pv 0.5 -a -90 "));

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	command = (MString("select -r "));
	for (i = 0; i < selFacesRedo.length(); i++)
		command += selFacesRedo[i] + " ";


	MGlobal::executeCommand(command);
	dgModifier.doIt();

    return MS::kSuccess;
}