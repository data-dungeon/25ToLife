////////////////////////////////////////////////////////////////////////////
//
// ProcessStamps.h
//
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ProcessStamps.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:29p
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PROCESS_STAMPS_H
#define PROCESS_STAMPS_H

#include "StampPch.h"
#include "ProcessStamps.h"
#include "StampManager.h"
#include "MeshLayers.h"
#include "MaterialMaps.h"

ProcessStampsManager::ProcessStampsManager(const FilterOptions &options,FilterDataManager &dataManager) : m_options(options), m_dataManager(dataManager) 
{
	m_pSceneAgg = m_dataManager.GetSceneRoot();
	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);
	
	m_pNodePoolAgg = m_pTupScene->GetNodePool();
	assert(m_pNodePoolAgg);
	m_pTupNodePool = new TupNodePool(m_pNodePoolAgg);
	
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
	if (!pSceneNodeAttributeTreeAgg)
	{
		throw(PluginException(ERROR_FATAL,"Node Attribute Tree Does not Exist!"));
	}
	m_pTupNodeConfigRoot->BuildConfigTree(pSceneNodeAttributeTreeAgg);
}

ProcessStampsManager::~ProcessStampsManager()
{
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupObjectPool;
	delete m_pTupModifierPool;
	delete m_pTupMaterialPool;
	delete m_pTupMapPool;
	delete m_pTupImagePool;
	delete m_pTupNodeConfigRoot;
}

void ProcessStampsManager::Process(void)
{
	MaterialMapsManager materialMapsManager(*m_pTupMaterialPool,*m_pTupMapPool);

	// init stamp manager


}

