/****************************************************************************

	$Workfile: pluginMainPCH.h $

	$Archive: /Avalanche/tools/Plugins/Maya/faceAttrib/pluginMainPCH.h $

	$Author: Adam Clayton $

	$Date: 12/19/02 3:58p $

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

// windows header files
//#include <windows.h>

#define REQUIRE_IOSTREAM

// maya header files
#include <maya/MGlobal.h>
//#include <maya/MFnPlugin.h>
//#include <maya/MPxCommand.h>
#include <maya/MMessage.h>
#include <maya/MModelMessage.h>

// Tupperware header files
#include <Tupperware/Atom.h>
#include <Tupperware/Aggregate.h>
#include <Tupperware/loadsave.h>
#include <Tupperware/TupString.h>
#include <Tupperware/TupArray.h>
#include <Tupperware/Keeper.h>

// UIDisplay header files
#include <Plugins/Libraries/UIDisplay/ConfigRoot.h>
#include <Plugins/Libraries/UIDisplay/UIController.h>
#include <Plugins/Libraries/UIDisplay/ExclusiveController.h>
#include <Plugins/Libraries/UIDisplay/FuncPtrController.h>
#include <Plugins/Libraries/UIDisplay/Registry.h>
#include <Plugins/Libraries/UIDisplay/CurrAttribBase.h>

#include "faceAttribWin32/FaceAttributes.h"

#endif	// __PLUGINMAINPCH_H__