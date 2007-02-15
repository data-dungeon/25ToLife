////////////////////////////////////////////////////////////////////////////
//
// StageFaces
//
// Class to handle access to stage faces
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: StageFaces.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:03a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 4  *****************
 * User: Food         Date: 3/12/03    Time: 12:32p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/24/03    Time: 4:59p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/21/03    Time: 3:59p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added stage faces helper class
 */

#include "PluginsPch.h"
#include "StageFaces.h"

StageFaceManager::StageFaceManager(TupTriMesh &tupTriMesh) : 
	m_pNextStageFaceIndices(NULL),
	m_numFaces(0)
{
	TupperwareList *pTupList = tupTriMesh.GetBaseAggregate()->FindListByKey("CombineMeshes_NextStageFaceIndices");
	if (!pTupList)
		pTupList = tupTriMesh.GetBaseAggregate()->FindListByKey("MultiTex_NextStageFaceIndices");
	if (pTupList)
	{
		m_pNextStageFaceIndices = pTupList->GetAsInt();
		m_numFaces = pTupList->GetLength();
	}
}
	
int StageFaceManager::GetNumStages(int faceIndex) const
{
	if (m_pNextStageFaceIndices)
	{
		assert(faceIndex>=0 && faceIndex<m_numFaces);
		int numStages = 1;
		int nextFace=faceIndex;
		while(1)
		{
			nextFace = m_pNextStageFaceIndices[nextFace];
			if (nextFace==-1)
				break;
			numStages++;
		}
		return numStages;
	}
	else
	{
		return 1;
	}
}

int StageFaceManager::GetStageFace(int faceIndex,int stageIndex) const
{
	if (m_pNextStageFaceIndices)
	{
		assert(faceIndex>=0 && faceIndex<m_numFaces);
		int stage = 0;
		int nextFace=faceIndex;
		while(1)
		{
			if (stageIndex == stage)
				return nextFace;

			nextFace = m_pNextStageFaceIndices[nextFace];
			if (nextFace==-1)
				break;
			stage++;
		}
		return -1;
	}
	else
	{
		assert(stageIndex==0);
		return faceIndex;
	}
}
