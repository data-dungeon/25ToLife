////////////////////////////////////////////////////////////////////////////
//
// MaterialMaps.h
//
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MaterialMaps.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:49a
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:29p
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 */

////////////////////////////////////////////////////////////////////////////

#include "StampPch.h"
#include "MaterialMaps.h"

bool MaterialMapsManager::ImageLayer::operator==(const MaterialMapsManager::ImageLayer& other) const
{
	return (m_imageRef==other.m_imageRef && 
			  m_opacity==other.m_opacity && 
			  m_channel==other.m_channel && 
			  m_blendMode==other.m_blendMode);
}

MaterialMapsManager::MaterialMapsManager(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool) :
	m_tupMaterialPool(tupMaterialPool),
	m_tupMapPool(tupMapPool),
	m_bLayersAdded(false),
	m_bMeshProcessing(false)
{
}

void MaterialMapsManager::StartMesh(int materialRef,TupTriMesh &tupTriMesh)
{
	assert(!m_bMeshProcessing);
	assert(materialRef!=-1); // starting material reference needs to be valid

	m_bMeshProcessing = true;
	m_meshMaterialRef = -1; // start with not valid

	m_initialMeshMaterialRefs.RemoveAll(); // start over
	m_addedMeshMaterialRefs.RemoveAll();

	// compute the starting channel for the layers
	int maxMeshMapChannel = 1;
	int numMeshMaps = tupTriMesh.GetNumMeshMaps();
	for (int meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
		TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
		int mapChannel = tupTriMeshMap.GetMapChannel();
		if (mapChannel>maxMeshMapChannel)
			maxMeshMapChannel=mapChannel;
	}
	m_startChannel = maxMeshMapChannel+1;

	// get the existing materials used on the mesh
	TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);
	TupMaterial tupMaterial(pMaterialAgg);
	char *pMeshMaterialName;
	tupMaterial.GetName(&pMeshMaterialName);
	m_meshMaterialName = pMeshMaterialName;
	m_meshMaterialName += " Layered";

	switch (tupMaterial.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
		case TupMaterial::COMPOSITE_MATERIAL:
		case TupMaterial::BLEND_MATERIAL:
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			m_initialMeshMaterialRefs.Add(materialRef);
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			{
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
				int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
				for (int subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubObjectMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
					TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubObjectMaterialAgg);
					int subMaterialRef;
					tupMultiSubObjectMaterialSubMaterial.GetMaterialReference(subMaterialRef);
					m_initialMeshMaterialRefs.Add(subMaterialRef);
				}
			}
			break;
	}

}

int MaterialMapsManager::GetMaterialIndex (int materialIndex,const TupArray<ImageLayer> &imageLayers)
{
	assert(m_bMeshProcessing);
	if (!m_bLayersAdded)
	{
		assert(m_meshMaterialRef==-1);
		m_meshMaterialRef = m_tupMaterialPool.AddMaterial();
		TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(m_meshMaterialRef);
		TupMaterial tupMaterial(pMaterialAgg);
		tupMaterial.AddName(m_meshMaterialName);
		tupMaterial.SetMaterialType(TupMaterial::MULTI_SUB_OBJECT_MATERIAL);
	}
	assert(m_meshMaterialRef!=-1);

	// build 
	int numLayers = imageLayers.GetSize();

	materialIndex = ValidateMaterialIndex(materialIndex);

	TupArray<int> materialStack;
	materialStack.Add(m_initialMeshMaterialRefs[materialIndex]); // stack starts with initial material for the material index

	// create materials, maps and build a stack of materials
	for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
	{
		const ImageLayer &imageLayer = imageLayers[layerIndex];

		int currentNumLayers = m_imageLayers.GetSize();
		int keeperIndex = m_imageLayers.Add(imageLayer);
		// if we added a new one
		if (keeperIndex==currentNumLayers)
		{
			// keep in sync
			m_imageLayerMaterialRefs.Add(CreateMaterial(imageLayer));
		}
		materialStack.Add(m_imageLayerMaterialRefs[keeperIndex]); // add material ref to stack
	}

	// maintain the material stacks
	int currentNumStacks = m_materialStacks.GetSize();
	int stackKeeperIndex = m_materialStacks.Add(materialStack);
	// if we added a new one
	if (stackKeeperIndex == currentNumStacks)
	{
		// create a composite material that handles the stack
		m_materialStackRefs.Add(CreateMaterial(materialStack));
	}

	// keep track of the materials used on the mesh (stacks)
	int materialRefsKeeperIndex = m_addedMeshMaterialRefs.Add(m_materialStackRefs[stackKeeperIndex]);

	// my new material index is my keeper index (taking into account the initial mesh materials used)
	return m_initialMeshMaterialRefs.GetSize()+materialRefsKeeperIndex;
}

int MaterialMapsManager::GetMaterialRef(void)
{
	return m_meshMaterialRef;
}

void MaterialMapsManager::FinishMesh(void)
{
	assert(m_bMeshProcessing);
	m_bMeshProcessing = false;
}

int MaterialMapsManager::GetStartChannel(void)
{
	assert(m_bMeshProcessing);
	return m_startChannel;
}

int MaterialMapsManager::ValidateMaterialIndex(int materialIndex)
{
	return (materialIndex>=0 && materialIndex<m_initialMeshMaterialRefs.GetSize()) ? materialIndex : 0;
}

int MaterialMapsManager::CreateMaterial(const ImageLayer &imageLayer)
{
	int newMaterialRef = m_tupMaterialPool.AddMaterial();
	TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(newMaterialRef);
	TupMaterial tupMaterial(pMaterialAgg);
	tupMaterial.SetMaterialType(TupMaterial::STANDARD_MATERIAL);
	tupMaterial.AddName("StampLayer");
	TupStandardMaterial tupStandardMaterial(pMaterialAgg);

	Vector3 diffuseColor; diffuseColor.Set(1.0f,1.0f,1.0f);
	Vector3 ambientColor; ambientColor.Set(1.0f,1.0f,1.0f);
	Vector3 specularColor; specularColor.Set(0.0f,0.0f,0.0f);
	Vector3 filterColor; filterColor.Set(0.5f,0.5f,0.5f);

	float reflectivity = 0.5;

	float glossiness(0.1f);
	float specularLevel(0.0f);
	float specularSoften(0.1f);
	float transparency(0.0f);
	float wireSize(1.0f);
	float indexOfRefraction(1.5f);
	float reflectionDimIntensity(0.0f);
	float reflectionDimMultiplier(3.0f);
	float opacity(imageLayer.m_opacity);
	float opacityFalloff(0.0f);
	int transparencyType(2);
	float selfIllumination(0.0f);
	int flags=0;

	tupStandardMaterial.AddShadingTypeName("Blinn");

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

	int mapRef = CreateMap(imageLayer);

	TupperwareAggregate *pMapAgg = tupStandardMaterial.AddMap(0);
	TupStandardMaterialMap tupMap(pMapAgg);
	tupMap.AddMapName("Diffuse Color");
	tupMap.AddAmount(1.0f);
	int mapFlags = TupStandardMaterialMap::ENABLED;
	tupMap.AddFlags(mapFlags);
	tupMap.AddMapReference(mapRef);

	return newMaterialRef;
}

int MaterialMapsManager::CreateMap(const ImageLayer &imageLayer)
{
	int newMapRef = m_tupMapPool.AddMap();
	TupperwareAggregate *pMapAgg = m_tupMapPool.GetMap(newMapRef);
	TupMap tupMap(pMapAgg);
	tupMap.AddName("StampLayer");
	tupMap.SetMapType(TupMap::BITMAP_MAP);

	TupBitmapMap tupBitmapMap(pMapAgg);

	int flags = 0; // clamping

	int mapping = 0;
	int mapChannel = imageLayer.m_channel;
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
	tupBitmapMap.AddBlendMode(imageLayer.m_blendMode);
	tupBitmapMap.AddFlags(flags);
	tupBitmapMap.AddImageReference(imageLayer.m_imageRef);

	return newMapRef;
}

int MaterialMapsManager::CreateMaterial(const TupArray<int> &materialStack)
{
	int newMaterialRef = m_tupMaterialPool.AddMaterial();
	TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(newMaterialRef);
	TupMaterial tupMaterial(pMaterialAgg);
	tupMaterial.SetMaterialType(TupMaterial::COMPOSITE_MATERIAL);
	tupMaterial.AddName("StampComposite");
	TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
	tupCompositeMaterial.AddBaseMaterialReference(materialStack[0]);
	int numSubMaterials = materialStack.GetSize()-1;
	for (int subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
	{
		int subMaterialRef = materialStack[subMaterialIndex+1];
		TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.AddSubMaterial(subMaterialIndex);
		TupCompositeMaterialSubMaterial tupSubMat(pSubMaterialAgg);
		tupSubMat.AddFlags(TupCompositeMaterialSubMaterial::ENABLED);
		tupSubMat.AddMaterialReference(subMaterialRef);
	}
	return newMaterialRef;
}

