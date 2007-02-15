////////////////////////////////////////////////////////////////////////////
//
// ShaderManager
//
// Class to keep track of shaders for importing
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ShaderManager.h $
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

#ifndef SHADER_MANAGER
#define SHADER_MANAGER

class ShaderManager
{
private:

	enum MapTypes
	{
		MAP_TYPE_DIFFUSE,
		MAP_TYPE_REFLECTION,
	};

	TupMaterialPool &m_importMaterialPool;
	TupMapPool &m_importMapPool;
	TupImagePool &m_importImagePool;

	// I own these
	TupperwareAggregate *m_pMaterialPoolAgg;
	TupMaterialPool *m_pTupMaterialPool;

	TupperwareAggregate *m_pMapPoolAgg;
	TupMapPool *m_pTupMapPool;

	TupperwareAggregate *m_pImagePoolAgg;
	TupImagePool *m_pTupImagePool;

	class UniqueShaderData
	{
	public:
		MObject m_shadingGroupObject; // actual shading group within Maya
		MObject m_shaderObject; // actual shader within Maya

		unsigned long m_materialCRC;
	};

	class UniqueTextureData
	{
	public:
		MObject m_textureObject; // actual texture node within Maya
		MapTypes m_mapType;

		unsigned long m_mapCRC;
	};

	TupArray<UniqueShaderData> m_uniqueShaderData;
	TupArray<UniqueTextureData> m_uniqueTextureData;

	TupArray<int> m_importMaterialRemap;
	TupArray<int> m_importMapRemap;
	Keeper<TupString> m_imagePoolStrings;

#ifdef DEFER_MESH_SHADER_APPLICATION
	class DeferedMeshShaderData
	{
	public:
		MDagPath m_meshPath;
		int m_numFaces;
		const int *m_pFaceMaterialRefs;
	};

	TupArray<DeferedMeshShaderData> m_deferedMeshShaders;
#endif

public:

	ShaderManager(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool);
	~ShaderManager();

#ifdef DEFER_MESH_SHADER_APPLICATION
	void AddMeshShaderData(MDagPath meshPath,const int *pFaceMaterialRefs,int numFaces);
	void ApplyShadersToMeshes(void);
#endif
	void SetUpMeshShaders(MDagPath meshPath,const int *pFaceMaterialRefs,int numFaces);

private:

	MObject GetShadingGroup(int materialRef);
	MObject CreateShadingGroup(const char *pSGName);
	void ConnectShadingGroupToMaterial(MObject sgObject,MObject materialObject);
	int GetMaterialRef(int importMaterialRef);
	void RemapMaterialRefs(TupperwareAggregate *pMaterialAgg);
	MObject BuildMayaShader(TupperwareAggregate *pMaterialAgg);
	int GetMapRef(int importMapRef,MapTypes mapType);
	MObject BuildMayaTexture(TupperwareAggregate *pMapAgg,ShaderManager::MapTypes mapType);
	int GetMapBlendMode(int mapRef);

	void GetExistingShaders(void);
	void AddStandardMaterial(TupperwareAggregate *pMaterialAgg,const MObject &shadingGroup);
	int AddMap(MObject mapNode,MapTypes mapType,int blendMode);
	void AddBitmapMap(TupperwareAggregate *pMapAgg,MObject mapNode,int blendMode);
	
	void AddMixMap(TupperwareAggregate *pMapAgg,MObject mapNode,MapTypes mapType,int blendMode);
	void AddCompositeMap(TupperwareAggregate *pMapAgg,MObject mapNode,MapTypes mapType,int blendMode);
	int AddMixMap(int map1Ref,int map2Ref,int alphaMapRef,float alphaAmount,MapTypes mapType);

	int AddImage(const char *mapName);

	void GetBitmapMapsFromMaterial(int materialRef,TupArray<int> &mapRefArray,bool bFromImportPools);
	void GetBitmapMapsFromMap(int mapRef,TupArray<int> &mapRefArray,bool bFromImportPools);
	void HookUVChooser(MDagPath &meshPath,int mapChannel,MObject textureObject);
};

#endif