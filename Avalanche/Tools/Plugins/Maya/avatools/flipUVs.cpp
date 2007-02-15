#include "stdafx.h"
#include "flipUVs.h"

flipUVs::flipUVs() {}

flipUVs::~flipUVs() {}

void* flipUVs::creator()
{
	return new flipUVs();
}

bool flipUVs::isUndoable() const
{
    return true;
}

MStatus flipUVs::undoIt()
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

	if (ui_udirection)
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -su -1"));
	else
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -sv -1"));

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

MStatus flipUVs::doIt( const MArgList& args )
{
	MStatus status;

	bool udirection = true;

	if (args.length() > 0)
		args.get(0,udirection);

	ui_udirection = udirection;

	status = redoIt();	

	return status;
}

MStatus flipUVs::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"flipUVs: Couldn't get active selection list.");

	MString result;
	MStringArray selFacesRedo,selUVsRedo;

	command = MString("filterExpand -sm 34 -ex true");
	MGlobal::executeCommand(command,selFacesRedo);
	dgModifier.doIt();
	if (selFacesRedo.length() == 0) {
		FATAL_ERROR(status,"flipUVs: No polygons selected.");
	}

	selFaces = selFacesRedo;

	command = (MString("polyListComponentConversion -ff -tuv "));

	for (unsigned i = 0; i < selFacesRedo.length(); i++)
		command += selFacesRedo[i] + " ";

	MGlobal::executeCommand(command,selUVsRedo);
	dgModifier.doIt();

	command = (MString("select -r "));
	for (i = 0; i < selUVsRedo.length(); i++)
		command += selUVsRedo[i] + " ";

	selUVs = selUVsRedo;

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	if (ui_udirection)
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -su -1"));
	else
		command = (MString("polyEditUV -pu 0.0 -pv 0.0 -sv -1"));

	MGlobal::executeCommand(command);
	dgModifier.doIt();

	command = (MString("select -r "));
	for (i = 0; i < selFacesRedo.length(); i++)
		command += selFacesRedo[i] + " ";


	MGlobal::executeCommand(command);
	dgModifier.doIt();

    return MS::kSuccess;
}
