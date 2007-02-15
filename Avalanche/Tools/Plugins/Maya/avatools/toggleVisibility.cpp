#include "stdafx.h"
#include "toggleVisibility.h"

toggleVisibility::toggleVisibility() {}

toggleVisibility::~toggleVisibility() {}

void* toggleVisibility::creator()
{
	return new toggleVisibility();
}

bool toggleVisibility::isUndoable() const
{
    return true;
}

MStatus toggleVisibility::undoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;
	MStringArray resultarrayUndo;

	resultarrayUndo = resultarray;

	MStringArray splitstring;
	resultarrayUndo[0].split('.',splitstring);

	if (splitstring.length() > 1) {
		toggleComponentsVisibility(resultarrayUndo);
	}
	else {
		toggleObjectsVisibility(resultarrayUndo);
	}

    return MS::kSuccess;
}

MStatus toggleVisibility::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus toggleVisibility::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;
	MStringArray resultarrayRedo;

	MGlobal::executeCommand("ls -sl",resultarrayRedo);
	dgModifier.doIt();

	if (resultarrayRedo.length() == 0) {
		FATAL_ERROR(status,"togglevisibility: Nothing selected.");
	}

	resultarray = resultarrayRedo;

	MStringArray splitstring;
	resultarrayRedo[0].split('.',splitstring);

	if (splitstring.length() > 1) {
		toggleComponentsVisibility(resultarrayRedo);
	}
	else {
		toggleObjectsVisibility(resultarrayRedo);
	}

    return MS::kSuccess;
}

void toggleVisibility::toggleComponentsVisibility(MStringArray &selected) {
	MString panel;
	MDGModifier dgModifier;
	MGlobal::executeCommand("getPanel -wf",panel);
	dgModifier.doIt();

	int isolate_on;
	MGlobal::executeCommand(MString("isolateSelect -q -s ") + panel,isolate_on);
	dgModifier.doIt();

	if (isolate_on == 0) {
		MGlobal::executeCommand(MString("isolateSelect -state 1 ") + panel);
		dgModifier.doIt();
		MGlobal::executeCommand(MString("isolateSelect -as ") + panel);
		dgModifier.doIt();
		MGlobal::executeCommand(MString("isolateSelect -u ") + panel);
		dgModifier.doIt();
	}
	else {
		MGlobal::executeCommand(MString("isolateSelect -state 0 ") + panel);
		dgModifier.doIt();
	}
}


void toggleVisibility::toggleObjectsVisibility(MStringArray &selected) {
	MStringArray splitstring;	
	MString command;
	int vis;
	MDGModifier dgModifier;

	for (unsigned i = 0; i < selected.length(); i++)
	{
		selected[i].split('.',splitstring);

		command = MString("getAttr ") + selected[i] + MString(".visibility");
		MGlobal::executeCommand(command,vis);
		dgModifier.doIt();

		command = MString("setAttr ") + selected[i] + MString(".visibility ") + ( 1 - vis );
		MGlobal::executeCommand(command,vis);
		dgModifier.doIt();
	}

	command = "select -r ";
	for (i = 0; i < selected.length(); i++)
		command += selected[i] + " ";

	MGlobal::executeCommand(command);
	dgModifier.doIt();
}