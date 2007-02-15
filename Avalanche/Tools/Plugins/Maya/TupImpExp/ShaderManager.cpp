////////////////////////////////////////////////////////////////////////////
//
// ShaderManager
//
// Class to keep track of shaders for importing
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ShaderManager.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/15/03    Time: 3:38p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * uv chooser connected
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/12/03    Time: 1:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * gets shaders that are already there and uses them for import
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/26/03    Time: 4:34p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * creating shaders
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/20/03    Time: 4:12p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * shader manager
 * comparison class
 */

#include "TupImpExpPch.h"
#include "ShaderManager.h"
#include "CompareTupObjects.h"

static bool InvalidCharacter(char charToTest)
{
	// number
	if (charToTest>='0' && charToTest<='9')
		return true;
	if (charToTest == '#')
		return true;
	if (charToTest == '&')
		return true;
	if (charToTest == '$')
		return true;
	if (charToTest == '%')
		return true;
	if (charToTest == '^')
		return true;
	if (charToTest == '*')
		return true;
	if (charToTest == '+')
		return true;
	if (charToTest == '-')
		return true;
	if (charToTest == ' ')
		return true;
	return false;
}

static void FixInvalidName(TupString &name)
{
	for (int index=0;index<name.GetLength();index++)
	{
		if (InvalidCharacter(name[index]))
		{
			name.Delete(index);
			index--;
		}
	}
	if (name.GetLength()==0)
	{
		name = "Material";
	}
	name += "1";
}

ShaderManager::ShaderManager(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool) :
	m_importMaterialPool(tupMaterialPool),
	m_importMapPool(tupMapPool),
	m_importImagePool(tupImagePool)
{
	m_pMaterialPoolAgg = new TupperwareAggregate;
	m_pTupMaterialPool = new TupMaterialPool(m_pMaterialPoolAgg);

	m_pMapPoolAgg = new TupperwareAggregate;
	m_pTupMapPool = new TupMapPool(m_pMapPoolAgg);

	m_pImagePoolAgg = new TupperwareAggregate;
	m_pTupImagePool = new TupImagePool(m_pImagePoolAgg);

	int numImportMaterials = m_importMaterialPool.GetNumMaterials();
	m_importMaterialRemap.SetSize(numImportMaterials);
	for (int materialIndex=0;materialIndex<numImportMaterials;materialIndex++)
	{
		m_importMaterialRemap[materialIndex] = -1; // no maya material yet
	}

	int numImportMaps = m_importMapPool.GetNumMaps();
	m_importMapRemap.SetSize(numImportMaps);
	for (int mapIndex=0;mapIndex<numImportMaps;mapIndex++)
	{
		m_importMapRemap[mapIndex] = -1; // no maya map yet
	}
	GetExistingShaders();
}

ShaderManager::~ShaderManager()
{
	delete m_pMaterialPoolAgg;
	delete m_pTupMaterialPool;
	
	delete m_pMapPoolAgg;
	delete m_pTupMapPool;
	
	delete m_pImagePoolAgg;
	delete m_pTupImagePool;
}

void ShaderManager::AddMeshShaderData(MDagPath meshPath,const int *pFaceMaterialRefs,int numFaces)
{
	DeferedMeshShaderData newDefered;
	newDefered.m_meshPath = meshPath;
	newDefered.m_pFaceMaterialRefs = pFaceMaterialRefs;
	newDefered.m_numFaces = numFaces;
	m_deferedMeshShaders.Add(newDefered);
}

void ShaderManager::ApplyShadersToMeshes(void)
{
	int numMeshes = m_deferedMeshShaders.GetSize();
	for (int meshIndex=0;meshIndex<numMeshes;meshIndex++)
	{
		DeferedMeshShaderData &meshData = m_deferedMeshShaders[meshIndex];
		SetUpMeshShaders(meshData.m_meshPath,meshData.m_pFaceMaterialRefs,meshData.m_numFaces);
	}
	m_deferedMeshShaders.RemoveAll();
}

void ShaderManager::SetUpMeshShaders(MDagPath meshPath,const int *pFaceMaterialRefs,int numFaces)
{
	MStatus status;

	Keeper<int> uniqueMaterialRefs;
	TupArray<MObject> shadingGroups;
	TupArray<TupArray<int> > facesUsingRefs;
	TupArray<int> facesUsingNoMaterial;

	for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		int materialRef = pFaceMaterialRefs[faceIndex];
		if (materialRef!=-1)
		{
			int uniqueMaterialRefIndex = uniqueMaterialRefs.GetIndex(materialRef);
			if (uniqueMaterialRefIndex!=-1)
			{
				facesUsingRefs[uniqueMaterialRefIndex].Add(faceIndex);
			}
			else // not already done
			{
				uniqueMaterialRefs.Add(materialRef);
				MObject newShadingGroup = GetShadingGroup(materialRef);
				shadingGroups.Add(newShadingGroup);
				TupArray<int> newFaces;
				newFaces.Add(faceIndex);
				facesUsingRefs.Add(newFaces);
			}
		}
		else
		{
			facesUsingNoMaterial.Add(faceIndex);
		}
	}

	MFnMesh fnMesh(meshPath);

	MItMeshPolygon mItMeshPolygon(meshPath);

	int numShadingGroups = shadingGroups.GetSize();
	int shadingGroupIndex;
	for (shadingGroupIndex=0;shadingGroupIndex<numShadingGroups;shadingGroupIndex++)
	{
		const TupArray<int> &facesUsingRef = facesUsingRefs[shadingGroupIndex];
		int numFacesToSet = facesUsingRef.GetSize();

		MSelectionList polySelection;
		for (int faceToSetIndex=0;faceToSetIndex<numFacesToSet;faceToSetIndex++)
		{
			int prevIndex;
			mItMeshPolygon.setIndex(facesUsingRef[faceToSetIndex],prevIndex);
			MObject polyFace = mItMeshPolygon.polygon();
			polySelection.add(meshPath,polyFace);
		}
		
		MFnSet set(shadingGroups[shadingGroupIndex], &status);
		if (!status)
		{
			MGlobal::displayWarning("Could not create shader set.");
		}
		
		status = set.addMembers(polySelection);
		if (!status)
		{
			MGlobal::displayWarning(MString("Could not add poly members to shader set. ") + status.errorString());
		}
	}
	for (shadingGroupIndex=0;shadingGroupIndex<numShadingGroups;shadingGroupIndex++)
	{
		TupArray<int> importBitmapMapRefs;
		int materialRef = uniqueMaterialRefs[shadingGroupIndex];
		GetBitmapMapsFromMaterial(materialRef,importBitmapMapRefs,true);
		int numBitmapMaps = importBitmapMapRefs.GetSize();
		for (int bitmapMapIndex=0;bitmapMapIndex<numBitmapMaps;bitmapMapIndex++)
		{
			int importMapRef = importBitmapMapRefs[bitmapMapIndex];
			TupperwareAggregate *pMapAgg = m_importMapPool.GetMap(importMapRef);
			TupBitmapMap tupBitmapMap(pMapAgg);
			int mapChannel;
			tupBitmapMap.GetMapChannel(mapChannel);
			// if mapping to other than first channel
			if (mapChannel!=1)
			{
				int remappedBitmapMap = m_importMapRemap[importMapRef];
				UniqueTextureData &textureData = m_uniqueTextureData[remappedBitmapMap];
				if (!textureData.m_textureObject.isNull())
				{
					HookUVChooser(meshPath,mapChannel,textureData.m_textureObject);
				}
			}
		}
	}
}

MObject ShaderManager::GetShadingGroup(int importMaterialRef)
{
	int materialRef = GetMaterialRef(importMaterialRef);
	// is the shading group done?
	if (m_uniqueShaderData[materialRef].m_shadingGroupObject.isNull())
	{
		// create one if not
		TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
		TupMaterial tupMaterial(pMaterialAgg);
		char *pMaterialName;
		tupMaterial.GetName(&pMaterialName);
		MFnDependencyNode shaderNode( m_uniqueShaderData[materialRef].m_shaderObject);
		m_uniqueShaderData[materialRef].m_shadingGroupObject = CreateShadingGroup(shaderNode.name().asChar());
		ConnectShadingGroupToMaterial(m_uniqueShaderData[materialRef].m_shadingGroupObject,m_uniqueShaderData[materialRef].m_shaderObject);
	}
	return m_uniqueShaderData[materialRef].m_shadingGroupObject;
}

MObject ShaderManager::CreateShadingGroup(const char *pSGName)
{
	MFnSet mFnSet;
	MStatus status;

	MSelectionList selection;
	status = selection.add(MObject::kNullObj);
	
	MObject sgObject = mFnSet.create(selection, MFnSet::kRenderableOnly, &status);
	if (status!=MStatus::kSuccess) 
	{
		return MObject::kNullObj;
	}
	
	MDGModifier dgModifier;
	status = dgModifier.renameNode(sgObject,MString(pSGName)+MString("SG"));
	dgModifier.doIt();
	return sgObject;
}

void ShaderManager::ConnectShadingGroupToMaterial(MObject sgObject,MObject materialObject)
{
	MFnDependencyNode sgDepNode(sgObject);
	MFnDependencyNode materialDepNode(materialObject);

	//	Force new connection between shading group and shader, there really isn't a simpler way.
	MGlobal::executeCommand (
		MString ("defaultNavigation -quiet true -force true -connectToExisting -source ") +
		materialDepNode.name() + MString (" -destination ") + sgDepNode.name(),false);
}

int ShaderManager::GetMaterialRef(int importMaterialRef)
{
	// handle null reference
	if (importMaterialRef==-1)
		return -1;

	// if already done then return new ref
	if (m_importMaterialRemap[importMaterialRef]!=-1)
		return m_importMaterialRemap[importMaterialRef];

	// if not done then attempt to add and another material to my pools
	TupperwareAggregate *pImportMaterialAgg = m_importMaterialPool.GetMaterial(importMaterialRef);
	TupperwareAggregate *pMaterialAgg = new TupperwareAggregate;
	pMaterialAgg->CopyChildren(pImportMaterialAgg);

	TupMaterial tupMaterial(pMaterialAgg);

	switch(tupMaterial.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				TupStandardMaterial tupStandardMaterial(pMaterialAgg);
				int numSubMaps = tupStandardMaterial.GetNumMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupStandardMaterial.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap(pSubMapAgg);
					MapTypes mapType;
					char *pSubMapName;
					tupSubMap.GetMapName(&pSubMapName);
					if (!stricmp(pSubMapName,"Diffuse Color"))
						mapType = MAP_TYPE_DIFFUSE;
					else if (!stricmp(pSubMapName,"Reflection"))
						mapType = MAP_TYPE_REFLECTION;
					else
						mapType = MAP_TYPE_DIFFUSE;

					int importMapRef;
					tupSubMap.GetMapReference(importMapRef);
					int mapRef = GetMapRef(importMapRef,mapType);
					tupSubMap.AddMapReference(mapRef);
				}
			}
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				TupBlendMaterial tupBlendMaterial(pMaterialAgg);
				int importMatRef;
				int matRef;
				tupBlendMaterial.GetMaterial1Reference(importMatRef);
				matRef = GetMaterialRef(importMatRef);
				tupBlendMaterial.AddMaterial1Reference(matRef);
				tupBlendMaterial.GetMaterial2Reference(importMatRef);
				matRef = GetMaterialRef(importMatRef);
				tupBlendMaterial.AddMaterial2Reference(matRef);

				int importMapRef;
				tupBlendMaterial.GetMaskReference(importMapRef);
				int mapRef = GetMapRef(importMapRef,MAP_TYPE_DIFFUSE);
				tupBlendMaterial.AddMaskReference(mapRef);
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
				int importMatRef;
				int matRef;
				tupCompositeMaterial.GetBaseMaterialReference(importMatRef);
				matRef = GetMaterialRef(importMatRef);
				tupCompositeMaterial.AddBaseMaterialReference(matRef);

				int numSubMaterials = tupCompositeMaterial.GetNumSubMaterials();
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.GetSubMaterial(subMaterialIndex);
					TupCompositeMaterialSubMaterial subMat(pSubMaterialAgg);
					subMat.GetMaterialReference(importMatRef);
					matRef = GetMaterialRef(importMatRef);
					subMat.AddMaterialReference(matRef);
				}
			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
				int importMatRef;
				int matRef;
				tupDoubleSidedMaterial.GetFrontMaterialReference(importMatRef);
				matRef = GetMaterialRef(importMatRef);
				tupDoubleSidedMaterial.AddFrontMaterialReference(matRef);

				tupDoubleSidedMaterial.GetBackMaterialReference(importMatRef);
				matRef = GetMaterialRef(importMatRef);
				tupDoubleSidedMaterial.AddBackMaterialReference(matRef);
			}
			break;
		default:
			delete pMaterialAgg;
			return -1;
	}
	unsigned long materialCRC = CompareTupObjects::GetMaterialCRC(pMaterialAgg);
	int numMaterials = m_pTupMaterialPool->GetNumMaterials();
	for (int materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		if (materialCRC == m_uniqueShaderData[materialIndex].m_materialCRC)
		{
			TupperwareAggregate *pTestMaterial = m_pTupMaterialPool->GetMaterial(materialIndex);
			if (CompareTupObjects::CompareMaterials(pMaterialAgg,pTestMaterial))
			{
				delete pMaterialAgg; // don't need this
				m_importMaterialRemap[importMaterialRef] = materialIndex;
				return materialIndex;
			}
		}
	}
	int materialRef = m_pTupMaterialPool->AddMaterial();
	TupperwareAggregate *pNewMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
	pNewMaterialAgg->MoveChildren(pMaterialAgg); // move over to other one
	delete pMaterialAgg;

	UniqueShaderData newShaderData;
	newShaderData.m_materialCRC = materialCRC;
	newShaderData.m_shaderObject = BuildMayaShader(pNewMaterialAgg);
	m_uniqueShaderData.Add(newShaderData);
	m_importMaterialRemap[importMaterialRef] = materialRef;

	return materialRef;
}

MObject ShaderManager::BuildMayaShader(TupperwareAggregate *pMaterialAgg)
{
	TupMaterial tupMaterial(pMaterialAgg);
	char *pMaterialName;
	tupMaterial.GetName(&pMaterialName);
	TupString materialName(pMaterialName);

	FixInvalidName(materialName);

	switch(tupMaterial.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				TupStandardMaterial tupStandardMaterial(pMaterialAgg);

				MString shaderNode;

				char *pShadingTypeName;
				tupStandardMaterial.GetShadingTypeName(&pShadingTypeName);
				if (!stricmp(pShadingTypeName,"Phong"))
					MGlobal::executeCommand ( MString ("shadingNode -asShader phong -name ") + MString(materialName), shaderNode );
				if (!stricmp(pShadingTypeName,"Blinn"))
					MGlobal::executeCommand ( MString ("shadingNode -asShader blinn -name ") + MString(materialName), shaderNode );
				else
					MGlobal::executeCommand ( MString ("shadingNode -asShader blinn -name ") + MString(materialName), shaderNode );

				MObject shaderObject = MayaHelper::GetMObject(shaderNode.asChar());
				MFnReflectShader mfnReflectShader(shaderObject);
				mfnReflectShader.setSpecularColor(MColor(0.0,0.0,0.0));

				int numSubMaps = tupStandardMaterial.GetNumMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupStandardMaterial.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap(pSubMapAgg);
					char *pSubMapName;
					tupSubMap.GetMapName(&pSubMapName);

					int mapRef;
					tupSubMap.GetMapReference(mapRef);

					if (!stricmp(pSubMapName,"Diffuse Color"))
					{
						MFnDependencyNode textureNode(m_uniqueTextureData[mapRef].m_textureObject);

						MGlobal::executeCommand (
							MString ("connectAttr ") + textureNode.name() + MString (".outColor ") +
							shaderNode + ".color" );
					}
					else if (!stricmp(pSubMapName,"Reflection"))
					{
						MFnDependencyNode textureNode(m_uniqueTextureData[mapRef].m_textureObject);
						
						MGlobal::executeCommand (
							MString ("connectAttr ") + textureNode.name() + MString (".outColor ") +
							shaderNode + ".reflectedColor" );
					}
					else if (!stricmp(pSubMapName,"Transparency"))
					{
						MFnDependencyNode textureNode(m_uniqueTextureData[mapRef].m_textureObject);
						
						MGlobal::executeCommand (
							MString ("connectAttr ") + textureNode.name() + MString (".outTransparency ") +
							shaderNode + ".transparency" );
					}
				}
				return shaderObject;
			}
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				return MObject::kNullObj;
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				return MObject::kNullObj;
			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				return MObject::kNullObj;
			}
			break;
		default:
			return MObject::kNullObj;
	}

	return MObject::kNullObj;
}

int ShaderManager::GetMapRef(int importMapRef,ShaderManager::MapTypes mapType)
{
	// handle null reference
	if (importMapRef==-1)
		return -1;
	
	// if already done then return new ref
	if (m_importMapRemap[importMapRef]!=-1)
		return m_importMapRemap[importMapRef];
	
	// if not done then attempt to add and another material to my pools
	TupperwareAggregate *pImportMapAgg = m_importMapPool.GetMap(importMapRef);
	TupperwareAggregate *pMapAgg = new TupperwareAggregate;
	pMapAgg->CopyChildren(pImportMapAgg);
	
	TupMap tupMap(pMapAgg);
	
	switch (tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				TupBitmapMap tupBitmapMap(pMapAgg);
				int importImageRef;
				tupBitmapMap.GetImageReference(importImageRef);
				TupperwareAggregate *pImageAgg = m_importImagePool.GetImage(importImageRef);
				TupImage tupImage(pImageAgg);
				char *pFilePathName;
				tupImage.GetFilePathName(&pFilePathName);
				TupString filePathName(pFilePathName);
				filePathName.Replace('\\','/');
				
				int imageRef = m_imagePoolStrings.GetIndex(filePathName);
				if (imageRef==-1) // if it is not in the list
				{
					imageRef = m_imagePoolStrings.Add(filePathName);
					TupperwareAggregate *pImageAgg = m_pTupImagePool->AddImage(imageRef);
					TupImage tupImage(pImageAgg);
					tupImage.AddFilePathName(filePathName);
				}
				tupBitmapMap.AddImageReference(imageRef);
//				tupBitmapMap.AddMapChannel(1); // set to first channel (hook up uvchooser later)
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap(pMapAgg);
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap subMap(pSubMapAgg);
					int importMapRef;
					int mapRef;

					subMap.GetMapReference(importMapRef);
					mapRef = GetMapRef(importMapRef,mapType);
					subMap.AddMapReference(mapRef);
				}
			}
			break;
		default:
			delete pMapAgg;
			return -1; // no map
			break;
	}
	unsigned long mapCRC = CompareTupObjects::GetMapCRC(pMapAgg);
	int numMaps = m_pTupMapPool->GetNumMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		if (mapCRC == m_uniqueTextureData[mapIndex].m_mapCRC && mapType == m_uniqueTextureData[mapIndex].m_mapType)
		{
			TupperwareAggregate *pTestMap = m_pTupMapPool->GetMap(mapIndex);
			if (CompareTupObjects::CompareMaps(pMapAgg,pTestMap))
			{
				delete pMapAgg; // don't need this
				m_importMapRemap[importMapRef] = mapIndex;
				return mapIndex;
			}
		}
	}
	int mapRef = m_pTupMapPool->AddMap();
	TupperwareAggregate *pNewMapAgg = m_pTupMapPool->GetMap(mapRef);
	pNewMapAgg->MoveChildren(pMapAgg); // move over to other one

	delete pMapAgg;

	UniqueTextureData newTextureData;
	newTextureData.m_mapCRC = mapCRC;
	newTextureData.m_mapType = mapType;
	newTextureData.m_textureObject = BuildMayaTexture(pNewMapAgg,mapType);
	m_uniqueTextureData.Add(newTextureData);
	m_importMapRemap[importMapRef] = mapRef;

	return mapRef;
}

MObject ShaderManager::BuildMayaTexture(TupperwareAggregate *pMapAgg,ShaderManager::MapTypes mapType)
{
	TupMap tupMap(pMapAgg);
	char *pMapName;
	tupMap.GetName(&pMapName);
	TupString mapName(pMapName);

	// remove the numbers from the end
	while (mapName.GetLength() && InvalidCharacter(mapName[mapName.GetLength()-1]))
		mapName.Delete(mapName.GetLength()-1);

	mapName.Replace('#','_');
	mapName.Replace(' ','_');

	mapName += "1";

	switch (tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				TupBitmapMap tupBitmapMap(pMapAgg);
				int flags;
				tupBitmapMap.GetFlags(flags);

				int imageRef;
				tupBitmapMap.GetImageReference(imageRef);
				TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageRef);
				TupImage tupImage(pImageAgg);
				char *pFilePathName;
				tupImage.GetFilePathName(&pFilePathName);


				MString fileNode;
				MGlobal::executeCommand ( MString ("shadingNode -asTexture file -name ") + MString(mapName), fileNode );

				MGlobal::executeCommand ( MString ("setAttr -type \"string\" ") +	fileNode + 
					MString (".fileTextureName \"") + MString(pFilePathName) + MString ("\"") );



				MString placementNode;
				MGlobal::executeCommand ( MString ("shadingNode -asUtility place2dTexture"),placementNode );
				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".coverage ") +
					fileNode + MString (".coverage") );
				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".translateFrame ") +
					fileNode + MString (".translateFrame") );
				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".rotateFrame ") +
					fileNode + MString (".rotateFrame") );

				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".mirrorU ") +
					fileNode + MString (".mirrorU") );

				int uMirror = (flags & TupBitmapMap::TUP_U_MIRROR) ? 1 : 0;
				MGlobal::executeCommand (
					MString ("setAttr ") +  placementNode + MString (".mirrorU ") + uMirror);


				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".mirrorV ") +
					fileNode + MString (".mirrorV") );

				int vMirror = (flags & TupBitmapMap::TUP_V_MIRROR) ? 1 : 0;
				MGlobal::executeCommand (
					MString ("setAttr ") +  placementNode + MString (".mirrorV ") + uMirror);

				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".stagger ") +
					fileNode + MString (".stagger") );

				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".wrapU ") +
					fileNode + MString (".wrapU") );

				int uWrap = (flags & TupBitmapMap::TUP_U_WRAP) ? 1 : 0;
				MGlobal::executeCommand (
					MString ("setAttr ") +  placementNode + MString (".wrapU ") + uWrap);

				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".wrapV ") +
					fileNode + MString (".wrapV") );

				int vWrap = (flags & TupBitmapMap::TUP_V_WRAP) ? 1 : 0;
				MGlobal::executeCommand (
					MString ("setAttr ") +  placementNode + MString (".wrapV ") + vWrap);

				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".repeatUV ") +
					fileNode + MString (".repeatUV") );

				float uTiling;
				tupBitmapMap.GetUTiling(uTiling);
				MGlobal::executeCommand (
					MString ("setAttr ") +  placementNode + MString (".repeatU ") +
					MString ("-type double ") + uTiling);
				
				float vTiling;
				tupBitmapMap.GetVTiling(vTiling);
				MGlobal::executeCommand (
					MString ("setAttr ") +  placementNode + MString (".repeatV ") +
					MString ("-type double ") + vTiling);


				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".offset ") +
					fileNode + MString (".offset") );
				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".rotateUV ") +
					fileNode + MString (".rotateUV") );

				float wAngle;
				tupBitmapMap.GetWAngle(wAngle);
				MGlobal::executeCommand (
					MString ("setAttr ") +  placementNode + MString (".rotateUV ") +
					MString ("-type double ") + wAngle);

				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".outUV ") +
					fileNode + MString (".uv") );
				
				MGlobal::executeCommand (
					MString ("connectAttr ") + placementNode + MString (".outUvFilterSize ") +
					fileNode + MString (".uvFilterSize") );
				
				return MayaHelper::GetMObject(fileNode.asChar());
			}
			break;
		case TupMap::MIX_MAP:
			{
				return MObject::kNullObj;
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap(pMapAgg);

				MString layeredTextureNode;
				MGlobal::executeCommand ( MString ("shadingNode -asTexture layeredTexture -name ") + MString(mapName), layeredTextureNode );

				int currentMayaSubMap = 0;
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				for (int subMapIndex=numSubMaps-1;subMapIndex>=0;subMapIndex--)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap subMap(pSubMapAgg);
					int mapRef;
					subMap.GetMapReference(mapRef);
					if (mapRef==-1)
						continue;

					MFnDependencyNode textureNode(m_uniqueTextureData[mapRef].m_textureObject);

					MGlobal::executeCommand (
						MString ("connectAttr ") + textureNode.name() + MString (".outColor ") +
						layeredTextureNode + ".inputs[" + currentMayaSubMap + "].color" );

					int blendMode = GetMapBlendMode(mapRef);

					MGlobal::executeCommand (
						MString ("setAttr ") + layeredTextureNode + ".inputs[" + currentMayaSubMap + "].blendMode " + blendMode );

					currentMayaSubMap++;
				}

				return MayaHelper::GetMObject(layeredTextureNode.asChar());
			}
			break;
		default:
			return MObject::kNullObj;
			break;
	}
	return MObject::kNullObj;
}

int ShaderManager::GetMapBlendMode(int mapRef)
{
	int blendMode = TupBitmapMap::BLEND_MODE_OVER;
	TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapRef);
	if (pMapAgg)
	{
		TupMap tupMap(pMapAgg);
		if (tupMap.GetMapType() == TupMap::BITMAP_MAP)
		{
			TupBitmapMap tupBitmapMap(pMapAgg);
			tupBitmapMap.GetBlendMode(blendMode);
		}
	}
	return blendMode;
}

void ShaderManager::GetExistingShaders(void)
{
	MStatus status;

	MItDependencyNodes iter( MFn::kShadingEngine);

	for ( ; !iter.isDone(); iter.next() ) 
	{
		MObject shadingGroup = iter.item();
		MFnDependencyNode shadingGroupNode(shadingGroup);
		
		TupperwareAggregate *pMaterialAgg = new TupperwareAggregate;

		TupMaterial tupMaterial(pMaterialAgg);
		tupMaterial.SetMaterialType(TupMaterial::STANDARD_MATERIAL);
		AddStandardMaterial(pMaterialAgg,shadingGroup);

		bool bMaterialFound = false;

		unsigned long materialCRC = CompareTupObjects::GetMaterialCRC(pMaterialAgg);
		int numMaterials = m_pTupMaterialPool->GetNumMaterials();
		for (int materialIndex=0;materialIndex<numMaterials;materialIndex++)
		{
			if (materialCRC == m_uniqueShaderData[materialIndex].m_materialCRC)
			{
				TupperwareAggregate *pTestMaterial = m_pTupMaterialPool->GetMaterial(materialIndex);
				if (CompareTupObjects::CompareMaterials(pMaterialAgg,pTestMaterial))
				{
					bMaterialFound = true;
					break;
				}
			}
		}
		if (bMaterialFound)
		{
			delete pMaterialAgg; // don't need this
			continue;
		}
		int materialRef = m_pTupMaterialPool->AddMaterial();
		TupperwareAggregate *pNewMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
		pNewMaterialAgg->MoveChildren(pMaterialAgg); // move over to other one
		delete pMaterialAgg;

		UniqueShaderData newShaderData;
		newShaderData.m_materialCRC = materialCRC;
		newShaderData.m_shadingGroupObject = shadingGroup;
		newShaderData.m_shaderObject = MObject::kNullObj;

		MPlug fnPlug = shadingGroupNode.findPlug("surfaceShader",&status);
		if (status == MS::kSuccess)
		{
			MPlugArray connectedNodes;
			fnPlug.connectedTo(connectedNodes,true,false,&status);
			if (status == MS::kSuccess)
			{
				if (connectedNodes.length() !=0)
				{
					newShaderData.m_shaderObject = connectedNodes[0].node();
				}
			}
		}
		m_uniqueShaderData.Add(newShaderData);
	}
}

void ShaderManager::AddStandardMaterial(TupperwareAggregate *pMaterialAgg,const MObject &shadingGroup)
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
						int mapRef = AddMap(colorNode,MAP_TYPE_DIFFUSE,blendMode);
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
						int mapRef = AddMap(reflectedColorNode,MAP_TYPE_REFLECTION,blendMode);
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
						int mapRef = AddMap(transparencyNode,MAP_TYPE_DIFFUSE,blendMode);
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

int ShaderManager::AddMap(MObject mapNode,ShaderManager::MapTypes mapType, int blendMode)
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
				AddCompositeMap(pMapAgg,mapNode,mapType,blendMode);
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
						return AddMap(imageNode,mapType,blendMode);
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

	int numMaps = m_pTupMapPool->GetNumMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		if (mapCRC==m_uniqueTextureData[mapIndex].m_mapCRC && mapType == m_uniqueTextureData[mapIndex].m_mapType)
		{
			TupperwareAggregate *pTestMapAgg = m_pTupMapPool->GetMap(mapIndex);
			if (CompareTupObjects::CompareMaps(pMapAgg,pTestMapAgg))
			{
				delete pMapAgg; // we don't need this
				return mapIndex; // if found another like me then return it
			}
		}
	}
	int mapRef = m_pTupMapPool->AddMap();
	TupperwareAggregate *pNewMapAgg = m_pTupMapPool->GetMap(mapRef);
	pNewMapAgg->MoveChildren(pMapAgg); // move over to other one
	delete pMapAgg;

	UniqueTextureData newTextureData;
	newTextureData.m_mapType = mapType;
	newTextureData.m_mapCRC = mapCRC;
	newTextureData.m_textureObject = mapNode;
	m_uniqueTextureData.Add(newTextureData);
	return mapRef;
}

void ShaderManager::AddBitmapMap(TupperwareAggregate *pMapAgg,MObject mapNode,int blendMode)
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


	mapChannel = 1; // GetMapChannel(mapNode);
	
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

void ShaderManager::AddCompositeMap(TupperwareAggregate *pMapAgg,MObject mapNode,ShaderManager::MapTypes mapType,int blendMode)
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
				colorMapRef = AddMap(colorNode,mapType,blendMode);
			}
		}
		
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

int ShaderManager::AddImage(const char *mapName)
{
	TupString fullPath(mapName);
	fullPath.MakeLower(); // not case sensitive
	
	int index = m_imagePoolStrings.GetIndex(fullPath);
	if (index==-1) // if it is not in the list
	{
		index = m_imagePoolStrings.Add(fullPath);
		TupperwareAggregate *pImageAgg = m_pTupImagePool->AddImage(index);
		TupImage tupImage(pImageAgg);
		tupImage.AddFilePathName(fullPath);
	}
	return index;
}

void ShaderManager::GetBitmapMapsFromMaterial(int materialRef,TupArray<int> &mapRefArray,bool bFromImportPools)
{
	TupperwareAggregate *pMaterialAgg;
	if (bFromImportPools)
		pMaterialAgg = m_importMaterialPool.GetMaterial(materialRef);
	else
		pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);

	TupMaterial tupMaterial(pMaterialAgg);

	switch(tupMaterial.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				TupStandardMaterial tupStandardMaterial(pMaterialAgg);
				int numSubMaps = tupStandardMaterial.GetNumMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupStandardMaterial.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap(pSubMapAgg);
					int mapRef;
					tupSubMap.GetMapReference(mapRef);
					if (mapRef!=-1)
					{
						GetBitmapMapsFromMap(mapRef,mapRefArray,bFromImportPools);
					}
				}
			}
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				TupBlendMaterial tupBlendMaterial(pMaterialAgg);
				int matRef;
				tupBlendMaterial.GetMaterial1Reference(matRef);
				GetBitmapMapsFromMaterial(matRef,mapRefArray,bFromImportPools);

				tupBlendMaterial.GetMaterial2Reference(matRef);
				GetBitmapMapsFromMaterial(matRef,mapRefArray,bFromImportPools);

				int mapRef;
				tupBlendMaterial.GetMaskReference(mapRef);
				if (mapRef!=-1)
				{
					GetBitmapMapsFromMap(mapRef,mapRefArray,bFromImportPools);
				}
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
				int matRef;
				tupCompositeMaterial.GetBaseMaterialReference(matRef);
				GetBitmapMapsFromMaterial(matRef,mapRefArray,bFromImportPools);

				int numSubMaterials = tupCompositeMaterial.GetNumSubMaterials();
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.GetSubMaterial(subMaterialIndex);
					TupCompositeMaterialSubMaterial subMat(pSubMaterialAgg);
					subMat.GetMaterialReference(matRef);
					GetBitmapMapsFromMaterial(matRef,mapRefArray,bFromImportPools);
				}
			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
				int matRef;
				tupDoubleSidedMaterial.GetFrontMaterialReference(matRef);
				GetBitmapMapsFromMaterial(matRef,mapRefArray,bFromImportPools);

				tupDoubleSidedMaterial.GetBackMaterialReference(matRef);
				GetBitmapMapsFromMaterial(matRef,mapRefArray,bFromImportPools);
			}
			break;
	}
}

void ShaderManager::GetBitmapMapsFromMap(int mapRef,TupArray<int> &mapRefArray,bool bFromImportPools)
{
	TupperwareAggregate *pMapAgg;
	if (bFromImportPools)
		pMapAgg = m_importMapPool.GetMap(mapRef);
	else
		pMapAgg = m_pTupMapPool->GetMap(mapRef);

	TupMap tupMap(pMapAgg);
	
	switch (tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				mapRefArray.Add(mapRef);
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap(pMapAgg);
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap subMap(pSubMapAgg);
					int subMapRef;
					subMap.GetMapReference(subMapRef);
					if (subMapRef!=-1)
					{
						GetBitmapMapsFromMap(subMapRef,mapRefArray,bFromImportPools);
					}
				}
			}
			break;
	}
}

void ShaderManager::HookUVChooser(MDagPath &meshPath,int mapChannel,MObject textureObject)
{
	MStatus status;
		
	MFnMesh fnMesh(meshPath, &status);
	if (!status)
		return;
		
	MFnDependencyNode textureNode(textureObject);
		
	MPlug uvCoordPlug = textureNode.findPlug("uvCoord",&status);
	if (!status)
		return;
		
	MPlugArray connectedNodes;
	uvCoordPlug.connectedTo(connectedNodes,true,false,&status);
	if (!status)
		return;
		
	if (connectedNodes.length()==0)
		return;
		
	MString placementNodeName;
	if (connectedNodes[0].node().hasFn(MFn::kPlace2dTexture))
	{
		MObject placementNodeObject = connectedNodes[0].node();
		MFnDependencyNode depNode(placementNodeObject);
		placementNodeName = depNode.name();
		
		MPlug uvCoordPlug = depNode.findPlug("uvCoord",&status);
		if (!status)
			return;
		
		uvCoordPlug.connectedTo(connectedNodes,true,false,&status);
		if (!status)
			return;
	}
	else
	{
		return;
	}

	MObject uvChooser;
		
	// if uv chooser already present
	if (connectedNodes.length()!=0 && connectedNodes[0].node().hasFn(MFn::kUvChooser))
	{
		uvChooser = connectedNodes[0].node();
	}
	else // create one
	{
		MString uvChooserNodeName;
		MGlobal::executeCommand("createNode \"uvChooser\"",uvChooserNodeName);
		
		//	Build UV Chooser node connections
		MGlobal::executeCommand(MString ("connectAttr ") + uvChooserNodeName + MString (".outUv ") + placementNodeName + MString (".uvCoord") );
		MGlobal::executeCommand(MString ("connectAttr ") + uvChooserNodeName + MString (".outVertexCameraOne ") + placementNodeName + MString (".vertexCameraOne") );
		MGlobal::executeCommand(MString ("connectAttr ") + uvChooserNodeName + MString (".outVertexUvOne ") + placementNodeName + MString (".vertexUvOne") );
		MGlobal::executeCommand(MString ("connectAttr ") + uvChooserNodeName + MString (".outVertexUvTwo ") + placementNodeName + MString (".vertexUvTwo") );
		MGlobal::executeCommand(MString ("connectAttr ") + uvChooserNodeName + MString (".outVertexUvThree ") + placementNodeName + MString (".vertexUvThree") );
		uvChooser = MayaHelper::GetMObject(uvChooserNodeName.asChar());
	}

	MFnDependencyNode uvChooserDepNode(uvChooser);

	MPlug uvsetPlug = uvChooserDepNode.findPlug("uvs",&status);
	int chooserIndex = MayaHelper::GetNextAvailableLogicalIndex(uvsetPlug);

	MGlobal::executeCommand(MString ("connectAttr ") + meshPath.fullPathName() + MString(".uvst[") + (mapChannel-1) + MString("].uvsn ") + uvChooserDepNode.name() + MString (".uvs[") + chooserIndex + MString("]"));
}