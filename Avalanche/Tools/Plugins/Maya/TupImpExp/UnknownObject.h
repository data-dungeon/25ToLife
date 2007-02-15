////////////////////////////////////////////////////////////////////////////
//
// UnknownObject
//
// Class to handle printing of unknown objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: UnknownObject.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/04/02   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * Skin and Bones exporting (matrices are not fixed yet)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/13/02   Time: 4:21p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of materials, maps, images, and attributes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/06/02   Time: 3:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of mesh stuff
 */

#ifndef UNKNOWN_OBJECT
#define UNKNOWN_OBJECT

class UnknownObject
{
public:
	static void AddAttributes(MFnDependencyNode fnNode,TupperwareAggregate *pNodeAgg);
	static void AddAttribute(MFnDependencyNode fnNode,MObject mObject,TupperwareAggregate *pAttributeAgg);
	static void AddData(MPlug plug,MFnData::Type type,TupperwareAggregate *pAttributeAgg);
	static void AddData(MPlug plug,MFnNumericData::Type type,TupperwareAggregate *pAttributeAgg);
	static void AddPlug(MFnDependencyNode fnNode,MPlug mPlug,TupperwareAggregate *pPlugAgg);

};

#endif
