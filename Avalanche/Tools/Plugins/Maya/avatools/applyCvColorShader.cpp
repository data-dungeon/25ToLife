#include "stdafx.h"
#include "applyCvColorShader.h"
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>


applyCvColorShader::applyCvColorShader() {}

applyCvColorShader::~applyCvColorShader() {}

void* applyCvColorShader::creator()
{
	return new applyCvColorShader();
}

bool applyCvColorShader::isUndoable() const
{
    return false;
}

MStatus applyCvColorShader::undoIt()
{
    return MS::kSuccess;
}

MStatus applyCvColorShader::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

static void BreakConnection(const MString &sourcePlugName,const MString &destPlugName)
{
	MDGModifier dgmodifier;
	
	MGlobal::executeCommand (
		MString ("disconnectAttr ") +
		sourcePlugName + MString (" ") + destPlugName);
	dgmodifier.doIt();
}

static void AddConnection(const MString &sourcePlugName,const MString &destPlugName)
{
	MDGModifier dgmodifier;

	MGlobal::executeCommand (
		MString ("defaultNavigation -quiet true -force true -connectToExisting -source ") +
		sourcePlugName + MString (" -destination ") + destPlugName);
	dgmodifier.doIt();
}

static void MoveConnections(MPlug &plug,const MString &destPlugName,bool asDst,bool asSource)
{
	MStatus status;
	MPlugArray connections;
	// move all connections to the color plug to input1 on the multiplyDivideColor
	plug.connectedTo(connections,asDst,asSource,&status);
	if (!status)
		return;

	int numConnections = connections.length();
	for (int connectionIndex=0;connectionIndex<numConnections;connectionIndex++)
	{
		MString connectionName = connections[connectionIndex].name();
//		if (asDst)
//			BreakConnection(connectionName,plug.name());
//		if (asSource)
//			BreakConnection(plug.name(),connectionName);
		AddConnection(connectionName,destPlugName);
	}
}

MStatus applyCvColorShader::redoIt()
{
	MStatus status;
	MDGModifier dgmodifier;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	if ((selection.length()==0)&&(!status))
	{
		MGlobal::displayError("No objects selected.");
		return MS::kFailure;
	}

	for (int i = 0; i < (int)selection.length(); i++)
	{
		MDagPath dagpath;

		selection.getDagPath(i,dagpath);

		if ((!dagpath.hasFn(MFn::kMesh))&&(dagpath.hasFn(MFn::kTransform)))
			continue;

		int instanceIndex = dagpath.instanceNumber();

		MFnDependencyNode depnode(dagpath.node());
		const char *debug = depnode.name().asChar();

		MFnMesh fnMesh(dagpath, &status);
		if (!status)
			return MS::kFailure;
		
		MIntArray keys;
		MObjectArray shadinggroups;

		fnMesh.getConnectedShaders(instanceIndex,shadinggroups,keys);
		
		for (int j = 0; j < (int)shadinggroups.length(); j++)
		{
			MFnDependencyNode sgdepnode(shadinggroups[j]);

			const char *debug = sgdepnode.name().asChar();

			MPlug surfaceshaderplug = sgdepnode.findPlug("surfaceShader",&status);
			if (!status)
				continue;

			MPlugArray connected;
			surfaceshaderplug.connectedTo(connected,true,false,&status);
			if (!status)
				continue;

			if (connected.length()==0)
				continue;

			MObject shadernode = connected[0].node();

			MFnDependencyNode shaderdepnode(shadernode);
			MString shaderDepNodeName = shaderdepnode.name();

			MPlug colorPlug = shaderdepnode.findPlug("color",&status);
			if (!status)
				continue;

			MPlug incandescencePlug = shaderdepnode.findPlug("incandescence",&status);
			if (!status)
				continue;

			MPlug transparencyPlug = shaderdepnode.findPlug("transparency",&status);
			if (!status)
				continue;

			MString cvcolorshadername;
			MGlobal::executeCommand("shadingNode -asUtility cvColorShader",cvcolorshadername);
			dgmodifier.doIt();
			
			MString multiplyDivideColorName;
			MGlobal::executeCommand("shadingNode -asUtility multiplyDivide",multiplyDivideColorName);
			dgmodifier.doIt();
			
			MString multiplyDivideAlphaName;
			MGlobal::executeCommand("shadingNode -asUtility multiplyDivide",multiplyDivideAlphaName);
			dgmodifier.doIt();

			// move color connections to multiplydivide color
			MoveConnections(colorPlug,multiplyDivideColorName + MString(".input1"),true,false);
			// move transparency connections to multiplydivide alpha
			MoveConnections(transparencyPlug,multiplyDivideAlphaName + MString(".input1"),true,false);
			// add connection from cvcolorshader to the multiply divide color
			AddConnection(cvcolorshadername + MString(".outColor"),multiplyDivideColorName + MString(".input2"));
			// add connection from cvcolorshader to the multiply divide alpha
			AddConnection(cvcolorshadername + MString(".outAlpha"),multiplyDivideAlphaName + MString(".input2X"));
			AddConnection(cvcolorshadername + MString(".outAlpha"),multiplyDivideAlphaName + MString(".input2Y"));
			AddConnection(cvcolorshadername + MString(".outAlpha"),multiplyDivideAlphaName + MString(".input2Z"));
			// add connection from the multiply divide color to the incandescence
			AddConnection(multiplyDivideColorName + MString(".output"),shaderDepNodeName + MString(".incandescence"));
			// add connection from the multiply divide alpha to the transparency
			AddConnection(multiplyDivideAlphaName + MString(".output"),shaderDepNodeName + MString(".transparency"));
		}
	}
	return MS::kSuccess;
}
