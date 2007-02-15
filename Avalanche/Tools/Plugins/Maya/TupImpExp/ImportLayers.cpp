////////////////////////////////////////////////////////////////////////////
//
// ImportLayers
//
// Tupperware import
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportLayers.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/11/03    Time: 4:22p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import and export of layers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/20/03    Time: 4:12p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * shader manager
 * comparison class
 */

#include "TupImpExpPch.h"
#include "TupImport.h"


void TupImport::AddDisplayLayer(MDagPath &path,int layerRef)
{
	if (layerRef==-1)
		return;

	if (!m_pLayerPool)
		return;

	TupperwareAggregate *pLayerAgg = m_pLayerPool->GetLayer(layerRef);
	TupLayer tupLayer(pLayerAgg);
	char *pLayerName;
	tupLayer.GetName(&pLayerName);

	if (!stricmp(pLayerName,"defaultLayer")) // if default layer then don't add 
		return;

	(*m_pAssignedNodesToLayer)[layerRef].Add(path);
}

class LayerSortData
{
public:
	int m_reference;
	int m_flags;
	TupString m_name;
	int m_colorIndex;
	int m_displayOrder;
	
	LayerSortData(void) : m_displayOrder(0),m_reference(0)
	{
	}
	
	bool LayerSortData::operator==(const LayerSortData& other) const
	{
		return (m_displayOrder==other.m_displayOrder);
	}
	
	bool LayerSortData::operator<=(const LayerSortData& other) const
	{
		return (m_displayOrder<=other.m_displayOrder);
	}

	bool LayerSortData::operator>=(const LayerSortData& other) const
	{
		return (m_displayOrder>=other.m_displayOrder);
	}

	bool LayerSortData::operator<(const LayerSortData& other) const
	{
		return (m_displayOrder<other.m_displayOrder);
	}

	bool LayerSortData::operator>(const LayerSortData& other) const
	{
		return (m_displayOrder>other.m_displayOrder);
	}
};


void TupImport::ImportLayers(void)
{
	if (!m_pLayerPool)
		return;

	Keeper<TupString> existingLayerNames;
	
	MItDependencyNodes iter( MFn::kDisplayLayer);
	for ( ; !iter.isDone(); iter.next() ) 
	{
		MObject object = iter.item();
		MFnDependencyNode layerNode(object);
		
		TupString layerName(layerNode.name().asChar());
		existingLayerNames.Add(layerName);
	}

	TupArray<LayerSortData> sortedLayers;
	int numLayers = m_pLayerPool->GetNumLayers();
	int layerIndex;
	for (layerIndex=0;layerIndex<numLayers;layerIndex++)
	{
		TupperwareAggregate *pLayerAgg = m_pLayerPool->GetLayer(layerIndex);
		if (!pLayerAgg)
			continue;
		TupLayer tupLayer(pLayerAgg);
		LayerSortData newLayer;
		char *pName;
		tupLayer.GetName(&pName);
		newLayer.m_name = pName;
		tupLayer.GetColorIndex(newLayer.m_colorIndex);
		tupLayer.GetDisplayOrder(newLayer.m_displayOrder);
		tupLayer.GetFlags(newLayer.m_flags);
		newLayer.m_reference = layerIndex;
		sortedLayers.Add(newLayer);
	}

	numLayers = sortedLayers.GetSize();
	TupSort<LayerSortData>::SortAscending(sortedLayers.GetData(),sortedLayers.GetSize());

	int displayOrder = existingLayerNames.GetSize();

	for (layerIndex=0;layerIndex<numLayers;layerIndex++)
	{
		LayerSortData &layerSortData = sortedLayers[layerIndex];

		if (!stricmp(layerSortData.m_name,"defaultLayer"))
			continue;

		// add layer to maya
		if (existingLayerNames.GetIndex(layerSortData.m_name)==-1)
		{
			MGlobal::executeCommand(MString("createDisplayLayer -empty -name ") + MString(layerSortData.m_name));
			
			MGlobal::executeCommand (
				MString ("setAttr ") +  MString(layerSortData.m_name) + MString (".color ") +
				MString (" ") + layerSortData.m_colorIndex);

			MGlobal::executeCommand (
				MString ("setAttr ") +  MString(layerSortData.m_name) + MString (".displayOrder ") +
				MString (" ") + displayOrder);

			displayOrder++;
			
			int displayTypeValue;
			switch (layerSortData.m_flags & TupLayer::FLAGS_DISPLAY_TYPE_MASK)
			{
				case TupLayer::FLAGS_DISPLAY_TYPE_NORMAL:
					displayTypeValue = 0;
					break;
				case TupLayer::FLAGS_DISPLAY_TYPE_TEMPLATE:
					displayTypeValue = 1;
					break;
				case TupLayer::FLAGS_DISPLAY_TYPE_REFERENCE:
					displayTypeValue = 2;
					break;
			}
			
			MGlobal::executeCommand (
				MString ("setAttr ") +  MString(layerSortData.m_name) + MString (".displayType ") +
				MString (" ") + displayTypeValue);
			
			int levelOfDetailValue;
			
			switch (layerSortData.m_flags & TupLayer::FLAGS_LEVEL_OF_DETAIL_MASK)
			{
				case TupLayer::FLAGS_LEVEL_OF_DETAIL_FULL:
					levelOfDetailValue = 0;
					break;
				case TupLayer::FLAGS_LEVEL_OF_DETAIL_BOX:
					displayTypeValue = 1;
					break;
			}
			
			MGlobal::executeCommand (
				MString ("setAttr ") +  MString(layerSortData.m_name) + MString (".levelOfDetail ") +
				MString (" ") + displayTypeValue);
			
			MGlobal::executeCommand (
				MString ("setAttr ") +  MString(layerSortData.m_name) + MString (".enabled ") +
				MString (" ") + ((layerSortData.m_flags & TupLayer::FLAGS_ENABLE_OVERRIDES) ? 1 : 0));
			
			MGlobal::executeCommand (
				MString ("setAttr ") +  MString(layerSortData.m_name) + MString (".visibility ") +
				MString (" ") + ((layerSortData.m_flags & TupLayer::FLAGS_VISIBLE) ? 1 : 0));
			
			MGlobal::executeCommand (
				MString ("setAttr ") +  MString(layerSortData.m_name) + MString (".playback ") +
				MString (" ") + ((layerSortData.m_flags & TupLayer::FLAGS_PLAYBACK) ? 1 : 0));
			
			MGlobal::executeCommand (
				MString ("setAttr ") +  MString(layerSortData.m_name) + MString (".texturing ") +
				MString (" ") + ((layerSortData.m_flags & TupLayer::FLAGS_TEXTURING) ? 1 : 0));
			
			MGlobal::executeCommand (
				MString ("setAttr ") +  MString(layerSortData.m_name) + MString (".shading ") +
				MString (" ") + ((layerSortData.m_flags & TupLayer::FLAGS_SHADING) ? 1 : 0));

		}

		TupArray<MDagPath> &pathsToAdd = (*m_pAssignedNodesToLayer)[layerSortData.m_reference];
		int numPaths = pathsToAdd.GetSize();
		if (numPaths)
		{
			MString command = MString("editDisplayLayerMembers -nr ") + MString(layerSortData.m_name) + MString(" ");
			
			for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
			{
				command += pathsToAdd[pathIndex].fullPathName();
				if (pathIndex+1<numPaths)
					command += MString(" ");
			}
			MGlobal::executeCommand(command);
		}
	}
}


