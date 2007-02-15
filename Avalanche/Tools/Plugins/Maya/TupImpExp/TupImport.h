////////////////////////////////////////////////////////////////////////////
//
// TupImport
//
// Tupperware importer class
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupImport.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 9/11/03    Time: 4:22p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import and export of layers
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/26/03    Time: 4:34p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * creating shaders
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export vertex colors (from apply vc modifier)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/13/03    Time: 4:52p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import nodes and polymeshes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:29p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 */

#ifndef TUP_IMPORT
#define TUP_IMPORT

class ShaderManager;
class JointManager;

class TupImport
{
public:
	TupImport(void);
	// ImportScene
	int ImportScene(const TCHAR *inputFilename);

private:
	// ImportNode
	void ImportNodes(void);
	void ImportNode(int nodeRef,int parentNodeRef,TupNode &tupParentNode,MDagPath parentDagPath);
	void AddNote(MObject object,const TupString &noteString);
	void FixBoneName(TupString &boneName);
	void AddEndBone(int boneNodeRef,const MDagPath &dagPath,int objectRef);
	void ConvertMayaBoneMatrix(const MMatrix &srcMatrix,Graphics4x4 &dstMatrix);

	// ImportTransform
	void ImportTransform(int nodeRef,int parentNodeRef,TupNode &tupNode,TupNode &tupParentNode,MObject transformObject);
	TupObject::ObjectTypes GetObjectType(int objectRef);
	void GetMatrix(int nodeRef,TupNode &tupNode,MMatrix &matrix);
	void ConvertToMayaMatrix(const Graphics4x4 &srcMatrix,MMatrix &dstMatrix);
	void ConvertBoneToMayaMatrix(const Graphics4x4 &srcMatrix,MMatrix &dstMatrix);
	void ConvertLightToMayaMatrix(const Graphics4x4 &srcMatrix,MMatrix &dstMatrix);

	// ImportObject
	int FindBaseObjectRef(int objectRef);
	void ImportObject(int objectRef,MObject parentObject);
	void ImportPolyMesh(TupperwareAggregate *pObjectAgg,MObject parentObject,
							  const TupArray<TupVertex3f> &colorVerts,const TupArray<int> &colorIndices,
							  const TupArray<float> &alphaVerts,const TupArray<int> &alphaIndices,bool bMixedColors,
							  const TupArray<TupString> &faceAttributesVerts,const TupArray<int> &faceAttributesIndices);
	void ImportTriMesh(TupperwareAggregate *pObjectAgg,MObject parentObject,
							  const TupArray<TupVertex3f> &colorVerts,const TupArray<int> &colorIndices,
							  const TupArray<float> &alphaVerts,const TupArray<int> &alphaIndices,bool bMixedColors,
							  const TupArray<TupString> &faceAttributesVerts,const TupArray<int> &faceAttributesIndices);
	void ImportSplineShape(TupperwareAggregate *pObjectAgg,MObject parentObject);
	void ImportBezierShape(TupperwareAggregate *pObjectAgg,MObject parentObject);
	void ImportLight(TupperwareAggregate *pObjectAgg,MObject parentObject);
	void ImportLocator(TupperwareAggregate *pObjectAgg,MObject parentObject);

	// ImportLayers
	void ImportLayers(void);
	void AddDisplayLayer(MDagPath &path,int layerRef);

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

	TupArray<TupArray<MDagPath> > *m_pAssignedNodesToLayer;

	int m_rootNodeRef;

	TupString m_inputFilename;

	ShaderManager *m_pShaderManager;
	JointManager *m_pJointManager;
};

#endif