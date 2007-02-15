////////////////////////////////////////////////////////////////////////////
//
// ExportMaterial
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportMaterial.cpp $
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 9/12/03    Time: 1:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * memory leak
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 8/26/03    Time: 4:34p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting transparency channel
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 8/20/03    Time: 4:11p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * moved map and material comparison to separate class
 * fixed memory leak
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/24/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * find out what was really used on the mesh
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 4/16/03    Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * new consolidation code
 * start of getting prebound mesh
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/06/03    Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export reflection maps
 * export lights
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 1/29/03    Time: 3:53p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * layered texture output
 * uv sets output
 * 
 * *****************  Version 5  *****************
 * User: Tony Jones   Date: 1/15/03    Time: 2:13p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/18/02   Time: 11:59a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * make sure shading type name is outputted
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

int TupExport::AddMaterial(const MObjectArray &shadingGroups,MIntArray &shaderIndices)
{
	int numShadingGroups = shadingGroups.length();
	int numShaderIndices = shaderIndices.length();

	// figure out which shaders were actually used
	TupArray<int> usedShaders;
	usedShaders.SetSize(numShadingGroups);
	int index;
	for (index=0;index<numShadingGroups;index++)
	{
		usedShaders[index] = 0; // clear this out
	}
	for (index=0;index<numShaderIndices;index++)
	{
		int shaderIndex = shaderIndices[index];
		if (shaderIndex != -1)
		{
			usedShaders[shaderIndex]++; // keep track of which ones that are used
		}
	}

	MObjectArray activeShadingGroups;
	activeShadingGroups.clear();

	// build a new shader array that just has the used one in it
	TupArray<int> shaderRemapArray;
	shaderRemapArray.SetSize(numShadingGroups);
	int numActiveShaders = 0;
	for (index=0;index<numShadingGroups;index++)
	{
		if (usedShaders[index])
		{
			activeShadingGroups.append(shadingGroups[index]);
			shaderRemapArray[index] = numActiveShaders;
			numActiveShaders++;
		}
	}
	
	// remap the shader indices to point to the active shaders (as well as handle the default one)
	bool bFoundDefaultShadingFace = false; // flag for keeping track of if we added a new default to the end of the active shaders
	for (index=0;index<numShaderIndices;index++)
	{
		int shaderIndex = shaderIndices[index];
		if (shaderIndex == -1) // if using default shading
		{
			bFoundDefaultShadingFace = true;
			shaderIndices[index] = activeShadingGroups.length(); // point to end
		}
		else
		{
			shaderIndices[index] = shaderRemapArray[shaderIndex]; // remap 
		}
	}

	if (numActiveShaders>1 || (numActiveShaders==1 && bFoundDefaultShadingFace)) // multi-sub object material
	{
		return AddMultiSubObjectMaterial(activeShadingGroups,shaderIndices,bFoundDefaultShadingFace);
	}
	else if (numActiveShaders) // standard material
	{
		return AddMaterial(activeShadingGroups[0]);
	}
	else
	{
		return -1; // no material
	}
}

int TupExport::AddMaterial(const MObject &shadingGroup)
{
	TupperwareAggregate *pMaterialAgg = new TupperwareAggregate;

	TupMaterial tupMaterial(pMaterialAgg);
	tupMaterial.SetMaterialType(TupMaterial::STANDARD_MATERIAL);
	AddStandardMaterial(pMaterialAgg,shadingGroup);

	unsigned long materialCRC = CompareTupObjects::GetMaterialCRC(pMaterialAgg);
	int numMaterials = m_pMaterialPool->GetNumMaterials();
	for (int materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		if (materialCRC == (*m_pMaterialCRCValues)[materialIndex])
		{
			TupperwareAggregate *pTestMaterial = m_pMaterialPool->GetMaterial(materialIndex);
			if (CompareTupObjects::CompareMaterials(pMaterialAgg,pTestMaterial))
			{
				delete pMaterialAgg; // don't need this
				return materialIndex;
			}
		}
	}
	int materialRef = m_pMaterialPool->AddMaterial();
	TupperwareAggregate *pNewMaterialAgg = m_pMaterialPool->GetMaterial(materialRef);
	pNewMaterialAgg->MoveChildren(pMaterialAgg); // move over to other one
	delete pMaterialAgg;
	m_pMaterialCRCValues->Add(materialCRC); // remember the CRC
	return materialRef;
}

void TupExport::AddStandardMaterial(TupperwareAggregate *pMaterialAgg,const MObject &shadingGroup)
{
	TupStandardMaterial tupStandardMaterial(pMaterialAgg);

	MFnDependencyNode fnNode(shadingGroup);

	int blendMode = TupBitmapMap::BLEND_MODE_OVER; // default blend mode

	MStatus status;
	MPlug fnPlug = fnNode.findPlug("surfaceShader",&status);
	if (status == MS::kSuccess)
	{
		MPlugArray connectedNodes;
		fnPlug.connectedTo(connectedNodes,true,false,&status);
		if (status == MS::kSuccess)
		{
			MColor diffuseColor(1.0f,1.0f,1.0f);
			MColor ambientColor(1.0f,1.0f,1.0f);
			MColor specularColor(0.0f,0.0f,0.0f);
			MColor filterColor(0.5f,0.5f,0.5f);
			float reflectivity = 0.5;

			float glossiness(0.1f);
			float specularLevel(0.0f);
			float specularSoften(0.1f);
			float transparency(0.0f);
			float wireSize(1.0f);
			float indexOfRefraction(1.5f);
			float reflectionDimIntensity(0.0f);
			float reflectionDimMultiplier(3.0f);
			float opacity(1.0f);
			float opacityFalloff(0.0f);
			int transparencyType(2);
			float selfIllumination(0.0f);
			int flags=0;

			if (connectedNodes.length() !=0)
			{
				MObject shadernode = connectedNodes[0].node();

				MFnDependencyNode fnShaderNode(shadernode);

				tupStandardMaterial.AddName(fnShaderNode.name().asChar());
		
				if (shadernode.apiType() == MFn::kBlinn)
				{
					MFnBlinnShader fnBlinn(shadernode);
					diffuseColor = fnBlinn.color();
					ambientColor = fnBlinn.ambientColor();
					reflectivity = fnBlinn.reflectivity();
					tupStandardMaterial.AddShadingTypeName("Blinn");
				}
				else if (shadernode.apiType() == MFn::kPhong)
				{
					MFnPhongShader fnPhong(shadernode);
					diffuseColor = fnPhong.color();
					ambientColor = fnPhong.ambientColor();
					reflectivity = fnPhong.reflectivity();
					tupStandardMaterial.AddShadingTypeName("Phong");
				}
				else if (shadernode.apiType() == MFn::kLambert)
				{
					MFnLambertShader fnLambert(shadernode);
					diffuseColor = fnLambert.color();
					ambientColor = fnLambert.ambientColor();
					tupStandardMaterial.AddShadingTypeName("Blinn");
				}
				else // spit this out even if we don't know what kind it is
				{
					tupStandardMaterial.AddShadingTypeName("Blinn");
				}
			}

			tupStandardMaterial.AddAmbientColor((float *)&ambientColor);
			tupStandardMaterial.AddDiffuseColor((float *)&diffuseColor);
			tupStandardMaterial.AddSpecularColor((float *)&specularColor);
			tupStandardMaterial.AddFilterColor((float *)&filterColor);

			tupStandardMaterial.AddGlossiness(glossiness);
			tupStandardMaterial.AddSpecularLevel(specularLevel);
			tupStandardMaterial.AddSpecularSoften(specularSoften);
			tupStandardMaterial.AddTransparency(transparency);
			tupStandardMaterial.AddWireSize(wireSize);
			tupStandardMaterial.AddIndexOfRefraction(indexOfRefraction);
			tupStandardMaterial.AddReflectionDimIntensity(reflectionDimIntensity);
			tupStandardMaterial.AddReflectionDimMultiplier(reflectionDimMultiplier);

			tupStandardMaterial.AddOpacity(opacity);
			tupStandardMaterial.AddOpacityFalloff(opacityFalloff);
			tupStandardMaterial.AddTransparencyType(transparencyType);

			tupStandardMaterial.AddSelfIllumination(selfIllumination);

			tupStandardMaterial.AddFlags(flags);


			if (connectedNodes.length() !=0)
			{
				int mapChannel=0; // start with channel 0

				MObject shadernode = connectedNodes[0].node();
				MFnDependencyNode fnNode(shadernode);

				MPlug colorplug = fnNode.findPlug("color",&status);
				if (status == MS::kSuccess) 
				{
					MPlugArray connectedNodes2;
					colorplug.connectedTo(connectedNodes2,true,false,&status);
					if ((status == MS::kSuccess)&&(connectedNodes2.length() > 0))
					{
						MObject colorNode = connectedNodes2[0].node();
						int mapRef = AddMap(colorNode,blendMode);
						if (mapRef!=-1)
						{
							TupperwareAggregate *pMapAgg = tupStandardMaterial.AddMap(mapChannel);
							TupStandardMaterialMap tupMap(pMapAgg);
							tupMap.AddMapName("Diffuse Color");
							tupMap.AddAmount(1.0f);
							int mapFlags = TupStandardMaterialMap::ENABLED;
							tupMap.AddFlags(mapFlags);
							tupMap.AddMapReference(mapRef);
							mapChannel++;
						}
					}
				}

				MPlug reflectedColorPlug = fnNode.findPlug("reflectedColor",&status);
				if (status == MS::kSuccess) 
				{
					MPlugArray connectedNodes2;
					reflectedColorPlug.connectedTo(connectedNodes2,true,false,&status);
					if ((status == MS::kSuccess)&&(connectedNodes2.length() > 0))
					{
						MObject reflectedColorNode = connectedNodes2[0].node();
						int mapRef = AddMap(reflectedColorNode,blendMode);
						if (mapRef!=-1)
						{
							TupperwareAggregate *pMapAgg = tupStandardMaterial.AddMap(mapChannel);
							TupStandardMaterialMap tupMap(pMapAgg);
							tupMap.AddMapName("Reflection");
							tupMap.AddAmount(reflectivity);
							int mapFlags = TupStandardMaterialMap::ENABLED;
							tupMap.AddFlags(mapFlags);
							tupMap.AddMapReference(mapRef);
							mapChannel++;
						}
					}
				}

				MPlug transparencyPlug = fnNode.findPlug("transparency",&status);
				if (status == MS::kSuccess) 
				{
					MPlugArray connectedNodes2;
					transparencyPlug.connectedTo(connectedNodes2,true,false,&status);
					if ((status == MS::kSuccess)&&(connectedNodes2.length() > 0))
					{
						MObject transparencyNode = connectedNodes2[0].node();
						int mapRef = AddMap(transparencyNode,blendMode);
						if (mapRef!=-1)
						{
							TupperwareAggregate *pMapAgg = tupStandardMaterial.AddMap(mapChannel);
							TupStandardMaterialMap tupMap(pMapAgg);
							tupMap.AddMapName("Transparency");
							tupMap.AddAmount(1.0);
							int mapFlags = TupStandardMaterialMap::ENABLED;
							tupMap.AddFlags(mapFlags);
							tupMap.AddMapReference(mapRef);
							mapChannel++;
						}
					}
				}

			}
		}
	}
}

int TupExport::AddMultiSubObjectMaterial(const MObjectArray &shadingGroups,MIntArray &shaderIndices,bool bAddDummyMaterialToEnd)
{
	TupArray<int> subMaterialRefs; // keep track of refs for shaderIndices
	Keeper<int> uniqueSubMaterialRefs; // keep track of unique refs

	int numShadingGroups = shadingGroups.length();
	int index;
	for (index=0;index<numShadingGroups;index++)
	{
		int subRef = AddMaterial(shadingGroups[index]);
		subMaterialRefs.Add(subRef);
		uniqueSubMaterialRefs.Add(subRef);
	}
	if (bAddDummyMaterialToEnd)
	{
		int dummyRef = -1;
		subMaterialRefs.Add(dummyRef);
		uniqueSubMaterialRefs.Add(dummyRef);
	}

	// grab the unique entries and sort them
	TupArray<int> sortedSubMaterialRefs(uniqueSubMaterialRefs.GetArray());
	TupSort<int>::SortAscending(sortedSubMaterialRefs.GetData(),sortedSubMaterialRefs.GetSize());

	// new keeper for fast lookup
	Keeper<int> searchSortedSubMaterialRefs;
	int numUniqueSubMaterials = uniqueSubMaterialRefs.GetSize();
	for (index=0;index<numUniqueSubMaterials;index++)
	{
		searchSortedSubMaterialRefs.Add(sortedSubMaterialRefs[index]);
	}

	// build an array for remapping the indicies
	int numSubMaterials = subMaterialRefs.GetSize();
	TupArray<int> subMaterialRemaps;
	for (index=0;index<numSubMaterials;index++)
	{
		int ref = subMaterialRefs[index];
		int remappedIndex = searchSortedSubMaterialRefs.GetIndex(ref);
		assert(remappedIndex!=-1);
		subMaterialRemaps.Add(remappedIndex);
	}

	// remap the indicies to the new ones
	int numShaderIndices = shaderIndices.length();
	for (index=0;index<numShaderIndices;index++)
	{
		int oldIndex = shaderIndices[index];
		int newIndex = subMaterialRemaps[oldIndex];
		shaderIndices[index] = newIndex;
	}

	TupperwareAggregate *pMaterialAgg = new TupperwareAggregate; // make one to add data into it
	TupMaterial tupMaterial(pMaterialAgg);
	tupMaterial.SetMaterialType(TupMaterial::MULTI_SUB_OBJECT_MATERIAL);

	TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
	tupMultiSubObjectMaterial.AddName("MultiSub");

	for (index=0;index<numUniqueSubMaterials;index++)
	{
		TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.AddSubMaterial(index);
		TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
		int subRef = sortedSubMaterialRefs[index];
		tupSubMaterial.AddMaterialReference(subRef);
		int flags = 0;
		if (subRef!=-1)
			flags |= TupMultiSubObjectMaterialSubMaterial::ENABLED;
		tupSubMaterial.AddFlags(flags);
		tupSubMaterial.AddSlotName("");
	}

	unsigned long materialCRC = CompareTupObjects::GetMaterialCRC(pMaterialAgg);
	int numMaterials = m_pMaterialPool->GetNumMaterials();
	for (int materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		if (materialCRC == (*m_pMaterialCRCValues)[materialIndex])
		{
			TupperwareAggregate *pTestMaterial = m_pMaterialPool->GetMaterial(materialIndex);
			if (CompareTupObjects::CompareMaterials(pMaterialAgg,pTestMaterial))
			{
				delete pMaterialAgg; // don't need this
				return materialIndex;
			}
		}
	}
	int materialRef = m_pMaterialPool->AddMaterial();
	TupperwareAggregate *pNewMaterialAgg = m_pMaterialPool->GetMaterial(materialRef);
	pNewMaterialAgg->MoveChildren(pMaterialAgg); // move over to other one
	m_pMaterialCRCValues->Add(materialCRC); // remember the CRC
	delete pMaterialAgg;
	return materialRef;
}

