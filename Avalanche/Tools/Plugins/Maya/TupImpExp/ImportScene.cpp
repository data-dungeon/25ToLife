////////////////////////////////////////////////////////////////////////////
//
// ImportScene
//
// Tupperware scene importing
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportScene.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 9/11/03    Time: 4:22p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import and export of layers
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/04/03    Time: 10:50a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * poly mesh to tri mesh now has triangulation routine
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/26/03    Time: 4:34p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * creating shaders
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * export vertex colors (from apply vc modifier)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/13/03    Time: 4:52p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import nodes and polymeshes
 */

#include "TupImpExpPch.h"
#include "TupImport.h"
#include "ShaderManager.h"
#include "JointManager.h"

int TupImport::ImportScene(const TCHAR *inputFilename)
{
	m_inputFilename = inputFilename;

	int result = TupperwareLoadSaveManager::Read(inputFilename,&m_pSceneAgg);
	if (!result)
		return FALSE;
	if (!m_pSceneAgg->GetFirstChildAtom())
		return FALSE;
	m_pTupScene = new TupScene(m_pSceneAgg);
	
	TupperwareAggregate *pNodePoolAgg = m_pTupScene->GetNodePool();
	assert(pNodePoolAgg);
	m_pNodePool = new TupNodePool(pNodePoolAgg);
	
	m_pTupScene->GetRootNodeReference(m_rootNodeRef);
	assert(m_rootNodeRef>=0);
	
	TupperwareAggregate *pObjectPoolAgg = m_pTupScene->GetObjectPool();
	assert(pObjectPoolAgg);
	m_pObjectPool = new TupObjectPool( pObjectPoolAgg );
	
	TupperwareAggregate *pModifierPoolAgg = m_pTupScene->GetModifierPool();
	assert(pModifierPoolAgg);
	m_pModifierPool = new TupModifierPool( pModifierPoolAgg );
	
	TupperwareAggregate *pMaterialPoolAgg = m_pTupScene->GetMaterialPool();
	assert(pMaterialPoolAgg);
	m_pMaterialPool = new TupMaterialPool( pMaterialPoolAgg );
	
	TupperwareAggregate *pMapPoolAgg = m_pTupScene->GetMapPool();
	assert(pMapPoolAgg);
	m_pMapPool = new TupMapPool( pMapPoolAgg );
	
	TupperwareAggregate *pImagePoolAgg = m_pTupScene->GetImagePool();
	assert(pImagePoolAgg);
	m_pImagePool = new TupImagePool( pImagePoolAgg );

	TupperwareAggregate *pLayerPoolAgg = m_pTupScene->GetLayerPool();
	if (pLayerPoolAgg)
	{
		m_pLayerPool = new TupLayerPool( pLayerPoolAgg );
		m_pAssignedNodesToLayer = new TupArray<TupArray<MDagPath> >;
		m_pAssignedNodesToLayer->SetSize(m_pLayerPool->GetNumLayers());
	}
	else
	{
		m_pLayerPool = NULL;
		m_pAssignedNodesToLayer = NULL;
	}

	m_pShaderManager = new ShaderManager(*m_pMaterialPool,*m_pMapPool,*m_pImagePool);
	m_pJointManager = new JointManager(*m_pNodePool,*m_pObjectPool,*m_pModifierPool);

	try
	{
		{
			// fix any problems with modifiers pointing to trimeshes
			TupFixRogueModifiers tupFixRogueModifiers(m_pObjectPool,m_pModifierPool);
			tupFixRogueModifiers.Process();
			TupObjectConverter tupObjectConverter(*m_pObjectPool,*m_pModifierPool);
//			tupObjectConverter.ConvertObjects(TupObject::POLY_MESH,TupObject::TRI_MESH);
#ifndef COLOR_TRI_STRIPS_ON_IMPORT
			tupObjectConverter.ConvertObjects(TupObject::TRI_MESH,TupObject::POLY_MESH);
#endif
			TupCoordinateSystemConverter tupCoordinateSystemConverter(*m_pTupScene,*m_pNodePool,*m_pObjectPool,*m_pModifierPool);
			tupCoordinateSystemConverter.ConvertScene(TupScene::UP_AXIS_Y);
			TupMultiSubObjectMaterialConverter multiSubObjectMaterialConverter(*m_pNodePool,*m_pObjectPool,*m_pMaterialPool);
			multiSubObjectMaterialConverter.ConvertScene();
		}
		m_pJointManager->ReadSkinData(); // gather information about the skin modifiers
		ImportNodes();
		m_pJointManager->ConnectSkinAndJoints();
		ImportLayers();
#ifdef DEFER_MESH_SHADER_APPLICATION
		m_pShaderManager->ApplyShadersToMeshes();
#endif


	}
	catch (...)
	{
		result = FALSE;
	}

	delete m_pShaderManager;
	
	delete m_pTupScene;
	delete m_pNodePool;
	delete m_pObjectPool;
	delete m_pModifierPool;
	delete m_pMaterialPool;
	delete m_pMapPool;
	delete m_pImagePool;

	delete m_pSceneAgg;

	delete m_pLayerPool;
	delete m_pAssignedNodesToLayer;

	return result;
}

