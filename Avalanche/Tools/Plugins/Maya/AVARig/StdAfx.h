////////////////////////////////////////////////////////////////////////////
//
// StdAfx.h
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

#ifndef STDAFX_H
#define	STDAFX_H

#define REQUIRE_IOSTREAM

#include <math.h>

#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include <maya/MItSelectionList.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPxCommand.h>
#include <maya/MDgModifier.h>
#include <maya/MDagModifier.h>
#include <maya/MPxNode.h>
#include <maya/MPxTransform.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnAttribute.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MAngle.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MVector.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>


// Tupperware header files
#include <Tupperware/Atom.h>
#include <Tupperware/Aggregate.h>
#include <Tupperware/loadsave.h>
#include <Tupperware/TupString.h>
#include <Tupperware/TupArray.h>
#include <Tupperware/Keeper.h>

#include "macros.h"

#endif
