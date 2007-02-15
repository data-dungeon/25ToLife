/****************************************************************************

	$Workfile: StdAfx.h $

	$Archive: /Avalanche/tools/Plugins/Maya/mapAttrib/mapAttribWin32/StdAfx.h $

	$Author: Adam Clayton $

	$Date: 6/27/03 1:47p $

	CONFIDENTIAL AND PROPRIETARY INFORMATION

	Copyright © Avalanche Software 2002

	All rights reserved.

****************************************************************************/
// stdafx.h : include file for standard MFC, COM/OLE, etc. include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#define __STDAFX_H_
#pragma once

#pragma warning (disable : 4786)

// these need to be added to the objective toolkit stdafx.h file in order to work
#define WINVER		 0x0501
#define _WIN32_WINNT 0x0501
#define	_WIN32_IE	 0x0560

//#define _AFXEXT

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define ISOLATION_AWARE_ENABLED	1

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxpriv.h>

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef STRICT
#define STRICT
#endif

// windows header files
#include <windows.h>
#include <comdef.h>
#include <direct.h>
//#include <tchar.h>

// stl header files
#include <string>
#include <vector>

#define REQUIRE_IOSTREAM

// maya header files
#include <maya/MGlobal.h>
//#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MArgParser.h>
#include <maya/MSyntax.h>

#include <maya/MString.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnMesh.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItGeometry.h>
#include <maya/MFnMesh.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnSet.h>
#include <maya/MDagModifier.h>
#include <maya/MFnTransform.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnTransform.h>
#include <maya/MDGModifier.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MFnDoubleIndexedComponent.h>
#include <maya/MFnTripleIndexedComponent.h>
#include <maya/MFnMatrixData.h>
#include <maya/MMessage.h>
#include <maya/MModelMessage.h>
#include <maya/MSceneMessage.h>
#include <maya/M3dView.h>

// Attributes
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnCompoundAttribute.h>

#include <maya/MAngle.h>
#include <maya/MTime.h>
#include <maya/MDistance.h>

#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnLight.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MObjectArray.h>
#include <maya/MStringArray.h>
#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MFileIO.h>
#include <maya/MFnData.h>
#include <maya/MFnComponentListData.h>
                 

// Tupperware header files
#include <Tupperware/Atom.h>
#include <Tupperware/Aggregate.h>
#include <Tupperware/Scalar.h>
#include <Tupperware/List.h>
#include <Tupperware/loadsave.h>
#include <Tupperware/TupString.h>
#include <Tupperware/TupArray.h>
#include <Tupperware/Hash.h>
#include <Tupperware/Keeper.h>
#include <Tupperware/tupSort.h>

// UIDisplay header files
#include <Plugins/Libraries/UIDisplay/ConfigRoot.h>
#include <Plugins/Libraries/UIDisplay/UIController.h>
#include <Plugins/Libraries/UIDisplay/ExclusiveController.h>
#include <Plugins/Libraries/UIDisplay/FuncPtrController.h>
#include <Plugins/Libraries/UIDisplay/Registry.h>
#include <Plugins/Libraries/UIDisplay/CurrAttribBase.h>
#include <Plugins/Libraries/UIDisplay/UIDisplayBase.h>

// AttrControls
#include <plugins/libraries/attrcontrols/CustomControls.h>
#include <Plugins/Libraries/AttrControls/WinUtil.h>
#include <plugins/libraries/attrcontrols/ICustomRollupWindow.h>
#include <plugins/libraries/attrcontrols/ICustomRollupPanel.h>
#include <Plugins/Libraries/AttrControls/DebugPrint.h>
#include <Plugins/Libraries/AttrControls/ICustomEditBox.h>
#include <Plugins/Libraries/AttrControls/ICustomSlider.h>
#include <Plugins/Libraries/AttrControls/ICustomSpinner.h>
#include <Plugins/Libraries/AttrControls/ICustomToolTip.h>

// KeyVal
#include <Plugins/Libraries/KeyVal/KeyValPanelController.h>
#include <Plugins/Libraries/KeyVal/KeyValCommunicatorBase.h>
#include <Plugins/Libraries/KeyVal/resource.h>

#include "resource.h"
#include "FilePath.h"
#include "RegistryKeys.h"

#define BEGIN_BLOCK	"BEGIN MAPATTRIBUTES"
#define END_BLOCK	"END MAPATTRIBUTES"

#define CFG_DEFAULT_NAME _T("MapAttr")	
#define CFG_DEFAULT_FILENAME _T("plug-ins\\MapAttr.cfg")	

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
