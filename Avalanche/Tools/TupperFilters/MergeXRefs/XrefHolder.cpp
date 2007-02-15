////////////////////////////////////////////////////////////////////////////
//
// XRefHolder
//
// Class to handle keeping track of data for another scene (used by xrefs)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: XrefHolder.cpp $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 10:35a
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 10:31a
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Check for database version.
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 6/21/02    Time: 1:55p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 6/21/02    Time: 8:42a
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * fix bug in finding xrefs within another scene
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/28/02    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Moved MergeXRefs from annotate filter to its own filter
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/24/02    Time: 4:24p
 * Updated in $/Avalanche/tools/TupperFilters/Annotate
 * Rearranged tupper3d usage on process tup class
 * Merge xref complete (does not do materials, maps, images)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/23/02    Time: 4:15p
 * Updated in $/Avalanche/tools/TupperFilters/Annotate
 * Merge XRef Phase A
 */

////////////////////////////////////////////////////////////////////////////

#include "MergeXRefsPch.h"
#include "XRefHolder.h"

// RefRemap methods
RefRemap::RefRemap(void) :
	m_nXRefReference(-1),
	m_nSceneReference(-1)
{
}

RefRemap::RefRemap(int nXRefReference,int nSceneReference,RemapType type) :
	m_nXRefReference(nXRefReference),
	m_nSceneReference(nSceneReference),
	m_type(type)
{
}

// Remap Manager methods
int RemapManager::Find(int nXRefReference,RefRemap::RemapType type) const
{
	int total = m_remapArray.GetSize();
	for (int index = 0;index<total;index++)
	{
		const RefRemap &remap = m_remapArray[index];
		// if we found it then return the index
		if (remap.m_type==type && remap.m_nXRefReference==nXRefReference)
			return remap.m_nSceneReference;
	}
	return -1; // not found
}

void RemapManager::Add(int nXRefReference,int nSceneReference,RefRemap::RemapType type)
{
#ifdef _DEBUG
	// make sure it is not already there
	int foundIndex = Find(nXRefReference,type);
	assert(foundIndex==-1);
#endif
	m_remapArray.Add(RefRemap(nXRefReference,nSceneReference,type));
}

// xref holder methods
XRefHolder::XRefHolder(TupperwareAggregate *pSceneAgg,const char *pFilename)
{
	assert(pSceneAgg);
	m_pSceneAgg = pSceneAgg;
	m_pTupScene = new TupScene(pSceneAgg);

	char *pVersionString;
	m_pTupScene->GetVersion(&pVersionString);
	if (!pVersionString)
	{
		throw(PluginException(ERROR_FATAL,"Database Version not available in file %s.\r\nPlease export with updated TupExport plugin.",pFilename));
	}
	// if the version does not match
	else if (strcmp(pVersionString,TupScene::GetSceneVersion()))
	{
		throw(PluginException(ERROR_FATAL,"Database Version in file %s is %s should be %s.\r\nPlease export with updated TupExport plugin.",pFilename,pVersionString,TupScene::GetSceneVersion()));
	}

	TupperwareAggregate *pNodePoolAgg = m_pTupScene->GetNodePool();
	assert(pNodePoolAgg);
	m_pTupNodePool = new TupNodePool(pNodePoolAgg);

	TupperwareAggregate *pObjectPoolAgg = m_pTupScene->GetObjectPool();
	assert(pObjectPoolAgg);
	m_pTupObjectPool = new TupObjectPool(pObjectPoolAgg);

	TupperwareAggregate *pModifierPoolAgg = m_pTupScene->GetModifierPool();
	assert(pModifierPoolAgg);
	m_pTupModifierPool = new TupModifierPool(pModifierPoolAgg);

	TupperwareAggregate *pMaterialPoolAgg = m_pTupScene->GetMaterialPool();
	assert(pMaterialPoolAgg);
	m_pTupMaterialPool = new TupMaterialPool(pMaterialPoolAgg);

	TupperwareAggregate *pMapPoolAgg = m_pTupScene->GetMapPool();
	assert(pMapPoolAgg);
	m_pTupMapPool = new TupMapPool(pMapPoolAgg);

	TupperwareAggregate *pImagePoolAgg = m_pTupScene->GetImagePool();
	assert(pImagePoolAgg);
	m_pTupImagePool = new TupImagePool(pImagePoolAgg);

	m_pRemapManager = new RemapManager;
}

XRefHolder::~XRefHolder()
{
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupObjectPool;
	delete m_pTupModifierPool;
	delete m_pTupMaterialPool;
	delete m_pTupMapPool;
	delete m_pTupImagePool;
	delete m_pRemapManager;
}

TupperwareAggregate *XRefHolder::GetSceneAgg(void) 
{
	return m_pSceneAgg;
}

TupScene *XRefHolder::GetScene(void) 
{
	return m_pTupScene;
}

TupNodePool *XRefHolder::GetNodePool(void) 
{
	return m_pTupNodePool;
}

TupObjectPool *XRefHolder::GetObjectPool(void) 
{
	return m_pTupObjectPool;
}

TupModifierPool *XRefHolder::GetModifierPool(void) 
{
	return m_pTupModifierPool;
}

TupMaterialPool *XRefHolder::GetMaterialPool(void) 
{
	return m_pTupMaterialPool;
}

TupMapPool *XRefHolder::GetMapPool(void) 
{
	return m_pTupMapPool;
}

TupImagePool *XRefHolder::GetImagePool(void) 
{
	return m_pTupImagePool; 
}

RemapManager *XRefHolder::GetRemapManager(void)
{
	return m_pRemapManager;
}


