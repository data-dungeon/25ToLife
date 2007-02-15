#include "stdafx.h"
#include "highlightShader.h"

highlightShader::highlightShader() {}

highlightShader::~highlightShader() {}

void* highlightShader::creator()
{
	return new highlightShader();
}

bool highlightShader::isUndoable() const
{
    return false;
}

MStatus highlightShader::undoIt()
{
    return MS::kSuccess;
}

MStatus highlightShader::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus highlightShader::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;
	MStringArray selected;

	MGlobal::executeCommand("ls -sl",selected);
	dgModifier.doIt();

	if (selected.length() == 0) {
		FATAL_ERROR(status,"highlightShader: Nothing selected.");
	}

	MStringArray SG;
	
	getSelectionShadingGroups(selected,SG);
	highlightMultilister(SG);

    return MS::kSuccess;
}

void highlightShader::getSelectionShadingGroups(MStringArray &selection, MStringArray &SG) {
	MStringArray setlist,shadersets;
	MDGModifier dgModifier;

	MGlobal::executeCommand("ls -sets",setlist);
	dgModifier.doIt();

	unsigned i;
	MString objType;

	for (i = 0; i < setlist.length(); i++) {
		MGlobal::executeCommand(MString("objectType ") + setlist[i],objType);
		dgModifier.doIt();
		if (objType == "shadingEngine")
			shadersets.append(setlist[i]);
	}

	SG.clear();

	int isComponent = false,memberexists = false;
	MStringArray groups,splitstring,shapes;

	for (i = 0; i < selection.length(); i++) {
		selection[i].split('.',splitstring);
		if (splitstring.length() > 1)
			isComponent = true;
		for (unsigned j = 0; j < shadersets.length(); j++) {
			if (isComponent) {
				MGlobal::executeCommand(MString("sets -im ") + shadersets[j] + " " + selection[i],memberexists);
				dgModifier.doIt();
				if (memberexists)
					SG.append(shadersets[j]);
			}
			else {
				MGlobal::executeCommand(MString("nodeType ") + selection[i],objType);
				dgModifier.doIt();

				if (objType == "transform") {
					MGlobal::executeCommand(MString("listRelatives -shapes ") + selection[i],shapes);
					dgModifier.doIt();
					if (shapes.length() == 0)
						continue;

					MGlobal::executeCommand(MString("sets -im ") + shadersets[j] + " " + selection[i],memberexists);
					dgModifier.doIt();
					if (memberexists)
						SG.append(shadersets[j]);
				}
				else if (objType == "nurbsSurface" || objType == "mesh") {
					MGlobal::executeCommand(MString("sets -im ") + shadersets[j] + " " + selection[i],memberexists);
					dgModifier.doIt();
					if (memberexists)
						SG.append(shadersets[j]);
				}
			}
		}
	}
}

MStatus highlightShader::highlightMultilister(MStringArray &SG) {
	MStatus status;
	MDGModifier dgModifier;
	MStringArray scriptPanels;
	MGlobal::executeCommand("getPanel -scriptType \"multiListerPanel\"",scriptPanels);
	dgModifier.doIt();

	if ( scriptPanels.length() > 1 ) {
		FATAL_ERROR(status, "More than one active multiListerPanel!" );
	}

	MString nameRoot = scriptPanels[0],topLayout;
	
	MGlobal::executeCommand(MString("scriptedPanel -q -control ") + nameRoot,topLayout);
	dgModifier.doIt();

	topLayout += MString("|") + nameRoot;

	MString theLister;

	MGlobal::executeCommand(MString("findActiveLister ") + topLayout + MString(" ") + nameRoot,theLister);
	dgModifier.doIt();

	int listerexists;
	MGlobal::executeCommand(MString("listerEditor -exists ") + theLister,listerexists);
	dgModifier.doIt();

	if (listerexists) {
		int inSelectMode;
		MGlobal::executeCommand(MString("listerEditor -q -inSelectMode ") + theLister,inSelectMode);
		dgModifier.doIt();

		if (inSelectMode == 0) {
			MGlobal::executeCommand(MString("listerEditor -e -chl ") + theLister);
			dgModifier.doIt();

			for (unsigned i = 0; i < SG.length(); i++) {
				MGlobal::executeCommand(MString("listerEditor -e -hl ") + SG[i] + " " + theLister);
				dgModifier.doIt();
			}
		}
	}
	return MS::kSuccess;
}
