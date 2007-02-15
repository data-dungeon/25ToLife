////////////////////////////////////////////////////////////////////////////
//
// MayaHelper
//
// Helper class for accessing data in Maya
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: MayaHelper.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/15/03    Time: 3:38p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * uv chooser connected
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/26/03    Time: 4:34p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * creating shaders
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vector3 issues
 * assert issues
 * add audio clips
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * motion path exporting
 */

#ifndef MAYA_HELPER_H
#define MAYA_HELPER_H

class MayaHelper
{
public:
	static float GetFloatAttrib(MFnDependencyNode &depNode,const char *pAttribName);
	static float GetTimeAttrib(MFnDependencyNode &depNode,const char *pAttribName);
	static int GetIntAttrib(MFnDependencyNode &depNode,const char *pAttribName);
	static bool GetBoolAttrib(MFnDependencyNode &depNode,const char *pAttribName);
	static MString GetStringAttrib(MFnDependencyNode &depNode,const char *pAttribName);

	static MObject GetMObject(const char *pDepNodeName);
	static int GetNextAvailableLogicalIndex(MPlug &plug);

	static const MPtrBase *GetMPtrBase(const MFnBase &mfnBase);
	static MPtrBase *GetMPtrBase(MFnBase &mfnBase);
	static const MPtrBase *GetMPtrBase(const MObject &mObject);
	static MPtrBase *GetMPtrBase(MObject &mObject);
};

#include "MayaHelper.hpp"

#endif
