////////////////////////////////////////////////////////////////////////////
//
// ProcessTup
//
// Class to handle processing the tupperware data and adding any special information that is needed for export
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef PROCESS_TUP_H
#define PROCESS_TUP_H

class FilterOptions;
class FilterDataManager;
class TupScene;
class TupNodePool;
class TupObjectPool;
class TupNode;
class TupMaterialPool;
class TupModifierPool;
//class Vector3;

class ProcessTup
{
	class StackEntry
	{
		int m_faceIndex;
		int m_stage;
	public:
		StackEntry(void) : m_faceIndex(-1), m_stage(0) { }
		StackEntry(int faceIndex) : m_faceIndex(faceIndex), m_stage(0) { }
		int GetFaceIndex(void) const { return m_faceIndex; }
		int GetStage(void) const { return m_stage; }
		
		void SetFaceIndex(int faceIndex) { m_faceIndex = faceIndex; }
		void SetStage(int stage) { m_stage = stage; }
	};

	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupArray<int> m_totalSOMS;
	TupArray<int> m_totalRenderInstances;
	TupArray<int> m_totalCollisionInstances;
	TupArray<int> m_totalTextureContexts;
	TupArray<int> m_totalTextures;
	int m_totalPaths;
	int m_totalBoneMeshes;
	TupConfigRoot *m_pTupNodeConfigRoot;
	TupConfigRoot *m_pTupFaceConfigRoot;
	TupConfigRoot *m_pTupMapConfigRoot;

	int m_rootNodeReference;

	// these point into the tree
	TupperwareAggregate *m_pSceneAgg;
	TupperwareAggregate *m_pNodePoolAgg;
	TupperwareAggregate *m_pRootNodeAgg;
	TupperwareAggregate *m_pObjectPoolAgg;
	TupperwareAggregate *m_pModifierPoolAgg;
	TupperwareAggregate *m_pMaterialPoolAgg;
	TupperwareAggregate *m_pMapPoolAgg;
	TupperwareAggregate *m_pImagePoolAgg;

	// I own these
	TupScene *m_pTupScene;
	TupNodePool *m_pTupNodePool;
	TupNode *m_pTupRootNode;
	TupObjectPool *m_pTupObjectPool;
	TupModifierPool *m_pTupModifierPool;
	TupMaterialPool *m_pTupMaterialPool;
	TupMapPool *m_pTupMapPool;
	TupImagePool *m_pTupImagePool;

	bool m_bAddVertexColorsRefsToMesh;
	bool m_bForceDynamicTextures;
	bool m_bBuildPartitions;
	bool m_bOptimizeUVs;
	bool m_bClearTwoSidedFaceFlags;
	bool m_bRemovePinkVertexColors;
	TupString m_platform;
	float m_receiveShadowNormalThreshold;
	bool m_bAdjustVertexColorGamma;
	float m_redVertexColorGamma;
	float m_greenVertexColorGamma;
	float m_blueVertexColorGamma;
	bool m_bGlobalMipMapping;
	bool m_bGlobalMipMapCrossFade;
	int m_globalNumMipMapSubMaps;
	float m_globalMipMapDistance;
	bool m_bLoadAuxMapAttributes;
	bool m_bSaveAuxImageAttributes;
	bool m_bSetTransparentAndObscuredStageFacesAsHidden;
	bool m_bForceBonesToAttachPoints;
   bool m_bRotateTallImages;
   bool m_bRemapDuplicateImages;

	TupArray<int> m_visibilitySetNodeRefs;
	TupArray<TupString> m_visibilitySetNodeNames;
	TupArray<ts_BoundingBox> m_visibilitySetBoundingBoxes;
	TupArray<OBBInstance> m_visibilitySetOBBs;
	
	ProcessTup(void); // this is not defined

public:
	ProcessTup(const FilterOptions &options,FilterDataManager &dataManager,bool bProcessStep);
	~ProcessTup();
	void Process(int pass);
	void GetFileDependencies(FilePathArray &filePathArray);

	void SetForceDynamicTextures(bool bValue);
	void SetBuildPartitions(bool bValue);
	void SetOptimizeUVs(bool bValue);
	void SetClearTwoSidedFaceFlags(bool bValue);
	void SetRemovePickVertexColors(bool bValue);
	void SetPlatform(const char *pPlatform);
	void SetReceiveShadowNormalThreshold(float threshold);
	void SetAdjustVertexColorGamma(bool bValue);
	void SetRedVertexColorGamma(float value);
	void SetGreenVertexColorGamma(float value);
	void SetBlueVertexColorGamma(float value);
	void SetGlobalMipMapping(bool bValue);
	void SetGlobalMipMapCrossFade(bool bValue);
	void SetGlobalNumMipMapSubMaps(int value);
	void SetGlobalMipMapDistance(float value);
	void SetLoadAuxMapAttributes(bool bValue);
	void SetSaveAuxImageAttributes(bool bValue);
	void SetTransparentAndObscuredStageFacesAsHiddenValue(bool bValue);
	void SetForceBonesToAttachPoints(bool bValue);
   void SetRotateTallImages(bool bValue);
   void SetRemapDuplicateImages(bool bValue);

	void RemovePinkVertexColors(void);
	
private:
	// file dependencies methods
	void AddImageFileDependencies(FilePathArray &filePathArray);

	// process methods
	void AddObjectMaterialLists(void);
	void AddObjectMaterialLists(int objectRef,int materialRef);

	void SetUpVisibilitySets(void);
	int CreateWorldVisibilitySet(void);
	void AddVisibilitySetRefToInstances(void);
	void AddCollisionInstanceIndexes(void);
	void AddPartionDataToObjectsAndInstances(void);
	void AdjustVisibilitySetBoundingBoxes(void);

	void AddTextureContextIndexes(void);
	void AddTextureIndexes(void);
	void AddMaterialHasTextureFlags(void);

	void AddAuxMapAttributes(void);
	void PropagateMapAttributesToImages(void);
	void SaveAuxImageAttributes(void);

	void AddMapAttributes(void);
	void AddImageAttributes(void);
	void LoadImageData(void);

	void AddImageData(void);
	void AddAnimatedImagesToLog(void);
	void AddSortedImagesToLog(void);
	void FixDiffuseMapImages(void);
	void RotateTallImages(void);
	void ScaleNarrowImages(void);
	void ScaleNonPowerOf2Images(void);
	void AddParentObjectReferences(void);
	void AddParentNodeReferences(void);
	void AddBaseObjectParentNodeReferences(void);
	void AddNodePathIndexes(void);

	void PropagateVertexColors(bool bForceDefault);
	void PropagateVertexColors(int objectRef);
	void ApplyApplyVCModifierForVertexColor(TupTriMesh &tupBaseMesh,TupModifier &tupModifier,TupperwareAggregate *pModifierContextAgg,TupTriMeshMap &tupTriMeshMap);
	void ApplyVertexPaintModifierForVertexColor(TupTriMesh &tupBaseMesh,TupModifier &tupModifier,TupperwareAggregate *pModifierContextAgg,TupTriMeshMap &tupTriMeshMap);
	void ApplyHoldVCModifierForVertexColor(TupTriMesh &tupBaseMesh,TupModifier &tupModifier,TupperwareAggregate *pModifierContextAgg,TupTriMeshMap &tupTriMeshMap);
	void AddObjectModelAttributes(void);
	void MarkEnvInstances(void);

	void PropagateVertexAlpha(bool bForceDefault);
	void PropagateVertexAlpha(int objectRef);
	void ApplyApplyVCModifierForVertexAlpha(TupTriMesh &tupBaseMesh,TupModifier &tupModifier,TupperwareAggregate *pModifierContextAgg,TupTriMeshMap &tupTriMeshMap);
	void ApplyVertexPaintModifierForVertexAlpha(TupTriMesh &tupBaseMesh,TupModifier &tupModifier,TupperwareAggregate *pModifierContextAgg,TupTriMeshMap &tupTriMeshMap);

	void PropagateMultiTexAlpha();
	void PropagateMultiTexAlpha(int objectRef,TupperwareList *pVertexAlphaVertsList,TupperwareList *pVertexAlphaIndicesList);
		
	void SetFaceRenderAttributes(void);
	void SetMeshSubMaterialEnvironmentMapData(void);

	void SetTransparentAndObscuredStageFacesAsHidden(void);

	void PropagateSkinModifiers(void);
	void RemapVertexBones(void);

	void PropagateClothModifiers(void);
	void AddClothMeshFlags(void);

	void BuildReplacementModels(void);
	void MarkObjectUsed(int objectRef,TupArray<bool> &objectsUsedArray);
	int RemapNodesToDummy(int fromObjectRef,int toObjectRef);

	void OneMaterialPerMesh(void);
	void BranchNodeObjectChains(int fromObjectRef,int toObjectRef,int materialRef);
	bool IsValidBranchObject(int objectRef,int materialRef,bool &copyObject);

	void AddVertexColorAndAlphaRefsToMeshAndNodes(void);

	void SetUpTextureSets(void);

	void AddMeshImageRefs(void);

	void AddMeshAnimatedTextureInfo(void);
	void AddMeshAnimatedTextureInfo(TupTriMesh &tupMesh,int &imageRef,float &animationSpeed,int materialRef);
	void AddMeshAnimatedTextureInfoStandard(int &imageRef,float &animationSpeed,int materialRef);
	void AddNodeAnimatedTextureInfo(void);

	void ForceBonesToAttachPoints(void);

	void FixRogueModifiers(void);
	bool FixRogueModifier(int modifierRef,TupperwareAggregate *pModifierContextAgg,TupTriMesh &tupBaseMesh);
	bool FixRogueApplyVCModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	bool FixRogueVertexPaintModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	bool FixRogueHoldVCModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	bool FixRogueFaceAttributeModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	bool FixRogueSkinModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	bool FixRogueClothModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);

	void FixMeshMaterialIndices(void);

	void MarkUsedMaterialsMapsAndImages(void);
	void DeleteUnusedMaterialsMapsAndImages(void);

	void RemapDuplicateImages(void);
	void RemapDuplicateAnimatedFrames(void);
	void RemoveDuplicateMaps(void);
	void RemoveDuplicateMaterials(void);

	void MergeImagePalettes(void);
	void MergeAnimatedImagePalettes(const char *pImageFileName,TupImageData &tupImageData,int &numPalettes,int &numMergedPalettes);

	void AnalyzePaletteColors(void);

	void CheckMeshIntegrity(void);

	void ConsolidateMeshMaps(void);
	void ConsolidateFaceMaterials(void);

	void OptimizeUVs(void);

	void CopySceneNodeSettings(void);

	void ChangeNodeNames(void);

	void AddConsolidatedInfoToLog(void);

	void AddTextureCombiningInfoToLog(void);

	void SetMeshRecieveNoShadowFlags(void);

	void AdjustVertexColorGamma(void);

};

#endif
