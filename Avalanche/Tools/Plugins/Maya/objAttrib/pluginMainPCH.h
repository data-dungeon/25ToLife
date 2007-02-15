/****************************************************************************

	$Workfile: pluginMainPCH.h $

	$Archive: /Avalanche/tools/Plugins/Maya/objAttrib/pluginMainPCH.h $

	$Author: Adam Clayton $

	$Date: 12/20/02 8:56a $

	CONFIDENTIAL AND PROPRIETARY INFORMATION

	Copyright © Avalanche Software 2002

	All rights reserved.

****************************************************************************/
// pluginMainPCH.h : header file

#ifndef __PLUGINMAINPCH_H__
#define __PLUGINMAINPCH_H__

#pragma warning (disable : 4786)

#ifndef STRICT
#define STRICT
#endif

// we are importing any MFC stuff
#define AFX_EXT_CLASS       __declspec(dllimport)
#define AFX_EXT_API         __declspec(dllimport)
#define AFX_EXT_DATA        __declspec(dllimport)

#define DllImport  __declspec( dllimport )
#define DllExport  __declspec( dllexport )

// windows header files
//#include <windows.h>
#include <assert.h>

// windows STL headers
#include <string>
#include <vector>

#define REQUIRE_IOSTREAM

// maya header files
#include <maya/MGlobal.h>
#include <maya/MSyntax.h>
#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPxCommand.h>
#include <maya/MPlug.h>
#include <maya/MArgDatabase.h>
#include <maya/MDagPathArray.h>
#include <maya/MStringArray.h>

#include "objAttribWin32/CommandArgs.h"
#include "objAttribWin32/UndoRedoState.h"

#endif	// __PLUGINMAINPCH_H__