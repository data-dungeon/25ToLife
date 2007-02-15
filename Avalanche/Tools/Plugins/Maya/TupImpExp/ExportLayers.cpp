////////////////////////////////////////////////////////////////////////////
//
// ExportLayer
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportLayers.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/11/03    Time: 4:22p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import and export of layers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting layers
 */

#include "TupImpExpPch.h"
#include "TupExport.h"

void TupExport::GetLayers(void)
{
	MItDependencyNodes iter( MFn::kDisplayLayer);
	for ( ; !iter.isDone(); iter.next() ) 
	{
		MObject object = iter.item();
		MFnDependencyNode layerNode(object);

		TupString layerName(layerNode.name().asChar());
		int oldNumLayers = m_pLayerNames->GetSize();
		int layerIndex = m_pLayerNames->Add(layerName);
		// if this name is already there...
		if (layerIndex!=oldNumLayers)
			continue;

		LayerData newLayer;
		newLayer.m_reference = 0;
		newLayer.m_colorIndex = MayaHelper::GetIntAttrib(layerNode,"color");
		newLayer.m_displayOrder = MayaHelper::GetIntAttrib(layerNode,"displayOrder");
		newLayer.m_bUsed = false;

		int flags = 0;

		int intValue;
		
		intValue = MayaHelper::GetIntAttrib(layerNode,"displayType");
		switch (intValue)
		{
			case 0:
				flags |= TupLayer::FLAGS_DISPLAY_TYPE_NORMAL;
				break;
			case 1:
				flags |= TupLayer::FLAGS_DISPLAY_TYPE_TEMPLATE;
				break;
			case 2:
				flags |= TupLayer::FLAGS_DISPLAY_TYPE_REFERENCE;
				break;
		}
		
		intValue = MayaHelper::GetIntAttrib(layerNode,"levelOfDetail");
		switch (intValue)
		{
			case 0:	
				flags |= TupLayer::FLAGS_LEVEL_OF_DETAIL_FULL;
				break;
			case 1:
				flags |= TupLayer::FLAGS_LEVEL_OF_DETAIL_BOX;
		}
		
		if (MayaHelper::GetBoolAttrib(layerNode,"enabled"))
		{
			flags |= TupLayer::FLAGS_ENABLE_OVERRIDES;
		}
		
		if (MayaHelper::GetBoolAttrib(layerNode,"visibility"))
		{
			flags |= TupLayer::FLAGS_VISIBLE;
		}
		
		if (MayaHelper::GetBoolAttrib(layerNode,"playback"))
		{
			flags |= TupLayer::FLAGS_PLAYBACK;
		}
		
		if (MayaHelper::GetBoolAttrib(layerNode,"texturing"))
		{
			flags |= TupLayer::FLAGS_TEXTURING;
		}
		
		if (MayaHelper::GetBoolAttrib(layerNode,"shading"))
		{
			flags |= TupLayer::FLAGS_SHADING;
		}
		newLayer.m_flags = flags;

		m_pLayers->Add(newLayer);
	}
}

void TupExport::AddSelectedNodeToLayer(const MDagPath &dagPath)
{
	MStatus status;
	MFnDagNode dagNode(dagPath);

	int layerRef = m_pLayerNames->GetIndex(TupString("defaultLayer"));
	MPlug layerPlug = dagNode.findPlug("drawOverride",&status);
	if (status)
	{
		MPlugArray conns;
		if (layerPlug.connectedTo(conns,true,false)) 
		{
			if (conns.length()!=0)
			{
				MObject layerObject = conns[0].node();
				if (layerObject.apiType() == MFn::kDisplayLayer)
				{
					MFnDependencyNode layerNode(layerObject);
					layerRef = m_pLayerNames->GetIndex(TupString(layerNode.name().asChar()));
				}
			}
		}
	}
	if (layerRef!=-1)
	{
		(*m_pLayers)[layerRef].m_bUsed = true;
	}
}

void TupExport::SetLayerReferenceValues(void)
{
	int startRef=0;
	for (int layerIndex=0;layerIndex<m_pLayerNames->GetSize();layerIndex++)
	{
		LayerData &layerData = (*m_pLayers)[layerIndex];
		if (layerData.m_bUsed || !m_bExportSelected) // do only used layers when exporting selection
		{
			layerData.m_reference = startRef;
			startRef++;
		}
		else
		{
			layerData.m_reference = -1;
		}
	}
}

void TupExport::AddLayerPool(void)
{
	for (int layerIndex=0;layerIndex<m_pLayerNames->GetSize();layerIndex++)
	{
		LayerData &layerData = (*m_pLayers)[layerIndex];
		if (layerData.m_reference!=-1)
		{
			TupperwareAggregate *pLayerAgg = m_pLayerPool->AddLayer(layerData.m_reference);

			TupLayer tupLayer(pLayerAgg);

			tupLayer.AddName((*m_pLayerNames)[layerIndex]);
			tupLayer.AddColorIndex(layerData.m_colorIndex);
			tupLayer.AddDisplayOrder(layerData.m_displayOrder);
			tupLayer.AddFlags(layerData.m_flags);
		}
	}
}

