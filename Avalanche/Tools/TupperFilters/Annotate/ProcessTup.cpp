////////////////////////////////////////////////////////////////////////////
//
// ProcessTup
//
// Scan through and add additional data to tupperware tree
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "AnnotatePch.h"
#include "ProcessTup.h"

ProcessTup::ProcessTup(const FilterOptions &options,FilterDataManager &dataManager,bool bProcessStep) : m_options(options), m_dataManager(dataManager) ,
	m_totalBoneMeshes(0),m_totalPaths(0),m_bForceDynamicTextures(false),m_bBuildPartitions(false),
	m_bOptimizeUVs(false),m_platform("undefined"),m_receiveShadowNormalThreshold(-1.0f),
	m_bAdjustVertexColorGamma(false),m_redVertexColorGamma(1.0f),m_greenVertexColorGamma(1.0f),m_blueVertexColorGamma(1.0f),
	m_bGlobalMipMapping(false),m_bGlobalMipMapCrossFade(false),m_globalNumMipMapSubMaps(0),m_globalMipMapDistance(100.0f),
	m_bLoadAuxMapAttributes(false),m_bSaveAuxImageAttributes(false),m_bSetTransparentAndObscuredStageFacesAsHidden(true),
	m_bForceBonesToAttachPoints(false),m_bRotateTallImages(true),m_bRemapDuplicateImages(true)
{
	m_pSceneAgg = m_dataManager.GetSceneRoot();
	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);

	char *pVersionString;
	m_pTupScene->GetVersion(&pVersionString);
	if (!pVersionString)
	{
		throw(PluginException(ERROR_FATAL,"Database Version not available in file %s.\r\nPlease export with updated TupExport plugin.",dataManager.GetInputFilename()));
	}
	// if the version does not match
	else if (strcmp(pVersionString,TupScene::GetSceneVersion()))
	{
		throw(PluginException(ERROR_FATAL,"Database Version in file %s is %s should be %s.\r\nPlease export with updated TupExport plugin.",dataManager.GetInputFilename(),pVersionString,TupScene::GetSceneVersion()));
	}

	m_pNodePoolAgg = m_pTupScene->GetNodePool();
	assert(m_pNodePoolAgg);
	m_pTupNodePool = new TupNodePool(m_pNodePoolAgg);

	m_pTupScene->GetRootNodeReference(m_rootNodeReference);
	assert(m_rootNodeReference>=0);
	m_pRootNodeAgg = m_pTupNodePool->GetNode(m_rootNodeReference);
	assert(m_pRootNodeAgg);
	m_pTupRootNode = new TupNode(m_pRootNodeAgg);

	m_pObjectPoolAgg = m_pTupScene->GetObjectPool();
	assert(m_pObjectPoolAgg);
	m_pTupObjectPool = new TupObjectPool( m_pObjectPoolAgg );

	m_pModifierPoolAgg = m_pTupScene->GetModifierPool();
	assert(m_pModifierPoolAgg);
	m_pTupModifierPool = new TupModifierPool( m_pModifierPoolAgg );

	m_pMaterialPoolAgg = m_pTupScene->GetMaterialPool();
	assert(m_pMaterialPoolAgg);
	m_pTupMaterialPool = new TupMaterialPool( m_pMaterialPoolAgg );

	m_pMapPoolAgg = m_pTupScene->GetMapPool();
	assert(m_pMapPoolAgg);
	m_pTupMapPool = new TupMapPool( m_pMapPoolAgg );

	m_pImagePoolAgg = m_pTupScene->GetImagePool();
	assert(m_pImagePoolAgg);
	m_pTupImagePool = new TupImagePool( m_pImagePoolAgg );

	m_pTupNodeConfigRoot = new TupConfigRoot;
	// Node Attribute Tree
	TupperwareAggregate *pSceneNodeAttributeTreeAgg = m_pTupScene->GetNodeAttributeTree();
	if (pSceneNodeAttributeTreeAgg)
	{
		m_pTupNodeConfigRoot->BuildConfigTree(pSceneNodeAttributeTreeAgg);
	}
	else if (bProcessStep)
	{
		throw(PluginException(ERROR_FATAL,"Node Attribute Tree Does not Exist!"));
	}

	m_pTupFaceConfigRoot = new TupConfigRoot;
	// Face Attribute Tree
	TupperwareAggregate *pSceneFaceAttributeTreeAgg = m_pTupScene->GetFaceAttributeTree();
	if (pSceneFaceAttributeTreeAgg)
	{
		m_pTupFaceConfigRoot->BuildConfigTree(pSceneFaceAttributeTreeAgg);
	}
	else if (bProcessStep)
	{
		throw(PluginException(ERROR_FATAL,"Face Attribute Tree Does not Exist!"));
	}

	m_pTupMapConfigRoot = new TupConfigRoot;
	// Face Attribute Tree
	TupperwareAggregate *pSceneMapAttributeTreeAgg = m_pTupScene->GetMapAttributeTree();
	if (pSceneMapAttributeTreeAgg)
	{
		m_pTupMapConfigRoot->BuildConfigTree(pSceneMapAttributeTreeAgg);
	}
	else if (bProcessStep)
	{
		throw(PluginException(ERROR_FATAL,"Map Attribute Tree Does not Exist!"));
	}

}

ProcessTup::~ProcessTup()
{
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupRootNode;
	delete m_pTupObjectPool;
	delete m_pTupModifierPool;
	delete m_pTupMaterialPool;
	delete m_pTupMapPool;
	delete m_pTupImagePool;
	delete m_pTupNodeConfigRoot;
	delete m_pTupFaceConfigRoot;
	delete m_pTupMapConfigRoot;
}

void ProcessTup::GetFileDependencies(FilePathArray &filePathArray)
{
	AddImageFileDependencies(filePathArray);
}

void ProcessTup::AddImageFileDependencies(FilePathArray &filePathArray)
{
	MaterialImageRefsManager materialImageRefsManager(*m_pTupMaterialPool,*m_pTupMapPool);
	TupArray<int> usedImages;
	int numImages=m_pTupImagePool->GetNumImages();
	usedImages.SetSize(numImages);
	int imageIndex;

	// start with all images not used
	for (imageIndex=0;imageIndex<numImages;imageIndex++)
		usedImages[imageIndex] = 0;

	int totalMaterials = m_pTupMaterialPool->GetNumMaterials();
	for(int materialIndex=0;materialIndex<totalMaterials;materialIndex++)
	{
		TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialIndex);

		// if empty slot - continue
		if (!pMaterialAgg)
			continue;

		TupMaterial tupMaterial(pMaterialAgg);
		if (tupMaterial.GetMaterialType() == TupMaterial::STANDARD_MATERIAL)
		{
			TupArray<MaterialImageRefsManager::MaterialImageData> imageRefs;
			materialImageRefsManager.BuildImageRefs(materialIndex,imageRefs);
			// increment the usedImages array
			for (imageIndex=0;imageIndex<imageRefs.GetSize();imageIndex++)
				usedImages[imageRefs[imageIndex].m_imageRef]++;
		}
	}
	for (imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		if (usedImages[imageIndex])
		{
			TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
			assert(pImageAgg);
			TupImage tupImage(pImageAgg);
			char *pImageFileName;
			// get filename for image
			tupImage.GetFilePathName(&pImageFileName);
			// get image data from image manager
			ImageManager &imageManager = m_dataManager.GetImageManager();
			imageManager.GetImageFileDependencies(pImageFileName,filePathArray);
		}
	}
}

void ProcessTup::Process(int pass)
{
	switch (pass)
	{
		case 1:
			{
				CopySceneNodeSettings();
				ChangeNodeNames();
				AddObjectMaterialLists();
				CheckMeshIntegrity();
				OneMaterialPerMesh();
				FixMeshMaterialIndices();
				ForceBonesToAttachPoints();
				FixRogueModifiers();

				ConsolidateMeshMaps(); // called before RemoveDuplicateMaps
				RemoveDuplicateMaps();
				RemoveDuplicateMaterials();
				ConsolidateFaceMaterials(); // called after RemoveDuplicateMaterials

				AddAuxMapAttributes();
				PropagateMapAttributesToImages();
				SaveAuxImageAttributes();

				// this should be the first to load the images
				LoadImageData(); // make sure PropagateMapAttributesToImages is called before me
				AddImageAttributes();
				AddMapAttributes();

            if (m_bRotateTallImages)
				   RotateTallImages();
				ScaleNonPowerOf2Images();
				ScaleNarrowImages();
				FixDiffuseMapImages();
				AddImageData();
            if (m_bRemapDuplicateImages)
            {
               RemapDuplicateImages();
               RemapDuplicateAnimatedFrames();
            }

				if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));
			}
			break;
		case 2:
			{
				BuildReplacementModels();
				AddParentObjectReferences();
				AddParentNodeReferences();
				AddBaseObjectParentNodeReferences();

				AddMaterialHasTextureFlags(); // set up a flag for all the standard materials if they have a texture or not
				AddObjectModelAttributes();

				SetFaceRenderAttributes();
				SetMeshSubMaterialEnvironmentMapData();

				PropagateClothModifiers();
				AddClothMeshFlags();

				RemapVertexBones();
				PropagateSkinModifiers();

				RemovePinkVertexColors();
				AdjustVertexColorGamma();

				bool bForceDefault = false; // start with not forcing colors
				// if we are forcing bones to attach point we need to force
				if (m_bForceBonesToAttachPoints)
					bForceDefault = true;
				PropagateVertexColors(bForceDefault);
				PropagateVertexAlpha(bForceDefault);

				PropagateMultiTexAlpha();
				AddVertexColorAndAlphaRefsToMeshAndNodes();

				SetTransparentAndObscuredStageFacesAsHidden();

				MarkEnvInstances();

				AddNodePathIndexes();	// this calculates the total paths in the scene too
				
				// visibility sets
				SetUpVisibilitySets();
				AddVisibilitySetRefToInstances();
				AdjustVisibilitySetBoundingBoxes();
				AddCollisionInstanceIndexes();
				AddPartionDataToObjectsAndInstances();

				MarkUsedMaterialsMapsAndImages();
				DeleteUnusedMaterialsMapsAndImages();
				
				AddTextureIndexes();
				AddTextureContextIndexes();

				AddMeshAnimatedTextureInfo();
				AddNodeAnimatedTextureInfo();

				SetMeshRecieveNoShadowFlags();
			
				AddMeshImageRefs();
				SetUpTextureSets();
				MergeImagePalettes();
				AnalyzePaletteColors();
				AddAnimatedImagesToLog();
				AddSortedImagesToLog();
				AddConsolidatedInfoToLog();
				//AddTextureCombiningInfoToLog();

				if (m_bOptimizeUVs)
				{
					OptimizeUVs();
				}

				// set the number of sets in the world
				m_pSceneAgg->AddListInt("Annotate_VisibilitySetNodeRefs",NULL,m_visibilitySetNodeRefs);
				// store the total number of meshes that will get saved to the DBL file
				m_pSceneAgg->AddListInt("Annotate_TotalSOMS",NULL,m_totalSOMS);
				// store the total number of meshes that will get saved to the DBL file
				m_pSceneAgg->AddScalarInt("Annotate_TotalBoneMeshes",NULL,m_totalBoneMeshes);
				// store the total number of texture contexts that will get saved to the DBL file
				m_pSceneAgg->AddListInt("Annotate_TotalTextureContexts",NULL,m_totalTextureContexts);
				// store the total number of textures that will get saved to the DBL file
				m_pSceneAgg->AddListInt("Annotate_TotalTextures",NULL,m_totalTextures);
				// store the total number of instances that will get saved to the DBL file
				m_pSceneAgg->AddListInt("Annotate_TotalRenderInstances",NULL,m_totalRenderInstances);
				// store the total number of instances that will get used in the collision filters
				m_pSceneAgg->AddListInt("Annotate_TotalCollisionInstances",NULL,m_totalCollisionInstances);
				// store the total number of paths that will get saved to the DBL file
				m_pSceneAgg->AddScalarInt("Annotate_TotalPaths",NULL,m_totalPaths);

				if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));
			}
			break;
		default:
			throw(PluginException(ERROR_FATAL,"Unknown Pass Value %d",pass));
	}
}

void ProcessTup::SetForceDynamicTextures(bool bValue)
{
	m_bForceDynamicTextures = bValue;
}

void ProcessTup::SetBuildPartitions(bool bValue)
{
	m_bBuildPartitions = bValue;
}

void ProcessTup::SetOptimizeUVs(bool bValue)
{
	m_bOptimizeUVs = bValue;
}

void ProcessTup::SetClearTwoSidedFaceFlags(bool bValue)
{
	m_bClearTwoSidedFaceFlags = bValue;
}

void ProcessTup::SetRemovePickVertexColors(bool bValue)
{
	m_bRemovePinkVertexColors = bValue;
}

void ProcessTup::SetPlatform(const char *pPlatform)
{
	m_platform = pPlatform;
}

void ProcessTup::SetReceiveShadowNormalThreshold(float threshold)
{
	m_receiveShadowNormalThreshold = threshold;
}

void ProcessTup::SetAdjustVertexColorGamma(bool bValue)
{
	m_bAdjustVertexColorGamma = bValue;
}

void ProcessTup::SetRedVertexColorGamma(float value)
{
	m_redVertexColorGamma = value;
}

void ProcessTup::SetGreenVertexColorGamma(float value)
{
	m_greenVertexColorGamma = value;
}

void ProcessTup::SetBlueVertexColorGamma(float value)
{
	m_blueVertexColorGamma = value;
}

void ProcessTup::SetGlobalMipMapping(bool bValue)
{
	m_bGlobalMipMapping = bValue;
}

void ProcessTup::SetGlobalMipMapCrossFade(bool bValue)
{
	m_bGlobalMipMapCrossFade = bValue;
}

void ProcessTup::SetGlobalNumMipMapSubMaps(int value)
{
	m_globalNumMipMapSubMaps = value;
}

void ProcessTup::SetGlobalMipMapDistance(float value)
{
	m_globalMipMapDistance = value;
}

void ProcessTup::SetLoadAuxMapAttributes(bool bValue)
{
	m_bLoadAuxMapAttributes = bValue;
}

void ProcessTup::SetSaveAuxImageAttributes(bool bValue)
{
	m_bSaveAuxImageAttributes = bValue;
}

void ProcessTup::SetTransparentAndObscuredStageFacesAsHiddenValue(bool bValue)
{
	m_bSetTransparentAndObscuredStageFacesAsHidden = bValue;
}

void ProcessTup::SetForceBonesToAttachPoints(bool bValue)
{
	m_bForceBonesToAttachPoints = bValue;
}

void ProcessTup::SetRotateTallImages(bool bValue)
{
   m_bRotateTallImages = bValue;
}

void ProcessTup::SetRemapDuplicateImages(bool bValue)
{
   m_bRemapDuplicateImages = bValue;
}


// build a list of materials that are used for each mesh
// if more than one material is used for a mesh we need to make more than one model in the dbl file
void ProcessTup::AddObjectMaterialLists(void)
{
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int materialRef;
		tupNode.GetMaterialReference(materialRef);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef!=-1)
		{
			AddObjectMaterialLists(objectRef,materialRef);
		}
	}
}

void ProcessTup::AddObjectMaterialLists(int objectRef,int materialRef)
{
	// find the base mesh for this object
	TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
	if (pBaseMeshAgg)
	{
		// update the number of materials for the object
		TupperwareList *pMaterialList = pBaseMeshAgg->FindListByKey("Annotate_MaterialList");
		if (pMaterialList)
		{
			// we there is already a list
			// check to see if the materialref is already there
			// if not add it to the list
			int numEntries = pMaterialList->GetLength();
			int *pMaterialListData = pMaterialList->GetAsInt();
			for (int i=0;i<numEntries;i++)
			{
				if (pMaterialListData[i]==materialRef)
					break;
			}
			// if we didn't find it
			if (i==numEntries)
			{
				// copy current list to another and add the new one
				int *pNewMaterialListData = new int[numEntries+1];
				for (i=0;i<numEntries;i++)
				{
					pNewMaterialListData[i] = pMaterialListData[i];
				}
				pNewMaterialListData[numEntries] = materialRef;
				// pass to tree (tree will manage deleting the old one and managing the new one)
				pMaterialList->SetAsInt(pNewMaterialListData,numEntries+1);
				delete pNewMaterialListData;
			}
		}
		else
		{
			// if no list then start with out materialref
			int *pNewMaterialListData = new int[1];
			pNewMaterialListData[0] = materialRef;
			pBaseMeshAgg->AddListInt("Annotate_MaterialList",NULL,pNewMaterialListData,1);
			delete pNewMaterialListData;
		}
	}
}

// set up visibility set information
void ProcessTup::SetUpVisibilitySets(void)
{
	// keep track of set info for linking them together
	TupArray<TupString> visibilitySetLinkStrings;
	TupArray<bool> visibilitySetCanSeeRestOfWorldFlags;

	Keeper<TupString> setNames;
	TupArray<int> nameIndexes;

	// set all arrays to account for the world set
	m_visibilitySetNodeRefs.SetSize(1);
	visibilitySetLinkStrings.SetSize(1);
	visibilitySetCanSeeRestOfWorldFlags.SetSize(1);
	m_visibilitySetBoundingBoxes.SetSize(1);
	m_visibilitySetOBBs.SetSize(1);
	m_visibilitySetNodeNames.SetSize(1);

	// add all the others to the list
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		
		// if empty slot - continue
		if (!pNodeAgg)
			continue;
		
		//-- check to see if instance_type is visibility_region
		TupNode tupNode( pNodeAgg );
		char *pNodeAttributesString;
		tupNode.GetUserProperties( &pNodeAttributesString );

		TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_OBJ_BLOCK, END_OBJ_BLOCK );
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey( "instance_type" );
		tupKeyValueContext.AddKey( "linked_nodes" );
		tupKeyValueContext.AddKey( "rest_of_world_region" );
		tupKeyValueContext.AddKey( "realtime_camera_region" );

		const int INSTANCE_TYPE = 0;
		const int LINKED_NODES = 1;
		const int REST_OF_WORLD_REGION = 2;
		const int REALTIME_CAMERA_REGION = 3;
		m_pTupNodeConfigRoot->GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );
		
		if ( tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp( tupKeyValueContext.GetValueAsString(INSTANCE_TYPE), "visibility_region" ) == 0 )
		{
			int myIndex = m_visibilitySetNodeRefs.GetSize(); // out index is the current size (not added yet)
			// add the node name to the keeper
			char *pNodeName;
			tupNode.GetName(&pNodeName);
			TupString nameString(pNodeName);
			int setNamesSize = setNames.GetSize();
			// keep the index array in sync with the keeper
			int setNamesIndex = setNames.Add(nameString);
			if (setNamesIndex==setNamesSize) // if I added a new entry
			{
				nameIndexes.Add(myIndex);
			}
			m_visibilitySetNodeRefs.Add(nodeIndex);

			pNodeAgg->AddScalarInt("Annotate_VisibilitySetIndex",NULL,myIndex);

			TupString linkString;
			//-- get list of other regions we can see.
			if ( tupKeyValueContext.ValueValidAt(LINKED_NODES))
			{
				linkString = tupKeyValueContext.GetValueAsString(LINKED_NODES);
			}
			visibilitySetLinkStrings.Add(linkString);
			
			bool bSeeRestOfWorld = false;
			//-- mark if the region can see the rest of the world
			if ( tupKeyValueContext.ValueValidAt(REST_OF_WORLD_REGION))
			{
				bSeeRestOfWorld = tupKeyValueContext.GetValueAsBool(REST_OF_WORLD_REGION );
			}
			visibilitySetCanSeeRestOfWorldFlags.Add(bSeeRestOfWorld);

			bool bRealtimeCameraRegion = true;
			// mark if the region is checked by the camera in the engine
			if ( tupKeyValueContext.ValueValidAt(REALTIME_CAMERA_REGION))
			{
				bRealtimeCameraRegion = tupKeyValueContext.GetValueAsBool(REALTIME_CAMERA_REGION );
			}
			pNodeAgg->AddScalarInt("Annotate_RealtimeCameraRegion",NULL,bRealtimeCameraRegion ? TRUE : FALSE);

			// add bounding box
			ts_BoundingBox boundingBox;
			boundingBox.Min = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat();
			boundingBox.Max = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat();
			m_visibilitySetBoundingBoxes.Add(boundingBox);

			// add OBB
			OBBInstance obbInstance;
			obbInstance.Orientation() = *(DirCos3x3 *)pNodeAgg->FindListByKey("Bounding_OBBOrientation")->GetAsFloat();
			obbInstance.Position() = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBCenter")->GetAsFloat();
			obbInstance.GetAABB().maxV = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBExtents")->GetAsFloat();
			obbInstance.GetAABB().minV = - obbInstance.GetAABB().maxV;
			m_visibilitySetOBBs.Add(obbInstance);

			// add name
			m_visibilitySetNodeNames.Add(nameString);
		}
	}
	// set up the links for the sets
	int numVisibilitySets = m_visibilitySetNodeRefs.GetSize();

	int setIndex;
	// realign the pointers inside the obbs to point to the correct data
	for (setIndex=0;setIndex<numVisibilitySets;setIndex++)
	{
		m_visibilitySetOBBs[setIndex].AlignBase();
	}

	// create the world set
	int worldVisibilitySetRef = CreateWorldVisibilitySet();

	// set up world set and ref
	m_visibilitySetNodeRefs[0] = worldVisibilitySetRef;

	TupperwareAggregate *pWorldVisibilitySetAgg = m_pTupNodePool->GetNode(worldVisibilitySetRef);

	ts_BoundingBox boundingBox;
	boundingBox.Min = *(Vector3 *)pWorldVisibilitySetAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat();
	boundingBox.Max = *(Vector3 *)pWorldVisibilitySetAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat();
	m_visibilitySetBoundingBoxes[0] = boundingBox;
	
	// add OBB
	OBBInstance obbInstance;
	obbInstance.Orientation() = *(DirCos3x3 *)pWorldVisibilitySetAgg->FindListByKey("Bounding_OBBOrientation")->GetAsFloat();
	obbInstance.Position() = *(Vector3 *)pWorldVisibilitySetAgg->FindListByKey("Bounding_OBBCenter")->GetAsFloat();
	obbInstance.GetAABB().maxV = *(Vector3 *)pWorldVisibilitySetAgg->FindListByKey("Bounding_OBBExtents")->GetAsFloat();
	obbInstance.GetAABB().minV = - obbInstance.GetAABB().maxV;
	m_visibilitySetOBBs[0] = obbInstance;

	m_visibilitySetNodeNames[0] = "_REST_OF_WORLD_";

	TupArray<int> worldVisiblitySetLinks;

	for (setIndex=1;setIndex<numVisibilitySets;setIndex++)
	{
		TupArray<int> visibilitySetLinks; // holds the indexes to sets I can see

		worldVisiblitySetLinks.Add(setIndex); // add me to the world

		TupArray<TupString> checkedRegionList;
		//-- break the string into the list of which names are checked
		TupConfigCheckList::ParseStrForCheckedNames( checkedRegionList, visibilitySetLinkStrings[setIndex]);
		for (int listIndex=0;listIndex<checkedRegionList.GetSize();listIndex++)
		{
			int linkNameIndex = setNames.GetIndex(checkedRegionList[listIndex]);
			// if found in the keeper of names and it is not me
			if (linkNameIndex!=-1 && nameIndexes[linkNameIndex]!=setIndex)
			{
				// add ref of linked set
				visibilitySetLinks.Add(nameIndexes[linkNameIndex]);
			}
		}
		// add in the world if I can see it
		if (visibilitySetCanSeeRestOfWorldFlags[setIndex])
		{
			visibilitySetLinks.Add(0);
		}

		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(m_visibilitySetNodeRefs[setIndex]);
		pNodeAgg->AddListInt("Annotate_VisibilitySetLinks",NULL,visibilitySetLinks);
	}
	// add in the links for the world set
	pWorldVisibilitySetAgg->AddListInt("Annotate_VisibilitySetLinks",NULL,worldVisiblitySetLinks);
}

// called from SetUpVisibilitySets
int ProcessTup::CreateWorldVisibilitySet(void)
{
	TupperwareAggregate *pRootNodeAgg = m_pTupNodePool->GetNode(m_rootNodeReference);
	TupNode tupRootNode(pRootNodeAgg);
	
	int nodeIndex = m_pTupNodePool->AddNode(); // add a new node to the pool
	tupRootNode.AddChildNodeReference(nodeIndex); // make a child of the root node
	
	TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
	TupNode tupNode(pNodeAgg);
	tupNode.AddObjectReference(-1);
	tupNode.AddMaterialReference(-1);
	TupperwareAggregate *pTransformAgg = tupNode.AddTransform();
	TupTransform tupTransform(pTransformAgg);
	tupTransform.SetTransformType(TupTransform::STATIC_MATRIX);
	TupStaticMatrix tupStaticMatrix(pTransformAgg);
	float mat[16];
	mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
	mat[1] = mat[2] = mat[3] = mat[4] = mat[6] = mat[7] = 0.0f;
	mat[8] = mat[9] = mat[11] = mat[12] = mat[13] = mat[14] = 0.0f;
	tupStaticMatrix.AddMatrix(mat);
	tupNode.AddFlags(0);
	tupNode.AddParentNodeReference(m_rootNodeReference);
	tupNode.AddName("_REST_OF_WORLD_");
	
	// make this the first one in the list
	pNodeAgg->AddScalarInt("Annotate_VisibilitySetIndex",NULL,0);
	// camera can use this region
	pNodeAgg->AddScalarInt("Annotate_RealtimeCameraRegion",NULL,TRUE);
	
	ts_BoundingBox worldBox;
	worldBox.Min.Set(-MAXFLOAT32,-MAXFLOAT32,-MAXFLOAT32);
	worldBox.Max.Set( MAXFLOAT32, MAXFLOAT32, MAXFLOAT32);

	Vector3 worldCenter;
	worldCenter.Set(0.0f,0.0f,0.0f);
	
	// add in the huge bounding box
	pNodeAgg->AddListFloat("Bounding_BoundingBoxMin",NULL,(float *)&worldBox.Min,3);
	pNodeAgg->AddListFloat("Bounding_BoundingBoxMax",NULL,(float *)&worldBox.Max,3);

	pNodeAgg->AddListFloat("Bounding_BoundingSphereCenter",NULL,(float *)&worldCenter,3);
	pNodeAgg->AddScalarFloat("Bounding_BoundingSphereRadius",NULL,MAXFLOAT32);
	
	DirCos3x3 orientation;
	orientation.Identity();
	pNodeAgg->AddListFloat("Bounding_OBBOrientation",NULL,(float *)&orientation,9);
	pNodeAgg->AddListFloat("Bounding_OBBCenter",NULL,(float *)&worldCenter,3);
	pNodeAgg->AddListFloat("Bounding_OBBExtents",NULL,(float *)&worldBox.Max,3);
	
	return nodeIndex; // return the reference to the new node
}

// add a scalar int to each node for indicating which visibility set the instance belongs to
void ProcessTup::AddVisibilitySetRefToInstances(void)
{
	const float areaFitThreshold = 0.1f; // at least this % needs to be in the visibility set to be considered

	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;
		
		TupNode tupNode(pNodeAgg);

		// make sure it is pointing to an object
		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef==-1)
			continue;

		// make sure it is pointing to a mesh
		TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
		if (!pBaseMeshAgg)
			continue;
		
		// check and set flags based on instance attributes
		// get my attributes from the node
		char *pNodeAttributesString;
		tupNode.GetUserProperties(&pNodeAttributesString);
		// count how many named points we have
		TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_OBJ_BLOCK,END_OBJ_BLOCK);
		TupKeyValueContext tupKeyValueContext;
		const int INSTANCE_TYPE = 0;
		const int VISIBILITY_SET_PLACEMENT = 1;
		const int VISIBILITY_SET_SHRINK_FACTOR = 2;
		const int VISIBILITY_SET_SPECIFIC_SET = 3;
		
		tupKeyValueContext.AddKey("instance_type");
		tupKeyValueContext.AddKey("visibility_set_placement");
		tupKeyValueContext.AddKey("visibility_set_shrink_factor");
		tupKeyValueContext.AddKey("visibility_set_specific_set");
		
		m_pTupNodeConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);

		// only on normal instances
		if (tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(INSTANCE_TYPE),"normal")!=0)
			continue;

		const int VISIBILITY_SET_PLACEMENT_INSIDE_SMALLEST = 0;
		const int VISIBILITY_SET_PLACEMENT_INSIDE_LARGEST = 1;
		const int VISIBILITY_SET_PLACEMENT_WORLD = 2;
		const int VISIBILITY_SET_PLACEMENT_SPECIFIC_SET = 3;

		int visibilitySetPlacement = VISIBILITY_SET_PLACEMENT_INSIDE_SMALLEST; // default
		TupString specificSetName;
		float shrinkFactor = 1.0f;

		// set up the style of visibility set placement based on the object attributes
		if (tupKeyValueContext.ValueValidAt(VISIBILITY_SET_PLACEMENT))
		{
			if (strcmp(tupKeyValueContext.GetValueAsString(VISIBILITY_SET_PLACEMENT),"inside_smallest")==0)
			{
				visibilitySetPlacement = VISIBILITY_SET_PLACEMENT_INSIDE_SMALLEST;
				if (tupKeyValueContext.ValueValidAt(VISIBILITY_SET_SHRINK_FACTOR))
				{
					shrinkFactor = tupKeyValueContext.GetValueAsFloat(VISIBILITY_SET_SHRINK_FACTOR);
				}
			}
			else if (strcmp(tupKeyValueContext.GetValueAsString(VISIBILITY_SET_PLACEMENT),"inside_largest")==0)
			{
				visibilitySetPlacement = VISIBILITY_SET_PLACEMENT_INSIDE_LARGEST;
				if (tupKeyValueContext.ValueValidAt(VISIBILITY_SET_SHRINK_FACTOR))
				{
					shrinkFactor = tupKeyValueContext.GetValueAsFloat(VISIBILITY_SET_SHRINK_FACTOR);
				}
			}
			else if (strcmp(tupKeyValueContext.GetValueAsString(VISIBILITY_SET_PLACEMENT),"world")==0)
			{
				visibilitySetPlacement = VISIBILITY_SET_PLACEMENT_WORLD;
			}
			else if (strcmp(tupKeyValueContext.GetValueAsString(VISIBILITY_SET_PLACEMENT),"specific_set")==0)
			{
				visibilitySetPlacement = VISIBILITY_SET_PLACEMENT_SPECIFIC_SET;
				if (tupKeyValueContext.ValueValidAt(VISIBILITY_SET_SPECIFIC_SET))
				{
					specificSetName = tupKeyValueContext.GetValueAsString(VISIBILITY_SET_SPECIFIC_SET);
				}
			}
		}

		int bestSet;
		int numVisibilitySets = m_visibilitySetNodeRefs.GetSize();

		switch (visibilitySetPlacement)
		{
			case VISIBILITY_SET_PLACEMENT_INSIDE_SMALLEST:
			case VISIBILITY_SET_PLACEMENT_INSIDE_LARGEST:
				{
					// if the break large meshes filter wishes this instance to go into a specific visibility set
					TupperwareScalar *pVisibiltySetPlacementNodeRefScalar = pNodeAgg->FindScalarByKey("BreakLargeMeshes_VisibilitySetPlaceNodeRef");
					if (pVisibiltySetPlacementNodeRefScalar)
					{
						int visibilitySetPlacementNodeRef = pVisibiltySetPlacementNodeRefScalar->GetAsInt();
						for (int setIndex=1;setIndex<numVisibilitySets;setIndex++)
						{
							if (m_visibilitySetNodeRefs[setIndex] == visibilitySetPlacementNodeRef)
							{
								bestSet = setIndex;
								break;
							}
						}
						assert(bestSet!=0); // this better get set
					}
					else 
					{
						// get the obb from the node
						OBBInstance nodeOBB;
						nodeOBB.Orientation() = *(DirCos3x3 *)pNodeAgg->FindListByKey("Bounding_OBBOrientation")->GetAsFloat();
						nodeOBB.Position() = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBCenter")->GetAsFloat();
						nodeOBB.GetAABB().maxV = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBExtents")->GetAsFloat();
						// shrink it down
						nodeOBB.GetAABB().maxV *= shrinkFactor;
						nodeOBB.GetAABB().minV = - nodeOBB.GetAABB().maxV;
						
						bestSet = 0;
						float bestAreaVolume; // keep track of the volume of the best region
						
						// 
						for (int setIndex=1;setIndex<numVisibilitySets;setIndex++)
						{
							if (m_visibilitySetOBBs[setIndex].TotallyInside(nodeOBB))
							{
								float areaVolume = Bounding::GetVolume(m_visibilitySetOBBs[setIndex].GetAABB().minV,m_visibilitySetOBBs[setIndex].GetAABB().minV);
								switch (visibilitySetPlacement)
								{
									case VISIBILITY_SET_PLACEMENT_INSIDE_SMALLEST:
										if (bestSet==0 || areaVolume<bestAreaVolume)
										{
											bestSet = setIndex;
											bestAreaVolume = areaVolume;
										}
										break;
									case VISIBILITY_SET_PLACEMENT_INSIDE_LARGEST:
										if (bestSet==0 || areaVolume>bestAreaVolume)
										{
											bestSet = setIndex;
											bestAreaVolume = areaVolume;
										}
										break;
								}
							}
						}
						if (bestSet == 0)
						{
							char *pName;
							tupNode.GetName(&pName);
							PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Instance %s cannot be placed in a visibility set.  Placing in world.",pName);
						}
					}
				}
				break;
			case VISIBILITY_SET_PLACEMENT_WORLD:
				{
					bestSet = 0; // use the world
				}
				break;
			case VISIBILITY_SET_PLACEMENT_SPECIFIC_SET:
				{
					bestSet = 0;
					for (int setIndex=1;setIndex<numVisibilitySets;setIndex++)
					{
						if (specificSetName==m_visibilitySetNodeNames[setIndex])
						{
							bestSet = setIndex;
							break;
						}
					}
					// log a warning due to set not found
					if (bestSet==0)
					{
						char *pName;
						tupNode.GetName(&pName);
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE,"Warning - Instance %s cannot be placed in visibility set %s - not found.  Placing in world",pName,(const char *)specificSetName);
					}
				}
				break;
		}
		// add the index to the best visibility set we are in
		pNodeAgg->AddScalarInt("Annotate_InVisibilitySetIndex",NULL,bestSet);
	}
}

// create a bounding box for the visibility set that will be used to represent the instances within it
void ProcessTup::AdjustVisibilitySetBoundingBoxes(void)
{
	int numVisibilitySets = m_visibilitySetNodeRefs.GetSize();

	// reset each visibility set bounding box
	int setIndex;
	for (setIndex=1;setIndex<numVisibilitySets;setIndex++)
	{
		Bounding::Clear(m_visibilitySetBoundingBoxes[setIndex].Min,m_visibilitySetBoundingBoxes[setIndex].Max);
	}
	
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		
		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		// add the index to the best visibility set we are in
		TupperwareScalar *pVisibilitySetIndexScalar = pNodeAgg->FindScalarByKey("Annotate_InVisibilitySetIndex");

		// must have this
		if (!pVisibilitySetIndexScalar)
			continue;

		int visibilitySetIndex = pVisibilitySetIndexScalar->GetAsInt();

		// must be a set that is not 0
		if (visibilitySetIndex==0)
			continue;

		// get the bounding box from the node
		ts_BoundingBox boundingBox;
		boundingBox.Min = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat();
		boundingBox.Max = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat();

		Bounding::AddToBoundingBox(m_visibilitySetBoundingBoxes[visibilitySetIndex].Min,
											m_visibilitySetBoundingBoxes[visibilitySetIndex].Max,
											boundingBox.Min);

		Bounding::AddToBoundingBox(m_visibilitySetBoundingBoxes[visibilitySetIndex].Min,
											m_visibilitySetBoundingBoxes[visibilitySetIndex].Max,
											boundingBox.Max);
	}

	// apply new bounding boxes to visibility set nodes
	for (setIndex=1;setIndex<numVisibilitySets;setIndex++)
	{
		int nodeRef = m_visibilitySetNodeRefs[setIndex];

		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeRef);
		// add adjusted bounding box info
		pNodeAgg->AddListFloat("Annotate_VisiblitySetBoundingBoxMin",NULL,(float*) &m_visibilitySetBoundingBoxes[setIndex].Min,3);
		pNodeAgg->AddListFloat("Annotate_VisiblitySetBoundingBoxMax",NULL,(float*) &m_visibilitySetBoundingBoxes[setIndex].Max,3);
	}
}


// add a scalar int to each node that will be used as collision
// this index is used in conjunction with the InVisibilitySetRef value
void ProcessTup::AddCollisionInstanceIndexes(void)
{
	if (m_bBuildPartitions)
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Building Partitions");

	int numVisibilitySets = m_visibilitySetNodeRefs.GetSize();
	// set the size to be the same as the number of visibility sets
	m_totalCollisionInstances.SetSize(numVisibilitySets);
	for (int setIndex=0;setIndex<numVisibilitySets;setIndex++)
	{
		m_totalCollisionInstances[setIndex] = 0; // start them all at 0
	}
	
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		
		// if empty slot - continue
		if (!pNodeAgg)
			continue;
		
		TupNode tupNode(pNodeAgg);
		
		// check and set flags based on instance attributes
		// get my attributes from the node
		char *pNodeAttributesString;
		tupNode.GetUserProperties(&pNodeAttributesString);
		// count how many named points we have
		TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_OBJ_BLOCK,END_OBJ_BLOCK);
		TupKeyValueContext tupKeyValueContext;
		const int INSTANCE_TYPE = 0;
		const int COLLISION_LAYER = 1;
		const int ENV_TYPE = 2;
		const int NO_FAR_CLIP = 3;
		const int COLLISION_TYPE = 4;
		tupKeyValueContext.AddKey("instance_type");
		tupKeyValueContext.AddKey("collision_layer");
		tupKeyValueContext.AddKey("env_type");
		tupKeyValueContext.AddKey("instance_no_far_clip");
		tupKeyValueContext.AddKey("CollisionType");
		
		m_pTupNodeConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
		// if set to something other than normal then skip
		if (tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(INSTANCE_TYPE),"normal")!=0)
		{
			continue;
		}

		// can't be set to no far clip
		if (tupKeyValueContext.ValueValidAt(NO_FAR_CLIP) && tupKeyValueContext.GetValueAsBool(NO_FAR_CLIP))
		{
			continue;
		}

		// if objattrib is disabling collision then skip too
		if (tupKeyValueContext.ValueValidAt(COLLISION_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(COLLISION_TYPE),"Disabled")==0)
		{
			continue;
		}
		
		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef!=-1)
		{
			TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
			if (pBaseMeshAgg)
			{
				// get collision layer from instance (used in droctopus and obbworld)
				int collisionLayer = 0;
				if (tupKeyValueContext.ValueValidAt(COLLISION_LAYER))
				{
					collisionLayer = tupKeyValueContext.GetValueAsInt(COLLISION_LAYER);
					if (collisionLayer<0||collisionLayer>31)
						collisionLayer = 0;
				}
				// make sure the env_type is "None" (don't add environmental objects)
				if (!strcmp(tupKeyValueContext.GetValueAsString(ENV_TYPE),"None"))
				{
					// get the index into the visibilitySet
					int visibilitySetIndex;
					if (m_bBuildPartitions) // if using partitions put in the partition that is the visibility set
						visibilitySetIndex = pNodeAgg->FindScalarByKey("Annotate_InVisibilitySetIndex")->GetAsInt();
					else // if not put in the world
						visibilitySetIndex = 0;

					// store collision layer
					pNodeAgg->AddScalarInt("Annotate_CollisionLayer",NULL,collisionLayer);

					// store the collision partition index (0-world)
					pNodeAgg->AddScalarInt("Annotate_CollisionPartitionIndex",NULL,visibilitySetIndex);

					// store collision instance index and keep track of the total 
					pNodeAgg->AddScalarInt("Annotate_CollisionInstanceIndex",NULL,m_totalCollisionInstances[visibilitySetIndex]);
					m_totalCollisionInstances[visibilitySetIndex]++; // update total for the visibility set
				}
			}
		}
	}
}


// this routine will figure out where the meshes go (into one or more partitions or the world)
// it will also mark the meshes and instances with their correct info for knowing which partitions they are
// needed for as well as their indicies in the SOM and instance lists
// this also calculates the total SOMS and total render instances for each partition
void ProcessTup::AddPartionDataToObjectsAndInstances(void)
{
	int numVisibilitySets = m_visibilitySetNodeRefs.GetSize();

	// how many partitions a mesh can be in and not be put in the main partition
	int meshPartitionThreshold = numVisibilitySets/5;
	if (meshPartitionThreshold<1)
		meshPartitionThreshold = 1;

	// initialize the totals 
	m_totalSOMS.SetSize(numVisibilitySets);
	m_totalRenderInstances.SetSize(numVisibilitySets);
	int setIndex;
	for (setIndex=0;setIndex<numVisibilitySets;setIndex++)
	{
		// start them all at 0
		m_totalSOMS[setIndex] = 0; 
		m_totalRenderInstances[setIndex] = 0;
	}

	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if empty slot - continue
		if (!pObjectAgg)
			continue;

		// it needs to be a mesh
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType() != TupObject::TRI_MESH)
			continue;

		TupperwareList *pBaseObjectParentNodeReferenceList = pObjectAgg->FindListByKey("Annotate_BaseObjectParentNodeReferenceList");
		if (!pBaseObjectParentNodeReferenceList)
			continue; // needs at least one node pointing to it

		TupArray<int> parentNodeArray;
		pBaseObjectParentNodeReferenceList->GetAsInt(parentNodeArray);
		int numParents = parentNodeArray.GetSize();
		Keeper<int> partitionKeeper;
		TupArray<int> parentsToSet;
		TupArray<int> parentsPartitions;
		
		bool bFoundWorldPartition = !m_bBuildPartitions; // if we are build partitions start with false

		// compute how many SOMS are used 
		int numSOMs = 1; // start with one (default)
		TupperwareAggregate *pBoneMeshAgg = pObjectAgg->FindAggregateByKey("Annotate_BoneMeshData");
		
		// if there is a cloth mesh then add more
		TupperwareAggregate *pClothMeshAgg = pObjectAgg->FindAggregateByKey("Annotate_ClothMeshData");
		if (pClothMeshAgg)
		{
			int numClothSOMS = pClothMeshAgg->FindScalarByKey("NumClothSOMs")->GetAsInt();
			int numNonClothFaces = pClothMeshAgg->FindScalarByKey("NumNonClothFaces")->GetAsInt();
			// if there is no base mesh and no bones
			if (numNonClothFaces==0 && !pBoneMeshAgg)
				numSOMs = 0; // no base SOM
			numSOMs+= numClothSOMS;
			if (numSOMs==0)
			{
				throw(PluginException(ERROR_FATAL,"Object does not have any SOMS specified"));
			}
			else if (numSOMs!=1)
			{
				throw(PluginException(ERROR_FATAL,"Cloth supports only one object and no bones (will be fixed later)"));
			}
		}
	
		// go through the parents and collect info on visibility sets
		for (int parentIndex=0;parentIndex<numParents;parentIndex++)
		{
			int parentRef = parentNodeArray[parentIndex];
			TupperwareAggregate *pParentNodeAgg = m_pTupNodePool->GetNode(parentRef);
			TupNode tupNode(pParentNodeAgg);
			
			// check and set flags based on instance attributes
			// get my attributes from the node
			char *pNodeAttributesString;
			tupNode.GetUserProperties(&pNodeAttributesString);
			// count how many named points we have
			TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_OBJ_BLOCK,END_OBJ_BLOCK);
			TupKeyValueContext tupKeyValueContext;
			const int INSTANCE_TYPE = 0;
			const int COLLISION_ONLY = 1;
			const int ENV_TYPE = 2;
			const int NO_FAR_CLIP = 3;
			tupKeyValueContext.AddKey("instance_type");
			tupKeyValueContext.AddKey("collision_only");
			tupKeyValueContext.AddKey("env_type");
			tupKeyValueContext.AddKey("instance_no_far_clip");
			
			m_pTupNodeConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
			// if set to something other than normal then skip
			if (tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(INSTANCE_TYPE),"normal")!=0)
			{
				continue;
			}
			// if we are collision only then skip
			if (tupKeyValueContext.GetValueAsBool(COLLISION_ONLY))
			{
				continue;
			}

			// set no far clip flag (used later on the scene tree)
			if (tupKeyValueContext.ValueValidAt(NO_FAR_CLIP) && tupKeyValueContext.GetValueAsBool(NO_FAR_CLIP))
			{
				pParentNodeAgg->AddScalarInt("Annotate_NoFarClip",NULL,1);
			}

			// keep track of this parent for later
			parentsToSet.Add(parentRef);

			// if we are an environmental object or have bones or cloth
			if (pBoneMeshAgg || pClothMeshAgg || strcmp(tupKeyValueContext.GetValueAsString(ENV_TYPE),"None")!=0)
			{
				// we are in the world
				int worldPartitionIndex = 0;
				partitionKeeper.Add(worldPartitionIndex);
				bFoundWorldPartition = true;
				parentsPartitions.Add(0);
				continue;
			}

			int visibilitySetIndex = pParentNodeAgg->FindScalarByKey("Annotate_InVisibilitySetIndex")->GetAsInt();
			// if this is in the world
			if (visibilitySetIndex==0)
			{
				bFoundWorldPartition = true;
			}
			// keep track of all the partitions this mesh is in
			partitionKeeper.Add(visibilitySetIndex);
			// keep track of the partition that parent wants to use
			parentsPartitions.Add(visibilitySetIndex);
		}

		int numUsedPartitions = partitionKeeper.GetSize();
		// if nobody wants me then skip
		if (numUsedPartitions == 0)
		{
			continue;
		}

		// this array will be placeed on the mesh object aggregate
		// it contains an int for each visibility set
		// the value will be >=0 if within that partition
		// if -1 it is not needed within that slot
		TupArray<int> renderPartitionSOMStarts;
		renderPartitionSOMStarts.SetSize(numVisibilitySets);
		for (setIndex=0;setIndex<numVisibilitySets;setIndex++)
		{
			// start them all at -1
			renderPartitionSOMStarts[setIndex] = -1; 
		}

		// if none in the world and we are using a low number of partitions
		if (!bFoundWorldPartition && (numUsedPartitions<=meshPartitionThreshold))
		{
			for (int partitionKeeperIndex=0;partitionKeeperIndex<numUsedPartitions;partitionKeeperIndex++)
			{
				int partitionIndex = partitionKeeper[partitionKeeperIndex];
				renderPartitionSOMStarts[partitionIndex] = m_totalSOMS[partitionIndex];
				m_totalSOMS[partitionIndex]+=numSOMs;
			}
			// save list of what partitions are active as well as the som start indes
			pObjectAgg->AddListInt("Annotate_RenderPartitionSOMStarts",NULL,renderPartitionSOMStarts);

			// save data to the parent nodes
			int numParentsToSet = parentsToSet.GetSize();
			for (int parentIndex=0;parentIndex<numParentsToSet;parentIndex++)
			{
				int parentRef = parentsToSet[parentIndex];
				TupperwareAggregate *pParentNodeAgg = m_pTupNodePool->GetNode(parentRef);
				int parentPartition = parentsPartitions[parentIndex];
				pParentNodeAgg->AddScalarInt("Annotate_RenderPartitionIndex",NULL,parentPartition);
				pParentNodeAgg->AddScalarInt("Annotate_RenderInstanceIndex",NULL,m_totalRenderInstances[parentPartition]);
				pParentNodeAgg->AddScalarInt("Annotate_SOMIndex",NULL,renderPartitionSOMStarts[parentPartition]);
				m_totalRenderInstances[parentPartition]++;
			}
		}
		// we need to go into the world
		else
		{
			renderPartitionSOMStarts[0] = m_totalSOMS[0];
			m_totalSOMS[0]+=numSOMs;

			// save list of what partitions are active as well as the som start indices
			pObjectAgg->AddListInt("Annotate_RenderPartitionSOMStarts",NULL,renderPartitionSOMStarts);

			// save data to the parent nodes
			int numParentsToSet = parentsToSet.GetSize();
			for (int parentIndex=0;parentIndex<numParentsToSet;parentIndex++)
			{
				int parentRef = parentsToSet[parentIndex];
				TupperwareAggregate *pParentNodeAgg = m_pTupNodePool->GetNode(parentRef);
				int parentPartition = parentsPartitions[parentIndex];
				pParentNodeAgg->AddScalarInt("Annotate_RenderPartitionIndex",NULL,0);
				pParentNodeAgg->AddScalarInt("Annotate_RenderInstanceIndex",NULL,m_totalRenderInstances[0]);
				pParentNodeAgg->AddScalarInt("Annotate_SOMIndex",NULL,renderPartitionSOMStarts[0]);
				m_totalRenderInstances[0]++;
			}
		}
	}
}

// calculates and sets each material, map, & image with a count of how many faces used it
// used later to determine which which texture contexts and textures to save
void ProcessTup::MarkUsedMaterialsMapsAndImages(void)
{
	int numVisibilitySets = m_visibilitySetNodeRefs.GetSize();
	AnnotateHelper::UsedMatsMapsAndImages usedMatsMapsAndImages;
	AnnotateHelper::CalculateUsedMatsMapsAndImages(*m_pTupObjectPool,*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,
													usedMatsMapsAndImages,numVisibilitySets);

	int totalMaterials = m_pTupMaterialPool->GetNumMaterials();
	for(int materialIndex=0;materialIndex<totalMaterials;materialIndex++)
	{
		TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialIndex);

		// if empty slot - continue
		if (!pMaterialAgg)
			continue;

		// set how many times this material is used in all the partitions
		pMaterialAgg->AddScalarInt("Annotate_MaterialUsedTotal",NULL,usedMatsMapsAndImages.m_usedMaterials[materialIndex].m_totalUsedCount);
		// set how many times this material is used in each partition
		pMaterialAgg->AddListInt("Annotate_MaterialUsed",NULL,usedMatsMapsAndImages.m_usedMaterials[materialIndex].m_usedPartitionCounts);
	}
	int totalMaps = m_pTupMapPool->GetNumMaps();
	for(int mapIndex=0;mapIndex<totalMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapIndex);

		// if empty slot - continue
		if (!pMapAgg)
			continue;
		// add how many times this map is used in all the partitions
		pMapAgg->AddScalarInt("Annotate_MapUsedTotal",NULL,usedMatsMapsAndImages.m_usedMaps[mapIndex].m_totalUsedCount);
		// set how many times this map is used in each partition
		pMapAgg->AddListInt("Annotate_MapUsed",NULL,usedMatsMapsAndImages.m_usedMaps[mapIndex].m_usedPartitionCounts);
	}
	int totalImages = m_pTupImagePool->GetNumImages();
	for(int imageIndex=0;imageIndex<totalImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);

		// if empty slot - continue
		if (!pImageAgg)
			continue;
		// add how many times this map is used
		pImageAgg->AddScalarInt("Annotate_ImageUsedTotal",NULL,usedMatsMapsAndImages.m_usedImages[imageIndex].m_totalUsedCount);
		// set how many times this map is used in each partition
		pImageAgg->AddListInt("Annotate_ImageUsed",NULL,usedMatsMapsAndImages.m_usedImages[imageIndex].m_usedPartitionCounts);
	}
}

// calculates and sets each material, map, & image with a count of how many faces used it
// used later to determine which which texture contexts and textures to save
void ProcessTup::DeleteUnusedMaterialsMapsAndImages(void)
{
	int totalMaterials = m_pTupMaterialPool->GetNumMaterials();
	for(int materialIndex=0;materialIndex<totalMaterials;materialIndex++)
	{
		TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialIndex);

		// if empty slot - continue
		if (!pMaterialAgg)
			continue;

		int materialUsed = pMaterialAgg->FindScalarByKey("Annotate_MaterialUsedTotal")->GetAsInt();
		// if not used then kill from pool
		if (materialUsed==0)
		{
			m_pTupMaterialPool->DeleteMaterial(materialIndex);
		}
	}
	int totalMaps = m_pTupMapPool->GetNumMaps();
	for(int mapIndex=0;mapIndex<totalMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapIndex);

		// if empty slot - continue
		if (!pMapAgg)
			continue;

		int mapUsed = pMapAgg->FindScalarByKey("Annotate_MapUsedTotal")->GetAsInt();
		// if not used then kill from pool
		if (mapUsed==0)
		{
			m_pTupMapPool->DeleteMap(mapIndex);
		}
	}
	int totalImages = m_pTupImagePool->GetNumImages();
	for(int imageIndex=0;imageIndex<totalImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);

		// if empty slot - continue
		if (!pImageAgg)
			continue;

		int imageUsed = pImageAgg->FindScalarByKey("Annotate_ImageUsedTotal")->GetAsInt();
		TupperwareScalar *pRemapImageRefScalar = pImageAgg->FindScalarByKey("Annotate_RemapImageRef");
		// if not used and not remapped
		if (imageUsed==0 && pRemapImageRefScalar==NULL)
		{
			m_pTupImagePool->DeleteImage(imageIndex);
		}
	}
}




// add a list of ints each material that will be a texture context for that partition -1 bring not used
void ProcessTup::AddTextureContextIndexes(void)
{
	int numVisibilitySets = m_visibilitySetNodeRefs.GetSize();
	m_totalTextureContexts.SetSize(numVisibilitySets);
	int setIndex;
	m_totalTextureContexts[0] = 1; // world partition has a blank for 0
	for (setIndex=1;setIndex<numVisibilitySets;setIndex++)
	{
		m_totalTextureContexts[setIndex] = 0; // all others start with 0
	}

	int totalMaterials = m_pTupMaterialPool->GetNumMaterials();
	for(int materialIndex=0;materialIndex<totalMaterials;materialIndex++)
	{
		TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialIndex);

		// if empty slot - continue
		if (!pMaterialAgg)
			continue;

		TupMaterial tupMaterial(pMaterialAgg);
		if (tupMaterial.GetMaterialType()==TupMaterial::STANDARD_MATERIAL)
		{
			// make this a texture context if it is being used
			int *pMaterialUsed = pMaterialAgg->FindListByKey("Annotate_MaterialUsed")->GetAsInt();

			// get a list of images used by the material
			Keeper<int> usedImages;
			AnnotateHelper::CalculateUsedImages(materialIndex,*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,usedImages);

			bool bMaterialUsedForRender = false;
			for (setIndex=0;setIndex<numVisibilitySets;setIndex++)
			{
				// if used in this partition
				if (pMaterialUsed[setIndex])
				{
					bMaterialUsedForRender = true;
					break;
				}
			}

			// if we use this one at all for rendering
			if (bMaterialUsedForRender)
			{
				int numUsedImages = usedImages.GetSize();
				bool bPlaceInWorldOnly = true;
				for (int imageIndex=0;imageIndex<numUsedImages;imageIndex++)
				{
					int imageRef = usedImages[imageIndex];
					TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageRef);
					int *pTextureIDs = pImageAgg->FindListByKey("Annotate_TextureIDs")->GetAsInt();
					// if this image is not used in the world partition
					if (pTextureIDs[0]==-1)
					{
						bPlaceInWorldOnly = false;
						break;
					}
				}
				TupArray<int> textureContextIDArray;
				textureContextIDArray.SetSize(numVisibilitySets);
				if (bPlaceInWorldOnly)
				{
					textureContextIDArray[0] = m_totalTextureContexts[0];
					m_totalTextureContexts[0]++;

					for (setIndex=1;setIndex<numVisibilitySets;setIndex++)
					{
						if (pMaterialUsed[setIndex])
						{
							textureContextIDArray[setIndex] = textureContextIDArray[0] | 0xff000000; // point to world
						}
						else
						{
							textureContextIDArray[setIndex] = -1; // not in this set
						}
					}
				}
				else
				{
					for (setIndex=0;setIndex<numVisibilitySets;setIndex++)
					{
						if (pMaterialUsed[setIndex])
						{
							textureContextIDArray[setIndex] = m_totalTextureContexts[setIndex];
							m_totalTextureContexts[setIndex]++;
						}
						else
						{
							textureContextIDArray[setIndex] = -1; // not in this set
						}
					}
				}
				pMaterialAgg->AddListInt("Annotate_TextureContextIDs",NULL,textureContextIDArray);
			}
		}
	}
}

// add a scalar int to each image that will be a texture
void ProcessTup::AddTextureIndexes(void)
{
	int numVisibilitySets = m_visibilitySetNodeRefs.GetSize();

	// how many partitions a mesh can be in and not be put in the main partition
	int texturePartitionThreshold = numVisibilitySets/5;
	if (texturePartitionThreshold<1)
		texturePartitionThreshold = 1;

	m_totalTextures.SetSize(numVisibilitySets);
	int setIndex;
	for (setIndex=0;setIndex<numVisibilitySets;setIndex++)
	{
		m_totalTextures[setIndex] = 0;
	}

	int totalImages = m_pTupImagePool->GetNumImages();
	for(int imageIndex=0;imageIndex<totalImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);

		// if empty slot - continue
		if (!pImageAgg)
			continue;

		TupArray<int> imageUsed;
		pImageAgg->FindListByKey("Annotate_ImageUsed")->GetAsInt(imageUsed);

		TupArray<int> textureIDArray;
		textureIDArray.SetSize(numVisibilitySets);

		int hasTextureID = FALSE;

		bool bUseWorldPartitionOnly = false;

		// if it is used in the world partition already then we will use this one
		if (imageUsed[0])
		{
			hasTextureID = TRUE;
			bUseWorldPartitionOnly = true;
		}
		else
		{
			int partitionCount=0;
			// count how many other partitions it is in
			for (setIndex=1;setIndex<numVisibilitySets;setIndex++)
			{
				if (imageUsed[setIndex])
				{
					hasTextureID = TRUE;
					partitionCount++;
				}
			}
			// if we are in too many partitions
			if (partitionCount>texturePartitionThreshold)
			{
				bUseWorldPartitionOnly = true;
			}
		}

		// if we are going to be used from the world partition
		if (bUseWorldPartitionOnly)
		{
			textureIDArray[0] = m_totalTextures[0];
			m_totalTextures[0]++;

			for (setIndex=1;setIndex<numVisibilitySets;setIndex++)
			{
				if (imageUsed[setIndex])
				{
					textureIDArray[setIndex] = textureIDArray[0] | 0xff000000; // point to the world texture
				}
				else
				{
					textureIDArray[setIndex] = -1; // not in this set
				}
			}
		}
		else // place in each partition that is using it
		{
			textureIDArray[0] = -1; // not in this set
			for (setIndex=1;setIndex<numVisibilitySets;setIndex++)
			{
				if (imageUsed[setIndex])
				{
					textureIDArray[setIndex] = m_totalTextures[setIndex];
					m_totalTextures[setIndex]++;
				}
				else
				{
					textureIDArray[setIndex] = -1; // not in this set
				}
			}
		}
		pImageAgg->AddListInt("Annotate_TextureIDs",NULL,textureIDArray);
		pImageAgg->AddScalarInt("Annotate_HasTextureID",NULL,hasTextureID);
	}
}


// add a scalar int to each material (not multi-sub object) for the texture context
void ProcessTup::AddMaterialHasTextureFlags(void)
{
	MaterialImageRefsManager materialImageRefsManager(*m_pTupMaterialPool,*m_pTupMapPool);

	int totalMaterials = m_pTupMaterialPool->GetNumMaterials();
	for(int materialIndex=0;materialIndex<totalMaterials;materialIndex++)
	{
		TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialIndex);

		// if empty slot - continue
		if (!pMaterialAgg)
			continue;

		TupMaterial tupMaterial(pMaterialAgg);
		if (tupMaterial.GetMaterialType() == TupMaterial::STANDARD_MATERIAL)
		{
			int hasTexture = FALSE;
			int hasTextureAlpha = FALSE;
			int hasTexture8BitAlpha = FALSE;

			TupArray<MaterialImageRefsManager::MaterialImageData> materialImageRefs;
			materialImageRefsManager.BuildImageRefs(materialIndex,materialImageRefs);
			for (int imageIndex=0;imageIndex<materialImageRefs.GetSize();imageIndex++)
			{
				hasTexture = TRUE;
				int imageRef = materialImageRefs[imageIndex].m_imageRef;
				assert(imageRef!=-1);
				TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageRef);
				if (pImageAgg->FindScalarByKey("Annotate_ImageHasAlpha")->GetAsInt())
					hasTextureAlpha = TRUE;
				if (pImageAgg->FindScalarByKey("Annotate_ImageHas8BitAlpha")->GetAsInt())
					hasTexture8BitAlpha = TRUE;
			}
			pMaterialAgg->AddScalarInt("Annotate_MaterialHasTexture",NULL,hasTexture);
			pMaterialAgg->AddScalarInt("Annotate_MaterialHasTextureAlpha",NULL,hasTextureAlpha);
			pMaterialAgg->AddScalarInt("Annotate_MaterialHasTexture8BitAlpha",NULL,hasTexture8BitAlpha);
		}
	}
}

// if requested, load an aux attribute file and merge with the map attributes
void ProcessTup::AddAuxMapAttributes(void)
{
	if (m_bLoadAuxMapAttributes)
	{
		TupAuxKeyValue inAuxKeyValue;

		TupString inputFilename(m_dataManager.GetInputFilename());
		FilePath::ChangeExtension(inputFilename,".AuxMapAttr");

		// get full path of filename
		TupperwareAggregate *pInMapAgg = new TupperwareAggregate;
		int result = TupperwareLoadSaveManager::Read(inputFilename,&pInMapAgg);
		if (result)
		{
			inAuxKeyValue.EntryNamesCaseSensitive(false);
			inAuxKeyValue.LoadEntries(pInMapAgg);

			// merge these attributes onto the maps
			int numMaps = m_pTupMapPool->GetNumMaps();
			for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
			{
				TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapIndex);
				if (!pMapAgg)
					continue;

				TupMap tupMap(pMapAgg);
				if (tupMap.GetMapType() != TupMap::BITMAP_MAP)
					continue;

				TupBitmapMap tupBitmapMap(pMapAgg);

				int imageReference;
				tupBitmapMap.GetImageReference(imageReference);
				if (imageReference == -1)
					continue;

				TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageReference);
				TupImage tupImage(pImageAgg);

				char *pImageFileName;
				tupImage.GetFilePathName(&pImageFileName);

				char *pMapAttributesString;
				tupBitmapMap.GetUserProperties(&pMapAttributesString);
				// get explicitly set attributes from map
				TupKeyValue tupKeyValue(pMapAttributesString,BEGIN_MAP_BLOCK,END_MAP_BLOCK);
				// merge in attributes from aux (don't replace)
				if (inAuxKeyValue.MergeToKeyValues(tupKeyValue,pImageFileName,false))
				{
					// put back on the map
					TupString newAttrString = tupKeyValue.BuildString(NULL,NULL,BEGIN_MAP_BLOCK,END_MAP_BLOCK);
					tupBitmapMap.AddUserProperties(newAttrString);
				}
			}
		}
		else
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Warning - not able to load map attr file %s",(const char *)inputFilename);
		}
		delete pInMapAgg;
	}
}

// take map attributes and merge and place on the images in the imagepool
void ProcessTup::PropagateMapAttributesToImages(void)
{
	TupArray<TupKeyValue> imageAttributes;
	int numImages = m_pTupImagePool->GetNumImages();
	imageAttributes.SetSize(numImages);

	int numMaps = m_pTupMapPool->GetNumMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapIndex);
		if (!pMapAgg)
			continue;

		TupMap tupMap(pMapAgg);
		if (tupMap.GetMapType() != TupMap::BITMAP_MAP)
			continue;

		TupBitmapMap tupBitmapMap(pMapAgg);

		int imageReference;
		tupBitmapMap.GetImageReference(imageReference);
		if (imageReference == -1)
			continue;

		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageReference);
		assert(pImageAgg);

		char *pMapAttributesString;
		tupBitmapMap.GetUserProperties(&pMapAttributesString);
		// get explicitly set attributes from map
		TupKeyValue tupKeyValue(pMapAttributesString,BEGIN_MAP_BLOCK,END_MAP_BLOCK);
		// merge attribute to image
		imageAttributes[imageReference].MergeKeyValues(tupKeyValue,false);
	}

	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
			continue;

		TupKeyValue &keyValue = imageAttributes[imageIndex];
		if (keyValue.GetNumAttribs())
		{
			TupString attribString = keyValue.BuildString(NULL,NULL);
			TupImage tupImage(pImageAgg);
			tupImage.AddUserProperties(attribString);
		}
	}
}

// if requested, load an aux attribute file and merge with the map attributes
void ProcessTup::SaveAuxImageAttributes(void)
{
	if (m_bSaveAuxImageAttributes)
	{
		TupAuxKeyValue outAuxKeyValue;

		int numImages = m_pTupImagePool->GetNumImages();

		for (int imageIndex=0;imageIndex<numImages;imageIndex++)
		{
			TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
			if (!pImageAgg)
				continue;

			TupImage tupImage(pImageAgg);

			char *pImageFileName;
			tupImage.GetFilePathName(&pImageFileName);

			char *pImageAttributesString;
			tupImage.GetUserProperties(&pImageAttributesString);

			TupKeyValue tupKeyValue(pImageAttributesString);

			TupKeyValueContext tupKeyValueContextAll;
			m_pTupMapConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContextAll,true);
			outAuxKeyValue.AddFromContext(tupKeyValue,tupKeyValueContextAll,pImageFileName);
		}

		TupString relativePath,fullPath,name,ext;
		// get full path of filename
		TupString outputFilename(m_dataManager.GetOutputFilename());
		FilePath::ChangeExtension(outputFilename,".AuxImageAttr");
		TupperwareAggregate *pOutMapAgg = new TupperwareAggregate;
		outAuxKeyValue.SaveEntries(pOutMapAgg);
		int result = TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,outputFilename,pOutMapAgg);
		if (!result)
			throw(PluginException(ERROR_FATAL,"Error writing Aux Image Attr File %s",(const char *)fullPath));
		delete pOutMapAgg;
	}
}

// scan through the attributes on the maps and add any atoms to the maps themselves
void ProcessTup::AddMapAttributes(void)
{
	int numMaps = m_pTupMapPool->GetNumMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapIndex);
		if (!pMapAgg)
			continue;

		TupMap tupMap(pMapAgg);
		if (tupMap.GetMapType() != TupMap::BITMAP_MAP)
			continue;

		TupBitmapMap tupBitmapMap(pMapAgg);

		char *pMapAttributesString;
		tupBitmapMap.GetUserProperties(&pMapAttributesString);
		// get explicitly set attributes from map
		TupKeyValue tupKeyValue(pMapAttributesString,BEGIN_MAP_BLOCK,END_MAP_BLOCK);

		TupKeyValueContext tupKeyValueContext;
		const int FILTER_TYPE = 0;
		const int TWO_SIDED_FACES = 1;
		
		tupKeyValueContext.AddKey("FilterType");
		tupKeyValueContext.AddKey("TwoSidedFaces");
		
		m_pTupMapConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
		
		// filter type
		if (tupKeyValueContext.ValueValidAt(FILTER_TYPE) && !strcmp(tupKeyValueContext.GetValueAsString(FILTER_TYPE),"PointSampled"))
		{
			pMapAgg->AddScalarInt("Annotate_PointSampled",NULL,1); // we are point sampling
		}

		// two sided faces
		if (!m_bClearTwoSidedFaceFlags && tupKeyValueContext.ValueValidAt(TWO_SIDED_FACES) && tupKeyValueContext.GetValueAsBool(TWO_SIDED_FACES))
		{
			pMapAgg->AddScalarInt("Annotate_TwoSidedFaces",NULL,1); // we are doing two sided faces on this map
		}
	}
}

// scan through the attributes on the images and add atoms to the image
void ProcessTup::AddImageAttributes(void)
{
	if (m_bGlobalMipMapping)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Global Mipmapping enabled. NumSubMaps = %d, CrossFade = %s, Distance = %f",
			m_globalNumMipMapSubMaps,m_bGlobalMipMapCrossFade ? "true" : "false",m_globalMipMapDistance);
	}
	int numImages = m_pTupImagePool->GetNumImages();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
			continue;

		TupImage tupImage(pImageAgg);

		char *pImageFileName;
		tupImage.GetFilePathName(&pImageFileName);

		char *pImageAttributesString;
		tupImage.GetUserProperties(&pImageAttributesString);
		// get explicitly set attributes from map
		TupKeyValue tupKeyValue(pImageAttributesString);

		TupKeyValueContext tupKeyValueContext;
		const int MIP_MAPPING = 0;
		const int CROSS_FADE = 1;
		const int NUM_SUB_MAPS = 2;
		const int MIP_MAP_DISTANCE = 3;
		const int REFLECTS = 4;
		const int APERTURE = 5;

		tupKeyValueContext.AddKey("MipMapping");
		tupKeyValueContext.AddKey("Crossfade");
		tupKeyValueContext.AddKey("NumSubMaps");
		tupKeyValueContext.AddKey("MipMapDistance");
		tupKeyValueContext.AddKey("Reflects");
		tupKeyValueContext.AddKey("Aperture");

		m_pTupMapConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);

		// mip mapping
		if (m_bGlobalMipMapping)
		{
			// get image data from image manager
			ImageManager &imageManager = m_dataManager.GetImageManager();
			TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
			assert(pImageDataAgg);
			// convert image data to have the alpha channel have the luminance of the color
			TupImageData tupImageData(pImageDataAgg);
			int generatedSubMaps = ImageUtilities::BuildMipMaps(tupImageData,m_globalNumMipMapSubMaps); // set up the mip map information
			if (generatedSubMaps)
			{
				pImageAgg->AddScalarInt("Annotate_MipMapping",NULL,1);
				pImageAgg->AddScalarInt("Annotate_MipMapCrossFade",NULL,m_bGlobalMipMapCrossFade ? 1 : 0);
				pImageAgg->AddScalarFloat("Annotate_MipMapDistance",NULL,m_globalMipMapDistance);
				pImageAgg->AddScalarInt("Annotate_MipMapNumSubMaps",NULL,generatedSubMaps);

				char *pCrossFadeString = m_bGlobalMipMapCrossFade ? "CrossFading Set" : "No CrossFading";
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"%s set up for Mipmapping. %s %d maps generated. Mip Map distance set to %f",pImageFileName,pCrossFadeString,generatedSubMaps,m_globalMipMapDistance);
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Attempted to set up mipmaps for %s.  No submaps generated.",pImageFileName);
			}
		}
		else if 	(tupKeyValueContext.ValueValidAt(MIP_MAPPING) && tupKeyValueContext.GetValueAsBool(MIP_MAPPING))
		{
			bool bCrossFade = tupKeyValueContext.GetValueAsBool(CROSS_FADE);
			int numSubMaps = tupKeyValueContext.GetValueAsInt(NUM_SUB_MAPS);
			float mipMapDistance = tupKeyValueContext.GetValueAsFloat(MIP_MAP_DISTANCE);

			// get image data from image manager
			ImageManager &imageManager = m_dataManager.GetImageManager();
			TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
			assert(pImageDataAgg);
			// convert image data to have the alpha channel have the luminance of the color
			TupImageData tupImageData(pImageDataAgg);
			int generatedSubMaps = ImageUtilities::BuildMipMaps(tupImageData,numSubMaps); // set up the mip map information
			if (generatedSubMaps)
			{
				pImageAgg->AddScalarInt("Annotate_MipMapping",NULL,1);
				pImageAgg->AddScalarInt("Annotate_MipMapCrossFade",NULL,bCrossFade ? 1 : 0);
				pImageAgg->AddScalarFloat("Annotate_MipMapDistance",NULL,mipMapDistance);
				pImageAgg->AddScalarInt("Annotate_MipMapNumSubMaps",NULL,generatedSubMaps);

				char *pCrossFadeString = bCrossFade ? "CrossFading Set" : "No CrossFading";
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"%s set up for Mipmapping. %s %d maps generated. Mip Map distance set to %f",pImageFileName,pCrossFadeString,generatedSubMaps,mipMapDistance);
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Attempted to set up mipmaps for %s.  No submaps generated.",pImageFileName);
			}
		}
		// Reflects
		if (tupKeyValueContext.ValueValidAt(REFLECTS) && tupKeyValueContext.GetValueAsBool(REFLECTS))
		{
			if (!pImageAgg->FindScalarByKey("Annotate_Reflects"))
			{
				pImageAgg->AddScalarInt("Annotate_Reflects",NULL,1);
			}
		}

		// Aperture
		if (tupKeyValueContext.ValueValidAt(APERTURE) && tupKeyValueContext.GetValueAsBool(APERTURE))
		{
			if (!pImageAgg->FindScalarByKey("Annotate_Aperture"))
			{
				pImageAgg->AddScalarInt("Annotate_Aperture",NULL,1);
			}
		}


	}
}

// method will load the image data into memory (uses attributes to help with the load)
void ProcessTup::LoadImageData(void)
{
	int numImages = m_pTupImagePool->GetNumImages();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
			continue;
		TupImage tupImage(pImageAgg);
		char *pImageFileName;
		// get filename for image
		tupImage.GetFilePathName(&pImageFileName);

		// get attributes
		char *pImageAttributeString;
		tupImage.GetUserProperties(&pImageAttributeString);

		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		if (imageManager.IsImageLoaded(pImageFileName))
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Warning - Image %s already loaded into memory - attributes not applied.",pImageFileName);
		}
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName,pImageAttributeString,m_pTupMapConfigRoot);
	}
}




// add width,height, etc to the image data (so everyone does not need to scan so much)
void ProcessTup::AddImageData(void)
{
	int numImages = m_pTupImagePool->GetNumImages();

	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
			continue;

		TupImage tupImage(pImageAgg);

		char *pImageFileName;
		// get filename for image
		tupImage.GetFilePathName(&pImageFileName);

		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		int imageManagerIndex = imageManager.GetImageIndex(pImageFileName);
		TupString actualImagePathName = imageManager.GetImageFilename(imageManagerIndex);

		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(imageManagerIndex);
		assert(pImageDataAgg);

		int dummyImage = 0;
		TupperwareScalar *pDummyImageScalar = pImageDataAgg->FindScalarByKey("Dummy");
		if (pDummyImageScalar)
		{
			dummyImage = pDummyImageScalar->GetAsInt();
		}

		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		assert(numFrames>0);
		TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0); // first frame only (for now)
		assert(pImageDataFrameAgg);
		TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
		int formatFlags;
		int hasAlpha = FALSE;
		int has8BitAlpha = FALSE;
		tupImageDataFrame.GetFormatFlags(formatFlags);
		if (formatFlags&TupImageDataFrame::HAS_ALPHA)
		{
			hasAlpha = TRUE;
			if (formatFlags&TupImageDataFrame::HAS_8BITALPHA)
				has8BitAlpha = TRUE;
		}

		int numMaps = tupImageDataFrame.GetNumImageDataMaps();
		assert(numMaps>0);
		TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(0);
		TupImageDataMap tupImageDataMap(pImageDataMapAgg);
		int imageWidth;
		tupImageDataMap.GetWidth(imageWidth);
		int imageHeight;
		tupImageDataMap.GetHeight(imageHeight);
		int numPixels = imageWidth*imageHeight;

		// mip maps
		int numPixelsMipMaps = 0;
		for (int mapIndex=1;mapIndex<numMaps;mapIndex++)
		{
			TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(mapIndex);
			TupImageDataMap tupImageDataMap(pImageDataMapAgg);
			int imageWidth;
			tupImageDataMap.GetWidth(imageWidth);
			int imageHeight;
			tupImageDataMap.GetHeight(imageHeight);
			numPixelsMipMaps += imageWidth*imageHeight;
		}

		int imageSize = 0;
		int imageSizeMipMaps = 0;

		TupString formatString;

		switch (formatFlags & TupImageDataFrame::FORMAT_MASK)
		{
			case TupImageDataFrame::RGBA_8888:
				formatString = "RGB8888";
				imageSize = numPixels*4;
				imageSizeMipMaps = numPixelsMipMaps*4;
				break;
			case TupImageDataFrame::RGB_888:
				formatString = "RGB888 ";
				imageSize = numPixels*3;
				imageSizeMipMaps = numPixelsMipMaps*3;
				break;
			case TupImageDataFrame::RGB_565:
				formatString = "RGB565 ";
				imageSize = numPixels*2;
				imageSizeMipMaps = numPixelsMipMaps*2;
				break;
			case TupImageDataFrame::RGB_555:
				imageSize =  numPixels*2;
				formatString = "RGB555 ";
				imageSizeMipMaps = numPixelsMipMaps*2;
				break;
			case TupImageDataFrame::RGBA_1555:
				formatString = "RGB1555";
				imageSize =  numPixels*2;
				imageSizeMipMaps = numPixelsMipMaps*2;
				break;
			case TupImageDataFrame::INDEXED_8:
				formatString = "8 Bit  ";
				imageSize = numPixels;
				imageSizeMipMaps = numPixelsMipMaps;
				break;
			case TupImageDataFrame::INDEXED_4:
				formatString = "4 Bit  ";
				imageSize = (numPixels)>>1;
				imageSizeMipMaps = (numPixelsMipMaps)>>1;
				break;
		}
		pImageAgg->AddScalarInt("Annotate_NumFrames",NULL,numFrames);
		pImageAgg->AddScalarInt("Annotate_NumMaps",NULL,numMaps);
		pImageAgg->AddScalarInt("Annotate_ImageHasAlpha",NULL,hasAlpha);
		pImageAgg->AddScalarInt("Annotate_ImageHas8BitAlpha",NULL,has8BitAlpha);
		pImageAgg->AddScalarInt("Annotate_ImageWidth",NULL,imageWidth);
		pImageAgg->AddScalarInt("Annotate_ImageHeight",NULL,imageHeight);
		pImageAgg->AddScalarInt("Annotate_ImageFlags",NULL,formatFlags);
		pImageAgg->AddScalarString("Annotate_ActualFilePath",NULL,actualImagePathName);
		pImageAgg->AddScalarInt("Annotate_DummyImage",NULL,dummyImage);
		pImageAgg->AddScalarString("Annotate_ImageFormatString",NULL,formatString);
		pImageAgg->AddScalarInt("Annotate_ImageSize",NULL,imageSize);
		pImageAgg->AddScalarInt("Annotate_ImageSizeMipMaps",NULL,imageSizeMipMaps);
	}
}

void ProcessTup::AddAnimatedImagesToLog(void)
{
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Animated Textures:");

	int numImages = m_pTupImagePool->GetNumImages();

	bool bFoundAnimatedTexture = false;

	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
			continue;

		TupImage tupImage(pImageAgg);

		char *pImageFileName;
		// get filename for image
		tupImage.GetFilePathName(&pImageFileName);

		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		int imageManagerIndex = imageManager.GetImageIndex(pImageFileName);
		TupString actualImagePathName = imageManager.GetImageFilename(imageManagerIndex);

		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(imageManagerIndex);
		assert(pImageDataAgg);

		int dummyImage = 0;
		TupperwareScalar *pDummyImageScalar = pImageDataAgg->FindScalarByKey("Dummy");
		if (pDummyImageScalar)
		{
			dummyImage = pDummyImageScalar->GetAsInt();
		}

		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		if (numFrames>1)
		{
			bFoundAnimatedTexture = true;
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Image %s contains %d frames",(const char *)actualImagePathName,numFrames);
			for (int frameIndex=0;frameIndex<numFrames;frameIndex++)
			{
				TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(frameIndex); // first frame only (for now)
				assert(pImageDataFrameAgg);
				TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
				TupperwareScalar *pSourceFileNameScalar = pImageDataFrameAgg->FindScalarByKey("Collage_SourceFileName");
				assert(pSourceFileNameScalar);
				const char *pSourceFileName = pSourceFileNameScalar->GetAsString();
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"  Frame %2d of %2d from file %s",frameIndex+1,numFrames,pSourceFileName);
			}
		}
	}
	if (!bFoundAnimatedTexture)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"No Animated Images Found");
	}
}


class ImageSortData
{
public:
	int m_width;
	int m_height;
	int m_numFrames;
	int m_numStoredFrames;
	int m_numMipMaps;
	int m_size;
	char *m_pOriginalPath;
	char *m_pActualPath;
	int m_dummy;
	int m_reversed;
	char *m_pFormatString;
	int m_imageRef;
	int m_numPartitions;
	
	ImageSortData(void) : m_width(0),m_height(0),m_numFrames(0),m_numMipMaps(0),m_size(0),m_pOriginalPath(NULL),m_pActualPath(NULL),m_dummy(0),m_reversed(0),m_pFormatString(NULL),m_imageRef(-1),m_numPartitions(0)
	{
	}
	
	bool ImageSortData::operator==(const ImageSortData& other) const
	{
		return (m_size==other.m_size &&
			strcmp(m_pOriginalPath,other.m_pOriginalPath)==0);
	}
	
	bool ImageSortData::operator<=(const ImageSortData& other) const
	{
		if (m_size>other.m_size)
			return true;
		else if (m_size<other.m_size)
			return false;
		// size ==
		// return true if path is less than or equal other
		return (strcmp(m_pOriginalPath,other.m_pOriginalPath)<1);
	}
	
	bool ImageSortData::operator>=(const ImageSortData& other) const
	{
		if (m_size<other.m_size)
			return true;
		else if (m_size>other.m_size)
			return false;
		// size ==
		// return true if path is greater than or equal other
		return (strcmp(m_pOriginalPath,other.m_pOriginalPath)>-1);
	}
	
	bool ImageSortData::operator<(const ImageSortData& other) const
	{
		if (m_size>other.m_size)
			return true;
		else if (m_size<other.m_size)
			return false;
		// size ==
		// return true if path is less than other
		return (strcmp(m_pOriginalPath,other.m_pOriginalPath)<0);
	}
	
	bool ImageSortData::operator>(const ImageSortData& other) const
	{
		if (m_size<other.m_size)
			return true;
		else if (m_size>other.m_size)
			return false;
		// size ==
		// return true if path is greater than other
		return (strcmp(m_pOriginalPath,other.m_pOriginalPath)>0);
	}
};

void ProcessTup::AddSortedImagesToLog(void)
{
	int numVisibilitySets = m_visibilitySetNodeRefs.GetSize();

	int numImages = m_pTupImagePool->GetNumImages();

	TupArray<ImageSortData> sortedImages;

	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
			continue;

		if (!pImageAgg->FindScalarByKey("Annotate_HasTextureID")->GetAsInt())
			continue;

		TupImage tupImage(pImageAgg);

		ImageSortData imageSortData;

		tupImage.GetFilePathName(&imageSortData.m_pOriginalPath);
		imageSortData.m_pActualPath = pImageAgg->FindScalarByKey("Annotate_ActualFilePath")->GetAsString();
		imageSortData.m_numFrames = pImageAgg->FindScalarByKey("Annotate_NumFrames")->GetAsInt();
		imageSortData.m_numStoredFrames = imageSortData.m_numFrames;
		TupperwareScalar *pRemappedAnimatedFramesScalar = pImageAgg->FindScalarByKey("Annotate_RemappedAnimatedFrames");
		if (pRemappedAnimatedFramesScalar)
		{
			imageSortData.m_numStoredFrames -= pRemappedAnimatedFramesScalar->GetAsInt();
		}

		imageSortData.m_numMipMaps = pImageAgg->FindScalarByKey("Annotate_NumMaps")->GetAsInt();
		imageSortData.m_width = pImageAgg->FindScalarByKey("Annotate_ImageWidth")->GetAsInt();
		imageSortData.m_height = pImageAgg->FindScalarByKey("Annotate_ImageHeight")->GetAsInt();
		imageSortData.m_dummy = pImageAgg->FindScalarByKey("Annotate_DummyImage")->GetAsInt();
		int numFramesForSize = imageSortData.m_numFrames;
		// adjust number
		// size is (first frame + mip maps)*numframes
		imageSortData.m_size = pImageAgg->FindScalarByKey("Annotate_ImageSize")->GetAsInt();
		imageSortData.m_size += pImageAgg->FindScalarByKey("Annotate_ImageSizeMipMaps")->GetAsInt();
		imageSortData.m_size *= imageSortData.m_numStoredFrames;
		imageSortData.m_pFormatString = pImageAgg->FindScalarByKey("Annotate_ImageFormatString")->GetAsString();

		TupperwareScalar *pImageReversedScalar = pImageAgg->FindScalarByKey("Annotate_ImageReversed");
		if (pImageReversedScalar)
		{
			imageSortData.m_reversed = pImageReversedScalar->GetAsInt();
		}
		int *pTextureIDs = pImageAgg->FindListByKey("Annotate_TextureIDs")->GetAsInt();
		for (int setIndex=0;setIndex<numVisibilitySets;setIndex++)
		{
			if (pTextureIDs[setIndex]!=-1 && (pTextureIDs[setIndex]&0xff000000)==0)
			{
				imageSortData.m_numPartitions++;
			}
		}
		sortedImages.Add(imageSortData);
	}
	int numSortedImages = sortedImages.GetSize();
	TupSort<ImageSortData>::SortAscending(sortedImages.GetData(),numSortedImages);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Sorted Texture List:");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"                                         Stored ");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"  Size    Width Height Partitions Frames Frames MipMaps Format  Path (*=Remapped X=Dummy Texture R=Reversed)");
	int totalSize=0;
	for (imageIndex=0;imageIndex<numSortedImages;imageIndex++)
	{
		ImageSortData &imageSortData = sortedImages[imageIndex];
		char dummyChar = imageSortData.m_dummy ? 'X' : ' ';
		char pathPrefix = (stricmp(imageSortData.m_pOriginalPath,imageSortData.m_pActualPath)==0) ? ' ' : '*';
		char reversedChar = imageSortData.m_reversed ? 'R' : ' ';

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"%8d  %4d  %4d      %3d      %2d    %2d    %2d    %s  %c%c%c%s",
			imageSortData.m_size,imageSortData.m_width,imageSortData.m_height,imageSortData.m_numPartitions,imageSortData.m_numFrames,imageSortData.m_numStoredFrames,imageSortData.m_numMipMaps,
			imageSortData.m_pFormatString,reversedChar,dummyChar,pathPrefix,imageSortData.m_pActualPath);

		totalSize += imageSortData.m_size;
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Total Textures: %d",numSortedImages);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Total texture size: %d bytes.",totalSize);
}

void ProcessTup::FixDiffuseMapImages(void)
{
	Keeper<int> imagesToFix;
	int numMaps = m_pTupMapPool->GetNumMaps();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapIndex);

		if (!pMapAgg)
			continue;

		TupMap tupMap(pMapAgg);
		if (tupMap.GetMapType()==TupMap::BITMAP_MAP)
		{
			TupBitmapMap tupBitmapMap(pMapAgg);
			int blendMode;
			TupperwareScalar *pBlendModeScalar = tupBitmapMap.GetBlendMode(blendMode);
			// if a multiply (diffuse)
			if (pBlendModeScalar && blendMode==TupBitmapMap::BLEND_MODE_MULTIPLY)
			{
				int imageReference;
				tupBitmapMap.GetImageReference(imageReference);
				if (imageReference!=-1)
				{
					imagesToFix.Add(imageReference);
				}
			}
		}
	}
	int numImagesToFix = imagesToFix.GetSize();
	for (int imageIndex=0;imageIndex<numImagesToFix;imageIndex++)
	{
		int imageReference = imagesToFix[imageIndex];
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageReference);
		assert(pImageAgg);
		TupImage tupImage(pImageAgg);
		// get filename for image
		char *pImageFileName;
		tupImage.GetFilePathName(&pImageFileName);
		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
		assert(pImageDataAgg);
		// convert image data to have the alpha channel have the luminance of the color
		TupImageData tupImageData(pImageDataAgg);
		ImageUtilities::ConvertLuminanceToAlpha(tupImageData);
		// mark image as being used as a diffuse map
		pImageAgg->AddScalarInt("Annotate_DiffuseMap",NULL,TRUE);
	}
}

// method will rotate any images that are taller than wide and mark the image data so the uv will be rotated as well
void ProcessTup::RotateTallImages(void)
{
	int numImages = m_pTupImagePool->GetNumImages();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
			continue;
		TupImage tupImage(pImageAgg);
		char *pImageFileName;
		// get filename for image
		tupImage.GetFilePathName(&pImageFileName);
		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
		assert(pImageDataAgg);
		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		assert(numFrames>0);
		TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0); // first frame only (for now)
		assert(pImageDataFrameAgg);
		TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
		int numMaps = tupImageDataFrame.GetNumImageDataMaps();
		assert(numMaps>0);
		TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(0);
		TupImageDataMap tupImageDataMap(pImageDataMapAgg);
		int imageWidth;
		tupImageDataMap.GetWidth(imageWidth);
		int imageHeight;
		tupImageDataMap.GetHeight(imageHeight);
		if (imageHeight>imageWidth)
		{
			// rotate the image
			ImageUtilities::Reverse(tupImageData);
			pImageAgg->AddScalarInt("Annotate_ImageReversed",NULL,TRUE);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Reversed image %s due to height = %d > width = %d",pImageFileName,imageHeight,imageWidth);
		}
	}
}

// method will scale any images that are two narrow
void ProcessTup::ScaleNarrowImages(void)
{
	int numImages = m_pTupImagePool->GetNumImages();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
			continue;
		TupImage tupImage(pImageAgg);
		char *pImageFileName;
		// get filename for image
		tupImage.GetFilePathName(&pImageFileName);
		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
		assert(pImageDataAgg);
		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		assert(numFrames>0);
		TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0); // first frame only (for now)
		assert(pImageDataFrameAgg);
		TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
		int numMaps = tupImageDataFrame.GetNumImageDataMaps();
		assert(numMaps>0);
		TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(0);
		TupImageDataMap tupImageDataMap(pImageDataMapAgg);
		int imageWidth;
		tupImageDataMap.GetWidth(imageWidth);
		int imageHeight;
		tupImageDataMap.GetHeight(imageHeight);
		int formatFlags;
		tupImageDataFrame.GetFormatFlags(formatFlags);
		int minWidth;
		switch (formatFlags & TupImageDataFrame::FORMAT_MASK)
		{
			case TupImageDataFrame::RGBA_8888:
				minWidth = 2;
				break;
			case TupImageDataFrame::RGB_888:
			case TupImageDataFrame::INDEXED_8:
			case TupImageDataFrame::INDEXED_4:
				minWidth = 8;
				break;
			case TupImageDataFrame::RGB_555:
			case TupImageDataFrame::RGB_565:
			case TupImageDataFrame::RGBA_1555:
				minWidth = 4;
				break;
			default:
				assert(false);
		}
		if (imageWidth<minWidth)
		{
			// rotate the image
			ImageUtilities::Scale(tupImageData,minWidth,imageHeight);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Image %s scaled from %d to %d wide due to being too narrow",pImageFileName,imageWidth,minWidth);
		}
	}
}

static int GetClosestPowerOf2(int value)
{
	int power = 1;
	while (value>power)
	{
		power=power<<1;
	}
	int deltaPower = power-value;
	int deltaLowerPower = value-(power>>1);
	if (deltaPower<deltaLowerPower)
		return power;
	else
		return power>>1;
}

// method will scale any images that are not a power of 2
void ProcessTup::ScaleNonPowerOf2Images(void)
{
	int numImages = m_pTupImagePool->GetNumImages();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
			continue;
		TupImage tupImage(pImageAgg);
		char *pImageFileName;
		// get filename for image
		tupImage.GetFilePathName(&pImageFileName);
		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
		assert(pImageDataAgg);
		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		assert(numFrames>0);
		TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0); // first frame only (for now)
		assert(pImageDataFrameAgg);
		TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
		int numMaps = tupImageDataFrame.GetNumImageDataMaps();
		assert(numMaps>0);
		TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(0);
		TupImageDataMap tupImageDataMap(pImageDataMapAgg);
		int imageWidth;
		tupImageDataMap.GetWidth(imageWidth);
		int imageHeight;
		tupImageDataMap.GetHeight(imageHeight);
		int closestPowerOf2Width = GetClosestPowerOf2(imageWidth);
		int closestPowerOf2Height = GetClosestPowerOf2(imageHeight);

		if (closestPowerOf2Width!=imageWidth ||
			 closestPowerOf2Height!=imageHeight)
		{
			// scale the image 
			ImageUtilities::Scale(tupImageData,closestPowerOf2Width,closestPowerOf2Height);
			ImageUtilities::MarkAsFixTexture(tupImageData);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Non Power of 2 image %s scaled from %d,%d to %d,%d - also marked with pink pattern",
				pImageFileName,imageWidth,imageHeight,closestPowerOf2Width,closestPowerOf2Height);
		}
	}
}

void ProcessTup::AddParentObjectReferences(void)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if empty slot - continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::DERIVED)
		{
			TupDerived tupDerived(pObjectAgg);
			int childReference;
			tupDerived.GetObjectReference(childReference);
			assert(childReference!=-1);
			TupperwareAggregate *pChildObjectAgg = m_pTupObjectPool->GetObjectAggregate(childReference);
			assert(pChildObjectAgg);
			// update the number of parent objects for the object
			TupperwareList *pParentObjectReferenceList = pChildObjectAgg->FindListByKey("Annotate_ParentObjectReferenceList");
			if (pParentObjectReferenceList)
			{
				// we there is already a list
				// check to see if the objectIndex is already there
				// if not add it to the list
				int numEntries = pParentObjectReferenceList->GetLength();
				int *pParentObjectReferenceListData = pParentObjectReferenceList->GetAsInt();
				for (int i=0;i<numEntries;i++)
				{
					if (pParentObjectReferenceListData[i]==objectIndex)
						break;
				}
				// if we didn't find it
				if (i==numEntries)
				{
					// copy current list to another and add the new one
					int *pNewParentObjectReferenceListData = new int[numEntries+1];
					for (i=0;i<numEntries;i++)
					{
						pNewParentObjectReferenceListData[i] = pParentObjectReferenceListData[i];
					}
					pNewParentObjectReferenceListData[numEntries] = objectIndex;
					// pass to tree (tree will manage deleting the old one and managing the new one)
					pParentObjectReferenceList->SetAsInt(pNewParentObjectReferenceListData,numEntries+1);
					delete pNewParentObjectReferenceListData;
				}
			}
			else
			{
				// if no list then then add one with my object index
				int *pNewParentObjectReferenceListData = new int[1];
				pNewParentObjectReferenceListData[0] = objectIndex;
				pChildObjectAgg->AddListInt("Annotate_ParentObjectReferenceList",NULL,pNewParentObjectReferenceListData,1);
				delete pNewParentObjectReferenceListData;
			}
		}
	}
}

void ProcessTup::AddParentNodeReferences(void)
{
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);
		int childReference;
		tupNode.GetObjectReference(childReference);
		if (childReference==-1)
		{
			continue; // skip the root node
		}
		TupperwareAggregate *pChildObjectAgg = m_pTupObjectPool->GetObjectAggregate(childReference);
		assert(pChildObjectAgg);
		// update the number of parent objects for the object
		TupperwareList *pParentNodeReferenceList = pChildObjectAgg->FindListByKey("Annotate_ParentNodeReferenceList");
		if (pParentNodeReferenceList)
		{
			// we there is already a list
			// check to see if the nodeIndex is already there
			// if not add it to the list
			int numEntries = pParentNodeReferenceList->GetLength();
			int *pParentNodeReferenceListData = pParentNodeReferenceList->GetAsInt();
			for (int i=0;i<numEntries;i++)
			{
				if (pParentNodeReferenceListData[i]==nodeIndex)
					break;
			}
			// if we didn't find it
			if (i==numEntries)
			{
				// copy current list to another and add the new one
				int *pNewParentNodeReferenceListData = new int[numEntries+1];
				for (i=0;i<numEntries;i++)
				{
					pNewParentNodeReferenceListData[i] = pParentNodeReferenceListData[i];
				}
				pNewParentNodeReferenceListData[numEntries] = nodeIndex;
				// pass to tree (tree will manage deleting the old one and managing the new one)
				pParentNodeReferenceList->SetAsInt(pNewParentNodeReferenceListData,numEntries+1);
				delete pNewParentNodeReferenceListData;
			}
		}
		else
		{
			// if no list then then add one with my object index
			int *pNewParentNodeReferenceListData = new int[1];
			pNewParentNodeReferenceListData[0] = nodeIndex;
			pChildObjectAgg->AddListInt("Annotate_ParentNodeReferenceList",NULL,pNewParentNodeReferenceListData,1);
			delete pNewParentNodeReferenceListData;
		}
	}
}

void ProcessTup::AddBaseObjectParentNodeReferences(void)
{
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		
		// if empty slot - continue
		if (!pNodeAgg)
			continue;
		
		TupNode tupNode(pNodeAgg);
		int childReference;
		tupNode.GetObjectReference(childReference);
		if (childReference==-1)
		{
			continue; // skip the root node
		}
		int baseObjectRef = AnnotateHelper::FindBaseObjectRef(*m_pTupObjectPool,childReference);
		TupperwareAggregate *pBaseObjectAgg = m_pTupObjectPool->GetObjectAggregate(baseObjectRef);
		assert(pBaseObjectAgg);
		// update the number of parent objects for the object
		TupperwareList *pParentNodeReferenceList = pBaseObjectAgg->FindListByKey("Annotate_BaseObjectParentNodeReferenceList");
		if (pParentNodeReferenceList)
		{
			// we there is already a list
			// check to see if the nodeIndex is already there
			// if not add it to the list
			int numEntries = pParentNodeReferenceList->GetLength();
			int *pParentNodeReferenceListData = pParentNodeReferenceList->GetAsInt();
			for (int i=0;i<numEntries;i++)
			{
				if (pParentNodeReferenceListData[i]==nodeIndex)
					break;
			}
			// if we didn't find it
			if (i==numEntries)
			{
				// copy current list to another and add the new one
				int *pNewParentNodeReferenceListData = new int[numEntries+1];
				for (i=0;i<numEntries;i++)
				{
					pNewParentNodeReferenceListData[i] = pParentNodeReferenceListData[i];
				}
				pNewParentNodeReferenceListData[numEntries] = nodeIndex;
				// pass to tree (tree will manage deleting the old one and managing the new one)
				pParentNodeReferenceList->SetAsInt(pNewParentNodeReferenceListData,numEntries+1);
				delete pNewParentNodeReferenceListData;
			}
		}
		else
		{
			// if no list then then add one with my object index
			int *pNewParentNodeReferenceListData = new int[1];
			pNewParentNodeReferenceListData[0] = nodeIndex;
			pBaseObjectAgg->AddListInt("Annotate_BaseObjectParentNodeReferenceList",NULL,pNewParentNodeReferenceListData,1);
			delete pNewParentNodeReferenceListData;
		}
	}
}

// add a scalar int to each node for the path index to use
void ProcessTup::AddNodePathIndexes(void)
{
	m_totalPaths = 0; // start with no paths

	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef!=-1)
		{
			TupperwareAggregate *pBasePathAgg = AnnotateHelper::FindBasePath(*m_pTupObjectPool,objectRef);
			if (pBasePathAgg)
			{
				int totalSplines = 0;
				TupObject tupObject(pBasePathAgg);
				if (tupObject.GetObjectType()==TupObject::BEZIER_SHAPE)
				{
					TupBezierShape tupBezierShape(pBasePathAgg);
					totalSplines = tupBezierShape.GetNumSplines();
				}
				else if (tupObject.GetObjectType()==TupObject::SPLINE_SHAPE)
				{
					TupSplineShape tupSplineShape(pBasePathAgg);
					totalSplines = tupSplineShape.GetNumSplines();
				}
				// store path index and keep track of the total
				if (totalSplines)
				{
					pNodeAgg->AddScalarInt("Annotate_PathIndex",NULL,m_totalPaths);

					// set up the name of the path
					char *pName;
					tupNode.GetName(&pName);
					TupString pathName(pName); // this is the default name

					// check and set flags based on instance attributes
					// get my attributes from the node
					char *pNodeAttributesString;
					tupNode.GetUserProperties(&pNodeAttributesString);
					// count how many named points we have
					TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_OBJ_BLOCK,END_OBJ_BLOCK);
					TupKeyValueContext tupKeyValueContext;
					tupKeyValueContext.AddKey("named_path_name");
					m_pTupNodeConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
					// if set to something other than an empty string then use the default name
					if (tupKeyValueContext.ValueValidAt(0) && strlen(tupKeyValueContext.GetValueAsString(0)))
					{
						pathName = tupKeyValueContext.GetValueAsString(0);
					}
					pNodeAgg->AddScalarString("Annotate_PathName",NULL,pathName);

					m_totalPaths+=totalSplines; // all paths are counted
				}
			}
		}
	}
}

void ProcessTup::PropagateVertexColors(bool bForceDefault)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if entry is empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			if (bForceDefault)
			{
				TupTriMesh tupBaseMesh(pObjectAgg);
				// build an aggregate as a child of the derived object to hold the vertex colors
				TupperwareAggregate *pVertexColorMeshMapAgg = pObjectAgg->AddAggregate("Annotate_VertexColorMeshMap","0");
				TupTriMeshMap tupTriMeshMap(pVertexColorMeshMapAgg);

				// get the number of faces from the mesh
				int numMapFaces;
				int *pTemp;
				tupBaseMesh.GetPositionIndices(&pTemp,numMapFaces);
				assert(numMapFaces>0);
				int *pMapIndices = new int[numMapFaces*3];
				memset(pMapIndices,0,sizeof(int)*(numMapFaces*3)); // set to 0
				float whiteColor[3] = {1.0f,1.0f,1.0f};
				int mapFlags = 0;
				tupTriMeshMap.AddMapVerts(whiteColor,1);
				tupTriMeshMap.AddMapIndices(pMapIndices,numMapFaces);
				tupTriMeshMap.AddMapFlags(mapFlags);
				delete pMapIndices;
			}
			else
			{
				TupperwareList *pParentObjectReferenceList = pObjectAgg->FindListByKey("Annotate_ParentObjectReferenceList");
				if (pParentObjectReferenceList)
				{
					// go through the parent objects (those that point to me)
					int numEntries = pParentObjectReferenceList->GetLength();
					int *pParentObjectReferenceListData = pParentObjectReferenceList->GetAsInt();
					for (int i=0;i<numEntries;i++)
					{
						PropagateVertexColors(pParentObjectReferenceListData[i]);
					}
				}
			}
		}
	}
}

void ProcessTup::PropagateVertexColors(int objectRef)
{
	TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupDerived tupDerived(pObjectAgg);
		int totalModifierSlots = tupDerived.GetNumModifierSlots();
		// if there is any modifiers
		if (totalModifierSlots)
		{
			bool foundValidModifier = false;
			// loop through the modifiers and see if any deal with the vertex colors
			for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pDerivedModifierAgg)
				{
					TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
					int modifierReference;
					tupDerivedModifier.GetModifierReference(modifierReference);
					TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierReference);
					assert(pModifierAgg);
					TupModifier tupModifier(pModifierAgg);
					switch(tupModifier.GetModifierType())
					{
						case TupModifier::APPLY_VC_MODIFIER:
							{
								TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
								int flags;
								tupApplyVCModifier.GetFlags(flags);
								if (flags&TupApplyVCModifier::FLAGS_ENABLED)
								{
									foundValidModifier = true;
								}
							}
							break;
						case TupModifier::VERTEX_PAINT_MODIFIER:
							{
								TupVertexPaintModifier tupVertexPaintModifier(pModifierAgg);
								int flags;
								tupVertexPaintModifier.GetFlags(flags);
								if (flags&TupVertexPaintModifier::FLAGS_ENABLED)
								{
									foundValidModifier = true;
								}
							}
							break;
						case TupModifier::HOLD_VC_MODIFIER:
							{
								TupHoldVCModifier tupHoldVCModifier(pModifierAgg);
								int flags;
								tupHoldVCModifier.GetFlags(flags);
								if (flags&TupHoldVCModifier::FLAGS_ENABLED)
								{
									foundValidModifier = true;
								}
							}
							break;
					}
				}
				// stop searching if there is one
				if (foundValidModifier)
					break;
			}
			// if there is a modifier that does vertex colors on the derived object
			if (foundValidModifier)
			{
				TupTriMesh tupDerivedMesh(pObjectAgg); // use to add the mesh maps
				TupperwareAggregate *pChildMeshMapAgg = AnnotateHelper::FindChildVertexColorMeshMap(*m_pTupObjectPool,objectRef);
				TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
				// do only when we have a base mesh
				if (pBaseMeshAgg)
				{
					TupTriMesh tupBaseMesh(pBaseMeshAgg);
					// build an aggregate as a child of the derived object to hold the vertex colors
					TupperwareAggregate *pVertexColorMeshMapAgg = pObjectAgg->AddAggregate("Annotate_VertexColorMeshMap","0");
					TupTriMeshMap tupTriMeshMap(pVertexColorMeshMapAgg);

					// if we have a vertex color mesh map to start with
					if (pChildMeshMapAgg)
					{
						// copy the child mesh map to our aggregate
						TupTriMeshMap tupChildMeshMap(pChildMeshMapAgg);
						assert(tupChildMeshMap.GetMapChannel()==TupTriMeshMap::MAP_CHANNEL_VERTEX_COLOR); // better be vertex color channel
						int numMapVerts;
						int numMapFaces;
						float *pMapVerts;
						int *pMapIndices;
						int mapFlags;
						tupChildMeshMap.GetMapVerts(&pMapVerts,numMapVerts);
						tupChildMeshMap.GetMapIndices(&pMapIndices,numMapFaces);
						tupChildMeshMap.GetMapFlags(mapFlags);
						tupTriMeshMap.AddMapVerts(pMapVerts,numMapVerts);
						tupTriMeshMap.AddMapIndices(pMapIndices,numMapFaces);
						tupTriMeshMap.AddMapFlags(mapFlags);
					}
					// if there is no vertex colors to start with then we need to build one
					else
					{
						// get the number of faces from the mesh
						int numMapFaces;
						int *pTemp;
						tupBaseMesh.GetPositionIndices(&pTemp,numMapFaces);
						assert(numMapFaces>0);
						int *pMapIndices = new int[numMapFaces*3];
						memset(pMapIndices,0,sizeof(int)*(numMapFaces*3)); // set to 0
						float whiteColor[3] = {1.0f,1.0f,1.0f};
						int mapFlags = 0;
						tupTriMeshMap.AddMapVerts(whiteColor,1);
						tupTriMeshMap.AddMapIndices(pMapIndices,numMapFaces);
						tupTriMeshMap.AddMapFlags(mapFlags);
						delete pMapIndices;
					}
					// loop through the modifiers and apply the modifier to the vertex color on the derived object
					for (int modifierIndex=totalModifierSlots-1;modifierIndex>=0;modifierIndex--)
					{
						TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
						if (pDerivedModifierAgg)
						{
							TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
							int modifierReference;
							tupDerivedModifier.GetModifierReference(modifierReference);
							TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
							assert(tupDerivedModifier.GetModifierIndex()==modifierIndex);
							TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierReference);
							assert(pModifierAgg);
							TupModifier tupModifier(pModifierAgg);
							switch(tupModifier.GetModifierType())
							{
								case TupModifier::APPLY_VC_MODIFIER:
									ApplyApplyVCModifierForVertexColor(tupBaseMesh,tupModifier,pModifierContextAgg,tupTriMeshMap);
									break;
								case TupModifier::VERTEX_PAINT_MODIFIER:
									ApplyVertexPaintModifierForVertexColor(tupBaseMesh,tupModifier,pModifierContextAgg,tupTriMeshMap);
									break;
								case TupModifier::HOLD_VC_MODIFIER:
									ApplyHoldVCModifierForVertexColor(tupBaseMesh,tupModifier,pModifierContextAgg,tupTriMeshMap);
									break;
							}
						}
					}
				}
			}
		}
		TupperwareList *pParentObjectReferenceList = pObjectAgg->FindListByKey("Annotate_ParentObjectReferenceList");
		if (pParentObjectReferenceList)
		{
			// we there is already a list
			// check to see if the objectIndex is already there
			// if not add it to the list
			int numEntries = pParentObjectReferenceList->GetLength();
			int *pParentObjectReferenceListData = pParentObjectReferenceList->GetAsInt();
			for (int i=0;i<numEntries;i++)
			{
				PropagateVertexColors(pParentObjectReferenceListData[i]);
			}
		}
	}
}

void ProcessTup::ApplyApplyVCModifierForVertexColor(TupTriMesh &tupBaseMesh,TupModifier &tupModifier,TupperwareAggregate *pModifierContextAgg,TupTriMeshMap &tupTriMeshMap)
{
	TupperwareAggregate *pModifierAgg = tupModifier.GetBaseAggregate();
	TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
	int flags;
	tupApplyVCModifier.GetFlags(flags);
	if (flags & TupApplyVCModifier::FLAGS_ENABLED)
	{
		// get the number of faces from the mesh
		int numSourceFaces;
		int *pSourceIndices;
		tupTriMeshMap.GetMapIndices(&pSourceIndices,numSourceFaces);

		// get the modifier information
		int numModifierVerts;
		int numModifierIndices;
		Vector3 *pModifierVerts;
		int *pModifierIndices;
		tupApplyVCModifier.GetColorVerts((float **)&pModifierVerts,numModifierVerts);
		tupApplyVCModifier.GetColorIndices(&pModifierIndices,numModifierIndices);

		// set up the destination information
		int *pDestIndices = new int[numSourceFaces*3];
		Keeper<Vector3> destVerts;

		if (flags & TupApplyVCModifier::FLAGS_MIXED_COLORS)
		{
			// mixed colors are in sync with the position pool
			int numPositionVerts;
			int numPositionFaces;
			Vector3 *pPositionVerts;
			int *pPositionIndices;
			tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
			tupBaseMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);
			assert(numPositionFaces==numSourceFaces);

			int numPoints = numSourceFaces*3;
			for (int pointIndex=0;pointIndex<numPoints;pointIndex++)
			{
				int modifierIndex;
				int positionIndex = pPositionIndices[pointIndex];
				if (positionIndex>=numModifierIndices)
				{
					modifierIndex=0; // if to big then use the first one
				}
				else
				{
					modifierIndex=pModifierIndices[positionIndex];
				}
				pDestIndices[pointIndex] = destVerts.Add(pModifierVerts[modifierIndex]);
			}
		}
		else
		{
			// each point of each face has an entry
			int numPoints = numSourceFaces*3;
			for (int pointIndex=0;pointIndex<numPoints;pointIndex++)
			{
				int modifierIndex = pointIndex;
				if (modifierIndex>=numModifierIndices)
				{
					modifierIndex=0;
				}
				pDestIndices[pointIndex] = destVerts.Add(pModifierVerts[pModifierIndices[modifierIndex]]);
			}
		}
		tupTriMeshMap.AddMapVerts((float *)destVerts.GetData(),destVerts.GetSize());
		tupTriMeshMap.AddMapIndices(pDestIndices,numSourceFaces);
		delete pDestIndices;
	}
}

void ProcessTup::ApplyVertexPaintModifierForVertexColor(TupTriMesh &tupBaseMesh,TupModifier &tupModifier,TupperwareAggregate *pModifierContextAgg,TupTriMeshMap &tupTriMeshMap)
{
	TupperwareAggregate *pModifierAgg = tupModifier.GetBaseAggregate();
	TupVertexPaintModifier tupVertexPaintModifier(pModifierAgg);
	int flags;
	tupVertexPaintModifier.GetFlags(flags);
	if (flags & TupApplyVCModifier::FLAGS_ENABLED)
	{
		// get information from the base mesh
		int numPositionVerts;
		int numPositionFaces;
		Vector3 *pPositionVerts;
		int *pPositionIndices;
		tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
		tupBaseMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);

		// get the source information
		int numSourceFaces;
		int numSourceVerts;
		Vector3 *pSourceVerts;
		int *pSourceIndices;
		tupTriMeshMap.GetMapVerts((float **)&pSourceVerts,numSourceVerts);
		tupTriMeshMap.GetMapIndices(&pSourceIndices,numSourceFaces);
		assert(numPositionFaces==numSourceFaces);

		TupVertexPaintModifierContext tupVertexPaintModifierContext(pModifierContextAgg);
		Vector3 *pModifierColorData;
		float *pModifierMixFactorData;
		int *pModifierFaceIndexData;
		int *pModifierPointIndexData;
		int numModifierColors;
		tupVertexPaintModifierContext.GetVertexColorChannelColorData((float **)&pModifierColorData,numModifierColors);
		tupVertexPaintModifierContext.GetVertexColorChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
		tupVertexPaintModifierContext.GetVertexColorChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
		tupVertexPaintModifierContext.GetVertexColorChannelPointIndexData(&pModifierPointIndexData,numModifierColors);

		// set up the destination information
		int *pDestIndices = new int[numSourceFaces*3];
		Keeper<Vector3> destVerts;
		// I need an array to keep track of which position points have been given a color by the modifier
		bool *pDestFacePointAddedTable = new bool[numPositionVerts];
		memset(pDestFacePointAddedTable,0,sizeof(bool)*(numPositionVerts));
		// I need an array to keep track of the altered colors (in sync with position pool)
		Vector3 *pDestModifierColors = new Vector3[numPositionVerts];

		// go through the list of modifier colors and add to the table in sync with the position pool
		for (int colorIndex = 0;colorIndex<numModifierColors;colorIndex++)
		{
			Vector3 &color = pModifierColorData[colorIndex];
			float mixFactor = pModifierMixFactorData[colorIndex];
			int faceIndex = pModifierFaceIndexData[colorIndex];
			int pointIndex = pModifierPointIndexData[colorIndex];
			assert(pointIndex>=0 && pointIndex<=2);

			int facePointIndex = (faceIndex<<1)+faceIndex+pointIndex;
			if (faceIndex<numSourceFaces)
			{
				int posVertexIndex = pPositionIndices[facePointIndex];
				Vector3 &sourceColor = pSourceVerts[pSourceIndices[facePointIndex]];
				Vector3 destColor;
				destColor = (mixFactor * color) + ((1.0f - mixFactor) * sourceColor);
				pDestModifierColors[posVertexIndex] = destColor;
				pDestFacePointAddedTable[posVertexIndex] = true;
			}
		}
		// go through all the faces and use the dest color or the source
		int numPoints = numSourceFaces*3;
		for (int pointIndex=0;pointIndex<numPoints;pointIndex++)
		{
			int posVertexIndex = pPositionIndices[pointIndex];
			if (pDestFacePointAddedTable[posVertexIndex])
			{
				// get the color from the modifier array
				pDestIndices[pointIndex] = destVerts.Add(pDestModifierColors[posVertexIndex]);
			}
			else
			{
				// use the source color
				pDestIndices[pointIndex] = destVerts.Add(pSourceVerts[pSourceIndices[pointIndex]]);
			}
		}
		tupTriMeshMap.AddMapVerts((float *)destVerts.GetData(),destVerts.GetSize());
		tupTriMeshMap.AddMapIndices(pDestIndices,numSourceFaces);
		delete pDestIndices;
		delete pDestFacePointAddedTable;
		delete pDestModifierColors;
	}
}

void ProcessTup::ApplyHoldVCModifierForVertexColor(TupTriMesh &tupBaseMesh,TupModifier &tupModifier,TupperwareAggregate *pModifierContextAgg,TupTriMeshMap &tupTriMeshMap)
{
	TupperwareAggregate *pModifierAgg = tupModifier.GetBaseAggregate();
	TupHoldVCModifier tupHoldVCModifier(pModifierAgg);
	int flags;
	tupHoldVCModifier.GetFlags(flags);
	if (flags & TupApplyVCModifier::FLAGS_ENABLED)
	{
		// get the number of faces from the mesh
		int numSourceFaces;
		int *pSourceIndices;
		tupTriMeshMap.GetMapIndices(&pSourceIndices,numSourceFaces);

		// get the modifier information
		int numModifierVerts;
		int numModifierFaces;
		Vector3 *pModifierVerts;
		int *pModifierIndices;
		tupHoldVCModifier.GetColorVerts((float **)&pModifierVerts,numModifierVerts);
		tupHoldVCModifier.GetColorIndices(&pModifierIndices,numModifierFaces);
		int numModifierIndices = numModifierFaces*3;

		// set up the destination information
		int *pDestIndices = new int[numSourceFaces*3];
		Keeper<Vector3> destVerts;

		// each point of each face has an entry
		int numPoints = numSourceFaces*3;
		for (int pointIndex=0;pointIndex<numPoints;pointIndex++)
		{
			int modifierIndex = pointIndex;
			if (modifierIndex>=numModifierIndices)
			{
				modifierIndex=0;
			}
			pDestIndices[pointIndex] = destVerts.Add(pModifierVerts[pModifierIndices[modifierIndex]]);
		}

		tupTriMeshMap.AddMapVerts((float *)destVerts.GetData(),destVerts.GetSize());
		tupTriMeshMap.AddMapIndices(pDestIndices,numSourceFaces);
		delete pDestIndices;
	}
}

void ProcessTup::PropagateVertexAlpha(bool bForceDefault)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if entry is empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			if (bForceDefault)
			{
				TupTriMesh tupBaseMesh(pObjectAgg);
				// build an aggregate as a child of the derived object to hold the vertex colors
				TupperwareAggregate *pVertexAlphaMeshMapAgg = pObjectAgg->AddAggregate("Annotate_VertexAlphaMeshMap","-2");
				TupTriMeshMap tupTriMeshMap(pVertexAlphaMeshMapAgg);

				// get the number of faces from the mesh
				int numMapFaces;
				int *pTemp;
				tupBaseMesh.GetPositionIndices(&pTemp,numMapFaces);
				assert(numMapFaces>0);
				int *pMapIndices = new int[numMapFaces*3];
				memset(pMapIndices,0,sizeof(int)*(numMapFaces*3)); // set to 0
				float whiteColor[3] = {1.0f,1.0f,1.0f};
				int mapFlags = 0;
				tupTriMeshMap.AddMapVerts(whiteColor,1);
				tupTriMeshMap.AddMapIndices(pMapIndices,numMapFaces);
				tupTriMeshMap.AddMapFlags(mapFlags);
				delete pMapIndices;
			}
			else
			{
				TupperwareList *pParentObjectReferenceList = pObjectAgg->FindListByKey("Annotate_ParentObjectReferenceList");
				if (pParentObjectReferenceList)
				{
					// go through the parent objects (those that point to me)
					int numEntries = pParentObjectReferenceList->GetLength();
					int *pParentObjectReferenceListData = pParentObjectReferenceList->GetAsInt();
					for (int i=0;i<numEntries;i++)
					{
						PropagateVertexAlpha(pParentObjectReferenceListData[i]);
					}
				}
			}
		}
	}
}

void ProcessTup::PropagateVertexAlpha(int objectRef)
{
	TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupDerived tupDerived(pObjectAgg);
		int totalModifierSlots = tupDerived.GetNumModifierSlots();
		// if there is any modifiers
		if (totalModifierSlots)
		{
			bool foundValidModifier = false;
			// loop through the modifiers and see if any deal with the vertex alpha
			for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pDerivedModifierAgg)
				{
					TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
					int modifierReference;
					tupDerivedModifier.GetModifierReference(modifierReference);
					TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierReference);
					assert(pModifierAgg);
					TupModifier tupModifier(pModifierAgg);
					switch(tupModifier.GetModifierType())
					{
						case TupModifier::APPLY_VC_MODIFIER:
							{
								TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
								int flags;
								tupApplyVCModifier.GetFlags(flags);
								if ((flags&TupApplyVCModifier::FLAGS_ENABLED) &&
									 (flags&TupApplyVCModifier::FLAGS_HAS_ALPHA))
								{
									foundValidModifier = true;
								}
							}
							break;
						case TupModifier::VERTEX_PAINT_MODIFIER:
							{
								TupVertexPaintModifier tupVertexPaintModifier(pModifierAgg);
								int flags;
								tupVertexPaintModifier.GetFlags(flags);
								if (flags&TupVertexPaintModifier::FLAGS_ENABLED)
								{
									foundValidModifier = true;
								}
							}
							break;
					}
				}
				// stop searching if there is one
				if (foundValidModifier)
					break;
			}
			// if there is a modifier that does vertex colors on the derived object
			if (foundValidModifier)
			{
				TupTriMesh tupDerivedMesh(pObjectAgg); // use to add the mesh maps
				TupperwareAggregate *pChildMeshMapAgg = AnnotateHelper::FindChildVertexAlphaMeshMap(*m_pTupObjectPool,objectRef);
				TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
				// do only when we have a base mesh
				if (pBaseMeshAgg)
				{
					TupTriMesh tupBaseMesh(pBaseMeshAgg);
					// build an aggregate as a child of the derived object to hold the vertex colors
					TupperwareAggregate *pVertexAlphaMeshMapAgg = pObjectAgg->AddAggregate("Annotate_VertexAlphaMeshMap","-2");
					TupTriMeshMap tupTriMeshMap(pVertexAlphaMeshMapAgg);

					// if we have a vertex color mesh map to start with
					if (pChildMeshMapAgg)
					{
						// copy the child mesh map to our aggregate
						TupTriMeshMap tupChildMeshMap(pChildMeshMapAgg);
						assert(tupChildMeshMap.GetMapChannel()==TupTriMeshMap::MAP_CHANNEL_ALPHA); // better be vertex alpha channel
						int numMapVerts;
						int numMapFaces;
						float *pMapVerts;
						int *pMapIndices;
						int mapFlags;
						tupChildMeshMap.GetMapVerts(&pMapVerts,numMapVerts);
						tupChildMeshMap.GetMapIndices(&pMapIndices,numMapFaces);
						tupChildMeshMap.GetMapFlags(mapFlags);
						tupTriMeshMap.AddMapVerts(pMapVerts,numMapVerts);
						tupTriMeshMap.AddMapIndices(pMapIndices,numMapFaces);
						tupTriMeshMap.AddMapFlags(mapFlags);
					}
					// if there is no vertex colors to start with then we need to build one
					else
					{
						// get the number of faces from the mesh
						int numMapFaces;
						int *pTemp;
						tupBaseMesh.GetPositionIndices(&pTemp,numMapFaces);
						assert(numMapFaces>0);
						int *pMapIndices = new int[numMapFaces*3];
						memset(pMapIndices,0,sizeof(int)*(numMapFaces*3)); // set to 0
						float whiteColor[3] = {1.0f,1.0f,1.0f};
						int mapFlags = 0;
						tupTriMeshMap.AddMapVerts(whiteColor,1);
						tupTriMeshMap.AddMapIndices(pMapIndices,numMapFaces);
						tupTriMeshMap.AddMapFlags(mapFlags);
						delete pMapIndices;
					}
					// loop through the modifiers and apply the modifier to the vertex color on the derived object
					for (int modifierIndex=totalModifierSlots-1;modifierIndex>=0;modifierIndex--)
					{
						TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
						if (pDerivedModifierAgg)
						{
							TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
							int modifierReference;
							tupDerivedModifier.GetModifierReference(modifierReference);
							TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
							assert(tupDerivedModifier.GetModifierIndex()==modifierIndex);
							TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierReference);
							assert(pModifierAgg);
							TupModifier tupModifier(pModifierAgg);
							switch(tupModifier.GetModifierType())
							{
								case TupModifier::APPLY_VC_MODIFIER:
									ApplyApplyVCModifierForVertexAlpha(tupBaseMesh,tupModifier,pModifierContextAgg,tupTriMeshMap);
									break;
								case TupModifier::VERTEX_PAINT_MODIFIER:
									ApplyVertexPaintModifierForVertexAlpha(tupBaseMesh,tupModifier,pModifierContextAgg,tupTriMeshMap);
									break;
							}
						}
					}
				}
			}
		}
		TupperwareList *pParentObjectReferenceList = pObjectAgg->FindListByKey("Annotate_ParentObjectReferenceList");
		if (pParentObjectReferenceList)
		{
			// we there is already a list
			// check to see if the objectIndex is already there
			// if not add it to the list
			int numEntries = pParentObjectReferenceList->GetLength();
			int *pParentObjectReferenceListData = pParentObjectReferenceList->GetAsInt();
			for (int i=0;i<numEntries;i++)
			{
				PropagateVertexAlpha(pParentObjectReferenceListData[i]);
			}
		}
	}
}

void ProcessTup::ApplyApplyVCModifierForVertexAlpha(TupTriMesh &tupBaseMesh,TupModifier &tupModifier,TupperwareAggregate *pModifierContextAgg,TupTriMeshMap &tupTriMeshMap)
{
	TupperwareAggregate *pModifierAgg = tupModifier.GetBaseAggregate();
	TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
	int flags;
	tupApplyVCModifier.GetFlags(flags);
	if ((flags&TupApplyVCModifier::FLAGS_ENABLED) &&
		(flags&TupApplyVCModifier::FLAGS_HAS_ALPHA))
	{
		// get the number of faces from the mesh
		int numSourceFaces;
		int *pSourceIndices;
		tupTriMeshMap.GetMapIndices(&pSourceIndices,numSourceFaces);

		// get the modifier information
		int numModifierVerts;
		int numModifierIndices;
		float *pModifierVerts;
		int *pModifierIndices;
		tupApplyVCModifier.GetAlphaVerts((float **)&pModifierVerts,numModifierVerts);
		tupApplyVCModifier.GetAlphaIndices(&pModifierIndices,numModifierIndices);

		// set up the destination information
		int *pDestIndices = new int[numSourceFaces*3];
		Keeper<Vector3> destVerts;

		if (flags & TupApplyVCModifier::FLAGS_MIXED_COLORS)
		{
			// mixed colors are in sync with the position pool
			int numPositionVerts;
			int numPositionFaces;
			Vector3 *pPositionVerts;
			int *pPositionIndices;
			tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
			tupBaseMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);
			assert(numPositionFaces==numSourceFaces);

			int numPoints = numSourceFaces*3;
			for (int pointIndex=0;pointIndex<numPoints;pointIndex++)
			{
				int modifierIndex;
				int positionIndex = pPositionIndices[pointIndex];
				if (positionIndex>=numModifierIndices)
				{
					modifierIndex=0; // if to big then use the first one
				}
				else
				{
					modifierIndex=pModifierIndices[positionIndex];
				}
				Vector3 newVert;
				newVert.Set(pModifierVerts[modifierIndex],0.0f,0.0f);
				pDestIndices[pointIndex] = destVerts.Add(newVert);
			}
		}
		else
		{
			// each point of each face has an entry
			int numPoints = numSourceFaces*3;
			for (int pointIndex=0;pointIndex<numPoints;pointIndex++)
			{
				int modifierIndex = pointIndex;
				if (modifierIndex>=numModifierIndices)
				{
					modifierIndex=0;
				}
				Vector3 newVert;
				newVert.Set(pModifierVerts[pModifierIndices[modifierIndex]],0.0f,0.0f);
				pDestIndices[pointIndex] = destVerts.Add(newVert);
			}
		}
		tupTriMeshMap.AddMapVerts((float *)destVerts.GetData(),destVerts.GetSize());
		tupTriMeshMap.AddMapIndices(pDestIndices,numSourceFaces);
		delete pDestIndices;
	}
}

void ProcessTup::ApplyVertexPaintModifierForVertexAlpha(TupTriMesh &tupBaseMesh,TupModifier &tupModifier,TupperwareAggregate *pModifierContextAgg,TupTriMeshMap &tupTriMeshMap)
{
	TupperwareAggregate *pModifierAgg = tupModifier.GetBaseAggregate();
	TupVertexPaintModifier tupVertexPaintModifier(pModifierAgg);
	int flags;
	tupVertexPaintModifier.GetFlags(flags);
	if (flags & TupApplyVCModifier::FLAGS_ENABLED)
	{
		// get information from the base mesh
		int numPositionVerts;
		int numPositionFaces;
		Vector3 *pPositionVerts;
		int *pPositionIndices;
		tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
		tupBaseMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);

		// get the source information
		int numSourceFaces;
		int numSourceVerts;
		Vector3 *pSourceVerts;
		int *pSourceIndices;
		tupTriMeshMap.GetMapVerts((float **)&pSourceVerts,numSourceVerts);
		tupTriMeshMap.GetMapIndices(&pSourceIndices,numSourceFaces);
		assert(numPositionFaces==numSourceFaces);

		TupVertexPaintModifierContext tupVertexPaintModifierContext(pModifierContextAgg);
		Vector3 *pModifierColorData;
		float *pModifierMixFactorData;
		int *pModifierFaceIndexData;
		int *pModifierPointIndexData;
		int numModifierColors;
		tupVertexPaintModifierContext.GetAlphaChannelColorData((float **)&pModifierColorData,numModifierColors);
		tupVertexPaintModifierContext.GetAlphaChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
		tupVertexPaintModifierContext.GetAlphaChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
		tupVertexPaintModifierContext.GetAlphaChannelPointIndexData(&pModifierPointIndexData,numModifierColors);

		// set up the destination information
		int *pDestIndices = new int[numSourceFaces*3];
		Keeper<Vector3> destVerts;
		// I need an array to keep track of which position points have been given a color by the modifier
		bool *pDestFacePointAddedTable = new bool[numPositionVerts];
		memset(pDestFacePointAddedTable,0,sizeof(bool)*(numPositionVerts));
		// I need an array to keep track of the altered colors (in sync with position pool)
		Vector3 *pDestModifierColors = new Vector3[numPositionVerts];

		// go through the list of modifier colors and add to the table in sync with the position pool
		for (int colorIndex = 0;colorIndex<numModifierColors;colorIndex++)
		{
			Vector3 &color = pModifierColorData[colorIndex];
			float mixFactor = pModifierMixFactorData[colorIndex];
			int faceIndex = pModifierFaceIndexData[colorIndex];
			int pointIndex = pModifierPointIndexData[colorIndex];
			assert(pointIndex>=0 && pointIndex<=2);

			int facePointIndex = (faceIndex<<1)+faceIndex+pointIndex;
			if (faceIndex<numSourceFaces)
			{
				int posVertexIndex = pPositionIndices[facePointIndex];
				Vector3 &sourceColor = pSourceVerts[pSourceIndices[facePointIndex]];
				Vector3 destColor;
				destColor = (mixFactor * color) + ((1.0f - mixFactor) * sourceColor);
				pDestModifierColors[posVertexIndex] = destColor;
				pDestFacePointAddedTable[posVertexIndex] = true;
			}
		}
		// go through all the faces and use the dest color or the source
		int numPoints = numSourceFaces*3;
		for (int pointIndex=0;pointIndex<numPoints;pointIndex++)
		{
			int posVertexIndex = pPositionIndices[pointIndex];
			if (pDestFacePointAddedTable[posVertexIndex])
			{
				// get the color from the modifier array
				pDestIndices[pointIndex] = destVerts.Add(pDestModifierColors[posVertexIndex]);
			}
			else
			{
				// use the source color
				pDestIndices[pointIndex] = destVerts.Add(pSourceVerts[pSourceIndices[pointIndex]]);
			}
		}
		tupTriMeshMap.AddMapVerts((float *)destVerts.GetData(),destVerts.GetSize());
		tupTriMeshMap.AddMapIndices(pDestIndices,numSourceFaces);
		delete pDestIndices;
		delete pDestFacePointAddedTable;
		delete pDestModifierColors;
	}
}

// multiply the alpha set by the multitex filter by the vertex alphas that are on the mesh and the derived objects
void ProcessTup::PropagateMultiTexAlpha(void)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if entry is empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			TupperwareList *pVertexAlphaVertsList = pObjectAgg->FindListByKey("MultiTex_VertexAlphaVerts");
			TupperwareList *pVertexAlphaIndicesList = pObjectAgg->FindListByKey("MultiTex_VertexAlphaIndices");

			if (pVertexAlphaVertsList && pVertexAlphaIndicesList)
			{
				PropagateMultiTexAlpha(objectIndex,pVertexAlphaVertsList,pVertexAlphaIndicesList);
			}
		}
	}
}

void ProcessTup::PropagateMultiTexAlpha(int objectRef,TupperwareList *pVertexAlphaVertsList,TupperwareList *pVertexAlphaIndicesList)
{
	// get data to multiply by the alphas on the objects
	float *pMultiTexAlphaVerts = pVertexAlphaVertsList->GetAsFloat();
	int numMultiTexAlphaVerts = pVertexAlphaVertsList->GetLength();
	int *pMultiTexAlphaIndices = pVertexAlphaIndicesList->GetAsInt();
	int numMultiTexAlphaIndices = pVertexAlphaIndicesList->GetLength();

	// set up new arrays
	Keeper<Vector3> newVerts;
	Vector3 opaqueEntry;
	opaqueEntry.Set(1.0f,0.0f,0.0f);
	newVerts.Add(opaqueEntry);
	TupArray<int> newIndices;
	newIndices.SetSize(numMultiTexAlphaIndices);
	
	TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::TRI_MESH)
	{
		bool bFoundAlphaMap = false;
		TupTriMesh tupTriMesh(pObjectAgg);
		int numMeshMaps = tupTriMesh.GetNumMeshMaps();
	
		for (int meshMapIndex = 0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
			TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
			if (tupTriMeshMap.GetMapChannel()==TupTriMeshMap::MAP_CHANNEL_ALPHA)
			{
				bFoundAlphaMap = true;
				Vector3 *pMapVerts;
				int numMapVerts;
				int *pMapIndices;
				int numMapFaces;
				tupTriMeshMap.GetMapVerts((float **)&pMapVerts,numMapVerts);
				tupTriMeshMap.GetMapIndices(&pMapIndices,numMapFaces);
				assert(numMapFaces*3==numMultiTexAlphaIndices);
				for (int facePointIndex=0;facePointIndex<numMultiTexAlphaIndices;facePointIndex++)
				{
					Vector3 alphaEntry;
					alphaEntry.Set(pMapVerts[pMapIndices[facePointIndex]].x() * pMultiTexAlphaVerts[pMultiTexAlphaIndices[facePointIndex]],0.0f,0.0f);
					newIndices[facePointIndex] = newVerts.Add(alphaEntry);
				}
			}
		}
		if (!bFoundAlphaMap)
		{
			for (int facePointIndex=0;facePointIndex<numMultiTexAlphaIndices;facePointIndex++)
			{
				Vector3 alphaEntry;
				alphaEntry.Set(pMultiTexAlphaVerts[pMultiTexAlphaIndices[facePointIndex]],0.0f,0.0f);
				newIndices[facePointIndex] = newVerts.Add(alphaEntry);
			}
		}
		TupperwareAggregate *pMeshMapAgg = tupTriMesh.AddMeshMap(TupTriMeshMap::MAP_CHANNEL_ALPHA);
		TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
		tupTriMeshMap.AddMapVerts((float *)newVerts.GetData(),newVerts.GetSize());
		tupTriMeshMap.AddMapIndices(newIndices.GetData(),newIndices.GetSize()/3);
		tupTriMeshMap.AddMapFlags(0); // for now
	}
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupperwareAggregate *pVertexAlphaMeshMapAgg = pObjectAgg->FindAggregateByKeyAndName("Annotate_VertexAlphaMeshMap","-2");
		if (pVertexAlphaMeshMapAgg)
		{
			TupTriMeshMap tupTriMeshMap(pVertexAlphaMeshMapAgg);
			Vector3 *pMapVerts;
			int numMapVerts;
			int *pMapIndices;
			int numMapFaces;
			tupTriMeshMap.GetMapVerts((float **) &pMapVerts,numMapVerts);
			tupTriMeshMap.GetMapIndices(&pMapIndices,numMapFaces);
			assert(numMapFaces*3==numMultiTexAlphaIndices);
			for (int facePointIndex=0;facePointIndex<numMultiTexAlphaIndices;facePointIndex++)
			{
				Vector3 alphaEntry;
				alphaEntry.Set(pMapVerts[pMapIndices[facePointIndex]].x() * pMultiTexAlphaVerts[pMultiTexAlphaIndices[facePointIndex]],0.0f,0.0f);
				newIndices[facePointIndex] = newVerts.Add(alphaEntry);
			}
			tupTriMeshMap.AddMapVerts((float *)newVerts.GetData(),newVerts.GetSize());
			tupTriMeshMap.AddMapIndices(newIndices.GetData(),newIndices.GetSize()/3);
			tupTriMeshMap.AddMapFlags(0); // for now
		}
	}
	TupperwareList *pParentObjectReferenceList = pObjectAgg->FindListByKey("Annotate_ParentObjectReferenceList");
	if (pParentObjectReferenceList)
	{
		// we there is already a list
		// check to see if the objectIndex is already there
		// if not add it to the list
		int numEntries = pParentObjectReferenceList->GetLength();
		int *pParentObjectReferenceListData = pParentObjectReferenceList->GetAsInt();
		for (int i=0;i<numEntries;i++)
		{
			PropagateMultiTexAlpha(pParentObjectReferenceListData[i],pVertexAlphaVertsList,pVertexAlphaIndicesList);
		}
	}
}

void ProcessTup::MarkEnvInstances(void)
{
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		
		// if slot empty then continue
		if (!pNodeAgg)
			continue;
		
		TupNode tupNode(pNodeAgg);

		int envInstance = FALSE;
		
		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef!=-1)
		{
			TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
			if (pBaseMeshAgg)
			{
				char *pNodeAttributesString;
				tupNode.GetUserProperties(&pNodeAttributesString);
				TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_OBJ_BLOCK,END_OBJ_BLOCK);
				TupKeyValueContext tupKeyValueContext;

				const int INSTANCE_TYPE = 0;
				const int ENV_TYPE = 1;
				tupKeyValueContext.AddKey("instance_type");
				tupKeyValueContext.AddKey("env_type");
				
				m_pTupNodeConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
				// if set to something other than normal then skip
				if (tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp(tupKeyValueContext.GetValueAsString(INSTANCE_TYPE),"normal")==0)
				{
					// if this is not "None"
					if (strcmp(tupKeyValueContext.GetValueAsString(ENV_TYPE),"None")!=0)
						envInstance = TRUE;
				}
			}
		}
		pNodeAgg->AddScalarInt("Annotate_EnvInstance",NULL,envInstance);
	}
}

// or the value from the node to the mesh
// add scalar if not already there on the mesh
static void MergeScalarToMesh(TupperwareAggregate *pNodeAgg,TupperwareAggregate *pMeshAgg,const char *pKey)
{
	int nodeValue = 0;
	TupperwareScalar *pScalar = pNodeAgg->FindScalarByKey( pKey );
	if( pScalar )
		nodeValue = pScalar->GetAsInt();
	TupperwareScalar *pMeshScalar = pMeshAgg->FindScalarByKey(pKey);
	if (pMeshScalar)
	{
		pMeshScalar->SetAsInt((pMeshScalar->GetAsInt()==1 || (nodeValue == 1)) ? 1 : 0);
	}
	else
	{
		pMeshAgg->AddScalarInt(pKey,NULL,nodeValue);
	}
}

// build a list of materials that are used for each mesh
// if more than one material is used for a mesh we need to make more than one model in the dbl file
void ProcessTup::AddObjectModelAttributes(void)
{
	if (m_bForceDynamicTextures)
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Forcing Dynamic textures");

	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if slot empty then continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef!=-1)
		{
			TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
			if (pBaseMeshAgg)
			{
				char *pNodeAttributesString;
				tupNode.GetUserProperties(&pNodeAttributesString);
				TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_OBJ_BLOCK,END_OBJ_BLOCK);
				TupKeyValueContext tupKeyValueContext;

				const int KEY_MODEL_NAME = 0;
				const int KEY_DYNAMIC_TEXTURE_SET = 1;
				const int KEY_MODEL_ALPHA_SORT = 2;
				const int KEY_FORCE_TWO_SIDED_FACES = 3;
				const int KEY_TEXTURE_TRANSFORM_ROT = 4;
				const int KEY_TEXTURE_TRANSFORM_ROTVEL = 5;
				const int KEY_NO_RECEIVE_SHADOWS = 6;
				
				tupKeyValueContext.AddKey("model_name");
				tupKeyValueContext.AddKey("dynamic_texture_set");
				tupKeyValueContext.AddKey("model_alpha_sort");
				tupKeyValueContext.AddKey("force_two_sided_faces");
				tupKeyValueContext.AddKey("inst_info\\texture_transform\\child\\texture_transform_rot");
				tupKeyValueContext.AddKey("inst_info\\texture_transform\\child\\texture_transform_rotvel");
				tupKeyValueContext.AddKey("no_receive_shadows");
				
				// get just the model_name value for the instance
				m_pTupNodeConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);

				// if the name is not None
				if (strcmp(tupKeyValueContext.GetValueAsString(KEY_MODEL_NAME),"None"))
				{
					TupperwareScalar *pModelNameScalar = pBaseMeshAgg->FindScalarByKey("Annotate_ModelName");
					// only add if not already there
					if (!pModelNameScalar)
					{
						pBaseMeshAgg->AddScalarString("Annotate_ModelName",NULL,tupKeyValueContext.GetValueAsString(0));
					}
				}
				// set the dynamic texture status
				int dynamicTextureSet = 0;
				if (m_bForceDynamicTextures || 
					tupKeyValueContext.GetValueAsBool(KEY_DYNAMIC_TEXTURE_SET))
				{
					dynamicTextureSet = 1;
				}

				TupperwareScalar *pDynamicTextureSetScalar = pBaseMeshAgg->FindScalarByKey("Annotate_DynamicTextureSet");
				// only add if not already there
				if (!pDynamicTextureSetScalar)
				{
					pBaseMeshAgg->AddScalarInt("Annotate_DynamicTextureSet",NULL,dynamicTextureSet);
				}

				MergeScalarToMesh(pNodeAgg,pBaseMeshAgg,"RealTimeLighting_Ambient");
				MergeScalarToMesh(pNodeAgg,pBaseMeshAgg,"RealTimeLighting_Point");
				MergeScalarToMesh(pNodeAgg,pBaseMeshAgg,"RealTimeLighting_Directional");
				MergeScalarToMesh(pNodeAgg,pBaseMeshAgg,"RealTimeLighting_Vertex");

				// set the alpha sort type
				TupString alphaSortString = "UseTextures";
				if (tupKeyValueContext.ValueValidAt(KEY_MODEL_ALPHA_SORT))
				{
					alphaSortString = tupKeyValueContext.GetValueAsString(KEY_MODEL_ALPHA_SORT);
				}
				TupperwareScalar *pAlphaSortStringScalar = pBaseMeshAgg->FindScalarByKey("Annotate_AlphaSort");
				if (!pAlphaSortStringScalar)
				{
					pBaseMeshAgg->AddScalarString("Annotate_AlphaSort",NULL,alphaSortString);
				}
				// set force two sided faces
				if (!m_bClearTwoSidedFaceFlags && tupKeyValueContext.ValueValidAt(KEY_FORCE_TWO_SIDED_FACES) && tupKeyValueContext.GetValueAsBool(KEY_FORCE_TWO_SIDED_FACES))
				{
					TupperwareScalar *pForceTwoSidedFacesScalar = pBaseMeshAgg->FindScalarByKey("Annotate_Force Two Sided Faces");
					if (!pForceTwoSidedFacesScalar)
					{
						pBaseMeshAgg->AddScalarInt("Annotate_Force Two Sided Faces",NULL,1);
					}
				}

				// if this mesh has texture transform rotation
				if ((tupKeyValueContext.ValueValidAt(KEY_TEXTURE_TRANSFORM_ROT) && tupKeyValueContext.GetValueAsFloat(KEY_TEXTURE_TRANSFORM_ROT)!=0.0f) ||
					 (tupKeyValueContext.ValueValidAt(KEY_TEXTURE_TRANSFORM_ROTVEL) && tupKeyValueContext.GetValueAsFloat(KEY_TEXTURE_TRANSFORM_ROTVEL)!=0.0f))
				{
					TupperwareScalar *pUseTextureTransformRotationScalar = pBaseMeshAgg->FindScalarByKey("Annotate_Use Texture Transform Rotation");
					if (!pUseTextureTransformRotationScalar)
					{
						pBaseMeshAgg->AddScalarInt("Annotate_Use Texture Transform Rotation",NULL,1);
					}
				}

				// set no receive shadow flag if any instance indicate it
				if (tupKeyValueContext.ValueValidAt(KEY_NO_RECEIVE_SHADOWS))
				{
					// if we don't want shadows tell the mesh about it if no one else has already
					if (tupKeyValueContext.GetValueAsBool(KEY_NO_RECEIVE_SHADOWS))
					{
						TupperwareScalar *pForceNoReceiveShadowsScalar = pBaseMeshAgg->FindScalarByKey("Annotate_ForceNoReceiveShadows");
						if (!pForceNoReceiveShadowsScalar)
						{
							pBaseMeshAgg->AddScalarInt("Annotate_ForceNoReceiveShadows",NULL,1);
						}
					}
					// if we do want shadows tell the mesh and override any previous sets
					else
					{
						TupperwareScalar *pForceNoReceiveShadowsScalar = pBaseMeshAgg->FindScalarByKey("Annotate_ForceNoReceiveShadows");
						if (!pForceNoReceiveShadowsScalar)
						{
							pBaseMeshAgg->AddScalarInt("Annotate_ForceNoReceiveShadows",NULL,0);
						}
						else
						{
							// set to 0 if currently set to 1
							if (pForceNoReceiveShadowsScalar->GetAsInt()==1)
							{
								pForceNoReceiveShadowsScalar->SetAsInt(0);
							}
						}
					}
				}
			}
		}
	}
}

// will point all nodes that are meant to be replaced with a dummy model
void ProcessTup::BuildReplacementModels(void)
{
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Building Dummy Replacement models");

	bool bDummiesCreated = false;
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if slot empty then continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);
		char *pNodeName;
		tupNode.GetName(&pNodeName);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef!=-1)
		{
			TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
			if (pBaseMeshAgg)
			{
				int baseMeshRef = AnnotateHelper::FindBaseMeshRef(*m_pTupObjectPool,objectRef);

				TupperwareScalar *pDummyModelScalar = pBaseMeshAgg->FindScalarByKey("Annotate_DummyMesh");
				// already pointing to a dummy?
				if (pDummyModelScalar)
					continue;

				char *pNodeAttributesString;
				tupNode.GetUserProperties(&pNodeAttributesString);
				TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_OBJ_BLOCK,END_OBJ_BLOCK);
				TupKeyValueContext tupKeyValueContext;

				tupKeyValueContext.AddKey("replace_model");

				// get just the model_name value for the instance
				m_pTupNodeConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);

				// set the replace model
				if (tupKeyValueContext.GetValueAsBool(0))
				{
					TupperwareScalar *pMayaObjectNameScalar = pBaseMeshAgg->FindScalarByKey("MayaObjectName");

					char *pShapeName = "mesh";
					if (pMayaObjectNameScalar)
					{
						pShapeName = pMayaObjectNameScalar->GetAsString();
					}
					bDummiesCreated = true;
					int newMeshRef = m_pTupObjectPool->AddObjectAggregate();
					TupperwareAggregate *pNewMeshAgg = m_pTupObjectPool->GetObjectAggregate(newMeshRef);
					MakeDummyMesh::AddToAgg(pNewMeshAgg); // create a dummy mesh
					pNewMeshAgg->AddScalarInt("Annotate_DummyMesh",NULL,1); // mark as dummy
					pNewMeshAgg->AddScalarInt("Annotate_MaterialRef",NULL,-1);
					TupTriMesh tupTriMesh(pNewMeshAgg);
					Bounding::AddObjectBounding(tupTriMesh);
					int numRemaps = RemapNodesToDummy(baseMeshRef,newMeshRef);
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"%d Nodes remapped to dummy from %s on request from %s",
						numRemaps,pShapeName,pNodeName);

				}
			}
		}
	}
	// if we created any dummies then kill off the unused ones
	if (bDummiesCreated)
	{
		int numObjects = m_pTupObjectPool->GetNumObjects();
		TupArray<bool> objectsUsedArray;
		objectsUsedArray.SetSize(numObjects);
		int objectIndex;

		for (objectIndex=0;objectIndex<numObjects;objectIndex++)
		{
			objectsUsedArray[objectIndex] = false;
		}
		for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
		{
			TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

			// if slot empty then continue
			if (!pNodeAgg)
				continue;

			TupNode tupNode(pNodeAgg);

			int objectRef;	
			tupNode.GetObjectReference(objectRef);
			if (objectRef!=-1)
			{
				MarkObjectUsed(objectRef,objectsUsedArray);
			}
		}
		for (objectIndex=0;objectIndex<numObjects;objectIndex++)
		{
			if (!objectsUsedArray[objectIndex])
			{
				m_pTupObjectPool->DeleteObjectAggregate(objectIndex);
			}
		}
	}
}

void ProcessTup::MarkObjectUsed(int objectRef,TupArray<bool> &objectsUsedArray)
{
	objectsUsedArray[objectRef] = true;
	TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectRef);
	assert(pObjectAgg);
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupDerived tupDerived(pObjectAgg);
		int childObjectRef;
		tupDerived.GetObjectReference(childObjectRef);
		MarkObjectUsed(childObjectRef,objectsUsedArray);
	}
}

int ProcessTup::RemapNodesToDummy(int fromObjectRef,int toObjectRef)
{
	int numRemapped = 0;
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if slot empty then continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef!=-1)
		{
			int baseRef = AnnotateHelper::FindBaseObjectRef(*m_pTupObjectPool,objectRef);
			if (baseRef == fromObjectRef)
			{
				numRemapped++;
				tupNode.AddObjectReference(toObjectRef);
				int noMaterialRef = -1;
				tupNode.AddMaterialReference(noMaterialRef);
			}
		}
	}
	return numRemapped;
}

// this method will make sure that each mesh only has 1 material being used
// this involves branching the derived object chains and changing the references of the nodes
// this method with also replace Annotate - MaterialList atoms with Annotate - MaterialRef
// since each mesh will reference only one material
void ProcessTup::OneMaterialPerMesh(void)
{
	int objectIndex;
	for (objectIndex=0;objectIndex<m_pTupObjectPool->GetNumObjects();objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			TupperwareList *pMaterialList = pObjectAgg->FindListByKey("Annotate_MaterialList");
			assert(pMaterialList);
			int numEntries = pMaterialList->GetLength();
			while (numEntries>1)
			{
				int *pMaterialListData = pMaterialList->GetAsInt();
				int materialToRemove = pMaterialListData[numEntries-1];
				// copy current list to another and remove the last entry
				int *pReducedMaterialListData = new int[numEntries-1];
				for (int entryIndex=0;entryIndex<numEntries-1;entryIndex++)
				{
					pReducedMaterialListData[entryIndex] = pMaterialListData[entryIndex];
				}
				// pass to tree (tree will manage deleting the old one and managing the new one)
				pMaterialList->SetAsInt(pReducedMaterialListData,numEntries-1);
				delete pReducedMaterialListData;
				int newObjectIndex = m_pTupObjectPool->AddObjectAggregate();
				TupperwareAggregate *pNewObjectAgg = m_pTupObjectPool->GetObjectAggregate(newObjectIndex);
				assert(pNewObjectAgg);
				// copy stuff from my object into the new one
				for (TupperwareIterator iter = pObjectAgg->FindAll(); !iter.Finished(); iter.Next()) 
				{
					pNewObjectAgg->Copy(iter.GetAsAtom());
				}
				TupperwareList *pNewMaterialList = pNewObjectAgg->FindListByKey("Annotate_MaterialList");
				assert(pNewMaterialList);
				// copy has only one material
				int *pNewMaterialListData = new int[1];
				pNewMaterialListData[0] = materialToRemove;
				// pass to tree (tree will manage deleting the old one and managing the new one)
				pNewMaterialList->SetAsInt(pNewMaterialListData,1);
				delete pNewMaterialListData;
				BranchNodeObjectChains(objectIndex,newObjectIndex,materialToRemove);
				numEntries--;
			}
		}
	}
	// go through and change material list to material scalar
	for (objectIndex=0;objectIndex<m_pTupObjectPool->GetNumObjects();objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			TupperwareList *pMaterialList = pObjectAgg->FindListByKey("Annotate_MaterialList");
			assert(pMaterialList);
			int numEntries = pMaterialList->GetLength();
			assert(numEntries==1);
			int materialRef = (pMaterialList->GetAsInt())[0];
			pObjectAgg->DeleteAtom(pMaterialList); // get rid of this one
			pObjectAgg->AddScalarInt("Annotate_MaterialRef",NULL,materialRef);
		}
	}
}

// this will separate a chain of nodes to objects into a separate set for making sure that each mesh only has one material being used
void ProcessTup::BranchNodeObjectChains(int fromObjectRef,int toObjectRef,int materialRef)
{
	for (int objectIndex=0;objectIndex<m_pTupObjectPool->GetNumObjects();objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		// scan for any derived objects that point to the fromObjectRef
		if (tupObject.GetObjectType()==TupObject::DERIVED)
		{
			TupDerived tupDerived(pObjectAgg);
			int childReference;
			tupDerived.GetObjectReference(childReference);
			assert(childReference!=-1);
			if (childReference == fromObjectRef)
			{
				bool copyObject;
				if (IsValidBranchObject(objectIndex,materialRef,copyObject))
				{
					if (copyObject)
					{
						int newObjectIndex = m_pTupObjectPool->AddObjectAggregate();
						TupperwareAggregate *pNewObjectAgg = m_pTupObjectPool->GetObjectAggregate(newObjectIndex);
						// copy stuff from my object into the new one
						for (TupperwareIterator iter = pObjectAgg->FindAll(); !iter.Finished(); iter.Next()) 
						{
							pNewObjectAgg->Copy(iter.GetAsAtom());
						}
						TupDerived newTupDerived(pNewObjectAgg);
						// point to new object
						newTupDerived.AddObjectReference(toObjectRef);
						// continue up the chain
						BranchNodeObjectChains(objectIndex,newObjectIndex,materialRef);
					}
					else
					{
						// if it doesn't need to be copied then just change the reference to toObjectRef
						tupDerived.AddObjectReference(toObjectRef);
					}
				}
			}
		}
	}
	// scan through the nodes and redirect any that are pointing to fromObject to toObject (if they also use the materialRef)
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if slot empty then continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int nodeMaterialRef;
		tupNode.GetMaterialReference(nodeMaterialRef);

		int nodeObjectRef;
		tupNode.GetObjectReference(nodeObjectRef);
		if (nodeObjectRef == fromObjectRef && nodeMaterialRef==materialRef)
		{
			char *pName;
			tupNode.GetName(&pName);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Remapping node %s from object %d to new object %d due to multiple material references",pName,fromObjectRef,toObjectRef);
			tupNode.AddObjectReference(toObjectRef);
		}
	}
}

bool ProcessTup::IsValidBranchObject(int objectRef,int materialRef,bool &copyObject)
{
	int numObjectNodes = 0;
	int numObjectMaterialNodes = 0;
	// scan through the nodes and see if any or all that are pointing to object also use the materialRef
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if slot empty then continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int nodeMaterialRef;
		tupNode.GetMaterialReference(nodeMaterialRef);

		int nodeObjectRef;
		tupNode.GetObjectReference(nodeObjectRef);
		if (nodeObjectRef == objectRef)
		{
			numObjectNodes++;
			if (nodeMaterialRef==materialRef)
			{
				numObjectMaterialNodes++;
			}
		}
	}
	if (numObjectNodes) // at least one node points to me
	{
		if (numObjectMaterialNodes) // at least one node uses the material
		{
			if (numObjectNodes!=numObjectMaterialNodes)
			{
				copyObject = true; // we need to copy the object (not all are using the material)
				return true; // there is at least one node using me
			}

		}
	}
	// continue checking the derived objects
	// scan through any objects that are pointing to me and call again
	int parentObjects = 0;
	int validParentObjects=0;
	int validParentObjectsCopy=0;
	for (int objectIndex=0;objectIndex<m_pTupObjectPool->GetNumObjects();objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		// scan for any derived objects that point to the fromObjectRef
		if (tupObject.GetObjectType()==TupObject::DERIVED)
		{
			TupDerived tupDerived(pObjectAgg);
			int childReference;
			tupDerived.GetObjectReference(childReference);
			assert(childReference!=-1);
			if (childReference == objectRef)
			{
				parentObjects++;
				if (IsValidBranchObject(objectIndex,materialRef,copyObject))
				{
					validParentObjects++;
					if (copyObject)
						validParentObjectsCopy++;
				}
			}
		}
	}
	// if at least one derived object is pointing to me
	if (parentObjects)
	{
		// if at least one derived object needs to be branched
		if (validParentObjects)
		{
			// if at least one of the parents needs to be copied
			if (validParentObjectsCopy)
			{
				copyObject = true; // we need to copy the object (not all are using the material)
				return true; // there is at least one node using me
			}
			// if there are a different number of valid parents than total parents
			else if (validParentObjects!=parentObjects)
			{
				copyObject = true; // we need to copy the object (not all the parents are using the material)
				return true; // there is at least one node using me
			}
			else
			{
				// if none of the parents require me to be copied
				copyObject = false; // we don't need to copy (all are using the material)
				return true;
			}
		}
		// none of the derived objects need to be branched
		else
		{
			if (numObjectMaterialNodes) // at least one node that points to me is using the material
			{
				copyObject = true; // we need to copy the object (not all are using the material)
				return true; // there is at least one node using me
			}
			// no nodes are using the material
			else
			{
				return false; // we don't need to branch me since no node or derived object uses the material
			}
		}
	}
	// I have no parent derived objects
	else
	{
		// if there is any nodes that use the material here they should all be using the material because
		// of the previous test prior to testing the derived objects
		if (numObjectMaterialNodes) // if all the nodes are using the material
		{
			copyObject = false; // don't need to copy
			return true; // there is at least one node using me
		}
		// no nodes are using the material
		else
		{
			return false; // we don't need to branch me since no nodes use the material
		}
	}
}

// build a list of vertex color sets on each base mesh and set the correct index within each node to point
// in the list on the base mesh
void ProcessTup::AddVertexColorAndAlphaRefsToMeshAndNodes(void)
{
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if slot empty then continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int materialRef;
		tupNode.GetMaterialReference(materialRef);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);

		if (objectRef!=-1)
		{
			int colorRef = AnnotateHelper::FindChildVertexColorObject(*m_pTupObjectPool,objectRef);
			int alphaRef = AnnotateHelper::FindChildVertexAlphaObject(*m_pTupObjectPool,objectRef);
			TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
			if (pBaseMeshAgg)
			{
				int meshColorAlphaOffset;

				TupperwareList *pVertexColorRefList = pBaseMeshAgg->FindListByKey("Annotate_VertexColorRefList");
				TupperwareList *pVertexAlphaRefList = pBaseMeshAgg->FindListByKey("Annotate_VertexAlphaRefList");
				if (pVertexColorRefList && pVertexAlphaRefList)
				{
					// there is already the lists
					// check to see if the color ref and alpha ref are already there
					// if not add it to the list
					int numColorEntries = pVertexColorRefList->GetLength();
					int numAlphaEntries = pVertexAlphaRefList->GetLength();
					assert(numColorEntries==numAlphaEntries);
					int *pVertexColorRefData = pVertexColorRefList->GetAsInt();
					int *pVertexAlphaRefData = pVertexAlphaRefList->GetAsInt();
					int meshColorAlphaIndex;
					for (meshColorAlphaIndex=0;meshColorAlphaIndex<numColorEntries;meshColorAlphaIndex++)
					{
						if (pVertexColorRefData[meshColorAlphaIndex]==colorRef &&
							pVertexAlphaRefData[meshColorAlphaIndex]==alphaRef)
						{
							break;
						}
					}
					meshColorAlphaOffset = meshColorAlphaIndex; // this is the index the node will use
					// if we didn't find it
					if (meshColorAlphaIndex==numColorEntries)
					{
						// copy current list to another and add the new one
						int *pNewVertexColorRefData = new int[numColorEntries+1];
						int *pNewVertexAlphaRefData = new int[numColorEntries+1];
						for (meshColorAlphaOffset=0;meshColorAlphaOffset<numColorEntries;meshColorAlphaOffset++)
						{
							pNewVertexColorRefData[meshColorAlphaOffset] = pVertexColorRefData[meshColorAlphaOffset];
							pNewVertexAlphaRefData[meshColorAlphaOffset] = pVertexAlphaRefData[meshColorAlphaOffset];
						}
						pNewVertexColorRefData[numColorEntries] = colorRef;
						pNewVertexAlphaRefData[numColorEntries] = alphaRef;
						// pass to tree (tree will manage deleting the old one and managing the new one)
						pVertexColorRefList->SetAsInt(pNewVertexColorRefData,numColorEntries+1);
						pVertexAlphaRefList->SetAsInt(pNewVertexAlphaRefData,numColorEntries+1);
						delete pNewVertexColorRefData;
						delete pNewVertexAlphaRefData;
					}
				}
				else
				{
					// both should be NULL
					assert(!pVertexColorRefList && !pVertexAlphaRefList);
					meshColorAlphaOffset = 0; // this is the index the node will use
					// if no list then start with out materialref
					int *pNewVertexColorRefData = new int[1];
					int *pNewVertexAlphaRefData = new int[1];
					pNewVertexColorRefData[0] = colorRef;
					pNewVertexAlphaRefData[0] = alphaRef;
					pBaseMeshAgg->AddListInt("Annotate_VertexColorRefList",NULL,pNewVertexColorRefData,1);
					pBaseMeshAgg->AddListInt("Annotate_VertexAlphaRefList",NULL,pNewVertexAlphaRefData,1);
					delete pNewVertexColorRefData;
					delete pNewVertexAlphaRefData;
				}
				// add the index within the VertexColorRefList to the node
				pNodeAgg->AddScalarInt("Annotate_MeshVertexColorAlphaOffset",NULL,meshColorAlphaOffset);
			}
		}
	}
}


void ProcessTup::SetUpTextureSets(void)
{
	int numTextureSets;

	int totalObjects = m_pTupObjectPool->GetNumObjects();
	int totalImages = m_pTupImagePool->GetNumImages();
	int objectIndex;
	int imageIndex;
	// check if the option for dynamic texture sets is set to true
	const char *pDynamicTextureSetsOption = m_options.GetValue("DynamicTextureSets");
	if (pDynamicTextureSetsOption && !stricmp(pDynamicTextureSetsOption,"true"))
	{
		// new subsets start at 1
		int nextAvailableSubSet = 1;
		// allocate space to hold the textures set before I place them in the tree
		int *pMeshTexSets = new int[totalObjects];
		int *pImageTexSets = new int[totalImages];
		// TJC - first, run through all textures and assign them to subset -1 
		// Meaning not assigned 
		for (imageIndex = 0;imageIndex<totalImages;imageIndex++)
		{
			pImageTexSets[imageIndex] = -1;
		}
		// set all meshes to point to subset 0 
		for (objectIndex=0;objectIndex<totalObjects;objectIndex++)
		{
			pMeshTexSets[objectIndex] = 0;
		}
		// TJC - run through all models and if the model is not set for dynamic texture sets
		// assign all its textures to sub-set 0. 
		for (objectIndex=0;objectIndex<totalObjects;objectIndex++)
		{
			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

			// if slot empty then continue
			if (!pObjectAgg)
				continue;

			TupObject tupObject(pObjectAgg);
			if (tupObject.GetObjectType()==TupObject::TRI_MESH)
			{
				int dynamicTextureSet = pObjectAgg->FindScalarByKey("Annotate_DynamicTextureSet")->GetAsInt();
				if (!dynamicTextureSet)
				{
					TupperwareList *pMeshImageRefList = pObjectAgg->FindListByKey("Annotate_MeshImageRefs");
					assert(pMeshImageRefList);
					int numImageRefs = pMeshImageRefList->GetLength();
					int *pImageRefs = pMeshImageRefList->GetAsInt();
					for (int refIndex = 0;refIndex<numImageRefs;refIndex++)
					{
						int imageRef = pImageRefs[refIndex];
						if (imageRef!=-1)
						{
							assert(imageRef<totalImages);
							pImageTexSets[imageRef] = 0;
						}
					}
				}
			}
		}
		// TJC - run through all dynamic texture models models and assign texture sub-set #s to 
		// each model.  If a model contains only sub-set 0 textures, 
		// dont assign it a sub-set #. 
		for (objectIndex=0;objectIndex<totalObjects;objectIndex++)
		{
			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

			// if slot empty then continue
			if (!pObjectAgg)
				continue;

			TupObject tupObject(pObjectAgg);
			if (tupObject.GetObjectType()==TupObject::TRI_MESH)
			{
				int dynamicTextureSet = pObjectAgg->FindScalarByKey("Annotate_DynamicTextureSet")->GetAsInt();
				if (dynamicTextureSet)
				{
					TupperwareList *pMeshImageRefList = pObjectAgg->FindListByKey("Annotate_MeshImageRefs");
					assert(pMeshImageRefList);
					int numImageRefs = pMeshImageRefList->GetLength();
					int *pImageRefs = pMeshImageRefList->GetAsInt();
					int otherTexSet = -1; // start with no other texture set
					bool bMultipleOtherTexSets = false;
					bool bFoundUnassignedImage = false;
					// go through the list of image references and see if any are not assigned
					for (int refIndex = 0;refIndex<numImageRefs;refIndex++)
					{
						int imageRef = pImageRefs[refIndex];
						if (imageRef!=-1)
						{
							assert(imageRef<totalImages);
							// if it is unassigned
							if (pImageTexSets[imageRef] == -1)
							{
								bFoundUnassignedImage = true;
							}
							// also check if assigned to another dynamic object
							else if (pImageTexSets[imageRef]>0)
							{
								// not assigned yet
								if (otherTexSet==-1)
								{
									// assign me to this set (for now)
									otherTexSet = pImageTexSets[imageRef];
								}
								else if (otherTexSet != pImageTexSets[imageRef])
								{
									bMultipleOtherTexSets = true;
								}
							}
						}
					}
					// if there were no images assigned to another dynamic set and we found at least one that has not been assigned
					if (otherTexSet==-1 && bFoundUnassignedImage)
					{
						// did find one... allocate a sub set #...
						pMeshTexSets[objectIndex] = nextAvailableSubSet++;
						// set all the unassigned images to this set as well
						for (int refIndex = 0;refIndex<numImageRefs;refIndex++)
						{
							int imageRef = pImageRefs[refIndex];
							if (imageRef!=-1)
							{
								assert(imageRef<totalImages);
								// if it is unassigned
								if (pImageTexSets[imageRef] == -1)
								{
									pImageTexSets[imageRef] = pMeshTexSets[objectIndex];
								}
							}
						}
					}
					// if all my images were assigned to another set
					else if (otherTexSet!=-1 && !bMultipleOtherTexSets && !bFoundUnassignedImage)
					{
						pMeshTexSets[objectIndex] = otherTexSet;
					}
					// if all my images need to be in the first set
					else
					{
						for (int refIndex = 0;refIndex<numImageRefs;refIndex++)
						{
							int imageRef = pImageRefs[refIndex];
							if (imageRef!=-1)
							{
								assert(imageRef<totalImages);
								pImageTexSets[imageRef] = 0;
							}
						}
					}
				}
			}
		}
		// TJC TODO: If a model contains a subset or superset of the 
		// -1 textures in another model, assign it to the same sub-set as 
		// the other model.  in other words, merge like sub-sets.. 

		// TJC - run back through textures and change any remaining -1 textures to 0.  these 
		// should only be the unused textures... 
		// and set palettes to go where-ever the textures go 
		for (imageIndex = 0;imageIndex<totalImages;imageIndex++)
		{
			if (pImageTexSets[imageIndex] == -1)
				pImageTexSets[imageIndex] = 0;
		}
		// set total for adding into the tree later
		numTextureSets = nextAvailableSubSet;
		// add the set numbers into the tree
		int totalObjects = m_pTupObjectPool->GetNumObjects();
		for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
		{
			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

			// if slot empty then continue
			if (!pObjectAgg)
				continue;

			TupObject tupObject(pObjectAgg);
			if (tupObject.GetObjectType()==TupObject::TRI_MESH)
			{
				pObjectAgg->AddScalarInt("Annotate_TextureSet",NULL,pMeshTexSets[objectIndex]);
			}
		}
		int totalImages = m_pTupImagePool->GetNumImages();
		for (int imageIndex=0;imageIndex<totalImages;imageIndex++)
		{
			TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);

			// if slot empty then continue
			if (!pImageAgg)
				continue;

			pImageAgg->AddScalarInt("Annotate_TextureSet",NULL,pImageTexSets[imageIndex]);
		}
		delete pMeshTexSets;
		delete pImageTexSets;
	}
	// if no dynamic texture sets then place all in the first one
	else
	{
		numTextureSets = 1;
		int totalObjects = m_pTupObjectPool->GetNumObjects();
		for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
		{
			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

			// if slot empty then continue
			if (!pObjectAgg)
				continue;

			TupObject tupObject(pObjectAgg);
			if (tupObject.GetObjectType()==TupObject::TRI_MESH)
			{
				pObjectAgg->AddScalarInt("Annotate_TextureSet",NULL,0);
			}
		}
		int totalImages = m_pTupImagePool->GetNumImages();
		for (int imageIndex=0;imageIndex<totalImages;imageIndex++)
		{
			TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);

			// if slot empty then continue
			if (!pImageAgg)
				continue;

			pImageAgg->AddScalarInt("Annotate_TextureSet",NULL,0);
		}
	}
	// add in the total texture sets
	TupperwareAggregate *m_pSceneAgg = m_pTupScene->GetBaseAggregate();
	m_pSceneAgg->AddScalarInt("Annotate_TotalTextureSets",NULL,numTextureSets);
}


// this function will add a list to each mesh indicating all the image references made by the mesh
void ProcessTup::AddMeshImageRefs(void)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			TupTriMesh tupTriMesh(pObjectAgg);
			Keeper<int> usedMaterialRefs; // keeper to hold the unique image refs
			Keeper<int> usedMapRefs; // keeper to hold the unique image refs
			Keeper<int> usedImageRefs; // keeper to hold the unique image refs
			AnnotateHelper::CalculateUsedMatsMapsAndImages(tupTriMesh,*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,
																usedMaterialRefs,usedMapRefs,usedImageRefs);
			// if there were no images then add -1
			if (usedImageRefs.GetSize()==0)
			{
				int dummy = -1;
				usedImageRefs.Add(dummy);
			}
			// add reference list to tree
			pObjectAgg->AddListInt("Annotate_MeshImageRefs",NULL,usedImageRefs.GetArray());
		}
	}
}

// this method will set the face render attributes into the mesh itself.
// this uses a combination of face attributes from the faceAttribute modifier as well as the material
void ProcessTup::SetFaceRenderAttributes(void)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{

		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
		if (!pObjectAgg) // make sure this slot is not empty
			continue;

		// only do objects that have no parent
		TupperwareList *pParentObjectReferenceList = pObjectAgg->FindListByKey("Annotate_ParentObjectReferenceList");
		if (pParentObjectReferenceList)
			continue;
			
		// find the base mesh (if there is one)
		TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectIndex);
		if (pBaseMeshAgg)
		{
			int meshStage1FaceFlags = 0; // keep track of what all the faces are doing and use later after going through the faces
			// see if the base mesh has not already done this (there may be more than one derived object pointing to the base mesh)
			TupperwareScalar *pMeshHasTexture8BitAlphaScalar = pBaseMeshAgg->FindScalarByKey("Annotate_MeshHasTexture8BitAlpha");
			// only do this once (if this is a problem we will need to make a branch unique in this filter)
			if (!pMeshHasTexture8BitAlphaScalar)
			{
				int baseMeshFaceFlags = 0; // flags to set on all faces from the mesh
				TupperwareScalar *pTwoSidedScalar = pBaseMeshAgg->FindScalarByKey("Annotate_Force Two Sided Faces");
				if (pTwoSidedScalar)
				{
					baseMeshFaceFlags |= TupTriMesh::FLAGS_FACE_TWO_SIDED;
				}

				TupTriMesh tupTriMesh(pBaseMeshAgg);
				int *pFaceFlags;
				int numFaces;
				tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);
				int *pFaceMaterialIndices;
				tupTriMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,numFaces);
				int faceIndex;
				for (faceIndex=0;faceIndex<numFaces;faceIndex++)
				{
					// set defaults
					pFaceFlags[faceIndex] &= ~(TupTriMesh::FLAGS_FACE_Z_BIAS_MASK|
											   TupTriMesh::FLAGS_FACE_TWO_SIDED|
												TupTriMesh::FLAGS_FACE_NO_Z_WRITE|
											   TupTriMesh::FLAGS_FACE_HAS_TEXTURE|
											   TupTriMesh::FLAGS_FACE_HAS_TEXTURE_ALPHA|
												TupTriMesh::FLAGS_FACE_HAS_TEXTURE_8BIT_ALPHA);

					pFaceFlags[faceIndex] |= baseMeshFaceFlags; // any that the mesh wants set
				}

				// get the material for this mesh (there can be only one entry in the material list)
				int materialRef = pBaseMeshAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
				// if this is using a material
				if (materialRef!=-1)
				{
					TupperwareAggregate *pTupMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
					assert(pTupMaterialAgg);
					TupMaterial tupMat(pTupMaterialAgg);
					int totalSubMaterials;
					TupArray<int> subMaterialFlagsArray;

					if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
					{
						// we need to deal with multiple materials
						TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
						totalSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
						if (totalSubMaterials)
						{
							subMaterialFlagsArray.SetSize(totalSubMaterials);

							for (int materialIndex = 0;materialIndex<totalSubMaterials;materialIndex++)
							{
								// defaults
								subMaterialFlagsArray[materialIndex] = 0;

								TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
								TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
								int materialPoolIndex;
								tupSubMaterial.GetMaterialReference(materialPoolIndex);
								if (materialPoolIndex!=-1)
								{
									TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial( materialPoolIndex );
									TupMaterial tupSubMat( pMaterialAgg );
									if (tupSubMat.GetMaterialType()==TupMaterial::STANDARD_MATERIAL)
									{
										TupStandardMaterial tupStandardMaterial(pMaterialAgg);
										int flags;
										tupStandardMaterial.GetFlags(flags);
										if (!m_bClearTwoSidedFaceFlags && (flags&TupStandardMaterial::FLAGS_TWO_SIDED))
										{
											subMaterialFlagsArray[materialIndex] |= TupTriMesh::FLAGS_FACE_TWO_SIDED;
										}
										int materialHasTexture = pMaterialAgg->FindScalarByKey("Annotate_MaterialHasTexture")->GetAsInt();
										if (materialHasTexture)
										{
											subMaterialFlagsArray[materialIndex] |= TupTriMesh::FLAGS_FACE_HAS_TEXTURE;
										}
										int materialHasTextureAlpha = pMaterialAgg->FindScalarByKey("Annotate_MaterialHasTextureAlpha")->GetAsInt();
										if (materialHasTextureAlpha)
										{
											subMaterialFlagsArray[materialIndex] |= TupTriMesh::FLAGS_FACE_HAS_TEXTURE_ALPHA;
										}
										int materialHasTexture8BitAlpha = pMaterialAgg->FindScalarByKey("Annotate_MaterialHasTexture8BitAlpha")->GetAsInt();
										if (materialHasTexture8BitAlpha)
										{
											subMaterialFlagsArray[materialIndex] |= TupTriMesh::FLAGS_FACE_HAS_TEXTURE_8BIT_ALPHA;
										}
									}
									// grab data from the maps referenced by the material
									TupArray<int> mapRefsArray;
									AnnotateHelper::GetMapsFromMaterial(*m_pTupMaterialPool,materialPoolIndex,mapRefsArray);
									int numMaps = mapRefsArray.GetSize();
									for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
									{
										int mapRef = mapRefsArray[mapIndex];
										if (mapRef==-1)
											continue;
										TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapRef);
										TupperwareScalar *pTwoSidedScalar = pMapAgg->FindScalarByKey("Annotate_TwoSidedFaces");
										if (pTwoSidedScalar)
										{
											subMaterialFlagsArray[materialIndex]  |= TupTriMesh::FLAGS_FACE_TWO_SIDED;
										}
									}
								}
							}
						}
						else
						{
							totalSubMaterials = 1;
							subMaterialFlagsArray.SetSize(totalSubMaterials);
							subMaterialFlagsArray[0] = 0;
						}
					}
					else if ( tupMat.GetMaterialType( ) == TupMaterial::STANDARD_MATERIAL )
					{
						totalSubMaterials = 1;
						subMaterialFlagsArray.SetSize(totalSubMaterials);
						subMaterialFlagsArray[0] = 0;

						TupStandardMaterial tupStandardMaterial(pTupMaterialAgg);
						int flags;
						tupStandardMaterial.GetFlags(flags);
						if (!m_bClearTwoSidedFaceFlags && (flags&TupStandardMaterial::FLAGS_TWO_SIDED))
						{
							subMaterialFlagsArray[0] |= TupTriMesh::FLAGS_FACE_TWO_SIDED;
						}
						int materialHasTexture = pTupMaterialAgg->FindScalarByKey("Annotate_MaterialHasTexture")->GetAsInt();
						if (materialHasTexture)
						{
							subMaterialFlagsArray[0] |= TupTriMesh::FLAGS_FACE_HAS_TEXTURE;
						}
						int materialHasTextureAlpha = pTupMaterialAgg->FindScalarByKey("Annotate_MaterialHasTextureAlpha")->GetAsInt();
						if (materialHasTextureAlpha)
						{
							subMaterialFlagsArray[0] |= TupTriMesh::FLAGS_FACE_HAS_TEXTURE_ALPHA;
						}
						int materialHasTexture8BitAlpha = pTupMaterialAgg->FindScalarByKey("Annotate_MaterialHasTexture8BitAlpha")->GetAsInt();
						if (materialHasTexture8BitAlpha)
						{
							subMaterialFlagsArray[0] |= TupTriMesh::FLAGS_FACE_HAS_TEXTURE_8BIT_ALPHA;
						}
						// grab data from the maps referenced by the material
						TupArray<int> mapRefsArray;
						AnnotateHelper::GetMapsFromMaterial(*m_pTupMaterialPool,materialRef,mapRefsArray);
						int numMaps = mapRefsArray.GetSize();
						for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
						{
							int mapRef = mapRefsArray[mapIndex];
							if (mapRef==-1)
								continue;
							TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapRef);
							TupperwareScalar *pTwoSidedScalar = pMapAgg->FindScalarByKey("Annotate_TwoSidedFaces");
							if (pTwoSidedScalar)
							{
								subMaterialFlagsArray[0]  |= TupTriMesh::FLAGS_FACE_TWO_SIDED;
							}
						}

					}
					else // any other materials
					{
						totalSubMaterials = 1;
						subMaterialFlagsArray.SetSize(totalSubMaterials);
						subMaterialFlagsArray[0] = 0;
					}
					// apply sub material settings to faces
					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						// the materialIndex is the offset for the multi-subobject material for each face
						int materialIndex = pFaceMaterialIndices[faceIndex];
						// make sure the the index fits within the total
						if (materialIndex<0 || materialIndex>=totalSubMaterials)
						{
							materialIndex = 0;
						}
						pFaceFlags[faceIndex] |= subMaterialFlagsArray[materialIndex];

						// update the mesh face flags
						// only include stage 1 faces
						if ((pFaceFlags[faceIndex] & TupTriMesh::FLAGS_FACE_STAGE_MASK)==0)
						{
							meshStage1FaceFlags |= pFaceFlags[faceIndex];
						}
					}
				}
				// check and override based on the faces attributes
				TupperwareAggregate *pFaceAttributesAgg = AnnotateHelper::FindFaceAttributes(*m_pTupModifierPool,*m_pTupObjectPool,objectIndex);
				if (pFaceAttributesAgg)
				{
					AnnotateHelper::FaceRenderAttributes *pFaceAttributeArray;
					int *pFaceAttributeIndices;
					int numAttributeFaces;
					AnnotateHelper::BuildFaceRenderAttributes(*m_pTupFaceConfigRoot,pFaceAttributesAgg,
															  &pFaceAttributeArray,&pFaceAttributeIndices,
															  numAttributeFaces);
					for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						if (faceIndex<numAttributeFaces)
						{
							AnnotateHelper::FaceRenderAttributes &faceRenderAttributes = pFaceAttributeArray[pFaceAttributeIndices[faceIndex]];
							// see if we need to stomp the opacity
							// see if we need to stomp the two sided flag
							if (!m_bClearTwoSidedFaceFlags && faceRenderAttributes.m_bSetTwoSided)
							{
								if (faceRenderAttributes.m_twoSidedValue)
								{
									pFaceFlags[faceIndex] |= TupTriMesh::FLAGS_FACE_TWO_SIDED;
								}
								else
								{
									pFaceFlags[faceIndex] &= ~TupTriMesh::FLAGS_FACE_TWO_SIDED;
								}
							}
							// set the zbias (no equivilent on material)
							pFaceFlags[faceIndex] = (pFaceFlags[faceIndex] & ~TupTriMesh::FLAGS_FACE_Z_BIAS_MASK) |
													((faceRenderAttributes.m_zbiasValue<<TupTriMesh::FLAGS_FACE_Z_BIAS_SHIFT)&TupTriMesh::FLAGS_FACE_Z_BIAS_MASK);
							if (faceRenderAttributes.m_bNoZWrite)
							{
								pFaceFlags[faceIndex] |= TupTriMesh::FLAGS_FACE_NO_Z_WRITE;
							}
							else
							{
								pFaceFlags[faceIndex] &= ~TupTriMesh::FLAGS_FACE_NO_Z_WRITE;
							}
						}
					}
					delete [] pFaceAttributeArray;
					delete [] pFaceAttributeIndices;
				}

				int numTwoSidedFaces = 0;
				int numNoZWriteFaces = 0;
				for (faceIndex=0;faceIndex<numFaces;faceIndex++)
				{
					if (pFaceFlags[faceIndex]&TupTriMesh::FLAGS_FACE_TWO_SIDED)
						numTwoSidedFaces++;
					if (pFaceFlags[faceIndex]&TupTriMesh::FLAGS_FACE_NO_Z_WRITE)
						numNoZWriteFaces++;
				}
				if (numTwoSidedFaces)
				{
					TupperwareScalar *pMayaObjectNameScalar = pBaseMeshAgg->FindScalarByKey("MayaObjectName");
					if (pMayaObjectNameScalar)
					{
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Mesh %s has %d of %d faces two sided",pMayaObjectNameScalar->GetAsString(),numTwoSidedFaces,numFaces);
					}
				}
				if (numNoZWriteFaces)
				{
					pBaseMeshAgg->AddScalarInt("Annotate_MeshHasNoZWriteFaces",NULL,1);
					TupperwareScalar *pMayaObjectNameScalar = pBaseMeshAgg->FindScalarByKey("MayaObjectName");
					if (pMayaObjectNameScalar)
					{
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Mesh %s has %d of %d faces that are marked as no z write",pMayaObjectNameScalar->GetAsString(),numNoZWriteFaces,numFaces);
					}
				}


				int meshHasTexture8BitAlpha = (meshStage1FaceFlags & TupTriMesh::FLAGS_FACE_HAS_TEXTURE_8BIT_ALPHA) ? TRUE : FALSE;
				pBaseMeshAgg->AddScalarInt("Annotate_MeshHasTexture8BitAlpha",NULL,meshHasTexture8BitAlpha);
			}
		}
	}
}

// go through the materials used on the mesh and gather any envionment map opacities
void ProcessTup::SetMeshSubMaterialEnvironmentMapData(void)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
		if (!pObjectAgg) // make sure this slot is not empty
			continue;

		// only do meshes
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType() != TupObject::TRI_MESH)
			continue;

		TupArray<float> subMaterialEnvironmentMapOpacityValues;
		TupArray<int> subMaterialHasEnvironmentMap;

		// get the material for this mesh (there can be only one entry in the material list)
		int materialRef = pObjectAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
		// if this is using a material
		if (materialRef!=-1)
		{
			TupperwareAggregate *pTupMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
			assert(pTupMaterialAgg);
			TupMaterial tupMat(pTupMaterialAgg);
			int totalSubMaterials;

			if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
			{
				// we need to deal with multiple materials
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
				totalSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
				if (totalSubMaterials)
				{
					subMaterialEnvironmentMapOpacityValues.SetSize(totalSubMaterials);
					subMaterialHasEnvironmentMap.SetSize(totalSubMaterials);

					for (int materialIndex = 0;materialIndex<totalSubMaterials;materialIndex++)
					{
						// defaults
						subMaterialEnvironmentMapOpacityValues[materialIndex] = 1.0f;
						subMaterialHasEnvironmentMap[materialIndex] = 0;

						TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
						TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
						int materialPoolIndex;
						tupSubMaterial.GetMaterialReference(materialPoolIndex);
						if (materialPoolIndex!=-1)
						{
							TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial( materialPoolIndex );
							TupMaterial tupSubMat( pMaterialAgg );
							if (tupSubMat.GetMaterialType()==TupMaterial::STANDARD_MATERIAL)
							{
								TupStandardMaterial tupStandardMaterial(pMaterialAgg);

								int totalMaps = tupStandardMaterial.GetNumMaps();
								// look for reflection 
								for(int mapIndex=0;mapIndex<totalMaps;mapIndex++ )
								{
									TupperwareAggregate *pMapAgg = tupStandardMaterial.GetMap(mapIndex);
									TupStandardMaterialMap tupMaterialMap(pMapAgg);
									char *mapName;
									tupMaterialMap.GetMapName(&mapName);
									assert(mapName);
									if (!stricmp(mapName,"Reflection"))
									{
										tupMaterialMap.GetAmount(subMaterialEnvironmentMapOpacityValues[materialIndex]);
										subMaterialHasEnvironmentMap[materialIndex] = 1;
									}
								}
							}
						}
					}
				}
				else
				{
					totalSubMaterials = 1;
					subMaterialEnvironmentMapOpacityValues.SetSize(totalSubMaterials);
					subMaterialEnvironmentMapOpacityValues[0] = 1.0f;
					subMaterialHasEnvironmentMap.SetSize(totalSubMaterials);
					subMaterialHasEnvironmentMap[0] = 0;
				}
			}
			else if ( tupMat.GetMaterialType( ) == TupMaterial::STANDARD_MATERIAL )
			{
				totalSubMaterials = 1;
				subMaterialEnvironmentMapOpacityValues.SetSize(totalSubMaterials);
				subMaterialEnvironmentMapOpacityValues[0] = 1.0f;
				subMaterialHasEnvironmentMap.SetSize(totalSubMaterials);
				subMaterialHasEnvironmentMap[0] = 0;

				TupStandardMaterial tupStandardMaterial(pTupMaterialAgg);

				int totalMaps = tupStandardMaterial.GetNumMaps();
				// look for reflection 
				for(int mapIndex=0;mapIndex<totalMaps;mapIndex++ )
				{
					TupperwareAggregate *pMapAgg = tupStandardMaterial.GetMap(mapIndex);
					TupStandardMaterialMap tupMaterialMap(pMapAgg);
					char *mapName;
					tupMaterialMap.GetMapName(&mapName);
					assert(mapName);
					if (!stricmp(mapName,"Reflection"))
					{
						tupMaterialMap.GetAmount(subMaterialEnvironmentMapOpacityValues[0]);
						subMaterialHasEnvironmentMap[0] = 1;
					}
				}
			}
			else // any other materials
			{
				totalSubMaterials = 1;
				subMaterialEnvironmentMapOpacityValues.SetSize(totalSubMaterials);
				subMaterialEnvironmentMapOpacityValues[0] = 1.0f;
				subMaterialHasEnvironmentMap.SetSize(totalSubMaterials);
				subMaterialHasEnvironmentMap[0] = 0;
			}
		}
		else // any other materials
		{
			int totalSubMaterials = 1;
			subMaterialEnvironmentMapOpacityValues.SetSize(totalSubMaterials);
			subMaterialEnvironmentMapOpacityValues[0] = 1.0f;
			subMaterialHasEnvironmentMap.SetSize(totalSubMaterials);
			subMaterialHasEnvironmentMap[0] = 0;
		}

		pObjectAgg->AddListFloat("Annotate_SubMaterialEnvironmentMapOpacityList",NULL,subMaterialEnvironmentMapOpacityValues);
		pObjectAgg->AddListInt("Annotate_SubMaterialHasEnvironmentMapList",NULL,subMaterialHasEnvironmentMap);
	}
}

class VertexColorMap
{
public:
	int m_numColorVerts;
	Vector3 *m_pColorVerts;
	int *m_pColorIndices;
};

void ProcessTup::SetTransparentAndObscuredStageFacesAsHidden(void)
{
	if (!m_bSetTransparentAndObscuredStageFacesAsHidden)
		return;

	int totalFaces = 0;
	int totalFacesToHide = 0;
	int totalObscuredFacesToHide = 0;
	int totalTransparentFacesToHide = 0;

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Marking Transparent and Obscured Stage Faces as Hidden");

	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pMeshAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pMeshAgg)
			continue;

		TupObject tupObject(pMeshAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			TupTriMesh tupTriMesh(pMeshAgg);

			int *pFaceFlags;
			int numFaces;
			tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);

			if (numFaces==0)
				continue;

			totalFaces+=numFaces;

			// search for the next stage face links
			int *pNextStageFaces = NULL;
			TupperwareList *pNextStageFaceIndicesList = pMeshAgg->FindListByKey("CombineMeshes_NextStageFaceIndices");
			if (pNextStageFaceIndicesList)
			{
				pNextStageFaces = pNextStageFaceIndicesList->GetAsInt();
			}
			TupArray<int> previousStageFaces;
			previousStageFaces.SetSize(numFaces);
			int faceIndex;
			// build previous stage face list
			for (faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				previousStageFaces[faceIndex] = -1;
			}
			if (pNextStageFaces)
			{
				for (faceIndex=0;faceIndex<numFaces;faceIndex++)
				{
					int nextStageFace = pNextStageFaces[faceIndex];
					if (nextStageFace!=-1)
					{
						previousStageFaces[nextStageFace] = faceIndex;
					}
				}
			}

			// search for the vertex alpha reference list
			// set up vertex color info so we can later determine if we need to increment the vertex color offsets
			int *pVertexAlphaRefData = NULL;
			int numAlphaSets = 0;
			TupperwareList *pVertexAlphaRefList = pMeshAgg->FindListByKey("Annotate_VertexAlphaRefList");
			if (pVertexAlphaRefList)
			{
				pVertexAlphaRefData = pVertexAlphaRefList->GetAsInt();
				numAlphaSets = pVertexAlphaRefList->GetLength();
				// check if all references are pointing to a -1 reference (standard white)
				if (numAlphaSets==1 && pVertexAlphaRefData[0] == -1 && pVertexAlphaRefData[0] == -1)
				{
					// lets say there are no vertex alpha sets
					numAlphaSets = 0;
				}
			}
			TupArray<VertexColorMap> vertexAlphaMaps;
			// set up the vertex color sets
			if (numAlphaSets)
			{
				vertexAlphaMaps.SetSize(numAlphaSets);

				for (int colorSetIndex = 0;colorSetIndex<numAlphaSets;colorSetIndex++)
				{
					VertexColorMap &vertexAlphaMap = vertexAlphaMaps[colorSetIndex];

					int alphaObjectRef = pVertexAlphaRefData[colorSetIndex];

					if (alphaObjectRef!=-1) // if valid object reference
					{
						TupperwareAggregate *pChildMeshMapAgg = AnnotateHelper::FindChildVertexAlphaMeshMap(*m_pTupObjectPool,alphaObjectRef);
						assert(pChildMeshMapAgg);

						TupTriMeshMap tupTriMeshMap(pChildMeshMapAgg);

						int numMapFaces;
						tupTriMeshMap.GetMapVerts((float **) &vertexAlphaMap.m_pColorVerts, vertexAlphaMap.m_numColorVerts );
						tupTriMeshMap.GetMapIndices( &vertexAlphaMap.m_pColorIndices, numMapFaces );
						assert(numFaces == numMapFaces);
					}
					else
					{
						// make this entry not point to any data
						vertexAlphaMap.m_numColorVerts=0;
						vertexAlphaMap.m_pColorVerts=NULL;
						vertexAlphaMap.m_pColorIndices=NULL;
					}
				}
			}
			int numFacesToHide = 0;
			int numObscuredFacesToHide = 0;
			int numTransparentFacesToHide = 0;

			TupArray<int> hideFaceEntries;
			hideFaceEntries.SetSize(numFaces);
			for (faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				hideFaceEntries[faceIndex] = 1; // default to keep this face
			}
			int faceIndex3;
			for (faceIndex=0,faceIndex3=0;faceIndex<numFaces;faceIndex++,faceIndex3+=3)
			{
				bool bFaceTransparent = true;
				bool bFaceOpaque = true;
				if (pFaceFlags[faceIndex]&TupTriMesh::FLAGS_FACE_HAS_TEXTURE_ALPHA)
				{
					bFaceOpaque = false;
				}
				if (numAlphaSets)
				{
					for (int alphaIndex=0;alphaIndex<numAlphaSets;alphaIndex++)
					{
						VertexColorMap &vertexAlphaMap = vertexAlphaMaps[alphaIndex];

						if (vertexAlphaMap.m_pColorVerts==NULL)
						{
							bFaceTransparent = false;
							continue;
						}

						for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
						{
							float facePointAlpha = vertexAlphaMap.m_pColorVerts[vertexAlphaMap.m_pColorIndices[faceIndex3+facePointIndex]].X();
							if (facePointAlpha!=0.0f)
							{
								bFaceTransparent = false;
							}
							if (facePointAlpha!=1.0f)
							{
								bFaceOpaque = false;
							}
						}
					}
				}
				else
				{
					bFaceTransparent = false;
				}
				// if face is opaque and has faces in lower layers
				if (pNextStageFaces && bFaceOpaque && previousStageFaces[faceIndex]!=-1)
				{
					// mark all the lower faces as hidden
					int lowerStageFace = previousStageFaces[faceIndex];
					while (lowerStageFace!=-1)
					{
						// if not already marked to be hidden
						if (hideFaceEntries[lowerStageFace]==1)
						{
							hideFaceEntries[lowerStageFace]=0;
							numObscuredFacesToHide++;
							numFacesToHide++;
						}
						lowerStageFace = previousStageFaces[lowerStageFace];
					}
				}
				// face is transparent
				if (bFaceTransparent)
				{
					if (hideFaceEntries[faceIndex]==1)
					{
						hideFaceEntries[faceIndex]=0;
						numTransparentFacesToHide++;
						numFacesToHide++;
					}
				}
			}
			if (numFacesToHide)
			{
				TupperwareScalar *pMayaObjectNameScalar = pMeshAgg->FindScalarByKey("MayaObjectName");

				char *pShapeName = "mesh";
				if (pMayaObjectNameScalar)
				{
					pShapeName = pMayaObjectNameScalar->GetAsString();
				}

				if (numFacesToHide==numFaces)
				{
					hideFaceEntries[0] = 1; // keep first face
					numFacesToHide--;
				}
				totalFacesToHide += numFacesToHide;
				totalObscuredFacesToHide += numObscuredFacesToHide;
				totalTransparentFacesToHide += numTransparentFacesToHide;

				PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Marking %d of %d faces from mesh %s as hidden - %d Obscured %d Transparent",numFacesToHide,numFaces,pShapeName,numObscuredFacesToHide,numTransparentFacesToHide);

				for (faceIndex=0;faceIndex<numFaces;faceIndex++)
				{
					// if this face is to be marked as hidden
					if (hideFaceEntries[faceIndex]==0)
					{
						pFaceFlags[faceIndex] |= TupTriMesh::FLAGS_FACE_HIDDEN;
					}
				}
				// if any staged faces to setup
				if (pNextStageFaces)
				{
					TupArray<int> newNextStageFaces;
					newNextStageFaces.SetSize(numFaces);
					TupArray<int> newFaceFlags;
					newFaceFlags.SetSize(numFaces);

					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						// mark all as no link
						newNextStageFaces[faceIndex] = -1;
						// copy the flags and clear the stages
						newFaceFlags[faceIndex] = (pFaceFlags[faceIndex] & ~TupTriMesh::FLAGS_FACE_STAGE_MASK);
					}
					// don't have stages link to hidden faces
					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						// if original face has stages and is the first face in the list
						if (((pFaceFlags[faceIndex] & TupTriMesh::FLAGS_FACE_STAGE_MASK)==0) && (pNextStageFaces[faceIndex]!=-1))
						{
							int stageValue = 0;
							int stageFaceIndex = faceIndex;
							int prevFaceToSet = -1;
							// loop through all faces in stages and set flags as well as next faces
							while(stageFaceIndex!=-1)
							{
								// if not hidden
								if ((pFaceFlags[stageFaceIndex] & TupTriMesh::FLAGS_FACE_HIDDEN)==0)
								{
									// set correct stage
									newFaceFlags[stageFaceIndex] |= (stageValue<<TupTriMesh::FLAGS_FACE_STAGE_SHIFT);
									stageValue++; // increment for next pass
									// set prev to point to me
									if (prevFaceToSet!=-1)
									{
										newNextStageFaces[prevFaceToSet] = stageFaceIndex;
									}
									prevFaceToSet = stageFaceIndex;
								}
								stageFaceIndex = pNextStageFaces[stageFaceIndex];
							}
						}
					}
					pNextStageFaceIndicesList->SetAsInt(newNextStageFaces);
					tupTriMesh.AddFaceFlags(newFaceFlags.GetData(),numFaces);
				}
			}
		}
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Marked a total of %d of %d faces as hidden - %d Obscured %d Transparent",totalFacesToHide,totalFaces,totalObscuredFacesToHide,totalTransparentFacesToHide);
}

void ProcessTup::PropagateSkinModifiers(void)
{
	m_totalBoneMeshes = 0;

	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::DERIVED)
		{
			TupDerived tupDerived(pObjectAgg);
			int totalModifierSlots = tupDerived.GetNumModifierSlots();
			// if there is any modifiers
			if (totalModifierSlots)
			{
				bool foundValidModifier = false;
				// loop through the modifiers and see if any deal with the skin modifiers
				for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
				{
					TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
					if (pDerivedModifierAgg)
					{
						TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
						int modifierReference;
						tupDerivedModifier.GetModifierReference(modifierReference);
						TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierReference);
						TupModifier tupModifier(pModifierAgg);
						switch(tupModifier.GetModifierType())
						{
							case TupModifier::SKIN_MODIFIER:
								{
									TupSkinModifier tupSkinModifier(pModifierAgg);

									TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
									TupSkinModifierContext tupSkinModifierContext(pModifierContextAgg);
									float *pFloatList;
									tupSkinModifierContext.GetBaseTM(&pFloatList);
									Graphics4x4 *pBaseMatrix = (Graphics4x4 *)pFloatList;
									Graphics4x4 invBaseMatrix;
									pBaseMatrix->Invert(invBaseMatrix);

									int flags;
									tupSkinModifier.GetFlags(flags);
									if (flags&TupSkinModifier::FLAGS_ENABLED)
									{
										TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectIndex);
										// do only when we have a base mesh
										if (pBaseMeshAgg)
										{
											TupperwareAggregate *pBoneMeshAgg = pBaseMeshAgg->FindAggregateByKey("Annotate_BoneMeshData");
											// see if there is already one on the mesh
											if (!pBoneMeshAgg)
											{
												TupTriMesh tupTriMesh(pBaseMeshAgg);
												Vector3 *pPositionVerts;
												int numVerts;
												tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numVerts);
												int nBonesRefs = tupSkinModifier.GetNumBones();
												if (nBonesRefs && numVerts)
												{
													// add main aggregate to mesh
													pBoneMeshAgg = pBaseMeshAgg->AddAggregate("Annotate_BoneMeshData");
													// add the number of bones
													pBoneMeshAgg->AddScalarInt("NumBones",NULL,nBonesRefs);

													for (int boneIndex=0;boneIndex<nBonesRefs;boneIndex++)
													{
														char boneString[40];
														itoa(boneIndex,boneString,10);
														TupperwareAggregate *pMeshBoneAgg = pBoneMeshAgg->AddAggregate("Bone",boneString);

														TupperwareAggregate *pBoneAgg = tupSkinModifier.GetBone(boneIndex);
														TupSkinModifierBone tupBone(pBoneAgg);
														int boneReference;
														tupBone.GetNodeReference(boneReference);
														pMeshBoneAgg->AddScalarInt("BoneRef",NULL,boneReference);

														// get the length of the bone
														float boneLength = 1.0f;
														TupperwareAggregate *pBoneNodeAgg = m_pTupNodePool->GetNode(boneReference);
														assert(pBoneNodeAgg);
														TupNode boneNode(pBoneNodeAgg);
														int boneNodeObjectRef;
														boneNode.GetObjectReference(boneNodeObjectRef);
														assert(boneNodeObjectRef!=-1);
														TupperwareAggregate *pBoneNodeObjectAgg = m_pTupObjectPool->GetObjectAggregate(boneNodeObjectRef);
														assert(pBoneNodeObjectAgg);
														TupObject tupBoneNodeObject(pBoneNodeObjectAgg);
														if (tupBoneNodeObject.GetObjectType()==TupObject::BONE)
														{
															TupBone tupBoneNodeObjectBone(pBoneNodeObjectAgg);
															tupBoneNodeObjectBone.GetLength(boneLength);
														}
														pMeshBoneAgg->AddScalarFloat("BoneLength",NULL,boneLength);

														// get the matrix for the bone
														tupBone.GetBoneInitObjectTM(&pFloatList);
														Graphics4x4 *pBoneMatrix = (Graphics4x4 *)pFloatList;

														// convert into mesh coordinates
														Graphics4x4 boneToModelMatrix((*pBoneMatrix) * invBaseMatrix);
														pMeshBoneAgg->AddListFloat("BoneModelTM",NULL,(float *)&boneToModelMatrix,16);
													}
													int *pMeshBoneVerts = new int[numVerts];
													assert(tupDerivedModifier.GetModifierIndex()==modifierIndex);
													TupperwareAggregate *pVertexPoolAgg = tupSkinModifierContext.GetVertexPool();
													TupSkinModifierContextVertexPool tupSkinModifierContextVertexPool(pVertexPoolAgg);
													int numBoneVerts = tupSkinModifierContextVertexPool.GetNumVertices();
													for (int vertexIndex = 0;vertexIndex<numVerts;vertexIndex++)
													{
														if (vertexIndex<numBoneVerts)
														{
															// get the bones and weights for this vertex
															TupperwareAggregate *pBoneVertexAgg = tupSkinModifierContextVertexPool.GetVertex(vertexIndex);
															TupSkinModifierContextVertex tupSkinModifierContextVertex(pBoneVertexAgg);
															int nBones;
															int *pBones;
															int nWeights;
															float *pWeights;
															tupSkinModifierContextVertex.GetBones(&pBones,nBones);
															tupSkinModifierContextVertex.GetWeights(&pWeights,nWeights);
															assert(nBones==nWeights);
															float bestWeight = -1.0f;
															int bestBone = -1;
															// go through list of bones and weights and find the one that has the most weight
															for (int weightIndex=0;weightIndex<nWeights;weightIndex++)
															{
																if (pWeights[weightIndex]>bestWeight)
																{
																	bestWeight=pWeights[weightIndex];
																	bestBone = pBones[weightIndex];
																}
															}
															pMeshBoneVerts[vertexIndex] = bestBone;
														}
														else
														{
															pMeshBoneVerts[vertexIndex] = -1;
														}
													}
													pBoneMeshAgg->AddListInt("MeshBoneVerts",NULL,pMeshBoneVerts,numVerts);
													delete pMeshBoneVerts;
													foundValidModifier = true;
													m_totalBoneMeshes++;
												}
											}
										}
									}
								}
								break;
						}
						// stop searching if there is one
						if (foundValidModifier)
							break;
					}
				}
			}
		}
	}
}


void ProcessTup::RemapVertexBones(void)
{
	const char *pRemapVertexBonesOption = m_options.GetValue("RemapVertexBones");
	if (pRemapVertexBonesOption && !stricmp(pRemapVertexBonesOption,"true"))
	{
		TupString relativePath,fullPath,name,ext;
		TupString filename = "RemapVertexBones.tup"; // default filename
		const char *pValue = m_options.GetValue("RemapVertexBonesFile");
		if (pValue)
		{
			filename = pValue;
		}

		FilePath::GetPathsToFile(relativePath,fullPath,name,ext,filename,m_dataManager.GetConfigDirectory());

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Loading Remap Vertex Bones File %s",(const char *)fullPath);

		if (_access(fullPath,0)==-1)
			throw(PluginException(ERROR_FATAL,"Remap Vertex Bones File not found %s",(const char *)fullPath));
		if (_access(fullPath,4)==-1)
			throw(PluginException(ERROR_FATAL,"Cannot access Remap Vertex Bones File %s",(const char *)fullPath));

		TupperwareAggregate *pRemapVertexBonesAgg;
		int result = TupperwareLoadSaveManager::Read(fullPath,&pRemapVertexBonesAgg);
		if (!result)
			throw(PluginException(ERROR_FATAL,"Error reading Remap Vertex Bones File %s",(const char *)fullPath));

		Keeper<TupString> boneToRemap;
		TupArray<TupString> boneToRemapTo;

		int numErrors = 0;

		for (TupperwareIterator iter = pRemapVertexBonesAgg->FindAtomByKey("RemapBone"); !iter.Finished(); iter.Next()) 
		{
			TupperwareAtom *pItemAtom = iter.GetAsAtom();

			const char *pName = pItemAtom->GetName();
			if (!pName)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_RED|LogFlags::FONT_STYLE_BOLD,
					"Bone Remap File - RemapBone entry with no name");
				numErrors++;
				continue;
			}

			TupString name(pName);
			name.MakeLower();

			if (pItemAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
			{
				TupperwareScalar *pItemScalar = (TupperwareScalar *) pItemAtom;

				if (pItemScalar->GetDataType()==TupperwareScalar::DATA_STRING)
				{
					const char *pValue = pItemScalar->GetAsString();
					if (!pValue)
						continue;
					TupString value(pValue);
					value.MakeLower();

					int oldIndex = boneToRemap.GetSize();
					int newIndex = boneToRemap.Add(name);
					if (newIndex==oldIndex)
					{
						boneToRemapTo.Add(value);
					}
					else
					{
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED|LogFlags::FONT_COLOR_DK_RED|LogFlags::FONT_STYLE_BOLD,
							"Bone Remap File - duplicate name found %s",(const char *)name);
						numErrors++;
						continue;
					}
				}
			}
		}

		delete pRemapVertexBonesAgg;

		// if there were any errors reading the bone group table do a throw here instead of the constructor
		if (numErrors || boneToRemapTo.GetSize()==0)
			throw(PluginException(ERROR_FATAL,"Errors reading Remap Vertex Bones File %s",(const char *)fullPath));

		m_totalBoneMeshes = 0;

		int totalObjects = m_pTupObjectPool->GetNumObjects();
		for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
		{
			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

			// if slot empty then continue
			if (!pObjectAgg)
				continue;

			TupObject tupObject(pObjectAgg);
			if (tupObject.GetObjectType()==TupObject::DERIVED)
			{
				TupDerived tupDerived(pObjectAgg);
				int totalModifierSlots = tupDerived.GetNumModifierSlots();
				// if there is any modifiers
				if (totalModifierSlots)
				{
					bool foundValidModifier = false;
					// loop through the modifiers and see if any deal with the skin modifiers
					for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
					{
						TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
						if (pDerivedModifierAgg)
						{
							TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
							int modifierReference;
							tupDerivedModifier.GetModifierReference(modifierReference);
							TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierReference);
							TupModifier tupModifier(pModifierAgg);
							switch(tupModifier.GetModifierType())
							{
									case TupModifier::SKIN_MODIFIER:
									{
										TupSkinModifier tupSkinModifier(pModifierAgg);

										TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
										TupSkinModifierContext tupSkinModifierContext(pModifierContextAgg);
										float *pFloatList;
										tupSkinModifierContext.GetBaseTM(&pFloatList);
										Graphics4x4 *pBaseMatrix = (Graphics4x4 *)pFloatList;
										Graphics4x4 invBaseMatrix;
										pBaseMatrix->Invert(invBaseMatrix);

										int flags;
										tupSkinModifier.GetFlags(flags);
										if (flags&TupSkinModifier::FLAGS_ENABLED)
										{
											TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectIndex);
											// do only when we have a base mesh
											if (pBaseMeshAgg)
											{
												int nBonesRefs = tupSkinModifier.GetNumBones();
												if (nBonesRefs)
												{
													// these are a pair so we can look up the modifier bone index from a bone name
													Keeper<TupString> uniqueBoneNames;
													TupArray<int> uniqueModifierBoneIndices;
													// this array keeps track of the index into the above array for each bone (to handle duplicate entries)
													TupArray<int> modifierBoneIndexToUniqueIndexArray;
													modifierBoneIndexToUniqueIndexArray.SetSize(nBonesRefs);
													// set up the above arrays and keeper
													int boneIndex;
													for (boneIndex=0;boneIndex<nBonesRefs;boneIndex++)
													{
														TupperwareAggregate *pBoneAgg = tupSkinModifier.GetBone(boneIndex);
														TupSkinModifierBone tupBone(pBoneAgg);
														int boneReference;
														tupBone.GetNodeReference(boneReference);

														TupperwareAggregate *pBoneNodeAgg = m_pTupNodePool->GetNode(boneReference);
														assert(pBoneNodeAgg);
														TupNode boneNode(pBoneNodeAgg);
														char *pBoneName;
														boneNode.GetName(&pBoneName);
														TupString boneName(pBoneName);
														boneName.MakeLower();

														int oldBoneNameSize = uniqueBoneNames.GetSize();
														int keeperIndex = uniqueBoneNames.Add(boneName);
														// if we added this name to the keeper
														if (keeperIndex==oldBoneNameSize)
														{
															uniqueModifierBoneIndices.Add(boneIndex);
														}
														modifierBoneIndexToUniqueIndexArray[boneIndex] = keeperIndex;
													}
													// remap bone index to new bone index
													TupArray<int> boneRemapTable;
													boneRemapTable.SetSize(nBonesRefs);

													// set up remap table to use with the vertices on the modifier
													for (boneIndex=0;boneIndex<nBonesRefs;boneIndex++)
													{
														// name of the bone
														TupString boneName = uniqueBoneNames[modifierBoneIndexToUniqueIndexArray[boneIndex]];
														int remapBoneIndex = boneToRemap.GetIndex(boneName);
														// is this bone needing to be remapped
														if (remapBoneIndex!=-1)
														{
															// get name of the bone to remap to
															TupString boneNameToRemapTo = boneToRemapTo[remapBoneIndex];
															// look it up on the modifier to get the bone index to use
															int uniqueRemapBoneIndex = uniqueBoneNames.GetIndex(boneNameToRemapTo);
															if (uniqueRemapBoneIndex==-1)
															{
																PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_RED|LogFlags::FONT_STYLE_BOLD,
																	"Destination bone %s not found in skin modifier",(const char *)boneNameToRemapTo);
																numErrors++;
																boneRemapTable[boneIndex] = boneIndex;
															}
															else
															{
																// remap index to destination
																boneRemapTable[boneIndex] = uniqueModifierBoneIndices[uniqueRemapBoneIndex];
															}
														}
														// bone is not remapped
														else
														{
															boneRemapTable[boneIndex] = boneIndex;
														}
													}
													// loop through the verts of the modifier and remap indices to point to different bones
													assert(tupDerivedModifier.GetModifierIndex()==modifierIndex);
													TupperwareAggregate *pVertexPoolAgg = tupSkinModifierContext.GetVertexPool();
													TupSkinModifierContextVertexPool tupSkinModifierContextVertexPool(pVertexPoolAgg);
													int numBoneVerts = tupSkinModifierContextVertexPool.GetNumVertices();
													for (int boneVertIndex = 0;boneVertIndex<numBoneVerts;boneVertIndex++)
													{
														// get the bones and weights for this vertex
														TupperwareAggregate *pBoneVertexAgg = tupSkinModifierContextVertexPool.GetVertex(boneVertIndex);
														TupSkinModifierContextVertex tupSkinModifierContextVertex(pBoneVertexAgg);
														int nBones;
														int *pBones;
														int nWeights;
														float *pWeights;
														tupSkinModifierContextVertex.GetBones(&pBones,nBones);
														tupSkinModifierContextVertex.GetWeights(&pWeights,nWeights);
														assert(nBones==nWeights);
														TupArray<int> newBones;
														TupArray<float> newWeights;
														// go through list of bones build a new set of bones and weights from the remap information
														for (int weightIndex=0;weightIndex<nWeights;weightIndex++)
														{
															int newBoneIndex = boneRemapTable[pBones[weightIndex]];
															// see if assignment can merge with an existing assignment
															bool bAddWeight = true;
															for (int newWeightIndex=0;newWeightIndex<newWeights.GetSize();newWeightIndex++)
															{
																if (newBones[newWeightIndex]== newBoneIndex)
																{
																	newWeights[newWeightIndex]+= pWeights[weightIndex]; // add this weight to the other
																	bAddWeight = false; // no need to add bone and weight
																	break;
																}
															}
															if (bAddWeight)
															{
																newBones.Add(newBoneIndex);
																newWeights.Add(pWeights[weightIndex]);
															}
														}
														tupSkinModifierContextVertex.AddBones(newBones.GetData(),newBones.GetSize());
														tupSkinModifierContextVertex.AddWeights(newWeights.GetData(),newWeights.GetSize());
													}
													foundValidModifier = true;
												}
											}
										}
									}
								}
								break;
						}
						// stop searching if there is one
						if (foundValidModifier)
							break;
					}
				}
			}
		}
		if (numErrors)
		{
			throw(PluginException(ERROR_FATAL,"Stopping due to %d errors remapping vertex bones",numErrors));
		}
	}
}

void ProcessTup::PropagateClothModifiers(void)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::DERIVED)
		{
			TupDerived tupDerived(pObjectAgg);
			int totalModifierSlots = tupDerived.GetNumModifierSlots();
			// if there is any modifiers
			if (totalModifierSlots)
			{
				bool foundValidModifier = false;
				// loop through the modifiers and see if any deal with the cloth modifiers
				for (int modifierIndex=0;modifierIndex<totalModifierSlots;modifierIndex++)
				{
					TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
					if (pDerivedModifierAgg)
					{
						TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
						int modifierReference;
						tupDerivedModifier.GetModifierReference(modifierReference);
						TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierReference);
						TupModifier tupModifier(pModifierAgg);
						switch(tupModifier.GetModifierType())
						{
								case TupModifier::CLOTH_MODIFIER:
								{
									TupClothModifier tupClothModifier(pModifierAgg);

									TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
									TupClothModifierContext tupClothModifierContext(pModifierContextAgg);

									int flags;
									tupClothModifier.GetFlags(flags);
									if (flags&TupClothModifier::FLAGS_ENABLED)
									{
										TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectIndex);
										// do only when we have a base mesh
										if (pBaseMeshAgg)
										{
											TupperwareAggregate *pClothMeshAgg = pBaseMeshAgg->FindAggregateByKey("Annotate_ClothMeshData");
											// see if there is already one on the mesh
											if (!pClothMeshAgg)
											{
												TupTriMesh tupTriMesh(pBaseMeshAgg);
												Vector3 *pPositionVerts;
												int numVerts;
												tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numVerts);
												if (numVerts)
												{
													int numPoints;
													int *pPointFlags;
													int numConnectionPoints;
													int *pConnectionPoints;
													tupClothModifierContext.GetPointFlags(&pPointFlags,numPoints);
													if (numPoints!=numVerts)
													{
														PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Cloth Modifier reference incorrect number of points.  Is %d. Should be %d.",numPoints,numVerts);
														continue;
													}
													tupClothModifierContext.GetConnectionPoints(&pConnectionPoints,numConnectionPoints);
													// add main aggregate to mesh
													pClothMeshAgg = pBaseMeshAgg->AddAggregate("Annotate_ClothMeshData");
													// add point flags
													pClothMeshAgg->AddListInt("PointFlags",NULL,pPointFlags,numPoints);
													// add connection points
													pClothMeshAgg->AddListInt("ConnectionPoints",NULL,pConnectionPoints,numConnectionPoints<<1);
													foundValidModifier = true;
												}
											}
										}
									}
								}
								break;
						}
						// stop searching if there is one
						if (foundValidModifier)
							break;
					}
				}
			}
		}
	}
}

// sets a flag in the face of the mesh indicating if the face is used in the cloth or not
void ProcessTup::AddClothMeshFlags(void)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if empty slot - continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			// if there is a cloth mesh then make another one
			TupperwareAggregate *pClothMeshAgg = pObjectAgg->FindAggregateByKey("Annotate_ClothMeshData");
			if (pClothMeshAgg)
			{
				TupTriMesh tupTriMesh(pObjectAgg);
				int *pPointFlags;
				TupperwareList *pPointFlagsList = pClothMeshAgg->FindListByKey("PointFlags");
				assert(pPointFlagsList);
				pPointFlags = pPointFlagsList->GetAsInt();
				int numPointFlags = pPointFlagsList->GetLength();

				int numPositionVerts;
				int numPositionFaces;
				Vector3 *pPositionVerts;
				int *pPositionIndices;
				tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);
				tupTriMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);
				int *pFaceFlags;
				int numFaces;
				tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);
				assert(numPositionVerts==numPointFlags);
				assert(numPositionFaces==numFaces);

				// get the connections
				TupperwareList *pPointConnectionsList = pClothMeshAgg->FindListByKey("ConnectionPoints");
				assert(pPointConnectionsList);
				int *pPointConnections = pPointConnectionsList->GetAsInt();
				int numPointConnections = pPointConnectionsList->GetLength();
				assert((numPointConnections&1)==0); // make sure it is even
				numPointConnections = numPointConnections>>1;

				int numNonClothFaces = 0;
				int numClothFaces = 0;
				int faceIndex,vertIndex,facePointIndex;
				// set the flag indicating if the face is using cloth (all 3 points need to be active)
				for (faceIndex=0;faceIndex<numFaces;faceIndex++)
				{
					// count the number of active points
					int numActivePoints = 0;
					for (facePointIndex=0;facePointIndex<3;facePointIndex++)
					{
						int positionIndex = pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex];
						if (pPointFlags[positionIndex] & TupClothModifierContext::FLAGS_POINT_ACTIVE)
							numActivePoints++;
					}
					// if all 3 points are active then we have a cloth face
					if (numActivePoints == 3)
					{
						pFaceFlags[faceIndex] |= TupTriMesh::FLAGS_FACE_USES_CLOTH;
						numClothFaces++;
					}
					else
					{
						// keep track of this value to know if we need to save a parent mesh
						numNonClothFaces++;
					}
				}
				int numClothSOMs = 0;
				// make sure there are some cloth faces
				if (numClothFaces)
				{
					// construct an object to the hold the face connectivity data
					TupTriMeshConnectivity tupTriMeshConnectivity;
					tupTriMeshConnectivity.SetMeshFaceFlagMask(TupTriMesh::FLAGS_FACE_USES_CLOTH);
					tupTriMeshConnectivity.SetMesh(tupTriMesh,TupTriMeshConnectivity::FLAGS_FACE_CHECK_FACE_FLAGS);

					TupArray<int> faceClothSOMIndices;
					faceClothSOMIndices.SetSize(numFaces);
					TupArray<int> vertClothSOMIndices;
					vertClothSOMIndices.SetSize(numPositionVerts);

					// init all faces to not be assigned to a cloth SOM
					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						faceClothSOMIndices[faceIndex] = -1; // all are not using cloth for now
					}
					// init all verts to not be assigned to a cloth SOM
					for (vertIndex=0;vertIndex<numPositionVerts;vertIndex++)
					{
						vertClothSOMIndices[vertIndex] = -1; // all are not using cloth for now
					}
					// loop through the faces and set each face to point to the approprate SOM index
					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						// find a face that has not been set but is using cloth
						if ((faceClothSOMIndices[faceIndex] == -1) && 
							(pFaceFlags[faceIndex] & TupTriMesh::FLAGS_FACE_USES_CLOTH))
						{
							Keeper<int> connectedFaces;
							// get all the faces I am connected to (including the seed face)
							tupTriMeshConnectivity.AddAllConnectedFaces(faceIndex,connectedFaces);
							// go through the connected faces and see if any of the points are set to a valid index
							int foundClothSOMIndex = -1;
							int connectedFaceIndex;
							for (connectedFaceIndex=0;connectedFaceIndex<connectedFaces.GetSize();connectedFaceIndex++)
							{
								int connectedFace = connectedFaces[connectedFaceIndex];
								// look up in the pointClothSOMIndices
								for (facePointIndex=0;facePointIndex<3;facePointIndex++)
								{
									int positionIndex = pPositionIndices[(connectedFace<<1)+connectedFace+facePointIndex];
									int vertClothSOMIndex = vertClothSOMIndices[positionIndex];
									// if valid
									if (vertClothSOMIndex!=-1)
									{
										if (foundClothSOMIndex==-1)
										{
											foundClothSOMIndex = vertClothSOMIndex;
										}
										else
										{
											// if already set make sure these are the same
											assert(foundClothSOMIndex==vertClothSOMIndex);
										}
									}
								}
							}
							// if we didn't find one to use
							if (foundClothSOMIndex==-1)
							{
								foundClothSOMIndex = numClothSOMs; // make a new index
								numClothSOMs++; // one more SOM
							}
							// now mark the faces and the points
							for (connectedFaceIndex=0;connectedFaceIndex<connectedFaces.GetSize();connectedFaceIndex++)
							{
								int connectedFace = connectedFaces[connectedFaceIndex];
								// set to an index where they will share the same SOM
								faceClothSOMIndices[connectedFace] = foundClothSOMIndex;

								// mark the points
								for (facePointIndex=0;facePointIndex<3;facePointIndex++)
								{
									int positionIndex = pPositionIndices[(connectedFace<<1)+connectedFace+facePointIndex];
									vertClothSOMIndices[positionIndex] = foundClothSOMIndex;
								}
							}
							// now loop through the connections and set the other end if one is not -1
							for (int connectionIndex=0;connectionIndex<numPointConnections;connectionIndex++)
							{
								int connectionA = pPointConnections[connectionIndex<<1];
								int connectionB = pPointConnections[(connectionIndex<<1)+1];
								int &connectionASOMIndex = vertClothSOMIndices[connectionA];
								int &connectionBSOMIndex = vertClothSOMIndices[connectionB];

								// only worry when they are not equal
								if (connectionASOMIndex != connectionBSOMIndex)
								{
									if (connectionASOMIndex == -1)
									{
										// b is set so put on a
										connectionASOMIndex = connectionBSOMIndex;
									}
									else if (connectionBSOMIndex == -1)
									{
										// a is set so put on b
										connectionBSOMIndex = connectionASOMIndex;
									}
									else
									{
										assert(false); // this should not happen
									}

								}
							}
						}
					}
					pClothMeshAgg->AddListInt("FaceClothSOMIndices",NULL,faceClothSOMIndices);
					pClothMeshAgg->AddListInt("VertClothSOMIndices",NULL,vertClothSOMIndices);
				}
				pClothMeshAgg->AddScalarInt("NumClothSOMs",NULL,numClothSOMs);
				pClothMeshAgg->AddScalarInt("NumNonClothFaces",NULL,numNonClothFaces);
			}
		}
	}
}


void ProcessTup::AddNodeAnimatedTextureInfo(void)
{
	int totalImages = m_pTupImagePool->GetNumImages();

	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);

		int animatedTextureID = -1;
		float animatedTextureSpeed = 1.0f;

		int materialRef;
		tupNode.GetMaterialReference(materialRef);

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		if (objectRef!=-1)
		{
			TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
			if (pBaseMeshAgg)
			{
				TupperwareList *pAnimatedTextureIDList = pBaseMeshAgg->FindListByKey("Annotate_AnimatedTextureIDs");
				if (pAnimatedTextureIDList)
				{
					TupperwareScalar *pRenderPartitionIndexScalar = pNodeAgg->FindScalarByKey("Annotate_RenderPartitionIndex");
					if (pRenderPartitionIndexScalar)
					{
						int visibilitySet = pRenderPartitionIndexScalar->GetAsInt();
						animatedTextureID = (pAnimatedTextureIDList->GetAsInt())[visibilitySet];
					}
				}
				TupperwareScalar *pAnimatedTextureSpeedScalar = pBaseMeshAgg->FindScalarByKey("Annotate_AnimatedTextureSpeed");
				if (pAnimatedTextureSpeedScalar)
				{
					animatedTextureSpeed = pAnimatedTextureSpeedScalar->GetAsFloat();
				}
			}
		}
		pNodeAgg->AddScalarInt("Annotate_AnimatedTextureID",NULL,animatedTextureID);
		pNodeAgg->AddScalarFloat("Annotate_AnimatedTextureSpeed",NULL,animatedTextureSpeed);
	}
}

void ProcessTup::AddMeshAnimatedTextureInfo(void)
{
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			int materialRef = pObjectAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
			TupTriMesh tupTriMesh(pObjectAgg);

			int imageRef = -1;
			float animationSpeed = 1.0f;

			if (materialRef!=-1)
			{
				AddMeshAnimatedTextureInfo(tupTriMesh,imageRef,animationSpeed,materialRef);
			}
			int textureID=-1;
			int numVisiblitySets = m_visibilitySetNodeRefs.GetSize();
			TupArray<int> animatedTextureIDs;
			animatedTextureIDs.SetSize(numVisiblitySets);

			if (imageRef!=-1)
			{
				TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageRef);
				assert(pImageAgg);
				// make this a texture context if it is being used
				TupperwareList *pTextureIDList = pImageAgg->FindListByKey("Annotate_TextureIDs");
				if (pTextureIDList)
				{
					pObjectAgg->AddListInt("Annotate_AnimatedTextureIDs",NULL,pTextureIDList->GetAsInt(),pTextureIDList->GetLength());
					pObjectAgg->AddScalarFloat("Annotate_AnimatedTextureSpeed",NULL,animationSpeed);
				}
			}
		}
	}
}

void ProcessTup::AddMeshAnimatedTextureInfo(TupTriMesh &tupTriMesh,int &imageRef,float &animationSpeed,int materialRef)
{
	int *pFaceMaterialIndices;
	int numFaces;
	tupTriMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,numFaces);

	TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
	assert(pMaterialAgg);
	TupMaterial tupMaterial(pMaterialAgg);

	if ( tupMaterial.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
	{
		// we need to deal with multiple materials
		TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMaterial);
		int totalSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
		if (totalSubMaterials)
		{
			// keep track of which materials that were actually used on the mesh
			TupArray<int> usedMaterials;
			usedMaterials.SetSize(totalSubMaterials);
			int materialIndex;
			for (materialIndex = 0;materialIndex<totalSubMaterials;materialIndex++)
			{
				usedMaterials[materialIndex] = 0;
			}
			for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int faceMaterialIndex = pFaceMaterialIndices[faceIndex];
				assert(faceMaterialIndex<totalSubMaterials);
				usedMaterials[faceMaterialIndex]++;
			}
			for (materialIndex = 0;materialIndex<totalSubMaterials;materialIndex++)
			{
				// only look at used ones
				if (usedMaterials[materialIndex])
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int materialPoolIndex;
					tupSubMaterial.GetMaterialReference(materialPoolIndex);
					if (materialPoolIndex!=-1)
					{
						TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial( materialPoolIndex );
						TupMaterial tupSubMat( pMaterialAgg );
						if (tupSubMat.GetMaterialType()==TupMaterial::STANDARD_MATERIAL)
						{
							AddMeshAnimatedTextureInfoStandard(imageRef,animationSpeed,materialPoolIndex);
						}
					}
				}
			}
		}
	}
	else if ( tupMaterial.GetMaterialType( ) == TupMaterial::STANDARD_MATERIAL )
	{
		AddMeshAnimatedTextureInfoStandard(imageRef,animationSpeed,materialRef);
	}
}

void ProcessTup::AddMeshAnimatedTextureInfoStandard(int &imageRef,float &animationSpeed,int materialRef)
{
	TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
	assert(pMaterialAgg);
	TupMaterial tupMaterial(pMaterialAgg);

	int totalImages = m_pTupImagePool->GetNumImages();
	assert(tupMaterial.GetMaterialType() == TupMaterial::STANDARD_MATERIAL);
	MaterialImageRefsManager materialImageRefsManager(*m_pTupMaterialPool,*m_pTupMapPool);

	TupArray<MaterialImageRefsManager::MaterialImageData> materialImageRefs;
	materialImageRefsManager.BuildImageRefs(materialRef,materialImageRefs);

	for (int imageIndex=0;imageIndex<materialImageRefs.GetSize();imageIndex++)
	{
		int mapReference = materialImageRefs[imageIndex].m_bitmapMapRef;
		TupperwareAggregate *pBitmapMapAgg = m_pTupMapPool->GetMap(mapReference);
		assert(pBitmapMapAgg);
		TupBitmapMap tupBitmapMap(pBitmapMapAgg);

		int imageReference = materialImageRefs[imageIndex].m_imageRef;
		assert(imageReference<totalImages);
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageReference);
		TupImage tupImage(pImageAgg);
		char *pImageFileName;
		// get filename for image
		tupImage.GetFilePathName(&pImageFileName);
		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
		assert(pImageDataAgg);
		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		// if this is animated
		if (numFrames>1)
		{
			if (imageRef==-1)
			{
				tupBitmapMap.GetPlaybackRate(animationSpeed);
				imageRef = imageReference;
			}
			else
			{
				// we hit another one
			}
		}
	}
}

void ProcessTup::ForceBonesToAttachPoints(void)
{
	if (!m_bForceBonesToAttachPoints)
		return;
	int numNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;
		TupNode tupNode(pNodeAgg);
		int nodeObjectRef;
		tupNode.GetObjectReference(nodeObjectRef);

		if (nodeObjectRef!=-1)
		{
			int baseObjectRef = AnnotateHelper::FindBaseObjectRef(*m_pTupObjectPool,nodeObjectRef);
			TupperwareAggregate *pBaseObjectAgg = m_pTupObjectPool->GetObjectAggregate(baseObjectRef);
			TupObject tupObject(pBaseObjectAgg);
			if (tupObject.GetObjectType() == TupObject::BONE)
			{
				char *pName;
				tupNode.GetName(&pName);
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Forcing Bone %s to be an attach point",pName);
				pNodeAgg->AddScalarInt("Annotate_ForceBoneToAttachPoint",NULL,1);
			}
		}
	}
}


// go through and make sure all the modifiers conform to the meshes beneath them
void ProcessTup::FixRogueModifiers(void)
{
	Keeper<int> modifiersToDelete;
	Keeper<int> derivedObjectsToDelete;

	int numObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
		if (!pObjectAgg)
			continue;
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::DERIVED)
		{
			TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectIndex);
			if (!pBaseMeshAgg)
				continue;
			TupObject tupBaseMeshObject(pBaseMeshAgg);
			assert(tupBaseMeshObject.GetObjectType() == TupObject::TRI_MESH); // this better be a trimesh
			TupTriMesh tupBaseMesh(pBaseMeshAgg);

			TupDerived tupDerived(pObjectAgg);
			int numModifierSlots = tupDerived.GetNumModifierSlots();
			// call on all the modifiers
			for (int modifierIndex=0;modifierIndex<numModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (pDerivedModifierAgg)
				{
					bool bDeleteContext = false;
					TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
					int modifierReference;
					tupDerivedModifier.GetModifierReference(modifierReference);
					// if this modifier is not set to be deleted
					if (modifiersToDelete.GetIndex(modifierReference)==-1)
					{
						TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
						bool bResult = FixRogueModifier(modifierReference,pModifierContextAgg,tupBaseMesh);
						// if the fix routine does not return true then delete it
						if (!bResult)
						{
							// add to keeper to delete later
							modifiersToDelete.Add(modifierReference);
							bDeleteContext = true;
						}
					}
					else
					{
						bDeleteContext = true;
					}

					// delete context
					if (bDeleteContext)
					{
						pDerivedModifierAgg->GetParent()->DeleteAtom(pDerivedModifierAgg);
					}
				}
			}
			// any empty derived objects need to get deleted
			numModifierSlots = tupDerived.GetNumModifierSlots();
			if (numModifierSlots==0)
			{
				derivedObjectsToDelete.Add(objectIndex);
			}
		}
	}
	for (int deleteModifierIndex = 0; deleteModifierIndex < modifiersToDelete.GetSize(); deleteModifierIndex++)
	{
		m_pTupModifierPool->DeleteModifier(modifiersToDelete[deleteModifierIndex]);
	}
	if (derivedObjectsToDelete.GetSize())
	{
		// setup replacement references
		TupArray<int> replaceObjectReferences;
		replaceObjectReferences.SetSize(derivedObjectsToDelete.GetSize());
		for (int derivedObjectIndex = 0; derivedObjectIndex < derivedObjectsToDelete.GetSize(); derivedObjectIndex++)
		{
			// scan through until we find an object that is not going to be deleted
			int nextObjectReference = derivedObjectsToDelete[derivedObjectIndex];
			do 
			{
				TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(nextObjectReference);
				assert(pObjectAgg);
				TupObject tupObject(pObjectAgg);
				if (tupObject.GetObjectType()==TupObject::DERIVED)
				{
					TupDerived tupDerived(pObjectAgg);
					tupDerived.GetObjectReference(nextObjectReference);
				}
			}
			while(derivedObjectsToDelete.GetIndex(nextObjectReference)!=-1);

			// set up remap (from deleted derived to valid object in chain)
			replaceObjectReferences[derivedObjectIndex] = nextObjectReference;
		}
		// remap any derived objects that are linking to a marked object to point to the remapped reference
		for (int objectIndex=0;objectIndex<numObjects;objectIndex++)
		{
			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
			if (!pObjectAgg)
				continue;
			TupObject tupObject(pObjectAgg);

			if (tupObject.GetObjectType()==TupObject::DERIVED)
			{
				// if this object is not one to delete
				if (derivedObjectsToDelete.GetIndex(objectIndex)==-1)
				{
					TupDerived tupDerived(pObjectAgg);
					int nextObjectReference;
					tupDerived.GetObjectReference(nextObjectReference);
					// if the one I am linking to is going away
					int nextObjectIndex = derivedObjectsToDelete.GetIndex(nextObjectReference);
					if (nextObjectIndex!=-1)
					{
						// replace reference to next safe one in the chain
						tupDerived.AddObjectReference(replaceObjectReferences[nextObjectIndex]);
					}
				}
				else
				{
					// go ahead and remove at this time
					m_pTupObjectPool->DeleteObjectAggregate(objectIndex);
				}
			}
		}
		// remap any node references that point to any derived objects that are deleted
		int numNodes = m_pTupNodePool->GetNumNodes();
		for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
		{
			TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
			if (!pNodeAgg)
				continue;
			TupNode tupNode(pNodeAgg);
			int objectReference;
			tupNode.GetObjectReference(objectReference);
			// if the one I am linking to is gone
			int objectIndex = derivedObjectsToDelete.GetIndex(objectReference);
			if (objectIndex!=-1)
			{
				// replace reference to next safe one in the chain
				tupNode.AddObjectReference(replaceObjectReferences[objectIndex]);
			}
		}
	}
}

bool ProcessTup::FixRogueModifier(int modifierRef,TupperwareAggregate *pModifierContextAgg,TupTriMesh &tupBaseMesh)
{
	TupperwareAggregate *pTupModifierAgg = m_pTupModifierPool->GetModifier(modifierRef);
	assert(pTupModifierAgg);
	TupModifier tupModifier(pTupModifierAgg);

	switch(tupModifier.GetModifierType())
	{
		case TupModifier::APPLY_VC_MODIFIER:
			return FixRogueApplyVCModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		case TupModifier::VERTEX_PAINT_MODIFIER:
			return FixRogueVertexPaintModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		case TupModifier::HOLD_VC_MODIFIER:
			return FixRogueHoldVCModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		case TupModifier::FACE_ATTRIBUTE_MODIFIER:
			return FixRogueFaceAttributeModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		case TupModifier::SKIN_MODIFIER:
			return FixRogueSkinModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		case TupModifier::CLOTH_MODIFIER:
			return FixRogueClothModifier(pModifierContextAgg,tupModifier,tupBaseMesh);
		default:
			assert(false);
	}
	return true;
}

bool ProcessTup::FixRogueApplyVCModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupApplyVCModifier tupApplyVCModifier(tupModifier);
	int *pColorIndices;
	int numColorIndices;
	tupApplyVCModifier.GetColorIndices(&pColorIndices,numColorIndices);
	int flags;
	tupApplyVCModifier.GetFlags(flags);

	int numFaces;
	int *pPositionIndices;
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);

	Vector3 *pModifierVerts;
	int numModifierVerts;
	tupApplyVCModifier.GetColorVerts((float **)&pModifierVerts,numModifierVerts);
	if (numModifierVerts==0)
	{
		float whiteColor[3] = {1.0f,1.0f,1.0f};
		tupApplyVCModifier.AddColorVerts(whiteColor,1);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Fixed Rogue Apply VC Modifier - Reference %s - No Colors used - Defaulting to white color",
			tupModifier.GetBaseAggregate()->GetName());
	}

	int *pAlphaIndices = NULL;
	int numAlphaIndices = 0;
	float *pModifierAlphaVerts = NULL;
	int numModifierAlphaVerts = 0;
	if (flags & TupApplyVCModifier::FLAGS_HAS_ALPHA)
	{
		tupApplyVCModifier.GetAlphaIndices(&pAlphaIndices,numAlphaIndices);
		tupApplyVCModifier.GetAlphaVerts((float **)&pModifierAlphaVerts,numModifierAlphaVerts);
		if (numModifierAlphaVerts==0)
		{
			float opaque = 1.0f;
			tupApplyVCModifier.AddAlphaVerts(&opaque,1);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Fixed Rogue Apply VC Modifier - Reference %s - No Alphas used - Defaulting to opaque",
				tupModifier.GetBaseAggregate()->GetName());
		}
	}

	// if this flag is set the indices are in sync with the position verts
	if (flags & TupApplyVCModifier::FLAGS_MIXED_COLORS)
	{
		int numPositionVerts;
		Vector3 *pPositionVerts;
		tupBaseMesh.GetPositionVerts((float **) &pPositionVerts,numPositionVerts);
		// if these are not the same then fix
		if (numColorIndices!=numPositionVerts)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Fixed Rogue Apply VC Modifier - Reference %s Color Using %d Points Needs to Use %d Points",
				tupModifier.GetBaseAggregate()->GetName(),numColorIndices,numPositionVerts);

			TupArray<int> newColorIndices;
			newColorIndices.SetSize(numPositionVerts);
			for (int posVertIndex=0;posVertIndex<numPositionVerts;posVertIndex++)
			{
				if (posVertIndex<numColorIndices)
				{
					newColorIndices[posVertIndex] = pColorIndices[posVertIndex];
				}
				else
				{
					newColorIndices[posVertIndex] = 0; // use index 0 for invalid entry
				}
			}
			tupApplyVCModifier.AddColorIndices(newColorIndices.GetData(),numPositionVerts);
		}
		if (pAlphaIndices && numAlphaIndices!=numPositionVerts)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Fixed Rogue Apply VC Modifier - Reference %s Alpha Using %d Points Needs to Use %d Points",
				tupModifier.GetBaseAggregate()->GetName(),numAlphaIndices,numPositionVerts);
			
			TupArray<int> newAlphaIndices;
			newAlphaIndices.SetSize(numPositionVerts);
			for (int posVertIndex=0;posVertIndex<numPositionVerts;posVertIndex++)
			{
				if (posVertIndex<numAlphaIndices)
				{
					newAlphaIndices[posVertIndex] = pAlphaIndices[posVertIndex];
				}
				else
				{
					newAlphaIndices[posVertIndex] = 0; // use index 0 for invalid entry
				}
			}
			tupApplyVCModifier.AddAlphaIndices(newAlphaIndices.GetData(),numPositionVerts);
		}
	}
	// indices are per point per face
	else
	{
		// if these are not in line
		if (numColorIndices!=(numFaces<<1)+numFaces)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Fixed Rogue Apply VC Modifier - Reference %s Color Using %d Points Needs to Use %d Points",
				tupModifier.GetBaseAggregate()->GetName(),numColorIndices,(numFaces<<1)+numFaces);

			TupArray<int> newColorIndices;
			newColorIndices.SetSize((numFaces<<1)+numFaces);

			// fill the array with valid values
			for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int baseFaceIndex = (faceIndex<<1)+faceIndex;

				for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
				{
					if (baseFaceIndex+facePointIndex<numColorIndices)
					{
						newColorIndices[baseFaceIndex+facePointIndex] = pColorIndices[baseFaceIndex+facePointIndex];
					}
					else
					{
						newColorIndices[baseFaceIndex+facePointIndex] = 0; // use index 0 if not enough entries
					}
				}
			}
			tupApplyVCModifier.AddColorIndices(newColorIndices.GetData(),(numFaces<<1)+numFaces);
		}
		if (pAlphaIndices && numAlphaIndices!=(numFaces<<1)+numFaces)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Fixed Rogue Apply VC Modifier - Reference %s Alpha Using %d Points Needs to Use %d Points",
				tupModifier.GetBaseAggregate()->GetName(),numAlphaIndices,(numFaces<<1)+numFaces);
			
			TupArray<int> newAlphaIndices;
			newAlphaIndices.SetSize((numFaces<<1)+numFaces);
			
			// fill the array with valid values
			for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int baseFaceIndex = (faceIndex<<1)+faceIndex;
				
				for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
				{
					if (baseFaceIndex+facePointIndex<numAlphaIndices)
					{
						newAlphaIndices[baseFaceIndex+facePointIndex] = pAlphaIndices[baseFaceIndex+facePointIndex];
					}
					else
					{
						newAlphaIndices[baseFaceIndex+facePointIndex] = 0; // use index 0 if not enough entries
					}
				}
			}
			tupApplyVCModifier.AddAlphaIndices(newAlphaIndices.GetData(),(numFaces<<1)+numFaces);
		}
	}
	return true;
}

bool ProcessTup::FixRogueVertexPaintModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupVertexPaintModifierContext tupVertexPaintModifierContext(pTupModifierContext);
	Vector3 *pModifierColorData;
	float *pModifierMixFactorData;
	int *pModifierFaceIndexData;
	int *pModifierPointIndexData;
	int numModifierColors;
	for (int channel = 0;channel<3;channel++)
	{
		switch (channel)
		{
			case 0:
				tupVertexPaintModifierContext.GetVertexColorChannelColorData((float **)&pModifierColorData,numModifierColors);
				tupVertexPaintModifierContext.GetVertexColorChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
				tupVertexPaintModifierContext.GetVertexColorChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
				tupVertexPaintModifierContext.GetVertexColorChannelPointIndexData(&pModifierPointIndexData,numModifierColors);
				break;
			case 1:
				tupVertexPaintModifierContext.GetAlphaChannelColorData((float **)&pModifierColorData,numModifierColors);
				tupVertexPaintModifierContext.GetAlphaChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
				tupVertexPaintModifierContext.GetAlphaChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
				tupVertexPaintModifierContext.GetAlphaChannelPointIndexData(&pModifierPointIndexData,numModifierColors);
				break;
			case 2:
				tupVertexPaintModifierContext.GetIlluminationChannelColorData((float **)&pModifierColorData,numModifierColors);
				tupVertexPaintModifierContext.GetIlluminationChannelMixFactorData(&pModifierMixFactorData,numModifierColors);
				tupVertexPaintModifierContext.GetIlluminationChannelFaceIndexData(&pModifierFaceIndexData,numModifierColors);
				tupVertexPaintModifierContext.GetIlluminationChannelPointIndexData(&pModifierPointIndexData,numModifierColors);
				break;
		}
		if (numModifierColors)
		{
			int numFaces;
			int *pPositionIndices;
			tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);
			// add only valid entries
			TupArray<Vector3> newColorArray;
			TupArray<float> newMixFactorArray;
			TupArray<int> newFaceIndexArray;
			TupArray<int> newPointIndexArray;

			int maxFaceIndex = 0;
			int maxPointIndex = 0;
			bool bFoundBadEntry = false;

			// fill the arrays with entries that are valid
			for (int colorIndex = 0;colorIndex<numModifierColors;colorIndex++)
			{
				Vector3 &color = pModifierColorData[colorIndex];
				float mixFactor = pModifierMixFactorData[colorIndex];
				int faceIndex = pModifierFaceIndexData[colorIndex];
				int pointIndex = pModifierPointIndexData[colorIndex];
				if (faceIndex<numFaces && pointIndex>=0 && pointIndex<=2)
				{
					newColorArray.Add(color);
					newMixFactorArray.Add(mixFactor);
					newFaceIndexArray.Add(faceIndex);
					newPointIndexArray.Add(pointIndex);
				}
				else
				{
					bFoundBadEntry = true;
					if (maxFaceIndex<faceIndex)
					{
						maxFaceIndex = faceIndex;
					}
					if (maxPointIndex<pointIndex)
					{
						maxPointIndex=pointIndex;
					}
				}
			}
			if (bFoundBadEntry)
			{
				switch (channel)
				{
					case 0:
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
							"Fixed Rogue Vertex Paint Modifier (Color Channel) - Reference %s Using %d Faces & %d Points Needs to Use %d Faces & 0-2 Points",
							tupModifier.GetBaseAggregate()->GetName(),maxFaceIndex,maxPointIndex,numFaces);

						tupVertexPaintModifierContext.AddVertexColorChannelColorData((float *)newColorArray.GetData(),newColorArray.GetSize());
						tupVertexPaintModifierContext.AddVertexColorChannelMixFactorData(newMixFactorArray.GetData(),newMixFactorArray.GetSize());
						tupVertexPaintModifierContext.AddVertexColorChannelFaceIndexData(newFaceIndexArray.GetData(),newFaceIndexArray.GetSize());
						tupVertexPaintModifierContext.AddVertexColorChannelPointIndexData(newPointIndexArray.GetData(),newPointIndexArray.GetSize());
						break;
					case 1:
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
							"Fixed Rogue Vertex Paint Modifier (Alpha Channel) - Reference %s Using %d Faces & %d Points Needs to Use %d Faces & 0-2 Points",
							tupModifier.GetBaseAggregate()->GetName(),maxFaceIndex,maxPointIndex,numFaces);

						tupVertexPaintModifierContext.AddAlphaChannelColorData((float *)newColorArray.GetData(),newColorArray.GetSize());
						tupVertexPaintModifierContext.AddAlphaChannelMixFactorData(newMixFactorArray.GetData(),newMixFactorArray.GetSize());
						tupVertexPaintModifierContext.AddAlphaChannelFaceIndexData(newFaceIndexArray.GetData(),newFaceIndexArray.GetSize());
						tupVertexPaintModifierContext.AddAlphaChannelPointIndexData(newPointIndexArray.GetData(),newPointIndexArray.GetSize());
						break;
					case 2:
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
							"Fixed Rogue Vertex Paint Modifier (Illumination Channel) - Reference %s Using %d Faces & %d Points Needs to Use %d Faces & 0-2 Points",
							tupModifier.GetBaseAggregate()->GetName(),maxFaceIndex,maxPointIndex,numFaces);

						tupVertexPaintModifierContext.AddIlluminationChannelColorData((float *)newColorArray.GetData(),newColorArray.GetSize());
						tupVertexPaintModifierContext.AddIlluminationChannelMixFactorData(newMixFactorArray.GetData(),newMixFactorArray.GetSize());
						tupVertexPaintModifierContext.AddIlluminationChannelFaceIndexData(newFaceIndexArray.GetData(),newFaceIndexArray.GetSize());
						tupVertexPaintModifierContext.AddIlluminationChannelPointIndexData(newPointIndexArray.GetData(),newPointIndexArray.GetSize());
						break;
				}
			}
		}
	}
	return true;
}

bool ProcessTup::FixRogueHoldVCModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupHoldVCModifier tupHoldVCModifier(tupModifier);
	int *pColorIndices;
	int numColorIndices;
	tupHoldVCModifier.GetColorIndices(&pColorIndices,numColorIndices);

	int numFaces;
	int *pPositionIndices;
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);

	// if these are not in line
	if (numColorIndices!=(numFaces<<1)+numFaces)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Fixed Rogue Hold VC Modifier - Reference %s Using %d Indices Needs to Use %d Indices",
			tupModifier.GetBaseAggregate()->GetName(),numColorIndices,(numFaces<<1)+numFaces);

		TupArray<int> newColorIndices;
		newColorIndices.SetSize((numFaces<<1)+numFaces);

		// fill the array with valid values
		for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			int baseFaceIndex = (faceIndex<<1)+faceIndex;

			for (int facePointIndex=0;facePointIndex<3;facePointIndex++)
			{
				if (baseFaceIndex+facePointIndex<numColorIndices)
				{
					newColorIndices[baseFaceIndex+facePointIndex] = pColorIndices[baseFaceIndex+facePointIndex];
				}
				else
				{
					newColorIndices[baseFaceIndex+facePointIndex] = 0; // use index 0 if not enough entries
				}
			}
		}
		tupHoldVCModifier.AddColorIndices(newColorIndices.GetData(),(numFaces<<1)+numFaces);
	}
	return true;
}

bool ProcessTup::FixRogueFaceAttributeModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupFaceAttributesModifierContext tupFaceAttributesModifierContext(pTupModifierContext);
	int *pAttributeIndices;
	int numAttributeIndices;
	tupFaceAttributesModifierContext.GetFaceAttributeIndices(&pAttributeIndices,numAttributeIndices);

	int numFaces;
	int *pPositionIndices;
	tupBaseMesh.GetPositionIndices(&pPositionIndices,numFaces);

	// if these are not in line
	if (numAttributeIndices!=numFaces)
	{

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Fixed Rogue Face Attribute Modifier - Reference %s Using %d Faces Needs to Use %d Faces",
			tupModifier.GetBaseAggregate()->GetName(),numAttributeIndices,numFaces);

		TupArray<int> newAttributeIndices;
		newAttributeIndices.SetSize(numFaces);

		// fill the array with valid values
		for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			if (faceIndex<numAttributeIndices)
			{
				newAttributeIndices[faceIndex] = pAttributeIndices[faceIndex];
			}
			else
			{
				newAttributeIndices[faceIndex] = 0; // use index 0 if not enough entries
			}
		}
		tupFaceAttributesModifierContext.AddFaceAttributeIndices(newAttributeIndices.GetData(),numFaces);
	}
	return true;
}

bool ProcessTup::FixRogueSkinModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupSkinModifier tupSkinModifier(tupModifier);
	int numBones = tupSkinModifier.GetNumBones();
	for (int boneIndex = 0; boneIndex < numBones; boneIndex++)
	{
		TupperwareAggregate *pBoneAgg = tupSkinModifier.GetBone(boneIndex);
		TupSkinModifierBone tupSkinModifierBone(pBoneAgg);
		int nodeReference;
		tupSkinModifierBone.GetNodeReference(nodeReference);

		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeReference);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		//-- check to see if instance_type is visibility_region
		TupNode tupNode( pNodeAgg );
		char *pNodeAttributesString;
		tupNode.GetUserProperties( &pNodeAttributesString );

		TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_OBJ_BLOCK, END_OBJ_BLOCK );
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey( "instance_type" );

		const int INSTANCE_TYPE = 0;
		m_pTupNodeConfigRoot->GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );

		TupperwareScalar *pForceBoneToAttachPointScalar = pNodeAgg->FindScalarByKey("Annotate_ForceBoneToAttachPoint");

		// check if any bones are marked as attach points
		if ( pForceBoneToAttachPointScalar || 
			(tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp( tupKeyValueContext.GetValueAsString(INSTANCE_TYPE), "attach_point" ) == 0 ))
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Deleting skin modifier due to attach point objects");

			// for now just say that the skin needs to go away
			return false;
		}
	}
	TupSkinModifierContext tupSkinModifierContext(pTupModifierContext);
	TupperwareAggregate *tupVertexPoolAgg = tupSkinModifierContext.GetVertexPool();
	TupSkinModifierContextVertexPool tupVertexPool(tupVertexPoolAgg);
	int numVertexPoolEntries = tupVertexPool.GetNumVertices();

	int numPositionVerts;
	Vector3 *pPositionVerts;
	tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);

	if (numVertexPoolEntries!=numPositionVerts)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Fixed Rogue Skin Modifier - Reference %s Using %d Points Needs to Use %d Points",
			tupModifier.GetBaseAggregate()->GetName(),numVertexPoolEntries,numPositionVerts);

		TupperwareAggregate *pNewTupVertexPoolAgg = new TupperwareAggregate;
		TupSkinModifierContextVertexPool tupNewVertexPool(pNewTupVertexPoolAgg);

		for (int positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
		{
			TupperwareAggregate *tupNewVertexPoolEntryAgg = tupNewVertexPool.AddVertex(positionVertIndex);
			if (positionVertIndex<numVertexPoolEntries)
			{
				TupperwareAggregate *tupVertexPoolEntryAgg = tupVertexPool.GetVertex(positionVertIndex);
				// copy stuff to new vertex
				for (TupperwareIterator iter = tupVertexPoolEntryAgg->FindAll(); !iter.Finished(); iter.Next()) 
				{
					tupNewVertexPoolEntryAgg->Copy(iter.GetAsAtom());
				}
			}
		}
		// replace data
		TupperwareAggregate *pAddedVertexPool = tupSkinModifierContext.AddVertexPool();
		for (TupperwareIterator iter = pNewTupVertexPoolAgg->FindAll(); !iter.Finished(); iter.Next()) 
		{
			pAddedVertexPool->Copy(iter.GetAsAtom());
		}
		delete pNewTupVertexPoolAgg;
	}
	return true;
}

bool ProcessTup::FixRogueClothModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh)
{
	TupClothModifierContext tupClothModifierContext(pTupModifierContext);

	int numPositionVerts;
	Vector3 *pPositionVerts;
	tupBaseMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);

	int *pClothPointFlags;
	int numClothPoints;
	tupClothModifierContext.GetPointFlags(&pClothPointFlags,numClothPoints);
	int *pConnections;
	int numConnections;
	tupClothModifierContext.GetConnectionPoints(&pConnections,numConnections);

	if (numClothPoints!=numPositionVerts)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Fixed Rogue Cloth Modifier - Reference %s Using %d Points Needs to Use %d Points",
			tupModifier.GetBaseAggregate()->GetName(),numClothPoints,numPositionVerts);
		TupArray<int> newClothPointFlags;
		newClothPointFlags.SetSize(numPositionVerts);
		TupArray<int> newConnections;

		for (int positionVertIndex=0;positionVertIndex<numPositionVerts;positionVertIndex++)
		{
			if (positionVertIndex<numClothPoints)
			{
				newClothPointFlags[positionVertIndex] = pClothPointFlags[positionVertIndex];
			}
			else
			{
				newClothPointFlags[positionVertIndex] = TupClothModifierContext::FLAGS_POINT_ACTIVE | TupClothModifierContext::FLAGS_POINT_ATTACHED;
			}
		}

		for (int connectionIndex=0;connectionIndex<numConnections;connectionIndex++)
		{
			// only copy valid connections
			if (pConnections[connectionIndex<<1]<positionVertIndex && pConnections[(connectionIndex<<1)+1]<positionVertIndex)
			{
				newConnections.Add(pConnections[connectionIndex<<1]);
				newConnections.Add(pConnections[(connectionIndex<<1)+1]);
			}
		}
		tupClothModifierContext.AddPointFlags(newClothPointFlags.GetData(),numPositionVerts);
		tupClothModifierContext.AddConnectionPoints(newConnections.GetData(),newConnections.GetSize()>>1);
	}
	return true;
}

void ProcessTup::FixMeshMaterialIndices(void)
{
	// go through and change material list to material scalar
	int objectIndex;
	for (objectIndex=0;objectIndex<m_pTupObjectPool->GetNumObjects();objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if slot empty then continue
		if (!pObjectAgg)
			continue;

		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			TupTriMesh tupTriMesh(pObjectAgg);
			int *pMaterialIndices;
			int numFaces,faceIndex;
			tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,numFaces);

			int materialRef = pObjectAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
			if (materialRef!=-1)
			{
				TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
				TupMaterial tupMaterial(pMaterialAgg);
				// if material is a multi-sub object material
				if (tupMaterial.GetMaterialType()==TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
				{
					TupMultiSubObjectMaterial tupMultiSubMaterial(pMaterialAgg);
					int numSubMaterials = tupMultiSubMaterial.GetNumSubMaterials();
					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						if (pMaterialIndices[faceIndex]>=numSubMaterials)
						{
							pMaterialIndices[faceIndex] = 0; // set all that are out of range to 0
						}
					}
				}
				else
				{
					for (faceIndex=0;faceIndex<numFaces;faceIndex++)
					{
						pMaterialIndices[faceIndex] = 0; // set all to 0
					}
				}

			}
			else // no material used
			{
				for (faceIndex=0;faceIndex<numFaces;faceIndex++)
				{
					pMaterialIndices[faceIndex] = 0; // set all to 0
				}
			}
		}
	}
}

void ProcessTup::RemapDuplicateImages(void)
{
	int numImages = m_pTupImagePool->GetNumImages();
	TupArray<u32> CRCArray;
	CRCArray.SetSize(numImages);
	TupArray<bool> activeImageArray;
	activeImageArray.SetSize(numImages);
	int imageIndex;
	// set which ones are active and the CRC values for each
	for (imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
		{
			activeImageArray[imageIndex] = false;
		}
		else
		{
			activeImageArray[imageIndex] = true;

			TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
			TupImage tupImage(pImageAgg);
			char *pImageFileName;
			// get filename for image
			tupImage.GetFilePathName(&pImageFileName);
			// get image data from image manager
			ImageManager &imageManager = m_dataManager.GetImageManager();
			TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
			assert(pImageDataAgg);
			// get the first frame from the image data
			TupImageData tupImageData(pImageDataAgg);
			CRCArray[imageIndex] = ImageUtilities::GetCRC(tupImageData);
		}
	}
	TupArray<int> imageRemapArray;
	imageRemapArray.SetSize(numImages);
	int totalRemapped = 0;
	for (imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		imageRemapArray[imageIndex] = -1;
		if (!activeImageArray[imageIndex])
			continue;

		u32 CRCValue = CRCArray[imageIndex];
		for (int testImageIndex=0;testImageIndex<imageIndex;testImageIndex++)
		{
			if (!activeImageArray[testImageIndex])
				continue;

			u32 testCRCValue = CRCArray[testImageIndex];
			if (CRCValue == testCRCValue)
			{
				// re found a match
				imageRemapArray[imageIndex] = testImageIndex;

				TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
				TupImage tupImage(pImageAgg);
				char *pImageFileName;
				// get filename for one being remapped 
				tupImage.GetFilePathName(&pImageFileName);

				TupperwareAggregate *pNewImageAgg = m_pTupImagePool->GetImage(testImageIndex);
				TupImage tupNewImage(pNewImageAgg);
				char *pNewImageFileName;
				// get filename for image being remapped to
				tupNewImage.GetFilePathName(&pNewImageFileName);

				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Remapping Duplicate Image %s to %s",pImageFileName,pNewImageFileName);
				totalRemapped++;
				break;
			}
		}
	}
	if (totalRemapped)
	{
		// add remap scalars to images that are remapped
		for (imageIndex=0;imageIndex<numImages;imageIndex++)
		{
			if (imageRemapArray[imageIndex]!=-1)
			{
				TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
				assert(pImageAgg);
				pImageAgg->AddScalarInt("Annotate_RemapImageRef",NULL,imageRemapArray[imageIndex]);
			}
		}
	}
}

void ProcessTup::RemapDuplicateAnimatedFrames(void)
{
	int numImages = m_pTupImagePool->GetNumImages();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
		{
			continue;
		}
		else
		{
			TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
			TupImage tupImage(pImageAgg);
			char *pImageFileName;
			// get filename for image
			tupImage.GetFilePathName(&pImageFileName);
			// get image data from image manager
			ImageManager &imageManager = m_dataManager.GetImageManager();
			TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
			assert(pImageDataAgg);
			// get the first frame from the image data
			TupImageData tupImageData(pImageDataAgg);
			int numFrames = tupImageData.GetNumImageDataFrames();
			if (numFrames > 1)
			{
				TupArray<u32> CRCArray;
				CRCArray.SetSize(numFrames);
				int frameIndex;
				for (frameIndex=0;frameIndex<numFrames;frameIndex++)
				{
					TupperwareAggregate *pFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
					assert(pFrameAgg);
					TupImageDataFrame tupImageDataFrame(pFrameAgg);
					CRCArray[frameIndex] = ImageUtilities::GetCRC(tupImageDataFrame);
				}

				int numRemappedFrames = 0;

				for (frameIndex=0;frameIndex<numFrames;frameIndex++)
				{
					u32 CRCValue = CRCArray[frameIndex];

					for (int testFrameIndex=0;testFrameIndex<frameIndex;testFrameIndex++)
					{
						u32 testCRCValue = CRCArray[testFrameIndex];
						if (CRCValue == testCRCValue)
						{
							TupperwareAggregate *pFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
							pFrameAgg->AddScalarInt("Annotate_RemapAnimatedFrameIndex",NULL,testFrameIndex);

							PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Remapping Duplicate Animation Frame %d to %d on Image %s",frameIndex,testFrameIndex,pImageFileName);
							numRemappedFrames++;
							break;
						}
					}
				}
				pImageAgg->AddScalarInt("Annotate_RemappedAnimatedFrames",NULL,numRemappedFrames);
			}
		}
	}
}


void ProcessTup::RemoveDuplicateMaps(void)
{
	int numMaps = m_pTupMapPool->GetNumMaps();
	TupArray<int> mapRefRemapArray;
	TupArray<u32> CRCArray;
	TupArray<bool> activeMapArray;
	mapRefRemapArray.SetSize(numMaps);
	CRCArray.SetSize(numMaps);
	activeMapArray.SetSize(numMaps);

	int mapIndex;
	// init to not remapped
	for (mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		mapRefRemapArray[mapIndex] = -1;
	}
	// keep trying until there was no duplicates found
	int totalRemapped = 0;
	int numPasses = 0;
	while(1)
	{
		// calculate CRC
		bool bFoundDuplicate = false;
		for (mapIndex=0;mapIndex<numMaps;mapIndex++)
		{
			TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapIndex);
			// if not valid or already remapped
			if (!pMapAgg || mapRefRemapArray[mapIndex] !=-1)
			{
				activeMapArray[mapIndex] = false;
			}
			else
			{
				activeMapArray[mapIndex] = true;
				CRCArray[mapIndex] = AnnotateHelper::GetMapCRC(*m_pTupMapPool,mapIndex);
			}
		}
		for (mapIndex=0;mapIndex<numMaps;mapIndex++)
		{
			if (!activeMapArray[mapIndex])
				continue;

			u32 CRCValue = CRCArray[mapIndex];
			for (int testMapIndex=0;testMapIndex<mapIndex;testMapIndex++)
			{
				if (!activeMapArray[testMapIndex])
					continue;

				u32 testCRCValue = CRCArray[testMapIndex];
				// if these are the same
				if (CRCValue == testCRCValue && AnnotateHelper::CompareMaps(*m_pTupMapPool,mapIndex,testMapIndex))
				{
					bFoundDuplicate = true;
					mapRefRemapArray[mapIndex] = testMapIndex;
					activeMapArray[mapIndex] = false;
					totalRemapped++;
					break;
				}
			}
		}
		if (bFoundDuplicate)
		{
			// remap maps
			AnnotateHelper::RemapMaps(*m_pTupMaterialPool,*m_pTupMapPool,mapRefRemapArray);
			for (mapIndex=0;mapIndex<numMaps;mapIndex++)
			{
				// remove any that were remapped this pass
				if (mapRefRemapArray[mapIndex]!=-1)
				{
					m_pTupMapPool->DeleteMap(mapIndex);
					mapRefRemapArray[mapIndex]=-1; // don't delete again
				}
			}
			// go again since the remaps may make other maps the same
			numPasses++;
		}
		else
		{
			// we are done
			break;
		}
	}
	if (totalRemapped)
	{
		if (numPasses==1)
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Found and remapped %d duplicate maps in 1 pass",totalRemapped);
		else
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Found and remapped %d duplicate maps in %d passes",totalRemapped,numPasses);
	}
}

void ProcessTup::RemoveDuplicateMaterials(void)
{
	int numMaterials = m_pTupMaterialPool->GetNumMaterials();
	TupArray<int> materialRefRemapArray;
	TupArray<u32> CRCArray;
	TupArray<bool> activeMaterialArray;
	materialRefRemapArray.SetSize(numMaterials);
	CRCArray.SetSize(numMaterials);
	activeMaterialArray.SetSize(numMaterials);

	int materialIndex;
	// init to not remapped
	for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		materialRefRemapArray[materialIndex] = -1;
	}
	// keep trying until there was no duplicates found
	int totalRemapped = 0;
	int numPasses = 0;
	while(1)
	{
		// calculate CRC
		bool bFoundDuplicate = false;
		for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
		{
			TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialIndex);
			// if not valid or already remapped
			if (!pMaterialAgg || materialRefRemapArray[materialIndex] !=-1)
			{
				activeMaterialArray[materialIndex] = false;
			}
			else
			{
				activeMaterialArray[materialIndex] = true;
				CRCArray[materialIndex] = AnnotateHelper::GetMaterialCRC(*m_pTupMaterialPool,materialIndex);
			}
		}
		for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
		{
			if (!activeMaterialArray[materialIndex])
				continue;

			u32 CRCValue = CRCArray[materialIndex];
			for (int testMaterialIndex=0;testMaterialIndex<materialIndex;testMaterialIndex++)
			{
				if (!activeMaterialArray[testMaterialIndex])
					continue;

				u32 testCRCValue = CRCArray[testMaterialIndex];
				// if these are the same
				if (CRCValue == testCRCValue && AnnotateHelper::CompareMaterials(*m_pTupMaterialPool,materialIndex,testMaterialIndex))
				{
					bFoundDuplicate = true;
					materialRefRemapArray[materialIndex] = testMaterialIndex;
					activeMaterialArray[materialIndex] = false;
					totalRemapped++;
					break;
				}
			}
		}
		if (bFoundDuplicate)
		{
			// remap maps
			AnnotateHelper::RemapMaterials(*m_pTupNodePool,*m_pTupObjectPool,*m_pTupMaterialPool,materialRefRemapArray);
			for (materialIndex=0;materialIndex<numMaterials;materialIndex++)
			{
				// remove any that were remapped this pass
				if (materialRefRemapArray[materialIndex]!=-1)
				{
					m_pTupMaterialPool->DeleteMaterial(materialIndex);
					materialRefRemapArray[materialIndex]=-1; // don't delete again
				}
			}
			// go again since the remaps may make other materials the same
			numPasses++;
		}
		else
		{
			// we are done
			break;
		}
	}
	if (totalRemapped)
	{
		if (numPasses==1)
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Found and remapped %d duplicate materials in 1 pass",totalRemapped);
		else
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Found and remapped %d duplicate materials in %d passes",totalRemapped,numPasses);
	}
}


class PaletteInfo
{
public:
	TupArray<int> m_usedColors;
	TupperwareAggregate *m_pImageDataFrameAgg;
	TupString m_name;
	int *m_textureIDs;
	bool m_bActive;
	bool m_bChild;
	int m_textureSet;
};

void ProcessTup::MergeImagePalettes(void)
{
	int numPalettes = 0;
	int numMergedPalettes = 0;
	int numImages = m_pTupImagePool->GetNumImages();
	int imageIndex;
	TupArray<PaletteInfo> paletteInfoArray;
	paletteInfoArray.SetSize(numImages);

	int numVisibilitySets = m_visibilitySetNodeRefs.GetSize();

	for (imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		PaletteInfo &paletteInfo = paletteInfoArray[imageIndex];
		paletteInfo.m_bActive = false;
		if (!pImageAgg)
			continue;

		if (!pImageAgg->FindScalarByKey("Annotate_HasTextureID")->GetAsInt())
			continue;

		TupImage tupImage(pImageAgg);
		char *pImageFileName;
		// get filename for image
		tupImage.GetFilePathName(&pImageFileName);
		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
		assert(pImageDataAgg);
		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		if (numFrames>1)
		{
			MergeAnimatedImagePalettes(pImageFileName,tupImageData,numPalettes,numMergedPalettes);
		}
		else
		{
			assert (numFrames==1);
			TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0);
			TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
			int formatFlags;
			tupImageDataFrame.GetFormatFlags(formatFlags);

			// is it a paletted frame?
			if ((formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_8 ||
				(formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_4)
			{
				numPalettes++;
				paletteInfo.m_bActive = true;
				paletteInfo.m_bChild = false;
				paletteInfo.m_pImageDataFrameAgg = pImageDataFrameAgg;
				paletteInfo.m_textureSet = pImageAgg->FindScalarByKey("Annotate_TextureSet")->GetAsInt();
				paletteInfo.m_name = pImageFileName;
				paletteInfo.m_textureIDs = pImageAgg->FindListByKey("Annotate_TextureIDs")->GetAsInt();
				ImageUtilities::GetUsedColors(tupImageDataFrame,paletteInfo.m_usedColors);

				int parentImageIndex;
				int bestParentImageIndex = -1;
				int bestAddedColors;
				int addedColors;
				// look at potential parents
				for (parentImageIndex=0;parentImageIndex<imageIndex;parentImageIndex++)
				{
					PaletteInfo &parentPaletteInfo = paletteInfoArray[parentImageIndex];
					// is this parent a potental canidate for a palette merge
					if (parentPaletteInfo.m_bActive && 
						!parentPaletteInfo.m_bChild &&
						parentPaletteInfo.m_textureSet == paletteInfo.m_textureSet &&
						parentPaletteInfo.m_usedColors.GetSize() == paletteInfo.m_usedColors.GetSize())
					{

						// make sure this texture is in all the same partitions as I am
						bool bSamePartitions = true;
						for (int setIndex=0;setIndex<numVisibilitySets;setIndex++)
						{
							int parentTextureID = parentPaletteInfo.m_textureIDs[setIndex];
							int myTextureID = paletteInfo.m_textureIDs[setIndex];
							// they both need to be active in the same 
							if (parentTextureID == -1)
							{
								if (myTextureID != -1)
								{
									bSamePartitions = false;
									break;
								}
							}
							else
							{
								if (myTextureID == -1)
								{
									bSamePartitions = false;
									break;
								}
							}
						}
						TupImageDataFrame tupParentImageDataFrame(parentPaletteInfo.m_pImageDataFrameAgg);
						TupArray<int> remapArray; // not used for now
						// check if these can merge
						if (bSamePartitions && ImageUtilities::MergePalettes(tupParentImageDataFrame,parentPaletteInfo.m_usedColors,
														tupImageDataFrame,paletteInfo.m_usedColors,
														remapArray,addedColors,true))
						{
							if (bestParentImageIndex==-1 || addedColors<bestAddedColors)
							{
								bestAddedColors = addedColors;
								bestParentImageIndex = parentImageIndex;
							}
						}
					}
				}
				// if a parent was found
				if (bestParentImageIndex!=-1)
				{
					numMergedPalettes++;
					PaletteInfo &parentPaletteInfo = paletteInfoArray[bestParentImageIndex];
					TupImageDataFrame tupParentImageDataFrame(parentPaletteInfo.m_pImageDataFrameAgg);
					TupArray<int> remapArray; 
					int priorUsedColors = ImageUtilities::GetUsedColors(parentPaletteInfo.m_usedColors);
					// merge the palettes, build the remap array, & update the used color array for parent
					ImageUtilities::MergePalettes(tupParentImageDataFrame,parentPaletteInfo.m_usedColors,
													tupImageDataFrame,paletteInfo.m_usedColors,
													remapArray,addedColors,false);
					int afterUsedColors = ImageUtilities::GetUsedColors(parentPaletteInfo.m_usedColors);
					// alter the pixels to fit the parent's palette
					ImageUtilities::RemapPalettedFrame(tupImageDataFrame,remapArray);
					// copy the parents palette to the child
					ImageUtilities::CopyPalette(tupImageDataFrame,tupParentImageDataFrame);
					paletteInfo.m_bChild = true; // I am a child (no one can use me as a parent)
					// I now have a parent palette to another image
					pImageAgg->AddScalarInt("Annotate_ParentPaletteReference",NULL,bestParentImageIndex);
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging Palettes from %s to %s. Added %d colors, was %d now %d",
						paletteInfo.m_name,parentPaletteInfo.m_name,afterUsedColors-priorUsedColors,priorUsedColors,afterUsedColors);
				}
			}
		}
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"%d palettes cut down to %d palettes",numPalettes,numPalettes-numMergedPalettes);
}

void ProcessTup::MergeAnimatedImagePalettes(const char *pImageFileName,TupImageData &tupImageData,int &numPalettes,int &numMergedPalettes)
{
	int numFrames = tupImageData.GetNumImageDataFrames();
	assert(numFrames>1);
	int frameIndex;
	TupArray<PaletteInfo> paletteInfoArray;
	paletteInfoArray.SetSize(numFrames);

	for (frameIndex=0;frameIndex<numFrames;frameIndex++)
	{
		PaletteInfo &paletteInfo = paletteInfoArray[frameIndex];
		TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
		TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
		int formatFlags;
		tupImageDataFrame.GetFormatFlags(formatFlags);

		// is it a paletted frame?
		if ((formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_8 ||
			(formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_4)
		{
			numPalettes++;
			paletteInfo.m_bActive = true;
			paletteInfo.m_bChild = false;
			paletteInfo.m_pImageDataFrameAgg = pImageDataFrameAgg;

			ImageUtilities::GetUsedColors(tupImageDataFrame,paletteInfo.m_usedColors);

			int parentFrameIndex;
			int bestParentFrameIndex = -1;
			int bestAddedColors;
			int addedColors;
			// look at potential parents
			for (parentFrameIndex=0;parentFrameIndex<frameIndex;parentFrameIndex++)
			{
				PaletteInfo &parentPaletteInfo = paletteInfoArray[parentFrameIndex];
				// is this parent a potental canidate for a palette merge
				if (parentPaletteInfo.m_bActive && 
					!parentPaletteInfo.m_bChild &&
					parentPaletteInfo.m_usedColors.GetSize() == paletteInfo.m_usedColors.GetSize())
				{
					TupImageDataFrame tupParentImageDataFrame(parentPaletteInfo.m_pImageDataFrameAgg);
					TupArray<int> remapArray; // not used for now
					// check if these can merge
					if (ImageUtilities::MergePalettes(tupParentImageDataFrame,parentPaletteInfo.m_usedColors,
													tupImageDataFrame,paletteInfo.m_usedColors,
													remapArray,addedColors,true))
					{
						if (bestParentFrameIndex==-1 || addedColors<bestAddedColors)
						{
							bestAddedColors = addedColors;
							bestParentFrameIndex = parentFrameIndex;
						}
					}
				}
			}
			// if a parent was found
			if (bestParentFrameIndex!=-1)
			{
				numMergedPalettes++;
				PaletteInfo &parentPaletteInfo = paletteInfoArray[bestParentFrameIndex];
				TupImageDataFrame tupParentImageDataFrame(parentPaletteInfo.m_pImageDataFrameAgg);
				TupArray<int> remapArray; 
				int priorUsedColors = ImageUtilities::GetUsedColors(parentPaletteInfo.m_usedColors);
				// merge the palettes, build the remap array, & update the used color array for parent
				ImageUtilities::MergePalettes(tupParentImageDataFrame,parentPaletteInfo.m_usedColors,
												tupImageDataFrame,paletteInfo.m_usedColors,
												remapArray,addedColors,false);
				int afterUsedColors = ImageUtilities::GetUsedColors(parentPaletteInfo.m_usedColors);
				// alter the pixels to fit the parent's palette
				ImageUtilities::RemapPalettedFrame(tupImageDataFrame,remapArray);
				// copy the parents palette to the child
				ImageUtilities::CopyPalette(tupImageDataFrame,tupParentImageDataFrame);
				paletteInfo.m_bChild = true; // I am a child (no one can use me as a parent)
				// I now have a parent palette to another image
				pImageDataFrameAgg->AddScalarInt("Annotate_ParentPaletteFrame",NULL,bestParentFrameIndex);
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging Palettes From %s - Frame %d to Frame %d. Added %d colors, was %d now %d",
					pImageFileName,frameIndex,bestParentFrameIndex,afterUsedColors-priorUsedColors,priorUsedColors,afterUsedColors);
			}
		}
	}
}

void ProcessTup::AnalyzePaletteColors(void)
{
	int numImages = m_pTupImagePool->GetNumImages();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(imageIndex);
		if (!pImageAgg)
			continue;

		if (!pImageAgg->FindScalarByKey("Annotate_HasTextureID")->GetAsInt())
			continue;

		TupImage tupImage(pImageAgg);
		char *pImageFileName;
		// get filename for image
		tupImage.GetFilePathName(&pImageFileName);
		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
		assert(pImageDataAgg);
		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		if (numFrames>1)
		{
			bool bFoundIndexedImage = false;
			int maxAlphasUsed = 0;
			int frameIndex;
			for (frameIndex=0;frameIndex<numFrames;frameIndex++)
			{
				TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
				TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
				int formatFlags;
				tupImageDataFrame.GetFormatFlags(formatFlags);

				// is it a paletted frame?
				if ((formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_8 ||
					(formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_4)
				{
					bFoundIndexedImage = true;
					TupperwareScalar *pParentPaletteFrameScalar = pImageAgg->FindScalarByKey("Annotate_ParentPaletteFrame");
					// we are not referencing a parent
					if (!pParentPaletteFrameScalar)
					{
						TupperwareAggregate *pPaletteAgg = tupImageDataFrame.GetImageDataPalette();
						TupImageDataPalette tupPalette(pPaletteAgg);
						u32 *pPaletteData;
						int numSourceBytes;
						tupPalette.GetData((void **)&pPaletteData,numSourceBytes);
						int numColors = numSourceBytes>>2;
						Keeper<int> alphasUsed;
						for (int paletteIndex=0;paletteIndex<numColors;paletteIndex++)
						{
							alphasUsed.Add(pPaletteData[paletteIndex]>>24);
						}
						if (alphasUsed.GetSize()>maxAlphasUsed)
							maxAlphasUsed = alphasUsed.GetSize();
					}
				}
			}
			if (bFoundIndexedImage)
			{
				pImageAgg->AddScalarInt("Annotate_PaletteAlphasUsed",NULL,maxAlphasUsed);
			}
		}
		else
		{
			assert (numFrames==1);
			TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0);
			TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
			int formatFlags;
			tupImageDataFrame.GetFormatFlags(formatFlags);

			// is it a paletted frame?
			if ((formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_8 ||
				(formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_4)
			{
				TupperwareScalar *pParentPaletteReferenceScalar = pImageAgg->FindScalarByKey("Annotate_ParentPaletteReference");
				// we are not referencing a parent
				if (!pParentPaletteReferenceScalar)
				{
					TupperwareAggregate *pPaletteAgg = tupImageDataFrame.GetImageDataPalette();
					TupImageDataPalette tupPalette(pPaletteAgg);
					u32 *pPaletteData;
					int numSourceBytes;
					tupPalette.GetData((void **)&pPaletteData,numSourceBytes);
					int numColors = numSourceBytes>>2;
					Keeper<int> alphasUsed;
					for (int paletteIndex=0;paletteIndex<numColors;paletteIndex++)
					{
						alphasUsed.Add(pPaletteData[paletteIndex]>>24);
					}
					pImageAgg->AddScalarInt("Annotate_PaletteAlphasUsed",NULL,alphasUsed.GetSize());
				}
			}
		}
	}
}

// makes sure the meshes are ok
void ProcessTup::CheckMeshIntegrity(void)
{
	int numErrors = 0;
	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
		
		// if empty slot - continue
		if (!pObjectAgg)
			continue;
		
		// it needs to be a mesh
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType() != TupObject::TRI_MESH)
			continue;

		TupTriMesh tupTriMesh(pObjectAgg);

		TupString name = "None";
		TupperwareScalar *pMayaObjectNameScalar = tupTriMesh.GetBaseAggregate()->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
		{
			name = pMayaObjectNameScalar->GetAsString();
		}

		float *pPositionVerts;
		float *pNormalVerts;
		int *pPositionIndices;
		int *pNormalIndices;
		int *pFaceMaterialIndices;
		int *pFaceFlags;
		
		int numPositionVerts;
		int numNormalVerts;
		int numPositionFaces,numNormalFaces,numMaterialFaces,numFlagFaces;
		
		TupperwareList *pPositionVertsList = tupTriMesh.GetPositionVerts(&pPositionVerts,numPositionVerts);
		if (!pPositionVertsList)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error - Mesh Integrity Check Mesh #%d %s - Mesh data does not exist.",objectIndex,(const char *)name);
			numErrors++;
			continue;
		}

		if (pPositionVertsList->GetLength() != numPositionVerts*3)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error - Mesh Integrity Check Mesh #%d %s - Position Verts number of float values not a multiple of 3 - possible invalid value within file",objectIndex,(const char *)name);
			numErrors++;
		}

		TupperwareList *pNormalVertsList = tupTriMesh.GetNormalVerts(&pNormalVerts,numNormalVerts);
		if (!pNormalVertsList || pNormalVertsList->GetLength() != numNormalVerts*3)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error - Mesh Integrity Check Mesh #%d %s - Normal Verts number of float values not a multiple of 3 - possible invalid value within file",objectIndex,(const char *)name);
			numErrors++;
			if (!pNormalVertsList)
				continue;
		}

		TupperwareList *pPositionIndiciesList = tupTriMesh.GetPositionIndices(&pPositionIndices,numPositionFaces);
		if (!pPositionIndiciesList || pPositionIndiciesList->GetLength() != numPositionFaces*3)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error - Mesh Integrity Check Mesh #%d %s - Position Indices number of index values not a multiple of 3 - possible invalid value within file",objectIndex,(const char *)name);
			numErrors++;
			if (!pPositionIndiciesList)
				continue;
		}

		TupperwareList *pNormalIndiciesList = tupTriMesh.GetNormalIndices(&pNormalIndices,numNormalFaces);
		if (!pNormalIndiciesList || pNormalIndiciesList->GetLength() != numNormalFaces*3)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error - Mesh Integrity Check Mesh #%d %s - Normal Indices number of index values not a multiple of 3 - possible invalid value within file",objectIndex,(const char *)name);
			numErrors++;
			if (!pNormalIndiciesList)
				continue;
		}

		if (numNormalFaces!=numPositionFaces)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error - Mesh Integrity Check Mesh #%d %s - Number of faces inconsistant between positions and normals",objectIndex,(const char *)name);
			numErrors++;
		}

		tupTriMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,numMaterialFaces);
		if (numMaterialFaces!=numPositionFaces)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error - Mesh Integrity Check Mesh #%d %s - Number of faces inconsistant between material indices and position",objectIndex,(const char *)name);
			numErrors++;
		}

		tupTriMesh.GetFaceFlags(&pFaceFlags,numFlagFaces);
		if (numFlagFaces!=numPositionFaces)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error - Mesh Integrity Check Mesh #%d %s - Number of faces inconsistant between face flags and position",objectIndex,(const char *)name);
			numErrors++;
		}

		int indexErrors;
		int index;
		indexErrors = 0;
		for (index=0;index<pPositionIndiciesList->GetLength();index++)
		{
			if (pPositionIndices[index]<0 || pPositionIndices[index]>=numPositionVerts)
			{
				indexErrors++;
			}
		}
		if (indexErrors)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error - Mesh Integrity Check Mesh #%d %s - Position Indices have %d indexes that are out of bounds",objectIndex,(const char *)name,indexErrors);
			numErrors++;
		}

		indexErrors = 0;
		for (index=0;index<pNormalIndiciesList->GetLength();index++)
		{
			if (pNormalIndices[index]<0 || pNormalIndices[index]>=numNormalVerts)
			{
				indexErrors++;
			}
		}
		if (indexErrors)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error - Mesh Integrity Check Mesh #%d %s - Normal Indices have %d indexes that are out of bounds",objectIndex,(const char *)name,indexErrors);
			numErrors++;
		}

		int numMeshMaps = tupTriMesh.GetNumMeshMaps();
		for (int meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
			TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
			int mapChannel = tupTriMeshMap.GetMapChannel();

			float *pMapVerts;
			int *pMapIndices;

			int numMapVerts;
			int numMapFaces;

			TupperwareList *pMapVertsList = tupTriMeshMap.GetMapVerts(&pMapVerts,numMapVerts);
			if (pMapVertsList)
			{
				if (pMapVertsList->GetLength() != numMapVerts*3)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
						"Error - Mesh Integrity Check Mesh #%d %s - MeshMap %d Verts number of float values not a multiple of 3 - possible invalid value within file",objectIndex,(const char *)name,mapChannel);
					numErrors++;
				}
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
					"Error - Mesh Integrity Check Mesh #%d %s - MeshMap %d No Verts Found",objectIndex,(const char *)name,mapChannel);
				numErrors++;
			}

			TupperwareList *pMapIndicesList = tupTriMeshMap.GetMapIndices(&pMapIndices,numMapFaces);
			if (pMapIndicesList)
			{
				if (pMapIndicesList->GetLength() != numMapFaces*3)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
						"Error - Mesh Integrity Check Mesh #%d %s - MeshMap %d Indices number of index values not a multiple of 3 - possible invalid value within file",objectIndex,(const char *)name,mapChannel);
					numErrors++;
				}
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
					"Error - Mesh Integrity Check Mesh #%d %s - MeshMap %d No Indices Found",objectIndex,(const char *)name,mapChannel);
				numErrors++;
			}

			if (numMapFaces!=numPositionFaces)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
					"Error - Mesh Integrity Check Mesh #%d %s - Number of faces inconsistant between positions and mesh map %d",objectIndex,(const char *)name,mapChannel);
				numErrors++;
			}

			indexErrors = 0;
			for (index=0;index<pMapIndicesList->GetLength();index++)
			{
				if (pMapIndices[index]<0 || pMapIndices[index]>=numMapVerts)
				{
					indexErrors++;
				}
			}
			if (indexErrors)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
					"Error - Mesh Integrity Check Mesh #%d %s - Mesh Map %d Indices has %d indices that are out of bounds",objectIndex,(const char *)name,mapChannel,indexErrors);
				numErrors++;
			}
		}
		// check for winding problems
#if 0
		if (!numErrors)
		{
			int numWindingErrors = 0;
			int faceIndex;
			int facePointIndex;
			Vector3 *pPositionVectors = (Vector3 *)pPositionVerts;
			Vector3 *pNormalVectors = (Vector3 *)pNormalVerts;
			for (faceIndex=0,facePointIndex=0;faceIndex<numPositionFaces;faceIndex++,facePointIndex+=3)
			{
				Vector3 *pPoint1 = &pPositionVectors[pPositionIndices[facePointIndex]];
				Vector3 *pPoint2 = &pPositionVectors[pPositionIndices[facePointIndex+1]];
				Vector3 *pPoint3 = &pPositionVectors[pPositionIndices[facePointIndex+2]];
				
				Vector3 faceNormal;
				faceNormal = Vector3::Cross((*pPoint3) - (*pPoint2), (*pPoint1) - (*pPoint2));
				faceNormal.SafeNormalize();
				
				Vector3 *pNormal1 = &pNormalVectors[pNormalIndices[facePointIndex]];
				Vector3 *pNormal2 = &pNormalVectors[pNormalIndices[facePointIndex+1]];
				Vector3 *pNormal3 = &pNormalVectors[pNormalIndices[facePointIndex+2]];
				
				const float NORMAL_TOLERANCE = -0.92f;	// 75 degrees
				if (Vector3::Dot(faceNormal, *pNormal1) < NORMAL_TOLERANCE)
				{
					numWindingErrors++;
				}
				
				if (Vector3::Dot(faceNormal, *pNormal2) < NORMAL_TOLERANCE)
				{
					numWindingErrors++;
				}
				
				if (Vector3::Dot(faceNormal, *pNormal3) < NORMAL_TOLERANCE)
				{
					numWindingErrors++;
				}
			}
			if (numWindingErrors)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
					"Warning - Mesh Integrity Check Mesh #%d %s - Has %d vertex normals greater than 165 degrees from face normal.",objectIndex,(const char *)name,numWindingErrors);
			}
		}
#endif 
	}
	if (numErrors)
	{
		throw(PluginException(ERROR_FATAL,"Mesh Integrity Check Failed."));
	}
}

static const float threshold = 0.001f;
static bool CanConnect(float targetValue,float startValue)
{
	float delta = targetValue-startValue;
	float intPart;
	if (delta>=0.0f)
	{
		intPart = (float)((int)(delta+0.5f));
	}
	else
	{
		intPart = (float)((int)(delta-0.5f));
	}
	float fracPart = delta-intPart;
	return (fracPart<=threshold && fracPart>=-threshold);
}

static float GetTileOffset(float targetValue,float startValue)
{
	float delta = targetValue-startValue;
	if (delta>=0.0f)
	{
		return (float)((int)(delta+0.5f));
	}
	else
	{
		return (float)((int)(delta-0.5f));
	}
}

class SourceMeshMap
{
public:
	TupperwareAggregate *m_pMeshMapAgg;
	int m_mapChannel;
	TupVertex3f *m_pMapVerts;
	int *m_pMapIndices;
	int m_numMapVerts;
	int m_numMapIndices;
};

class DestMeshMap
{
public:
	Keeper<TupVertex3f> m_mapVerts;
	TupArray<int> m_mapIndices;
};

class MeshMapRemap
{
public:
	int m_sourceMapRef;
	int m_sourceMapChannel;
	int m_destMapChannel;
	int m_imageRef;
};

void ProcessTup::ConsolidateMeshMaps(void)
{
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Consolidating Mesh Maps");

	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if empty slot - continue
		if (!pObjectAgg)
			continue;

		// it needs to be a mesh
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType() != TupObject::TRI_MESH)
			continue;

		TupperwareScalar *pMaterialRefScalar = pObjectAgg->FindScalarByKey("Annotate_MaterialRef");
		int materialRef = pMaterialRefScalar->GetAsInt();
		// make sure it is using a material
		if (materialRef==-1)
			continue;

		TupString mayaObjectName;
		TupperwareScalar *pMayaObjectNameScalar = pObjectAgg->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
		{
			mayaObjectName = pMayaObjectNameScalar->GetAsString();
		}

		TupTriMesh tupTriMesh(pObjectAgg);

		// gather information about the current mesh maps (only include texture uv maps)
		TupArray<SourceMeshMap> sourceMeshMaps;

		int numMeshMaps = tupTriMesh.GetNumMeshMaps();
		int meshMapIndex;
		int maxMapChannel = -1;
		for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
			TupTriMeshMap triMeshMap(pMeshMapAgg);

			int mapChannel = triMeshMap.GetMapChannel();
			if (mapChannel >= TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
			{
				SourceMeshMap newSourceMeshMap;
				newSourceMeshMap.m_pMeshMapAgg = pMeshMapAgg; // keep track of aggregate to delete later
				triMeshMap.GetMapVerts((float **) &newSourceMeshMap.m_pMapVerts, newSourceMeshMap.m_numMapVerts);
				triMeshMap.GetMapIndices(&newSourceMeshMap.m_pMapIndices, newSourceMeshMap.m_numMapIndices);
				newSourceMeshMap.m_mapChannel = mapChannel;
				sourceMeshMaps.Add(newSourceMeshMap);

				// keep track of max channel
				if (mapChannel>maxMapChannel)
					maxMapChannel = mapChannel;
			}
		}

		// if we are not using more than 1 uv mesh map then go on to the next one
		if (sourceMeshMaps.GetSize()<2)
			continue;

		// set up table to remap from map channel to mesh map index
		TupArray<int> channelToMeshMap;
		channelToMeshMap.SetSize(maxMapChannel+1);
		for (meshMapIndex=0;meshMapIndex<maxMapChannel;meshMapIndex++)
		{
			channelToMeshMap[meshMapIndex] = -1;
		}
		for (meshMapIndex=0;meshMapIndex<sourceMeshMaps.GetSize();meshMapIndex++)
		{
			SourceMeshMap &sourceMeshMap = sourceMeshMaps[meshMapIndex];
			channelToMeshMap[sourceMeshMap.m_mapChannel] = meshMapIndex;
		}

		TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
		assert(pMaterialAgg);

		TupMaterial tupMat(pMaterialAgg);

		TupArray<TupArray<MeshMapRemap> > subMaterialsRemaps;
		Keeper<int> usedImageRefs;

		int materialIndex;
		int totalSubMaterials;

		if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{
			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
			totalSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();

			if (totalSubMaterials)
			{
				subMaterialsRemaps.SetSize(totalSubMaterials);

				for (materialIndex = 0;materialIndex<totalSubMaterials;materialIndex++)
				{
					TupArray<MeshMapRemap> &subMaterialRemaps = subMaterialsRemaps[materialIndex];

					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int subMaterialRef;
					tupSubMaterial.GetMaterialReference(subMaterialRef);

					if (subMaterialRef != -1)
					{
						Keeper<int> bitmapMapRefs;

						AnnotateHelper::CalculateUsedBitmapMapsFromMaterial(*m_pTupMaterialPool,*m_pTupMapPool,subMaterialRef,bitmapMapRefs);

						for (int bitmapMapIndex = 0; bitmapMapIndex < bitmapMapRefs.GetSize(); bitmapMapIndex++)
						{
							int bitmapMapRef = bitmapMapRefs[bitmapMapIndex];

							TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(bitmapMapRef);
							TupBitmapMap tupBitmapMap(pMapAgg);

							MeshMapRemap newMapRemap;
							newMapRemap.m_sourceMapRef = bitmapMapRef;
							tupBitmapMap.GetMapChannel(newMapRemap.m_sourceMapChannel);
							tupBitmapMap.GetImageReference(newMapRemap.m_imageRef);
							subMaterialRemaps.Add(newMapRemap);
							usedImageRefs.Add(newMapRemap.m_imageRef);
						}
					}
				}
			}
			else
			{
				// if no sub materials skip
				continue;
			}
		}
		else
		{
			// if using only one material then skip
			continue;
		}

		// build arrays to keep track of what images are used on together on the same materials
		TupArray<TupArray<int> > imagesUsedTogether;
		imagesUsedTogether.SetSize(usedImageRefs.GetSize());

		for (materialIndex = 0;materialIndex<totalSubMaterials;materialIndex++)
		{
			TupArray<MeshMapRemap> &subMaterialRemaps = subMaterialsRemaps[materialIndex];
			int numBitmapMaps = subMaterialRemaps.GetSize();

			for (int bitmapMapIndex = 0; bitmapMapIndex < numBitmapMaps; bitmapMapIndex++)
			{
				MeshMapRemap &meshMapRemap = subMaterialRemaps[bitmapMapIndex];
				int imageRef = meshMapRemap.m_imageRef;
				int usedImageIndex = usedImageRefs.GetIndex(imageRef);
				TupArray<int> &imagesUsedTogetherSub = imagesUsedTogether[usedImageIndex];

				for (int bitmapMapIndex2 = 0; bitmapMapIndex2 < numBitmapMaps; bitmapMapIndex2++)
				{
					if (bitmapMapIndex2 == bitmapMapIndex)
						continue;

					MeshMapRemap &meshMapRemap2 = subMaterialRemaps[bitmapMapIndex2];

					int imageRef2 = meshMapRemap2.m_imageRef;
					if (imageRef2 == imageRef)
						continue;

					// add unique image refs to the together array
					bool imageFound = false;
					for (int togetherIndex = 0; togetherIndex < imagesUsedTogetherSub.GetSize(); togetherIndex++)
					{
						if (imagesUsedTogetherSub[togetherIndex] == imageRef2)
						{
							imageFound = true;
							break;
						}
					}
					if (!imageFound)
					{
						imagesUsedTogetherSub.Add(imageRef2);
					}
				}
			}
		}

		// assign each image a map channel to use
		// this is based on how the images are used together within the same material
		TupArray<int> assignedImageMapChannel;

		assignedImageMapChannel.SetSize(usedImageRefs.GetSize());

		maxMapChannel = -1;

		for (int imageIndex = 0; imageIndex < usedImageRefs.GetSize(); imageIndex++)
		{
			TupArray<int> &imagesUsedTogetherSub = imagesUsedTogether[imageIndex];

			int targetMapChannel = 1;
			while (1) 
			{
				bool bMapChannelOK = true;
				for (int togetherIndex = 0; togetherIndex < imagesUsedTogetherSub.GetSize(); togetherIndex++)
				{
					int togetherImageRef = imagesUsedTogetherSub[togetherIndex];
					int togetherUsedImageIndex = usedImageRefs.GetIndex(togetherImageRef);
					// has this image been assigned?
					if (togetherUsedImageIndex<imageIndex)
					{
						if (assignedImageMapChannel[togetherUsedImageIndex] == targetMapChannel)
						{
							bMapChannelOK = false;
							break;
						}
					}
				}
				// if this channel is ok stop
				if (bMapChannelOK)
				{
					break;
				}
				else
				{
					// try next channel until we find an open one
					targetMapChannel++;
				}
			} 
			assignedImageMapChannel[imageIndex] = targetMapChannel;

			// keep track of how many mesh maps we will use after consolidation
			if (targetMapChannel > maxMapChannel)
				maxMapChannel = targetMapChannel;
		}
		 
		// if no images used any map channels then skip
		if (maxMapChannel==-1)
			continue;

		// set dest map channels for each map for each material
		for (materialIndex = 0;materialIndex<totalSubMaterials;materialIndex++)
		{
			TupArray<MeshMapRemap> &subMaterialRemaps = subMaterialsRemaps[materialIndex];
			int numBitmapMaps = subMaterialRemaps.GetSize();

			for (int bitmapMapIndex = 0; bitmapMapIndex < numBitmapMaps; bitmapMapIndex++)
			{
				MeshMapRemap &meshMapRemap = subMaterialRemaps[bitmapMapIndex];
				int imageRef = meshMapRemap.m_imageRef;
				int usedImageIndex = usedImageRefs.GetIndex(imageRef);

				meshMapRemap.m_destMapChannel = assignedImageMapChannel[usedImageIndex];
			}
		}

		int *pMaterialIndices;
		int numFaces;
		tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,numFaces);
		int numFaces3 = (numFaces<<1) + numFaces;
		int faceIndex;
		int faceIndex3;

		TupArray<DestMeshMap> destMeshMaps;
		destMeshMaps.SetSize(maxMapChannel); // max is size since we start at 1

		// start with empty mesh maps (all pointing at 0
		for (int mapChannel = 0; mapChannel < maxMapChannel; mapChannel++)
		{
			DestMeshMap &destMeshMap = destMeshMaps[mapChannel];
			destMeshMap.m_mapVerts.Add(TupVertex3f(0.0f,0.0f,0.0f));
			destMeshMap.m_mapIndices.SetSize(numFaces3);
			for (faceIndex3 = 0; faceIndex3 < numFaces3; faceIndex3++)
				destMeshMap.m_mapIndices[faceIndex3] = 0;
		}

		// create consolidated mesh maps from source mesh maps
		for (faceIndex = 0,faceIndex3 = 0; faceIndex < numFaces; faceIndex++, faceIndex3+=3)
		{
			int faceMaterialIndex = pMaterialIndices[faceIndex];

			TupArray<MeshMapRemap> &subMaterialRemaps = subMaterialsRemaps[faceMaterialIndex];

			for (int remapIndex = 0; remapIndex < subMaterialRemaps.GetSize(); remapIndex++)
			{
				MeshMapRemap &meshMapRemap = subMaterialRemaps[remapIndex];

				SourceMeshMap &sourceMeshMap = sourceMeshMaps[channelToMeshMap[meshMapRemap.m_sourceMapChannel]];
				DestMeshMap &destMeshMap = destMeshMaps[meshMapRemap.m_destMapChannel-1]; // offset by 1

				for (int facePointIndex = 0; facePointIndex < 3; facePointIndex++)
				{
					TupVertex3f &uv = sourceMeshMap.m_pMapVerts[sourceMeshMap.m_pMapIndices[faceIndex3+facePointIndex]];
					destMeshMap.m_mapIndices[faceIndex3+facePointIndex] = destMeshMap.m_mapVerts.Add(uv);
				}
			}
		}

		// delete the old mesh maps
		for (meshMapIndex = 0; meshMapIndex < sourceMeshMaps.GetSize(); meshMapIndex++)
		{
			SourceMeshMap &sourceMeshMap = sourceMeshMaps[meshMapIndex];
			TupperwareAggregate *pParentAgg = sourceMeshMap.m_pMeshMapAgg->GetParent();
			pParentAgg->DeleteAtom(sourceMeshMap.m_pMeshMapAgg);
		}

		// add new mesh maps
		for (meshMapIndex = 0; meshMapIndex < destMeshMaps.GetSize(); meshMapIndex++)
		{
			DestMeshMap &destMeshMap = destMeshMaps[meshMapIndex];

			TupperwareAggregate *meshMapAgg = tupTriMesh.AddMeshMap(meshMapIndex+1);
			TupTriMeshMap tupTriMeshMap(meshMapAgg);
			TupString mapName;
			mapName.Format("ConsolidatedMap%d",meshMapIndex+1);
			tupTriMeshMap.AddMapName(mapName);
			tupTriMeshMap.AddMapVerts((float *)destMeshMap.m_mapVerts.GetArray().GetData(),destMeshMap.m_mapVerts.GetSize());
			tupTriMeshMap.AddMapIndices(destMeshMap.m_mapIndices.GetData(),numFaces);
		}

		// remap the materials and maps to point to the correct mesh maps
		Keeper<int> remappedMaterials;
		TupArray<int> remappedMaterialRefs;
		Keeper<int> remappedMaps;
		TupArray<int> remappedMapRefs;

		int newMaterialRef = AnnotateHelper::RemapMapChannels(*m_pTupMaterialPool,*m_pTupMapPool,
			materialRef,remappedMaterials,remappedMaterialRefs,
			remappedMaps,remappedMapRefs,
			usedImageRefs,assignedImageMapChannel);

		// mark mesh to use new material
		pMaterialRefScalar->SetAsInt(newMaterialRef);

		// remap material references on relivant nodes
		int numNodes = m_pTupNodePool->GetNumNodes();
		for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
		{
			TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
			if (!pNodeAgg)
				continue;

			TupNode tupNode(pNodeAgg);
			int nodeObjectRef;
			tupNode.GetObjectReference(nodeObjectRef);
			if (nodeObjectRef!=-1)
			{
				int baseObjectRef = AnnotateHelper::FindBaseObjectRef(*m_pTupObjectPool,nodeObjectRef);
				// if the base matches the object reference
				if (baseObjectRef == objectIndex)
				{
					int nodeMaterialRef;
					tupNode.GetMaterialReference(nodeMaterialRef);
					assert(nodeMaterialRef==materialRef);
					tupNode.AddMaterialReference(newMaterialRef); // set to new material
				}
			}
		}

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Consolidated MeshMaps on %s from %d to %d meshmaps",(const char *)mayaObjectName,sourceMeshMaps.GetSize(),destMeshMaps.GetSize());
	}
}

void ProcessTup::ConsolidateFaceMaterials(void)
{
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Consolidating Face Materials");

	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

		// if empty slot - continue
		if (!pObjectAgg)
			continue;

		// it needs to be a mesh
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType() != TupObject::TRI_MESH)
			continue;

		TupperwareScalar *pMaterialRefScalar = pObjectAgg->FindScalarByKey("Annotate_MaterialRef");
		int materialRef = pMaterialRefScalar->GetAsInt();
		// make sure it is using a material
		if (materialRef==-1)
			continue;

		TupString mayaObjectName;
		TupperwareScalar *pMayaObjectNameScalar = pObjectAgg->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
		{
			mayaObjectName = pMayaObjectNameScalar->GetAsString();
		}

		TupTriMesh tupTriMesh(pObjectAgg);

		TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
		assert(pMaterialAgg);

		int *pMaterialIndices;
		int numFaces;
		tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,numFaces);

		TupMaterial tupMat(pMaterialAgg);

		Keeper<int> usedMaterialRefs;

		int materialIndex;
		int totalSubMaterials;

		TupArray<int> subMaterialRefs;

		TupString oldMaterialName;

		if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{

			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);

			char *pMaterialName;
			tupMultiSubObjectMaterial.GetName(&pMaterialName);
			oldMaterialName = pMaterialName;

			totalSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();

			if (totalSubMaterials)
			{
				subMaterialRefs.SetSize(totalSubMaterials);

				for (materialIndex = 0;materialIndex<totalSubMaterials;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int subMaterialRef;
					tupSubMaterial.GetMaterialReference(subMaterialRef);
					subMaterialRefs[materialIndex] = subMaterialRef;
				}
			}
			else
			{
				// if no sub materials skip
				continue;
			}
		}
		else
		{
			// if using only one material then skip
			continue;
		}

		// compute the number of sub materials used on mesh
		for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
		{
			int faceMaterialRef = subMaterialRefs[pMaterialIndices[faceIndex]];
			usedMaterialRefs.Add(faceMaterialRef);
		}

		// if there are none to consolidate then skip
		if (usedMaterialRefs.GetSize()>=totalSubMaterials)
			continue;

		// remap face material indices to keeper
		for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
		{
			int faceMaterialRef = subMaterialRefs[pMaterialIndices[faceIndex]];
			pMaterialIndices[faceIndex] = usedMaterialRefs.GetIndex(faceMaterialRef);
		}


		int newMaterialRef = m_pTupMaterialPool->AddMaterial();
		TupperwareAggregate *pNewMaterialAgg = m_pTupMaterialPool->GetMaterial(newMaterialRef);

		TupMaterial tupMaterial(pNewMaterialAgg);
		tupMaterial.SetMaterialType(TupMaterial::MULTI_SUB_OBJECT_MATERIAL);
		TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pNewMaterialAgg);

		tupMultiSubObjectMaterial.AddName(oldMaterialName);
		int numSubMaterials = usedMaterialRefs.GetSize();
		assert(numSubMaterials);
		for (int subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
		{
			TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.AddSubMaterial(subMaterialIndex);
			TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubMaterialAgg);
			tupMultiSubObjectMaterialSubMaterial.AddMaterialReference(usedMaterialRefs[subMaterialIndex]);
			// always enabled
			tupMultiSubObjectMaterialSubMaterial.AddFlags(TupMultiSubObjectMaterialSubMaterial::ENABLED);
			// we ignore the slot name for now
		}

		// mark mesh to use new material
		pMaterialRefScalar->SetAsInt(newMaterialRef);

		// remap material references on relivant nodes
		int numNodes = m_pTupNodePool->GetNumNodes();
		for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
		{
			TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
			if (!pNodeAgg)
				continue;

			TupNode tupNode(pNodeAgg);
			int nodeObjectRef;
			tupNode.GetObjectReference(nodeObjectRef);
			if (nodeObjectRef!=-1)
			{
				int baseObjectRef = AnnotateHelper::FindBaseObjectRef(*m_pTupObjectPool,nodeObjectRef);
				// if the base matches the object reference
				if (baseObjectRef == objectIndex)
				{
					int nodeMaterialRef;
					tupNode.GetMaterialReference(nodeMaterialRef);
					assert(nodeMaterialRef==materialRef);
					tupNode.AddMaterialReference(newMaterialRef); // set to new material
				}
			}
		}

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Consolidated Face Materials on %s from %d to %d sub materials",(const char *)mayaObjectName,subMaterialRefs.GetSize(),usedMaterialRefs.GetSize());
	}
}


void ProcessTup::OptimizeUVs(void)
{
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Optimizing UVs");

	int totalOffsetFaces = 0;

	int totalObjects = m_pTupObjectPool->GetNumObjects();
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
		
		// if empty slot - continue
		if (!pObjectAgg)
			continue;
		
		// it needs to be a mesh
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType() != TupObject::TRI_MESH)
			continue;

		int materialRef = pObjectAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
		// make sure it is using a material
		if (materialRef==-1)
			continue;

		// connect the faces
		TupTriMesh tupTriMesh(pObjectAgg);

		int *pMaterialIndices;
		int tempFaces;
		tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,tempFaces);
		
		TupTriMeshConnectivity tupTriMeshConnectivity;
		tupTriMeshConnectivity.SetMeshFaceFlagMask(TupTriMesh::FLAGS_FACE_Z_BIAS_MASK|TupTriMesh::FLAGS_FACE_TWO_SIDED|TupTriMesh::FLAGS_FACE_NO_Z_WRITE|TupTriMesh::FLAGS_FACE_USES_CLOTH|TupTriMesh::FLAGS_FACE_STAGE_MASK|TupTriMesh::FLAGS_FACE_NO_RECEIVE_SHADOWS);
		tupTriMeshConnectivity.SetMesh(tupTriMesh,TupTriMeshConnectivity::FLAGS_FACE_CHECK_MATERIAL_INDEX|TupTriMeshConnectivity::FLAGS_FACE_CHECK_FACE_OPACITY|TupTriMeshConnectivity::FLAGS_FACE_CHECK_NORMALS|TupTriMeshConnectivity::FLAGS_FACE_CHECK_FACE_FLAGS);

		int numMeshMaps = tupTriMeshConnectivity.GetNumMeshMaps(); // make sure there are some mesh maps to do
		if (!numMeshMaps)
			continue;

		// set up the channel to meshmap remap table
		int maxMapChannel = -1;
		int meshMapIndex;
		for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			const TupTriMeshConnectivity::MeshMap &meshMap = tupTriMeshConnectivity.GetMeshMap(meshMapIndex);
			int mapChannel = meshMap.GetMapChannel();
			if (mapChannel>maxMapChannel)
				maxMapChannel = mapChannel;
		}
		TupArray<int> channelToMeshMap;
		channelToMeshMap.SetSize(maxMapChannel+1);
		for (meshMapIndex=0;meshMapIndex<maxMapChannel;meshMapIndex++)
		{
			channelToMeshMap[meshMapIndex] = -1;
		}
		for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
		{
			const TupTriMeshConnectivity::MeshMap &meshMap = tupTriMeshConnectivity.GetMeshMap(meshMapIndex);
			int mapChannel = meshMap.GetMapChannel();
			channelToMeshMap[mapChannel] = meshMapIndex;
		}

		// set up the flags for tiling

		BuildUVTransformManager buildUVTransformManager(*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,false);
		
		TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
		assert(pMaterialAgg);
		
		TupMaterial tupMat(pMaterialAgg);

		TupArray<TupArray<UVTransform> > uvTransformArray;
		TupArray<TupArray<bool> > materialTileU;
		TupArray<TupArray<bool> > materialTileV;
			
		if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{
			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
			int totalUVTransforms = tupMultiSubObjectMaterial.GetNumSubMaterials();

			if (totalUVTransforms)
			{
				uvTransformArray.SetSize(totalUVTransforms);
				materialTileU.SetSize(totalUVTransforms);
				materialTileV.SetSize(totalUVTransforms);

				for (int materialIndex = 0;materialIndex<totalUVTransforms;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int subMaterialRef;
					tupSubMaterial.GetMaterialReference(subMaterialRef);
					buildUVTransformManager.BuildUVTransforms(subMaterialRef,uvTransformArray[materialIndex]);
					materialTileU[materialIndex].SetSize(numMeshMaps);
					materialTileV[materialIndex].SetSize(numMeshMaps);
				}
			}
			else
			{
				uvTransformArray.SetSize(1);
				uvTransformArray[0].SetSize(1);
				uvTransformArray[0][0].Init();
				materialTileU.SetSize(1);
				materialTileU[0].SetSize(numMeshMaps);
				materialTileV.SetSize(1);
				materialTileV[0].SetSize(numMeshMaps);
			}
		}
		else
		{
			uvTransformArray.SetSize(1);
			buildUVTransformManager.BuildUVTransforms(materialRef,uvTransformArray[0]);
			materialTileU.SetSize(1);
			materialTileU[0].SetSize(numMeshMaps);
			materialTileV.SetSize(1);
			materialTileV[0].SetSize(numMeshMaps);
		}

		// set bool values for each mesh map for that material index indicating if it can tile in U and V
		for (int materialIndex = 0;materialIndex<uvTransformArray.GetSize();materialIndex++)
		{
			// set defaults
			for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
			{
				materialTileU[materialIndex][meshMapIndex] = true; 
				materialTileV[materialIndex][meshMapIndex] = true;
			}

			int numUVTransforms = uvTransformArray[materialIndex].GetSize();
			for (int transformIndex=0;transformIndex<numUVTransforms;transformIndex++)
			{
				int flags = uvTransformArray[materialIndex][transformIndex].m_flags;
				int mapChannel = uvTransformArray[materialIndex][transformIndex].m_mapChannel;
				// valid channel
				if (mapChannel<TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
					continue;
				// used mesh map
				meshMapIndex = channelToMeshMap[mapChannel];
				if (meshMapIndex==-1)
					continue;

				// clear tile flags if wrapping not set
				if (flags & UVTransform::FLAG_REVERSED_IMAGE)
				{
					if ((flags & UVTransform::FLAGS_U_WRAP) == 0)
						materialTileV[materialIndex][meshMapIndex] = false;
					if ((flags & UVTransform::FLAGS_V_WRAP) == 0)
						materialTileU[materialIndex][meshMapIndex] = false;
				}
				else
				{
					if ((flags & UVTransform::FLAGS_U_WRAP) == 0)
						materialTileU[materialIndex][meshMapIndex] = false;
					if ((flags & UVTransform::FLAGS_V_WRAP) == 0)
						materialTileV[materialIndex][meshMapIndex] = false;
				}
			}
		}

		int numFaces = tupTriMeshConnectivity.GetNumFaces();
		TupArray<TupArray<float> > faceUOffsets; faceUOffsets.SetSize(numFaces);
		TupArray<TupArray<float> > faceVOffsets; faceVOffsets.SetSize(numFaces);
		TupArray<bool> faceDoneFlags; faceDoneFlags.SetSize(numFaces);

		// init face information
		int faceIndex;
		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			faceUOffsets[faceIndex].SetSize(numMeshMaps);
			faceVOffsets[faceIndex].SetSize(numMeshMaps);
			for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
			{
				faceUOffsets[faceIndex][meshMapIndex] = 0.0f;
				faceVOffsets[faceIndex][meshMapIndex] = 0.0f;
			}
			faceDoneFlags[faceIndex] = false;
		}

		int offsetFaceCount = 0;

		for (faceIndex=0;faceIndex<numFaces;faceIndex++)
		{
			if (!faceDoneFlags[faceIndex])
			{
				faceDoneFlags[faceIndex] = true;

				std::stack<StackEntry> findStack;
				
				StackEntry stackEntry(faceIndex);
				findStack.push(stackEntry);
				
				while(!findStack.empty())
				{
					// get last item off the stack
					StackEntry &entry = findStack.top(); 
					int currentFaceIndex = entry.GetFaceIndex();
					int stage = entry.GetStage();

					int nextStage = stage+1;
					if (nextStage<3)
					{
						entry.SetStage(nextStage); // set to go to the next edge on the face
					}
					else
					{
						findStack.pop(); // we are done with this face (after the following code)
					}

					TupTriMeshConnectivity::TriFace &face = tupTriMeshConnectivity.GetFace(currentFaceIndex);
					TupTriMeshConnectivity::Edge &edge = face.GetEdge(stage);
					int adjacentFaceIndex = edge.GetFaceIndex();

					int materialIndex = pMaterialIndices[currentFaceIndex];

					// test if the adjacent face can be adjusted to line up with the current face
					if ((adjacentFaceIndex!=-1) && !faceDoneFlags[adjacentFaceIndex])
					{
						assert(materialIndex==pMaterialIndices[adjacentFaceIndex]);

						bool bOffsetFace = true;

						TupArray<float> connectedFaceUOffsets;
						TupArray<float> connectedFaceVOffsets;

						connectedFaceUOffsets.SetSize(numMeshMaps);
						connectedFaceVOffsets.SetSize(numMeshMaps);

						for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
						{
							TupTriMeshConnectivity::MeshMap &meshMap = tupTriMeshConnectivity.GetMeshMap(meshMapIndex);
							TupTriMeshConnectivity::Vertex *pMeshMapVerts = meshMap.GetVerts();
							int *pMeshMapIndices = meshMap.GetMapIndices();

							int face1Pointa = stage;
							int face1Pointb = face1Pointa<2 ? face1Pointa+1 : 0;
							int face2Pointa = edge.GetPointIndex();
							int face2Pointb = face2Pointa<2 ? face2Pointa+1 : 0;

							float face1PointaU = pMeshMapVerts[pMeshMapIndices[(currentFaceIndex<<1) + currentFaceIndex + face1Pointa]].x + faceUOffsets[currentFaceIndex][meshMapIndex];
							float face1PointbU = pMeshMapVerts[pMeshMapIndices[(currentFaceIndex<<1) + currentFaceIndex + face1Pointb]].x + faceUOffsets[currentFaceIndex][meshMapIndex];
							float face2PointaU = pMeshMapVerts[pMeshMapIndices[(adjacentFaceIndex<<1) + adjacentFaceIndex + face2Pointa]].x;
							float face2PointbU = pMeshMapVerts[pMeshMapIndices[(adjacentFaceIndex<<1) + adjacentFaceIndex + face2Pointb]].x;

							float face1PointaV = pMeshMapVerts[pMeshMapIndices[(currentFaceIndex<<1) + currentFaceIndex + face1Pointa]].y + faceVOffsets[currentFaceIndex][meshMapIndex];
							float face1PointbV = pMeshMapVerts[pMeshMapIndices[(currentFaceIndex<<1) + currentFaceIndex + face1Pointb]].y + faceVOffsets[currentFaceIndex][meshMapIndex];
							float face2PointaV = pMeshMapVerts[pMeshMapIndices[(adjacentFaceIndex<<1) + adjacentFaceIndex + face2Pointa]].y;
							float face2PointbV = pMeshMapVerts[pMeshMapIndices[(adjacentFaceIndex<<1) + adjacentFaceIndex + face2Pointb]].y;

							if (CanConnect(face1PointaU,face2PointbU) && 
								 CanConnect(face1PointbU,face2PointaU) &&
								 CanConnect(face1PointaV,face2PointbV) && 
								 CanConnect(face1PointbV,face2PointaV))
							{
								float offsetUa = GetTileOffset(face1PointaU,face2PointbU);
								float offsetUb = GetTileOffset(face1PointbU,face2PointaU);
								float offsetVa = GetTileOffset(face1PointaV,face2PointbV);
								float offsetVb = GetTileOffset(face1PointbV,face2PointaV);

								if (offsetUa==offsetUb && offsetVa==offsetVb) // they line up as well as connect
								{
									// if we are going to change the U
									if (offsetUa!=0.0f)
									{
										// if we can't tile then don't do
										if (!materialTileU[materialIndex][meshMapIndex])
										{
											bOffsetFace = false;
											break;
										}
									}
									// if we are going to change the V
									if (offsetVa!=0.0f)
									{
										// if we can't tile then don't do
										if (!materialTileV[materialIndex][meshMapIndex])
										{
											bOffsetFace = false;
											break;
										}
									}
									connectedFaceUOffsets[meshMapIndex] = offsetUa;
									connectedFaceVOffsets[meshMapIndex] = offsetVa;
								}
								else
								{
									bOffsetFace = false;
									break;
								}
							}
							else
							{
								bOffsetFace = false;
								break;
							}
						}
						if (bOffsetFace)
						{
							for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
							{
								faceUOffsets[adjacentFaceIndex][meshMapIndex] = connectedFaceUOffsets[meshMapIndex];
								faceVOffsets[adjacentFaceIndex][meshMapIndex] = connectedFaceVOffsets[meshMapIndex];
							}
							offsetFaceCount++;
							faceDoneFlags[adjacentFaceIndex] = true;
							StackEntry stackEntry(adjacentFaceIndex);
							findStack.push(stackEntry);
						}
					}
				}
			}
		}
		// if we offsetted any faces
		if (offsetFaceCount)
		{
			int numFacePoints = (numFaces<<1)+numFaces;

			for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
			{
				Keeper<Vector3> newMapVerts;
				TupArray<int> newMapIndices;

				newMapIndices.SetSize(numFacePoints);

				TupTriMeshConnectivity::MeshMap &meshMap = tupTriMeshConnectivity.GetMeshMap(meshMapIndex);
				TupTriMeshConnectivity::Vertex *pMeshMapVerts = meshMap.GetVerts();
				int *pMeshMapIndices = meshMap.GetMapIndices();
				int sourceMeshMapIndex = meshMap.GetMapIndex();

				for (faceIndex=0;faceIndex<numFaces;faceIndex++)
				{
					int facePointIndex = (faceIndex<<1)+faceIndex;
					for (int pointIndex=0;pointIndex<3;pointIndex++)
					{
						Vector3 newUV;
						newUV.Set(pMeshMapVerts[pMeshMapIndices[facePointIndex+pointIndex]].x + faceUOffsets[faceIndex][meshMapIndex],
									 pMeshMapVerts[pMeshMapIndices[facePointIndex+pointIndex]].y + faceVOffsets[faceIndex][meshMapIndex],
									 0.0f);
						newMapIndices[facePointIndex+pointIndex] = newMapVerts.Add(newUV);
					}
				}
				TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(sourceMeshMapIndex);
				TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
				tupTriMeshMap.AddMapIndices(newMapIndices.GetData(),numFaces);
				tupTriMeshMap.AddMapVerts((float *)newMapVerts.GetData(),newMapVerts.GetSize());
			}
			totalOffsetFaces += offsetFaceCount;
		}
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Adjusted %d faces for better uv alignment",totalOffsetFaces);
}

void ProcessTup::CopySceneNodeSettings(void)
{
	bool bFoundSceneSettings = false;
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

		// if empty slot - continue
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);
		char *pName;
		tupNode.GetName(&pName);
		TupString name(pName);

		char *pNodeAttributesString;
		tupNode.GetUserProperties( &pNodeAttributesString );

		TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_OBJ_BLOCK, END_OBJ_BLOCK );
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey( "instance_type" );

		const int INSTANCE_TYPE = 0;
		m_pTupNodeConfigRoot->GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );

		if ( tupKeyValueContext.ValueValidAt(INSTANCE_TYPE) && strcmp( tupKeyValueContext.GetValueAsString(INSTANCE_TYPE), "scene_settings" ) == 0 )
		{
			if (!bFoundSceneSettings)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Scene Settings copied from Node %s",(const char *)name);
				m_pSceneAgg->AddScalarString("Annotate_SceneSettings",NULL,pNodeAttributesString);
				bFoundSceneSettings = true;
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Scene Settings found on more than one node - skipping %s",(const char *)name);
			}
		}
	}
}

void ProcessTup::ChangeNodeNames(void)
{
	int totalNodes = m_pTupNodePool->GetNumNodes();
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		
		// if empty slot - continue
		if (!pNodeAgg)
			continue;
		
		TupNode tupNode(pNodeAgg);
		char *pName;
		tupNode.GetName(&pName);
		TupString oldName(pName);

		char *pNodeAttributesString;
		tupNode.GetUserProperties( &pNodeAttributesString );
		
		TupKeyValue tupKeyValue( pNodeAttributesString, BEGIN_OBJ_BLOCK, END_OBJ_BLOCK );
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey( "override_instance_name" );
		tupKeyValueContext.AddKey( "instance_name" );

		m_pTupNodeConfigRoot->GetKeyValueContexts( tupKeyValue, tupKeyValueContext, false );

		if (tupKeyValueContext.ValueValidAt(0) && tupKeyValueContext.GetValueAsBool(0) && tupKeyValueContext.ValueValidAt(1))
		{
			const char *pName = tupKeyValueContext.GetValueAsString(1);
			if (strlen(pName))
			{
				tupNode.AddName(pName);
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Node %s name changed to %s",(const char *)oldName,pName);
			}
		}
	}
}

class ConsolidatedMeshSortData
{
public:
	char *m_pMayaShapeName;
	int m_totalMeshes;
	int m_totalVertexColors;
	int m_totalInstances;
	int m_totalImages;
	int m_totalImageSize;
	int m_numFaces;
	
	ConsolidatedMeshSortData(void) : m_pMayaShapeName(NULL),m_totalMeshes(0),m_totalVertexColors(0),m_totalInstances(0),m_numFaces(0)
	{
	}
	
	bool ConsolidatedMeshSortData::operator==(const ConsolidatedMeshSortData& other) const
	{
		return (m_totalImageSize==other.m_totalImageSize);
	}
	
	bool ConsolidatedMeshSortData::operator<=(const ConsolidatedMeshSortData& other) const
	{
		return (other.m_totalImageSize<=m_totalImageSize);
	}
	
	bool ConsolidatedMeshSortData::operator>=(const ConsolidatedMeshSortData& other) const
	{
		return (other.m_totalImageSize>=m_totalImageSize);
	}
	
	bool ConsolidatedMeshSortData::operator<(const ConsolidatedMeshSortData& other) const
	{
		return (other.m_totalImageSize<m_totalImageSize);
	}
	
	bool ConsolidatedMeshSortData::operator>(const ConsolidatedMeshSortData& other) const
	{
		return (other.m_totalImageSize>m_totalImageSize);
	}
};


void ProcessTup::AddConsolidatedInfoToLog(void)
{
	TupArray<ConsolidatedMeshSortData> sortedMeshes;

	int totalInstances = 0;
	int totalMeshes = 0;
	int totalMeshFaces = 0;
	int totalInstanceFaces = 0;

	int totalObjects = m_pTupObjectPool->GetNumObjects();
	sortedMeshes.SetSize(totalObjects);
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
		
		// if empty slot - continue
		if (!pObjectAgg)
			continue;

		// is this a valid render mesh somewhere?
		TupperwareList *pRenderPartitionSOMStartsList = pObjectAgg->FindListByKey("Annotate_RenderPartitionSOMStarts");
		if (!pRenderPartitionSOMStartsList)
			continue;
		
		// it needs to be a mesh
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			TupTriMesh tupTriMesh(pObjectAgg);
			int *pFaceFlags;
			int numFaces;
			tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);
			sortedMeshes[objectIndex].m_numFaces=numFaces;

			TupperwareScalar *pMayaObjectNameScalar = pObjectAgg->FindScalarByKey("MayaObjectName");
			if (pMayaObjectNameScalar)
			{
				sortedMeshes[objectIndex].m_pMayaShapeName = pMayaObjectNameScalar->GetAsString();
				sortedMeshes[objectIndex].m_totalMeshes=1;
				for (TupperwareIterator iter = pObjectAgg->FindByKey("MayaConsolidatedObjectName"); !iter.Finished(); iter.Next()) 
				{
					sortedMeshes[objectIndex].m_totalMeshes++;
				}
				TupperwareList *pParentNodeReferenceList = pObjectAgg->FindListByKey("Annotate_BaseObjectParentNodeReferenceList");
				if (pParentNodeReferenceList)
				{
					sortedMeshes[objectIndex].m_totalInstances = pParentNodeReferenceList->GetLength();
				}
			}
			totalMeshes++;
			totalInstances += sortedMeshes[objectIndex].m_totalInstances;
			totalMeshFaces += numFaces;
			totalInstanceFaces += numFaces * sortedMeshes[objectIndex].m_totalInstances;

			sortedMeshes[objectIndex].m_totalImages = 0;
			sortedMeshes[objectIndex].m_totalImageSize = 0;
		

			TupperwareList *pMeshImageRefList = pObjectAgg->FindListByKey("Annotate_MeshImageRefs");
			assert(pMeshImageRefList);
			int numImageRefs = pMeshImageRefList->GetLength();
			int *pImageRefData = pMeshImageRefList->GetAsInt();
			for (int imageRefIndex=0;imageRefIndex<numImageRefs;imageRefIndex++)
			{
				int ref = pImageRefData[imageRefIndex];
				if (ref!=-1)
				{
					sortedMeshes[objectIndex].m_totalImages++;

					TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(ref);
					assert(pImageAgg);
					sortedMeshes[objectIndex].m_totalImageSize += pImageAgg->FindScalarByKey("Annotate_ImageSize")->GetAsInt();
					sortedMeshes[objectIndex].m_totalImageSize += pImageAgg->FindScalarByKey("Annotate_ImageSizeMipMaps")->GetAsInt();
				}
			}
			// set number of vertex colors sets
			TupperwareList *pVertexColorRefList = pObjectAgg->FindListByKey("Annotate_VertexColorRefList");
			TupperwareList *pVertexAlphaRefList = pObjectAgg->FindListByKey("Annotate_VertexAlphaRefList");
			if (pVertexColorRefList && pVertexAlphaRefList)
			{
				int *pVertexColorRefData = pVertexColorRefList->GetAsInt();
				int *pVertexAlphaRefData = pVertexAlphaRefList->GetAsInt();
				int numColorSets = pVertexColorRefList->GetLength();
				// check if all references are pointing to a -1 reference (standard white)
				if (numColorSets==1 && pVertexColorRefData[0] == -1 && pVertexAlphaRefData[0] == -1)
				{
					// lets say there are no vertex color sets
					numColorSets = 0;
				}
				sortedMeshes[objectIndex].m_totalVertexColors=numColorSets;
			}
		}
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Mesh Texture and Consolidation Report:");
	TupSort<ConsolidatedMeshSortData>::SortAscending(sortedMeshes.GetData(),totalObjects);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"           Texture");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW," #Textures   Size    #Instances #Meshes #Colors #Faces    Source Maya Shape");
	for (objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		// if any instances for this
		if (sortedMeshes[objectIndex].m_totalInstances)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"  %4d      %6d     %4d     %4d    %4d  %6d    %s",
				sortedMeshes[objectIndex].m_totalImages,sortedMeshes[objectIndex].m_totalImageSize,
				sortedMeshes[objectIndex].m_totalInstances,sortedMeshes[objectIndex].m_totalMeshes,
				sortedMeshes[objectIndex].m_totalVertexColors,sortedMeshes[objectIndex].m_numFaces,
				sortedMeshes[objectIndex].m_pMayaShapeName);
		}
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW," Total Instances %d",totalInstances);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW," Total Meshes %d",totalMeshes);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW," Total Mesh Faces %d",totalMeshFaces);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW," Total Instance Faces %d",totalInstanceFaces);
}

class TextureCombiningMeshSortData
{
public:
	char *m_pMayaShapeName;
	int m_totalImages;
	int m_totalImageSize;
	int m_totalCombinedImagesSize;
	int m_percentChange;
	int m_numFaces;
	int m_maxUTiles;
	int m_maxVTiles;
	
	TextureCombiningMeshSortData(void) : 
		m_pMayaShapeName(NULL),
		m_totalImages(0),
		m_totalImageSize(0),
		m_totalCombinedImagesSize(0),
		m_percentChange(0),
		m_maxUTiles(0),
		m_maxVTiles(0),
		m_numFaces(0)
	{
	}
	
	bool TextureCombiningMeshSortData::operator==(const TextureCombiningMeshSortData& other) const
	{
		return (m_percentChange==other.m_percentChange && m_numFaces == other.m_numFaces);
	}

	bool TextureCombiningMeshSortData::operator<=(const TextureCombiningMeshSortData& other) const
	{
		if (m_percentChange>other.m_percentChange)
			return false;
		else if (m_percentChange<other.m_percentChange)
			return true;
		// m_percentQualify ==
		// return true if path is less than or equal other
		return (m_numFaces>=other.m_numFaces);
	}

	bool TextureCombiningMeshSortData::operator>=(const TextureCombiningMeshSortData& other) const
	{
		if (m_percentChange<other.m_percentChange)
			return false;
		else if (m_percentChange>other.m_percentChange)
			return true;
		// m_percentQualify ==
		// return true if path is greater than or equal other
		return (m_numFaces<=other.m_numFaces);
	}

	bool TextureCombiningMeshSortData::operator<(const TextureCombiningMeshSortData& other) const
	{
		if (m_percentChange>other.m_percentChange)
			return false;
		else if (m_percentChange<other.m_percentChange)
			return true;
		// m_percentQualify ==
		// return true if path is greater than or equal other
		return (m_numFaces>other.m_numFaces);
	}

	bool TextureCombiningMeshSortData::operator>(const TextureCombiningMeshSortData& other) const
	{
		if (m_percentChange<other.m_percentChange)
			return false;
		else if (m_percentChange>other.m_percentChange)
			return true;
		// m_percentQualify ==
		// return true if path is greater than or equal other
		return (m_numFaces<other.m_numFaces);
	}
};

void ProcessTup::AddTextureCombiningInfoToLog(void)
{
	TupArray<TextureCombiningMeshSortData> sortedCombineInfo;

	int totalObjects = m_pTupObjectPool->GetNumObjects();
	sortedCombineInfo.SetSize(totalObjects);
	for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);
		
		// if empty slot - continue
		if (!pObjectAgg)
			continue;

		// is this a valid render mesh somewhere?
		TupperwareList *pRenderPartitionSOMStartsList = pObjectAgg->FindListByKey("Annotate_RenderPartitionSOMStarts");
		if (!pRenderPartitionSOMStartsList)
			continue;
		
		// it needs to be a mesh
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::TRI_MESH)
		{
			int materialRef = pObjectAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
			// make sure it is using a material
			if (materialRef==-1)
				continue;

			TupTriMesh tupTriMesh(pObjectAgg);
			int *pFaceFlags;
			int numFaces;
			tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);
			sortedCombineInfo[objectIndex].m_numFaces=numFaces;

			int *pMaterialIndices;
			int tempFaces;
			tupTriMesh.GetFaceMaterialIndices(&pMaterialIndices,tempFaces);

			TupperwareScalar *pMayaObjectNameScalar = pObjectAgg->FindScalarByKey("MayaObjectName");
			if (pMayaObjectNameScalar)
			{
				sortedCombineInfo[objectIndex].m_pMayaShapeName = pMayaObjectNameScalar->GetAsString();
			}

			TupperwareList *pMeshImageRefList = pObjectAgg->FindListByKey("Annotate_MeshImageRefs");
			int numImageRefs = pMeshImageRefList->GetLength();
			int *pImageRefData = pMeshImageRefList->GetAsInt();
			Keeper<int> meshImageRefs;
			TupArray<int> meshImageSizes;
			TupArray<int> meshImageMaxTileU;
			TupArray<int> meshImageMaxTileV;

			for (int imageRefIndex=0;imageRefIndex<numImageRefs;imageRefIndex++)
			{
				int ref = pImageRefData[imageRefIndex];
				if (ref!=-1)
				{
					int newRefIndex = meshImageRefs.Add(ref);

					sortedCombineInfo[objectIndex].m_totalImages++;

					TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(ref);
					assert(pImageAgg);
					int imageSize = pImageAgg->FindScalarByKey("Annotate_ImageSize")->GetAsInt();
					imageSize += pImageAgg->FindScalarByKey("Annotate_ImageSizeMipMaps")->GetAsInt();

					sortedCombineInfo[objectIndex].m_totalImageSize += imageSize;
					meshImageSizes.Add(imageSize);
					int filler = 0;
					meshImageMaxTileU.Add(filler);
					meshImageMaxTileV.Add(filler);
				}
			}

			if (sortedCombineInfo[objectIndex].m_totalImages == 0)
				continue;

			TupTriMeshConnectivity tupTriMeshConnectivity;
			tupTriMeshConnectivity.SetMeshFaceFlagMask(TupTriMesh::FLAGS_FACE_Z_BIAS_MASK|TupTriMesh::FLAGS_FACE_TWO_SIDED|TupTriMesh::FLAGS_FACE_NO_Z_WRITE|TupTriMesh::FLAGS_FACE_USES_CLOTH|TupTriMesh::FLAGS_FACE_STAGE_MASK|TupTriMesh::FLAGS_FACE_NO_RECEIVE_SHADOWS);
			tupTriMeshConnectivity.SetMesh(tupTriMesh,TupTriMeshConnectivity::FLAGS_FACE_CHECK_MATERIAL_INDEX|TupTriMeshConnectivity::FLAGS_FACE_CHECK_FACE_OPACITY|TupTriMeshConnectivity::FLAGS_FACE_CHECK_NORMALS|TupTriMeshConnectivity::FLAGS_FACE_CHECK_FACE_FLAGS);

			int numMeshMaps = tupTriMeshConnectivity.GetNumMeshMaps(); // make sure there are some mesh maps to do
			if (!numMeshMaps)
				continue;

			// set up the channel to meshmap remap table
			int maxMapChannel = -1;
			int meshMapIndex;
			for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
			{
				const TupTriMeshConnectivity::MeshMap &meshMap = tupTriMeshConnectivity.GetMeshMap(meshMapIndex);
				int mapChannel = meshMap.GetMapChannel();
				if (mapChannel>maxMapChannel)
					maxMapChannel = mapChannel;
			}
			TupArray<int> channelToMeshMap;
			channelToMeshMap.SetSize(maxMapChannel+1);
			for (meshMapIndex=0;meshMapIndex<maxMapChannel;meshMapIndex++)
			{
				channelToMeshMap[meshMapIndex] = -1;
			}
			for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
			{
				const TupTriMeshConnectivity::MeshMap &meshMap = tupTriMeshConnectivity.GetMeshMap(meshMapIndex);
				int mapChannel = meshMap.GetMapChannel();
				channelToMeshMap[mapChannel] = meshMapIndex;
			}

			// set up the flags for tiling

			BuildUVTransformManager buildUVTransformManager(*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,false);
			
			TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);
			assert(pMaterialAgg);
			
			TupMaterial tupMat(pMaterialAgg);

			TupArray<TupArray<UVTransform> > uvTransformArray;
			TupArray<TupArray<bool> > materialTileU;
			TupArray<TupArray<bool> > materialTileV;
				
			if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
			{
				// we need to deal with multiple materials
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
				int totalUVTransforms = tupMultiSubObjectMaterial.GetNumSubMaterials();

				if (totalUVTransforms)
				{
					uvTransformArray.SetSize(totalUVTransforms);
					materialTileU.SetSize(totalUVTransforms);
					materialTileV.SetSize(totalUVTransforms);

					for (int materialIndex = 0;materialIndex<totalUVTransforms;materialIndex++)
					{
						TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
						TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
						int subMaterialRef;
						tupSubMaterial.GetMaterialReference(subMaterialRef);
						buildUVTransformManager.BuildUVTransforms(subMaterialRef,uvTransformArray[materialIndex]);
						materialTileU[materialIndex].SetSize(numMeshMaps);
						materialTileV[materialIndex].SetSize(numMeshMaps);
					}
				}
				else
				{
					uvTransformArray.SetSize(1);
					uvTransformArray[0].SetSize(1);
					uvTransformArray[0][0].Init();
					materialTileU.SetSize(1);
					materialTileU[0].SetSize(numMeshMaps);
					materialTileV.SetSize(1);
					materialTileV[0].SetSize(numMeshMaps);
				}
			}
			else
			{
				uvTransformArray.SetSize(1);
				buildUVTransformManager.BuildUVTransforms(materialRef,uvTransformArray[0]);
				materialTileU.SetSize(1);
				materialTileU[0].SetSize(numMeshMaps);
				materialTileV.SetSize(1);
				materialTileV[0].SetSize(numMeshMaps);
			}

			// set bool values for each mesh map for that material index indicating if it can tile in U and V
			for (int materialIndex = 0;materialIndex<uvTransformArray.GetSize();materialIndex++)
			{
				// set defaults
				for (meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
				{
					materialTileU[materialIndex][meshMapIndex] = true; 
					materialTileV[materialIndex][meshMapIndex] = true;
				}

				int numUVTransforms = uvTransformArray[materialIndex].GetSize();
				for (int transformIndex=0;transformIndex<numUVTransforms;transformIndex++)
				{
					int flags = uvTransformArray[materialIndex][transformIndex].m_flags;
					int mapChannel = uvTransformArray[materialIndex][transformIndex].m_mapChannel;
					// valid channel
					if (mapChannel<TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
						continue;
					// used mesh map
					meshMapIndex = channelToMeshMap[mapChannel];
					if (meshMapIndex==-1)
						continue;

					// clear tile flags if wrapping not set
					if (flags & UVTransform::FLAG_REVERSED_IMAGE)
					{
						if ((flags & UVTransform::FLAGS_U_WRAP) == 0)
							materialTileV[materialIndex][meshMapIndex] = false;
						if ((flags & UVTransform::FLAGS_V_WRAP) == 0)
							materialTileU[materialIndex][meshMapIndex] = false;
					}
					else
					{
						if ((flags & UVTransform::FLAGS_U_WRAP) == 0)
							materialTileU[materialIndex][meshMapIndex] = false;
						if ((flags & UVTransform::FLAGS_V_WRAP) == 0)
							materialTileV[materialIndex][meshMapIndex] = false;
					}
				}
			}

			// init face information
			int faceIndex, faceIndex3;
			for (faceIndex=0,faceIndex3=0;faceIndex<numFaces;faceIndex++,faceIndex3+=3)
			{
				int materialIndex = pMaterialIndices[faceIndex];

				int numUVTransforms = uvTransformArray[materialIndex].GetSize();
				for (int transformIndex=0;transformIndex<numUVTransforms;transformIndex++)
				{
					UVTransform &uvTransform = uvTransformArray[materialIndex][transformIndex];
					int flags = uvTransform.m_flags;
					int mapChannel = uvTransformArray[materialIndex][transformIndex].m_mapChannel;
					// valid channel
					if (mapChannel<TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
						continue;
					// used mesh map
					meshMapIndex = channelToMeshMap[mapChannel];
					if (meshMapIndex==-1)
						continue;

					TupTriMeshConnectivity::MeshMap &meshMap = tupTriMeshConnectivity.GetMeshMap(meshMapIndex);
					TupTriMeshConnectivity::Vertex *pMeshMapVerts = meshMap.GetVerts();
					int *pMeshMapIndices = meshMap.GetMapIndices();

					int maxUTiles = 0;
					if (flags & UVTransform::FLAGS_U_WRAP)
					{
						float pixelAdd = 0.5f/(float)(uvTransform.m_imageWidth);
						float facePointaU = pMeshMapVerts[pMeshMapIndices[faceIndex3 + 0]].x;
						float facePointbU = pMeshMapVerts[pMeshMapIndices[faceIndex3 + 1]].x;
						float facePointcU = pMeshMapVerts[pMeshMapIndices[faceIndex3 + 2]].x;

						float minU = facePointaU<facePointbU ? facePointaU : facePointbU;
						minU = minU<facePointcU ? minU : facePointcU;

						float maxU = facePointaU>facePointbU ? facePointaU : facePointbU;
						maxU = maxU>facePointcU ? maxU : facePointcU;

						if (minU<0.0f)
							minU = (float)((int)(minU+pixelAdd)-1.0f);
						else
							minU = (float)((int)(minU+pixelAdd));

						float deltaU = maxU-minU;
						while(deltaU>(1.00f+pixelAdd))
						{
							maxUTiles++;
							deltaU--;
						}
					}

					int maxVTiles = 0;
					if (flags & UVTransform::FLAGS_V_WRAP)
					{
						float pixelAdd = 0.5f/(float)(uvTransform.m_imageHeight);

						float facePointaV = pMeshMapVerts[pMeshMapIndices[faceIndex3 + 0]].y;
						float facePointbV = pMeshMapVerts[pMeshMapIndices[faceIndex3 + 1]].y;
						float facePointcV = pMeshMapVerts[pMeshMapIndices[faceIndex3 + 2]].y;

						float minV = facePointaV<facePointbV ? facePointaV : facePointbV;
						minV = minV<facePointcV ? minV : facePointcV;

						float maxV = facePointaV>facePointbV ? facePointaV : facePointbV;
						maxV = maxV>facePointcV ? maxV : facePointcV;

						if (minV<0.0f)
							minV = (float)((int)(minV+pixelAdd)-1.0f);
						else
							minV = (float)((int)(minV+pixelAdd));

						float deltaV = maxV-minV;
						while(deltaV>(1.00f+pixelAdd))
						{
							maxVTiles++;
							deltaV--;
						}
					}

					int meshImageIndex = meshImageRefs.GetIndex(uvTransformArray[materialIndex][transformIndex].m_imageRef);
					if (meshImageMaxTileU[meshImageIndex]<maxUTiles)
						meshImageMaxTileU[meshImageIndex] = maxUTiles;
					if (meshImageMaxTileV[meshImageIndex]<maxVTiles)
						meshImageMaxTileV[meshImageIndex] = maxVTiles;
				}
			}
			for (int statsIndex = 0; statsIndex < meshImageRefs.GetSize(); statsIndex++)
			{
				sortedCombineInfo[objectIndex].m_totalCombinedImagesSize += (meshImageSizes[statsIndex]*(1+meshImageMaxTileU[statsIndex])*(1+meshImageMaxTileV[statsIndex]));
				if (sortedCombineInfo[objectIndex].m_maxUTiles<meshImageMaxTileU[statsIndex])
					sortedCombineInfo[objectIndex].m_maxUTiles = meshImageMaxTileU[statsIndex];
				if (sortedCombineInfo[objectIndex].m_maxVTiles<meshImageMaxTileV[statsIndex])
					sortedCombineInfo[objectIndex].m_maxVTiles = meshImageMaxTileV[statsIndex];
			}
			sortedCombineInfo[objectIndex].m_percentChange = (100*sortedCombineInfo[objectIndex].m_totalCombinedImagesSize)/sortedCombineInfo[objectIndex].m_totalImageSize;
		}
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Mesh Texture Combining Stats Report:");
	TupSort<TextureCombiningMeshSortData>::SortAscending(sortedCombineInfo.GetData(),totalObjects);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"           Texture    Combined   Percent    Max U   Max V       ");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW," #Textures   Size       Size     Increase  Tiling  Tiling   #Faces    Source Maya Shape");

	for (objectIndex=0;objectIndex<totalObjects;objectIndex++)
	{
		// if any instances for this
		if (sortedCombineInfo[objectIndex].m_totalImages>1)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"  %4d      %6d     %6d      %3d     %3d     %3d      %6d    %s",
				sortedCombineInfo[objectIndex].m_totalImages,sortedCombineInfo[objectIndex].m_totalImageSize,
				sortedCombineInfo[objectIndex].m_totalCombinedImagesSize,sortedCombineInfo[objectIndex].m_percentChange,
				sortedCombineInfo[objectIndex].m_maxUTiles,sortedCombineInfo[objectIndex].m_maxVTiles,
				sortedCombineInfo[objectIndex].m_numFaces,
				sortedCombineInfo[objectIndex].m_pMayaShapeName);
		}
	}
}

void ProcessTup::SetMeshRecieveNoShadowFlags(void)
{
	// do only on the gamecube
	if( (stricmp(m_platform,"GCN") != 0)  &&  (stricmp( m_platform, "DX") != 0)  &&  (stricmp( m_platform, "XBOX") != 0) )
		return;

	// keep track if we have a valid threshold (if we don't then set set the flags to not receive shadows)
	bool bValidThreshold = (m_receiveShadowNormalThreshold==-1.0f) ? false : true;

	int numNodes = m_pTupNodePool->GetNumNodes();

	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);
		int objectRef;
		tupNode.GetObjectReference(objectRef);
		if (objectRef==-1)
			continue;

		TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(*m_pTupObjectPool,objectRef);
		if (!pBaseMeshAgg)
			continue;

		int numBonesOnMesh=0;
		TupperwareAggregate *pBoneMeshAgg = pBaseMeshAgg->FindAggregateByKey("Annotate_BoneMeshData");
		if (pBoneMeshAgg)
		{
			numBonesOnMesh = pBoneMeshAgg->FindScalarByKey("NumBones")->GetAsInt();
		}

		TupperwareAggregate *pTransformAgg = tupNode.GetTransform();
		TupTransform tupTransform(pTransformAgg);
		if (tupTransform.GetTransformType()!=TupTransform::STATIC_MATRIX)
			continue;

		Graphics4x4 *pMatrix;
		TupStaticMatrix tupStaticMatrix(pTransformAgg);
		tupStaticMatrix.GetMatrix((float **)&pMatrix);
		Graphics4x4 transformMatrix = *pMatrix;
		transformMatrix.tx = 0.0f;
		transformMatrix.ty = 0.0f;
		transformMatrix.tz = 0.0f;

		TupTriMesh tupTriMesh(pBaseMeshAgg);

		int *pFaceFlags;
		int numFaces;
		tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);

		bool bAllNoReceive = false;
		// if mesh indicated to not get shadows then don't set the flag
		TupperwareScalar *pForceNoReceiveShadowsScalar = pBaseMeshAgg->FindScalarByKey("Annotate_ForceNoReceiveShadows");
		if (pForceNoReceiveShadowsScalar && pForceNoReceiveShadowsScalar->GetAsInt() == 1)
			bAllNoReceive = true;

		// if more than one bone or we don't have a valid threshold then mark them all to not receive shadoes
		if (numBonesOnMesh>1 || !bValidThreshold || bAllNoReceive)
		{
			for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				pFaceFlags[faceIndex] |= TupTriMesh::FLAGS_FACE_NO_RECEIVE_SHADOWS;
			}
		}
		else
		{
			Vector3 *pFaceNormalVerts;
			int numFaceNormalVerts;
			tupTriMesh.GetFaceNormalVerts((float **)&pFaceNormalVerts,numFaceNormalVerts);
			int *pFaceNormalIndices;
			tupTriMesh.GetFaceNormalIndices(&pFaceNormalIndices,numFaces);

			TupArray<Vector3> transformedVerts;
			transformedVerts.SetSize(numFaceNormalVerts);
			transformMatrix.TransformArray(pFaceNormalVerts,transformedVerts.GetData(),numFaceNormalVerts);

			for (int faceNormalVertIndex=0;faceNormalVertIndex<numFaceNormalVerts;faceNormalVertIndex++)
			{
				transformedVerts[faceNormalVertIndex].SafeNormalize();
			}

			for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				// if not set then check
				if (!(pFaceFlags[faceIndex] & TupTriMesh::FLAGS_FACE_NO_RECEIVE_SHADOWS))
				{
					// if normal is facing below the threshold then set the flag
					if (transformedVerts[pFaceNormalIndices[faceIndex]].Y()<m_receiveShadowNormalThreshold)
						pFaceFlags[faceIndex] |= TupTriMesh::FLAGS_FACE_NO_RECEIVE_SHADOWS;
				}
			}
		}
	}
}

void ProcessTup::AdjustVertexColorGamma(void)
{
	if (!m_bAdjustVertexColorGamma)
		return;

	TupArray<float> redGammaTable;
	TupArray<float> greenGammaTable;
	TupArray<float> blueGammaTable;

	redGammaTable.SetSize(256);
	greenGammaTable.SetSize(256);
	blueGammaTable.SetSize(256);

	int colorIndex;
	for (colorIndex=0;colorIndex<256;colorIndex++)
	{
		float inputValue = ((float)colorIndex)/255.0f;
		redGammaTable[colorIndex] = Math::Pow(inputValue,1.0f/m_redVertexColorGamma);
		greenGammaTable[colorIndex] = Math::Pow(inputValue,1.0f/m_greenVertexColorGamma);
		blueGammaTable[colorIndex] = Math::Pow(inputValue,1.0f/m_blueVertexColorGamma);
	}

	int numModifiers = m_pTupModifierPool->GetNumModifiers();

	for (int modifierIndex=0;modifierIndex<numModifiers;modifierIndex++)
	{
		TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierIndex);
		if (!pModifierAgg)
			continue;

		TupModifier tupModifier(pModifierAgg);
		// works only on apply vc modifier (the only way Maya does vertex colors in the Tup)
		if (tupModifier.GetModifierType()==TupModifier::APPLY_VC_MODIFIER)
		{
			TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
			int flags;
			tupApplyVCModifier.GetFlags(flags);
			if (flags & TupApplyVCModifier::FLAGS_ENABLED)
			{
				// get the modifier information
				int numModifierVerts;
				Vector3 *pModifierVerts;
				tupApplyVCModifier.GetColorVerts((float **)&pModifierVerts,numModifierVerts);

				for (colorIndex=0;colorIndex<numModifierVerts;colorIndex++)
				{
					pModifierVerts[colorIndex].X(redGammaTable[(u8)(pModifierVerts[colorIndex].X()*255.0f)]);
					pModifierVerts[colorIndex].Y(greenGammaTable[(u8)(pModifierVerts[colorIndex].Y()*255.0f)]);
					pModifierVerts[colorIndex].Z(blueGammaTable[(u8)(pModifierVerts[colorIndex].Z()*255.0f)]);
				}
			}
		}
	}
}

void ProcessTup::RemovePinkVertexColors(void)
{
	if (!m_bRemovePinkVertexColors)
		return;

	int numModifiers = m_pTupModifierPool->GetNumModifiers();

	for (int modifierIndex=0;modifierIndex<numModifiers;modifierIndex++)
	{
		TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(modifierIndex);
		if (!pModifierAgg)
			continue;

		TupModifier tupModifier(pModifierAgg);
		// works only on apply vc modifier (the only way Maya does vertex colors in the Tup)
		if (tupModifier.GetModifierType()==TupModifier::APPLY_VC_MODIFIER)
		{
			TupApplyVCModifier tupApplyVCModifier(pModifierAgg);
			int flags;
			tupApplyVCModifier.GetFlags(flags);
			if (flags & TupApplyVCModifier::FLAGS_ENABLED)
			{
				// get the modifier information
				int numModifierVerts;
				Vector3 *pModifierVerts;
				tupApplyVCModifier.GetColorVerts((float **)&pModifierVerts,numModifierVerts);

				// if one vert and it is pink turn to white
				if (numModifierVerts==1 && 
					 pModifierVerts[0].X() == 1.0f &&
					 pModifierVerts[0].Y() == 0.0f &&
					 pModifierVerts[0].Z() == 1.0f)
				{
					pModifierVerts[0].Y(1.0f);
				}
			}
		}
	}
}
