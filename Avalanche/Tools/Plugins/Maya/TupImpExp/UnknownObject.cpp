////////////////////////////////////////////////////////////////////////////
//
// UnknownObject
//
// Class to handle printing of unknown objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: UnknownObject.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/04/02   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * Skin and Bones exporting (matrices are not fixed yet)
 * 
 * *****************  Version 4  *****************
 * User: Tony Jones   Date: 11/20/02   Time: 3:18p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
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

#include "TupImpExpPch.h"
#include "UnknownObject.h"

void UnknownObject::AddAttributes(MFnDependencyNode fnNode,TupperwareAggregate *pNodeAgg)
{
	TupperwareAggregate *pAttributesAgg = pNodeAgg->AddAggregate("Attributes");

	int numAttributes = fnNode.attributeCount();
	pAttributesAgg->AddScalarInt("TotalAttributes",NULL,numAttributes);
	int attributeIndex;
	for (attributeIndex=0;attributeIndex<numAttributes;attributeIndex++)
	{
		MObject attributeObject = fnNode.attribute(attributeIndex);
		char string[10];
		itoa(attributeIndex,string,10);
		TupperwareAggregate *pAttributeAgg = pAttributesAgg->AddAggregate("Attribute",string);

		AddAttribute(fnNode,attributeObject,pAttributeAgg);
	}

	TupperwareAggregate *pPlugsAgg = pNodeAgg->AddAggregate("Plugs");

	MPlugArray plugArray;
	fnNode.getConnections(plugArray);
	int numPlugs = plugArray.length();
	pPlugsAgg->AddScalarInt("TotalPlugs",NULL,numPlugs);
	int plugIndex;
	for (plugIndex=0;plugIndex<numPlugs;plugIndex++)
	{
		MPlug plug = plugArray[plugIndex];
		char string[10];
		itoa(plugIndex,string,10);
		TupperwareAggregate *pPlugAgg = pPlugsAgg->AddAggregate("Plug",string);

		AddPlug(fnNode,plug,pPlugAgg);
	}



#if 0
		pAttributeAgg->AddScalarString("AttributeType",NULL,mObject.apiTypeStr());

		MPlug mPlug = fnNode.findPlug(mObject);
		
		pAttributeAgg->AddScalarString("PlugName",NULL,mPlug.partialName().asChar());
		if (mPlug.isCompound())
		{
			TupperwareAggregate *pCompoundAgg = pAttributeAgg->AddAggregate("Compound");
			int numCompoundChildren = mPlug.numChildren();
			pCompoundAgg->AddScalarInt("NumChildren",NULL,numCompoundChildren);

			int compoundIndex;
			for (compoundIndex=0;compoundIndex<numCompoundChildren;compoundIndex++)
			{
				char string[10];
				itoa(compoundIndex,string,10);
				TupperwareAggregate *pCompoundChild = pCompoundAgg->AddAggregate("CompoundChild",string);
				MPlug compoundChildPlug = mPlug.child(compoundIndex);
				pCompoundChild->AddScalarString("PlugName",NULL,compoundChildPlug.name().asChar());
				MObject compoundChildMObject = mPlug;
				pCompoundChild->AddScalarString("PlugType",NULL,compoundChildMObject.apiTypeStr());
			}
		}
	}
#endif

}

void UnknownObject::AddAttribute(MFnDependencyNode fnNode,MObject mObject,TupperwareAggregate *pAttributeAgg)
{
	pAttributeAgg->AddScalarString("AttributeType",NULL,mObject.apiTypeStr());
	MPlug mPlug = fnNode.findPlug(mObject);
	pAttributeAgg->AddScalarString("PlugName",NULL,mPlug.name().asChar());
//	pAttributeAgg->AddScalarString("PartialName",NULL,mPlug.partialName().asChar());
	
	switch (mObject.apiType())
	{
		case MFn::kUnitAttribute:
			{
				MFnUnitAttribute attr(mObject);
				switch (attr.unitType())
				{
					case MFnUnitAttribute::kInvalid:
						{
							pAttributeAgg->AddScalarString("UnitType",NULL,"Invalid");
						}
						break;
					case MFnUnitAttribute::kAngle:
						{
							pAttributeAgg->AddScalarString("UnitType",NULL,"Angle");
							MAngle angle;
							mPlug.getValue(angle);
							pAttributeAgg->AddScalarFloat("Value",NULL,(float)angle.asDegrees());
						}
						break;
					case MFnUnitAttribute::kDistance:
						{
							pAttributeAgg->AddScalarString("UnitType",NULL,"Distance");
							MDistance distance;
							mPlug.getValue(distance);
							pAttributeAgg->AddScalarFloat("Value",NULL,(float)distance.asMeters());
						}
						break;
					case MFnUnitAttribute::kTime:
						{
							pAttributeAgg->AddScalarString("UnitType",NULL,"Time");
							MTime time;
							mPlug.getValue(time);
							pAttributeAgg->AddScalarFloat("Value",NULL,(float)time.as(MTime::kMilliseconds));
						}
						break;
				}
			}
			break;
		case MFn::kTypedAttribute:
			{
				MFnTypedAttribute attr(mObject);
				AddData(mPlug,attr.attrType(),pAttributeAgg);
			}
			break;
		case MFn::kNumericAttribute:
			{
				MFnNumericAttribute attr(mObject);
				AddData(mPlug,attr.unitType(),pAttributeAgg);
			}
			break;
		case MFn::kMessageAttribute:
			break;
		case MFn::kMatrixAttribute:
			break;
		case MFn::kLightDataAttribute:
			break;
		case MFn::kGenericAttribute:
			break;
		case MFn::kEnumAttribute:
			break;
		case MFn::kCompoundAttribute:
			break;
	}
}

#if 0

	AddAttributes(fnNode,pObjectAgg);

	MPlug instObjGroups = fnNode.findPlug("instObjGroups",&status);
	int numChildren = instObjGroups.numChildren();
	int childIndex;
	for (childIndex=0;childIndex<numChildren;childIndex++)
	{
		MPlug child = instObjGroups.child(childIndex);
		TupString childName = child.name().asChar();
		int numGChildren = child.numChildren();
		int gChildrenIndex;
		for (gChildrenIndex=0;gChildrenIndex<numGChildren;gChildrenIndex++)
		{
			MPlug gChild = child.child(gChildrenIndex);
			TupString gChildName = gChild.name().asChar();
			MObject gChildObject = gChild;
			if (gChildObject.apiType() == MFn::kTypedAttribute)
			{
				MFnTypedAttribute typedAttribute(gChildObject);
				if (typedAttribute.attrType() == MFn::kComponentListData)
				{
					MFnComponentListData componentList(gChildObject);	
					int numComponents = componentList.length();
					int componentIndex;
					for (componentIndex=0;componentIndex<numComponents;componentIndex++)
					{
						MObject component = componentList[componentIndex];
						TupString typeName = UnknownObject::GetObjectType(component.apiType());
					}
				}
			}
			else
			{
				TupString typeName = UnknownObject::GetObjectType(gChildObject.apiType());
			}

		}
	}
#endif

void UnknownObject::AddData(MPlug mPlug,MFnData::Type type,TupperwareAggregate *pAttributeAgg)
{
	switch (type)
	{
		case MFnData::kInvalid:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kInvalid");
			}
			break;
		case MFnData::kNumeric:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kNumeric");
			}
			break;
		case MFnData::kPlugin:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kPlugin");
			}
			break;
		case MFnData::kPluginGeometry:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kPluginGeometry");
			}
			break;
		case MFnData::kString:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kString");
				MString string;
				mPlug.getValue(string);
				pAttributeAgg->AddScalarString("Value",NULL,string.asChar());
			}
			break;
		case MFnData::kMatrix:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kMatrix");
			}
			break;
		case MFnData::kStringArray:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kStringArray");
			}
			break;
		case MFnData::kDoubleArray:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kDoubleArray");
			}
			break;
		case MFnData::kIntArray:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kIntArray");
			}
			break;
		case MFnData::kPointArray:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kPointArray");
			}
			break;
		case MFnData::kVectorArray:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kVectorArray");
			}
			break;
		case MFnData::kComponentList:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kComponentList");
			}
			break;
		case MFnData::kMesh:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kMesh");
			}
			break;
		case MFnData::kLattice:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kLattice");
			}
			break;
		case MFnData::kNurbsCurve:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kNurbsCurve");
			}
			break;
		case MFnData::kNurbsSurface:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kNurbsSurface");
			}
			break;
		case MFnData::kSphere:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kSphere");
			}
			break;
		case MFnData::kDynArrayAttrs:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kDynArrayAttrs");
			}
			break;
		case MFnData::kDynSweptGeometry:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kDynSweptGeometry");
			}
			break;
		case MFnData::kSubdSurface:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kSubdSurface");
			}
			break;
	}
}

void UnknownObject::AddData(MPlug mPlug,MFnNumericData::Type type,TupperwareAggregate *pAttributeAgg)
{
	switch (type)
	{
		case MFnNumericData::kInvalid:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kInvalid");
			}
			break;
		case MFnNumericData::kBoolean:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kBoolean");
				bool value;
				mPlug.getValue(value);
				pAttributeAgg->AddScalarInt("Value",NULL,(int)value);
			}
			break;
		case MFnNumericData::kByte:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kByte");
				char value;
				mPlug.getValue(value);
				pAttributeAgg->AddScalarInt("Value",NULL,(int)value);
			}
			break;
		case MFnNumericData::kChar:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kChar");
				char value;
				mPlug.getValue(value);
				pAttributeAgg->AddScalarInt("Value",NULL,(int)value);
			}
			break;
		case MFnNumericData::kShort:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kShort");
				short value;
				mPlug.getValue(value);
				pAttributeAgg->AddScalarInt("Value",NULL,(int)value);
			}
			break;
		case MFnNumericData::k2Short:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"k2Short");
			}
			break;
		case MFnNumericData::k3Short:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"k3Short");
			}
			break;
		case MFnNumericData::kInt:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kInt");
				int value;
				mPlug.getValue(value);
				pAttributeAgg->AddScalarInt("Value",NULL,value);
			}
			break;
		case MFnNumericData::k2Int:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"k2Int");
			}
			break;
		case MFnNumericData::k3Int:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"k3Int");
			}
			break;
		case MFnNumericData::kFloat:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kFloat");
				float value;
				mPlug.getValue(value);
				pAttributeAgg->AddScalarFloat("Value",NULL,value);
			}
			break;
		case MFnNumericData::k2Float:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"k2Float");
			}
			break;
		case MFnNumericData::k3Float:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"k3Float");
			}
			break;
		case MFnNumericData::kDouble:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"kDouble");
				double value;
				mPlug.getValue(value);
				pAttributeAgg->AddScalarFloat("Value",NULL,(float)value);
			}
			break;
		case MFnNumericData::k2Double:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"k2Double");
			}
			break;
		case MFnNumericData::k3Double:
			{
				pAttributeAgg->AddScalarString("DataType",NULL,"k3Double");
			}
			break;
	}
}


void UnknownObject::AddPlug(MFnDependencyNode fnNode,MPlug mPlug,TupperwareAggregate *pPlugAgg)
{
	pPlugAgg->AddScalarString("Name",NULL,mPlug.name().asChar());
}
