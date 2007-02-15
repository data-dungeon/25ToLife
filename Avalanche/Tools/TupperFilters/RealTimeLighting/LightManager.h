////////////////////////////////////////////////////////////////////////////
//
// LightManager
//
// Class to handle the gathering of lights for use in the realtime lighting plugin
//
////////////////////////////////////////////////////////////////////////////

#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

class LightManager
{
public:
	struct Light
	{
		TupVertex3f m_worldPosition;
		float m_quadraticAttenuation;
		TupString m_name;
		int m_lightIndex;
		bool m_bAffectsStaticInstances;
		bool m_bAffectsTerrain;
	};

private:

	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;
	TupConfigRoot &m_tupConfigRoot;

	int m_tupRootNodeRef;

	TupArray<Light> m_lights;

public:

	LightManager( TupNodePool &tupNodePool, TupObjectPool &tupObjectPool, TupModifierPool &tupModifierPool, int tupRootNodeRef, TupConfigRoot &tupConfigRoot);
	const TupArray<Light> &GetLights(void);
};

#endif
