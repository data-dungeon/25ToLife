////////////////////////////////////////////////////////////////////////////
//
// LightMeshes
//
// Class to handle the breaking up of meshes into Lit and UnLit
//
////////////////////////////////////////////////////////////////////////////

#ifndef LIGHT_MESHES_H
#define LIGHT_MESHES_H

#ifndef LIGHT_MANAGER_H
#include "LightManager.h"
#endif

class RealTimeLightingSettings;

class LightMeshesManager
{
	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;
	TupConfigRoot &m_tupConfigRoot;

	int m_tupRootNodeRef;

	struct LightGroup
	{
		LightGroup(void) : m_numFaces(0) {}

		int m_numFaces;
		TupArray<int> m_lights;
	};

	const TupArray<LightManager::Light> &m_lights;

	const RealTimeLightingSettings &m_settings;

public:

	LightMeshesManager( TupNodePool &tupNodePool, TupObjectPool &tupObjectPool, TupModifierPool &tupModifierPool, int tupRootNodeRef, TupConfigRoot &tupConfigRoot, LightManager &lightManager, const RealTimeLightingSettings &settings);
	void LightMarkedNodes( void );
};

#endif
