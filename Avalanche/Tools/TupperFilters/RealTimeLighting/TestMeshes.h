////////////////////////////////////////////////////////////////////////////
//
// TestMeshes
//
// Class to handle the testing of meshes to determine which instances can be broken up
//
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_MESHES_H
#define TEST_MESHES_H

#ifndef LIGHT_MANAGER_H
#include "LightManager.h"
#endif

class RealTimeLightingSettings;
class TestMeshesManager
{
	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;
	TupConfigRoot &m_tupConfigRoot;
	int m_tupRootNodeRef;

	int m_numBestLights;

	const TupArray<LightManager::Light> &m_lights;

	const RealTimeLightingSettings &m_settings;
public:

	TestMeshesManager(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,int tupRootNodeRef,TupConfigRoot &tupConfigRoot,LightManager &lightManager,const RealTimeLightingSettings &settings);

	bool TestAndMarkNodes(void);

private:
	bool TestAttributes(TupNode &tupNode);
	bool TestModifiers(TupArray<int> &modifierReferenceArray,TupArray<TupperwareAggregate *> &modifierContextReferenceArray);

};

#endif
