////////////////////////////////////////////////////////////////////////////
//
// TupExport
//
// Tupperware exporter class
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupExport.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 9/11/03    Time: 4:22p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import and export of layers
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting layers
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/20/03    Time: 4:12p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * shader manager
 * comparison class
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:56p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fix map channel on uv set chooser
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 */

#ifndef TUP_EXPORT
#define TUP_EXPORT

class SkinManager;
class MeshManager;
class TupExportAddShaders;

class TupExport
{
	friend class TupExportAddShaders;

	class LayerData
	{
	public:
		int m_reference;
		bool m_bUsed;
		int m_flags;
		TupString m_name;
		int m_colorIndex;
		int m_displayOrder;
	};

public:
   TupExport(bool bExportSelected);
	TupExport(bool bConsolidateObjects, bool bBinaryOutput,bool bExportSelected);
   virtual ~TupExport();

	// ExportScene
	int ExportScene(const TCHAR *outputFilename);
   void BuildScene(const TCHAR *outputFilename);

   bool ValidNode(MDagPath &mDagPath);
   static bool ValidNodeName(const char *pName);
   static bool ValidObject(MDagPath &mDagPath);
	static void AddUnits(TupScene *pTupScene);

   TupperwareAggregate * GetSceneAgg(void) { return m_pSceneAgg; }
   void SetSceneAgg(TupperwareAggregate *pSceneAgg) { m_pSceneAgg = pSceneAgg; }

	static void FixBoneName(TupString &boneName);

private:
	void AddNodesToPools(MFnDagNode &rootNode);
	void AddNodesToScene(MFnDagNode &rootNode,TupNode &tupRootNode,int rootIndex);
	void BuildSkinData(void);
	void AddExtraBones(void);
	void AddRelatedBones(int boneRef,const Keeper<int> &attachedBones,Keeper<int> &freeBones);
	static MObject GetJointForCluster(MObject& jointCluster);
	static MMatrix GetBindPoseMatrixForJoint(MObject& jointNode);

		
	// ExportNode
	void AddNode(int parentNodeReference,int nodeReference,TupperwareAggregate *pNodeAgg,MDagPath mdagPath);
	void AddChildrenNodesToPools(MDagPath mdagPath);
	void AddSelectedDagPathsForExport(void);
	void AddSelectedDagPathForExport(const MDagPath &mDagPath,bool bAddParents,bool bAddChildren);
	void AddConnectedNodes(void);

	// ExportTransform
	void AddTransform(TupNode &tupNode,MDagPath mdagPath);
	void ConvertMayaMatrix(const MMatrix &srcMatrix,Graphics4x4 &dstMatrix);
	void ConvertMayaBoneMatrix(const MMatrix &srcMatrix,Graphics4x4 &dstMatrix);
	void ConvertMayaLightMatrix(const MMatrix &srcMatrix,Graphics4x4 &dstMatrix);
	MMatrix GetParentBoneTM(MDagPath mDagPath);

	// ExportMaterial
	int AddMaterial(const MObjectArray &shaders,MIntArray &shaderIndices);
	int AddMaterial(const MObject &shadingGroup);
	void AddStandardMaterial(TupperwareAggregate *pMaterialAgg,const MObject &shadingGroup);
	int AddMultiSubObjectMaterial(const MObjectArray &shadingGroups,MIntArray &shaderIndices,bool bAddNullMaterialToEnd);

	// ExportMap
	int AddMap(MObject mapNode,int blendMode);
	void AddBitmapMap(TupperwareAggregate *pMapAgg,MObject mapNode,int blendMode);
	bool IsPlugConnectedToPlug(MPlug plugA,MPlug plugB);
	int GetMapChannel(MObject node);
		
	void AddCompositeMap(TupperwareAggregate *pMapAgg,MObject mapNode,int blendMode);

	// ExportImage
	int AddImage(const char *mapName);

	// ExportObject
	int AddObject(MDagPath mdagPath,int &materialRef);
	void AddMesh(TupperwareAggregate *pObjectAgg,int meshManagerIndex);
	void AddSplineShape(TupperwareAggregate *pObjectAgg,MDagPath mdagPath);
	void AddBone(TupperwareAggregate *pObjectAgg,MDagPath mdagPath);
	void AddLight(TupperwareAggregate *pObjectAgg,MDagPath mdagPath);
	void AddCamera(TupperwareAggregate *pObjectAgg,MDagPath mdagPath);
	void AddLocator(TupperwareAggregate *pObjectAgg,MDagPath mdagPath);
	void AddLookAt(TupperwareAggregate *pObjectAgg,MDagPath mdagPath);

	void AddUnknownObject(TupperwareAggregate *pObjectAgg,MDagPath mdagPath);
	MDagPath GetPreBoundMesh(MDagPath meshPath);

	// ExportModifier
	int AddVertexColorModifier(int meshManagerIndex,int objectRef);
	int AddSkinModifier(MDagPath mdagPath,int objectRef);
	int AddFaceAttributeModifier(MDagPath mdagPath,int objectRef);

	// ExportConnections
	void AddConnections(void);
	void AddMotionPathConnections(void);

	// ExportAudio
	void AddAudio(void);

	// ExportLayer
	void GetLayers(void);
	void AddSelectedNodeToLayer(const MDagPath &dagPath);
	void SetLayerReferenceValues(void);
	void AddLayerPool(void);

	TupperwareAggregate *m_pSceneAgg;
	TupScene *m_pTupScene;
	TupNodePool *m_pNodePool;
	TupObjectPool *m_pObjectPool;
	TupModifierPool *m_pModifierPool;
	TupMaterialPool *m_pMaterialPool;
	TupMapPool *m_pMapPool;
	TupImagePool *m_pImagePool;
	TupAnimSetPool *m_pAnimSetPool;
	TupConnectionPool *m_pConnectionPool;
	TupAudioPool *m_pAudioPool;
	TupLayerPool *m_pLayerPool;

	TupString m_outputFilename;

	int m_rootNodeRef;
	MDagPath m_shapePath;
	
	bool m_bConsolidateObjects;
	bool m_bBinaryOutput;
	bool m_bExportSelected;

	Keeper<MDagPath> *m_pNodePoolDagPaths; // keep track of all the dag paths for each of the nodes in the node pool
	Keeper<MDagPath> *m_pExportDagPaths; // keep track of all the dag paths that will be exported (when exporting selections)

	Keeper<int> *m_pUniqueObjectPtrs; // keep track of the unique objects

	Keeper<TupString> *m_pLayerNames; // keep track of layers
	TupArray<LayerData> *m_pLayers;


	Keeper<TupString> *m_pImagePoolStrings;

	class UniqueObjectRefs
	{
	public:
		int m_objectRef; // the reference in the object pool for the top derived object or the base object
		int m_materialRef; // the reference in the material pool for the main object
	};

	// for each unique object we store a bunch of other refs
	TupArray<UniqueObjectRefs> *m_pUniqueObjectRefs;

	TupArray<unsigned long> *m_pMaterialCRCValues;
	TupArray<unsigned long> *m_pMapCRCValues;
	TupArray<TupArray<int> > *m_pMultiSubMaterialRefs;

	SkinManager *m_pSkinManager;
	TupExportAddShaders *m_pAddShaders;
	MeshManager *m_pMeshManager;

	TupAnimChannelPool *m_pCurrentTupAnimChannelPool;

};

#endif