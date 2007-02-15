////////////////////////////////////////////////////////////////////////////
//
// TupExport
//
// Tupperware exporter
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupExport.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting layers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 */

#include "TupImpExpPch.h"
#include "TupExport.h"
#include "SkinManager.h"
#include "MeshManager.h"

TupExport::TupExport(bool bExportSelected) :
	m_bExportSelected(bExportSelected)
{
   m_bConsolidateObjects = true;
   m_bBinaryOutput = false;

   m_pNodePool = NULL;
   m_pObjectPool = NULL;
   m_pModifierPool = NULL;
   m_pMaterialPool = NULL;
   m_pMapPool = NULL;
   m_pImagePool = NULL;
   m_pAnimSetPool = NULL;
   m_pConnectionPool = NULL;
   m_pAudioPool = NULL;
   m_pLayerPool = NULL;
   m_pUniqueObjectPtrs = NULL;
   m_pUniqueObjectRefs = NULL;
   m_pLayerNames = NULL;
   m_pLayers = NULL;
   m_pNodePoolDagPaths = NULL;
   m_pExportDagPaths = NULL;
   m_pMaterialCRCValues = NULL;
   m_pMapCRCValues = NULL;
   m_pImagePoolStrings = NULL;
   m_pSceneAgg = NULL;
   m_pTupScene = NULL;
   m_pSkinManager = NULL;
   m_pAddShaders = NULL;
   m_pMeshManager = NULL;
}

TupExport::TupExport(bool bConsolidateObjects, bool bBinaryOutput, bool bExportSelected) :
	m_bConsolidateObjects(bConsolidateObjects),
	m_bBinaryOutput(bBinaryOutput),
	m_bExportSelected(bExportSelected)
{
   m_pNodePool = NULL;
   m_pObjectPool = NULL;
   m_pModifierPool = NULL;
   m_pMaterialPool = NULL;
   m_pMapPool = NULL;
   m_pImagePool = NULL;
   m_pAnimSetPool = NULL;
   m_pConnectionPool = NULL;
   m_pAudioPool = NULL;
   m_pLayerPool = NULL;
   m_pUniqueObjectPtrs = NULL;
   m_pUniqueObjectRefs = NULL;
   m_pLayerNames = NULL;
   m_pLayers = NULL;
   m_pNodePoolDagPaths = NULL;
   m_pExportDagPaths = NULL;
   m_pMaterialCRCValues = NULL;
   m_pMapCRCValues = NULL;
   m_pImagePoolStrings = NULL;
   m_pSceneAgg = NULL;
   m_pTupScene = NULL;
   m_pSkinManager = NULL;
   m_pAddShaders = NULL;
   m_pMeshManager = NULL;
}


TupExport::~TupExport()
{
   // delete pools
   if (m_pNodePool) delete m_pNodePool;
   if (m_pObjectPool) delete m_pObjectPool;
   if (m_pModifierPool) delete m_pModifierPool;
   if (m_pMaterialPool) delete m_pMaterialPool;
   if (m_pMapPool) delete m_pMapPool;
   if (m_pImagePool) delete m_pImagePool;
   if (m_pAnimSetPool) delete m_pAnimSetPool;
   if (m_pConnectionPool) delete m_pConnectionPool;
   if (m_pAudioPool) delete m_pAudioPool;
   if (m_pLayerPool) delete m_pLayerPool;

   // delete keepers
   if (m_pUniqueObjectPtrs) delete m_pUniqueObjectPtrs;
   if (m_pUniqueObjectRefs) delete m_pUniqueObjectRefs;
   if (m_pLayerNames) delete m_pLayerNames;
   if (m_pLayers) delete m_pLayers;
   if (m_pNodePoolDagPaths) delete m_pNodePoolDagPaths;
   if (m_pExportDagPaths) delete m_pExportDagPaths;
   if (m_pMaterialCRCValues) delete m_pMaterialCRCValues;
   if (m_pMapCRCValues) delete m_pMapCRCValues;
   if (m_pImagePoolStrings) delete m_pImagePoolStrings;

   // delete tree
   if (m_pSceneAgg) delete m_pSceneAgg;
   if (m_pTupScene) delete m_pTupScene;
   if (m_pSkinManager) delete m_pSkinManager;
   if (m_pAddShaders) delete m_pAddShaders;
   if (m_pMeshManager) delete m_pMeshManager;
}