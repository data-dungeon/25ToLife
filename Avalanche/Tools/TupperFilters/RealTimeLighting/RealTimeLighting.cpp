////////////////////////////////////////////////////////////////////////////
//
// RealTimeLighting
//
// Class to handle the RealTimeLighting Plugin
//
////////////////////////////////////////////////////////////////////////////

#include "RealTimeLightingPch.h"
#include "RealTimeLighting.h"
#include "TestMeshes.h"
#include "LightMeshes.h"

RealTimeLightingFilter::RealTimeLightingFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

int RealTimeLightingFilter::GetTotalPasses(void) const
{
	return 1;
}

void RealTimeLightingFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog( &m_log );
	PluginLog::Add( LogFlags::VERBOSE_LEVEL_MED,"Running Real Time Lighting Filter" );
	CheckOptions(options);

	TupConfigRoot tupConfigRoot;
	TupperwareAggregate *pSceneRootAgg = dataManager.GetSceneRoot();
	TupScene tupScene(pSceneRootAgg);
	// Node Attribute Tree
	TupperwareAggregate *pSceneNodeAttributeTreeAgg = tupScene.GetNodeAttributeTree();
	if (!pSceneNodeAttributeTreeAgg)
	{
		throw(PluginException(ERROR_FATAL,"Node Attribute Tree Does not Exist!"));
	}
	tupConfigRoot.BuildConfigTree(pSceneNodeAttributeTreeAgg);

	TupperwareAggregate *pNodePoolAgg = tupScene.GetNodePool();
	assert(pNodePoolAgg);
	TupNodePool tupNodePool(pNodePoolAgg);

	TupperwareAggregate *pObjectPoolAgg = tupScene.GetObjectPool();
	assert(pObjectPoolAgg);
	TupObjectPool tupObjectPool(pObjectPoolAgg);

	TupperwareAggregate *pModifierPoolAgg = tupScene.GetModifierPool();
	assert(pModifierPoolAgg);
	TupModifierPool tupModifierPool(pModifierPoolAgg);

	int tupRootNodeRef;
	tupScene.GetRootNodeReference(tupRootNodeRef);

	LightManager lightManager(tupNodePool, tupObjectPool, tupModifierPool, tupRootNodeRef, tupConfigRoot);
	TestMeshesManager testMeshesManager( tupNodePool, tupObjectPool, tupModifierPool, tupRootNodeRef, tupConfigRoot, lightManager, m_settings );
	LightMeshesManager LightMeshesManager( tupNodePool, tupObjectPool, tupModifierPool, tupRootNodeRef, tupConfigRoot, lightManager, m_settings );

	if (testMeshesManager.TestAndMarkNodes())
	{
		LightMeshesManager.LightMarkedNodes( );
	}
}

void RealTimeLightingFilter::CheckOptions(const FilterOptions &options)
{
	m_settings.m_bAssignLightsToInstance = true;
	m_settings.m_maxAssignedPointLights = 2;
	m_settings.m_bBreakUpMesh = false;
	m_settings.m_pointLightThreshold = 0.05f;

	// Assign Lights To Instance
	const char *pAssignLightsToInstance = options.GetValue("AssignLightsToInstances");
	if (pAssignLightsToInstance && !stricmp(pAssignLightsToInstance,"false"))
	{
		m_settings.m_bAssignLightsToInstance = false;
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"AssignLightsToInstances Disabled");
	}

	// Max Assigned Point Lights
	const char *pMaxAssignedPointLights = options.GetValue("MaxAssignedPointLights");
	if (pMaxAssignedPointLights)
	{
		m_settings.m_maxAssignedPointLights = atoi(pMaxAssignedPointLights);
		if (m_settings.m_maxAssignedPointLights<1)
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_YELLOW,"Max Assigned Point Lights cannot be less than 1.  Using default of 2");
			m_settings.m_maxAssignedPointLights = 2; // set back to default
		}
		else
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Max Assigned Point Lights set to %d",m_settings.m_maxAssignedPointLights);
		}
	}

	// Break Up Mesh
	const char *pBreakUpMesh = options.GetValue("BreakUpMesh");
	if (pBreakUpMesh && !stricmp(pBreakUpMesh,"true"))
	{
		m_settings.m_bBreakUpMesh = true;
		m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Break Up Mesh Enabled");
	}

	// Point Light Threshold
	const char *pPointLightThreshold = options.GetValue("PointLightThreshold");
	if (pPointLightThreshold)
	{
		m_settings.m_pointLightThreshold = (float) atof(pPointLightThreshold);
		if (m_settings.m_pointLightThreshold<0.0)
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_YELLOW,"Point Light Threshold cannot be less than or equal to zero.  Using default of 0.001");
			m_settings.m_pointLightThreshold = 0.05f; // set back to default
		}
		else
		{
			m_log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Point Light Threshold set to %f",m_settings.m_pointLightThreshold);
		}
	}
}
