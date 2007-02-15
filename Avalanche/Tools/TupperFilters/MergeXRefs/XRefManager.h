////////////////////////////////////////////////////////////////////////////
//
// XRefManager
//
// Class to handle keeping track of the xrefs and allowing them to be merged with the current scene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: XRefManager.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/24/02    Time: 4:24p
 * Updated in $/Avalanche/tools/TupperFilters/Annotate
 * Rearranged tupper3d usage on process tup class
 * Merge xref complete (does not do materials, maps, images)
 */

////////////////////////////////////////////////////////////////////////////

#ifndef XREF_MANAGER_H
#define XREF_MANAGER_H

#ifndef XREF_HOLDER_H
#include "XRefHolder.h"
#endif

class XRefManager
{
	// access to other scenes
	FilterDataManager &m_dataManager;
	// access to main scene
	TupScene &m_tupScene;
	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjPool;
	TupModifierPool &m_tupModifierPool;
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;

	TupArray<XRefHolder *> m_holderArray;

	XRefManager(void); // this is not defined

public:
	XRefManager(FilterDataManager &dataManager,TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjPool,
		TupModifierPool &tupModifierPool,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool);
	~XRefManager();

	int AddScene(const char *pFilename);
	int GetSceneIndex(const char *pFilename) const;
	int GetNumScenes(void) const;
	TupperwareAggregate *GetSceneAgg(int sceneIndex);
	TupScene &GetScene(int sceneIndex);
	TupNodePool &GetNodePool(int sceneIndex);
	TupObjectPool &GetObjectPool(int sceneIndex);
	TupModifierPool &GetModifierPool(int sceneIndex);
	TupMaterialPool &GetMaterialPool(int sceneIndex);
	TupMapPool &GetMapPool(int sceneIndex);
	TupImagePool &GetImagePool(int sceneIndex);
	int GetRootNodeReference(int sceneIndex);

	int GetRemappedNode(int sceneIndex,int nXRefReference);
	int GetRemappedObject(int sceneIndex,int nXRefReference);
	int GetRemappedModifier(int sceneIndex,int nXRefReference);
	int GetRemappedMaterial(int sceneIndex,int nXRefReference);
	int GetRemappedMap(int sceneIndex,int nXRefReference);
	int GetRemappedImage(int sceneIndex,int nXRefReference);

	int CopyNodeFromScene(int sceneIndex,int nXRefReference);
	int CopyObjectFromScene(int sceneIndex,int nXRefReference);
	int CopyModifierFromScene(int sceneIndex,int nXRefReference);
	int CopyMaterialFromScene(int sceneIndex,int nXRefReference);
	int CopyMapFromScene(int sceneIndex,int nXRefReference);
	int CopyImageFromScene(int sceneIndex,int nXRefReference);

private:
	int FindRemap(int sceneIndex,int nXRefReference,RefRemap::RemapType type) const;
	void AddRemap(int sceneIndex,int nXRefReference,int nSceneReference,RefRemap::RemapType type);
};

#endif
