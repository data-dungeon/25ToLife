/****************************************************************************

$Workfile: pluginMain.cpp $

$Archive: /Avalanche/tools/Plugins/Maya/objAttrib/pluginMain.cpp $

$Author: Adam_Clayton $

$Date: 2005/08/29 $

CONFIDENTIAL AND PROPRIETARY INFORMATION

Copyright © Avalanche Software 2002

All rights reserved.

****************************************************************************/
// pluginMain.cpp : implementation file

// precomp'ed header files
#include "pluginMainPCH.h"

#include "pluginMain.h"
#include "ObjAttribCmd.h"

MSyntax ObjAttribCmd::m_cachedSyntax;
bool ObjAttribCmd::m_bCachedSyntax = false;

ObjAttribCmd::ObjAttribCmd()
{
}

ObjAttribCmd::~ObjAttribCmd()
{
}

void *ObjAttribCmd::creator(void)
{
	return new ObjAttribCmd;
}

MStatus ObjAttribCmd::doIt(const MArgList& args)
{
	MStringArray resultStrings;

	// make sure the syntax is built
	if (!m_bCachedSyntax)
		newSyntax();

	MStatus status;
	MArgDatabase argDatabase(m_cachedSyntax,args,&status);
	if (!status)
	{
		MGlobal::displayError( MString("Unable to parse command: ") + status.errorString());
		return status;
	}
	status = ::ExecuteCommand(argDatabase, resultStrings, m_undoRedoState);
	MPxCommand::setResult(resultStrings);
	return redoIt();
}

MStatus ObjAttribCmd::undoIt()
{
	MStatus status;
	bool bRefreshWindow = false;
	int nodesToUpdate = m_undoRedoState.m_updatedNodes.length();
	for (int nodeIndex = 0; nodeIndex < nodesToUpdate; nodeIndex++)
	{
		const MDagPath &dagPath = m_undoRedoState.m_updatedNodes[nodeIndex];
		const MString &oldAttributes = m_undoRedoState.m_previousNodeAttributes[nodeIndex];
		SetNoteAttr(dagPath,oldAttributes);
		bRefreshWindow = true;
	}
	if (m_undoRedoState.m_bSelectionChanged)
	{
		status = MGlobal::setActiveSelectionList(m_undoRedoState.m_previousSelection);
	}
	else if (bRefreshWindow)
	{
		::RefreshWindow();
	}
	MGlobal::executeCommand("autoUpdateAttrEd");
	return status;
}

MStatus ObjAttribCmd::redoIt()
{
	MStatus status;
	bool bRefreshWindow = false;
	int nodesToUpdate = m_undoRedoState.m_updatedNodes.length();
	for (int nodeIndex = 0; nodeIndex < nodesToUpdate; nodeIndex++)
	{
		const MDagPath &dagPath = m_undoRedoState.m_updatedNodes[nodeIndex];
		const MString &newAttributes = m_undoRedoState.m_newNodeAttributes[nodeIndex];
		SetNoteAttr(dagPath,newAttributes);
		bRefreshWindow = true;
	}
	if (m_undoRedoState.m_bSelectionChanged)
	{
		status = MGlobal::setActiveSelectionList(m_undoRedoState.m_newSelection);
	}
	else if (bRefreshWindow)
	{
		::RefreshWindow();
	}

	MGlobal::executeCommand("autoUpdateAttrEd");
	return status;
}

bool ObjAttribCmd::isUndoable() const
{
	// if something to do then it is undoable
	return (m_undoRedoState.m_bSelectionChanged || m_undoRedoState.m_updatedNodes.length()>0);
}

// static
MSyntax ObjAttribCmd::newSyntax()
{
	if (m_bCachedSyntax)
		return m_cachedSyntax;

	// actions
	m_cachedSyntax.addFlag( kQueryFlag, kQueryFlagLong );
	m_cachedSyntax.addFlag( kSetFlag, kSetFlagLong );
	m_cachedSyntax.addFlag( kPurgeFlag, kPurgeFlagLong );
	m_cachedSyntax.addFlag( kFindFlag, kFindFlagLong );
	m_cachedSyntax.addFlag( kRemapFlag, kRemapFlagLong );
	m_cachedSyntax.addFlag( kApplyDefaultsFlag, kApplyDefaultsFlagLong );
	m_cachedSyntax.addFlag( kPurgeUnusedFlag, kPurgeUnusedFlagLong );
   m_cachedSyntax.addFlag( kRefreshDialogFlag, kRefreshDialogFlagLong, MSyntax::kBoolean );

	// find options
	m_cachedSyntax.addFlag( kMatchCaseFlag, kMatchCaseFlagLong );
	m_cachedSyntax.addFlag( kMatchWholeWordFlag, kMatchWholeWordFlagLong );
	m_cachedSyntax.addFlag( kSearchDefaultFlag, kSearchDefaultFlagLong );
	m_cachedSyntax.addFlag( kSearchUnusedFlag, kSearchUnusedFlagLong );
	m_cachedSyntax.addFlag( kAppendSelectionFlag, kAppendSelectionFlagLong );
	m_cachedSyntax.addFlag( kSelectFlag, kSelectFlag );

	// general options
	m_cachedSyntax.addFlag( kKeyValueFlag, kKeyValueFlagLong, MSyntax::kString, MSyntax::kString );
	m_cachedSyntax.makeFlagMultiUse( kKeyValueFlag );
	m_cachedSyntax.addFlag( kKeyFlag, kKeyFlagLong, MSyntax::kString );
	m_cachedSyntax.makeFlagMultiUse( kKeyFlag );
	m_cachedSyntax.addFlag( kValueFlag, kValueFlagLong, MSyntax::kString );
	m_cachedSyntax.makeFlagMultiUse( kValueFlag );
	m_cachedSyntax.addFlag( kApplySceneFlag, kApplySceneFlagLong );

	// set options
	m_cachedSyntax.addFlag( kReplaceAllValuesFlag, kReplaceAllValuesFlagLong );
	m_cachedSyntax.addFlag( kPurgeKeyOnSetFlag, kPurgeKeyOnSetFlagLong, MSyntax::kString );
	m_cachedSyntax.makeFlagMultiUse( kPurgeKeyOnSetFlag );
	m_cachedSyntax.addFlag( kRemoveValueOnSetFlag, kRemoveValueOnSetFlagLong );

	// remap options
	m_cachedSyntax.addFlag( kRemapKeyFlag, kRemapKeyFlagLong, MSyntax::kString, MSyntax::kString );
	m_cachedSyntax.makeFlagMultiUse( kRemapKeyFlag );
	m_cachedSyntax.addFlag( kRemapKeyValueFlag, kRemapKeyValueFlagLong, MSyntax::kString, MSyntax::kString, MSyntax::kString );
	m_cachedSyntax.makeFlagMultiUse( kRemapKeyValueFlag );

	// query options
	m_cachedSyntax.addFlag( kQueryValuesFlag, kQueryValuesFlagLong );
	m_cachedSyntax.addFlag( kQueryControlTypeFlag, kQueryControlTypeFlagLong );
	m_cachedSyntax.addFlag( kQueryIsDefaultFlag, kQueryIsDefaultFlagLong );
	m_cachedSyntax.addFlag( kQueryDefaultValueFlag, kQueryDefaultValueFlagLong );
	m_cachedSyntax.addFlag( kQuerySelectionIndexFlag, kQuerySelectionIndexFlagLong );
	m_cachedSyntax.addFlag( kQueryAvailableValuesFlag, kQueryAvailableValuesFlagLong );
	m_cachedSyntax.addFlag( kQueryControlUIFlag, kQueryControlUIFlagLong );
	m_cachedSyntax.addFlag( kVersionFlag, kVersionFlagLong );
	m_cachedSyntax.addFlag( kAttributeConfigPathFlag, kAttributeConfigPathFlagLong );
	m_cachedSyntax.addFlag( kAttributeConfigNameFlag, kAttributeConfigNameFlagLong );

	m_cachedSyntax.setObjectType(MSyntax::kSelectionList, 0);

	m_bCachedSyntax = true;

	return m_cachedSyntax;
}

MStatus ObjAttribCmd::SetNoteAttr(MDagPath dagPath,const MString &noteString)
{
	MStatus status;
	MString command;
	MFnDependencyNode fnNode(dagPath.node());

	// if there are notes to put
	if (noteString.length())
	{
		MPlug plug = fnNode.findPlug("notes",&status);
		if (!status)
		{
			command = MString("addAttr -dt \"string\" -ln \"notes\" -sn \"nts\" ") + dagPath.fullPathName();
			MGlobal::executeCommand(command);
			plug = fnNode.findPlug("notes",&status);
			if (!status)
				return status;
		}
		status = plug.setValue(noteString.asChar());
	}
	else // delete attribute if no notes to put
	{
		MPlug plug = fnNode.findPlug("notes",&status);
		if (status)
		{
			command = MString("deleteAttr -at notes ") + dagPath.fullPathName();
			MGlobal::executeCommand(command);
			plug = fnNode.findPlug("notes",&status);
			if (status)
			{
				MGlobal::displayError(MString("error deleting notes from "+dagPath.fullPathName()));
				return MS::kFailure;
			}
			status = MS::kSuccess;
		}
	}
	return status;
}
