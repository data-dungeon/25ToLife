////////////////////////////////////////////////////////////////////////////
//
// ExportMap
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportMap.cpp $
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 9/12/03    Time: 1:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * comparison on adding custom mix map
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 8/20/03    Time: 4:11p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * moved map and material comparison to separate class
 * fixed memory leak
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:56p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fix map channel on uv set chooser
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 7/01/03    Time: 4:24p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * map attributes
 * y up 
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 6/27/03    Time: 2:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * map attributes export
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/16/03    Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * new consolidation code
 * start of getting prebound mesh
 * 
 * *****************  Version 8  *****************
 * User: Tony Jones   Date: 2/07/03    Time: 3:51p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/06/03    Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export reflection maps
 * export lights
 * 
 * *****************  Version 6  *****************
 * User: Tony Jones   Date: 1/30/03    Time: 2:37p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fixed case where placement node does not exist but the uv chooser is
 * connection directly to the file node
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 1/29/03    Time: 3:53p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * layered texture output
 * uv sets output
 * 
 * *****************  Version 4  *****************
 * User: Tony Jones   Date: 1/15/03    Time: 12:59p
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
#include "TupExport.h"
#include "CompareTupObjects.h"

int TupExport::AddMap(MObject mapNode,int blendMode)
{
	// create a new map to hold the data (later we will check it against the others)
	TupperwareAggregate *pMapAgg = new TupperwareAggregate;
	TupMap tupMap(pMapAgg);

	MFn::Type type = mapNode.apiType();
	switch (type)
	{
		case MFn::kFileTexture:
			{
				tupMap.SetMapType(TupMap::BITMAP_MAP);
				AddBitmapMap(pMapAgg,mapNode,blendMode);
			}
			break;
		case MFn::kLayeredTexture:
			{
				tupMap.SetMapType(TupMap::COMPOSITE_MAP);
				AddCompositeMap(pMapAgg,mapNode,blendMode);
			}
			break;
		case MFn::kEnvBall:
			{
				// pass through to texture
				MFnDependencyNode fnNode(mapNode);
				MStatus status;
				MPlug colorplug = fnNode.findPlug("image",&status);
				if (status == MS::kSuccess) 
				{
					MPlugArray connectedNodes;
					colorplug.connectedTo(connectedNodes,true,false,&status);
					if ((status == MS::kSuccess)&&(connectedNodes.length() > 0))
					{
						MObject imageNode = connectedNodes[0].node();
						delete pMapAgg; // we don't need this
						// call again with this node
						return AddMap(imageNode,blendMode);
					}
				}
				// if we cannot find the proper connections we are a unknown map
				tupMap.SetMapType(TupMap::UNKNOWN);
			}
			break;
		default:
			{
				// if we don't know the type then we are an unknown map
				tupMap.SetMapType(TupMap::UNKNOWN);
			}
			break;
	}
	unsigned long mapCRC = CompareTupObjects::GetMapCRC(pMapAgg);

	int numMaps = m_pMapPool->GetNumMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		if (mapCRC==(*m_pMapCRCValues)[mapIndex])
		{
			TupperwareAggregate *pTestMapAgg = m_pMapPool->GetMap(mapIndex);
			if (CompareTupObjects::CompareMaps(pMapAgg,pTestMapAgg))
			{
				delete pMapAgg; // we don't need this
				return mapIndex; // if found another like me then return it
			}
		}
	}
	int mapRef = m_pMapPool->AddMap();
	TupperwareAggregate *pNewMapAgg = m_pMapPool->GetMap(mapRef);
	pNewMapAgg->MoveChildren(pMapAgg); // move over to other one
	m_pMapCRCValues->Add(mapCRC); // remember the hash
	delete pMapAgg;
	return mapRef;
}

void TupExport::AddBitmapMap(TupperwareAggregate *pMapAgg,MObject mapNode,int blendMode)
{
	int mapHash = TupMap::BITMAP_MAP;

	TupBitmapMap tupBitmapMap(pMapAgg);
	MStatus status;

	MFnDependencyNode fnNode(mapNode);
	tupBitmapMap.AddName(fnNode.name().asChar());

	int flags = 0;

	int imageReference = -1;
	int mapping = 0;
	int mapChannel = 1;
	float UOffset = 0.0f;
	float VOffset = 0.0f;
	float UTiling = 1.0f;
	float VTiling = 1.0f;
	int mapAxis = 0;
	float UAngle = 0.0f;
	float VAngle = 0.0f;
	float WAngle = 0.0f;
	float blur = 1.0;
	float blurOffset = 0.0f;
	float noiseAmount = 1.0f;
	int noiseLevels = 1;
	float noiseSize = 1.0f;
	float croppingPlacementU = 0.0f;
	float coppingPlacementV = 0.0f;
	float croppingPlacementW = 1.0f;
	float croppingPlacementH = 1.0f;
	float placementJitter = 1.0f;
	int startFrame = 0;
	float playbackRate = 1.0f;
	float outputAmount = 1.0f;
	float outputRGBOffset = 0.0f;
	float outputRGBLevel = 1.0f;
	float outputBumpAmount = 1.0f;

	MPlug filenameplug = fnNode.findPlug("fileTextureName",&status);
	if (status == MS::kSuccess)
	{
		MString filename;
		status = filenameplug.getValue(filename);
		imageReference = AddImage(filename.asChar());
	}

	// user properties
	TupString noteString;
	MPlug notesPlug = fnNode.findPlug("notes",&status);
	if (status)
	{
		MString tempString;
		notesPlug.getValue(tempString);	
		noteString = tempString.asChar();
		noteString.Replace("\r\n","\n");
		noteString.Replace("\r","\n");
	}
	if (noteString.GetLength())
	{
		tupBitmapMap.AddUserProperties(noteString);
	}


	mapChannel = GetMapChannel(mapNode);
	
	MPlug uvCoordPlug = fnNode.findPlug("uvCoord",&status);
	if (status == MS::kSuccess)
	{
		MPlugArray connectedNodes;
		uvCoordPlug.connectedTo(connectedNodes,true,false,&status);

		if (status == MS::kSuccess)
		{
			if (connectedNodes.length()) // make sure 0 exists
			{
				MObject placementNode = connectedNodes[0].node();
				if (placementNode.hasFn(MFn::kPlace2dTexture))
				{
					MFnDependencyNode placementDepNode(placementNode);

					MPlug offsetUplug = placementDepNode.findPlug("offsetU",&status);
					if (status == MS::kSuccess)
					{
						float value;
						offsetUplug.getValue(value);
						UOffset = value;
					}

					MPlug offsetVplug = placementDepNode.findPlug("offsetV",&status);
					if (status == MS::kSuccess)
					{
						float value;
						offsetVplug.getValue(value);
						VOffset = value;
					}

					MPlug repeatUplug = placementDepNode.findPlug("repeatU",&status);
					if (status == MS::kSuccess)
					{
						float value;
						repeatUplug.getValue(value);
						UTiling = value;
					}

					MPlug repeatVplug = placementDepNode.findPlug("repeatV",&status);
					if (status == MS::kSuccess)
					{
						float value;
						repeatVplug.getValue(value);
						VTiling = value;
					}

					MPlug rotateUVplug = placementDepNode.findPlug("rotateUV",&status);
					if (status == MS::kSuccess)
					{
						float value;
						rotateUVplug.getValue(value);
						WAngle = value;
					}

					MPlug wrapUplug = placementDepNode.findPlug("wrapU",&status);
					if (status == MS::kSuccess)
					{
						int value;
						wrapUplug.getValue(value);
						if (value)
							flags |= TupBitmapMap::TUP_U_WRAP;
					}

					MPlug wrapVplug = placementDepNode.findPlug("wrapV",&status);
					if (status == MS::kSuccess)
					{
						int value;
						wrapVplug.getValue(value);
						if (value)
							flags |= TupBitmapMap::TUP_V_WRAP;
					}

					MPlug mirrorUplug = placementDepNode.findPlug("mirrorU",&status);
					if (status == MS::kSuccess)
					{
						int value;
						mirrorUplug.getValue(value);
						if (value)
							flags |= TupBitmapMap::TUP_U_MIRROR;
					}

					MPlug mirrorVplug = placementDepNode.findPlug("mirrorV",&status);
					if (status == MS::kSuccess)
					{
						int value;
						mirrorVplug.getValue(value);
						if (value)
							flags |= TupBitmapMap::TUP_V_MIRROR;
					}
				}
			}
		}
	}
	// indicate we need to transform like Maya
	flags |= TupBitmapMap::TUP_UV_TRANSFORM_TYPE_MAYA;

	tupBitmapMap.AddMapping(mapping);
	tupBitmapMap.AddMapChannel(mapChannel);
	tupBitmapMap.AddUOffset(UOffset);
	tupBitmapMap.AddVOffset(VOffset);
	tupBitmapMap.AddUTiling(UTiling);
	tupBitmapMap.AddVTiling(VTiling);
	tupBitmapMap.AddMapAxis(mapAxis);
	tupBitmapMap.AddUAngle(UAngle);
	tupBitmapMap.AddVAngle(VAngle);
	tupBitmapMap.AddWAngle(WAngle);
	tupBitmapMap.AddBlur(blur);
	tupBitmapMap.AddBlurOffset(blurOffset);
	tupBitmapMap.AddNoiseAmount(noiseAmount);
	tupBitmapMap.AddNoiseLevels(noiseLevels);
	tupBitmapMap.AddNoiseSize(noiseSize);
	tupBitmapMap.AddCroppingPlacementU(croppingPlacementU);
	tupBitmapMap.AddCroppingPlacementV(coppingPlacementV);
	tupBitmapMap.AddCroppingPlacementW(croppingPlacementW);
	tupBitmapMap.AddCroppingPlacementH(croppingPlacementH);
	tupBitmapMap.AddPlacementJitter(placementJitter);
	tupBitmapMap.AddStartFrame(startFrame);
	tupBitmapMap.AddPlaybackRate(playbackRate);

	tupBitmapMap.AddOutputAmount(outputAmount);
	tupBitmapMap.AddOutputRGBOffset(outputRGBOffset);
	tupBitmapMap.AddOutputRGBLevel(outputRGBLevel);
	tupBitmapMap.AddOutputBumpAmount(outputBumpAmount);
	tupBitmapMap.AddBlendMode(blendMode);
	tupBitmapMap.AddFlags(flags);
	tupBitmapMap.AddImageReference(imageReference);
}

bool TupExport::IsPlugConnectedToPlug(MPlug plugA,MPlug plugB)
{
	MStatus status;
	MPlugArray connected;
	plugA.connectedTo(connected,true,true,&status);
	for (int i = 0; i < (int)connected.length(); i++)
	{
		if (plugB == connected[i])
			return true;
	}
	return false;
}

int TupExport::GetMapChannel(MObject node)
{
	MStatus status;

	MFnMesh fnMesh(m_shapePath, &status);
	if (!status)
		return 1;

	MFnDependencyNode depNode(node);
	
	MPlug uvCoordPlug = depNode.findPlug("uvCoord",&status);
	if (!status)
		return 1;
	
	MPlugArray connectedNodes;
	uvCoordPlug.connectedTo(connectedNodes,true,false,&status);
	if (!status)
		return 1;
	
	if (connectedNodes.length()==0)
		return 1;

	if (connectedNodes[0].node().hasFn(MFn::kPlace2dTexture))
	{
		MObject placementNode = connectedNodes[0].node();
		MFnDependencyNode depNode(placementNode);

		MPlug uvCoordPlug = depNode.findPlug("uvCoord",&status);
		if (!status)
			return 1;

		uvCoordPlug.connectedTo(connectedNodes,true,false,&status);
		if (!status)
			return 1;
		
		if (connectedNodes.length()==0)
			return 1;

	}
	
	if (!connectedNodes[0].node().hasFn(MFn::kUvChooser))
		return 1;
	
	MObject uvchooser = connectedNodes[0].node();
	MFnDependencyNode uvchooserdepnode(uvchooser);

	MFnDependencyNode meshDepNode(m_shapePath.node());
	
	// find out which index on the mesh is used when hooked to the uvchooser
	MPlug uvSetplug = meshDepNode.findPlug("uvSet",&status);
	int numMeshElements = uvSetplug.numElements(&status);
	for (int i = 0; i < numMeshElements; i++)
	{
		MPlug plug = uvSetplug.elementByPhysicalIndex(i,&status);
		if (status)
		{
			int numchildren = plug.numChildren();
			if (numchildren>0)
			{
				MPlug uvsetnameplug = plug.child(0,&status);
				if (status)
				{
					MString uvsetnameplugname = uvsetnameplug.name();
					
					MPlug uvSetsplug = uvchooserdepnode.findPlug("uvSets",&status);
					if (status)
					{
						int numChooserElements = uvSetsplug.numElements(&status);
						
						for (int j = 0; j < numChooserElements; j++)
						{
							MPlug uvchooseruvsetplug = uvSetsplug.elementByPhysicalIndex(j,&status);
							if (IsPlugConnectedToPlug(uvsetnameplug,uvchooseruvsetplug))
							{
								return i+1;
							}
						}
					}
				}
			}
		}
	}
	return 1;
}



void TupExport::AddCompositeMap(TupperwareAggregate *pMapAgg,MObject mapNode,int blendMode)
{
	TupCompositeMap tupCompositeMap(pMapAgg);

	MStatus status;
	MFnDependencyNode fnNode(mapNode);
	tupCompositeMap.AddName(fnNode.name().asChar());
	
	MPlug plug = fnNode.findPlug("inputs",&status);
	if (!status)
		return;
	
	int numSubMaps = plug.numConnectedElements(&status);

	int nextSubMapIndex = 0;
	for (int subMapIndex=numSubMaps-1;subMapIndex>=0;subMapIndex--)
	{
		MPlug elementplug = plug.elementByPhysicalIndex(subMapIndex,&status);

		// defaults
		int colorMapRef = -1;

		//	third child is ".blend"
		MPlug blendModeplug = elementplug.child(2,&status);
		if (status)
		{
			blendModeplug.getValue(blendMode);
		}
		
		//	first child in compound attribute is ".color"
		MPlug colorplug = elementplug.child(0,&status);
		if (status)
		{
			MPlugArray connected;
			colorplug.connectedTo(connected,true,false,&status);
									
			if (connected.length()>0)
			{
				MObject colorNode = connected[0].node();
				colorMapRef = AddMap(colorNode,blendMode);
			}
		}
		
		// do we need a mix map for this?
		if (colorMapRef!=-1)
		{
			TupperwareAggregate *pSubMapAgg = tupCompositeMap.AddSubMap(nextSubMapIndex);
			TupCompositeMapSubMap subMap(pSubMapAgg);
			subMap.AddMapReference(colorMapRef);
			subMap.AddFlags(TupCompositeMapSubMap::ENABLED);
			nextSubMapIndex++;
		}
	}
}


