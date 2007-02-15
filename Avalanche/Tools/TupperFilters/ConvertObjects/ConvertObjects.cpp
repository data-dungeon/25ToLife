////////////////////////////////////////////////////////////////////////////
//
// ConvertObjectsFilter
//
// Class to handle the Convert Objects 
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ConvertObjects.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/05/03    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperFilters/ConvertObjects
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 7/29/03    Time: 2:58p
 * Created in $/Avalanche/tools/TupperFilters/ConvertObjects
 */

////////////////////////////////////////////////////////////////////////////

#include "ConvertObjectsPch.h"
#include "ConvertObjects.h"


int ConvertObjectsFilter::GetTotalPasses(void)	const
{
	return 1;
}

void ConvertObjectsFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	// set up log
	PluginLog::SetLog(&m_log);

	TupperwareAggregate *pSceneAgg = dataManager.GetSceneRoot();
	assert(pSceneAgg);
	TupScene tupScene(pSceneAgg);

	TupperwareAggregate *pObjectPoolAgg = tupScene.GetObjectPool();
	assert(pObjectPoolAgg);
	TupObjectPool tupObjectPool(pObjectPoolAgg);
	
	TupperwareAggregate *pModifierPoolAgg = tupScene.GetModifierPool();
	assert(pModifierPoolAgg);
	TupModifierPool tupModifierPool(pModifierPoolAgg);

	TupObjectConverter tupObjectConverter(tupObjectPool,tupModifierPool);
	try
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Validating meshes");
		ValidateMeshes(tupObjectPool);
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Converting any triangle meshes to poly meshes");
//		if (!tupObjectConverter.ConvertObjects(TupObject::TRI_MESH,TupObject::POLY_MESH))
//		{
//			throw(PluginException(ERROR_FATAL,"Error Converting Objects"));
//		}
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Converting any poly meshes to triangle meshes");
		if (!tupObjectConverter.ConvertObjects(TupObject::POLY_MESH,TupObject::TRI_MESH))
		{
			throw(PluginException(ERROR_FATAL,"Error Converting Objects"));
		}
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"ConvertObjects - Process\r\n  %s",(const char *)(e)));
	}
}

void ConvertObjectsFilter::ValidateMeshes(TupObjectPool &tupObjectPool)
{
	bool bFoundProblem = false;
	int numObjects = tupObjectPool.GetNumObjects();
	for (int objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = tupObjectPool.GetObjectAggregate(objectIndex);
		if (!pObjectAgg)
			continue;
		TupObject tupObject(pObjectAgg);
		switch (tupObject.GetObjectType())
		{
			case TupObject::POLY_MESH:
				{
					TupPolyMesh tupPolyMesh(pObjectAgg);
					if (!ValidateMesh(tupPolyMesh))
					{
						bFoundProblem = true;
					}
				}
				break;
			case TupObject::TRI_MESH:
				{
					TupPolyMesh tupTriMesh(pObjectAgg);
					if (!ValidateMesh(tupTriMesh))
					{
						bFoundProblem = true;
					}
				}
				break;
		}
	}
	if (bFoundProblem)
	{
		throw(PluginException(ERROR_FATAL,"Stopping due to invalid meshes"));
	}
}

bool ConvertObjectsFilter::ValidateMesh(TupPolyMesh &tupPolyMesh)
{	
	int numPositionVerts;
	TupVertex3f *pPositionVerts;

	tupPolyMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);

	bool bFoundProblem = false;

	if (numPositionVerts == 0)
	{
		TupString objectName("unknown");
		TupperwareScalar *pMayaObjectNameScalar = tupPolyMesh.GetBaseAggregate()->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
			objectName = pMayaObjectNameScalar->GetAsString();
		bFoundProblem = true;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE,"Model with no vertices\r\n  %s", (const char*)objectName);
	}
	return !bFoundProblem;
}

bool ConvertObjectsFilter::ValidateMesh(TupTriMesh &tupTriMesh)
{	
	int numPositionVerts;
	TupVertex3f *pPositionVerts;

	tupTriMesh.GetPositionVerts((float **)&pPositionVerts,numPositionVerts);

	bool bFoundProblem = false;

	if (numPositionVerts == 0)
	{
		TupString objectName("unknown");
		TupperwareScalar *pMayaObjectNameScalar = tupTriMesh.GetBaseAggregate()->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
			objectName = pMayaObjectNameScalar->GetAsString();
		bFoundProblem = true;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE,"Model with no vertices\r\n  %s", (const char*)objectName);
	}
	return !bFoundProblem;
}