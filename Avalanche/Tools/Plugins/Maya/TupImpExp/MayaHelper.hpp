////////////////////////////////////////////////////////////////////////////
//
// MayaHelper
//
// Helper class for accessing data in Maya
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: MayaHelper.hpp $
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

inline float MayaHelper::GetFloatAttrib(MFnDependencyNode &depNode,const char *pAttribName)
{
	double value = 0.0f;
	
	MStatus status;
	MPlug plug = depNode.findPlug(pAttribName,&status);
	if (status==MStatus::kSuccess)
	{
		plug.getValue(value);
	}
	return (float) value;
}

inline float MayaHelper::GetTimeAttrib(MFnDependencyNode &depNode,const char *pAttribName)
{
	MTime value;
	
	MStatus status;
	MPlug plug = depNode.findPlug(pAttribName,&status);
	if (status==MStatus::kSuccess)
	{
		plug.getValue(value);
	}
	return (float) value.value();
}

inline int MayaHelper::GetIntAttrib(MFnDependencyNode &depNode,const char *pAttribName)
{
	int value = 0;
	
	MStatus status;
	MPlug plug = depNode.findPlug(pAttribName,&status);
	if (status==MStatus::kSuccess)
	{
		plug.getValue(value);
	}
	return value;
}

inline bool MayaHelper::GetBoolAttrib(MFnDependencyNode &depNode,const char *pAttribName)
{
	bool bValue = false;
	
	MStatus status;
	MPlug plug = depNode.findPlug(pAttribName,&status);
	if (status==MStatus::kSuccess)
	{
		plug.getValue(bValue);
	}
	return bValue;
}

inline MString MayaHelper::GetStringAttrib(MFnDependencyNode &depNode,const char *pAttribName)
{
	MString value;
	
	MStatus status;
	MPlug plug = depNode.findPlug(pAttribName,&status);
	if (status==MStatus::kSuccess)
	{
		plug.getValue(value);
	}
	return value;
}

inline MObject MayaHelper::GetMObject(const char *pDepNodeName)
{
	MSelectionList selection;
	
	MStatus status = selection.add(MString(pDepNodeName));
	
	if (!status)
		return MObject::kNullObj;
	
	MObject object;
	
	selection.getDependNode(0,object);
	
	return object;
}

inline int MayaHelper::GetNextAvailableLogicalIndex(MPlug &plug)
{
	MStatus status;

	MIntArray indices;
	plug.getExistingArrayAttributeIndices(indices,&status);
	if (status!=MStatus::kSuccess)
		return 0;
	int numIndices = indices.length();
	TupArray<bool> activeArray;

	for (int index=0;index<numIndices;index++)
	{
		int slot = indices[index];
		if (slot>=activeArray.GetSize())
			activeArray.SetSize(slot+1);
		activeArray[slot] = true;
	}
	int numSlots = activeArray.GetSize();
	for (int slotIndex=0;slotIndex<numSlots;slotIndex++)
	{
		if (!activeArray[slotIndex])
			return slotIndex;
	}
	return numSlots;
}

struct MObjectTemp
{
	MPtrBase *		ptr;
	MFn::Type		tp;
};

inline const MPtrBase *MayaHelper::GetMPtrBase(const MObject &mObject)
{
	return ((const MObjectTemp *)&mObject)->ptr;
}

inline MPtrBase *MayaHelper::GetMPtrBase(MObject &mObject)
{
	return ((MObjectTemp *)&mObject)->ptr;
}

inline MPtrBase* MayaHelper::GetMPtrBase(MFnBase &mfnBase)
{
	MObject mObject = mfnBase.object();
	return GetMPtrBase(mObject);
}

inline const MPtrBase* MayaHelper::GetMPtrBase(const MFnBase &mfnBase)
{
	MObject mObject = mfnBase.object();
	return GetMPtrBase(mObject);
}

